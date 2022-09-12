// Indent=4. Tab=4. ColumnWidth=120. CR/LF. Codepage=ASCII.
// Original filename: OmfControlPoint.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfControlPoint.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for OMF Control Point Class objects (CTLP).
//*********************************************************************************************************************
COmfObject* COmfControlPoint::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfControlPoint(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfControlPoint::COmfControlPoint(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
	, m_pProperties(NULL)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfControlPoint::~COmfControlPoint(void)
{
	m_pContainer->CoreFreeObjRefArray(&m_pProperties);
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfControlPoint::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfControlPoint))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfControlPoint*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetDataKind(PWORD pwDataKind)
{
	// OMFI:CTLP:DataKind
	return OrdGetDataKind(ePropCtlpDataKind, pwDataKind);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetDataKindIDString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:CTLP:DataKind
	return OrdGetDataKindAsStringW(ePropCtlpDataKind, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetEditHintType(PWORD pwHintType)
{
	// OMFI:CTLP:EditHint
	return OrdReadUInt16(ePropCtlpEditHint, pwHintType);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//	OMFI:CTLP:Time
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetTime(POMF_RATIONAL pRational)
{
	return OrdReadRational(ePropCtlpTime, pRational);
}

HRESULT COmfControlPoint::GetTimeAsFloat(PFLOAT pFloat)
{
	return OrdReadRationalAsFloat(ePropCtlpTime, pFloat);
}

HRESULT COmfControlPoint::GetTimeAsDouble(DOUBLE* pDouble)
{
	return OrdReadRationalAsDouble(ePropCtlpTime, pDouble);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::IteratePointProperties(IOmfooIterator **ppIterator)
{
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pProperties)
	{
		// OMFI:CTLP:PointProperties
		hr = OrdAllocObjRefArray(ePropCtlpPointProperties, &m_pProperties);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pProperties, ppIterator);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:CTLP:Value
	return OrdReadStringW(ePropCtlpValue, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsByte(PBYTE pb)
{
	// OMFI:CTLP:Value
	return OrdReadUInt8(ePropCtlpValue, PUINT8(pb));
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsInt16(PINT16 pn16)
{
	// OMFI:CTLP:Value
	return OrdReadInt16(ePropCtlpValue, pn16);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsInt32(PINT32 pn32)
{
	// OMFI:CTLP:Value
	return OrdReadInt32(ePropCtlpValue, pn32);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsInt64(PINT64 pn64)
{
	// OMFI:CTLP:Value
	return OrdReadInt64(ePropCtlpValue, pn64);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsUInt16(PUINT16 pu16)
{
	// OMFI:CTLP:Value
	return OrdReadUInt16(ePropCtlpValue, pu16);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsUInt32(PUINT32 pu32)
{
	// OMFI:CTLP:Value
	return OrdReadUInt32(ePropCtlpValue, pu32);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsUInt64(PUINT64 pu64)
{
	// OMFI:CTLP:Value
	return OrdReadUInt64(ePropCtlpValue, pu64);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsPoint(POMF_POINT pPoint)
{
	return OrdReadPoint(ePropCtlpValue, pPoint);
}

//*********************************************************************************************************************
//	IOmfControlPoint
//*********************************************************************************************************************
HRESULT COmfControlPoint::GetValueAsRational(POMF_RATIONAL pRational)
{
	// OMFI:CTLP:Value
	return OrdReadRational(ePropCtlpValue, pRational);
}

HRESULT COmfControlPoint::GetRationalAsFloat(PFLOAT pFloat)
{
	return OrdReadRationalAsFloat(ePropCtlpValue, pFloat);
}

HRESULT COmfControlPoint::GetRationalAsDouble(__out DOUBLE* pDouble)
{
	return OrdReadRationalAsDouble(ePropCtlpValue, pDouble);
}
