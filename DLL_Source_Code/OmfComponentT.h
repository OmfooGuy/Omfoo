// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfComponentT.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************

//	ATCP
//	CVAL
//	ECCP
//	EFFE
//	ERAT
//	FILL
//	MASK
//	MGRP
//	NEST
//	PDWN
//	REPT
//	SCLP
//	SEQU
//	SLCT
//	SPED
//	SREF
//	TCCP
//	TRAN
//	TRKG
//	TRKR
//	VVAL
//	WARP

//*********************************************************************************************************************
//	OMF1 Component Class Hierarchy.
//*********************************************************************************************************************
//	CPNT				// abstract, but treated as concrete to support extended classes that might inherit it.
//		CLIP			// abstract, but treated as concrete to support extended classes that might inherit it.
//			ATCP		// supported
//			ECCP		// supported
//			FILL		// supported
//			SCLP		// supported
//			TCCP		// supported - inherits IOmfClipComponent
//			TRKR		// supported 
//		SEQU			// supported
//		TRKG			// abstract, but treated as concrete to support extended classes that might inherit it.
//			TKFX		// custom
//				PVOL	// custom
//				TNFX	// ??? custom ???
//			MOBJ		// supported - exposes IOmfTrackGroup using multiple inheritance
//			TRAN		// supported - inherits IOmfTrackGroup
//			SLCT		// supported - inherits IOmfTrackGroup
//			WARP		// abstract, but treated as concrete to support extended classes that might inherit it.
//				MASK	// supported
//				REPT	// supported
//				SPED	// supported
//				STRB	// ??? custom ???
//
//*********************************************************************************************************************
//	OMF2 Component Class Hierarchy.
//*********************************************************************************************************************
//	CPNT				// abstract, but treated as concrete to support extended classes that might inherit it.
//		SEGM			// abstract, but treated as concrete to support extended classes that might inherit it.
//			CVAL		// supported
//			ECCP		// supported
//			EFFE		// supported
//			ERAT		// supported
//			FILL		// supported
//			MGRP		// supported
//			NEST		// supported
//			PDWN		// supported
//			SREF		// supported
//			SLCT		// supported
//			SEQU		// supported
//			SCLP		// supported
//			TCCP		// supported
//			VVAL		// supported
//		TRAN			// supported
//
//*********************************************************************************************************************


//*********************************************************************************************************************
//	Template class to implement the methods of IOmfComponent.
//	Available in OMF1 and OMF2.
//*********************************************************************************************************************
template <class TBase = IOmfComponent>
class __declspec(novtable) COmfComponentT : protected COmfObject, protected TBase
{
protected:
	COmfComponentT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfObject(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfComponent))||
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
	// OMF1 only - Retrieves the OMFI:CPNT:Name property as a wide character (UTF-16) string.
	//*****************************************************************************************************************
	STDMETHODIMP GetCpntName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
	{
		if (m_fOmfVer1)
		{
			return OrdReadStringW(ePropCpntName, cchBuffer, pBuffer, pcchRequired);
		}
		else
		{
			return OMF_E_NOT_IN_OMF2;
		}
	}

	//*****************************************************************************************************************
	//	OMF1 & OMF2 - Returns S_OK if the component is a transition or S_FALSE if it is not.
	//*****************************************************************************************************************
	STDMETHODIMP IsATransition(void)
	{
		// Call this when we need to know if a CPNT inherits TRAN.
		// Test for the presence of the OMFI:TRAN:CutPoint property.
		// This property is required in both OMF1 and OMF2.
		return OrdIsPropertyPresent(ePropTranCutPoint);
	}

	//*****************************************************************************************************************
	//	OMF1 - Retrieves the component's OMFI:CPNT:EffectID property as a wide character (UTF-16) string.
	//	OMF2 - Retrieves the OMFI:DDEF:DataKindID string from the Data Definition object (DDEF) specified
	//		   in the component's OMFI:CPNT:DataKind property as a wide character (UTF-16) string.
	//*****************************************************************************************************************
	STDMETHODIMP GetDataKindIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)
	{
		if (m_fOmfVer1)
		{
			// OMFI:CPNT:EffectID
			return OrdReadStringW(ePropCpntEffectID, cchBuffer, pBuffer, pcchRequired);
		}
		else
		{
			// OMFI:CPNT:DataKind (omfi:ObjRef) >>> OMFI:DDEF:DataKindID (omfi:UniqueName). 
			return OrdGetDataKindAsStringW(ePropCpntDataKind, cchBuffer, pBuffer, pcchRequired);
		}
	}

	//*****************************************************************************************************************
	// Retrieves the component's data kind. See the OMF_DATAKIND enumeration for details.
	// OMF1 - This comes directly from the OMFI:CPNT:TrackKind property.
	// OMF2 - This comes indirectly from the OMFI:CPNT:DataKind property.
	//*****************************************************************************************************************
	STDMETHODIMP GetDataKindIDWord(__out PWORD pwDataKind)
	{
		if (m_fOmfVer1)
		{
			// OMFI:CPNT:TrackKind
			return OrdReadUInt16(ePropCpntTrackKind, PUINT16(&pwDataKind));
		}
		else
		{
			// OMFI:CPNT:DataKind
			return OrdGetDataKind(ePropCpntDataKind, pwDataKind);
		}
	}

	//*****************************************************************************************************************
	// OMF2 only - Instantiates the ObjRef specified by the OMFI:CPNT:DataKind property.
	//*****************************************************************************************************************
	STDMETHODIMP GetDataDefinitionObject(__in REFIID riid, __out PVOID *ppvOut)
	{
		if (m_fOmfVer1)
		{
			return OMF_E_NOT_IN_OMF1;
		}
		else
		{
			// OMFI:CPNT:DataKind
			return OrdInstantiateObjRef(ePropCpntDataKind, riid, ppvOut);
		}
	}

	//*****************************************************************************************************************
	// OMF1 only - Retrieves the OMFI:CPNT:EditRate property as an OMF_RATIONAL.
	//*****************************************************************************************************************
	STDMETHODIMP GetEditRate(__out POMF_RATIONAL pEditRate)
	{
		if (m_fOmfVer1)
		{
			// OMFI:CPNT:EditRate
			return OrdReadRational(ePropCpntEditRate, pEditRate);
		}
		else
		{
			return OMF_E_NOT_IN_OMF2;
		}
	}

	//*****************************************************************************************************************
	// OMF1 only - Retrieves the OMFI:CPNT:EditRate property as a 32-bit FLOAT.
	//*****************************************************************************************************************
	STDMETHODIMP GetEditRateAsFloat(__out PFLOAT pEditRate)
	{
		if (m_fOmfVer1)
		{
			// OMFI:CPNT:EditRate
			return OrdReadRationalAsFloat(ePropCpntEditRate, pEditRate);
		}
		else
		{
			return OMF_E_NOT_IN_OMF2;
		}
	}

	//*****************************************************************************************************************
	// OMF1 only - Retrieves the OMFI:CPNT:EditRate property as a 64-bit DOUBLE.
	//*****************************************************************************************************************
	STDMETHODIMP GetEditRateAsDouble(__out DOUBLE* pEditRate)
	{
		if (m_fOmfVer1)
		{
			// OMFI:CPNT:EditRate
			return OrdReadRationalAsDouble(ePropCpntEditRate, pEditRate);
		}
		else
		{
			return OMF_E_NOT_IN_OMF2;
		}
	}

	//*****************************************************************************************************************
	// OMF1 - Retrieves the OMFI:CLIP:Length property if the object is a CLIP.
	//		  Retrieves the OMFI:TRKG:GroupLength property if the object is a TRKG.
	//		  If the component is a SEQU then the length is calculated at runtime.
	// OMF2 - Retrieves the OMFI:CPNT:Length property.
	//*****************************************************************************************************************
	STDMETHODIMP GetLength(__out PUINT64 pLength)
	{
		HRESULT hr = (-1);
		if (m_fOmfVer1)
		{
			// OMFI:CLIP:Length
			if (FAILED(hr = OrdReadLength64(ePropClipLength, pLength)))
			{
				// OMFI:TRKG:GroupLength
				hr = OrdReadLength64(ePropTrkgGroupLength, pLength);
			}
		}
		else
		{
			// OMFI:CPNT:Length
			hr = OrdReadLength64(ePropCpntLength, pLength);
		}
		return hr;
	}

	//*****************************************************************************************************************
	// OMF1 only - Instantiates the OMFI:CPNT:Precomputed object.
	//*****************************************************************************************************************
	STDMETHODIMP GetPrecomputed(__in REFIID riid, __out PVOID *ppvOut)
	{
		if (m_fOmfVer1)
		{
			return OrdInstantiateObjRef(ePropCpntPrecomputed, riid, ppvOut);
		}
		else
		{
			return OMF_E_NOT_IN_OMF2;
		}
	}

	//*****************************************************************************************************************
	// OMF1 - Instantiates the OMFI:CPNT:Attributes object and queries for the interface specified by riid.
	// OMF2 - Instantiates the OMFI:CPNT:UserAttributes object and queries for the interface specified by riid.
	//*****************************************************************************************************************
	STDMETHODIMP GetCpntAttributeList(__in REFIID riid, __out PVOID *ppvOut)
	{
		return OrdInstantiateObjRef(m_fOmfVer1 ? ePropCpntAttributes : ePropCpntUserAttributes, riid, ppvOut);
	}

	//*****************************************************************************************************************
	// OMF1 only - Instantiates the OMFI:CPNT:SessionAttrs object and queries for the interface specified by riid.
	//*****************************************************************************************************************
	STDMETHODIMP GetSessionAttributeList(__in REFIID riid, __out PVOID *ppvOut)
	{
		if (m_fOmfVer1)
		{
			return OrdInstantiateObjRef(ePropCpntSessionAttrs, riid, ppvOut);
		}
		else
		{
			return OMF_E_NOT_IN_OMF2;
		}
	}
};

//*********************************************************************************************************************
//	This template class implements the methods of IOmfSegment (and IOmfClipComponent).
//*********************************************************************************************************************
template <class TBase = IOmfSegment>
class __declspec(novtable) COmfSegmentT : protected COmfComponentT<TBase>
{
protected:
	COmfSegmentT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfComponentT<TBase>(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfComponent))||
				(riid == __uuidof(IOmfSegment))||
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
	};
	STDMETHODIMP_(ULONG) AddRef()
	{
		return NonDelegatingAddRef();
	}
	STDMETHODIMP_(ULONG) Release()
	{
		return NonDelegatingRelease();
	}
};

//*********************************************************************************************************************
//	This template class implements the methods of IOmfTrackGroupComponent.
//*********************************************************************************************************************
template <class TBase = IOmfTrackGroupComponent>
class __declspec(novtable) COmfTrackGroupComponentT : protected COmfComponentT<TBase>
{
protected:
	COmfTrackGroupComponentT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfComponentT<TBase>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	//*****************************************************************************************************************
	// Destructor
	//*****************************************************************************************************************
	virtual ~COmfTrackGroupComponentT(void)
	{
		FreeObjRefArray(&m_pTracks);
	}

	//*****************************************************************************************************************
	// INonDelegatingUnknown
	//*****************************************************************************************************************
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfComponent))||
				(riid == __uuidof(IOmfTrackGroupComponent))||
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
	};
	STDMETHODIMP_(ULONG) AddRef()
	{
		return NonDelegatingAddRef();
	}
	STDMETHODIMP_(ULONG) Release()
	{
		return NonDelegatingRelease();
	}

	//*****************************************************************************************************************
	// IOmfTrackGroupComponent
	//*****************************************************************************************************************
	STDMETHODIMP IterateTracks(__out IOmfooIterator **ppIterator)
	{
		HRESULT hr = S_OK;

		// Create the array now if it hasn’t been created yet.
		if (NULL == m_pTracks)
		{
			// OMFI:TRKG:Tracks
			hr = OrdAllocObjRefArray(ePropTrkgTracks, &m_pTracks);
		}

		if (SUCCEEDED(hr))
		{
			hr = WrapObjRefArray(m_pTracks, ppIterator);
		}

		return hr;
	}

private:
	POMFOO_OBJREF_ARRAY	m_pTracks;
};

//*********************************************************************************************************************
//	This template class implements the methods of IOmfTimeWarpComponent.
//*********************************************************************************************************************
template <class TBase = IOmfTimeWarpComponent>
class __declspec(novtable) COmfTimeWarpComponentT : protected COmfTrackGroupComponentT<TBase>
{
protected:
	COmfTimeWarpComponentT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfTrackGroupComponentT<TBase>(rBlop, pContainer, pParent, pNewReserved)
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
			if ((riid == __uuidof(IOmfComponent))||
				(riid == __uuidof(IOmfTrackGroupComponent))||
				(riid == __uuidof(IOmfTimeWarpComponent))||
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
	};
	STDMETHODIMP_(ULONG) AddRef()
	{
		return NonDelegatingAddRef();
	}
	STDMETHODIMP_(ULONG) Release()
	{
		return NonDelegatingRelease();
	}

	// IOmfTimeWarpComponent methods in V-table order.
	STDMETHODIMP GetWarpEditRate(__out POMF_RATIONAL pEditRate)
	{
		// OMFI:WARP:EditRate
		return OrdReadRational(ePropWarpEditRate, pEditRate);
	}

	STDMETHODIMP GetWarpEditRateAsFloat(__out PFLOAT pFloat)
	{
		// OMFI:WARP:EditRate
		return OrdReadRationalAsFloat(ePropWarpEditRate, pFloat);
	}

	STDMETHODIMP GetWarpEditRateAsDouble(__out DOUBLE* pDouble)
	{
		// OMFI:WARP:EditRate
		return OrdReadRationalAsDouble(ePropWarpEditRate, pDouble);
	}

	STDMETHODIMP GetPhaseOffset(__out PWORD pwOffset)
	{
		// OMFI:WARP:PhaseOffset
		return OrdReadInt16(ePropWarpPhaseOffset, PINT16(pwOffset));
	}
};


