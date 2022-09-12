// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadableFile.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include <winbase.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "ReadableFile.h"
#include "DllMain.h"

//	CReadableFile is 64-bit aware, and can handle files much larger than 4 gig.
//	Nevertheless we still need to define some type of 'maximum file size' in order to prevent internal math overflow.
//	We use this constant internally to test and validate various size/seek position arguments.
//	This is the maximum file size of a NTFS volume for Windows Server 2019. (9007199252643840 bytes)
const unsigned __int64	READFILE_MAX_FILESIZE	= 0x001FFFFFFFE00000;

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CReadableFile::CReadableFile(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CReadableFile::~CReadableFile(void)
{
	// MemFree() is our own heap routine that we define elsewhere.
	MemFree(m_pwzFullPath);

	if (IsValidHandle(m_hFileRead))
	{
		CloseHandle(m_hFileRead);
		m_hFileRead = INVALID_HANDLE_VALUE;
	}
}

//*********************************************************************************************************************
//	This class is not reusable. We can only call OpenReadableFile() once per instance. Even if the first call fails.
//	Our pwzFileName argumemt can be a partial pathname.
//*********************************************************************************************************************
HRESULT CReadableFile::OpenReadableFile(PCWSTR pwzFileName)
{
	BY_HANDLE_FILE_INFORMATION	sFileInfo	= {0};
	ULONG	cchFullPath	= 0;
	ULONG	cchBuffer	= 0;
	HRESULT	hr			= S_OK;

	// First detect the situation where our caller tries to re-initialize us.
	// If the first call to OpenReadableFile() succeeded then m_hFileRead will be a valid non-zero file handle.
	// But if the first call failed then m_hFileRead will be INVALID_HANDLE_VALUE.
	// So either way, if the value in m_hFileRead is non-zero then that means that we've been here before.
	if (m_hFileRead)
	{
		// Fwiw no other method in CReadableFile ever returns E_ACCESSDENIED.
		hr = E_ACCESSDENIED;
		goto L_Exit;
	}

	// Temporarily invalidate m_hFileRead while we ingest the filename.
	// This way if we encounter an error we can go directly to L_Exit.
	m_hFileRead = INVALID_HANDLE_VALUE;

	// Make the first call to GetFullPathNameW() without a buffer pointer to get the required buffer size.
	// The return value will be the required buffer size as measured in WCHARs, including the null-termintor.
	cchBuffer = GetFullPathNameW(pwzFileName, 0, NULL, NULL);
	if (cchBuffer == 0)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_Exit;
	}

	// Allocate memory for the full string.
	// MemAlloc() is our own heap allocation routine that we define elsewhere.
	m_pwzFullPath	= LPWSTR(MemAlloc(cchBuffer * sizeof(WCHAR)));
	if (m_pwzFullPath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto L_Exit;
	}

	// Now call GetFullPathNameW() again with an appropriately sized buffer and save the result in m_pwzFullPath.
	// Note that here our buffer pointer is non-zero, and so the return value does _NOT_ include the null-terminator. 
	cchFullPath = GetFullPathNameW(pwzFileName, cchBuffer, m_pwzFullPath, NULL);

	// Internal reality check.
	// Remember that cchFullPath does not include the null-terminator.
	if ((cchFullPath+1) != cchBuffer)
	{
		// Should always succeed.
		hr = E_UNEXPECTED;
		goto L_Exit;
	}

	// Now open the file with shared read-only rights.
	m_hFileRead = CreateFileW(m_pwzFullPath,		// file to open
						GENERIC_READ,				// open for reading
						FILE_SHARE_READ,			// share for reading
						NULL,						// default security
						OPEN_EXISTING,				// existing file only
						FILE_ATTRIBUTE_READONLY,	// read only
						NULL);						// no attr. template

	// Check for errors.
	if (m_hFileRead == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto L_Exit;
	}

	// Get the file's size, dates, and other info.
	if (GetFileInformationByHandle(m_hFileRead, &sFileInfo))
	{
		// Collect the results.
		m_cbPhysicalEndOfFileLo		= sFileInfo.nFileSizeLow;
		m_cbPhysicalEndOfFileHi		= sFileInfo.nFileSizeHigh;
		m_qwFileCreationTime		= *PQWORD(&sFileInfo.ftCreationTime);
		m_qwFileLastAccessTime		= *PQWORD(&sFileInfo.ftLastAccessTime);
		m_qwFileLastWriteTime		= *PQWORD(&sFileInfo.ftLastWriteTime);

		// These three members uniquely identify a file on a single computer.
		m_dwFileIndexHigh			= sFileInfo.nFileIndexHigh;
		m_dwFileIndexLow			= sFileInfo.nFileIndexLow;
		m_dwVolumeSerialNumber		= sFileInfo.dwVolumeSerialNumber;

		// The seek position for our SeekRead() function is a virtual file position (not a physical file position).
		// This allows this class to read nested and embedded files (aka 'a file within a file').
		// We can set the nested file's start position and length by calling our SetRegion() method.
		// So here (for now, by default) we initialize that region to the entire length of the file.
		m_cbVirtualStartOfFile64	= 0;						// start at the physical beginning
		m_cbVirtualEndOfFile64		= m_cbPhysicalEndOfFile64;	// end at the physical end
	}
	else	// GetFileInformationByHandle() failed.
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		CloseHandle(m_hFileRead);
		m_hFileRead = INVALID_HANDLE_VALUE;
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	The seek position argument for SeekRead() is a virtual file positiom (not necessarily a physical file position).
//	This allows this class to read nested files (a file within a file).
//	This method sets the virtual/nested file length and virtual/nested start position.
//*********************************************************************************************************************
HRESULT CReadableFile::SetRegion(__in UINT64 cbOffset, __in UINT64 cbLength)
{
	if ((cbOffset >= READFILE_MAX_FILESIZE) ||
		(cbLength > READFILE_MAX_FILESIZE) ||
		(cbOffset >= m_cbPhysicalEndOfFile64) ||
		(cbLength > m_cbPhysicalEndOfFile64) ||
		((cbOffset + cbLength) > m_cbPhysicalEndOfFile64))
	{
		// Oops!
		// Our caller has asked us to do something we cannot do.
		BREAK_IF_DEBUG

		return E_INVALIDARG;
	}

	m_cbVirtualStartOfFile64	= cbOffset;
	m_cbVirtualEndOfFile64		= cbLength;
	return S_OK;
}

//*********************************************************************************************************************
//	Public
//	Perform a synchronous seek-and-read in one atomic operation.
//	Note that the cbSeekPos argument is a virtual file position that can be modified with SetRegion().
//	If SetRegion() is never called then the nested region defaults to the entire length of the physical file.
//	This routine will fail if it cannot read every byte.
//*********************************************************************************************************************
HRESULT CReadableFile::SeekRead(UINT64 cbSeekPos, PVOID pDest, UINT32 cbRequest)
{
	OVERLAPPED	sOverlapped			= {0};
	HRESULT		hr					= S_OK;

	union {
		struct	{
			UINT32	cbPhysStartPosLo;		// Lower 32 bits of cbPhysStartPos64
			UINT32	cbPhysStartPosHi;		// Upper 32 bits of cbPhysStartPos64
		};
		UINT64	cbPhysStartPos64;
	};

	// Validate caller's buffer pointer.
	if (IsBadWritePointer(pDest, cbRequest))
	{
		hr = E_POINTER;
		goto L_Exit;
	}

	// This operation is unforgiving. It will fail if it cannot read every byte.
	// It will not allow the caller to request more bytes than are in the nested region.
	// It is the caller’s responsibility to make sure the cbSeekPos and cbRequest arguments fall within bounds.
	if ((cbSeekPos > m_cbVirtualEndOfFile64) ||
		(cbRequest > m_cbVirtualEndOfFile64) ||
		((cbSeekPos + cbRequest) > m_cbVirtualEndOfFile64))
	{
		// Mimic the behaviour of ReadFile().
		hr = __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		goto L_Exit;
	}

	// Calculate the actual physical read start position.
	cbPhysStartPos64 = m_cbVirtualStartOfFile64 + cbSeekPos;

	// Populate our OVERLAPPED structure.
	// "If hFile is not opened with FILE_FLAG_OVERLAPPED and lpOverlapped is not NULL,
	// the read operation starts at the offset that is specified in the OVERLAPPED structure.
	// ReadFile does not return until the read operation is complete, and then the system updates the file pointer."
	sOverlapped.Offset		= cbPhysStartPosLo;
	sOverlapped.OffsetHigh	= cbPhysStartPosHi;

	// Do it.
	// lpNumberOfBytesRead can only be NULL when the lpOverlapped parameter is not NULL.
	if (0 == ReadFile(m_hFileRead, pDest, cbRequest, NULL, &sOverlapped))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Public.
//*********************************************************************************************************************
HRESULT CReadableFile::GetBytesPerSector(__in PCWSTR pwzPath, __out PUINT32 pcbBytesPerSector)
{
	HRESULT hr = E_FAIL;

	// Validate caller's buffer pointer.
	if (IsBadWritePointer(pcbBytesPerSector, sizeof(UINT32)))
	{
		hr = E_POINTER;
	}
	else
	{
		*pcbBytesPerSector = 0;

		// Isolate the root information.
		PCWSTR pwzAfterRoot = PathSkipRootW(pwzPath);
		if (pwzAfterRoot > pwzPath)
		{
			ULONG cchRoot = 1+((ULONG(UINT_PTR(pwzAfterRoot)-UINT_PTR(pwzPath)))/sizeof(WCHAR));
			if (cchRoot < 64)
			{
				WCHAR wzPathRoot[64] = {0};
				lstrcpynW(wzPathRoot, pwzPath, cchRoot);
				if (GetDiskFreeSpaceW(wzPathRoot,
										LPDWORD(NULL),
										LPDWORD(pcbBytesPerSector),
										LPDWORD(NULL),
										LPDWORD(NULL)))
				{
					hr = S_OK;
				}
				else
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
				}
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Public.
//*********************************************************************************************************************
HRESULT CReadableFile::GetBytesAvailable(__in PCWSTR pwzPath, __out PUINT64 pcbBytesAvailable)
{
	HRESULT hr = E_FAIL;

	// Validate caller's buffer pointer.
	if (IsBadWritePointer(pcbBytesAvailable, sizeof(UINT64)))
	{
		hr = E_POINTER;
	}
	else
	{
		*pcbBytesAvailable = 0;

		// Isolate the root information.
		PCWSTR pwzAfterRoot = PathSkipRootW(pwzPath);
		if (pwzAfterRoot > pwzPath)
		{
			ULONG cchRoot = 1+((ULONG(UINT_PTR(pwzAfterRoot)-UINT_PTR(pwzPath)))/sizeof(WCHAR));
			if (cchRoot < 64)
			{
				WCHAR wzPathRoot[64] = {0};
				lstrcpynW(wzPathRoot, pwzPath, cchRoot);
				UINT64 cbBytesAvailable = 0;
				if (GetDiskFreeSpaceExW(wzPathRoot, PULARGE_INTEGER(&cbBytesAvailable), NULL, NULL))
				{
					*pcbBytesAvailable = cbBytesAvailable;
					hr = S_OK;
				}
				else
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
				}
			}
		}
	}

	return hr;
}
