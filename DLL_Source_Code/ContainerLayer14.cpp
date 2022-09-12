// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ContainerLayer14.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "Omfoo_Alpha_Header.h"
#include "ContainerLayer14.h"
#include <shlwapi.h>
#include "MiscStatic.h"
using namespace NsMiscStatic;

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CContainerLayer14::CContainerLayer14(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CContainerLayer14::~CContainerLayer14(void)
{
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CContainerLayer14::Load(PCWSTR pwzFileName)
{
	HRESULT hr = __super::Load(pwzFileName);
	if (SUCCEEDED(hr))
	{
		// Chop up the filename into a filename prefix that can be used to generate more filenames.
		// See IOmfMediaData::CreateFileNameForExport().
		CreatePrefixW(m_pwzFullPath, ELEMS(m_wzExportPrefix), m_wzExportPrefix);
	}
	return hr;
}

//*********************************************************************************************************************
//	Static private helper routine.
//	Chop up a string (a filename, mob name, etc.) into a prefix that can be used to generate more filenames.
//	See IOmfMediaData::CreateFileNameForExport().
//
//	On entry:
//		pSrc must consist of valid pathname characters. It can be a full path including dots, slashes, colons, etc.
//		pDst must point to a buffer large enough to hold cchDst characters (including the null-terminator).
//*********************************************************************************************************************

//	This is our fake/improvised CT_CTYPE1 character type bit that we combine with the results of GetStringTypeW().
//	If this bit is set then that means that this character begins a new human-readable word.
#define CX_CHOP_HERE	0x8000

HRESULT CContainerLayer14::CreatePrefixW(__in PCWSTR pSrc, __in const ULONG cchDst, __out PWCHAR pDst)
{
	WCHAR wzPrefixWip[256]	= {0};
	WORD  aCharTypes[260]	= {0};	// must be at least four elements longer than cchPrefixWip.

	// Internal reality check #1.
	if (cchDst < 2)
	{
		// We treat this as an assertion failure because this is a private, internal, method.
		// We should always provide a writable destination buffer that is large enough to hold
		// at least one character plus a null-terminator.
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}

	// Internal reality check #2.
	if (IsBadWritePointer(pDst, cchDst*2))
	{
		// We treat this as an assertion failure because this is a private, internal, method.
		// We should always provide a valid writable destination buffer. 
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}
	else
	{
		// Null-terminate our destination buffer.
		pDst[0] = 0;
	}

	// Internal reality check #3.
	// Make sure the string at pSrc is short enough to be copied into wzPrefixWip.
	if (IsBadStringPointerW(pSrc, ELEMS(wzPrefixWip)))
	{
		// We treat this as an assertion failure because this is a private internal method.
		// We should always provide a readable source buffer, even if it contains an empty string. 
		BREAK_IF_DEBUG
		return OMFOO_E_ASSERTION_FAILURE;
	}

	// Copy up to ELEMS(wzPrefixWip) characters (includes the null-terminator) into wzPrefixWip.
	// Skip over the path if there is one.
	lstrcpynW(wzPrefixWip, PathFindFileNameW(pSrc), ELEMS(wzPrefixWip));

	// Wipe the filename extension with zeroes (if there is one).
	PWSTR pWipe	= PathFindExtensionW(wzPrefixWip);
	PWSTR pEnd	= &wzPrefixWip[ELEMS(wzPrefixWip)];
	while (pWipe < pEnd) {*pWipe++ = 0;}

	// Get the string length (excluding the null terminator).
	ULONG cchPrefixWip = lstrlenW(wzPrefixWip);

	// If this string is empty we exit gracefully.
	if (cchPrefixWip == 0)
	{
		BREAK_IF_DEBUG		// how can this ever happen?
		return OMF_E_CANT_COMPLETE;
	}

	// Get character-type information for each character.
	GetStringTypeW(CT_CTYPE1, wzPrefixWip, cchPrefixWip, aCharTypes);

	// Calculate the maximum number characters that can make up the prefix.
	// This will normally be cchDst-1. But we may need to reduce it if our own internal buffer is too small.
	ULONG cchMax = cchDst-1; 
	if (cchMax > ELEMS(wzPrefixWip)-1)
	{
		// Unexpected case where caller allows prefixes longer than our internal workspace.
		// Since we are our own caller - how can this ever happen?
		cchMax = ELEMS(wzPrefixWip)-1;
	}

	// Wipe all characters starting at offset cchMax to the end of wzPrefixWip.
	// Some (or even all) of these characters may have been be zeroed already when we wiped off the filename extension.
	pWipe = &wzPrefixWip[cchMax];
	while (pWipe < pEnd) {*pWipe++ = 0;}

	// Get the string length again.
	cchPrefixWip = lstrlenW(wzPrefixWip);

	// Check again for zero-length string.
	if (cchPrefixWip == 0)
	{
		return OMF_E_CANT_COMPLETE;
	}

	// Loop to detect bad characters.
	for (UINT i = 0; i < cchPrefixWip; i++)
	{
		// Fail on control characters or undefined characters.
		if ((aCharTypes[i] & C1_CNTRL)||(0 == (aCharTypes[i] & C1_DEFINED)))
		{
			BREAK_IF_DEBUG		// how can this ever happen?
			return OMF_E_CANT_COMPLETE;
		}
	}

	// We repurpose the upper five bits of the CT_CTYPE1 flag bits returned by GetStringTypeW().
	// These bits are currently undefined: they are currently always zero, and probably always will be.
	// Nevertheless we clear them just to ensure that future versions of GetStringTypeW() haven't set them.
	for (UINT i = 0; i < cchPrefixWip; i++)
	{
		aCharTypes[i] &= 0x03FF;
	}

	// Loop to detect camel-case word divisions.
	// If we find one then we set the CX_CHOP_HERE bit in the corresponding aCharTypes[] array.
	for (UINT i = 0; i < cchPrefixWip; i++)
	{
		// Note that it's safe to reach beyond i because aCharTypes[] is four elements longer than cchPrefixWip.
		if ((aCharTypes[i+0] & C1_LOWER)&&
			(aCharTypes[i+1] & C1_UPPER)&&
			(aCharTypes[i+2] & C1_LOWER))
		{
			aCharTypes[i+1] |= CX_CHOP_HERE;
		}
	}

	// Loop to find transitions 
	WORD wPreviousBits = aCharTypes[0] & (C1_ALPHA|C1_PUNCT|C1_SPACE|C1_DIGIT);
	for (UINT i = 0; i < cchPrefixWip; i++)
	{
		WORD wCurrentBits = aCharTypes[i] & (C1_ALPHA|C1_PUNCT|C1_SPACE|C1_DIGIT);
		if (wCurrentBits != wPreviousBits)
		{
			wPreviousBits = wCurrentBits;
			aCharTypes[i] |= CX_CHOP_HERE;
		}
	}

	// Loop to discover the right-most chop point (if there is one).
	UINT nRightmostChopPoint = 0;
	for (UINT i = 0; i < cchPrefixWip; i++)
	{
		if (aCharTypes[i] & CX_CHOP_HERE)
		{
			nRightmostChopPoint = i;
		}
	}

	if (nRightmostChopPoint)
	{
		wzPrefixWip[nRightmostChopPoint] = 0;
		aCharTypes[nRightmostChopPoint] = 0;
	}

	// Trim off trailing spaces and punctuation, until we encounter an alphanumeric character.
	cchPrefixWip = lstrlenW(wzPrefixWip);
	if (cchPrefixWip)
	{
		for (UINT i = cchPrefixWip; i > 0; i--)
		{
			if (aCharTypes[i]&(C1_ALPHA|C1_DIGIT))
			{
				break;
			}
			else
			{
			//	aCharTypes[i]	= 0;	// unnecessary
				wzPrefixWip[i]	= 0;
			}
		}
	}

	// Loop to find the first (left-most) dot or underscore.
	// If we find one then we truncate the string right there.
	for (UINT i = 0; i < cchPrefixWip; i++)
	{
		if ((wzPrefixWip[i] == '.')||(wzPrefixWip[i] == '_'))
		{
			wzPrefixWip[i]	= 0;
			break;
		}
	}

	lstrcpyW(pDst, wzPrefixWip);
	return S_OK;
}


