// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer15.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer15.h"
#include "StreamOnReadableFile.h"
#include "DllMain.h"
#include <shlwapi.h>

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer15::CContainerLayer15(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer15::~CContainerLayer15(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer15::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//	if (SUCCEEDED(hr))
//	{
//		hr = ????????????
//	}
//	return hr;
//}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//*********************************************************************************************************************
ULONG CContainerLayer15::GetMdatCategory(__in ULONG idx)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return m_aMdatTable[idx].bCategory;
	}

	#ifdef _DEBUG
	if (idx != (-1))
	{
		__debugbreak();
	}
	#endif

	return 0;
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaDesctiptor class.
//*********************************************************************************************************************
DWORD CContainerLayer15::GetMdatMediaDataObject(__in ULONG idx)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return m_aMdatTable[idx].oMDAT.dwObject;
	}

	#ifdef _DEBUG
	if (idx != (-1))
	{
		__debugbreak();
	}
	#endif

	return 0;
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//*********************************************************************************************************************
DWORD CContainerLayer15::GetMdatSourceMobObject(__in ULONG idx)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return m_aMdatTable[idx].oSMOB.dwObject;
	}
	else
	{
		BREAK_IF_DEBUG
		return 0;
	}
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//*********************************************************************************************************************
DWORD CContainerLayer15::GetMdatMediaDescriptorObject(__in ULONG idx)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return m_aMdatTable[idx].oMDES.dwObject;
	}
	else
	{
		BREAK_IF_DEBUG
		return 0;
	}
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:xxxx:MobID property, where xxxx is the class fourCC of the subclass.
//	OMF2 - Retrieves the OMFI:MDAT:MobID property.
//*********************************************************************************************************************
HRESULT CContainerLayer15::GetMdatMobID(__in ULONG idx, __out POMF_MOB_ID pMobID)
{
	// OMFI:MDAT:MobID	<- rev 1 & rev 2
	// OMFI:AIFC:MobID	<- rev 1
	// OMFI:IDAT:MobID	<- rev 1
	// OMFI:JPEG:MobID	<- rev 1
	// OMFI:TIFF:MobID	<- rev 1
	// OMFI:WAVE:MobID	<- rev 1
	// OMFI:????:MobID	<- rev 1 - could be the fourCC of an extended vendor-defined class.
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		if (IsBadWritePointer(pMobID, sizeof(OMF_MOB_ID)))
		{
			return E_POINTER;
		}
		else
		{
			pMobID->dwPrefix = m_aMdatTable[idx].mobID.dwPrefix;
			pMobID->dwMajor = m_aMdatTable[idx].mobID.dwMajor;
			pMobID->dwMinor = m_aMdatTable[idx].mobID.dwMinor;
			return S_OK;
		}
	}
	else
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//	It finds the nth entry in m_aMdatTable[] and passes its reference to the like-named counterpart.
//	For more into see the non-overloaded implementation.
//*********************************************************************************************************************
HRESULT CContainerLayer15::GetMdatRawFileParams(__in ULONG idx,
													__out PUINT64 pOffset,
														__out PUINT64 pLength,
															__out_opt PHANDLE pHandle)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return QueryMdatRawFileParams(m_aMdatTable[idx], pOffset, pLength, pHandle);
	}
	else
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//	It finds the nth entry in m_aMdatTable[] and passes its reference to the like-named counterpart.
//	For more into see the non-overloaded implementation.
//*********************************************************************************************************************
HRESULT CContainerLayer15::CreateStreamOnMdatData(__in ULONG idx, __in REFIID riid, __out PVOID *ppvOut)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return CreateStreamOnMdatData(m_aMdatTable[idx], riid, ppvOut);
	}
	else
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//	Creates a DirectShow source filter on the subclass's payload that can be used with the DirectShow API.
//*********************************************************************************************************************
HRESULT CContainerLayer15::CreateDShowSourceFilterOnData(__in ULONG idx,
															__in REFIID riid,
																__out PVOID *ppvOut)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return CreateDShowSourceFilterOnData(m_aMdatTable[idx], riid, ppvOut);
	}
	else
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//	It finds the nth entry in m_aMdatTable[] and passes its reference to the like-named counterpart.
//	For more into see the non-overloaded implementation.
//*********************************************************************************************************************
HRESULT CContainerLayer15::CreateFileNameForMdat(__in ULONG idx,
													__in DWORD dwSyntax,
														__in ULONG cchBuffer,
															__out_opt PWCHAR pBuffer,
																__out_opt PULONG pcchRequired)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return CreateFileNameForMdat(m_aMdatTable[idx], dwSyntax, cchBuffer, pBuffer, pcchRequired);
	}
	else
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
}

//*********************************************************************************************************************
//	Callback/helper routine for our COmfMediaData class.
//	It finds the nth entry in m_aMdatTable[] and passes its reference to the like-named counterpart.
//	For more into see the non-overloaded implementation.
//*********************************************************************************************************************
HRESULT CContainerLayer15::ExtractMdatDataToFile(__in ULONG idx,
													__in PCWSTR pwzDestFullPath,
														__in_opt IOmfooExtractCallback *pCallback,
															__in_opt ULONG nPagesPerCallback,
																__in BOOL fOverwrite)
{
	if (m_aMdatTable && (idx < m_cMDATs))
	{
		return ExtractMdatDataToFile(m_aMdatTable[idx], pwzDestFullPath, pCallback, nPagesPerCallback, fOverwrite);
	}
	else
	{
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
}

//*********************************************************************************************************************
//	Retrieves the physical file position and length for MDAT's embedded payload.
//*********************************************************************************************************************
HRESULT CContainerLayer15::QueryMdatRawFileParams(__in MDAT_CACHE_ENTRY& rCE,
														__out PUINT64 pOffset,
															__out PUINT64 pLength,
																__out_opt PHANDLE pHandle)
{
	if (IsBadWritePointer(pOffset, sizeof(UINT64))||
		IsBadWritePointer(pLength, sizeof(UINT64)))
	{
		return E_POINTER;
	}
	else
	{
		*pOffset	= rCE.cbPayloadOffset;
		*pLength	= rCE.cbPayloadLength;
	}

	// Optional
	if (pHandle)
	{
		if (IsBadWritePointer(pHandle, sizeof(HANDLE)))
		{
			return E_POINTER;
		}
		else
		{
			// Now open the file with shared read-only rights.
			HANDLE hFile = CreateFileW(m_pwzFullPath,			// file to open
									GENERIC_READ,				// open for reading
									FILE_SHARE_READ,			// share for reading
									NULL,						// default security
									OPEN_EXISTING,				// existing file only
									FILE_ATTRIBUTE_READONLY,	// read only
									NULL);						// no attr. template

			// Return handle to caller even if it's bad.
			*pHandle = hFile;

			// Return meaningful error code if it's bad.
			if (hFile == INVALID_HANDLE_VALUE)
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Creates a read-only Windows OLE IStream object on the media data, and then queries it for the interface
//	specified by riid. This implementation exposes IStream and ISequentialStream.
//*********************************************************************************************************************
HRESULT CContainerLayer15::CreateStreamOnMdatData(__in MDAT_CACHE_ENTRY& rCE, __in REFIID riid, __out PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		UINT64		cbOffset	= rCE.cbPayloadOffset;
		UINT64		cbLength	= rCE.cbPayloadLength;
		LPUNKNOWN	pOwner		= LPUNKNOWN(static_cast<INonDelegatingUnknown*>(this));

		hr = E_OUTOFMEMORY;

		// Note that CStreamOnReadableFile is created with an outstanding reference count of 1.
		CStreamOnReadableFile* pStream = new CStreamOnReadableFile(pOwner);
		if (pStream)
		{
			if (SUCCEEDED(hr = pStream->OpenReadableFile(m_pwzFullPath)))
			{
				if (SUCCEEDED(hr = pStream->SetRegion(cbOffset, cbLength)))
				{
					hr = pStream->QueryInterface(riid, ppvOut);
				}
			}
			pStream->Release();
			pStream = NULL;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Creates a DirectShow source filter on the media data, and then queries it for the interface
//	specified by riid. This implementation exposes IBaseFilter.
//*********************************************************************************************************************
HRESULT CContainerLayer15::CreateDShowSourceFilterOnData(__in MDAT_CACHE_ENTRY& rCE,
															__in REFIID riid,
																__out PVOID *ppvOut)
{
//	__debugbreak();

	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		//UINT64		cbOffset	= rCE.cbPayloadOffset;
		//UINT64		cbLength	= rCE.cbPayloadLength;
		//LPUNKNOWN	pOwner		= LPUNKNOWN(static_cast<INonDelegatingUnknown*>(this));

		//hr = E_OUTOFMEMORY;

		//// Note that CStreamOnReadableFile is created with an outstanding reference count of 1.
		//CStreamOnReadableFile* pStream = new CStreamOnReadableFile(pOwner);
		//if (pStream)
		//{
		//	if (SUCCEEDED(hr = pStream->OpenReadableFile(m_pwzFullPath)))
		//	{
		//		if (SUCCEEDED(hr = pStream->SetRegion(cbOffset, cbLength)))
		//		{
		//			hr = pStream->QueryInterface(riid, ppvOut);
		//		}
		//	}
		//	pStream->Release();
		//	pStream = NULL;
		//}
	}

	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Creates a filename (with an appropriate filename extension) that can be used use with ExtractDataToFile().
//	The content/format/style/syntax of the string is determined as follows:
//
//	SYNTAX	DESCRIPTION
//	  0		Return a filename extension only, including the leading dot.
//			If Omfoo cannot provide a meaningful filename extension this will return ".raw"
//
//	  1		Create a filename based on the MDAT's category ('audio', 'video', or 'media') along
//			with an enumerated category instance number.
//			For example "Audio File #08.wav", "Audio File #13.wav", "Video File #02.dv", or "Media File #1.raw".
//
//	  2		Return a hexadecimal representation of the MDAT's OMFI:MDAT:MobID property (where the three
//			32-bit fields are separated by underscores), followed by the filename extension.
//			For example "75B0C5A4_B0F5A485_F39EDE2C.wav"
//
//	  3		Same as rule #2 above, but include a prefix derived from the name of the loaded OMF file.
//			For example if the omf file is named "TripToTheMoon.omf" then the name my look something
//			like "Trip_75B0C5A4_B0F5A485_F39EDE2C.wav"
//
//	  4		Same as rule #3 above, but include a prefix derived from the name of the top level composition mob.
//			For example if the top level composition mob is named "Wizard of Oz" then the name may look something
//			like "Wizard_75B0C5A4_B0F5A485_F39EDE2C.wav"
//
//	  5		Create a filename based entirely on the name of the Source Mob (SMOB) associated with the MDAT.
//			This rule will only succeed if all Source Mobs have unique names, and if all of those names consist
//			entirely of valid Win32 filename characters. Otherwise this will return OMF_E_CANT_COMPLETE.
//*********************************************************************************************************************
HRESULT CContainerLayer15::CreateFileNameForMdat(__in MDAT_CACHE_ENTRY& rCE,
														__in DWORD dwSyntax,
															__in ULONG cchBuffer,
																__out PWCHAR pBuffer,
																	__out PULONG pcchRequired)
{
	WCHAR	wzFileNameExt[12]			= {0};
	WCHAR	wzResult[MAX_PATH_FILENAME]	= {0};
	WCHAR	wzTopMobName[96]			= {0};
	WCHAR	wzHexMobID[36]				= {0};	// 1 + lstrlen("01234567_89ABCDEF_01234567_XXXXXXXX")
	WCHAR	wzCategoryInstance[8]		= {0};	// 32767
	HRESULT	hr = S_OK;

	// Populate wzHexMobID[].
	UInt32ToHexW(rCE.mobID.dwPrefix, &wzHexMobID[0]);
	UInt32ToHexW(rCE.mobID.dwMajor, &wzHexMobID[9]);
	UInt32ToHexW(rCE.mobID.dwMinor, &wzHexMobID[18]);
	wzHexMobID[8]	= '_';
	wzHexMobID[17]	= '_';

	// If this file has two or more MDATs with identical OMFI:MDAT:MobID properties then we need to
	// make wzHexMobID unique by appending a hex representation of the MDAT's 32-bit Bento object ID.
	if (m_cMdatDupUIDs)
	{
		wzHexMobID[26]	= '_';
		UInt32ToHexW(rCE.oMDAT.dwObject, &wzHexMobID[27]);
	}

	// Transform the ASCII filename extension (which lives in a QWORD) into a UTF-16 string.
	wzFileNameExt[0] = BYTE('.');
	wzFileNameExt[1] = PBYTE(&rCE.qwFileNameExt)[0];
	wzFileNameExt[2] = PBYTE(&rCE.qwFileNameExt)[1];
	wzFileNameExt[3] = PBYTE(&rCE.qwFileNameExt)[2];
	wzFileNameExt[4] = PBYTE(&rCE.qwFileNameExt)[3];
	wzFileNameExt[5] = PBYTE(&rCE.qwFileNameExt)[4];
	wzFileNameExt[6] = PBYTE(&rCE.qwFileNameExt)[5];
	wzFileNameExt[7] = PBYTE(&rCE.qwFileNameExt)[6];
	wzFileNameExt[8] = PBYTE(&rCE.qwFileNameExt)[7];
	wzFileNameExt[9] = 0;

	// Remove any trailing spaces that may exist in the filename extension.
	// Extra spaces are normal and expected.
	// It can happen because of the way some routines return ASCII characters packed into QWORDs.
	PathRemoveBlanksW(wzFileNameExt);

	switch (dwSyntax)
	{
	case 0:
		// Return a filename extension only, including the leading dot.
		lstrcpyW(wzResult, wzFileNameExt);
		break;

	case 1:
		// Create a filename based on the MDAT's category ('audio', 'video', or 'media') with an instance number.
		// For example "Audio File #12.wav", "Video File #04.dv", or "Media File #01.raw".
		if (rCE.bCategory == MDAT_CATEGORY_AUDIO_FILE)
		{
			lstrcpyW(wzResult, L"Audio File #");
		}
		else if (rCE.bCategory == MDAT_CATEGORY_VIDEO_FILE)
		{
			lstrcpyW(wzResult, L"Video File #");
		}
		else
		{
			lstrcpyW(wzResult, L"Media File #");
		}
		UIntToDecimal9999W(m_cMDATs+1, rCE.wCategoryInstance, wzCategoryInstance);
		lstrcatW(wzResult, wzCategoryInstance);
		lstrcatW(wzResult, wzFileNameExt);
		break;

	case 2:
		// Return the MDAT's OMFI:MDAT:MobID property in hex, followed by the filename extension.
		// For example "75B0C5A4_B0F5A485_F39EDE2C.wav"
		lstrcpyW(wzResult, wzHexMobID);
		lstrcatW(wzResult, wzFileNameExt);
		break;

	case 3:
		// Same as rule #2 above, but includes a prefix derived from the name of the loaded OMF file.
		// For example if the OMF file is named "Wizard of Oz.omf" then the result will look something
		// like "Wizard_75B0C5A4_B0F5A485_F39EDE2C.wav"
		lstrcpyW(wzResult, m_wzExportPrefix);	// up to EXPORT_PREFIX_MAX-1 WCHARs
		lstrcatW(wzResult, L"_");				// always 1 WCHAR
		lstrcatW(wzResult, wzHexMobID);			// always 26 WCHARs
		lstrcatW(wzResult, wzFileNameExt);
		break;

	case 4:
		// Same as #3 above, but include a prefix derived from the name of the top level composition mob.
		// For example if the top level composition mob is named "TripToTheMoon" then the name may look
		// something like "Trip_75B0C5A4_B0F5A485_F39EDE2C.wav"

		// We create/re-create m_wzTopMobPrefix 'on the fly' whenever we detect a change in m_dwWorkingCodePage.
		if (m_dwWorkingCodePage != m_dwCheckCodePage)
		{
			// Assume we will fail!
			hr = OMF_E_CANT_COMPLETE;

			// Update m_dwCheckCodePage.
			m_dwCheckCodePage = m_dwWorkingCodePage;

			// Forget the current prefix.
			m_wzTopMobPrefix[0] = 0;

			// Make sure we found a top level mob.
			if (m_dwTopLevelMob == 0)
			{
				hr = OMF_E_MOB_NOT_FOUND;
			}
			else
			{
				// Read the OMFI:MOBJ:Name (or OMFI:CPNT:Name) property from the top level composition mob.
				ULONG cchMobName = 0;
				hr = ObjReadStringW(m_dwTopLevelMob, m_dwPropMobjName, ELEMS(wzTopMobName), wzTopMobName, &cchMobName);
				if (SUCCEEDED(hr))
				{
					// Test for invalid filename characters and path separators.
					for (UINT i = 0; i < cchMobName; i++)
					{
						UINT uType = PathGetCharTypeW(wzTopMobName[i]);
						if ((uType == GCT_INVALID)||(uType & (GCT_WILD|GCT_SEPARATOR)))
						{
							// If we encounter an invalid character (or path separator) we just truncate the string.
							wzTopMobName[i] = 0;
							break;
						}
					}

					hr = CreatePrefixW(wzTopMobName, ELEMS(m_wzTopMobPrefix), m_wzTopMobPrefix);
				}
				else
				{
					// If ObjReadStringW() failed then we return its return code.
					BREAK_IF_DEBUG
				}
			}
		}

		// If the string in m_wzTopMobPrefix[] is valid ...
		if (SUCCEEDED(hr))
		{
			lstrcpyW(wzResult, m_wzTopMobPrefix);	// up to EXPORT_PREFIX_MAX-1 WCHARs
			lstrcatW(wzResult, L"_");				// always 1 WCHAR
			lstrcatW(wzResult, wzHexMobID);			// always 26 WCHARs
			lstrcatW(wzResult, wzFileNameExt);
		}
		break;

	case 5:
		// Create a filename using the name of the Source Mob (SMOB) associated with the MDAT.
		// This will only succeed if all Source Mobs have unique names, and only if every name consists
		// entirely of valid Win32 filename characters. Otherwise this will return OMF_E_CANT_COMPLETE.
		if (m_cMDATs == m_cSmobsWithUniqueNames)
		{
			if (MultiByteToWideChar(m_dwWorkingCodePage,
									DWORD(0),
									rCE.szSmobName,
									INT(-1),
									wzResult,
									ELEMS(wzResult)))
			{
				lstrcatW(wzResult, wzFileNameExt);
			}
			else
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
		}
		else
		{
			hr = OMF_E_CANT_COMPLETE;
		}
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}

	return CallTwiceStringHandlerW(hr, wzResult, cchBuffer, pBuffer, pcchRequired);	
}

//*********************************************************************************************************************
//	Extracts/exports the embedded media data to a file.
//	If a file with the specifed name already exists and fOverwrite is TRUE, then that file will be overwritten.
//*********************************************************************************************************************
HRESULT CContainerLayer15::ExtractMdatDataToFile(__in MDAT_CACHE_ENTRY& rCE,
													__in PCWSTR pwzDestFullPath,
														__in_opt IOmfooExtractCallback *pUnknown,
															__in_opt ULONG nPagesPerCallback,
																__in BOOL fOverwrite)
{
	// Kludge to patch invalid FORM ckSize values on the fly.
	// Some AIFC and WAVE payloads (based on the Electronic Arts 'EA IFF 85' standard) have an invalid ckSize value.
	// The invalid ckSize erroneously represents the size of the entire OMF file minus eight.
	// (It should represent the size of the embedded WAV or AIF file minus eight.)
	// These files probably began life as standard *.aif or *.wav files that were later transformed into *.omf files
	// by appending OMF data to then end. This flaw exists in OMF1 and OMF2 files, and I suspect that they were created
	// with OMF Toolkit Versions prior to v2.1.2 because v2.1.2 introduced a new method called omfsAppendRawFile()
	// that automated this transformation.
	//
	// >>>> So here's part 1 (of 2) of the IFF ckSize kludge. <<<<
	UINT32 cbPatchChunkSize = 0;
	if ((rCE.oMDAT.dwFourCC == FCC('AIFC'))&&(rCE.cbPayloadLength >= 8)&&(rCE.cbPayloadLength < 0xFFFFFFFF))
	{
		cbPatchChunkSize = Endian32(UINT32(rCE.cbPayloadLength-8));
	}
	else if ((rCE.oMDAT.dwFourCC == FCC('WAVE'))&&(rCE.cbPayloadLength >= 8)&&(rCE.cbPayloadLength < 0xFFFFFFFF))
	{
		cbPatchChunkSize = UINT32(rCE.cbPayloadLength-8);
	}
	// >>>> End of part 1 (of 2) of the IFF ckSize kludge. <<<<

	HRESULT			hr		= S_OK;
	LARGE_INTEGER	liPtrEx	= {0};

	UINT64	cbReadPosition	= rCE.cbPayloadOffset;
	UINT64	cbReadRemaining	= rCE.cbPayloadLength;
	UINT64	cbRoundUpLength	= rCE.cbPayloadLength;

	// Round up the payload length to the next page boundary (4096 bytes).
	if (WORD(cbRoundUpLength) & 0x0FFF)
	{
		cbRoundUpLength |= 0x0FFF;
		cbRoundUpLength++;
	}

	// Two OVERLAPPED structures.
	OVERLAPPED	ovlRead		= {0};
	OVERLAPPED	ovlWrite	= {0};

	// Two file handles.
	HANDLE	hFileRead		= NULL;
	HANDLE	hFileWrite		= NULL;

	// A two-element array of pointers to two memory banks.
	PBYTE	aMemBank[2]		= {NULL, NULL};

	// Size of each memory bank.
	UINT32	cbMemoryBank	= 0;

	// Pointer to the current memory bank.
	PBYTE	pCurrentBank	= NULL;

	// Counter to determine which bank pCurrentBank points to.
	ULONG	nToggles		= 0;

	// Two functional wait events, plus a third NULL event.
	union {
	struct {
	HANDLE	hEventRead;
	HANDLE	hEventWrite;
	HANDLE	hNullEvent;
	};
	HANDLE	aEvents[3];	
	};

	hEventRead	= NULL;
	hEventWrite	= NULL;
	hNullEvent	= NULL;

	UINT64	cbWritePosition	= 0;

	DWORD	cbReadRequest	= 0;
	DWORD	cbReadResult	= 0;
	DWORD	cbWriteRequest	= 0;
	DWORD	cbWriteResult	= 0;
	ULONG	nWaitObjects	= 0;

	DWORD	dwError			= NOERROR;
	DWORD	dwWaitResult	= NOERROR;

	// If caller provided a callback handler then verify that it's an IOmfooExtractCallback.
	IOmfooExtractCallback *pCallback = NULL;
	if (pUnknown)
	{
		if (IsBadUnknown(pUnknown))
		{
			return E_FAIL;
		}
		else
		{
			if (FAILED(hr = pUnknown->QueryInterface(IID_PPV_ARGS(&pCallback))))
			{
				return hr;
			}
		}
	}

	// A note about memory buffer sizes and files opened with FILE_FLAG_NO_BUFFERING:
	// On x86 (32-bit) systems the maximum buffer size for unbuffered write operations is just under 64MB.
	// On x64 (64-bit) systems the maximum buffer size for unbuffered write operations is just under 32MB.
	// More specifically ...
	// The maximum for x86 (32-bit) systems is 0x03FF8000. The formula is ((65535-28)/4)*4096 = 67076096 bytes.
	// The maximum for x64 (64-bit) systems is 0x01FF9000. The formula is ((65535-48)/8)*4096 = 33525760 bytes.
	// Anything greater than this will cause WriteFile() to fail with the return code ERROR_NO_SYSTEM_RESOURCES.
	// This is a known limitation of WriteFile() and all Windows operating systems.

	// If caller did NOT supply nPagesPerCallback then default to 7200 pages per bank. (29491200 bytes)
	if (nPagesPerCallback == 0)
	{
		cbMemoryBank = 0x01C20000;
	}
	else if (nPagesPerCallback <= 7200)
	{
		cbMemoryBank = nPagesPerCallback * 4096;
	}
	else
	{
		return E_INVALIDARG;
	}

	// If cbMemoryBank is larger than the size of the embedded file then use the lesser.
	if (cbRoundUpLength < cbMemoryBank)
	{
		cbMemoryBank = UINT32(cbRoundUpLength);
	}

	// Allocate memory for two adjacent equally-sized memory banks.
	PBYTE pMemBase = PBYTE(VirtualAlloc(NULL, cbMemoryBank*2, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
	if (pMemBase == NULL)
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// Save their pointers in our aMemBank[] array.
	aMemBank[0]	= pMemBase;
	aMemBank[1]	= &pMemBase[cbMemoryBank];

	// Create event handle for reading.
	if (NULL == (hEventRead = CreateEventW(LPSECURITY_ATTRIBUTES(NULL), FALSE, FALSE, LPCWSTR(0))))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// Create event handle for writing.
	if (NULL == (hEventWrite = CreateEventW(LPSECURITY_ATTRIBUTES(NULL), FALSE, FALSE, LPCWSTR(0))))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// Open a handle to our current OMF file for asynchronous reading.
	// We cannot use the one in CReadableFile::m_hFileRead because it was not opened with FILE_FLAG_OVERLAPPED.
	hFileRead = CreateFileW(m_pwzFullPath,			// file to open
							GENERIC_READ,			// open for reading (SYNCHRONIZE = ok)
							FILE_SHARE_READ,		// share for reading
							NULL,					// default security
							OPEN_EXISTING,			// existing file only
							FILE_FLAG_OVERLAPPED,	// for asynchronous I/O
							NULL);

	// If that failed for any reason ...
	if (IsBadHandle(hFileRead))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// Open the destination file for asynchronous writing.
	hFileWrite = CreateFileW(pwzDestFullPath,		// file to open/create
							GENERIC_WRITE,			// open/create for writing (SYNCHRONIZE = fail)
							0x00000000,				// FILE_SHARE_NONE (don't share)
							NULL,					// default security
							fOverwrite ? CREATE_ALWAYS : CREATE_NEW,
							FILE_FLAG_WRITE_THROUGH |\
							FILE_FLAG_OVERLAPPED |\
							FILE_FLAG_NO_BUFFERING |\
							FILE_ATTRIBUTE_NORMAL |\
							FILE_FLAG_SEQUENTIAL_SCAN,
							NULL);

	// If that failed for any reason ...
	if (IsBadHandle(hFileWrite))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// "It is not an error to set the file pointer to a position beyond the end of the file."
	if (!SetFilePointerEx(hFileWrite, *PLARGE_INTEGER(&cbRoundUpLength), NULL, FILE_BEGIN))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// "The size of the file does not increase until you call the SetEndOfFile()."
	if (!SetEndOfFile(hFileWrite))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// Now set the file pointer back to the start of the file.
	if (!SetFilePointerEx(hFileWrite, liPtrEx, NULL, FILE_BEGIN))
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_CleanupExit;
	}

	// Initialize our bank pointer to the first memory bank.
	pCurrentBank = aMemBank[0];

	// Main write/read loop.
	do {
		// Give pEvents something to point to.
		PHANDLE	pEvents = &hNullEvent;

		// Reset the object counter for WaitForMultipleObjects().
		nWaitObjects = 0;

		// Are there any pending blocks waiting to be written from a previous read operation?
		if (cbWriteRequest)
		{
			// >>>> Begin part 2 (of 2) of the IFF ckSize kludge. <<<<
			// If this is the very first write buffer then tweak the cbSize member of the file header.
			// We only do this once.
			if (cbPatchChunkSize)
			{
				if (cbWriteRequest >= 8)
				{
					PDWORD pdw = PDWORD(pCurrentBank);
					pdw[1] = cbPatchChunkSize;
				}

				// Now set cbPatchChunkSize to zero so that we never do this again!
				cbPatchChunkSize = 0;
			}
			// >>>> End of part 2 (of 2) of the IFF ckSize kludge. That's all. It is done! <<<<

			// Round cbWriteRequest up to the next page boundary if necessary.
			// This can only happen on the last iteration of this loop.
			// It will happen when the embedded file length is not evenly divisible by 4096.
			if (cbWriteRequest & 0x0FFF)
			{
				cbWriteRequest |= 0x0FFF;
				cbWriteRequest++;
			}

			// Initialize/re-initialize these every time.
			ovlWrite.Internal		= 0;
			ovlWrite.InternalHigh	= 0;
			ovlWrite.Offset			= PUINT32(&cbWritePosition)[0];
			ovlWrite.OffsetHigh		= PUINT32(&cbWritePosition)[1];
			ovlWrite.hEvent			= hEventWrite;

			// Initiate the next async write.
			if (!WriteFile(hFileWrite, pCurrentBank, cbWriteRequest, NULL, &ovlWrite))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_PENDING)
				{
					BREAK_IF_DEBUG
					hr = HRESULT_FROM_WIN32(dwError);
					goto L_CleanupExit;
				}
			}

			// Modify pEvents to point to the 2nd entry in m_aEvents[].
			pEvents	= &hEventWrite;

			// Increment the object counter for WaitForMultipleObjects().
			nWaitObjects++;
		}

		// Toggle our memory bank.
		pCurrentBank = aMemBank[(++nToggles) & 0x00000001];

		// Are there any bytes remaining to be read from the source file?
		if (cbReadRemaining)
		{
			// Decide on the number of bytes to read.
			if (cbReadRemaining >= cbMemoryBank)
			{
				cbReadRequest = cbMemoryBank;
			}
			else
			{
				cbReadRequest = UINT32(cbReadRemaining);
			}

			// Initialize/re-initialize these every time.
			ovlRead.Internal		= 0;
			ovlRead.InternalHigh	= 0;
			ovlRead.Offset			= PUINT32(&cbReadPosition)[0];
			ovlRead.OffsetHigh		= PUINT32(&cbReadPosition)[1];
			ovlRead.hEvent			= hEventRead;

			// Initiate the next async read.
			if (!ReadFile(hFileRead, pCurrentBank, cbReadRequest, LPDWORD(0), &ovlRead))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_PENDING)
				{
					BREAK_IF_DEBUG
					hr = HRESULT_FROM_WIN32(dwError);
					goto L_CleanupExit;
				}
			}

			// Modify pEvents to point to the 1st entry in m_aEvents[].
			pEvents	= &hEventRead;

			// Increment the object counter for WaitForMultipleObjects().
			nWaitObjects++;
		}

		// Are there any pending read or write operations?
		if (nWaitObjects)
		{
			// Did caller supply a callback?
			if (pCallback)
			{
				// Does caller want us to abort?
				if (FAILED(pCallback->UpdateProgress(cbWritePosition, rCE.cbPayloadLength)))
				{
					BREAK_IF_DEBUG
					hr = E_ABORT;
					goto L_CleanupExit;
				}
			}

			// Wait until the states of all objects have been signaled.
			// Wait time is measured in milliseconds.
			dwWaitResult = WaitForMultipleObjects(nWaitObjects, pEvents, TRUE, DWORD(30 * 1000));
			if (dwWaitResult > 1)
			{
				BREAK_IF_DEBUG
				hr = (dwWaitResult == WAIT_TIMEOUT) ? HRESULT_FROM_WIN32(WAIT_TIMEOUT) : E_FAIL;
				goto L_CleanupExit;
			}

			if (cbWriteRequest)
			{
				if (!GetOverlappedResult(hFileWrite, &ovlWrite, &cbWriteResult, TRUE))
				{
					BREAK_IF_DEBUG
					hr = HRESULT_FROM_WIN32(GetLastError());
					goto L_CleanupExit;
				}

				if (cbWriteResult != cbWriteRequest)
				{
					BREAK_IF_DEBUG
					hr = E_FAIL;
					goto L_CleanupExit;
				}

				cbWritePosition += cbWriteResult;
				cbWriteRequest	= 0;
				cbWriteResult	= 0;
			}

			if (cbReadRequest)
			{
				if (!GetOverlappedResult(hFileRead, &ovlRead, &cbReadResult, TRUE))
				{
					BREAK_IF_DEBUG
					hr = HRESULT_FROM_WIN32(GetLastError());
					goto L_CleanupExit;
				}

				if (cbReadResult != cbReadRequest)
				{
					BREAK_IF_DEBUG
					hr = E_FAIL;
					goto L_CleanupExit;
				}

				cbWriteRequest	= cbReadResult;	
				cbReadPosition	+= cbReadResult;
				cbReadRemaining	-= cbReadResult;
				cbReadRequest	= 0;
				cbReadResult	= 0;
			}
		}
	} while (nWaitObjects);

	// hFileWrite was opened with FILE_FLAG_NO_BUFFERING, and so we always wrote in multiples of 4096 bytes.
	// That means that our previous write operation probably wrote some extra junk bytes at the end. If so, we need
	// to close the file and re-open it without FILE_FLAG_NO_BUFFERING so we can truncate it to the proper length.
	if (WORD(rCE.cbPayloadLength) & 0x0FFF)
	{
		// Close the current write handle.
		if (!CloseHandle(hFileWrite))
		{
			BREAK_IF_DEBUG
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto L_CleanupExit;
		}

		// Reopen the destination file without the FILE_FLAG_NO_BUFFERING flag.
		hFileWrite = CreateFileW(pwzDestFullPath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (IsBadHandle(hFileWrite))
		{
			BREAK_IF_DEBUG
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto L_CleanupExit;
		}

		// Move the file pointer to the desired end position. - this is where we intend to chop-it-off.
		liPtrEx.QuadPart = rCE.cbPayloadLength;
		if (!SetFilePointerEx(hFileWrite, liPtrEx, NULL, FILE_BEGIN))
		{
			BREAK_IF_DEBUG
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto L_CleanupExit;
		}

		// Now call SetEndOfFile() to truncate it.
		if (!SetEndOfFile(hFileWrite))
		{
			BREAK_IF_DEBUG
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto L_CleanupExit;
		}

		// Fall through.
	}

	// Did caller supply a callback?
	if (pCallback)
	{
		// Does caller want us to cancel?
		// This is the last call to UpdateProgress() so cbComplete and cbTotal are the same.
		if (FAILED(pCallback->UpdateProgress(rCE.cbPayloadLength, rCE.cbPayloadLength)))
		{
			BREAK_IF_DEBUG
			hr = E_ABORT;
			goto L_CleanupExit;
		}
	}

	// If we made it to here we succeeded!
	hr = S_OK;

	// Now clean up.
L_CleanupExit:

	// Did caller abort?
	if (hr == E_ABORT)
	{
		BREAK_IF_DEBUG

		// Cancel all outstanding read/write operations.
		CancelIo(hFileRead);
		CancelIo(hFileWrite);

		// Close the destination file and then delete it.
		CloseHandle(hFileWrite);
		hFileWrite = INVALID_HANDLE_VALUE;
		if (!DeleteFileW(pwzDestFullPath))
		{
			BREAK_IF_DEBUG
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
	}

	// Close all open handles.
	if (IsValidHandle(hFileRead))
	{
		CloseHandle(hFileRead);
	}

	if (IsValidHandle(hFileWrite))
	{
		CloseHandle(hFileWrite);
	}

	if (IsValidHandle(hEventRead))
	{
		CloseHandle(hEventRead);
	}

	if (IsValidHandle(hEventWrite))
	{
		CloseHandle(hEventWrite);
	}

	// Free buffer memory.
	if (pMemBase)
	{
		VirtualFree(pMemBase, SIZE_T(0), MEM_RELEASE);
	}

	// Release pCallback if it exists, and set pCallback to NULL.
	IUnknown_AtomicRelease((PVOID*)&pCallback);

	// Done!
	return hr;
}

