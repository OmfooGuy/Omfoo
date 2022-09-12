// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer02.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer02.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer02::CContainerLayer02(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer02::~CContainerLayer02(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer02::Load(PCWSTR pwzFileName)
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
//	These methods all begin with the prefix 'Rord'.
//	They accept the object ID argument as a reference to a BENTO_BLOP structure,
//	and the property ID argument as a PropOrdinal.
//*********************************************************************************************************************
HRESULT CContainerLayer02::RordIsPropertyPresent(BENTO_BLOP& rBlop, PropOrdinal eProperty)
{
	return CoreIsPropertyPresent(rBlop, OrdinalToPropertyID(eProperty));
}

HRESULT CContainerLayer02::RordGetPropInfo(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload)
{
	return CoreGetPropInfo(rBlop, OrdinalToPropertyID(eProperty), pdwStoredType, pdwRefList, pnFragments, pcbPayload);
}

HRESULT CContainerLayer02::RordReadStrict(BENTO_BLOP& rBlop, PropOrdinal eProperty, DWORD dwRequestedType, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(rBlop, OrdinalToPropertyID(eProperty), dwRequestedType, cbDest, pDest);
}

HRESULT CContainerLayer02::RordReadStrictNoType(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cbDest, PVOID pDest)
{
	return CoreReadStrict(rBlop, OrdinalToPropertyID(eProperty), (-1), cbDest, pDest);
}

HRESULT CContainerLayer02::RordReadRawBytes(BENTO_BLOP& rBlop, PropOrdinal eProperty,
											ULONG cbDest, PVOID pDest, PULONG pbRequired)
{
	return CoreReadRawBytes(rBlop, OrdinalToPropertyID(eProperty), cbDest, pDest, pbRequired);
}

HRESULT CContainerLayer02::RordReadStringA(BENTO_BLOP& rBlop, PropOrdinal eProperty,
											ULONG cchBuffer, PCHAR pszDest, PULONG pcchRequired)
{
	return CoreReadStringA(rBlop, OrdinalToPropertyID(eProperty), cchBuffer, pszDest, pcchRequired);
}

HRESULT CContainerLayer02::RordReadStringW(BENTO_BLOP& rBlop, PropOrdinal eProperty,
											ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CoreReadStringW(rBlop, OrdinalToPropertyID(eProperty), cchBuffer, pBuffer, pcchRequired);
}

HRESULT CContainerLayer02::RordReadBoolean(BENTO_BLOP& rBlop, PropOrdinal eProperty, PBOOLEAN pBoolean)
{
	return CoreReadBoolean(rBlop, OrdinalToPropertyID(eProperty), pBoolean);
}

HRESULT CContainerLayer02::RordReadInt8(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT8 pn8)
{
	return CoreReadInt8(rBlop, OrdinalToPropertyID(eProperty), pn8);
}

HRESULT CContainerLayer02::RordReadInt16(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT16 pn16)
{
	return CoreReadInt16(rBlop, OrdinalToPropertyID(eProperty), pn16);
}

HRESULT CContainerLayer02::RordReadInt32(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT32 pn32)
{
	return CoreReadInt32(rBlop, OrdinalToPropertyID(eProperty), pn32);
}

HRESULT CContainerLayer02::RordReadInt64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT64 pn64)
{
	return CoreReadInt64(rBlop, OrdinalToPropertyID(eProperty), pn64);
}

HRESULT CContainerLayer02::RordReadUInt8(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT8 pu8)
{
	return CoreReadUInt8(rBlop, OrdinalToPropertyID(eProperty), pu8);
}

HRESULT CContainerLayer02::RordReadUInt16(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT16 pu16)
{
	return CoreReadUInt16(rBlop, OrdinalToPropertyID(eProperty), pu16);
}

HRESULT CContainerLayer02::RordReadUInt32(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT32 pu32)
{
	return CoreReadUInt32(rBlop, OrdinalToPropertyID(eProperty), pu32);
}

HRESULT CContainerLayer02::RordReadUInt64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT64 pu64)
{
	return CoreReadUInt64(rBlop, OrdinalToPropertyID(eProperty), pu64);
}

HRESULT CContainerLayer02::RordReadFourCC(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwFourCC)
{
	return CoreReadFourCC(rBlop, OrdinalToPropertyID(eProperty), pdwFourCC);
}

HRESULT CContainerLayer02::RordReadLength64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT64 pLength)
{
	return CoreReadLength64(rBlop, OrdinalToPropertyID(eProperty), pLength);
}

HRESULT CContainerLayer02::RordReadPosition64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT64 pPosition)
{
	return CoreReadPosition64(rBlop, OrdinalToPropertyID(eProperty), pPosition);
}

HRESULT CContainerLayer02::RordReadDataOffset64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT64 pcbOffset)
{
	return CoreReadDataOffset64(rBlop, OrdinalToPropertyID(eProperty), pcbOffset);
}

HRESULT CContainerLayer02::RordReadCompCodeArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_COMP_CODE_ARRAY pArray)
{
	return CoreReadCompCodeArray(rBlop, OrdinalToPropertyID(eProperty), pArray);
}

HRESULT CContainerLayer02::RordReadCompSizeArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_COMP_SIZE_ARRAY pArray)
{
	return CoreReadCompSizeArray(rBlop, OrdinalToPropertyID(eProperty), pArray);
}

HRESULT CContainerLayer02::RordReadInt32Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadInt32Array(rBlop, OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer02::RordReadInt64Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadInt64Array(rBlop, OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer02::RordReadPosition32Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return CoreReadPosition32Array(rBlop, OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer02::RordReadPosition64Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return CoreReadPosition64Array(rBlop, OrdinalToPropertyID(eProperty), nMaxElements, pDest, pnActualElements);
}

HRESULT CContainerLayer02::RordQueryDataValue(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_DATA_VALUE pDataValue)
{
	return CoreQueryDataValue(rBlop, OrdinalToPropertyID(eProperty), pDataValue);
}

HRESULT CContainerLayer02::RordReadDataValueAsRawBytes(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired)
{
	return CoreReadDataValueAsRawBytes(rBlop, OrdinalToPropertyID(eProperty), cbDest, pDest, pcbRequired);
}

HRESULT CContainerLayer02::RordReadDataValueAsStringW(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pDest, PULONG pcchRequired)
{
	return CoreReadDataValueAsStringW(rBlop, OrdinalToPropertyID(eProperty), cchBuffer, pDest, pcchRequired);
}

HRESULT CContainerLayer02::RordReadGuid(BENTO_BLOP& rBlop, PropOrdinal eProperty, LPGUID pGuid)
{
	return CoreReadGuid(rBlop, OrdinalToPropertyID(eProperty), pGuid);
}

HRESULT CContainerLayer02::RordReadMobID(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_MOB_ID pMobID)
{
	return CoreReadMobID(rBlop, OrdinalToPropertyID(eProperty), pMobID);
}

HRESULT CContainerLayer02::RordReadObjRef(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwRefList, PDWORD pdwObjRef)
{
	return CoreReadObjRef(rBlop, OrdinalToPropertyID(eProperty), pdwRefList, pdwObjRef);
}

HRESULT CContainerLayer02::RordReadPoint(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_POINT pPoint)
{
	return CoreReadPoint(rBlop, OrdinalToPropertyID(eProperty), pPoint);
}

HRESULT CContainerLayer02::RordReadProductVersion(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_PRODUCT_VERSION pVersion)
{
	return CoreReadProductVersion(rBlop, OrdinalToPropertyID(eProperty), pVersion);
}

HRESULT CContainerLayer02::RordReadRational(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_RATIONAL pRat)
{
	return CoreReadRational(rBlop, OrdinalToPropertyID(eProperty), pRat);
}

HRESULT CContainerLayer02::RordReadRationalAsFloat(BENTO_BLOP& rBlop, PropOrdinal eProperty, PFLOAT pFloat)
{
	return CoreReadRationalAsFloat(rBlop, OrdinalToPropertyID(eProperty), pFloat);
}

HRESULT CContainerLayer02::RordReadRationalAsDouble(BENTO_BLOP& rBlop, PropOrdinal eProperty, DOUBLE* pDouble)
{
	return CoreReadRationalAsDouble(rBlop, OrdinalToPropertyID(eProperty), pDouble);
}

HRESULT CContainerLayer02::RordReadTimeStamp(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_TIMESTAMP pTimeStamp)
{
	return CoreReadTimeStamp(rBlop, OrdinalToPropertyID(eProperty), pTimeStamp);
}

HRESULT CContainerLayer02::RordReadExtended80AsDouble(BENTO_BLOP& rBlop, PropOrdinal eProperty, DOUBLE* pDouble)
{
	return CoreReadExtended80AsDouble(rBlop, OrdinalToPropertyID(eProperty), pDouble);
}

HRESULT CContainerLayer02::RordReadTimeStampAsFileTime(BENTO_BLOP& rBlop, PropOrdinal eProperty, LPFILETIME pFileTime)
{
	return CoreReadTimeStampAsFileTime(rBlop, OrdinalToPropertyID(eProperty), pFileTime);
}

HRESULT CContainerLayer02::RordReadVersionType(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_VERSIONTYPE pVersionType)
{
	return CoreReadVersionType(rBlop, OrdinalToPropertyID(eProperty), pVersionType);
}

HRESULT CContainerLayer02::RordGetDataKind(BENTO_BLOP& rBlop, PropOrdinal eProperty, PWORD pwDataKind)
{
	return CoreGetDataKindAsWord(rBlop, OrdinalToPropertyID(eProperty), pwDataKind);
}

HRESULT CContainerLayer02::RordGetDataKindAsStringA(BENTO_BLOP& rBlop, PropOrdinal eProperty,
														ULONG cchBuffer, PCHAR pchBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringA(rBlop, OrdinalToPropertyID(eProperty), cchBuffer, pchBuffer, pcchRequired);
}

HRESULT CContainerLayer02::RordGetDataKindAsStringW(BENTO_BLOP& rBlop, PropOrdinal eProperty,
														ULONG cchBuffer, PWCHAR pchBuffer, PULONG pcchRequired)
{
	return CoreGetDataKindAsStringW(rBlop, OrdinalToPropertyID(eProperty), cchBuffer, pchBuffer, pcchRequired);
}

HRESULT CContainerLayer02::RordExtractDataToFile(BENTO_BLOP& rBlop, PropOrdinal eProperty, PCWSTR pwzFullPath, BOOL fOverwrite)
{
	return CoreExtractDataToFile(rBlop, OrdinalToPropertyID(eProperty), pwzFullPath, fOverwrite);
}

HRESULT CContainerLayer02::RordCreateStreamOnDataValue(BENTO_BLOP& rBlop, PropOrdinal eProperty, REFIID riid, PVOID *ppvOut)
{
	return CoreCreateStreamOnDataValue(rBlop, OrdinalToPropertyID(eProperty), riid, ppvOut);
}

HRESULT CContainerLayer02::RordCreateHGlobalOnDataValue(BENTO_BLOP& rBlop, PropOrdinal eProperty, UINT uFlags, HGLOBAL* phGlobal)
{
	return CoreCreateHGlobalOnDataValue(rBlop, OrdinalToPropertyID(eProperty), uFlags, phGlobal);
}

// Array routines.
HRESULT CContainerLayer02::RordGetArrayInfo(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwDataType,
											PDWORD pdwRefList, PULONG pnElements, PULONG pcbData)
{
	return CoreGetArrayInfo(rBlop, OrdinalToPropertyID(eProperty), pdwDataType, pdwRefList, pnElements, pcbData);
}

HRESULT CContainerLayer02::RordAllocMobIndexArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	return CoreAllocMobIndexArray(rBlop, OrdinalToPropertyID(eProperty), ppArray);
}

HRESULT CContainerLayer02::RordAllocObjRefArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMFOO_OBJREF_ARRAY* ppArray)
{
	return CoreAllocObjRefArray(rBlop, OrdinalToPropertyID(eProperty), ppArray);
}

HRESULT CContainerLayer02::RordAllocPositionArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMFOO_POSITION_ARRAY* ppArray)
{
	return CoreAllocPositionArray(rBlop, OrdinalToPropertyID(eProperty), ppArray);
}

