// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfAttributeArray.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfAttributeArray
	: private COmfObject
	, private IOmfAttributeArray
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfAttributeArray(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfAttributeArray(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID* ppv);

	DECLARE_IUNKNOWN

	// IOmfAttributeArray methods in V-table order.
	STDMETHODIMP	FindAttbByName(__in LPCSTR pszAttributeName, __in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	IterateMembers(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	FindIntAttributeByName(__in LPCSTR pszAttributeName, __out PINT32 pn32);
	STDMETHODIMP	FindStringAttributeByNameA(__in LPCSTR pszAttributeName, __in ULONG cchBuffer,
													__out_opt LPSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	FindStringAttributeByNameW(__in LPCSTR pszAttributeName, __in ULONG cchBuffer,
													__out_opt LPWSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	FindObjectAttributeByName(__in LPCSTR pszAttributeName, __in REFIID riid, __out PVOID *ppvOut);

private:
	POMFOO_OBJREF_ARRAY	m_pAttrArray;
	HRESULT				m_hrAttrArray;
};

