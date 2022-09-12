// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer96.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer95.h"

class CContainerLayer97;
class COmfObject;
class COmfooIterator;
typedef COmfObject* (__stdcall *PFN_NEW_OMFOO_OOBJ)(BENTO_BLOP& rBlop, CContainerLayer97* pContainer, COmfObject* pParent, PVOID pNewReserved);

#pragma pack(push, 1)	// align structure members to byte boundaries
typedef struct {
	DWORD				dwClassFourCC;
	PFN_NEW_OMFOO_OOBJ	pfnNewInstance;
} NEW_OMFOO_OOBJ_TABLE_ENTRY, *PNEW_OMFOO_OOBJ_TABLE_ENTRY, *NEW_OMFOO_OOBJ_TABLE;
#pragma pack(pop)

#define NUM_INSTANTIATION_TABLE_ENTRIES	(85)

class CContainerLayer96 : public CContainerLayer95
{
protected:
			CContainerLayer96(void);
	virtual	~CContainerLayer96(void);
//	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

	static NEW_OMFOO_OOBJ_TABLE_ENTRY	m_aInstantiationTable[NUM_INSTANTIATION_TABLE_ENTRIES];
};
