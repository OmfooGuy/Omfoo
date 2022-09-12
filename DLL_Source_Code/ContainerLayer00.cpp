// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer00.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer00.h"
#include "DllMain.h"
#include <shlwapi.h>

#include "MiscStatic.h"
using namespace NsMiscStatic;

#define JAN1_1970_AS_SYSTEMTIME	((unsigned __int64)(0x019DB1DED53E8000))
#define SECS_1601_TO_1970		((unsigned __int64)(0x00000002B6109100))
#define TICKS_1601_TO_1970		((unsigned __int64)(0x019DB1DED53E8000))
#define TICKSPERSEC				((unsigned __int32)(0x00989680))
#define SECS_PER_DAY			((unsigned __int32)(0x00015180))
#define SECS_PER_2DAYS			((unsigned __int32)(0x0002A300)) 

// Version 1.0 of the OMF Interchange format was published in November 1993.
// Any OMF timestamp that purports to be earlier than January 1, 1993 is surely invalid.
#define SECS_1970_TO_1993		((unsigned __int32)(0x2B438980))

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer00::CContainerLayer00(void)
	: m_hrFirewall(E_HANDLE)
{
	// Retrieve the current system Windows ANSI code page.
	m_dwWindowsAnsiCodePage	= GetWindowsAnsiCodePage();

	// Retrieve the current system legacy Macintosh code page.
	m_dwLegacyMacCodePage	= GetLegacyMacCodePage();

	// Capture the current system time as a 32-bit value measured in seconds since 1970.
	m_dwSessionTimeStamp	= GetCurrentTimeInSecondsSince1970();
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer00::~CContainerLayer00(void)
{
	// Release the string memory.
	MemFree(m_pStringCache);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer00::Load(PCWSTR pwzFileName)
{
	if (IsBadStringPointerW(pwzFileName, MAX_PATH_FULLPATH))
	{
		return E_POINTER;
	}

	if (lstrcmpiW(PathFindExtensionW(pwzFileName), L".omf"))
	{
		return E_INVALIDARG;
	}

	HRESULT hr = OpenOmfFile(pwzFileName);
	if (SUCCEEDED(hr))
	{
		CHECK(IngestMobIDs());				// cache the mob IDs.
		CHECK(DetectDuplicateMobIDs());		//
		CHECK(IngestStrings());				// cache the strings.
		CHECK(DetectCodePagePartOne());		// detects pure 7-bit ASCII and 8-bit UTF-8.
		CHECK(DetectPlatform());			// was this file created on Windows? legacy Macintosh? or modern Mac?
		CHECK(DetectCodePagePartTwo());		// detects other legacy ANSI code pages if DetectCodePagePartOne() failed.
	//	CHECK(SurveyTimestamps());			// just for debugging
	}

	return hr;
}

//*********************************************************************************************************************
//	GetCurrentTimeInSecondsSince1970().
//	Windows measures time as a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601.
//	But OMF measures time as a 32-bit value representing the number of seconds since January 1, 1970.
//	This routine captures current system time (in UTC), and returns it as a 32-bit value that OMF understands. 
//	See the omfi:TimeStamp data type in Appendix B, Table 9, of the OMF specification version 2.1, at page 222.
//	Note that this way of measuring time will overflow on 03:14:07am, 1/19/2038, UTC.
//*********************************************************************************************************************
DWORD CContainerLayer00::GetCurrentTimeInSecondsSince1970(void)
{
	UINT64	u64 = 0;
	GetSystemTimeAsFileTime(LPFILETIME(&u64));
	u64 /= TICKSPERSEC;
	if (u64 > SECS_1601_TO_1970)
	{
		u64 -= SECS_1601_TO_1970;
		return UINT32(u64);
	}

	return 0;
}

//*********************************************************************************************************************
//	Protected helper.
//*********************************************************************************************************************
QWORD CContainerLayer00::CvtSecondsSince1970ToFileTime(DWORD dwSecondsSince1970)
{
	QWORD qwFileTime = dwSecondsSince1970 * TICKSPERSEC;
	qwFileTime += TICKS_1601_TO_1970;
	return qwFileTime;
}

//*********************************************************************************************************************
//	There's a bug in some builds of OmfToolkit.dll that clobbers the upper 12 bits of the 32-bit timestamp with zeros.
//	It corrupts omfi:TimeStamp and sometimes omfi:UID (because the second 32 bits [the 'major' member] is a timestamp).
//	The bug existed in OMF1 and OMF2, and afaik it only existed in Windows.
//	It looks to me like the bug may have originated in a routine called omfsGetDateTime().
//	This routine checks the bounds of dwTimeValue.
//	The lower bound is fixed at January 1, 1993 (because OMF emerged in 1993).
//	The upper bound is calculated at runtime - it's the current system time plus two days.
//	Fwiw this timestamp scheme will overflow on 03:14:07 UTC on 19 January 2038.
//*********************************************************************************************************************
BOOL CContainerLayer00::IsValidTimeValue(DWORD dwTimeValue)
{
	if (dwTimeValue > SECS_1970_TO_1993)
	{
		if (dwTimeValue < QWORD(m_dwSessionTimeStamp + SECS_PER_2DAYS))
		{
			return TRUE;
		}
	}
	return FALSE;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Just for debugging and troubleshooting the omfi:TimeStamp data type.
//	On entry m_dwSessionTimeStamp must be valid.
//	On exit m_fHasTimestampBug, m_dwTimeValueMin, and m_dwTimeValueMax are meaningful.
//*********************************************************************************************************************
HRESULT CContainerLayer00::SurveyTimestamps(void)
{
	PTOCX_ITEM	pCurItem	= m_aToc;
	PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
	HRESULT		hr			= S_OK;

	QWORD	qwTimeStampMax	= QWORD(m_dwSessionTimeStamp + SECS_PER_2DAYS);
	DWORD	dwTimeValueMin	= 0xFFFFFFFF;
	DWORD	dwTimeValueMax	= 0x00000000;
	do
	{
		if ((pCurItem->dwDataType == m_dwTypeTimeStamp) &&
			(pCurItem->bStorageMode == SM_OFFSET) &&
			(pCurItem->cbLength64 >= sizeof(DWORD)))
		{
			DWORD dwTimeValue = 0;
			if (SUCCEEDED(hr=SeekRead(pCurItem->cbOffset64, &dwTimeValue, sizeof(DWORD))))
			{
				if (m_fOmfBigEndian)
				{
					dwTimeValue = Endian32(dwTimeValue);
				}

				// If the three upper nybbles are zero then ...
				if (0 == (dwTimeValue & 0xFFF00000))
				{
					m_fHasTimestampBug = TRUE;
					continue;
				}

				if ((dwTimeValue > SECS_1970_TO_1993)&&(dwTimeValue < qwTimeStampMax))
				{
					if (dwTimeValueMin > dwTimeValue)
					{
						dwTimeValueMin = dwTimeValue;
					}
					if (dwTimeValueMax < dwTimeValue)
					{
						dwTimeValueMax = dwTimeValue;
					}
				}
			}
			else
			{
				// SeekRead() failed.
				break;
			}
		}
	} while (++pCurItem < pEndItem);

	if (SUCCEEDED(hr))
	{
		m_dwTimeValueMin = dwTimeValueMin;
		m_dwTimeValueMax = dwTimeValueMax;

		//// Temporary code to see the timestamp in the debugger.
		//if (dwTimeValueMax)
		//{
		//	SYSTEMTIME	sysTime = {0};	
		//	QWORD qwFileTimeMax = CvtSecondsSince1970ToFileTime(dwTimeValueMax);
		//	if (FileTimeToSystemTime(LPFILETIME(&qwFileTimeMax), &sysTime))
		//	{
		//		__debugbreak();
		//	}
		//	else
		//	{
		//		__debugbreak();
		//	}
		//}
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Retrieve all of the omfi:UID data types and cache them in their own TOCX_ITEM structure.
//	Note that once we do this we can no longer read the OMF_MOB_IDs directly from the file because the cached values
//	clobber the file offsets.
//*********************************************************************************************************************
HRESULT CContainerLayer00::IngestMobIDs(void)
{
	HRESULT		hr			= S_OK;
	PTOCX_ITEM	pCurItem	= m_aToc;
	PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
	do {
		if ((pCurItem->dwDataType == m_dwTypeUID)&&
			(pCurItem->bStorageMode == SM_OFFSET))
		{
			if (pCurItem->cbLength64 == sizeof(OMF_MOB_ID))
			{
				// Read in three DWORDs (because that's what a OMF_MOB_ID is).
				// We will reserve the fourth DWORD as a BOOL to indicate when the OMF_MOB_ID is not unique.
				hr = SeekRead(pCurItem->cbOffset64, pCurItem->aCachedDwords, sizeof(OMF_MOB_ID));
				if (SUCCEEDED(hr))
				{
					// Update the storage mode.
					pCurItem->bStorageMode = SM_CACHED;

					// Wipe the fourth DWORD.
					pCurItem->aCachedDwords[3] = 0;
				}
				else
				{
					BREAK_IF_DEBUG
					// Fatal disk error in SeekRead()!
					// No way to recover!
					goto L_Exit;
				}
			}
		}
	} while (++pCurItem < pEndItem);

	if (m_fOmfBigEndian)
	{
		// Loop through everything we just did and byte-swap the members of each OMF_MOB_ID structure.
		pCurItem = m_aToc;
		do {
			if (pCurItem->dwDataType == m_dwTypeUID)
			{
				if (pCurItem->bStorageMode == SM_CACHED)
				{
					pCurItem->aCachedDwords[0] = Endian32(pCurItem->aCachedDwords[0]);	// dwPrefix
					pCurItem->aCachedDwords[1] = Endian32(pCurItem->aCachedDwords[1]);	// dwMajor
					pCurItem->aCachedDwords[2] = Endian32(pCurItem->aCachedDwords[2]);	// dwMinor
				}
			}
		} while (++pCurItem < pEndItem);
	}

//
L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Called once per lifetime from Load().
//	Detect any MOBJs in m_aToc[] that have duplicate UIDS in their OMFI:MOBJ:MobID properties.
//	This will return OMF_E_MOBJ_IDENTITY_CRISIS if two or more MOBJs share the same globally unique ID.
//	This condition is considered a fatal error, so it will abort at the first failure.
//*********************************************************************************************************************
HRESULT CContainerLayer00::DetectDuplicateMobIDs(void)
{
	HRESULT		hr = S_OK;
	PTOCX_ITEM	pOuterItem	= m_aToc;
	PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
	do {
		if ((pOuterItem->dwProperty == m_dwPropMobjMobID)&&
			(pOuterItem->dwDataType == m_dwTypeUID)&&
			(pOuterItem->bStorageMode == SM_CACHED))
		{
			PTOCX_ITEM pInnerItem = pOuterItem;
			while (++pInnerItem < pEndItem)
			{
				if ((pInnerItem->dwProperty == m_dwPropMobjMobID)&&
					(pInnerItem->dwDataType == m_dwTypeUID)&&
					(pInnerItem->bStorageMode == SM_CACHED))
				{
					if ((pInnerItem->aCachedDwords[2] == pOuterItem->aCachedDwords[2])&&	// dwMinor
						(pInnerItem->aCachedDwords[1] == pOuterItem->aCachedDwords[1])&&	// dwMajor
						(pInnerItem->aCachedDwords[0] == pOuterItem->aCachedDwords[0]))		// dwPrefix
					{
						// Some of my OMF1 files have MOBJs with _two_ OMFI:MOBJ:MobID properties in the same object.
						// But the good news is that if we are here then that means that both UIDs are the same value.
						// So if/when this happens it's harmless and not considered a fatal error.
						if (pInnerItem->dwObject == pOuterItem->dwObject)
						{
							// Just nullify the second instance.
							pInnerItem->dwProperty = (-1);
							pInnerItem->dwDataType = (-1);
							pInnerItem->aCachedDwords[0] = 0;
							pInnerItem->aCachedDwords[1] = 0;
							pInnerItem->aCachedDwords[2] = 0;
							pInnerItem->aCachedDwords[3] = 0;
						}
						else
						{
							// Two different MOBJs have the exact same OMFI:MOBJ:MobID property.
							BREAK_IF_DEBUG
							hr = OMF_E_MOBJ_IDENTITY_CRISIS;
							goto L_Exit;
						}
					}
					// 
					else if (pInnerItem->dwObject == pOuterItem->dwObject)
					{
						// A single MOBJ contains two OMFI:MOBJ:MobID properties and they are NOT the same!
						BREAK_IF_DEBUG
						hr = OMF_E_MOBJ_IDENTITY_CRISIS;
						goto L_Exit;
					}
				}
			}
		}
	} while (++pOuterItem < pEndItem);

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Called once per lifetime from Load().
//	Populate m_pStringCache[] and m_cbStringCache.
//	Once we do this there is no turning back.
//	We can no longer read the omfi:String data types from the file because the cache clobbers the file offset.
//*********************************************************************************************************************
HRESULT CContainerLayer00::IngestStrings(void)
{
	HRESULT	hr = S_OK;

	// Calculate the total size of all properties having the omfi:String data type.
	// Strings longer than OMFOO_STRMAX_STRING will not be cached, and so they will not be included in this count.
	m_cbStringCache	= DetermineStringCacheMemoryRequirements();

	// Reality check.
	// DetermineStringCacheMemoryRequirements() will return zero if the total accumulated
	// size of all omfi:String payloads exceed 67108863 (0x03FFFFFF) bytes.
	// This is just a safety precaution. I don’t expect this to ever happen.
	if (m_cbStringCache == 0)
	{
		BREAK_IF_DEBUG
		hr = OMFOO_E_ASSERTION_FAILURE;
		goto L_Exit;
	}

	// We set aside 16 bytes at the very end of m_pStringCache.
	typedef CHAR OMFOO_STRING_CACHE_SAFETY_ZONE[16];

	// Allocate a block of memory so we can read in the strings.
	// Note that the size of the safety zone is NOT included in m_cbStringCache.
	m_pStringCache = PCHAR(MemAlloc(m_cbStringCache + sizeof(OMFOO_STRING_CACHE_SAFETY_ZONE)));
	if (m_pStringCache == NULL)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_Exit;
	}

	// Now read in all omfi:String properties and concatenate them end-to-end.
	// Note that our concatenation process preserves the null terminators between each string.
	hr = PopulateStringCache();

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from IngestStrings().
//	Calculate the total size of all properties having the data type omfi:String.
//	Returns zero if the total accumulated size exceeds 67108863 (0x03FFFFFF) characters/bytes.  
//	Note that most omfi:String routines in omftoolkit.dll maxed out at 64 bytes, and a few others maxed out at 256.
//	So this is large enough to accommodate over a quarter-million maximum-length omfi:String payloads.
//*********************************************************************************************************************
UINT32 CContainerLayer00::DetermineStringCacheMemoryRequirements(void)
{
	PTOCX_ITEM	pCurItem	= m_aToc;
	PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
	UINT32	cbAllStrings	= 0;

	do 
	{
		if (pCurItem->dwDataType == m_dwTypeString)
		{
			// If an individual string purpports to be longer than OMFOO_STRMAX_STRING then we don't cache it.
			// We don't treat this as an error, but it might be an indication that something's screwed up.
			// Our basic string reading routines will still work if the string isn't cached.
			// But the string will not participate in string-search routines that only examine the cache.
			if (pCurItem->cbLength64 >= OMFOO_STRMAX_STRING)
			{
				continue;
			}

			if ((pCurItem->bStorageMode == SM_OFFSET) ||
				(pCurItem->bStorageMode == SM_IMMEDIATE))
			{
				// Add the length of this string to our accumulator.
				cbAllStrings += pCurItem->cbLengthLo;

				// Include one extra byte at the end of each string to assure that they are all null-terminated.
				cbAllStrings++;

				// If the total accumulated size of all concatenated strings exceeds 67108863 bytes/characters
				// then our memory allocator will probably fail. So we may as well bail out now.
				if (cbAllStrings > 0x03FFFFFF)
				{
					// Reset the running character count, exit this loop, and return zero to our caller.
					cbAllStrings = 0;
					break;
				}
			}
		}
	} while (++pCurItem < pEndItem);

	return cbAllStrings;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from IngestStrings().
//	Read all omfi:String properties into memory and concatenate them end-to-end.
//	Note that our concatenation process preserves the null terminators between each string.
//*********************************************************************************************************************
HRESULT CContainerLayer00::PopulateStringCache()
{
	HRESULT	hr	= S_OK;

	if ((m_pStringCache) && (m_cbStringCache))
	{
		PTOCX_ITEM	pCurItem	= m_aToc;
		PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
		LPSTR		pCurStr		= m_pStringCache;
		do
		{
			// If this TOCX_ITEM isn't a omfi:String then advance to the next TOCX_ITEM.
			if (pCurItem->dwDataType != m_dwTypeString)
			{
				continue;
			}

			if (pCurItem->bStorageMode == SM_IMMEDIATE)
			{
				if (pCurItem->cbLength64 <= sizeof(DWORD))
				{
					CopyMemory(pCurStr, pCurItem->aCachedBytes, pCurItem->cbLengthLo);

					// Wipe the entire sixteen-byte cache area for the current TOCX_ITEM.
					pCurItem->aCachedQwords[0] = 0;
					pCurItem->aCachedQwords[1] = 0;

					// Place the pointer to the base of the current string in the cache.
					pCurItem->pCachedString = pCurStr;

					// Advance pointer to the end of the current string.
					// Do not assume that pCurItem->cbLengthLo holds the exact string length.
					// There may be extra bytes after the terminator (or no terminator at all).
					// Instead call lstrlenA(). It's safe to call because we know that there will
					// always be extra zeros at the end - thanks to OMFOO_STRING_CACHE_SAFETY_ZONE.
					pCurStr += lstrlenA(pCurStr);

					// Advance one more character for the null-terminator.
					pCurStr++;

					// Update the storage mode.
					pCurItem->bStorageMode = SM_CACHED;
				}
				else
				{
					// Reality check.
					// How did we get here?
					// How can this happen?
					// Immediate value is larger than four bytes!
					BREAK_IF_DEBUG
				}
				continue;
			}

			if (pCurItem->bStorageMode != SM_OFFSET)
			{
				// Reality check.
				// How did we get here?
				// How can this happen?
				// The current TOCX_ITEM is a omfi:String but the storage mode isn't SM_OFFSET!
				BREAK_IF_DEBUG
				continue;
			}

			// If an individual string is longer than OMFOO_STRMAX_STRING then we don't cache it.
			// This is NOT considered an error. Our string read routines will still work, but they will
			// have to read the string from the file.
			if (pCurItem->cbLength64 >= OMFOO_STRMAX_STRING)
			{
				// Has ths ever happened?
				BREAK_IF_DEBUG
				continue;
			}

			if (FAILED(hr = SeekRead(pCurItem->cbOffset64, pCurStr, pCurItem->cbLengthLo)))
			{
				BREAK_IF_DEBUG
				// A file system error occured in SeekRead() while caching a omfi:String property.
				// This is a fatal error.
				// Exit and return the error code to our caller.
				break;
			}

			// Wipe the entire sixteen-byte cache area for the current TOCX_ITEM.
			pCurItem->aCachedQwords[0] = 0;
			pCurItem->aCachedQwords[1] = 0;

			// Place the pointer to the base of the current string in the cache.
			pCurItem->pCachedString = pCurStr;

			// Advance pointer to the end of the current string.
			// Do not assume that pCurItem->cbLengthLo holds the exact string length.
			// There may be extra bytes after the terminator (or no terminator at all).
			// Instead call lstrlenA(). It's safe to call because we know that there will
			// always be extra zeros at the end - thanks to OMFOO_STRING_CACHE_SAFETY_ZONE.
			pCurStr += lstrlenA(pCurStr);

			// Advance one more character for the null-terminator.
			pCurStr++;

			// Update the storage mode.
			pCurItem->bStorageMode = SM_CACHED;
		} while (++pCurItem < pEndItem);
	}

	return hr;
}

//*********************************************************************************************************************
//	Called once per lifetime from Load().
//	Try to figure out if this OMF file was created on a Mac or a PC.
//	We save our finding in m_dwHostOS.
//	The possible results are HOST_OS_WINDOWS, HOST_OS_MACINTOSH, and HOST_OS_UNKNOWN.
//	Some day we might want to improve this and make a distinction between HOST_OS_LEGACY_MAC and HOST_OS_MODERN_MAC.
//
//	The outstanding issue here is how to prioritize and rank these tests.
//
//	The OMFI Identification Class (IDNT) and its OMFI:IDNT:Platform property can provide this information,
//	but unfortunatly the OMFI:HEAD:IdentificationList and the OMFI:IDNT:Platform properties are both optional.
//	Further, an OMF file can conceivably have two or more Identification Class (IDNT) objects with conflicting
//	OMFI:IDNT:Platform strings. This can happen when the file was created on one platform and modified on another.
//	They are not mutually exclusive.
//*********************************************************************************************************************
HRESULT CContainerLayer00::DetectPlatform(void)
{
	HRESULT hr = S_OK;

	// Call helpers.
	CHECK(ScanIdntsForPlatform());
	CHECK(ScanNetlsForPlatform());

	// TEST #1
	// Check the results of ScanIdntsForPlatform().
	// The results are stored in m_nWinIdentities and m_nMacIdentities.
	// If the results are clear-cut and non-ambiguous, then use them to decide the platform.
	if ((m_nWinIdentities > 0) && (m_nMacIdentities == 0))
	{
		m_dwHostOS = HOST_OS_WINDOWS;
		goto L_Exit;
	}
	if ((m_nWinIdentities == 0) && (m_nMacIdentities > 0))
	{
		m_dwHostOS = HOST_OS_MACINTOSH;
		goto L_Exit;
	}

	// TEST #2
	// If the Bento layer or the OMF layer is big-endian then this OMF file was probably created on a Mac.
	if (m_fBentoBigEndian || m_fOmfBigEndian)
	{
		m_dwHostOS = HOST_OS_MACINTOSH;
		goto L_Exit;
	}

	// TEST #3
	// If any of these objects exist then this OMF file was probably created on a Mac.
	if (HasObjectOfClass(FCC('MACL')) ||	// Legacy Mac Locator
		HasObjectOfClass(FCC('SD2D')) ||	// Sound Designer II Media File Descriptor
		HasObjectOfClass(FCC('SD2M')) ||	// Sound Designer II Media Data
		HasObjectOfClass(FCC('SDSL')))		// Sound Designer II Media File Locator
	{
		m_dwHostOS = HOST_OS_MACINTOSH;
		goto L_Exit;
	}

	// TEST #4
	// Check the results of ScanNetlsForPlatform().
	// The results are stored in m_nWinNetls and m_nMacNetls.
	// If the results are clear-cut and non-ambiguous, then use them to decide the platform.
	if ((m_nWinNetls > 0) && (m_nMacNetls == 0))
	{
		m_dwHostOS = HOST_OS_WINDOWS;
		goto L_Exit;
	}
	if ((m_nWinNetls == 0) && (m_nMacNetls > 0))
	{
		m_dwHostOS = HOST_OS_MACINTOSH;
		goto L_Exit;
	}

	// TEST #5
	// If any of these objects exist then this OMF file was probably created on a Windows machine.
	if (HasObjectOfClass(FCC('WINL')) ||	// Windows Locator Class (WINL)
		HasObjectOfClass(FCC('DOSL')))		// DOS Locator Class (DOSL)
	{
		m_dwHostOS = HOST_OS_WINDOWS;
		goto L_Exit;
	}

	// TEST #6
	// If this file has WAVE Audio Descriptor Class (WAVD) objects and no AIFC Audio Descriptor Class (AIFD)
	// objects then it was probably created on a Windows machine.
	if (HasObjectOfClass(FCC('WAVD')) && !HasObjectOfClass(FCC('AIFD')))
	{
		m_dwHostOS = HOST_OS_WINDOWS;
		goto L_Exit;
	}

	// I am out of ideas.
	// Maybe try these as a last resort ...

	// TEST #N
	// Scan the IDNTs again. This time examine their OMFI:IDNT:CompanyName and OMFI:IDNT:ProductName strings.
	// Look for companies and products that only support one platform and not any other.

	// Test #N
	// Scan all Avid Media File Manager Locator Class (MFML) objects. Afaik these only exist in OMF1.
	// Test their OMFI:MFML:LastKnownVolume strings.
	// These strings include drive-letter/colon combinations that can only exist on Windows.
	// They are always enclosed in parentheses.
	// A real-life example is "PW7389 (E:)"

	// Test #N
	// Scan all Avid Media Stream Manager Locator Class (MSML) objects. Afaik these only exist in OMF1.
	// Test their OMFI:MSML:LastKnownVolume strings.
	// These strings include drive-letter/colon combination that can only exist on Windows.
	// They are always enclosed in parentheses.
	// Real life examples are "Media (E:)" and "TRIAL BY JURY (G:)".

	// TEST #N
	// Scan all strings in the string cache for any words that might indicate the platform.
	// For example "Macintosh HD"
	// Another example "pict" "PICT" or ".pct"

	// TEST #N
	// If there are any Windows Locator Class (WINL) objects with the OMFI:WINL:Shortcut property
	// then this file was almost certainly created on a Windows machine.
	//if (HasPropertyInstances(OrdinalToPropertyID(ePropWinlShortcut)))
	//{
	//	m_dwHostOS = HOST_OS_WINDOWS;
	//	goto L_Exit;
	//}

	// If there are any Mac Locator Class (MACL) objects with the OMFI:MACL:DirID property
	// then this file was almost certainly created on a legacy big-endian 68k/PowerPC Macintosh.
	//if (HasPropertyInstances(OrdinalToPropertyID(ePropMaclDirID)))
	//{
	//	m_dwHostOS = HOST_OS_LEGACY_MAC;
	//	goto L_Exit;
	//}

//#ifdef _DEBUG
//	__debugbreak();
//	OutputDebugStringW(m_wzFullPath);
//	__debugbreak();
//#endif

	// As a last resort ...
	m_dwHostOS = HOST_OS_UNKNOWN;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from DetectPlatform().
//	We come here to analyze all strings belonging to the OMFI:IDNT:Platform property.
//	We search for strings that can tell us if this OMF file was created on a Macintosh or a Windows PC.
//
//	We save our results here -
//		m_nMacIdentities	- the number of OMFI:IDNT:Platform properties that identify as Apple, Mac, or Macintosh.
//		m_nWinIdentities	- the number of OMFI:IDNT:Platform properties that identify as Microsoft, Win, or Windows.
//		m_nTotalIdentities	- the total number of all OMFI:IDNT:Platform properties regardless of what they indicate.
//
//*********************************************************************************************************************
HRESULT CContainerLayer00::ScanIdntsForPlatform()
{
	// We must call IngestStrings() before comming here.
	if ((m_pStringCache == NULL) || (m_cbStringCache == 0))
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}

	// OMFI:IDNT:Platform
	DWORD dwPropPlatform = OrdinalToPropertyID(ePropIdntPlatform);
	if (dwPropPlatform)
	{
		PTOCX_ITEM pCurItem = m_aToc;
		PTOCX_ITEM pEndItem = &m_aToc[m_nTocItems];
		do
		{
			if ((pCurItem->dwProperty == dwPropPlatform) &&	// OMFI:IDNT:Platform
				(pCurItem->dwDataType == m_dwTypeString) &&	// omfi:String
				(pCurItem->bStorageMode == SM_CACHED))		// must live in m_pStringCache[]
			{
				// Increment the number of all OMFI:IDNT:Platform properties regardless of how they identify.
				m_nTotalIdentities++;

				// Search for "WIN32", "win32", "Windows", "Windows NT", "WINNT 4.0", etc.
				if (0 == StrCmpNIA(pCurItem->pCachedString, "Win", 3))
				{
					m_nWinIdentities++;
					continue;
				}

				// Search for "Microsoft"
				if (0 == StrCmpNIA(pCurItem->pCachedString, "Microsoft", 9))
				{
					m_nWinIdentities++;
					continue;
				}

				// Search for "Macintosh", "Mac", "MacOS", "macOS", etc.
				if (0 == StrCmpNIA(pCurItem->pCachedString, "Mac", 3))
				{
					m_nMacIdentities++;
					continue;
				}

				// Search for "Apple Computer", "Apple", etc.
				if (0 == StrCmpNIA(pCurItem->pCachedString, "Apple", 5))
				{
					m_nMacIdentities++;
					continue;
				}
			}
		} while (++pCurItem < pEndItem);
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from DetectPlatform().
//	We come here to analyze all strings belonging to the OMFI:NETL:URLString property.
//	If the string begins with "file:///Users/" then that indicates that the file was created on a Darwin-based Mac.
//	But if the string begins with a drive designator ("C:\") then that indicates that the file was created on Windows.
//
//	We save our results here -
//		m_nMacNetls		- the number of OMFI:NETL:URLString properties that indicate Macintosh.
//		m_nWinNetls		- the number of OMFI:NETL:URLString properties that indicate Windows.
//		m_nNetlsTotal	- the total number of all OMFI:NETL:URLString properties regardless of what they indicate.
//*********************************************************************************************************************
HRESULT CContainerLayer00::ScanNetlsForPlatform()
{
	// We must call IngestStrings() before comming here.
	if ((m_pStringCache == NULL) || (m_cbStringCache == 0))
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}

	// OMFI:NETL:URLString
	DWORD dwPropUrlString = OrdinalToPropertyID(ePropNetlURLString);
	if (dwPropUrlString)
	{
		PTOCX_ITEM pCurItem = m_aToc;
		PTOCX_ITEM pEndItem = &m_aToc[m_nTocItems];
		do
		{
			if ((pCurItem->dwProperty == dwPropUrlString) &&	// OMFI:NETL:URLString
				(pCurItem->dwDataType == m_dwTypeString) &&		// omfi:String
				(pCurItem->bStorageMode == SM_CACHED))			// must live in m_pStringCache[]
			{
				// Increment the number of all OMFI:NETL:URLString properties regardless of how they identify.
				m_nNetlsTotal++;

				// Case sensitive search for "file:///Users/" to detect Darwin-based macOS.
				// A Windows machine will never create this, and neither will a legacy Macintosh.
				if (0 == StrCmpNA(pCurItem->pCachedString, "file:///Users/", 14))
				{
					m_nMacNetls++;
				}
				else
				{
					// Convert the file URL into a Windows path.
					CHAR	szPath[512] = {0};
					ULONG	cchPath		= ELEMS(szPath);
					if (SUCCEEDED(PathCreateFromUrlA(pCurItem->pCachedString, szPath, &cchPath, 0)))
					{
						// If the path includes a drive designator then it couldn't have originated on a Mac.
						BYTE ch = szPath[1];
						if (((ch >= 'A')&&(ch <= 'Z'))||((ch >= 'a')&&(ch <= 'z')) &&
							(szPath[0] == '\\') &&
							(szPath[2] == ':') &&
							(szPath[3] == '\\'))
						{
							m_nWinNetls++;
						}
						//else
						//{
						//	m_nNetlsOther++;
						//}
					}
				}
			}
		} while (++pCurItem < pEndItem);
	}

	return S_OK;	
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	We must call IngestStrings() first.
//	Here we try to decide on a codepage for m_dwDetectedCodePage.
//	On entry these members must be valid:

//		m_cbStringCache
//		m_pStringCache[]
//
//	On exit ...
//		m_dwDetectedCodePage	= CP_ASCII, CP_UTF8, or zero (undetected).
//
//*********************************************************************************************************************
HRESULT CContainerLayer00::DetectCodePagePartOne(void)
{
	// We must call IngestStrings() before comming here.
	if ((m_pStringCache == NULL) || (m_cbStringCache == 0))
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}

	// Take inventory of the characters and control codes in the cache.
	for (SIZE_T i = 0; i < m_cbStringCache; i++)
	{
		BYTE ch = m_pStringCache[i];
		m_aCharCounters[ch]++;
	}

	// This loop counts the control codes.
	for (SIZE_T i = 0x01; i < 0x20; i++)
	{
		m_cchControls += m_aCharCounters[i];
	}

	// This loop counts the printable ASCII.
	for (SIZE_T i = 0x20; i < 0x7F; i++)
	{
		m_cchPrintableAscii += m_aCharCounters[i];
	}

	// This loop counts the extended ASCII.
	for (SIZE_T i = 0x80; i <= 0xFF; i++)
	{
		m_cchNonAscii += m_aCharCounters[i];
	}

	// The ASCII DEL (Delete) code is also a control code, so add these to the counter.
	m_cchControls += m_aCharCounters[0x7F];

	// If the cache includes printable ASCII and no control codes ...
	if ((m_cchControls == 0) && (m_cchPrintableAscii > 0))
	{
		// ASCII is the easiest to detect, and it is by far the most common.
		// 95% of the time we're done right here!
		if (m_cchNonAscii == 0)
		{
			// Game over!
			m_dwDetectedCodePage = CP_ASCII;
		}
		else
		{
			// Test for UTF-8.
			// Admittedly - the strings are probably not UTF-8 but it’s easy to detect.
			// The routines in omfToolkit.dll use the ANSI C library string functions and they can certainly handle it.
			// UTF-8 has been available since mid-2001 on Windows and Mac. So it is at least conceivable that this OMF
			// file might use it.
			if (IsHalfUtf8(m_pStringCache, m_cbStringCache))
			{
				// If our own pre-screening test passes then hand it off to MultiByteToWideChar().
				if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, m_pStringCache, m_cbStringCache, NULL, 0))
				{
					m_dwDetectedCodePage = CP_UTF8;
				}
			}
		}
	}

	// 
	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Must call DetectCodePagePartOne() and DetectPlatform() before calling here.
//	On exit m_dwWorkingCodePage is either m_dwDetectedCodePage, CP_ACP, or CP_MACCP.
//*********************************************************************************************************************
HRESULT CContainerLayer00::DetectCodePagePartTwo()
{
	// In DetectCodePagePartOne() we tested for ASCII and UTF-8.
	// If it succeeded then m_dwDetectedCodePage is non-zero.
	// Skip ahead, we are almost done!
	if (m_dwDetectedCodePage)
	{
		goto L_UpdateExit;
	}

	// If the platform is a Mac ...
	if (m_dwHostOS == HOST_OS_MACINTOSH)
	{
		// Test the default legacy Macintosh code page for this session.
		// Unfortunately this will probably always succeed (a false positive) because all of the
		// Legacy Macintosh character sets used all of the codes in the entire range of 0x00~0xFF.
		if (MultiByteToWideChar(m_dwLegacyMacCodePage, MB_ERR_INVALID_CHARS,
												m_pStringCache, m_cbStringCache, NULL, 0))
		{
			m_dwDetectedCodePage = m_dwLegacyMacCodePage;
			goto L_UpdateExit;
		}
		// How did we get here?
		// Make sure m_dwLegacyMacCodePage is 10000 ~ 10082.
		// Don't treat this as a fatal error.
		BREAK_IF_DEBUG
	}

	// Try the default ANSI Windows code page for this session.
	// Typical Windows code pages are 1250~1258, but not always.
	// Note that some of the upper codes (0x80~0xFF) in some of these pages are undefined.
	// That's actually a good thing because it may help MultiByteToWideChar() reject incorrect pages.
	if (MultiByteToWideChar(m_dwWindowsAnsiCodePage, MB_ERR_INVALID_CHARS,
											m_pStringCache, m_cbStringCache, NULL, 0))
	{
		m_dwDetectedCodePage = m_dwWindowsAnsiCodePage;
		goto L_UpdateExit;
	}

	// If that didn't work then walk through all of the Windows Latin/ANSI code pages (except the one we just tested).
	// If any succeed then make it the detectd code page.
	for (DWORD dwCodePage = 1250; dwCodePage <= 1258; dwCodePage++)
	{
		if (dwCodePage != m_dwWindowsAnsiCodePage)
		{
			if (MultiByteToWideChar(dwCodePage, MB_ERR_INVALID_CHARS,
									PCSTR(m_pStringCache), m_cbStringCache, NULL, 0))
			{
				m_dwDetectedCodePage = dwCodePage;
				goto L_UpdateExit;
			}
		}
	}

	// This is our last-resort and default catch-all handler.
	// Use the Windows MultiLanguage2 Object to detect the code page.
	// This will never detect a Macintosh code page!
	m_dwDetectedCodePage	= MLangDetectCodePageA(m_pStringCache, m_cbStringCache);

L_UpdateExit:
	// Save the detected code page as the working code page.
	m_dwWorkingCodePage = m_dwDetectedCodePage;

	// Always succeed.
	return S_OK;
}

