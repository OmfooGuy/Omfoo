// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer98.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer98.h"
#include <shlwapi.h>

#pragma warning(disable:4100)	// unreferenced formal parameter
#pragma warning(disable:4189)	// local variable is initialized but not referenced

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer98::CContainerLayer98(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer98::~CContainerLayer98(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
HRESULT CContainerLayer98::Load(PCWSTR pwzFileName)
{
	// {4C449BAD-C728-4368-8D54-99DB6E718BA3}
//	static const GUID CLSID_FilterWithPin = 
//	{ 0x4c449bad, 0xc728, 0x4368, { 0x8d, 0x54, 0x99, 0xdb, 0x6e, 0x71, 0x8b, 0xa3 } };

	HRESULT hr = __super::Load(pwzFileName);
	if (FAILED(hr))
	{
		goto L_Exit;
	}

/*	TEST TO SEE IF ALL MDATs ARE MEMBERS OF HEAD's MDAT array.
	if (m_fOmfVer1)
	{
		// OMF1 - Test to see if all MDATs in the file are members of the HEAD's OMFI:MediaData array.
		DWORD		dwFourCC	= 0;
		PTOCX_ITEM	pCurItem	= m_aToc;
		PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
		do
		{
			if ((pCurItem->dwProperty == m_dwPropClass)&&
				(pCurItem->dwDataType == m_dwTypeClass)&&
				(pCurItem->bStorageMode == SM_IMMEDIATE))
			{
				dwFourCC = pCurItem->dwImmediateDword;
			}
			else if (pCurItem->dwDataType == m_dwTypeUID)
			{
			}
		} while (++pCurItem < pEndItem);
	}
	else
	{
		// OMF2 - Test to see if all MDATs in the file are members of the HEAD's OMFI:HEAD:MediaData array.
		DWORD		dwPropMdatMobID	= OrdinalToPropertyID(ePropMdatMobID);	// OMFI:MDAT:MobID
		ULONG		cCompare	= 0;
		PTOCX_ITEM	pCurItem	= m_aToc;
		PTOCX_ITEM	pEndItem	= &m_aToc[m_nTocItems];
		do
		{
			if ((pCurItem->dwProperty == dwPropMdatMobID)&&
				(pCurItem->dwDataType == m_dwTypeUID))
			{
				cCompare++;
			}
		} while (++pCurItem < pEndItem);

		if (m_cMDATs != cCompare)
		{
			// Oops!
			// Wtf?
			__debugbreak();
		}
	}
*/

L_Exit:
	return hr;
}

/*
	// Temporary test code to verify CDShowFilterWithPin()
	__debugbreak();
	UINT64 cbOffset	= 0;
	UINT64 cbLength	= 0;

	//CDShowFilterWithPin* pDShowFilterWithPin = new CDShowFilterWithPin(CLSID_FilterWithPin);
	CDShowFilterWithPin* pDShowFilterWithPin = new CDShowMediaSeeking(CLSID_FilterWithPin);
	//CDShowFilterWithPin* pDShowFilterWithPin = new CDShowAsyncReader(CLSID_FilterWithPin);
	if (pDShowFilterWithPin == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto L_Exit;
	}

	if (FAILED(hr = pDShowFilterWithPin->OpenReadableFile(m_pwzFullPath)))
	{
		__debugbreak();
		goto L_Exit;
	}

	if (FAILED(hr = pDShowFilterWithPin->SetRegion(cbOffset, cbLength)))
	{
		__debugbreak();
		goto L_Exit;
	}

	ULONG		nRefs		= 0;
	PDSFILTER	pBaseFilter	= pDShowFilterWithPin->AddRefFilter();
	PDSPIN		pPin		= pDShowFilterWithPin->AddRefPin();

	nRefs = pPin->Release();
	nRefs = pBaseFilter->Release();
	nRefs = pDShowFilterWithPin->SharedRelease();

	// End of temporary test code to verify CDShowFilterWithPin()
*/
