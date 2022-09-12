// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfIdentity.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfIdentity.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF ???? class object (????).
//*********************************************************************************************************************
COmfObject* COmfIdentity::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfIdentity(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfIdentity::COmfIdentity(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfIdentity::~COmfIdentity(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfIdentity::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfIdentity))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfIdentity*>(this));
			return S_OK;
		}

		hr = __super::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetCompanyName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:IDNT:CompanyName
	return OrdReadStringW(ePropIdntCompanyName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetProductName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:IDNT:ProductName
	return OrdReadStringW(ePropIdntProductName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetProductVersion(POMF_PRODUCT_VERSION pAppVersion)
{
	// OMFI:IDNT:ProductVersion
	return OrdReadProductVersion(ePropIdntProductVersion, pAppVersion);
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetProductVersionString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:IDNT:ProductVersionString
	return OrdReadStringW(ePropIdntProductVersionString, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetProductID(PULONG pnProductID)
{
	// OMFI:IDNT:ProductID
	return OrdReadUInt32(ePropIdntProductID, PUINT32(pnProductID));
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetDate(__out LPFILETIME pFileTime)
{
	// OMFI:IDNT:Date
	return OrdReadTimeStampAsFileTime(ePropIdntDate, pFileTime);
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetToolkitVersion(POMF_PRODUCT_VERSION pToolkitVersion)
{
	// OMFI:IDNT:ToolkitVersion
	return OrdReadProductVersion(ePropIdntToolkitVersion, pToolkitVersion);
}

//*********************************************************************************************************************
//	IOmfIdentity.
//*********************************************************************************************************************
HRESULT COmfIdentity::GetPlatform(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:IDNT:Platform
	return OrdReadStringW(ePropIdntPlatform, cchBuffer, pBuffer, pcchRequired);
}

