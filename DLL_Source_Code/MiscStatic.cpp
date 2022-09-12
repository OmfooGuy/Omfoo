// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: MiscStatic.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include "MiscStatic.h"
using namespace NsMiscStatic;

const static CHAR aHexTable[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

//*********************************************************************************************************************
//	Enhanced version of IsBadStringPtrA().
//	This will return TRUE if the string length exceeds cchMax, whereas IsBadStringPtrA() will return FALSE.
//	For example IsBadStringPtrA("ABCDEFGHIJKLMNOP", 9) will return FALSE but IsBadStringPointerA("ABCDEFGHIJKLMNOP", 9)
//	will return TRUE. The string is deemed 'bad' because it is not null-terminated within the first 9 characters.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsBadStringPointerA(__in_opt LPCSTR psz, __in UINT_PTR cchMax)
{
	if (IsBadStringPtrA(psz, cchMax))
	{
		return TRUE;
	}

	for (UINT i = 0; i < cchMax; i++)
	{
		if (psz[i] == 0)
		{
			// The string is properly terminated. So it is not 'bad'.
			return FALSE;
		}
	}

	// The string is NOT properly terminated. So it is 'bad'!
	return TRUE;
}

//*********************************************************************************************************************
//	Enhanced version of IsBadStringPtrW().
//	This routine returns TRUE if the string length exceeds cchMax, whereas IsBadStringPtrW() will return FALSE.
//	For example IsBadStringPtrW("ABCDEFGHIJKLMN", 9) will return FALSE, but IsBadStringPointerW("ABCDEFGHIJKLMN", 9)
//	will return TRUE. The string is deemed 'bad' because it is not null-terminated within the first 9 characters.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsBadStringPointerW(__in_opt LPCWSTR pwz, __in UINT_PTR cchMax)
{
	if (IsBadStringPtrW(pwz, cchMax))
	{
		return TRUE;
	}

	for (UINT i = 0; i < cchMax; i++)
	{
		if (pwz[i] == 0)
		{
			// The string is properly terminated. So it is not 'bad'.
			return FALSE;
		}
	}

	// The string is NOT properly terminated. So it is 'bad'!
	return TRUE;
}

//*********************************************************************************************************************
//	Test to see if an LPUNKNOWN actually points to an IUnknown.
//	Returns TRUE if the pointer is bad.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsBadUnknown(LPUNKNOWN punk)
{
	typedef struct {
		FARPROC	QueryInterface;
		FARPROC	AddRef;
		FARPROC	Release;
	} UNKVTABLE, *PUNKVTABLE;

	typedef struct {
		PUNKVTABLE lpVtbl;
	} UNKSTRUCT, *PUNKSTRUCT;

	if ((punk == NULL) ||
		IsBadReadPointer(punk, sizeof(UNKSTRUCT)) ||
		IsBadReadPointer(PUNKSTRUCT(punk)->lpVtbl, sizeof(UNKVTABLE)) ||
		IsBadCodePointer(PUNKSTRUCT(punk)->lpVtbl->QueryInterface) ||
		IsBadCodePointer(PUNKSTRUCT(punk)->lpVtbl->AddRef) ||
		IsBadCodePointer(PUNKSTRUCT(punk)->lpVtbl->Release))
	{
		return TRUE;
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Verify that the memory referenced in REFIID is readable, and that the memory pointed to by ppvOut is writable.	
//*********************************************************************************************************************
HRESULT __stdcall NsMiscStatic::VerifyIID_PPV_ARGS(__in REFIID riid, __out PVOID *ppvOut)
{
	// Verify that the caller's destination pointer is writable.
	if (IsBadWritePointer(ppvOut, sizeof(LPUNKNOWN)))
	{
		return E_POINTER;
	}

	// Wipe it now as a courtesy.
	*ppvOut = NULL;

	// Validate that the caller's IID is readable.
	if (IsBadReadPointer(&riid, sizeof(IID)))
	{
		return E_INVALIDARG;
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Converts an old-school big-endian 80-bit SANE (Standard Apple Numerics Environment) IEEE 754 extended precision
//	floating-point number (made famous by the Motorola 68881 floating-point coprocessor and the AIFF sound file format)
//	into a 64-bit little-endian DOUBLE.
//*********************************************************************************************************************
HRESULT __stdcall NsMiscStatic::Extended80ToDouble(__in PVOID pIn, __out DOUBLE* pOut)
{
//	An extended80 uses one bit for the sign of the mantissa, 15 bits for the exponent, and 64 bits for the mantissa.
//	The exponent field is biased by 16383, meaning that 16383 has to be subtracted from the value in the exponent to
//	compute the actual power of 2. An exponent value of 32767 (0x7FFF) is reserved to enable the representation of
//	special states such as 'Infinity' and 'Not a Number'. If the exponent is zero, the value is a denormal number
//	and the exponent of 2 is -16382.
#pragma pack(push, 2)	// align structure members to 16-bit boundaries
typedef struct _EXTENDED80 {
	WORD	wExpNegate;
	QWORD	qwMantissa;
} EXTENDED80, *PEXTENDED80;
#pragma pack(pop)

	if (IsBadWritePtr(pOut, sizeof(DOUBLE)))
	{
		return E_POINTER;
	}

	if (IsBadReadPtr(pIn, sizeof(EXTENDED80)))
	{
		return E_POINTER;
	}

	WORD	wExpNegate	= Endian16(PEXTENDED80(pIn)->wExpNegate);
	QWORD	qwMantissa	= Endian64(PEXTENDED80(pIn)->qwMantissa);
	BOOL	bNormalized	= INT64(qwMantissa) < 0;
	BOOL	bNegate		= INT16(wExpNegate) < 0;
	LONG	nExponent80	= wExpNegate & 0x7FFF;
	LONG	nPowerOfTwo	= 0;

	// We use this union to manipulate the exponent and mantissa fields of a 64-bit DOUBLE.
	// It's just two different ways of looking at (and manipulating) the same 64-bit value.
	// It allows the processor and the C++ assignment operator (=) to do some of the conversion for us.
	union {
		DOUBLE	fTrickster;
		QWORD	qwTrickster;
	};

	// Handle special case where the exponent is -1.
	if (nExponent80 == 0x7FFF)
	{
		*PUINT64(pOut) = 0x7FFFFFFFFFFFFFFF;	// return IEEE 754 NAN
		return E_INVALIDARG;
	}

	// Handle special case where the exponent is zero.
	if (nExponent80 == 0)
	{
		// If the exponent and mantissa are both zero then the result is some type of 'zero'.
		if (qwMantissa == 0)
		{
			if (bNegate)
			{
				// Negative zero.
				*PUINT64(pOut) = 0x8000000000000000;
			}
			else
			{
				// Positive zero.
				*PUINT64(pOut) = 0x0000000000000000;
			}
			return S_OK;
		}
		else
		{
			// If the exponent is zero but the mantissa is non-zero then this is some sort
			// of denormal, pseudo-denormal, pseudo-infinity, pseudo not-a-number, etc. 
			BREAK_IF_DEBUG
			// TODO
			*PUINT64(pOut) = 0x7FFFFFFFFFFFFFFF;	// return IEEE 754 NAN
			return E_INVALIDARG;
		}
	}

	// TODO - add support for denormalized numbers!
	// See the BSR instruction and the _BitScanReverse64() intrinsic.
	if (!bNormalized)
	{
		BREAK_IF_DEBUG
		*PUINT64(pOut) = 0x7FFFFFFFFFFFFFFF;	// return IEEE 754 NAN
		return E_INVALIDARG;
	}

	// The exponent field of an extended80 is biased by 16383, meaning that 16383 has
	// to be subtracted from it to compute the actual power of 2.
	nPowerOfTwo = nExponent80 - 16383;

	// In contrast to the double-precision format, the extended80 format does not utilize an implicit/hidden bit.
	// Rather, bit 63 contains the integer part of the significand and bits 62-0 hold the fractional part.
	// Bit 63 will be 1 on all normalized numbers.
#pragma warning( push )
#pragma warning( disable : 4244 )	// "conversion from 'UINT64' to 'DOUBLE', possible loss of data"
	// Convert the extended80's mantissa (an unsigned positive 64-bit integer) to a double, using the C++ '=' operator.
	// Note that we are not just copying bits verbatim. The C++ compiler and processor are converting the value for us.
	fTrickster = qwMantissa;
#pragma warning( pop )

	// Isolate the exponent field from the DOUBLE.
	QWORD qwExponent = qwTrickster & 0x7FF0000000000000;

	// Clear the exponent field in the DOUBLE. (The sign bit #63 should already be zero.)
	qwTrickster &= 0x000FFFFFFFFFFFFF;

	// Our exponent is always 0x43E0000000000000 when the extended80 is normalized.
	// This is just a 'reality check' for my sanity and debugging.
	if (qwExponent != 0x43E0000000000000)
	{
		BREAK_IF_DEBUG							// has this ever happened?
		*PUINT64(pOut) = 0x7FFFFFFFFFFFFFFF;	// return IEEE 754 NAN
		return E_UNEXPECTED;
	}

	// A DOUBLE uses 11 bits for the exponent, and 52 bits for the mantissa.
	// The exponent field is biased by 1023, meaning that 1023 has to be subtracted to compute the actual power of 2.
	QWORD qwNewExponent = nPowerOfTwo + 1023;
	if (qwNewExponent > 0x00000000000007FF)
	{
		BREAK_IF_DEBUG
		*PUINT64(pOut) = 0x7FFFFFFFFFFFFFFF;	// return IEEE 754 NAN
		return E_UNEXPECTED;
	}

	// Update Trickster with the new exponent.
	qwNewExponent <<= 52;
	qwTrickster |= qwNewExponent;

	// Tweak the sign bit if necessary.
	if (bNegate)
	{
		fTrickster *= -1.0;
	}

	// Give the result to our caller.
	*pOut = fTrickster;

	// Success!
	return S_OK;
}

//*********************************************************************************************************************
//	Find the lowest common divisor using recursion.
//*********************************************************************************************************************
ULONG __stdcall NsMiscStatic::LowestCommonDivisor(ULONG v1, ULONG v2)
{
	if (v2==0) return v1;
	else return NsMiscStatic::LowestCommonDivisor(v2, v1%v2);
}

//*********************************************************************************************************************
//	If uValue is a power of two then return the power.
//	Otherwise return zero.
//*********************************************************************************************************************
ULONG IsAPowerOfTwo(__in ULONG uValue)
{
	ULONG	uBitMask	= uValue;
	ULONG	uPower		= 0;

	if (uBitMask)
	{
		while (0 == (uBitMask & 0x00000001))
		{
			uBitMask >>= 1;
			++uPower;
		}

		// One more shift.
		uBitMask >>= 1;
	}

	// If uBitMask is non-zero then uBitMask is not a power of two.
	return (uBitMask != 0) ? 0 : uPower;
}

//*********************************************************************************************************************
//	Returns TRUE if caller's DWORD is a fourCC.
//	There is no formal definition of a fourCC. 
//	The rule we use here begins with the axiom that each byte is an alphanumeric ASCII character,
//	but we also allow for spaces and underscores.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsValidFourCC(__in DWORD dwFourCC)
{
	SIZE_T	i	= 0;
	while (((UINT8(dwFourCC) >= UINT8('A')) && (UINT8(dwFourCC) <= UINT8('Z')))
			|| ((UINT8(dwFourCC) >= UINT8('a')) && (UINT8(dwFourCC) <= UINT8('z')))
			|| ((UINT8(dwFourCC) >= UINT8('0')) && (UINT8(dwFourCC) <= UINT8('9')))
			|| (UINT8(dwFourCC) == UINT8(' ')) || (UINT8(dwFourCC) == UINT8('_')))
	{
		dwFourCC >>= 8;
		i++;
	}

	return (i == sizeof(DWORD));
}

//*********************************************************************************************************************
//	Fast sleazy way to convert a 64-bit value into a sixteen character null-terminated hexadecimal ASCII string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold 16 CHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt64ToHexA(__in UINT64 u64, __out PCHAR pResult)
{
	PCHAR pDst	= &pResult[16];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u64 & 0x000000000000000F];
		u64 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert a 32-bit value into an eight character null-terminated hexadecimal ASCII string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold 8 CHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt32ToHexA(__in UINT32 u32, __out PCHAR pResult)
{
	PCHAR pDst	= &pResult[8];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u32 & 0x0000000F];
		u32 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert a 16-bit value into a four character null-terminated hexadecimal ASCII string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold four CHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt16ToHexA(__in UINT16 u16, __out PCHAR pResult)
{
	PCHAR pDst	= &pResult[4];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u16 & 0x000F];
		u16 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert an 8-bit value into a two character null-terminated hexadecimal ASCII string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold two CHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt8ToHexA(__in UINT8 u8, __out PCHAR pResult)
{
	PCHAR pDst	= &pResult[2];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u8 & 0x0F];
		u8 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert a 64-bit value into a sixteen character null-terminated hexadecimal UTF-16 string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold 16 WCHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt64ToHexW(__in UINT64 u64, __out PWCHAR pResult)
{
	PWCHAR pDst	= &pResult[16];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u64 & 0x000000000000000F];
		u64 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert a 32-bit value into an eight character null-terminated hexadecimal UTF-16 string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold 8 WCHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt32ToHexW(__in UINT32 u32, __out PWCHAR pResult)
{
	PWCHAR pDst	= &pResult[8];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u32 & 0x0000000F];
		u32 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert a 16-bit value into a four character null-terminated hexadecimal UTF-16 string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold four WCHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt16ToHexW(__in UINT16 u16, __out PWCHAR pResult)
{
	PWCHAR pDst	= &pResult[4];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u16 & 0x000F];
		u16 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//	Fast sleazy way to convert an 8-bit value into a two character null-terminated hexadecimal UTF-16 string.
//	It does not include the "0x" prefix.
//	The destination buffer that pResult points to must be large enough to hold two WCHARs.
//*********************************************************************************************************************
void __stdcall NsMiscStatic::UInt8ToHexW(__in UINT8 u8, __out PWCHAR pResult)
{
	PWCHAR pDst	= &pResult[2];
	*pDst		= 0;	// null terminator

	do
	{
		*--pDst = aHexTable[u8 & 0x0F];
		u8 >>= 4;
	} while (pDst > pResult);
}

//*********************************************************************************************************************
//
//	Fast sleazy way to convert a 32-bit unsigned integer into a four character ascii/decimal numeric string.
//	The destination buffer that pResult points to must be large enough to hold four WCHARs.
//	The result may or may not include trailing nulls.
//	The value in uValueMax serves as a template to determine the number of leading zeros to include.
//	Typical values are 9999, 999, 99, and 9.
//	For example if uValueMax is 9999 and uValue is 500 then the result will be "0500" (with no null terminator).
//	But if uValueMax is 999 and uValue is 500 then the result will be "500" (with one null terminator).
//	If uValue is greater than uValueMax then the result will be "~~~~", "~~~", "~~", or "~".
//*********************************************************************************************************************
DWORD __stdcall NsMiscStatic::UIntToDecimal9999W(__in UINT32 uValueMax, __in UINT32 uValue, __out PWCHAR pResult)
{
	DWORD	dwResult = '~~~~';
	UINT32	uInternalMax = (uValueMax > 9999) ? 9999 : uValueMax;

	if (uValue <= uInternalMax)
	{
		UINT32 uDgt1, uDgt2, uDgt3, uDgt4;

		uDgt1 = uValue;
		uDgt4 = uDgt1 / 1000;
		uDgt1 -= (uDgt4 * 1000);
		uDgt3 = uDgt1 / 100;
		uDgt1 -= (uDgt3 * 100);
		uDgt2 = uDgt1 / 10;
		uDgt1 -= (uDgt2 * 10);

		dwResult = ((uDgt1 << 24) | (uDgt2 << 16) | (uDgt3 << 8) | uDgt4) + '0000';
	}

	if (uInternalMax < 1000)
	{
		dwResult >>= 8;
		if (uInternalMax < 100)
		{
			dwResult >>= 8;
			if (uInternalMax < 10)
			{
				dwResult >>= 8;
			}
		}
	}

	pResult[0] = PUINT8(&dwResult)[0];
	pResult[1] = PUINT8(&dwResult)[1];
	pResult[2] = PUINT8(&dwResult)[2];
	pResult[3] = PUINT8(&dwResult)[3];

	return dwResult;
}

//*********************************************************************************************************************
//	Verify that an ASCII string is indeed a null-terminated ASCII string. (It must be null-terminated.)
//	The ASCII character set includes all printable characters and the space character. (Codes 0x20 ~ 0x7E.)
//	Note that all codes less than 0x20 are non-printable control codes.
//	Also note that code 0x7F is the non-printable "delete" (DEL) control character.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsPrintableAscii(PCSTR pszSrc, UINT_PTR cchMax)
{
	if ((NULL == pszSrc)||(cchMax == 0)||IsBadStringPointerA(pszSrc, cchMax))
	{
		return FALSE;
	}

	for (ULONG i = 0; i < cchMax; i++)
	{
		BYTE ch = PBYTE(pszSrc)[i];

		if ((0 == ch) && (i > 0))
		{
			return TRUE;
		}

		if ((ch < 0x20) || (ch > 0x7E))
		{
			return FALSE;
 		}
	}

	// Return FALSE because even tho all bytes are 'printable ASCII' it is NOT a null-terminated string!
	return FALSE;
}

//*********************************************************************************************************************
//	Return TRUE if the string consists entirely of ASCII numbers '0' thru '9'.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsStringNumericA(__in PCSTR pszSrc, __in UINT_PTR cchMax)
{
	if ((NULL == pszSrc)||(cchMax == 0)||IsBadStringPointerA(pszSrc, cchMax))
	{
		return FALSE;
	}

	for (ULONG i = 0; i < cchMax; i++)
	{
		BYTE ch = PBYTE(pszSrc)[i];

		if ((0 == ch) && (i > 0))
		{
			return TRUE;
		}

		if ((ch < '0') || (ch > '9'))
		{
			return FALSE;
 		}
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Test if a string consist entirely of valid _filename_ characters.
//	This will fail on valid pathnames.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsValidFilenameA(__in PCSTR pszSrc, __in UINT_PTR cchMax)
{
	if ((NULL == pszSrc)||(cchMax == 0)||IsBadStringPointerA(pszSrc, cchMax))
	{
		return FALSE;
	}

	if ((pszSrc[0] == '.') || (pszSrc[0] == ' '))
	{
		// The first character can not be a period or a space.
		return FALSE;
	}

	for (ULONG i = 0; i < cchMax; i++)
	{
		if (i > 255)
		{
			// If we've already traversed 255 characters then it's not a Win32 filename.
			return FALSE;
		}

		BYTE ch = PBYTE(pszSrc)[i];
		if ((0 == ch) && (i > 0))
		{
			// This is the natural/expected way to succeed and exit.
			return TRUE;
		}

		if ((ch < 0x20) ||		// ASCII C1 control codes
			(ch == '<') ||		// (less than)
			(ch == '>') ||		// (greater than)
			(ch == ':') ||		// (colon)
			(ch == '"') ||		// (double quote)
			(ch == '/') ||		// (forward slash)
			(ch == '\\')||		// (backslash)
			(ch == '|') ||		// (vertical bar or pipe)
			(ch == '?') ||		// (question mark)
			(ch == '*') ||		// (asterisk)
			(ch == '~') ||		// (tilde)
			(ch == 0x7F))		// ASCII DEL
		{
			// Illegal filename character.
			return FALSE;
 		}
	}

	// String is not null-terminated.
	return FALSE;
}

//*********************************************************************************************************************
//	Test if a string consist entirely of valid _filename_ characters.
//	This will fail on valid pathnames.
//*********************************************************************************************************************
BOOL __stdcall NsMiscStatic::IsValidFilenameW(__in PCWSTR pwzSrc, __in UINT_PTR cchMax)
{
	if ((NULL == pwzSrc)||(cchMax == 0)||IsBadStringPointerW(pwzSrc, cchMax))
	{
		return FALSE;
	}

	if ((pwzSrc[0] == '.') || (pwzSrc[0] == ' '))
	{
		// The first character can not be a period or a space.
		return FALSE;
	}

	for (ULONG i = 0; i < cchMax; i++)
	{
		if (i > 255)
		{
			// If we've already traversed 255 characters then it's not a Win32 filename.
			return FALSE;
		}

		WORD ch = PWORD(pwzSrc)[i];
		if ((0 == ch) && (i > 0))
		{
			// This is the natural/expected way to succeed and exit.
			return TRUE;
		}

		if ((ch < 0x0020) ||	// ASCII C1 control codes
			(ch > 0xD7FF) ||	// unicode supplementary characters
			(ch == '<') ||		// (less than)
			(ch == '>') ||		// (greater than)
			(ch == ':') ||		// (colon)
			(ch == '"') ||		// (double quote)
			(ch == '/') ||		// (forward slash)
			(ch == '\\')||		// (backslash)
			(ch == '|') ||		// (vertical bar or pipe)
			(ch == '?') ||		// (question mark)
			(ch == '*') ||		// (asterisk)
			(ch == '~') ||		// (tilde)
			(ch == 0x7F))		// ASCII DEL
		{
			// Illegal filename character.
			return FALSE;
 		}
	}

	// String is not null-terminated.
	return FALSE;
}

//*********************************************************************************************************************
//	Helper routine for string methods that feature call-twice semantics.
//	This is the CHAR version of CallTwiceStringHandlerW(). See below.
//	Note that this routine is intended for internal use only.
//	We do not test the validity of pszSrc because we are our own caller.
//	If you need to test the validity of pszSrc the call IsBadStringPointerA() before calling here!
//*********************************************************************************************************************
HRESULT __stdcall NsMiscStatic::CallTwiceStringHandlerA(__in HRESULT hrEcho,
														__in_opt PCSTR pszSrc,
															__in ULONG cchBuffer,
																__out_opt PCHAR pBuffer,
																	__out_opt PULONG pcchRequired)
{
	// Our caller must supply at least one of these arguments.
	if ((NULL == pBuffer) && (NULL == pcchRequired))
	{
		return E_INVALIDARG;
	}

	// The pBuffer argument is optional.
	// But if our caller supplied one then we need to be sure we can write to it.
	if (pBuffer)
	{
		// If pBuffer is non-NULL then make sure cchBuffer is non-zero.
		if (cchBuffer == 0)
		{
			return E_INVALIDARG;
		}

		if (IsBadWritePointer(pBuffer, cchBuffer))
		{
			return E_POINTER;
		}

		// Terminate our caller's destination buffer. Right here, right now.
		// Do this as a courtesy and a safety precaution.
		*pBuffer = 0;
	}
	else
	{
		// If pBuffer is NULL then make sure cchBuffer is zero.
		// Our caller should have done this themselves, but we don't treat it as a fatal error if they didn't.
		// We use this 'zero' internally; we gotta do it; we depend on it.
		cchBuffer = 0;
	}

	// The pcchRequired argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pcchRequired)
	{
		if (IsBadWritePointer(pcchRequired, sizeof(ULONG)))
		{
			return E_POINTER;
		}

		// Set caller's cchRequired to zero. Right here, right now.
		*pcchRequired = 0;
	}

	// If hrEcho is an error code (if it's negative) then don't copy pszSrc and/or don't return its string length.
	// Instead just return hrEcho.
	if (FAILED(hrEcho))
	{
		return hrEcho;
	}

	// If pszSrc is null then we fail.
	// Our caller SHOULD have provided an error code in hrEcho, but just in case they didn't.
	if (pszSrc == NULL)
	{
		return E_FAIL;	// Unspecified error
	}

	// Succeed on zero-length strings.
	if (pszSrc[0] == 0)
	{
		return S_OK;
	}

	// Get the string length, and include the null-terminator .
	ULONG cchRequired = ULONG(lstrlenA(pszSrc)+1);

	// Give it to caller if caller wants it.
	if (pcchRequired)
	{
		*pcchRequired = cchRequired;
	}

	// Copy string to caller's buffer (if caller wants it) and truncate it if necessary.
	if (pBuffer)
	{
		lstrcpynA(pBuffer, pszSrc, cchRequired);
	}

	// 'Fail' if the buffer contains a truncated, null-terminated version of the string.
	return (cchRequired <= cchBuffer) ? S_OK : HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
}

//*********************************************************************************************************************
//	Helper routine for string methods that feature call-twice semantics.
//	This is the WCHAR version of CallTwiceStringHandlerA(). See above.
//	Note that this routine is intended for internal use only.
//	We do not test the validity of pwzSrc because we are our own caller.
//	If you need to test the validity of pwzSrc the call IsBadStringPointerW() before calling here!
//*********************************************************************************************************************
HRESULT __stdcall NsMiscStatic::CallTwiceStringHandlerW(__in HRESULT hrEcho,
														__in_opt LPCWSTR pwzSrc,
															__in ULONG cchBuffer,
																__out_opt PWCHAR pBuffer,
																	__out_opt PULONG pcchRequired)
{
	// Our caller must supply at least one of these arguments.
	if ((NULL == pBuffer) && (NULL == pcchRequired))
	{
		return E_INVALIDARG;
	}

	// The pBuffer argument is optional.
	// But if our caller supplied one then we need to be sure we can write to it.
	if (pBuffer)
	{
		// If pBuffer is non-NULL then make sure cchBuffer is non-zero.
		if (cchBuffer == 0)
		{
			return E_INVALIDARG;
		}

		if (IsBadWritePointer(pBuffer, (cchBuffer * sizeof(WCHAR))))
		{
			return E_POINTER;
		}

		// Terminate our caller's destination buffer. Right here, right now.
		// Do this as a courtesy and a safety precaution.
		*pBuffer = 0;
	}
	else
	{
		// If pBuffer is NULL then make sure cchBuffer is zero.
		// Our caller should have done this themselves, but we don't treat it as a fatal error if they didn't.
		// We use this 'zero' internally so we gotta do it - we depend on it.
		cchBuffer = 0;
	}

	// The pcchRequired argument is optional, but if our caller supplied one then make sure we can write to it.
	if (pcchRequired)
	{
		if (IsBadWritePointer(pcchRequired, sizeof(ULONG)))
		{
			return E_POINTER;
		}

		// Set caller's cchRequired to zero. Right here, right now.
		*pcchRequired = 0;
	}

	// If hrEcho is an error code (if it's negative) then don't copy pwzSrc and/or don't return its string length.
	// Instead just return hrEcho.
	if (FAILED(hrEcho))
	{
		return hrEcho;
	}

	// If pwzSrc is NULL then we fail.
	// Our caller should have known that their pointer is NULL, and they should have provided an error code in hrEcho,
	// but just in case they didn't.
	if (pwzSrc == NULL)
	{
		return E_FAIL;	// Unspecified error
	}

	// Succeed on zero-length strings.
	if (pwzSrc[0] == 0)
	{
		return S_OK;
	}

	// Get the string length, and include the null-terminator .
	ULONG cchRequired = ULONG(lstrlenW(pwzSrc)+1);

	// Give it to caller if caller wants it.
	if (pcchRequired)
	{
		*pcchRequired = cchRequired;
	}

	// Copy string to caller's buffer (if caller wants it) and truncate it if necessary.
	if (pBuffer)
	{
		lstrcpynW(pBuffer, pwzSrc, cchRequired);
	}

	// 'Fail' if the buffer contains a truncated, null-terminated version of the string.
	return (cchRequired <= cchBuffer) ? S_OK : __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
}


