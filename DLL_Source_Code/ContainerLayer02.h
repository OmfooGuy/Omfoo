// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer02.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer01.h"

class CContainerLayer02 : public CContainerLayer01
{
protected:
			CContainerLayer02(void);
	virtual	~CContainerLayer02(void);

public:
	STDMETHODIMP	RordIsPropertyPresent(BENTO_BLOP& rBlop, PropOrdinal eProperty);
	STDMETHODIMP	RordGetPropInfo(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload);
	STDMETHODIMP	RordReadStrict(BENTO_BLOP& rBlop, PropOrdinal eProperty, DWORD dwRequestedType, ULONG cbDest, PVOID pDest);
	STDMETHODIMP	RordReadStrictNoType(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cbDest, PVOID pDest);

	STDMETHODIMP	RordReadRawBytes(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	RordReadStringA(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RordReadStringW(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	RordReadBoolean(BENTO_BLOP& rBlop, PropOrdinal eProperty, PBOOLEAN pBoolean);
	STDMETHODIMP	RordReadInt8(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT8 pn8);
	STDMETHODIMP	RordReadInt16(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT16 pn16);
	STDMETHODIMP	RordReadInt32(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT32 pn32);
	STDMETHODIMP	RordReadInt64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT64 pn64);
	STDMETHODIMP	RordReadUInt8(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT8 pn8);
	STDMETHODIMP	RordReadUInt16(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT16 pu16);
	STDMETHODIMP	RordReadUInt32(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT32 pu32);
	STDMETHODIMP	RordReadUInt64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT64 pu64);

	STDMETHODIMP	RordReadFourCC(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwFourCC);
	STDMETHODIMP	RordReadLength64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT64 pLength64);
	STDMETHODIMP	RordReadPosition64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PINT64 pPosition64);
	STDMETHODIMP	RordReadDataOffset64(BENTO_BLOP& rBlop, PropOrdinal eProperty, PUINT64 pcbOffset);
	STDMETHODIMP	RordReadCompCodeArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_COMP_CODE_ARRAY pArray);
	STDMETHODIMP	RordReadCompSizeArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_COMP_SIZE_ARRAY pArray);

	STDMETHODIMP	RordReadInt32Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	RordReadInt64Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);
	STDMETHODIMP	RordReadPosition32Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	RordReadPosition64Array(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);

	STDMETHODIMP	RordReadGuid(BENTO_BLOP& rBlop, PropOrdinal eProperty, LPGUID pGuid);
	STDMETHODIMP	RordReadMobID(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_MOB_ID pMobID);
	STDMETHODIMP	RordReadObjRef(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwRefList, PDWORD pdwObjRef);
	STDMETHODIMP	RordReadPoint(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_POINT pPoint);
	STDMETHODIMP	RordReadProductVersion(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	RordReadRational(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_RATIONAL pRational);
	STDMETHODIMP	RordReadRationalAsFloat(BENTO_BLOP& rBlop, PropOrdinal eProperty, PFLOAT pFloat);
	STDMETHODIMP	RordReadRationalAsDouble(BENTO_BLOP& rBlop, PropOrdinal eProperty, DOUBLE* pDouble);

	STDMETHODIMP	RordReadExtended80AsDouble(BENTO_BLOP& rBlop, PropOrdinal eProperty, DOUBLE* pDouble);
	STDMETHODIMP	RordReadTimeStamp(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	RordReadTimeStampAsFileTime(BENTO_BLOP& rBlop, PropOrdinal eProperty, LPFILETIME pFileTime);
	STDMETHODIMP	RordReadVersionType(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	RordGetDataKind(BENTO_BLOP& rBlop, PropOrdinal eProperty, PWORD pwDataKind);
	STDMETHODIMP	RordGetDataKindAsStringA(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RordGetDataKindAsStringW(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RordExtractDataToFile(BENTO_BLOP& rBlop, PropOrdinal eProperty, PCWSTR pwzFullPath, BOOL fOverwrite);

	STDMETHODIMP	RordQueryDataValue(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	RordReadDataValueAsRawBytes(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	RordReadDataValueAsStringW(BENTO_BLOP& rBlop, PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RordCreateStreamOnDataValue(BENTO_BLOP& rBlop, PropOrdinal eProperty, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	RordCreateHGlobalOnDataValue(BENTO_BLOP& rBlop, PropOrdinal eProperty, UINT uFlags, HGLOBAL* phGlobal);

	// Array routines.
	STDMETHODIMP	RordGetArrayInfo(BENTO_BLOP& rBlop, PropOrdinal eProperty, PDWORD pdwDataType, PDWORD pdwRefList, PULONG pnElements, PULONG pcbData);
	STDMETHODIMP	RordAllocMobIndexArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	RordAllocObjRefArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	RordAllocPositionArray(BENTO_BLOP& rBlop, PropOrdinal eProperty, POMFOO_POSITION_ARRAY* ppArray);
};
