// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfMediaDataT.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once

//*********************************************************************************************************************
//	Template class to implement the methods of IOmfMediaData.
//*********************************************************************************************************************
template <class TBase = IOmfMediaData>
class __declspec(novtable) COmfMediaDataT : protected COmfObject, protected TBase
{
protected:
	ULONG	m_idx;			// our index in CContainerLayer10::m_aMdatTable[], or -1 if we are not a member.
	ULONG	m_nCategory;	// 0 = not detected, 1 = "Audio File", 2 = "Video File", 3 = "Media File"

	COmfMediaDataT(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
		: COmfObject(rBlop, pContainer, pParent, pNewReserved)
		, m_idx(-1)
		, m_nCategory(0)
	{
		// Retrieve our position in the OMFI:HEAD:MediaData (or OMFI:MediaData) array. It was stored in
		// the wMdatIdx member of our BENTO_BLOP structure during the Container's initialization routines.
		// This index is stored as a 1-based value so that we can accommodate MDATs that are not members
		// of those arrays. If we subtract 1 from zero it will underflow to 0xFFFFFFFF. Then, all of our
		// subsequent calls to routines like CContainerLayer11::ExtractMdatDataToFile(m_idx) will safely
		// fail using just one simple compare operation.
		m_idx = ULONG(this->wMdatIdx)-1;

		// Get a local copy of our MDAT_CATEGORY.
		// If this MDAT is not a member of OMFI:HEAD:MediaData (or OMFI:MediaData) then m_idx will be 0xFFFFFFFF
		// and so GetMdatCategory() will safely return MDAT_CATEGORY_UNDETECTED.
		m_nCategory	= m_pContainer->GetMdatCategory(m_idx);
	}

	//*****************************************************************************************************************
	// INonDelegatingUnknown
	//*****************************************************************************************************************
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
	{
		HRESULT hr = VerifyIID_PPV_ARGS(riid, ppvOut);
		if (SUCCEEDED(hr))
		{
			if ((riid == __uuidof(IOmfMediaData))||
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
	// Returns S_OK if the subclass's payload contains audio (but no video), or S_FALSE if it does not.
	//*****************************************************************************************************************
	STDMETHODIMP IsAudio(void)
	{
		if (m_nCategory == MDAT_CATEGORY_AUDIO_FILE)	{return S_OK;}
		if ((m_nCategory == MDAT_CATEGORY_VIDEO_FILE)||(m_nCategory == MDAT_CATEGORY_OTHER_FILE))	{return S_FALSE;}
		return E_UNEXPECTED;
	}

	//*****************************************************************************************************************
	// Returns S_OK if the subclass's payload contains video (and perhaps audio too), or S_FALSE if it does not.
	//*****************************************************************************************************************
	STDMETHODIMP IsVideo(void)
	{
		if (m_nCategory == MDAT_CATEGORY_VIDEO_FILE)	{return S_OK;}
		if ((m_nCategory == MDAT_CATEGORY_AUDIO_FILE)||(m_nCategory == MDAT_CATEGORY_OTHER_FILE))	{return S_FALSE;}
		return E_UNEXPECTED;
	}

	//*****************************************************************************************************************
	// OMF1 - Retrieves the OMFI:xxxx:MobID property (where xxxx is the class fourCC of the MDAT's subclass).
	// OMF2 - Retrieves the OMFI:MDAT:MobID property.
	//*****************************************************************************************************************
	STDMETHODIMP GetMobID(__out POMF_MOB_ID pMobID)
	{
		return m_pContainer->GetMdatMobID(m_idx, pMobID);
	}

	//*****************************************************************************************************************
	// Instantiates the Source Mob (SMOB) associated with this Media Data, and then queries it for the interface
	// specified by riid. Returns E_NOINTERFACE if the result does not expose the requested interface.
	//*****************************************************************************************************************
	STDMETHODIMP GetSourceMob(__in REFIID riid, __out PVOID *ppvOut)
	{
		DWORD dwSMOB = m_pContainer->GetMdatSourceMobObject(m_idx);
		return m_pContainer->Instantiate(dwSMOB, static_cast<COmfObject*>(NULL), NULL, riid, ppvOut);
	}

	//*****************************************************************************************************************
	// Instantiates the Media Descriptor (MDES) that describes this data, and then queries it for the interface
	// specified by riid. Returns E_NOINTERFACE if the result does not expose the requested interface.
	//*****************************************************************************************************************
	STDMETHODIMP GetMediaDescriptor(__in REFIID riid, __out PVOID *ppvOut)
	{
		DWORD dwMDES = m_pContainer->GetMdatMediaDescriptorObject(m_idx);
		return m_pContainer->Instantiate(dwMDES, static_cast<COmfObject*>(NULL), NULL, riid, ppvOut);
	}

	//*****************************************************************************************************************
	// Retrieves the raw file position and length for the MDAT's payload and optionally duplicates the file handle.
	//*****************************************************************************************************************
	STDMETHODIMP GetRawFileParams(__out PUINT64 pOffset, __out PUINT64 pLength, __out_opt PHANDLE pHandle)
	{
		return m_pContainer->GetMdatRawFileParams(m_idx, pOffset, pLength, pHandle);
	}

	//*****************************************************************************************************************
	// Creates a Windows Stream object on the payload and queries it for the interface specified by riid.
	//*****************************************************************************************************************
	STDMETHODIMP CreateStreamOnData(__in REFIID riid, __out PVOID *ppvOut)
	{
		return m_pContainer->CreateStreamOnMdatData(m_idx, riid, ppvOut);
	}

	//*****************************************************************************************************************
	// Creates a DirectShow source filter on the payload that can be used with the DirectShow API.
	//*****************************************************************************************************************
	STDMETHODIMP CreateDShowSourceFilterOnData(__in REFIID riid, __out PVOID *ppvOut)
	{
		return m_pContainer->CreateDShowSourceFilterOnData(m_idx, riid, ppvOut);
	}

	//*****************************************************************************************************************
	// Creates a filename (with an appropriate filename extension) that can be used use with ExtractDataToFile().
	//*****************************************************************************************************************
	STDMETHODIMP CreateFileNameForExport(__in DWORD dwSyntax,
											__in ULONG cchBuffer,
												__out_opt PWCHAR pBuffer,
													__out_opt PULONG pcchRequired)
	{
		return m_pContainer->CreateFileNameForMdat(m_idx, dwSyntax, cchBuffer, pBuffer, pcchRequired);
	}

	//*****************************************************************************************************************
	// Extracts/exports the embedded media data to an external file.
	//*****************************************************************************************************************
	STDMETHODIMP ExtractDataToFile(__in PCWSTR pwzFullPathNewFile,
										__in_opt IOmfooExtractCallback *pCallback,
											__in_opt ULONG nPagesPerCallback,
												__in BOOL fOverwrite)
	{
		return m_pContainer->ExtractMdatDataToFile(m_idx, pwzFullPathNewFile, pCallback, nPagesPerCallback, fOverwrite);
	}
};
