// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer04.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer04.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer04::CContainerLayer04(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer04::~CContainerLayer04(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer04::Load(PCWSTR pwzFileName)
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
//	These methods all begin with the prefix 'Ord'.
//	They accept the object ID argument as DWORD (the actual 32-bit value in the Bento TOC),
//	and the property ID argument as a PropOrdinal.
//*********************************************************************************************************************
HRESULT CContainerLayer04::OrdIsPropertyPresent(DWORD dwObject, PropOrdinal eProperty)
{
	return CoreIsPropertyPresent(GetBlop(dwObject), OrdinalToPropertyID(eProperty));
}

HRESULT CContainerLayer04::OrdGetPropInfo(DWORD dwObject, PropOrdinal eProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload)
{
	return CoreGetPropInfo(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pdwStoredType, pdwRefList, pnFragments, pcbPayload);
}

HRESULT CContainerLayer04::OrdReadStrict(DWORD dwObject, PropOrdinal eProperty, DWORD dwRequestedType, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(GetBlop(dwObject), OrdinalToPropertyID(eProperty), dwRequestedType, cbDest, pDest);
}

HRESULT CContainerLayer04::OrdReadStrictNoType(DWORD dwObject, PropOrdinal eProperty, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(GetBlop(dwObject), OrdinalToPropertyID(eProperty), (-1), cbDest, pDest);
}

HRESULT CContainerLayer04::OrdReadRawBytes(DWORD dwObject, PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pbRequired)
{
	return CoreReadRawBytes(GetBlop(dwObject), OrdinalToPropertyID(eProperty), cbDest, pDest, pbRequired);
}

HRESULT CContainerLayer04::OrdReadStringA(DWORD dwObject, PropOrdinal eProperty,
											ULONG cchBuffer, PCHAR pszDest, PULONG pcchRequired)
{
	return CoreReadStringA(GetBlop(dwObject), OrdinalToPropertyID(eProperty), cchBuffer, pszDest, pcchRequired);
}

HRESULT CContainerLayer04::OrdReadStringW(DWORD dwObject, PropOrdinal eProperty,
											ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreReadStringW(GetBlop(dwObject), OrdinalToPropertyID(eProperty), cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer04::OrdReadBoolean(DWORD dwObject, PropOrdinal eProperty, PBOOLEAN pBoolean)
{
	return CoreReadBoolean(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pBoolean);
}

HRESULT CContainerLayer04::OrdReadInt8(DWORD dwObject, PropOrdinal eProperty, PINT8 pn8)
{
	return CoreReadInt8(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pn8);
}

HRESULT CContainerLayer04::OrdReadInt16(DWORD dwObject, PropOrdinal eProperty, PINT16 pn16)
{
	return CoreReadInt16(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pn16);
}

HRESULT CContainerLayer04::OrdReadInt32(DWORD dwObject, PropOrdinal eProperty, PINT32 pn32)
{
	return CoreReadInt32(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pn32);
}

HRESULT CContainerLayer04::OrdReadInt64(DWORD dwObject, PropOrdinal eProperty, PINT64 pn64)
{
	return CoreReadInt64(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pn64);
}

HRESULT CContainerLayer04::OrdReadUInt8(DWORD dwObject, PropOrdinal eProperty, PUINT8 pu8)
{
	return CoreReadUInt8(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pu8);
}

HRESULT CContainerLayer04::OrdReadUInt16(DWORD dwObject, PropOrdinal eProperty, PUINT16 pu16)
{
	return CoreReadUInt16(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pu16);
}

HRESULT CContainerLayer04::OrdReadUInt32(DWORD dwObject, PropOrdinal eProperty, PUINT32 pu32)
{
	return CoreReadUInt32(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pu32);
}

HRESULT CContainerLayer04::OrdReadUInt64(DWORD dwObject, PropOrdinal eProperty, PUINT64 pu64)
{
	return CoreReadUInt64(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pu64);
}

HRESULT CContainerLayer04::OrdReadLength64(DWORD dwObject, PropOrdinal eProperty, PUINT64 pLength)
{
	return CoreReadLength64(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pLength);
}

HRESULT CContainerLayer04::OrdReadPosition64(DWORD dwObject, PropOrdinal eProperty, PINT64 pPosition)
{
	return CoreReadPosition64(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pPosition);
}

HRESULT CContainerLayer04::OrdReadCompCodeArray(DWORD dwObject, PropOrdinal eProperty, POMF_COMP_CODE_ARRAY pArray)
{
	return CoreReadCompCodeArray(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pArray);
}

HRESULT CContainerLayer04::OrdReadCompSizeArray(DWORD dwObject, PropOrdinal eProperty, POMF_COMP_SIZE_ARRAY pArray)
{
	return CoreReadCompSizeArray(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pArray);
}

HRESULT CContainerLayer04::OrdReadInt32Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadInt32Array(GetBlop(dwObject), OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer04::OrdReadInt64Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadInt64Array(GetBlop(dwObject), OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer04::OrdReadPosition32Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadPosition32Array(GetBlop(dwObject), OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer04::OrdReadPosition64Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadPosition64Array(GetBlop(dwObject), OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer04::OrdQueryDataValue(DWORD dwObject, PropOrdinal eProperty, POMF_DATA_VALUE pDataValue)
{
	return CoreQueryDataValue(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pDataValue);
}

HRESULT CContainerLayer04::OrdReadDataValueAsRawBytes(DWORD dwObject, PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired)
{
	return CoreReadDataValueAsRawBytes(GetBlop(dwObject), OrdinalToPropertyID(eProperty), cbDest, pDest, pcbRequired);
}

HRESULT CContainerLayer04::OrdReadGuid(DWORD dwObject, PropOrdinal eProperty, LPGUID pGuid)
{
	return CoreReadGuid(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pGuid);
}

HRESULT CContainerLayer04::OrdReadMobID(DWORD dwObject, PropOrdinal eProperty, POMF_MOB_ID pMobID)
{
	return CoreReadMobID(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pMobID);
}

HRESULT CContainerLayer04::OrdReadObjRef(DWORD dwObject, PropOrdinal eProperty, PDWORD pdwRefList, PDWORD pdwObjRef)
{
	return CoreReadObjRef(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pdwRefList, pdwObjRef);
}

HRESULT CContainerLayer04::OrdReadPoint(DWORD dwObject, PropOrdinal eProperty, POMF_POINT pPoint)
{
	return CoreReadPoint(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pPoint);
}

HRESULT CContainerLayer04::OrdReadProductVersion(DWORD dwObject, PropOrdinal eProperty, POMF_PRODUCT_VERSION pVersion)
{
	return CoreReadProductVersion(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pVersion);
}

HRESULT CContainerLayer04::OrdReadRational(DWORD dwObject, PropOrdinal eProperty, POMF_RATIONAL pRat)
{
	return CoreReadRational(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pRat);
}

HRESULT CContainerLayer04::OrdReadRationalAsFloat(DWORD dwObject, PropOrdinal eProperty, PFLOAT pFloat)
{
	return CoreReadRationalAsFloat(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pFloat);
}

HRESULT CContainerLayer04::OrdReadRationalAsDouble(DWORD dwObject, PropOrdinal eProperty, DOUBLE* pDouble)
{
	return CoreReadRationalAsDouble(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pDouble);
}

HRESULT CContainerLayer04::OrdReadTimeStamp(DWORD dwObject, PropOrdinal eProperty, POMF_TIMESTAMP pTimeStamp)
{
	return CoreReadTimeStamp(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pTimeStamp);
}

HRESULT CContainerLayer04::OrdReadTimeStampAsFileTime(DWORD dwObject, PropOrdinal eProperty, LPFILETIME pFileTime)
{
	return CoreReadTimeStampAsFileTime(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pFileTime);
}

HRESULT CContainerLayer04::OrdReadVersionType(DWORD dwObject, PropOrdinal eProperty, POMF_VERSIONTYPE pVersionType)
{
	return CoreReadVersionType(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pVersionType);
}

HRESULT CContainerLayer04::OrdGetDataKind(DWORD dwObject, PropOrdinal eProperty, PWORD pwDataKind)
{
	return CoreGetDataKindAsWord(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pwDataKind);
}

HRESULT CContainerLayer04::OrdGetDataKindAsStringA(DWORD dwObject, PropOrdinal eProperty,
														ULONG cchBuffer, PCHAR pchBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringA(GetBlop(dwObject), OrdinalToPropertyID(eProperty), cchBuffer, pchBuffer, pcchRequired);
}

HRESULT CContainerLayer04::OrdGetDataKindAsStringW(DWORD dwObject, PropOrdinal eProperty,
														ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringW(GetBlop(dwObject), OrdinalToPropertyID(eProperty), cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer04::OrdExtractDataToFile(DWORD dwObject, PropOrdinal eProperty, PCWSTR pwzFullPath, BOOL fOverwrite)
{
	return CoreExtractDataToFile(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pwzFullPath, fOverwrite);
}

HRESULT CContainerLayer04::OrdCreateStreamOnDataValue(DWORD dwObject, PropOrdinal eProperty, REFIID riid, PVOID *ppvOut)
{
	return CoreCreateStreamOnDataValue(GetBlop(dwObject), OrdinalToPropertyID(eProperty), riid, ppvOut);
}

HRESULT CContainerLayer04::OrdCreateHGlobalOnDataValue(DWORD dwObject, PropOrdinal eProperty, UINT uFlags, HGLOBAL* phGlobal)
{
	return CoreCreateHGlobalOnDataValue(GetBlop(dwObject), OrdinalToPropertyID(eProperty), uFlags, phGlobal);
}

// Array routines.
HRESULT CContainerLayer04::OrdGetArrayInfo(DWORD dwObject, PropOrdinal eProperty, PDWORD pdwDataType,
											PDWORD pdwRefList, PULONG pnElements, PULONG pcbData)
{
	return CoreGetArrayInfo(GetBlop(dwObject), OrdinalToPropertyID(eProperty), pdwDataType, pdwRefList, pnElements, pcbData);
}

HRESULT CContainerLayer04::OrdAllocMobIndexArray(DWORD dwObject, PropOrdinal eProperty, POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	return CoreAllocMobIndexArray(GetBlop(dwObject), OrdinalToPropertyID(eProperty), ppArray);
}

HRESULT CContainerLayer04::OrdAllocObjRefArray(DWORD dwObject, PropOrdinal eProperty, POMFOO_OBJREF_ARRAY* ppArray)
{
	return CoreAllocObjRefArray(GetBlop(dwObject), OrdinalToPropertyID(eProperty), ppArray);
}

HRESULT CContainerLayer04::OrdAllocPositionArray(DWORD dwObject, PropOrdinal eProperty, POMFOO_POSITION_ARRAY* ppArray)
{
	return CoreAllocPositionArray(GetBlop(dwObject), OrdinalToPropertyID(eProperty), ppArray);
}


