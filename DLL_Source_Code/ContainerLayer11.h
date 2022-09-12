// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer11.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer10.h"

class CContainerLayer11 : public CContainerLayer10
{
protected:
			CContainerLayer11(void);
	virtual	~CContainerLayer11(void);

	HRESULT	IdentifyIDAT_CDCI_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyIDAT_CDCI_AUNC(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyIDAT_JPED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyIDAT_RGBA_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyIDAT_RLED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyIDAT_RLED_PRLE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyJPEG_CDCI_AVHD(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyJPEG_CDCI_DV_C(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyJPEG_CDCI_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyJPEG_JPED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyJPEG_JPED_JFIF(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyMPEG_MPGI_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyMPEG_MPGI_MPG2(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyRLE__RLED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);

	HRESULT	IdentifyRLE__RLED_PRLE(__inout MDAT_CACHE_ENTRY& rCE,
									__in const PBYTE pMem,
									__in const UINT32 cbMem);
};
