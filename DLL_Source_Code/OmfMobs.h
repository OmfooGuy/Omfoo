// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMobs.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "OmfObject.h"
#include "BackDoors.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

class CContainerLayer97;

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfCommonMob.
//*********************************************************************************************************************
template <class TBase = IOmfCommonMob>
class __declspec(novtable) CMobjCoreT : protected COmfObject, protected IMobjBackDoor, protected TBase
{
protected:
	CMobjCoreT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfObject(rBlop, pContainer, pParent, pNewReserved)
		, m_pSlotArray(NULL)
		, m_hrSlotArray(0)
		, m_dwMDES(0)
		, m_dwMdesFourCC(0)
		, m_wMdesMobKind(0)
	{
		m_hrSlotArray = OrdAllocObjRefArray(m_fOmfVer1 ? ePropTrkgTracks : ePropMobjSlots, &m_pSlotArray);
	}

	virtual	~CMobjCoreT(void)
	{
		FreeObjRefArray(&m_pSlotArray);
	}

	// IUnknown methods in V-table order.
	DECLARE_IUNKNOWN

	// IMobjBackDoor methods in V-table order.
	STDMETHODIMP GetSlotIndex(DWORD dwMSLT)
	{
		HRESULT hr = m_hrSlotArray;
		if (SUCCEEDED(hr))
		{
			hr = OMFOO_E_ASSERTION_FAILURE;
			if (m_pSlotArray)
			{
				ULONG nElements = m_pSlotArray->nElements;
				for (ULONG i = 0; i < nElements; i++)
				{
					if (m_pSlotArray->a[i] == dwMSLT)
					{
						return HRESULT(i);
					}
				}
			}
		}
		return hr;
	}

	STDMETHODIMP IsAPrimaryMob(void)
	{
		return m_pContainer->MobIsAPrimaryMob(this->dwObject);
	}

	STDMETHODIMP IsACompositionMob(void)
	{
		return (this->dwFourCC == FCC('CMOB')) ? S_OK : S_FALSE;
	}

	STDMETHODIMP IsAMasterMob(void)
	{
		return (this->dwFourCC == FCC('MMOB')) ? S_OK : S_FALSE;
	}

	STDMETHODIMP IsASourceMob(void)
	{
		return (this->dwFourCC == FCC('SMOB')) ? S_OK : S_FALSE;
	}

	STDMETHODIMP IsAFileMob(void)
	{
		return (m_wMdesMobKind == PT_FILE_MOB) ? S_OK : S_FALSE;
	}

	STDMETHODIMP IsATapeMob(void)
	{
		return (m_wMdesMobKind == PT_TAPE_MOB) ? S_OK : S_FALSE;
	}

	STDMETHODIMP IsAFilmMob(void)
	{
		return (m_wMdesMobKind == PT_FILM_MOB) ? S_OK : S_FALSE;
	}

	STDMETHODIMP IsANagraMob(void)
	{
		return (m_wMdesMobKind == PT_NAGRA_MOB) ? S_OK : S_FALSE;
	}

	STDMETHODIMP GetMobID(POMF_MOB_ID pMobID)
	{
		return OrdReadMobID(ePropMobjMobID, pMobID);
	}

	STDMETHODIMP GetAppCode(PDWORD pdwAppCode)
	{
		return OrdReadUInt32(m_fOmfVer1 ? ePropMobjUsageCode : ePropMobjAppCode, PUINT32(pdwAppCode));
	}

	STDMETHODIMP GetMobName(ULONG cchBuffer, PWCHAR pBuffer, PULONG pcchRequired)
	{
		return OrdReadStringW(m_fOmfVer1 ? ePropCpntName : ePropMobjName, cchBuffer, pBuffer, pcchRequired);
	}

	STDMETHODIMP GetCreationTime(LPFILETIME pFileTime)
	{
		HRESULT hr = OrdReadTimeStampAsFileTime(ePropMobjCreationTime, pFileTime);
		if (FAILED(hr))
		{
			hr = OrdReadTimeStampAsFileTime(ePropMobj_CreationTime, pFileTime);
		}
		// See also the "_MOBCREATIONTIME" attribute in some OMF1 files, as an alternative to "OMFI:MOBJ:_CreationTime"
		return hr;
	}

	STDMETHODIMP GetLastModified(LPFILETIME pFileTime)
	{
		return OrdReadTimeStampAsFileTime(ePropMobjLastModified, pFileTime);
	}

	STDMETHODIMP GetUserAttributeList(REFIID riid, PVOID *ppvOut)
	{
		return OrdInstantiateObjRef(m_fOmfVer1 ? ePropCpntAttributes : ePropMobjUserAttributes, riid, ppvOut);
	}

	STDMETHODIMP IterateSlots(IOmfooIterator **ppIterator)
	{
		if (IsBadWritePointer(ppIterator, sizeof(PVOID)))
			return E_POINTER;

		*ppIterator = NULL;

		if (m_pSlotArray)
		{
			return WrapObjRefArray(m_pSlotArray, ppIterator);
		}
		else
		{
			return m_hrSlotArray;
		}
	}

	//
	POMFOO_OBJREF_ARRAY	m_pSlotArray;
	HRESULT				m_hrSlotArray;

	DWORD	m_dwMDES;
	DWORD	m_dwMdesFourCC;
	WORD	m_wMdesMobKind;
};

//*********************************************************************************************************************
//	COmfCommonMob
//	A complete implementation of IOmfCommonMob.
//*********************************************************************************************************************
class COmfCommonMob : protected CMobjCoreT<IOmfCommonMob>
{
friend class CNewMobjFactory;
private:
	COmfCommonMob(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: CMobjCoreT<IOmfCommonMob>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// INonDelegatingUnknown
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if (riid == __uuidof(IOmfCommonMob))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfCommonMob*>(this));
			}
			else if (riid == __uuidof(IMobjBackDoor))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IMobjBackDoor*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}
};

//*********************************************************************************************************************
//	COmfCompositionMob
//	A complete implementation of IOmfCompositionMob.
//*********************************************************************************************************************
class COmfCompositionMob : protected CMobjCoreT<IOmfCompositionMob>
{
friend class CNewMobjFactory;
private:
	COmfCompositionMob(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: CMobjCoreT<IOmfCompositionMob>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// INonDelegatingUnknown
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfCommonMob))||
				(riid == __uuidof(IOmfCompositionMob)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfCompositionMob*>(this));
			}
			else if (riid == __uuidof(IMobjBackDoor))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IMobjBackDoor*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	// IOmfCompositionMob methods in V-table order.
	STDMETHODIMP GetDefFadeLength(__out PUINT64 pLength)
	{
	//	OMFI:CMOB:DefFadeLength
		return OrdReadLength64(ePropCmobDefFadeLength, pLength);
	}

	STDMETHODIMP GetDefFadeType(__out PWORD pwFadeType)
	{
	//	OMFI:CMOB:DefFadeType
		return OrdReadUInt16(ePropCmobDefFadeType, pwFadeType);
	}

	STDMETHODIMP GetDefFadeEditUnit(__out POMF_RATIONAL pEditUnit)
	{
	//	OMFI:CMOB:DefFadeEditUnit
		return OrdReadRational(ePropCmobDefFadeEditUnit, pEditUnit);
	}

	STDMETHODIMP GetDefFadeEditUnitAsFloat(__out PFLOAT pFloat)
	{
	//	OMFI:CMOB:DefFadeEditUnit
		return OrdReadRationalAsFloat(ePropCmobDefFadeEditUnit, pFloat);
	}

	STDMETHODIMP GetDefFadeEditUnitAsDouble(__out DOUBLE* pDouble)
	{
	//	OMFI:CMOB:DefFadeEditUnit
		return OrdReadRationalAsDouble(ePropCmobDefFadeEditUnit, pDouble);
	}
};

//*********************************************************************************************************************
//	COmfMasterMob
//	A complete implementation of IOmfMasterMob.
//*********************************************************************************************************************
class COmfMasterMob : protected CMobjCoreT<IOmfMasterMob>
{
friend class CNewMobjFactory;
private:
	COmfMasterMob(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: CMobjCoreT<IOmfMasterMob>(rBlop, pContainer, pParent, pNewReserved)
	{
	}

	// INonDelegatingUnknown
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfCommonMob))||
				(riid == __uuidof(IOmfMasterMob)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfMasterMob*>(this));
			}
			else if (riid == __uuidof(IMobjBackDoor))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IMobjBackDoor*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	// IOmfMasterMob methods in V-table order.
	// (No additional methods.)
};

//*********************************************************************************************************************
//	COmfSourceMob
//	A complete implementation of IOmfSourceMob.
//*********************************************************************************************************************
class COmfSourceMob : protected CMobjCoreT<IOmfSourceMob>
{
friend class CNewMobjFactory;
private:
	COmfSourceMob(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: CMobjCoreT<IOmfSourceMob>(rBlop, pContainer, pParent, pNewReserved)
	{
		if (m_fOmfVer1)
		{
			if (SUCCEEDED(OrdReadObjRef(ePropMobjPhysicalMedia, NULL, &m_dwMDES)))
			{
				m_dwMdesFourCC = m_pContainer->GetObjectClassFourCC(m_dwMDES);
				m_pContainer->OrdReadUInt16(m_dwMDES, ePropMdesMobKind, PUINT16(&m_wMdesMobKind));
			}
		}
		else
		{
			if (SUCCEEDED(OrdReadObjRef(ePropSmobMediaDescription, NULL, &m_dwMDES)))
			{
				m_dwMdesFourCC = m_pContainer->GetObjectClassFourCC(m_dwMDES);
			}
		}

		if (m_wMdesMobKind == 0)
		{
			if (S_OK == m_pContainer->IsClassATypeOf(m_dwMdesFourCC, FCC('MDFL')))
			{
				m_wMdesMobKind = PT_FILE_MOB;
			}
			else if (S_OK == m_pContainer->IsClassATypeOf(m_dwMdesFourCC, FCC('MDTP')))
			{
				m_wMdesMobKind = PT_TAPE_MOB;
			}
			else if (S_OK == m_pContainer->IsClassATypeOf(m_dwMdesFourCC, FCC('MDFM')))
			{
				m_wMdesMobKind = PT_FILM_MOB;
			}
			else if (S_OK == m_pContainer->IsClassATypeOf(m_dwMdesFourCC, FCC('MDNG')))
			{
				m_wMdesMobKind = PT_NAGRA_MOB;
			}
		}
	}

	// INonDelegatingUnknown
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfCommonMob))||
				(riid == __uuidof(IOmfSourceMob)))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IOmfSourceMob*>(this));
			}
			else if (riid == __uuidof(IMobjBackDoor))
			{
				NonDelegatingAddRef();
				*ppvOut = LPUNKNOWN(static_cast<IMobjBackDoor*>(this));
			}
			else
			{
				hr = COmfObject::NonDelegatingQueryInterface(riid, ppvOut);
			}
		}
		return hr;
	}

	// IOmfSourceMob methods in V-table order.
	STDMETHODIMP GetMediaDescriptor(__in REFIID riid, __out PVOID *ppvOut)
	{
		return m_pContainer->Instantiate(m_dwMDES, this, NULL, riid, ppvOut);
	}

	STDMETHODIMP GetMediaData(__in REFIID riid, __out PVOID *ppvOut)
	{
		DWORD dwMDAT = m_pContainer->BlopFindMdatFromSmob(*this);
		return m_pContainer->Instantiate(dwMDAT, this, NULL, riid, ppvOut);
	}

	STDMETHODIMP GetPreviousGenerationMob(__in REFIID riid, __out PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if (m_wMdesMobKind == PT_FILE_MOB)
			{
				DWORD dwPrevious = 0;
				hr = m_pContainer->BlopFindPreviousGenerationMob(*this, &dwPrevious);
				if (S_OK == hr)
				{
					hr = m_pContainer->Instantiate(dwPrevious, static_cast<COmfObject*>(NULL), NULL, riid, ppvOut);
				}
			}
			else
			{
				hr = OMF_E_MOB_NOT_FOUND;
			}
		}
		return hr;
	}
};
