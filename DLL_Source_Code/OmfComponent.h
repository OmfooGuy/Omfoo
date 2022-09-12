// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfComponent.h
// Copyright (C) 2022 David Miller
// This file is  part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "Omfoo_Alpha_Header.h"
#include "OmfObject.h"
#include "OmfComponentT.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//	NOTICE: All of the C++ classes defined in this file inherit COmfComponentT, which in turn inherits COmfObject.
//	They all provide access to OMF OOBJs that inherit CPNT.
//	These C++ are not meant to be instantiated on the stack.
//	They all assume that their C++ operator new() has zeroed all of their memory at birth - even on debug builds.
//	Fwiw the global C++ operator new() is declaired in DllMain.cpp.
//
//	Note that most of these C++ class definitions are arranged alphabetically according to their OMF class fourCC.

//*********************************************************************************************************************
//	OMF Component Class object (CPNT).
//	Available in OMF1 and OMF2.
//*********************************************************************************************************************
class COmfComponent : protected COmfComponentT<IOmfComponent>
{
friend class CNewCpntFactory;
protected:
	COmfComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfComponentT<IOmfComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Attributes Clip Class object (ATCP).
//	Only available in OMF1.
//*********************************************************************************************************************
class COmfAttributesClip : protected COmfComponentT<IOmfAttributesClip>
{
friend class CNewCpntFactory;
protected:
	COmfAttributesClip(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfComponentT<IOmfAttributesClip>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfAttributesClip methods in V-table order.
	STDMETHODIMP GetAtcpAttributeList(__in REFIID riid, __out PVOID *ppvOut);
};

//*********************************************************************************************************************
//	OMF Clip Class object (CLIP).
//	Only available in OMF1.
//*********************************************************************************************************************
class COmfClipComponent : protected COmfComponentT<IOmfClipComponent>
{
friend class CNewCpntFactory;
protected:
	COmfClipComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfComponentT<IOmfClipComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfClipComponent methods in V-table order.
	// CLIP has only one property (OMFI:CLIP:Length) which is already accesible from IOmfComponent::GetLength().
};

//*********************************************************************************************************************
//	OMF Track Reference object (TRKR).
//	Only available in OMF1.
//*********************************************************************************************************************
class COmfTrackReference : protected COmfSegmentT<IOmfTrackReference>
{
friend class CNewCpntFactory;
protected:
	COmfTrackReference(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfTrackReference>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfTrackReference methods in V-table order.
	STDMETHODIMP GetRelativeScope(__out PWORD pwRelativeScope);
	STDMETHODIMP GetRelativeTrack(__out PWORD pwRelativeTrack);
};

//*********************************************************************************************************************
//	OMF Constant Value Class object (CVAL).
//*********************************************************************************************************************
class COmfConstantValueSegment : protected COmfSegmentT<IOmfConstantValueSegment>
{
friend class CNewCpntFactory;
protected:
	COmfConstantValueSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfConstantValueSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
		// OMFI:CPNT:DataKind
	//	OrdGetDataKind(ePropCpntDataKind, &m_wDataKind);
	}

	// IOmfConstantValueSegment methods in V-table order.
	STDMETHODIMP GetParameterID(__out LPGUID pGuid);
	STDMETHODIMP GetValueAsString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP GetValueAsByte(__out PBYTE pb);
	STDMETHODIMP GetValueAsInt16(__out PINT16 pn16);
	STDMETHODIMP GetValueAsInt32(__out PINT32 pn32);
	STDMETHODIMP GetValueAsInt64(__out PINT64 pn64);
	STDMETHODIMP GetValueAsUInt16(__out PUINT16 pu16);
	STDMETHODIMP GetValueAsUInt32(__out PUINT32 pu32);
	STDMETHODIMP GetValueAsUInt64(__out PUINT64 pu64);
	STDMETHODIMP GetValueAsPoint(__out POMF_POINT pPoint);
	STDMETHODIMP GetValueAsRational(__out POMF_RATIONAL pRational);
	STDMETHODIMP GetRationalAsFloat(__out PFLOAT pFloat);
	STDMETHODIMP GetRationalAsDouble(__out DOUBLE* pDouble);
	STDMETHODIMP GetValueAsDataValue(__out POMF_DATA_VALUE pDataValue);
	STDMETHODIMP CreateHGlobalOnDataValue(__in UINT uFlags, __out HGLOBAL* phGlobal);
	STDMETHODIMP CreateStreamOnDataValue(__in REFIID riid, __out PVOID *ppvOut);
};

//*********************************************************************************************************************
//	OMF Edgecode Class object (ECCP).
//*********************************************************************************************************************
class COmfEdgecodeSegment : protected COmfSegmentT<IOmfEdgecodeSegment>
{
friend class CNewCpntFactory;
protected:
	COmfEdgecodeSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfEdgecodeSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfEdgecodeSegment methods in V-table order.
	STDMETHODIMP GetStart(__out PINT64 pStart);
	STDMETHODIMP GetFilmKind(__out PWORD pwFilmKind);
	STDMETHODIMP GetCodeFormat(__out PWORD pwCodeFormat);
	STDMETHODIMP GetHeader(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
};

//*********************************************************************************************************************
//	OMF Effect Invocation Class object (EFFE).
//*********************************************************************************************************************
class COmfEffectSegment : protected COmfSegmentT<IOmfEffectSegment>
{
friend class CNewCpntFactory;
protected:
	COmfEffectSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfEffectSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
		m_dwEDEF = OrdReadObjRef(ePropEffeEffectKind, NULL, &m_dwEDEF);
	}
	virtual ~COmfEffectSegment(void)
	{
		FreeObjRefArray(&m_pEffectSlots);
	}

	// IOmfEffectSegment methods in V-table order.
	STDMETHODIMP GetEffectIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP GetEffectIDWord(__out PWORD pwEffectID);
	STDMETHODIMP GetEffectName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP GetEffectDescription(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired);
	STDMETHODIMP IsTimeWarp(void);
	STDMETHODIMP GetBypassArgID(__out PLONG plBypassArgID);
	STDMETHODIMP GetEffectDefinitionObject(__in REFIID riid, __out PVOID *ppvOut);

	STDMETHODIMP GetBypassOverride(__out PLONG pnBypassOverride);
	STDMETHODIMP IterateEffectSlots(__out IOmfooIterator **ppIterator);
	STDMETHODIMP GetFinalRendering(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP GetWorkingRendering(__in REFIID riid, __out PVOID *ppvOut);

private:
	POMFOO_OBJREF_ARRAY	m_pEffectSlots;
	DWORD				m_dwEDEF;
};

//*********************************************************************************************************************
//	OMF Edit Rate Converter Class object (ERAT).
//*********************************************************************************************************************
class COmfEditRateSegment : protected COmfSegmentT<IOmfEditRateSegment>
{
friend class CNewCpntFactory;
protected:
	COmfEditRateSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfEditRateSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
		// OMFI:CPNT:DataKind
		// omfi:ObjRef
	}

	// IOmfEditRateSegment methods in V-table order.
	STDMETHODIMP GetInputSegment(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP GetInputEditRate(__out POMF_RATIONAL pEditRate);
	STDMETHODIMP GetInputEditRateAsFloat(__out PFLOAT pFloat);
	STDMETHODIMP GetInputEditRateAsDouble(__out DOUBLE* pDouble);
	STDMETHODIMP GetInputOffset(__out PINT64 pInputOffset);
	STDMETHODIMP GetResultOffset(__out PINT64 pResultOffset);
};

//*********************************************************************************************************************
//	OMF Filler Class object (FILL).
//*********************************************************************************************************************
class COmfFillerSegment : protected COmfSegmentT<IOmfFillerSegment>
{
friend class CNewCpntFactory;
protected:
	COmfFillerSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfFillerSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Media Group Class object (MGRP).
//*********************************************************************************************************************
class COmfMediaGroupSegment : protected COmfSegmentT<IOmfMediaGroupSegment>
{
friend class CNewCpntFactory;
protected:
	COmfMediaGroupSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfMediaGroupSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
	virtual ~COmfMediaGroupSegment(void)
	{
		FreeObjRefArray(&m_pChoices);
	}

	// IOmfMediaGroupSegment methods in V-table order.
	STDMETHODIMP IterateChoices(__out IOmfooIterator **ppIterator);
	STDMETHODIMP GetStillFrame(__in REFIID riid, __out PVOID *ppvOut);

private:
	POMFOO_OBJREF_ARRAY	m_pChoices;
};

//*********************************************************************************************************************
//	OMF Nested Scope Class object (NEST).
//*********************************************************************************************************************
class COmfNestedScopeSegment : protected COmfSegmentT<IOmfNestedScopeSegment>
{
friend class CNewCpntFactory;
protected:
	COmfNestedScopeSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfNestedScopeSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
	virtual ~COmfNestedScopeSegment(void)
	{
		FreeObjRefArray(&m_pNestSlots);
	}

	// IOmfNestedScopeSegment methods in V-table order.
	STDMETHODIMP IterateSlots(__out IOmfooIterator **ppIterator);

private:
	POMFOO_OBJREF_ARRAY	m_pNestSlots;
};

//*********************************************************************************************************************
//	OMF Pulldown Class object (PDWN).
//*********************************************************************************************************************
class COmfPulldownSegment : protected COmfSegmentT<IOmfPulldownSegment>
{
friend class CNewCpntFactory;
protected:
	COmfPulldownSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfPulldownSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfPulldownSegment methods in V-table order.
	STDMETHODIMP GetInputSegment(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP GetPhaseFrame(__out PWORD pwPhaseFrame);
	STDMETHODIMP GetPulldownDirection(__out PWORD pwDirection);
	STDMETHODIMP GetPulldownKind(__out PWORD pwPulldownKind);
};

//*********************************************************************************************************************
//	OMF Source Clip Class object (SCLP).
//*********************************************************************************************************************
class COmfSourceClipSegment : protected COmfSegmentT<IOmfSourceClipSegment>
{
friend class CNewCpntFactory;
protected:
	COmfSourceClipSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfSourceClipSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
		if (m_fOmfVer1)
		{
			// OMFI:SCLP:SourceTrack, omfi:Short
			m_hrTrackID = OrdReadUInt16(ePropSclpSourceTrack, PUINT16(&m_dwTrackID));
		}
		else
		{
			// OMFI:SCLP:SourceTrackID, omfi:UInt32
			m_hrTrackID = OrdReadUInt32(ePropSclpSourceTrackID, PUINT32(&m_dwTrackID));
		}

		// OMFI:SCLP:SourceID
		m_hrMobID = OrdReadMobID(ePropSclpSourceID, &m_mobID);

		// Page 197 of the OMF Interchange Specification 2.1 says, 	
		// "If the SourceID has a value 0-0-0, then SourceTrackID must have a 0 value."
		// However some OMF v2.0 SourceTrackIDs use 0xFFFFFF00 to represent that condition.
		// So here we tweak it when necessary.
		if ((0 == m_mobID.dwMinor) && (0 == m_mobID.dwMajor) && (0 == m_mobID.dwPrefix))
		{
			m_dwTrackID			= 0;
			m_fIsOriginalSource	= TRUE;
		}
	}

	// IOmfSourceClipSegment methods in V-table order.
	STDMETHODIMP GetSourceID(__out POMF_MOB_ID pMobID);
	STDMETHODIMP GetSourceTrackID(__out PDWORD pdwTrackID);
	STDMETHODIMP ResolveRef(__in REFIID riid, __out PVOID *ppvOut);
	STDMETHODIMP GetStartTime(__out PINT64 pStartTime);
	STDMETHODIMP GetFadeInLength(__out PUINT64 pLength);
	STDMETHODIMP GetFadeInType(__out PDWORD pdwFadeType);
	STDMETHODIMP GetFadeOutLength(__out PUINT64 pLength);
	STDMETHODIMP GetFadeOutType(__out PDWORD pdwFadeType);

private:
	OMF_MOB_ID	m_mobID;
	DWORD		m_dwTrackID;
	BOOL		m_fIsOriginalSource;
	HRESULT		m_hrMobID;
	HRESULT		m_hrTrackID;
};

//*********************************************************************************************************************
//	OMF Segment Class object (SEGM).
//*********************************************************************************************************************
class COmfSegment : protected COmfSegmentT<IOmfSegment>
{
friend class CNewCpntFactory;
protected:
	COmfSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Sequence Class object (SEQU).
//*********************************************************************************************************************
class COmfSequenceSegment : protected COmfSegmentT<IOmfSequenceSegment>
{
friend class CNewCpntFactory;
protected:
	COmfSequenceSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfSequenceSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
	virtual ~COmfSequenceSegment(void)
	{
		FreeObjRefArray(&m_pComponents);
	}

	// IOmfSequenceSegment methods in V-table order.
	STDMETHODIMP IterateComponents(__out IOmfooIterator **ppIterator);

private:
	POMFOO_OBJREF_ARRAY	m_pComponents;
};

//*********************************************************************************************************************
//	OMF Selector Class object (SLCT).
//*********************************************************************************************************************
class COmfSelectorSegment : protected COmfSegmentT<IOmfSelectorSegment>
{
friend class CNewCpntFactory;
protected:
	COmfSelectorSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfSelectorSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
	virtual ~COmfSelectorSegment(void)
	{
		FreeObjRefArray(&m_pAlternates);
	}

	// IOmfSelectorSegment methods in V-table order.
	STDMETHODIMP IsGanged(void);
	STDMETHODIMP GetSelectedTrack(__out PWORD pwTrack);
	STDMETHODIMP IterateAlternates(__out IOmfooIterator **ppIterator);
	STDMETHODIMP GetSelected(__in REFIID riid, __out PVOID *ppvOut);

private:
	POMFOO_OBJREF_ARRAY	m_pAlternates;
};

//*********************************************************************************************************************
//	OMF Scope Reference Class object (SREF).
//*********************************************************************************************************************
class COmfScopeReferenceSegment : protected COmfSegmentT<IOmfScopeReferenceSegment>
{
friend class CNewCpntFactory;
protected:
	COmfScopeReferenceSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfScopeReferenceSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfScopeReferenceSegment methods in V-table order.
	STDMETHODIMP GetRelativeScope(__out PDWORD pdwRelativeScope);
	STDMETHODIMP GetRelativeSlot(__out PDWORD pdwRelativeSlot);
};

//*********************************************************************************************************************
//	OMF Timecode Class object (TCCP).
//*********************************************************************************************************************
class COmfTimecodeSegment : protected COmfSegmentT<IOmfTimecodeSegment>
{
friend class CNewCpntFactory;
protected:
	COmfTimecodeSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfTimecodeSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfTimecodeSegment methods in V-table order.
	STDMETHODIMP IsDrop(void);
	STDMETHODIMP GetFramesPerSec(__out PWORD pwFPS);
	STDMETHODIMP GetStart(__out PINT64 pStart);
};

//*********************************************************************************************************************
//	OMF Transition Class object (TRAN).
//*********************************************************************************************************************
class COmfTransComponent : protected COmfComponentT<IOmfTransition>
{
friend class CNewCpntFactory;
protected:
	COmfTransComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfComponentT<IOmfTransition>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfTransComponent methods in V-table order.
	STDMETHODIMP GetCutPoint(__out PINT64 pCutPoint);
	STDMETHODIMP GetEffect(__in REFIID riid, __out PVOID *ppvOut);
};

//*********************************************************************************************************************
//	OMF Varying Value Class object (VVAL).
//*********************************************************************************************************************
class COmfVaryingValueSegment : protected COmfSegmentT<IOmfVaryingValueSegment>
{
friend class CNewCpntFactory;
protected:
	COmfVaryingValueSegment(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfSegmentT<IOmfVaryingValueSegment>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
	virtual ~COmfVaryingValueSegment(void)
	{
		FreeObjRefArray(&m_pPointList);
	}

	// IOmfVaryingValue methods in V-table order.
	STDMETHODIMP GetParameterID(__out LPGUID pGuid);
	STDMETHODIMP GetInterpolationKind(__out PWORD pwInterpKind);
	STDMETHODIMP GetExtrapolationKind(__out PWORD pwExtrapKind);
	STDMETHODIMP GetFieldCount(__out PWORD pwFieldCount);
	STDMETHODIMP IterateControlPoints(__out IOmfooIterator **ppIterator);

private:
	POMFOO_OBJREF_ARRAY	m_pPointList;
};

//*********************************************************************************************************************
//	OMF Track Group Class object (TRKG).
//*********************************************************************************************************************
class COmfTrackGroupComponent : protected COmfTrackGroupComponentT<IOmfTrackGroupComponent>
{
friend class CNewCpntFactory;
protected:
	COmfTrackGroupComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfTrackGroupComponentT<IOmfTrackGroupComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Time Warp Class object (WARP).
//*********************************************************************************************************************
class COmfTimeWarpComponent : protected COmfTimeWarpComponentT<IOmfTimeWarpComponent>
{
friend class CNewCpntFactory;
protected:
	COmfTimeWarpComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfTimeWarpComponentT<IOmfTimeWarpComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
};

//*********************************************************************************************************************
//	OMF Video Frame Mask Class object (MASK).
//*********************************************************************************************************************
class COmfVideoMaskComponent : protected COmfTimeWarpComponentT<IOmfVideoMaskComponent>
{
friend class CNewCpntFactory;
protected:
	COmfVideoMaskComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfTimeWarpComponentT<IOmfVideoMaskComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfVideoMaskComponent methods in V-table order.
	// OMFI:MASK:IsDouble
	STDMETHODIMP IsDouble(void);

	// OMFI:MASK:MaskBits
	STDMETHODIMP GetMaskBits(__out PDWORD pdwMaskBits);
};

//*********************************************************************************************************************
//	OMF Video Repeat Class object (REPT).
//*********************************************************************************************************************
class COmfVideoRepeatComponent : protected COmfTimeWarpComponentT<IOmfVideoRepeatComponent>
{
friend class CNewCpntFactory;
protected:
	COmfVideoRepeatComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfTimeWarpComponentT<IOmfVideoRepeatComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// IOmfVideoRepeatComponent methods in V-table order.
	// (No additional properties.)
};

//*********************************************************************************************************************
//	OMF Video Speed Class object (SPED).
//*********************************************************************************************************************
class COmfVideoSpeedComponent : protected COmfTimeWarpComponentT<IOmfVideoSpeedComponent>
{
friend class CNewCpntFactory;
protected:
	COmfVideoSpeedComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfTimeWarpComponentT<IOmfVideoSpeedComponent>(rBlop, pContainer, pParent, pNewReserved)
	{
	}
	
	// IOmfVideoSpeedComponent methods in V-table order.
	STDMETHODIMP IsFreezeFrame(void);
	STDMETHODIMP GetSpeedAsRational(__out POMF_RATIONAL pSpeed);
	STDMETHODIMP GetSpeedAsFloat(__out PFLOAT pFloat);
	STDMETHODIMP GetSpeedAsDouble(__out DOUBLE* pDouble);
};

// CHECK(omfsNewClass(sess, kClsRequired, kOmfTstRev1x, OMClassSPED, OMClassWARP));
// CHECK(omfsNewClass(sess, kClsRequired, kOmfTstRev1x, OMClassMASK, OMClassWARP));
// CHECK(omfsNewClass(sess, kClsRequired, kOmfTstRev1x, OMClassREPT, OMClassWARP));
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class COmfTrackGroupComponent : protected COmfTrackGroupComponentT<IOmfTrackGroupComponent>
//{
//friend class CNewCpntFactory;
//protected:
//	COmfTrackGroupComponent(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
//		: COmfComponentT<IOmfTrackGroupComponent>(rBlop, pContainer, pParent, pNewReserved)
//	{
//	}
//
//	// IOmfTrackGroupComponent methods in V-table order.
//	STDMETHODIMP IterateTracks(__out IOmfooIterator **ppIterator);
//};
