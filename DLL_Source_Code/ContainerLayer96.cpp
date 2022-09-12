// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer96.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer96.h"
#include "OmfObject.h"

#include "OmfAttribute.h"
#include "OmfAttributeArray.h"
#include "OmfClassDictionaryEntry.h"
#include "OmfControlPoint.h"
#include "OmfDataDefinition.h"
#include "OmfEffectDefinition.h"
#include "OmfEffectSlot.h"
#include "OmfHeader.h"
#include "OmfIdentity.h"
#include "OmfLocator.h"
#include "OmfMobSlot.h"
#include "OmfTrackDescription.h"

#include "NewCpntFactory.h"
#include "NewMdatFactory.h"
#include "NewMdesFactory.h"
#include "NewMobjFactory.h"

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer96::CContainerLayer96(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer96::~CContainerLayer96(void)
{
}

//*********************************************************************************************************************
//	Add code here if we need to do things immediately after the file is opened.
//*********************************************************************************************************************
//HRESULT CContainerLayer96::Load(PCWSTR pwzFileName)
//{
//	HRESULT hr = __super::Load(pwzFileName);
//	if (SUCCEEDED(hr))
//	{
//		hr = ????????????
//	}
//	return hr;
//}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
NEW_OMFOO_OOBJ_TABLE_ENTRY CContainerLayer96::m_aInstantiationTable[] =
{
	{FCC('AIFC'), CNewMdatFactory::NewAIFC},
	{FCC('AIFD'), CNewMdesFactory::NewAIFD},
	{FCC('ATCP'), CNewCpntFactory::NewATCP},
	{FCC('ATTB'), COmfAttribute::NewInstance},
	{FCC('ATTR'), COmfAttributeArray::NewInstance},
	{FCC('CLSD'), COmfClassDictionaryEntry::NewInstance},
	{FCC('CDCI'), CNewMdesFactory::NewCDCI},
	{FCC('CPNT'), CNewCpntFactory::NewCPNT},
	{FCC('CMOB'), CNewMobjFactory::NewCMOB},
	{FCC('CVAL'), CNewCpntFactory::NewCVAL},
	{FCC('CTLP'), COmfControlPoint::NewInstance},
	{FCC('CWDL'), COmfLocator::NewInstance},
	{FCC('CWFL'), COmfLocator::NewInstance},
	{FCC('DDEF'), COmfDataDefinition::NewInstance},
	{FCC('DIDD'), CNewMdesFactory::NewDIDD},
	{FCC('DOML'), COmfLocator::NewInstance},
	{FCC('DOSL'), COmfLocator::NewInstance},
	{FCC('ECCP'), CNewCpntFactory::NewECCP},
	{FCC('EDEF'), COmfEffectDefinition::NewInstance},
	{FCC('EFFE'), CNewCpntFactory::NewEFFE},
	{FCC('ERAT'), CNewCpntFactory::NewERAT},
	{FCC('ESLT'), COmfEffectSlot::NewInstance},
	{FCC('FILE'), COmfLocator::NewInstance},
	{FCC('FILL'), CNewCpntFactory::NewFILL},
//	{FCC('GFXA), ????},
	{FCC('GMFL'), CNewMdesFactory::NewGMFL},
	{FCC('HEAD'), COmfHeader::NewInstance},
	{FCC('IDNT'), COmfIdentity::NewInstance},
	{FCC('IDAT'), CNewMdatFactory::NewIDAT},
	{FCC('JPED'), CNewMdesFactory::NewJPED},
	{FCC('JPEG'), CNewMdatFactory::NewJPEG},
	{FCC('LOCR'), COmfLocator::NewInstance},
	{FCC('MACD'), COmfLocator::NewInstance},
	{FCC('MACL'), COmfLocator::NewInstance},
	{FCC('MASK'), CNewCpntFactory::NewMASK},
	{FCC('MDAT'), CNewMdatFactory::NewMDAT},
	{FCC('MDES'), CNewMdesFactory::NewMDES},
	{FCC('MDFL'), CNewMdesFactory::NewMDFL},
	{FCC('MDFM'), CNewMdesFactory::NewMDFM},
	{FCC('MDNG'), CNewMdesFactory::NewMDNG},
	{FCC('MDTP'), CNewMdesFactory::NewMDTP},
	{FCC('MFML'), COmfLocator::NewInstance},
	{FCC('MGRP'), CNewCpntFactory::NewMGRP},
	{FCC('MMOB'), CNewMobjFactory::NewMMOB},
	{FCC('MOBJ'), CNewMobjFactory::NewMOBJ},
	{FCC('MPEG'), CNewMdatFactory::NewMPEG},
	{FCC('MPGI'), CNewMdesFactory::NewMPGI},
	{FCC('MSLT'), COmfMobSlot::NewInstance},
	{FCC('MSML'), COmfLocator::NewInstance},
	{FCC('NEST'), CNewCpntFactory::NewNEST},
	{FCC('NETL'), COmfLocator::NewInstance},
	{FCC('OOBJ'), COmfObject::NewInstance},
	{FCC('PDWN'), CNewCpntFactory::NewPDWN},
	{FCC('QTMD'), CNewMdesFactory::NewQTMD},
	{FCC('REPT'), CNewCpntFactory::NewREPT},
	{FCC('RGBA'), CNewMdesFactory::NewRGBA},
	{FCC('RLED'), CNewMdesFactory::NewRLED},
	{FCC('RLE '), CNewMdatFactory::NewRLE_},
	{FCC('SREF'), CNewCpntFactory::NewSREF},
	{FCC('SEGM'), CNewCpntFactory::NewSEGM},
	{FCC('SEQU'), CNewCpntFactory::NewSEQU},
	{FCC('SCLP'), CNewCpntFactory::NewSCLP},
	{FCC('SDSL'), COmfLocator::NewInstance},
	{FCC('SD2D'), CNewMdesFactory::NewSD2D},
	{FCC('SD2M'), CNewMdatFactory::NewSD2M},
	{FCC('SLCT'), CNewCpntFactory::NewSLCT},
	{FCC('SMOB'), CNewMobjFactory::NewSMOB},
	{FCC('SPED'), CNewCpntFactory::NewSPED},
	{FCC('TCCP'), CNewCpntFactory::NewTCCP},
	{FCC('TIFD'), CNewMdesFactory::NewTIFD},
	{FCC('TIFF'), CNewMdatFactory::NewTIFF},
	{FCC('TRAK'), COmfMobSlot::NewInstance},
	{FCC('TRAN'), CNewCpntFactory::NewTRAN},
	{FCC('TRKD'), COmfTrackDescription::NewInstance},
	{FCC('TRKG'), CNewCpntFactory::NewTRKG},
	{FCC('TRKR'), CNewCpntFactory::NewTRKR},
	{FCC('TXTL'), COmfLocator::NewInstance},
	{FCC('UNXL'), COmfLocator::NewInstance},
	{FCC('URLL'), COmfLocator::NewInstance},
	{FCC('VVAL'), CNewCpntFactory::NewVVAL},
	{FCC('WARP'), CNewCpntFactory::NewWARP},
	{FCC('WAVE'), CNewMdatFactory::NewWAVE},
	{FCC('WAVD'), CNewMdesFactory::NewWAVD},
	{FCC('WIND'), COmfLocator::NewInstance},
	{FCC('WINF'), COmfLocator::NewInstance},
	{FCC('WINL'), COmfLocator::NewInstance}
};


