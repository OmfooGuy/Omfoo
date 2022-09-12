// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer08.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer07.h"

class CContainerLayer08 : public CContainerLayer07
{
friend class COmfHeader;
protected:
			CContainerLayer08(void);
	virtual	~CContainerLayer08(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

private:
	HRESULT	InventoryAppCodes(void);
	HRESULT	IngestRev1SourceMobs(void);
	HRESULT	IngestRev1CompositionMobs(void);
	HRESULT	DecideRev1TopLevelMob(void);

	HRESULT	CountAllSourceMobs(void);
	HRESULT	CountAllCompositionMobs(void);
	HRESULT	CountAllMasterMobs(void);
	HRESULT	CountAllGenericMobs(void);

	HRESULT	CountRev2PrimaryMobs(void);
	HRESULT	CountRev2PublicMobs(void);
	HRESULT	TraverseSourceClips(void);
	HRESULT	DecideRev2TopLevelMob(void);

private:
	DWORD	m_dwAppCodesExistMask;	// each bit indicates that the corresponding code is used somewhere in this file
	DWORD	m_dwCMobAppCodeBitMask;	// each bit indicates that the corresponding code is used by CMOBs
	DWORD	m_dwMMobAppCodeBitMask;	// each bit indicates that the corresponding code is used by MMOBs
	DWORD	m_dwSMobAppCodeBitMask;	// each bit indicates that the corresponding code is used by SMOBs
	DWORD	m_dwGMobAppCodeBitMask;	// each bit indicates that the corresponding code is used by MOBJs

	ULONG	m_aAppCodeCounters[16];	// Counters to count the number of mobs having a specific AppCode/UsageCode.
	ULONG	m_nUnknownAppCodes;		// Total number of mobs that have an AppCode/UsageCode value greater than 15.

	ULONG	m_nTotalMobs;			// Total number of all mobs (CMOBs, MMOBs, SMOBs, and MOBJs) in this file.
	ULONG	m_nGenericMobs;			// Total number of Generic Mobs (MOBJ) in this file.
	ULONG	m_nSourceMobs;			// Total number of Source Mobs (SMOB) in this file.
	ULONG	m_nMasterMobs;			// Total number of Master Mobs (MMOB) in this file.
	ULONG	m_nCompositionMobs;		// Total number of Composition Mobs (CMOB) in this file.

	ULONG	m_nPrimarySMobs;		// Number of Source Mobs (SMOB) in OMFI:HEAD:PrimaryMobs.
	ULONG	m_nPrimaryMMobs;		// Number of Master Mobs (MMOB) in OMFI:HEAD:PrimaryMobs.
	ULONG	m_nPrimaryCMobs;		// Number of Composition Mobs (CMOB) in OMFI:HEAD:PrimaryMobs.

	ULONG	m_nPublicSMobs;			// Number of Source Mobs (SMOBs) in OMFI:HEAD:Mobs.
	ULONG	m_nPublicMMobs;			// Number of Master Mobs (MMOBs) in OMFI:HEAD:Mobs.
	ULONG	m_nPublicCMobs;			// Number of Composition Mobs (CMOBs) in OMFI:HEAD:Mobs.

protected:
	DWORD	m_dwTopLevelMob;
};
