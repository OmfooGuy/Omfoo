// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfComponents.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfComponent.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//	Note that most of these are arranged alphabetically according to their OMF class fourCC.

//*********************************************************************************************************************
//	IOmfAttributesClip
//	OMF Attributes Clip Class object (ATCP).
//*********************************************************************************************************************
HRESULT COmfAttributesClip::GetAtcpAttributeList(REFIID riid, PVOID *ppvOut)
{
	// OMFI:ATCP:Attributes
	return OrdInstantiateObjRef(ePropAtcpAttributes, riid, ppvOut);
}

//*********************************************************************************************************************
//	IOmfClipComponent
//	OMF Clip Class object (CLIP).
//*********************************************************************************************************************
//	This class has only one property (OMFI:CLIP:Length) which is already accesible from IOmfComponent::GetLength() 
//	and implemented in COmfComponentT. So there are no additional properties or methods.

//*********************************************************************************************************************
//	IOmfTrackReference
//	OMF Track Reference Class object (TRKR).
//*********************************************************************************************************************
HRESULT COmfTrackReference::GetRelativeScope(__out PWORD pwRelativeScope)
{
	// OMFI:TRKR:RelativeScope
	return OrdReadUInt16(ePropTrkrRelativeScope, pwRelativeScope);
}

HRESULT COmfTrackReference::GetRelativeTrack(__out PWORD pwRelativeTrack)
{
	// OMFI:TRKR:RelativeTrack
	return OrdReadUInt16(ePropTrkrRelativeTrack, pwRelativeTrack);
}

//*********************************************************************************************************************
//	IOmfConstantValueSegment
//	OMF Constant Value Class object (CVAL).
//*********************************************************************************************************************
HRESULT COmfConstantValueSegment::GetParameterID(LPGUID pGuid)
{
	// OMFI:CVAL:ParameterID
	return OrdReadGuid(ePropCvalParameterID, pGuid);
}

HRESULT COmfConstantValueSegment::GetValueAsString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:CVAL:Value
	return OrdReadStringW(ePropCvalValue, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfConstantValueSegment::GetValueAsByte(PBYTE pb)
{
	// OMFI:CVAL:Value
	return OrdReadUInt8(ePropCvalValue, pb);
}

HRESULT COmfConstantValueSegment::GetValueAsInt16(PINT16 pn16)
{
	// OMFI:CVAL:Value
	return OrdReadInt16(ePropCvalValue, pn16);
}

HRESULT COmfConstantValueSegment::GetValueAsInt32(PINT32 pn32)
{
	// OMFI:CVAL:Value
	return OrdReadInt32(ePropCvalValue, pn32);
}

HRESULT COmfConstantValueSegment::GetValueAsInt64(PINT64 pn64)
{
	// OMFI:CVAL:Value
	return OrdReadInt64(ePropCvalValue, pn64);
}

HRESULT COmfConstantValueSegment::GetValueAsUInt16(PUINT16 pu16)
{
	// OMFI:CVAL:Value
	return OrdReadUInt16(ePropCvalValue, pu16);
}

HRESULT COmfConstantValueSegment::GetValueAsUInt32(PUINT32 pu32)
{
	// OMFI:CVAL:Value
	return OrdReadUInt32(ePropCvalValue, pu32);
}

HRESULT COmfConstantValueSegment::GetValueAsUInt64(PUINT64 pu64)
{
	// OMFI:CVAL:Value
	return OrdReadUInt64(ePropCvalValue, pu64);
}

HRESULT COmfConstantValueSegment::GetValueAsPoint(POMF_POINT pPoint)
{
	// OMFI:CVAL:Value
	return OrdReadPoint(ePropCvalValue, pPoint);
}

HRESULT COmfConstantValueSegment::GetValueAsRational(POMF_RATIONAL pRational)
{
	// OMFI:CVAL:Value
	return OrdReadRational(ePropCvalValue, pRational);
}

HRESULT COmfConstantValueSegment::GetRationalAsFloat(PFLOAT pFloat)
{
	// OMFI:CVAL:Value
	return OrdReadRationalAsFloat(ePropCvalValue, pFloat);
}

HRESULT COmfConstantValueSegment::GetRationalAsDouble(DOUBLE* pDouble)
{
	// OMFI:CVAL:Value
	return OrdReadRationalAsDouble(ePropCvalValue, pDouble);
}

HRESULT COmfConstantValueSegment::GetValueAsDataValue(POMF_DATA_VALUE pDataValue)
{
	// OMFI:CVAL:Value
	return OrdQueryDataValue(ePropCvalValue, pDataValue);
}

HRESULT COmfConstantValueSegment::CreateHGlobalOnDataValue(UINT uFlags, HGLOBAL* phGlobal)
{
	// OMFI:CVAL:Value
	return OrdCreateHGlobalOnDataValue(ePropCvalValue, uFlags, phGlobal);
}

HRESULT COmfConstantValueSegment::CreateStreamOnDataValue(REFIID riid, PVOID *ppvOut)
{
	// OMFI:CVAL:Value
	return OrdCreateStreamOnDataValue(ePropCvalValue, riid, ppvOut);
}

//*********************************************************************************************************************
//	IOmfEdgecodeSegment
//	OMF Edgecode Class object (ECCP).
//*********************************************************************************************************************
HRESULT COmfEdgecodeSegment::GetStart(PINT64 pStart)
{
//	OMFI:ECCP:StartEC, OMFI:ECCP:Start
	return OrdReadPosition64(m_fOmfVer1 ? ePropEccpStartEC : ePropEccpStart, pStart);
}

HRESULT COmfEdgecodeSegment::GetFilmKind(PWORD pwFilmKind)
{
	// OMFI:ECCP:FilmKind
	return OrdReadUInt16(ePropEccpFilmKind, pwFilmKind);
}

HRESULT COmfEdgecodeSegment::GetCodeFormat(PWORD pwCodeFormat)
{
	// OMFI:ECCP:CodeFormat
	return OrdReadUInt16(ePropEccpCodeFormat, pwCodeFormat);
}

HRESULT COmfEdgecodeSegment::GetHeader(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	CHAR	aMultiByte[64]	= {0};
	WCHAR	aWideChar[64]	= {0};
	ULONG	cbRequired		= 0;

	// OMFI:ECCP:Header
	// The routine omfmMobAddEdgecodeClip() in omfToolkit.dll v2.1.2 always created a header of exactly eight bytes.
	HRESULT hr = OrdReadRawBytes(ePropEccpHeader, ELEMS(aMultiByte)-1, aMultiByte, &cbRequired);
	if (SUCCEEDED(hr))
	{
		// If the original string had fewer than eight characters then it was padded with zeroes.
		// So use lstrlenA() to get the actual length.
		ULONG cbMultiByte	= lstrlenA(aMultiByte);

		// Always US-ASCII 20127 (7-bit).
		ULONG cchResult	= MultiByteToWideChar(CP_ASCII, MB_ERR_INVALID_CHARS,
												aMultiByte, cbMultiByte, aWideChar, ELEMS(aWideChar)-1);

		// If the payload was pure ASCII (and it should be) then these should match.
		if (cchResult != cbMultiByte)
		{
			hr = OMF_E_BAD_TEXT;
		}
	}

	return CallTwiceStringHandlerW(hr, aWideChar, cchBuffer, pBuffer, pcchRequired);
}

//*********************************************************************************************************************
//	IOmfEffectSegment
//	OMF Effect Invocation Class object (EFFE).
//*********************************************************************************************************************
HRESULT COmfEffectSegment::GetEffectIDString(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:EDEF:EffectID, omfi:UniqueName
	return m_pContainer->OrdReadStringW(m_dwEDEF, ePropEdefEffectID, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfEffectSegment::GetEffectIDWord(PWORD pwEffectID)
{
	// OMFI:EDEF:EffectID, omfi:UniqueName
	CHAR szUniqueName[64] = {0};
	HRESULT hr = m_pContainer->OrdReadStringA(m_dwEDEF, ePropEdefEffectID, ELEMS(szUniqueName), szUniqueName, NULL);
	if (SUCCEEDED(hr))
	{
		hr = m_pContainer->EffectIDStringToWord(szUniqueName, pwEffectID);
	}
	return hr;
}

HRESULT COmfEffectSegment::GetEffectName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:EDEF:EffectName
	return m_pContainer->OrdReadStringW(m_dwEDEF, ePropEdefEffectName, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfEffectSegment::GetEffectDescription(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
{
	// OMFI:EDEF:EffectDescription
	return m_pContainer->OrdReadStringW(m_dwEDEF, ePropEdefEffectDescription, cchBuffer, pBuffer, pcchRequired);
}

HRESULT COmfEffectSegment::IsTimeWarp(void)
{
	// OMFI:EDEF:IsTimeWarp
	return m_pContainer->OrdReadBoolean(m_dwEDEF, ePropEdefIsTimeWarp, NULL);
}

HRESULT COmfEffectSegment::GetBypassArgID(PLONG pnArgID)
{
	// OMFI:EDEF:Bypass, omfi:ArgIDType
	// The Argument ID value that identifies the primary input slot.
	// Optional.
	return m_pContainer->OrdReadUInt32(m_dwEDEF, ePropEdefBypass, PUINT32(pnArgID));
}

HRESULT COmfEffectSegment::GetEffectDefinitionObject(REFIID riid, PVOID *ppvOut)
{
	// OMFI:CPNT:DataKind, omfi:ObjRef
	return m_pContainer->Instantiate(m_dwEDEF, this, NULL, riid, ppvOut);
}

HRESULT COmfEffectSegment::GetBypassOverride(PLONG pnBypassOverride)
{
	// OMFI:EFFE:BypassOverride, omfi:ArgIDType
	// Specifies the ArgID value of the input media Effect Slot to be substituted for the Effect Invocation
	// if the application cannot generate the effect. Overrides the bypass specified in the Effect Definition.
	// Optional.
	return OrdReadUInt32(ePropEffeBypassOverride, PUINT32(pnBypassOverride));
}

HRESULT COmfEffectSegment::IterateEffectSlots(IOmfooIterator **ppIterator)
{
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pEffectSlots)
	{
		// OMFI:EFFE:EffectSlots
		hr = OrdAllocObjRefArray(ePropEffeEffectSlots, &m_pEffectSlots);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pEffectSlots, ppIterator);
	}

	return hr;
}

HRESULT COmfEffectSegment::GetFinalRendering(REFIID riid, PVOID *ppvOut)
{
	// OMFI:EFFE:FinalRendering
	return OrdInstantiateObjRef(ePropEffeFinalRendering, riid, ppvOut);
}

HRESULT COmfEffectSegment::GetWorkingRendering(REFIID riid, PVOID *ppvOut)
{
	// OMFI:EFFE:WorkingRendering
	return OrdInstantiateObjRef(ePropEffeWorkingRendering, riid, ppvOut);
}

//*********************************************************************************************************************
//	IOmfEditRateSegment
//	OMF Edit Rate Converter Class object (ERAT).
//*********************************************************************************************************************
HRESULT COmfEditRateSegment::GetInputSegment(REFIID riid, PVOID *ppvOut)
{
	// OMFI:ERAT:InputSegment
	return OrdInstantiateObjRef(ePropEratInputSegment, riid, ppvOut);
}

HRESULT COmfEditRateSegment::GetInputEditRate(POMF_RATIONAL pEditRate)
{
	// OMFI:ERAT:InputEditRate
	if (SUCCEEDED(OrdReadRational(ePropEratInputEditRate, pEditRate)))
		return S_OK;

	// In OMF 2.0 files this property is mispelled as OMFI:ERAT:InputEditRage!
	return OrdReadRational(ePropEratInputEditRage, pEditRate);
}

HRESULT COmfEditRateSegment::GetInputEditRateAsFloat(PFLOAT pFloat)
{
	// OMFI:ERAT:InputEditRate
	if (SUCCEEDED(OrdReadRationalAsFloat(ePropEratInputEditRate, pFloat)))
		return S_OK;

	// In some files this property is mispelled as OMFI:ERAT:InputEditRage!
	return OrdReadRationalAsFloat(ePropEratInputEditRage, pFloat);
}

HRESULT COmfEditRateSegment::GetInputEditRateAsDouble(DOUBLE* pDouble)
{
	// OMFI:ERAT:InputEditRate
	if (SUCCEEDED(OrdReadRationalAsDouble(ePropEratInputEditRate, pDouble)))
		return S_OK;

	// In some files this property is mispelled as OMFI:ERAT:InputEditRage!
	return OrdReadRationalAsDouble(ePropEratInputEditRage, pDouble);
}

HRESULT COmfEditRateSegment::GetInputOffset(PINT64 pInputOffset)
{
	// OMFI:ERAT:InputOffset
	return OrdReadPosition64(ePropEratInputOffset, pInputOffset);
}

HRESULT COmfEditRateSegment::GetResultOffset(PINT64 pResultOffset)
{
	// OMFI:ERAT:ResultOffset
	return OrdReadPosition64(ePropEratResultOffset, pResultOffset);
}

//*********************************************************************************************************************
//	IOmfFillerSegment
//	OMF Filler Class object (FILL).
//*********************************************************************************************************************
//
//	(No additional methods.)

//*********************************************************************************************************************
//	IOmfOmfMediaGroupSegment
//	OMF Media Group Class object (MGRP).
//*********************************************************************************************************************
HRESULT COmfMediaGroupSegment::IterateChoices(IOmfooIterator **ppIterator)
{
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pChoices)
	{
		// OMFI:MGRP:Choices
		hr = OrdAllocObjRefArray(ePropMgrpChoices, &m_pChoices);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pChoices, ppIterator);
	}

	return hr;
}

HRESULT COmfMediaGroupSegment::GetStillFrame(REFIID riid, PVOID *ppvOut)
{
	// OMFI:MGRP:StillFrame
	return OrdInstantiateObjRef(ePropMgrpStillFrame, riid, ppvOut);
}

//*********************************************************************************************************************
//	IOmfNestedScopeSegment
//	OMF Nested Scope Class object (NEST).
//*********************************************************************************************************************
HRESULT COmfNestedScopeSegment::IterateSlots(IOmfooIterator **ppIterator)
{
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pNestSlots)
	{
		// OMFI:NEST:Slots
		hr = OrdAllocObjRefArray(ePropNestSlots, &m_pNestSlots);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pNestSlots, ppIterator);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfPulldownSegment
//	OMF Pulldown Class object (PDWN).
//*********************************************************************************************************************
HRESULT COmfPulldownSegment::GetInputSegment(REFIID riid, PVOID *ppvOut)
{
	// OMFI:PDWN:InputSegment
	return OrdInstantiateObjRef(ePropPdwnInputSegment, riid, ppvOut);
}

HRESULT COmfPulldownSegment::GetPhaseFrame(PWORD pwPhaseFrame)
{
	// OMFI:PDWN:PulldownPhaseFrame
	return OrdReadUInt16(ePropPdwnPulldownPhaseFrame, pwPhaseFrame);
}

HRESULT COmfPulldownSegment::GetPulldownDirection(PWORD pwDirection)
{
	// OMFI:PDWN:PulldownDirection
	return OrdReadUInt16(ePropPdwnPulldownDirection, pwDirection);
}

HRESULT COmfPulldownSegment::GetPulldownKind(PWORD pwPulldownKind)
{
	// OMFI:PDWN:PulldownKind
	return OrdReadUInt16(ePropPdwnPulldownKind, pwPulldownKind);
}

//*********************************************************************************************************************
//	IOmfSourceClipSegment
//	OMF Source Clip Class object (SCLP).
//*********************************************************************************************************************
HRESULT COmfSourceClipSegment::GetSourceID(POMF_MOB_ID pMobID)
{
	// OMFI:SCLP:SourceID
	if (IsBadWritePointer(pMobID, sizeof(OMF_MOB_ID)))
	{
		return E_POINTER;
	}
	else
	{
		*pMobID = m_mobID;
		return m_hrMobID;
	}
}

HRESULT COmfSourceClipSegment::GetSourceTrackID(PDWORD pdwTrackID)
{
	// OMFI:SCLP:SourceTrackID, omfi:UInt32
	// OMFI:SCLP:SourceTrack, omfi:Short
	if (IsBadWritePointer(pdwTrackID, sizeof(DWORD)))
	{
		return E_POINTER;
	}
	else
	{
		*pdwTrackID	= m_dwTrackID;
		return m_hrTrackID;
	}
}

HRESULT COmfSourceClipSegment::ResolveRef(REFIID riid, PVOID *ppvOut)
{
	HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (FAILED(hr))
	{
		goto L_Exit;
	}

	if (m_fIsOriginalSource)
	{
		hr = S_FALSE;
		goto L_Exit;
	}

	// Find the MOBJ with the corresponding mobID.
	DWORD dwMOBJ = m_pContainer->FindMobjFromMobID(m_mobID);
	if (dwMOBJ)
	{
		hr = m_pContainer->Instantiate(dwMOBJ, m_pParent, this, riid, ppvOut);
		goto L_Exit;
	}

	// If dwMOBJ is zero then it's probably because its external (in another OMF file).
	BREAK_IF_DEBUG
	// "F:\Omf Test Files\OMFs from Reaper forum\AVID media composer nitris\omf1 test.omf"
	hr = OMF_E_MOB_NOT_FOUND;	// OMF_E_NO_PREVIOUS_GENERATION, 

L_Exit:
	return hr;
}

HRESULT COmfSourceClipSegment::GetStartTime(PINT64 pStartTime)
{
	// OMFI:SCLP:SourcePosition, OMFI:SCLP:StartTime
	return OrdReadPosition64(m_fOmfVer1 ? ePropSclpSourcePosition : ePropSclpStartTime, pStartTime);
}

HRESULT COmfSourceClipSegment::GetFadeInLength(PUINT64 pLength)
{
	// OMFI:SCLP:FadeInLength
	return OrdReadLength64(ePropSclpFadeInLength, pLength);
}

HRESULT COmfSourceClipSegment::GetFadeInType(PDWORD pdwFadeType)
{
	// OMFI:SCLP:FadeInType
	return OrdReadUInt32(ePropSclpFadeInType, PUINT32(pdwFadeType));
}

HRESULT COmfSourceClipSegment::GetFadeOutLength(PUINT64 pLength)
{
	// OMFI:SCLP:FadeOutLength
	return OrdReadLength64(ePropSclpFadeOutLength, pLength);
}

HRESULT COmfSourceClipSegment::GetFadeOutType(PDWORD pdwFadeType)
{
	// OMFI:SCLP:FadeOutType
	return OrdReadUInt32(ePropSclpFadeOutType, PUINT32(pdwFadeType));
}

//*********************************************************************************************************************
//	IOmfSequenceSegment
//	OMF Sequence Class object (SEQU).
//*********************************************************************************************************************
HRESULT COmfSequenceSegment::IterateComponents(IOmfooIterator **ppIterator)
{
	// OMF1 - Creates an iterator to instantiate the objects in the OMFI:SEQU:Sequence array.
	// OMF2 - Creates an iterator to instantiate the objects in the OMFI:SEQU:Components array.
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pComponents)
	{
		hr = OrdAllocObjRefArray(m_fOmfVer1 ? ePropSequSequence : ePropSequComponents, &m_pComponents);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pComponents, ppIterator);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfSelectorSegment
//	OMF Selector Class object (SLCT).
//*********************************************************************************************************************
HRESULT COmfSelectorSegment::IsGanged(void)
{
	// OMFI:SLCT:IsGanged
	return OrdReadBoolean(ePropSlctIsGanged, NULL);
}

HRESULT COmfSelectorSegment::GetSelectedTrack(__out PWORD pwTrack)
{
	// OMFI:SLCT:SelectedTrack
	return OrdReadUInt16(ePropSlctSelectedTrack, pwTrack);
}

HRESULT COmfSelectorSegment::IterateAlternates(IOmfooIterator **ppIterator)
{
	// OMF1 - Creates an iterator to instantiate the objects in the OMFI:TRKG:Tracks array.
	// OMF2 - Creates an iterator to instantiate the objects in the OMFI:SLCT:Alternates array.
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pAlternates)
	{
		hr = OrdAllocObjRefArray(m_fOmfVer1 ? ePropTrkgTracks : ePropSlctAlternates, &m_pAlternates);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pAlternates, ppIterator);
	}

	return hr;
}

HRESULT COmfSelectorSegment::GetSelected(__in REFIID riid, __out PVOID *ppvOut)
{
	// OMF1 - Instantiates the ObjRef specified by the OMFI:SLCT:SelectedTrack property.
	// OMF2 - Instantiates the ObjRef specified by the OMFI:SLCT:Selected property.
	// Note that in OMF1 the SelectedTrack is a TRAK that lives in the OMFI:TRKG:Tracks array.
	// But in OMF2 the OMFI:SLCT:Selected is a SEGM that lives in the OMFI:SLCT:Selected property,
	// and _not_ a member of the OMFI:SLCT:Alternates array.
	HRESULT hr = S_OK;

	if (m_fOmfVer1)
	{
		// Create the array now if it hasn’t been created yet.
		if (NULL == m_pAlternates)
		{
			hr = OrdAllocObjRefArray(ePropTrkgTracks, &m_pAlternates);
		}

		if (SUCCEEDED(hr))
		{
			// OMFI:SLCT:SelectedTrack
			WORD wSelectedTrack = (-1);
			if (SUCCEEDED(hr = OrdReadUInt16(ePropSlctSelectedTrack, &wSelectedTrack)))
			{
				hr = OMF_E_OOBJ_NOT_FOUND;

				// Search each member of the array for a label number that matches wSelectedTrack.
				for (UINT i = 0; i < m_pAlternates->nElements; i++)
				{
					// OMFI:TRAK:LabelNumber
					WORD	wLabelNumber	= 0;
					DWORD	dwTRAK			= m_pAlternates->a[i];
					if (SUCCEEDED(m_pContainer->OrdReadUInt16(dwTRAK, ePropTrakLabelNumber, &wLabelNumber)))
					{
						// Is this the selected track?
						if (wLabelNumber == wSelectedTrack)
						{
							// Get the track's CPNT.
							DWORD dwCPNT = 0;
							// OMFI:TRAK:TrackComponent
							if (SUCCEEDED(m_pContainer->OrdReadObjRef(dwTRAK, ePropTrakTrackComponent, NULL, &dwCPNT)))
							{
								hr = m_pContainer->Instantiate(dwCPNT, this, NULL, riid, ppvOut);
							}
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		// OMFI:SLCT:Selected
		hr = OrdInstantiateObjRef(ePropSlctSelected, riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	COmfScopeReferenceSegment
//	OMF Scope Reference Class object (SREF).
//*********************************************************************************************************************
HRESULT COmfScopeReferenceSegment::GetRelativeScope(PDWORD pdwRelativeScope)
{
	// OMFI:SREF:RelativeScope
	return OrdReadUInt32(ePropSrefRelativeScope, PUINT32(pdwRelativeScope));
}

HRESULT COmfScopeReferenceSegment::GetRelativeSlot(PDWORD pdwRelativeSlot)
{
	// OMFI:SREF:RelativeSlot
	return OrdReadUInt32(ePropSrefRelativeSlot, PUINT32(pdwRelativeSlot));
}

//*********************************************************************************************************************
//	IOmfTimecodeSegment
//	OMF Timecode Class object (TCCP).
//*********************************************************************************************************************
HRESULT COmfTimecodeSegment::IsDrop(void)
{
	if (m_fOmfVer1)
	{
		// OMFI:TCCP:Flags
		DWORD dwFlags = 0;
		HRESULT hr = OrdReadUInt32(ePropTccpFlags, PUINT32(&dwFlags));
		if (SUCCEEDED(hr))
		{
			return (dwFlags > 0) ? S_OK : S_FALSE;
		}
		else
		{
			return hr;
		}
	}
	else
	{
		// OMFI:TCCP:Drop
		return OrdReadBoolean(ePropTccpDrop, NULL);
	}
}

HRESULT COmfTimecodeSegment::GetFramesPerSec(PWORD pwFPS)
{
	// OMFI:TCCP:FPS
	return OrdReadUInt16(ePropTccpFPS, pwFPS);
}

HRESULT COmfTimecodeSegment::GetStart(PINT64 pStart)
{
	// OMFI:TCCP:StartTC, OMFI:TCCP:Start
	return OrdReadPosition64(m_fOmfVer1 ? ePropTccpStartTC : ePropTccpStart, pStart);
}

//*********************************************************************************************************************
//	IOmfTransComponent
//	OMF Transition Class object (TRAN).
//*********************************************************************************************************************
HRESULT COmfTransComponent::GetCutPoint(__out PINT64 pCutPoint)
{
	// OMFI:TRAN:CutPoint
	return OrdReadPosition64(ePropTranCutPoint, pCutPoint);
}

HRESULT COmfTransComponent::GetEffect(__in REFIID riid, __out PVOID *ppvOut)
{
	// OMFI:TRAN:Effect
	return OrdInstantiateObjRef(ePropTranEffect, riid, ppvOut);
}

//*********************************************************************************************************************
//	IOmfVaryingValueSegment
//	OMF Varying Value Class object (VVAL).
//*********************************************************************************************************************
HRESULT COmfVaryingValueSegment::GetParameterID(LPGUID pGuid)
{
	// OMFI:VVAL:ParameterID
	return OrdReadGuid(ePropVvalParameterID, pGuid);
}

HRESULT COmfVaryingValueSegment::GetInterpolationKind(PWORD pwInterpKind)
{
	// OMFI:VVAL:Interpolation
	return OrdReadUInt16(ePropVvalInterpolation, pwInterpKind);
}

HRESULT COmfVaryingValueSegment::GetExtrapolationKind(PWORD pwExtrapKind)
{
	// OMFI:VVAL:Extrapolation
	return OrdReadUInt16(ePropVvalExtrapolation, pwExtrapKind);
}

HRESULT COmfVaryingValueSegment::GetFieldCount(PWORD pwFieldCount)
{
	// OMFI:VVAL:FieldCount
	return OrdReadUInt16(ePropVvalFieldCount, pwFieldCount);
}

HRESULT COmfVaryingValueSegment::IterateControlPoints(IOmfooIterator **ppIterator)
{
	HRESULT hr = S_OK;

	// Create the array now if it hasn’t been created yet.
	if (NULL == m_pPointList)
	{
		// OMFI:VVAL:PointList
		hr = OrdAllocObjRefArray(ePropVvalPointList, &m_pPointList);
	}

	if (SUCCEEDED(hr))
	{
		hr = WrapObjRefArray(m_pPointList, ppIterator);
	}

	return hr;
}

//*********************************************************************************************************************
//	IOmfVideoMaskComponent
//	OMF Video Frame Mask Class object (MASK).
//*********************************************************************************************************************
HRESULT COmfVideoMaskComponent::IsDouble(void)
{
	// OMFI:MASK:IsDouble
	return OrdReadBoolean(ePropMaskIsDouble, NULL);
}

HRESULT COmfVideoMaskComponent::GetMaskBits(PDWORD pdwMaskBits)
{
	//	OMFI:MASK:MaskBits
	return OrdReadUInt32(ePropMaskMaskBits, PUINT32(pdwMaskBits));
}

//*********************************************************************************************************************
//	IOmfVideoSpeedComponent
//	OMF Video Speed Class object (SPED).

//	OMFI:SPED:Numerator / OMFI:SPED:Denominator
//	ePropSpedNumerator,						// OMFI:SPED:Numerator, OMInt32
//	ePropSpedDenominator,					// OMFI:SPED:Denominator, OMInt32
//	ePropSpedIsFreezeFrame,					// OMFI:SPED:IsFreezeFrame
//*********************************************************************************************************************
HRESULT COmfVideoSpeedComponent::IsFreezeFrame(void)
{
	// OMFI:SPED:IsFreezeFrame
	return OrdReadBoolean(ePropSpedIsFreezeFrame, NULL);
}

HRESULT COmfVideoSpeedComponent::GetSpeedAsRational(POMF_RATIONAL pSpeed)
{
	if (IsBadWritePointer(pSpeed, sizeof(OMF_RATIONAL)))
	{
		return E_POINTER;
	}
	else
	{
		pSpeed->nNumerator		= 0;
		pSpeed->nDenominator	= 0;
	}

	INT32	nNumerator		= 0;
	INT32	nDenominator	= 0;

	// OMFI:SPED:Numerator
	HRESULT	hr = OrdReadInt32(ePropSpedNumerator, &nNumerator);
	if (SUCCEEDED(hr))
	{
		// OMFI:SPED:Denominator
		if (SUCCEEDED(hr = OrdReadInt32(ePropSpedDenominator, &nDenominator)))
		{
			pSpeed->nNumerator		= nNumerator;
			pSpeed->nDenominator	= nDenominator;
		}
	}
	return hr;
}

HRESULT COmfVideoSpeedComponent::GetSpeedAsFloat(PFLOAT pFloat)
{
	if (IsBadWritePointer(pFloat, sizeof(FLOAT)))
	{
		return E_POINTER;
	}
	else
	{
		*pFloat = 0; 
	}

	INT32	nNumerator		= 0;
	INT32	nDenominator	= 0;

	// OMFI:SPED:Numerator
	HRESULT	hr = OrdReadInt32(ePropSpedNumerator, &nNumerator);
	if (SUCCEEDED(hr))
	{
		// OMFI:SPED:Denominator
		if (SUCCEEDED(hr = OrdReadInt32(ePropSpedDenominator, &nDenominator)))
		{
			if (nDenominator)
			{
				DOUBLE fQuotient = nNumerator;
				fQuotient /= nDenominator;

				// Conversion from 'DOUBLE' to 'FLOAT', possible loss of data!
				*pFloat = FLOAT(fQuotient);
			}
			else
			{
				hr = OMF_E_DIVIDE_BY_ZERO;
			}
		}
	}
	return hr;
}

HRESULT COmfVideoSpeedComponent::GetSpeedAsDouble(DOUBLE* pDouble)
{
	if (IsBadWritePointer(pDouble, sizeof(DOUBLE)))
	{
		return E_POINTER;
	}
	else
	{
		*pDouble = 0;
	}

	INT32	nNumerator		= 0;
	INT32	nDenominator	= 0;

	// OMFI:SPED:Numerator
	HRESULT	hr = OrdReadInt32(ePropSpedNumerator, &nNumerator);
	if (SUCCEEDED(hr))
	{
		// OMFI:SPED:Denominator
		if (SUCCEEDED(hr = OrdReadInt32(ePropSpedDenominator, &nDenominator)))
		{
			if (nDenominator)
			{
				DOUBLE fQuotient = nNumerator;
				fQuotient /= nDenominator;
				*pDouble = fQuotient;
			}
			else
			{
				hr = OMF_E_DIVIDE_BY_ZERO;
			}
		}
	}

	return hr;
}

