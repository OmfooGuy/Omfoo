// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfAttribute.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfAttribute
	: private COmfObject
	, private IOmfAttribute
{
public:
	static COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfAttribute(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfAttribute(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfAttribute methods in V-table order.
	STDMETHODIMP	GetAttributeNameA(__in ULONG cchBuffer, __out_opt LPSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	GetAttributeNameW(__in ULONG cchBuffer, __out_opt LPWSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	GetAttributeKind(__out PWORD pwAttributeKind);
	STDMETHODIMP	ReadIntAttribute(__out PINT32 pn32);
	STDMETHODIMP	ReadStringAttributeA(__in ULONG cchBuffer, __out_opt LPSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	ReadStringAttributeW(__in ULONG cchBuffer, __out_opt LPWSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	ReadObjAttribute(__in REFIID riid, __out PVOID *ppvOut);

	STDMETHODIMP	ReadBobSizeAttribute(__out PULONG pcbBobSize);
	STDMETHODIMP	ReadBobDataAsString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	ReadBobDataAsRawBytes(__in ULONG cbBuffer, __out_opt PVOID pBuffer, __out_opt PULONG pcbRequired);
	STDMETHODIMP	CreateHGlobalOnDataValue(__in UINT uFlags, __out HGLOBAL* phGlobal);
	STDMETHODIMP	CreateStreamOnDataValue(__in REFIID riid, __out PVOID *ppvOut);

	// OMFI:ATTB:Kind
	WORD	m_wAttbKind;
};

