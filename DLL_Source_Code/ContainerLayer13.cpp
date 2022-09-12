// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer13.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer13.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer13::CContainerLayer13(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer13::~CContainerLayer13(void)
{
}

//*********************************************************************************************************************
//	
//*********************************************************************************************************************
HRESULT CContainerLayer13::Load(PCWSTR pwzFileName)
{
	HRESULT hr = __super::Load(pwzFileName);
	if (SUCCEEDED(hr))
	{
		CHECK(MegaIngest());
		CHECK(CategorizeMdats());
	}
	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	Iterate through each MDAT and identify the file format/content of the payload/embedded file.
//*********************************************************************************************************************
HRESULT CContainerLayer13::MegaIngest(void)
{
	HRESULT	hr = S_OK;
	if (m_aMdatTable)
	{
		// First we allocate a temporary memory buffer where we can read in the first few bytes of the payload.
		// cbAlloc is the maximum amount of memory that we are willing to allocate for our temporary buffer.
		// It does NOT need to be large enough to hold an entire MDAT payload - it only needs to be large enough
		// to detect (or verify) its content (the payload can be partial/truncated).
		// The minimum size needed is currently 144000 bytes, because that is the size of one PAL DV frame.
		PBYTE	pMem	= NULL;
		UINT32	cbAlloc	= 0x00040000;	// 256 Kibibytes, 18 bits, 262144 bytes

		// If the size of the largest embedded file is less than cbAlloc then use the lesser size instead.
		if (m_cbMaxMdatPayload < cbAlloc)
		{
			cbAlloc = UINT32(m_cbMaxMdatPayload);
		}

		// VirtualAlloc() will round cbAlloc up to the next largest 4096 byte page size.
		if (pMem = PBYTE(VirtualAlloc(NULL, cbAlloc, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE)))
		{
			for (ULONG i = 0; i < m_cMDATs; i++)
			{
				UINT64	cbSeekPos	= m_aMdatTable[i].cbPayloadOffset;
				UINT64	cbFile		= m_aMdatTable[i].cbPayloadLength;
				UINT32	cbMem		= cbAlloc;

				// If the size of the media/embedded file is less than the size of our memory block
				// then tweak cbMem so that the detector routines won't test beyond the length
				// of the media/embedded file.
				if (cbFile < cbAlloc)
				{
					cbMem = UINT32(cbFile);
				}

				// Read in some (but not necessarily all) bytes.
				if (FAILED(hr = SeekRead(cbSeekPos, pMem, cbMem)))
				{
					// A file system error is always a fatal error.
					BREAK_IF_DEBUG
					break;
				}

				// Call helper routine to do the actual detection.
				if (FAILED(hr = IdentifyOneMdat(m_aMdatTable[i], pMem, cbMem)))
				{
					// If a detection routine encountered a file system error, an out-of-memory error,
					// or an internal assertion error then abort this for() loop and treat it as a fatal error.
					// This will ultimately cause CContainerLayer13::Load() to fail.
					if ((HRESULT_FACILITY(hr)==FACILITY_WIN32)||
						(hr == OMFOO_E_ASSERTION_FAILURE)||
						(hr == E_OUTOFMEMORY))
					{
						break;
					}

					// Else set the fIdentifyFailed flag for this MDAT_CACHE_ENTRY structure.
					m_aMdatTable[i].fIdentifyFailed = TRUE;
					hr = S_OK;
				}
			}

			// Free our big temporary buffer.
			VirtualFree(pMem, SIZE_T(0), MEM_RELEASE);
		}
		else
		{
			// VirtualAlloc() failed!
			BREAK_IF_DEBUG
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Called once per lifetime from MegaIngest().
//	Here we call a unique handler for each known MDAT class.
//	Each handler is ultimately responsible for populating these members of the MDAT_CACHE_ENTRY structure:
//
//	bCategory			- MDAT_CATEGORY_AUDIO_FILE, MDAT_CATEGORY_VIDEO_FILE, MDAT_CATEGORY_OTHER_FILE
//	qwFileNameExt		- an ASCII filename extension packed into a QWORD, zero-padded when necessary.
//	fSelfContained		- TRUE if the payload does not require additional meta-data - like a frame index table.
//	fPayloadBigEndian	- TRUE if the payload is big endian (AIFC, SD2M, TIFF BE, etc.).
//	fPayloadConfirmed	- TRUE if the handler examined the first few bytes of the payload and confirmed the content.
//
//	A handler may dispatch/delegate its work to other 'sub-handlers'.
//	Each handler returns S_FALSE on non-fatal errors - when it can't identify or confirm the file format or payload.
//	A handler may also return a fatal error code - in which case this entire series of handler routines should abort.
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyOneMdat(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;
 	switch (rCE.oMDAT.dwFourCC)
	{
	case FCC('AIFC'):	// inherits MDAT
		hr = IdentifyAIFC(rCE, pMem, cbMem);
		break;

	case FCC('IDAT'):	// inherits MDAT
		hr = IdentifyIDAT(rCE, pMem, cbMem);
		break;

	case FCC('JPEG'):	// inherits IDAT
		hr = IdentifyJPEG(rCE, pMem, cbMem);
		break;

	//case FCC('MDAT'):	// inherits OOBJ
	//	hr = IdentifyMDAT(rCE, pMem, cbMem);
	//	break;

	case FCC('MPEG'):	// inherits IDAT
		hr = IdentifyMPEG(rCE, pMem, cbMem);
		break;

	case FCC('RLE '):	// inherits IDAT
		hr = IdentifyRLE_(rCE, pMem, cbMem); 
		break;

	case FCC('SD2M'):	// inherits MDAT
		hr = IdentifySD2M(rCE, pMem, cbMem);
		break;

	case FCC('TIFF'):	// inherits MDAT
		hr = IdentifyTIFF(rCE, pMem, cbMem);
		break;

	case FCC('WAVE'):	// inherits MDAT
		hr = IdentifyWAVE(rCE, pMem, cbMem);
		break;

	default:
		// This MDAT has an unknown class fourCC.
		// How did we get here?
		// And now that we're here, what should we do?
		// Break and look at the MDES fourCC, and maybe add code to ingest it.
		#ifdef _DEBUG
		CHAR szMdatFourCC[8] = {0};
		CHAR szMdesFourCC[8] = {0};
		CHAR szCompFourCC[8] = {0};
		*PDWORD(szMdatFourCC) = rCE.oMDAT.dwFourCC;
		*PDWORD(szMdesFourCC) = rCE.oMDES.dwFourCC;
		*PDWORD(szCompFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;

		// See it it inherits IDAT.
		if (S_OK == IsBlopATypeOf(rCE.oMDAT, FCC('IDAT')))
		{
			BREAK_IF_DEBUG
			hr = S_FALSE;
		}
		else
		{
			BREAK_IF_DEBUG
			hr = S_FALSE;
		}
	}

	//if (hr != S_OK)
	//{
	//	// For debugging only!
	//	// Call ourself recursively to single-step and see where we failed.
	////	__debugbreak();
	//	//hr = IdentifyOneMdat(rCE, pMem, cbMem);
	//}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when MDAT is a AIFC Audio Data Class (AIFC).
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyAIFC(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is an AIFC, and so we know for sure it contains audio.
	rCE.bCategory = MDAT_CATEGORY_AUDIO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('AIFD'):
		hr = IdentifyAIFC_AIFD(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when MDAT is a Image Data Class (IDAT).
//	Returns S_FALSE on non-fatal errors - when it can't identify the payload (the file format) of the embedded file.
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyIDAT(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is an IDAT, and so we know for sure it contains video.
	rCE.bCategory = MDAT_CATEGORY_VIDEO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('CDCI'):
		hr = IdentifyIDAT_CDCI(rCE, pMem, cbMem);
		break;

	case FCC('JPED'):
		hr = IdentifyIDAT_JPED(rCE, pMem, cbMem);
		break;

	case FCC('RGBA'):
		hr = IdentifyIDAT_RGBA(rCE, pMem, cbMem);
		break;

	case FCC('RLED'):
		hr = IdentifyIDAT_RLED(rCE, pMem, cbMem);
		break;

	//case FCC('XXXX'):
	//	hr = IdentifyIDAT_XXXX(rCE, pMem, cbMem);
	//	break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when the MDAT is a JPEG Image Data Class (JPEG).
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyJPEG(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is an JPEG, and so we know for sure it contains video.
	rCE.bCategory = MDAT_CATEGORY_VIDEO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('JPED'):
		hr = IdentifyJPEG_JPED(rCE, pMem, cbMem);
		break;

	case FCC('CDCI'):
		hr = IdentifyJPEG_CDCI(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when MDAT is the Media Data Class (MDAT).
//	This class is abstract, so we should never come here. This code is just a stub/reality check.
//	Always returns S_FALSE for now - until we find out if we ever even need it.
//*********************************************************************************************************************
//HRESULT CContainerLayer13::IdentifyMDAT(__inout MDAT_CACHE_ENTRY& rCE,
//											__in const PBYTE pMem,
//												__in const UINT32 cbMem,
//XXXYYYXXX)
//{
	// How did we get here?
	// And now that we're here, what should we do?
	// Break and look at the MDES fourCC.
	//#ifdef _DEBUG
	//CHAR szMdatFourCC[8] = {0};
	//CHAR szMdesFourCC[8] = {0};
	//CHAR szCompFourCC[8] = {0};
	//*PDWORD(szMdatFourCC) = rCE.oMDAT.dwFourCC;
	//*PDWORD(szMdesFourCC) = rCE.oMDES.dwFourCC;
	//*PDWORD(szCompFourCC) = rCE.dwNonambiguousCompressionFourCC;
	//__debugbreak();
	//#endif
//	return S_FALSE;
//}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when MDAT is an MPEG Image Data Class (MPEG).
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyMPEG(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is an MPEG, and so we know for sure it contains video.
	rCE.bCategory = MDAT_CATEGORY_VIDEO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('MPGI'):
		hr = IdentifyMPEG_MPGI(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when the Media Data Class (MDAT) is 'RLE '.

//	Non-standard class found in some OMF2 files. inherits 'IDAT'.
//	See test file "FG305R_15___GAME NIGHT V.omf."

//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyRLE_(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is an RLE_, and so we know for sure it contains video.
	rCE.bCategory = MDAT_CATEGORY_VIDEO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('RLED'):
		hr = IdentifyRLE__RLED(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when MDAT is a Sound Designer II Audio Data Class (SD2M).
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifySD2M(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is an AIFC, and so we know for sure it contains audio.
	rCE.bCategory = MDAT_CATEGORY_AUDIO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('SD2D'):
		hr = IdentifySD2M_SD2D(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when MDAT is a TIFF Image Data Class (TIFF).
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyTIFF(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is a TIFF, and so we know for sure it contains video.
	rCE.bCategory = MDAT_CATEGORY_VIDEO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('TIFD'):
		hr = IdentifyTIFF_TIFD(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyOneMdat().
//	Come here when the Media Data Class (MDAT) is a WAVE Audio Data Class (WAVE).
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer13::IdentifyWAVE(__inout MDAT_CACHE_ENTRY& rCE,
											__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// We categorized these as 'audio', 'video', or 'other' back in CContainerLayer10::CollectSampleRates().
	// We made our determination based entirely on the OMFI:MDFL:SampleRate property.
	// That way we could categorize an MDAT even if/when we didn't recognize any of the MDAT or MDES fourCCS.
	// But now we know that this is a WAVE, and so we know for sure it contains audio.
	rCE.bCategory = MDAT_CATEGORY_AUDIO_FILE;

	switch (rCE.oMDES.dwFourCC)
	{
	case FCC('WAVD'):
		hr = IdentifyWAVE_WAVD(rCE, pMem, cbMem);
		break;

	default:
		// Unknown MDES fourCC.
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.oMDES.dwFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper called once per lifetime from Load().
//	1) Calculate & populate the nCategoryInstance member of each MDAT_CACHE_ENTRY structure in m_aMdatTable[].
//	2) Calculate & populate our m_cMdatsWithAudio, m_cMdatsWithVideo, and m_cMdatsWithOther counters.
//
//	We must call CollectSampleRates() and MegaIngest() before calling here.
//	This implementation always succeeds!
//*********************************************************************************************************************
HRESULT CContainerLayer13::CategorizeMdats(void)
{
	if (m_aMdatTable)
	{
		PMDAT_CACHE_ENTRY pCurMdat	= m_aMdatTable;
		PMDAT_CACHE_ENTRY pEndMdat	= &m_aMdatTable[m_cMDATs];
		do {
			if (pCurMdat->bCategory == MDAT_CATEGORY_AUDIO_FILE)
			{
				// "Audio File"
				pCurMdat->wCategoryInstance = ++m_cMdatsWithAudio;
			}
			else if (pCurMdat->bCategory == MDAT_CATEGORY_VIDEO_FILE)
			{
				// "Video File"
				pCurMdat->wCategoryInstance = ++m_cMdatsWithVideo;
			}
			else
			{
				// "Madia File" (unknown)
				pCurMdat->wCategoryInstance = ++m_cMdatsWithOther;
			}
		} while (++pCurMdat < pEndMdat);
	}

	return S_OK;
}



