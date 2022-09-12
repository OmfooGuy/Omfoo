// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer12.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer12.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer12::CContainerLayer12(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer12::~CContainerLayer12(void)
{
}

//*********************************************************************************************************************
//	Private helper for IdentifyAIFD().
//	Come here when the MDAT is an AIFC and the MDES is a AIFD.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT	CContainerLayer12::IdentifyAIFC_AIFD(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// Examine the first few bytes of the payload to confirm that this is actually an AIFF (or AIFC) file.
	if (rCE.qwFileNameExt =	DetectFORM(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = TRUE;
		rCE.fPayloadBigEndian = TRUE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyIDAT().
//	Come here when the MDAT is an IDAT and the MDES is a CDCI.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyIDAT_CDCI(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;
	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyIDAT_CDCI_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyIDAT_CDCI_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('AUNC'):
		hr = IdentifyIDAT_CDCI_AUNC(rCE, pMem, cbMem);	// Yes! This happens!
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyIDAT().
//	Come here when the MDAT is an IDAT and the MDES is a JPED.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyIDAT_JPED(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	#ifdef _DEBUG
	CHAR szFourCC[8] = {0};
	*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
	__debugbreak();
	#endif

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyIDAT_JPED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyIDAT_JPED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	default:
		BREAK_IF_DEBUG
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyIDAT().
//	Come here when the MDAT is a IDAT and the MDES is a RGBA.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyIDAT_RGBA(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		hr = IdentifyIDAT_RGBA_NONE(rCE, pMem, cbMem);	// Yes! This happens.
		break;

	case FCC('NONE'):
		hr = IdentifyIDAT_RGBA_NONE(rCE, pMem, cbMem);	// Yes! This happens.
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyIDAT().
//	Come here when the MDAT is a IDAT and the MDES is a RLED.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyIDAT_RLED(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyIDAT_RLED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyIDAT_RLED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('PRLE'):
		hr = IdentifyIDAT_RLED_PRLE(rCE, pMem, cbMem);	// Yes. This happens in OMF1!
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyJPEG().
//	Come here when the MDAT is a JPEG and the MDES is a JPED.
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyJPEG_CDCI(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyJPEG_CDCI_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyJPEG_CDCI_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('AVHD'):
		hr = IdentifyJPEG_CDCI_AVHD(rCE, pMem, cbMem);	// Yes this happens!
		break;

	case FCC('DV/C'):
		hr = IdentifyJPEG_CDCI_DV_C(rCE, pMem, cbMem);	// Yes this happens!
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyJPEG().
//	Come here when the MDAT is a JPEG and the MDES is a JPED.
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyJPEG_JPED(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyJPEG_JPED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyJPEG_JPED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('JFIF'):
		hr = IdentifyJPEG_JPED_JFIF(rCE, pMem, cbMem);	// Yes! This happens.
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyMPEG().
//	Come here when the MDAT is an MPEG, and the MDES is an MPGI.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyMPEG_MPGI(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyMPEG_MPGI_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyMPEG_MPGI_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('MPG2'):
		hr = IdentifyMPEG_MPGI_MPG2(rCE, pMem, cbMem);	// Yes! This happens!
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyRLE_().
//	Come here when the MDAT is a RLE_ and the MDES is a RLED.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyRLE__RLED(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	switch (rCE.dwNonambiguousCompressionFourCC)
	{
	case 0x00000000:
		BREAK_IF_DEBUG
		hr = IdentifyRLE__RLED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('NONE'):
		BREAK_IF_DEBUG
		hr = IdentifyRLE__RLED_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
		break;

	case FCC('PRLE'):
		BREAK_IF_DEBUG
		hr = IdentifyRLE__RLED_PRLE(rCE, pMem, cbMem);	// Yes! This happens!
		break;

	default:
		#ifdef _DEBUG
		CHAR szFourCC[8] = {0};
		*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
		__debugbreak();
		#endif
		break;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifySD2M().
//	Come here when the MDAT is a SD2M and the MDES is a SD2D.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifySD2M_SD2D(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	UINT16 wNumOfChannels = 0;	// OMFI:SD2D:NumOfChannels
	UINT16 wBitsPerSample = 0;	// OMFI:SD2D:BitsPerSample
	if (SUCCEEDED(RordReadUInt16(rCE.oMDES, ePropSd2dNumOfChannels, &wNumOfChannels)))
	{
		if ((wNumOfChannels == 1)||(wNumOfChannels == 2))
		{
			if (SUCCEEDED(RordReadUInt16(rCE.oMDES, ePropSd2dBitsPerSample, &wBitsPerSample)))
			{
				if ((wBitsPerSample > 0)&&(wBitsPerSample <= 24))
				{
					BYTE cbBytesPerSample = 1;
					cbBytesPerSample += BOOLEAN(wBitsPerSample > 8);
					cbBytesPerSample += BOOLEAN(wBitsPerSample > 16);
					BYTE cbBytesPerFrame = BYTE(wNumOfChannels) * cbBytesPerSample;
					UINT64 cbPayloadRequired = cbBytesPerFrame * rCE.qwDuration;
					if (rCE.cbPayloadLength >= cbPayloadRequired)
					{
						rCE.fSelfContained    = FALSE;	// because the payload is only PCM samples; no meta-data.
						rCE.fPayloadBigEndian = TRUE;	// because it's a Mac file format.
						rCE.fPayloadConfirmed = TRUE;	// because it passed our little tests.
						hr = S_OK;
					}
				}
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyTIFF().
//	Come here when the MDAT is a TIFF, and the MDES is a TIFD.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyTIFF_TIFD(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// Tagged Image File Format (little endian).
	if (rCE.qwFileNameExt = DetectTIFF_LE(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = FALSE;	// TODO - Does this need a frame index table?
		rCE.fPayloadBigEndian = FALSE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
		goto L_Exit;
	}
	// Tagged Image File Format (big endian).
	if (rCE.qwFileNameExt = DetectTIFF_BE(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = FALSE;	// TODO - Does this need a frame index table?
		rCE.fPayloadBigEndian = TRUE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
		goto L_Exit;
	}

	BREAK_IF_DEBUG

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private helper for IdentifyWAVE().
//	Come here when the MDAT is a WAVE, and the MDES is a WAVD.
//	Returns S_FALSE on non-fatal errors (when we can't verify/identify the file format of the embedded file).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyWAVE_WAVD(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	// Examine the first few bytes of the payload to confirm that this is actually a RIFF/WAVE file.
	if (rCE.qwFileNameExt = DetectRIFF(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = TRUE;
		rCE.fPayloadBigEndian = FALSE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
		goto L_Exit;
	}

	BREAK_IF_DEBUG

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private helper for CContainerLayer13::IdentifyOneMdat().
//	Returns S_FALSE on non-fatal errors (when we can't identify or confirm the file format of the embedded file).
//	Come here when we don't recognize the Media Data Class (MDAT).
//*********************************************************************************************************************
HRESULT CContainerLayer12::IdentifyLastChance(__inout MDAT_CACHE_ENTRY& rCE,
												__in const PBYTE pMem,
												__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	#ifdef _DEBUG
	CHAR szFourCC[8] = {0};
	*PDWORD(&szFourCC) = rCE.dwNonambiguousCompressionFourCC;
	__debugbreak();
	#endif

	//switch (rCE.dwNonambiguousCompressionFourCC)
	//{
	//case 0x00000000:
	//	BREAK_IF_DEBUG
	//	break;

	//case FCC('NONE'):
	//	BREAK_IF_DEBUG
	//	hr = IdentifyXXXX_YYYY_NONE(rCE, pMem, cbMem);	// Does this ever happen with my test files?
	//	break;

	//default:
	//	BREAK_IF_DEBUG
	//	break;
	//}

	return hr;
}



