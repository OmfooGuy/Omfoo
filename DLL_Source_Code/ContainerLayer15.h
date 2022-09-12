// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer15.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer14.h"

class CContainerLayer15 : public CContainerLayer14
{
protected:
			CContainerLayer15(void);
	virtual	~CContainerLayer15(void);
//	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

public:
	ULONG	GetMdatCategory(__in ULONG idx);
	DWORD	GetMdatMediaDataObject(__in ULONG idx);
	DWORD	GetMdatSourceMobObject(__in ULONG idx);
	DWORD	GetMdatMediaDescriptorObject(__in ULONG idx);

	// These are overloaded versions of their like-named counterparts.
	// They all accept an index to m_aMdatTable[] as their MDAT argument.
	// They find the corresponding entry in m_aMdatTable[] and then call the lower routine.
	STDMETHODIMP	GetMdatMobID(__in ULONG idx,
									__out POMF_MOB_ID pMobID);

	STDMETHODIMP	GetMdatRawFileParams(__in ULONG idx,
											__out PUINT64 pOffset,
												__out PUINT64 pLength,
													__out_opt PHANDLE pHandle);

	STDMETHODIMP	CreateStreamOnMdatData(__in ULONG idx,
												__in REFIID riid,
													__out PVOID *ppvOut);

	STDMETHODIMP	CreateDShowSourceFilterOnData(__in ULONG idx,
													__in REFIID riid,
														__out PVOID *ppvOut);

	STDMETHODIMP	CreateFileNameForMdat(__in ULONG idx,
												__in DWORD dwSyntax,
													__in ULONG cchBuffer,
														__out_opt PWCHAR pBuffer,
															__out_opt PULONG pcchRequired);

	STDMETHODIMP	ExtractMdatDataToFile(__in ULONG idx,
											__in PCWSTR pwzFullPathNewFile,
												__in_opt IOmfooExtractCallback *pCallback,
													__in_opt ULONG nPagesPerCallback,
														__in BOOL fOverwrite);

	// These are the routines that do the actual work.
	STDMETHODIMP	QueryMdatRawFileParams(__in MDAT_CACHE_ENTRY& rCE,
												__out PUINT64 pOffset,
													__out PUINT64 pLength,
														__out_opt PHANDLE pHandle);

	STDMETHODIMP	CreateStreamOnMdatData(__in MDAT_CACHE_ENTRY& rCE,
												__in REFIID riid, __out PVOID *ppvOut);


	STDMETHODIMP	CreateDShowSourceFilterOnData(__in MDAT_CACHE_ENTRY& rCE,
													__in REFIID riid,
														__out PVOID *ppvOut);

	STDMETHODIMP	CreateFileNameForMdat(__in MDAT_CACHE_ENTRY& rCE,
												__in DWORD dwSyntax,
													__in ULONG cchBuffer,
														__out_opt PWCHAR pBuffer,
															__out_opt PULONG pcchRequired);

	STDMETHODIMP	ExtractMdatDataToFile(__in MDAT_CACHE_ENTRY& rCE,
											__in PCWSTR pwzFullPathNewFile,
												__in_opt IOmfooExtractCallback *pCallback,
													__in_opt ULONG nPagesPerCallback,
														__in BOOL fOverwrite);

};
