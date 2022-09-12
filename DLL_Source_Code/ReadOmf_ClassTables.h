// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadOmf_ClassTables.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once

#pragma pack(push, 1)	// align structure members to byte boundaries
typedef struct {
	DWORD	dwClassFourCC;
	DWORD	dwParentFourCC;
} CLASS_HIERARCHY_ENTRY, *PCLASS_HIERARCHY_ENTRY, *CLASS_HIERARCHY_TABLE;
#pragma pack(pop)

struct CReadOmf_ClassTables
{
protected:
#define OMF1_NUM_STANDARD_CLASSES		(51)
#define OMF2_NUM_STANDARD_CLASSES		(58)
#define OMFX_NUM_NONSTANDARD_CLASSES	(18)

	const static CLASS_HIERARCHY_ENTRY m_aOmf1StandardClassHierarchyTable[OMF1_NUM_STANDARD_CLASSES];
	const static CLASS_HIERARCHY_ENTRY m_aOmf2StandardClassHierarchyTable[OMF2_NUM_STANDARD_CLASSES];
	const static CLASS_HIERARCHY_ENTRY m_aOmfImprovisedClassHierarchyTable[OMFX_NUM_NONSTANDARD_CLASSES];
};
