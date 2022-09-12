// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfDataDefinition.h
// Copyright (C) 2022 David Miller
// This file is  part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfDataDefinition
	: private COmfObject
	, private IOmfDataDefinition
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfDataDefinition(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfDataDefinition(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfDataDefinition methods in V-table order.
	STDMETHODIMP	GetDataKindIDString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetDataKindIDWord(PWORD pwDataKindID);
};

