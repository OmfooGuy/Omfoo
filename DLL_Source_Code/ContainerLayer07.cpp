// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer07.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer07.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer07::CContainerLayer07(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer07::~CContainerLayer07(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer07::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//	if (SUCCEEDED(hr))
//	{
//		hr = ????????????
//	}
//	return hr;
//}

//*********************************************************************************************************************
//	These methods are just wrappers. They find the BENTO_BLOP structure for their 32-bit object id argument,
//	and then pass that structure down to a corresponding 'Robj' method that does the actual work.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMdatFromMdes(DWORD dwMDES)
{return BlopFindMdatFromMdes(GetBlop(dwMDES));}

DWORD CContainerLayer07::FindMdatFromSmob(DWORD dwSMOB)
{return BlopFindMdatFromSmob(GetBlop(dwSMOB));}

DWORD CContainerLayer07::FindMdesFromMdat(DWORD dwMDAT)
{return BlopFindMdesFromMdat(GetBlop(dwMDAT));}

DWORD CContainerLayer07::FindMdesFromSmob(DWORD dwSMOB)
{return BlopFindMdesFromSmob(GetBlop(dwSMOB));}

DWORD CContainerLayer07::FindSmobFromMdat(DWORD dwMDAT)
{return BlopFindSmobFromMdat(GetBlop(dwMDAT));}

DWORD CContainerLayer07::FindSmobFromMdes(DWORD dwMDES)
{return BlopFindSmobFromMdes(GetBlop(dwMDES));}

BOOL CContainerLayer07::FindMobIDForMdat(DWORD dwMDAT, POMF_MOB_ID pMobID)
{return BlopFindMobIDForMdat(GetBlop(dwMDAT), pMobID);}

//*********************************************************************************************************************
//	Find the Media Data object (MDAT) associated with the specified Media Descriptor object (MDES).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindMdatFromMdes(BENTO_BLOP& rMDES)
{
	DWORD dwSMOB = BlopFindSmobFromMdes(rMDES);
	return BlopFindMdatFromSmob(GetBlop(dwSMOB));
}

//*********************************************************************************************************************
//	Find the Media Data object (MDAT) associated with the specified Source Mob (SMOB).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindMdatFromSmob(BENTO_BLOP& rSMOB)
{
	OMF_MOB_ID	mobID	= {0};
	DWORD		dwMDAT	= 0;

	// OMFI:MOBJ:MobID
	if (SUCCEEDED(CoreReadMobID(rSMOB, m_dwPropMobjMobID, &mobID)))
	{
		dwMDAT = FindMdatFromMobID(mobID);
	}

	return dwMDAT;
}

//*********************************************************************************************************************
//	Find the Media Descriptor object (MDES) associated with the specified Media Data object (MDAT).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindMdesFromMdat(BENTO_BLOP& rMDAT)
{
	DWORD	dwRefList	= 0;
	DWORD	dwMDES		= 0;
	DWORD	dwSMOB		= BlopFindSmobFromMdat(rMDAT);
	CoreReadObjRef(GetBlop(dwSMOB), m_dwPropMediaDesc, &dwRefList, &dwMDES);
	return dwMDES;
}

//*********************************************************************************************************************
//	Find the Media Descriptor object (MDES) associated with the specified Source Mob (SMOB).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindMdesFromSmob(BENTO_BLOP& rSMOB)
{
	DWORD		dwMDES		= 0;
	DWORD		dwRefList	= 0;
	CoreReadObjRef(rSMOB, m_dwPropMediaDesc, &dwRefList, &dwMDES);
	return dwMDES;
}

//*********************************************************************************************************************
//	Find the Source Mob (SMOB) associated with the specified Media Data object (MDAT).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdat(BENTO_BLOP& rMDAT)
{
	OMF_MOB_ID	mobID	= {0};
	DWORD		dwSMOB	= 0;

	if (BlopFindMobIDForMdat(rMDAT, &mobID))
	{
		dwSMOB = FindSmobFromMobID(mobID);
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes(BENTO_BLOP& rMDES)
{
	DWORD dwSMOB = 0;

	if (m_fOmfVer1)
	{
		dwSMOB = BlopFindSmobFromMdes_SearchLoop1(rMDES);
		if (0 == dwSMOB)
		{
			dwSMOB = BlopFindSmobFromMdes_SearchLoop2(rMDES);
			if (0 == dwSMOB)
			{
				dwSMOB = BlopFindSmobFromMdes_SearchLoop3(rMDES);
			}
		}
	}
	else
	{
		dwSMOB = BlopFindSmobFromMdes_SearchLoop4(rMDES);
		if (0 == dwSMOB)
		{
			dwSMOB = BlopFindSmobFromMdes_SearchLoop5(rMDES);
			if (0 == dwSMOB)
			{
				dwSMOB = BlopFindSmobFromMdes_SearchLoop6(rMDES);
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Find the mob id associated with the specified Media Data object (MDAT).
//	This easy under OMF2 because all MDATs have (or are supposed to have) the OMFI:MDAT:MobID property.
//	But under OMF1 things are more complicated.
//	First of all, if the MDAT _does_ have a mob id, then it's property name is tied to its class.
//	For example:
//
//		OMFI:AIFC:MobID
//		OMFI:IDAT:MobID
//		OMFI:JPEG:MobID
//		OMFI:SD2M:MobID
//		OMFI:TIFF:MobID
//		OMFI:WAVE:MobID
//
//	See what I mean?
//
//	To further complicate things, in some OMF1 files the MDATs have no mob id property at all!
//	For an example see the test file "Fireman 4x3 NTSC (audio in frames) Rev1.omf".
//	In this case we have to search through the "OMFI:MediaData" array (in m_pV1MediaData) to match the object id with
//	its mob id.
//*********************************************************************************************************************
BOOL CContainerLayer07::BlopFindMobIDForMdat(BENTO_BLOP& rMDAT, POMF_MOB_ID pMobID)
{
	ULONG	i = 0;
	ULONG	nElements		= 0;

	// Validate caller's pointer.
	if (IsBadWritePointer(pMobID, sizeof(OMF_MOB_ID)))
	{
		return FALSE;
	}

	// Wipe caller's buffer.
	ZeroMemory(pMobID, sizeof(OMF_MOB_ID));

	// Validate caller's rMDAT argument.
	if (0 == rMDAT.dwObject)
	{
		return FALSE;
	}

	if (SUCCEEDED(CoreReadFirstMobID(rMDAT, pMobID)))
	{
		return TRUE;
	}

	// If that doesn't work then see if the mob id lives in the OMFI:MediaData array.
	// This array is only available in OMF1 files and it is optional.
	if (m_pV1MediaData)
	{
		nElements = m_pV1MediaData->nElements;
		for (i = 0; i < nElements; i++)
		{
			// Compare object ids.
			if (rMDAT.dwObject == m_pV1MediaData->a[i].dwObject)
			{
				// Copy the entire mob id. (12 bytes)
				*pMobID = m_pV1MediaData->a[i].mobID;
				return TRUE;
			}
		}
	}

	return FALSE;
}

//*********************************************************************************************************************
//	
//*********************************************************************************************************************
DWORD CContainerLayer07::FindNthMobByName(__in ULONG nInstance, __in_opt DWORD dwClassFourCC, __in PCSTR pszMobName)
{
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pLastBlop	= &m_aBlopTable[m_nBlops];
	DWORD	dwMOBJ			= 0;
	ULONG	nHits			= 0;

	if (dwClassFourCC)
	{
		do
		{
			if (pCurBlop->dwFourCC == dwClassFourCC)
			{
				PTOCX_ITEM pCurItem	= &m_aToc[pCurBlop->iFirstItem];
				PTOCX_ITEM pEndItem	= &pCurItem[pCurBlop->wTotalItems];
				do
				{
					if ((pCurItem->dwProperty == m_dwPropMobjName) &&
						(pCurItem->dwDataType == m_dwTypeString) &&
						(pCurItem->bStorageMode == SM_CACHED))
					{
						if (lstrcmpiA(pCurItem->pCachedString, pszMobName))
						{
							// We found and tested this mob's name property but it doesn't match.
							// A mob can have only one OMFI:MOBJ:Name (or OMFI:CPNT:Name) property.
							// So break out of this inner do() loop, and advance to the next blop.
							break;
						}

						// We found a match!
						// Is this the Nth instance?
						if (nHits == nInstance)
						{
							// Success! We found the Nth match!
							// Save this mob's object ID.
							// Break out of both do{} loops and exit.
							dwMOBJ = pCurBlop->dwObject;
							goto L_Exit;
						}

						// If this is NOT the Nth instance then increment the hit counter.
						nHits++;

						// A mob can have only one OMFI:MOBJ:Name (or OMFI:CPNT:Name) property.
						// So break out of this inner do() loop, and advance to the next blop.
						break;
					}
				} while (++pCurItem < pEndItem);
			}
		} while (++pCurBlop < pLastBlop);
	}
	else
	{
		do
		{
			// FCC('OOBJ')
			if ((pCurBlop->dwFourCC == FCC('SMOB')) ||
				(pCurBlop->dwFourCC == FCC('MMOB')) ||
				(pCurBlop->dwFourCC == FCC('CMOB')) ||
				(pCurBlop->dwFourCC == FCC('MOBJ')))
			{
				PTOCX_ITEM pCurItem	= &m_aToc[pCurBlop->iFirstItem];
				PTOCX_ITEM pEndItem	= &pCurItem[pCurBlop->wTotalItems];
				do
				{
					if ((pCurItem->dwProperty == m_dwPropMobjName) &&
						(pCurItem->dwDataType == m_dwTypeString) &&
						(pCurItem->bStorageMode == SM_CACHED))
					{
						if (lstrcmpiA(pCurItem->pCachedString, pszMobName))
						{
							// We found and tested this mob's name property but it doesn't match.
							// A mob can have only one OMFI:MOBJ:Name (or OMFI:CPNT:Name) property.
							// So break out of this inner do() loop, and advance to the next blop.
							break;
						}

						// We found a match!
						// Is this the Nth instance?
						if (nHits == nInstance)
						{
							// Success! We found the Nth match!
							// Save this mob's object ID.
							// Break out of both do{} loops and exit.
							dwMOBJ = pCurBlop->dwObject;
							goto L_Exit;
						}

						// If this is NOT the Nth instance then increment the hit counter.
						nHits++;

						// A mob can have only one OMFI:MOBJ:Name (or OMFI:CPNT:Name) property.
						// So break out of this inner do() loop, and advance to the next blop.
						break;
					}
				} while (++pCurItem < pEndItem);
			}
		} while (++pCurBlop < pLastBlop);
	}

L_Exit:
	return dwMOBJ;
}

//*********************************************************************************************************************
//	Find the Media Data object (MDAT) associated with the specified mob ID.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMdatFromMobID(OMF_MOB_ID& rMobID)
{
	DWORD dwMDAT = 0;

	if (m_fOmfVer1)
	{
		dwMDAT = FindMdatFromMobID_SearchLoop1(rMobID);
	}
	else
	{
		dwMDAT = FindMdatFromMobID_SearchLoop2(rMobID);
	}

	// Either version...
	if (0 == dwMDAT)
	{
		dwMDAT = FindMdatFromMobID_SearchLoop3(rMobID);
	}

	return dwMDAT;
}

//*********************************************************************************************************************
//	Find the Media Descriptor object (MDES) associated with the specified mob ID.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMdesFromMobID(OMF_MOB_ID& rMobID)
{
	DWORD dwSMOB = FindSmobFromMobID(rMobID);
	return FindMdesFromSmob(dwSMOB);
}

//*********************************************************************************************************************
//	Find the Source Mob (SMOB) associated with the specified mob ID.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindSmobFromMobID(OMF_MOB_ID& rMobID)
{
	DWORD dwSMOB = FindSmobFromMobID_SearchLoop1(rMobID);
	{
		if (0 == dwSMOB)
		{
			dwSMOB = FindSmobFromMobID_SearchLoop2(rMobID);
			if (0 == dwSMOB)
			{
				dwSMOB = FindSmobFromMobID_SearchLoop3(rMobID);
				if (0 == dwSMOB)
				{
					dwSMOB = FindMobjFromMobID(rMobID);
				}
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Find the Source Clip (SCLP) associated with the specified mob ID.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindSclpFromMobID(OMF_MOB_ID& rMobID)
{
	// Use m_aBlopTable[] to search for all SCLPs.
	OMF_MOB_ID	mobID2				= {0};
	SIZE_T		i					= 0;
	DWORD		dwSCLP				= 0;
	DWORD		dwPropSclpSourceID	= OrdinalToPropertyID(ePropSclpSourceID);

	for (i = 0; i < m_nBlops; i++)
	{
		if (m_aBlopTable[i].dwFourCC == FCC('SCLP'))
		{
			// OMFI:SCLP:SourceID
			if (SUCCEEDED(CoreReadMobID(m_aBlopTable[i], dwPropSclpSourceID, &mobID2)))
			{
				if ((rMobID.dwMinor	== mobID2.dwMinor) &&
					(rMobID.dwMajor	== mobID2.dwMajor) &&
					(rMobID.dwPrefix	== mobID2.dwPrefix))
				{
					dwSCLP = m_aBlopTable[i].dwObject;
					break;
				}
			}
		}
	}

	return dwSCLP;
}

//*********************************************************************************************************************
//	Performs a comprehensive search for the mob having the specified mob ID.
//	If successfull, it will return the object ID of that mob.
//	This routine finds Composition Mobs (CMOB), Master Mobs (MMOB), Source Mobs (SMOB), and 'Generic Mobs' (MOBJ).
//	The first loop tests each member of the "OMFI:CompositionMobs" array for a matching mobID.
//	If that fails, the second loop tests each member of the "OMFI:SourceMobs" array for a matching mobID.
//	If that fails, the third loop tests each TOCX_ITEM structure in CReadBento::m_aToc[] for a matching mobID.
//	This technique works with OMF1 and OMF2.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMobjFromMobID(OMF_MOB_ID& rMobID)
{
	// "OMFI:CompositionMobs" - OMF1 only, optional
	if (m_pCompositionMobs)
	{
		for (UINT i = 0; i < m_pCompositionMobs->nElements; i++)
		{
			if ((m_pCompositionMobs->a[i].mobID.dwMinor == rMobID.dwMinor) &&
				(m_pCompositionMobs->a[i].mobID.dwMajor == rMobID.dwMajor) &&
				(m_pCompositionMobs->a[i].mobID.dwPrefix == rMobID.dwPrefix))
			{
				return m_pCompositionMobs->a[i].dwObject;
			}
		}
	}

	// "OMFI:SourceMobs" - OMF1 only, optional
	if (m_pSourceMobs)
	{
		for (UINT i = 0; i < m_pSourceMobs->nElements; i++)
		{
			if ((m_pSourceMobs->a[i].mobID.dwMinor == rMobID.dwMinor) &&
				(m_pSourceMobs->a[i].mobID.dwMajor == rMobID.dwMajor) &&
				(m_pSourceMobs->a[i].mobID.dwPrefix == rMobID.dwPrefix))
			{
				return m_pSourceMobs->a[i].dwObject;
			}
		}
	}

	// OMF1 & OMF2
	PTOCX_ITEM	pCurItem	= m_aToc;
	PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
	do
	{
		if ((pCurItem->dwProperty	== m_dwPropMobjMobID) &&
			(pCurItem->dwDataType	== m_dwTypeUID) &&
			(pCurItem->bStorageMode	== SM_CACHED))
		{
			POMF_MOB_ID pMobID		= POMF_MOB_ID(&pCurItem->aImmediateBytes);
			if ((pMobID->dwMinor	== rMobID.dwMinor) &&
				(pMobID->dwMajor	== rMobID.dwMajor) &&
				(pMobID->dwPrefix	== rMobID.dwPrefix))
			{
				return pCurItem->dwObject;
			}
			//else
			//{
			//	// WTF?
			//	// All OMF_MOB_IDs should have been cached before we come here.
			//	// See CContainerLayer00:HarvestMobIDs().
			//	BREAK_IF_DEBUG
			//	break;
			//}
		}
	} while (++pCurItem < pEndItem);

	return 0;
}

//*********************************************************************************************************************
//	Private helper for BlopFindSmobFromMdes(). We come here when the file is OMF1.
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES).
//	This loop searches the "OMFI:SourceMobs" array - if it exists.
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes_SearchLoop1(BENTO_BLOP& rMDES)
{
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwObjRef	= 0;
	DWORD		dwRefList	= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwSMOB		= 0;

	// OMFI:SourceMobs
	if (m_pSourceMobs)
	{
		nElements = m_pSourceMobs->nElements;
		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pSourceMobs->a[i].dwObject;

			// OMFI:MOBJ:PhysicalMedia
			if (SUCCEEDED(CoreReadObjRef(GetBlop(dwOOBJ), m_dwPropMediaDesc, &dwRefList, &dwObjRef)))
			{
				if (dwObjRef == rMDES.dwObject)
				{
					dwSMOB = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for BlopFindSmobFromMdes(). We come here when the file is OMF1.
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES).
//	This loop searches the "OMFI:ObjectSpine" array - if it exists.
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes_SearchLoop2(BENTO_BLOP& rMDES)
{
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwObjRef	= 0;
	DWORD		dwRefList	= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwSMOB		= 0;

	// OMFI:ObjectSpine
	if (m_pObjectSpine)
	{
		nElements = m_pObjectSpine->nElements;
		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pObjectSpine->a[i];

			// OMFI:MOBJ:PhysicalMedia
			if (SUCCEEDED(CoreReadObjRef(GetBlop(dwOOBJ), m_dwPropMediaDesc, &dwRefList, &dwObjRef)))
			{
				if (dwObjRef == rMDES.dwObject)
				{
					dwSMOB = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for BlopFindSmobFromMdes(). We come here when the file is OMF1.
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES). 
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes_SearchLoop3(BENTO_BLOP& rMDES)
{
	PTOCX_ITEM	pItem		= m_aToc;
	PTOCX_ITEM	pEnd		= &m_aToc[m_nTocItems];
	DWORD		dwCompare	= 0;
	DWORD		dwObjRef	= 0;
	DWORD		dwSMOB		= 0;

	// Note: Use m_fBentoBigEndian not m_fOmfBigEndian!
	if (m_fBentoBigEndian)
		dwCompare = Endian32(rMDES.dwObject);
	else
		dwCompare = rMDES.dwObject;

	do
	{
		// OMFI:MOBJ:PhysicalMedia
		if ((pItem->dwProperty == m_dwPropMediaDesc) &&
			(pItem->bStorageMode == SM_OFFSET) &&
			(pItem->cbLength64 == 8))
		{
			if (SUCCEEDED(SeekRead(pItem->cbOffset64, &dwObjRef, sizeof(DWORD))))
			{
				if (dwObjRef == dwCompare)
				{
					dwSMOB = pItem->dwObject;
					break;
				}
			}
		}
	} while (++pItem < pEnd);

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for BlopFindSmobFromMdes(). We come here when the file is OMF2.
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES).
//	This loop searches the "OMFI:HEAD:PrimaryMobs" array - if it exists
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes_SearchLoop4(BENTO_BLOP& rMDES)
{
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwObjRef	= 0;
	DWORD		dwRefList	= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwSMOB		= 0;

	// OMFI:HEAD:PrimaryMobs
	if (m_pPrimaryMobs)
	{
		nElements = m_pPrimaryMobs->nElements;
		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pPrimaryMobs->a[i];

			// OMFI:SMOB:MediaDescription
			if (SUCCEEDED(CoreReadObjRef(GetBlop(dwOOBJ), m_dwPropMediaDesc, &dwRefList, &dwObjRef)))
			{
				if (dwObjRef == rMDES.dwObject)
				{
					dwSMOB = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for BlopFindSmobFromMdes(). We come here when the file is OMF2.
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES).
//	This loop searches the "OMFI:HEAD:Mobs" array - if it exists.
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes_SearchLoop5(BENTO_BLOP& rMDES)
{
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwObjRef	= 0;
	DWORD		dwRefList	= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwSMOB		= 0;

	// OMFI:HEAD:Mobs
	if (m_pPublicMobs)
	{
		nElements = m_pPublicMobs->nElements;
		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pPublicMobs->a[i];

			// OMFI:SMOB:MediaDescription
			if (SUCCEEDED(CoreReadObjRef(GetBlop(dwOOBJ), m_dwPropMediaDesc, &dwRefList, &dwObjRef)))
			{
				if (dwObjRef == rMDES.dwObject)
				{
					dwSMOB = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for BlopFindSmobFromMdes(). We come here when the file is OMF2.
//	Find the Source Mob (SMOB) associated with the specified Media Descriptor object (MDES).
//*********************************************************************************************************************
DWORD CContainerLayer07::BlopFindSmobFromMdes_SearchLoop6(BENTO_BLOP& rMDES)
{
	PTOCX_ITEM	pItem		= m_aToc;
	PTOCX_ITEM	pEnd		= &m_aToc[m_nTocItems];
	DWORD		dwCompare	= 0;
	DWORD		dwSMOB		= 0;

	// This is a rare situation where we might need to byteswap a Bento object ID.
	// This type of byte-swapping normally takes place in CReadBento.
	// But in this case we are retrieving the object ID directly from the dwImmediateDword member of the TOCX_ITEM.
	// Note: Use m_fBentoBigEndian not m_fOmfBigEndian!
	if (m_fBentoBigEndian)
		dwCompare = Endian32(rMDES.dwObject);
	else
		dwCompare = rMDES.dwObject;

	do
	{
		// OMFI:SMOB:MediaDescription
		if ((pItem->dwProperty == m_dwPropMediaDesc) &&
			(pItem->bStorageMode == SM_IMMEDIATE))
		{
			if (pItem->dwImmediateDword == dwCompare)
			{
				dwSMOB = pItem->dwObject;
				break;
			}
		}
	} while (++pItem < pEnd);

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for FindMdatFromMobID(). We come here when the file is OMF1.
//	Find the Media Data object (MDAT) associated with the specified mob ID.
//	This loop searches the OMFI:MediaData array - if it exists
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMdatFromMobID_SearchLoop1(OMF_MOB_ID& rMobID)
{
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwMDAT		= 0;

	// OMFI:MediaData
	// This array is only available in OMF1 files and it's optional.
	// The array pairs an MDAT object id with a corresponding mob id.
	if (m_pV1MediaData)
	{
		nElements = m_pV1MediaData->nElements;

		for (i = 0; i < nElements; i++)
		{
			if ((rMobID.dwMinor == m_pV1MediaData->a[i].mobID.dwMinor) &&
				(rMobID.dwMajor == m_pV1MediaData->a[i].mobID.dwMajor) &&
				(rMobID.dwPrefix == m_pV1MediaData->a[i].mobID.dwPrefix))
			{
				dwMDAT = m_pV1MediaData->a[i].dwObject;
				break;
			}
		}
	}

	return dwMDAT;
}

//*********************************************************************************************************************
//	Private helper for FindMdatFromMobID(). We come here when the file is OMF2.
//	Find the Media Data object (MDAT) associated with the specified mob ID.
//	This loop searches the OMFI:HEAD:MediaData array - if it exists
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMdatFromMobID_SearchLoop2(OMF_MOB_ID& rMobID)
{
	OMF_MOB_ID	mobID2		= {0};
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwMDAT		= 0;

	// OMFI:HEAD:MediaData
	// This array is only available in OMF2 files and it's optional.
	if (m_pV2MediaData)
	{
		nElements = m_pV2MediaData->nElements;

		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pV2MediaData->a[i];

			if (SUCCEEDED(CoreReadFirstMobID(GetBlop(dwOOBJ), &mobID2)))
			{
				if ((mobID2.dwMinor == rMobID.dwMinor) &&
					(mobID2.dwMajor == rMobID.dwMajor) &&
					(mobID2.dwPrefix == rMobID.dwPrefix))
				{
					dwMDAT = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwMDAT;
}

//*********************************************************************************************************************
//	Private helper for FindMdatFromMobID().
//	Find the Media Data object (MDAT) associated with the specified mob ID.
//	Come here in OMF1 after we've searched our m_pV1MediaData array (OMFI:MediaData) without success.
//	Come here in OMF2 after we've searched our m_pV2MediaData array (OMFI:HEAD:MediaData) without success.
//	This routine walks through the entire m_aBlopTable[] and tests each entry to see if it inherits 'MDAT'.
//	If it does inherit 'MDAT' then read its mod id and compare.
//	Note that we cannot rely of the property name string - because we can't be sure what it is.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindMdatFromMobID_SearchLoop3(OMF_MOB_ID& rMobID)
{
	// Use m_aBlopTable[] to search for all MDATs.
	OMF_MOB_ID	mobID2		= {0};
	SIZE_T		i			= 0;
	DWORD		dwMDAT		= 0;

	for (i = 0; i < m_nBlops; i++)
	{
		// Does this entry inherit 'MDAT' ???
		if (S_OK == IsBlopATypeOf(m_aBlopTable[i], FCC('MDAT')))
		{
			// OMFI:MDAT:MobID
			// OMFI:AIFC:MobID
			// OMFI:IDAT:MobID
			// OMFI:JPEG:MobID
			// OMFI:SD2M:MobID
			// OMFI:TIFF:MobID
			// OMFI:WAVE:MobID
			if (SUCCEEDED(CoreReadFirstMobID(m_aBlopTable[i], &mobID2)))
			{
				if ((rMobID.dwMinor	== mobID2.dwMinor) &&
					(rMobID.dwMajor	== mobID2.dwMajor) &&
					(rMobID.dwPrefix	== mobID2.dwPrefix))
				{
					dwMDAT = m_aBlopTable[i].dwObject;
					break;
				}
			}
		}
	}

	return dwMDAT;
}

//*********************************************************************************************************************
//	Private helper for FindSmobFromMobID().
//	Find the Source Mob (SMOB) associated with the specified mob ID.
//	This loop searches the OMFI:SourceMobs array - if it exists.
//*********************************************************************************************************************
DWORD CContainerLayer07::FindSmobFromMobID_SearchLoop1(OMF_MOB_ID& rMobID)
{
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwSMOB		= 0;

	// OMFI:SourceMobs - OMF1 only, optional.
	if (m_pSourceMobs)
	{
		nElements = m_pSourceMobs->nElements;
		for (i = 0; i < nElements; i++)
		{
			if ((m_pSourceMobs->a[i].mobID.dwMajor == rMobID.dwMajor) &&
				(m_pSourceMobs->a[i].mobID.dwMinor == rMobID.dwMinor) &&
				(m_pSourceMobs->a[i].mobID.dwPrefix == rMobID.dwPrefix))
			{
				dwSMOB = m_pSourceMobs->a[i].dwObject;
				break;
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for FindSmobFromMobID().
//	Find the Source Mob (SMOB) associated with the specified mob ID.
//	This loop searches the OMFI:HEAD:PrimaryMobs array - if it exists.
//	The OMF Interchange Specification 2.1 says this array "should be examined first". (Page 152)
//*********************************************************************************************************************
DWORD CContainerLayer07::FindSmobFromMobID_SearchLoop2(OMF_MOB_ID& rMobID)
{
	OMF_MOB_ID	mobID2		= {0};
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwSMOB		= 0;

	if (m_pPrimaryMobs)
	{
		nElements = m_pPrimaryMobs->nElements;
		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pPublicMobs->a[i];

			// OMFI:MOBJ:MobID
			if (SUCCEEDED(CoreReadMobID(GetBlop(dwOOBJ), m_dwPropMobjMobID, &mobID2)))
			{
				if ((mobID2.dwMinor == rMobID.dwMinor) &&
					(mobID2.dwMajor == rMobID.dwMajor) &&
					(mobID2.dwPrefix == rMobID.dwPrefix))
				{
					dwSMOB = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwSMOB;
}

//*********************************************************************************************************************
//	Private helper for FindSmobFromMobID().
//	Find the Source Mob (SMOB) associated with the specified mob ID.
//	This loop searches the OMFI:HEAD:Mobs array - if it exists.
//	The OMF Interchange Specification 2.1 says this array includes "all Mobs" in the file. (Page 152)
//*********************************************************************************************************************
DWORD CContainerLayer07::FindSmobFromMobID_SearchLoop3(OMF_MOB_ID& rMobID)
{
	OMF_MOB_ID	mobID2		= {0};
	SIZE_T		nElements	= 0;
	SIZE_T		i			= 0;
	DWORD		dwOOBJ		= 0;
	DWORD		dwSMOB		= 0;

	if (m_pPublicMobs)
	{
		nElements = m_pPublicMobs->nElements;
		for (i = 0; i < nElements; i++)
		{
			dwOOBJ = m_pPublicMobs->a[i];

			// OMFI:MOBJ:MobID
			if (SUCCEEDED(CoreReadMobID(GetBlop(dwOOBJ), m_dwPropMobjMobID, &mobID2)))
			{
				if ((mobID2.dwMajor == rMobID.dwMajor) &&
					(mobID2.dwMinor == rMobID.dwMinor) &&
					(mobID2.dwPrefix == rMobID.dwPrefix))
				{
					dwSMOB = dwOOBJ;
					break;
				}
			}
		}
	}

	return dwSMOB;
}


