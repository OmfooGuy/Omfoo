// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer00.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "ReadOmf.h"
#include "DetectContent.h"
#include "NonDelegatingUnknown.h"
#include "MiscStatic.h"

class CContainerLayer00
	: public CReadOmf
	, public CDetectContent
	, public IOmfooReader
	, public INonDelegatingUnknown
{
protected:

enum {
	// The maximum length for any omfi:String property including the null-terminator.
	// Avid's 2.1.2 omfToolkit.dll limited generic strings to no more than 255 characters plus a null-terminator.
	OMFOO_STRMAX_STRING	= 256,
};

//	These are the enumerated values for m_dwFirstOS.
//	They represent the host operating system that ran the program that created the currently loaded *.omf file.
enum HOST_OS_TYPE {
	HOST_OS_UNKNOWN		= 0,	// not tested
	HOST_OS_FAIL		= 1,	// unexpected/inconsistent/conflicting information.
	HOST_OS_WINDOWS		= 2,	// any version of Microsoft Windows.
	HOST_OS_MACINTOSH	= 3,	// any flavor of Mac/Macintosh.
	HOST_OS_LEGACY_MAC	= 4,	// big-endian Motorola 68k or PowerPC Macintosh running the original 'Classic OS'.
	HOST_OS_MODERN_MAC	= 5,	// modern Intel or M1 based Macintosh running some flavor of Darwin-based 'macOS'.
};

			CContainerLayer00(void);
	virtual	~CContainerLayer00(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

protected:
	static DWORD	GetCurrentTimeInSecondsSince1970(void);
	static QWORD	CvtSecondsSince1970ToFileTime(DWORD dwSecondsSince1970);
	BOOL			IsValidTimeValue(DWORD dwTimeValue);
	HRESULT			SurveyTimestamps(void);

private:
	HRESULT	IngestMobIDs(void);
	HRESULT	DetectDuplicateMobIDs(void);
	HRESULT	IngestStrings(void);
	UINT32	DetermineStringCacheMemoryRequirements(void);
	HRESULT	PopulateStringCache(void);
	HRESULT	DetectCodePagePartOne(void);
	HRESULT	ScanIdntsForPlatform(void);
	HRESULT	ScanNetlsForPlatform(void);
	HRESULT	DetectPlatform(void);
	HRESULT	DetectCodePagePartTwo(void);

protected:
	HRESULT	m_hrFirewall;		// This is initialized to E_HANDLE in our constructor.
								// It stays there until IOmfooReader::Load() has completed.
								// After that it will always be S_OK or E_FAIL.
								// We use it to block unexpected calls to IOmfooReader.
								// For example if IOmfooReader::GetHeadObject() is called before
								// IOmfooReader::Load() it will return E_HANDLE.

	// The instance in time when our constructor was called, measured in seconds since 1/1/1970.
	DWORD	m_dwSessionTimeStamp;

	// Our test routine SurveyTimestamps() saves its results here.
	BOOL	m_fHasTimestampBug;
	DWORD	m_dwTimeValueMin;
	DWORD	m_dwTimeValueMax;

private:
	DWORD	m_dwLegacyMacCodePage;		// The current legacy system Macintosh code page for this Win32 process.
										// For example "10000 MAC Roman; Western European"
										// See CP_MACCP in the MS docs for MultiByteToWideChar().
										// This is what CP_MACCP resolves to.

	DWORD	m_dwWindowsAnsiCodePage;	// The current legacy system Windows ANSI code page for this Win32 process.
										// For example "1252 Windows ANSI Latin 1; Western European"
										// See CP_ACP in the MS docs for MultiByteToWideChar().
										// This is what CP_ACP resolves to (unless it's set to UTF-8 in Win 10 or later).

	ULONG	m_aCharCounters[256];	// 256 counters that count the number of instances of each value.
	ULONG	m_cchControls;			// total number of bytes that fall within the range 0x01~0x1F, and also 0x7F.
	ULONG	m_cchPrintableAscii;	// total number of bytes that fall within the range 0x20~0x7E.
	ULONG	m_cchNonAscii;			// total number of bytes that fall within the range 0x80~0xFF.

	ULONG	m_nTotalIdentities;	// Total number of OMFI:IDNT:Platform properties.
	ULONG	m_nMacIdentities;	// Number of OMFI:IDNT:Platform properties that identify as "Apple" or "Mac" etc.
	ULONG	m_nWinIdentities;	// Number of OMFI:IDNT:Platform properties that identify as "Microsoft" or "Win" etc.

	ULONG	m_nNetlsTotal;		// Total number of all OMFI:NETL:URLString properties.
	ULONG	m_nMacNetls;		// Number of OMFI:NETL:URLString properties that begin with "file:///Users/".
	ULONG	m_nWinNetls;		// Number of OMFI:NETL:URLString properties that include a drive designator.

	DWORD	m_dwHostOS;			// One of our enumerated HOST_OS_TYPE values (see above).

protected:
	PCHAR	m_pStringCache;		// All omfi:String payloads concatenated end-to-end, separated with ASCII NULLs.
	ULONG	m_cbStringCache;	// Total size (in bytes) of all of the payloads having the omfi:String data type.

	DWORD	m_dwDetectedCodePage;		// The code page that we detected in DetectCodePage().
public:
	DWORD	m_dwWorkingCodePage;		// The code page we use for all internal calls to MultiByteToWideChar().
};
