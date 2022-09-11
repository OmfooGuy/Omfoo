# Omfoo

WELCOME TO OMFOO!
Omfoo.dll is a Windows dynamic-link library (DLL) for reading Open Media Framework (OMF) files.
https://en.wikipedia.org/wiki/Open_Media_Framework_Interchange

"Open Media Framework" and "OMF" are registered trademarks of Avid Technology, Inc.
Omfoo is not not affiliated with or endorsed by Avid. To use this DLL you will need a copy
of Avid's OMF Interchange Specification, Version 2.1 - which is not included in this package.

The Omfoo API is based on Microsoft's Component Object Model.
https://en.wikipedia.org/wiki/Component_Object_Model

All Omfoo interfaces inherit IUnknown.
https://en.wikipedia.org/wiki/IUnknown

All methods except AddRef() and Release() return an HRESULT.
https://en.wikipedia.org/wiki/HRESULT.

Omfoo's container interface is called OmfooReader. You can instantiate it by registering
the DLL and then calling CoCreateInstance(CLSID_OmfooReader, ...), or you can load it manually
by calling LoadLibrary("Omfoo.dll"), GetProcAddress(hModule, "DllGetClassObject"), etc.

The source code for this project is essentially free. But please read the license! If you modify
any of the existing interfaces you must re-assign new IIDs. I've provided a command line utility
called UUIDXCHG.EXE to do that for you.

- David Miller
