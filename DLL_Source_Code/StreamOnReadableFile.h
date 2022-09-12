// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: StreamOnReadableFile.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ReadableFile.h"

class CStreamOnReadableFile
	: public CReadableFile
	, protected IStream
{
public:
			CStreamOnReadableFile(LPUNKNOWN pUnkOwner);
	virtual ~CStreamOnReadableFile(void);

	STDMETHODIMP	SetStatStgNameW(__in PCWSTR pwzStatStgName);

	// IUnknown methods in V-table order.
	STDMETHODIMP	QueryInterface(REFIID riid, PVOID* ppv);
	ULONG __stdcall	AddRef(void);
	ULONG __stdcall	Release(void);

private:
	// ISequentialStream methods in V-table order.
	STDMETHODIMP	Read(void *pv, ULONG cbRequest, PULONG pcbBytesRead);
	STDMETHODIMP	Write(const void *pv, ULONG cb, PULONG pcbWritten);

	// IStream methods in V-table order.
	STDMETHODIMP	Seek(LARGE_INTEGER liMove, DWORD dwOrigin, ULARGE_INTEGER *pNewPosition);
	STDMETHODIMP	SetSize(ULARGE_INTEGER liNewSize);
	STDMETHODIMP	CopyTo(IStream *pstm, ULARGE_INTEGER cb, PULARGE_INTEGER pcbBytesRead, PULARGE_INTEGER pcbWritten);
	STDMETHODIMP	Commit(DWORD dwCommitFlags);
	STDMETHODIMP	Revert(void);
	STDMETHODIMP	LockRegion(ULARGE_INTEGER liOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHODIMP	UnlockRegion(ULARGE_INTEGER liOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHODIMP	Stat(STATSTG *pstatstg, DWORD dwStatFlag);
	STDMETHODIMP	Clone(IStream **ppstm);

private:
	// Helpers for Seek().
	STDMETHODIMP	SeekSet(INT64 cbMove64);
	STDMETHODIMP	SeekCur(INT64 cbMove64);
	STDMETHODIMP	SeekEnd(INT64 cbMove64);

	WCHAR		m_wzStatStgName[32];
	UINT64		m_cbCurrentStreamPosition;
	LPUNKNOWN	m_pUnkOwner;
	LONG		m_cRefs;
};

