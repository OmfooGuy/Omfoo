// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadOmf_ClassTables.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "ReadOmf_ClassTables.h"

#pragma const_seg(CONSTANT_SEGMENT_NAME)

// There are various C/C++ macros that make it easier to declare FOURCC values in source code.
// This one is barrowed from <Aviriff.h> in the Windows SDK.
#ifndef FCC
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))
#endif

#define MAKE_HIERARCHY_ENTRY(clss, prnt)   {unsigned __int32(FCC(clss)), unsigned __int32(FCC(prnt))}

//*********************************************************************************************************************
//	OMF1 standard class hierarchy table.
//*********************************************************************************************************************
const CLASS_HIERARCHY_ENTRY CReadOmf_ClassTables::m_aOmf1StandardClassHierarchyTable[OMF1_NUM_STANDARD_CLASSES] =
{
	MAKE_HIERARCHY_ENTRY('AIFC','MDAT'),
	MAKE_HIERARCHY_ENTRY('AIFD','MDFL'),
	MAKE_HIERARCHY_ENTRY('ATCP','CLIP'),
	MAKE_HIERARCHY_ENTRY('ATTB','OOBJ'),
	MAKE_HIERARCHY_ENTRY('ATTR','OOBJ'),
	MAKE_HIERARCHY_ENTRY('CDCI','DIDD'),
	MAKE_HIERARCHY_ENTRY('CLIP','CPNT'),
	MAKE_HIERARCHY_ENTRY('CLSD','OOBJ'),
	MAKE_HIERARCHY_ENTRY('CMOB','MOBJ'),	// there is no CMOB in OMF1 but we fake it for convenience.
	MAKE_HIERARCHY_ENTRY('CPNT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('DIDD','MDFL'),
	MAKE_HIERARCHY_ENTRY('DOSL','LOCR'),
	MAKE_HIERARCHY_ENTRY('ECCP','CLIP'),
	MAKE_HIERARCHY_ENTRY('FILL','CLIP'),
	MAKE_HIERARCHY_ENTRY('HEAD','OOBJ'),
	MAKE_HIERARCHY_ENTRY('IDAT','MDAT'),
	MAKE_HIERARCHY_ENTRY('IDNT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('JPEG','IDAT'),
	MAKE_HIERARCHY_ENTRY('LOCR','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MACL','LOCR'),
	MAKE_HIERARCHY_ENTRY('MASK','WARP'),
	MAKE_HIERARCHY_ENTRY('MDAT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MDES','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MDFL','MDES'),
	MAKE_HIERARCHY_ENTRY('MDFM','MDES'),
	MAKE_HIERARCHY_ENTRY('MDNG','MDES'),	// yes, this is defined in OMF1
	MAKE_HIERARCHY_ENTRY('MDTP','MDES'),
	MAKE_HIERARCHY_ENTRY('MMOB','MOBJ'),	// there is no MMOB in OMF1 but we fake it for convenience.
	MAKE_HIERARCHY_ENTRY('MOBJ','TRKG'),
	MAKE_HIERARCHY_ENTRY('NETL','LOCR'),
	MAKE_HIERARCHY_ENTRY('OOBJ', 0),		// there is no OOBJ in OMF1 but we fake it for convenience.
	MAKE_HIERARCHY_ENTRY('REPT','WARP'),
	MAKE_HIERARCHY_ENTRY('RGBA','DIDD'),
	MAKE_HIERARCHY_ENTRY('SCLP','CLIP'),
	MAKE_HIERARCHY_ENTRY('SEQU','CPNT'),
	MAKE_HIERARCHY_ENTRY('SLCT','TRKG'),
	MAKE_HIERARCHY_ENTRY('SMOB','MOBJ'),	// there is no SMOB in OMF1 but we fake it for convenience.
	MAKE_HIERARCHY_ENTRY('SPED','WARP'),
	MAKE_HIERARCHY_ENTRY('TCCP','CLIP'),
	MAKE_HIERARCHY_ENTRY('TIFD','MDFL'),
	MAKE_HIERARCHY_ENTRY('TIFF','MDAT'),
	MAKE_HIERARCHY_ENTRY('TRAK','OOBJ'),
	MAKE_HIERARCHY_ENTRY('TRAN','TRKG'),
	MAKE_HIERARCHY_ENTRY('TRKG','CPNT'),
	MAKE_HIERARCHY_ENTRY('TRKR','CLIP'),
	MAKE_HIERARCHY_ENTRY('TXTL','LOCR'),
	MAKE_HIERARCHY_ENTRY('UNXL','LOCR'),
	MAKE_HIERARCHY_ENTRY('WARP','TRKG'),
	MAKE_HIERARCHY_ENTRY('WAVD','MDFL'),
	MAKE_HIERARCHY_ENTRY('WAVE','MDAT'),
	MAKE_HIERARCHY_ENTRY('WINL','LOCR')
};

//*********************************************************************************************************************
//	OMF2 standard class hierarchy table.
//*********************************************************************************************************************
const CLASS_HIERARCHY_ENTRY CReadOmf_ClassTables::m_aOmf2StandardClassHierarchyTable[OMF2_NUM_STANDARD_CLASSES] =
{
	MAKE_HIERARCHY_ENTRY('AIFC','MDAT'),
	MAKE_HIERARCHY_ENTRY('AIFD','MDFL'),
	MAKE_HIERARCHY_ENTRY('ANIM','OOBJ'),
	MAKE_HIERARCHY_ENTRY('ATTB','OOBJ'),
	MAKE_HIERARCHY_ENTRY('ATTR','OOBJ'),
	MAKE_HIERARCHY_ENTRY('CDCI','DIDD'),
	MAKE_HIERARCHY_ENTRY('CLSD','OOBJ'),
	MAKE_HIERARCHY_ENTRY('CMOB','MOBJ'),
	MAKE_HIERARCHY_ENTRY('CPNT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('CTLP','OOBJ'),
	MAKE_HIERARCHY_ENTRY('CVAL','SEGM'),
	MAKE_HIERARCHY_ENTRY('DDEF','OOBJ'),
	MAKE_HIERARCHY_ENTRY('DIDD','MDFL'),
	MAKE_HIERARCHY_ENTRY('DOSL','LOCR'),
	MAKE_HIERARCHY_ENTRY('DORA','ANIM'),
	MAKE_HIERARCHY_ENTRY('ECCP','SEGM'),
	MAKE_HIERARCHY_ENTRY('EDEF','OOBJ'),
	MAKE_HIERARCHY_ENTRY('EFFE','SEGM'),
	MAKE_HIERARCHY_ENTRY('ERAT','SEGM'),
	MAKE_HIERARCHY_ENTRY('ESLT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('FILL','SEGM'),
	MAKE_HIERARCHY_ENTRY('HEAD','OOBJ'),
	MAKE_HIERARCHY_ENTRY('IDAT','MDAT'),
	MAKE_HIERARCHY_ENTRY('IDNT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('JPEG','IDAT'),
	MAKE_HIERARCHY_ENTRY('LOCR','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MACL','LOCR'),
	MAKE_HIERARCHY_ENTRY('MDAT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MDES','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MDFL','MDES'),
	MAKE_HIERARCHY_ENTRY('MDFM','MDES'),
	MAKE_HIERARCHY_ENTRY('MDTP','MDES'),
	MAKE_HIERARCHY_ENTRY('MGRP','SEGM'),
	MAKE_HIERARCHY_ENTRY('MMOB','MOBJ'),
	MAKE_HIERARCHY_ENTRY('MOBJ','OOBJ'),
	MAKE_HIERARCHY_ENTRY('MSLT','OOBJ'),
	MAKE_HIERARCHY_ENTRY('NEST','SEGM'),
	MAKE_HIERARCHY_ENTRY('NETL','LOCR'),
	MAKE_HIERARCHY_ENTRY('OOBJ', 0),
	MAKE_HIERARCHY_ENTRY('PDWN','SEGM'),
	MAKE_HIERARCHY_ENTRY('RGBA','DIDD'),
	MAKE_HIERARCHY_ENTRY('SCLP','SEGM'),
	MAKE_HIERARCHY_ENTRY('SEGM','CPNT'),
	MAKE_HIERARCHY_ENTRY('SEQU','SEGM'),
	MAKE_HIERARCHY_ENTRY('SLCT','SEGM'),
	MAKE_HIERARCHY_ENTRY('SMOB','MOBJ'),
	MAKE_HIERARCHY_ENTRY('SREF','SEGM'),
	MAKE_HIERARCHY_ENTRY('TCCP','SEGM'),
	MAKE_HIERARCHY_ENTRY('TIFD','MDFL'),
	MAKE_HIERARCHY_ENTRY('TIFF','MDAT'),
	MAKE_HIERARCHY_ENTRY('TRAN','CPNT'),
	MAKE_HIERARCHY_ENTRY('TRKD','OOBJ'),
	MAKE_HIERARCHY_ENTRY('TXTL','LOCR'),
	MAKE_HIERARCHY_ENTRY('UNXL','LOCR'),
	MAKE_HIERARCHY_ENTRY('VVAL','SEGM'),
	MAKE_HIERARCHY_ENTRY('WAVD','MDFL'),
	MAKE_HIERARCHY_ENTRY('WAVE','MDAT'),
	MAKE_HIERARCHY_ENTRY('WINL','LOCR')
};

//*********************************************************************************************************************
//	Improvised, non-standard, class hierarchy table.
//	This is a list of some additional 'extended' classes that I’ve discovered by reverse-engineering various OMF files.
//	Use this table as a last resort. Always try the OMFI:HEAD:ClassDictionary first.
//*********************************************************************************************************************
const CLASS_HIERARCHY_ENTRY CReadOmf_ClassTables::m_aOmfImprovisedClassHierarchyTable[OMFX_NUM_NONSTANDARD_CLASSES] =
{
	MAKE_HIERARCHY_ENTRY('DOML','TXTL'),	// Domain Locator
	MAKE_HIERARCHY_ENTRY('GMFL','MDES'),	// Generic Media File Descriptor
	MAKE_HIERARCHY_ENTRY('JPED','CDCI'),	// JPEG Digital Image Descriptor
	MAKE_HIERARCHY_ENTRY('MFML','TXTL'),	// Avid Media File Manager Locator
	MAKE_HIERARCHY_ENTRY('MPEG','IDAT'),	// MPEG Bitstream Image Data
	MAKE_HIERARCHY_ENTRY('MPGI','CDCI'),	// MPEG Bitstream Image Descriptor
	MAKE_HIERARCHY_ENTRY('MSML','TXTL'),	// Avid Media Stream Manager Locator
	MAKE_HIERARCHY_ENTRY('PVOL','TKFX'),	// OMF1 Pan/Volume effect.
	MAKE_HIERARCHY_ENTRY('QTMD','DIDD'),	// QuickTime Media Descriptor
	MAKE_HIERARCHY_ENTRY('RLED','DIDD'),	// "PRLE" Compressed Media File Descriptor
	MAKE_HIERARCHY_ENTRY('RLE ','IDAT'),	// RLE Compressed Media Data
	MAKE_HIERARCHY_ENTRY('SD2D','MDFL'),	// Sound Designer II Media File Descriptor
	MAKE_HIERARCHY_ENTRY('SD2M','MDAT'),	// Sound Designer II Media Data
	MAKE_HIERARCHY_ENTRY('SDSL','TXTL'),	// Sound Designer II Media File Locator

	MAKE_HIERARCHY_ENTRY('STRB','WARP'),	// OMF1 only
	MAKE_HIERARCHY_ENTRY('TDES','MDES'),	// Tape Descriptor? only found in OMF1 ????
	MAKE_HIERARCHY_ENTRY('TKFX','TRKG'),	// OMF1 only
	MAKE_HIERARCHY_ENTRY('TNFX','TKFX'),	// OMF1 only

//	MAKE_HIERARCHY_ENTRY('CWFL','LOCR'),	// CoreWinFileLocator
//	MAKE_HIERARCHY_ENTRY('CWDL','LOCR'),	// CoreWinDirLocator
//	MAKE_HIERARCHY_ENTRY('WIND','LOCR'),	// WinDirLocator
//	MAKE_HIERARCHY_ENTRY('WINF','LOCR'),	// WinFileLocator
//	MAKE_HIERARCHY_ENTRY('MACD','LOCR'),	// MacDirLocator
//	MAKE_HIERARCHY_ENTRY('FILE','LOCR'),	// MacFileLocator
//	MAKE_HIERARCHY_ENTRY('URLL','LOCR')		// URLLocator
};

//	TODO
//	More OMF2 fourCCs
//	'GFXA','OOBJ'	// Graphics attribute ???

//	More OMF1 fourCCs
//	'ASPI','TKFX'
//	'ASpi','TKFX'
//	'EQBD', none
//	'EQMB','TKFX'
//	'FXPS','SMLS'
//	'LITM', none
//	'MCBR', none	// Media Composer Bin Reference
//	'MCMR',	none	// Media Composer Mob Reference
//	'PVOL','TKFX'	// Pan & Volume
//	'RSET','TRKG'
//	'SMLS', none
//	'STRB','WARP'
//	'TKDA', none
//	'TKDS', none
//	'TKMN', none
//	'TKPA', none
//	'TKPS', none
//	'TMBC', none
//	'TMCS', none

// Restore default segment.
#pragma const_seg()
