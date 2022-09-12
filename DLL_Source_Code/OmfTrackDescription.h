// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfTrackDescription.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfTrackDescription
	: protected COmfObject
	, protected IOmfTrackDescription
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfTrackDescription(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfTrackDescription(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfTrackDescription methods in V-table order.
	STDMETHODIMP	GetOrigin(__out PINT64 pOrigin);
	STDMETHODIMP	GetTrackID(__out PDWORD pdwTrackID);
	STDMETHODIMP	GetPhysicalTrack(__out PDWORD pdwPhysicalTrack);
	STDMETHODIMP	GetTrackName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
};

