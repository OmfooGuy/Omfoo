// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer13.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer12.h"

class CContainerLayer13 : public CContainerLayer12
{
protected:
			CContainerLayer13(void);
	virtual	~CContainerLayer13(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

private:
	HRESULT	MegaIngest(void);

	// Helper called once per lifetime from MegaIngest().
	HRESULT	IdentifyOneMdat(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
									__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is an AIFC.
	HRESULT IdentifyAIFC(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is an IDAT.
	HRESULT IdentifyIDAT(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is a JPEG.
	HRESULT IdentifyJPEG(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is an MDAT.
	//HRESULT IdentifyMDAT(__inout MDAT_CACHE_ENTRY& rCE,
	//						__in const PBYTE pMem,
	//							__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is a MPEG.
	HRESULT IdentifyMPEG(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is a RLE_.
	HRESULT IdentifyRLE_(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is a SD2M.
	HRESULT IdentifySD2M(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is a TIFF.
	HRESULT IdentifyTIFF(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Handler for IdentifyOneMdat() when the MDAT is a WAVE.
	HRESULT IdentifyWAVE(__inout MDAT_CACHE_ENTRY& rCE,
							__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT	CategorizeMdats(void);
};
