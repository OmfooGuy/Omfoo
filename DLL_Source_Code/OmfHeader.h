// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfHeader.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfHeader
	: public COmfObject
	, public IOmfHeader
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfHeader(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfHeader(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfHeader methods in V-table order.
	STDMETHODIMP	GetByteOrder(__out PWORD pwByteOrder);
	STDMETHODIMP	GetVersion(__in BOOL fVerbatim, __out POMF_VERSIONTYPE pVersionType);
	STDMETHODIMP	GetToolkitVersion(__out POMF_PRODUCT_VERSION pVersion);
	STDMETHODIMP	GetLastModified(__out LPFILETIME pFileTime);
	STDMETHODIMP	IterateIdentificationList(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateClassDictionary(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateMediaData(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	FindMediaData(__in POMF_MOB_ID pMobID, __in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	IterateExternalFiles(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateCompositionMobs(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateSourceMobs(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateMobSpine(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IterateDefinitionObjects(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	IteratePrimaryMobs(__out IOmfooIterator **ppIterator);
	STDMETHODIMP	FindMobByID(__in POMF_MOB_ID pMobID, __in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	FindNthMobByName(__in LPCSTR pszMobName, __in ULONG nInstance, 
										__in_opt DWORD dwClassFourCC, __in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	FindTopLevelMob(__in REFIID riid, __out PVOID *ppvOut);
};

