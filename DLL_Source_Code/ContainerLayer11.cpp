// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer11.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer11.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer11::CContainerLayer11(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer11::~CContainerLayer11(void)
{
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyIDAT_CDCI() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyIDAT_CDCI_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyIDAT_CDCI() when the compression is 'AUNC'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyIDAT_CDCI_AUNC(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	// Yes! This happens!
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyIDAT_JPED() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyIDAT_JPED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyIDAT_RGBA() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//
//	List of all known RGBA properties and their corresponding data types.
//
//	Prop:	OMFI:RGBA:Palette
//	Type:	omfi:DataValue
//
//	Prop:	OMFI:RGBA:PaletteLayout
//	Type:	omfi:CompCodeArray
//
//	Prop:	OMFI:RGBA:PaletteStructure
//	Type:	omfi:CompSizeArray
//
//	Prop:	OMFI:RGBA:PaletteWidth
//	Type:	omfi:UInt32
//
//	Prop:	OMFI:RGBA:PixelLayout
//	Type:	omfi:CompCodeArray
//
//	Prop:	OMFI:RGBA:PixelStructure
//	Type:	omfi:CompSizeArray
//
//	Prop:	OMFI:RGBA:PixelWidth
//	Type:	omfi:UInt32
//
//	"To describe pixels with a palette, the RGBA object should have a 'P' character in the PixelLayout property
//	and not have a 'B', 'G', or 'R' character in the PixelLayout property value."
//
//	Note that OMFI:RGBA:Palette, OMFI:RGBA:PaletteLayout, and OMFI:RGBA:PaletteStructure are documented in Appendix A
//	of the OMF Specification Version 2.1 at pages 188 and 189. But there is no mention of OMFI:RGBA:PaletteWidth.
//	Similarly OMFI:RGBA:PixelLayout and OMFI:RGBA:PixelStructure are documented, but OMFI:RGBA:PixelWidth is not.
//	It looks to me like OMFI:RGBA:PaletteWidth was deprecated in favor of OMFI:RGBA:PaletteStructure,
//	and OMFI:RGBA:PixelWidth was deprecated in favor of OMFI:RGBA:PixelStructure.
//
//	None of my OMF test files have palettized RGBAs.
//	Note that there is no support for color palettes in the RGBA routines in Avid's reference code for omfToolkit.dll.
//	Moreover, there is a comment in the routine writeDescriptorData() in the source file omcRGBA.c that reads:
//
//		"OMFI:RGBA:Palette, OMFI:RGBA:PaletteLayout & OMFI:RGBA:PaletteStructure - not yet needed..."
//
//	Conclusion: FOR NOW WE DO NOT SUPPORT COLOR PALETTES!
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyIDAT_RGBA_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	HRESULT	hr = S_FALSE;

	//// Temporary union to build a filename extension.
	//union {
	//	CHAR	szExtension[8];
	//	QWORD	qwExtension;
	//};

	//// Temporary union to save bit-widths of each component (but not including 'O' non-components).
	//union {
	//	BYTE	aBitWidths[8];
	//	QWORD	qwBitWidths;
	//};

/*
	// OMFI:DIDD:StoredHeight, omfi:UInt32, omfi:Long
	ULONG nStoredHeight = 0;
	if (FAILED(hr = RordReadUInt32(rCE.oMDES, ePropDiddStoredHeight, PUINT32(&nStoredHeight))))
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// OMFI:DIDD:StoredWidth, omfi:UInt32, omfi:Long
	ULONG nStoredWidth = 0;
	if (FAILED(hr = RordReadUInt32(rCE.oMDES, ePropDiddStoredWidth, PUINT32(&nStoredWidth))))
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// OMFI:RGBA:PixelLayout, omfi:CompCodeArray
	CHAR aPixelLayout[8] = {0};
	if (FAILED(hr = RordReadCompCodeArray(rCE.oMDES, ePropRgbaPixelLayout, aPixelLayout)))
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// OMFI:RGBA:PixelStructure, omfi:CompSizeArray
	BYTE aPixelStructure[8] = {0};
	if (FAILED(RordReadCompSizeArray(rCE.oMDES, ePropRgbaPixelStructure, aPixelStructure)))
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// OMFI:DIDD:ImageAlignmentFactor, omfi:Int32 (optional)
	ULONG nAlignment = 0;
	RordReadUInt32(rCE.oMDES, ePropDiddImageAlignmentFactor, PUINT32(&nAlignment));

	// Each character except ASCII zero '0' (0x30) can appear no more than one time in the array.
	// The array is terminated by an actual zero (0x00) and has a maximum of 8 elements (including the terminator).
	// Note that the OMF v2.1 spec mentions a 'Fill component' but it never explains what it is or how it is used.
	// 'A' Alpha component
	// 'B' Blue component
	// 'F' Fill component	<< WTF is this? See page 188 of the OMF v2.1 Specification. WTF is it???
	// 'G' Green component
	// 'R' Red component
	// '0' no component	(ASCII 0x30 - the zero character)

	// Initialize a QWORD with all seven permissible characters followed by a null-terminator.
	// The characters in the lower 32 bits can only occur once.
	QWORD	qwBlessed	= 0x004F504641424752;	// "RGBAFPO"
	PCHAR	aBlessed	= PCHAR(&qwBlessed);
	PCHAR	pExt		= szExtension;

	ULONG	nBitsPerPixel = 0;
	ULONG	cchLayout = lstrlenA(aPixelLayout);
	for (ULONG i = 0; i < cchLayout; i++)
	{
		BOOL fConsumed = FALSE;

		// Retrieve the next character from aPixelLayout[].
		CHAR ch = aPixelLayout[i];

		// Loop to ingest each character in aPixelStructure[], except ...
		for (UINT j = 0; j < 4; j++)
		{
			// If this character is R, G, B, or A then it can only occur once.
			if (ch == aBlessed[j])
			{
				aBlessed[j] = 0;	// Remove it from the list of blessed characters.
				*pExt++ = ch;		// Append it to the filename extension (a work in progress).
				nBitsPerPixel += aPixelStructure[i];
				fConsumed = TRUE;
				break;
			}
		}

		// If this character is ASCII zero '0' (0x30)
		if (ch == 0x30)
		{
			// No component
			nBitsPerPixel += aPixelStructure[i];
			fConsumed = TRUE;
		}

		// If this character is 'P' or 'F' or anything else then it is unacceptable.
		if (!fConsumed)
		{
			BREAK_IF_DEBUG
			hr = S_FALSE;
			goto L_Exit;
		}
	}

	// Is nBitsPerPixel evenly divisible by 8?
	if (nBitsPerPixel & 0x00000003)
	{
		BREAK_IF_DEBUG
		hr = S_FALSE;
		goto L_Exit;
	}

	UINT32 cbBytesPerPixel = nBitsPerPixel >>= 3;
	UINT32 cbBytesPerFrame = cbBytesPerPixel * nStoredHeight * nStoredWidth;
	UINT64 cbMinBytesInPayload = rCE.qwDuration * cbBytesPerFrame;

	CharLowerA(szExtension);
	ULONG cchExt = lstrlenA(szExtension);
	if (cchExt == 4)
	{
		if (nBitsPerPixel == 32)
		{
			lstrcatA(szExtension, "32");
		}
		hr = S_OK;
		goto L_Confirm;
	}

	if (cchExt == 3)
	{
		if (nBitsPerPixel == 24)
		{
			lstrcatA(szExtension, "24");
		}
		hr = S_OK;
		goto L_Confirm;
	}

	if (nBitsPerPixel == 16)
	{
		if (DWORD(qwBitWidths) == 0x00050605)
		{
			lstrcatA(szExtension, "565");
			hr = S_OK;
			goto L_Confirm;
		}

		if (DWORD(qwBitWidths) == 0x00050505)
		{
			lstrcatA(szExtension, "555");
			hr = S_OK;
			goto L_Confirm;
		}
		hr = S_OK;
	}

L_Confirm:



		//// Replace the filename extension.
		//rCE.qwFileNameExt = MAKE_EXTENSION('mjpg');

		//rCE.fSelfContained    = FALSE;	// Not self-contained because it requires frame indexes.
		//rCE.fPayloadBigEndian = FALSE;
		//rCE.fPayloadConfirmed = TRUE;
		//hr = S_OK;

//		rCE.fSelfContained    = FALSE;	// Not self-contained because it requires frame indexes.
//		rCE.fPayloadBigEndian = FALSE;
//		rCE.fPayloadConfirmed = TRUE;
*/

//L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyIDAT_RLED() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyIDAT_RLED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyIDAT_RLED() when the compression is 'PRLE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	This combination might be for OMF1 only. ??? For OMF2 see IdentifyRLE__RLED_PRLE().
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyIDAT_RLED_PRLE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyJPEG_CDCI() when the compression is 'AVHD'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyJPEG_CDCI_AVHD(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	HRESULT hr = S_FALSE;
	if (rCE.qwFileNameExt = DetectAvidDNxHD(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = TRUE;
		rCE.fPayloadBigEndian = FALSE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
	}

	return hr;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyJPEG_CDCI() when the compression is 'DV/C'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyJPEG_CDCI_DV_C(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	HRESULT hr = S_FALSE;

	// Call DetectDV() to see is it's actually DV.
	if (rCE.qwFileNameExt = DetectDV(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = TRUE;
		rCE.fPayloadBigEndian = FALSE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
	}
	else
	{
		BREAK_IF_DEBUG
	}

	return hr;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyJPEG_CDCI() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyJPEG_CDCI_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyJPEG_JPED() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyJPEG_JPED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyJPEG_JPED() when the compression is 'JFIF'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyJPEG_JPED_JFIF(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	HRESULT hr = S_FALSE;
	
	// Call DetectJPEG() to see is it's actually JPEG.
	if (rCE.qwFileNameExt = DetectJPEG(pMem, cbMem, rCE.cbPayloadLength))
	{
		// Replace the filename extension.
		// Use *.mjpg (and not *.jpg) as our filename extension.
		rCE.qwFileNameExt = MAKE_EXTENSION('mjpg');

		rCE.fSelfContained    = FALSE;	// Not self-contained because it requires frame indexes.
		rCE.fPayloadBigEndian = FALSE;
		rCE.fPayloadConfirmed = TRUE;	// TRUE because DetectJPEG() succeeded.
		hr = S_OK;
	}
	else
	{
		BREAK_IF_DEBUG
	}

	return hr;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyMPEG_MPGI() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyMPEG_MPGI_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyMPEG_MPGI() when the compression is 'MPG2'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//
//	Prop:	OMFI:JPED:OffsetToFrameIndexes
//	Type:	omfi:Int64
//
//	Prop:	OMFI:MPGI:OffsetToFrameIndexes
//	Type:	omfi:Int64
//
//	Prop:	OMFI:MPGI:RandomAccess
//	Type:	omfi:Boolean
//	Value:	0x01
//
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyMPEG_MPGI_MPG2(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	HRESULT hr = S_FALSE;
	if (rCE.qwFileNameExt =	DetectMP2V(pMem, cbMem, rCE.cbPayloadLength))
	{
		rCE.fSelfContained    = FALSE;
		rCE.fPayloadBigEndian = FALSE;
		rCE.fPayloadConfirmed = TRUE;
		hr = S_OK;
	}
	else
	{
		BREAK_IF_DEBUG
	}

	return hr;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyRLE__RLED() when the compression is zero or 'NONE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyRLE__RLED_NONE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG
	return E_NOTIMPL;
}

//*********************************************************************************************************************
//	Called from CContainerLayer12::IdentifyRLE__RLED() when the compression is 'PRLE'.
//	The purpose of this routine is to come up with a filename extension that matches the payload,
//	and then pack it into a QWORD and save it in rCE.qwFileNameExt. If the filename extension has
//	fewer than eight bytes (and most do) then the QWORD is padded with zeros in the upper bytes.
//	Future versions of this routine might provide DirectShow media type guids and subtype guids.
//	On entry pMem points to a temporary memory bank holding the first cbMem bytes of the payload.
//	This combination might be for OMF2 only. ??? For OMF1 see IdentifyIDAT_RLED_PRLE().
//*********************************************************************************************************************
HRESULT CContainerLayer11::IdentifyRLE__RLED_PRLE(__inout MDAT_CACHE_ENTRY& rCE,
													__in const PBYTE pMem,
														__in const UINT32 cbMem)
{
	BREAK_IF_DEBUG		// Yes, this happens!
	return E_NOTIMPL;
}

