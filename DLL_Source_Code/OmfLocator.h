// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfLocator.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"
#include "ShellLinkInternals.h"

//	Implements IOmfLocator for all OMFI objects that inherit LOCR.
class COmfLocator
	: private COmfObject
	, private NsShellLinkInternals
	, private IOmfLocator
{
public:
	static	COmfObject* __stdcall NewInstance(BENTO_BLOP& rBlop,
												CContainerLayer97* pContainer,
												COmfObject* pParent,
												PVOID pNewReserved);
private:
	COmfLocator(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);
	virtual			~COmfLocator(void);
	STDMETHODIMP	NonDelegatingQueryInterface(REFIID riid, void ** ppvOut);

    DECLARE_IUNKNOWN

	// IOmfLocator methods in V-table order.
	STDMETHODIMP	GetRawLocation(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetFileName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetFullPath(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetPathRoot(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetVolumeName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);
	STDMETHODIMP	GetServerName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired);

	// Call this once per lifetime during NonDelegatingQueryInterface() to populate our strings.
	STDMETHODIMP	Initialize(void);

	// Core Windows Directory Locator Class (CWDL)
	HRESULT DecomposeCWDL(void);

	// Core Windows File Locator Class (CWFL)
	HRESULT DecomposeCWFL(void);

	// Domain Locator Class (DOML)
	HRESULT DecomposeDOML(void);

	// DOS Locator Class (DOSL)
	HRESULT DecomposeDOSL(void);

	// Locator Class (LOCR)
	HRESULT DecomposeLOCR(void);

	// Mac Directory Locator Class (MACD)
	HRESULT DecomposeMACD(void);

	// Mac File Locator Class (FILE)
	HRESULT DecomposeFILE(void);

	// Mac Locator Class (MACL)
	HRESULT DecomposeMACL(void);

	// Avid Media File Manager Locator Class (MFML)
	HRESULT DecomposeMFML(void);

	// Avid Media Stream Manager Locator Class (MSML)
	HRESULT DecomposeMSML(void);

	// Network Locator Class (NETL)
	HRESULT DecomposeNETL(void);

	// Sound Designer II Stream Locator Class (SDSL)
	HRESULT DecomposeSDSL(void);

	// Text Locator Class (TXTL)
	HRESULT DecomposeTXTL(void);

	// UNIX Locator Class (UNXL)
	HRESULT DecomposeUNXL(void);

	// URL Locator Class (URLL)
	HRESULT DecomposeURLL(void);

	// Windows Directory Locator Class (WIND)
	HRESULT DecomposeWIND(void);

	// Windows File Locator Class (WINF)
	HRESULT DecomposeWINF(void);

	// Windows Locator Class (WINL)
	HRESULT DecomposeWINL(void);

	// Common helper for DecomposeDOSL(), DecomposeWINL(), and ShortcutHelper().
	HRESULT Default(void);

	// Helper for DecomposeWINL().
	HRESULT ShortcutHelper(PBYTE pShortcut, UINT32 cbShortcut);

	WCHAR	m_wzRawLocation[260];
	WCHAR	m_wzFullPath[260];
	WCHAR	m_wzFileName[260];
	WCHAR	m_wzPathRoot[64];
	WCHAR	m_wzVolumeName[32];		// NTFS = 32, Legacy Mac = 27 (or 48), Modern Mac (ApFS) = 256
	WCHAR	m_wzServerName[32];		// Host name, Machine name, Server name, NetBIOS name, etc.

	HRESULT	m_hrRawLocation;
	HRESULT	m_hrFullPath;
	HRESULT	m_hrFileName;
	HRESULT	m_hrPathRoot;
	HRESULT	m_hrVolumeName;
	HRESULT	m_hrServerName;
};
