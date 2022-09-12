// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer99.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer99.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	CContainerLayer99
//*********************************************************************************************************************
HRESULT CContainerLayer99::CreateInstance(LPUNKNOWN punkOuter, REFIID riid, PVOID *ppvOut)
{
	HRESULT			hr = E_UNEXPECTED;
	CContainerLayer99*	pContainer	= 0;

	if (punkOuter)
	{
		hr = CLASS_E_NOAGGREGATION;
		goto L_CleanUpExit;
	}

	// 
	if (NULL == (pContainer = new CContainerLayer99()))
	{
		hr = E_OUTOFMEMORY;
		goto L_CleanUpExit;
	}

	// Call NDQI to get the requested interface.
	hr = pContainer->NonDelegatingQueryInterface(riid, ppvOut);

	// This may or may not delete the CContainerLayer99 object, depending on hr and the caller's riid.
	pContainer->NonDelegatingRelease();

L_CleanUpExit:
	return hr;
}

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer99::CContainerLayer99(void) : m_cRefs(1)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer99::~CContainerLayer99(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT CContainerLayer99::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	LPUNKNOWN pUnk = NULL;

	// Validate caller's pointer.
	if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
		return E_POINTER;

	// Wipe caller's pointer right here right now.
	*ppvOut = NULL;

	// Validate caller's IID.
	if (IsBadReadPointer(&riid, sizeof(IID)))
		return E_INVALIDARG;

	if (riid == __uuidof(IUnknown))
	{
		pUnk = LPUNKNOWN(static_cast<INonDelegatingUnknown*>(this));
	}
	else if (riid == __uuidof(IOmfooReader))
	{
		pUnk = LPUNKNOWN(static_cast<IOmfooReader*>(this));
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
ULONG CContainerLayer99::NonDelegatingAddRef(void)
{
	return InterlockedIncrement(&m_cRefs);
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
ULONG CContainerLayer99::NonDelegatingRelease(void)
{
	ULONG cRefs = InterlockedDecrement(&m_cRefs);
	if (0 == cRefs)
	{
		delete this;
	}
	return cRefs;
}

//*********************************************************************************************************************
//	IUnknown::QueryInterface()
//*********************************************************************************************************************
HRESULT CContainerLayer99::QueryInterface(REFIID riid, PVOID *ppvOut)
{
	return NonDelegatingQueryInterface(riid, ppvOut);
}

//*********************************************************************************************************************
//	IUnknown::AddRef()
//*********************************************************************************************************************
ULONG CContainerLayer99::AddRef(void)
{
	return NonDelegatingAddRef();
}

//*********************************************************************************************************************
//	IUnknown::Release()
//*********************************************************************************************************************
ULONG CContainerLayer99::Release(void)
{
	return NonDelegatingRelease();
}

//*********************************************************************************************************************
//	IOmfooReader::Load()
//*********************************************************************************************************************
HRESULT CContainerLayer99::Load(__in PCWSTR pwzFileName)
{
	// Descend through all of our lower layers until we reach CReadableFile::OpenReadableFile().
	// It will return E_ACCESSDENIED if it is called more than once, regardless of if the first call failed or succeeded.
	// None of our CContainerLayerNN::Load() methods will ever return E_ACCESSDENIED.
	// All of our lower layers have the opportunity to fail after their  __super::Load() is called.
	// That error code propagates up the CContainerLayerNN::Load() chain until it reaches right here right now.
	HRESULT hr = __super::Load(pwzFileName);

	// m_hrFirewall is initialized to E_HANDLE in CContainerLayer00's constructor.
	// We are the only one who can change it - and this is where we do it.
	// If CReadableFile::OpenReadableFile() succeeded, and if all of our lower layers succeeded,
	// then it will return S_OK, and so we update m_hrFirewall with S_OK.
	if (SUCCEEDED(hr))
	{
		// Update ContainerLayer00::m_hrFirewall.
		// This replaces E_HANDLE with S_OK.
		m_hrFirewall = S_OK;
	}
	else
	{
		// If hr is a bona-fide error, and not simply the result of calling Load() more than once,
		// then update m_hrFirewall with a permanent general-purpose failure code.
		if (hr != E_ACCESSDENIED)
		{
			m_hrFirewall = E_FAIL;
		}
	}

	// We always return the error code that was passed up from our lower layers.
	return hr;
}

//*********************************************************************************************************************
//	IOmfooReader::GetCurFile()
//*********************************************************************************************************************
HRESULT CContainerLayer99::GetCurFile(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrFirewall, m_pwzFullPath, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfooReader::IsOmf1()
//*********************************************************************************************************************
HRESULT CContainerLayer99::IsOmf1(void)
{
	HRESULT hr = m_hrFirewall;
	if (SUCCEEDED(hr))
	{
		hr = m_fOmfVer1 ? S_OK : S_FALSE;
	}
	return hr;
}

//*********************************************************************************************************************
//	IOmfooReader::IsOmf2()
//*********************************************************************************************************************
HRESULT CContainerLayer99::IsOmf2(void)
{
	HRESULT hr = m_hrFirewall;
	if (SUCCEEDED(hr))
	{
		hr = m_fOmfVer1 ? S_FALSE : S_OK;
	}
	return hr;
}

//*********************************************************************************************************************
//	IOmfooReader::IsBentoBigEndian()
//*********************************************************************************************************************
HRESULT CContainerLayer99::IsBentoBigEndian(void)
{
	HRESULT hr = m_hrFirewall;
	if (SUCCEEDED(hr))
	{
		hr = m_fBentoBigEndian ? S_OK : S_FALSE;;
	}
	return hr;
}

//*********************************************************************************************************************
//	IOmfooReader::IsOmfBigEndian()
//*********************************************************************************************************************
HRESULT CContainerLayer99::IsOmfBigEndian(void)
{
	HRESULT hr = m_hrFirewall;
	if (SUCCEEDED(hr))
	{
		hr = m_fOmfBigEndian ? S_OK : S_FALSE;;
	}
	return hr;
}

//*********************************************************************************************************************
//	IOmfooReader::GetDetectedCodePage()
//*********************************************************************************************************************
HRESULT CContainerLayer99::GetDetectedCodePage(__out PDWORD pdwCodePage)
{
	if (IsBadWritePointer(pdwCodePage, sizeof(PVOID)))
	{
		return E_POINTER;
	}

	if (SUCCEEDED(m_hrFirewall))
	{
		*pdwCodePage = m_dwDetectedCodePage;
		return S_OK;
	}
	else
	{
		// Don't return 0x0000000 because 0x00000000 is CP_ACP, which is the default ANSI code page.
		*pdwCodePage = 0xFFFFFFFF;
		return m_hrFirewall;
	}
}

//*********************************************************************************************************************
//	IOmfooReader::SetWorkingCodePage()
//*********************************************************************************************************************
HRESULT CContainerLayer99::SetWorkingCodePage(__in DWORD dwDesiredCodePage)
{
	HRESULT hr = m_hrFirewall;
	if (SUCCEEDED(hr))
	{
		CPINFOEXW	cpInfo	= {0};

		// Call GetCPInfoEx() to verify dwDesiredCodePage, and to resolve 'virtual' pages like CP_ACP and CP_MACCP.
		if (GetCPInfoExW(dwDesiredCodePage, DWORD(0), &cpInfo))
		{
			m_dwWorkingCodePage	= cpInfo.CodePage;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	IOmfooReader::GetHeadObject()
//*********************************************************************************************************************
HRESULT CContainerLayer99::GetHeadObject(__in REFIID riid, __out PVOID *ppvOut)
{
	HRESULT	hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		if (SUCCEEDED(hr = m_hrFirewall))
		{
			// The HEAD object is the only OMF object with a fixed object ID.
			// The ID is called CM_StdObjID_TOC in the Bento Specification.
			// Its literal value is 0x00000001.
			// It's always the very first BENTO_BLOP in CReadOmf::m_aBlopTable[].
			hr = Instantiate(m_aBlopTable[0], static_cast<COmfObject*>(NULL), NULL, riid, ppvOut);
		}
	}
	return hr;
}
