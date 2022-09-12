// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfObject.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "ReadOmf.h"

//*********************************************************************************************************************
//	Structures
//*********************************************************************************************************************
//#ifdef _WIN64
//#pragma pack(push,8)	// Align structure members to 8-byte boundaries.
//#else
//#pragma pack(push,4)	// Align structure members to 4-byte boundaries.
//#endif

// Private structure used internally during IOmfObject::IsSelf().
#pragma pack(push, 1)	// align structure members to byte boundaries
typedef struct {
	PBENTO_BLOP	pBlop;
	#ifndef _WIN64
	DWORD	dwPad;
	#endif
	// These values come directly from CReadableFile via GetFileInformationByHandle().
	DWORD	dwFileIndexHigh;
	DWORD	dwFileIndexLow;
	DWORD	dwVolumeSerialNumber;
} SELF_COMPARE_INFO, *PSELF_COMPARE_INFO;
#pragma pack(pop)		// restore compiler's previous alignment settings

// Private interface used internally during IOmfObject::IsSelf().
struct __declspec(uuid("880DC05E-46D4-4185-A353-07718CC2CD39")) IOobjBackDoor;
interface IOobjBackDoor : public IUnknown
{
	virtual HRESULT __stdcall GetSelfCompareInfo(__out PSELF_COMPARE_INFO pInfo)= 0;
};

class COmfObject
	: protected BENTO_BLOP
	, protected CReadOmf_StringOrdinals
	, protected INonDelegatingUnknown
	, protected IOobjBackDoor
	, public IOmfObject
{
friend class CContainerLayer96;
friend class CContainerLayer97;
friend class COmfooIterator;
friend class CObjRefArray;
friend class CMobIndexArray;

private:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);

protected:
	COmfObject(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual ~COmfObject(void);

	// INonDelegatingUnknown methods in V-table order.
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);
	ULONG __stdcall	NonDelegatingAddRef(void);
	ULONG __stdcall	NonDelegatingRelease(void);

public:
    DECLARE_IUNKNOWN

private:
	// IOobjBackDoor methods in V-table order.
	STDMETHODIMP	GetSelfCompareInfo(__out PSELF_COMPARE_INFO pInfo);

protected:
	// IOmfObject methods in V-table order.
	STDMETHODIMP	GetContainer(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	GetParent(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	GetClassFourCC(void);
	STDMETHODIMP	IsSelf(__in IUnknown *punk, __in BOOL fStrict);
	STDMETHODIMP	IsATypeOf(__in DWORD dwClassFourCC);
	STDMETHODIMP	IsPropertyPresent(__in LPCSTR pszPropertyName);

	STDMETHODIMP	ReadRawBytes(__in LPCSTR pszPropertyName,
											__in ULONG cbDest,
											__out_opt PVOID pDest,
												__out_opt PULONG pcbRequired);

	STDMETHODIMP	ReadStringA(__in LPCSTR pszPropertyName,
											__in ULONG cchBuffer,
											__out_opt PCHAR pBuffer,
												__out_opt PULONG pcchRequired);

	STDMETHODIMP	ReadStringW(__in LPCSTR pszPropertyName,
										__in ULONG cchBuffer,
											__out_opt PWCHAR pBuffer,
												__out_opt PULONG pcchRequired);

	STDMETHODIMP	ReadBoolean(__in LPCSTR pszPropertyName, __out PBOOLEAN pBoolean);
	STDMETHODIMP	ReadInt8(__in LPCSTR pszPropertyName, __out PINT8 pn8);
	STDMETHODIMP	ReadInt16(__in LPCSTR pszPropertyName, __out PINT16 pn16);
	STDMETHODIMP	ReadInt32(__in LPCSTR pszPropertyName, __out PINT32 pn32);
	STDMETHODIMP	ReadInt64(__in LPCSTR pszPropertyName, __out PINT64 pn64);
	STDMETHODIMP	ReadUInt8(__in LPCSTR pszPropertyName, __out PUINT8 pu8);
	STDMETHODIMP	ReadUInt16(__in LPCSTR pszPropertyName, __out PUINT16 pu16);
	STDMETHODIMP	ReadUInt32(__in LPCSTR pszPropertyName, __out PUINT32 pu32);
	STDMETHODIMP	ReadUInt64(__in LPCSTR pszPropertyName, __out PUINT64 pu64);
	STDMETHODIMP	ReadLength64(__in LPCSTR pszPropertyName, __out PUINT64 pLength);
	STDMETHODIMP	ReadPosition64(__in LPCSTR pszPropertyName, __out PINT64 pPosition);

	STDMETHODIMP	ReadInt32Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT32 pDest,
												__out PULONG pnActualElements);

	STDMETHODIMP	ReadInt64Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT64 pDest,
												__out PULONG pnActualElements);

	STDMETHODIMP	ReadPosition32Array(__in LPCSTR pszPropertyName,
											__in ULONG nMaxElements,
												__out_opt PUINT32 pDest,
												__out PULONG pnActualElements);

	STDMETHODIMP	ReadPosition64Array(__in LPCSTR pszPropertyName,
											__in ULONG nMaxElements,
												__out_opt PUINT64 pDest,
												__out PULONG pnActualElements);

	STDMETHODIMP	ReadGuid(__in LPCSTR pszPropertyName, __out LPGUID pGuid);
	STDMETHODIMP	ReadMobID(__in LPCSTR pszPropertyName, __out POMF_MOB_ID pMobID);
	STDMETHODIMP	ReadPoint(__in LPCSTR pszPropertyName, __out POMF_POINT pPoint);
	STDMETHODIMP	ReadProductVersion(__in LPCSTR pszPropertyName, __out POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	ReadVersionType(__in LPCSTR pszPropertyName, __out POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	ReadRational(__in LPCSTR pszPropertyName, __out POMF_RATIONAL pRational);
	STDMETHODIMP	ReadRationalAsFloat(__in LPCSTR pszPropertyName, __out PFLOAT pFloat);
	STDMETHODIMP	ReadRationalAsDouble(__in LPCSTR pszPropertyName, __out DOUBLE* pDouble);

	STDMETHODIMP	ReadTimeStamp(__in LPCSTR pszPropertyName, __out POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	ReadTimeStampAsFileTime(__in LPCSTR pszPropertyName, __out LPFILETIME pFileTime);

	STDMETHODIMP	ReadCompCodeArray(__in LPCSTR pszPropertyName, __out POMF_COMP_CODE_ARRAY pDest);
	STDMETHODIMP	ReadCompSizeArray(__in LPCSTR pszPropertyName, __out POMF_COMP_SIZE_ARRAY pDest);

	STDMETHODIMP	QueryDataValue(__in LPCSTR pszPropertyName, __out POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	ReadDataValueAsRawBytes(__in LPCSTR pszPropertyName, __in ULONG cbBuffer,
																__out_opt PVOID pBuffer, __out PULONG pcbBuffer);

	STDMETHODIMP	CreateHGlobalOnDataValue(__in LPCSTR pszPropertyName, __in UINT uFlags, __out HGLOBAL* phGlobal);
	STDMETHODIMP	CreateStreamOnDataValue(__in LPCSTR pszPropertyName, __in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	ReadExtended80AsDouble(__in LPCSTR pszPropertyName, __out DOUBLE* pDouble);

	STDMETHODIMP	GetDataKind(__in LPCSTR pszPropertyName, PWORD pwDataKind);
	STDMETHODIMP	GetDataKindAsStringA(__in LPCSTR pszPropertyName, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	InstantiateObjRef(__in LPCSTR pszPropertyName, __in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	IterateObjRefArray(__in LPCSTR pszPropertyName, __out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateMobIndexArray(__in LPCSTR pszPropertyName, __out IOmfooIterator **ppIterator);

//
public:
	STDMETHODIMP	OrdIsPropertyPresent(PropOrdinal eProperty);
	STDMETHODIMP	OrdReadStrict(PropOrdinal eProperty, ULONG cbDest, PVOID pDest);
	STDMETHODIMP	OrdReadRawBytes(PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired);
	STDMETHODIMP	OrdReadStringA(PropOrdinal eProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	OrdReadStringW(PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	OrdReadBoolean(PropOrdinal eProperty, PBOOLEAN pBoolean);
	STDMETHODIMP	OrdReadInt8(PropOrdinal eProperty, PINT8 pn8);
	STDMETHODIMP	OrdReadInt16(PropOrdinal eProperty, PINT16 pn16);
	STDMETHODIMP	OrdReadInt32(PropOrdinal eProperty, PINT32 pn32);
	STDMETHODIMP	OrdReadInt64(PropOrdinal eProperty, PINT64 pn64);
	STDMETHODIMP	OrdReadUInt8(PropOrdinal eProperty, PUINT8 pu8);
	STDMETHODIMP	OrdReadUInt16(PropOrdinal eProperty, PUINT16 pu16);
	STDMETHODIMP	OrdReadUInt32(PropOrdinal eProperty, PUINT32 pu32);
	STDMETHODIMP	OrdReadUInt64(PropOrdinal eProperty, PUINT64 pu64);

	STDMETHODIMP	OrdReadFourCC(PropOrdinal eProperty, PDWORD pdwFourCC);
	STDMETHODIMP	OrdReadLength64(PropOrdinal eProperty, PUINT64 pLength);
	STDMETHODIMP	OrdReadPosition64(PropOrdinal eProperty, PINT64 pPosition);
	STDMETHODIMP	OrdReadDataOffset64(PropOrdinal eProperty, PUINT64 pcbOffset);
	STDMETHODIMP	OrdReadInt32Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	OrdReadInt64Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);
	STDMETHODIMP	OrdReadPosition32Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	OrdReadPosition64Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);

	STDMETHODIMP	OrdReadGuid(PropOrdinal eProperty, LPGUID pGuid);
	STDMETHODIMP	OrdReadMobID(PropOrdinal eProperty, POMF_MOB_ID pMobID);
	STDMETHODIMP	OrdReadObjRef(PropOrdinal eProperty, PDWORD pdwRefList, PDWORD pdwObjRef);
	STDMETHODIMP	OrdReadPoint(PropOrdinal eProperty, POMF_POINT pPoint);
	STDMETHODIMP	OrdReadProductVersion(PropOrdinal eProperty, POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	OrdReadVersionType(PropOrdinal eProperty, POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	OrdReadRational(PropOrdinal eProperty, POMF_RATIONAL pRational);
	STDMETHODIMP	OrdReadRationalAsFloat(PropOrdinal eProperty, PFLOAT pFloat);
	STDMETHODIMP	OrdReadRationalAsDouble(PropOrdinal eProperty, DOUBLE* pDouble);

	STDMETHODIMP	OrdReadTimeStamp(PropOrdinal eProperty, POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	OrdReadTimeStampAsFileTime(PropOrdinal eProperty, LPFILETIME pFileTime);

	STDMETHODIMP	OrdReadCompCodeArray(PropOrdinal eProperty, POMF_COMP_CODE_ARRAY pDest);
	STDMETHODIMP	OrdReadCompSizeArray(PropOrdinal eProperty, POMF_COMP_SIZE_ARRAY pDest);

	STDMETHODIMP	OrdQueryDataValue(PropOrdinal eProperty, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	OrdReadDataValueAsRawBytes(PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired);
	STDMETHODIMP	OrdReadDataValueAsStringW(PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pDest, PULONG pcchRequired);
	STDMETHODIMP	OrdCreateHGlobalOnDataValue(PropOrdinal eProperty, UINT uFlags, HGLOBAL* phGlobal);
	STDMETHODIMP	OrdCreateStreamOnDataValue(PropOrdinal eProperty, REFIID riid, PVOID *ppvOut);

	STDMETHODIMP	OrdGetDataKind(PropOrdinal eProperty, PWORD pwDataKind);
	STDMETHODIMP	OrdGetDataKindAsStringA(PropOrdinal eProperty, ULONG cchBuffer, PCHAR pDest, PULONG pcchRequired);
	STDMETHODIMP	OrdGetDataKindAsStringW(PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pDest, PULONG pcchRequired);

	STDMETHODIMP	OrdInstantiateObjRef(PropOrdinal eProperty, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	OrdIterateObjRefArray(PropOrdinal eProperty, IOmfooIterator **ppIterator);
	STDMETHODIMP	OrdIterateMobIndexArray(PropOrdinal eProperty, IOmfooIterator **ppIterator);

	STDMETHODIMP	WrapObjRefArray(POMFOO_OBJREF_ARRAY pArray, IOmfooIterator **ppIterator);
	STDMETHODIMP	WrapMobIndexArray(POMFOO_MOB_INDEX_ARRAY pArray, IOmfooIterator **ppIterator);

	// General-purpose array routines. (Accepts the property ID argument as an enumerated PropOrdinal.)
	STDMETHODIMP	OrdGetArrayInfo(PropOrdinal eProperty, PDWORD pdwDataType, PDWORD pdwRefList, PULONG pnElements, PULONG pcbData);
	STDMETHODIMP	OrdAllocMobIndexArray(PropOrdinal eProperty, POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	OrdAllocObjRefArray(PropOrdinal eProperty, POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	OrdAllocPositionArray(PropOrdinal eProperty, POMFOO_POSITION_ARRAY* ppArray);
	STDMETHODIMP	FreeObjRefArray(POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	FreeMobIndexArray(POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	FreePositionArray(POMFOO_POSITION_ARRAY* ppArray);

protected:
	// Protected helper routines that simply forward the call back to the container.
	DWORD	GetWorkingCodePage(void);
	DWORD	OrdinalToPropertyID(PropOrdinal eProperty);

	// Pointer to CContainerLayer97 (and all lower layers) for internal call-back routines.
	CContainerLayer97*	m_pContainer;

	// Pointer to the COmfObject who owns us. (Optional)
	COmfObject*	m_pParent;

	// Local copies copied from CReadOmf for convenience. 
	BOOL		m_fOmfVer1;
	BOOL		m_fOmfBigEndian;

	// We initialize this to FALSE and never touch it again.
	// Our subclasses can use this to initialize themselves on the very first call to QueryInterface()
	// queries for their specialized interface.
	BOOL		m_fInitialized;

private:
	LONG		m_cRefs;
};
