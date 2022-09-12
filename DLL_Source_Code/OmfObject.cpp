// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII// Original filename: OmfObject.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfObject.h"
#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	CObjRefArray.
//	Internal helper class for COmfObject::IterateObjRefArray().
//	This is an implementation of IOmfooIterator for when the data type is omfi:ObjRefArray.
//	It uses an OMFOO_OBJREF_ARRAY that is passed to it in its constructor.
//*********************************************************************************************************************
class CObjRefArray : private COmfooIterator
{
friend class COmfObject;

private:
	COmfObject*			m_pParent;
	POMFOO_OBJREF_ARRAY	m_pArray;
	BOOL				m_fFreeOnRelease;

	//*****************************************************************************************************************
	// Constructor
	CObjRefArray(COmfObject* pParent, POMFOO_OBJREF_ARRAY pArray, BOOL fFreeOnRelease)
		: COmfooIterator(pParent->m_pContainer)
		, m_pParent(pParent)
		, m_pArray(pArray)
		, m_fFreeOnRelease(fFreeOnRelease)
	{
		m_pParent->NonDelegatingAddRef();

		if (pArray)
		{
			m_nElements = pArray->nElements;
		}
	}

	//*****************************************************************************************************************
	// Destructor
	virtual ~CObjRefArray(void)
	{
		POMFOO_OBJREF_ARRAY	pArray	= m_pArray;
		m_pArray					= NULL;

		COmfObject*		pParent	= m_pParent;
		m_pParent					= NULL;

		if (m_fFreeOnRelease)
		{
			// Free the OMFOO_OBJREF_ARRAY.
			m_pContainer->CoreFreeObjRefArray(&pArray);
		}

		if (pParent)
		{
			// Free the COmfObject that instantiated us.
			pParent->NonDelegatingRelease();
		}
	}

	//*****************************************************************************************************************
	// IOmfooIterator
	STDMETHODIMP Next(__in REFIID riid, __out PVOID *ppvOut)
	{
		// Defaut 'error code' is really no error at all.
		// This is our natual return code after all members of the array are exhausted.
		HRESULT hr = OMF_E_NO_MORE_ITEMS;

		// Validate caller's pointer.
		if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
			return E_POINTER;

		// Wipe it now.
		*ppvOut = NULL;

		// Validate caller's IID.
		if (IsBadReadPointer(&riid, sizeof(IID)))
			return E_INVALIDARG;

		// Search until we succeed, or until we reach the end of the array.
		while (m_iPos < m_nElements)
		{
			DWORD dwObjRef = m_pArray->a[m_iPos++];
			hr = m_pContainer->Instantiate(dwObjRef, m_pParent, this, riid, ppvOut);

			if (hr == OMF_E_NOINTERFACE)
			{
				// If the current dwObjRef does not expose the requested interface then just blow it off and move on.
				// Reinitialize hr and loop again.
				hr = OMF_E_NO_MORE_ITEMS;
			}
			else
			{
				// Exit this while() loop and return to caller.
				break;
			}
		}

		return hr;
	}
};

//*********************************************************************************************************************
//	CMobIndexArray.
//	Internal helper class for COmfObject::IterateObjRefArray().
//	This is an implementation of IOmfooIterator for when the data type is omfi:omfi:MobIndex.
//	It uses an OMFOO_MOB_INDEX_ARRAY that is passed to it in its constructor.
//*********************************************************************************************************************
class CMobIndexArray : private COmfooIterator
{
friend class COmfObject;
private:
	COmfObject*			m_pParent;
	POMFOO_MOB_INDEX_ARRAY	m_pArray;
	BOOL					m_fFreeOnRelease;

	//*****************************************************************************************************************
	// Constructor
	CMobIndexArray(COmfObject* pParent, POMFOO_MOB_INDEX_ARRAY pArray, BOOL fFreeOnRelease)
		: COmfooIterator(pParent->m_pContainer)
		, m_pParent(pParent)
		, m_pArray(pArray)
		, m_fFreeOnRelease(fFreeOnRelease)
	{
		m_pParent->NonDelegatingAddRef();

		if (m_pArray)
		{
			m_nElements = m_pArray->nElements;
		}
	}

	//*****************************************************************************************************************
	// Destructor
	virtual ~CMobIndexArray(void)
	{
		POMFOO_MOB_INDEX_ARRAY	pArray	= m_pArray;
		m_pArray						= NULL;

		COmfObject*			pParent	= m_pParent;
		m_pParent						= NULL;

		if (m_fFreeOnRelease)
		{
			// Free the OMFOO_MOB_INDEX_ARRAY.
			m_pContainer->CoreFreeMobIndexArray(&pArray);
		}

		if (pParent)
		{
			// Free the COmfObject that instantiated us.
			pParent->NonDelegatingRelease();
		}
	}

	//*****************************************************************************************************************
	// IOmfooIterator
	STDMETHODIMP Next(__in REFIID riid, __out PVOID *ppvOut)
	{
		// Validate caller's pointer.
		if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
			return E_POINTER;

		// Wipe it now.
		*ppvOut = NULL;

		// Validate caller's IID.
		if (IsBadReadPointer(&riid, sizeof(IID)))
			return E_INVALIDARG;

		// Search until we succeed, or until we reach the end of the array.
		HRESULT hr = OMF_E_NO_MORE_ITEMS;
		while (m_iPos < m_nElements)
		{
			hr = m_pContainer->Instantiate(m_pArray->a[m_iPos++].dwObject, m_pParent, this, riid, ppvOut);

			if (hr == OMF_E_NOINTERFACE)
			{
				// Restore hr and loop again.
				hr = OMF_E_NO_MORE_ITEMS;
			}
			else
			{
				// Exit this while() loop.
				break;
			}
		}

		return hr;
	}
};

//*********************************************************************************************************************
//
//*********************************************************************************************************************
COmfObject* COmfObject::NewInstance(BENTO_BLOP& rBlop,
										CContainerLayer97* pContainer,
										COmfObject* pParent,
										PVOID pNewReserved)
{
	return new COmfObject(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfObject::COmfObject(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: m_cRefs(1)
	, m_pContainer(pContainer)
	, m_pParent(pParent)
{
	*PBENTO_BLOP(this) = rBlop;

	// Add a reference to our container.
	m_pContainer->NonDelegatingAddRef();

	// Make a local copy these to keep our code readable - and because we use them a lot!
	m_fOmfVer1			= m_pContainer->m_fOmfVer1;
	m_fOmfBigEndian		= m_pContainer->m_fOmfBigEndian;

	// Add a reference to our parent (if we have one).
	if (m_pParent)
	{
		m_pParent->NonDelegatingAddRef();
	}
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfObject::~COmfObject(void)
{
	// WARNING! DO NOT RELEASE OUR PARENT OBJECT OR THE CONTAINER IN OUR DESTRUCTOR!
	// We need to delete ourself before we release our parent and container.
	// If we hold the last reference to m_pParent or m_pContainer then those objects will delete themselves
	// before they return from their own NonDelegatingRelease() method.
	// We want to destroy objects in the inverse order in which they were created.
	// We want m_pContainer be the first object created and the last object destroyed.
	// See COmfObject::NonDelegatingRelease().
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfObject::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	LPUNKNOWN pUnk = NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
		return E_POINTER;

	// Wipe it now.
	*ppvOut = NULL;

	// Validate caller's IID.
	if (IsBadReadPointer(&riid, sizeof(IID)))
		return E_INVALIDARG;

	if (riid == __uuidof(IUnknown))
	{
		pUnk = LPUNKNOWN(static_cast<INonDelegatingUnknown*>(this));
	}
	else if (riid == __uuidof(IOmfObject))
	{
		pUnk = LPUNKNOWN(static_cast<IOmfObject*>(this));
	}
	else if (riid == __uuidof(IOobjBackDoor))
	{
		pUnk = LPUNKNOWN(static_cast<IOobjBackDoor*>(this));
	}
	else
	{
		return E_NOINTERFACE;
	}

	NonDelegatingAddRef();
	*ppvOut = pUnk;
	return S_OK;
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
ULONG COmfObject::NonDelegatingAddRef(void)
{
	return InterlockedIncrement(&m_cRefs);
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
ULONG COmfObject::NonDelegatingRelease(void)
{
	ULONG cRefs = InterlockedDecrement(&m_cRefs);
	if (0 == cRefs)
	{
		// Make a local copy of m_pContainer, but don't AddRef() it.
		// We keep m_pContainer in tact so that derived classes can access it in their destructor.
		CContainerLayer97* pContainer = m_pContainer;

		// Make a local copy of m_pParent, but don't AddRef() it.
		// We keep m_pParent in tact so that derived classes can access it in their destructor.
		COmfObject* pParent = m_pParent;

		// Now delete ourself before we release pParent and pContainer.
		// Derived classes should call FreeMobIndexArray), FreeObjRefArray(), etc., during their destructors.
		delete this;

		// Now that we no longer exist we release our parent using the local pointer on the stack.
		if (pParent)
		{
			pParent->NonDelegatingRelease();
		}

		// Now that we no longer exist we release our container using the local pointer on the stack.
		if (pContainer)
		{
			pContainer->NonDelegatingRelease();
		}
	}

	return cRefs;
}

//*********************************************************************************************************************
//	IOobjBackDoor
//*********************************************************************************************************************
HRESULT COmfObject::GetSelfCompareInfo(__out PSELF_COMPARE_INFO pInfo)
{
	if (IsBadWritePointer(pInfo, sizeof(SELF_COMPARE_INFO)))
	{
		return E_POINTER;
	}

	SecureZeroMemory(pInfo, sizeof(SELF_COMPARE_INFO));

	pInfo->pBlop				= PBENTO_BLOP(this);
	pInfo->dwFileIndexHigh		= m_pContainer->m_dwFileIndexHigh;
	pInfo->dwFileIndexLow		= m_pContainer->m_dwFileIndexLow;
	pInfo->dwVolumeSerialNumber	= m_pContainer->m_dwVolumeSerialNumber;

	return S_OK;
}

//*********************************************************************************************************************
//	Queries the object's Container for the interface specified by riid and returns it to the caller.
//	Use this to get from an IOmfObject back to the IOmfooReader that owns it.
//	Note that the current version of the Container only exposes IOmfooReader, but other interfaces may be added later.
//*********************************************************************************************************************
HRESULT COmfObject::GetContainer(__in REFIID riid, __out PVOID *ppvOut)
{
	return m_pContainer->NonDelegatingQueryInterface(riid, ppvOut);
}

//*********************************************************************************************************************
//	Queries the object's Parent for the interface specified by riid and returns it to the caller.
//*********************************************************************************************************************
HRESULT COmfObject::GetParent(__in REFIID riid, __out PVOID *ppvOut)
{
	if (m_pParent)
	{
		return m_pParent->NonDelegatingQueryInterface(riid, ppvOut);
	}
	else
	{
		return OMF_E_CANT_COMPLETE;
	}
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the object's class fourCC (its OMFI:ObjID property) in the HRESULT.
//	OMF2 - Retrieves the object's class fourCC (its OMFI:OOBJ:ObjClass property) in the HRESULT. 
//	This works because fourCCs are positive 32-bit values, and qualify as "success codes with additional information".
//*********************************************************************************************************************
HRESULT COmfObject::GetClassFourCC(void)
{
	if ((this->dwFourCC & 0x80808080)||
		(this->dwFourCC < 0x20202020)||
		(this->dwFourCC > 0x7E7E7E7E))
	{
		return OMF_E_BAD_FOURCC;
	}
	else
	{
		return this->dwFourCC;
	}
}

//*********************************************************************************************************************
//	Compare this IOmfObject to an IUnknown to see if they both represent the same object.
//	If fStrict is TRUE then they both must must have originated from the same container (i.e. the same IOmfooReader).
//	But if fStrict is FALSE then they only need to represent the same OOBJ in the same OMF file.
//	Get it?
//	Returns S_OK if they are the same, or S_FALSE if they are not.
//*********************************************************************************************************************
HRESULT COmfObject::IsSelf(__in IUnknown *punk, __in BOOL fStrict)
{
	// Validate caller's punk.
	if (IsBadUnknown(punk))
	{
		return E_POINTER;
	}

	// Query caller's punk for IOobjBackDoor.
	IOobjBackDoor* pOther = NULL;
	if (FAILED(punk->QueryInterface(IID_PPV_ARGS(&pOther))))
	{
		// If punk->QueryInterface() failed for any reason it is not us.
		return S_FALSE;
	}

	// This is the first test.
	// If both pointers are identical then they belong to the same COmfObject.
	// So this is certainly us!
	IUnknown* pThis = LPUNKNOWN(static_cast<IOobjBackDoor*>(this));
	if (pThis == pOther)
	{
		pOther->Release();
		return S_OK;
	}

	// Even when pointers don't match they still may be two different COmfObjects that represent the same BLOP
	// in the same container and the same OMF file.
	SELF_COMPARE_INFO	sInfo = {0};
	if (FAILED(pOther->GetSelfCompareInfo(&sInfo)))
	{
		// Decrement its ref count, and return its error code.
		pOther->Release();
		return E_UNEXPECTED;
	}

	// This is the second test.
	// But this isn't really necessary - because the first test should have already snagged it.
	if (sInfo.pBlop == PBENTO_BLOP(this))
	{
		pOther->Release();
		return S_OK;
	}

	// If fStrict is TRUE then we are done.
	if (fStrict)
	{
		pOther->Release();
		return S_FALSE;
	}

	// This is the third test.
	// If all of these values match then that means that we have two different COmfObjects that represent
	// the same BLOP in the same OMF file. Even if they originated from two different IOmfooReader[s].
	if ((sInfo.pBlop->dwObject		== this->dwObject)&&
		(sInfo.pBlop->dwFourCC		== this->dwFourCC)&&
		(sInfo.pBlop->iFirstItem	== iFirstItem)&&
		(sInfo.pBlop->wTotalItems	== wTotalItems)&&
		(sInfo.dwFileIndexHigh		== m_pContainer->m_dwFileIndexHigh)&&
		(sInfo.dwFileIndexLow		== m_pContainer->m_dwFileIndexLow)&&
		(sInfo.dwVolumeSerialNumber	== m_pContainer->m_dwVolumeSerialNumber))
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

//*********************************************************************************************************************
//	Tests the object's relationship to a given class.
//	Returns S_OK if the object inherits the given class, or S_FALSE if it does not.
//*********************************************************************************************************************
HRESULT COmfObject::IsATypeOf(__in DWORD dwClassFourCC)
{
	return m_pContainer->IsBlopATypeOf(*this, dwClassFourCC);
}
	
//*********************************************************************************************************************
//	Test if a given property is present in the object.
//	Returns S_OK if the object contains the property specified by pszPropertyName, or OMF_E_PROP_NOT_FOUND if not.
//*********************************************************************************************************************
HRESULT COmfObject::IsPropertyPresent(__in LPCSTR pszPropertyName)
{
	return m_pContainer->RstrIsPropertyPresent(*this, pszPropertyName);
}

//*********************************************************************************************************************
//	Generic read routine. Retrieves raw, unprocessed property bytes, as specified by pszPropertyName.
//	The pDest and pcbRequired arguments are both optional, but you must provide at least one.
//	The OMF data type is not tested. It is up to you to know what you are reading and to byte-swap it if necessary.
//	This method has optional call-twice semantics. You can make the first call with pDest set to NULL, to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the bytes.
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pDest is not large enough to hold the bytes.
//*********************************************************************************************************************
HRESULT COmfObject::ReadRawBytes(__in LPCSTR pszPropertyName,
												__in ULONG cbDest,
												__out_opt PVOID pDest,
													__out_opt PULONG pcbRequired)
{
	return m_pContainer->RstrReadRawBytes(*this, pszPropertyName, cbDest, pDest, pcbRequired);
}

//*********************************************************************************************************************
//	Retrieves a null-terminated 8-bit string, as specified by pszPropertyName.
//	This function reads the character bytes straight from the property data.
//	The OMF data type is not tested, but it should be omfi:String, omfi:data:String, omfi:UniqueName, etc.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL, to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the full string.
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is nots large enough to hold the string.
//	Note that the character counts in cchBuffer and *pcchRequired represent CHARs (1 byte).
//*********************************************************************************************************************
HRESULT COmfObject::ReadStringA(__in LPCSTR pszPropertyName, __in ULONG cchBuffer,
										__out_opt PCHAR pBuffer, __out_opt PULONG pcchRequired)
{
	return m_pContainer->RstrReadStringA(*this, pszPropertyName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Retrieves a null-terminated 16-bit string, as specified by pszPropertyName.
//	Automatically detects the string's code page/character set, and converts it to UTF-16 (aka "Wide Characters").
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL to get the
//	required buffer size, and then call again with an appropriately sized buffer to retrieve the full string.
//	Returns OMF_E_BAD_TEXT if the data is not text (or if it cannot detect the character set),
//	or OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is not large enough to hold the string.
//	Note that the character counts in cchBuffer and *pcchRequired represent WCHARs (two bytes).
//*********************************************************************************************************************
HRESULT COmfObject::ReadStringW(__in LPCSTR pszPropertyName, __in ULONG cchBuffer,
										__out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
{
	return m_pContainer->RstrReadStringW(*this, pszPropertyName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Retrieves a BOOLEAN property, as specified by pszPropertyName.
//	The OMF data type must be omfi:Boolean or this will return OMF_E_TYPE_SURPRISE.
//	Returns OMF_E_PROP_NOT_FOUND if the object does not have the specified property.
//*********************************************************************************************************************
HRESULT COmfObject::ReadBoolean(__in LPCSTR pszPropertyName, __out PBOOLEAN pBoolean)
{
	return m_pContainer->RstrReadBoolean(*this, pszPropertyName, pBoolean);
}

//*********************************************************************************************************************
//	The OMF data type is not tested, but the size of the data is crucial. If the size of the data does not match the
//	size of the integer you are requesting, it returns OMF_E_SIZE_SURPRISE.
//	The values are automatically byte-swapped when necessary.
//*********************************************************************************************************************
HRESULT COmfObject::ReadInt8(__in LPCSTR pszPropertyName, __out PINT8 pn8)
{
	return m_pContainer->RstrReadInt8(*this, pszPropertyName, pn8);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadInt16(__in LPCSTR pszPropertyName, __out PINT16 pn16)
{
	return m_pContainer->RstrReadInt16(*this, pszPropertyName, pn16);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadInt32(__in LPCSTR pszPropertyName, __out PINT32 pn32)
{
	return m_pContainer->RstrReadInt32(*this, pszPropertyName, pn32);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadInt64(__in LPCSTR pszPropertyName, __out PINT64 pn64)
{
	return m_pContainer->RstrReadInt64(*this, pszPropertyName, pn64);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadUInt8(__in LPCSTR pszPropertyName, __out PUINT8 pu8)
{
	return m_pContainer->RstrReadUInt8(*this, pszPropertyName, pu8);
}
//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadUInt16(__in LPCSTR pszPropertyName, __out PUINT16 pu16)
{
	return m_pContainer->RstrReadUInt16(*this, pszPropertyName, pu16);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadUInt32(__in LPCSTR pszPropertyName, __out PUINT32 pu32)
{
	return m_pContainer->RstrReadUInt32(*this, pszPropertyName, pu32);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadUInt64(__in LPCSTR pszPropertyName, __out PUINT64 pu64)
{
	return m_pContainer->RstrReadUInt64(*this, pszPropertyName, pu64);
}

//*********************************************************************************************************************
//	Retrieves a 'length' property as a UINT64. Note that this value is UNSIGNED!
//	The OMF data type must be omfi:Length32 or omfi:Length64, or this method will fail with OMF_E_TYPE_SURPRISE.
//	If the value is an omfi:Length32 it is promoted to 64 bits with zero-padding.
//*********************************************************************************************************************
HRESULT COmfObject::ReadLength64(__in LPCSTR pszPropertyName, __out PUINT64 pLength)
{
	return m_pContainer->RstrReadLength64(*this, pszPropertyName, pLength);
}

//*********************************************************************************************************************
//	Retrieves a 'position' property as a signed INT64. Note that this value is SIGNED!
//	The OMF data type must be omfi:Position32 or omfi:Position64, or this method will fail with OMF_E_TYPE_SURPRISE.
//	If the value is an omfi:Position32 it is sign-extended and promoted to 64 bits.
//*********************************************************************************************************************
HRESULT COmfObject::ReadPosition64(__in LPCSTR pszPropertyName, __out PINT64 pPosition)
{
	return m_pContainer->RstrReadPosition64(*this, pszPropertyName, pPosition);
}

//*********************************************************************************************************************
//	Populates caller's buffer with an array of 32-bit unsigned integers as specified by pszPropertyName.
//	The OMF data type must be omfi:Int32Array or this method will fail with OMF_E_TYPE_SURPRISE.
//
//	On entry the nMaxElements argument holds the maximum size of caller's buffer (as measured in elements).
//	On exit the ULONG pointed to by pnActualElements receives the actual size of the array (as measured in elements).
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necesssary.
//
//	This method has optional call-twice semantics. You can make the first call with pDest set to NULL to get the
//	required size (in elements), and then call again with an appropriately sized buffer to retrieve the full array.
//*********************************************************************************************************************
HRESULT COmfObject::ReadInt32Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT32 pDest,
												__out PULONG pnActualElements)
{
	return m_pContainer->RstrReadInt32Array(*this, pszPropertyName, nMaxElements, pDest, pnActualElements);
}

//*********************************************************************************************************************
//	Populates caller's buffer with an array of 64-bit unsigned integers as specified by pszPropertyName.
//	The OMF data type can be omfi:Int32Array or omfi:Int64Array. When the OMF data type is omfi:Int32Array
//	the 32-bit integers are zero-padded (NOT sign-extended) to 64-bits.
//	On entry the nMaxElements argument holds the maximum size of caller's buffer (as measured in elements).
//	On exit the ULONG pointed to by pnActualElements receives the actual size of the array (as measured in elements).
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necesssary.
//	This method has optional call-twice semantics. You can make the first call with pDest set to NULL to get the
//	required size (in elements), and then call again with an appropriately sized buffer to retrieve the full array.
//*********************************************************************************************************************
HRESULT COmfObject::ReadInt64Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT64 pDest,
												__out PULONG pnActualElements)
{
	return m_pContainer->RstrReadInt64Array(*this, pszPropertyName, nMaxElements, pDest, pnActualElements);
}

//*********************************************************************************************************************
//	Populates caller's buffer with an array of 32-bit unsigned 'positons' as specified by pszPropertyName.
//	The OMF data type must be omfi:Positiom32Array or this method will fail with OMF_E_TYPE_SURPRISE.
//
//	On entry the nMaxElements argument holds the maximum size of caller's buffer (as measured in elements).
//	On exit the ULONG pointed to by pnActualElements receives the actual size of the array (as measured in elements).
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necessary.
//
//	This method has optional call-twice semantics. You can make the first call with pDest set to NULL to get the
//	required size (in elements), and then call again with an appropriately sized buffer to retrieve the full array.
//*********************************************************************************************************************
HRESULT COmfObject::ReadPosition32Array(__in LPCSTR pszPropertyName,
											__in ULONG nMaxElements,
												__out_opt PUINT32 pDest,
													__out PULONG pnActualElements)
{
	return m_pContainer->RstrReadPosition32Array(*this, pszPropertyName, nMaxElements, pDest, pnActualElements);
}

//*********************************************************************************************************************
//	Populates caller's buffer with an array of 64-bit unsigned 'positions' as specified by pszPropertyName.
//	The OMF data type can be omfi:Positiom32Array _or_ omfi:Positiom64Array.
//	If the OMF data type is omfi:Positiom32Array then the 32-bit 'positions' are zero-padded to 64-bits.
//	If the OMF data type is anything else then this method will fail with OMF_E_TYPE_SURPRISE.
//
//	On entry the nMaxElements argument holds the maximum size of caller's buffer (as measured in elements).
//	On exit the ULONG pointed to by pnActualElements receives the actual size of the array (as measured in elements).
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necessary.

//	This method has optional call-twice semantics. You can make the first call with pDest set to NULL to get the
//	required size (in elements), and then call again with an appropriately sized buffer to retrieve the full array.
//*********************************************************************************************************************
HRESULT COmfObject::ReadPosition64Array(__in LPCSTR pszPropertyName,
											__in ULONG nMaxElements,
												__out_opt PUINT64 pDest,
													__out PULONG pnActualElements)
{
	return m_pContainer->RstrReadPosition64Array(*this, pszPropertyName, nMaxElements, pDest, pnActualElements);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadGuid(__in LPCSTR pszPropertyName, __out LPGUID pGuid)
{
	return m_pContainer->RstrReadGuid(*this, pszPropertyName, pGuid);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadMobID(__in LPCSTR pszPropertyName, __out POMF_MOB_ID pMobID)
{
	return m_pContainer->RstrReadMobID(*this, pszPropertyName, pMobID);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadPoint(__in LPCSTR pszPropertyName, __out POMF_POINT pPoint)
{
	return m_pContainer->RstrReadPoint(*this, pszPropertyName, pPoint);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadCompCodeArray(__in LPCSTR pszPropertyName, __out POMF_COMP_CODE_ARRAY pDest)
{
	return m_pContainer->RstrReadCompCodeArray(*this, pszPropertyName, pDest);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadCompSizeArray(__in LPCSTR pszPropertyName, __out POMF_COMP_SIZE_ARRAY pDest)
{
	return m_pContainer->RstrReadCompSizeArray(*this, pszPropertyName, pDest);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadProductVersion(__in LPCSTR pszPropertyName, __out POMF_PRODUCT_VERSION pVersion)
{
	return m_pContainer->RstrReadProductVersion(*this, pszPropertyName, pVersion);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadRational(__in LPCSTR pszPropertyName, __out POMF_RATIONAL pRational)
{
	return m_pContainer->RstrReadRational(*this, pszPropertyName, pRational);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadRationalAsFloat(__in LPCSTR pszPropertyName, __out PFLOAT pFloat)
{
	return m_pContainer->RstrReadRationalAsFloat(*this, pszPropertyName, pFloat);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadRationalAsDouble(__in LPCSTR pszPropertyName, __out DOUBLE* pDouble)
{
	return m_pContainer->RstrReadRationalAsDouble(*this, pszPropertyName, pDouble);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadTimeStamp(__in LPCSTR pszPropertyName, __out POMF_TIMESTAMP pTimeStamp)
{
	return m_pContainer->RstrReadTimeStamp(*this, pszPropertyName, pTimeStamp);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadTimeStampAsFileTime(__in LPCSTR pszPropertyName, __out LPFILETIME pFileTime)
{
	return m_pContainer->RstrReadTimeStampAsFileTime(*this, pszPropertyName, pFileTime);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadVersionType(__in LPCSTR pszPropertyName, __out POMF_VERSIONTYPE pVersionType)
{
	return m_pContainer->RstrReadVersionType(*this, pszPropertyName, pVersionType);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::QueryDataValue(__in LPCSTR pszPropertyName, __out POMF_DATA_VALUE pDataValue)
{
	return m_pContainer->RstrQueryDataValue(*this, pszPropertyName, pDataValue);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadDataValueAsRawBytes(__in LPCSTR pszPropertyName, __in ULONG cbDest,
																__out_opt PVOID pDest, __out_opt PULONG pcbRequired)
{
	return m_pContainer->RstrReadDataValueAsRawBytes(*this, pszPropertyName, cbDest, pDest, pcbRequired);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::CreateHGlobalOnDataValue(__in LPCSTR pszPropertyName, __in UINT uFlags, __out HGLOBAL* phGlobal)
{
	return m_pContainer->RstrCreateHGlobalOnDataValue(*this, pszPropertyName, uFlags, phGlobal);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::CreateStreamOnDataValue(__in LPCSTR pszPropertyName, __in REFIID riid, __out PVOID *ppvOut)
{
	return m_pContainer->RstrCreateStreamOnDataValue(*this, pszPropertyName, riid, ppvOut);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::ReadExtended80AsDouble(__in LPCSTR pszPropertyName, __out DOUBLE* pDouble)
{
	return m_pContainer->RstrReadExtended80AsDouble(*this, pszPropertyName, pDouble);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::GetDataKind(__in LPCSTR pszPropertyName, __out PWORD pwDataKind)
{
	return m_pContainer->RstrGetDataKind(*this, pszPropertyName, pwDataKind);
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT COmfObject::GetDataKindAsStringA(__in LPCSTR pszPropertyName, __in ULONG cchBuffer, PCHAR pBuffer, __out PULONG pcchRequired)
{
	return m_pContainer->RstrGetDataKindAsStringA(*this, pszPropertyName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Instantiates the object specified by pszPropertyName and returns the interface specified by riid.
//	The OMF data type must be omfi:ObjRef or this will fail with the return code OMF_E_TYPE_SURPRISE.
//*********************************************************************************************************************
HRESULT COmfObject::InstantiateObjRef(__in LPCSTR pszPropertyName, __in REFIID riid, __out PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		DWORD	dwRefList	= 0;
		DWORD	dwObjRef	= 0;
		hr = m_pContainer->RstrReadObjRef(*this, pszPropertyName, &dwRefList, &dwObjRef);
		if (SUCCEEDED(hr))
		{
			hr = m_pContainer->Instantiate(dwObjRef, this, NULL, riid, ppvOut);
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Creates an iterator to examine objects in the specified object array.
//	The OMF data type must be omfi:ObjRefArray or this will fail with the return code OMF_E_TYPE_SURPRISE.
//*********************************************************************************************************************
HRESULT COmfObject::IterateObjRefArray(__in LPCSTR pszPropertyName, __out IOmfooIterator **ppIterator)
{
	POMFOO_OBJREF_ARRAY	pArray = NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppIterator = NULL;

	HRESULT hr = m_pContainer->RstrAllocObjRefArray(*this, pszPropertyName, &pArray);
	if (SUCCEEDED(hr))
	{
		// CObjRefArray will free the array memory in its destructor.
		const BOOL fFreeOnRelease = TRUE;
		CObjRefArray* pObjRefArray = new CObjRefArray(this, pArray, fFreeOnRelease);
		if (pObjRefArray)
		{
			*ppIterator = static_cast<IOmfooIterator*>(pObjRefArray);
			hr = S_OK;
		}
		else
		{
			// Free the array if new() operator failed.
			m_pContainer->CoreFreeObjRefArray(&pArray);
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	OMF1 only - Creates an iterator to examine objects in the specified mob index array.
//	The OMF data type must be omfi:MobIndex or this method will fail with the return code OMF_E_TYPE_SURPRISE.
//*********************************************************************************************************************
HRESULT COmfObject::IterateMobIndexArray(__in LPCSTR pszPropertyName, __out IOmfooIterator **ppIterator)
{
	POMFOO_MOB_INDEX_ARRAY	pArray		= NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppIterator = NULL;

	HRESULT hr = m_pContainer->RstrAllocMobIndexArray(*this, pszPropertyName, &pArray);

	// CMobIndexArray will free the array memory in its destructor.
	const BOOL fFreeOnRelease = TRUE;
	CMobIndexArray* pMobIndexArray = new CMobIndexArray(this, pArray, fFreeOnRelease);
	if (pMobIndexArray)
	{
		*ppIterator = static_cast<IOmfooIterator*>(pMobIndexArray);
		hr = S_OK;
	}
	else
	{
		// Free the array if new() operator failed.
		m_pContainer->CoreFreeMobIndexArray(&pArray);
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

//********************************************************************************************************************
//	Protected helpers. These simply forward the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::OrdIsPropertyPresent(PropOrdinal eProperty)
{
	return m_pContainer->RordIsPropertyPresent(*this, eProperty);
}

HRESULT COmfObject::OrdReadStrict(PropOrdinal eProperty, ULONG cbDest, PVOID pDest)
{
	return m_pContainer->RordReadStrictNoType(*this, eProperty, cbDest, pDest);
}

HRESULT COmfObject::OrdReadRawBytes(PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired)
{
	return m_pContainer->RordReadRawBytes(*this, eProperty, cbDest, pDest, pcbRequired);
}

HRESULT COmfObject::OrdReadStringA(PropOrdinal eProperty, ULONG cchBuffer, PCHAR pBuffer, PULONG pcchRequired)
{
	return m_pContainer->RordReadStringA(*this, eProperty, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfObject::OrdReadStringW(PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return m_pContainer->RordReadStringW(*this, eProperty, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfObject::OrdReadBoolean(PropOrdinal eProperty, PBOOLEAN pBoolean)
{
	return m_pContainer->RordReadBoolean(*this, eProperty, pBoolean);
}

HRESULT COmfObject::OrdReadInt8(PropOrdinal eProperty, PINT8 pn8)
{
	return m_pContainer->RordReadInt8(*this, eProperty, pn8);
}

HRESULT COmfObject::OrdReadInt16(PropOrdinal eProperty, PINT16 pn16)
{
	return m_pContainer->RordReadInt16(*this, eProperty, pn16);
}

HRESULT COmfObject::OrdReadInt32(PropOrdinal eProperty, PINT32 pn32)
{
	return m_pContainer->RordReadInt32(*this, eProperty, pn32);
}

HRESULT COmfObject::OrdReadInt64(PropOrdinal eProperty, PINT64 pn64)
{
	return m_pContainer->RordReadInt64(*this, eProperty, pn64);
}

HRESULT COmfObject::OrdReadUInt8(PropOrdinal eProperty, PUINT8 pu8)
{
	return m_pContainer->RordReadUInt8(*this, eProperty, pu8);
}

HRESULT COmfObject::OrdReadUInt16(PropOrdinal eProperty, PUINT16 pu16)
{
	return m_pContainer->RordReadUInt16(*this, eProperty, pu16);
}

HRESULT COmfObject::OrdReadUInt32(PropOrdinal eProperty, PUINT32 pu32)
{
	return m_pContainer->RordReadUInt32(*this, eProperty, pu32);
}

HRESULT COmfObject::OrdReadUInt64(PropOrdinal eProperty, PUINT64 pu64)
{
	return m_pContainer->RordReadUInt64(*this, eProperty, pu64);
}

HRESULT COmfObject::OrdReadFourCC(PropOrdinal eProperty, PDWORD pdwFourCC)
{
	return m_pContainer->RordReadFourCC(*this, eProperty, pdwFourCC);
}

HRESULT COmfObject::OrdReadLength64(PropOrdinal eProperty, PUINT64 pLength)
{
	return m_pContainer->RordReadLength64(*this, eProperty, pLength);
}

HRESULT COmfObject::OrdReadPosition64(PropOrdinal eProperty, PINT64 pPosition)
{
	return m_pContainer->RordReadPosition64(*this, eProperty, pPosition);
}

HRESULT COmfObject::OrdReadDataOffset64(PropOrdinal eProperty, PUINT64 pcbOffset)
{
	return m_pContainer->RordReadDataOffset64(*this, eProperty, pcbOffset);
}

HRESULT COmfObject::OrdReadInt32Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return m_pContainer->RordReadInt32Array(*this, eProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT COmfObject::OrdReadInt64Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return m_pContainer->RordReadInt64Array(*this, eProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT COmfObject::OrdReadPosition32Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT32 pDest, PULONG pnActualElements)
{
	return m_pContainer->RordReadPosition32Array(*this, eProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT COmfObject::OrdReadPosition64Array(PropOrdinal eProperty, ULONG nMaxElements, PUINT64 pDest, PULONG pnActualElements)
{
	return m_pContainer->RordReadPosition64Array(*this, eProperty, nMaxElements, pDest, pnActualElements);
}

HRESULT COmfObject::OrdReadGuid(PropOrdinal eProperty, LPGUID pGuid)
{
	return m_pContainer->RordReadGuid(*this, eProperty, pGuid);
}

HRESULT COmfObject::OrdReadMobID(PropOrdinal eProperty, POMF_MOB_ID pMobID)
{
	return m_pContainer->RordReadMobID(*this, eProperty, pMobID);
}

HRESULT COmfObject::OrdReadObjRef(PropOrdinal eProperty, PDWORD pdwRefList, PDWORD pdwObjRef)
{
	return m_pContainer->RordReadObjRef(*this, eProperty, pdwRefList, pdwObjRef);
}

HRESULT COmfObject::OrdReadPoint(PropOrdinal eProperty, POMF_POINT pPoint)
{
	return m_pContainer->RordReadPoint(*this, eProperty, pPoint);
}

HRESULT COmfObject::OrdReadProductVersion(PropOrdinal eProperty, POMF_PRODUCT_VERSION pVersion)
{
	return m_pContainer->RordReadProductVersion(*this, eProperty, pVersion);
}

HRESULT COmfObject::OrdReadRational(PropOrdinal eProperty, POMF_RATIONAL pRational)
{
	return m_pContainer->RordReadRational(*this, eProperty, pRational);
}

HRESULT COmfObject::OrdReadRationalAsFloat(PropOrdinal eProperty, PFLOAT pFloat)
{
	return m_pContainer->RordReadRationalAsFloat(*this, eProperty, pFloat);
}

HRESULT COmfObject::OrdReadRationalAsDouble(PropOrdinal eProperty, DOUBLE* pDouble)
{
	return m_pContainer->RordReadRationalAsDouble(*this, eProperty, pDouble);
}

HRESULT COmfObject::OrdReadTimeStamp(PropOrdinal eProperty, POMF_TIMESTAMP pTimeStamp)
{
	return m_pContainer->RordReadTimeStamp(*this, eProperty, pTimeStamp);
}

HRESULT COmfObject::OrdReadTimeStampAsFileTime(PropOrdinal eProperty, LPFILETIME pFileTime)
{
	return m_pContainer->RordReadTimeStampAsFileTime(*this, eProperty, pFileTime);
}

HRESULT COmfObject::OrdReadVersionType(PropOrdinal eProperty, POMF_VERSIONTYPE pVersionType)
{
	return m_pContainer->RordReadVersionType(*this, eProperty, pVersionType);
}

HRESULT COmfObject::OrdReadCompCodeArray(PropOrdinal eProperty, POMF_COMP_CODE_ARRAY pDest)
{
	return m_pContainer->RordReadCompCodeArray(*this, eProperty, pDest);
}

HRESULT COmfObject::OrdReadCompSizeArray(PropOrdinal eProperty, POMF_COMP_SIZE_ARRAY pDest)
{
	return m_pContainer->RordReadCompSizeArray(*this, eProperty, pDest);
}

HRESULT COmfObject::OrdQueryDataValue(PropOrdinal eProperty, POMF_DATA_VALUE pDataValue)
{
	return m_pContainer->RordQueryDataValue(*this, eProperty, pDataValue);
}

HRESULT COmfObject::OrdReadDataValueAsRawBytes(PropOrdinal eProperty, ULONG cbDest, PVOID pDest, PULONG pcbRequired)
{
	return m_pContainer->RordReadDataValueAsRawBytes(*this, eProperty, cbDest, pDest, pcbRequired);
}

HRESULT COmfObject::OrdReadDataValueAsStringW(PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pDest, PULONG pcchRequired)
{
	return m_pContainer->RordReadDataValueAsStringW(*this, eProperty, cchBuffer, pDest, pcchRequired);
}

HRESULT COmfObject::OrdCreateStreamOnDataValue(PropOrdinal eProperty, REFIID riid, PVOID *ppvOut)
{
	return m_pContainer->RordCreateStreamOnDataValue(*this, eProperty, riid, ppvOut);
}

HRESULT COmfObject::OrdCreateHGlobalOnDataValue(PropOrdinal eProperty, UINT uFlags, HGLOBAL* phGlobal)
{
	return m_pContainer->RordCreateHGlobalOnDataValue(*this, eProperty, uFlags, phGlobal);
}

HRESULT COmfObject::OrdGetDataKind(PropOrdinal eProperty, PWORD pwDataKind)
{
	return m_pContainer->RordGetDataKind(*this, eProperty, pwDataKind);
}

HRESULT COmfObject::OrdGetDataKindAsStringA(PropOrdinal eProperty, ULONG cchBuffer, PCHAR pDest, PULONG pcchRequired)
{
	return m_pContainer->RordGetDataKindAsStringA(*this, eProperty, cchBuffer, pDest, pcchRequired);
}

HRESULT COmfObject::OrdGetDataKindAsStringW(PropOrdinal eProperty, ULONG cchBuffer, PWCHAR pDest, PULONG pcchRequired)
{
	return m_pContainer->RordGetDataKindAsStringW(*this, eProperty, cchBuffer, pDest, pcchRequired);
}

//********************************************************************************************************************
//	Protected helper.
//********************************************************************************************************************
HRESULT COmfObject::OrdInstantiateObjRef(PropOrdinal eProperty, REFIID riid, PVOID *ppvOut)
{
	DWORD	dwRefList	= 0;
	DWORD	dwObjRef	= 0;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
		return E_POINTER;

	// Wipe it now.
	*ppvOut = NULL;

	// Validate caller's IID.
	if (IsBadReadPointer(&riid, sizeof(IID)))
		return E_INVALIDARG;

	HRESULT hr = m_pContainer->RordReadObjRef(*this, eProperty, &dwRefList, &dwObjRef);
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->Instantiate(dwObjRef, this, NULL, riid, ppvOut);
	}
	return hr;
}

//********************************************************************************************************************
//	Protected helper.
//********************************************************************************************************************
HRESULT COmfObject::OrdIterateObjRefArray(PropOrdinal eProperty, IOmfooIterator **ppIterator)
{
	POMFOO_OBJREF_ARRAY	pArray		= NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppIterator = NULL;

	HRESULT hr = m_pContainer->RordAllocObjRefArray(*this, eProperty, &pArray);
	if (SUCCEEDED(hr))
	{
		// CObjRefArray will free the array memory in its destructor.
		const BOOL fFreeOnRelease = TRUE;
		CObjRefArray* pObjRefArray = new CObjRefArray(this, pArray, fFreeOnRelease);
		if (pObjRefArray)
		{
			*ppIterator = static_cast<IOmfooIterator*>(pObjRefArray);
			hr = S_OK;
		}
		else
		{
			// Free the array if new() operator failed.
			m_pContainer->CoreFreeObjRefArray(&pArray);
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

//********************************************************************************************************************
//	Protected helper.
//********************************************************************************************************************
HRESULT COmfObject::OrdIterateMobIndexArray(PropOrdinal eProperty, IOmfooIterator **ppIterator)
{
	POMFOO_MOB_INDEX_ARRAY	pArray		= NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppIterator = NULL;

	HRESULT hr = m_pContainer->RordAllocMobIndexArray(*this, eProperty, &pArray);

	// CMobIndexArray will free the array memory in its destructor.
	const BOOL fFreeOnRelease = TRUE;
	CMobIndexArray* pMobIndexArray = new CMobIndexArray(this, pArray, fFreeOnRelease);
	if (pMobIndexArray)
	{
		*ppIterator = static_cast<IOmfooIterator*>(pMobIndexArray);
		hr = S_OK;
	}
	else
	{
		// Free the array if new() operator failed.
		m_pContainer->CoreFreeMobIndexArray(&pArray);
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

//********************************************************************************************************************
//	Protected helper.
//********************************************************************************************************************
HRESULT COmfObject::WrapObjRefArray(POMFOO_OBJREF_ARRAY pArray, IOmfooIterator **ppIterator)
{
	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppIterator = NULL;

	// Validate caller's OMF_OBJREF_ARRAY.
	if (NULL == pArray)
		return OMF_E_PROP_NOT_FOUND;

	if (m_pContainer->IsBadObjRefArray(pArray))
		return OMF_E_BAD_ARRAY;

	// Tell CObjRefArray to NOT free the array in its destructor.
	const BOOL fFreeOnRelease = FALSE;
	CObjRefArray* pObjRefArray = new CObjRefArray(this, pArray, fFreeOnRelease);
	if (NULL == pObjRefArray)
	{
		return E_OUTOFMEMORY;
	}

	*ppIterator = static_cast<IOmfooIterator*>(pObjRefArray);
	return S_OK;
}

//********************************************************************************************************************
//	Protected helper.
//********************************************************************************************************************
HRESULT COmfObject::WrapMobIndexArray(POMFOO_MOB_INDEX_ARRAY pArray, IOmfooIterator **ppIterator)
{
	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppIterator = NULL;

	// Validate caller's OMF_MOB_INDEX_ARRAY.
	if (NULL == pArray)
		return OMF_E_PROP_NOT_FOUND;

	if (m_pContainer->IsBadMobIndexArray(pArray))
		return E_INVALIDARG;

	// Tell CMobIndexArray to NOT free the OMF_MOB_INDEX_ARRAY memory in its destructor.
	const BOOL fFreeOnRelease = FALSE;
	CMobIndexArray* pMobIndexArray = new CMobIndexArray(this, pArray, fFreeOnRelease);
	if (NULL == pMobIndexArray)
	{
		return E_OUTOFMEMORY;
	}

	*ppIterator = static_cast<IOmfooIterator*>(pMobIndexArray);
	return S_OK;
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::OrdGetArrayInfo(PropOrdinal eProperty, PDWORD pdwDataType,
											PDWORD pdwRefList, PULONG pnElements, PULONG pcbData)
{
	return m_pContainer->RordGetArrayInfo(*this, eProperty, pdwDataType, pdwRefList, pnElements, pcbData);
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::OrdAllocMobIndexArray(PropOrdinal eProperty, POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	return m_pContainer->RordAllocMobIndexArray(*this, eProperty, ppArray);
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::OrdAllocObjRefArray(PropOrdinal eProperty, POMFOO_OBJREF_ARRAY* ppArray)
{
	return m_pContainer->RordAllocObjRefArray(*this, eProperty, ppArray);
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::OrdAllocPositionArray(PropOrdinal eProperty, POMFOO_POSITION_ARRAY* ppArray)
{
	return m_pContainer->RordAllocPositionArray(*this, eProperty, ppArray);
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::FreeMobIndexArray(POMFOO_MOB_INDEX_ARRAY* ppArray)
{
	return m_pContainer->CoreFreeMobIndexArray(ppArray);
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::FreeObjRefArray(POMFOO_OBJREF_ARRAY* ppArray)
{
	return m_pContainer->CoreFreeObjRefArray(ppArray);
}

//********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//********************************************************************************************************************
HRESULT COmfObject::FreePositionArray(POMFOO_POSITION_ARRAY* ppArray)
{
	return m_pContainer->CoreFreePositionArray(ppArray);
}

//*********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//*********************************************************************************************************************
DWORD COmfObject::GetWorkingCodePage(void)
{
	return m_pContainer->m_dwWorkingCodePage;
}

//*********************************************************************************************************************
//	Protected helper. Simply forwards the call back to the container.
//*********************************************************************************************************************
DWORD COmfObject::OrdinalToPropertyID(__in PropOrdinal eProperty)
{
	return m_pContainer->OrdinalToPropertyID(eProperty);
}

