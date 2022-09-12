// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer10.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ContainerLayer09.h"

//*********************************************************************************************************************
//	Structures.
//*********************************************************************************************************************
#pragma pack(push, 1)		// Explicitly align the members of these structures to byte boundaries!
typedef struct {
	BENTO_BLOP	oMDAT;			// a copy of the BENTO_BLOP structure for the Media Data object (MDAT).
	BENTO_BLOP	oSMOB;			// a copy of the BENTO_BLOP structure for the Source Mob (SMOB) that owns this MDAT.
	BENTO_BLOP	oMDES;			// a copy of the BENTO_BLOP for the Source Mob's OMFI:SMOB:MediaDescription object.
	OMF_MOB_ID	mobID;			// the MDAT's OMFI:MDAT:MobID property, matches the SMOB's OMFI:MOBJ:MobID property.
	FLOAT	fSampleRate;		// the media descriptor's OMFI:MDFL:SampleRate property as a 32-bit FLOAT.
	UINT64	cbPayloadOffset;	// physical file position reported by the OMFI:MDAT:xxxxData property.
	UINT64	cbPayloadLength;	// physical data length reported by the OMFI:MDAT:xxxxData property.
	QWORD	qwDuration;			// duration, as reported by the media descriptor's OMFI:MDFL:Length property.
	QWORD	qwFileNameExt;		// ASCII filename extension (up to 8 CHARs) packed into a QWORD. No leading dot.

	DWORD	dwPropMediaData;	// Bento property ID for the MDAT's data property. For example "OMFI:IDAT:ImageData".
//	DWORD	dwKitCodecID;		// the first four characters of the media descriptor's OMFI:MDES:KitCodecID property.
	DWORD	dwCompressFourCC;	// the media descriptor's OMFI:DIDD:DIDCompressMethod property, if it exists.
	CHAR	szCompression[12];	// the media descriptor's OMFI:DIDD:Compression property, if it exists.
	DWORD	dwNonambiguousCompressionFourCC;
	CHAR	szSmobNameExt[6];	// If the SMOB name had a filename extension then we isolate it here including the leading dot.

	BOOLEAN	fIsOMFI;			// the media descriptor's OMFI:MDFL:IsOMFI property.
	BOOLEAN	fMobNameSuffixPHYS;	// TRUE if the SMOB name ended with ".PHYS" (We removed it in szSmobName)
	BOOLEAN	fMobNameCharsOk;	// TRUE if the string in szSmobName consists entirely of valid Win32 filename characters.
	BOOLEAN	fMobNameUnique;		// TRUE if the string in szSmobName is unique, and not like any other.
	BOOLEAN	fSelfContained;		// TRUE if the payload does not require a frame index table or other meta-data.
	BOOLEAN	fPayloadBigEndian;	// TRUE if the payload is big endian. (AIFC, TIFF)
	BOOLEAN	fPayloadConfirmed;	// TRUE if
	BOOLEAN	fIdentifyFailed;	// TRUE if

	WORD	wCategoryInstance;	// our own zero-based category instance number. Used for generating filenames.
	BYTE	bCategory;			// 0 = not detected yet, 1 = "Audio File", 2 = "Video File", 3 = "Media File"
	BOOLEAN	fDupMdatUID;		// TRUE if there are two or more MDATs (including this one) with this same mobID.
	WORD	cClones;			// number of other MDATs that have identical matching offsets and lengths.
	WORD	cNestedMdats;		// number of other MDATs with payloads that fit entirely inside this one.
	WORD	cOverlappedStarts;	// number of other partial MDAT payloads that begin inside this one.
	WORD	cOverlappedEnds;	// number of other partial MDAT payloads that end inside this one.

	CHAR	szSmobName[96];		// OMFI:MOBJ:Name property of the Source Mob (SMOB) w/o filename extension or ".PHYS"
	BYTE	aReserved2[14];		// dummy member to force the size of this structure up to 256 (0x00000100) bytes
} MDAT_CACHE_ENTRY, *PMDAT_CACHE_ENTRY, *MDAT_CACHE_TABLE;
#pragma pack(pop)

//	Enumerated values for the bCategory member of the MDAT_CACHE_ENTRY structure.
enum MDAT_CATEGORY {
	MDAT_CATEGORY_UNDETECTED	= 0,	// catagory not detected yet
	MDAT_CATEGORY_AUDIO_FILE	= 1,	// "Audio File"
	MDAT_CATEGORY_VIDEO_FILE	= 2,	// "Video File"
	MDAT_CATEGORY_OTHER_FILE	= 3,	// Not audio or video. We tried to detect it, but we just don't know.
};

class CContainerLayer10 : public CContainerLayer09
{
protected:
			CContainerLayer10(void);
	virtual	~CContainerLayer10(void);
	STDMETHODIMP	Load(__in PCWSTR pwzFileName);

	// Internal helpers called once per lifetime from Load().
private:
	HRESULT	CreateMdatCacheTable(void);
	HRESULT		CreateOmf1MdatCacheTable(void);
	HRESULT		CreateOmf2MdatCacheTable(void);
	HRESULT	TagDuplicateMdatIDs(void);
	HRESULT	IngestSmobBlops(void);
	HRESULT	IngestMdesBlops(void);
	HRESULT	CollectDataValues(void);
	HRESULT	IngestLinkedDidds(void);

	HRESULT	IngestSmobNames(void);
	HRESULT	CollectSampleRates(void);
	HRESULT	CollectCompressionFourCCs(void);
	HRESULT	CollectCompressionStrings(void);
	HRESULT	SolveCompressionAmbiguity(void);
	HRESULT	DetectOverlappers(void);

protected:
	// Our array of MDAT_CACHE_ENTRY structures.
	MDAT_CACHE_TABLE m_aMdatTable;

	UINT64	m_cbMaxMdatPayload;		// size of the largest MDAT payload rounded up to the next 4096-byte boundary.
	ULONG	m_cMDATs;				// total number of elements in m_aMdatTable.
	ULONG	m_cMdatDupUIDs;			// number of MDATs with duplicate OMFI:MDAT:MobID values.
	DWORD	m_dwPropMdflIsOMFI;		// the Bento objectID of the OMFI:MDFL:IsOMFI property.
	DWORD	m_dwPropMdflLength;		// the Bento objectID of the OMFI:MDFL:Length property.
	WORD	m_cMdatsWithAudio;		// number of MDATs that contain audio only.
	WORD	m_cMdatsWithVideo;		// number of MDATs that contain video (and possibly audio).
	WORD	m_cMdatsWithOther;		// number of MDATs that cannot be categorized as audio or video.
	WORD	m_cSmobsWithNames;		// number of SMOBs with names.
	WORD	m_cSmobsWithExtensions;	// number of SMOBs that have a filename extension as part of their name.
	WORD	m_cSmobsWithSuffixPHYS;	// number of SMOBs that have names that end with the ".PHYS" suffix.
	WORD	m_cSmobsWithUniqueNames;// number of SMOBs that have unique one-of-a-kind raw names.

	// Just for debugging (for now).
	WORD	m_cMdatClones;			// number of MDATs that have the same exact offsets and lengths as other MDATs.
	WORD	m_cNestedMdats;			// number of MDATs that live entirely inside another MDAT.
	WORD	m_cOverlappedStarts;	// number of MDATs that begin inside another MDAT but end elsewhere.
	WORD	m_cOverlappedEnds;		// number of MDATs that end inside another MDAT.
};
