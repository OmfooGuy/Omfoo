// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadOmf_StringTables.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
struct CReadOmf_StringTables {
protected:
	static const unsigned __int32	m_aPropertyStringHashTable[430];
	static const unsigned __int16	m_aPropertyStringOffsets[430];
	static const unsigned __int32	m_aPropertyStringData[2346];

	static const unsigned __int32	m_aDataTypeStringHashTable[66];
	static const unsigned __int16	m_aDataTypeStringOffsets[66];
	static const unsigned __int32	m_aDataTypeStringData[254];

	static const unsigned __int32	m_aDataKindStringHashTable[38];
	static const unsigned __int16	m_aDataKindStringOffsets[38];
	static const unsigned __int32	m_aDataKindStringData[178];

	static const unsigned __int32	m_aEffectIDStringHashTable[10];
	static const unsigned __int16	m_aEffectIDStringOffsets[10];
	static const unsigned __int32	m_aEffectIDStringData[72];
};
