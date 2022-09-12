// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer06.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer06.h"
#include "DllMain.h"

#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer06::CContainerLayer06(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer06::~CContainerLayer06(void)
{
	CoreFreeMobIndexArray(&m_pCompositionMobs);
	CoreFreeObjRefArray(&m_pExternalFiles);
	CoreFreeObjRefArray(&m_pObjectSpine);
	CoreFreeMobIndexArray(&m_pSourceMobs);
	CoreFreeMobIndexArray(&m_pV1MediaData);
	CoreFreeObjRefArray(&m_pDefinitionObjects);
	CoreFreeObjRefArray(&m_pPrimaryMobs);
	CoreFreeObjRefArray(&m_pPublicMobs);
	CoreFreeObjRefArray(&m_pV2MediaData);
	CoreFreeObjRefArray(&m_pClassDictionary);
	CoreFreeObjRefArray(&m_pIdentificationList);

	MemFree(m_pRuntimeExtendedClassHierarchyTable);
	m_pRuntimeExtendedClassHierarchyTable = NULL;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer06::Load(PCWSTR pwzFileName)
{
	HRESULT hr = __super::Load(pwzFileName);
	if (SUCCEEDED(hr))
	{
		// Allocate and populate various arrays that stay with us for life.
		if (SUCCEEDED(hr = AllocHeadArrays()))
		{
			// Allocate and populate our internal class-lookup tables.
			if (SUCCEEDED(hr = ParseClassDictionary()))
			{
				// Do more things here if needed!
			}
		}
	}
	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during Load().
//	Allocates and populates crucial object arrays from the HEAD oject and keeps them forever.
//	Each element in the array holds a 32-bit object ID. 
//	Specifically, it allocates and populates the following arrays:
//
//		m_pCompositionMobs		"OMFI:CompositionMobs" - OMF1, optional
//		m_pExternalFiles		"OMFI:ExternalFiles" - OMF1, optional
//		m_pObjectSpine			"OMFI:ObjectSpine" - OMF1, required
//		m_pSourceMobs			"OMFI:SourceMobs" - OMF1, optional
//		m_pV1MediaData			"OMFI:MediaData" - OMF1, optional
//		m_pDefinitionObjects	"OMFI:HEAD:DefinitionObjects" - OMF2, optional
//		m_pPrimaryMobs			"OMFI:HEAD:PrimaryMobs" - OMF2, optional
//		m_pPublicMobs			"OMFI:HEAD:Mobs" - OMF2, required
//		m_pV2MediaData			"OMFI:HEAD:MediaData" - OMF2, optional
//		m_pClassDictionary		"OMFI:ClassDictionary" - OMF1, or OMFI:HEAD:ClassDictionary - OMF2, optional
//		m_pIdentificationList	"OMFI:HEAD:IdentificationList" - OMF1 & OMF2, optional
//
//	Note that this implementaton never fails.
//*********************************************************************************************************************
HRESULT CContainerLayer06::AllocHeadArrays(void)
{
	if (m_fOmfVer1)
	{
		// OMFI:ObjectSpine
		// Create a cache of all OMF objects (but not Bento objects) and save their object IDs in m_pObjectSpine.
		// This array is officially 'required' but we don't need it, and in fact we never even use it.
		// So we don't treat its absence as a fatal error.
		RordAllocObjRefArray(m_aBlopTable[0], ePropRev1ObjectSpine, &m_pObjectSpine);

		// OMFI:CompositionMobs
		// Despite its name, this list also contains master mobs.
		// (In OMF1 all mobs were either source mobs or composition mobs. There was no such thing as a master mob.)
		RordAllocMobIndexArray(m_aBlopTable[0], ePropRev1CompositionMobs, &m_pCompositionMobs);

		// OMFI:ExternalFiles
		// I can’t say much about this because I don’t have any test files with this property!
		// It looks like it was added in OMF Toolkit 2.1, but absent in earlier versions.
		// We don't need it, and we never use it. But if it exists then this routine will read it.
		RordAllocObjRefArray(m_aBlopTable[0], ePropRev1ExternalFiles, &m_pExternalFiles);

		// OMFI:SourceMobs
		// Create a cache of the source mobs and save its pointer in m_pSourceMobs.
		// This array is only available in OMF1 files and it is optional.
		RordAllocMobIndexArray(m_aBlopTable[0], ePropRev1SourceMobs, &m_pSourceMobs);

		// OMFI:MediaData
		// Create a cache of media data objects and save its pointer in m_pV1MediaData.
		// This array is only available in OMF1 files and it is optional.
		RordAllocMobIndexArray(m_aBlopTable[0], ePropRev1MediaData, &m_pV1MediaData);

		// OMFI:ClassDictionary
		// This array is available in OMF1 and OMF2 files.
		// The implementation is identical, but the property has different names.
		RordAllocObjRefArray(m_aBlopTable[0], ePropRev1ClassDictionary, &m_pClassDictionary);
	}
	else
	{
		// OMFI:HEAD:Mobs
		// This array is only available in OMF2 files.
		// It is supposed to be 'required' but we can actually survive without it.
		// Therefore we don't treat its absence as a fatal error.
		RordAllocObjRefArray(m_aBlopTable[0], ePropHeadMobs, &m_pPublicMobs);

		// OMFI:HEAD:DefinitionObjects
		// This array is only available in OMF2 files.
		// It is supposed to be 'required' but we can actually survive without it.
		// Therefore we don't treat its absence as a fatal error.
		RordAllocObjRefArray(m_aBlopTable[0], ePropHeadDefinitionObjects, &m_pDefinitionObjects);

		// OMFI:HEAD:PrimaryMobs
		// This array is only available in OMF2 files and it is optional.
		RordAllocObjRefArray(m_aBlopTable[0], ePropHeadPrimaryMobs, &m_pPrimaryMobs);

		// OMFI:HEAD:MediaData
		// This array is only available in OMF2 files and it is optional.
		RordAllocObjRefArray(m_aBlopTable[0], ePropHeadMediaData, &m_pV2MediaData);

		// OMFI:HEAD:ClassDictionary
		// This array is is optional. It is available in OMF1 and OMF2 files, but it has different names.
		RordAllocObjRefArray(m_aBlopTable[0], ePropHeadClassDictionary, &m_pClassDictionary);
	}

	// OMFI:HEAD:IdentificationList
	// This array is optional. It is available in OMF1 and OMF2 files.
	RordAllocObjRefArray(m_aBlopTable[0], ePropHeadIdentificationList, &m_pIdentificationList);

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during Load().
//	Parses the OMFI:HEAD:ClassDictionary array that we created in AllocHeadArrays().
//	Updates the following ...
//
//	m_pRuntimeExtendedClassHierarchyTable	>> dynamcally allocated memory-based array.
//	m_pImprovisedClassHierarchyTable		>> always points to aOmfImprovisedClassHierarchyTable[].
//	m_pStandardClassHierarchyTable	>> points to aOmf1StandardClassHierarchyTable or aOmf2StandardClassHierarchyTable

//	m_nStandardClasses				> number of elements in m_pStandardClassHierarchyTable[].
//	m_nImprovisedClasses			> number of elements in m_pImprovisedClassHierarchyTable[].
//	m_nRuntimeExtendedClasses		> number of elements in m_pRuntimeExtendedClassHierarchyTable[].
//*********************************************************************************************************************
HRESULT CContainerLayer06::ParseClassDictionary(void)
{
	DWORD	dwRefList			= 0;
	DWORD	dwObjRef			= 0;
	DWORD	dwFourCC			= 0;
	DWORD	dwPropClsdParent	= OrdinalToPropertyID(ePropClsdParentClass);	// OMFI:CLSD:ParentClass

	if (m_fOmfVer1)
	{
		m_pStandardClassHierarchyTable	= m_aOmf1StandardClassHierarchyTable;
		m_nStandardClasses				= ELEMS(m_aOmf1StandardClassHierarchyTable);	// OMF1_NUM_STANDARD_CLASSES
	}
	else
	{
		m_pStandardClassHierarchyTable	= m_aOmf2StandardClassHierarchyTable;
		m_nStandardClasses				= ELEMS(m_aOmf2StandardClassHierarchyTable);	// OMF2_NUM_STANDARD_CLASSES
	}

	// This table is my own invention. It’s a list of non-standard class fourCCs that I’ve discovered by
	// reverse-engineering various OMF files. Each entry contains a class fourCC and its corresponding parent fourCC.
	// Only use this array as a last resort, when the class fourCC isn’t in m_pRuntimeExtendedClassHierarchyTable.
	// We probably don't even need this because m_pRuntimeExtendedClassHierarchyTable seems to work every time.
	m_pImprovisedClassHierarchyTable	= m_aOmfImprovisedClassHierarchyTable;
	m_nImprovisedClasses				= ELEMS(m_aOmfImprovisedClassHierarchyTable);	// OMFOO_NUM_IMPROVISED_CLASSES

	// Does the OMFI:ClassDictionary/OMFI:HEAD:ClassDictionary array exist?
	// Note that this array is 'optional'.
	// Each member of this array is a Class Dictionary Entry object (CLSD).
	if (NULL == m_pClassDictionary)
	{
		// Q: Has this never happened?
		// A: Yes.
		// "F:\Omf Test Files\After Nov 1 2018\Simple 2020 v102.omf"
		// "F:\Omf Test Files\After Nov 1 2018\step v1.02.omf"
		//
		//
		// This is not considered a fatal error.
		//BREAK_IF_DEBUG
		return S_OK;
	}

	// Are the property IDs defined in this file?
	if ((0 == m_dwPropClsdClass) || (0 == dwPropClsdParent))
	{
		BREAK_IF_DEBUG
		return OMF_E_REQUIRED_PROP_MISSING;
	}

	// Calculate memory requirements for m_pRuntimeExtendedClassHierarchyTable.
	ULONG	nElements	= m_pClassDictionary->nElements;
	ULONG	cbAlloc		= nElements * sizeof(CLASS_HIERARCHY_ENTRY);

	// Memory allocated with our allocator is always zeroed.
	m_pRuntimeExtendedClassHierarchyTable	= PCLASS_HIERARCHY_ENTRY(MemAlloc(cbAlloc));

	// Reality check.
	if (NULL == m_pRuntimeExtendedClassHierarchyTable)
	{
		BREAK_IF_DEBUG
		return OMF_E_OUTOFMEMORY;
	}

	// Save the element count.
	m_nRuntimeExtendedClasses = m_pClassDictionary->nElements;

	// For each member in the OMFI:HEAD:ClassDictionary array ...
	for (ULONG i = 0; i < nElements; i++)
	{
		// Get the OMFI:CLSD:ClassID (OMF1) or OMFI:CLSD:Class (OMF2) property for m_pClassDictionary->a[i].
		if (FAILED(CoreReadStrict(GetBlop(m_pClassDictionary->a[i]), m_dwPropClsdClass, (-1), sizeof(DWORD), &dwFourCC)))
		{
			// This property is the class FourCC of the "child".
			// This property is "required" but if it is missing we don’t treat it as a fatal error.
			continue;
		}

		// Save the child's class FourCC in our table.
		m_pRuntimeExtendedClassHierarchyTable[i].dwClassFourCC = dwFourCC;

		// Get the OMFI:CLSD:ParentClass property for m_pClassDictionary->a[i].
		// This is an omfi:ObjRef to the parent CLSD object.
		if (FAILED(CoreReadObjRef(GetBlop(m_pClassDictionary->a[i]), dwPropClsdParent, &dwRefList, &dwObjRef)))
		{
			// This property is "optional" - so we just skip over it if it is missing.
			continue;
		}

		// Get the OMFI:CLSD:ClassID (rev 1) or OMFI:CLSD:Class (rev 2) property from the parent.
		// This property is the class FourCC of the "parent".
		if (FAILED(CoreReadFourCC(GetBlop(dwObjRef), m_dwPropClsdClass, &dwFourCC)))
		{
			// This property is "required" but if it is missing we don’t treat it as a fatal error.
			continue;
		}

		// Save the parent's class FourCC in our table.
		m_pRuntimeExtendedClassHierarchyTable[i].dwParentFourCC = dwFourCC;
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Return S_OK if dwClassFourCC matches one of the standard OMF class fourCCs defined in the OMF spec.
//	Returns S_FALSE if it does not.
//	Returns E_INVALIDARG if dwClassFourCC isn't even a valid fourCC.
//
//	Note that the result depends on the version of the current OMF file. For example if the file is OMF1 and the
//	fourCC is EDEF (the Effect Definition class) then this will return S_FALSE because the Effect Definition class
//	doesn't exist in OMF1.
//*********************************************************************************************************************
HRESULT CContainerLayer06::IsStandardClassFourCC(DWORD dwClassFourCC)
{
	if (IsValidFourCC(dwClassFourCC))
	{
		for (ULONG i = 0; i < m_nStandardClasses; i++)
		{
			if (dwClassFourCC == m_pStandardClassHierarchyTable[i].dwClassFourCC)
			{
				return S_OK;
			}
		}

		return S_FALSE;
	}

	return E_INVALIDARG;
}

//*********************************************************************************************************************
//	Return S_OK if the OMF class represeted by dwClassFourCC inherits the OMF class represeted by dwTypeFourCC.
//	Returns S_FALSE if it does not.
//	Returns E_INVALIDARG if dwClassFourCC or dwTypeFourCC isn't a valid fourCC.
//*********************************************************************************************************************
HRESULT CContainerLayer06::IsClassATypeOf(DWORD dwClassFourCC, DWORD dwCompareFourCC)
{
	if (IsValidFourCC(dwClassFourCC) && IsValidFourCC(dwCompareFourCC))
	{
		DWORD dwCurrentFourCC = dwClassFourCC;
		do {
			if (dwCurrentFourCC == dwCompareFourCC)
			{
				return S_OK;
			}
		} while (dwCurrentFourCC = InternalGetParentClassFourCC(dwCurrentFourCC));
		return S_FALSE;
	}

	return E_INVALIDARG;
}

//*********************************************************************************************************************
//	Tests the object's relationship to a given class.
//	Returns S_OK if the object inherits the given class, or S_FALSE if it does not.
//*********************************************************************************************************************
HRESULT CContainerLayer06::IsObjectATypeOf(DWORD dwObject, DWORD dwClassFourCC)
{
	return IsBlopATypeOf(GetBlop(dwObject), dwClassFourCC);
}

HRESULT CContainerLayer06::IsBlopATypeOf(BENTO_BLOP& rBlop, DWORD dwClassFourCC)
{
	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (rBlop.dwObject)
		return IsClassATypeOf(rBlop.dwFourCC, dwClassFourCC);
	else
		return OMF_E_OOBJ_NOT_FOUND;
}

//*********************************************************************************************************************
//	Returns the parent class (aka the 'super class') of dwClassFourCC, or zero if not found.
//*********************************************************************************************************************
DWORD CContainerLayer06::InternalGetParentClassFourCC(DWORD dwClassFourCC)
{
	DWORD	dwParentFourCC		= 0;
	DWORD	dwCurrentFourCC		= dwClassFourCC;

	if (IsValidFourCC(dwClassFourCC))
	{
		if (dwCurrentFourCC != FCC('OOBJ'))
		{
			for (ULONG i = 0; i < m_nStandardClasses; i++)
			{
				if (dwCurrentFourCC == m_pStandardClassHierarchyTable[i].dwClassFourCC)
				{
					dwParentFourCC = m_pStandardClassHierarchyTable[i].dwParentFourCC;
					goto L_Exit;
				}
			}

			for (ULONG i = 0; i < m_nRuntimeExtendedClasses; i++)
			{
				if (dwCurrentFourCC == m_pRuntimeExtendedClassHierarchyTable[i].dwClassFourCC)
				{
					dwParentFourCC = m_pRuntimeExtendedClassHierarchyTable[i].dwParentFourCC;
					goto L_Exit;
				}
			}

			for (ULONG i = 0; i < m_nImprovisedClasses; i++)
			{
				if (dwCurrentFourCC == m_pImprovisedClassHierarchyTable[i].dwClassFourCC)
				{
					dwParentFourCC = m_pImprovisedClassHierarchyTable[i].dwParentFourCC;
					goto L_Exit;
				}
			}
		}
	}

L_Exit:
	return dwParentFourCC;
}

//*********************************************************************************************************************
//	Retrieves the first standard class in the dwFourCC's hierarchy, or zero if there is none.
//*********************************************************************************************************************
DWORD CContainerLayer06::InternalGetStandardClassFourCC(DWORD dwClassFourCC)
{
	DWORD	dwCurrentFourCC		= dwClassFourCC;
	ULONG	nRetries			= m_nRuntimeExtendedClasses;
	if (nRetries < m_nImprovisedClasses)
	{
		nRetries = m_nImprovisedClasses;
	}

	if (!IsValidFourCC(dwClassFourCC))
	{
		goto L_Fail;
	}

L_SearchLoop:

	// See if dwCurrentFourCC is listed in the standard class table.
	for (ULONG i = 0; i < m_nStandardClasses; i++)
	{
		if (dwCurrentFourCC == m_pStandardClassHierarchyTable[i].dwClassFourCC)
		{
			goto L_Exit;
		}
	}

	// See if dwCurrentFourCC is listed in the runtime extended class table.
	for (ULONG i = 0; i < m_nRuntimeExtendedClasses; i++)
	{
		if (dwCurrentFourCC == m_pRuntimeExtendedClassHierarchyTable[i].dwClassFourCC)
		{
			if (nRetries)
			{
				nRetries--;
				dwCurrentFourCC = m_pRuntimeExtendedClassHierarchyTable[i].dwParentFourCC;
				goto L_SearchLoop;
			}
			goto L_Fail;
		}
	}

	// See if dwCurrentFourCC is listed in the improvised class table.
	for (ULONG i = 0; i < m_nImprovisedClasses; i++)
	{
		if (dwCurrentFourCC == m_pImprovisedClassHierarchyTable[i].dwClassFourCC)
		{
			if (nRetries)
			{
				nRetries--;
				dwCurrentFourCC = m_pImprovisedClassHierarchyTable[i].dwParentFourCC;
				goto L_SearchLoop;
			}
			goto L_Fail;
		}
	}

L_Fail:
	dwCurrentFourCC = 0;

L_Exit:
	return dwCurrentFourCC;
}

//*********************************************************************************************************************
//	Call this when you don't know the specific name of a property.
//	This queries a BENTO_BLOP for a 32-bit Bento property ID. 
//	It twiddles with the class fourCC that lives in the middle field of most OMF property names.
//	The PropOrdinal serves as a template for building intermediate property names.
//	It builds the first property name using the class fourCC in the BENTO_BLOP.
//	It passes each intermediate string to NameToPropertyID() until it succeeds (until the property exists in the file).
//	Then, if that succeeds, it tests to see if the property exists in the BENTO_BLOP.
//	If it does then it returns that property ID to the caller.
//	If it fails then it finds the parent class fourCC and trys again until it reaches the fourCC in the template.
//
//	I wrote this with MDATs in mind because it's an (almost) guaranteed way to find the property ID of their payload.
//	For example an OMF2 AIFC Audio Data class (AIFC) object's payload property is OMFI:AIFC:AudioData.
//	And an OMF2 WAVE Audio Data class (WAVE) object's payload property is OMFI:WAVE:AudioData.
//	Those are easy because they're documented. But if/when we encounter an extended/undocumented class we have no
//	way knowing for sure the specific property name.
//
//	The Sound Designer II Media Data class (SD2M) is a good example.
//	Its payload property is named OMFI:SD2M:AudioData, but we might not know that.
//	If we pass ePropMdatAudioData as the eTemplate then this routine will find it.
//
//	Here are some real-world examples that might be useful.
//
//		ePropOobj64BitSupport	- OMFI:OOBJ:64BitSupport
//		ePropOobjAMEVersion		- OMFI:OOBJ:AMEVersion
//		ePropOobjMobID			- OMFI:OOBJ:MobID
//		ePropOobjVersion		- OMFI:OOBJ:Version
//		ePropLocrFileNam		- OMFI:LOCR:FileName
//		ePropLocrPathName		- OMFI:LOCR:PathName
//		ePropMdatData			- OMFI:MDAT:Data
//		ePropMdatAudioData		- OMFI:MDAT:AudioData
//		ePropMdatImageData		- OMFI:MDAT:ImageData
//
//*********************************************************************************************************************
DWORD CContainerLayer06::AutoProp(__in BENTO_BLOP& rBlop, __in PropOrdinal eTemplate)
{
	CHAR	szWorkspace[BENTO_STRMAX_UNIQUE_NAME]	= {0};
	DWORD	dwProperty		= 0;
	DWORD	dwTestFourCC	= 0;

	// Make sure caller's BENTO_BLOP is one of the ones in CReadOmf::m_aBlopTable[] and not CReadOmf::m_oEmptyBlop.
	if (0 == rBlop.dwObject)
	{
		return 0;
	}

	// Make sure caller's BENTO_BLOP has a class fourCC.
	if (!IsValidFourCC(rBlop.dwFourCC))
	{
		return 0;
	}

	// Find the const string associated with the ordinal.
	LPCSTR pszTemplate = OrdinalToPropertyName(eTemplate);
	if (pszTemplate == NULL)
	{
		return 0;
	}

	// Copy the const string into a buffer where we can manipulate it.
	lstrcpyA(szWorkspace, pszTemplate);

	// The middle field must be exactly four characters long.
	// We verify that by testing for colons.
	if ((szWorkspace[4] != ':') || (szWorkspace[9] != ':'))
	{
		return 0;
	}

	dwTestFourCC = rBlop.dwFourCC;
	do
	{
		if (dwTestFourCC == FCC('OOBJ'))
		{
			goto L_HierarchyOkay;
		}
	} while (dwTestFourCC = InternalGetParentClassFourCC(dwTestFourCC));

	// Caller's BENTO_BLOP does not inherit or belong to the class fourCC in caller's eTemplate.
	return 0;

L_HierarchyOkay:

	// Get class fourCC again!
	dwTestFourCC	= rBlop.dwFourCC;
	do
	{
		// Copy the current class fourCC into the center field of the property name string.
		*PDWORD(&szWorkspace[5]) = dwTestFourCC;

		// Call NameToPropertyID() to get the 32-bit propertry ID.
		// If the property is not defined in this file the NameToPropertyID() will return 0.
		dwProperty = NameToPropertyID(szWorkspace);

		// See if the object has the property.
		if (S_OK == CoreIsPropertyPresent(rBlop, dwProperty))
		{
			// If yes then we're done.
			return dwProperty;
		}

		// If the current class fourCC is 'OOBJ' then we've reached the end of the hierarchy.
		if (dwTestFourCC == FCC('OOBJ'))
		{
			return 0;
		}

		// Else get the parent class and try again.
	} while (dwTestFourCC = InternalGetParentClassFourCC(dwTestFourCC));

	return 0;
}

//*********************************************************************************************************************
//	Scans a BENTO_BLOP for the first property having a omfi:DataValue or omfi:VarLenBytes data type.
//	Returns the Bento property ID.
//*********************************************************************************************************************
DWORD CContainerLayer06::FindFirstDataValue(BENTO_BLOP& rBlop)
{
	PTOCX_ITEM pCurItem	= &m_aToc[rBlop.iFirstItem];
	PTOCX_ITEM pEndItem	= &pCurItem[rBlop.iFirstItem];
	do
	{
		if (pCurItem->dwDataType)
		{
			if ((pCurItem->dwDataType == m_dwTypeDataValue)||
				(pCurItem->dwDataType == m_dwTypeVarLenBytes))
			{
				return pCurItem->dwProperty;
			}
		}
	} while (++pCurItem < pEndItem);

	// Not found.
	return 0;
}

