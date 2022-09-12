// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer98.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer97.h"

class CContainerLayer98 : public CContainerLayer97
{
protected:
			CContainerLayer98(void);
	virtual	~CContainerLayer98(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);	
};
