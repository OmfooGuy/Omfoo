// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer12.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer11.h"

class CContainerLayer12 : public CContainerLayer11
{
protected:
			CContainerLayer12(void);
	virtual	~CContainerLayer12(void);

	HRESULT	IdentifyAIFC_AIFD(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyIDAT_CDCI(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyIDAT_JPED(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyIDAT_RGBA(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyIDAT_RLED(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyJPEG_CDCI(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyJPEG_JPED(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyMPEG_MPGI(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyRLE__RLED(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifySD2M_SD2D(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT IdentifyTIFF_TIFD(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	HRESULT	IdentifyWAVE_WAVD(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);

	// Default handler for when we don't recognize the MDAT's class fourCC. 
	HRESULT IdentifyLastChance(__inout MDAT_CACHE_ENTRY& rCE,
								__in const PBYTE pMem,
								__in const UINT32 cbMem);
};
