// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer05.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer05.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer05::CContainerLayer05(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer05::~CContainerLayer05(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer05::Load(PCWSTR pwzFileName)
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
//	These methods all begin with the prefix 'Obj'.
//	They accept the object ID argument as DWORD (the actual 32-bit value in the Bento TOC),
//	and the property ID argument as a DWORD.
//*********************************************************************************************************************
HRESULT CContainerLayer05::ObjIsPropertyPresent(DWORD dwObject, DWORD dwProperty)
{
	return CoreIsPropertyPresent(GetBlop(dwObject), dwProperty);
}

HRESULT CContainerLayer05::ObjGetPropInfo(DWORD dwObject, DWORD dwProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload)
{
	return CoreGetPropInfo(GetBlop(dwObject), dwProperty, pdwStoredType, pdwRefList, pnFragments, pcbPayload);
}

HRESULT CContainerLayer05::ObjReadStrict(DWORD dwObject, DWORD dwProperty, DWORD dwRequestedType, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(GetBlop(dwObject), dwProperty, dwRequestedType, cbDest, pDest);
}

HRESULT CContainerLayer05::ObjReadStrictNoType(DWORD dwObject, DWORD dwProperty, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(GetBlop(dwObject), dwProperty, (-1), cbDest, pDest);
}

HRESULT CContainerLayer05::ObjReadRawBytes(DWORD dwObject, DWORD dwProperty, ULONG cbDest, PVOID pDest, PULONG pbRequired)
{
	return CoreReadRawBytes(GetBlop(dwObject), dwProperty, cbDest, pDest, pbRequired);
}

HRESULT CContainerLayer05::ObjReadStringA(DWORD dwObject, DWORD dwProperty,
											ULONG cchBuffer, PCHAR pszDest, PULONG pcchRequired)
{
	return CoreReadStringA(GetBlop(dwObject), dwProperty, cchBuffer, pszDest, pcchRequired);
}

HRESULT CContainerLayer05::ObjReadStringW(DWORD dwObject, DWORD dwProperty,
											ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreReadStringW(GetBlop(dwObject), dwProperty, cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer05::ObjReadBoolean(DWORD dwObject, DWORD dwProperty, PBOOLEAN pBoolean)
{
	return CoreReadBoolean(GetBlop(dwObject), dwProperty, pBoolean);
}

HRESULT CContainerLayer05::ObjReadInt8(DWORD dwObject, DWORD dwProperty, PINT8 pn8)
{
	return CoreReadInt8(GetBlop(dwObject), dwProperty, pn8);
}

HRESULT CContainerLayer05::ObjReadInt16(DWORD dwObject, DWORD dwProperty, PINT16 pn16)
{
	return CoreReadInt16(GetBlop(dwObject), dwProperty, pn16);
}

HRESULT CContainerLayer05::ObjReadInt32(DWORD dwObject, DWORD dwProperty, PINT32 pn32)
{
	return CoreReadInt32(GetBlop(dwObject), dwProperty, pn32);
}

HRESULT CContainerLayer05::ObjReadInt64(DWORD dwObject, DWORD dwProperty, PINT64 pn64)
{
	return CoreReadInt64(GetBlop(dwObject), dwProperty, pn64);
}

HRESULT CContainerLayer05::ObjReadUInt8(DWORD dwObject, DWORD dwProperty, PUINT8 pu8)
{
	return CoreReadUInt8(GetBlop(dwObject), dwProperty, pu8);
}

HRESULT CContainerLayer05::ObjReadUInt16(DWORD dwObject, DWORD dwProperty, PUINT16 pu16)
{
	return CoreReadUInt16(GetBlop(dwObject), dwProperty, pu16);
}

HRESULT CContainerLayer05::ObjReadUInt32(DWORD dwObject, DWORD dwProperty, PUINT32 pu32)
{
	return CoreReadUInt32(GetBlop(dwObject), dwProperty, pu32);
}

HRESULT CContainerLayer05::ObjReadUInt64(DWORD dwObject, DWORD dwProperty, PUINT64 pu64)
{
	return CoreReadUInt64(GetBlop(dwObject), dwProperty, pu64);
}

HRESULT CContainerLayer05::ObjReadFourCC(DWORD dwObject, DWORD dwProperty, PDWORD pdwFourCC)
{
	return CoreReadFourCC(GetBlop(dwObject), dwProperty, pdwFourCC);
}

HRESULT CContainerLayer05::ObjReadLength64(DWORD dwObject, DWORD dwProperty, PUINT64 pLength)
{
	return CoreReadLength64(GetBlop(dwObject), dwProperty, pLength);
}

HRESULT CContainerLayer05::ObjReadPosition64(DWORD dwObject, DWORD dwProperty, PINT64 pPosition)
{
	return CoreReadPosition64(GetBlop(dwObject), dwProperty, pPosition);
}

HRESULT CContainerLayer05::ObjReadInt32Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadInt32Array(GetBlop(dwObject), dwProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer05::ObjReadInt64Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadInt64Array(GetBlop(dwObject), dwProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer05::ObjReadPosition32Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadPosition32Array(GetBlop(dwObject), dwProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer05::ObjReadPosition64Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadPosition64Array(GetBlop(dwObject), dwProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer05::ObjQueryDataValue(DWORD dwObject, DWORD dwProperty, POMF_DATA_VALUE pDataValue)
{
	return CoreQueryDataValue(GetBlop(dwObject), dwProperty, pDataValue);
}

HRESULT CContainerLayer05::ObjReadDataValueAsRawBytes(DWORD dwObject, DWORD dwProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired)
{
	return CoreReadDataValueAsRawBytes(GetBlop(dwObject), dwProperty, cbDest, pDest, pcbRequired);
}

HRESULT CContainerLayer05::ObjReadGuid(DWORD dwObject, DWORD dwProperty, LPGUID pGuid)
{
	return CoreReadGuid(GetBlop(dwObject), dwProperty, pGuid);
}

HRESULT CContainerLayer05::ObjReadFirstMobID(DWORD dwObject, POMF_MOB_ID pMobID)
{
	return CoreReadFirstMobID(GetBlop(dwObject), pMobID);
}

HRESULT CContainerLayer05::ObjReadMobID(DWORD dwObject, DWORD dwProperty, POMF_MOB_ID pMobID)
{
	return CoreReadMobID(GetBlop(dwObject), dwProperty, pMobID);
}

HRESULT CContainerLayer05::ObjReadObjRef(DWORD dwObject, DWORD dwProperty, PDWORD pdwRefList, PDWORD pdwObjRef)
{
	return CoreReadObjRef(GetBlop(dwObject), dwProperty, pdwRefList, pdwObjRef);
}

HRESULT CContainerLayer05::ObjReadPoint(DWORD dwObject, DWORD dwProperty, POMF_POINT pPoint)
{
	return CoreReadPoint(GetBlop(dwObject), dwProperty, pPoint);
}

HRESULT CContainerLayer05::ObjReadProductVersion(DWORD dwObject, DWORD dwProperty, POMF_PRODUCT_VERSION pVersion)
{
	return CoreReadProductVersion(GetBlop(dwObject), dwProperty, pVersion);
}

HRESULT CContainerLayer05::ObjReadRational(DWORD dwObject, DWORD dwProperty, POMF_RATIONAL pRat)
{
	return CoreReadRational(GetBlop(dwObject), dwProperty, pRat);
}

HRESULT CContainerLayer05::ObjReadRationalAsFloat(DWORD dwObject, DWORD dwProperty, PFLOAT pFloat)
{
	return CoreReadRationalAsFloat(GetBlop(dwObject), dwProperty, pFloat);
}

HRESULT CContainerLayer05::ObjReadRationalAsDouble(DWORD dwObject, DWORD dwProperty, DOUBLE* pDouble)
{
	return CoreReadRationalAsDouble(GetBlop(dwObject), dwProperty, pDouble);
}

HRESULT CContainerLayer05::ObjReadTimeStamp(DWORD dwObject, DWORD dwProperty, POMF_TIMESTAMP pTimeStamp)
{
	return CoreReadTimeStamp(GetBlop(dwObject), dwProperty, pTimeStamp);
}

HRESULT CContainerLayer05::ObjReadTimeStampAsFileTime(DWORD dwObject, DWORD dwProperty, LPFILETIME pFileTime)
{
	return CoreReadTimeStampAsFileTime(GetBlop(dwObject), dwProperty, pFileTime);
}

HRESULT CContainerLayer05::ObjReadVersionType(DWORD dwObject, DWORD dwProperty, POMF_VERSIONTYPE pVersionType)
{
	return CoreReadVersionType(GetBlop(dwObject), dwProperty, pVersionType);
}

HRESULT CContainerLayer05::ObjGetDataKind(DWORD dwObject, DWORD dwProperty, PWORD pwDataKind)
{
	return CoreGetDataKindAsWord(GetBlop(dwObject), dwProperty, pwDataKind);
}

HRESULT CContainerLayer05::ObjGetDataKindAsStringA(DWORD dwObject, DWORD dwProperty,
														ULONG cchBuffer, PCHAR pchBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringA(GetBlop(dwObject), dwProperty, cchBuffer, pchBuffer, pcchRequired);
}

HRESULT CContainerLayer05::ObjGetDataKindAsStringW(DWORD dwObject, DWORD dwProperty,
														ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringW(GetBlop(dwObject), dwProperty, cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer05::ObjExtractDataToFile(DWORD dwObject, DWORD dwProperty, PCWSTR pwzFullPath, BOOL fOverwrite)
{
	return CoreExtractDataToFile(GetBlop(dwObject), dwProperty, pwzFullPath, fOverwrite);
}

HRESULT CContainerLayer05::ObjCreateStreamOnDataValue(DWORD dwObject, DWORD dwProperty, REFIID riid, PVOID *ppvOut)
{
	return CoreCreateStreamOnDataValue(GetBlop(dwObject), dwProperty, riid, ppvOut);
}

HRESULT CContainerLayer05::ObjCreateHGlobalOnDataValue(DWORD dwObject, DWORD dwProperty, UINT uFlags, HGLOBAL* phGlobal)
{
	return CoreCreateHGlobalOnDataValue(GetBlop(dwObject), dwProperty, uFlags, phGlobal);
}

// Array routines.
HRESULT CContainerLayer05::ObjGetArrayInfo(DWORD dwObject, DWORD dwProperty, PDWORD pdwDataType,
											PDWORD pdwRefList, PULONG pnElements, PULONG pcbData)
{
	return CoreGetArrayInfo(GetBlop(dwObject), dwProperty, pdwDataType, pdwRefList, pnElements, pcbData);
}

HRESULT CContainerLayer05::ObjAllocMobIndexArray(DWORD dwObject, DWORD dwProperty, POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	return CoreAllocMobIndexArray(GetBlop(dwObject), dwProperty, ppArray);
}

HRESULT CContainerLayer05::ObjAllocObjRefArray(DWORD dwObject, DWORD dwProperty, POMFOO_OBJREF_ARRAY* ppArray)
{
	return CoreAllocObjRefArray(GetBlop(dwObject), dwProperty, ppArray);
}

HRESULT CContainerLayer05::ObjAllocPositionArray(DWORD dwObject, DWORD dwProperty, POMFOO_POSITION_ARRAY* ppArray)
{
	return CoreAllocPositionArray(GetBlop(dwObject), dwProperty, ppArray);
}


