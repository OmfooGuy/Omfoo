// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer04.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer03.h"

class CContainerLayer04 : public CContainerLayer03
{
protected:
			CContainerLayer04(void);
	virtual	~CContainerLayer04(void);

public:
	STDMETHODIMP	OrdIsPropertyPresent(DWORD dwObject, PropOrdinal eProperty);
	STDMETHODIMP	OrdGetPropInfo(DWORD dwObject, PropOrdinal eProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload);
	STDMETHODIMP	OrdReadStrict(DWORD dwObject, PropOrdinal eProperty, DWORD dwRequestedType, ULONG cbDest, PVOID pDest);
	STDMETHODIMP	OrdReadStrictNoType(DWORD dwObject, PropOrdinal eProperty, ULONG cbDest, PVOID pDest);

	STDMETHODIMP	OrdReadRawBytes(DWORD dwObject, PropOrdinal eProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pbRequired);
	STDMETHODIMP	OrdReadStringA(DWORD dwObject, PropOrdinal eProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	OrdReadStringW(DWORD dwObject, PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	OrdReadBoolean(DWORD dwObject, PropOrdinal eProperty, PBOOLEAN pBoolean);
	STDMETHODIMP	OrdReadInt8(DWORD dwObject, PropOrdinal eProperty, PINT8 pn8);
	STDMETHODIMP	OrdReadInt16(DWORD dwObject, PropOrdinal eProperty, PINT16 pn16);
	STDMETHODIMP	OrdReadInt32(DWORD dwObject, PropOrdinal eProperty, PINT32 pn32);
	STDMETHODIMP	OrdReadInt64(DWORD dwObject, PropOrdinal eProperty, PINT64 pn64);
	STDMETHODIMP	OrdReadUInt8(DWORD dwObject, PropOrdinal eProperty, PUINT8 pu8);
	STDMETHODIMP	OrdReadUInt16(DWORD dwObject, PropOrdinal eProperty, PUINT16 pu16);
	STDMETHODIMP	OrdReadUInt32(DWORD dwObject, PropOrdinal eProperty, PUINT32 pu32);
	STDMETHODIMP	OrdReadUInt64(DWORD dwObject, PropOrdinal eProperty, PUINT64 pu64);

	STDMETHODIMP	OrdReadLength64(DWORD dwObject, PropOrdinal eProperty, PUINT64 pLength64);
	STDMETHODIMP	OrdReadPosition64(DWORD dwObject, PropOrdinal eProperty, PINT64 pPosition64);
	STDMETHODIMP	OrdReadCompCodeArray(DWORD dwObject, PropOrdinal eProperty, POMF_COMP_CODE_ARRAY pArray);
	STDMETHODIMP	OrdReadCompSizeArray(DWORD dwObject, PropOrdinal eProperty, POMF_COMP_SIZE_ARRAY pArray);
	STDMETHODIMP	OrdReadInt32Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	OrdReadInt64Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);
	STDMETHODIMP	OrdReadPosition32Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	OrdReadPosition64Array(DWORD dwObject, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);

	STDMETHODIMP	OrdReadGuid(DWORD dwObject, PropOrdinal eProperty, LPGUID pGuid);
	STDMETHODIMP	OrdReadMobID(DWORD dwObject, PropOrdinal eProperty, POMF_MOB_ID pMobID);
	STDMETHODIMP	OrdReadObjRef(DWORD dwObject, PropOrdinal eProperty, PDWORD pdwRefList, PDWORD pdwObjRef);
	STDMETHODIMP	OrdReadPoint(DWORD dwObject, PropOrdinal eProperty, POMF_POINT pPoint);
	STDMETHODIMP	OrdReadProductVersion(DWORD dwObject, PropOrdinal eProperty, POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	OrdReadRational(DWORD dwObject, PropOrdinal eProperty, POMF_RATIONAL pRational);
	STDMETHODIMP	OrdReadRationalAsFloat(DWORD dwObject, PropOrdinal eProperty, PFLOAT pFloat);
	STDMETHODIMP	OrdReadRationalAsDouble(DWORD dwObject, PropOrdinal eProperty, DOUBLE* pDouble);
	STDMETHODIMP	OrdReadTimeStamp(DWORD dwObject, PropOrdinal eProperty, POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	OrdReadTimeStampAsFileTime(DWORD dwObject, PropOrdinal eProperty, LPFILETIME pFileTime);
	STDMETHODIMP	OrdReadVersionType(DWORD dwObject, PropOrdinal eProperty, POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	OrdGetDataKind(DWORD dwObject, PropOrdinal eProperty, PWORD pwDataKind);
	STDMETHODIMP	OrdGetDataKindAsStringA(DWORD dwObject, PropOrdinal eProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	OrdGetDataKindAsStringW(DWORD dwObject, PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	OrdExtractDataToFile(DWORD dwObject, PropOrdinal eProperty, PCWSTR pwzFullPath, BOOL fOverwrite);

	STDMETHODIMP	OrdQueryDataValue(DWORD dwObject, PropOrdinal eProperty, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	OrdReadDataValueAsRawBytes(DWORD dwObject, PropOrdinal eProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	OrdCreateStreamOnDataValue(DWORD dwObject, PropOrdinal eProperty, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	OrdCreateHGlobalOnDataValue(DWORD dwObject, PropOrdinal eProperty, UINT uFlags, HGLOBAL* phGlobal);

	// Array routines.
	STDMETHODIMP	OrdGetArrayInfo(DWORD dwObject, PropOrdinal eProperty, PDWORD pdwDataType, PDWORD pdwRefList, PULONG pnElements, PULONG pcbData);
	STDMETHODIMP	OrdAllocMobIndexArray(DWORD dwObject, PropOrdinal eProperty, POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	OrdAllocObjRefArray(DWORD dwObject, PropOrdinal eProperty, POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	OrdAllocPositionArray(DWORD dwObject, PropOrdinal eProperty, POMFOO_POSITION_ARRAY* ppArray);
};
