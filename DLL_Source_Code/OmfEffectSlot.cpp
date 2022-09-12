// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfEffectSlot.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfEffectSlot.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF ???? class object (????).
//*********************************************************************************************************************
COmfObject* COmfEffectSlot::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfEffectSlot(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfEffectSlot::COmfEffectSlot(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfEffectSlot::~COmfEffectSlot(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfEffectSlot::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfEffectSlot))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfEffectSlot*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfEffectSlot
//*********************************************************************************************************************
HRESULT COmfEffectSlot::GetArgID(PDWORD pdwArgID)
{
	// OMFI:ESLT:ArgID
	return OrdReadUInt32(ePropEsltArgID, PUINT32(pdwArgID));
}

//*********************************************************************************************************************
//	IOmfEffectSlot
//*********************************************************************************************************************
HRESULT COmfEffectSlot::GetArgValue(REFIID riid, PVOID *ppvOut)
{
	// MFI:ESLT:ArgValue
	return OrdInstantiateObjRef(ePropEsltArgValue, riid, ppvOut);
}
