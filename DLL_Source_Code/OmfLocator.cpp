// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfLocator.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfLocator.h"
#include <shlwapi.h>
#include <shlobj.h>

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Inline helpers.
//*********************************************************************************************************************
__forceinline BOOL IsDriveLetterW(__in const WCHAR ch)
{
	return (((ch >= 'A')&&(ch <= 'Z')) || ((ch >= 'a')&&(ch <= 'z')));
}

__forceinline BOOL IsPrefixDriveLetterWithTwoForwardSlashesW(__in LPCWSTR pPrefix)
{
	if ((IsDriveLetterW(pPrefix[0]))
		&& (pPrefix[1] == '/')
		&& (pPrefix[2] == '/')
		&& IsCharAlphaNumericW(pPrefix[3]))
	{
		return TRUE;
	}

	return FALSE;
}

__forceinline BOOL IsPrefixTwoForwardSlashesW(__in LPCWSTR pPrefix)
{
	if ((pPrefix[0] == '/')
		&& (pPrefix[1] == '/')
		&& IsCharAlphaNumericW(pPrefix[2]))
	{
		return TRUE;
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Locator Class (LOCR).
//	LOCR is abstract and has no properties.
//*********************************************************************************************************************
COmfObject* COmfLocator::NewInstance(BENTO_BLOP& rBlop,
										CContainerLayer97* pContainer,
										COmfObject* pParent,
										PVOID pNewReserved)
{
	return new COmfLocator(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Private constructor called from NewInstance().
//*********************************************************************************************************************
COmfLocator::COmfLocator(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfLocator::~COmfLocator(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfLocator::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfLocator))
		{
			// If this is the first time our caller queries for IOmfLocator then we need to initialize ourself.
			if (!m_fInitialized)
			{
				m_fInitialized	= TRUE;

				// Assume that this locator does not support any of these strings.
				m_hrRawLocation	= OMF_E_LOCR_NO_STRING;
				m_hrFileName	= OMF_E_LOCR_NO_STRING;
				m_hrFullPath	= OMF_E_LOCR_NO_STRING;
				m_hrPathRoot	= OMF_E_LOCR_NO_STRING;
				m_hrVolumeName	= OMF_E_LOCR_NO_STRING;
				m_hrServerName	= OMF_E_LOCR_NO_STRING;

				// Call helper routine to populate those strings, and to conditionally update their
				// corresponding HRESULTS (but only if an error occurs).
				// Note that not all LOCRS populate all the strings.
				if (SUCCEEDED(hr = Initialize()))
				{
					// If a string exists then its first WCHAR will always be non-zero. 
					// So conditionally update the corresponding error codes from OMF_E_LOCR_NO_STRING to S_OK.
					if (m_wzRawLocation[0])	{m_hrRawLocation	= S_OK;}
					if (m_wzFileName[0])	{m_hrFileName	= S_OK;}
					if (m_wzFullPath[0])	{m_hrFullPath	= S_OK;}
					if (m_wzPathRoot[0])	{m_hrPathRoot	= S_OK;}
					if (m_wzVolumeName[0])	{m_hrVolumeName	= S_OK;}
					if (m_wzServerName[0])	{m_hrServerName	= S_OK;}
				}
				else
				{
					// If Initialize() failed then use the error code as the error code for each string.
					m_hrRawLocation	= hr;
					m_hrFileName	= hr;
					m_hrFullPath	= hr;
					m_hrPathRoot	= hr;
					m_hrVolumeName	= hr;
					m_hrServerName	= hr;
				}
			}

			// We always expose IOmfLocator even when Initialize() fails.
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfLocator*>(this));

			// Always succeed.
			return S_OK;
		}

		// 
		hr = __super::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	These are the six public methods of IOmfLocator. All strings are WCHARs.
//	By the time these are called their strings are populated, and their corresponding error codes are meaningful.
//*********************************************************************************************************************
HRESULT COmfLocator::GetRawLocation(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrRawLocation, m_wzRawLocation, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfLocator::GetFileName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrFileName, m_wzFileName, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfLocator::GetFullPath(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrFullPath, m_wzFullPath, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfLocator::GetPathRoot(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrPathRoot, m_wzPathRoot, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfLocator::GetVolumeName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrVolumeName, m_wzVolumeName, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfLocator::GetServerName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	return CallTwiceStringHandlerW(m_hrServerName, m_wzServerName, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime from NonDelegatingQueryInterface().
//	We come here 'on demand' the very first time the caller queries for our IOmfLocator interface.
//*********************************************************************************************************************
HRESULT COmfLocator::Initialize(void)
{
	HRESULT	hr = OMF_E_LOCR_UNKNOWN_SUBCLASS;
	ULONG	cRetries		= 3;
	DWORD	dwClassFourCC	= this->dwFourCC;

	// Big switch statement calls appropriate initialization routine for each known locator class.
	// If the class fourCC is unknown then find the object's parent class and try again.
L_Retry:
	switch (dwClassFourCC)
	{
	// Core Windows Directory Locator Class (CWDL)
	case FCC('CWDL'):
		hr = DecomposeCWDL();
		break;

	// Core Windows File Locator Class (CWFL)
	case FCC('CWFL'):
		hr = DecomposeCWFL();
		break;

	// Domain Locator Class (DOML)
	case FCC('DOML'):
		hr = DecomposeDOML();
		break;

	// DOS Locator Class (DOSL)
	case FCC('DOSL'):
		hr = DecomposeDOSL();
		break;

	// Locator Class (LOCR)
	case FCC('LOCR'):
		hr = DecomposeLOCR();
		break;

	// Mac Directory Locator Class (MACD)
	case FCC('MACD'):
		hr = DecomposeMACD();
		break;

	// Mac File Locator Class (FILE)
	case FCC('FILE'):
		hr = DecomposeFILE();
		break;

	// Legacy Macintosh Locator Class (MACL)
	case FCC('MACL'):
		hr = DecomposeMACL();
		break;

	// Avid Media File Manager Locator Class (MFML)
	case FCC('MFML'):
		hr = DecomposeMFML();
		break;

	// Avid Media Stream Manager Locator Class (MSML)
	case FCC('MSML'):
		hr = DecomposeMSML();
		break;

	// Network Locator Class (NETL)
	case FCC('NETL'):
		hr = DecomposeNETL();
		break;

	// Sound Designer II Stream Locator Class (SDSL)
	case FCC('SDSL'):
		hr = DecomposeSDSL();
		break;

	// Text Locator Class (TXTL)
	case FCC('TXTL'):
		hr = DecomposeTXTL();
		break;

	// UNIX Locator Class (UNXL)
	case FCC('UNXL'):
		hr = DecomposeUNXL();
		break;

	// URL Locator Class (URLL)
	case FCC('URLL'):
		hr = DecomposeURLL();
		break;

	// Windows Directory Locator Class (WIND)
	case FCC('WIND'):
		hr = DecomposeWIND();
		break;

	// Windows File Locator Class (WINF)
	case FCC('WINF'):
		hr = DecomposeWINF();
		break;

	// Standard Windows Locator Class (WINL)
	case FCC('WINL'):
		hr = DecomposeWINL();
		break;
	}

	// If a handler can't populate the strings - or of there is no handler at all,
	// then get the parent class fourCC and try again.
	if ((hr == OMF_E_LOCR_UNKNOWN_SUBCLASS) && (--cRetries > 0))
	{
		dwClassFourCC = m_pContainer->InternalGetParentClassFourCC(dwClassFourCC);
		if ((dwClassFourCC != 0) && (dwClassFourCC != FCC('OOBJ')))
		{
			// 
			goto L_Retry;
		}

		// Oops! No parent class!
		// All classes must inherit 'OOBJ'.
		// All locator classes should inherit 'LOCR'.
		// So how did we get here?
		BREAK_IF_DEBUG
	}
	return hr;
}

//*********************************************************************************************************************
//	Core Windows Directory Locator Class (CWDL)
//	This class exists in Avid Media Composer bin files (AVB) but I知 not sure they've ever been used in OMF files.
//	I discovered it by examining the Class Registry section of Avid system dump files.
//	None of my OMF test files have them and I don't know any of the property names or data types.
//	This fourCC might be a wild goose chase. So for now this is just a placeholder.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeCWDL()
{
	BREAK_IF_DEBUG

	// Congratulations!
	// Please contact us so we can support this locator class in the next release of Omfoo.
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Core Windows File Locator Class (CWFL)
//	This class exists in Avid Media Composer bin files (AVB) but I知 not sure they've ever been used in OMF files.
//	I discovered it by examining the Class Registry section of Avid system dump files.
//	None of my OMF test files have them and I don't know any of the property names or data types.
//	This fourCC might be a wild goose chase. So for now this is just a placeholder.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeCWFL()
{
	BREAK_IF_DEBUG

	// Congratulations!
	// Please contact us so we can support this locator class in the next release of Omfoo.
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Domain Locator Class (DOML)
//	Inherits TXTL.
//	OMF1 only ?????
//	(undocumented)
//	No known properties, but the Avid Media Stream Manager Locator Class (MSML) inherits it.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeDOML()
{
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	DOS Locator Class (DOSL)
//	Inherits LOCR.
//
//	Property				Data Type			Examples
//	OMFI:DOSL:PathName		omfi:String			"L:\WAVs Count-offs\Count_01.wav"
//												"C:\Temp.Download\OMF test from Nuendo\streethop95m.wav"
//												"1TEST2_.a1.aiff"
//												"E:\Xmass\TESTSE~1\14-2\LEVPAN~1\FOOTS_~1.WAV"
//												"C:\Temp.Download\OMF test from Nuendo\streethop95m.wav"
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeDOSL()
{
	// OMFI:DOSL:PathName
	HRESULT	hr = OrdReadStringW(ePropDoslPathName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL);
	if (SUCCEEDED(hr))
	{
		// Let the default handler isolate the components.
		hr = Default();
	}

	return hr;
}

//*********************************************************************************************************************
//	Locator Class (LOCR)
//	Inherits OOBJ.
//	LOCR is abstract and has no properties.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeLOCR()
{
	BREAK_IF_DEBUG

	// Congratulations!
	// You are probably here because this is some type of unknown/undocumented class that inherits LOCR.
	// Please contact us so we can support this locator class in the next release of Omfoo.

	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Mac Directory Locator Class (MACD)
//	(undocumented)
//	OMFI:MACD:UTF8CanonicalPathName
//	OMFI:MACDL:FileName
//	OMFI:MACDL:DirID
//	OMFI:MACDL:VRef
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeMACD()
{
	BREAK_IF_DEBUG

	// Congratulations!
	// You are here because this OMF file contains MACDs.
	// None of my OMF test files have these.
	// Please contact us so we can support this locator class in the next release of Omfoo.
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Mac File Locator Class (FILE)
//	This class exists in Avid Media Composer bin files (AVB) but I知 not sure they've ever been used in OMF files.
//	I discovered it by examining the Class Registry section of Avid system dump files.
//	None of my OMF test files have them and I don't know any of the property names or data types.
//	This fourCC might be a wild goose chase. So for now this is just a placeholder.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeFILE()
{
	BREAK_IF_DEBUG

	// Congratulations!
	// Please contact us so we can support this locator class in the next release of Omfoo.
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Mac Locator Class (MACL)
//	Inherits LOCR.
//
//	Example of Classic Mac fullpath = "Mac HD:Documents:myfile"
//
//	Property				Data Type					Examples
//	OMFI:MACL:DirID			omfi:Long, omfi:Int32	
//	OMFI:MACL:FileName		omfi:String					"sonyV01.B79869F3_B7B79869F3.omf"
//														"OMF1 w ext SDII"
//														"OMF2 w ext SDII"
//	OMFI:MACL:PathName		omfi:String					UNTESTED! No test files have this property!
//	OMFI:MACL:VName			omfi:String					"14904a"

//	OMFI:MACL:SDSMobID		omfi:UID					?????
//	OMFI:MACL:SDSAddress	?????						?????
//	OMFI:MACL:VRef			omfi:Short					?????		Apparently OMF1 only
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeMACL()
{
	// OMFI:MACL:FileName
	HRESULT	hr = OrdReadStringW(ePropMaclFileName, ELEMS(m_wzFileName), m_wzFileName, NULL);
	if (SUCCEEDED(hr))
	{
		// OMFI:MACL:VName
		if (FAILED(OrdReadStringW(ePropMaclVName, ELEMS(m_wzVolumeName), m_wzVolumeName, NULL)))
		{
			// OMFI:MACL:VRef
			UINT16 wMacVRef = 0;
			if (SUCCEEDED(OrdReadUInt16(ePropMaclVRef, &wMacVRef)))
			{
				// Apple always names the boot drive "Macintosh HD". So we fake it and hard-code it right here!
				if (wMacVRef == 0)
				{
					lstrcpyW(m_wzVolumeName, L"Macintosh HD");
				}
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Avid Media File Manager Locator Class (MFML)
//	Inherits TXTL.
//	This LOCR identifies the location of the "C:\Avid MediaFiles" folder.
//
//	Property					Data Type			Examples
//	OMFI:MFML:MobID				omfi:UID			UNTESTED! No test files have this property!
//	OMFI:MFML:TrackType			omfi:TrackType		TT_NULL, TT_PICTURE, TT_SOUND, etc.
//	OMFI:MFML:LastKnownVolume	omfi:String			"PW7389 (E:)"
//	OMFI:MFML:LinkUIDLow		????				????
//	OMFI:MFML:LinkUIDHigh		????				????
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeMFML()
{
	// This is not a file locator.
	m_hrFileName = OMF_E_LOCR_NOT_A_FILE;

	// OMFI:MFML:LastKnownVolume.
	HRESULT	hr = OrdReadStringW(ePropMfmlLastKnownVolume, ELEMS(m_wzVolumeName), m_wzVolumeName, NULL);
	if (SUCCEEDED(hr))
	{
		ULONG cchStrLen = lstrlenW(m_wzVolumeName);
		if (cchStrLen >= 6 )
		{
			if ((m_wzVolumeName[cchStrLen-1] == ')') &&
				(m_wzVolumeName[cchStrLen-2] == ':') &&
				(m_wzVolumeName[cchStrLen-4] == '(') &&
				(m_wzVolumeName[cchStrLen-5] == ' '))
			{
				WCHAR chDriveLetter = m_wzVolumeName[cchStrLen-3];
				if ((chDriveLetter >= 'A') && (chDriveLetter <= 'Z'))
				{
					m_wzPathRoot[0] = chDriveLetter;
					m_wzPathRoot[1] = ':';
					m_wzPathRoot[2] = '\\';
					m_wzPathRoot[3] = 0;
				}
			}
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	Avid Media Stream Manager Locator Class (MSML)
//	Inherits DOML.
//	This LOCR identifies the location of the "C:\OMFI MediaFiles" folder.
//
//	Property					Data Type			Examples
//	OMFI:MSML:AMEVersion		omfi:VarLenBytes	1, 2, 3
//	OMFI:MSML:MobID				omfi:UID
//	OMFI:MSML:DomainType		omfi:Long			0x00000001
//	OMFI:MinorVersion			omfi:Char			0x01
//	OMFI:MSML:LastKnownVolume	omfi:String			"TRIAL BY JURY (G:)"
//													"PW7389 (E:)"
//													"Offline (F:)"
//													"Online (G:)"
//													"Media (E:)"
//													""
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeMSML()
{
	// This is not a file locator.
	m_hrFileName = OMF_E_LOCR_NOT_A_FILE;

	// OMFI:MSML:LastKnownVolume.
	HRESULT	hr = OrdReadStringW(ePropMsmlLastKnownVolume, ELEMS(m_wzVolumeName), m_wzVolumeName, NULL);
	if (SUCCEEDED(hr))
	{
		ULONG cchStrLen = lstrlenW(m_wzVolumeName);
		if (cchStrLen >= 6 )
		{
			if ((m_wzVolumeName[cchStrLen-1] == ')') &&
				(m_wzVolumeName[cchStrLen-2] == ':') &&
				(m_wzVolumeName[cchStrLen-4] == '(') &&
				(m_wzVolumeName[cchStrLen-5] == ' '))
			{
				WCHAR chDriveLetter = m_wzVolumeName[cchStrLen-3];
				if ((chDriveLetter >= 'A') && (chDriveLetter <= 'Z'))
				{
					m_wzPathRoot[0] = chDriveLetter;
					m_wzPathRoot[1] = ':';
					m_wzPathRoot[2] = '\\';
					m_wzPathRoot[3] = 0;
				}
			}
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	Network Locator Class (NETL)
//	Inherits LOCR.
//
//	Property					Data Type			Examples
//	OMFI:NETL:URLString			omfi:String			"file:///L%3a/WAVs%20Count-offs/Count_01.wav"
//													"file:///14904a/OMFI%20MediaFiles/sonyA02.B79869F3_B7B79869F3.omf"
//													"file:///F%3A/OMFI%20MediaFiles/20A02.3A6646B93A664BDF.wav"
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeNETL()
{
	CHAR	szRawLocation[ELEMS(m_wzRawLocation)] = {0};
	WCHAR	wzWorkspace[ELEMS(m_wzRawLocation)] = {0};

	// URLs (including file URIs) can only contain certain 7-bit printable ASCII characters.
	// Characters which are not allowed in URIs, but which are allowed in filenames, must be percent-encoded.
	// Unicode characters outside of the ASCII range must first be UTF-8 encoded, and then percent-encoded
	// down to 7-bit ASCII.
	// So the first thing to do is read the string exactly as it is stored in the file.
	HRESULT hr = OrdReadStringA(ePropNetlURLString, ELEMS(szRawLocation), szRawLocation, NULL);
	if (FAILED(hr))
	{
		// Note that some OMF1 implementations create empty NETLs with no properties.
		// So that might be what's going on here.
		goto L_Exit;
	}

	// Copy szRawLocation to m_wzRawLocation using CP_ASCII so that we can then in turn, call PathCreateFromUrlW().
	// Do NOT use m_dwWorkingCodePage.
	MultiByteToWideChar(CP_ASCII, 0, szRawLocation, -1, m_wzRawLocation, ELEMS(m_wzRawLocation));

	ULONG cch = ELEMS(m_wzRawLocation);
	if (FAILED(hr = PathCreateFromUrlW(m_wzRawLocation, wzWorkspace, &cch, 0)))
	{
		goto L_Exit;
	}

	// Remove any trailing spaces.
	// This can happen if m_wzRawLocation ended with one or more percent-encoded spaces. %20
	PathRemoveBlanksW(wzWorkspace);

	// Now ingest what's next.
	LPWSTR pProbe = wzWorkspace;

	// If a backslash precedes what's next (like a drive letter) then skip over it.
	if (pProbe[0] == '\\')
	{
		pProbe++;
	}

	// Test for the unexpected case when the URL contains only a file name.
	if (PathIsFileSpecW(pProbe))
	{
		// copy pProbe to m_wzFileName.
		lstrcpyW(m_wzFileName, pProbe);

		// Remove any spaces around the filename.
		PathRemoveBlanksW(m_wzFileName);

		// We're done.
		goto L_Exit;
	}

	// Find the file name.
	// We do this AFTER calling PathCreateFromUrl() because it converts percent-encoded characters.
	LPWSTR pFileName = PathFindFileNameW(pProbe);

	// If PathFindFileName() fails it will return a pointer to pProbe.
	// So we test for success like this:
	if (pFileName > pProbe)
	{
		// Save pFileName in m_wzFileName.
		lstrcpyW(m_wzFileName, pFileName);

		// Remove any spaces around the filename.
		PathRemoveBlanksW(m_wzFileName);
	}

	// Test for a colon and backslash in the second and third positions,
	// and make sure an alpha-numeric character follows it.
	if ((pProbe[1] == ':') &&
		(pProbe[2] == '\\') &&
		IsCharAlphaNumericW(pProbe[3]))
	{
		lstrcpyW(m_wzFullPath, pProbe);

		// Finally test for the actual drive letter.
		INT nDriveNum = PathGetDriveNumberW(pProbe);
		if ((nDriveNum >= 0) && (nDriveNum < 26))
		{
			// Convert it all into a standard human-readable MS-DOS drive designator.
			m_wzPathRoot[0] = WCHAR(nDriveNum + 'A');
			m_wzPathRoot[1] = ':';
			m_wzPathRoot[2] = '\\';
			m_wzPathRoot[3] = 0;
		}
		goto L_Exit;
	}

	// If m_wzFullPath does not begin with a drive letter then ...
	lstrcpyW(m_wzFullPath, pProbe);
	LPWSTR pwzNextComponent = PathFindNextComponentW(pProbe);
	if (pwzNextComponent)
	{
		*pwzNextComponent = 0;
		PathRemoveBackslash(pProbe);
		lstrcpynW(m_wzPathRoot, pProbe, ELEMS(m_wzPathRoot));
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Sound Designer II Stream Locator Class (SDSL)
//	Inherits TXTL.
//	This implementation is just a placeholder and always fails because I don't know the property name(s).
//	All I know is OMFI:SDSL:AMEVersion
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeSDSL()
{
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Text Locator Class (TXTL)
//	Inherits LOCR.
//
//	Property					Data Type			Examples
//	OMFI:TXTL:Name				omfi:String			UNTESTED! OPTIONAL! No test files have this property!
//	OMFI:TXTL:Version			omfi:VersionType	UNTESTED! OPTIONAL! No test files have this property!
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeTXTL()
{
	// "A Text Locator (TXTL) object provides information to the user to help locate the file. It is not handled
	// by applications automatically because the meaning or format of the Name property value is not defined."
	HRESULT hr = OrdReadStringW(ePropTxtlName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL);
	if (SUCCEEDED(hr))
	{
		// Isolate the file name.
		LPWSTR pFileName = PathFindFileNameW(m_wzRawLocation);

		// If PathFindFileName() fails it will return a pointer to the beginning of m_wzRawLocation.
		// So we test for success like this:
		if ((PathIsFileSpecW(m_wzRawLocation) || pFileName > m_wzRawLocation))
		{
			// Save filename in m_wzFileName.
			lstrcpynW(m_wzFileName, pFileName, ELEMS(m_wzFileName));
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	UNIX Locator Class (UNXL)
//	Inherits LOCR.
//
//	Property					Data Type			Examples
//	OMFI:UNXL:PathName			omfi:String			"1TEST2_.a1.aiff"
//													"1TEST2_.a4.aiff"
//													"ffffffbb_0002b888_05843261.wav"
//													"fake.omf"
//	OMFI:UNXDL:PathName			?????				Directory locator
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeUNXL()
{
	HRESULT hr = OrdReadStringW(ePropUnxlPathName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL);
	if (SUCCEEDED(hr))
	{
		// Isolate the file name.
		LPWSTR pFileName = PathFindFileNameW(m_wzRawLocation);

		// If PathFindFileName() fails it will return a pointer to the beginning of m_wzRawLocation.
		// So we need to test for success like this:
		if ((PathIsFileSpecW(m_wzRawLocation) || pFileName > m_wzRawLocation))
		{
			// Save it in m_wzFileName.
			lstrcpynW(m_wzFileName, pFileName, ELEMS(m_wzFileName));
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	URL Locator Class (URLL)
//	(undocumented)
//	This locator class exists in Avid Media Composer bin files (AVB) but I知 not sure it's used in OMF files.
//	It is listed in the class registry section of Avid system dump files.
//	None of my OMF test files have it and unfortunatly I don't know the property names or data types.
//	So for now this is just a placeholder.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeURLL()
{
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Windows Directory Locator Class (WIND)
//	(undocumented)
//	This locator class exists in Avid Media Composer bin files (AVB) but I知 not sure it's used in OMF files.
//	It is listed in the class registry section of Avid system dump files.
//	None of my OMF test files have it and unfortunatly I don't know the property names or data types.
//	So for now this is just a placeholder.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeWIND()
{
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Windows File Locator Class (WINF)
//	(undocumented)
//	This locator class exists in Avid Media Composer bin files (AVB) but I知 not sure it's used in OMF files.
//	It is listed in the class registry section of Avid system dump files.
//	None of my OMF test files have it and unfortunatly I don't know the property names or data types.
//	So for now this is just a placeholder.
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeWINF()
{
	return OMF_E_LOCR_UNKNOWN_SUBCLASS;
}

//*********************************************************************************************************************
//	Windows Locator Class (WINL)
//	Inherits LOCR.
//
//	Property					Data Type			Examples
//	OMFI:WINL:FullPathName		omfi:String
//	OMFI:WINL:PathName			omfi:String			"L:\WAVs Count-offs\Count_08.wav"
//	OMFI:WINL:Shortcut			omfi:DataValue		The raw binary contents of a Windows shortcut (*.lnk) file.
//	OMFI:FL:PathName			omfi:String			"G//OMFI MediaFiles/091CAFE4V01.421CB94B.FBA610.omf"
//	OMFI:DL:PathName			omfi:String			???
//	OMFI:Attributes				omfi:VarLenBytes	immediate value = 0x02
//*********************************************************************************************************************
HRESULT COmfLocator::DecomposeWINL()
{
	HRESULT		hr = (-1);

	// OMFI:WINL:Shortcut
	HGLOBAL	hGlobal	= NULL;
	if (SUCCEEDED(OrdCreateHGlobalOnDataValue(ePropWinlShortcut, GMEM_FIXED, &hGlobal)))
	{
		PBYTE pShortcut = PBYTE(GlobalLock(hGlobal));
		if (pShortcut)
		{
			UINT32 cbShortcut = UINT32(GlobalSize(hGlobal));
			hr = ShortcutHelper(pShortcut, cbShortcut);
			GlobalUnlock(hGlobal);
		}
		GlobalFree(hGlobal);
		hGlobal = NULL;

		// Explicity test for S_OK because ShortcutHelper() will return S_FALSE when it only
		// finds m_wzVolumeName[] or m_wzServerName[].
		if (hr == S_OK)
		{
			goto L_Exit;
		}
	}

	// OMFI:WINL:PathName
	if (SUCCEEDED(hr = OrdReadStringW(ePropWinlPathName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL)))
	{
		hr = Default();
		goto L_Exit;
	}

	// OMFI:WINL:FullPathName
	if (SUCCEEDED(hr = OrdReadStringW(ePropWinlFullPathName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL)))
	{
		hr = Default();
		goto L_Exit;
	}

	// OMFI:FL:PathName
	// This string always uses forward slashes (and not backslashes).
	// It also uses a forward slash where the colon belongs.
	if (SUCCEEDED(hr = OrdReadStringW(ePropFlPathName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL)))
	{
		hr = Default();
		goto L_Exit;
	}

	// OMFI:DL:PathName
	// This property appears in a string dump of AvidXpressProHD 7 but none of my test files have it.
	// It's obviously similar to OMFI:FL:PathName.
	// The FL in OMFI:FL:PathName probably means "file location" and so the DL in OMFI:DL:PathName
	// probably means 'directory location' or 'domain locator'.
	if (SUCCEEDED(hr = OrdReadStringW(ePropDlPathName, ELEMS(m_wzRawLocation), m_wzRawLocation, NULL)))
	{
		BREAK_IF_DEBUG
		m_hrFileName = OMF_E_LOCR_NOT_A_FILE;
	}
	else
	{
		hr = S_OK;
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Common helper for DecomposeDOSL() and DecomposeWINL().
//	We come here for OMFI:DOSL:PathName, OMFI:WINL:PathName, OMFI:WINL:FullPathName, and OMFI:FL:PathName.
//*********************************************************************************************************************
HRESULT COmfLocator::Default()
{
	// Copy m_wzRawLocation to m_wzFullPath where we can manipulate it.
	lstrcpynW(m_wzFullPath, m_wzRawLocation, ELEMS(m_wzFullPath));

	// Remove all leading and trailing spaces.
	PathRemoveBlanksW(m_wzFullPath);

	// Fix up the forward-slash syntax used by OMFI:FL:PathName.
	// For example ...
	// "E//OMFI MediaFiles/107B1A02.3F85F36F_33F85F367.aif"
	if (IsPrefixDriveLetterWithTwoForwardSlashesW(m_wzFullPath))
	{
		// If the path begins with a drive letter followed by two forward slashes
		// then replace all forward slashes with backslashes.
		LPWSTR	pProbe = &m_wzFullPath[1];
		while (pProbe)
		{
			*pProbe = '\\';
			pProbe = StrChrW(pProbe, '/');
		}

		// Then replace the very first backslash at offset #1 with a colon.
		m_wzFullPath[1] = ':';
	}
	// Else if the path begins with two forward slashes then replace all forward slashes with backslashes.
	else if (IsPrefixTwoForwardSlashesW(m_wzFullPath))
	{
		LPWSTR	pProbe = m_wzFullPath;
		while (pProbe)
		{
			*pProbe = '\\';
			pProbe = StrChrW(pProbe, '/');
		}
	}

	// Isolate the filename.
	LPWSTR pFileName = PathFindFileNameW(m_wzFullPath);
	if (PathIsFileSpec(m_wzFullPath) || (pFileName > m_wzFullPath))
	{
		// Fix a bug where there is a backslash following the filename extension.
		// I've seen this in old Avid Media Composer files (OMF2) when the property is OMFI:WINL:PathName.
		// Here are two actual examples:
		//	"\\UNITYFMPROMO\The Simpsons\OMFI MediaFiles\A7\VO1B8FC9A03.3FD7C661.AC3680.aif\"
		//	"D:\hot chick\sfxstring.aif\"

		// So here's a fix. This will affect m_wzFullPath[] too, which is desirable.
		PathRemoveBackslashW(pFileName);
		lstrcpyW(m_wzFileName, pFileName);
	}

	// Get the drive number if there is one.
	INT nDriveNumber = PathGetDriveNumberW(m_wzFullPath);
	if (nDriveNumber >= 0)
	{
		// Convert the number into a human-readable drive letter. For example "C:\"
		m_wzPathRoot[0] = WCHAR(nDriveNumber + 'A');
		m_wzPathRoot[1] = ':';
		m_wzPathRoot[2] = '\\';
		m_wzPathRoot[3] = 0;
	}
	else
	{
		// Isolate the root information. 
		WCHAR wzTemporary[ELEMS(m_wzFullPath)] = {0};
		lstrcpyW(wzTemporary, m_wzFullPath);
		if (PathStripToRootW(wzTemporary))
		{
			lstrcpynW(m_wzPathRoot, wzTemporary, ELEMS(m_wzPathRoot));
		}
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Helper for DecomposeWINL().
//	OMFI:WINL:Shortcut
//
//	Parse the contents an embedded Windows *.lnk file (aka a 'shortcut') and isolate and save various string results.
//	This routine's primary goal is to populate m_wzRawLocation and then pass it off to COmfLocator::Default().
//	It will only succeed if COmfLocator::Default() gets called and succeeds.
//
//	The benefit of parsing the shortcut is that it preserves the full path name in its native UTF-16 format.
//	It also holds other useful strings like the volume name and the NetBIOS name (and this routine will find them).
//
//	Note that for some reason, a program called "Pro-Convert" version "5.0.787" from a company called "Cui Bono - Soft"
//	saved a copy of its OMFI:DOSL:PathName string here (without a null-terminator).
//*********************************************************************************************************************
HRESULT COmfLocator::ShortcutHelper(PBYTE pShortcut, UINT32 cbShortcut)
{
	HRESULT	hr				= E_FAIL;					// assume we will experience an "unspecified error"
	const PBYTE	pMemEnd		= &pShortcut[cbShortcut];	// a pointer to the first byte past the end of memory.
	PBYTE	pTraverse		= pShortcut;				// an advancing pointer we use to traverse the payload.
	UINT32	cbRemaining		= cbShortcut;				// the number of bytes that have yet to be traversed.
	UINT32	cbOffset		= 0;						// byte offset into the payload of what pLinkInfo points to.

	const PLINK_HEADER	pHeader	= PLINK_HEADER(pShortcut);	// pointer to the LINK_HEADER structure
	LPCSHITEMID			pShellItemID	= NULL;				// defined in <shtypes.h>
	PLINK_INFO			pLinkInfo		= NULL;				// pointer to the LINK_INFO structure
	PLINK_VOLUME_INFO	pVolumeInfo		= NULL;				// pointer to the LINK_VOLUME_INFO structure
	PLINK_NETWORK_INFO	pNetworkInfo	= NULL;				// pointer to the LINK_NETWORK_INFO structure

	UINT32	cbItemIDList		= 0;		// total number of bytes in pItemIDList INCLUDING null terminator.
	ULONG	nItemIDs			= 0;		// total number of SHITEMID structures in pItemIDList.

	// String pointers.
	PCSTR	pszVolumeName		= NULL;		// ANSI, lives in the LINK_INFO structure if it exists
	PCSTR	pszLocalBasePath	= NULL;		// utf16, lives in the LINK_INFO structure if it exists

	PCWSTR	pwzVolumeName		= NULL;		// ANSI, lives in the LINK_INFO structure if it exists
	PCWSTR	pwzLocalBasePath	= NULL;		// utf16, lives in the LINK_INFO structure if it exists

	CHAR	szNetBiosName[20]	= {0};		// the 16 ASCII chars of the NewBIOS name (which are not null-terminated)
											// followed by four null terminators that we append to terminate them.

	// Pointers to LINK_STRING_DATA structures.
	PLINK_STRING_DATA	pRelPath	= NULL;
	PLINK_STRING_DATA	pWorkingDir	= NULL;

	// TRUE if those LINK_STRING_DATA structures (above) hold UTF-16, or FALSE if they hold ANSI.
	BOOL				fUnicode	= FALSE;

	if (IsBadReadPtr(pShortcut, cbShortcut))
	{
		BREAK_IF_DEBUG	// We called this routine ourself from DecomposeWINL(). This should never happen! 
		hr = OMFOO_E_ASSERTION_FAILURE;
		goto L_Exit;
	}

	// Make sure we have enough memory to hold an entire LINK_HEADER structure.
	if (cbShortcut < sizeof(LINK_HEADER))
	{
		BREAK_IF_DEBUG
		hr = E_FAIL;
		goto L_Exit;
	}

	// Verify that the buffer actually contains a LINK_HEADER stucture by testing the cbStructSize member.
	// Page 11 of [MS-SHLLINK].pdf says, "This value MUST be 0x0000004C"
	if (pHeader->cbStructSize < sizeof(LINK_HEADER))
	{
		BREAK_IF_DEBUG
		hr = E_FAIL;
		goto L_Exit;
	}

	// Verify that the buffer actually contains a LINK_HEADER stucture by testing clsid.
	if (pHeader->clsid != CLSID_ShellLink)
	{
		BREAK_IF_DEBUG
		hr = E_FAIL;
		goto L_Exit;
	}

	// Copy these if we want them.
//	m_ftCreation		= pHeader->ftCreation;
//	m_ftLastAccess		= pHeader->ftLastAccess;
//	m_ftModification	= pHeader->ftModification;
//	m_cbFileLengthLo	= pHeader->cbFileLength;

	// If the shell link contains Unicode (UTF-16) strings then SLDF_UNICODE should be set.
	// Otherwise, it contains strings that are encoded using the system default AMSI code page.
	fUnicode = (pHeader->dwFlags & SLDF_UNICODE) ? TRUE : FALSE;

	// Advance pTraverse to the very first byte after the LINK_HEADER structure.
	// It is possible (but in practice, unlikely) that the file ends here.
	// If this were to happen pTraverse would point to pMemEnd.
	pTraverse += pHeader->cbStructSize;

	// Update cbRemaining to reflect the number of bytes that follow the LINK_HEADER structure.
	// It is possible (but in practice, unlikely) that there will be no more bytes to process.
	cbRemaining -= pHeader->cbStructSize;

	// Query this flag to determine if a variable-length ITEMIDLIST structure exists.
	// Note that the ITEMIDLIST is defined in <shtypes.h>
	if (pHeader->dwFlags & SLDF_HAS_ID_LIST)
	{
		// Make sure there's enough memory remaining to fetch the cb size member SHITEMID structure.
		// The smallest amount of memory we need is two bytes.
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(USHORT)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Cast the current value of pTraverse as a LPCSHITEMID.
		pShellItemID = LPCSHITEMID(pTraverse);

		// Total number of bytes in the current LINK_ITEM structure, aka a SHITEMID.
		USHORT cbCurrentItem = 0;

		// Fetch the number of bytes in each SHITEMID structure.
		// Exit the loop when the length is zero.
		while (cbCurrentItem = LPSHITEMID(pTraverse)->cb)
		{
			// A LINK_ITEM_ID structure must be at least two bytes long or else something's seriously messed up!
			if (cbCurrentItem < sizeof(USHORT))
			{
				BREAK_IF_DEBUG
				// Something's seriously messed up!
				hr = E_FAIL;
				goto L_Exit;
			}

			// Increment the LINK_ITEM_ID counter.
			nItemIDs++;

			// Add the size of this structure to the total byte count.
			cbItemIDList += cbCurrentItem;

			// Advance our pointer to the next SHITEMID structure.
			pTraverse += cbCurrentItem;

			// Adjust our byte counter.
			cbRemaining -= cbCurrentItem;

			// Make sure there's enough memory remaining to continue.
			// Again, the smallest amount of memory we need is two bytes (so we can fetch the next structure's size).
			if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(USHORT)))
			{
				BREAK_IF_DEBUG
				hr = E_FAIL;
				goto L_Exit;
			}
		}

		// Don't forget to add in the length field of the final 16-bit null-terminator.
		cbItemIDList += sizeof(USHORT); 
		pTraverse += sizeof(USHORT);
		cbRemaining -= sizeof(USHORT);
	}

	// Query this flag to determine if a variable-length LINK_INFO structure exists.
	if (pHeader->dwFlags & SLDF_HAS_LINK_INFO)
	{
		UINT32	cbLinkInfoDataArea	= 0;		// number of bytes in the variable-length data area.
		PBYTE	pLinkInfoDataStart	= NULL;		// lower bounds of LINK_INFO data area.
		PBYTE	pLinkInfoDataEnd	= NULL;		// upper bounds of LINK_INFO data area.

		PCHAR	pcVolatile			= NULL;		// a temporary pointer that we use to test ANSI strings.
		PCHAR	pcSniffer			= NULL;		// a temporary pointer that we use to find ANSI null-terminators.

		PWCHAR	pwVolatile			= NULL;		// a temporary pointer that we use to test wide strings.
		PWCHAR	pwSniffer			= NULL;		// a temporary pointer that we use to find wide null-terminators.

		// Make sure there's enough memory remaining to traverse a variable-length LINK_INFO structure.
		// The smallest amount we need is OFFSETOF(LINK_INFO, cbVolumeIDOffset).
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < OFFSETOF(LINK_INFO, cbVolumeIDOffset)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Save the current value of pTraverse as a pointer to our LINK_INFO.
		pLinkInfo = PLINK_INFO(pTraverse);

		// cbTotalSize is a 32-bit, unsigned integer that specifies the size, in bytes, of the complete
		// LINK_INFO structure. All offsets specified in this structure MUST be less than this value,
		// and all strings contained in this structure MUST fit within the extent defined by this size.
		if (pLinkInfo->cbTotalSize > INT32(cbRemaining))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// cbHeaderSize is a 32-bit, unsigned integer that specifies the size, in bytes, of the header section.
		// If cbHeaderSize is less than 0x0000001C, or greater than cbTotalSize, then something is messed up.
		if ((pLinkInfo->cbHeaderSize < OFFSETOF(LINK_INFO, cbLocalBasePathOffsetUnicode))
			|| (pLinkInfo->cbHeaderSize >= pLinkInfo->cbTotalSize))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Calculate minimum and maximum bounds for the data area.
		pLinkInfoDataStart	= PBYTE(pLinkInfo) + pLinkInfo->cbHeaderSize;
		pLinkInfoDataEnd	= PBYTE(pLinkInfo) + pLinkInfo->cbTotalSize;
		cbLinkInfoDataArea	= pLinkInfo->cbTotalSize - pLinkInfo->cbHeaderSize;
		if (INT32(cbLinkInfoDataArea) < sizeof(UINT32))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// If bit #0 is set the cbVolumeIDOffset and cbLocalBasePathOffset members are present, and their
		// locations are specified by the values of the cbVolumeIDOffset and cbLocalBasePathOffset fields.
		// If cbHeaderSize is greater than or equal to 0x00000024, then the optional unicode strings are present.
		if (pLinkInfo->dwFlags & 0x00000001)
		{
			cbOffset = pLinkInfo->cbVolumeIDOffset;
			if ((cbOffset < pLinkInfo->cbHeaderSize) || (cbOffset > pLinkInfo->cbTotalSize))
			{
				// Something is messed up!
				BREAK_IF_DEBUG
				hr = E_FAIL;
				goto L_Exit;
			}

			// Create a pointer to the LINK_VOLUME_INFO structure.
			pVolumeInfo = PLINK_VOLUME_INFO(PBYTE(pLinkInfo) + cbOffset);
			if ((PBYTE(pVolumeInfo) < pLinkInfoDataStart)
				|| (PBYTE(pVolumeInfo) >= pLinkInfoDataEnd)
				|| (pVolumeInfo->cbStructSize < OFFSETOF(LINK_VOLUME_INFO, cbLabelOffsetUnicode)))
			{
				BREAK_IF_DEBUG
				hr = E_FAIL;
				goto L_Exit;
			}

			// If the value of cbLabelOffset is 0x00000014, it MUST be ignored, and the value of the 
			// cbLabelOffsetUnicode field MUST be used to locate the volume label string.
			cbOffset = pVolumeInfo->cbLabelOffset;
			if (cbOffset != 0x00000014)
			{
				pcSniffer = pcVolatile = PCHAR(PBYTE(pVolumeInfo) + cbOffset);
				while (PBYTE(pcSniffer) < pLinkInfoDataEnd)
				{
					if (0 == *pcSniffer++)
					{
					// Success! We found the null-terminator to the ANSI volume name string!
					// So now save the pointer to that string where we can find it later.
						pszVolumeName = pcVolatile;
						break;
					}
				}
			}
			else
			{
				if (pVolumeInfo->cbStructSize >= sizeof(LINK_VOLUME_INFO))
				{
					cbOffset = pVolumeInfo->cbLabelOffsetUnicode;
					pwSniffer = pwVolatile = PWCHAR(PBYTE(pVolumeInfo) + cbOffset);
					while (PBYTE(pwSniffer) < (pLinkInfoDataEnd-1))
					{
						if (0 == *pwSniffer++)
						{
						// Success! We found the null-terminator to the UTF-16 volume name string!
						// Now save the pointer to that string where we can find it later.
							pwzVolumeName = pwVolatile;
							break;
						}
					}
				}
			}

			// Now that we have pVolumeInfo and pszVolumeName (or pwzVolumeName) we turn our attention
			// back to the LINK_INFO structure and attempt to find pszLocalBasePath (or pwzLocalBasePath).
			// The member wzLocalBasePathOffsetUnicode[] can be present only if pLinkInfo->cbHeaderSize is
			// greater than or equal to 0x00000024.
			if (pLinkInfo->cbHeaderSize >= 0x00000024)	// OFFSETOF(LINK_INFO, cbCommonPathSuffixOffsetUnicode))
			{
				cbOffset = pLinkInfo->cbLocalBasePathOffsetUnicode;
				pwSniffer = pwVolatile = PWCHAR(PBYTE(pLinkInfo) + cbOffset);
				if ((PBYTE(pcVolatile) >= pLinkInfoDataStart) && (PBYTE(pcVolatile) < pLinkInfoDataEnd))
				{
					while (PBYTE(pwSniffer) < (pLinkInfoDataEnd-1))
					{
						if (0 == *pwSniffer++)
						{
						// Success! We found the null-terminator to the UTF-16 full pathname string.
						// Now save the pointer to that string where we can find it later.
							pwzLocalBasePath = pwVolatile;
							break;
						}
					}
				}
			}
			else
			{
				cbOffset = pLinkInfo->cbLocalBasePathOffset;
				pcSniffer = pcVolatile = PCHAR(PBYTE(pLinkInfo) + cbOffset);
				if ((PBYTE(pcVolatile) >= pLinkInfoDataStart) && (PBYTE(pcVolatile) < pLinkInfoDataEnd))
				{
					while (PBYTE(pcSniffer) < pLinkInfoDataEnd)
					{
						if (0 == *pcSniffer++)
						{
						// Success! We found the null-terminator to the ANSI full pathname string.
						// Now save the pointer to that string where we can find it later.
							pszLocalBasePath = PCSTR(pcVolatile);
							break;
						}
					}
				}
			}
		}

		// If bit #1 is set, the cbCommonNetworkRelativeLink member is valid.
 		if (pLinkInfo->dwFlags & 0x00000002)
		{
			BREAK_IF_DEBUG	// Do we have any test files with this feature?

			/*
			cbOffset = pLinkInfo->cbCommonNetworkRelativeLinkOffset;
			pNetworkInfo = PLINK_NETWORK_INFO(PBYTE(pLinkInfo) + cbOffset);
			if ((PBYTE(pNetworkInfo) >= pLinkInfoDataStart) && (PBYTE(pNetworkInfo) < pLinkInfoDataEnd))
			{
				//if (pNetworkInfo->cbSize == OFFSETOF(LINK_NETWORK_INFO, cbNetNameOffsetUnicode))
				//{
				//	cbOffset = pNetworkInfo->cbNetNameOffset;
				//	cbOffset = pNetworkInfo->cbDeviceNameOffset;
				//}
				//else
				//{
				//	cbOffset = pNetworkInfo->cbNetNameOffsetUnicode;	// (optional)
				//	cbOffset = pNetworkInfo->cbDeviceNameOffsetUnicode;	// (optional)
				//}
			}
			*/
		}

		// Advance pTraverse to the very first byte after the LINK_INFO structure.
		// It's possible that the file ends here.
		// If this were to happen pTraverse would point to pMemEnd.
		pTraverse += pLinkInfo->cbTotalSize;

		// Update cbRemaining to reflect the number of bytes that follow the LINK_INFO structure.
		// It is possible that there will be no more bytes to process.
		cbRemaining -= pLinkInfo->cbTotalSize;
	}

	// NAME_STRING
	// This is an optional structure that specifies a description of the shortcut,
	// which is displayed to end users to identify the purpose of the shell link.
	// In some documents this string is called the "Description".
	// Note that THIS IS _NOT_ THE NAME OF THE TARGET FILE!
	// This structure MUST be present if the SLDF_HAS_NAME bit is set.
	// We simply skip over this string if it exists.
	if (pHeader->dwFlags & SLDF_HAS_NAME)
	{
		// Make sure there's enough memory to fetch the character count from the LINK_STRING_DATA structure.
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(SHORT)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Get the character count (the string does not have a null-terminator).
		USHORT cch = PLINK_STRING_DATA(pTraverse)->cch;

		// All LINK_STRING_DATA structures are at last two bytes long to accommodate the character count.
		UINT32 cbStruct	= sizeof(USHORT) + (cch << fUnicode);

		pTraverse += cbStruct;
		cbRemaining -= cbStruct;
	}

	// RELATIVE_PATH
	// An optional structure that specifies the location of the link target relative to the file that
	// contains the shell link. When specified, this string SHOULD be used when resolving the link. 
	// This structure MUST be present if the SLDF_HAS_RELPATH bit is set.
	// We simply skip over this string if it exists.
	if (pHeader->dwFlags & SLDF_HAS_RELPATH)
	{
		// Make sure there's enough memory to fetch the character count from the LINK_STRING_DATA structure.
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(SHORT)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Get the character count. WARNING: The string does not have a null-terminator.
		WORD cch = PLINK_STRING_DATA(pTraverse)->cch;

		// All LINK_STRING_DATA structure are at last two bytes long to accommodate the character count.
		UINT32 cbStruct	= sizeof(WORD);

		// A zero-length string is permissible and valid, but we only care about strings with characters.
		if (cch)
		{
			cbStruct += cch << fUnicode;
			pRelPath = PLINK_STRING_DATA(pTraverse);
		}

		pTraverse += cbStruct;
		cbRemaining -= cbStruct;
	}

	// WORKING_DIR:
	// An optional structure that specifies the file system path of the working directory to be used when
	// activating the link target.
	// This structure MUST be present if the SLDF_HAS_WORKINGDIR bit is set.
	// We simply skip over this string if it exists.
	if (pHeader->dwFlags & SLDF_HAS_WORKINGDIR)
	{
		// Make sure there's enough memory to fetch the character count from the LINK_STRING_DATA structure.
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(SHORT)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Get the character count (the string does not have a null-terminator).
		USHORT cch = PLINK_STRING_DATA(pTraverse)->cch;

		// All LINK_STRING_DATA structure are at last two bytes long to accommodate the character count.
		UINT32 cbStruct	= sizeof(USHORT);

		// A zero-length string is permissible and valid, but we only care about strings with characters.
		if (cch)
		{
			cbStruct += cch << fUnicode;
			pWorkingDir = PLINK_STRING_DATA(pTraverse);
		}

		pTraverse += cbStruct;
		cbRemaining -= cbStruct;
	}

	// COMMAND_LINE_ARGUMENTS:
	// An optional structure that stores the command-line arguments that are specified when activating the link target.
	// This structure MUST be present if the SLDF_HAS_ARGS bit is set.
	// We simply skip over this string if it exists.
	if (pHeader->dwFlags & SLDF_HAS_ARGS)
	{
		// Make sure there's enough memory to traverse this structure.
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(SHORT)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Get the character count (the string does not have a null-terminator).
		WORD cch = PLINK_STRING_DATA(pTraverse)->cch;

		// All LINK_STRING_DATA structure are at last two bytes long to accommodate the character count.
		UINT32 cbStruct	= sizeof(WORD) + (cch << fUnicode);

		pTraverse += cbStruct;
		cbRemaining -= cbStruct;
	}

	// ICON_LOCATION:
	// An optional structure that specifies the location of the icon to be used when displaying a shell
	// link item in an icon view. This structure MUST be present if the SLDF_HAS_ICONLOCATION bit is set.
	// We simply skip over this string if it exists.
	if (pHeader->dwFlags & SLDF_HAS_ICONLOCATION)
	{
		// Make sure there's enough memory to traverse this structure.
		if ((pTraverse >= pMemEnd) || (INT32(cbRemaining) < sizeof(WORD)))
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		// Get the character count. WARNING: the string does not have a null-terminator!
		USHORT cch = PLINK_STRING_DATA(pTraverse)->cch;

		// All LINK_STRING_DATA structures are at last two bytes long to accommodate the character count.
		UINT32 cbStruct	= sizeof(WORD) + (cch << fUnicode);

		pTraverse += cbStruct;
		cbRemaining -= cbStruct;
	}

	// Now look for LINK_EXTRA_DATA structures that are appended to the end of the link.
	while ((pTraverse < pMemEnd) && (cbRemaining >= sizeof(LINK_EXTRA_DATA)))
	{
		UINT32	cbBlock		= PLINK_EXTRA_DATA(pTraverse)->cbBlockSize;
		DWORD	dwSignature	= PLINK_EXTRA_DATA(pTraverse)->dwSignature;

		if (cbBlock > cbRemaining)
		{
			BREAK_IF_DEBUG
			hr = E_FAIL;
			goto L_Exit;
		}

		if (dwSignature == 0xA0000003)	// TrackerDataBlock
		{
			PLINK_TRACKER_PROPS pTrackerProps = PLINK_TRACKER_PROPS(pTraverse);
			if ((pTrackerProps->cbLength == 0x00000058) && (pTrackerProps->dwVersion == 0x00000000))
			{
				// Copy the machine name character by character because it is not null-terminated.
				UINT i	= 0;
				CHAR ch	= 0;
				do
				{
					ch = pTrackerProps->chMachineID[i];
					szNetBiosName[i] = ch;
				} while ((ch != 0) && (++i < ELEMS(pTrackerProps->chMachineID)));
			}
		}

		if (dwSignature == 0xA000000C)	// VistaAndAboveIDListDataBlock
		{
			BREAK_IF_DEBUG
		}

		pTraverse += cbBlock;
		cbRemaining -= cbBlock;
	}

	// Now we're done collecting strings and string pointers.
	if ((cbRemaining == 0) || ((cbRemaining == sizeof(UINT32)) && (0 == *PUINT32(pTraverse))))
	{
		//#ifdef _DEBUG
		//// Copy the string pointers so we can see them in the debugger.
		//PLINK_STRING_DATA	pDbgRelPath		= pRelPath;
		//PLINK_STRING_DATA	pDbgWorkingDir	= pWorkingDir;
	
		//PCSTR	pszDbgNetBiosName	= szNetBiosName;
		//PCSTR	pszDbgVolumeName	= pszVolumeName;
		//PCSTR	pszDbgBasePath		= pszLocalBasePath;
		//PCWSTR	pwzDbgVolumeName	= pwzVolumeName;
		//PCWSTR	pwzDbgBasePath		= pwzLocalBasePath;
	
		//__debugbreak();
		//#endif

		if (szNetBiosName[0])
		{
			// A NetBIOS name is always 16 ASCII characters without a terminator.
			// However our szNetBiosName[] array holds 20 characters.
			// The last four characters are always zero so that we can effectively terminate the string.
			if (MultiByteToWideChar(CP_ASCII, 0, szNetBiosName, -1, m_wzServerName, ELEMS(m_wzServerName)))
			{
				// Returns the number of WCHAR values written to m_wzServerName if successful.
				// Therefore we promote our HRESULT from E_FAIL to S_FALSE to indicate partial success.
				hr = S_FALSE;
			}
		}

		if (pwzVolumeName)
		{
			// The string already exists as a null-terminated PCWSTR.
			// So just copy it verbatim.
			lstrcpynW(m_wzVolumeName, pwzVolumeName, ELEMS(m_wzVolumeName));

			// Now promote our HRESULT from E_FAIL to S_FALSE to indicate partial success.
			hr = S_FALSE;
		}
		else if (pszVolumeName)
		{
			if (MultiByteToWideChar(GetWorkingCodePage(), 0, pszVolumeName, -1, m_wzVolumeName, ELEMS(m_wzVolumeName)))
			{
				// Returns the number of WCHAR values written to m_wzVolumeName if successful.
				// Therefore we promote our HRESULT from E_FAIL to S_FALSE to indicate partial success.
				hr = S_FALSE;
			}
		}

		// The preferred way to get the full pathname is though the ITEMIDLIST.
		if (pShellItemID)
		{
			PCIDLIST_ABSOLUTE pItemIDList = PCIDLIST_ABSOLUTE(pShellItemID->abID);
			if (SHGetPathFromIDListW(pItemIDList, m_wzRawLocation))
			{
				hr = Default();
				goto L_Exit;
			}
		}

		// Notwithstanding that, the next best way to get the full pathname is via pwzLocalBasePath.
		if (pwzLocalBasePath)
		{
			lstrcpynW(m_wzRawLocation, pwzLocalBasePath, ELEMS(m_wzRawLocation));
			hr = Default();
			goto L_Exit;
		}

		// Notwithstanding that, the next best way to get the full pathname is via pszLocalBasePath.
		if (pszLocalBasePath)
		{
			MultiByteToWideChar(GetWorkingCodePage(), 0, pszLocalBasePath, -1, m_wzRawLocation, ELEMS(m_wzRawLocation));
			hr = Default();
			goto L_Exit;
		}

		// If all else fails try to get the filename from pRelPath as a last resort.
		if (pRelPath)
		{
			// Get the character count (the string does not have a null-terminator).
			UINT cchMax = pRelPath->cch;

			// Make sure the string will not overflow m_wzRawLocation[].
			if (cchMax >= ELEMS(m_wzRawLocation))
			{
				cchMax = ELEMS(m_wzRawLocation)-1;
			}

			//
			if (fUnicode)
			{
				lstrcpynW(m_wzRawLocation, pRelPath->asUtf16, cchMax);
				hr = Default();
				goto L_Exit;
			}
			else
			{
				MultiByteToWideChar(GetWorkingCodePage(), 0, pRelPath->asAnsi, cchMax, m_wzRawLocation, ELEMS(m_wzRawLocation)-1);
				hr = Default();
				goto L_Exit;
			}
		}
	}

	// If we've made it to this point then COmfLocator::Default() was never called, and
	// m_wzRawLocation[], m_wzFullPath[], m_wzFileName[], and m_wzPathRoot[] are all empty.
	// The structure of the shortcut's payload seems valid and has passed our tests.
	// If the strings in m_wzVolumeName[] or m_wzServerName[] are valid then we return S_FALSE.
	// Otherwise we return E_FAIL.

L_Exit:
	return hr;
}
