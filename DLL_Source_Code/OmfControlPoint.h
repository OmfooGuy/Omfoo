// Indent=4. Tab=4. ColumnWidth=120. CR/LF. Codepage=ASCII.
// Original filename: OmfControlPoint.h
// Copyright (C) 2022 David Miller
// This file is  part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfControlPoint
	: public COmfObject
	, public IOmfControlPoint
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
public:
	COmfControlPoint(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfControlPoint(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut);

	DECLARE_IUNKNOWN

	// IOmfControlPoint methods in V-table order.
	STDMETHODIMP	GetDataKind(__out PWORD pwDataKind);
	STDMETHODIMP	GetDataKindIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	GetEditHintType(__out PWORD pwHintType);
	STDMETHODIMP	GetTime(__out POMF_RATIONAL pRational);
	STDMETHODIMP	GetTimeAsFloat(__out PFLOAT pFloat);
	STDMETHODIMP	GetTimeAsDouble(__out DOUBLE* pDouble);

	STDMETHODIMP	IteratePointProperties(__out IOmfooIterator **ppIterator);

	STDMETHODIMP	GetValueAsString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	GetValueAsByte(__out PBYTE pb);
	STDMETHODIMP	GetValueAsInt16(__out PINT16 pn16);
	STDMETHODIMP	GetValueAsInt32(__out PINT32 pn32);
	STDMETHODIMP	GetValueAsInt64(__out PINT64 pn64);
	STDMETHODIMP	GetValueAsUInt16(__out PUINT16 pu16);
	STDMETHODIMP	GetValueAsUInt32(__out PUINT32 pu32);
	STDMETHODIMP	GetValueAsUInt64(__out PUINT64 pu64);
	STDMETHODIMP	GetValueAsPoint(__out POMF_POINT pPoint);
	STDMETHODIMP	GetValueAsRational(__out POMF_RATIONAL pRational);
	STDMETHODIMP	GetRationalAsFloat(__out PFLOAT pFloat);
	STDMETHODIMP	GetRationalAsDouble(__out DOUBLE* pDouble);

private:
	POMFOO_OBJREF_ARRAY m_pProperties;
};

