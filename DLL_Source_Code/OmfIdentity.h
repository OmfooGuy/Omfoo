// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfIdentity.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfIdentity
	: private COmfObject
	, private IOmfIdentity
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfIdentity(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfIdentity(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfIdentity methods in V-table order.
	STDMETHODIMP	GetCompanyName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetProductName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetProductVersion(POMF_PRODUCT_VERSION pAppVersion);
	STDMETHODIMP	GetProductVersionString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetProductID(PULONG pnProductID);
	STDMETHODIMP	GetDate(__out LPFILETIME pFileTime);
	STDMETHODIMP	GetToolkitVersion(POMF_PRODUCT_VERSION pToolkitVersion);
	STDMETHODIMP	GetPlatform(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
};

