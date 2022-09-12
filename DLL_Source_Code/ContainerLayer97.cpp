// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer97.cpp
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
//	CTableIterator
//*********************************************************************************************************************
class CTableIterator : protected COmfooIterator
{
friend class CContainerLayer97;
protected:
	DWORD	m_dwClassFourCC;
	BOOL	m_fStrict;

	// Constructor
	CTableIterator(CContainerLayer97* pContainer, DWORD dwClassFourCC, BOOL fStrict)
		: COmfooIterator(pContainer)
		, m_dwClassFourCC(dwClassFourCC)
		, m_fStrict(fStrict)
	{
		m_nElements = m_pContainer->m_nBlops;
	}

	virtual ~CTableIterator(void)
	{
	}

	STDMETHODIMP Next(__in REFIID riid, __out PVOID *ppvOut)
	{
		return m_pContainer->IteratorCallback(m_iPos, m_dwClassFourCC, m_fStrict, riid, ppvOut);
	}
};

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer97::CContainerLayer97(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer97::~CContainerLayer97(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer97::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//	if (SUCCEEDED(hr))
//	{
//		hr = ????????????
//	}
//	return hr;
//}

//*********************************************************************************************************************
//	Callback routine for the CTableIterator class (which inherits IOmfooIterator).
//	We come here from within IOmfooIterator::Next().
//
//	Possible return codes: S_OK, OMF_E_NO_MORE_ITEMS.
//*********************************************************************************************************************
HRESULT CContainerLayer97::IteratorCallback(ULONG& rPos, DWORD dwClassFourCC, BOOL fStrict, REFIID riid, PVOID *ppvOut)
{
	// Validate caller's pointer.
	HRESULT	hr	= VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		// Assume there are no more items.
		hr		= OMF_E_NO_MORE_ITEMS;
		ULONG i	= rPos;

		// Did caller provide a class fourCC?
		if (dwClassFourCC)
		{
			if (fStrict)
			{
				// While there's more blops in the blop table.
				while (i < m_nBlops)
				{
					if (m_aBlopTable[i].dwFourCC == dwClassFourCC)
					{
						// Instantiate a IOmfObject and query for the specified interface.
						if (SUCCEEDED(Instantiate(m_aBlopTable[i++],
											static_cast<COmfObject*>(NULL),
											NULL,
											riid, ppvOut)))
						{
							hr = S_OK;
							break;
						}
					}
					else
					{
						i++;
					}
				}
			}
			else
			{
				// While there's more blops in the blop table.
				while (i < m_nBlops)
				{
					if (S_OK == IsBlopATypeOf(m_aBlopTable[i], dwClassFourCC))
					{
						// Instantiate a IOmfObject and query for the specified interface.
						if (SUCCEEDED(Instantiate(m_aBlopTable[i++],
											static_cast<COmfObject*>(NULL),
											NULL,
											riid, ppvOut)))
						{
							hr = S_OK;
							break;
						}
					}
					else
					{
						i++;
					}
				}
			}
		}
		else
		{
			// While there are more blops in the blop table.
			while (i < m_nBlops)
			{
				// Instantiate a IOmfObject and query for the specified interface.
				if (SUCCEEDED(Instantiate(m_aBlopTable[i++],
										static_cast<COmfObject*>(NULL),
										NULL,
										riid, ppvOut)))
				{
					hr = S_OK;
					break;
				}
			}
		}

		// Give updated position index back to caller.
		rPos = i;
	}
	return hr;
}

//*********************************************************************************************************************
//	Our main public function.
//	Instantiates a COmfObject on dwObject and then queries it for the interface specified by riid.
//
//	Possible return codes:
//
//	OMF_E_OOBJ_NOT_FOUND	= dwObject is zero or cannot be found in CReadOmf::m_aBlopTable[].
//	E_OUTOFMEMORY			= can't instantiate C++ object because new() operator failed.
//	E_NOINTERFACE			= the wrapper does not expose the requested interface.
//*********************************************************************************************************************
HRESULT CContainerLayer97::Instantiate(DWORD dwObject, COmfObject* pParent,
												PVOID pNewReserved, REFIID riid, PVOID *ppvOut)
{
	// Find the BENTO_BLOP for caller's dwObject and then hand everything off to polymorphic counterpart.
	return Instantiate(GetBlop(dwObject), pParent, pNewReserved, riid, ppvOut);
}

//*********************************************************************************************************************
//	Private, polymorphic hand-off routine for Instantiate(DWORD dwObject ...) 
//	Possible return codes:
//
//	OMF_E_OOBJ_NOT_FOUND	= dwObject is zero or cannot be found in CReadOmf::m_aBlopTable[].
//	E_OUTOFMEMORY			= can't instantiate C++ object because new() operator failed.
//	E_NOINTERFACE			= the wrapper does not expose the requested interface.
//*********************************************************************************************************************
HRESULT CContainerLayer97::Instantiate(BENTO_BLOP& rBlop, COmfObject* pParent,
												PVOID pNewReserved, REFIID riid, PVOID *ppvOut)
{
	// Boilerplate.
	HRESULT	hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		COmfObject* pOmfObject = NULL;
		hr = OMF_E_OOBJ_NOT_FOUND;
		// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
		if (rBlop.dwObject)
		{
			// Instantiate a COmfObject on dwObject.
			pOmfObject = NewObjectSwitch(rBlop, pParent, pNewReserved);

			// Internal integrity check. Make sure our memory allocator succeeded.
			// NewObjectSwitch() will always return something, and can only fail on a memory error.
			if (NULL == pOmfObject)
			{
				BREAK_IF_DEBUG
				hr = E_OUTOFMEMORY;
			}
			else
			{
				// Does the wrapper expose the requested interface?
				hr = pOmfObject->QueryInterface(riid, ppvOut);

				// Release the wrapper regardless of HRESULT and wipe its pointer. This may or may not delete the object.
				pOmfObject->NonDelegatingRelease();
				pOmfObject = NULL;
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Our main function. This is our huge switch statement.
//	Recognizes all OMF1 and OMF2 standard classes (including the "abstract" classes).
//*********************************************************************************************************************
COmfObject* CContainerLayer97::NewObjectSwitch(BENTO_BLOP& rBlop, COmfObject* pParent, PVOID pNewReserved)
{
	COmfObject*	pNewObject	= NULL;
	ULONG		nRetries		= 8;
	DWORD		dwClassFourCC	= rBlop.dwFourCC;

	do {
		for (ULONG i = 0; i < ELEMS(m_aInstantiationTable); i++)
		{
			if (m_aInstantiationTable[i].pfnNewInstance == NULL)
			{
				continue;
			}

			if (dwClassFourCC == m_aInstantiationTable[i].dwClassFourCC)
			{
				pNewObject = m_aInstantiationTable[i].pfnNewInstance(rBlop, this, pParent, pNewReserved);
				goto L_Exit;
			}
		}

		// If we don't recognize the class fourCC then get its parent class fourCC and try again.
		// But don't try again if nRetries is zero.
		dwClassFourCC = InternalGetParentClassFourCC(dwClassFourCC);
	} while (dwClassFourCC && --nRetries);

	// Default - use a generic wrapper.
	// This can only happen if rBlop is not an OMFI class object.
	pNewObject = COmfObject::NewInstance(rBlop, this, pParent, pNewReserved);

L_Exit:
	return pNewObject;
}

//*********************************************************************************************************************
//	IOmfooReader::IterateObjects().
//*********************************************************************************************************************
HRESULT CContainerLayer97::IterateObjects(__in_opt DWORD dwClassFourCC,
														__in BOOL fStrict,
															__out IOmfooIterator **ppIterator)
{
	// Validate caller's pointer.
	if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	// Wipe caller's pointer.
	*ppIterator = NULL;

	// m_hrFirewall lives in CContainerLayer00 and is initialized to E_HANDLE in its constructor.
	// It stays there until IOmfooReader::Load() is called - and until all of the CContainerLayerNN::Load()
	// methods have had their chance to intercept / tweak it.
	// After that it will always be S_OK or E_FAIL.
	HRESULT hr = m_hrFirewall;
	if (SUCCEEDED(hr))
	{
		if ((dwClassFourCC == 0) || IsValidFourCC(dwClassFourCC))
		{
			// See CContainerLayer97::IteratorCallback().
			CTableIterator* pIterator = new CTableIterator(this, dwClassFourCC, fStrict);
			if (pIterator)
			{
				*ppIterator = static_cast<IOmfooIterator*>(pIterator);
				hr = S_OK;
			}
			else
			{
				BREAK_IF_DEBUG
				hr = E_OUTOFMEMORY;
			}
		}
		else
		{
			hr = OMF_E_BAD_FOURCC;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
COmfooIterator::COmfooIterator(CContainerLayer97* pContainer)
	: m_pContainer(pContainer)
	, m_cRefs(1)
	, m_nElements(0)
	, m_iPos(0)
{
	m_pContainer->NonDelegatingAddRef();
}

COmfooIterator::~COmfooIterator(void)
{
}

//*********************************************************************************************************************
// IUnknown
//*********************************************************************************************************************
STDMETHODIMP COmfooIterator::QueryInterface(REFIID riid, PVOID *ppvOut)
{
	// Validate caller's pointer.
	if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
		return E_POINTER;

	// Wipe it now.
	*ppvOut = NULL;

	// Validate caller's IID.
	if (IsBadReadPointer(&riid, sizeof(IID)))
		return E_INVALIDARG;

	// Compare...
	if ((riid == __uuidof(IUnknown)) || (riid == __uuidof(IOmfooIterator)))
	{
		InterlockedIncrement(&m_cRefs);
		*ppvOut = static_cast<IOmfooIterator*>(this);
		return S_OK;
	}

	return E_NOINTERFACE;
}

//*********************************************************************************************************************
// IUnknown
//*********************************************************************************************************************
ULONG __stdcall COmfooIterator::AddRef(void)
{
	return InterlockedIncrement(&m_cRefs);
}

//*********************************************************************************************************************
// IUnknown
//*********************************************************************************************************************
ULONG __stdcall COmfooIterator::Release(void)
{
	ULONG cRefs = InterlockedDecrement(&m_cRefs);
	if (0 == cRefs)
	{
		// Create a local pointer to the container but DO NOT NULLIFY m_pContainer.
		CContainerLayer97* pContainer = m_pContainer;

		// Delete ourself before we release the container.
		delete this;

		// Now release our container using the local pointer on the stack.
		if (pContainer)
		{
			pContainer->NonDelegatingRelease();
		}
	}
	return cRefs;
}

//*********************************************************************************************************************
// IOmfooIterator
//*********************************************************************************************************************
STDMETHODIMP COmfooIterator::GetCount(__out PULONG pnObjects)
{
	// Validate caller's pointer.
	if (IsBadWritePointer(pnObjects, sizeof(ULONG)))
		return E_POINTER;

	*pnObjects = m_nElements;
	return S_OK;
}

//*********************************************************************************************************************
// IOmfooIterator
//*********************************************************************************************************************
STDMETHODIMP COmfooIterator::Reset(void)
{
	m_iPos = 0;
	return S_OK;
}

//*********************************************************************************************************************
// IOmfooIterator
//*********************************************************************************************************************
//STDMETHODIMP COmfooIterator::Next(__in REFIID riid, __out PVOID *ppvOut)
//{
//	return E_NOTIMPL;
//}



