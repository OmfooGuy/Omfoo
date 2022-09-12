// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: OmfHeader.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer97.h"
#include "OmfHeader.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Static creation stub for CContainerLayer96::m_aInstantiationTable[] called by CContainerLayer97::NewObjectSwitch().
//	This one is for the OMF Header class object (HEAD).
//*********************************************************************************************************************
COmfObject* COmfHeader::NewInstance(BENTO_BLOP& rBlop,
											CContainerLayer97* pContainer,
											COmfObject* pParent,
											PVOID pNewReserved)
{
	return new COmfHeader(rBlop, pContainer, pParent, pNewReserved);
}

//*********************************************************************************************************************
//	Constructor
//*********************************************************************************************************************
COmfHeader::COmfHeader(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved)
	: COmfObject(rBlop, pContainer, pParent, pNewReserved)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
COmfHeader::~COmfHeader(void)
{
}

//*********************************************************************************************************************
//	INonDelegatingUnknown
//*********************************************************************************************************************
HRESULT COmfHeader::NonDelegatingQueryInterface(REFIID riid, PVOID *ppvOut)
{
	HRESULT	hr = VerifyIID_PPV_ARGS(riid, ppvOut);
	if (SUCCEEDED(hr))
	{
		if (riid == __uuidof(IOmfHeader))
		{
			NonDelegatingAddRef();
			*ppvOut = LPUNKNOWN(static_cast<IOmfHeader*>(this));
			return S_OK;
		}

		hr = __super::NonDelegatingQueryInterface(riid, ppvOut);
	}

	return hr;
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:ByteOrder property.
//	OMF2 - Retrieves the OMFI:HEAD:ByteOrder property.
//*********************************************************************************************************************
HRESULT COmfHeader::GetByteOrder(__out PWORD pwByteOrder)
{
	return OrdReadUInt16(m_fOmfVer1 ? ePropRev1ByteOrder : ePropHeadByteOrder, pwByteOrder);
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:Version property.
//	OMF2 - Retrieves the OMFI:HEAD:Version property.
//*********************************************************************************************************************
HRESULT COmfHeader::GetVersion(__in BOOL fVerbatim, __out POMF_VERSIONTYPE pVersionType)
{
	if (fVerbatim)
	{
		return OrdReadVersionType(m_fOmfVer1 ? ePropRev1Version : ePropHeadVersion, pVersionType);
	}
	else
	{
		// Validate caller's pointer.
		if (IsBadWritePointer(pVersionType, sizeof(OMF_VERSIONTYPE)))
		{
			return E_POINTER;
		}
		else
		{
			*pVersionType = m_pContainer->m_omfVersion;
			return S_OK;
		}
	}
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:ToolkitVersion property.
//	OMF2 - Retrieves the OMFI:HEAD:ToolkitVersion property.
//*********************************************************************************************************************
HRESULT COmfHeader::GetToolkitVersion(__out POMF_PRODUCT_VERSION pVersion)
{
	HRESULT hr = E_POINTER;

	if (IsBadWritePointer(pVersion, sizeof(OMF_PRODUCT_VERSION)))
	{
		goto L_Exit;
	}
	else
	{
		pVersion->wMajor	= 0;
		pVersion->wMinor	= 0;
		pVersion->wTertiary	= 0;
		pVersion->wPatch	= 0;
		pVersion->wDetail	= 0;
	}

	if (m_fOmfVer1)
	{
		OMF_VERSIONTYPE omfVersionType = {0};

		// OMFI:ToolkitVersion, omfi:VersionType
		if (SUCCEEDED(hr = OrdReadVersionType(ePropRev1ToolkitVersion, &omfVersionType)))
		{
			pVersion->wMajor	= omfVersionType.bMajor;
			pVersion->wMinor	= omfVersionType.bMinor;
		}
	}
	else
	{
		// OMFI:HEAD:ToolkitVersion, omfi:Int32Array
		UINT32	aVersion[3]		= {0};
		ULONG	nActualElements	= 0;

		if (SUCCEEDED(hr = OrdReadInt32Array(ePropHeadToolkitVersion, ULONG(3), aVersion, &nActualElements)))
		{
			pVersion->wMajor	= WORD(aVersion[0]);
			pVersion->wMinor	= WORD(aVersion[1]);
			pVersion->wTertiary	= WORD(aVersion[2]);
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	OMF1 - Retrieves the OMFI:LastModified property.
//	OMF2 - Retrieves the OMFI:HEAD:LastModified property.
//*********************************************************************************************************************
HRESULT COmfHeader::GetLastModified(__out LPFILETIME pFileTime)
{
	return OrdReadTimeStampAsFileTime(m_fOmfVer1 ? ePropRev1LastModified : ePropHeadLastModified, pFileTime);
}
	
//*********************************************************************************************************************
//	OMF1 & OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:IdentificationList array.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateIdentificationList(__out IOmfooIterator **ppIterator)
{
	return WrapObjRefArray(m_pContainer->m_pIdentificationList, ppIterator);
}

//*********************************************************************************************************************
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:ClassDictionary array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:ClassDictionary array.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateClassDictionary(__out IOmfooIterator **ppIterator)
{
	return WrapObjRefArray(m_pContainer->m_pClassDictionary, ppIterator);
}

//*********************************************************************************************************************
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:MediaData array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:MediaData array.
//	Note - We do not allocate the OMF_OBJREF_ARRAY ourself.
//	The iterator we create uses the m_pV1MediaData or m_pV2MediaData member of CContainerLayer01
//	which is allocated during CContainerLayer01's initialization stage and freed in its destructor.
//	This differs from our normal convention where the COmfObject-derived class maintains its arrays
//	internally and keeps them until the object is destroyed.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateMediaData(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return WrapMobIndexArray(m_pContainer->m_pV1MediaData, ppIterator);
	}
	else
	{
		return WrapObjRefArray(m_pContainer->m_pV2MediaData, ppIterator);
	}
}

//*********************************************************************************************************************
//	OMF1 - Searches the OMFI:MediaData array for the media data object (MDAT) specified by pMobID.
//	OMF2 - Searches the OMFI:HEAD:MediaData array for the media data object (MDAT) specified by pMobID.
//*********************************************************************************************************************
HRESULT COmfHeader::FindMediaData(__in POMF_MOB_ID pMobID, __in REFIID riid, __out PVOID *ppvOut)
{
	// Validate caller's pointer.
	if (IsBadWritePointer(ppvOut, sizeof(PVOID)))
		return E_POINTER;

	// Wipe it now.
	*ppvOut = NULL;

	// Validate caller's IID.
	if (IsBadReadPointer(&riid, sizeof(IID)))
		return E_INVALIDARG;

	if (IsBadReadPointer(pMobID, sizeof(OMF_MOB_ID)))
		return E_POINTER;

	//if ((0 == pMobID->dwMinor) && (0 == pMobID->dwMajor) && (0 == pMobID->dwPrefix))
	//{
	//	return E_INVALIDARG;
	//}

	DWORD dwMDAT = m_pContainer->FindMdatFromMobID(*pMobID);
	return m_pContainer->Instantiate(dwMDAT, this, NULL, riid, ppvOut);
}

//*********************************************************************************************************************
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:ExternalFiles array.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2 because there is no equivalent property in OMF2.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateExternalFiles(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return WrapObjRefArray(m_pContainer->m_pExternalFiles, ppIterator);
	}
	else
	{
		return OMF_E_NOT_IN_OMF2;
	}
}

//*********************************************************************************************************************
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:CompositionMobs array.
//	OMF2 - Creates an iterator to instantiate all composition mobs in the container.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateCompositionMobs(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return WrapMobIndexArray(m_pContainer->m_pCompositionMobs, ppIterator);
	}
	else
	{
		return OMF_E_NOT_IN_OMF2;
	}
}

//*********************************************************************************************************************
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:SourceMobs array.
//	OMF2 - Creates an iterator to instantiate all source mobs in the container.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateSourceMobs(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return WrapMobIndexArray(m_pContainer->m_pSourceMobs, ppIterator);
	}
	else
	{
		return m_pContainer->IterateObjects(FCC('SMOB'), TRUE, ppIterator);
	}
}

//*********************************************************************************************************************
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:ObjectSpine array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:Mobs array.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateMobSpine(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return WrapObjRefArray(m_pContainer->m_pObjectSpine, ppIterator);
	}
	else
	{
		return WrapObjRefArray(m_pContainer->m_pPublicMobs, ppIterator);
	}
}

//*********************************************************************************************************************
//	OMF1 - Returns OMF_E_NOT_IN_OMF1 because there are no Data Definition Class objects in OMF1.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:DefinitionObjects array.
//*********************************************************************************************************************
HRESULT COmfHeader::IterateDefinitionObjects(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return OMF_E_NOT_IN_OMF1;
	}
	else
	{
		return WrapObjRefArray(m_pContainer->m_pDefinitionObjects, ppIterator);
	}
}

//*********************************************************************************************************************
//	OMF1 - Returns OMF_E_NOT_IN_OMF1 because there is no equivalent array in OMF1.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:PrimaryMobs array.
//*********************************************************************************************************************
HRESULT COmfHeader::IteratePrimaryMobs(__out IOmfooIterator **ppIterator)
{
	if (m_fOmfVer1)
	{
		return OMF_E_NOT_IN_OMF1;
	}
	else
	{
		return WrapObjRefArray(m_pContainer->m_pPrimaryMobs, ppIterator);
	}
}

//*********************************************************************************************************************
//	OMF1 & OMF2 - Instantiates the Mob with the specified name and then queries for the interface specified by riid.
//*********************************************************************************************************************
HRESULT COmfHeader::FindNthMobByName(__in LPCSTR pszMobName, __in ULONG nInstance,
											__in_opt DWORD dwClassFourCC, __in REFIID riid, __out PVOID *ppvOut)
{
	// Validate caller's IID and ppvOut.
	HRESULT	hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (SUCCEEDED(hr))
	{
		// Okay to pass dwMOBJ to m_pContainer->Instantiate() even when it's zero. 
		DWORD dwMOBJ = m_pContainer->FindNthMobByName(nInstance, dwClassFourCC, pszMobName);
		if (dwMOBJ)
		{
			hr = m_pContainer->Instantiate(dwMOBJ, static_cast<COmfObject*>(NULL), NULL, riid, ppvOut);
		}
		else
		{
			hr = OMF_E_MOB_NOT_FOUND;
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	OMF1 & OMF2 - Instantiates the Mob with the specified mob ID and then queries for the interface specified by riid.
//*********************************************************************************************************************
HRESULT COmfHeader::FindMobByID(__in POMF_MOB_ID pMobID, __in REFIID riid, __out PVOID *ppvOut)
{
	// Validate caller's IID and ppvOut.
	HRESULT	hr = VerifyIID_PPV_ARGS(riid, ppvOut);

	if (FAILED(hr))
	{
		return hr;
	}

	// Validate caller's POMF_MOB_ID.
	if (IsBadReadPointer(pMobID, sizeof(OMF_MOB_ID)))
		return E_POINTER;

	if ((0 == pMobID->dwMinor) && (0 == pMobID->dwMajor) && (0 == pMobID->dwPrefix))
	{
		return E_INVALIDARG;
	}

	DWORD dwMOBJ = m_pContainer->FindMobjFromMobID(*pMobID);
	if (0 == dwMOBJ)
	{
		return OMF_E_MOB_NOT_FOUND;
	}

	return m_pContainer->Instantiate(dwMOBJ, static_cast<COmfObject*>(NULL), NULL, riid, ppvOut);
}

//*********************************************************************************************************************
//	OMF1 & OMF2 - Find the file's primary Composition Mob.
//	Returns E_NOINTERFACE if it does not expose the requested interface.
//*********************************************************************************************************************
HRESULT COmfHeader::FindTopLevelMob(__in REFIID riid, __out PVOID *ppvOut)
{
	DWORD dwMOBJ = m_pContainer->m_dwTopLevelMob;
	return m_pContainer->Instantiate(dwMOBJ, this, NULL, riid, ppvOut);
}

