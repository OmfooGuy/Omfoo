// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer01.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer00.h"

#pragma pack(push, 1)		// Explicitly align the members of these structures to byte boundaries!

//	Our internal structure for the data type omfi:MobIndex.
//	Note that this is _not_ the exact way it is actually stored in the file.
//	The difference is that when it is stored in the file nElements member is a USHORT.
//	This is why an omfi:MobIndex can never have more than 65535 elements.
typedef struct {
	ULONG	nElements;							// number of OMF1_MOB_INDEX_ENTRY structures in the array
	DWORD	dwRefList;							// see CM_StdObjID_ObjReferences and CM_StdObjID_ObjRefData in Bento spec.
	OMF1_MOB_INDEX_ENTRY	a[ANYSIZE_ARRAY];	// See OMF_MOB_INDEX_ENTRY above.
} OMFOO_MOB_INDEX_ARRAY, *POMFOO_MOB_INDEX_ARRAY;

//	Our internal structure for the data type omfi:ObjRefArray.
//	Note that this is _not_ the exact way it is actually stored in the file.
//	The difference is that when it is stored in the file nElements member is a USHORT.
//	This is why an omfi:ObjRefArray can never have more than 65535 elements.
typedef struct {
	ULONG	nElements;			// number of object IDs in the array
	DWORD	dwRefList;			// see CM_StdObjID_ObjReferences and CM_StdObjID_ObjRefData in Bento spec.
	DWORD	a[ANYSIZE_ARRAY];	// variable-length array of object IDs
} OMFOO_OBJREF_ARRAY, *POMFOO_OBJREF_ARRAY;

//	Internal structure for the data type omfi:Position32Array and omfi:Position64Array.
//	When the data type is omfi:Position32Array the 32-bit values are promoted to UINT64s.
typedef struct {
	ULONG	nElements;			// number of 'positions' in the array
	DWORD	dwRefList;			// see CM_StdObjID_ObjReferences and CM_StdObjID_ObjRefData in Bento spec.
	UINT64	a[ANYSIZE_ARRAY];	// array of UNSIGNED 64-bit 'positions'
} OMFOO_POSITION_ARRAY, *POMFOO_POSITION_ARRAY;

//	This structure represents one entry in a OMFOO_REFERENCE_LIST.
//	The OMFOO_REFERENCE_LIST structure is an array of these. See below.
typedef struct {
	DWORD		dwKey;
	DWORD		dwObject;
} OMFOO_REFERENCE_LIST_ENTRY, *POMFOO_REFERENCE_LIST_ENTRY;

//	Internal structure for the property CM_StdObjID_ObjReferences, with the data type CM_StdObjID_ObjRefData.
typedef struct {
	ULONG	nElements;			// number of elements in the array
	DWORD	dwReserved;			//
	OMFOO_REFERENCE_LIST_ENTRY	a[ANYSIZE_ARRAY];
} OMFOO_REFERENCE_LIST, *POMFOO_REFERENCE_LIST;
#pragma pack(pop)

//*********************************************************************************************************************
//
//*********************************************************************************************************************
class CContainerLayer01 : public CContainerLayer00
{
protected:
			CContainerLayer01(void);
	virtual	~CContainerLayer01(void);

public:
	STDMETHODIMP	CoreIsPropertyPresent(BENTO_BLOP& rBlop, DWORD dwProperty);
	STDMETHODIMP	CoreGetPropInfo(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload);
	STDMETHODIMP	CoreReadStrict(BENTO_BLOP& rBlop, DWORD dwProperty, DWORD dwRequestedType, ULONG cbBuffer, PVOID pBuffer);

	STDMETHODIMP	CoreReadRawBytes(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	CoreReadStringA(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	CoreReadStringW(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	CoreReadBoolean(BENTO_BLOP& rBlop, DWORD dwProperty, PBOOLEAN pBoolean);
	STDMETHODIMP	CoreReadInt8(BENTO_BLOP& rBlop, DWORD dwProperty, PINT8 pn8);
	STDMETHODIMP	CoreReadInt16(BENTO_BLOP& rBlop, DWORD dwProperty, PINT16 pn16);
	STDMETHODIMP	CoreReadInt32(BENTO_BLOP& rBlop, DWORD dwProperty, PINT32 pn32);
	STDMETHODIMP	CoreReadInt64(BENTO_BLOP& rBlop, DWORD dwProperty, PINT64 pn64);
	STDMETHODIMP	CoreReadUInt8(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT8 pn8);
	STDMETHODIMP	CoreReadUInt16(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT16 pu16);
	STDMETHODIMP	CoreReadUInt32(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT32 pu32);
	STDMETHODIMP	CoreReadUInt64(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT64 pu64);

	STDMETHODIMP	CoreReadFourCC(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwFourCC);
	STDMETHODIMP	CoreReadLength64(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT64 pLength);
	STDMETHODIMP	CoreReadPosition64(BENTO_BLOP& rBlop, DWORD dwProperty, PINT64 pPosition);
	STDMETHODIMP	CoreReadDataOffset64(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT64 pcbOffset);
	STDMETHODIMP	CoreReadCompCodeArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_COMP_CODE_ARRAY pArray);
	STDMETHODIMP	CoreReadCompSizeArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_COMP_SIZE_ARRAY pArray);

	STDMETHODIMP	CoreReadInt32Array(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	CoreReadInt64Array(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);
	STDMETHODIMP	CoreReadPosition32Array(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements);
	STDMETHODIMP	CoreReadPosition64Array(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements);

	STDMETHODIMP	CoreReadGuid(BENTO_BLOP& rBlop, DWORD dwProperty, LPGUID pGuid);
	STDMETHODIMP	CoreReadFirstMobID(BENTO_BLOP& rBlop, POMF_MOB_ID pMobID);
	STDMETHODIMP	CoreReadMobID(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_MOB_ID pMobID);
	STDMETHODIMP	CoreReadObjRef(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwRefList, PDWORD pdwObjRef);
	STDMETHODIMP	CoreReadPoint(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_POINT pPoint);
	STDMETHODIMP	CoreReadProductVersion(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	CoreReadRational(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_RATIONAL pRational);
	STDMETHODIMP	CoreReadRationalAsFloat(BENTO_BLOP& rBlop, DWORD dwProperty, PFLOAT pFloat);
	STDMETHODIMP	CoreReadRationalAsDouble(BENTO_BLOP& rBlop, DWORD dwProperty, DOUBLE* pDouble);

	STDMETHODIMP	CoreReadExtended80AsDouble(BENTO_BLOP& rBlop, DWORD dwProperty, DOUBLE* pDouble);
	STDMETHODIMP	CoreReadTimeStamp(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_TIMESTAMP pTimeStamp);
	STDMETHODIMP	CoreReadTimeStampAsFileTime(BENTO_BLOP& rBlop, DWORD dwProperty, LPFILETIME pFileTime);
	STDMETHODIMP	CoreReadVersionType(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_VERSIONTYPE pVersionType);

	STDMETHODIMP	CoreGetDataKindAsWord(BENTO_BLOP& rBlop, DWORD dwProperty, PWORD pwDataKind);
	STDMETHODIMP	CoreGetDataKindAsStringA(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	CoreGetDataKindAsStringW(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	CoreExtractDataToFile(BENTO_BLOP& rBlop, DWORD dwProperty, PCWSTR pwzFullPath, BOOL fOverwrite);

	STDMETHODIMP	CoreQueryFirstDataValue(BENTO_BLOP& rBlop, BOOL fImmediate, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	CoreQueryDataValue(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_DATA_VALUE pDataValue);
	STDMETHODIMP	CoreReadDataValueAsRawBytes(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired);
	STDMETHODIMP	CoreReadDataValueAsStringW(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	CoreCreateStreamOnDataValue(BENTO_BLOP& rBlop, DWORD dwProperty, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	CoreCreateHGlobalOnDataValue(BENTO_BLOP& rBlop, DWORD dwProperty, UINT uFlags, HGLOBAL* phGlobal);

	// General-purpose array routines. (Accepts the property ID argument as a DWORD.)
	STDMETHODIMP	CoreGetArrayInfo(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwStoredType, PDWORD pdwRefList, PULONG pnElements, PULONG pcbData);
	STDMETHODIMP	CoreAllocMobIndexArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	CoreFreeMobIndexArray(POMFOO_MOB_INDEX_ARRAY* ppArray);
	STDMETHODIMP	CoreAllocObjRefArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	CoreFreeObjRefArray(POMFOO_OBJREF_ARRAY* ppArray);
	STDMETHODIMP	CoreAllocPositionArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMFOO_POSITION_ARRAY* ppArray);
	STDMETHODIMP	CoreFreePositionArray(POMFOO_POSITION_ARRAY* ppArray);
	STDMETHODIMP	CoreAllocReferenceList(BENTO_BLOP& rBlop, POMFOO_REFERENCE_LIST* ppList);
	STDMETHODIMP	CoreFreeReferenceList(POMFOO_REFERENCE_LIST* ppList);

	// Array verification.
	BOOL	IsBadMobIndexArray(POMFOO_MOB_INDEX_ARRAY pArray);
	BOOL	IsBadObjRefArray(POMFOO_OBJREF_ARRAY pArray);
	BOOL	IsBadPositionArray(POMFOO_POSITION_ARRAY pArray);
	BOOL	IsBadReferenceList(POMFOO_REFERENCE_LIST pList);

	// Attributes routines.
	STDMETHODIMP	CoreFindAttributeByName(POMFOO_OBJREF_ARRAY pArray,
												LPCSTR pszAttributeName, BOOL fRecurse, DWORD& rATTB, WORD& rKind);

	STDMETHODIMP	CoreFindIntAttributeByName(POMFOO_OBJREF_ARRAY pArray,
													LPCSTR pszAttributeName, BOOL fRecurse, PINT32 pn32);

	STDMETHODIMP	CoreFindObjectAttributeByName(POMFOO_OBJREF_ARRAY pArray,
													LPCSTR pszAttributeName, BOOL fRecurse, PDWORD pdwObject);

	STDMETHODIMP	CoreFindDataAttributeByName(POMFOO_OBJREF_ARRAY pArray,
													LPCSTR pszAttributeName, BOOL fRecurse,
																POMF_DATA_VALUE pDataValue);

	STDMETHODIMP	CoreFindStringAttributeByNameA(POMFOO_OBJREF_ARRAY pArray,
													LPCSTR pszAttributeName, BOOL fRecurse,
														ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired);

	STDMETHODIMP	CoreFindStringAttributeByNameW(POMFOO_OBJREF_ARRAY pArray,
													LPCSTR pszAttributeName, BOOL fRecurse,
														ULONG cchBuffer, LPWSTR pBuffer, PULONG pcchRequired);

protected:
	ULONG	m_nMobIndexArrays;		// number of unmatched calls to AllocMobIndexArray() and FreeMobIndexArray()
	ULONG	m_nObjRefArrays;		// number of unmatched calls to AllocObjRefArray() and FreeObjRefArray()
	ULONG	m_nPositionArrays;		// number of unmatched calls to AllocPositionArray() and FreePositionArray()
	ULONG	m_nReferenceLists;		// number of unmatched calls to CoreAllocReferenceList() and CoreFreeReferenceList()
};
