// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer01.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer01.h"
#include "DllMain.h"
#include "StreamOnReadableFile.h"
#include "StreamOnRawBytes.h"
#include <shlwapi.h>

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer01::CContainerLayer01(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer01::~CContainerLayer01(void)
{
	ASSERT(0 == m_nMobIndexArrays);
	ASSERT(0 == m_nObjRefArrays);
	ASSERT(0 == m_nPositionArrays);
	ASSERT(0 == m_nReferenceLists);
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer01::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//	if (SUCCEEDED(hr))
//	{
//		hr = ????????????
//	}
//	return hr;
//}

//*********************************************************************************************************************
//	Test if a given property is present in the object.
//	Returns S_OK if the object contains the property specified by dwProperty, or OMF_E_PROP_NOT_FOUND if it does not.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreIsPropertyPresent(BENTO_BLOP& rBlop, DWORD dwProperty)
{
	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Validate caller's dwProperty argument.
	if (0 == dwProperty)
	{
		return OMF_E_PROP_NOT_DEFINED;
	}

	// Loop through each TOCX_ITEM structure belonging to the blop until we find one whose pCurItem->dwProperty
	// matches caller's dwProperty argument, or until there are no items left in the blop.
	PTOCX_ITEM	pCurItem	= &m_aToc[rBlop.iFirstItem];
	PTOCX_ITEM	pStopItem	= &pCurItem[rBlop.wTotalItems];
	do
	{
		if (pCurItem->dwProperty == dwProperty)
		{
			// Found it!
			return S_OK;
		}
	} while (++pCurItem < pStopItem);

	return OMF_E_PROP_NOT_FOUND;
}

//*********************************************************************************************************************
//	Retrieves information about the specified property in the specified object.
//	The data size is measured in bytes and returned as a UINT64.
//	The number of fragments is returned as a ULONG.
//	The data type is returned as a DWORD.
//	If dwProperty is zero it returns OMF_E_PROP_NOT_DEFINED.
//	If the data type is zero it returns OMF_E_TYPE_SURPRISE.
//	"Although Bento allows a single property to have multiple types and a value for each type,
//	OMF Interchange allows only a single type and value for each property in an object."
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreGetPropInfo(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwStoredType,
														PDWORD pdwRefList, PULONG pnFragments, PUINT64 pcbPayload)
{
	HRESULT		hr = E_UNEXPECTED;
	PTOCX_ITEM	pItem			= NULL;	// dynamic pointer to the current TOCX_ITEM under scrutiny
	ULONG		nItems			= 0;	// number of contiguous TOC items associated with this property
	DWORD		dwRefList		= 0;
	DWORD		dwStoredType	= 0;	// the data type as it is stored in the first TOCX_ITEM
	ULONG		nFragments		= 0;	// number of fragments holding data.
	UINT64		cbAccumulate	= 0;	// running total of the size. The sum of all fragments.

	// Wipe caller's return values.
	if (pdwStoredType)
	{
		if (IsBadWritePointer(pdwStoredType, sizeof(DWORD)))
		{return E_POINTER;}
		*pdwStoredType	= 0;
	}

	if (pdwRefList)
	{
		if (IsBadWritePointer(pdwRefList, sizeof(DWORD)))
		{return E_POINTER;}
		*pdwRefList	= 0;
	}

	if (pnFragments)
	{
		if (IsBadWritePointer(pnFragments, sizeof(ULONG)))
		{return E_POINTER;}
		*pnFragments	= 0;
	}

	if (pcbPayload)
	{
		if (IsBadWritePointer(pcbPayload, sizeof(UINT64)))
		{return E_POINTER;}
		*pcbPayload	= 0;
	}

	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_Exit;
	}

	// Validate caller's dwProperty argument.
	if (0 == dwProperty)
	{
		hr = OMF_E_PROP_NOT_DEFINED;
		goto L_Exit;
	}

	// Get a pointer to the first TOCX_ITEM structure for this blop.
	pItem	= &m_aToc[rBlop.iFirstItem];

	// Get the total number of items for this blop.
	nItems	= rBlop.wTotalItems;

	// Loop through each TOCX_ITEM structure belonging to the blop until we find one whose pItem->dwProperty
	// matches caller's dwProperty argument, or until there are no items left. (Skip reference list IDs)
	while (nItems)
	{
		if (pItem->dwProperty == dwProperty)
		{
			if (pItem->bStorageMode == SM_REFLISTID)
			{
				// The ID of a bookkeeping object associated with this value.
				// Occurs before any data references.
				// Omitted if the value does not have a bookkeeping object.
				dwRefList = pItem->dwRefList;
			}
			else
			{
				// Found it!
				// Jump out of this loop and go do more stuff.
				goto L_PropExists;
			}
		}
		else
		{
			dwRefList = 0;
		}

		// The current item is not the requested property.
		// Advance pointer to the next item and decrement the loop count.
		++pItem;
		--nItems;
	}

	// The specified blop does not have the specified property.
	hr = OMF_E_PROP_NOT_FOUND;
	goto L_Exit;

L_PropExists:
	// We found the property!
	// Now get its data type.
	dwStoredType = pItem->dwDataType;

	// Make sure dwStoredType is non-zero.
	// This should NEVER be zero! And if it is then something is SERIOUSLY messed up!
	if (dwStoredType == 0)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_Exit;
	}

	// Loop to ...
	do
	{
		// Make sure our intermediate math will not result in overflow.
		// The payload can never be larger than the file!
		if (cbAccumulate > m_cbVirtualEndOfFile64)
		{
			BREAK_IF_DEBUG	// Has this ever happened?
			return OMF_E_SIZE_SURPRISE;
		}

		// Accumulate byte size.
		cbAccumulate += pItem->cbLength64;

		// Increment the fragment counter.
		++nFragments;

		// Is this the final (and probably the only) fragment?
		if (!pItem->fContinued)
		{
			// We are done!
			// Save caller's return values.
			// This is the normal/ expected way to terminate this loop.
			if (pdwStoredType)	*pdwStoredType	= dwStoredType;
			if (pdwRefList)		*pdwRefList		= dwRefList;
			if (pnFragments)	*pnFragments	= nFragments;
			if (pcbPayload)		*pcbPayload		= cbAccumulate;

			hr = S_OK;
			goto L_Exit;
		}

		// Make sure more items remain.
		// Decrement the item count.
		// Advance pointer to the next TOCX_ITEM structure.
		// Make sure the next item is a continuation of the same property,
		// and then make sure it has the same data type.
	} while ((--nItems) && ((++pItem)->dwProperty == dwProperty) && (pItem->dwDataType == dwStoredType));

	// This should never happen.
	// If loop terminates here it's because something in our code went wrong!
	BREAK_IF_DEBUG	// Has this ever happened?
	hr =  OMFOO_E_ASSERTION_FAILURE;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Internal read operation.
//	The dwRequestedType argument is optional. Use (-1) as a wildcard. Do not use zero or this routine will return OMF_E_TYPE_NOT_DEFINED.
//	It will fail with OMF_E_TYPE_SURPRISE if the data type in the TOC item does not match dwRequestedType.
//	If you don't know the exact size of the data then call CoreGetDataSize() first.
//	All arguments are all REQUIRED. The size of the payload must match cbBuffer - EXACTLY.
//	Note that this routine can and will read FRAGMENTED PAYLOADS.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadStrict(BENTO_BLOP& rBlop, DWORD dwProperty, DWORD dwRequestedType, ULONG cbBuffer, PVOID pBuffer)
{
	PTOCX_ITEM	pItem			= NULL;	// dynamic pointer to the current TOCX_ITEM under scrutiny
	ULONG		nItems			= 0;	// number of contiguous TOC items (aka fragments) associated with this property
	DWORD		dwStoredType	= 0;	// the data type as it is stored in the first TOCX_ITEM

	HRESULT		hr				= OMF_E_PROP_NOT_FOUND;
	ULONG		cbConsumed		= 0;
	ULONG		cbRemaining		= cbBuffer;

	// Validate caller's destination pointer.
	if (IsBadWritePointer(pBuffer, cbBuffer))
	{
		return E_POINTER;
	}

	// Wipe caller's destination buffer.
	SecureZeroMemory(pBuffer, cbBuffer);

	// Validate caller's rBlop argument by making sure the dwObject is non-zero.
	// Note that Blops with zero-value dwObjects indicate that a previous call to one of our own object
	// 'discovery' routines has failed to find the requested object. Nevertheless this is normal and expected.
	// This is by design so that CReadOmf::GetBlop() will always succeed.
	// We do this to facilitate program flow by deferring error handling.
	// See CReadOmf::GetBlop() and CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Validate caller's dwProperty argument.
	if (0 == dwProperty)
	{
		return OMF_E_PROP_NOT_DEFINED;
	}

	// Validate caller's dwRequestedType argument.
	if (0 == dwRequestedType)
	{
		return OMF_E_TYPE_NOT_DEFINED;
	}

	// Get a pointer to the first TOCX_ITEM structure for this blop.
	pItem	= &m_aToc[rBlop.iFirstItem];

	// Get the total number of items for this blop.
	nItems	= rBlop.wTotalItems;

	// Loop through each TOCX_ITEM structure belonging to the object until we find one whose pItem->dwProperty
	// matches caller's dwProperty argument, or until there are no items left. (Skip reference list IDs)
	while (nItems)
	{
		if ((pItem->dwProperty == dwProperty) && (pItem->bStorageMode != SM_REFLISTID))
		{
			// Found it!
			// Jump out of this loop and go do more stuff.
			goto L_PropExists;
		}

		// The current item is not the requested property.
		// Advance pointer to the next item and decrement the loop count.
		++pItem;
		--nItems;
	}

	// The specified blop does not have the specified property.
	return OMF_E_PROP_NOT_FOUND;

L_PropExists:
	// We found the property!
	// Now get its data type.
	dwStoredType = pItem->dwDataType;

	// Make sure dwStoredType is non-zero.
	// This should NEVER be zero, and if it is then something is SERIOUSLY messed up!
	if (dwStoredType == 0)
	{
		return OMF_E_TYPE_SURPRISE;
	}

	// Compare stored type with caller's requested data type. Note that (-1) is a wildcard.
	if ((dwStoredType != dwRequestedType) && (dwRequestedType != (-1)))
	{
		return OMF_E_TYPE_SURPRISE;
	}

	// Loop to read bytes ...
	do
	{
		if (pItem->bStorageMode == SM_IMMEDIATE)
		{
			if (pItem->cbLength64 > 4)
			{
				BREAK_IF_DEBUG	// Has this ever happened?
				hr =  OMF_E_STORAGE_SURPRISE;
				goto L_CleanupExit;
			}

			if (pItem->cbLength64 > cbRemaining)
			{
				BREAK_IF_DEBUG	// Has this ever happened?
				hr =  OMF_E_SIZE_SURPRISE;
				goto L_CleanupExit;
			}

			// Copy the data from TOC memory to caller's buffer.
			CopyMemory(&PBYTE(pBuffer)[cbConsumed], &pItem->aImmediateBytes, pItem->cbLengthLo);
		}
		else if (pItem->bStorageMode == SM_OFFSET)
		{
			if (pItem->cbLength64 > cbRemaining)
			{
				BREAK_IF_DEBUG	// Has this ever happened?
				hr =  OMF_E_SIZE_SURPRISE;
				goto L_CleanupExit;
			}

			if (FAILED(hr = SeekRead(pItem->cbOffset64, &PBYTE(pBuffer)[cbConsumed], pItem->cbLengthLo)))
			{
				BREAK_IF_DEBUG	// Has this ever happened?
				goto L_CleanupExit;
			}
		}
		else
		{
			// We can't read the data because we don't know how it is stored ...
			// This can happen when pItem->bStorageMode is SM_CACHED.
			BREAK_IF_DEBUG
			hr = OMF_E_STORAGE_SURPRISE;
			goto L_CleanupExit;
		}

		// Update byte counters. We can use 32-bit math (cbLengthLo) because we know that pItem->cbLengthHi is zero.
		cbConsumed	+= pItem->cbLengthLo;
		cbRemaining	-= pItem->cbLengthLo;

		// Reality check for underflow.
		// This can only happen if there is a bug in our code.
		if (LONG(cbRemaining) < 0)
		{
			BREAK_IF_DEBUG
			hr = OMFOO_E_ASSERTION_FAILURE;
			goto L_CleanupExit;
		}

		// Is this the final (or the only) fragment?
		if (!pItem->fContinued)
		{
			// Have we read all bytes?
			if (0 == cbRemaining)
			{
				// We are done.
				// This is the normal/ expected way to terminate this loop.
				hr = S_OK;
				goto L_Exit;
			}

			// We hit the last fragment but we haven't read enough bytes!
			BREAK_IF_DEBUG	// Has this ever happened?
			hr = OMF_E_SIZE_SURPRISE;
			goto L_CleanupExit;
		}

	// Make sure more items remain.
	// Advance pointer to the next TOCX_ITEM structure.
	// Make sure the next item is a continuation of the same property,
	// and then make sure it has the same data type.
	} while ((--nItems) && ((++pItem)->dwProperty == dwProperty) && (pItem->dwDataType == dwStoredType));

	// This should never happen.
	// If loop terminates here it's because something in our code went wrong!
	BREAK_IF_DEBUG	// Has this ever happened?
	hr =  OMFOO_E_ASSERTION_FAILURE;

L_CleanupExit:
	// Wipe caller's buffer again because something failed.
	SecureZeroMemory(pBuffer, cbBuffer);

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Generic read routine. Retrieves raw, unprocessed bytes, as specified by dwProperty.
//	The pBuffer and pcbRequired arguments are both optional, but you must provide at least one.
//	The OMF data type is not tested. It is up to you to know what you are reading and to byte-swap it if necessary.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL, to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the bytes.
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is not large enough to hold the bytes.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadRawBytes(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cbBuffer, PVOID pBuffer, PULONG pcbRequired)
{
	UINT64	cbPayload		= 0;
	DWORD	dwStoredType	= 0;
	HRESULT	hr				= E_UNEXPECTED;

	// Our caller must supply at least one of these arguments.
	if ((NULL == pBuffer) && (NULL == pcbRequired))
	{
		hr = E_INVALIDARG;
		goto L_Exit;
	}

	// The pBuffer argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pBuffer)
	{
		if (IsBadWritePointer(pBuffer, cbBuffer))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe it.
		ZeroMemory(pBuffer, cbBuffer);
	}
	else
	{
		// If pBuffer is NULL then make sure cbBuffer is zero.
		// We need this internally.
		cbBuffer = 0;
	}

	// The pcbRequired argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pcbRequired)
	{
		if (IsBadWritePointer(pcbRequired, sizeof(ULONG)))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe it.
		*pcbRequired = 0;
	}

	// Get the payload data type and size. On exit dwStoredType will always be non-zero.
	if (FAILED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, &cbPayload)))
	{
		goto L_Exit;
	}

	if (cbPayload > ULONG(-1))
	{
		BREAK_IF_DEBUG	// Has this ever happened?
		hr = OMF_E_SIZE_SURPRISE;
		goto L_Exit;
	}

	// Give cbRequired to caller if they want it.
	if (pcbRequired)
	{
		*pcbRequired = ULONG(cbPayload);
	}

	// Fail if cbPayload is larger than caller's destination buffer, or if pBuffer is NULL.
	if ((cbPayload > cbBuffer)  || (pBuffer == NULL))
	{
		hr = OMF_E_INSUFFICIENT_BUFFER;
		goto L_Exit;
	}

	hr = CoreReadStrict(rBlop, dwProperty, (-1), ULONG(cbPayload), pBuffer);

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	8-bit string read routine.
//	The data type must be omfi:String, omfi:UniqueName, CM_StdObjID_7BitASCII, or CM_StdObjID_8BitASCII.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadStringA(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer,
															PCHAR pBuffer, PULONG pcchRequired)
{
	PTOCX_ITEM	pTocItem	= NULL;
	HRESULT		hr			= FindTocItemForProperty(rBlop, dwProperty, pTocItem);
	if (FAILED(hr))
	{
		return CallTwiceStringHandlerA(hr, NULL, cchBuffer, pBuffer, pcchRequired);
	}

	// Is the data type a byte-oriented string?
	if ((pTocItem->dwDataType != m_dwTypeString) &&			// omfi:String
		(pTocItem->dwDataType != m_dwTypeUniqueName) &&		// omfi:UniqueName
		(pTocItem->dwDataType != CM_StdObjID_7BitASCII) &&	// Bento 7-bit ASCII [Pre-defined data type #21]
		(pTocItem->dwDataType != CM_StdObjID_8BitASCII))	// Bento 8-bit ASCII [Pre-defined data type #22]
	{
		// The data type is not a byte-oriented string.
		return CallTwiceStringHandlerA(OMF_E_TYPE_SURPRISE, NULL, cchBuffer, pBuffer, pcchRequired);
	}

	if (pTocItem->bStorageMode == SM_CACHED)
	{
		return CallTwiceStringHandlerA(S_OK, pTocItem->pCachedString, cchBuffer, pBuffer, pcchRequired);
	}

	if (pTocItem->bStorageMode == SM_IMMEDIATE)
	{
		if (pTocItem->cbLength64 <= 4)
		{
			CHAR szMultiByte[8] = {0};
			CopyMemory(szMultiByte, &pTocItem->dwImmediateDword, pTocItem->cbLengthLo);
			return CallTwiceStringHandlerA(S_OK, szMultiByte, cchBuffer, pBuffer, pcchRequired);
		}
		else
		{
			BREAK_IF_DEBUG
			// The size of the payload is unexpectedly huge.
			return CallTwiceStringHandlerA(OMF_E_SIZE_SURPRISE, NULL, cchBuffer, pBuffer, pcchRequired);
		}
	}

	if (pTocItem->bStorageMode == SM_OFFSET)
	{
		CHAR szMultiByte[OMFOO_STRMAX_STRING]	= {0};
		UINT32 cbRequest = pTocItem->cbLengthLo;
		if (cbRequest >= ELEMS(szMultiByte))
		{
			cbRequest = ELEMS(szMultiByte)-1;
		}
		hr = SeekRead(pTocItem->cbOffset64, szMultiByte, cbRequest);
		return CallTwiceStringHandlerA(hr, szMultiByte, cchBuffer, pBuffer, pcchRequired);
	}

	// We always call this even when we fail because it manages and validates our caller's arguments.
	return CallTwiceStringHandlerA(OMF_E_STORAGE_SURPRISE, NULL, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Wide string read routine.
//	The data type must be omfi:String, omfi:UniqueName, CM_StdObjID_7BitASCII, or CM_StdObjID_8BitASCII.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadStringW(BENTO_BLOP& rBlop, DWORD dwProperty, ULONG cchBuffer,
															PWCHAR pBuffer, PULONG pcchRequired)
{
	WCHAR	wzWideChar[OMFOO_STRMAX_STRING]		= {0};
	CHAR	szMultiByte[OMFOO_STRMAX_STRING]	= {0};
	LPCSTR		pszSrc		= NULL;
	PTOCX_ITEM	pTocItem	= NULL;
	DWORD		dwCodePage	= 0;
	HRESULT		hr			= FindTocItemForProperty(rBlop, dwProperty, pTocItem);
	if (FAILED(hr))
	{
		// This isn't necessarily an unexpected error.
		goto L_Finalize;
	}

	// Is the data type a byte-oriented string?
	if ((pTocItem->dwDataType == m_dwTypeString) ||			// omfi:String
		(pTocItem->dwDataType == CM_StdObjID_8BitASCII))	// Bento 8-bit ASCII [Pre-defined data type #22]
	{
		dwCodePage = m_dwWorkingCodePage;
	}
	else if ((pTocItem->dwDataType == m_dwTypeUniqueName) ||	// omfi:UniqueName
			(pTocItem->dwDataType == CM_StdObjID_7BitASCII))	// Bento 7-bit ASCII [Pre-defined data type #21]
	{
		dwCodePage = CP_ASCII;
	}
	else
	{
		// The data type is not a byte-oriented string.
		hr = OMF_E_TYPE_SURPRISE;
		BREAK_IF_DEBUG
		goto L_Finalize;
	}

	if (pTocItem->bStorageMode == SM_CACHED)
	{
		pszSrc = pTocItem->pCachedString;
		goto L_Convert;
	}

	if (pTocItem->bStorageMode == SM_IMMEDIATE)
	{
		CopyMemory(szMultiByte, &pTocItem->dwImmediateDword, pTocItem->cbLengthLo);
		pszSrc = szMultiByte;
		goto L_Convert;
	}

	if (pTocItem->bStorageMode == SM_OFFSET)
	{
		UINT32 cbRequest = pTocItem->cbLengthLo;
		if (cbRequest >= ELEMS(szMultiByte))
		{
			cbRequest = ELEMS(szMultiByte)-1;
		}
		if (FAILED(hr = SeekRead(pTocItem->cbOffset64, szMultiByte, cbRequest)))
		{
			BREAK_IF_DEBUG
			goto L_Finalize;
		}
		else
		{
			pszSrc = szMultiByte;
			goto L_Convert;
		}
	}

	hr = OMF_E_STORAGE_SURPRISE;
	BREAK_IF_DEBUG
	goto L_Finalize;

L_Convert:
	#ifdef _DEBUG
	const DWORD dwFlags = MB_ERR_INVALID_CHARS;
	#else
	const DWORD dwFlags = 0;
	#endif

	if (MultiByteToWideChar(dwCodePage, dwFlags, pszSrc, INT(-1), wzWideChar, ELEMS(wzWideChar)))
	{
		hr = S_OK;
	}
	else
	{
		BREAK_IF_DEBUG
		hr = HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION);
	}

L_Finalize:
	// We always call this even when we fail because it manages and validates our caller's arguments.
	return CallTwiceStringHandlerW(hr, wzWideChar, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Data type must be omfi:Boolean.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadBoolean(BENTO_BLOP& rBlop, DWORD dwProperty, PBOOLEAN pBoolean)
{
	BOOLEAN bTemporary	= FALSE;

	// The pBoolean argument is optional, but if our caller supplied one then we need to make sure that
	// we can write to it.
	if (pBoolean)
	{
		if (IsBadWritePointer(pBoolean, sizeof(BOOLEAN)))
		{
			return E_POINTER;
		}

		*pBoolean = 0;
	}

	HRESULT hr = CoreReadStrict(rBlop, dwProperty, OrdinalToDataTypeID(eTypeBoolean), sizeof(BOOLEAN), &bTemporary);

	if (SUCCEEDED(hr))
	{
		if (pBoolean)
		{
			*pBoolean = bTemporary ? TRUE : FALSE;
		}

		hr = bTemporary ? S_OK : S_FALSE;
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadInt8(BENTO_BLOP& rBlop, DWORD dwProperty, PINT8 pn8)
{
	return CoreReadStrict(rBlop, dwProperty, (-1), sizeof(INT8), pn8);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadInt16(BENTO_BLOP& rBlop, DWORD dwProperty, PINT16 pn16)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(INT16), pn16);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		*pn16 = Endian16(*pn16);
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadInt32(BENTO_BLOP& rBlop, DWORD dwProperty, PINT32 pn32)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(INT32), pn32);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		*pn32 = Endian32(*pn32);
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadInt64(BENTO_BLOP& rBlop, DWORD dwProperty, PINT64 pn64)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(INT64), pn64);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		*pn64 = Endian64(*pn64);
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadUInt8(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT8 pu8)
{
	return CoreReadStrict(rBlop, dwProperty, (-1), sizeof(UINT8), pu8);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadUInt16(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT16 pu16)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(UINT16), pu16);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		*pu16 = Endian16(*pu16);
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadUInt32(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT32 pu32)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(UINT32), pu32);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		*pu32 = Endian32(*pu32);
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadUInt64(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT64 pu64)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(UINT64), pu64);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		*pu64 = Endian64(*pu64);
	}

	return hr;
}

//*********************************************************************************************************************
//	There is no formal definition of a fourCC.
//	This definition of fourCC is very liberal.
//	It only has to consist of four printable ASCII chars.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadFourCC(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwFourCC)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(DWORD), pdwFourCC);
	if (SUCCEEDED(hr))
	{
		DWORD dwFourCC = *pdwFourCC;

		if ((dwFourCC < 0x20202020)||
			(dwFourCC > 0x7E7E7E7E)||
			(dwFourCC & 0x80808080))
		{
			hr = OMF_E_BAD_FOURCC;
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	Lengths are unsigned.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadLength64(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT64 pLength)
{
	HRESULT hr				= E_UNEXPECTED;
	UINT64	u64				= 0;
	UINT32	u32				= 0;
	DWORD	dwStoredType	= 0;

	// Validate caller's pointer.
	if (IsBadWritePointer(pLength, sizeof(UINT64)))
	{
		return E_POINTER;
	}

	// Wipe caller's return value.
	*pLength = 0;

	// Get the payload data type and size. On exit dwStoredType will always be non-zero.
	if (SUCCEEDED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, NULL)))
	{
		hr = OMF_E_TYPE_SURPRISE;

		if (m_fOmfVer1)
		{
			// Signed data types ...
			if ((dwStoredType == m_dwTypeInt32)			// omfi:Int32
				|| (dwStoredType == m_dwTypeLong)		// omfi:Long
				|| (dwStoredType == m_dwTypeUInt32)		// omfi:UInt32
				|| (dwStoredType == m_dwTypeULong))		// omfi:Ulong
			{
				if (SUCCEEDED(hr = CoreReadUInt32(rBlop, dwProperty, &u32)))
				{
					*pLength = u32;
				}
			}
		}
		else
		{
			if (dwStoredType == m_dwTypeLength32)		// omfi:Length32
			{
				if (SUCCEEDED(hr = CoreReadUInt32(rBlop, dwProperty, &u32)))
				{
					*pLength = u32;
				}
			}
			else
			{
				if (dwStoredType == m_dwTypeLength64)	// omfi:Length64
				{
					if (SUCCEEDED(hr = CoreReadUInt64(rBlop, dwProperty, &u64)))
					{
						*pLength = u64;
					}
				}
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Positions are signed.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadPosition64(BENTO_BLOP& rBlop, DWORD dwProperty, PINT64 pPosition)
{
	HRESULT hr			= E_UNEXPECTED;
	INT64	n64			= 0;
	UINT32	u32			= 0;
	INT32	n32			= 0;
	DWORD	dwStoredType	= 0;

	// Validate caller's pointer.
	if (IsBadWritePointer(pPosition, sizeof(INT64)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe caller's return value.
	*pPosition = 0;

	// Get the payload data type. On exit dwStoredType will always be non-zero.
	if (SUCCEEDED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, NULL)))
	{
		hr = OMF_E_TYPE_SURPRISE;

		if (m_fOmfVer1)
		{
			// There are no omfi:Position32 or omfi:Position64 data types in OMF1! So we fake it!
			// If the source is signed then we extend the sign bit.
			if ((dwStoredType == m_dwTypeInt32) || (dwStoredType == m_dwTypeLong))
			{
				if (SUCCEEDED(hr = CoreReadInt32(rBlop, dwProperty, &n32)))
				{
					*pPosition = n32;		// Promote 32-bit signed value to 64-bit signed
				}
			}
			else 
			{
				// But if the source is unsigned then we DO NOT extend the sign bit. 
				if ((dwStoredType == m_dwTypeUInt32) || (dwStoredType == m_dwTypeULong))
				{
					if (SUCCEEDED(hr = CoreReadUInt32(rBlop, dwProperty, &u32)))
					{
						*pPosition = INT64(UINT64(u32));
					}
				}
			}
		}
		else
		{
			if (dwStoredType == m_dwTypePosition32)
			{
				if (SUCCEEDED(hr = CoreReadInt32(rBlop, dwProperty, &n32)))
				{
					*pPosition = n32;	// promote 32-bit signed to 64-bit signed
				}
			}
			else
			{
				if (dwStoredType == m_dwTypePosition64)
				{
					if (SUCCEEDED(hr = CoreReadInt64(rBlop, dwProperty, &n64)))
					{
						*pPosition = n64;
					}
				}
			}
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	This is to retrieve properties like these ...
//
//		OMFI:ContainerOffsetAtClose, omfi:Long
//		OMFI:TOCOffsetAtClose, omfi:Long
//		OMFI:CDCI:OffsetToFrameIndexes, omfi:UInt32
//		OMFI:DIDD:DataOffset, omfi:Int32
//		OMFI:DIDD:DIDDFirstFrameOffset, omfi:Int32
//		OMFI:DIDD:FirstFrameOffset, omfi:Long, omfi:Int32
//		OMFI:DIDD:OffsetToRLEFrameIndexes, omfi:Long
//		OMFI:JPED:OffsetToFrameIndexes, omfi:Int64
//		OMFI:MDFL:dataOffset, omfi:Long
//		OMFI:MPGI:OffsetToFrameIndexes, omfi:Int64
//		OMFI:RLED:OffsetToFrameIndexes, omfi:UInt32
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadDataOffset64(BENTO_BLOP& rBlop, DWORD dwProperty, PUINT64 pcbOffset)
{
	HRESULT hr				= E_UNEXPECTED;
	UINT64	cbPayload		= 0;
	DWORD	dwStoredType	= 0;

	// Validate caller's pointer.
	if (IsBadWritePointer(pcbOffset, sizeof(UINT64)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe caller's return value.
	*pcbOffset = 0;

	// Get the payload size and data type. On exit dwStoredType will always be non-zero.
	if (FAILED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, &cbPayload)))
	{
		goto L_Exit;
	}

	if (cbPayload == sizeof(UINT64))
	{
		hr = CoreReadUInt64(rBlop, dwProperty, pcbOffset);
		goto L_Exit;
	}

	if (cbPayload == sizeof(UINT32))
	{
		UINT32 u32 = 0;
		if (SUCCEEDED(hr = CoreReadUInt32(rBlop, dwProperty, &u32)))
		{
			*pcbOffset = u32;
		}
		goto L_Exit;
	}

	// Unexpected data type.
	hr = OMF_E_TYPE_SURPRISE;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	omfi:CompCodeArray
//	An array of CHARs that specifies the order that the color components are stored in the palette.
//	Each element in the array represents a different color component.
//	The array is terminated by a 0 byte and has a maximum of 8 elements (including the terminating byte).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadCompCodeArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_COMP_CODE_ARRAY pArray)
{
	HRESULT		hr			= E_POINTER;
	PTOCX_ITEM	pTocItem	= NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(pArray, sizeof(OMF_COMP_CODE_ARRAY)))
	{
		goto L_Exit;
	}

	// Wipe caller's buffer.
	ZeroMemory(pArray, sizeof(OMF_COMP_CODE_ARRAY));

	if (FAILED(hr = FindTocItemForProperty(rBlop, dwProperty, pTocItem)))
	{
		goto L_Exit;
	}

	DWORD dwTypeCompCodeArray = OrdinalToDataTypeID(eTypeCompCodeArray);
	if (dwTypeCompCodeArray == 0)
	{
		hr = OMF_E_TYPE_NOT_DEFINED;
		goto L_Exit;
	}

	if (pTocItem->dwDataType != dwTypeCompCodeArray)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_Exit;
	}

	if (pTocItem->cbLength64 > sizeof(OMF_COMP_CODE_ARRAY))
	{
		hr = OMF_E_SIZE_SURPRISE;
		goto L_Exit;
	}

	switch (pTocItem->bStorageMode)
	{
	case SM_OFFSET:
		hr = SeekRead(pTocItem->cbOffset64, pArray, pTocItem->cbLengthLo);
		break;

	case SM_IMMEDIATE:
		CopyMemory(pArray, pTocItem->aImmediateBytes, pTocItem->cbLengthLo);
		hr = S_OK;
		break;

	default:
		hr = OMF_E_STORAGE_SURPRISE;
		break;
	}

	// Make sure the payload is null-terminated.
	if (SUCCEEDED(hr) && (pTocItem->cbLengthLo > 0))
	{
		UINT32 cbOffsetToTerminator = pTocItem->cbLengthLo-1;
		if (pArray[cbOffsetToTerminator] != 0)
		{
			BREAK_IF_DEBUG

			// Wipe caller's buffer (again).
			ZeroMemory(pArray, sizeof(OMF_COMP_CODE_ARRAY));
			hr = OMF_E_BAD_ARRAY;
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	omfi:CompSizeArray
//	An array of BYTEs that specifies the number of bits allocated to store each component in the order
//	specified in the PixelLayout property. The array is terminated by a 0 byte and has a maximum of 8
//	elements (including the terminating byte).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadCompSizeArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_COMP_SIZE_ARRAY pArray)
{
	HRESULT		hr			= E_POINTER;
	PTOCX_ITEM	pTocItem	= NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(pArray, sizeof(OMF_COMP_SIZE_ARRAY)))
	{
		goto L_Exit;
	}

	// Wipe caller's buffer.
	ZeroMemory(pArray, sizeof(OMF_COMP_SIZE_ARRAY));

	if (FAILED(hr = FindTocItemForProperty(rBlop, dwProperty, pTocItem)))
	{
		goto L_Exit;
	}

	DWORD dwTypeCompSizeArray = OrdinalToDataTypeID(eTypeCompSizeArray);
	if (dwTypeCompSizeArray == 0)
	{
		hr = OMF_E_TYPE_NOT_DEFINED;
		goto L_Exit;
	}

	if (pTocItem->dwDataType != dwTypeCompSizeArray)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_Exit;
	}

	if (pTocItem->cbLength64 > sizeof(OMF_COMP_SIZE_ARRAY))
	{
		hr = OMF_E_SIZE_SURPRISE;
		goto L_Exit;
	}

	switch (pTocItem->bStorageMode)
	{
	case SM_OFFSET:
		hr = SeekRead(pTocItem->cbOffset64, pArray, pTocItem->cbLengthLo);
		break;

	case SM_IMMEDIATE:
		CopyMemory(pArray, pTocItem->aImmediateBytes, pTocItem->cbLengthLo);
		hr = S_OK;
		break;

	default:
		hr = OMF_E_STORAGE_SURPRISE;
		break;
	}

	// Make sure the payload is null-terminated.
	if (SUCCEEDED(hr) && (pTocItem->cbLengthLo > 0))
	{
		UINT32 cbOffsetToTerminator = pTocItem->cbLengthLo-1;
		if (pArray[cbOffsetToTerminator] != 0)
		{
			BREAK_IF_DEBUG

			// Wipe caller's buffer (again).
			ZeroMemory(pArray, sizeof(OMF_COMP_CODE_ARRAY));
			hr = OMF_E_BAD_ARRAY;
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:Int32Array.
//	This has optional call-twice semantics. Make the first call with pBuffer set to NULL to get the required buffer
//	size (as measured in elements), and then call again with an appropriately sized buffer to retrieve the full array.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadInt32Array(BENTO_BLOP& rBlop, DWORD dwProperty,
												ULONG nMaxElements, PUINT32 pBuffer, PULONG pnActualElements)
{
	HRESULT	hr					= S_OK;
	ULONG	cbArrayData			= 0;
	DWORD	dwStoredType		= 0;
	UINT32	cbBuffer				= 0;
	ULONG	nElementsCalculated	= 0;

	// The pnActualElements pointer is`required.
	if (IsBadWritePointer(pnActualElements, sizeof(ULONG)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe this. Right here. Right now.
	*pnActualElements = 0;

	// Calculate the byte size of caller's destination buffer.
	cbBuffer = nMaxElements * sizeof(UINT32);

	// The pBuffer argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pBuffer)
	{
		if (IsBadWritePointer(pBuffer, cbBuffer))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe all of it. Right here. Right now.
		ZeroMemory(pBuffer, cbBuffer);
	}

	// Get array info.
	if (FAILED(hr = CoreGetArrayInfo(rBlop, dwProperty, &dwStoredType, NULL, &nElementsCalculated, &cbArrayData)))
	{
		goto L_Exit;
	}

	// If the data type must be omfi:Int32Array.
	if (dwStoredType != m_dwTypeInt32Array)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_Exit;
	}

	// Give the element count back to our caller.
	*pnActualElements = nElementsCalculated;

	// Fail if caller's buffer is too small to hold all of the UINT32s, or if there is no destination pointer.
	if ((nElementsCalculated > nMaxElements) || (pBuffer == NULL))
	{
		hr = OMF_E_INSUFFICIENT_BUFFER;
		goto L_Exit;
	}

	// Reality check.
	// If cbArrayData is greater than cbBuffer then our internal math is seriously messed up!
	if (cbArrayData > cbBuffer)
	{
		hr = OMFOO_E_ASSERTION_FAILURE;
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// Read the entire array directly into caller's buffer.
	if (SUCCEEDED(hr = CoreReadStrict(rBlop, dwProperty, dwStoredType, cbArrayData, pBuffer)))
	{
		// Byteswap the elements if necessary.
		if (m_fOmfBigEndian)
		{
			for (ULONG i = 0; i < nElementsCalculated; i++)
			{
				pBuffer[i] = Endian32(pBuffer[i]);
			}
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Data type can be omfi:Int32Array or omfi:Int64Array.
//	This has optional call-twice semantics. Make the first call with pBuffer set to NULL to get the required buffer
//	size (as measured in elements), and then call again with an appropriately sized buffer to retrieve the full array.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadInt64Array(BENTO_BLOP& rBlop, DWORD dwProperty,
												ULONG nMaxElements, PUINT64 pBuffer, PULONG pnActualElements)
{
	HRESULT	hr					= S_OK;
	ULONG	cbArrayData			= 0;
	DWORD	dwStoredType		= 0;
	UINT32	cbBuffer				= 0;
	ULONG	nElementsCalculated	= 0;

	// The pnActualElements pointer is`required.
	if (IsBadWritePointer(pnActualElements, sizeof(ULONG)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe this. Right here. Right now.
	*pnActualElements = 0;

	// Calculate the byte size of caller's destination buffer.
	cbBuffer = nMaxElements * sizeof(UINT64);

	// The pBuffer argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pBuffer)
	{
		if (IsBadWritePointer(pBuffer, cbBuffer))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe all of it. Right here. Right now.
		ZeroMemory(pBuffer, cbBuffer);
	}

	// Get array info.
	if (FAILED(hr = CoreGetArrayInfo(rBlop, dwProperty, &dwStoredType, NULL, &nElementsCalculated, &cbArrayData)))
	{
		goto L_Exit;
	}

	// If the data type is omfi:Int32Array.
	if (dwStoredType == m_dwTypeInt32Array)
	{
		// Give the element count back to our caller.
		*pnActualElements = nElementsCalculated;

		// Fail if caller's buffer is too small to hold all of the UINT64s, or if there is no destination pointer.
		if ((nElementsCalculated > nMaxElements) || (pBuffer == NULL))
		{
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Reality check.
		// If cbArrayData is greater than cbBuffer then our internal math is seriously screwed up!
		if ((cbArrayData *2) > cbBuffer)
		{
			hr = OMFOO_E_ASSERTION_FAILURE;
			BREAK_IF_DEBUG
			goto L_Exit;
		}

		// Read the entire array directly into caller's buffer.
		if (SUCCEEDED(hr = CoreReadStrict(rBlop, dwProperty, dwStoredType, cbArrayData, pBuffer)))
		{
			PUINT32 pSrc = PUINT32(pBuffer);
			ULONG i = nElementsCalculated;

			// Byteswap the elements if necessary.
			if (m_fOmfBigEndian)
			{
				while (i > 0)
				{
					i--;
					pBuffer[i] = Endian32(pSrc[i]);
				}
			}
			else
			{	
				while (i > 0)
				{
					i--;
					pBuffer[i] = pSrc[i];
				}
			}
		}
		goto L_Exit;
	}

	// If the data type is omfi:Int64Array.
	if (dwStoredType == m_dwTypeInt64Array)
	{
		// Give the element count back to our caller.
		*pnActualElements = nElementsCalculated;

		// Fail if caller's buffer is too small to hold all of the UINT64s, or if there is no destination pointer.
		if ((nElementsCalculated > nMaxElements) || (pBuffer == NULL))
		{
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Reality check.
		// If cbArrayData is greater than cbBuffer then our internal math is seriously screwed up!
		if (cbArrayData > cbBuffer)
		{
			hr = OMFOO_E_ASSERTION_FAILURE;
			BREAK_IF_DEBUG
			goto L_Exit;
		}

		// Read the entire array directly into caller's buffer.
		if (SUCCEEDED(hr = CoreReadStrict(rBlop, dwProperty, dwStoredType, cbArrayData, pBuffer)))
		{
			// Byteswap the elements if necessary.
			if (m_fOmfBigEndian)
			{
				for (ULONG i = 0; i < nElementsCalculated; i++)
				{
					pBuffer[i] = Endian64(pBuffer[i]);
				}
			}
		}
		goto L_Exit;
	}

	hr = OMF_E_TYPE_SURPRISE;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Reads a Position32Array into a caller-supplied buffer. The data type must be omfi:Position32Array.
//	This method has optional call-twice semantics. Make the first call with pBuffer set to NULL to get the required
//	buffer size (measured in 32-bit elements), and then call again with an appropriately sized buffer to retrieve
//	the full array. See also CoreReadPosition64Array() and CoreAllocPositionArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadPosition32Array(BENTO_BLOP& rBlop, DWORD dwProperty,
													ULONG nMaxElements, PUINT32 pBuffer, PULONG pnActualElements)
{
	HRESULT	hr					= S_OK;
	UINT64	cbPayload			= 0;
	DWORD	dwStoredType		= 0;
	UINT32	cbBuffer				= 0;
	UINT32	cbArrayBody			= 0;
	ULONG	nElementsCalculated	= 0;
	ULONG	nElementsReported	= 0;

	// The pnActualElements pointer is`required.
	if (IsBadWritePointer(pnActualElements, sizeof(ULONG)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe this. Right here. Right now.
	*pnActualElements = 0;

	// Make sure caller's nMaxElements argument is not insanely huge.
	// If it's greater than 65536 then tweak it!
	if (nMaxElements > 0x00010000)
	{
		nMaxElements = 0x00010000;
	}

	// Calculate the byte size of caller's buffer.
	cbBuffer = nMaxElements * sizeof(UINT32);

	// The pBuffer argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pBuffer)
	{
		if (IsBadWritePointer(pBuffer, cbBuffer))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe it. Right here, right now.
		ZeroMemory(pBuffer, cbBuffer);
	}

	// Get the payload data type and size. On exit dwStoredType will always be non-zero.
	if (FAILED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, &cbPayload)))
	{
		goto L_Exit;
	} 

	// Make sure the data type is omfi:Position32Array.
	if (dwStoredType == m_dwTypePos32Array)
	{
		// Create a local copy of the array, assuming its maximum length.
		// Populate this first, then copy it to caller's buffer.
		OMF_POSITION32_ARRAY aLocalPos32Array = {0};

		// Make sure the payload is large enough to hold the 16-bit element count.
		if (cbPayload < sizeof(WORD))
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Make sure the complete payload size is less than or equal to a fully populated OMF_POSITION32_ARRAY structure.
		if (cbPayload > sizeof(OMF_POSITION32_ARRAY))
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Calculate the size of the 32-bit elements.
		cbArrayBody = ULONG(cbPayload) - sizeof(WORD);

		// cbArrayBody must be evenly divisible by 4 because that is sizeof(UINT32).
		if (cbArrayBody & 0x00000003)
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Calculate the number of elements based on the size of the array body.
		nElementsCalculated = cbArrayBody/sizeof(UINT32);

		// Read the entire payload into our temporary array.
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), ULONG(cbPayload), &aLocalPos32Array)))
		{
			goto L_Exit;
		}

		// Fetch the element count as it is reported in the file.
		if (m_fOmfBigEndian)
		{
			// Byteswap and promote to 32 bits.
			nElementsReported = Endian16(aLocalPos32Array.wElements);
		}
		else
		{
			// Promote to 32-bits.
			nElementsReported = aLocalPos32Array.wElements;
		}

		// Compare
		if (nElementsReported != nElementsCalculated)
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Give element count back to caller.
		*pnActualElements = nElementsReported;

		// Is caller's buffer large enough to hold all the elements?
		if (nElementsReported > nMaxElements)
		{
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Did our caller provide a destination??
		if (NULL == pBuffer)
		{
			// Not really an error code.
			// Under these circumstances this is not an error because the destination buffer is optional.
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Copy each element from our temporary array to the caller's buffer.
		if (m_fOmfBigEndian)
		{
			for (UINT i = 0; i < nElementsReported; i++)
			{
				pBuffer[i] = Endian32(aLocalPos32Array.a[i]);
			}
		}
		else
		{
			for (UINT i = 0; i < nElementsReported; i++)
			{
				pBuffer[i] = aLocalPos32Array.a[i];
			}
		}

		hr = S_OK;
		goto L_Exit;
	}

	// Data type is not omfi:Position32Array
	hr = OMF_E_TYPE_SURPRISE;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Reads a Position32Array or a Position64Array into a caller-supplied buffer.
//	The data type can be omfi:Position32Array or omfi:Position64Array.
//	If the data type is omfi:Position32Array then the 32-bit 'positions' are promoted to 64 bits.
//	This method has optional call-twice semantics. Make the first call with pBuffer set to NULL to get the required
//	buffer size (measured in 64-bit elements), and then call again with an appropriately sized buffer to retrieve
//	the full array. See also CoreAllocPositionArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadPosition64Array(BENTO_BLOP& rBlop, DWORD dwProperty,
													ULONG nMaxElements, PUINT64 pBuffer, PULONG pnActualElements)
{
	HRESULT	hr					= S_OK;
	UINT64	cbPayload			= 0;
	DWORD	dwStoredType		= 0;
	UINT32	cbBuffer				= 0;
	UINT32	cbArrayBody			= 0;
	ULONG	nElementsCalculated	= 0;
	ULONG	nElementsReported	= 0;

	// The pnActualElements pointer is`required.
	if (IsBadWritePointer(pnActualElements, sizeof(ULONG)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe this. Right here. Right now.
	*pnActualElements = 0;

	// Make sure caller's nMaxElements argument is not insanely huge.
	// If it's greater than 65536 then tweak it!
	if (nMaxElements > 0x00010000)
	{
		nMaxElements = 0x00010000;
	}

	// Calculate the byte size of caller's buffer.
	cbBuffer = nMaxElements * sizeof(UINT64);

	// The pBuffer argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pBuffer)
	{
		if (IsBadWritePointer(pBuffer, cbBuffer))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe it. Right here, right now.
		ZeroMemory(pBuffer, cbBuffer);
	}

	// Get the payload data type and size. On exit dwStoredType will always be non-zero.
	if (FAILED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, &cbPayload)))
	{
		goto L_Exit;
	} 

	// Is the data type is omfi:Position32Array?
	if (dwStoredType == m_dwTypePos32Array)
	{
		// Create a local copy of the array, assuming its maximum length.
		// Populate this first, then copy it to caller's buffer.
		OMF_POSITION32_ARRAY aLocalPos32Array = {0};

		// Make sure the payload is large enough to hold the 16-bit element count.
		if (cbPayload < sizeof(WORD))
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Make sure the complete payload size is less than or equal to a fully populated OMF_POSITION32_ARRAY structure.
		if (cbPayload > sizeof(OMF_POSITION32_ARRAY))
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Calculate the size of the 32-bit elements.
		cbArrayBody = ULONG(cbPayload) - sizeof(WORD);

		// cbArrayBody must be evenly divisible by sizeof(UINT32).
		if (cbArrayBody & 0x00000003)
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Calculate the number of elements based on the size of the array body.
		nElementsCalculated = cbArrayBody/sizeof(UINT32);

		// Read the entire payload into our temporary array.
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), ULONG(cbPayload), &aLocalPos32Array)))
		{
			goto L_Exit;
		}

		// Fetch the element count as it is reported in the file.
		if (m_fOmfBigEndian)
		{
			// Byteswap and promote to 32 bits.
			nElementsReported = Endian16(aLocalPos32Array.wElements);
		}
		else
		{
			// Promote to 32-bits.
			nElementsReported = aLocalPos32Array.wElements;
		}

		// Compare
		if (nElementsReported != nElementsCalculated)
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Give element count back to caller.
		*pnActualElements = nElementsReported;

		// Is caller's buffer large enough to hold all the elements?
		if (nElementsReported > nMaxElements)
		{
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Did our caller provide a destination?
		if (NULL == pBuffer)
		{
			// Not really an error code.
			// Under these circumstances this is not an error because the destination buffer is optional.
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Copy each element from our temporary array to the caller's buffer.
		if (m_fOmfBigEndian)
		{
			for (UINT i = 0; i < nElementsReported; i++)
			{
				pBuffer[i] = Endian32(aLocalPos32Array.a[i]);
			}
		}
		else
		{
			for (UINT i = 0; i < nElementsReported; i++)
			{
				pBuffer[i] = aLocalPos32Array.a[i];
			}
		}

		hr = S_OK;
		goto L_Exit;
	}

	// Is the data type is omfi:Position64Array?
	if (dwStoredType == m_dwTypePos64Array)
	{
		// Create a local copy of the array, assuming its maximum length.
		// Populate this first, then copy it to caller's buffer.
		OMF_POSITION64_ARRAY aLocalPos64Array = {0};

		// Make sure the payload is large enough to hold the 16-bit element count.
		if (cbPayload < sizeof(WORD))
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Make sure the data size is less than or equal to a fully populated OMF_POSITION64_ARRAY structure.
		if (cbPayload > sizeof(OMF_POSITION64_ARRAY))
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Calculate the size of the 64-bit elements.
		cbArrayBody = ULONG(cbPayload) - sizeof(WORD);

		// cbArrayBody must be evenly divisible by sizeof(UINT64).
		if (cbArrayBody & 0x00000007)
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Calculate the number of elements based on the size of the array body.
		nElementsCalculated = cbArrayBody/sizeof(UINT64);

		// Read the entire payload into our temporary array.
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), ULONG(cbPayload), &aLocalPos64Array)))
		{
			goto L_Exit;
		}

		// Fetch the element count as it is reported in the file.
		if (m_fOmfBigEndian)
		{
			// Byteswap and promote to 32 bits.
			nElementsReported = Endian16(aLocalPos64Array.wElements);
		}
		else
		{
			// Promote to 32-bits.
			nElementsReported = aLocalPos64Array.wElements;
		}

		// Compare
		if (nElementsReported != nElementsCalculated)
		{
			hr = OMF_E_BAD_ARRAY;
			goto L_Exit;
		}

		// Give element count back to caller.
		*pnActualElements = nElementsReported;

		// Is caller's buffer large enough to hold all the elements?
		if (nElementsReported > nMaxElements)
		{
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Did our caller provide a destination??
		if (NULL == pBuffer)
		{
			// Not really an error code.
			// Under these circumstances this is not an error because the destination buffer is optional.
			hr = OMF_E_INSUFFICIENT_BUFFER;
			goto L_Exit;
		}

		// Copy each element from our temporary array to the caller's buffer. 
		if (m_fOmfBigEndian)
		{
			for (UINT i = 0; i < nElementsReported; i++)
			{
				pBuffer[i] = Endian64(aLocalPos64Array.a[i]);
			}
		}
		else
		{
			for (UINT i = 0; i < nElementsReported; i++)
			{
				pBuffer[i] = aLocalPos64Array.a[i];
			}
		}

		hr = S_OK;
		goto L_Exit;
	}

	// Data type is not omfi:Position32Array or omfi:Position64Array
	hr = OMF_E_TYPE_SURPRISE;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue (OMF2) or omfi:VarLenBytes (OMF1).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreQueryFirstDataValue(BENTO_BLOP& rBlop, BOOL fImmediate, POMF_DATA_VALUE pDataValue)
{
	HRESULT hr = OMF_E_PROP_NOT_FOUND;

	// Validate caller's pointer.
	if (IsBadWritePointer(pDataValue, sizeof(OMF_DATA_VALUE)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// Wipe caller's buffer.
	pDataValue->cbOffset	= 0;
	pDataValue->cbLength	= 0;

	// Make sure caller's BENTO_BLOP is not the empty blop.
	if (0 == rBlop.dwObject)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Anticipate failure.
	hr = OMF_E_TYPE_NOT_FOUND;

	// Loop through each TOCX_ITEM structure belonging to the object until we find one whose pItem->dwDataType
	// is "omfi:UID" or until there are no items left
	PTOCX_ITEM	pCurItem	= &m_aToc[rBlop.iFirstItem];
	PTOCX_ITEM	pEndItem	= &pCurItem[rBlop.wTotalItems];

	do
	{
		// We can be sure that m_dwTypeDataValue and m_dwTypeVarLenBytes are always non-zero.
		if ((pCurItem->dwDataType == m_dwTypeDataValue)||
			(pCurItem->dwDataType == m_dwTypeVarLenBytes))
		{
			if ((pCurItem->bStorageMode == SM_IMMEDIATE) && fImmediate)
			{
				if (pCurItem->cbLength64 > 4)
				{
					hr = OMF_E_STORAGE_SURPRISE;
					goto L_Exit;
				}

				pDataValue->cbLength	= pCurItem->cbLength64;
				pDataValue->cbOffset	= -1;
				CopyMemory(&pDataValue->cbOffset, pCurItem->aImmediateBytes, pCurItem->cbLengthLo);
				hr = S_OK;
				goto L_Exit;
			}

			if (pCurItem->bStorageMode != SM_OFFSET)
			{
				hr = OMF_E_STORAGE_SURPRISE;
				goto L_Exit;
			}

			// Values are already byte-swapped and promoted to 64 bits.
			pDataValue->cbOffset = pCurItem->cbOffset64;
			pDataValue->cbLength = pCurItem->cbLength64;
			hr = S_OK;
			goto L_Exit;
		}
		// Try again.
		// Examine the next TOCX_ITEM.
	} while (++pCurItem < pEndItem);

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue (OMF2) or omfi:VarLenBytes (OMF1).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreQueryDataValue(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_DATA_VALUE pDataValue)
{
	HRESULT		hr			= E_UNEXPECTED;
	PTOCX_ITEM	pTocItem	= NULL;
	if (IsBadWritePointer(pDataValue, sizeof(OMF_DATA_VALUE)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}
	else
	{
		pDataValue->cbOffset	= 0;
		pDataValue->cbLength	= 0;
	}

	if (FAILED(hr = FindTocItemForProperty(rBlop, dwProperty, pTocItem)))
	{
		goto L_Exit;
	}

	if ((pTocItem->dwDataType != m_dwTypeDataValue) &&
		(pTocItem->dwDataType != m_dwTypeVarLenBytes))
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_Exit;
	}

	if (pTocItem->bStorageMode == SM_IMMEDIATE)
	{
		if (pTocItem->cbLength64 > 4)
		{
			hr = OMF_E_STORAGE_SURPRISE;
			goto L_Exit;
		}

		pDataValue->cbLength	= pTocItem->cbLength64;
		pDataValue->cbOffset	= -1;
		CopyMemory(&pDataValue->cbOffset, pTocItem->aImmediateBytes, pTocItem->cbLengthLo);
		hr = S_OK;
		goto L_Exit;
	}

	if (pTocItem->bStorageMode != SM_OFFSET)
	{
		hr = OMF_E_STORAGE_SURPRISE;
		goto L_Exit;
	}

	// Values are already byte-swapped and promoted to 64 bits.
	pDataValue->cbOffset = pTocItem->cbOffset64;
	pDataValue->cbLength = pTocItem->cbLength64;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue (OMF2) or omfi:VarLenBytes (OMF1).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadDataValueAsRawBytes(BENTO_BLOP& rBlop,
															DWORD dwProperty,
																ULONG cbBuffer,
																	PVOID pBuffer,
																		PULONG pcbRequired)
{
	OMF_DATA_VALUE	sDataValue	= {0};
	HRESULT			hr			= E_POINTER;
	ULONG			cbPayload	= 0;

	// The pcbRequired argument is required.
	if (IsBadWritePointer(pcbRequired, sizeof(ULONG)))
	{
		hr = E_POINTER;
		goto L_Exit;
	}
	else
	{
		// Wipe it.
		*pcbRequired = 0;
	}

	// The pBuffer argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pBuffer)
	{
		if (IsBadWritePointer(pBuffer, cbBuffer))
		{
			hr = E_POINTER;
			goto L_Exit;
		}

		// Wipe caller's destination buffer. Right here, right now.
		ZeroMemory(pBuffer, cbBuffer);
	}

	if (FAILED(hr = CoreQueryDataValue(rBlop, dwProperty, &sDataValue)))
	{
		goto L_Exit;
	}

	// Restrict size to 1 Mebibyte, 20 bits, 1048576 bytes.
	// If caller needs more than that they should use CoreCreateStreamOnDataValue() instead.
	if (sDataValue.cbLength > 0x0000000000100000)
	{
		hr = OMF_E_CANT_COMPLETE;
		goto L_Exit;
	}

	// Give the payload size to the caller.
	cbPayload = ULONG(sDataValue.cbLength);

	*pcbRequired = cbPayload;

	// Is the payload too big for caller's buffer?
	if (cbPayload > cbBuffer)
	{
		hr = OMF_E_INSUFFICIENT_BUFFER;
		goto L_Exit;
	}

	// If cbOffset is negative then the data is stored immediately in its lower four bytes.
	if (sDataValue.cbOffset < 0)
	{
		if (cbPayload <= 4)
		{
			CopyMemory(pBuffer, PBYTE(&sDataValue.cbOffset), cbPayload);
			hr = S_OK;
		}
		else
		{
			// The OMF_DATA_VALUE structure purports to store its data immediately in
			// its cbOffset member, but its cbLength member is greater than four.
			BREAK_IF_DEBUG
			hr = OMFOO_E_ASSERTION_FAILURE;
		}
	}
	else
	{
		hr = SeekRead(sDataValue.cbOffset, pBuffer, cbPayload);
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadGuid(BENTO_BLOP& rBlop, DWORD dwProperty, LPGUID pGuid)
{
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(GUID), pGuid);

	//// ISSUE: Do we need to byte-swap the result?
	//if (SUCCEEDED(hr) && m_fOmfBigEndian)
	//{
	//	pGuid->Data1	= Endian32(pGuid->Data1);
	//	pGuid->Data2	= Endian16(pGuid->Data2);
	//	pGuid->Data3	= Endian16(pGuid->Data3);
	//}

	return hr;
}

//*********************************************************************************************************************
//	Retrieves the very first property whose data type is 'omfi:UID' regardless of the class or property ID.
//	This is used when reading the mobIDs of OMF1 Media Data Class (MDAT) objects.
//
//		OMFI:AIFC:MobID
//		OMFI:IDAT:MobID
//		OMFI:JPEG:MobID
//		OMFI:MDAT:MobID
//		OMFI:MFML:MobID
//		OMFI:MOBJ:MobID
//		OMFI:MOBR:MobID
//		OMFI:MSML:MobID
//		OMFI:SD2M:MobID
//		OMFI:TIFF:MobID
//		OMFI:WAVE:MobID
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadFirstMobID(BENTO_BLOP& rBlop, POMF_MOB_ID pMobID)
{
	HRESULT		hr		= OMF_E_PROP_NOT_FOUND;

	// Validate caller's pointer.
	if (IsBadWritePointer(pMobID, sizeof(OMF_MOB_ID)))
	{
		return E_POINTER;
	}

	// Wipe caller's buffer.
	pMobID->dwMajor = 0;
	pMobID->dwMinor = 0;
	pMobID->dwPrefix = 0;

	// Make sure caller's BENTO_BLOP is not the empty blop.
	if (0 == rBlop.dwObject)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Anticipate failure.
	hr = OMF_E_TYPE_NOT_FOUND;

	// Loop through each TOCX_ITEM structure belonging to the object until we find one whose pItem->dwDataType
	// is "omfi:UID" or until there are no items left
	PTOCX_ITEM	pCurItem	= &m_aToc[rBlop.iFirstItem];
	PTOCX_ITEM	pEndItem	= &pCurItem[rBlop.wTotalItems];
	do
	{
		if (pCurItem->dwDataType == m_dwTypeUID)
		{
			hr = CoreReadMobID(rBlop, pCurItem->dwProperty, pMobID);

			// Done!
			break;
		}
		// Try again.
		// Examine the next TOCX_ITEM.
	} while (++pCurItem < pEndItem);

	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:UID.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadMobID(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_MOB_ID pMobID)
{
	HRESULT		hr			= E_POINTER;
	PTOCX_ITEM	pTocItem	= NULL;
	if (IsBadWritePointer(pMobID, sizeof(OMF_MOB_ID)))
	{
		goto L_Exit;
	}
	else
	{
		pMobID->dwMajor = 0;
		pMobID->dwMinor = 0;
		pMobID->dwPrefix = 0;
	}

	if (FAILED(hr = FindTocItemForPropertyAndType(rBlop, dwProperty, m_dwTypeUID, pTocItem)))
	{
		goto L_Exit;
	}

	if (pTocItem->bStorageMode == SM_CACHED)
	{
		pMobID->dwPrefix = pTocItem->aCachedDwords[0];	// dwPrefix
		pMobID->dwMajor = pTocItem->aCachedDwords[1];	// dwMajor
		pMobID->dwMinor	= pTocItem->aCachedDwords[2];	// dwMinor
		hr = S_OK;
		goto L_Exit;
	}

	BREAK_IF_DEBUG	// All MobIDs should have been cached in CContainerLayer00::IngestMobIDs().

	if (pTocItem->bStorageMode != SM_OFFSET)
	{
		hr = OMF_E_STORAGE_SURPRISE;
		goto L_Exit;
	}

	if (SUCCEEDED(hr = CoreReadStrict(rBlop, dwProperty, m_dwTypeUID, sizeof(OMF_MOB_ID), pMobID)))
	{
		if (m_fOmfBigEndian)
		{
			pMobID->dwPrefix = Endian32(pMobID->dwPrefix);
			pMobID->dwMajor = Endian32(pMobID->dwMajor);
			pMobID->dwMinor = Endian32(pMobID->dwMinor);
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadObjRef(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwRefList, PDWORD pdwObjRef)
{
	PTOCX_ITEM	pItem		= NULL;	// dynamic pointer to the current TOCX_ITEM under scrutiny
	ULONG		nItems		= 0;	// number of contiguous TOC items (aka 'fragments') associated with this property
	DWORD	dwStoredType	= 0;	// the data type as it is stored in the first TOCX_ITEM

	HRESULT	hr = (-1);

	// The pdwRefList argument is optional. But if our caller supplied one then verify it.
	if (pdwRefList)
	{
		if (IsBadWritePointer(pdwRefList, sizeof(DWORD)))
		{
			return E_POINTER;
		}
		else
		{
			*pdwRefList	= 0;
		}
	}

	if (IsBadWritePointer(pdwObjRef, sizeof(DWORD)))
	{
		return E_POINTER;
	}

	// Wipe caller's return value.
	*pdwObjRef	= 0;

	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Validate caller's dwProperty argument.
	if (0 == dwProperty)
	{
		return OMF_E_PROP_NOT_DEFINED;
	}

	// Get a pointer to the first TOCX_ITEM structure for this blop.
	pItem	= &m_aToc[rBlop.iFirstItem];

	// Get the total number of items for this blop.
	nItems	= rBlop.wTotalItems;

	// Loop through each TOCX_ITEM structure belonging to the object until we find one whose pItem->dwProperty
	// matches caller's dwProperty argument, or until there are no items left.
	while (nItems)
	{
		if ((pItem->dwProperty == dwProperty) && (pItem->bStorageMode != SM_REFLISTID))
		{
			// Found it!
			// Jump out of this loop and go do more stuff.
			goto L_PropExists;
		}

		// The current item is not the requested property.
		// Advance pointer to the next item and decrement the loop count.
		++pItem;
		--nItems;
	}

	// The specified object does not have the specified property.
	return OMF_E_PROP_NOT_FOUND;

L_PropExists:
	// We found the property!
	// Now get its data type.
	dwStoredType = pItem->dwDataType;

	// Make sure it's non-zero.
	// This should NEVER be zero, but if it is then something is SERIOUSLY messed up, and we need to stop NOW!
	if (dwStoredType == 0)
	{
		return OMF_E_TYPE_SURPRISE;
	}

	// Make sure this property is the right type.
	if (dwStoredType != m_dwTypeObjRef)
	{
		return OMF_E_TYPE_SURPRISE;
	}

	if (m_fOmfVer1)
	{
		OMF1_OBJREF_ENTRY	v1	= {0};
		if (pItem->bStorageMode == SM_OFFSET)
		{
			if (pItem->cbLength64 != sizeof(OMF1_OBJREF_ENTRY))
			{
				hr = OMF_E_SIZE_SURPRISE;
			}
			else
			{
				hr = SeekRead(pItem->cbOffset64, &v1, sizeof(OMF1_OBJREF_ENTRY));
				if (SUCCEEDED(hr))
				{
					if (m_fBentoBigEndian)
					{
						*pdwObjRef	= Endian32(v1.dwObject);
					}
					else
					{
						*pdwObjRef	= v1.dwObject;
					}
				}
			}
		}
		else
		{
			if (pItem->bStorageMode == SM_IMMEDIATE)
			{
				// ISSUE: Is the data ever immediate for rev 1?
				BREAK_IF_DEBUG	// Has this ever happened?
				hr = OMF_E_FAIL;
			}
			else
			{
				// We can't deal with the data because we don't know how it is stored ...
				BREAK_IF_DEBUG	// Has this ever happened?
				hr = OMF_E_STORAGE_SURPRISE;
			}
		}
	}
	else
	{
		DWORD	dwRefList	= 0;
		DWORD	dwObjRef	= 0;

		// Test the very first item to see if it is a ReferenceListID.
		// Page 66 of the Bento Specification Rev 1.0d5 says this is optional, but if it exists it must occur first.
		if (pItem->bStorageMode == SM_REFLISTID)
		{
			// Save the ID. (Already byteswapped)
			dwRefList = pItem->dwRefList;

			// Advance pointer to the next item ...
			++pItem;

			// Decrement item count ...
			--nItems;

			if ((0 == nItems) || (pItem->dwProperty != dwProperty) || (pItem->dwDataType != dwStoredType))
			{
				BREAK_IF_DEBUG	// Has this ever happened?
				hr = OMF_E_SIZE_SURPRISE;
				goto L_Exit;
			}
		}

		// 
		if (pItem->cbLength64 != sizeof(DWORD))
		{
			BREAK_IF_DEBUG	// Has this ever happened?
			hr = OMF_E_SIZE_SURPRISE;
			goto L_Exit;
		}

		if (pItem->bStorageMode == SM_IMMEDIATE)
		{
			// When the data is stored immediately in the TOC ...
			dwObjRef = pItem->dwImmediateDword;
		}
		else if (pItem->bStorageMode == SM_OFFSET)
		{
			// When the data is stored somewhere in the file outside of the TOC ...
			if (FAILED(hr = SeekRead(pItem->cbOffset64, &dwObjRef, sizeof(DWORD))))
			{
				BREAK_IF_DEBUG	// Has this ever happened?
				goto L_Exit;
			}
		}
		else
		{
			// When we can't deal with the data because we don't know how it is stored ...
			BREAK_IF_DEBUG	// Has this ever happened?
			hr = OMF_E_STORAGE_SURPRISE;
			goto L_Exit;
		}

		// Save dwRefList for caller. (Already byteswapped.)
		if (pdwRefList)
		{
			*pdwRefList	= dwRefList;
		}

		// WARNING! NOTE!
		// Believe it or not there was at least one legacy Macintosh program ("Media Magic" from Dark Matter Digital)
		// that used a big-endian byte order for the Bento layer, and a little-endian byte order for the OMF layer!
		// So here we use m_fBentoBigEndian (not m_fOmfBigEndian)!
		if (m_fBentoBigEndian)
		{
			*pdwObjRef = Endian32(dwObjRef);
		}
		else
		{
			*pdwObjRef = dwObjRef;
		}

		// Success!
		hr = S_OK;
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	omfi:data:Point
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadPoint(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_POINT pPoint)
{
	HRESULT	hr = CoreReadStrict(rBlop, dwProperty, (-1), sizeof(OMF_POINT), pPoint);

	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		pPoint->x.nNumerator	= Endian32(pPoint->x.nNumerator);
		pPoint->x.nDenominator	= Endian32(pPoint->x.nDenominator);
		pPoint->y.nNumerator	= Endian32(pPoint->y.nNumerator);
		pPoint->y.nDenominator	= Endian32(pPoint->y.nDenominator);
	}

	// Some implementations write zero to the denominator! 
	if (pPoint->x.nDenominator == 0)
		pPoint->x.nDenominator = 1;		// fixed!

	if (pPoint->y.nDenominator == 0)
		pPoint->y.nDenominator = 1;		// fixed!

	//	omfi:data:Point
	//	omfi:DataValue
	return hr;
}

//*********************************************************************************************************************
//	omfi:ProductVersion
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadProductVersion(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_PRODUCT_VERSION pVersion)
{
	HRESULT	hr = CoreReadStrict(rBlop, dwProperty, OrdinalToDataTypeID(eTypeProductVersion),
																			sizeof(OMF_PRODUCT_VERSION), pVersion);
	if (SUCCEEDED(hr) && m_fOmfBigEndian)
	{
		pVersion->wMajor	= Endian16(pVersion->wMajor);
		pVersion->wMinor	= Endian16(pVersion->wMinor);
		pVersion->wTertiary	= Endian16(pVersion->wTertiary);
		pVersion->wPatch	= Endian16(pVersion->wPatch);
		pVersion->wDetail	= Endian16(pVersion->wDetail);
	}

	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:Rational or omfi:ExactEditRate.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadRational(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_RATIONAL pRational)
{
	// Try omfi:Rational first because it exists in OMF1 and OMF2.
	DWORD dwDataType = OrdinalToDataTypeID(eTypeRational);
	HRESULT hr = CoreReadStrict(rBlop, dwProperty, dwDataType, sizeof(OMF_RATIONAL), pRational);
	if (FAILED(hr) && m_fOmfVer1)
	{
		// omfi:ExactEditRate is OMF1 only.
		dwDataType = OrdinalToDataTypeID(eTypeExactEditRate);
		hr = CoreReadStrict(rBlop, dwProperty, dwDataType, sizeof(OMF_RATIONAL), pRational);
	}

	if (SUCCEEDED(hr))
	{
		if (m_fOmfBigEndian)
		{
			pRational->nNumerator	= Endian32(pRational->nNumerator);
			pRational->nDenominator	= Endian32(pRational->nDenominator);
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:ExactEditRate or omfi:Rational.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadRationalAsFloat(BENTO_BLOP& rBlop, DWORD dwProperty, PFLOAT pFloat)
{
	// Validate caller's destination pointer.
	if (IsBadWritePointer(pFloat, sizeof(FLOAT)))
	{
		return E_POINTER;
	}
	else
	{
		*pFloat = 0;
	}

	DOUBLE	fDouble	= 0.0;
	HRESULT hr = CoreReadRationalAsDouble(rBlop, dwProperty, &fDouble);
	if (SUCCEEDED(hr))
	{
		*pFloat = FLOAT(fDouble);
	}
	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:ExactEditRate or omfi:Rational.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadRationalAsDouble(BENTO_BLOP& rBlop, DWORD dwProperty, DOUBLE* pDouble)
{
	// Validate caller's destination pointer.
	if (IsBadWritePointer(pDouble, sizeof(DOUBLE)))
	{
		return E_POINTER;
	}
	else
	{
		*pDouble = 0;
	}

	OMF_RATIONAL rat = {0};
	HRESULT hr = CoreReadRational(rBlop, dwProperty, &rat);
	if (SUCCEEDED(hr))
	{
		if (rat.nDenominator)
		{
			DOUBLE fQuotient = rat.nNumerator;
			fQuotient /= rat.nDenominator;
			*pDouble = fQuotient;
		}
		else
		{
			hr = OMF_E_DIVIDE_BY_ZERO;
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue (OMF2) or omfi:VarLenBytes (OMF1).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadExtended80AsDouble(BENTO_BLOP& rBlop, DWORD dwProperty, DOUBLE* pDouble)
{
	OMF_DATA_VALUE	sDataValue	= {0};
	HRESULT hr = CoreQueryDataValue(rBlop, dwProperty, &sDataValue);
	if (SUCCEEDED(hr))
	{
		hr = OMF_E_SIZE_SURPRISE;
		if (sDataValue.cbLength == 10)
		{
			BYTE aExtended80[10] = {0};
			if (SUCCEEDED(hr = CoreReadStrict(rBlop, dwProperty, (-1), ULONG(10), &aExtended80)))
			{
				hr = Extended80ToDouble(aExtended80, pDouble);
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Read a property with an omfi:TimeStamp data type, and place the result in the caller's OMF_TIMESTAMP structure.
//	Note that this data type is supposed to be five bytes long - but there are several variations.
//	Some are four, five, six, or even eight bytes long.
//	The eight-byte variety is found in v1.0 files, but only in the HEAD's OMFI:LastModified property.
//	Note that this routine will also read a v1.0 OMFI:MOBJ:_CreationTime property with a omfi:Long data type.

//	The longer lengths were probably a kludge to force the Bento storage mode to "32-bit offset, 32-bit length".
//
//	Also note that there was a bug in some builds of OmfToolkit.dll that cleared the upper three nybbles
//	(the upper 12 bits) of the 32-bit timestamp with zeros.
//	The bug existed in OMF1 and OMF2, and afaik it only existed in Windows.
//	The bug was probably created in a routine called omfsGetDateTime().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadTimeStamp(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_TIMESTAMP pTimeStamp)
{
	// Temporary union for reading an omfi:TimeStamp payload. (Because they come in four sizes!)
	#pragma pack(push, 1)	// align members to byte boundaries
	union {
		DWORD			dw;		// a single 32-bit value holding the time in seconds since January 1, 1970.
		OMF_TIMESTAMP	ts;		// a five-byte OMF timestamp structure.
		QWORD			qw;		// something to look at just for debugging - so we can see the bytes beyond.
	} u = {0};
	#pragma pack(pop)	// restore compiler's previous settings

	HRESULT		hr	= OMF_E_BAD_TIMESTAMP;
	UINT64		cbPayload		= 0;	// sizeof(DWORD), sizeof(OMF_TIMESTAMP), or sizeof(QWORD).
	DWORD		dwStoredType	= 0;	// omfi:TimeStamp, or omfi:Long.
	DWORD		dwTimeValue		= 0;	// the time part expressed in seconds since January 1, 1970.

	// Validate caller's destination pointer.
	if (IsBadWritePointer(pTimeStamp, sizeof(OMF_TIMESTAMP)))
	{
		return E_POINTER;
	}

	// Wipe caller's destination buffer.
	pTimeStamp->dwTimeValue	= 0;
	pTimeStamp->fIsGMT		= FALSE;

	// Get the payload data type and size.
	// On exit dwStoredType will always be non-zero.
	if (FAILED(hr = CoreGetPropInfo(rBlop, dwProperty, &dwStoredType, NULL, NULL, &cbPayload)))
	{
		// If we can't get the data size, then it's because the property doesn't exist in this object.
		goto L_Exit;
	}

	// The data type must be omfi:TimeStamp or omfi:Long.
	if ((dwStoredType != m_dwTypeTimeStamp) &&
		(dwStoredType != m_dwTypeLong))
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_Exit;
	}

	// Can be 4, 5, 6, or 8 bytes long.
	if ((cbPayload != 4) && 
		(cbPayload != 5) &&
		(cbPayload != 6) &&
		(cbPayload != 8))
	{
		hr = OMF_E_SIZE_SURPRISE;
		goto L_Exit;
	}

	// Read the raw bytes.
	if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), ULONG(cbPayload), &u)))
	{
		goto L_Exit;
	}

	// Now that we've read the whole payload we try to make sense of it. 
	// We begin by assuming (without deciding) that the payload begins with a DWORD
	// representing the elapsed time in seconds since 1/1/1970.
	if (m_fOmfBigEndian)
	{
		// Isolate and byteswap the first four bytes.
		dwTimeValue = Endian32(u.dw);
	}
	else
	{
		// Just isolate the first four bytes.
		dwTimeValue = u.dw;
	}

	// Test to see if the first four bytes are a valid 32-bit time value.
	if (!IsValidTimeValue(dwTimeValue))
	{
		hr = OMF_E_BAD_TIMESTAMP;
		goto L_Exit;
	}

	// If so then save it in caller's structure.
	pTimeStamp->dwTimeValue = dwTimeValue;

	// Now we're sure we have a valid 32-bit time value, but we're still not sure about the GMT flag.
//	hr = S_FALSE;

	// If it's a valid 32-bit time value then test the fifth byte to see if it's a valid BOOLEAN.
	// Only do this test if the payload is exactly five bytes long.
	if (cbPayload == sizeof(OMF_TIMESTAMP))
	{
		// Test for 0x01 (TRUE) or 0x00 (FALSE).
		// Anything else is invalid!
		if ((u.ts.fIsGMT == TRUE) || (u.ts.fIsGMT == FALSE))
		{
			// Yes! The first four bytes are a valid time stamp and the fifth byte is a valid BOOLEAN.
			// So copy that BOOLEAN into our caller's structure.
			pTimeStamp->fIsGMT = u.ts.fIsGMT;

			// Complete success!
			hr = S_OK;
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadTimeStampAsFileTime(BENTO_BLOP& rBlop, DWORD dwProperty, LPFILETIME pFileTime)
{
	OMF_TIMESTAMP	omfTimeStamp	= {0,0};

	// Validate caller's destination pointer.
	if (IsBadWritePointer(pFileTime, sizeof(FILETIME)))
	{
		return E_POINTER;
	}

	HRESULT hr = CoreReadTimeStamp(rBlop, dwProperty, &omfTimeStamp);
	if (SUCCEEDED(hr))
	{
		QWORD qwFileTime = CvtSecondsSince1970ToFileTime(omfTimeStamp.dwTimeValue);
		if (omfTimeStamp.fIsGMT)
		{
			FileTimeToLocalFileTime(LPFILETIME(&qwFileTime), pFileTime);
		}
		else
		{
			*PQWORD(pFileTime) = qwFileTime;
		}
	}
	else
	{
		pFileTime->dwLowDateTime	= 0;
		pFileTime->dwHighDateTime	= 0;
	}

	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:VersionType
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadVersionType(BENTO_BLOP& rBlop, DWORD dwProperty, POMF_VERSIONTYPE pVersionType)
{
	return CoreReadStrict(rBlop,dwProperty,OrdinalToDataTypeID(eTypeVersionType),sizeof(OMF_VERSIONTYPE),pVersionType);
}

//*********************************************************************************************************************
//	If rBlop is a Control Point Class object (CTLP) then dwProperty must be OMFI:CTLP:DataKind.
//	If rBlop is a Component Class object (CPNT) then dwProperty must be OMFI:CPNT:DataKind.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreGetDataKindAsWord(BENTO_BLOP& rBlop, DWORD dwProperty, PWORD pwDataKind)
{
	CHAR	chBuffer[OMF_STRMAX_DATAKIND_NAME]	= {0};

	// Validate caller's pointer.
	if (IsBadWritePointer(pwDataKind, sizeof(WORD)))
		return E_POINTER;

	// Wipe it now.
	*pwDataKind = 0;

	HRESULT	hr	= CoreGetDataKindAsStringA(rBlop, dwProperty, sizeof(chBuffer), chBuffer, NULL);
	if (SUCCEEDED(hr))
	{
		hr = DataKindIDStringToWord(chBuffer, pwDataKind);
	}

	return hr;
}

//*********************************************************************************************************************
//	If rBlop is a Control Point Class object (CTLP) then dwProperty must be OMFI:CTLP:DataKind.
//	If rBlop is a Component Class object (CPNT) then dwProperty must be OMFI:CPNT:DataKind.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreGetDataKindAsStringA(BENTO_BLOP& rBlop, DWORD dwProperty,
													ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired)
{
	DWORD	dwRefList	= 0;
	DWORD	dwDDEF		= 0;

	// Get the Data Definition object (DDEF).
	// We don't care if CoreReadObjRef() fails because CoreReadStringA() will handle an invalid DDEF,
	// and it will always validate and wipe the caller's buffer!
	CoreReadObjRef(rBlop, dwProperty, &dwRefList, &dwDDEF);

	// OMFI:DDEF:DataKindID
	// omfi:UniqueName
	return CoreReadStringA(GetBlop(dwDDEF), OrdinalToPropertyID(ePropDdefDataKindID), cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Same as above but for UTF-16.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreGetDataKindAsStringW(BENTO_BLOP& rBlop, DWORD dwProperty,
													ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	DWORD	dwRefList	= 0;
	DWORD	dwDDEF		= 0;

	// Get the Data Definition object (DDEF).
	// We don't care if CoreReadObjRef() fails because CoreReadStringA() will handle an invalid DDEF,
	// and it will always validate and wipe the caller's buffer!
	CoreReadObjRef(rBlop, dwProperty, &dwRefList, &dwDDEF);

	// OMFI:DDEF:DataKindID
	// omfi:UniqueName
	return CoreReadStringW(GetBlop(dwDDEF), OrdinalToPropertyID(ePropDdefDataKindID), cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue or omfi:VarLenBytes.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreExtractDataToFile(BENTO_BLOP& rBlop, DWORD dwProperty, PCWSTR pwzFullPath, BOOL fOverwrite)
{
	//OMF_DATA_VALUE	sDataValue	= {0};
	//HRESULT hr = CoreQueryDataValue(rBlop, dwProperty, &sDataValue);

	//if (SUCCEEDED(hr))
	//{
	//}

	BREAK_IF_DEBUG		// This is, and always has been, an unimplemented stub.
						// Who called us? How did we get here?
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue or omfi:VarLenBytes.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreReadDataValueAsStringW(BENTO_BLOP& rBlop, DWORD dwProperty,
																ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	CHAR	aPayload[512]	= {0};
	WCHAR	aWideChar[512]	= {0};
	LPSTR	pMultiByte		= NULL;
	DWORD	dwTargetProp	= 0;
	DWORD	dwCodePage		= 0;
	ULONG	cbMultiByte		= 0;
	ULONG	cchWideChar		= 0;
	ULONG	cbPayload		= 0;
	ULONG	cbNonAscii		= 0;

	HRESULT hr = CoreReadRawBytes(rBlop, dwProperty, ELEMS(aPayload), aPayload, &cbPayload);
	if (FAILED(hr))
	{
		goto L_Default;
	}

	pMultiByte	= aPayload;
	cbMultiByte	= cbPayload;

	// Detect and skip over the standard UTF-8 BOM if one exists.
	if ((cbPayload > 3) && (aPayload[0] == 0xEF) && (aPayload[1] == 0xBB) && (aPayload[2] == 0xBF))
	{
		pMultiByte	+= 3;
		cbMultiByte	-= 3;
	}
	else
	{
		// Detect and skip over Avid's UTF-8 BOM if one exists.
		if ((cbPayload > 2) && (aPayload[0] == 0) && (aPayload[1] == 0))
		{
			pMultiByte	+= 2;
			cbMultiByte	-= 2;
		}
	}

	// If this succeeds we can be sure that the payload is null-terminated.
	if (IsStringHalfAscii(pMultiByte, cbMultiByte, PUINT32(&cbNonAscii)))
	{
		// If there are any bytes in the range 0x80~0xFF.
		if (cbNonAscii)
		{
			if (IsHalfUtf8(pMultiByte, cbMultiByte))
			{
				dwCodePage = CP_UTF8;
			}
			else
			{
				// If the payload isn't UTF-8 then we have no way of knowing for sure the actual code page.
				dwCodePage = m_dwWorkingCodePage;
			}
		}
		else
		{
			dwCodePage = CP_ASCII;
		}

		// "If this parameter is -1, the function processes the entire input string, including the null terminator.
		// Therefore, the resulting wide character string has a null terminator, and the length returned by the
		// function includes the terminating null character."
		if (0 == (cchWideChar = MultiByteToWideChar(dwCodePage, MB_ERR_INVALID_CHARS, pMultiByte,
																		(-1), aWideChar, ELEMS(aWideChar))))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
		else
		{
			hr = S_OK;
		}
	}
	else
	{
		hr = OMF_E_BAD_TEXT;
	}

L_Default:
	return CallTwiceStringHandlerW(hr, aWideChar, cchWideChar, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue or omfi:VarLenBytes.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreCreateStreamOnDataValue(BENTO_BLOP& rBlop, DWORD dwProperty, REFIID riid, PVOID *ppvOut)
{
	HRESULT	hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		OMF_DATA_VALUE	sDataValue	= {0};
		if (SUCCEEDED(hr = CoreQueryDataValue(rBlop, dwProperty, &sDataValue)))
		{
			hr = E_OUTOFMEMORY;
			LPUNKNOWN punkOwner	= LPUNKNOWN(static_cast<INonDelegatingUnknown*>(this));

			// If cbOffset is negative then the data is stored immediately in its lower four bytes.
			if (sDataValue.cbOffset < 0)
			{
				if (sDataValue.cbLength <= 4)
				{
					// Use CStreamOnRawBytes to implement the IStream.
					CStreamOnRawBytes* pStream = new CStreamOnRawBytes(punkOwner);
					if (pStream)
					{
						if (SUCCEEDED(hr = pStream->Initialize(PBYTE(&sDataValue.cbOffset), sDataValue.cbLength)))
						{
							//	pStream->SetStatStgNameW();
							hr = pStream->QueryInterface(riid, ppvOut);
						}
						pStream->Release();
						pStream = NULL;
					}
					else
					{
						// Could not instantiate CStreamOnRawBytes.
						BREAK_IF_DEBUG
						hr = E_OUTOFMEMORY;
					}
				}
				else
				{
					// Oops! The OMF_DATA_VALUE structure purports to store its data immediately
					// in its cbOffset member, but its cbLength member is greater than four.
					BREAK_IF_DEBUG
					hr = OMFOO_E_ASSERTION_FAILURE;
				}
			}
			else
			{
				// Use CStreamOnReadableFile to implement the IStream.
				CStreamOnReadableFile* pStream = new CStreamOnReadableFile(punkOwner);
				if (pStream)
				{
					if (SUCCEEDED(hr = pStream->OpenReadableFile(m_pwzFullPath)))
					{
						if (SUCCEEDED(hr = pStream->SetRegion(sDataValue.cbOffset, sDataValue.cbLength)))
						{
						//	pStream->SetStatStgNameW();
							hr = pStream->QueryInterface(riid, ppvOut);
						}
					}
					pStream->Release();
					pStream = NULL;
				}
				else
				{
					// Could not instantiate CStreamOnReadableFile.
					BREAK_IF_DEBUG
					hr = E_OUTOFMEMORY;
				}
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Data type must be omfi:DataValue (OMF2) or omfi:VarLenBytes (OMF1).
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreCreateHGlobalOnDataValue(BENTO_BLOP& rBlop, DWORD dwProperty,
																	UINT uFlags, HGLOBAL* phGlobal)
{
	OMF_DATA_VALUE	sDataValue	= {0};
	HRESULT			hr			= E_POINTER;
	if (IsBadWritePointer(phGlobal, sizeof(HGLOBAL)))
	{
		goto L_Exit;
	}
	else
	{
		*phGlobal = NULL;
	}

	if (FAILED(hr = CoreQueryDataValue(rBlop, dwProperty, &sDataValue)))
	{
		goto L_Exit;
	}

	// Restrict size to 1 Mebibyte, 20 bits, 1048576 bytes.
	// If caller needs more than that they should use CoreCreateStreamOnDataValue() instead.
	if (sDataValue.cbLength > 0x0000000000100000)
	{
		hr = OMF_E_CANT_COMPLETE;
		goto L_Exit;
	}

	// Pass caller's uFlags argument directly to GlobalAlloc().
	HGLOBAL hGlobal	= GlobalAlloc(uFlags, UINT32(sDataValue.cbLength));
	if (hGlobal == NULL)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_Exit;
	}

	// "Memory objects allocated with GMEM_FIXED always have a lock count of zero.
	// For these objects, the value of the returned pointer is equal to the value
	// of the specified handle."
	PVOID pLockedMem = GlobalLock(hGlobal);
	if (pLockedMem)
	{
		// If cbOffset is negative then the data is stored immediately in its lower four bytes.
	if (sDataValue.cbOffset < 0)
		{
			if (sDataValue.cbLength <= 4)
			{
				CopyMemory(pLockedMem, PBYTE(&sDataValue.cbOffset), UINT32(sDataValue.cbLength));
				hr = S_OK;
			}
			else
			{
				// The OMF_DATA_VALUE structure purports to store its data immediately in
				// its cbOffset member, but its cbLength member is greater than four.
				BREAK_IF_DEBUG
				hr = OMFOO_E_ASSERTION_FAILURE;
			}
		}
		else
		{
			// "If the function succeeds, the return value is a pointer to the first
			// byte of the memory block."
			hr = SeekRead(sDataValue.cbOffset, pLockedMem, UINT32(sDataValue.cbLength));
		}

		// "Memory objects allocated with GMEM_FIXED always have a lock count of zero.
		// If the specified memory block is fixed memory, this function returns TRUE."
		GlobalUnlock(pLockedMem);
	}
	else
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

	// If GlobalLock() or SeekRead() failed then free hGlobal.
	if (FAILED(hr))
	{
		// If the function succeeds, the return value is NULL.
		hGlobal = GlobalFree(hGlobal);
	}

	// Give hGlobal to caller, even when it's NULL.
	*phGlobal = hGlobal;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Retrieves the data type of the array, the reflist of the array, the number of elements in the array,
//	and the total number of bytes that make up the array. The data type must be one of these:
//	omfi:MobIndex, omfi:ObjRefArray, omfi:Position32Array, omfi:Position64Array, omfi:Int32Array or omfi:Int64Array
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreGetArrayInfo(BENTO_BLOP& rBlop, DWORD dwProperty, PDWORD pdwStoredType,
												PDWORD pdwRefList, PULONG pnElements, PULONG pcbArrayData)
{
	PTOCX_ITEM	pItem			= NULL;	// dynamic pointer to the current TOCX_ITEM under scrutiny
	ULONG		nItems			= 0;	// number of contiguous TOC items (aka fragments) associated with this property
	DWORD		dwStoredType	= 0;	// the data type as it is stored in the first TOCX_ITEM

	HRESULT		hr				= E_UNEXPECTED;
	DWORD		dwRefList		= 0;
	BOOL		fHasPrefix		= 0;	// TRUE if data type's payload starts with a 16-bit element count.
	UINT64		cbAccumulate	= 0;	// running total of the size. The sum of all fragments.
	UINT32		cbOneElement	= 0;	// 
	ULONG	nElementsReported	= 0;	// total number of array elements as reported in the file
	ULONG	nElementsCalculated	= 0;	// total number of array elements calculated from the payload size.

	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		hr = OMF_E_OOBJ_NOT_FOUND;
		goto L_CleanupExit;
	}

	// Validate caller's dwProperty argument.
	if (0 == dwProperty)
	{
		hr = OMF_E_PROP_NOT_DEFINED;
		goto L_CleanupExit;
	}

	// Get a pointer to the first TOCX_ITEM structure for this blop.
	pItem	= &m_aToc[rBlop.iFirstItem];

	// Get the total number of items for this blop.
	nItems	= rBlop.wTotalItems;

	// Loop through each TOCX_ITEM structure belonging to the object until we find one
	// whose pItem->dwProperty matches caller's dwProperty argument, or until there are no items left.
	while (nItems)
	{
		if (pItem->dwProperty == dwProperty)
		{
			if (pItem->bStorageMode == SM_REFLISTID)
			{
				// The ID of a bookkeeping object associated with this value.
				// Occurs before any data references.
				// Omitted if the value does not have a bookkeeping object.
				dwRefList = pItem->dwRefList;
			}
			else
			{
				// Found it!
				// Jump out of this loop and go do more stuff.
				goto L_PropExists;
			}
		}

		// The current item is not the requested property.
		// Advance pointer to the next item and decrement the loop count.
		++pItem;
		--nItems;
	}

	// The specified blop does not have the specified property.
	// This isn't necessarily an 'error' or unexpected situation.
	// The specified blop simply does not have the specified property.
	hr = OMF_E_PROP_NOT_FOUND;
	goto L_CleanupExit;

L_PropExists:
	// We found the property!
	// Now get its data type.
	dwStoredType = pItem->dwDataType;

	// Make sure dwStoredType is non-zero.
	// This should NEVER happen, and I have never seen it happen.
	// But if it ever does happen then we need to detect it here.
	if (dwStoredType == 0)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_CleanupExit;
	}

	// Make sure this property's data type is some sort of array.
	if (dwStoredType == m_dwTypeObjRefArray)
	{
		fHasPrefix = TRUE;		// TRUE because payload starts with a 16-bit element count.

		if (m_fOmfVer1)
			cbOneElement = 8;	// sizeof(OMF1_OBJREF_ENTRY)
		else
			cbOneElement = 4;	// sizeof(OMF2_OBJREF_ENTRY)
	}
	else if (dwStoredType == m_dwTypeMobIndex)
	{
		fHasPrefix = TRUE;		// TRUE because payload starts with a 16-bit element count.
		cbOneElement = 20;		// sizeof(OMF1_MOB_INDEX_ENTRY)
	}
	else if (dwStoredType == m_dwTypePos32Array)
	{
		fHasPrefix = TRUE;		// TRUE because payload starts with a 16-bit element count.
		cbOneElement = 4;		// sizeof(UINT32)
	}
	else if (dwStoredType == m_dwTypePos64Array)
	{
		fHasPrefix = TRUE;		// TRUE because payload starts with a 16-bit element count.
		cbOneElement = 8;		// sizeof(UINT64)
	}
	else if (dwStoredType == m_dwTypeInt32Array)
	{
		fHasPrefix = FALSE;		// FALSE because payload does not start with an element count.
		cbOneElement = 4;		// sizeof(UINT32)
	}
	else if (dwStoredType == m_dwTypeInt64Array)
	{
		fHasPrefix = FALSE;		// FALSE because payload does not start with an element count.
		cbOneElement = 8;		// sizeof(UINT64)
	}
	else
	{
		BREAK_IF_DEBUG	// Has this ever happened?
		hr = OMF_E_TYPE_SURPRISE;
		goto L_CleanupExit;
	}

	if (fHasPrefix)
	{
		// When the array begins with a 16-bit element count ...
		// ... make sure the sie is big enough to hold that element count!
		if (pItem->cbLength64 < sizeof(WORD))
		{
			// Detect special case:
			// Sometimes when the array is empty (when it has no elements) there is no element count.
			if (pItem->cbLength64 == 0)
			{
				// All is cool! The array is empty! No problem! We're done!
				hr = S_OK;
				goto L_CleanupExit;
			}
			else
			{
				BREAK_IF_DEBUG	// The array paylaod is only one byte long!
								// Has this ever happened?
				hr = OMF_E_BAD_ARRAY;
				goto L_CleanupExit;
			}
		}

		// Read in that element count.
		if (pItem->bStorageMode == SM_OFFSET)
		{
			WORD wElements	= 0;
			if (FAILED(hr = SeekRead(pItem->cbOffset64, &wElements, sizeof(WORD))))
			{
				goto L_CleanupExit;
			}

			if (m_fOmfBigEndian)
			{
				nElementsReported = Endian16(wElements);
			}
			else
			{
				nElementsReported = wElements;
			}
		}
		else if (pItem->bStorageMode == SM_IMMEDIATE)
		{
			if (m_fOmfBigEndian)
			{
				nElementsReported = Endian16(pItem->aImmediateWords[0]);
			}
			else
			{
				nElementsReported = pItem->aImmediateWords[0];
			}
		}
		else
		{
			BREAK_IF_DEBUG	// Has this ever happened?
			hr = OMF_E_BAD_ARRAY;
			goto L_CleanupExit;
		}
	}
	else
	{
		// When the data type DOES NOT begin with a 16-bit element count.
	}

	// Now loop through each item and calculate the total size, save the result in cbAccumulate.
	// Note that the total size will include the 16-bit element count (if the data type supports it).
	do
	{
		// This test is redunant for the first iteration of this loop.
		if ((pItem->bStorageMode != SM_IMMEDIATE) && (pItem->bStorageMode != SM_OFFSET))
		{
			continue;
		}

		// Is the payload insanely huge?
		if (pItem->cbLength64 >= m_cbLabelOffset64)	// Offset to our file's Bento Label structure.
		{
			BREAK_IF_DEBUG
			// Q: Has this ever happened?
			// A: Yes.
			// See the test file "FG305R_15___GAME NIGHT V.omf"
			// The problem is that the offset is valid but the length is insane.
			// Maybe the length is absolute end???
			//
			//	Object:	0x00010B4D
			//	Class:	JPEG
			//	Prop:	OMFI:JPEG:FrameIndex
			//	Type:	omfi:Position32Array
			//	Format:	4-byte offset, 4-byte length
			//	Offset:	0x68DA1600
			//	Length:	0x973DEC0A
			//
			// This issue remains unsolved. (Fwiw the first 16 bits are 0xFFFF.)
			hr = OMF_E_BAD_ARRAY;
			goto L_CleanupExit;
		}

		// Accumulate byte size.
		cbAccumulate += pItem->cbLength64;

		// Is cbAccumulate is insanely huge?
		// REMEMBER: cbAccumulate is just the number of bytes in an array.
		if (cbAccumulate >= MAX_ONE_MEBIBYTE)
		{
			BREAK_IF_DEBUG	// Has this ever happened?
			hr = OMF_E_BAD_ARRAY;
			goto L_CleanupExit;
		}

		// Is this the final (and probably the only) fragment?
		if (!pItem->fContinued)
		{
			if (fHasPrefix)
			{
				// Calculate the number of elements based on the size (in bytes) of the payload.
				nElementsCalculated = (ULONG(cbAccumulate) - sizeof(WORD))/cbOneElement;

				// Compare result to the number of elements reported in the first 16 bits.
				if (nElementsReported != nElementsCalculated)
				{
					BREAK_IF_DEBUG	// Has this ever happened?
					hr = OMF_E_BAD_ARRAY;
					goto L_CleanupExit;
				}

				if (((nElementsReported * cbOneElement) + sizeof(WORD)) != ULONG(cbAccumulate))
				{
					BREAK_IF_DEBUG	// Has this ever happened?
					hr = OMF_E_BAD_ARRAY;
					goto L_CleanupExit;
				}
			}
			else
			{
				nElementsCalculated = ULONG(cbAccumulate)/cbOneElement;

				if ((nElementsCalculated * cbOneElement) != ULONG(cbAccumulate))
				{
					BREAK_IF_DEBUG	// Has this ever happened?
					hr = OMF_E_BAD_ARRAY;
					goto L_CleanupExit;
				}
			}

			// We are almost done!
			hr = S_OK;
			goto L_CleanupExit;
		}

	// In rare cases when more items remain ...
	// ... then advance pointer to the next TOCX_ITEM structure.
	// ... then make sure the next item is a continuation the same property,
	// ... and then make sure it has the same data type.
	} while ((--nItems) && ((++pItem)->dwProperty == dwProperty) && (pItem->dwDataType == dwStoredType));

	// If the loop terminates here then it's because something went wrong!
	BREAK_IF_DEBUG				// Has this ever happened?
	cbAccumulate = 0;
	hr = OMF_E_BAD_ARRAY;

L_CleanupExit:

	// Save our results for caller - even when there are errors.
	// These are all 'optional'.
	if (pdwStoredType)	*pdwStoredType	= dwStoredType;
	if (pdwRefList)		*pdwRefList		= dwRefList;
	if (pnElements)		*pnElements		= nElementsCalculated;
	if (pcbArrayData)	*pcbArrayData	= ULONG(cbAccumulate);	// okay, safe to demote this UINT64 to a ULONG.

	return hr;
}

//*********************************************************************************************************************
//	Allocate and populate a variable-length OMFOO_MOB_INDEX_ARRAY structure. The data type must be omfi:MobIndex.
//	Note that the prefix for this array (as it is stored in the OMF file) is two bytes long and consists of a single
//	WORD that holds the element count. When we load it into memory we replace it with our own prefix which is eight
//	bytes long and consists of a ULONG for the element count, and a DWORD to hold dwRefList.
//	When the caller is done using this array they must call CoreFreeMobIndexArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreAllocMobIndexArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	POMFOO_MOB_INDEX_ARRAY	pArray	= NULL;

	HRESULT	hr				= E_UNEXPECTED;
	DWORD	dwStoredType	= 0;
	DWORD	dwRefList		= 0;
	ULONG	nElements		= 0;
	ULONG	cbArrayData		= 0;
	UINT32	cbMemAlloc		= 0;

	// Check caller's pointer.
	if (IsBadWritePointer(ppArray, sizeof(PVOID)))
	{
		hr = E_POINTER;
		goto L_CleanupExit;
	}

	// Wipe caller's pointer.
	*ppArray = NULL;

	// Get array info.
	if (FAILED(hr = CoreGetArrayInfo(rBlop, dwProperty, &dwStoredType, &dwRefList, &nElements, &cbArrayData)))
	{
		goto L_CleanupExit;
	}

	// Make sure this property is omfi:MobIndex.
	if (dwStoredType != m_dwTypeMobIndex)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_CleanupExit;
	}

	// Calculate memory requirements.
	// This is step #1 of the prefix switcheroo.
	// We ignore the prefix to the OMF1_MOB_INDEX_ARRAY and make room for the prefix to the OMFOO_MOB_INDEX_ARRAY. 
	cbMemAlloc = OFFSETOF(OMFOO_MOB_INDEX_ARRAY,a) + (nElements * sizeof(OMF1_MOB_INDEX_ENTRY));

	// Reality check.
	if (cbMemAlloc != (cbArrayData + 6))
	{
		BREAK_IF_DEBUG
		hr = OMFOO_E_ASSERTION_FAILURE;
		goto L_CleanupExit;
	}

	// Allocate memory.
	pArray	= POMFOO_MOB_INDEX_ARRAY(MemAlloc(cbMemAlloc));
	if (NULL == pArray)
	{
		BREAK_IF_DEBUG
		hr = OMF_E_OUTOFMEMORY;
		goto L_CleanupExit;
	}

	// Read in the entire OMF1 payload.
	// This is step #2 of the prefix switcheroo.
	// We advance our destination pointer by six bytes before we read in the bytes.
	// After we're done we'll touch up our prefix.
	POMF1_MOB_INDEX_ENTRY pTempOmf1 = POMF1_MOB_INDEX_ENTRY(PBYTE(pArray)+6);
	if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), cbArrayData, pTempOmf1)))
	{
		goto L_CleanupExit;
	}

	// Byte-swap each Bento object ID if necessary.
	if (m_fBentoBigEndian)
	{
		POMF1_MOB_INDEX_ENTRY pEntry = pArray->a;
		for (ULONG i = 0; i < nElements; i++)
		{
			pEntry->dwObject = Endian32(pEntry->dwObject);
			++pEntry;
		}
	}

	// Byte-swap OMF values if necessary.
	if (m_fOmfBigEndian)
	{
		POMF1_MOB_INDEX_ENTRY pEntry = pArray->a;
		for (ULONG i = 0; i < nElements; i++)
		{
			pEntry->dwKey			= Endian32(pEntry->dwKey);
			pEntry->mobID.dwPrefix	= Endian32(pEntry->mobID.dwPrefix);
			pEntry->mobID.dwMajor	= Endian32(pEntry->mobID.dwMajor);
			pEntry->mobID.dwMinor	= Endian32(pEntry->mobID.dwMinor);
			++pEntry;
		}
	}

L_CleanupExit:
	if (SUCCEEDED(hr))
	{
		// This is step #3 of the prefix switcheroo.
		// Save nElements and the dwRefList in our prefix.
		pArray->nElements	= nElements;
		pArray->dwRefList	= dwRefList;	// this 32-bit move clobbers the old 16-bit element count.
		*ppArray			= pArray;

		// Increment the counter (used for debugging only).
		InterlockedIncrement(PLONG(&m_nMobIndexArrays));
	}
	else
	{
		MemFree(pArray);
	}

	return hr;
}

//*********************************************************************************************************************
//	Free the memory allocated with CoreAllocMobIndexArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFreeMobIndexArray(POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	if ((NULL == ppArray) || IsBadReadPointer(ppArray, sizeof(PVOID)) || IsBadWritePointer(ppArray, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	POMFOO_MOB_INDEX_ARRAY pArray = *ppArray;
	*ppArray = NULL;
	if (pArray == NULL)
		return S_FALSE;

	if (MemFree(pArray))
	{
		// Decrement the counter (used for debugging only).
		InterlockedDecrement(PLONG(&m_nMobIndexArrays));
		return S_OK;
	}

	return E_FAIL;
}

//*********************************************************************************************************************
//	Allocate and populate a variable-length OMFOO_OBJREF_ARRAY structure. The data type must be omfi:ObjRefArray.
//	Note that the prefix for this array (as it is stored in the OMF file) is two bytes long and consists of a single
//	WORD that holds the element count. When we load it into memory we replace it with our own prefix which is eight
//	bytes long and consists of a ULONG for the element count, and a DWORD to hold dwRefList.
//	When the caller is done using this array they must call CoreFreeObjRefArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreAllocObjRefArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMFOO_OBJREF_ARRAY* ppArray)
{
	POMFOO_OBJREF_ARRAY		pArray	= NULL;

	HRESULT	hr				= E_UNEXPECTED;
	DWORD	dwStoredType	= 0;
	DWORD	dwRefList		= 0;
	ULONG	nElements		= 0;
	ULONG	cbArrayData		= 0;
	UINT32	cbMemAlloc		= 0;

	// Check caller's pointer.
	if (IsBadWritePointer(ppArray, sizeof(PVOID)))
	{
		hr = E_POINTER;
		goto L_CleanupExit;
	}

	// Wipe caller's pointer.
	*ppArray = NULL;

	// Get array info.
	if (FAILED(hr = CoreGetArrayInfo(rBlop, dwProperty, &dwStoredType, &dwRefList, &nElements, &cbArrayData)))
	{
		goto L_CleanupExit;
	}

	// Make sure this property is omfi:ObjRefArray.
	if (dwStoredType != m_dwTypeObjRefArray)
	{
		hr = OMF_E_TYPE_SURPRISE;
		goto L_CleanupExit;
	}

	if (m_fOmfVer1)
	{
		// Calculate memory requirements.
		// This is step #1 of the prefix switcheroo.
		// We ignore the prefix to the OMF1_OBJREF_ARRAY and make room for the prefix to the OMFOO_OBJREF_ARRAY. 
		cbMemAlloc = OFFSETOF(OMFOO_OBJREF_ARRAY,a) + (nElements * sizeof(OMF1_OBJREF_ENTRY));

		// Reality check.
		if (cbMemAlloc != (cbArrayData + 6))
		{
			BREAK_IF_DEBUG
			hr = __HRESULT_FROM_WIN32(ERROR_ASSERTION_FAILURE);
			goto L_CleanupExit;
		}
		
		// Allocate memory.
		pArray	= POMFOO_OBJREF_ARRAY(MemAlloc(cbMemAlloc));
		if (NULL == pArray)
		{
			BREAK_IF_DEBUG
			hr = OMF_E_OUTOFMEMORY;
			goto L_CleanupExit;
		}

		// Read in the entire OMF1 payload.
		// This is step #2 of the prefix switcheroo.
		// We advance our destination pointer by six bytes before we read in the bytes.
		// After we're done we'll touch up our prefix.
		POMF1_OBJREF_ARRAY pTempOmf1 = POMF1_OBJREF_ARRAY(PBYTE(pArray)+6);
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), cbArrayData, pTempOmf1)))
		{
			goto L_CleanupExit;
		}

		if (nElements > 0)
		{
			POMF1_OBJREF_ENTRY	pSrc	= pTempOmf1->a;
			PDWORD				pDst	= pArray->a;
			SIZE_T				nLoop	= nElements;

			if (m_fBentoBigEndian)
			{
				while (nLoop--)
				{
					*pDst++ = Endian32(pSrc->dwObject);
					++pSrc;
				}
			}
			else
			{
				while (nLoop--)
				{
					*pDst++ = pSrc->dwObject;
					++pSrc;
				}
			}
		}

		hr = S_OK;
		goto L_CleanupExit;
	}
	else
	{
		// Calculate memory requirements.
		// This is step #1 of the prefix switcheroo.
		// We ignore the prefix to the OMF2_OBJREF_ARRAY and make room for the prefix to the OMFOO_OBJREF_ARRAY. 
		cbMemAlloc = OFFSETOF(OMFOO_OBJREF_ARRAY,a) + (nElements * sizeof(OMF2_OBJREF_ENTRY));

		// Reality check.
		if (cbMemAlloc != (cbArrayData + 6))
		{
			BREAK_IF_DEBUG
			hr = OMFOO_E_ASSERTION_FAILURE;
			goto L_CleanupExit;
		}

		// Allocate memory.
		pArray	= POMFOO_OBJREF_ARRAY(MemAlloc(cbMemAlloc));
		if (NULL == pArray)
		{
			BREAK_IF_DEBUG
			hr = OMF_E_OUTOFMEMORY;
			goto L_CleanupExit;
		}

		// Read in the entire OMF2 payload.
		// This is step #2 of the prefix switcheroo.
		// We advance our destination pointer by six bytes before we read in the bytes.
		// After we're done we'll touch up our prefix.
		POMF2_OBJREF_ARRAY pTempOmf2 = POMF2_OBJREF_ARRAY(PBYTE(pArray)+6);
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), cbArrayData, pTempOmf2)))
		{
			goto L_CleanupExit;
		}

		// Byte-swap each Bento object ID if necessary.
		if (m_fBentoBigEndian)
		{
			for (ULONG i = 0; i < nElements; i++)
			{
				pArray->a[i] = Endian32(pArray->a[i]);
			}
		}
	}

L_CleanupExit:
	if (SUCCEEDED(hr))
	{
		// This is step #3 of the prefix switcheroo.
		// Save nElements and the dwRefList in our prefix.
		pArray->nElements	= nElements;
		pArray->dwRefList	= dwRefList;	// this 32-bit move clobbers the old 16-bit element count.
		*ppArray			= pArray;

		// Increment the counter (used for debugging only).
		InterlockedIncrement(PLONG(&m_nObjRefArrays));
	}
	else
	{
		MemFree(pArray);
	}

	return hr;
}

//*********************************************************************************************************************
//	Free the memory allocated with CoreAllocObjRefArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFreeObjRefArray(POMFOO_OBJREF_ARRAY* ppArray)
{
	if ((NULL == ppArray) || IsBadReadPointer(ppArray, sizeof(PVOID)) || IsBadWritePointer(ppArray, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	POMFOO_OBJREF_ARRAY pArray = *ppArray;
	*ppArray = NULL;
	if (pArray == NULL)
		return S_FALSE;

	if (MemFree(pArray))
	{
		// Decrement the counter.
		InterlockedDecrement(PLONG(&m_nObjRefArrays));
		return S_OK;
	}

	return E_FAIL;
}

//*********************************************************************************************************************
//	This is similar to CoreReadPosition64Array() except that this version allocates a OMFOO_POSITION_ARRAY structure
//	and populates the entries. When the caller is done using this array they must call CoreFreePositionArray().
//	The data type can be omfi:Position32Array or omfi:Position64Array. If the data type is omfi:Position32Array
//	then those 'positions' are promoted to 64 bits.
//
//	Note that the prefix for this array (as it existsis stored in the OMF file) is two bytes long and consists of a
//	single WORD that holds the element count. When we load it into memory we replace it with our own prefix, which
//	is eigh bytes long and consists of a ULONG for the element count, and a DWORD to hold dwRefList.
//	When the caller is done using this array they must call CoreFreePositionArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreAllocPositionArray(BENTO_BLOP& rBlop, DWORD dwProperty, POMFOO_POSITION_ARRAY* ppArray)
{
	POMFOO_POSITION_ARRAY	pArray	= NULL;

	HRESULT	hr				= E_UNEXPECTED;
	DWORD	dwStoredType	= 0;
	DWORD	dwRefList		= 0;
	ULONG	nElements		= 0;
	ULONG	cbArrayData		= 0;
	UINT32	cbMemAlloc		= 0;

	// Check caller's pointer.
	if (IsBadWritePointer(ppArray, sizeof(PVOID)))
	{
		hr = E_POINTER;
		goto L_CleanupExit;
	}

	// Wipe caller's pointer.
	*ppArray = NULL;

	// Get array info.
	if (FAILED(hr = CoreGetArrayInfo(rBlop, dwProperty, &dwStoredType, &dwRefList, &nElements, &cbArrayData)))
	{
		goto L_CleanupExit;
	}

	// If the property is omfi:Position32Array.
	if (dwStoredType == m_dwTypePos32Array)
	{
		// Calculate memory requirements.
		// This is step #1 of the prefix switcheroo.
		// We ignore the prefix to the OMF_POSITION32_ARRAY and make room for the prefix to the OMFOO_POSITION_ARRAY.
		cbMemAlloc = OFFSETOF(OMFOO_POSITION_ARRAY,a) + (nElements * sizeof(UINT64));

		// Allocate memory.
		pArray	= POMFOO_POSITION_ARRAY(MemAlloc(cbMemAlloc));
		if (NULL == pArray)
		{
			BREAK_IF_DEBUG
			hr = OMF_E_OUTOFMEMORY;
			goto L_CleanupExit;
		}

		// Read in the entire OMF_POSITION32_ARRAY payload.
		// This is step #2 of the prefix switcheroo.
		// We advance our destination pointer by six bytes before we read in the bytes.
		// After we're done we'll touch up our prefix.
		POMF_POSITION32_ARRAY pPos32 = POMF_POSITION32_ARRAY(PBYTE(pArray)+6);
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), cbArrayData, pPos32)))
		{
			goto L_CleanupExit;
		}

		// Byte-swap each offset if necessary.
		if (m_fOmfBigEndian)
		{
			ULONG i = nElements;
			while (i > 0)
			{
				i--;
				pArray->a[i] = Endian32(pPos32->a[i]);
			}
		}
		else
		{
			ULONG i = nElements;
			while (i > 0)
			{
				i--;
				pArray->a[i] = pPos32->a[i];
			}
		}

		goto L_CleanupExit;
	}

	// If the property is omfi:Position64Array
	if (dwStoredType == m_dwTypePos64Array)
	{
		// Calculate memory requirements.
		// This is step #1 of the prefix switcheroo.
		// We ignore the prefix to the OMF_POSITION64_ARRAY and make room for the prefix to the OMFOO_POSITION_ARRAY.
		cbMemAlloc = OFFSETOF(OMFOO_POSITION_ARRAY,a) + (nElements * sizeof(UINT64));

		// Reality check.
		if (cbMemAlloc != (cbArrayData + 6))
		{
			BREAK_IF_DEBUG
			hr = __HRESULT_FROM_WIN32(ERROR_ASSERTION_FAILURE);
			goto L_CleanupExit;
		}

		// Allocate memory.
		pArray	= POMFOO_POSITION_ARRAY(MemAlloc(cbMemAlloc));
		if (NULL == pArray)
		{
			BREAK_IF_DEBUG
			hr = OMF_E_OUTOFMEMORY;
			goto L_CleanupExit;
		}

		// Read in the entire OMF_POSITION64_ARRAY payload.
		// This is step #2 of the prefix switcheroo.
		// We advance our destination pointer by six bytes before we read in the bytes.
		// After we're done we'll touch up our prefix.
		POMF_POSITION64_ARRAY pPos64 = POMF_POSITION64_ARRAY(PBYTE(pArray)+6);
		if (FAILED(hr = CoreReadStrict(rBlop, dwProperty, (-1), cbArrayData, pPos64)))
		{
			goto L_CleanupExit;
		}

		// Byte-swap each offset if necessary.
		if (m_fOmfBigEndian)
		{
			for (ULONG i = 0; i < nElements; i++)
			{
				pArray->a[i] = Endian64(pArray->a[i]);
			}
		}

		goto L_CleanupExit;
	}

	hr = OMF_E_TYPE_SURPRISE;

L_CleanupExit:
	if (SUCCEEDED(hr))
	{
		// This is step #3 of the prefix switcheroo.
		// Save nElements and the dwRefList in our prefix.
		pArray->nElements	= nElements;
		pArray->dwRefList	= dwRefList;	// this 32-bit move clobbers the old 16-bit element count.
		*ppArray			= pArray;

		// Increment the counter (used for debugging only).
		InterlockedIncrement(PLONG(&m_nPositionArrays));
	}
	else
	{
		MemFree(pArray);
	}

	return hr;
}

//*********************************************************************************************************************
//	Free the memory allocated with CoreAllocPositionArray().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFreePositionArray(POMFOO_POSITION_ARRAY* ppArray)
{
	if ((NULL == ppArray) || IsBadReadPointer(ppArray, sizeof(PVOID)) || IsBadWritePointer(ppArray, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	POMFOO_POSITION_ARRAY pArray = *ppArray;
	*ppArray = NULL;
	if (pArray == NULL)
		return S_FALSE;

	if (MemFree(pArray))
	{
		// Decrement the counter.
		InterlockedDecrement(PLONG(&m_nPositionArrays));
		return S_OK;
	}

	return E_FAIL;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreAllocReferenceList(BENTO_BLOP& rBlop, POMFOO_REFERENCE_LIST* ppList)
{
/*
	POMFOO_REFERENCE_LIST pList	= NULL;
	PTOCX_ITEM	pItem			= NULL;
	PBYTE		pb				= NULL;
	UINT64		cbAccumulate	= 0;
	ULONG		nItems			= 0;
	ULONG		nElements		= 0;
	ULONG		cbExpected		= 0;
	ULONG		cbConsumed		= 0;
	ULONG		cbAlloc			= 0;
	HRESULT		hr				= OMF_E_BAD_ARRAY;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppList, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	// Wipe caller's return value.
	*ppList = NULL;

	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// A reference list object is a type of Bento object, and so it does not have an OMF class FourCC.
	if (rBlop.dwClassFourCC)
	{
		return OMF_E_INVALIDARG;
	}

	// Get a pointer to the first TOCX_ITEM structure for this blop.
	pItem	= &m_aToc[rBlop.iFirstItem];

	// Get the total number of items for this blop.
	nItems	= rBlop._nTocItems;

	for (;;)
	{
		if ((0 == nItems) ||
			(pItem->dwProperty != CM_StdObjID_ObjReferences) ||
			(pItem->dwDataType != CM_StdObjID_ObjRefData))
		{
			BREAK_IF_DEBUG
			goto L_CleanupExit;
		}

		if ((pItem->bStorageMode != SM_IMMEDIATE) && (pItem->bStorageMode != SM_OFFSET))
		{
			BREAK_IF_DEBUG
			goto L_CleanupExit;
		}

		if (pItem->cbLengthHi)
		{
			BREAK_IF_DEBUG
			goto L_CleanupExit;
		}

		// Accumulate size.
		cbAccumulate += pItem->cbLengthLo;
	
		// Is this the final (or perhaps the only) fragment?
		if (!pItem->fContinued)
		{
			// We are done.
			// This is the normal/ expected way to terminate this loop.
			goto L_GotSize;
		}

		// Advance pointer to the next TOCX_ITEM structure.
		++pItem;

		// Decrement the item count.
		--nItems;
	}	// for (;;)	// infinite loop never exits naturally

L_GotSize:

	// Is cbAccumulate greater than or equal to 2^32?
	if (cbAccumulate >= 0x000000100000000)
	{
		hr = OMF_E_BAD_ARRAY;
		BREAK_IF_DEBUG
		goto L_CleanupExit;
	}

	nElements	= UINT32(cbAccumulate) / sizeof(OMFOO_REFERENCE_LIST_ENTRY);

	// Is nElements greater than or equal to 2^16?
	if (nElements >= 0x00010000)
	{
		hr = OMF_E_BAD_ARRAY;
		BREAK_IF_DEBUG
		goto L_CleanupExit;
	}

	cbExpected	= nElements * sizeof(OMFOO_REFERENCE_LIST_ENTRY);

	// Size of data must be evenly divisible by sizeof(OMFOO_REFERENCE_LIST_ENTRY).
	if (UINT32(cbAccumulate) != cbExpected)
	{
		hr = OMF_E_BAD_ARRAY;
		BREAK_IF_DEBUG
		goto L_CleanupExit;
	}

	// Allocate memory.
	cbAlloc	= OFFSETOF(OMFOO_REFERENCE_LIST,a) + cbExpected;
	pList	= POMFOO_REFERENCE_LIST(MemAlloc(cbAlloc));

	if (NULL == pList)
	{
		hr = OMF_E_OUTOFMEMORY;
		goto L_CleanupExit;
	}

	pb		= PBYTE(&pList->a);
	pItem	= &m_aToc[rBlop.iFirstItem];

	do
	{
		if (pItem->bStorageMode == SM_IMMEDIATE)
		{
			// Copy the data from TOC memory to caller's buffer.
			CopyMemory(&pb[cbConsumed], &pItem->dwImmediateDword, pItem->cbLengthLo);
		}
		else if (pItem->bStorageMode == SM_OFFSET)
		{
			// When the data is stored somewhere in the file outside of the TOC ...
			if (FAILED(hr = SeekRead(pItem->cbOffset64, &pb[cbConsumed], pItem->cbLengthLo)))
			{
				BREAK_IF_DEBUG
				goto L_CleanupExit;
			}
		}
		else
		{
			hr = OMF_E_BAD_ARRAY;
			BREAK_IF_DEBUG
			goto L_CleanupExit;
		}

		cbConsumed += pItem->cbLengthLo;
	} while ((pItem++)->fContinued);

	// If we made it here we are going to succeed!
	hr		= S_OK;

	// Swap byte order if necessary.
	if (m_fBentoBigEndian)
	{
		for (UINT i = 0; i < nElements; i++)
		{
			pList->a[i].dwKey		= Endian32(pList->a[i].dwKey);
			pList->a[i].dwObject	= Endian32(pList->a[i].dwObject);
		}
	}

L_CleanupExit:
	if (SUCCEEDED(hr))
	{
		pList->nElements	= nElements;
		*ppList				= pList;

		// Increment the counter (used for debugging only).
		InterlockedIncrement(PLONG(&m_nReferenceLists));
	}
	else
	{
		MemFree(pList);
	}
	return hr;
*/
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Free the memory allocated with CoreAllocReferenceList().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFreeReferenceList(POMFOO_REFERENCE_LIST* ppList)
{
	if ((NULL == ppList) || IsBadReadPointer(ppList, sizeof(PVOID)) || IsBadWritePointer(ppList, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	POMFOO_REFERENCE_LIST pList = *ppList;
	*ppList = NULL;
	if (pList == NULL)
		return S_FALSE;

	if (MemFree(pList))
	{
		// Decrement the counter (used for debugging only).
		InterlockedDecrement(PLONG(&m_nReferenceLists));
		return S_OK;
	}

	return E_FAIL;
}

//*********************************************************************************************************************
//	Array verifier.
//*********************************************************************************************************************
BOOL CContainerLayer01::IsBadMobIndexArray(POMFOO_MOB_INDEX_ARRAY pArray)
{
	if (MemValidate(pArray))
	{
		SIZE_T cb = OFFSETOF(OMFOO_MOB_INDEX_ARRAY, a);
		if (!IsBadReadPointer(pArray, cb))
		{
			cb += sizeof(OMF1_MOB_INDEX_ENTRY) * pArray->nElements;
			if (!IsBadReadPointer(pArray, cb))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//*********************************************************************************************************************
//	Array verifier.
//*********************************************************************************************************************
BOOL CContainerLayer01::IsBadObjRefArray(POMFOO_OBJREF_ARRAY pArray)
{
	if (MemValidate(pArray))
	{
		SIZE_T cb = OFFSETOF(OMFOO_OBJREF_ARRAY, a);
		if (!IsBadReadPointer(pArray, cb))
		{
			cb += sizeof(DWORD) * pArray->nElements;
			if (!IsBadReadPointer(pArray, cb))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//*********************************************************************************************************************
//	Array verifier.
//*********************************************************************************************************************
BOOL CContainerLayer01::IsBadPositionArray(POMFOO_POSITION_ARRAY pArray)
{
	if (MemValidate(pArray))
	{
		SIZE_T cb = OFFSETOF(OMFOO_POSITION_ARRAY, a);
		if (!IsBadReadPointer(pArray, cb))
		{
			cb += sizeof(UINT64) * pArray->nElements;
			if (!IsBadReadPointer(pArray, cb))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//*********************************************************************************************************************
//	Array verifier.
//*********************************************************************************************************************
BOOL CContainerLayer01::IsBadReferenceList(POMFOO_REFERENCE_LIST pList)
{
	if (MemValidate(pList))
	{
		SIZE_T cb = OFFSETOF(OMFOO_REFERENCE_LIST, a);
		if (!IsBadReadPointer(pList, cb))
		{
			cb += sizeof(OMFOO_REFERENCE_LIST_ENTRY) * pList->nElements;
			if (!IsBadReadPointer(pList, cb))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//*********************************************************************************************************************
//	Helper routine for CoreFindIntAttributeByName(), CoreFindObjectAttributeByName(), CoreFindStringAttributeByNameA(),
//	and CoreFindStringAttributeByNameW(). This does the work of verifing the pArray and pszAttributeName arguments,
//	and fetches and returns the attribute's object ID and its OMFI:ATTB:Kind property.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFindAttributeByName(POMFOO_OBJREF_ARRAY pArray, LPCSTR pszAttributeName,
																BOOL fRecurse, DWORD& rATTB, WORD& rAttrKind)
{
	HRESULT	hr			= (-1);
	DWORD	dwATTB		= 0;
	WORD	wAttrKind	= 0;
	CHAR	szCompare[OMF_STRMAX_ATTB_NAME]	= {0};

	DWORD dwPropAttbName = OrdinalToPropertyID(ePropAttbName);
	DWORD dwPropAttbKind = OrdinalToPropertyID(ePropAttbKind);

	rATTB		= 0;
	rAttrKind	= 0;

	// Validate caller's attribute name.
	if (!IsPrintableAscii(pszAttributeName, OMF_STRMAX_ATTB_NAME))
	{
		hr = E_INVALIDARG;
		goto L_Exit;
	}

	if (IsBadObjRefArray(pArray))
	{
		hr = OMF_E_BAD_ARRAY;
		goto L_Exit;
	}

	if ((0 == dwPropAttbName) || (0 == dwPropAttbKind))
	{
		hr = OMF_E_PROP_NOT_DEFINED;
		goto L_Exit;
	}

	for (UINT i = 0; i < pArray->nElements; i++)
	{
		// OMFI:ATTB:Name
		if (SUCCEEDED(CoreReadStringA(GetBlop(pArray->a[i]), dwPropAttbName, sizeof(szCompare), szCompare, NULL)))
		{
			if (0 == lstrcmpA(pszAttributeName, szCompare))
			{
				dwATTB = pArray->a[i];

				// OMFI:ATTB:Kind
				if (FAILED(CoreReadUInt16(GetBlop(dwATTB), dwPropAttbKind, &wAttrKind)))
				{
					wAttrKind = AK_NULL;
				}

				rATTB		= dwATTB;
				rAttrKind	= wAttrKind;
				hr			= S_OK;
				goto L_Exit;
			}
		}
	}

	if (fRecurse)
	{
		DWORD			dwRefList	= 0;
		DWORD				dwATTR	= 0;
		POMFOO_OBJREF_ARRAY	pNested	= NULL;

		DWORD	dwPropAttbObjAttribute	= OrdinalToPropertyID(ePropAttbObjAttribute);
		DWORD	dwPropAttrAttrRefs		= OrdinalToPropertyID(ePropAttrAttrRefs);

		if ((0 == dwPropAttbObjAttribute) || (0 == dwPropAttrAttrRefs))
		{
			goto L_Exit;
		}

		for (UINT i = 0; i < pArray->nElements; i++)
		{
			// OMFI:ATTB:ObjAttribute
			if (FAILED(CoreReadObjRef(GetBlop(pArray->a[i]), dwPropAttbObjAttribute, &dwRefList, &dwATTR)))
			{
				continue;
			}

			// OMFI:ATTR:AttrRefs
			if (FAILED(CoreAllocObjRefArray(GetBlop(dwATTR), dwPropAttrAttrRefs, &pNested)))
			{
				continue;
			}

			hr = CoreFindAttributeByName(pNested, pszAttributeName, FALSE, dwATTB, wAttrKind);
			CoreFreeObjRefArray(&pNested);
			if (SUCCEEDED(hr))
			{
				rATTB		= dwATTB;
				rAttrKind	= wAttrKind;
				hr			= S_OK;
				goto L_Exit;
			}
			//else
			//{
			//	hr = OMF_E_OOBJ_NOT_FOUND;
			//	goto L_Exit;
			//}
		}
	}

	hr = OMF_E_ATTB_NOT_FOUND;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Searches caller's array for an attribute by name and retrieves its OMFI:ATTB:IntAttribute property if found.
//	The attribute's OMFI:ATTB:Kind property must be AK_INT32 or this routine will return MOM_E_UNEXPECTED_TYPE.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFindIntAttributeByName(POMFOO_OBJREF_ARRAY pArray,
															LPCSTR pszAttributeName,
																BOOL fRecurse, PINT32 pn32)
{
	DWORD	dwATTB		= 0;
	WORD	wAttrKind	= 0;
	HRESULT	hr = CoreFindAttributeByName(pArray, pszAttributeName, fRecurse, dwATTB, wAttrKind);

	if (SUCCEEDED(hr))
	{
		if ((AK_NULL == wAttrKind) || (AK_INT32 == wAttrKind))
		{
			// OMFI:ATTB:IntAttribute
			hr = CoreReadInt32(GetBlop(dwATTB), OrdinalToPropertyID(ePropAttbIntAttribute), pn32);
		}
		else
		{
			hr = OMF_E_ATTB_SURPRISE;	// the named attribute is not the specified kind.
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Searches caller's array for an attribute by name and retrieves its OMFI:ATTB:StringAttribute property if found.
//	The attribute's OMFI:ATTB:Kind property must be AK_OBJECT or this routine will return MOM_E_UNEXPECTED_TYPE.
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFindObjectAttributeByName(POMFOO_OBJREF_ARRAY pArray,
																LPCSTR pszAttributeName,
																	BOOL fRecurse, PDWORD pdwObject)
{
	DWORD	dwATTB		= 0;
	WORD	wAttrKind	= 0;
	HRESULT	hr = CoreFindAttributeByName(pArray, pszAttributeName, fRecurse, dwATTB, wAttrKind);

	if (SUCCEEDED(hr))
	{
		if ((AK_NULL == wAttrKind) || (AK_OBJECT == wAttrKind))
		{
			// OMFI:ATTB:ObjAttribute
			DWORD	dwRefList	= 0;
			hr = CoreReadObjRef(GetBlop(dwATTB), OrdinalToPropertyID(ePropAttbObjAttribute), &dwRefList, pdwObject);
		}
		else
		{
			hr = OMF_E_ATTB_SURPRISE;	// the named attribute is not the specified kind.
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Searches caller's array for an attribute by name and retrieves its OMFI:ATTB:DataAttribute property if found.
//	The attribute's OMFI:ATTB:Kind property must be AK_DATAVALUE or this routine will return MOM_E_UNEXPECTED_TYPE.

//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFindDataAttributeByName(POMFOO_OBJREF_ARRAY pArray,
															LPCSTR pszAttributeName,
																BOOL fRecurse, POMF_DATA_VALUE pDataValue)
{
	// Validate caller's destination pointer.
	if (IsBadWritePointer(pDataValue, sizeof(OMF_DATA_VALUE)))
	{
		return E_POINTER;
	}

	// Wipe caller's destination buffer.
	pDataValue->cbOffset = 0;
	pDataValue->cbLength = 0;

	DWORD	dwATTB		= 0;
	WORD	wAttrKind	= 0;

	HRESULT	hr = CoreFindAttributeByName(pArray, pszAttributeName, fRecurse, dwATTB, wAttrKind);

	if (SUCCEEDED(hr))
	{
		if ((AK_NULL == wAttrKind) || (AK_DATAVALUE == wAttrKind))
		{
			// OMFI:ATTB:DataAttribute
			hr = CoreQueryDataValue(GetBlop(dwATTB), OrdinalToPropertyID(ePropAttbDataAttribute), pDataValue);
		}
		else
		{
			hr = OMF_E_ATTB_SURPRISE;	// the named attribute is not the specified kind.
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Searches the array for an attribute by name and retrieves its OMFI:ATTB:StringAttribute property if found.
//	The attribute's OMFI:ATTB:Kind property must be AK_STRING or this routine will return MOM_E_UNEXPECTED_TYPE.
//	This is the ANSI version. It returns the string exactly as it is stored in the payload.
//	See also CoreFindStringAttributeByNameW().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFindStringAttributeByNameA(POMFOO_OBJREF_ARRAY pArray,
																LPCSTR pszAttributeName,
																	BOOL fRecurse, ULONG cchBuffer,
																		PCHAR pBuffer, PULONG pcchRequired)
{
	DWORD	dwATTB		= 0;
	WORD	wAttrKind	= 0;
	HRESULT	hr = CoreFindAttributeByName(pArray, pszAttributeName, fRecurse, dwATTB, wAttrKind);

	if (SUCCEEDED(hr))
	{
		if ((AK_NULL == wAttrKind) || (AK_STRING == wAttrKind))
		{
			// OMFI:ATTB:StringAttribute
			hr = CoreReadStringA(GetBlop(dwATTB), OrdinalToPropertyID(ePropAttbStringAttribute), cchBuffer, pBuffer, pcchRequired);
		}
		else
		{
			hr = OMF_E_ATTB_SURPRISE;	// the named attribute is not the specified kind.
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Searches the array for an attribute by name and retrieves its OMFI:ATTB:StringAttribute property if found.
//	The attribute's OMFI:ATTB:Kind property must be AK_STRING or this routine will return MOM_E_UNEXPECTED_TYPE.
//	This version returns the string in Wide Characters.
//	See also CoreFindStringAttributeByNameA().
//*********************************************************************************************************************
HRESULT CContainerLayer01::CoreFindStringAttributeByNameW(POMFOO_OBJREF_ARRAY pArray,
																LPCSTR pszAttributeName,
																	BOOL fRecurse, ULONG cchBuffer,
																		LPWSTR pBuffer, PULONG pcchRequired)
{
	DWORD	dwATTB		= 0;
	WORD	wAttrKind	= 0;
	HRESULT	hr = CoreFindAttributeByName(pArray, pszAttributeName, fRecurse, dwATTB, wAttrKind);

	if (SUCCEEDED(hr))
	{
		if ((AK_NULL == wAttrKind) || (AK_STRING == wAttrKind))
		{
			// OMFI:ATTB:StringAttribute
			hr = CoreReadStringW(GetBlop(dwATTB), OrdinalToPropertyID(ePropAttbStringAttribute), cchBuffer, pBuffer, pcchRequired);
		}
		else
		{
			hr = OMF_E_ATTB_SURPRISE;	// the named attribute is not the specified kind.
		}
	}

	return hr;
}
