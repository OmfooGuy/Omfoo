// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfAttribute.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfAttribute.h"
#include "DllMain.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF Attribute class object (ATTB).
//*********************************************************************************************************************
COmfObject* COmfAttribute::NewInstance(BENTO_BLOP& rBlop,
													CContainerLayer97* pContainer,
													COmfObject* pParent,
													PVOID pNewReserved)
{
	return new COmfAttribute(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfAttribute::COmfAttribute(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
	, m_wAttbKind(0)
{
	// OMFI:ATTB:Kind
	// This might not exist in some early OMF1 files.
	OrdReadInt16(ePropAttbKind, PINT16(&m_wAttbKind));
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfAttribute::~COmfAttribute(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfAttribute::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfAttribute))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfAttribute*>(this));
			return S_OK;
		}

		hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:Name property.
//*********************************************************************************************************************
HRESULT COmfAttribute::GetAttributeNameA(ULONG cchBuffer, LPSTR pszDest, PULONG pcchRequired)
{
	return OrdReadStringA(ePropAttbName, cchBuffer, pszDest, pcchRequired);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:Name property.
//*********************************************************************************************************************
HRESULT COmfAttribute::GetAttributeNameW(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return OrdReadStringW(ePropAttbName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:Kind property.
//	This can be one of AK_NULL, AK_INT32, AK_STRING, AK_OBJECT, AK_DATAVALUE, or AK_BOBDATA.
//*********************************************************************************************************************
HRESULT COmfAttribute::GetAttributeKind(PWORD pwAttributeKind)
{
	return OrdReadUInt16(ePropAttbKind, pwAttributeKind);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:IntAttribute property when the OMFI:ATTB:Kind property is AK_INT32.
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadIntAttribute(PINT32 pn32)
{
	return OrdReadInt32(ePropAttbIntAttribute, pn32);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:StringAttribute property when the OMFI:ATTB:Kind property is AK_STRING.
//	This version returns the string exactly as it is stored in the payload.
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadStringAttributeA(ULONG cchBuffer, LPSTR pszDest, PULONG pcchRequired)
{
	return OrdReadStringA(ePropAttbStringAttribute, cchBuffer, pszDest, pcchRequired);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:StringAttribute property when the OMFI:ATTB:Kind property is AK_STRING.
//	This version calls MultiByteToWideChar() using the working code page.
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadStringAttributeW(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return OrdReadStringW(ePropAttbStringAttribute, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Instantiates the OMFI:ATTB:ObjAttribute object when the OMFI:ATTB:Kind property is AK_OBJECT.
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadObjAttribute(REFIID riid, PVOID *ppvOut)
{
	return OrdInstantiateObjRef(ePropAttbObjAttribute, riid, ppvOut);
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:BobSize property when the OMFI:ATTB:Kind property is AK_DATAVALUE.
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadBobSizeAttribute(PULONG pcbBobSize)
{
	return OrdReadUInt32(ePropAttbBobSize, PUINT32(pcbBobSize));
}

//*********************************************************************************************************************
//	Reads and decodes the OMFI:ATTB:BobData property as a null-terminated wide character (UTF-16LE) string.
//	The OMFI:ATTB:Kind must be AK_NULL (0) or AK_DATAVALUE (4).
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
//	Returns OMF_E_BAD_TEXT if the payload is not text or the character set is undecipherable
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadBobDataAsString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:ATTB:BobData
	return OrdReadDataValueAsStringW(ePropAttbBobData, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Fills caller's buffer with the payload of the the OMFI:ATTB:BobData or OMFI:ATTB:DataAttribute
//	properties, or any other property whose OMFI:ATTB:Kind is AK_NULL (0) or AK_DATAVALUE (4).
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
//*********************************************************************************************************************
HRESULT COmfAttribute::ReadBobDataAsRawBytes(ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired)
{
	// OMFI:ATTB:BobData
	if (S_OK == OrdIsPropertyPresent(ePropAttbBobData))
	{
		return m_pContainer->RordReadDataValueAsRawBytes(*this, ePropAttbBobData, cbBuffer, pBuffer, pcbRequired);
	}

	// OMFI:ATTB:DataAttribute
	if (S_OK == OrdIsPropertyPresent(ePropAttbDataAttribute))
	{
		return m_pContainer->RordReadDataValueAsRawBytes(*this, ePropAttbDataAttribute, cbBuffer, pBuffer, pcbRequired);
	}

	// Catch-all handler for any other attribute whose OMFI:ATTB:Kind is AK_DATAVALUE (4).
	if (m_wAttbKind == AK_DATAVALUE)
	{
		// Scan our BENTO_BLOP for the first property having a omfi:DataValue or omfi:VarLenBytes data type.
		DWORD dwTargetProp = m_pContainer->FindFirstDataValue(*this);
		if (dwTargetProp)
		{
			return m_pContainer->CoreReadDataValueAsRawBytes(*this, dwTargetProp, cbBuffer, pBuffer, pcbRequired);
		}
	}

	// The specified object doesn't have a property with the specific type.
	return OMF_E_PROP_NOT_FOUND;
}

//*********************************************************************************************************************
//	Retrieves the OMFI:ATTB:BobData and OMFI:ATTB:DataAttribute properties, and any other undocumented properties
//	whose OMFI:ATTB:Kind is AK_DATAVALUE (4). The OMF data type must be omfi:DataValue or omfi:VarLenBytes.
//	The uFlags argument is passed directly to GlobalAlloc(). If zero is specified the default is GMEM_FIXED.
//	If this method succeeds the HGLOBAL it returns must be freed by calling the Win32 API function GlobalFree().
//*********************************************************************************************************************
HRESULT COmfAttribute::CreateHGlobalOnDataValue(__in UINT uFlags, __out HGLOBAL* phGlobal)
{
	// OMFI:ATTB:BobData
	if (S_OK == OrdIsPropertyPresent(ePropAttbBobData))
	{
		return m_pContainer->RordCreateHGlobalOnDataValue(*this, ePropAttbBobData, uFlags, phGlobal);
	}

	// OMFI:ATTB:DataAttribute
	if (S_OK == OrdIsPropertyPresent(ePropAttbDataAttribute))
	{
		return m_pContainer->RordCreateHGlobalOnDataValue(*this, ePropAttbDataAttribute, uFlags, phGlobal);
	}

	// Catch-all handler for any other attribute whose OMFI:ATTB:Kind is AK_DATAVALUE (4).
	if (m_wAttbKind == AK_DATAVALUE)
	{
		// Scan our BENTO_BLOP for the first property having a omfi:DataValue or omfi:VarLenBytes data type.
		DWORD dwTargetProp = m_pContainer->FindFirstDataValue(*this);
		if (dwTargetProp)
		{
			return m_pContainer->CoreCreateHGlobalOnDataValue(*this, dwTargetProp, uFlags, phGlobal);
		}
	}

	// The specified object doesn't have a property with the specific type.
	return OMF_E_PROP_NOT_FOUND;
}

//*********************************************************************************************************************
//	Creates a standard Windows Stream object on the OMFI:ATTB:BobData and OMFI:ATTB:DataAttribute properties,
//	and any other properties whose OMFI:ATTB:Kind is AK_DATAVALUE (4).
//	The data type must be omfi:DataValue or omfi:VarLenBytes.
//*********************************************************************************************************************
HRESULT COmfAttribute::CreateStreamOnDataValue(REFIID riid, PVOID *ppvOut)
{
	// OMFI:ATTB:BobData
	if (S_OK == OrdIsPropertyPresent(ePropAttbBobData))
	{
		return m_pContainer->RordCreateStreamOnDataValue(*this, ePropAttbBobData, riid, ppvOut);
	}

	// OMFI:ATTB:DataAttribute
	if (S_OK == OrdIsPropertyPresent(ePropAttbDataAttribute))
	{
		return m_pContainer->RordCreateStreamOnDataValue(*this, ePropAttbDataAttribute, riid, ppvOut);
	}

	// Catch-all handler for any other attribute whose OMFI:ATTB:Kind is AK_DATAVALUE (4).
	if (m_wAttbKind == AK_DATAVALUE)
	{
		// Scan our BENTO_BLOP for the first property having a omfi:DataValue or omfi:VarLenBytes data type.
		DWORD dwTargetProp = m_pContainer->FindFirstDataValue(*this);
		if (dwTargetProp)
		{
			return m_pContainer->CoreCreateStreamOnDataValue(*this, dwTargetProp, riid, ppvOut);
		}
	}

	// The specified object doesn't have a property with the specific type.
	return OMF_E_PROP_NOT_FOUND;
}

