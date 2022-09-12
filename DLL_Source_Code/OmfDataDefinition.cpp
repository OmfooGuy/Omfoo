// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfDataDefinition.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfDataDefinition.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF ???? class object (????).
//*********************************************************************************************************************
COmfObject* COmfDataDefinition::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfDataDefinition(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfDataDefinition::COmfDataDefinition(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfDataDefinition::~COmfDataDefinition(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfDataDefinition::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfDataDefinition))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfDataDefinition*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfDataDefinition
//*********************************************************************************************************************
HRESULT COmfDataDefinition::GetDataKindIDString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:DDEF:DataKindID
	return OrdReadStringW(ePropDdefDataKindID, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfDataDefinition::GetDataKindIDWord(__out PWORD pwDataKindID)
{
	CHAR	szUniqueName[64] = {0};
	HRESULT	hr = OrdReadStringA(ePropDdefDataKindID, ELEMS(szUniqueName), szUniqueName, NULL);
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->DataKindIDStringToWord(szUniqueName, pwDataKindID);
	}
	return hr;
}

