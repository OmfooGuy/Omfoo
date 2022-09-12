// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMobSlot.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfMobSlot.h"
#include "BackDoors.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF ???? class object (????).
//*********************************************************************************************************************
COmfObject* COmfMobSlot::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfMobSlot(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfMobSlot::COmfMobSlot(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
	, m_dwCPNT(0)
	, m_dwTRKD(0)
	, m_hrSlotIndex(OMF_E_CANT_COMPLETE)
{
	if (m_fOmfVer1)
	{
		// OMFI:TRAK:TrackComponent
		OrdReadObjRef(ePropTrakTrackComponent, NULL, &m_dwCPNT);
	}
	else
	{
		// OMFI:MSLT:Segment
		OrdReadObjRef(ePropMsltSegment, NULL, &m_dwCPNT);

		// OMFI:MSLT:TrackDesc
		OrdReadObjRef(ePropMsltTrackDesc, NULL, &m_dwTRKD);
	}

	// Get our 
	if (pParent)
	{
		IMobjBackDoor* pBackDoor = NULL;
		if (SUCCEEDED(pParent->QueryInterface(IID_PPV_ARGS(&pBackDoor))))
		{
			m_hrSlotIndex = pBackDoor->GetSlotIndex(this->dwObject);
			pBackDoor->Release();
			pBackDoor = NULL;
		}
	}
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfMobSlot::~COmfMobSlot(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfMobSlot::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfMobSlot))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfMobSlot*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMF class fourCC of the ObjRef specified in the OMFI:TRAK:TrackComponent property.
//	OMF2 - Retrieves the OMF class fourCC of the ObjRef specified in the OMFI:MSLT:Segment property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetSegmentClass(__out PDWORD pdwFourCC)
{
	if (IsBadWritePointer(pdwFourCC, sizeof(DWORD)))
	{
		return E_POINTER;
	}
	else
	{
		*pdwFourCC = m_pContainer->GetObjectClassFourCC(m_dwCPNT);
		return S_OK;
	}
}

//*********************************************************************************************************************
//	OMF1 - Instantiates the ObjRef specified in the OMFI:TRAK:TrackComponent property.
//	OMF2 - Instantiates the ObjRef specified in the OMFI:MSLT:Segment property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetSegment(__in REFIID riid, __out PVOID *ppvOut)
{
	return m_pContainer->Instantiate(m_dwCPNT, this, NULL, riid, ppvOut);
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the track component's OMFI:CPNT:EditRate property.
//	OMF2 - Retrieves the OMFI:MSLT:EditRate property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetEditRate(POMF_RATIONAL pEditRate)
{
	if (m_fOmfVer1)
	{
		// OMFI:CPNT:EditRate
		return m_pContainer->OrdReadRational(m_dwCPNT, ePropCpntEditRate, pEditRate);
	}
	else
	{
		// OMFI:MSLT:EditRate
		return OrdReadRational(ePropMsltEditRate, pEditRate);
	}
}

// Same as above except FLOAT.
HRESULT COmfMobSlot::GetEditRateAsFloat(PFLOAT pFloat)
{
	if (m_fOmfVer1)
	{
		// OMFI:CPNT:EditRate
		return m_pContainer->OrdReadRationalAsFloat(m_dwCPNT, ePropCpntEditRate, pFloat);
	}
	else
	{
		// OMFI:MSLT:EditRate
		return OrdReadRationalAsFloat(ePropMsltEditRate, pFloat);
	}
}

// Same as above except DOUBLE.
HRESULT COmfMobSlot::GetEditRateAsDouble(DOUBLE* pDouble)
{
	if (m_fOmfVer1)
	{
		// OMFI:CPNT:EditRate
		return m_pContainer->OrdReadRationalAsDouble(m_dwCPNT, ePropCpntEditRate, pDouble);
	}
	else
	{
		// OMFI:MSLT:EditRate
		return OrdReadRationalAsDouble(ePropMsltEditRate, pDouble);
	}
}

//*********************************************************************************************************************
//	OMF1 - Returns OMF_E_NOT_IN_OMF1.
//	OMF2 - Instantiates the ObjRef specified in the OMFI:MSLT:TrackDesc property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetTrackDesc(__in REFIID riid, __out PVOID *ppvOut)
{
	if (m_fOmfVer1)
	{
		return OMF_E_NOT_IN_OMF1;
	}
	else
	{
		return m_pContainer->Instantiate(m_dwTRKD, this, NULL, riid, ppvOut);
	}
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:MOBJ:StartPosition property of the mob that owns the track.
//	OMF2 - Retrieves the track descriptor's OMFI:TRKD:Origin property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetOrigin(__out PINT64 pOrigin)
{
	if (m_fOmfVer1)
	{
		// If we were instantiated from a MOBJ then we can get our origin like this.
		if (m_pParent)
		{
			// OMFI:MOBJ:StartPosition
			// omfi:Long
			return m_pParent->OrdReadPosition64(ePropMobjStartPosition, pOrigin);
		}
		else
		{
			// If we weren't instantiated from a MOBJ then we have to easy way of knowing who our owner is.
			// This will happen, for example, by calling pOmfooReader->IterateObjects(FCC('TRKD'), FALSE, &pIterator);
			return OMF_E_CANT_COMPLETE;
		}
	}
	else
	{
		// OMFI:TRKD:Origin
		// omfi:Position32 or omfi:Position64
		return m_pContainer->OrdReadPosition64(m_dwTRKD, ePropTrkdOrigin, pOrigin);
	}
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:TRAK:LabelNumber property.
//	OMF2 - Retrieves the track descriptor's OMFI:TRKD:TrackID property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetTrackID(__out PDWORD pdwTrackID)
{
	if (m_fOmfVer1)
	{
		if (IsBadWritePointer(pdwTrackID, sizeof(DWORD)))
			return E_POINTER;

		// OMFI:TRAK:LabelNumber
		UINT16 wLabelNumber	= 0;
		HRESULT hr = OrdReadUInt16(ePropTrakLabelNumber, &wLabelNumber);
		*pdwTrackID	= DWORD(wLabelNumber);
		return hr;
	}
	else
	{
		// OMFI:TRKD:TrackID
		return m_pContainer->OrdReadUInt32(m_dwTRKD, ePropTrkdTrackID, PUINT32(pdwTrackID));
	}
}

//*********************************************************************************************************************
//	OMF1 - Returns the TRAK's one-based position in the owning mob's OMFI:TRKG:Tracks array.
//	OMF2 - Returns the MSLT's one-base position in the owning mob's OMFI:MOBJ:Slots array.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetCompatibleTrackID(__out PDWORD pdwTrackID)
{
	if (IsBadWritePointer(pdwTrackID, sizeof(DWORD)))
		return E_POINTER;

	if (SUCCEEDED(m_hrSlotIndex))
	{
		*pdwTrackID = m_hrSlotIndex+1;	// return value is one-based!
		return S_OK;
	}
	else
	{
		*pdwTrackID = 0;
		return m_hrSlotIndex;
	}
}

//*********************************************************************************************************************
//	OMF1 - Returns OMF_E_NOT_IN_OMF1.
//	OMF2 - Retrieves the slot's name from the track descriptor's OMFI:TRKD:TrackName property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetTrackName(__in ULONG cchBuffer, __out_opt LPWSTR pBuffer, __out_opt PULONG pcchRequired)
{
	// Only available in OMF2.
	if (m_fOmfVer1)
	{
		return OMF_E_NOT_IN_OMF1;
	}
	else
	{
		// OMFI:TRKD:TrackName
		return m_pContainer->OrdReadStringW(m_dwTRKD, ePropTrkdTrackName, cchBuffer, pBuffer, pcchRequired);
	}
}

//*********************************************************************************************************************
//	OMF1 - Returns OMF_E_NOT_IN_OMF1.
//	OMF2 - Retrieves the track descriptor's OMFI:TRKD:PhysicalTrack property.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetPhysicalTrack(__out PDWORD pdwPhysicalTrack)
{
	// Only available in OMF2.
	if (m_fOmfVer1)
	{
		return OMF_E_NOT_IN_OMF1;
	}
	else
	{
		// OMFI:TRKD:PhysicalTrack
		return m_pContainer->OrdReadUInt32(m_dwTRKD, ePropTrkdPhysicalTrack, PUINT32(pdwPhysicalTrack));
	}
}

//*********************************************************************************************************************
//	OMF1 - Instantiates the ObjRef specified in the OMFI:TRAK:FillerProxy property.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2.
//
//	"The OMFI:TRAK:FillerProxy property has been replaced with Nested Scope and Scope References.
//	The Filler class is not used for implicit references."
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetFillerProxy(__in REFIID riid, __out PVOID *ppvOut)
{
	if (m_fOmfVer1)
	{
		// OMFI:TRAK:FillerProxy
		return OrdInstantiateObjRef(ePropTrakFillerProxy, riid, ppvOut);
	}
	else
	{
		return OMF_E_NOT_IN_OMF2;
	}
}

//*********************************************************************************************************************
//	OMF1 - Instantiates the ObjRef specified in the OMFI:TRAK:Attributes property.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetTrackAttributeList(__in REFIID riid, __out PVOID *ppvOut)
{
	if (m_fOmfVer1)
	{
		// OMFI:TRAK:Attributes
		return OrdInstantiateObjRef(ePropTrakAttributes, riid, ppvOut);
	}
	else
	{
		return OMF_E_NOT_IN_OMF2;
	}
}

//*********************************************************************************************************************
//	OMF1 - Instantiates the ObjRef specified in the OMFI:TRAK:SessionAttrs property.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2.
//*********************************************************************************************************************
HRESULT COmfMobSlot::GetSessionAttributeList(__in REFIID riid, __out PVOID *ppvOut)
{
	if (m_fOmfVer1)
	{
		// OMFI:TRAK:SessionAttrs
		return OrdInstantiateObjRef(ePropTrakSessionAttrs, riid, ppvOut);
	}
	else
	{
		return OMF_E_NOT_IN_OMF2;
	}
}

