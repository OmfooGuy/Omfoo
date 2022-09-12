// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMediaData.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

#include "OmfMediaDataT.h"

//*********************************************************************************************************************
//	OMF Media Data Class (MDAT).
//*********************************************************************************************************************
class COmfMediaData : protected COmfMediaDataT<IOmfMediaData>
{
friend class CNewMdatFactory;
protected:
	COmfMediaData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfMediaData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// No aditional methods.
};

//*********************************************************************************************************************
//	AIFC Audio Data Class (AIFC).
//*********************************************************************************************************************
class COmfAifcData : protected COmfMediaDataT<IOmfAifcData>
{
friend class CNewMdatFactory;
protected:
	COmfAifcData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfAifcData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	Image Data Class (IDAT).
//*********************************************************************************************************************
class COmfIdatData : protected COmfMediaDataT<IOmfIdatData>
{
friend class CNewMdatFactory;
protected:
	COmfIdatData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfIdatData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	JPEG Image Data Class (JPEG).
//*********************************************************************************************************************
class COmfJpegData : protected COmfMediaDataT<IOmfJpegData>
{
friend class CNewMdatFactory;
protected:
	COmfJpegData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfJpegData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// INonDelegatingUnknown
	//*****************************************************************************************************************
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfJpegData))||
				(riid == __uuidof(IOmfIdatData))||
				(riid == __uuidof(IOmfMediaData)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfJpegData*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:JPEG:FrameIndex property as an array of UINT32s.
	// The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)
	{
		return OrdReadPosition32Array(ePropJpegFrameIndex, nMaxElements, pArrayBase, pnActualElements);
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:JPEG:FrameIndexExt property as an array of UINT64s.
	// The data type must be Position32Array or omfi:Position64Array or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)
	{
		HRESULT hr = OrdReadPosition64Array(ePropJpegFrameIndexExt, nMaxElements, pArrayBase, pnActualElements);
		if (FAILED(hr))
		{
			hr = OrdReadPosition64Array(ePropJpegFrameIndex, nMaxElements, pArrayBase, pnActualElements);
		}
		return hr;
	}
};

//*********************************************************************************************************************
//	MPEG Image Data Class (MPEG)
//*********************************************************************************************************************
class COmfMpegData : protected COmfMediaDataT<IOmfMpegData>
{
friend class CNewMdatFactory;
protected:
	COmfMpegData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfMpegData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// INonDelegatingUnknown
	//*****************************************************************************************************************
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfMpegData))||
				(riid == __uuidof(IOmfIdatData))||
				(riid == __uuidof(IOmfMediaData)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfMpegData*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:MPEG:FrameIndex property as an array of UINT32s.
	// The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)
	{
		return OrdReadPosition32Array(ePropMpegFrameIndex, nMaxElements, pArrayBase, pnActualElements);
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:MPEG:MPEGFrameIndex property as an array of UINT64s.
	// The data type must be omfi:Position32Array or omfi:Position64Array or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)
	{
		HRESULT hr = OrdReadPosition64Array(ePropMpegMPEGFrameIndex, nMaxElements, pArrayBase, pnActualElements);
		if (FAILED(hr))
		{
			hr = OrdReadPosition64Array(ePropMpegFrameIndex, nMaxElements, pArrayBase, pnActualElements);
		}
		return hr;
	}
};

//*********************************************************************************************************************
//	RLE Compressed Image Data (RLE ).
//*********************************************************************************************************************
class COmfRleiData : protected COmfMediaDataT<IOmfRleiData>
{
friend class CNewMdatFactory;
protected:
	COmfRleiData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfRleiData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// INonDelegatingUnknown
	//*****************************************************************************************************************
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfRleiData))||
				(riid == __uuidof(IOmfIdatData))||
				(riid == __uuidof(IOmfMediaData)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfRleiData*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:RLED:FrameIndex property as an array of UINT32s.
	// The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)
	{
		return OrdReadPosition32Array(ePropRledFrameIndex, nMaxElements, pArrayBase, pnActualElements);
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:RLED:FrameIndex property as an array of UINT64s.
	// The data type must be omfi:Position32Array or omfi:Position64Array, or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)
	{
		return OrdReadPosition64Array(ePropRledFrameIndex, nMaxElements, pArrayBase, pnActualElements);
	}
};

//*********************************************************************************************************************
//	Sound Designer II Audio Data (SD2M).
//*********************************************************************************************************************
class COmfSd2fData : protected COmfMediaDataT<IOmfSd2fData>
{
friend class CNewMdatFactory;
protected:
	COmfSd2fData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfSd2fData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	TIFF Image Data Class (TIFF).
//*********************************************************************************************************************
class COmfTiffData : protected COmfMediaDataT<IOmfTiffData>
{
friend class CNewMdatFactory;
protected:
	COmfTiffData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfTiffData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	WAVE Audio Data Class (WAVE).
//*********************************************************************************************************************
class COmfWaveData : protected COmfMediaDataT<IOmfWaveData>
{
friend class CNewMdatFactory;
protected:
	COmfWaveData(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDataT<IOmfWaveData>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};
