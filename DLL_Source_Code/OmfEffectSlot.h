// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfEffectSlot.h
// Copyright (C) 2022 David Miller
// This file is  part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfEffectSlot
	: public COmfObject
	, public IOmfEffectSlot
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
public:
	COmfEffectSlot(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfEffectSlot(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfEffectSlot methods in V-table order.
	STDMETHODIMP	GetArgID(__out PDWORD pdwArgID);
	STDMETHODIMP	GetArgValue(__in REFIID riid, __out PVOID *ppvOut);
};

