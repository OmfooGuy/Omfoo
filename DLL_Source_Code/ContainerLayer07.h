// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer07.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer06.h"

class CContainerLayer07 : public CContainerLayer06
{
protected:
			CContainerLayer07(void);
	virtual	~CContainerLayer07(void);
//	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

public:
//	The function names say it all. 
	DWORD	FindMdatFromMdes(__in DWORD dwMDES);
	DWORD	FindMdatFromSmob(__in DWORD dwSMOB);
	DWORD	FindMdesFromMdat(__in DWORD dwMDAT);
	DWORD	FindMdesFromSmob(__in DWORD dwSMOB);
	DWORD	FindSmobFromMdat(__in DWORD dwMDAT);
	DWORD	FindSmobFromMdes(__in DWORD dwMDES);

	DWORD	BlopFindMdatFromMdes(__in BENTO_BLOP& rMDES);
	DWORD	BlopFindMdatFromSmob(__in BENTO_BLOP& rSMOB);
	DWORD	BlopFindMdesFromMdat(__in BENTO_BLOP& rMDAT);
	DWORD	BlopFindMdesFromSmob(__in BENTO_BLOP& rSMOB);
	DWORD	BlopFindSmobFromMdat(__in BENTO_BLOP& rMDAT);
	DWORD	BlopFindSmobFromMdes(__in BENTO_BLOP& rMDES);

	DWORD	FindMdatFromMobID(__in OMF_MOB_ID& rMobID);
	DWORD	FindMdesFromMobID(__in OMF_MOB_ID& rMobID);
	DWORD	FindSmobFromMobID(__in OMF_MOB_ID& rMobID);
	DWORD	FindSclpFromMobID(__in OMF_MOB_ID& rMobID);
	DWORD	FindMobjFromMobID(__in OMF_MOB_ID& rMobID);

//	Finds the mob ID for the Media Data object in a way that is compatible with both OMF1 and OMF2.
	BOOL	FindMobIDForMdat(__in DWORD dwMDAT, __out POMF_MOB_ID pMobID);
	BOOL	BlopFindMobIDForMdat(__in BENTO_BLOP& rMDAT, __out POMF_MOB_ID pMobID);
	DWORD	FindNthMobByName(__in ULONG nInstance, __in_opt DWORD dwClassFourCC, __in PCSTR pszMobName);

private:
//	Private helpers for FindSmobFromMdes() and BlopFindSmobFromMdes().
	DWORD	BlopFindSmobFromMdes_SearchLoop1(__in BENTO_BLOP& rMDES);
	DWORD	BlopFindSmobFromMdes_SearchLoop2(__in BENTO_BLOP& rMDES);
	DWORD	BlopFindSmobFromMdes_SearchLoop3(__in BENTO_BLOP& rMDES);
	DWORD	BlopFindSmobFromMdes_SearchLoop4(__in BENTO_BLOP& rMDES);
	DWORD	BlopFindSmobFromMdes_SearchLoop5(__in BENTO_BLOP& rMDES);
	DWORD	BlopFindSmobFromMdes_SearchLoop6(__in BENTO_BLOP& rMDES);

//	Private helpers for FindMdatFromMobID() and BlopFindMdatFromMobID().
	DWORD	FindMdatFromMobID_SearchLoop1(__in OMF_MOB_ID& rMobID);
	DWORD	FindMdatFromMobID_SearchLoop2(__in OMF_MOB_ID& rMobID);
	DWORD	FindMdatFromMobID_SearchLoop3(__in OMF_MOB_ID& rMobID);

//	Private helpers for FindSmobFromMobID() and BlopFindSmobFromMobID().
	DWORD	FindSmobFromMobID_SearchLoop1(__in OMF_MOB_ID& rMobID);
	DWORD	FindSmobFromMobID_SearchLoop2(__in OMF_MOB_ID& rMobID);
	DWORD	FindSmobFromMobID_SearchLoop3(__in OMF_MOB_ID& rMobID);
};

