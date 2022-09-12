// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer05.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer04.h"

class CContainerLayer05 : public CContainerLayer04
{
protected:
			CContainerLayer05(void);
	virtual	~CContainerLayer05(void);

public:
	STDMETHODIMP	ObjIsPropertyPresent(DWORD dwObject, DWORD dwProperty);
	STDMETHODIMP	ObjGetPropInfo(DWORD dwObject, DWORD dwProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload);
	STDMETHODIMP	ObjReadStrict(DWORD dwObject, DWORD dwProperty, DWORD dwRequestedType, ULONG cbDest, PVOID pDest);
	STDMETHODIMP	ObjReadStrictNoType(DWORD dwObject, DWORD dwProperty, ULONG cbDest, PVOID pDest);

	STDMETHODIMP	ObjReadRawBytes(DWORD dwObject, DWORD dwProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pbRequired);
	STDMETHODIMP	ObjReadStringA(DWORD dwObject, DWORD dwProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	ObjReadStringW(DWORD dwObject, DWORD dwProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	ObjReadBoolean(DWORD dwObject, DWORD dwProperty, PBOOLEAN pBoolean);
	STDMETHODIMP	ObjReadInt8(DWORD dwObject, DWORD dwProperty, PINT8 pn8);
	STDMETHODIMP	ObjReadInt16(DWORD dwObject, DWORD dwProperty, PINT16 pn16);
	STDMETHODIMP	ObjReadInt32(DWORD dwObject, DWORD dwProperty, PINT32 pn32);
	STDMETHODIMP	ObjReadInt64(DWORD dwObject, DWORD dwProperty, PINT64 pn64);
	STDMETHODIMP	ObjReadUInt8(DWORD dwObject, DWORD dwProperty, PUINT8 pu8);
	STDMETHODIMP	ObjReadUInt16(DWORD dwObject, DWORD dwProperty, PUINT16 pu16);
	STDMETHODIMP	ObjReadUInt32(DWORD dwObject, DWORD dwProperty, PUINT32 pu32);
	STDMETHODIMP	ObjReadUInt64(DWORD dwObject, DWORD dwProperty, PUINT64 pu64);

	STDMETHODIMP	ObjReadFourCC(DWORD dwObject, DWORD dwProperty, PDWORD pdwFourCC);
	STDMETHODIMP	ObjReadLength64(DWORD dwObject, DWORD dwProperty, PUINT64 pLength64);
	STDMETHODIMP	ObjReadPosition64(DWORD dwObject, DWORD dwProperty, PINT64 pPosition64);
	STDMETHODIMP	ObjReadInt32Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	ObjReadInt64Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);
	STDMETHODIMP	ObjReadPosition32Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	ObjReadPosition64Array(DWORD dwObject, DWORD dwProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);

	STDMETHODIMP	ObjReadGuid(DWORD dwObject, DWORD dwProperty, LPGUID pGuid);
	STDMETHODIMP	ObjReadFirstMobID(DWORD dwObject, POMF_MOB_ID pMobID);
	STDMETHODIMP	ObjReadMobID(DWORD dwObject, DWORD dwProperty, POMF_MOB_ID pMobID);
	STDMETHODIMP	ObjReadObjRef(DWORD dwObject, DWORD dwProperty, PDWORD pdwRefList, PDWORD pdwObjRef);
	STDMETHODIMP	ObjReadPoint(DWORD dwObject, DWORD dwProperty, POMF_POINT pPoint);
	STDMETHODIMP	ObjReadProductVersion(DWORD dwObject, DWORD dwProperty, POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	ObjReadRational(DWORD dwObject, DWORD dwProperty, POMF_RATIONAL pRational);
	STDMETHODIMP	ObjReadRationalAsFloat(DWORD dwObject, DWORD dwProperty, PFLOAT pFloat);
	STDMETHODIMP	ObjReadRationalAsDouble(DWORD dwObject, DWORD dwProperty, DOUBLE* pDouble);
	STDMETHODIMP	ObjReadTimeStamp(DWORD dwObject, DWORD dwProperty, POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	ObjReadTimeStampAsFileTime(DWORD dwObject, DWORD dwProperty, LPFILETIME pFileTime);
	STDMETHODIMP	ObjReadVersionType(DWORD dwObject, DWORD dwProperty, POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	ObjGetDataKind(DWORD dwObject, DWORD dwProperty, PWORD pwDataKind);
	STDMETHODIMP	ObjGetDataKindAsStringA(DWORD dwObject, DWORD dwProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	ObjGetDataKindAsStringW(DWORD dwObject, DWORD dwProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	ObjExtractDataToFile(DWORD dwObject, DWORD dwProperty, PCWSTR pwzFullPath, BOOL fOverwrite);

	STDMETHODIMP	ObjQueryDataValue(DWORD dwObject, DWORD dwProperty, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	ObjReadDataValueAsRawBytes(DWORD dwObject, DWORD dwProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	ObjCreateStreamOnDataValue(DWORD dwObject, DWORD dwProperty, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	ObjCreateHGlobalOnDataValue(DWORD dwObject, DWORD dwProperty, UINT uFlags, HGLOBAL* phGlobal);

	// Array routines.
	STDMETHODIMP	ObjGetArrayInfo(DWORD dwObject, DWORD dwProperty, PDWORD pdwDataType, PDWORD pdwRefList, PULONG pnElements, PULONG pcbData);
	STDMETHODIMP	ObjAllocMobIndexArray(DWORD dwObject, DWORD dwProperty, POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	ObjAllocObjRefArray(DWORD dwObject, DWORD dwProperty, POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	ObjAllocPositionArray(DWORD dwObject, DWORD dwProperty, POMFOO_POSITION_ARRAY* ppArray);
};
