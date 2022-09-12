// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfEffectDefinition.h
// Copyright (C) 2022 David Miller
// This file is  part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfEffectDefinition
	: private COmfObject
	, private IOmfEffectDefinition
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfEffectDefinition(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfEffectDefinition(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfEffectDefinition methods in V-table order.
	OMFOOAPI	GetEffectIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	OMFOOAPI	GetEffectIDWord(__out PWORD pwEffectID);
	OMFOOAPI	GetEffectName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	OMFOOAPI	GetDescription(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	OMFOOAPI	GetBypassArgID(__out PLONG pnArgID);
	OMFOOAPI	IsTimeWarp(void);
};

