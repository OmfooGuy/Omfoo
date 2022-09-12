// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer09.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer08.h"

class CContainerLayer09 : public CContainerLayer08
{
protected:
			CContainerLayer09(void);
	virtual	~CContainerLayer09(void);
//	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

public:
	STDMETHODIMP	MobIsAPrimaryMob(DWORD dwMOBJ);
	STDMETHODIMP	BlopFindPreviousGenerationMob(BENTO_BLOP& rBlop, __out PDWORD pdwSMOB);
private:
	STDMETHODIMP	BlopFindPrevGenMobRev1(BENTO_BLOP& rBlop, __out PDWORD pdwSMOB);
	STDMETHODIMP	BlopFindPrevGenMobRev2(BENTO_BLOP& rBlop, __out PDWORD pdwSMOB);
};

