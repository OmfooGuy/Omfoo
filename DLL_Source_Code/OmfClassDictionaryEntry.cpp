// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfClassDictionaryEntry.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfClassDictionaryEntry.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF Class Dictionary Entry class object (CLSD).
//*********************************************************************************************************************
COmfObject* COmfClassDictionaryEntry::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfClassDictionaryEntry(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfClassDictionaryEntry::COmfClassDictionaryEntry(BENTO_BLOP& rBlop, CContainerLayer97* pContainer,
	COmfObject* pParent, PVOID pNewReserved) : COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfClassDictionaryEntry::~COmfClassDictionaryEntry(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfClassDictionaryEntry::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfClassDictionaryEntry))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfClassDictionaryEntry*>(this));
			return S_OK;
		}

		hr = __super::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:CLSD:ClassID property as a 32-bit DWORD.
//	OMF2 - Retrieves the OMFI:CLSD:Class property as a 32-bit DWORD.
//*********************************************************************************************************************
HRESULT COmfClassDictionaryEntry::GetClassFourCC(PDWORD pdwFourCC)
{
	return OrdReadUInt32(m_fOmfVer1 ? ePropClsdClassID : ePropClsdClass, PUINT32(pdwFourCC));
}

//*********************************************************************************************************************
//	OMF1 & OMF2 - Instantiates the parent CLSD object and then queries it for the interface specified by riid.
//*********************************************************************************************************************
HRESULT COmfClassDictionaryEntry::GetParentEntry(REFIID riid, PVOID *ppvOut)
{
	// OMFI:CLSD:ParentClass
	return OrdInstantiateObjRef(ePropClsdParentClass, riid, ppvOut);
}
