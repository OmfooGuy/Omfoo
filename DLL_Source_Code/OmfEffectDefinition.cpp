// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfEffectDefinition.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfEffectDefinition.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF Effect Definition Class object (EDEF).
//*********************************************************************************************************************
COmfObject* COmfEffectDefinition::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfEffectDefinition(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfEffectDefinition::COmfEffectDefinition(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfEffectDefinition::~COmfEffectDefinition(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfEffectDefinition))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfEffectDefinition*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	OMFI:EDEF:EffectID, omfi:UniqueName.
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::GetEffectIDString(ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
{
	return OrdReadStringW(ePropEdefEffectID, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Reads the OMFI:EDEF:EffectID property string and maps it to one of the OMF_EFFECT_KIND enumerations.
//	See "Omfoo_Enumerated_Types.h"
//	Returns OMF_E_EDEF_SURPRISE if there is no match for the string in omfi:UniqueName.
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::GetEffectIDWord(PWORD pwEffectID)
{
	// OMFI:EDEF:EffectID			omfi:UniqueName
	CHAR	szUniqueName[64] = {0};
	HRESULT	hr = OrdReadStringA(ePropEdefEffectID, ELEMS(szUniqueName), szUniqueName, NULL);
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->EffectIDStringToWord(szUniqueName, pwEffectID);
	}
	return hr;
}

//*********************************************************************************************************************
//	OMFI:EDEF:EffectName, omfi:String.
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::GetEffectName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return OrdReadStringW(ePropEdefEffectName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	OMFI:EDEF:EffectDescription, omfi:String.
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::GetDescription(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return OrdReadStringW(ePropEdefEffectDescription, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	OMFI:EDEF:Bypass, omfi:ArgIDType.
//	The Bypass property is intended to provide applications a way to play effects that they do not know or cannot generate.
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::GetBypassArgID(PLONG pnArgID)
{
	return OrdReadUInt32(ePropEdefBypass, PUINT32(pnArgID));
}

//*********************************************************************************************************************
//	OMFI:EDEF:IsTimeWarp, omfi:Boolean.
//*********************************************************************************************************************
HRESULT COmfEffectDefinition::IsTimeWarp(void)
{
	return OrdReadBoolean(ePropEdefIsTimeWarp, NULL);
}

