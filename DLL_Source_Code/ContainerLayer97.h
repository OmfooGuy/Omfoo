// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer97.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer96.h"

class COmfObject;

class CContainerLayer97 : public CContainerLayer96
{
protected:
			CContainerLayer97(void);
	virtual	~CContainerLayer97(void);
//	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

public:
	// Callback for IOmfHeader::GetLastModified().
	STDMETHODIMP	GetHeadLastModified(__out POMF_TIMESTAMP pTimeStamp);

public:
	STDMETHODIMP	IteratorCallback(ULONG& rPos, DWORD dwClassFourCC, BOOL fStrict, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	Instantiate(DWORD dwObject, COmfObject* pParent, PVOID pNewReserved, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	Instantiate(BENTO_BLOP& rBlop, COmfObject* pParent, PVOID pNewReserved, REFIID riid, PVOID *ppvOut);
	STDMETHODIMP	IterateObjects(__in_opt DWORD dwClassFourCC, __in BOOL fStrict, __out IOmfooIterator **ppIterator);

private:
	COmfObject*	NewObjectSwitch(BENTO_BLOP& rBlop, COmfObject* pParent, PVOID pNewReserved);
};

//*********************************************************************************************************************
//
//*********************************************************************************************************************
class COmfooIterator : protected IOmfooIterator
{
public:
			COmfooIterator(CContainerLayer97* pContainer);
	virtual ~COmfooIterator(void);

	// IUnknown methods in V-table order.
	STDMETHODIMP	QueryInterface(REFIID riid, PVOID *ppvOut);
	ULONG __stdcall	AddRef(void);
	ULONG __stdcall	Release(void);

	// IOmfooIterator methods in V-table order.
	STDMETHODIMP	GetCount(__out PULONG pnObjects);
	STDMETHODIMP	Reset(void);
//	STDMETHODIMP	Next(__in REFIID riid, __out PVOID *ppvOut);

protected:
	CContainerLayer97*	m_pContainer;
	LONG	m_cRefs;
	ULONG	m_nElements;	// maximum index
	ULONG	m_iPos;			// current index
};

