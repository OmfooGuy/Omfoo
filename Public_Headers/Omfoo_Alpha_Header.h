// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: Omfoo_Alpha_Header.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo API. You should have received a
// copy of the Omfoo end-user license agreement (EULA) along with it.
// Please see OMFOO_DLL_USER_AGREEMENT.TXT.
// ---------------------------------------------------------------------------
#ifndef __OMFOO_ALPHA_HEADER_H__
#define __OMFOO_ALPHA_HEADER_H__

#if !defined(_MSC_VER)
#error Omfoo_Alpha_Header.h has only been tested with Microsoft C++ compilers.
#endif
#if !defined(_WIN32)
#error Omfoo_Alpha_Header.h is only designed for Windows 32-bit and 64-bit desktop applications.
#endif

//	WELCOME TO OMFOO!
//	Omfoo.dll is a Windows dynamic-link library (DLL) for reading Open Media Framework (OMF) files.
//	https://en.wikipedia.org/wiki/Open_Media_Framework_Interchange
//
//	"Open Media Framework" and "OMF" are registered trademarks of Avid Technology, Inc.
//	Omfoo is not not affiliated with or endorsed by Avid. To use this DLL you will need a copy
//	of Avid's OMF Interchange Specification, Version 2.1 - which is not included in this package.
//
//	The Omfoo API is based on Microsoft's Component Object Model.
//	https://en.wikipedia.org/wiki/Component_Object_Model
//
//	All Omfoo interfaces inherit IUnknown.
//	https://en.wikipedia.org/wiki/IUnknown
//
//	All methods except AddRef() and Release() return an HRESULT.
//	https://en.wikipedia.org/wiki/HRESULT.
//
//	Before you use Omfoo you should register the DLL. You only need to do this once.
//	The DLL is named 'Omfoo.dll'. There is a 32-bit build and a 64-bit build.
//	You will need administrator privileges to register it, and the way to do it depends on your version of Windows.
//	https://www.wikihow.com/Register-a-DLL
//	https://support.microsoft.com/en-us/help/844592/how-to-register-a-dll-file
//
//	Once you've registered the DLL you can begin to use Omfoo.
//	The first object you can access is called the 'OmfooReader'.
//	You instantiate an 'OmfooReader' by calling CoCreateInstance().
//
//		IOmfooReader* pOmfooReader = NULL;
//		HRESULT hr = ::CoCreateInstance(CLSID_OmfooReader, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&pOmfooReader));
//		if (SUCCEEDED(hr))
//		{
//			/* Opens the file as read-only. */
//			hr = pOmfooReader->Load(/* path to an existing OMF file*/);
//			if (SUCCEEDED(hr))
//			{
//				/* Do stuff here */
//			}
//			pOmfooReader->Release();
//			pOmfooReader = NULL;
//		}
//
//	All Omfoo methods use the standard COM calling convention.
#define OMFOOAPI	virtual HRESULT __stdcall

//	OMF makes heavy use of FOURCCs. 
//	There are various C/C++ macros that make it easier to declare FOURCC values in source code.
//	The one below is barrowed from <Aviriff.h> in the Windows SDK.
#ifndef FCC
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))
#endif

//	We use these do-nothing #defines to indicate how arguments are passed in and out of Omfoo methods.
#ifndef	__in
#define __in
#define __out
#define __inout
#define __in_opt
#define __out_opt
#endif

//	These are Omfoo's nested include files. DO NOT CHANGE THE ORDER!
#include "Omfoo_Structures.h"
#include "Omfoo_Enumerated_Types.h"
#include "Omfoo_Interfaces.h"
#include "Omfoo_Error_Codes.h"

#endif	// __OMFOO_ALPHA_HEADER_H__

