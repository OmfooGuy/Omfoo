// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfClassDictionaryEntry.h
// Copyright (C) 2022 David Miller
// This file is  part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfClassDictionaryEntry
	: private COmfObject
	, private IOmfClassDictionaryEntry
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfClassDictionaryEntry(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfClassDictionaryEntry(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, void** ppvOut);

	DECLARE_IUNKNOWN

	// IOmfClassDictionaryEntry methods in V-table order.
	STDMETHODIMP	GetClassFourCC(PDWORD pdwFourCC);
	STDMETHODIMP	GetParentEntry(REFIID riid, PVOID *ppvOut);
};

