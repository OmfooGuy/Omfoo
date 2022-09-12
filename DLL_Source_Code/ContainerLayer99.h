// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfContainer.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer98.h"

class CContainerLayer99 : public CContainerLayer98
{
friend class CTableIterator;
public:
	static HRESULT __stdcall CreateInstance(LPUNKNOWN punkOuter, REFIID riid, PVOID *ppvOut);

protected:
				CContainerLayer99(void);
	virtual		~CContainerLayer99(void);
//	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

public:
	// INonDelegatingUnknown methods in V-table order.
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);
	ULONG __stdcall	NonDelegatingAddRef(void);
	ULONG __stdcall	NonDelegatingRelease(void);

protected:
	// IUnknown methods in V-table order.
	STDMETHODIMP	QueryInterface(REFIID riid, PVOID *ppvOut);
	ULONG __stdcall	AddRef(void);
	ULONG __stdcall	Release(void);

	// IOmfooReader methods in V-table order.
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);
	STDMETHODIMP	GetCurFile(__in ULONG cchBuffer, __out_opt LPWSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	IsOmf1(void);
	STDMETHODIMP	IsOmf2(void);
	STDMETHODIMP	IsBentoBigEndian(void);
	STDMETHODIMP	IsOmfBigEndian(void);
	STDMETHODIMP	GetDetectedCodePage(__out PDWORD pdwCodePage);
	STDMETHODIMP	SetWorkingCodePage(__in DWORD dwDesiredCodePage);
	STDMETHODIMP	GetHeadObject(__in REFIID riid, __out PVOID *ppvOut);

private:
	// Reference count for IUnknown.
	LONG		m_cRefs;
};

