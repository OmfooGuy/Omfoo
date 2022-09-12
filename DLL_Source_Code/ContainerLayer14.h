// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer14.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer13.h"

class CContainerLayer14 : public CContainerLayer13
{
protected:
			CContainerLayer14(void);
	virtual	~CContainerLayer14(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

	static HRESULT	CreatePrefixW(__in PCWSTR pSrc, __in const ULONG cchDst, __out PWCHAR pDst);

	// Prefixes we use for generating media filenames.
	// These string buffers must be the same size, and that size must be PREFIX_BUFFER_SIZE.
	// See CreateFileNameForMdat() and CreatePrefix().
#define PREFIX_BUFFER_SIZE	10	// includes null terminator
	WCHAR	m_wzExportPrefix[PREFIX_BUFFER_SIZE];
	WCHAR	m_wzTopMobPrefix[PREFIX_BUFFER_SIZE];
	DWORD	m_dwCheckCodePage;
};
