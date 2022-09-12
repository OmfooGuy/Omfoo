// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadableFile.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
class CReadableFile
{
protected:
	enum {
	MAX_PATH_FILENAME		= 256,		// Maximum length of an NTFS filename with extension and NULL terminator.
	MAX_PATH_COMPONENT		= 256,		// Maximum length of an NTFS path component with NULL terminator.
	MAX_PATH_VOLUMELABEL	= 32,		// Maximum length of an NTFS volume label.
	MAX_PATH_SERVER			= 64,		// This is our own restriction. Real world server names are always shorter.
	MAX_PATH_SHARE			= 64,		// This is our own restriction. Real world share names are always shorter.
	MAX_PATH_DESIGNATOR		= 4,		// Fixed length of a root path when the prefix is a drive designator. "C:\"
	MAX_PATH_UNC_ROOT		= 128,		// Maximum length of a root that conforms to the Universal Naming Convention.
	MAX_PATH_FULLPATH		= 32768,	// Maximum length of an NTFS full path.
										// Because the "MaximumLength" member of a UNICODE_STRING struct is a USHORT.
										// - and because that USHORT is a BYTE count (and not a WCHAR count).
										// - and because the UNICODE_STRING struct does not require a null-terminator.
	};

protected:
			CReadableFile(void);
	virtual	~CReadableFile(void);

public:
	HRESULT	OpenReadableFile(__in PCWSTR pwzFileName);
	HRESULT	SetRegion(__in UINT64 cbOffset, __in UINT64 cbLength);
	HRESULT	SeekRead(__in UINT64 cbSeekPos, __out PVOID pDest, __in UINT32 cbRequest);

	static HRESULT GetBytesPerSector(__in PCWSTR pwzPath, __out PUINT32 pcbBytesPerSector);
	static HRESULT GetBytesAvailable(__in PCWSTR pwzPath, __out PUINT64 pcbBytesAvailable);

protected:
	__forceinline bool	IsValidHandle(HANDLE handle)
						{return(!((handle == NULL)||(handle == INVALID_HANDLE_VALUE)));}

	__forceinline bool	IsBadHandle(HANDLE handle)
						{return((handle == NULL)||(handle == INVALID_HANDLE_VALUE));}

private:
	HANDLE	m_hFileRead;	// our file handle

	union {
	struct {
	UINT32	m_cbPhysicalEndOfFileLo;
	UINT32	m_cbPhysicalEndOfFileHi;
	};
	UINT64	m_cbPhysicalEndOfFile64;
	};

protected:
	union {
	struct {
	UINT32	m_cbVirtualStartOfFileLo;
	UINT32	m_cbVirtualStartOfFileHi;
	};
	UINT64	m_cbVirtualStartOfFile64;
	};

	union {
	struct {
	UINT32	m_cbVirtualEndOfFileLo;
	UINT32	m_cbVirtualEndOfFileHi;
	};
	UINT64	m_cbVirtualEndOfFile64;
	};

	LPWSTR	m_pwzFullPath;
	QWORD	m_qwFileCreationTime;
	QWORD	m_qwFileLastAccessTime;
	QWORD	m_qwFileLastWriteTime;

public:
	// Public because they uniquely identify this file, without having to compare path strings.
	DWORD	m_dwFileIndexHigh;
	DWORD	m_dwFileIndexLow;
	DWORD	m_dwVolumeSerialNumber;
};
