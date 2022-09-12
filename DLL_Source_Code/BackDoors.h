// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: BackDoors.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once

struct __declspec(uuid("B8DFED10-0383-4e66-972B-911402925468")) IMobjBackDoor;
interface IMobjBackDoor : public IUnknown
{
	// OMF1 - Given a TRAK's object ID, return its zero-based position in the mob's OMFI:TRKG:Tracks array.
	// OMF2 - Given an MSLT's object ID, return its zero-based position in the mob's OMFI:MOBJ:Slots array.
	// The position is returned in the HRESULT.
	virtual HRESULT __stdcall	GetSlotIndex(__in DWORD dwMSLT)= 0;

	// OMF1 - Returns the total number of elements in the OMFI:TRKG:Tracks array.
	// OMF2 - Returns the total number of elements in the OMFI:MOBJ:Slots array.
//	virtual HRESULT __stdcall	GetSlotCount(void)= 0;
};
