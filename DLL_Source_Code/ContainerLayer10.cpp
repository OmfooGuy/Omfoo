// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer10.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer10.h"
#include "DllMain.h"
#include <shlwapi.h>

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer10::CContainerLayer10(void)
{
	// Temporary test to verify that sizeof(MDAT_CACHE_ENTRY)) is exactly 256 bytes.
#ifdef _DEBUG
	ULONG cbCacheEntry	= ULONG(sizeof(MDAT_CACHE_ENTRY));
	ULONG cbDiff		= 0x00000100 - cbCacheEntry;
	if (cbDiff != 0)
	{
		__debugbreak();
	}
#endif	// _DEBUG
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer10::~CContainerLayer10(void)
{
	MemFree(m_aMdatTable);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer10::Load(PCWSTR pwzFileName)
{
	HRESULT hr = __super::Load(pwzFileName);

	if (SUCCEEDED(hr))
	{
		// Get these property IDs right here right now.
		m_dwPropMdflIsOMFI = OrdinalToPropertyID(ePropMdflIsOMFI);	// OMFI:MDFL:IsOMFI
		m_dwPropMdflLength = OrdinalToPropertyID(ePropMdflLength);	// OMFI:MDFL:Length

		CHECK(CreateMdatCacheTable());
		CHECK(TagDuplicateMdatIDs());
		CHECK(IngestSmobBlops());
		CHECK(IngestMdesBlops());
		CHECK(CollectDataValues());
		CHECK(IngestLinkedDidds());
		CHECK(IngestSmobNames());
		CHECK(CollectSampleRates());
	//	CHECK(CollectCompressionInfo());
		CHECK(CollectCompressionFourCCs());
		CHECK(CollectCompressionStrings());
		CHECK(SolveCompressionAmbiguity());
	//	CHECK(DetectOverlappers());
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	If the HEAD has a an array of MDATs then create a corresponding array of MDAT_CACHE_ENTRY structures.
//*********************************************************************************************************************
HRESULT CContainerLayer10::CreateMdatCacheTable(void)
{
	// There are subtle differences between OMF1 and OMF2.
	// But when these routines are done thes results are the same.
	// On exit the oMDAT and mobID members for each MDAT_CACHE_ENTRY in m_aMdatTable[] will be valid.
	if (m_fOmfVer1)
	{
		return CreateOmf1MdatCacheTable();
	}
	else
	{
		return CreateOmf2MdatCacheTable();
	}
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from CreateMdatCacheTable().
//	Come here when the file is OMF1.
//	If the HEAD has a OMFI:MediaData array then create a corresponding array of MDAT_CACHE_ENTRY structures.
//	The array type for OMF1 is omfi:MobIndex, which is an array of OMF1_MOB_INDEX_ENTRY structures.
//	On exit the oMDAT and mobID members for each MDAT_CACHE_ENTRY in m_aMdatTable[] will be valid.
//*********************************************************************************************************************
HRESULT CContainerLayer10::CreateOmf1MdatCacheTable(void)
{
	// Assume we will succeed.
	HRESULT hr = S_OK;

	// We allocated and populated the OMFI:MediaData array in CContainerLayerNN::AllocHeadArrays().
	if (m_pV1MediaData == NULL)
	{
		// This property is optional.
		// If it doesn't exist then it's is not a fatal error.
		// It just means that this omf file doesn't contain any embedded media.
		goto L_Exit;
	}

	// How many elements in the array?
	m_cMDATs = m_pV1MediaData->nElements;

	// We don't need to allocate our m_aMdatTable if m_pV1MediaData has no elements.
	// I've never seen an omf file with an empty OMFI:MediaData array (where the array exists without any members).
	// But just in case this file has none we exit (and succeed).
	if (m_cMDATs == 0)
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// m_cMDATs can never be greater than 65535 (0x0000FFFF) because the array's element count is a USHORT.
	// And we can only accommodate up to 65534 elements because of the way we internally store the 16-bit indices.
	// (We store the indices in a one-based format, so we can reserve zero as a special case.) 
	// It is _extremely_ unlikely that any OMF file will ever have exactly 65535 MDATs (the maximum allowable).
	// But just in case it ever happens we treat it as a fatal error (even though it's actually our own shortcoming).
	if (m_cMDATs > 65534)
	{
		BREAK_IF_DEBUG
		hr = OMF_E_MDAT_SURPRISE;
		goto L_Exit;
	}

	// Allocate memory for an array of MDAT_CACHE_ENTRY structures.
	m_aMdatTable = MDAT_CACHE_TABLE(MemAlloc(m_cMDATs * sizeof(MDAT_CACHE_ENTRY)));
	if (m_aMdatTable == NULL)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_Exit;
	}

	// Sort the objects in the OMFI:MediaData array according to their Bento object ID.
	// Nevermind. We don't need to do this.
	// But if we ever do then here's the code.
	//if (m_cMDATs > 1)
	//{
	//	BOOL fStillSwapping = FALSE;
	//	do {
	//		fStillSwapping = FALSE;
	//		POMF1_MOB_INDEX_ENTRY pCur = m_pV1MediaData->a;
	//		POMF1_MOB_INDEX_ENTRY pEnd = &pCur[m_cMDATs-1];
	//		do {
	//			POMF1_MOB_INDEX_ENTRY pNxt = &pCur[1];
	//			if (pCur->dwObject > pNxt->dwObject)
	//			{
	//				// Q: Does this ever happen?
	//				// A: Afaik, no! Not with any of my test files.
	//				BREAK_IF_DEBUG
	//				OMF1_MOB_INDEX_ENTRY tmp = *pCur;
	//				*pCur = *pNxt;
	//				*pNxt = tmp;
	//				fStillSwapping = TRUE;
	//			}
	//		} while (++pCur < pEnd);
	//	} while (fStillSwapping);
	//}

	// For each entry ...
	for (ULONG i = 0; i < m_cMDATs; i++)
	{
		// Search CReadOmf::m_aBlopTable[] for the BENTO_BLOP structure that represents m_pV1MediaData->a[i].dwObject.
		BENTO_BLOP& rMDAT = GetBlop(m_pV1MediaData->a[i].dwObject);

		// If GetBlop() failed then the BENTO_BLOP.dwObject member will be zero.
		if (rMDAT.dwObject == 0)
		{
			BREAK_IF_DEBUG
			hr = OMF_E_MDAT_SURPRISE;
			goto L_Exit;
		}

		// Save our local zero-based index in the BENTO_BLOP structure's wMdatIdx member as a one-based index.
		// Note that we are saving this in the BENTO_BLOP that lives in CReadOmf::m_aBlopTable[].
		rMDAT.wMdatIdx = WORD(i+1);

		// Now copy that BENTO_BLOP structure (in its entirety, including wMdatIdx) over to the oMDAT member
		// of our current MDAT_CACHE_ENTRY structure.
		m_aMdatTable[i].oMDAT = rMDAT;

		// Retrieve the MDAT's UID from the HEAD's OMFI:MediaData/omfi:MobIndex array.
		// Do NOT try to retrieve it from its OMFI:xxxx:MobID property because -
		// 1) Some OMF1 MDATs lack a "OMFI:xxxx:MobID" property, and ...
		// 2) Even if they have a "OMFI:xxxx:MobID" property, we can't be sure of the property's name.
		//	  Here are all of the property names that I'm aware of, but there could be more.
		//		OMFI:AIFC:MobID
		//		OMFI:IDAT:MobID
		//		OMFI:JPEG:MobID
		//		OMFI:MDAT:MobID
		//		OMFI:SD2M:MobID
		//		OMFI:TIFF:MobID
		//		OMFI:WAVE:MobID
		m_aMdatTable[i].mobID = m_pV1MediaData->a[i].mobID;
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from CreateMdatCacheTable().
//	Come here when the file is OMF2.
//	If the HEAD has a OMFI:HEAD:MediaData array then create a corresponding array of MDAT_CACHE_ENTRY structures.
//	The array type for OMF2 is omfi:ObjRefArray, which is an array of OMF2_OBJREF_ENTRY structures.
//	On exit the oMDAT and mobID members for each MDAT_CACHE_ENTRY in m_aMdatTable[] will be valid.
//*********************************************************************************************************************
HRESULT CContainerLayer10::CreateOmf2MdatCacheTable(void)
{
	// Assume we will succeed.
	HRESULT hr = S_OK;

	// We allocated and populated the OMFI:HEAD:MediaData array in CContainerLayerNN::AllocHeadArrays().
	if (m_pV2MediaData == NULL)
	{
		// This property is optional.
		// If it doesn't exist then it's is not a fatal error.
		// It just means that this omf file doesn't have any embedded media.
		goto L_Exit;
	}

	// How many elements in the array?
	m_cMDATs = m_pV2MediaData->nElements;

	// We don't need to allocate our m_aMdatTable if m_pV2MediaData has no elements.
	// I've never seen an omf file with an empty OMFI:HEAD:MediaData array (where the array exists without any members).
	// But just in case this file has none we exit (and succeed).
	if (m_cMDATs == 0)
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// m_cMDATs can never be greater than 65535 (0x0000FFFF) because the array's element count is a USHORT.
	// And we can only accommodate up to 65534 elements because of the way we internally store the 16-bit indices.
	// (We store the indices in a one-based format, so we can use zero as a special case.)
	// It is _extremely_ unlikely that any OMF file will ever have exactly 65535 MDATs (the maximum allowable).
	// But just in case it ever happens we treat it as a fatal error (even though it's actually our own shortcoming).
	if (m_cMDATs > 65534)
	{
		BREAK_IF_DEBUG
		hr = OMF_E_MDAT_SURPRISE;
		goto L_Exit;
	}

	// Allocate memory for an array of MDAT_CACHE_ENTRY structures.
	m_aMdatTable = MDAT_CACHE_TABLE(MemAlloc(m_cMDATs * sizeof(MDAT_CACHE_ENTRY)));
	if (m_aMdatTable == NULL)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_Exit;
	}

	// Sort the objects in the OMFI:HEAD:MediaData array according to their Bento object ID.
	// Nevermind. We don't need to do this.
	// But if we ever do then here's the code.
	//if (m_cMDATs > 1)
	//{
	//	BOOL fStillSwapping = FALSE;
	//	do {
	//		fStillSwapping = FALSE;
	//		PDWORD pCur = &m_pV2MediaData->a[0];
	//		PDWORD pEnd = &pCur[m_cMDATs-1];
	//		do {
	//			DWORD dw = pCur[0];
	//			if (dw > pCur[1])
	//			{
	//				// Q: Does this ever happen?
	//				// A: Afaik, no! Not with any of my test files.
	//				BREAK_IF_DEBUG
	//				*pCur = pCur[1];
	//				pCur[1] = dw;
	//				fStillSwapping = TRUE;
	//			}
	//		} while (++pCur < pEnd);
	//	} while (fStillSwapping);
	//}

	// In OMF2 the MDAT's mobID properties are always named OMFI:MDAT:MobID.
	DWORD dwPropMdatMobID = OrdinalToPropertyID(ePropMdatMobID);
	if (dwPropMdatMobID == 0)
	{
		// Oops! We have an OMFI:HEAD:MediaData array but the OMFI:MDAT:MobID property is not defined!
		BREAK_IF_DEBUG
		hr = OMF_E_MDAT_SURPRISE;
		goto L_Exit;
	}

	// For each entry ...
	for (ULONG i = 0; i < m_cMDATs; i++)
	{
		// Search CReadOmf::m_aBlopTable[] for the BENTO_BLOP structure that represents m_pV2MediaData->a[i].
		BENTO_BLOP& rMDAT = GetBlop(m_pV2MediaData->a[i]);

		// If GetBlop() failed then the BENTO_BLOP.dwObject member will be zero.
		if (rMDAT.dwObject == 0)
		{
			BREAK_IF_DEBUG
			hr = OMF_E_MDAT_SURPRISE;
			goto L_Exit;
		}

		// Save our local zero-based index in the BENTO_BLOP structure's wMdatIdx member as a one-based index.
		// Note that we are saving this in the BENTO_BLOP that lives in CReadOmf::m_aBlopTable[].
		rMDAT.wMdatIdx = WORD(i+1);

		// Now copy that BENTO_BLOP structure (in its entirety, including wMdatIdx) over to the MDAT member
		// of our current MDAT_CACHE_ENTRY structure.
		m_aMdatTable[i].oMDAT = rMDAT;

		// Retrieve the MDAT's UID from its OMFI:MDAT:MobID property.
		if (FAILED(hr = CoreReadMobID(m_aMdatTable[i].oMDAT, dwPropMdatMobID, &m_aMdatTable[i].mobID)))
		{
			BREAK_IF_DEBUG
			hr = OMF_E_MDAT_SURPRISE;
			goto L_Exit;
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Come here after calling CreateMdatCacheTable().
//
//	The digital media data associated with a Source Mob is immutable, it cannot be updated or modified.
//	If it is necessary to change the data, an application must delete the Mob ID and create a new Source Mob.
//	(See the OMF Specification Version 2.1, Chapter 2 at page 25, under the subheading "Creating a File Source Mob".)
//
//	Nevertheless sometimes two or more MDATs will share a common SMOB.
//	In other words, two or more MDATs will have identical OMFI:MDAT:MobID properties.
//	When this happens the SMOB's OMFI:SMOB:MediaDescription property is always some type of DIDD (which inherits MDFL).
//	It holds a property named OMFI:DIDD:NextDIDDescriptor, which is an omfi:ObjRef to a previous generation's MDES.
//	In other words the OMFI:DIDD:NextDIDDescriptor property forms a one-way linked list back to the previous MDES.
//	I've only seen this with JPEDs, RGBAs, RLEDs, but it may exist in other classes too.
//	I've seen it two generations deep (JPED >> RGBA) and three generations deep (JPED >> RGBA >> RLED).
//
//	So ... this routine is the first step in dealing with this undocumented curve-ball.
//
//	It scans m_aMdatTable[] for MDAT_CACHE_ENTRY.mobIDs with duplicate values.
//	If/when it finds them it sets the fDupMdatUID member of _both_ offending MDAT_CACHE_ENTRY structures to TRUE.
//	This implementation always succeeds.
//	On exit m_cMdatDupUIDs holds the total number of MDATs with duplicate MobIDs (incluing the 'original').
//*********************************************************************************************************************
HRESULT CContainerLayer10::TagDuplicateMdatIDs(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pOuter	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEnd		= &m_aMdatTable[m_cMDATs];
		do {
			PMDAT_CACHE_ENTRY pInner = m_aMdatTable;
			do {
				if (pInner != pOuter)
				{
					if ((pOuter->mobID.dwMinor == pInner->mobID.dwMinor)&&
						(pOuter->mobID.dwMajor == pInner->mobID.dwMajor)&&
						(pOuter->mobID.dwPrefix == pInner->mobID.dwPrefix))
					{
						// If UIDs match then set both fags in both MDAT_CACHE_ENTRY structures to TRUE.
						pOuter->fDupMdatUID = TRUE;
						pInner->fDupMdatUID = TRUE;
					}
				}
			} while (++pInner < pEnd);
		} while (++pOuter < pEnd);

		// Now calculate the total number of duplicate UIDs in the whole file (including the 'originals').
		pOuter = m_aMdatTable;
		do {
			m_cMdatDupUIDs += pOuter->fDupMdatUID;
		} while (++pOuter < pEnd);
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Initialize the BENTO_BLOP for each SMOB in our m_aMdatTable[].
//	Come here after calling CreateMdatCacheTable() and TagDuplicateMdatIDs().
//	Find the SMOB associated with each MDAT.
//	When we find it copy its BENTO_BLOP structure from CReadOmf::m_aBlopTable[] into MDAT_CACHE_ENTRY.oSMOB.
//*********************************************************************************************************************
HRESULT CContainerLayer10::IngestSmobBlops(void)
{
	// Assume we will succeed.
	HRESULT	hr = S_OK;
	ULONG	cHits = 0;
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		do {
			PBENTO_BLOP	pCurBlop	= m_aBlopTable;
			PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];
			do {
				if (pCurBlop->dwFourCC == FCC('SMOB'))
				{
					PTOCX_ITEM pCurItem	= &m_aToc[pCurBlop->iFirstItem];
					PTOCX_ITEM pEndItem	= &pCurItem[pCurBlop->wTotalItems];
					do {
						if ((pCurItem->dwProperty	== m_dwPropMobjMobID)&&
							(pCurItem->dwDataType	== m_dwTypeUID)&&
							(pCurItem->bStorageMode	== SM_CACHED))
						{
							if ((pCurItem->aCachedDwords[2] == pCurMdat->mobID.dwMinor)&&
								(pCurItem->aCachedDwords[1] == pCurMdat->mobID.dwMajor)&&
								(pCurItem->aCachedDwords[0] == pCurMdat->mobID.dwPrefix))
							{
								// Copy the BENTO_BLOP structure in its entirety over to the oSMOB member
								// of our current MDAT_CACHE_ENTRY structure.
								pCurMdat->oSMOB = *pCurBlop;
								cHits++;
							}
							break;
						}
					} while (++pCurItem < pEndItem);
				}
			} while (++pCurBlop < pEndBlop);
		} while (++pCurMdat < pEndMdat);
	}

	if (cHits != m_cMDATs)
	{
		BREAK_IF_DEBUG
		// Couldn't ingest SMOBs.
		// We can't find an SMOB associated with an MDAT.
		// Q: Does this ever happen with any of my test files?
		// A: Nope.

		// Q: Should we treat this as a fatal error?
		// A: Probably not, but how do we handle it?
		// For now treat this as an irrecoverable/fatal error.
		hr = OMF_E_MDAT_SURPRISE;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Initialize the BENTO_BLOP for each MDES in our m_aMdatTable[].
//	Note that we re-visit this task in IngestLinkedDidds().
//*********************************************************************************************************************
HRESULT CContainerLayer10::IngestMdesBlops(void)
{
	if (m_aMdatTable)
	{
		for (UINT i = 0; i < m_cMDATs; i++)
		{
			// Find the MDES for this MDAT.
			DWORD	dwMDES		= 0;
			if (SUCCEEDED(CoreReadObjRef(m_aMdatTable[i].oSMOB, m_dwPropMediaDesc, NULL, &dwMDES)))
			{
				m_aMdatTable[i].oMDES = GetBlop(dwMDES);

				// The OMFI:MDFL:IsOMFI property is supposed to be 'required'.
				// Nevertheless some AIFD/AIFC combos don't have it.
				// Sometimes the OMFI:MDFL:IsOMFI property isn't even defined.
				// So for us this isn't a show-stopper.
				CoreReadBoolean(m_aMdatTable[i].oMDES, m_dwPropMdflIsOMFI, &m_aMdatTable[i].fIsOMFI);

				// The OMFI:MDFL:Length property is also required, but only if the MDES is a MDFL.
				if (FAILED(CoreReadLength64(m_aMdatTable[i].oMDES, m_dwPropMdflLength, &m_aMdatTable[i].qwDuration)))
				{
					BREAK_IF_DEBUG
				}
			}
			else
			{
				// WTF?
				// OMF1 - The MOBJ associated with this MDAT doesn't have a OMFI:MOBJ:PhysicalMedia property.
				// OMF2 - The SMOB associated with this MDAT doesn't have a OMFI:SMOB:MediaDescription property.
				// Therefore m_aMdatTable[i].oMDES is copy of CReadOmf::m_oEmptyBlop, which is all zeroes.
				// Does this ever happen?
				BREAK_IF_DEBUG
			}
		}
	}
	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//
//	OMF1 --------
//	This loop caches the offsets and lengths of the payload for each MDAT in the Header's OMFI:MediaData array.
//	These offsets and lengths live in a property having the omfi:DataValue or omfi:VarLenBytes data type.
//	The storage mode in the Bento TOC is always SM_OFFSET.
//	The names of these properties vary from class to class.
//	Here are some examples ...
//		OMFI:AIFC:Data
//		OMFI:SD2M:Data
//		OMFI:TIFF:Data
//		OMFI:WAVE:Data
//
//	OMF2 --------
//	This loop caches the offsets and lengths of the payload for each MDAT in the Header's OMFI:HEAD:MediaData array.
//	These offsets and lengths live in a property having the omfi:DataValue data type.
//	The storage mode in the Bento TOC is always SM_OFFSET.
//	The names of these properties vary from class to class.
//	Here are some examples ...
//		OMFI:AIFC:AudioData
//		OMFI:IDAT:ImageData
//		OMFI:SD2M:AudioData
//		OMFI:TIFF:ImageData
//		OMFI:WAVE:AudioData
//
//	WARNING: This routine collects the first omfi:DataValue (or omfi:VarLenBytes) property it encounters.
//	If any 3rd-party MDAT class has more than one property that uses the omfi:DataValue or omfi:VarLenBytes data type,
//	then this routine may 'collect' the wrong one.
//
//	On exit m_cbMaxMdatPayload holds the size of the largest payload rounded up to the next 4096-byte boundary.
//*********************************************************************************************************************
HRESULT CContainerLayer10::CollectDataValues(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		do {
			PTOCX_ITEM pCurItem = &m_aToc[pCurMdat->oMDAT.iFirstItem];
			PTOCX_ITEM pEndItem = &pCurItem[pCurMdat->oMDAT.wTotalItems];
			do {
				// Note that m_dwTypeDataValue and m_dwTypeVarLenBytes are always non-zero.
				if ((pCurItem->dwDataType == m_dwTypeDataValue)||
					(pCurItem->dwDataType == m_dwTypeVarLenBytes))
				{
					if (pCurItem->bStorageMode == SM_OFFSET)
					{
						// Get the offsets and lengths, and save them here.
						pCurMdat->cbPayloadOffset	= pCurItem->cbOffset64;
						pCurMdat->cbPayloadLength	= pCurItem->cbLength64;

						// Save the property ID too.
						pCurMdat->dwPropMediaData	= pCurItem->dwProperty;
						break;
					}
				}
			} while (++pCurItem < pEndItem);
		} while (++pCurMdat < pEndMdat);

		// Now find the size of the largest MDAT payload and save it in m_cbMaxMdatPayload.
		pCurMdat = m_aMdatTable;
		UINT64 cbLargestMdatPayload = 0;
		do {
			if (pCurMdat->cbPayloadLength > cbLargestMdatPayload)
			{
				cbLargestMdatPayload = pCurMdat->cbPayloadLength;
			}
		} while (++pCurMdat < pEndMdat);

		// Round it up to the next 4096-byte boundary, and then save it.
		if (cbLargestMdatPayload & 0x0FFF)
		{
			m_cbMaxMdatPayload = (cbLargestMdatPayload | 0x0FFF) + 1;
		}
		else
		{
			m_cbMaxMdatPayload = cbLargestMdatPayload;
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Must call TagDuplicateMdatIDs(), IngestSmobBlops(), IngestMdesBlops(), and CollectDataValues() before this.
//*********************************************************************************************************************
HRESULT CContainerLayer10::IngestLinkedDidds(void)
{
	HRESULT hr = S_OK;

	if (m_aMdatTable && m_cMdatDupUIDs)
	{
		ULONG	cHits = 0;

		// The OMFI:DIDD:NextDIDDescriptor/OMFI:DIDD:NextDIDDesc property must be defined.
		// We don't actually use them, but it serves as an indicator that the file may contain be duplicate UIDs.
		if (m_dwPropNextDIDDesc && m_dwPropDataOffset)
		{
			PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
			PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
			do {
				if (pCurMdat->fDupMdatUID)
				{
					PTOCX_ITEM pCurItem = m_aToc;
					PTOCX_ITEM pEndItem = &m_aToc[m_nTocItems];
					do {
						if ((pCurItem->dwProperty == m_dwPropDataOffset) &&
							(pCurItem->bStorageMode == SM_IMMEDIATE) &&
							(pCurItem->cbLength64 == sizeof(UINT32)))
						{
							UINT32 cbDataOffset = pCurItem->dwImmediateDword;
							if (m_fOmfBigEndian) cbDataOffset = Endian32(cbDataOffset);
							if (cbDataOffset == pCurMdat->cbPayloadOffset)
							{
								// Find the BENTO_BLOP structure for the new object.
								// The will clobber the existing structure, and that's okay. That's what we want. 
								pCurMdat->oMDES = GetBlop(pCurItem->dwObject);

								// The OMFI:MDFL:IsOMFI property is supposed to be 'required'.
								// But for us this isn't a show-stopper.
								CoreReadBoolean(pCurMdat->oMDES, m_dwPropMdflIsOMFI, &pCurMdat->fIsOMFI);

								// Increment our counter that counts the number of ...
								cHits++;

								// Break out of inner do/while loop, because we've found the correct MDES
								// that matches this MDAT.
								break;
							}
						}
					} while (++pCurItem < pEndItem);
				}
			} while (++pCurMdat < pEndMdat);

			if (m_cMdatDupUIDs != cHits)
			{
				// Oops!
				// This file defines OMFI:DIDD:DataOffset (or OMFI:MDFL:dataOffset) but ...
				BREAK_IF_DEBUG
			}
		}
		else
		{
			// Oops! Two or more MDATs have the same globally unique ID. (OMFI:MDAT:MobID)
			// We can't figure out which MDES describes which MDAT because their SMOB/UID is the same.
			// And the linked MDES thing with OMFI:DIDD:NextDIDDescriptor isn't the cause!
			// Fatal error!
			BREAK_IF_DEBUG
			hr = OMF_E_MDAT_IDENTITY_CRISIS;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//*********************************************************************************************************************
HRESULT CContainerLayer10::IngestSmobNames(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		do
		{
			LPSTR pSmobName = pCurMdat->szSmobName;
			if (SUCCEEDED(CoreReadStringA(pCurMdat->oSMOB, m_dwPropMobjName,
											ELEMS(pCurMdat->szSmobName), pSmobName, NULL)))
			{
				// Test for the ".PHYS" suffix.
				if (0 == lstrcmpA(PathFindExtensionA(pSmobName), ".PHYS"))
				{
					// If it has one then chop it off!
					PathRemoveExtensionA(pSmobName);

					// Set a flag to remember that it had one.
					pCurMdat->fMobNameSuffixPHYS = TRUE;
				}

				// Remove the filename extension and save it in szSmobNameExt.
				LPSTR pszExt = PathFindExtensionA(pSmobName);
				ULONG cchExt = lstrlenA(pszExt);

				// Allow for filename extensions that are up to four characters long.
				// The count in cchExt includes the leading dot.
				if ((cchExt > 0) && (cchExt < ELEMS(pCurMdat->szSmobNameExt)))
				{
					// If the string consists entirely of ASCII numbers '0' thru '9' then
					// it is not a "filename extension" as far as we are concerned.
					if (!IsStringNumericA(&pszExt[1], cchExt))
					{
						// Copy the extension along with the dot.
						lstrcpyA(pCurMdat->szSmobNameExt, pszExt);

						// Force the extension to lowercase.
						CharLowerA(pCurMdat->szSmobNameExt);

						// Remove the extension from szSmobName.
						PathRemoveExtensionA(pSmobName);

						// Increment our counter.
						m_cSmobsWithExtensions++;
					}
				}

				// See if the string can be used as a valid Win32 filename (without alteration).
				BOOLEAN fHasName = IsValidFilenameA(pSmobName, 96);

				// Save result in the current MDAT_CACHE_ENTRY structure.
				pCurMdat->fMobNameCharsOk = fHasName;

				// For now we will assume that the string is unique.
				// We will come back to this in another loop and clear it if it turns out to be nonunique.
				pCurMdat->fMobNameUnique = fHasName;

				// Conditionally increment the counter that counts the number of SMOBs that have
				// names that can serve as part of a filename.
				m_cSmobsWithNames += fHasName;
			}
		} while (++pCurMdat < pEndMdat);

		// This loop conditionally clears the fMobNameUnique flags that we just set in the previous loop.
		pCurMdat = m_aMdatTable;
		do {
			PMDAT_CACHE_ENTRY pOther = m_aMdatTable;
			do {
				if (pOther != pCurMdat)
				{
					// Compare strings. The comparison is NOT case-sensitive.
					if (0 == lstrcmpiA(pOther->szSmobName, pCurMdat->szSmobName))
					{
						// If strings match then clear the 'unique' flags that we just set in the previous loop.
						// Do this for both MDAT_CACHE_ENTRY structures.
						pOther->fMobNameUnique		= FALSE;
						pCurMdat->fMobNameUnique	= FALSE;
					}
				}
			} while (++pOther < pEndMdat);
		} while (++pCurMdat < pEndMdat);

		// This loop calculates m_cSmobsWithUniqueNames.
		pCurMdat = m_aMdatTable;
		do {
			m_cSmobsWithUniqueNames += pCurMdat->fMobNameUnique;
		} while (++pCurMdat < pEndMdat);
	}

	// Always succeed.
	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	1) Collect the OMFI:MDFL:SampleRate property for each MDAT_CACHE_ENTRY in m_aMdatTable[].
//	2) Save the result in fSampleRate.
//	3) Then use fSampleRate to determine if the content is audio, video, or other.
//	4) Save result in bCategory as MDAT_CATEGORY_AUDIO_FILE, MDAT_CATEGORY_VIDEO_FILE, or MDAT_CATEGORY_OTHER_FILE.
//	Note that we will re-visit (and possibly correct/update) bCategory in IdentifyIDAT().
//*********************************************************************************************************************
HRESULT CContainerLayer10::CollectSampleRates(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];

		// OMFI:MDFL:SampleRate
		// omfi:Rational
		DWORD dwPropMdflSampleRate = OrdinalToPropertyID(ePropMdflSampleRate);
		if (dwPropMdflSampleRate)
		{
			do {
				FLOAT fSampleRate = 0.0;
				if (SUCCEEDED(CoreReadRationalAsFloat(pCurMdat->oMDES, dwPropMdflSampleRate, &fSampleRate)))
				{
					pCurMdat->fSampleRate = fSampleRate;

					// Is this an audio sample rate?
					if ((fSampleRate >= 8000) && (fSampleRate <= 384000))	// 8kHz~384kHz
					{
						pCurMdat->bCategory = MDAT_CATEGORY_AUDIO_FILE;
					}
					// Is this a video frame rate?
					else if ((fSampleRate >= 15) && (fSampleRate <= 240))	// 15fps~240fps
					{
						pCurMdat->bCategory = MDAT_CATEGORY_VIDEO_FILE;
					}
					else
					{
						pCurMdat->bCategory = MDAT_CATEGORY_OTHER_FILE;
					}
				}
				else
				{
					// Note that a still image is sometimes stored as one video frame.
					// And it may have a frame rate of 1.0 or even no frame rate at all.
					// That is why we re-visit bCategory in IdentifyIDAT().
					pCurMdat->bCategory = MDAT_CATEGORY_OTHER_FILE;
				}
			} while (++pCurMdat < pEndMdat);
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//*********************************************************************************************************************
/*
HRESULT CContainerLayer10::CollectCompressionInfo(void)
{
	if (m_aMdatTable)
	{
		// OMFI:DIDD:Compression, omfi:String
		DWORD dwPropCompressString = OrdinalToPropertyID(ePropDiddCompression);

		// OMFI:DIDD:DIDCompressMethod, omfi:ObjectTag
		DWORD dwPropCompressFourCC = OrdinalToPropertyID(ePropDiddDIDCompressMethod);

		// If the OMFI:DIDD:Compression property is defined but the OMFI:DIDD:DIDCompressMethod property is not ... 
		if ((dwPropCompressString != 0)&&(dwPropCompressFourCC == 0))
		{
			PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
			PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
			do {
				CHAR szCompression[16] = {0};
				if (SUCCEEDED(CoreReadStringA(pCurMdat->oMDES,
											dwPropCompressString,
											ELEMS(szCompression),
											szCompression, NULL))
				{
					// If OMFI:DIDD:Compression is not an empty string. 
					if (szCompression[0])
					{
						// If OMFI:DIDD:Compression exists and is exactly four characters long then it's a fourCC.
						// This is the most common and expected situation.
						// So just copy the first four characters of the string.
						if (lstrlenA(szCompression) == 4)
						{
							pCurMdat->dwNonambiguousCompressionFourCC = *PDWORD(pCurMdat->szCompression);
						}
						else
						{
							// If OMFI:DIDD:Compression exists but is not four characters long then
							// test for this specific string.
							if (0 == lstrcmpA(pCurMdat->szCompression, "1095389255"))
							{
								pCurMdat->dwNonambiguousCompressionFourCC = FCC('AJPG');
							}
						}
					}
				}
			} while (++pCurMdat < pEndMdat);
			goto L_Exit;
		}



		if ((dwPropCompressFourCC)&&(!dwPropCompressString))
		{
			PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
			PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
			do {
				CoreReadFourCC(pCurMdat->oMDES, dwPropCompressFourCC, &pCurMdat->dwNonambiguousCompressionFourCC);
			} while (++pCurMdat < pEndMdat);
			goto L_Exit;
		}






//
		//if (dwPropCompressMethod || dwPropCompressString)
		//{
		//	PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		//	PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		//	do {
		//		DWORD dwCompressFourCC = ;

		//		CoreReadFourCC(pCurMdat->oMDES, dwPropCompressMethod, &pCurMdat->dwCompressFourCC);
		//	} while (++pCurMdat < pEndMdat);
		//}

	}
L_Exit:
	return S_OK;
}
*/

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Collect the OMFI:DIDD:DIDCompressMethod property for each MDAT_CACHE_ENTRY in m_aMdatTable[].
//	This property is optional. It can be found in OMF1 and OMF2 files.
//	It is not one of the standard properties defined in the OMF v2.1 specification.
//	Note that the OMFI:DIDD:DIDCompressMethod and OMFI:DIDD:Compression properties appear to be redundant.
//*********************************************************************************************************************
HRESULT CContainerLayer10::CollectCompressionFourCCs(void)
{
	if (m_aMdatTable)
	{
		// OMFI:DIDD:DIDCompressMethod
		// omfi:ObjectTag
		DWORD dwPropCompressMethod = OrdinalToPropertyID(ePropDiddDIDCompressMethod);
		if (dwPropCompressMethod)
		{
			PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
			PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
			do {
				CoreReadFourCC(pCurMdat->oMDES, dwPropCompressMethod, &pCurMdat->dwCompressFourCC);
			} while (++pCurMdat < pEndMdat);
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Collect the OMFI:DIDD:Compression property for each MDAT_CACHE_ENTRY in m_aMdatTable[].
//	This property is optional; if there is no compression, the property is omitted.
//	Note that the OMFI:DIDD:DIDCompressMethod and OMFI:DIDD:Compression properties appear to be redundant.
//	One is a fourCC and the other is a string.
//*********************************************************************************************************************
HRESULT CContainerLayer10::CollectCompressionStrings(void)
{
	if (m_aMdatTable)
	{
		// OMFI:DIDD:Compression
		// omfi:String
		DWORD dwPropCompression = OrdinalToPropertyID(ePropDiddCompression);
		if (dwPropCompression)
		{
			PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
			PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
			do {
				// This 'string' is usually a fourCC. But not always!
				// Afaik the largest string we expect to find is "1095389255" which is 10 chars plus a terminator.
				// If the string is longer than MDAT_CACHE_ENTRY.szCompression[] then CoreReadStringA() will
				// truncate it and return OMF_E_INSUFFICIENT_BUFFER.
				CoreReadStringA(pCurMdat->oMDES,
								dwPropCompression,
								ELEMS(pCurMdat->szCompression),
								pCurMdat->szCompression, NULL);
			} while (++pCurMdat < pEndMdat);
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	We must call CollectCompressionFourCCs() and CollectCompressionStrings() before comming here.
//
//	The OMFI:DIDD:DIDCompressMethod and OMFI:DIDD:Compression properties are redundant. They serve the same purpose.
//	The former is a omfi:ObjectTag and the later is omfi:String, but even tho the later is a string it almost always
//	holds a simple fourCC. When these two properties are present they should always agree.
//	The only exception is "1095389255" which is the equivalent of the fourCC 'AJPG'.
//	See codecGetSelectInfoJPEG() in the file "omcAvJPED.c" in Avid's reference code.
//
//	This redundancy raises the question, "What do we do if these properties ever disagree?"
//	And that is what this method is all about. It compares both properties to make sure they agree, and saves
//	the 'approved' fourCC in the dwNonambiguousCompressionFourCC member of each MDAT_CACHE_ENTRY structure in m_aMdatTable[].
//	If they disagree [and I've never seen this in any of my test files] we put (-1) in dwNonambiguousCompressionFourCC.
//*********************************************************************************************************************
HRESULT CContainerLayer10::SolveCompressionAmbiguity(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		do {
			// If OMFI:DIDD:Compression doesn't exist then just copy the fourCC in dwCompressFourCC to dwNonambiguousCompressionFourCC.
			// If OMFI:DIDD:DIDCompressMethod doesn't exist then we are copying a zero. And that's okay.
			if (pCurMdat->szCompression[0] == 0)
			{
				pCurMdat->dwNonambiguousCompressionFourCC = pCurMdat->dwCompressFourCC;
				continue;
			}

			// If OMFI:DIDD:Compression exists and is exactly four characters long then it's a fourCC.
			// This is the most common and expected situation.
			if (lstrlenA(pCurMdat->szCompression) == 4)
			{
				// Copy the fourCC from the string into a temporary DWORD.
				DWORD dwTemporary = *PDWORD(pCurMdat->szCompression);

				// If both properties exist and the fourCC from OMFI:DIDD:Compression matches the
				// fourCC from OMFI:DIDD:DIDCompressMethod then save that fourCC in dwNonambiguousCompressionFourCC.
				// This is the most common and expected situation.
				if (dwTemporary == pCurMdat->dwCompressFourCC)
				{
					pCurMdat->dwNonambiguousCompressionFourCC = dwTemporary;
					continue;
				}

				// If the OMFI:DIDD:DIDCompressMethod property does not exist then
				// copy the fourCC from OMFI:DIDD:Compression to dwNonambiguousCompressionFourCC.
				if (pCurMdat->dwCompressFourCC == 0)
				{
					pCurMdat->dwNonambiguousCompressionFourCC = dwTemporary;
					continue;
				}

				// If both properties exist but the fourCC from OMFI:DIDD:Compression does not match
				// the fourCC from OMFI:DIDD:DIDCompressMethod then save (-1) in dwNonambiguousCompressionFourCC.
				// This will indicate a special case, a conflict - where the world has gone to hell.
				// This is the unexpected situation that this routine is designed to detect.
				// Does it ever happen?
				BREAK_IF_DEBUG
				pCurMdat->dwNonambiguousCompressionFourCC = (-1);
				continue;
			}

			// There is one known exception to this design philosophy. For some reason the JPED codec
			// in Avid's omfToolkit.dll sometimes used the string "1095389255" for OMFI:DIDD:Compression. 
			// See codecGetSelectInfoJPEG() in the file "omcAvJPED.c" in Avid's reference code.
			// It is the same as "AJPEG".
			if (0 == lstrcmpA(pCurMdat->szCompression, "1095389255"))
			{
				if ((pCurMdat->dwCompressFourCC == 0)||(pCurMdat->dwCompressFourCC == FCC('AJPG')))
				{
					pCurMdat->dwNonambiguousCompressionFourCC = FCC('AJPG');
				}
				else
				{
					// Oops! The OMFI:DIDD:Compression property is "1095389255" but the OMFI:DIDD:DIDCompressMethod is not "AJPG".
					BREAK_IF_DEBUG
					pCurMdat->dwNonambiguousCompressionFourCC = (-1);
				}
				continue;
			}

			// If we ever discover other compression strings that are not fourCCs then add more handlers here.
			BREAK_IF_DEBUG

			//if (0 == lstrcmpA(pCurMdat->szCompression, "some unknown string"))
			//{
			//	if ((pCurMdat->dwCompressFourCC == 0)||(pCurMdat->dwCompressFourCC == FCC('????')))
			//	{
			//		pCurMdat->dwNonambiguousCompressionFourCC = FCC('????');
			//	}
			//}
		} while (++pCurMdat < pEndMdat);
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Just for debugging (for now).
//*********************************************************************************************************************
HRESULT CContainerLayer10::DetectOverlappers(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		do {
			UINT64 cbThisDataStart	= pCurMdat->cbPayloadOffset;
			UINT64 cbThisDataEnd	= pCurMdat->cbPayloadOffset + pCurMdat->cbPayloadLength;
			PMDAT_CACHE_ENTRY pOther = m_aMdatTable;
			do {
				if (pOther != pCurMdat)
				{
					UINT64 cbThatDataStart	= pOther->cbPayloadOffset;
					UINT64 cbThatDataEnd	= pOther->cbPayloadOffset + pOther->cbPayloadLength;

					if (cbThisDataEnd <= cbThatDataStart)
					{
						continue;
					}

					if (cbThisDataStart >= cbThatDataEnd)
					{
						continue;
					}

					if ((cbThisDataStart == cbThatDataStart)&&(cbThisDataEnd == cbThatDataEnd))
					{
						//pCurMdat->cClones++;
						//pOther->cClones++;
						m_cMdatClones++;
						continue;
					}

					BREAK_IF_DEBUG

					if ((cbThatDataStart == cbThisDataStart) && (cbThatDataEnd == cbThisDataEnd))
					{
						pCurMdat->cClones++;
						pOther->cClones++;
						m_cMdatClones++;
						continue;
					}

					if ((cbThatDataStart >= cbThisDataStart) && (cbThatDataStart < cbThisDataEnd))
					{
						if (cbThatDataEnd <= cbThisDataEnd)
						{
							// Inc the number of other MDAT payloads that live entirely inside this MDAT.
							pCurMdat->cNestedMdats++;

							// Inc the total number of MDATs that live entirely inside another MDAT.
							m_cNestedMdats++;
						}
						else
						{
							// Inc the number of other MDAT payloads that begin inside this MDAT.
							pCurMdat->cOverlappedStarts++;

							// Inc the total number of MDATs that begin inside another MDAT.
							m_cOverlappedStarts++;
						}
						continue;
					}

					if ((cbThatDataEnd > cbThisDataStart) && (cbThatDataEnd < cbThisDataEnd))
					{
						// Inc the number of other MDAT payloads that end inside this MDAT.
						pCurMdat->cOverlappedEnds++;

						// Inc the total number of MDATs that end inside another MDAT.
						m_cOverlappedEnds++;
					}
				}
			} while (++pOther < pEndMdat);
		} while (++pCurMdat < pEndMdat);
	}

	// Just for debugging.
	if (m_cNestedMdats || m_cOverlappedStarts || m_cOverlappedEnds)
	{
		BREAK_IF_DEBUG
	}

	return S_OK;
}
