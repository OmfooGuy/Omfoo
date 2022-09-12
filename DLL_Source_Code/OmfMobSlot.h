// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMobSlot.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

class COmfMobSlot
	: private COmfObject
	, private IOmfMobSlot
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfMobSlot(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfMobSlot(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, void ** ppvOut);

	DECLARE_IUNKNOWN

	// IOmfMobSlot methods in V-table order
	STDMETHODIMP	GetSegmentClass(__out PDWORD pdwFourCC);
	STDMETHODIMP	GetSegment(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	GetEditRate(__out POMF_RATIONAL pEditRate);
	STDMETHODIMP	GetEditRateAsFloat(__out PFLOAT pFloat);
	STDMETHODIMP	GetEditRateAsDouble(__out DOUBLE* pDouble);
	STDMETHODIMP	GetTrackDesc(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	GetOrigin(__out PINT64 pOrigin);
	STDMETHODIMP	GetTrackID(__out PDWORD pdwTrackID);
	STDMETHODIMP	GetCompatibleTrackID(__out PDWORD pdwTrackID);
	STDMETHODIMP	GetTrackName(__in ULONG cchBuffer, __out_opt LPWSTR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP	GetPhysicalTrack(__out PDWORD pdwPhysicalTrack);
	STDMETHODIMP	GetFillerProxy(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	GetTrackAttributeList(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP	GetSessionAttributeList(__in REFIID riid, __out PVOID *ppvOut);

	DWORD	m_dwCPNT;			// rev 1 & 2
	DWORD	m_dwTRKD;			// rev 2 only
	HRESULT	m_hrSlotIndex;
};

