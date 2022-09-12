// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer03.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer03.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer03::CContainerLayer03(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer03::~CContainerLayer03(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer03::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//	if (SUCCEEDED(hr))
//	{
//		hr = ????????????
//	}
//	return hr;
//}

//*********************************************************************************************************************
//	Thunks to the primary data access routines in CContainerLayer01.
//	These methods all begin with the prefix 'Rstr'.
//	They accept the object ID argument as a reference to a BENTO_BLOP structure,
//	and the property ID argument as an ascii strings.
//*********************************************************************************************************************
HRESULT CContainerLayer03::RstrIsPropertyPresent(BENTO_BLOP& rBlop, LPCSTR pszPropertyName)
{
	return CoreIsPropertyPresent(rBlop, NameToPropertyID(pszPropertyName));
}

HRESULT CContainerLayer03::RstrGetPropInfo(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload)
{
	return CoreGetPropInfo(rBlop, NameToPropertyID(pszPropertyName), pdwStoredType, pdwRefList, pnFragments, pcbPayload);
}

HRESULT CContainerLayer03::RstrReadStrict(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, DWORD dwRequestedType, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(rBlop, NameToPropertyID(pszPropertyName), dwRequestedType, cbDest, pDest);
}

HRESULT CContainerLayer03::RstrReadStrictNoType(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(rBlop, NameToPropertyID(pszPropertyName), (-1), cbDest, pDest);
}

HRESULT CContainerLayer03::RstrReadRawBytes(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cb, PVOID pv, PULONG pcbRequired)
{
	return CoreReadRawBytes(rBlop, NameToPropertyID(pszPropertyName), cb, pv, pcbRequired);
}

HRESULT CContainerLayer03::RstrReadStringA(BENTO_BLOP& rBlop, LPCSTR pszPropertyName,
											ULONG cchBuffer, PCHAR pszDest, PULONG pcchRequired)
{
	return CoreReadStringA(rBlop, NameToPropertyID(pszPropertyName), cchBuffer, pszDest, pcchRequired);
}

HRESULT CContainerLayer03::RstrReadStringW(BENTO_BLOP& rBlop, LPCSTR pszPropertyName,
											ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreReadStringW(rBlop, NameToPropertyID(pszPropertyName), cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer03::RstrReadBoolean(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PBOOLEAN pBoolean)
{
	return CoreReadBoolean(rBlop, NameToPropertyID(pszPropertyName), pBoolean);
}

HRESULT CContainerLayer03::RstrReadInt8(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT8 pn8)
{
	return CoreReadInt8(rBlop, NameToPropertyID(pszPropertyName), pn8);
}

HRESULT CContainerLayer03::RstrReadInt16(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT16 pn16)
{
	return CoreReadInt16(rBlop, NameToPropertyID(pszPropertyName), pn16);
}

HRESULT CContainerLayer03::RstrReadInt32(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT32 pn32)
{
	return CoreReadInt32(rBlop, NameToPropertyID(pszPropertyName), pn32);
}

HRESULT CContainerLayer03::RstrReadInt64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT64 pn64)
{
	return CoreReadInt64(rBlop, NameToPropertyID(pszPropertyName), pn64);
}

HRESULT CContainerLayer03::RstrReadUInt8(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT8 pu8)
{
	return CoreReadUInt8(rBlop, NameToPropertyID(pszPropertyName), pu8);
}

HRESULT CContainerLayer03::RstrReadUInt16(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT16 pu16)
{
	return CoreReadUInt16(rBlop, NameToPropertyID(pszPropertyName), pu16);
}

HRESULT CContainerLayer03::RstrReadUInt32(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT32 pu32)
{
	return CoreReadUInt32(rBlop, NameToPropertyID(pszPropertyName), pu32);
}

HRESULT CContainerLayer03::RstrReadUInt64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT64 pu64)
{
	return CoreReadUInt64(rBlop, NameToPropertyID(pszPropertyName), pu64);
}

HRESULT CContainerLayer03::RstrReadLength64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT64 pLength)
{
	return CoreReadLength64(rBlop, NameToPropertyID(pszPropertyName), pLength);
}

HRESULT CContainerLayer03::RstrReadPosition64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT64 pPosition)
{
	return CoreReadPosition64(rBlop, NameToPropertyID(pszPropertyName), pPosition);
}

HRESULT CContainerLayer03::RstrReadCompCodeArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_COMP_CODE_ARRAY pArray)
{
	return CoreReadCompCodeArray(rBlop, NameToPropertyID(pszPropertyName), pArray);
}

HRESULT CContainerLayer03::RstrReadCompSizeArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_COMP_SIZE_ARRAY pArray)
{
	return CoreReadCompSizeArray(rBlop, NameToPropertyID(pszPropertyName), pArray);
}

HRESULT CContainerLayer03::RstrReadInt32Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadInt32Array(rBlop, NameToPropertyID(pszPropertyName), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer03::RstrReadInt64Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadInt64Array(rBlop, NameToPropertyID(pszPropertyName), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer03::RstrReadPosition32Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadPosition32Array(rBlop, NameToPropertyID(pszPropertyName), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer03::RstrReadPosition64Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadPosition64Array(rBlop, NameToPropertyID(pszPropertyName), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer03::RstrQueryDataValue(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_DATA_VALUE pDataValue)
{
	return CoreQueryDataValue(rBlop, NameToPropertyID(pszPropertyName), pDataValue);
}

HRESULT CContainerLayer03::RstrReadDataValueAsRawBytes(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cbDest, PVOID pDest, PULONG pcbRequired)
{
	return CoreReadDataValueAsRawBytes(rBlop, NameToPropertyID(pszPropertyName), cbDest, pDest, pcbRequired);
}

HRESULT CContainerLayer03::RstrReadGuid(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, LPGUID pGuid)
{
	return CoreReadGuid(rBlop, NameToPropertyID(pszPropertyName), pGuid);
}

HRESULT CContainerLayer03::RstrReadMobID(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_MOB_ID pMobID)
{
	return CoreReadMobID(rBlop, NameToPropertyID(pszPropertyName), pMobID);
}

HRESULT CContainerLayer03::RstrReadObjRef(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PDWORD pdwRefList, PDWORD pdwObjRef)
{
	return CoreReadObjRef(rBlop, NameToPropertyID(pszPropertyName), pdwRefList, pdwObjRef);
}

HRESULT CContainerLayer03::RstrReadPoint(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_POINT pPoint)
{
	return CoreReadPoint(rBlop, NameToPropertyID(pszPropertyName), pPoint);
}

HRESULT CContainerLayer03::RstrReadProductVersion(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_PRODUCT_VERSION pVersion)
{
	return CoreReadProductVersion(rBlop, NameToPropertyID(pszPropertyName), pVersion);
}

HRESULT CContainerLayer03::RstrReadRational(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_RATIONAL pRat)
{
	return CoreReadRational(rBlop, NameToPropertyID(pszPropertyName), pRat);
}

HRESULT CContainerLayer03::RstrReadRationalAsFloat(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PFLOAT pFloat)
{
	return CoreReadRationalAsFloat(rBlop, NameToPropertyID(pszPropertyName), pFloat);
}

HRESULT CContainerLayer03::RstrReadRationalAsDouble(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, DOUBLE* pDouble)
{
	return CoreReadRationalAsDouble(rBlop, NameToPropertyID(pszPropertyName), pDouble);
}

HRESULT CContainerLayer03::RstrReadExtended80AsDouble(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, DOUBLE* pDouble)
{
	return CoreReadExtended80AsDouble(rBlop, NameToPropertyID(pszPropertyName), pDouble);
}

HRESULT CContainerLayer03::RstrReadTimeStamp(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_TIMESTAMP pTimeStamp)
{
	return CoreReadTimeStamp(rBlop, NameToPropertyID(pszPropertyName), pTimeStamp);
}

HRESULT CContainerLayer03::RstrReadTimeStampAsFileTime(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, LPFILETIME pFileTime)
{
	return CoreReadTimeStampAsFileTime(rBlop, NameToPropertyID(pszPropertyName), pFileTime);
}

HRESULT CContainerLayer03::RstrReadVersionType(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_VERSIONTYPE pVersionType)
{
	return CoreReadVersionType(rBlop, NameToPropertyID(pszPropertyName), pVersionType);
}

HRESULT CContainerLayer03::RstrGetDataKind(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PWORD pwDataKind)
{
	return CoreGetDataKindAsWord(rBlop, NameToPropertyID(pszPropertyName), pwDataKind);
}

HRESULT CContainerLayer03::RstrGetDataKindAsStringA(BENTO_BLOP& rBlop, LPCSTR pszPropertyName,
														ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringA(rBlop, NameToPropertyID(pszPropertyName), cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer03::RstrGetDataKindAsStringW(BENTO_BLOP& rBlop, LPCSTR pszPropertyName,
														ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringW(rBlop, NameToPropertyID(pszPropertyName), cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer03::RstrExtractDataToFile(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PCWSTR pwzFullPath, BOOL fOverwrite)
{
	return CoreExtractDataToFile(rBlop, NameToPropertyID(pszPropertyName), pwzFullPath, fOverwrite);
}

HRESULT CContainerLayer03::RstrCreateStreamOnDataValue(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, REFIID riid, PVOID *ppvOut)
{
	return CoreCreateStreamOnDataValue(rBlop, NameToPropertyID(pszPropertyName), riid, ppvOut);
}

HRESULT CContainerLayer03::RstrCreateHGlobalOnDataValue(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, UINT uFlags, HGLOBAL* phGlobal)
{
	return CoreCreateHGlobalOnDataValue(rBlop, NameToPropertyID(pszPropertyName), uFlags, phGlobal);
}

// Array routines.
HRESULT CContainerLayer03::RstrGetArrayInfo(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PDWORD pdwDataType,
											PDWORD pdwRefList, PULONG pnElements, PULONG pcbData)
{
	return CoreGetArrayInfo(rBlop, NameToPropertyID(pszPropertyName), pdwDataType, pdwRefList, pnElements, pcbData);
}

HRESULT CContainerLayer03::RstrAllocMobIndexArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	return CoreAllocMobIndexArray(rBlop, NameToPropertyID(pszPropertyName), ppArray);
}

HRESULT CContainerLayer03::RstrAllocObjRefArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMFOO_OBJREF_ARRAY* ppArray)
{
	return CoreAllocObjRefArray(rBlop, NameToPropertyID(pszPropertyName), ppArray);
}

HRESULT CContainerLayer03::RstrAllocPositionArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMFOO_POSITION_ARRAY* ppArray)
{
	return CoreAllocPositionArray(rBlop, NameToPropertyID(pszPropertyName), ppArray);
}
