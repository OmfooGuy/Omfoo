// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfAttributeArray.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfAttributeArray.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF Attribute Array class object (ATTR).
//*********************************************************************************************************************
COmfObject* COmfAttributeArray::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfAttributeArray(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfAttributeArray::COmfAttributeArray(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
	, m_pAttrArray(NULL)
	, m_hrAttrArray(E_UNEXPECTED)
{
	// OMFI:ATTR:AttrRefs
	m_hrAttrArray = OrdAllocObjRefArray(ePropAttrAttrRefs, &m_pAttrArray);
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfAttributeArray::~COmfAttributeArray(void)
{
	FreeObjRefArray(&m_pAttrArray);
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfAttributeArray::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfAttributeArray))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfAttributeArray*>(this));
			return S_OK;
		}

		hr = __super::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	Searches the OMFI:ATTR:AttrRefs array for an attribute by name and instantiates a wrapper on it, if it exists.
//*********************************************************************************************************************
HRESULT COmfAttributeArray::FindAttbByName(__in LPCSTR pszAttributeName, __in REFIID riid, __out PVOID *ppvOut)
{
	HRESULT	hr = m_hrAttrArray;
	if (SUCCEEDED(hr))
	{
		DWORD	dwATTB	= 0;
		WORD	wKind	= 0;	// unused
		if (SUCCEEDED(hr = m_pContainer->CoreFindAttributeByName(m_pAttrArray, pszAttributeName, FALSE, dwATTB, wKind)))
		{
			hr = m_pContainer->Instantiate(dwATTB, this, NULL, riid, ppvOut);
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	Instantiates an iterator to examine members of the OMFI:ATTR:AttrRefs array.
//*********************************************************************************************************************
HRESULT COmfAttributeArray::IterateMembers(__out IOmfooIterator **ppIterator)
{
	if (m_pAttrArray)
	{
		return WrapObjRefArray(m_pAttrArray, ppIterator);
	}
	else
	{
		return m_hrAttrArray;
	}
}

//*********************************************************************************************************************
//	Searches the array for an attribute by name and retrieves its OMFI:ATTB:IntAttribute property if it exists.
//	The attribute's OMFI:ATTB:Kind property must be AK_INT32 or this routine will return MOM_E_UNEXPECTED_TYPE.
//*********************************************************************************************************************
HRESULT COmfAttributeArray::FindIntAttributeByName(__in LPCSTR pszAttributeName, __out PINT32 pn32)
{
	HRESULT hr = m_hrAttrArray;
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->CoreFindIntAttributeByName(m_pAttrArray, pszAttributeName, 0, pn32);
	}
	return hr;
}

//*********************************************************************************************************************
//	Searches the array for an attribute by name and retrieves its OMFI:ATTB:StringAttribute property if it exists.
//	The attribute's OMFI:ATTB:Kind property must be AK_STRING or this routine will return MOM_E_UNEXPECTED_TYPE.
//	The ANSI version returns the string exactly as it is stored in the payload.
//	The Wide Character version calls MultiByteToWideChar() on the payload, and then returns the result to the caller.
//*********************************************************************************************************************
HRESULT COmfAttributeArray::FindStringAttributeByNameA(__in LPCSTR pszAttributeName, __in ULONG cchBuffer,
														__out_opt LPSTR pszDest, __out_opt PULONG pcchRequired)
{
	HRESULT hr = m_hrAttrArray;
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->CoreFindStringAttributeByNameA(m_pAttrArray, pszAttributeName, 0, cchBuffer, pszDest, pcchRequired);
	}
	return hr;
}

HRESULT COmfAttributeArray::FindStringAttributeByNameW(__in LPCSTR pszAttributeName, __in ULONG cchBuffer,
														__out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
{
	HRESULT hr = m_hrAttrArray;
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->CoreFindStringAttributeByNameW(m_pAttrArray, pszAttributeName, 0, cchBuffer, pBuffer, pcchRequired);
	}
	return hr;
}

//*********************************************************************************************************************
//	Searches the array for an attribute by name and instantiates a wrapper on its OMFI:ATTB:ObjAttribute property
//	(if it exists). Then it queries the wrapper for the interface specified by riid.
//	Returns E_NOINTERFACE if the wrapper does not expose the requested interface.
//	The attribute's OMFI:ATTB:Kind property must be AK_OBJECT or this routine will return MOM_E_UNEXPECTED_TYPE.
//*********************************************************************************************************************
HRESULT COmfAttributeArray::FindObjectAttributeByName(__in LPCSTR pszAttributeName, __in REFIID riid, __out PVOID *ppvOut)
{
	HRESULT	hr = m_hrAttrArray;
	if (SUCCEEDED(hr))
	{
		DWORD dwObject = 0;
		if (SUCCEEDED(hr = m_pContainer->CoreFindObjectAttributeByName(m_pAttrArray, pszAttributeName, 0, &dwObject)))
		{
			hr = m_pContainer->Instantiate(dwObject, this, NULL, riid, ppvOut);
		}
	}
	return hr;
}
