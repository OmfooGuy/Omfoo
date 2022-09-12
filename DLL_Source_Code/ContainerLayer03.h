// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer03.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer02.h"

class CContainerLayer03 : public CContainerLayer02
{
protected:
			CContainerLayer03(void);
	virtual	~CContainerLayer03(void);

public:
	STDMETHODIMP	RstrIsPropertyPresent(BENTO_BLOP& rBlop, LPCSTR pszPropertyName);
	STDMETHODIMP	RstrGetPropInfo(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload);
	STDMETHODIMP	RstrReadStrict(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, DWORD dwRequestedType, ULONG cbDest, PVOID pDest);
	STDMETHODIMP	RstrReadStrictNoType(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cbDest, PVOID pDest);

	STDMETHODIMP	RstrReadRawBytes(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cbBuffer, PVOID pBuffer, PULONG pbRequired);
	STDMETHODIMP	RstrReadStringA(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RstrReadStringW(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	RstrReadBoolean(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PBOOLEAN pBoolean);
	STDMETHODIMP	RstrReadInt8(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT8 pn8);
	STDMETHODIMP	RstrReadInt16(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT16 pn16);
	STDMETHODIMP	RstrReadInt32(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT32 pn32);
	STDMETHODIMP	RstrReadInt64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT64 pn64);
	STDMETHODIMP	RstrReadUInt8(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT8 pu8);
	STDMETHODIMP	RstrReadUInt16(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT16 pu16);
	STDMETHODIMP	RstrReadUInt32(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT32 pu32);
	STDMETHODIMP	RstrReadUInt64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT64 pu64);

	STDMETHODIMP	RstrReadLength64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PUINT64 pLength64);
	STDMETHODIMP	RstrReadPosition64(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PINT64 pPosition64);
	STDMETHODIMP	RstrReadCompCodeArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_COMP_CODE_ARRAY pArray);
	STDMETHODIMP	RstrReadCompSizeArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_COMP_SIZE_ARRAY pArray);
	STDMETHODIMP	RstrReadInt32Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	RstrReadInt64Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);
	STDMETHODIMP	RstrReadPosition32Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	RstrReadPosition64Array(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);

	STDMETHODIMP	RstrReadGuid(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, LPGUID pGuid);
	STDMETHODIMP	RstrReadMobID(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_MOB_ID pMobID);
	STDMETHODIMP	RstrReadObjRef(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PDWORD pdwRefList, PDWORD pdwObjRef);
	STDMETHODIMP	RstrReadPoint(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_POINT pPoint);
	STDMETHODIMP	RstrReadProductVersion(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	RstrReadRational(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_RATIONAL pRational);
	STDMETHODIMP	RstrReadRationalAsFloat(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PFLOAT pFloat);
	STDMETHODIMP	RstrReadRationalAsDouble(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, DOUBLE* pDouble);

	STDMETHODIMP	RstrReadExtended80AsDouble(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, DOUBLE* pDouble);
	STDMETHODIMP	RstrReadTimeStamp(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	RstrReadTimeStampAsFileTime(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, LPFILETIME pFileTime);
	STDMETHODIMP	RstrReadVersionType(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	RstrGetDataKind(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PWORD pwDataKind);
	STDMETHODIMP	RstrGetDataKindAsStringA(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RstrGetDataKindAsStringW(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	RstrExtractDataToFile(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PCWSTR pwzFullPath, BOOL fOverwrite);

	STDMETHODIMP	RstrQueryDataValue(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	RstrReadDataValueAsRawBytes(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	RstrCreateStreamOnDataValue(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	RstrCreateHGlobalOnDataValue(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, UINT uFlags, HGLOBAL* phGlobal);

	// Array routines.
	STDMETHODIMP	RstrGetArrayInfo(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, PDWORD pdwDataType, PDWORD pdwRefList, PULONG pnElements, PULONG pcbData);
	STDMETHODIMP	RstrAllocMobIndexArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	RstrAllocObjRefArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	RstrAllocPositionArray(BENTO_BLOP& rBlop, LPCSTR pszPropertyName, POMFOO_POSITION_ARRAY* ppArray);
};
