// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: StreamOnRawBytes.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include <winbase.h>
#include <shlwapi.h>
#include "StreamOnRawBytes.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
CStreamOnRawBytes::CStreamOnRawBytes(LPUNKNOWN pUnkOwner)
	: m_pUnkOwner(NULL)
	, m_cbCurrentStreamPosition(0)
	, m_cbVirtualEndOfFile64(0)
	, m_qwCreationTime(0)
	, m_cRefs(1)
{
	IUnknown_Set(&m_pUnkOwner, pUnkOwner);
	ZeroMemory(m_wzStatStgName, sizeof(m_wzStatStgName));
	ZeroMemory(m_aRawBytes, sizeof(m_aRawBytes));
	GetSystemTimeAsFileTime(LPFILETIME(&m_qwCreationTime));
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CStreamOnRawBytes::~CStreamOnRawBytes(void)
{
}

//*********************************************************************************************************************
//	Public.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Initialize(__in const PBYTE pMem, __in const UINT64 cbMem)
{
	if (cbMem > sizeof(m_aRawBytes))
	{
		return E_INVALIDARG;
	}

	if (IsBadReadPointer(pMem, UINT_PTR(cbMem)))
	{
		return E_POINTER;
	}

	 CopyMemory(m_aRawBytes, pMem, UINT_PTR(cbMem));
	m_cbVirtualEndOfFile64 = cbMem;
	return S_OK;
}

//*********************************************************************************************************************
//	Public.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::SetStatStgNameW(PCWSTR pwzStatStgName)
{
	if (IsBadStringPointerW(pwzStatStgName, ELEMS(m_wzStatStgName)))
	{
		return E_POINTER;
	}

	lstrcpynW(m_wzStatStgName, pwzStatStgName, ELEMS(m_wzStatStgName));
	return S_OK;
}

//*********************************************************************************************************************
//	IUnknown.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::QueryInterface(REFIID riid, PVOID *ppvOut)
{
	// Validate caller's pointer and IID.
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		// Compare...
		if ((riid == IID_IUnknown) || (riid == IID_ISequentialStream) || (riid == IID_IStream))
		{
			InterlockedIncrement(&m_cRefs);
			*ppvOut = static_cast<IStream*>(this);
			hr = S_OK;
		}
		else
		{
			hr = E_NOINTERFACE;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	IUnknown.
//*********************************************************************************************************************
ULONG CStreamOnRawBytes::AddRef(void)
{
	return InterlockedIncrement(&m_cRefs);
}

//*********************************************************************************************************************
//	IUnknown.
//*********************************************************************************************************************
ULONG CStreamOnRawBytes::Release(void)
{
	ULONG cRefs = InterlockedDecrement(&m_cRefs);
	if (0 == cRefs)
	{
		// Get a local pointer to our owner, but don't AddRef() it.
		// A NULL pointer is okay if we don't have an owner.
		LPUNKNOWN	pUnkOwner	= m_pUnkOwner;

		// Delete ourself.
		// We do NOT release our owner in our destructor because our owner might own the allocator that allocated us.
		delete this;

		// Now release our owner (if it exists) using the local pointer.
		IUnknown_AtomicRelease((void**)&pUnkOwner);
	}
	return cRefs;
}

//*********************************************************************************************************************
//	ISequentialStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Write(const void *pv, ULONG cb, PULONG pcbWritten)
{
	UNREFERENCED_PARAMETER(pv);
	UNREFERENCED_PARAMETER(cb);

	if (!IsBadWritePointer(pcbWritten, sizeof(ULONG)))
	{
		*pcbWritten = 0;
	}

	// "The caller does not have the required permissions for writing to this stream object."
	return STG_E_ACCESSDENIED;
}

//*********************************************************************************************************************
//	ISequentialStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Read(void *pv, ULONG cbRequest, PULONG pcbBytesRead)
{
	HRESULT	hr			= S_OK;
	ULONG	cbBytesRead	= 0;

	// pcbBytesRead is optional, but if caller supplied one then make sure we can write to it, and wipe it. 
	if (pcbBytesRead)
	{
		if (IsBadWritePointer(pcbBytesRead, sizeof(ULONG)))
		{
			hr = STG_E_INVALIDPOINTER;
		}
		else
		{
			*pcbBytesRead = 0;
		}
	}

	if (IsBadWritePointer(pv, cbRequest))
	{
		hr = STG_E_INVALIDPOINTER;
	}

	if (SUCCEEDED(hr))
	{
		if (m_cbCurrentStreamPosition > sizeof(m_aRawBytes))
		{
			return E_UNEXPECTED;
		}

		if (cbRequest > sizeof(m_aRawBytes))
		{
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		}

		if ((m_cbCurrentStreamPosition + cbRequest) > sizeof(m_aRawBytes))
		{
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		}

		PBYTE pSrc = &m_aRawBytes[m_cbCurrentStreamPosition];
		CopyMemory(pv, pSrc, cbRequest);
		m_cbCurrentStreamPosition += cbRequest;
		cbBytesRead = cbRequest;

		hr = S_OK;
	}

	// pcbBytesRead is optional, but if caller supplied one then give them their own byte count.
	if (pcbBytesRead)
	{
		*pcbBytesRead = cbBytesRead;
	}

	return hr;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Seek(LARGE_INTEGER liMove, DWORD dwOrigin, PULARGE_INTEGER pNewPosition)
{
	HRESULT	hr = HRESULT_FROM_WIN32(INVALID_SET_FILE_POINTER);

	// Did caller provide a place to save the new position?
	if (pNewPosition)
	{
		if (IsBadWritePointer(pNewPosition, sizeof(ULARGE_INTEGER)))
		{
			return STG_E_INVALIDPOINTER;
		}
		else
		{
			// Set the 'new' position to the 'current' position in case we fail.
			pNewPosition->QuadPart = m_cbCurrentStreamPosition;
		}
	}

	switch (dwOrigin)
	{
	case STREAM_SEEK_SET:
		hr = SeekSet(liMove.QuadPart);
		break;

	case STREAM_SEEK_CUR:
		hr = SeekCur(liMove.QuadPart);
		break;

	case STREAM_SEEK_END:
		hr = SeekEnd(liMove.QuadPart);
		break;

	default:
		hr = STG_E_INVALIDFUNCTION;
		break;
	}

	// Did caller provide a place to save the new position?
	if (pNewPosition)
	{
		// Update the 'new' position with the new position.
		pNewPosition->QuadPart = m_cbCurrentStreamPosition;
	}

	return hr;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::SetSize(ULARGE_INTEGER liNewSize)
{
	UNREFERENCED_PARAMETER(liNewSize);

	// "The caller does not have the required permissions for writing to this stream object."
	return STG_E_ACCESSDENIED;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::CopyTo(IStream *punk,
								ULARGE_INTEGER uliRequested,
								ULARGE_INTEGER *puliRead,
								ULARGE_INTEGER *puliWritten)
{
	UNREFERENCED_PARAMETER(punk);
	UNREFERENCED_PARAMETER(uliRequested);
	UNREFERENCED_PARAMETER(puliRead);
	UNREFERENCED_PARAMETER(puliWritten);

	return STG_E_INVALIDFUNCTION;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Commit(DWORD dwCommitFlags)
{
	UNREFERENCED_PARAMETER(dwCommitFlags);
	return S_OK;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Revert(void)
{
	return S_OK;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::LockRegion(ULARGE_INTEGER liOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	UNREFERENCED_PARAMETER(liOffset);
	UNREFERENCED_PARAMETER(cb);
	UNREFERENCED_PARAMETER(dwLockType);

	// "Locking is not supported at all or the specific type of lock requested is not supported."
	return STG_E_INVALIDFUNCTION;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::UnlockRegion(ULARGE_INTEGER liOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	UNREFERENCED_PARAMETER(liOffset);
	UNREFERENCED_PARAMETER(cb);
	UNREFERENCED_PARAMETER(dwLockType);

	// "Locking is not supported at all or the specific type of lock requested is not supported."
	return STG_E_INVALIDFUNCTION;
}

//*********************************************************************************************************************
//	IStream.
//
//	STORAGE OBJECT NAMING CONVENTIONS:
//	Storage and stream objects are named according to a set of conventions.
//	The name of a root storage object is the actual name of the file in the underlying file system.
//	It conforms to the conventions and restrictions that the file system imposes. File name strings passed
//	to storage-related methods and functions are passed on, uninterpreted and unchanged, to the file system.
//
//	The name of a nested element contained within a storage object is managed by the implementation of the
//	particular storage object. All implementations of storage objects must support nested element names 32
//	characters in length (including the NULL terminator), although some implementations might support longer names.
//	Whether the storage object does any case conversion is implementation-defined. As a result, applications that
//	define element names must choose names that are acceptable whether or not case conversion is performed.
//	The COM implementation of compound files supports names with a maximum length of 32 characters, and does not
//	perform any case conversion.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Stat(STATSTG *pstatstg, DWORD dwStatFlag)
{
	HRESULT hr = S_OK;

	if (IsBadWritePointer(pstatstg, sizeof(STATSTG)))
		return STG_E_INVALIDPOINTER;

	ZeroMemory(pstatstg, sizeof(STATSTG));
	if (dwStatFlag > STATFLAG_NONAME)	return STG_E_INVALIDFLAG;

	// If our caller wants a name, and if we have one to give ...
	if (((dwStatFlag & STATFLAG_NONAME) == 0) && (m_wzStatStgName[0]))
	{
		hr = SHStrDupW(m_wzStatStgName, &pstatstg->pwcsName);
	}

	pstatstg->type				= STGTY_STREAM;
	pstatstg->cbSize.QuadPart	= m_cbVirtualEndOfFile64;
	pstatstg->mtime				= *LPFILETIME(&m_qwCreationTime);
	pstatstg->ctime				= *LPFILETIME(&m_qwCreationTime);
	pstatstg->atime				= *LPFILETIME(&m_qwCreationTime);

	return hr;
}

//*********************************************************************************************************************
//	IStream.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::Clone(IStream **ppstm)
{
	UNREFERENCED_PARAMETER(ppstm);
	return STG_E_UNIMPLEMENTEDFUNCTION;
}

//*********************************************************************************************************************
//	Private helper for IStream::Seek().
//	Come here when the dwOrigin argument is STREAM_SEEK_SET.
//	The new seek pointer is an offset relative to the beginning of the stream.
//	Our job is to translate cbMove64 into a new value for m_cbCurrentStreamPosition.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::SeekSet(INT64 cbMove64)
{
	if ((cbMove64 < 0) || (UINT64(cbMove64) > m_cbVirtualEndOfFile64))
	{
		return HRESULT_FROM_WIN32(INVALID_SET_FILE_POINTER);
	}

	// Easy
	m_cbCurrentStreamPosition = UINT64(cbMove64);
	return S_OK;
}

//*********************************************************************************************************************
//	Private helper for IStream::Seek().
//	Come here when the dwOrigin argument is STREAM_SEEK_CUR.
//	The new seek pointer is an offset relative to the current seek pointer location.
//	Our job is to translate cbMove64 into a new value for m_cbCurrentStreamPosition.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::SeekCur(INT64 cbMove64)
{
	// Math is safe because m_cbVirtualEndOfFile64 can never be greater than sizeof(m_aRawBytes).
	INT64 cbMinimumNegativeSeek	= 0-INT64(m_cbVirtualEndOfFile64);

	if ((cbMove64 < cbMinimumNegativeSeek) || (UINT64(cbMove64) > m_cbVirtualEndOfFile64))
	{
		return HRESULT_FROM_WIN32(INVALID_SET_FILE_POINTER);	// STG_E_SEEKERROR
	}

	INT64 cbTargetPosition64 = INT64(m_cbCurrentStreamPosition) + cbMove64;

	if ((cbTargetPosition64 < 0) || (cbTargetPosition64 > INT64(m_cbVirtualEndOfFile64)))
	{
		return HRESULT_FROM_WIN32(INVALID_SET_FILE_POINTER);	// STG_E_SEEKERROR
	}

	m_cbCurrentStreamPosition = UINT64(cbTargetPosition64);
	return S_OK;
}

//*********************************************************************************************************************
//	Private helper for IStream::Seek().
//	Come here when the dwOrigin argument is STREAM_SEEK_END.
//	The new seek pointer is an offset relative to the end of the stream.
//	Our job is to translate cbMove64 into a new value for m_cbCurrentStreamPosition.
//*********************************************************************************************************************
HRESULT CStreamOnRawBytes::SeekEnd(INT64 cbMove64)
{
	// Math is okay because m_cbVirtualEndOfFile can never be greater than READFILE_MAX_FILESIZE.
	INT64 cbMinimumNegativeSeek	= 0-INT64(m_cbVirtualEndOfFile64);

	if ((cbMove64 < cbMinimumNegativeSeek) || (cbMove64 > 0))
	{
		return HRESULT_FROM_WIN32(INVALID_SET_FILE_POINTER);
	}

	INT64 cbTargetPosition64 = INT64(m_cbCurrentStreamPosition) + cbMove64;

	if ((cbTargetPosition64 < 0) || (UINT64(cbTargetPosition64) > m_cbVirtualEndOfFile64))
	{
		return HRESULT_FROM_WIN32(INVALID_SET_FILE_POINTER);
	}

	m_cbCurrentStreamPosition = UINT64(cbTargetPosition64);
	return S_OK;
}

