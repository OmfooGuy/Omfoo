// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer08.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer08.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer08::CContainerLayer08(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer08::~CContainerLayer08(void)
{
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer08::Load(PCWSTR pwzFileName)
{
	HRESULT hr = __super::Load(pwzFileName);

	if (SUCCEEDED(hr))
	{
		if (m_fOmfVer1)
		{
			InventoryAppCodes();
			IngestRev1SourceMobs();
			IngestRev1CompositionMobs();
			CountAllSourceMobs();
			CountAllCompositionMobs();
			CountAllMasterMobs();
			CountAllGenericMobs();
			TraverseSourceClips();
			DecideRev1TopLevelMob();
		}
		else
		{
			InventoryAppCodes();
			CountAllSourceMobs();
			CountAllCompositionMobs();
			CountAllMasterMobs();
			CountRev2PrimaryMobs();
			CountRev2PublicMobs();
			TraverseSourceClips();
			DecideRev2TopLevelMob();
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Take inventory of each mob's OMFI:MOBJ:AppCode property.
//	Note that in OMF1 this property is named OMFI:MOBJ:UsageCode.
//	Update our m_aAppCodeCounters[] array, our m_nUnknownAppCodes member, and our m_dwAppCodesExistMask member.
//	Copy the value (a 32-bit DWORD) into the 8-bit bMobjAppCode member of the mob's BENTO_BLOP structure.
//*********************************************************************************************************************
HRESULT CContainerLayer08::InventoryAppCodes()
{
	// The property is optional in OMF2 (and undocumented).
	if (m_dwPropAppCode)
	{
		PTOCX_ITEM pCurItem	= m_aToc;
		PTOCX_ITEM pEndItem	= &m_aToc[m_nTocItems];

		do
		{
			if ((pCurItem->dwProperty == m_dwPropAppCode) &&
				(pCurItem->bStorageMode == SM_IMMEDIATE) &&
				(pCurItem->cbLength64 == sizeof(DWORD)))
			{
				DWORD dwAppCode = 0;
				if (m_fOmfBigEndian)
					dwAppCode = Endian32(pCurItem->dwImmediateDword);
				else
					dwAppCode = pCurItem->dwImmediateDword;

				if (dwAppCode < ELEMS(m_aAppCodeCounters))
				{
					// Set the code-specific bit.
					m_dwAppCodesExistMask |= 0x00000001 << dwAppCode;
		
					// Increment the code-specific counter.
					m_aAppCodeCounters[dwAppCode]++;
				}
				else
				{
					// Set the 'unknown code' bit.
					m_dwAppCodesExistMask |= 0x80000000;

					// Increment the 'unknown code' counter.
					m_nUnknownAppCodes++;

					// Special-case value for bMobjAppCode.
					dwAppCode = 31;
				}

				// Patch the bMobjAppCode member of the mob's BENTO_BLOP structure.
				GetBlop(pCurItem->dwObject).bMobjAppCode = BYTE(dwAppCode);
			}
		} while (++pCurItem < pEndItem);
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//
//	There is no SMOB class in OMF1 so we fake it. Here we search through all MOBJs to see if they fit the description
//	of an OMF2 Source Mob, and if so we update the _dwClassFourCC member of their BENTO_BLOP from MOBJ to SMOB.
//
//	There are two ways to determine if an OMF1 MOBJ is a Source Mob.
//	The first way is to see if the MOBJ a member of the OMFI:SourceMobs array.
//	The second way is to see if it has the OMFI:MOBJ:PhysicalMedia property.
//	This routine does both.
//*********************************************************************************************************************
HRESULT CContainerLayer08::IngestRev1SourceMobs(void)
{
	// "OMFI:SourceMobs" - OMF1 only, optional.
	// If this array exists then systematically replace each member's fourCC with 'SMOB'.
	if (m_pSourceMobs)
	{
		for (UINT i = 0; i < m_pSourceMobs->nElements; i++)
		{
			GetBlop(m_pSourceMobs->a[i].dwObject).dwFourCC = FCC('SMOB');
		}
	}

	// Most (?? all ??) OMF1 Source Mobs are members of the OMFI:SourceMobs array (see above).
	// So at this point we've already promoted most of them (if not all of them) from MOBJs to SMOBs. 
	// Nevertheless, just to be sure, next we walk though our blop table and pick-off any remaining MOBJs
	// that have a OMFI:MOBJ:PhysicalMedia property.
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];
	do {
		if (pCurBlop->dwFourCC == FCC('MOBJ'))
		{
			PTOCX_ITEM pCurItem	= &m_aToc[pCurBlop->iFirstItem];
			PTOCX_ITEM pEndItem	= &pCurItem[pCurBlop->wTotalItems];
			do
			{
				// OMFI:MOBJ:PhysicalMedia
				if (pCurItem->dwProperty == m_dwPropMediaDesc)
				{
					// Update the fourCC.
					pCurBlop->dwFourCC = FCC('SMOB');
					break;
				}
			} while (++pCurItem < pEndItem);
		}
	} while (++pCurBlop < pEndBlop);

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//
//	There are no MMOB or CMOB classes in OMF1 so we fake it. Note that omfToolkit.dll faked it too.
//	Here we search through all MOBJs to see if they fit the description of a Master Mob or a Composition Mob,
//	and if so we promote their class fourCC accordingly.
//	We must call InventoryAppCodes() and PromoteRev1SourceMobs() first before comming here.
//*********************************************************************************************************************
HRESULT CContainerLayer08::IngestRev1CompositionMobs(void)
{
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];

	do {
		// If this is an OMF1 Mob ...
		if (pCurBlop->dwFourCC == FCC('MOBJ'))
		{
			// We placed the OMFI:MOBJ:UsageCode property in the BENTO_BLOP structure in InventoryAppCodes().
			DWORD dwAppCode = pCurBlop->bMobjAppCode;

			// If the OMFI:MOBJ:UsageCode property is either of these then this is a Master Mob.
			// This is the same way "omfToolkit.dll" does it.
			if ((dwAppCode == AC_MASTER) || (dwAppCode == AC_PRECOMPUTE))
			{
				pCurBlop->dwFourCC = FCC('MMOB');
				continue;
			}

			// If the OMFI:MOBJ:UsageCode property is any of these then this is a Composition Mob.
			// This is the same way omfToolkit.dll does it.
			if ((dwAppCode == AC_NULL) || (dwAppCode == AC_SUBCLIP) ||
				(dwAppCode == AC_EFFECT) || (dwAppCode == AC_GROUP) ||
				(dwAppCode == AC_GROUPINDIRECT) || (dwAppCode == AC_MOTION))
			{
				pCurBlop->dwFourCC = FCC('CMOB');
				continue;
			}

			// If the OMFI:MOBJ:UsageCode property is anything else then we just keep the 'MOBJ' four cc.
		}
	} while (++pCurBlop < pEndBlop);

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Saves the result in m_dwTopLevelMob.
//	This implementation always succeeds.
//*********************************************************************************************************************
HRESULT CContainerLayer08::DecideRev1TopLevelMob(void)
{
	if (m_pCompositionMobs)
	{
		// Find the first CMOB in the header's OMFI:CompositionMob array that has no owner,
		// and whose OMFI:MOBJ:UsageCode property is AC_NULL.
		for (UINT i = 0; i < m_pCompositionMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pCompositionMobs->a[i].dwObject);
			if ((rBlop.dwFourCC == FCC('CMOB')) &&
				(rBlop.bMobjAppCode == AC_NULL) &&
				(!rBlop.fMobjHasOwner))
			{
				// Found it!
				// Save the object ID and exit.
				m_dwTopLevelMob = m_pCompositionMobs->a[i].dwObject;
				goto L_Exit;
			}
		}

		// Same as above except we ignore the OMFI:MOBJ:UsageCode property.
		for (UINT i = 0; i < m_pCompositionMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pCompositionMobs->a[i].dwObject);
			if ((rBlop.dwFourCC == FCC('CMOB')) &&
				(!rBlop.fMobjHasOwner))
			{
				// Found it!
				// Save the object ID and exit.
				m_dwTopLevelMob = m_pCompositionMobs->a[i].dwObject;
				goto L_Exit;
			}
		}

		// Find the first MMOB in the header's OMFI:CompositionMob array that has no owner.
		for (UINT i = 0; i < m_pCompositionMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pCompositionMobs->a[i].dwObject);
			if ((rBlop.dwFourCC == FCC('MMOB')) &&
				(!rBlop.fMobjHasOwner))
			{
				// Found it!
				// Save the object ID and exit.
				m_dwTopLevelMob = m_pCompositionMobs->a[i].dwObject;
				goto L_Exit;
			}
		}
	}

	// If the OMFI:CompositionMob array doesn't exist, or if the above tests failed then
	// find the first MMOB in the blop table that has no owner.
	for (UINT i = 0; i < m_nBlops; i++)
	{
		if ((m_aBlopTable[i].dwFourCC == FCC('CMOB')) &&
			(m_aBlopTable[i].bMobjAppCode == AC_NULL) &&
			(!m_aBlopTable[i].fMobjHasOwner))
		{
			// Found it!
			// Save the object ID and exit.
			m_dwTopLevelMob = m_aBlopTable[i].dwObject;
			goto L_Exit;
		}
	}

	// Same as above except we ignore the OMFI:MOBJ:UsageCode property.
	for (UINT i = 0; i < m_nBlops; i++)
	{
		if ((m_aBlopTable[i].dwFourCC == FCC('CMOB')) &&
			(!m_aBlopTable[i].fMobjHasOwner))
		{
			// Found it!
			// Save the object ID and exit.
			m_dwTopLevelMob = m_aBlopTable[i].dwObject;
			goto L_Exit;
		}
	}

	// If all of the tests above fail then return the very first MMOB that has no owner.
	for (UINT i = 0; i < m_nBlops; i++)
	{
		if ((m_aBlopTable[i].dwFourCC == FCC('MMOB')) &&
			(!m_aBlopTable[i].fMobjHasOwner))
		{
			// Found it!
			// Save the object ID and exit.
			m_dwTopLevelMob = m_aBlopTable[i].dwObject;
			goto L_Exit;
		}
	}

L_Exit:
	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//*********************************************************************************************************************
HRESULT CContainerLayer08::CountAllSourceMobs(void)
{
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];

	do
	{
		if (pCurBlop->dwFourCC == FCC('SMOB'))
		{
			DWORD dwAppCode = pCurBlop->bMobjAppCode;
			m_dwSMobAppCodeBitMask |= (0x00000001 << dwAppCode);
			m_nSourceMobs++;
			m_nTotalMobs++;
		}
	} while (++pCurBlop < pEndBlop);

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//*********************************************************************************************************************
HRESULT CContainerLayer08::CountAllCompositionMobs(void)
{
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];

	do
	{
		if (pCurBlop->dwFourCC == FCC('CMOB'))
		{
			DWORD dwAppCode = pCurBlop->bMobjAppCode;
			m_dwCMobAppCodeBitMask |= (0x00000001 << dwAppCode);
			m_nCompositionMobs++;
			m_nTotalMobs++;
		}
	} while (++pCurBlop < pEndBlop);

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//*********************************************************************************************************************
HRESULT CContainerLayer08::CountAllMasterMobs(void)
{
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];

	do
	{
		if (pCurBlop->dwFourCC == FCC('MMOB'))
		{
			DWORD dwAppCode = pCurBlop->bMobjAppCode;
			m_dwMMobAppCodeBitMask |= (0x00000001 << dwAppCode);
			m_nMasterMobs++;
			m_nTotalMobs++;
		}		
	} while (++pCurBlop < pEndBlop);

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//*********************************************************************************************************************
HRESULT CContainerLayer08::CountAllGenericMobs(void)
{
	PBENTO_BLOP	pCurBlop	= m_aBlopTable;
	PBENTO_BLOP	pEndBlop	= &m_aBlopTable[m_nBlops];

	do
	{
		if (pCurBlop->dwFourCC == FCC('MOBJ'))
		{
			DWORD dwAppCode = pCurBlop->bMobjAppCode;
			m_dwGMobAppCodeBitMask |= (0x00000001 << dwAppCode);
			m_nGenericMobs++;
			m_nTotalMobs++;
		}		
	} while (++pCurBlop < pEndBlop);

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Count the number of CMOBs, MMOBs, and SMOBs in the file by traversing the OMFI:HEAD:PrimaryMobs array.
//	The results are stored in our m_nPrimaryCMobs, m_nPrimaryMMobs, and m_nPrimarySMobs members.
//	This implementation always returns S_OK.
//	Note that the OMFI:HEAD:PrimaryMobs array is optional.
//*********************************************************************************************************************
HRESULT CContainerLayer08::CountRev2PrimaryMobs(void)
{
	// "OMFI:HEAD:PrimaryMobs" - OMF2 only, optional.
	if (m_pPrimaryMobs)
	{
		for (UINT i = 0; i < m_pPrimaryMobs->nElements; i++)
		{
			switch (GetBlop(m_pPrimaryMobs->a[i]).dwFourCC)
			{
			case FCC('CMOB'):
				// Increment the CMOB counter. 
				m_nPrimaryCMobs++;
				break;

			case FCC('MMOB'):
				// Increment the MMOB counter. 
				m_nPrimaryMMobs++;
				break;

			case FCC('SMOB'):
				// Increment the SMOB counter. 
				m_nPrimarySMobs++;
				break;
			}
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Count the number of CMOBs, MMOBs, and SMOBs in the file by traversing the OMFI:HEAD:Mobs array.
//	The results are stored in the m_nPublicCMobs, m_nPublicMMobs, and m_nPublicSMobs members of our MOB_ANALYTICS
//	structure. Note that the OMFI:HEAD:Mobs array is required.
//	This implementation always returns S_OK.
//*********************************************************************************************************************
HRESULT CContainerLayer08::CountRev2PublicMobs(void)
{
	// "OMFI:HEAD:Mobs" - OMF2 only, required.
	if (m_pPublicMobs)
	{
		for (UINT i = 0; i < m_pPublicMobs->nElements; i++)
		{
			switch (GetBlop(m_pPublicMobs->a[i]).dwFourCC)
			{
			case FCC('CMOB'):
				// Increment the CMOB counter. 
				m_nPublicCMobs++;
				break;

			case FCC('MMOB'):
				// Increment the MMOB counter. 
				m_nPublicMMobs++;
				break;

			case FCC('SMOB'):
				// Increment the SMOB counter. 
				m_nPublicSMobs++;
				break;
			}
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Come here to initialize the fMobjHasOwner flag of every mob's BENTO_BLOP structure.
//	1) Traverse all Source Clips.
//	2) Read their OMFI:SCLP:SourceID property - which is the object ID of a MMOB or CMOB.
//	3) Look up the mob's corresponding BENTO_BLOP structure and set its fMobjHasOwner flag.
//	We use this flag to help us decide if a mob is a top-level-mob.
//*********************************************************************************************************************
HRESULT CContainerLayer08::TraverseSourceClips(void)
{
	// OMFI:SCLP:SourceID
	DWORD dwPropSclpSourceID = OrdinalToPropertyID(ePropSclpSourceID);

	// While there are more blops in the blop table.
	for (SIZE_T i = 0; i < m_nBlops; i++)
	{
		// Is this a Source Clip?
		if (m_aBlopTable[i].dwFourCC == FCC('SCLP'))
		{
			// If yes then read the OMFI:SCLP:SourceID property.
			OMF_MOB_ID mobID	= {0};
			if (SUCCEEDED(CoreReadMobID(m_aBlopTable[i], dwPropSclpSourceID, &mobID)))
			{
				// Then find the corresponding mob and its blop, and set its fMobjHasOwner flag.
				GetBlop(FindMobjFromMobID(mobID)).fMobjHasOwner = TRUE;
			}
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Saves the result in m_dwTopLevelMob.
//*********************************************************************************************************************
HRESULT CContainerLayer08::DecideRev2TopLevelMob(void)
{
	// Strategy #1.
	// Find the first Composition Mob in the OMFI:HEAD:PrimaryMobs array with an OMFI:MOBJ:AppCode value of AC_NULL.
	// If it is not the 'source' for a Source Clip then deem it the "Top Level Mob".
	if (m_nPrimaryCMobs > 0)
	{
		for (UINT i = 0; i < m_pPrimaryMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPrimaryMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('CMOB')) &&
				(rBlop.bMobjAppCode == AC_NULL) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}

		// Strategy #2.
		// Same as #1 but this time ignore the app code.
		for (UINT i = 0; i < m_pPrimaryMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPrimaryMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('CMOB')) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}
	}

	// Strategy #3.
	// Find the first Master Mob in the OMFI:HEAD:PrimaryMobs array with an OMFI:MOBJ:AppCode value of AC_NULL,
	// and if it is not the 'source' for a Source Clip then deem it the "Top Level Mob".
	if (m_nPrimaryMMobs > 0)
	{
		for (UINT i = 0; i < m_pPrimaryMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPrimaryMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('MMOB')) &&
				(rBlop.bMobjAppCode == AC_NULL) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}

		// Strategy #4.
		// Same as #3 but this time ignore the app code.
		for (UINT i = 0; i < m_pPrimaryMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPrimaryMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('MMOB')) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}
	}

	// Strategy #5.
	// Find the first Composition Mob in the OMFI:HEAD:Mobs array with an OMFI:MOBJ:AppCode value of AC_NULL.
	// If it is not the 'source' for a Source Clip then deem it the "Top Level Mob".
	if (m_nPublicCMobs > 0)
	{
		for (UINT i = 0; i < m_pPublicMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPublicMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('CMOB')) &&
				(rBlop.bMobjAppCode == AC_NULL) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}

		// Strategy #6.
		// Same as #5 but this time ignore the app code.
		for (UINT i = 0; i < m_pPublicMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPublicMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('CMOB')) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}
	}

	// Strategy #7.
	// Find the first Master Mob in the OMFI:HEAD:Mobs array with an OMFI:MOBJ:AppCode value of AC_NULL.
	// If it is not the 'source' for a Source Clip then deem it the "Top Level Mob".
	if (m_nPrimaryMMobs > 0)
	{
		for (UINT i = 0; i < m_pPublicMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPublicMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('MMOB')) &&
				(rBlop.bMobjAppCode == AC_NULL) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}

		// Strategy #8.
		// Same as #7 but this time ignore the app code.
		for (UINT i = 0; i < m_pPublicMobs->nElements; i++)
		{
			BENTO_BLOP& rBlop = GetBlop(m_pPrimaryMobs->a[i]);
			if ((rBlop.dwFourCC == FCC('MMOB')) &&
				(!rBlop.fMobjHasOwner))
			{
				m_dwTopLevelMob = rBlop.dwObject;
				goto L_Exit;
			}
		}
	}

L_Exit:
	return S_OK;
}

