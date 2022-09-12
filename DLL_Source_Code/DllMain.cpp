// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: DllMain.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include <shlwapi.h>
#include "ContainerLayer99.h"

// {2E8CD6C8-E7D5-45d5-BCDA-A61882687340}
static const GUID CLSID_OmfooReader = 
{ 0xed56c718, 0x7102, 0x4546, { 0xa9, 0x7b, 0xac, 0x82, 0x9a, 0x48, 0xd5, 0xdc } };

#define	MY_MODULE_MAXPATH	1024
#define	CHARS_IN_GUID		39

//*********************************************************************************************************************
//	Globals.
//*********************************************************************************************************************
LONG		g_cClassFactoryRefs	= 0;	// Reference count for our class factories.
LONG		g_cGenericHeapRefs	= 0;	// Number of outstanding memory allocations created with MemAlloc().
LONG		g_cCppNewObjectRefs	= 0;	// Number of outstanding allocations created with our C++ operator new().
HANDLE		g_hHeap				= 0;	// Our heap; this is our alternative to the default CRT heap.
HMODULE		g_hModule			= 0;

//*********************************************************************************************************************
//	Our implementation of IClassFactory.
//*********************************************************************************************************************
class COurClassFactory : private IClassFactory
{
public:
	STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppvOut)
	{
		if (IsBadWritePointer(ppvOut, sizeof(void*)))
		{
			return E_POINTER;
		}
		else
		{
			*ppvOut = NULL;
		}

		if (IsBadReadPointer(&riid, sizeof(IID)))
		{
			return E_INVALIDARG;
		}
		else
		{
			if ((IID_IUnknown == riid)||(IID_IClassFactory == riid))
			{
				*ppvOut = static_cast<IClassFactory*>(this);
				InterlockedIncrement(&g_cClassFactoryRefs);
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
	}

private:
	ULONG STDMETHODCALLTYPE	AddRef(void)	{return InterlockedIncrement(&g_cClassFactoryRefs);}
	ULONG STDMETHODCALLTYPE	Release(void)	{return InterlockedDecrement(&g_cClassFactoryRefs);}

	STDMETHODIMP	LockServer(BOOL fLock)
	{
		if (fLock)	{InterlockedIncrement(&g_cClassFactoryRefs);}
		else		{InterlockedDecrement(&g_cClassFactoryRefs);}
		return S_OK;
	}

	STDMETHODIMP	CreateInstance(IUnknown *pUnkOuter, REFIID riid, PVOID *ppvOut)
					{return CContainerLayer99::CreateInstance(pUnkOuter, riid, ppvOut);}
};

// Static, singleton.
static COurClassFactory	g_ClassFactory;

//*********************************************************************************************************************
//	Public general-purpose memory allocation routine. It’s just a wrapper for HeapAlloc().
//	Uses the same heap as our C++ operator new() but it maintains its reference count in g_cGenericHeapRefs.
//	We roll our own allocators to eliminate dependencies on any C++ runtime libraries.
//	The memory returned is always zeroed.
//*********************************************************************************************************************
PVOID __stdcall MemAlloc(ULONG cb)
{
	PVOID pv = ::HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, cb);
	if (pv)
	{
		InterlockedIncrement(&g_cGenericHeapRefs);
	}

	return pv;
}

//*********************************************************************************************************************
//	Public memory verifier routine.
//*********************************************************************************************************************
BOOL __stdcall MemValidate(PVOID pv)
{
	return ::HeapValidate(g_hHeap, DWORD(0), pv);
}

//*********************************************************************************************************************
//	Public memory re-allocator routine.
//*********************************************************************************************************************
PVOID __stdcall MemRealloc(PVOID pv, ULONG cb)
{
	return ::HeapReAlloc(g_hHeap, HEAP_ZERO_MEMORY, pv, cb);
}

//*********************************************************************************************************************
//	Public memory routine.
//	This code is identical to our global C++ operator delete() except it decrements g_cGenericHeapRefs.
//*********************************************************************************************************************
BOOL __stdcall MemFree(PVOID pv)
{
	BOOL fResult = TRUE;
	if (pv)
	{	
		fResult = ::HeapFree(g_hHeap, DWORD(0), pv);
		if (fResult)
		{
			InterlockedDecrement(&g_cGenericHeapRefs);
		}
	}
	return fResult;
}

//*********************************************************************************************************************
//	Global C++ operator new(). It’s just a wrapper for HeapAlloc().
//	Uses the same heap as our MemAlloc() routine but it maintains its reference count in g_cCppNewObjectRefs.
//	We roll our own allocators to eliminate dependencies on any C++  runtime libraries.
//	The memory returned is always zeroed, even in debug builds.
//*********************************************************************************************************************
PVOID operator new(size_t cb)
{
	PVOID pv = ::HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, cb);
	if (pv)
	{
		InterlockedIncrement(&g_cCppNewObjectRefs);
	}
	return pv;
}

//*********************************************************************************************************************
//	Global C++ operator delete().
//	This code is identical to MemFree() except that it decrements g_cCppNewObjectRefs.
//*********************************************************************************************************************
void operator delete(void* pv)
{
	if (pv)
	{
		if (::HeapFree(g_hHeap, DWORD(0), pv))
		{
			InterlockedDecrement(&g_cCppNewObjectRefs);
		}
	}
}

//*********************************************************************************************************************
//	Public memory routine.
//	Returns the amount of free memory that is available to this application right now, as measured in 4096-byte pages.
//*********************************************************************************************************************
ULONG __stdcall SnapshotAvailablePages(void)
{
	UINT64 nAvailablePages		= 0;
	MEMORYSTATUSEX	memStatus	= {0};
	memStatus.dwLength			= sizeof(memStatus);
	if (GlobalMemoryStatusEx(&memStatus))
	{
		nAvailablePages = memStatus.ullAvailPhys/4096;

		// 64-bit Windows 10 Pro, Enterprise, and Education Editions support up to two tebibytes of RAM.
		// If there is more memory available than that then we just clip it so the result will fit in a ULONG.
		// 2199023255552 / 4096 = 536870912 (0x20000000)
		if (nAvailablePages > 0x20000000)
		{
			nAvailablePages = 0x20000000;
		}
	}

	// The result will always fit in a 32-bit ULONG.
	return ULONG(nAvailablePages);
}

//*********************************************************************************************************************
//	Called from DllMain().
//*********************************************************************************************************************
BOOL WINAPI OnProcessAttach(HMODULE hModule)
{
	// Save our HMODULE here.
	g_hModule	= hModule;

	// Create a growable heap.
	g_hHeap = ::HeapCreate(DWORD(0), SIZE_T(0), SIZE_T(0));
	if (NULL == g_hHeap)
	{
		return FALSE;
	}

	// If the function succeeds, the return value is nonzero.
	if (0 == ::DisableThreadLibraryCalls(hModule))
	{
		return FALSE;
	}

	return TRUE;
}

//*********************************************************************************************************************
//	Called from DllMain().
//	Note that the DLL_PROCESS_DETACH notification is only sent if the DLL is being unloaded by a call to FreeLibrary(). 
//*********************************************************************************************************************
BOOL WINAPI OnProcessDetach(HMODULE hModule)
{
	UNREFERENCED_PARAMETER(hModule);

	ASSERT(0 == g_cGenericHeapRefs);		// Number of outstanding memory allocations.1
	ASSERT(0 == g_cCppNewObjectRefs);		// Number of outstanding C++ new() allocations.

	if (g_hHeap)
	{
		::HeapDestroy(g_hHeap);
		g_hHeap	= NULL;
	}
	return TRUE;
}

//*********************************************************************************************************************
//	"When the system calls DllMain with any value other than DLL_PROCESS_ATTACH, the return value is ignored."
//*********************************************************************************************************************
#ifdef _DEBUG
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID lpReserved)
#else
extern "C" BOOL WINAPI _DllMainCRTStartup(HMODULE hModule, DWORD dwReason, PVOID lpReserved)
#endif
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
		return OnProcessAttach(hModule);

	if (dwReason == DLL_PROCESS_DETACH)
		return OnProcessDetach(hModule);

	return TRUE;
}

//*********************************************************************************************************************
//	Creates registry entries for the DLL.
//	DllRegisterServer supports the standard return values E_OUTOFMEMORY and E_UNEXPECTED, as well as the following: 
//
//		S_OK 
//		The registry entries were created successfully.
//
//		SELFREG_E_TYPELIB 
//		The server was unable to complete the registration of all the type libraries used by its classes.
//
//		SELFREG_E_CLASS 
//		The server was unable to complete the registration of all the object classes.
//
//*********************************************************************************************************************
STDAPI DllRegisterServer()
{
	WCHAR	wszModuleFileName[MY_MODULE_MAXPATH] = {0};
	WCHAR	wszSubKey[64] = {0};
	HRESULT hr			= S_OK;
	ULONG	cch			= 0;
	HKEY	hKey		= 0;
	HKEY	hSubKey		= 0;
	DWORD	dwLastErr	= ERROR_SUCCESS;

	// Look up this DLL’s fully qualified path.
	cch = GetModuleFileNameW(g_hModule, wszModuleFileName, MY_MODULE_MAXPATH);
	if (cch >= MY_MODULE_MAXPATH)
	{
		hr = E_FAIL;
		goto L_Exit;
	}

	lstrcpyW(wszSubKey, L"CLSID\\");
	StringFromGUID2(CLSID_OmfooReader, &wszSubKey[6], CHARS_IN_GUID);
	dwLastErr = RegCreateKeyW(HKEY_CLASSES_ROOT, wszSubKey, &hKey);
	if (dwLastErr)
	{
		hr = HRESULT_FROM_WIN32(dwLastErr);
		goto L_Exit;
	}

	dwLastErr = RegCreateKeyW(hKey, L"InprocServer32", &hSubKey);
	if (dwLastErr)
	{
		RegCloseKey(hKey);
		hr = HRESULT_FROM_WIN32(dwLastErr);
		hKey = NULL;
		goto L_Exit;
	}

	dwLastErr = RegSetValueW(hSubKey, LPWSTR(0), REG_SZ, wszModuleFileName, DWORD(lstrlenW(wszModuleFileName)));
	if (dwLastErr)
	{
		hr = HRESULT_FROM_WIN32(dwLastErr);
	}

	RegCloseKey(hSubKey);
	RegCloseKey(hKey);
	hKey = hSubKey = NULL;

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Removes registry entries for the DLL.
//	DllUnregisterServer supports the standard return values E_OUTOFMEMORY and E_UNEXPECTED, as well as the following: 
//
//		S_OK 
//		The registry entries were created successfully.
//
//		S_FALSE 
//		Unregistration of this server's known entries was successful, but other entries still exist for this server's classes.
//
//		SELFREG_E_TYPELIB 
//		The server was unable to remove the entries of all the type libraries used by its classes.
//
//		SELFREG_E_CLASS 
//		The server was unable to remove the entries of all the object classes.
//
//*********************************************************************************************************************
STDAPI DllUnregisterServer()
{
	WCHAR	wszSubKey[64]	= {0};
	lstrcpyW(wszSubKey, L"CLSID\\");
	StringFromGUID2(CLSID_OmfooReader, &wszSubKey[6], CHARS_IN_GUID);
	if (SHDeleteKeyW(HKEY_CLASSES_ROOT, wszSubKey))
	{
		return HRESULT(0x80040201);	// SELFREG_E_CLASS
	}

	return S_OK;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
STDAPI DllInstall(BOOL bInstall, LPCWSTR pwzCmdLine)
{
	UNREFERENCED_PARAMETER(bInstall);
	UNREFERENCED_PARAMETER(pwzCmdLine);
	return HRESULT(0x80040201);	// SELFREG_E_CLASS
}

//*********************************************************************************************************************
//	Creates a class factory instance.
//*********************************************************************************************************************
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, PVOID *ppvOut)
{
	if (IsBadReadPointer(&rclsid, sizeof(IID)))
	{
		return E_INVALIDARG;
	}

	if (rclsid == CLSID_OmfooReader)
	{
		return g_ClassFactory.QueryInterface(riid, ppvOut);
	}

	// "ClassFactory cannot supply requested class"
	return CLASS_E_CLASSNOTAVAILABLE;
}

//*********************************************************************************************************************
//	Queries whether the DLL can safely be unloaded. 
//*********************************************************************************************************************
STDAPI DllCanUnloadNow()
{
	// Verify that we've freed all memory.
	if (g_cClassFactoryRefs || g_cGenericHeapRefs || g_cCppNewObjectRefs)
	{
		// Nothing we can do about it now - except break if we are debugging.
		BREAK_IF_DEBUG
		return S_FALSE;
	}

	return S_OK;
}
