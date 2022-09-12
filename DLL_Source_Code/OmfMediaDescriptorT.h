// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMediaDescriptorT.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfMediaDescriptor.
//*********************************************************************************************************************
template <class TBase = IOmfMediaDescriptor>
class __declspec(novtable) COmfMediaDescriptorT : protected COmfObject, protected TBase
{
protected:
	POMFOO_OBJREF_ARRAY	m_pLocrArray;
	HRESULT				m_hrLocrArray;
	ULONG				m_idx;
	DWORD				m_dwMDAT;

	COmfMediaDescriptorT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfObject(rBlop, pContainer, pParent, pNewReserved)
		, m_pLocrArray(NULL)
		, m_hrLocrArray(S_FALSE)
		, m_idx(-1)
		, m_dwMDAT(0)
	{
		// This will only succeed when the data is embedded in this OMF file.
		// Otherwise it returns -1.
	//	m_idx = m_pContainer->GetMdesTableIndex(this->dwObject);

		// Get the Bento object ID of our corresponding MDAT.
		// This will only succeed when the data is embedded in this OMF file.
		// Otherwise it returns 0.
	//	m_dwMDAT = m_pContainer->GetMdatMediaDataObject(m_idx);
	}

	virtual ~COmfMediaDescriptorT(void)
	{
		FreeObjRefArray(&m_pLocrArray);
	}

	//*****************************************************************************************************************
	// INonDelegatingUnknown
	//*****************************************************************************************************************
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfMediaDescriptor))||
				(riid == __uuidof(TBase)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<TBase*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// IUnknown
	//*****************************************************************************************************************
	STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppvOut)
	{
		return NonDelegatingQueryInterface(riid, ppvOut);
	}
	STDMETHODIMP_(ULONG) AddRef()
	{
		return NonDelegatingAddRef();
	}
	STDMETHODIMP_(ULONG) Release()
	{
		return NonDelegatingRelease();
	}

	//*****************************************************************************************************************
	// OMFI:MDES:MobKind, omfi:PhysicalMobType
	//*****************************************************************************************************************
	STDMETHODIMP GetPhysicalMobKind(__out PWORD pwMobKind)
	{
		return OrdReadUInt16(ePropMdesMobKind, pwMobKind);
	}

	//*****************************************************************************************************************
	// OMFI:MDES:KitCodecID, omfi:String - as a wide character [UTF-16] string.
	//*****************************************************************************************************************
	STDMETHODIMP GetKitCodecID(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
	{
		return OrdReadStringW(ePropMdesKitCodecID, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// OMFI:MDES:KitCodecDesc, omfi:String - as a wide character [UTF-16] string.
	//*****************************************************************************************************************
	STDMETHODIMP GetKitCodecDesc(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
	{
		return OrdReadStringW(ePropMdesKitCodecDesc, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// Creates an iterator to instantiate the objects in the OMFI:MDES:Locator array.
	//*****************************************************************************************************************
	STDMETHODIMP IterateLocators(__out IOmfooIterator **ppIterator)
	{
		if (m_hrLocrArray == S_FALSE)
		{
			// OMFI:MDES:Locator
			m_hrLocrArray = OrdAllocObjRefArray(ePropMdesLocator, &m_pLocrArray);
		}

		if (m_pLocrArray)
		{
			return WrapObjRefArray(m_pLocrArray, ppIterator);
		}
		else
		{
			return m_hrLocrArray;
		}
	}
};

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfFileDescriptor.
//*********************************************************************************************************************
template <class TBase = IOmfFileDescriptor>
class __declspec(novtable) COmfFileDescriptorT : protected COmfMediaDescriptorT<TBase>
{
protected:
	COmfFileDescriptorT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDescriptorT<TBase>(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfMediaDescriptor))||
				(riid == __uuidof(IOmfFileDescriptor))||
				(riid == __uuidof(TBase)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<TBase*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// Returns S_OK if the OMFI:MDFL:IsOMFI property is TRUE, or S_FALSE if it is FALSE.
	//*****************************************************************************************************************
	STDMETHODIMP IsOMFI(void)
	{
		return OrdReadBoolean(ePropMdflIsOMFI, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:MDFL:SampleRate, omfi:Rational
	//*****************************************************************************************************************
	STDMETHODIMP GetSampleRate(__out POMF_RATIONAL pSampleRate)
	{
		return OrdReadRational(ePropMdflSampleRate, pSampleRate);
	}
	STDMETHODIMP GetSampleRateAsFloat(__out PFLOAT pFloat)
	{
		return OrdReadRationalAsFloat(ePropMdflSampleRate, pFloat);
	}
	STDMETHODIMP GetSampleRateAsDouble(__out DOUBLE* pDouble)
	{
		return OrdReadRationalAsDouble(ePropMdflSampleRate, pDouble);
	}

	//*****************************************************************************************************************
	// OMFI:MDFL:Length
	//*****************************************************************************************************************
	STDMETHODIMP GetLength(__out PUINT64 pLength)
	{
		return OrdReadLength64(ePropMdflLength, pLength);
	}

	//*****************************************************************************************************************
	// OMFI:MDFL:dataOffset
	//*****************************************************************************************************************
	STDMETHODIMP GetDataOffset(__out PUINT64 pcbOffset)
	{
		return OrdReadDataOffset64(ePropMdfldataOffset, pcbOffset);
	}
};

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfDiddDescriptor.
//*********************************************************************************************************************
template <class TBase = IOmfDiddDescriptor>
class __declspec(novtable) COmfDiddDescriptorT : protected COmfFileDescriptorT<TBase>
{
protected:
	COmfDiddDescriptorT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfFileDescriptorT<TBase>(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfMediaDescriptor))||
				(riid == __uuidof(IOmfFileDescriptor))||
				(riid == __uuidof(IOmfDiddDescriptor))||
				(riid == __uuidof(TBase)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<TBase*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DIDCompressMethod, omfi:ObjectTag
	//*****************************************************************************************************************
	STDMETHODIMP GetDIDCompressMethod(__out PDWORD pdwCompressionFourCC)
	{
		return OrdReadFourCC(ePropDiddDIDCompressMethod, pdwCompressionFourCC);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:Compression, omfi:String - as a wide character [UTF-16] string.
	//*****************************************************************************************************************
	STDMETHODIMP GetCompression(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
	{
		return OrdReadStringW(ePropDiddCompression, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DIDResolutionID, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetDIDResolutionID(__out PDWORD pdwResolutionID)
	{
		return OrdReadUInt32(ePropDiddDIDResolutionID, PUINT32(pdwResolutionID));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:Uniformness, omfi:Boolean
	// Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	//*****************************************************************************************************************
	STDMETHODIMP IsUniform(void)
	{
		return OrdReadBoolean(ePropDiddUniformness, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:StoredHeight, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetStoredHeight(__out PULONG pnStoredHeight)
	{
		return OrdReadUInt32(ePropDiddStoredHeight, PUINT32(pnStoredHeight));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:StoredWidth, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetStoredWidth(__out PULONG pnStoredWidth)
	{
		return OrdReadUInt32(ePropDiddStoredWidth, PUINT32(pnStoredWidth));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:SampledHeight, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetSampledHeight(__out PULONG pnSampledHeight)
	{
		return OrdReadUInt32(ePropDiddSampledHeight, PUINT32(pnSampledHeight));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:SampledWidth, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetSampledWidth(__out PULONG pnSampledWidth)
	{
		return OrdReadUInt32(ePropDiddSampledWidth, PUINT32(pnSampledWidth));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:SampledXOffset, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetSampledXOffset(__out PLONG pnXOffset)
	{
		return OrdReadUInt32(ePropDiddSampledXOffset, PUINT32(pnXOffset));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:SampledYOffset, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetSampledYOffset(__out PLONG pnYOffset)
	{
		return OrdReadUInt32(ePropDiddSampledYOffset, PUINT32(pnYOffset));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DisplayHeight, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetDisplayHeight(__out PULONG pnHeight)
	{
		return OrdReadUInt32(ePropDiddDisplayHeight, PUINT32(pnHeight));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DisplayWidth, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetDisplayWidth(__out PLONG pnWidth)
	{
		return OrdReadUInt32(ePropDiddDisplayWidth, PUINT32(pnWidth));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DisplayXOffset, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetDisplayXOffset(__out PLONG pnXOffset)
	{
		return OrdReadUInt32(ePropDiddDisplayXOffset, PUINT32(pnXOffset));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DisplayYOffset, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetDisplayYOffset(__out PLONG pnYOffset)
	{
		return OrdReadUInt32(ePropDiddDisplayYOffset, PUINT32(pnYOffset));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:FrameLayout, omfi:LayoutType (two bytes)
	//*****************************************************************************************************************
	STDMETHODIMP GetFrameLayout(__out PWORD pwLayoutType)
	{
		return OrdReadUInt16(ePropDiddFrameLayout, pwLayoutType);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:VideoLineMap, omfi:Int32Array
	//*****************************************************************************************************************
	STDMETHODIMP GetVideoLineMap(__in ULONG nMaxElements,
									__out_opt PUINT32 pArrayBase,
										__out PULONG pnActualElements)
	{
		return OrdReadInt32Array(ePropDiddVideoLineMap, nMaxElements, pArrayBase, pnActualElements);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:ImageAspectRatio, omfi:Rational
	//*****************************************************************************************************************
	STDMETHODIMP GetImageAspectRatio(__out POMF_RATIONAL pAspectRatio)
	{
		return OrdReadRational(ePropDiddImageAspectRatio, pAspectRatio);
	}
	STDMETHODIMP GetImageAspectRatioAsFloat(__out PFLOAT pFloat)
	{
		return OrdReadRationalAsFloat(ePropDiddImageAspectRatio, pFloat);
	}
	STDMETHODIMP GetImageAspectRatioAsDouble(__out DOUBLE* pDouble)
	{
		return OrdReadRationalAsDouble(ePropDiddImageAspectRatio, pDouble);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:AlphaTransparency, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetAlphaTransparency(__out PULONG pnAlphaValue)
	{
		return OrdReadUInt32(ePropDiddAlphaTransparency, PUINT32(pnAlphaValue));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:ImageAlignmentFactor, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetAlignmentFactor(__out PULONG pnAlignment)
	{
		return OrdReadUInt32(ePropDiddImageAlignmentFactor, PUINT32(pnAlignment));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:Gamma, omfi:Rational
	//*****************************************************************************************************************
	STDMETHODIMP GetGamma(__out POMF_RATIONAL pGamma)
	{
		return OrdReadRational(ePropDiddGamma, pGamma);
	}
	STDMETHODIMP GetGammaAsFloat(__out PFLOAT pFloat)
	{
		return OrdReadRationalAsFloat(ePropDiddGamma, pFloat);
	}
	STDMETHODIMP GetGammaAsDouble(__out DOUBLE* pDouble)
	{
		return OrdReadRationalAsDouble(ePropDiddGamma, pDouble);
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:ClientFillStart, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetClientFillStart(__out PULONG pnFillStart)
	{
		return OrdReadUInt32(ePropDiddClientFillStart, PUINT32(pnFillStart));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:ClientFillEnd, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetClientFillEnd(__out PULONG pnFillEnd)
	{
		return OrdReadUInt32(ePropDiddClientFillEnd, PUINT32(pnFillEnd));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DataOffset, omfi:Int32, ???
	//*****************************************************************************************************************
	STDMETHODIMP GetDataOffset(__out PUINT64 pcbOffset)
	{
		HRESULT hr = OrdReadDataOffset64(ePropDiddDataOffset, pcbOffset);
		if (FAILED(hr))
		{
			// If OMFI:DIDD:DataOffset is absent try OMFI:MDFL:dataOffset.
			hr = OrdReadDataOffset64(ePropMdfldataOffset, pcbOffset);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:OffsetToRLEFrameIndexes, ???, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetOffsetToRLEFrameIndexes(__out PULONG pcbFrameIndexOffset)
	{
		return OrdReadUInt32(ePropDiddOffsetToRLEFrameIndexes, PUINT32(pcbFrameIndexOffset));
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:FrameIndexByteOrder, omfi:Short, omfi:Int16
	// Returns S_OK if the value is big-endian Motorola 'MM' (0x4D4D)
	// Returns S_FALSE if the value is little-endian Intel 'II' (0x4949)
	// Returns OMF_E_BAD_OMF_BYTE_ORDER if the value is anything else.
	//*****************************************************************************************************************
	STDMETHODIMP IsFrameIndexBigEndian(void)
	{
		WORD wByteOrder = 0;
		HRESULT hr = OrdReadUInt16(ePropDiddFrameIndexByteOrder, &wByteOrder);
		if (SUCCEEDED(hr))
		{
			if (wByteOrder == 0x4D4D)
			{
				hr = S_OK;
			}
			else if (wByteOrder == 0x4949)
			{
				hr = S_FALSE;
			}
			else
			{
				hr = OMF_E_BAD_PROP_BYTE_ORDER;
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:FirstFrameOffset aka OMFI:DIDD:DIDDFirstFrameOffset, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetFirstFrameOffset(__out PULONG pcbFirstFrameOffset)
	{
		HRESULT hr = OrdReadUInt32(ePropDiddFirstFrameOffset, PUINT32(pcbFirstFrameOffset));
		if (FAILED(hr))
		{
			hr = OrdReadUInt32(ePropDiddDIDDFirstFrameOffset, PUINT32(pcbFirstFrameOffset));
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:FrameSampleSize aka OMFI:DIDD:DIDDFrameSampleSize, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetFrameSampleSize(__out PULONG pcbSampleSize)
	{
		HRESULT hr = OrdReadUInt32(ePropDiddFrameSampleSize, PUINT32(pcbSampleSize));
		if (FAILED(hr))
		{
			hr = OrdReadUInt32(ePropDiddDIDDFrameSampleSize, PUINT32(pcbSampleSize));
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:DIDDImageSize aka OMFI:DIDD:DIDImageSize, omfi:Int32, omfi:Long
	//*****************************************************************************************************************
	STDMETHODIMP GetDIDDImageSize(__out PULONG pcbImageSize)
	{
		HRESULT hr = OrdReadUInt32(ePropDiddDIDImageSize, PUINT32(pcbImageSize));
		if (FAILED(hr))
		{
			hr = OrdReadUInt32(ePropDiddDIDDImageSize, PUINT32(pcbImageSize));
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:DIDD:NextDIDDesc aka OMFI:DIDD:NextDIDDescriptor
	//*****************************************************************************************************************
	STDMETHODIMP NextDIDDescriptor(__in REFIID riid, __out PVOID *ppvOut)
	{
		HRESULT hr = OrdInstantiateObjRef(ePropDiddNextDIDDescriptor, riid, ppvOut);
		if (FAILED(hr))
		{
			hr = OrdInstantiateObjRef(ePropDiddNextDIDDesc, riid, ppvOut);
		}
		return hr;
	}
};

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfCdciDescriptor.
//*********************************************************************************************************************
template <class TBase = IOmfCdciDescriptor>
class __declspec(novtable) COmfCdciDescriptorT : protected COmfDiddDescriptorT<TBase>
{
protected:
	COmfCdciDescriptorT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfDiddDescriptorT<TBase>(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfMediaDescriptor))||
				(riid == __uuidof(IOmfFileDescriptor))||
				(riid == __uuidof(IOmfDiddDescriptor))||
				(riid == __uuidof(IOmfCdciDescriptor))||
				(riid == __uuidof(TBase)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<TBase*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:ComponentWidth, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetComponentWidth(__out PULONG pnWidth)
	{
		return OrdReadUInt32(ePropCdciComponentWidth, PUINT32(pnWidth));
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:HorizontalSubsampling, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetHorizSubsampling(__out PULONG pnHorizSubsampling)
	{
		return OrdReadUInt32(ePropCdciHorizontalSubsampling, PUINT32(pnHorizSubsampling));
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:VerticalSubsampling, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetVertSubsampling(__out PULONG pnVertSubsampling)
	{
		return OrdReadUInt32(ePropCdciVerticalSubsampling, PUINT32(pnVertSubsampling));
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:ColorSiting, omfi:ColorSitingType (16 bits) 
	//*****************************************************************************************************************
	STDMETHODIMP GetColorSiting(__out PWORD pwColorSiting)
	{
		return OrdReadUInt16(ePropCdciColorSiting, pwColorSiting);
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:BlackReferenceLevel, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetBlackReferenceLevel(__out PULONG pnBlackLevel)
	{
		return OrdReadUInt32(ePropCdciBlackReferenceLevel, PUINT32(pnBlackLevel));
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:WhiteReferenceLevel, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetWhiteReferenceLevel(__out PULONG pnWhiteLevel)
	{
		return OrdReadUInt32(ePropCdciWhiteReferenceLevel, PUINT32(pnWhiteLevel));
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:ColorRange, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetColorRange(__out PULONG pnColorRange)
	{
		return OrdReadUInt32(ePropCdciColorRange, PUINT32(pnColorRange));
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:PaddingBits, omfi:UInt16
	//*****************************************************************************************************************
	STDMETHODIMP GetPaddingBits(__out PWORD pwPaddingBits)
	{
		return OrdReadUInt16(ePropCdciPaddingBits, pwPaddingBits);
	}

	//*****************************************************************************************************************
	// OMFI:CDCI:OffsetToFrameIndexes, omfi:UInt32, omfi:UInt64
	//*****************************************************************************************************************
	STDMETHODIMP GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)
	{
		return OrdReadDataOffset64(ePropCdciOffsetToFrameIndexes, pcbOffset);
	}
};

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfRgbaDescriptor.
//*********************************************************************************************************************
template <class TBase = IOmfRgbaDescriptor>
class __declspec(novtable) COmfRgbaDescriptorT : protected COmfDiddDescriptorT<TBase>
{
protected:
	COmfRgbaDescriptorT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfDiddDescriptorT<TBase>(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfMediaDescriptor))||
				(riid == __uuidof(IOmfFileDescriptor))||
				(riid == __uuidof(IOmfDiddDescriptor))||
				(riid == __uuidof(IOmfRgbaDescriptor))||
				(riid == __uuidof(TBase)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<TBase*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:RGBA:Palette, omfi:DataValue
	//*****************************************************************************************************************
	STDMETHODIMP GetPaletteAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropRgbaPalette, cbBuffer, pBuffer, pcbRequired);
	}

	//*****************************************************************************************************************
	// OMFI:RGBA:PaletteLayout, omfi:CompCodeArray
	//*****************************************************************************************************************
	STDMETHODIMP GetPaletteLayout(__out POMF_COMP_CODE_ARRAY pResult)
	{
		return OrdReadCompCodeArray(ePropRgbaPaletteLayout, pResult);
	}

	//*****************************************************************************************************************
	// OMFI:RGBA:PaletteStructure, omfi:CompSizeArray
	//*****************************************************************************************************************
	STDMETHODIMP GetPaletteStructure(__out POMF_COMP_SIZE_ARRAY pResult)
	{
		return OrdReadCompSizeArray(ePropRgbaPaletteStructure, pResult);
	}

	//*****************************************************************************************************************
	// OMFI:RGBA:PixelLayout, omfi:CompCodeArray
	//*****************************************************************************************************************
	STDMETHODIMP GetPixelLayout(__out POMF_COMP_CODE_ARRAY pResult)
	{
		return OrdReadCompCodeArray(ePropRgbaPixelLayout, pResult);
	}

	//*****************************************************************************************************************
	// OMFI:RGBA:PixelStructure, omfi:CompSizeArray
	//*****************************************************************************************************************
	STDMETHODIMP GetPixelStructure(__out POMF_COMP_SIZE_ARRAY pResult)
	{
		return OrdReadCompSizeArray(ePropRgbaPixelStructure, pResult);
	}
};

