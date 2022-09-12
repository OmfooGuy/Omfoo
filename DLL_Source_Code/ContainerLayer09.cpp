// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer09.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer09.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer09::CContainerLayer09(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer09::~CContainerLayer09(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer09::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//
//	return hr;
//}

//*********************************************************************************************************************
//	Returns S_OK if the object associated with rBlop is a member of OMFI:HEAD:PrimaryMobs.
//	Returns S_FALSE if it is not.
//*********************************************************************************************************************
HRESULT CContainerLayer09::MobIsAPrimaryMob(DWORD dwMOBJ)
{
	// Only available in OMF2.
	if (m_fOmfVer1)
	{
		return OMF_E_NOT_IN_OMF1;
	}

	if (dwMOBJ == 0)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// This array is only available in OMF2 files and it is optional.
	if (m_pPrimaryMobs)
	{
		for (UINT i = 0; i < m_pPrimaryMobs->nElements; i++)
		{
			if (m_pPrimaryMobs->a[i] == dwMOBJ)
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

//*********************************************************************************************************************
//	Given a Source Mob, return another Source Mob that represents the media in its previous form or generation.
//	If the Source Mob describes the original source then return S_FALSE.
//	This is discussed in Chapter 4 of the OMF Interchange Specification Version 2.1, beginning on page 57
//	under the subheading "Following the Links to Original Sources".
//	On entry rArgMob is presumed to be some type of SMOB.
//*********************************************************************************************************************
HRESULT CContainerLayer09::BlopFindPreviousGenerationMob(BENTO_BLOP& rArgMob, PDWORD pdwSMOB)
{
	if (m_fOmfVer1)
		return BlopFindPrevGenMobRev1(rArgMob, pdwSMOB);
	else
		return BlopFindPrevGenMobRev2(rArgMob, pdwSMOB);
}

//*********************************************************************************************************************
//	Private helper for BlopFindPreviousGenerationMob().
//	Given a Source Mob, return another Source Mob that represents the media in its previous form or generation.
//	If the Source Mob describes the original source then return S_FALSE.
//	On entry rArgMob is presumed to be some type of SMOB.
//*********************************************************************************************************************
HRESULT CContainerLayer09::BlopFindPrevGenMobRev1(BENTO_BLOP& rArgMob, PDWORD pdwSMOB)
{
	OMF_MOB_ID			mobID		= {0};
	POMFOO_OBJREF_ARRAY	pSlotList	= NULL;
	HRESULT				hr			= (-1);
	DWORD				dwSCLP		= 0;
	DWORD				dwTRAK		= 0;
	DWORD				dwCPNT		= 0;
	DWORD				dwSMOB		= 0;
	DWORD				dwRefList	= 0;
	WORD				wTrackID	= 0;
	WORD				wLabelNum	= 0;

	if (IsBadWritePointer(pdwSMOB, sizeof(DWORD)))
		return E_POINTER;

	// Wipe return value.
	*pdwSMOB = 0;

	// "MOBJ"
	if ((rArgMob.dwFourCC != FCC('SMOB'))&&(rArgMob.dwFourCC != FCC('MOBJ')))
	{
		return OMF_E_CLASS_SURPRISE;
	}

	// OMFI:MOBJ:MobID
	if (FAILED(hr = CoreReadMobID(rArgMob, m_dwPropMobjMobID, &mobID)))
	{
		goto L_CleanUp;
	}

	// Find the associated source clip with the matching mob id.
	dwSCLP = FindSclpFromMobID(mobID);
	if (0 == dwSCLP)
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_CleanUp;
	}

	// OMFI:SCLP:SourceTrack
	if (FAILED(hr = OrdReadUInt16(dwSCLP, ePropSclpSourceTrack, &wTrackID)))
	{
		goto L_CleanUp;
	}

	// OMFI:TRKG:Tracks
	if (FAILED(hr = RordAllocObjRefArray(rArgMob, ePropTrkgTracks, &pSlotList)))
	{
		goto L_CleanUp;
	}

	for (UINT i = 0; i < pSlotList->nElements; i++)
	{
		// OMFI:TRAK:LabelNumber
		if (SUCCEEDED(RordReadUInt16(GetBlop(pSlotList->a[i]), ePropTrakLabelNumber, &wLabelNum)))
		{
			if (wLabelNum == wTrackID)
			{
				dwTRAK = pSlotList->a[i];
				break;
			}
		}
	}

	if (0 == dwTRAK)
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_CleanUp;
	}

	// OMFI:TRAK:TrackComponent
	if (FAILED(hr = RordReadObjRef(GetBlop(dwTRAK), ePropTrakTrackComponent, &dwRefList, &dwCPNT)))
	{
		goto L_CleanUp;
	}

	// OMFI:SCLP:SourceID
	if (FAILED(hr = RordReadMobID(GetBlop(dwCPNT), ePropSclpSourceID, &mobID)))
	{
		// This property is optional. If this property is absent then be sure dwCPNT is an SCLP.
		if (S_OK == IsBlopATypeOf(GetBlop(dwCPNT), FCC('SCLP')))
		{
			// If dwCPNT is an SCLP and the OMFI:SCLP:SourceID property is absent, then it's because
			// "this mob describes the original source."
			hr = S_FALSE;
		}
		goto L_CleanUp;
	}

	// Test for special case mobID.
	// "The Source Clip specifies the MobID of the previous generation of media."
	// "If the value is 0-0-0 it means that this mob describes the original source."
	if ((0 == mobID.dwMinor) && (0 == mobID.dwMajor) && (0 == mobID.dwPrefix))
	{
		// Caller's dwArgMob describes the original source. Not an error. This is common. This is normal.
		hr = S_FALSE;
		goto L_CleanUp;
	}

	// Find the mob with the matching mob id.
	dwSMOB = FindMobjFromMobID(mobID);
	if (0 == dwSMOB)
	{
		hr = OMF_E_MOB_NOT_FOUND;
		goto L_CleanUp;
	}

	// Test for unexpected recursion. This is pure paranoia. I have never seen this happen.
	if (rArgMob.dwObject == dwSMOB)
	{
		hr = E_UNEXPECTED;
		goto L_CleanUp;
	}

	// Put SMOB where caller can find it. 
	*pdwSMOB = dwSMOB;

	// Success!
	hr = S_OK;

L_CleanUp:
	CoreFreeObjRefArray(&pSlotList);
	return hr;
}

//*********************************************************************************************************************
//	Private helper for BlopFindPreviousGenerationMob().
//	Given a Source Mob, return another Source Mob that represents the media in its previous form or generation.
//	If the Source Mob describes the original source then return S_FALSE.
//	On entry rArgMob is presumed to be some type of SMOB.
//*********************************************************************************************************************
HRESULT CContainerLayer09::BlopFindPrevGenMobRev2(BENTO_BLOP& rArgMob, PDWORD pdwSMOB)
{
	OMF_MOB_ID			mobID		= {0};
	POMFOO_OBJREF_ARRAY	pSlotList	= NULL;
	HRESULT				hr			= (-1);
	DWORD				dwSCLP		= 0;
	DWORD				dwMSLT		= 0;
	DWORD				dwTRKD		= 0;
	DWORD				dwSEGM		= 0;
	DWORD				dwSMOB		= 0;
	DWORD				dwRefList	= 0;
	UINT32				u32TrackID	= 0;
	UINT32				u32			= 0;

	if (IsBadWritePointer(pdwSMOB, sizeof(DWORD)))
		return E_POINTER;

	// Wipe return value.
	*pdwSMOB = 0;

	// "SMOB"
	if (rArgMob.dwFourCC != FCC('SMOB'))
	{
		return OMF_E_CLASS_SURPRISE;
	}

	// OMFI:MOBJ:MobID
	if (FAILED(hr = CoreReadMobID(rArgMob, m_dwPropMobjMobID, &mobID)))
	{
		goto L_CleanUp;
	}

	// Find the associated source clip with the matching mob id.
	dwSCLP = FindSclpFromMobID(mobID);
	if (0 == dwSCLP)
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_CleanUp;
	}

	// OMFI:SCLP:SourceTrackID
	if (FAILED(hr = RordReadUInt32(GetBlop(dwSCLP), ePropSclpSourceTrackID, &u32TrackID)))
	{
		goto L_CleanUp;
	}

	// OMFI:MOBJ:Slots
	if (FAILED(hr = RordAllocObjRefArray(rArgMob, ePropMobjSlots, &pSlotList)))
	{
		goto L_CleanUp;
	}

	for (UINT i = 0; i < pSlotList->nElements; i++)
	{
		// OMFI:MSLT:TrackDesc
		if (FAILED(RordReadObjRef(GetBlop(pSlotList->a[i]), ePropMsltTrackDesc, &dwRefList, &dwTRKD)))
			continue;

		// OMFI:TRKD:TrackID
		if (FAILED(RordReadUInt32(GetBlop(dwTRKD), ePropTrkdTrackID, &u32)))
			continue;

		if (u32TrackID == u32)
		{
			dwMSLT = pSlotList->a[i];
			break;
		}
	}

	if (0 == dwMSLT)
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_CleanUp;
	}

	// OMFI:MSLT:Segment
	if (FAILED(hr = RordReadObjRef(GetBlop(dwMSLT), ePropMsltSegment, &dwRefList, &dwSEGM)))
	{
		goto L_CleanUp;
	}

	// OMFI:OOBJ:ObjClass
	if (FCC('SCLP') != InternalGetStandardClassFourCC(GetBlop(dwSEGM).dwFourCC))
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_CleanUp;
	}

	// OMFI:SCLP:SourceID
	if (FAILED(hr = RordReadMobID(GetBlop(dwSEGM), ePropSclpSourceID, &mobID)))
	{
		// This property is optional. If this property is absent then be sure dwSEGM is an SCLP.
		if (S_OK == IsBlopATypeOf(GetBlop(dwSEGM), FCC('SCLP')))
		{
			// If dwCPNT is an SCLP and the OMFI:SCLP:SourceID property is absent, then it's because
			// "this mob describes the original source."
			hr = S_FALSE;
		}
		goto L_CleanUp;
	}

	// Test for special case mobID.
	// "The Source Clip specifies the MobID of the previous generation of media."
	// "If the value is 0-0-0 it means that this mob describes the original source."
	if ((0 == mobID.dwMinor) && (0 == mobID.dwMajor) && (0 == mobID.dwPrefix))
	{
		// Caller's rArgMob describes the original source. Not an error. This is common. This is normal.
		hr = S_FALSE;
		goto L_CleanUp;
	}

	// Find the mob with the matching mob id.
	dwSMOB = FindMobjFromMobID(mobID);
	if (0 == dwSMOB)
	{
		hr = OMF_E_MOB_NOT_FOUND;
		goto L_CleanUp;
	}

	// Test for unexpected recursion. This is pure paranoia. I have never seen this happen.
	if (rArgMob.dwObject == dwSMOB)
	{
		hr = E_UNEXPECTED;
		goto L_CleanUp;
	}

	// Put SMOB where caller can find it. 
	*pdwSMOB = dwSMOB;

	// Success!
	hr = S_OK;

L_CleanUp:
	CoreFreeObjRefArray(&pSlotList);
	return hr;
}
