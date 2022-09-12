// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but are changed infrequently
//


#pragma once
#include "targetver.h"
#include <windows.h>
#include <stdlib.h>		// _byteswap_ushort, _byteswap_ulong, _byteswap_uint64

//#pragma warning(disable:4996)	// "this function or variable may be unsafe. Consider using strcpy_s instead."
//#pragma intrinsic(strcpy, strcat, strcmp, strlen)

#ifndef CONSTANT_SEGMENT_NAME
#define	CONSTANT_SEGMENT_NAME	".iresa32$BBB"
#endif	// CONSTANT_SEGMENT_NAME

#pragma warning(disable:4201)	// nameless struct/union
//#pragma warning(disable:4512)	// assignment operator could not be generated
//#pragma warning(disable:4706)	// assignment within conditional expression

//#pragma warning(push)
//#pragma warning(disable:4163)	// "not available as an intrinsic function"
//#pragma warning(disable:4164)	// "intrinsic function not declared"
//#pragma warning(disable:4996)	// "this function or variable may be unsafe. Consider using strcpy_s instead."

//#pragma intrinsic(_memcmp, _memcpy, _memset)		// TWEEKY TWEEKY!

//
//#pragma intrinsic(strchr, strncpy)
//#pragma intrinsic(__stricmp)
//#pragma intrinsic(wcslen, wcscpy)
//#pragma intrinsic(wcscmp, wcsncpy)
#pragma intrinsic(_rotl, _rotl64, _rotr, _rotr64)
//#pragma intrinsic(_rotr8, _rotr16, _rotl8, _rotl16)
#pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
//#pragma warning(pop)

#define	Endian16	_byteswap_ushort
#define	Endian32	_byteswap_ulong
#define	Endian64	_byteswap_uint64

#define CHECK(x)			{HRESULT hrX=(x);if(FAILED(hrX))return hrX;}

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(x)	if (!(x))	__debugbreak();
#else
#define ASSERT(x)	((void)0)
#endif	// _DEBUG
#endif	// !ASSERT

#ifdef _DEBUG
#define BREAK_IF_DEBUG		__debugbreak();
#else
#define BREAK_IF_DEBUG
#endif

/*
#ifdef _DEBUG
#define BREAK_FOR_CURIOSITY	__debugbreak();		// Not a fatal error: just a learning experience.
#define BREAK_ON_UNEXPECTED	__debugbreak();		// Figure out how this happened, this breakpoint should be unreachable.
#define BREAK_ON_FATAL		__debugbreak();		// We know how and why this happened, and we know there is no way to fix it.
#else
#define BREAK_FOR_CURIOSITY
#define BREAK_ON_UNEXPECTED
#define BREAK_ON_FATAL
#endif
*/

#ifdef  _WIN64
#define OFFSETOF(s,m)   (size_t)((ptrdiff_t)&(((s *)0)->m))
#else
#define OFFSETOF(s,m)   (size_t)&(((s *)0)->m)
#endif

#ifndef ELEMS
#define ELEMS(s)	(sizeof(s)/sizeof(s[0]))
#endif

#define IsBadReadPointer			IsBadReadPtr
#define IsBadWritePointer			IsBadWritePtr
#define IsBadCodePointer			IsBadCodePtr

// Macro to determine if an HRESULT is a Win32 error code.
#define IS_WIN32_ERROR(hr)  (HRESULT_FACILITY(hr)==FACILITY_WIN32)

// Uncomment these if necessary. 
//typedef unsigned long		FOURCC;
typedef unsigned __int64	QWORD, *PQWORD;
typedef void **PPVOID;

/////////////

//typedef __int8	int8_t;
//typedef __int16	int16_t;
//typedef __int32	int32_t;
//typedef __int64	int64_t;
//
//typedef unsigned __int8	uint8_t;
//typedef unsigned __int16	uint16_t;
//typedef unsigned __int32	uint32_t;
//typedef unsigned __int64	uint64_t;
//
//typedef unsigned __int8	char8_t
//typedef unsigned __int16	char16_t

//The /Zc:char8_t option is available starting in Visual Studio 2019 version 16.1.
//It's enabled automatically when you specify /std:c++20 or later (such as /std:c++latest).
//Otherwise, it's off by default.



