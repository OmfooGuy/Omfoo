// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer06.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer05.h"

class CContainerLayer06 : public CContainerLayer05
{
protected:
			CContainerLayer06(void);
	virtual	~CContainerLayer06(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

public:
	// Class FourCC routines.
	// These return S_OK if the condition is TRUE or S_FALSE if the condition is FALSE.
	HRESULT	IsStandardClassFourCC(DWORD dwClassFourCC);
	HRESULT	IsClassATypeOf(DWORD dwClassFourCC, DWORD dwCompareFourCC);
	HRESULT	IsObjectATypeOf(DWORD dwObject, DWORD dwClassFourCC);
	HRESULT	IsBlopATypeOf(BENTO_BLOP& rBlop, DWORD dwClassFourCC);

	// Miscellaneous weirdo routines.
	DWORD	AutoProp(BENTO_BLOP& rBlop, PropOrdinal ePropTemplate);

	// omfi:DataValue or omfi:VarLenBytes
	DWORD	FindFirstDataValue(BENTO_BLOP& rBlop);

	// More class FourCC routines.
	// Returns a FourCC.
	DWORD	InternalGetParentClassFourCC(DWORD dwFourCC);
	DWORD	InternalGetStandardClassFourCC(DWORD dwFourCC);

private:
	// Private initialization routines called once per lifetime in Load().
	HRESULT	AllocHeadArrays(void);
	HRESULT	ParseClassDictionary(void);

protected:
	// Pre-allocated arrays from the HEAD.
	// These are initialized in AllocHeadArrays().
	POMFOO_MOB_INDEX_ARRAY	m_pCompositionMobs;		// "OMFI:CompositionMobs" - OMF1 only, optional
	POMFOO_OBJREF_ARRAY		m_pExternalFiles;		// "OMFI:ExternalFiles" - OMF1 only, optional
	POMFOO_OBJREF_ARRAY		m_pObjectSpine;			// "OMFI:ObjectSpine" - OMF1 only, required
	POMFOO_MOB_INDEX_ARRAY	m_pSourceMobs;			// "OMFI:SourceMobs" - OMF1 only, optional
	POMFOO_MOB_INDEX_ARRAY	m_pV1MediaData;			// "OMFI:MediaData" - OMF1 only, optional
	POMFOO_OBJREF_ARRAY		m_pDefinitionObjects;	// "OMFI:HEAD:DefinitionObjects" - OMF2 only, optional
	POMFOO_OBJREF_ARRAY		m_pPrimaryMobs;			// "OMFI:HEAD:PrimaryMobs" - OMF2 only, optional
	POMFOO_OBJREF_ARRAY		m_pPublicMobs;			// "OMFI:HEAD:Mobs" - OMF2 only, required
	POMFOO_OBJREF_ARRAY		m_pV2MediaData;			// "OMFI:HEAD:MediaData" - OMF2 only, optional
	POMFOO_OBJREF_ARRAY		m_pClassDictionary;		// "OMFI:ClassDictionary", "OMFI:HEAD:ClassDictionary"
	POMFOO_OBJREF_ARRAY		m_pIdentificationList;	// "OMFI:HEAD:IdentificationList" - OMF1 & OMF2, optional

//
protected:
	// Class tables.
	const CLASS_HIERARCHY_ENTRY*	m_pStandardClassHierarchyTable;
	const CLASS_HIERARCHY_ENTRY*	m_pImprovisedClassHierarchyTable;
	CLASS_HIERARCHY_ENTRY*			m_pRuntimeExtendedClassHierarchyTable;

	ULONG	m_nStandardClasses;				// number of entries in m_pStandardClassHierarchyTable
	ULONG	m_nImprovisedClasses;			// number of entries in m_pImprovisedClassHierarchyTable
	ULONG	m_nRuntimeExtendedClasses;		// number of entries in m_pRuntimeExtendedClassHierarchyTable
};
