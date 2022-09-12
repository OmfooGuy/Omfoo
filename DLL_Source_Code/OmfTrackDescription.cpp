// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfTrackDescription.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfTrackDescription.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF Track Description Class object (TRKD).
//*********************************************************************************************************************
COmfObject* COmfTrackDescription::NewInstance(BENTO_BLOP& rBlop,
														CContainerLayer97* pContainer,
														COmfObject* pParent,
														PVOID pNewReserved)
{
	return new COmfTrackDescription(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfTrackDescription::COmfTrackDescription(BENTO_BLOP& rBlop, CContainerLayer97* pContainer,
	COmfObject* pParent, PVOID pNewReserved) : COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfTrackDescription::~COmfTrackDescription(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfTrackDescription::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfTrackDescription))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfTrackDescription*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	Retrieves the OMFI:TRKD:Origin property.
//*********************************************************************************************************************
HRESULT COmfTrackDescription::GetOrigin(__out PINT64 pOrigin)
{
	// omfi:Position32 or omfi:Position64
	return OrdReadPosition64(ePropTrkdOrigin, pOrigin);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:TRKD:TrackID property.
//*********************************************************************************************************************
HRESULT COmfTrackDescription::GetTrackID(__out PDWORD pdwTrackID)
{
	return OrdReadUInt32(ePropTrkdTrackID, PUINT32(pdwTrackID));
}

//*********************************************************************************************************************
//	Retrieves the OMFI:TRKD:PhysicalTrack property.
//*********************************************************************************************************************
HRESULT COmfTrackDescription::GetPhysicalTrack(__out PDWORD pdwPhysicalTrack)
{
	return OrdReadUInt32(ePropTrkdPhysicalTrack, PUINT32(pdwPhysicalTrack));
}

//*********************************************************************************************************************
//	Retrieves the OMFI:TRKD:TrackName property as a wide character (UTF-16) string.
//*********************************************************************************************************************
HRESULT COmfTrackDescription::GetTrackName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
{
	return OrdReadStringW(ePropTrkdTrackName, cchBuffer, pBuffer, pcchRequired);
}
