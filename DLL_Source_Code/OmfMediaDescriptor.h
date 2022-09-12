// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMediaDescriptor.h
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

#include "OmfMediaDescriptorT.h"

//*********************************************************************************************************************
//	OMF Media Descriptor Class (MDES).
//*********************************************************************************************************************
class COmfMediaDescriptor : protected COmfMediaDescriptorT<IOmfMediaDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfMediaDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDescriptorT<IOmfMediaDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF General Media File escriptor Class (GMFL).
//*********************************************************************************************************************
class COmfGmflDescriptor : protected COmfMediaDescriptorT<IOmfGmflDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfGmflDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDescriptorT<IOmfGmflDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Media Film Descriptor Class (MDFM).
//*********************************************************************************************************************
class COmfFilmDescriptor : protected COmfMediaDescriptorT<IOmfFilmDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfFilmDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDescriptorT<IOmfFilmDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:FilmAspectRatio, omfi:Rational
	//*****************************************************************************************************************
	STDMETHODIMP GetAspectRatio(POMF_RATIONAL pAspectRatio)
	{
		return OrdReadRational(ePropMdfmFilmAspectRatio, pAspectRatio);
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:FilmAspectRatio, omfi:Rational
	//*****************************************************************************************************************
	STDMETHODIMP GetAspectRatioAsFloat(PFLOAT pFloat)
	{
		return OrdReadRationalAsFloat(ePropMdfmFilmAspectRatio, pFloat);
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:FilmAspectRatio, omfi:Rational
	//*****************************************************************************************************************
	STDMETHODIMP GetAspectRatioAsDouble(DOUBLE* pDouble)
	{
		return OrdReadRationalAsDouble(ePropMdfmFilmAspectRatio, pDouble);
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:FilmFormat, omfi:FilmType
	//*****************************************************************************************************************
	STDMETHODIMP GetFilmFormat(PWORD pwFormat)
	{
		return OrdReadUInt16(ePropMdfmFilmFormat, pwFormat);
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:FrameRate, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetFrameRate(PULONG pnFrameRate)
	{
		return OrdReadUInt32(ePropMdfmFrameRate, PUINT32(pnFrameRate));
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:Manufacturer, omfi:String
	//*****************************************************************************************************************
	STDMETHODIMP GetManufacturer(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
	{
		return OrdReadStringW(ePropMdfmManufacturer, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:Model, omfi:String
	//*****************************************************************************************************************
	STDMETHODIMP GetModel(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
	{
		return OrdReadStringW(ePropMdfmModel, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// OMFI:MDFM:PerforationsPerFrame, omfi:UInt8
	//*****************************************************************************************************************
	STDMETHODIMP GetPerforationsPerFrame(PBYTE pbPerforations)
	{
		return OrdReadUInt8(ePropMdfmPerforationsPerFrame, pbPerforations);
	}
};

//*********************************************************************************************************************
//	OMF Media Tape Descriptor Class (MDTP).
//*********************************************************************************************************************
class COmfTapeDescriptor : protected COmfMediaDescriptorT<IOmfTapeDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfTapeDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfMediaDescriptorT<IOmfTapeDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:MDTP:FormFactor, omfi:TapeCaseType
	//*****************************************************************************************************************
	STDMETHODIMP GetFormFactor(__out PWORD pwFormFactor)
	{
		return OrdReadUInt16(ePropMdtpFormFactor, pwFormFactor);
	}

	//*****************************************************************************************************************
	// OMFI:MDTP:Length, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetLengthInMinutes(__out PULONG pnLength)
	{
		return OrdReadUInt32(ePropMdtpLength, PUINT32(pnLength));
	}

	//*****************************************************************************************************************
	// OMFI:MDTP:Manufacturer, omfi:String
	//*****************************************************************************************************************
	STDMETHODIMP GetManufacturer(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
	{
		return OrdReadStringW(ePropMdtpManufacturer, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// OMFI:MDTP:Model, omfi:String
	//*****************************************************************************************************************
	STDMETHODIMP GetModel(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
	{
		return OrdReadStringW(ePropMdtpModel, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// OMFI:MDTP:TapeFormat, omfi:TapeFormatType
	//*****************************************************************************************************************
	STDMETHODIMP GetTapeFormat(__out PWORD pwTapeFormat)
	{
		return OrdReadUInt16(ePropMdtpTapeFormat, pwTapeFormat);
	}

	//*****************************************************************************************************************
	// OMFI:MDTP:VideoSignal, omfi:VideoSignalType
	//*****************************************************************************************************************
	STDMETHODIMP GetVideoSignal(__out PWORD pwVideoSignal)
	{
		return OrdReadUInt16(ePropMdtpVideoSignal, pwVideoSignal);
	}
};

//*********************************************************************************************************************
//	OMF Media File Descriptor Class (MDFL).
//	All IOmfFileDescriptor methods are implemented in COmfFileDescriptorT.
//*********************************************************************************************************************
class COmfFileDescriptor : protected COmfFileDescriptorT<IOmfFileDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfFileDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfFileDescriptorT<IOmfFileDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF AIFC Audio Descriptor Class (AIFD).
//*********************************************************************************************************************
class COmfAifcDescriptor : protected COmfFileDescriptorT<IOmfAifcDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfAifcDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfFileDescriptorT<IOmfAifcDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:AIFD:Summary, omfi:DataValue or omfi:VarLenBytes
	//*****************************************************************************************************************
	STDMETHODIMP GetSummaryAsRawBytes(__in ULONG cbBuffer, __out_opt PVOID pBuffer, __out PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropAifdSummary, cbBuffer, pBuffer, pcbRequired);
	}
};

//*********************************************************************************************************************
//	Sound Designer II Audio Descriptor Class (SD2D).
//*********************************************************************************************************************
class COmfSd2fDescriptor : protected COmfFileDescriptorT<IOmfSd2fDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfSd2fDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfFileDescriptorT<IOmfSd2fDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:SD2D:BitsPerSample, omf:UInt16
	//*****************************************************************************************************************
	STDMETHODIMP GetBitsPerSample(__out PWORD pwBitsPerSample)
	{
		return OrdReadUInt16(ePropSd2dBitsPerSample, pwBitsPerSample);
	}

	//*****************************************************************************************************************
	// OMFI:SD2D:NumOfChannels, omf:UInt16
	//*****************************************************************************************************************
	STDMETHODIMP GetNumChannels(__out PWORD pwChannels)
	{
		return OrdReadUInt16(ePropSd2dNumOfChannels, pwChannels);
	}
};

//*********************************************************************************************************************
//	OMF TIFF Image Descriptor Class (TIFD).
//*********************************************************************************************************************
class COmfTiffDescriptor : protected COmfFileDescriptorT<IOmfTiffDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfTiffDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfFileDescriptorT<IOmfTiffDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:IsContiguous, omfi:Boolean
	//*****************************************************************************************************************
	STDMETHODIMP IsContiguous(void)
	{
		return OrdReadBoolean(ePropTifdIsContiguous, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:IsUniform, omfi:Boolean
	//*****************************************************************************************************************
	STDMETHODIMP IsUniform(void)
	{
		return OrdReadBoolean(ePropTifdIsUniform, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:LeadingLines, omfi:Int32
	//*****************************************************************************************************************
	STDMETHODIMP GetLeadingLines(__out PULONG pnLeadingLines)
	{
		return OrdReadUInt32(ePropTifdLeadingLines, PUINT32(pnLeadingLines));
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:TrailingLines, omfi:Int32
	//*****************************************************************************************************************
	STDMETHODIMP GetTrailingLinesLines(__out PULONG pnTrailingLines)
	{
		return OrdReadUInt32(ePropTifdTrailingLines, PUINT32(pnTrailingLines));
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:JPEGTableID, omfi:JPEGTableIDType (a four byte value that needs byte-swapped when necessary)
	//*****************************************************************************************************************
	STDMETHODIMP GetJPEGTableID(__out PDWORD pdwTableID)
	{
		return OrdReadUInt32(ePropTifdJPEGTableID, PUINT32(pdwTableID));
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:FP16QTables, omfi:DataValue or omfi:VarLenBytes
	//*****************************************************************************************************************
	STDMETHODIMP GetFP16QTablesAsRawBytes(__in ULONG cbBuffer,
											__out_opt PVOID pBuffer,
												__out PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropTifdFP16QTables, cbBuffer, pBuffer, pcbRequired);
	}

	//*****************************************************************************************************************
	// OMFI:TIFD:Summary, omfi:DataValue or omfi:VarLenBytes
	//*****************************************************************************************************************
	STDMETHODIMP GetSummaryAsRawBytes(__in ULONG cbBuffer, __out_opt PVOID pBuffer, __out PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropTifdSummary, cbBuffer, pBuffer, pcbRequired);
	}
};

//*********************************************************************************************************************
//	OMF WAVE Audio Descriptor Class (WAVD). 
//*********************************************************************************************************************
class COmfWaveDescriptor : protected COmfFileDescriptorT<IOmfWaveDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfWaveDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfFileDescriptorT<IOmfWaveDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:WAVD:Summary, omfi:DataValue or omfi:VarLenBytes
	//*****************************************************************************************************************
	STDMETHODIMP GetSummaryAsRawBytes(__in ULONG cbBuffer, __out_opt PVOID pBuffer, __out_opt PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropWavdSummary, cbBuffer, pBuffer, pcbRequired);
	}
};

//*********************************************************************************************************************
//	OMF Digital Image Descriptor Class (DIDD).
//*********************************************************************************************************************
class COmfDiddDescriptor : protected COmfDiddDescriptorT<IOmfDiddDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfDiddDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfDiddDescriptorT<IOmfDiddDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Color Difference Component Image Descriptor Class (CDCI).
//*********************************************************************************************************************
class COmfCdciDescriptor : protected COmfCdciDescriptorT<IOmfCdciDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfCdciDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfCdciDescriptorT<IOmfCdciDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF RGBA Component Image Descriptor Class (RGBA).
//*********************************************************************************************************************
class COmfRgbaDescriptor : protected COmfRgbaDescriptorT<IOmfRgbaDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfRgbaDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfRgbaDescriptorT<IOmfRgbaDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	JPED Image Descriptor Class (JPED).
//*********************************************************************************************************************
class COmfJpegDescriptor : protected COmfCdciDescriptorT<IOmfJpegDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfJpegDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfCdciDescriptorT<IOmfJpegDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:JPED:JPEDImageStartAlignment, omfi:Int32
	//*****************************************************************************************************************
	STDMETHODIMP GetJPEDImageStartAlignment(__out PULONG pnAlignment)
	{
		return OrdReadUInt32(ePropJpedJPEDImageStartAlignment, PUINT32(pnAlignment));
	}

	//*****************************************************************************************************************
	// OMFI:JPED:JPEGTag, omfi:Int32
	//*****************************************************************************************************************
	STDMETHODIMP GetJPEGTag(__out PDWORD pdwJpegTag)
	{
		return OrdReadUInt32(ePropJpedJPEGTag, PUINT32(pdwJpegTag));
	}

	//*****************************************************************************************************************
	// OMFI:JPED:OffsetToFrameIndexes, omfi:Int64
	//*****************************************************************************************************************
	STDMETHODIMP GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)
	{
		HRESULT hr = OrdReadDataOffset64(ePropJpedOffsetToFrameIndexes, pcbOffset);
		if (FAILED(hr))
		{
			// JPED inherits CDCI, so try this as a contingency.
			// OMFI:CDCI:OffsetToFrameIndexes, omfi:UInt32
			hr = OrdReadDataOffset64(ePropCdciOffsetToFrameIndexes, pcbOffset);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:JPED:QntznTblLen, omfi:Int32
	//*****************************************************************************************************************
	STDMETHODIMP GetQntznTblLen(__out PULONG pcbTableLength)
	{
		return OrdReadUInt32(ePropJpedQntznTblLen, PUINT32(pcbTableLength));
	}

	//*****************************************************************************************************************
	// OMF1 - OMFI:JPED:QuantizationTables, omfi:VarLenBytes
	// OMF2 - OMFI:JPED:QntznTbl, omfi:DataValue
	//*****************************************************************************************************************
	STDMETHODIMP GetQntznTblAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)
	{
		HRESULT hr = OrdReadDataValueAsRawBytes(ePropJpedQntznTbl, cbBuffer, pBuffer, pcbRequired);
		if (FAILED(hr))
		{
			hr = OrdReadDataValueAsRawBytes(ePropJpedQuantizationTables, cbBuffer, pBuffer, pcbRequired);
		}
		return hr;
	}
};

//*********************************************************************************************************************
//	MPEG Bitstream Image Descriptor Class (MPGI).
//*********************************************************************************************************************
class COmfMpegDescriptor : protected COmfCdciDescriptorT<IOmfMpegDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfMpegDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfCdciDescriptorT<IOmfMpegDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:GOPStructure, omfi:Uchar
	//*****************************************************************************************************************
	STDMETHODIMP GetGOPStructure(__out PBYTE pbResult)
	{
		return OrdReadUInt8(ePropMpgiGOPStructure, pbResult);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:isMPEG1, omfi:Boolean
	// Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	//*****************************************************************************************************************
	STDMETHODIMP IsMPEG1(void)
	{
		return OrdReadBoolean(ePropMpgiisMPEG1, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:LeadingDiscard, omfi:Boolean
	// Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	//*****************************************************************************************************************
	STDMETHODIMP IsLeadingDiscard(void)
	{
		return OrdReadBoolean(ePropMpgiLeadingDiscard, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:TrailingDiscard, omfi:Boolean
	// Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	//*****************************************************************************************************************
	STDMETHODIMP IsTrailingDiscard(void)
	{
		return OrdReadBoolean(ePropMpgiTrailingDiscard, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:MPEGStreamType, omfi:Uchar
	//*****************************************************************************************************************
	STDMETHODIMP GetMPEGStreamType(__out PBYTE pbType)
	{
		return OrdReadUInt8(ePropMpgiMPEGStreamType, pbType);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:MPEGVersion, omfi:Uchar
	//*****************************************************************************************************************
	STDMETHODIMP GetMPEGVersion(__out PBYTE pbVersion)
	{
		return OrdReadUInt8(ePropMpgiMPEGVersion, pbVersion);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:OffsetToFrameIndexes, omfi:Int64
	//*****************************************************************************************************************
	STDMETHODIMP GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)
	{
		HRESULT hr = OrdReadDataOffset64(ePropMpgiOffsetToFrameIndexes, pcbOffset);
		if (FAILED(hr))
		{
			// MPGI inherits CDCI, so try this as a contingency.
			// OMFI:CDCI:OffsetToFrameIndexes, omfi:UInt32
			hr = OrdReadDataOffset64(ePropCdciOffsetToFrameIndexes, pcbOffset);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:MinGOP, aka OMFI:MPGI:omMPGIMinGOPLength, omfi:Ushort
	//*****************************************************************************************************************
	STDMETHODIMP GetMinGOPLength(__out PWORD pwMinGOP)
	{
		HRESULT hr = OrdReadUInt16(ePropMpgiMinGOP, pwMinGOP);
		if (FAILED(hr))
		{
			hr = OrdReadUInt16(ePropMpgiomMPGIMinGOPLength, pwMinGOP);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:MaxGOP, aka OMFI:MPGI:omMPGIMaxGOPLength, omfi:Ushort
	//*****************************************************************************************************************
	STDMETHODIMP GetMaxGOPLength(__out PWORD pwMaxGOP)
	{
		HRESULT hr = OrdReadUInt16(ePropMpgiMaxGOP, pwMaxGOP);
		if (FAILED(hr))
		{
			hr = OrdReadUInt16(ePropMpgiomMPGIMaxGOPLength, pwMaxGOP);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:ProfileAndLevel, omfi:Uchar
	//*****************************************************************************************************************
	STDMETHODIMP GetProfileAndLevel(__out PBYTE pbProfAndLevel)
	{
		return OrdReadUInt8(ePropMpgiProfileAndLevel, pbProfAndLevel);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:RandomAccess, omfi:Boolean
	// Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	//*****************************************************************************************************************
	STDMETHODIMP IsRandomAccess(void)
	{
		return OrdReadBoolean(ePropMpgiRandomAccess, NULL);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:SequenceHdr, omfi:VarLenBytes or omfi:DataValue
	//*****************************************************************************************************************
	STDMETHODIMP GetSequenceHdrAsRawBytes(__in ULONG cbBuffer,
											__out_opt PVOID pBuffer,
												__out_opt PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropMpgiSequenceHdr, cbBuffer, pBuffer, pcbRequired);
	}

	//*****************************************************************************************************************
	// OMFI:MPGI:StreamType
	//*****************************************************************************************************************
	STDMETHODIMP GetStreamType(__out PBYTE pbStmType)
	{
		return OrdReadUInt8(ePropMpgiStreamType, pbStmType);
	}
};

//*********************************************************************************************************************
//	Quicktime Image Descriptor Class (QTMD).
//*********************************************************************************************************************
class COmfQtmdDescriptor : protected COmfDiddDescriptorT<IOmfQtmdDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfQtmdDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfDiddDescriptorT<IOmfQtmdDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// OMFI:QTMD:fType
	STDMETHODIMP GetfTypeAsRawBytes(__in ULONG cbBuffer, __out_opt PVOID pBuffer, __out_opt PULONG pcbRequired)
	{
		return OrdReadDataValueAsRawBytes(ePropQtmdfType, cbBuffer, pBuffer, pcbRequired);
	}
};

//*********************************************************************************************************************
//	Run-Length Encoded Image Descriptor Class (RLED).
//*********************************************************************************************************************
class COmfRleiDescriptor : protected COmfDiddDescriptorT<IOmfRleiDescriptor>
{
friend class CNewMdesFactory;
protected:
	COmfRleiDescriptor(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfDiddDescriptorT<IOmfRleiDescriptor>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:RLED:FrameIndex property as an array of UINT32s.
	// The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray32(ULONG nMaxElements, PUINT32 pArrayBase, PULONG pnActualElements)
	{
		HRESULT hr = OrdReadPosition32Array(ePropRledFrameIndex, nMaxElements, pArrayBase, pnActualElements);
		if (FAILED(hr))
		{
			// STRANGE BUT TRUE:
			// Sometimes the OMFI:RLED:FrameIndex property lives in the corresponding 'RLE ' media data object.
			hr = m_pContainer->OrdReadPosition32Array(m_dwMDAT, ePropRledFrameIndex,
															nMaxElements, pArrayBase, pnActualElements);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// Retrieves the OMFI:RLED:FrameIndex property as an array of UINT64s.
	// The data type can be omfi:Position32Array or omfi:Position64Array.
	// If the data type is omfi:Position32Array then the 32-bit values are zero-padded to 64 bits.
	//*****************************************************************************************************************
	STDMETHODIMP ReadFrameIndexArray64(ULONG nMaxElements, PUINT64 pArrayBase, PULONG pnActualElements)
	{
		HRESULT hr = OrdReadPosition64Array(ePropRledFrameIndex, nMaxElements, pArrayBase, pnActualElements);
		if (FAILED(hr))
		{
			// STRANGE BUT TRUE:
			// Sometimes the OMFI:RLED:FrameIndex property lives in the corresponding 'RLE ' media data object.
			hr = m_pContainer->OrdReadPosition64Array(m_dwMDAT, ePropRledFrameIndex,
															nMaxElements, pArrayBase, pnActualElements);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMFI:RLED:OffsetToFrameIndexes, omfi:UInt32
	//*****************************************************************************************************************
	STDMETHODIMP GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)
	{
		HRESULT hr = OrdReadDataOffset64(ePropRledOffsetToFrameIndexes, pcbOffset);
		if (FAILED(hr))
		{
			// Try this as a contingency.
			// OMFI:DIDD:OffsetToRLEFrameIndexes
			hr = OrdReadDataOffset64(ePropDiddOffsetToRLEFrameIndexes, pcbOffset);
		}
		return hr;
	}
};

