// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII.
// Original filename: Omfoo_Interfaces.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo API. You should have received a
// copy of the Omfoo end-user license agreement (EULA) along with it.
// Please see OMFOO_EULA.TXT or send inquiries to OmfooGuy@gmail.com.
// ---------------------------------------------------------------------------
#ifndef __OMFOO_INTERFACES_H__
#define __OMFOO_INTERFACES_H__

//*********************************************************************************************************************
//	IOmfooIterator
//	Available in OMF1 and OMF2.
//	This is Omfoo's iterator/enumerator object.
//	It is similar to Microsoft's IObjectArray or IEnumObjects interface, but it retrieves just one object at a time.
//	We use this to iterate the members of the OMF data type omfi:ObjRefArray and omfi:MobIndex.
//*********************************************************************************************************************
struct __declspec(uuid("105F84F9-E160-4f97-9F86-4F0B39AD195A")) IOmfooIterator;
interface IOmfooIterator : public IUnknown
{
//	Retrieves the number of objects in the set.
	OMFOOAPI GetCount(__out PULONG pcObjects)= 0;

//	Resets the internal enumeration index to 0.
	OMFOOAPI Reset(void)= 0;

//	Instantiates the next object in the set and queries it for the interface specified by riid.
//	If the object does not expose the requested interface the object is released, and the internal position pointer
//	is advanced to the next object. This process repeats until an object is found that exposes the requested interface,
//	or until the end of the set is reached. It returns OMF_E_NO_MORE_ITEMS when the end is reached.
//	You can always retrieve every object in the set by setting riid to IID_IUnknown.
	OMFOOAPI Next(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfooReader
//	Available in OMF1 and OMF2.
//	This is Omfoo's bootstrap interface. It provides access to the Bento Container that encapsulates every OMF file.
//	It's like a read-only version of Microsoft's IPersistFile. Its purpose is to open the specified file, to initialize
//	itself from the file's contents, to provide basic information about the file's format and version, and to provide
//	access to the OMF Header Object (HEAD). All other objects in the Omfoo API originate either directly or indirectly
//	from here. You instantiate it by calling CoCreateInstance() and then you initialize it by calling Load().
//	Note that in future versions of Omfoo the Container may expose additional interfaces.
//*********************************************************************************************************************
struct __declspec(uuid("EDE3DE52-4CE3-4194-B655-5BE9F27020CF")) IOmfooReader;
interface IOmfooReader : public IUnknown
{
//	Open and load an existing OMF file with read-only/share access rights.
//	You can only call this once per lifetime.
//	The file handle stays open (and the file remains 'loaded') for the life of this IOmfooReader object.
	OMFOOAPI Load(__in PCWSTR pwzFileName)= 0;

//	Retrieves the fully qualified pathname of the loaded OMF file.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the full string.
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is not large enough to hold the result.
//	Note that the character counts in cchBuffer and *pcchRequired represent WCHARs (2 bytes).
	OMFOOAPI GetCurFile(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Fast, easy, reliable, foolproof way to tell if the file is OMF1.
//	Returns S_OK if the file is OMF1 or S_FALSE if it is not.
//	You can also get this information by examining the Header's OMFI:HEAD:Version property.
	OMFOOAPI IsOmf1(void)= 0;

//	Fast, easy, reliable, foolproof way to tell if the file is OMF2.
//	Returns S_OK if the file is OMF2 or S_FALSE if it is not.
//	You can also get this information by examining the Header's OMFI:HEAD:Version property.
	OMFOOAPI IsOmf2(void)= 0;

//	Returns S_OK if the file's Bento layer is big-endian or S_FALSE if it is little-endian.
//	This method is provided for debugging and troubleshooting only.
//	Omfoo automatically handles all endianness issues in the Bento layer.
	OMFOOAPI IsBentoBigEndian(void)= 0;

//	Returns S_OK if the file's OMF layer is big-endian or S_FALSE if it is little-endian.
//	You can also get this information by examining the Header’s OMFI:HEAD:ByteOrder property.
//	Note that Omfoo normally handles most endianness issues in the OMF layer - except where noted.
	OMFOOAPI IsOmfBigEndian(void)= 0;

//	During its Load() method Omfoo caches all properties having the omfi:String data type and performs
//	a series of tests to decide which code page to use for its internal calls to MultiByteToWideChar().
//	This method retrieves that detected code page.
	OMFOOAPI GetDetectedCodePage(__out PDWORD pdwCodePage)= 0;

//	This method overrides the detected code page and causes Omfoo to use the specified code page instead.
//	All of its internal calls to MultiByteToWideChar() will use this code page.
//	Returns E_INVALIDARG if dwNewWorkingCodePage is not installed on the system.
	OMFOOAPI SetWorkingCodePage(__in DWORD dwNewWorkingCodePage)= 0;

//	Retrieves the OMF Header Object (HEAD) and queries it for the interface specified by riid.
//	The current implementation of the Header exposes IOmfObject and IOmfHeader.
//	Note that in future versions of Omfoo the Header may expose additional interfaces.
	OMFOOAPI GetHeadObject(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Creates an iterator to instantiate every object in the file.
//	If dwClassFourCC is non-zero and fStrict is TRUE then only objects that belong to that specific class will appear.
//	If dwClassFourCC is non-zero and fStrict is FALSE then any objects that inherit that class will appear.
//	If dwClassFourCC is zero then fStrict is ignored, and every object in the Container will be enumerated.
//	Also note that you can create fourCCS using the FCC macro, which is declared in <Aviriff.h>.
	OMFOOAPI IterateObjects(__in_opt DWORD dwClassFourCC, __in BOOL fStrict, __out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfObject
//	Available in OMF1 and OMF2.
//	This is a general-purpose interface that can be used to read any property from any OMF object in any OMF file.
//	It's like a read-only version of Microsoft's IPropertyBag interface (without the VARIANT stuff).
//	It provides generic routines to retrieve property data by name, however it has no specific knowledge
//	of the object that it represents or the data that it retrieves.
//	All OMF objects int the Omfoo DLL expose this interface.
//*********************************************************************************************************************
struct __declspec(uuid("3F069D94-DBBB-422a-94CC-73AE0FE4C5DD")) IOmfObject;
interface IOmfObject : public IUnknown
{
//	Retrieves the object's container, queries it for the interface specified by riid, and then returns the result.
//	The current implementation of the Container only exposes IOmfooReader but other interfaces may be added later.
//	Use this routine to navigate from an IOmfObject back to the IOmfooReader that owns it.
	OMFOOAPI GetContainer(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Retrieves the parent object that instantiated this object.
//	For example if this IOmfObject represents a Media Descriptor (MDES) then this may return the Source Mob (SMOB)
//	that references it. In that case you may want to query for IOmfSourceMob or IOmfObject.
//	If this instance of this IOmfObject has no parent then this method will return OMF_E_CANT_COMPLETE.
	OMFOOAPI GetParent(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Retrieves the object's class fourCC (its OMFI:ObjID property) in the HRESULT.
//	OMF2 - Retrieves the object's class fourCC (its OMFI:OOBJ:ObjClass property) in the HRESULT. 
	OMFOOAPI GetClassFourCC(void)= 0;

//	Compare this IOmfObject to an IUnknown to see if they both represent the same OMFI object.
//	Note that IOmfooReader::Load() opens the file with read-only/share access rights. So it's possible for
//	an application to instantiate two or more IOmfooReaders on the same OMF file, and then instantiate two
//	or more IOmfObjects on the same OMFI object!
//	If fStrict is TRUE then both IOmfObjects must must have originated from the same IOmfooReader.
//	If fStrict is FALSE then they only need to represent the same OMFI object in the same OMF file.
//	Returns S_OK if they represent the same OMFI object or S_FALSE if they do not.
	OMFOOAPI IsSelf(__in IUnknown *punk, __in BOOL fStrict)= 0;

//	Tests the object's relationship to a specific OMF class.
//	The dwClassFourCC argument can be one of the standard fourCCs defined in the OMF Specification,
//	or it can be an extended class defined via the HEAD object's Class Dictionary.
//	You can create fourCCS using the FCC macro, which is declared in <Aviriff.h>.
//	Returns S_OK if the object inherits the given class, or OMF_E_NO_INHERITANCE if it does not.
//	Returns OMF_E_BAD_FOURCC if dwClassFourCC is not composed of four printable ASCII characters.
	OMFOOAPI IsATypeOf(__in DWORD dwClassFourCC)= 0;

//	Test if a given property is present in the object.
//	Returns S_OK if the object contains the property specified by pszPropertyName, or OMF_E_PROP_NOT_FOUND if not.
	OMFOOAPI IsPropertyPresent(__in LPCSTR pszPropertyName)= 0;

//	Generic read routine. Retrieves raw unprocessed bytes as specified by pszPropertyName.
//	The pBuffer and pcbRequired arguments are both optional, but you must provide at least one.
//	The OMF data type is not enforced. It is up to you to know what you are reading and to byte-swap it if necessary.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the bytes.
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is not large enough to hold the payload.
	OMFOOAPI ReadRawBytes(__in LPCSTR pszPropertyName,
										__in ULONG cbBuffer,
											__out_opt PVOID pBuffer,
												__out_opt PULONG pcbRequired)= 0;

//	Retrieves a null-terminated string as specified by pszPropertyName exactly as it is stored in the file.
//	The OMF data type must be omfi:String or omfi:UniqueName or this routine will return OMF_E_TYPE_SURPRISE.
//	The pBuffer and pcchRequired arguments are both optional, but you must provide at least one.
//	The character counts in cchBuffer and *pcchRequired represent CHARs (one byte).
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is not large enough to hold the string.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the full string.
	OMFOOAPI ReadStringA(__in LPCSTR pszPropertyName,
										__in ULONG cchBuffer,
											__out_opt PCHAR pBuffer,
												__out_opt PULONG pcchRequired)= 0;

//	Retrieves a null-terminated wide character (UTF-16) string as specified by pszPropertyName.
//	The OMF data type must be omfi:String or omfi:UniqueName, or this routine will return OMF_E_TYPE_SURPRISE.
//	The pBuffer and pcchRequired arguments are both optional, but you must provide at least one.
//	The character counts in cchBuffer and *pcchRequired represent WCHARs (two bytes).
//	Returns OMF_E_INSUFFICIENT_BUFFER if the buffer pointed to by pBuffer is not large enough to hold the string.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL to get
//	the required buffer size, and then call again with an appropriately sized buffer to retrieve the full string.
//	Note that the pszPropertyName argument is always ASCII even though the string it returns is always UTF-16.
	OMFOOAPI ReadStringW(__in LPCSTR pszPropertyName,
										__in ULONG cchBuffer,
											__out_opt PWCHAR pBuffer,
												__out_opt PULONG pcchRequired)= 0;

//	Retrieves a BOOLEAN property as specified by pszPropertyName.
//	The OMF data type must be omfi:Boolean or this will return OMF_E_TYPE_SURPRISE.
//	The pBoolean argument is optional (it can be NULL). If you provide one then on
//	exit the BOOLEAN it points to will be set to TRUE (1) or FALSE (0).
//	Returns S_OK if the property is TRUE, or S_FALSE if the property is FALSE.
//	Returns OMF_E_PROP_NOT_FOUND if the object does not have the specified property.
	OMFOOAPI ReadBoolean(__in LPCSTR pszPropertyName, __out_opt PBOOLEAN pBoolean)= 0;

//	These methods retrieve an integer property specified by pszPropertyName.
//	The OMF data type is ignored but the size of the payload must agree with the size of the integer requested.
//	If the sizes don't match these routines return OMF_E_SIZE_SURPRISE.
//	The values are automatically byte-swapped when necessary.
	OMFOOAPI ReadInt8(__in LPCSTR pszPropertyName, __out PINT8 pn8)= 0;
	OMFOOAPI ReadInt16(__in LPCSTR pszPropertyName, __out PINT16 pn16)= 0;
	OMFOOAPI ReadInt32(__in LPCSTR pszPropertyName, __out PINT32 pn32)= 0;
	OMFOOAPI ReadInt64(__in LPCSTR pszPropertyName, __out PINT64 pn64)= 0;
	OMFOOAPI ReadUInt8(__in LPCSTR pszPropertyName, __out PUINT8 pu8)= 0;
	OMFOOAPI ReadUInt16(__in LPCSTR pszPropertyName, __out PUINT16 pu16)= 0;
	OMFOOAPI ReadUInt32(__in LPCSTR pszPropertyName, __out PUINT32 pu32)= 0;
	OMFOOAPI ReadUInt64(__in LPCSTR pszPropertyName, __out PUINT64 pu64)= 0;

//	Retrieves a omfi:Length32 or omfi:Length64 property as a UINT64.
//	The OMF data type must be omfi:Length32 or omfi:Length64 or this method will fail with OMF_E_TYPE_SURPRISE.
//	When the data type is omfi:Length32 the value is considered unsigned, and is promoted to 64 bits with zero padding.
	OMFOOAPI ReadLength64(__in LPCSTR pszPropertyName, __out PUINT64 pLength)= 0;

//	Retrieves a omfi:Position32 or omfi:Position64 property as an INT64.
//	The OMF data type must be omfi:Position32 or omfi:Position64 or this method will fail with OMF_E_TYPE_SURPRISE.
//	When the data type is omfi:Position32 the value is considered signed, and so it is sign-extended to 64 bits.
	OMFOOAPI ReadPosition64(__in LPCSTR pszPropertyName, __out PINT64 pPosition)= 0;

//	Populates the caller's buffer with an array of 32-bit unsigned integers (UINT32s) as specified by pszPropertyName.
//	The OMF data type must be omfi:Int32Array or this method will fail with OMF_E_TYPE_SURPRISE.
//	On entry the nMaxElements argument holds the maximum size of caller's buffer (as measured in UINT32 elements).
//	On exit *pnActualElements receives the actual number of UINT32s in the array. It is NOT a byte count!
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necesssary.
//	This method has optional call-twice semantics. You can make the first call with pArrayBase set to NULL to get
//	the required size as measured in UINT32 elements, and then call again with an appropriately sized buffer to
//	retrieve the full array.
	OMFOOAPI ReadInt32Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT32 pArrayBase,
												__out PULONG pnActualElements)= 0;

//	Populates the caller's buffer with an array of 64-bit unsigned integers (UINT64s) as specified by pszPropertyName.
//	The data type must be omfi:Int32Array or omfi:Int64Array or this method will fail with OMF_E_TYPE_SURPRISE.
//	When the OMF data type is omfi:Int32Array the 32-bit integers are zero-padded (NOT sign-extended) to 64-bits.
//	On entry the nMaxElements argument holds the maximum size of caller's buffer as measured in UINT64 elements.
//	On exit *pnActualElements receives the actual number of UINT64s in the array. It is NOT a byte count!
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necesssary.
//	This method has optional call-twice semantics. You can make the first call with pArrayBase set to NULL to get
//	the required size (measured in elements), and then call again with an appropriately sized buffer to retrieve
//	the full array.
	OMFOOAPI ReadInt64Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT64 pArrayBase,
												__out PULONG pnActualElements)= 0;

//	Populates the caller's buffer with an array of 32-bit unsigned 'positions' as specified by pszPropertyName.
//	The OMF data type must be omfi:Position32Array or this method will fail with OMF_E_TYPE_SURPRISE.
//	On entry the nMaxElements argument holds the maximum size of caller's buffer (as measured in elements).
//	On exit the ULONG pointed to by pnActualElements receives the actual size of the array (as measured in elements).
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necessary.
//	This method has optional call-twice semantics. You can make the first call with pArrayBase set to NULL to get
//	the required size (measured in elements), and then call again with an appropriately sized buffer to retrieve
//	the full array.
	OMFOOAPI ReadPosition32Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT32 pArrayBase,
												__out PULONG pnActualElements)= 0;

//	Populates the caller's buffer with an array of 64-bit unsigned 'positions' as specified by pszPropertyName.
//	The data type must be omfi:Positiom32Array or omfi:Positiom64Array or this will fail with OMF_E_TYPE_SURPRISE.
//	When the OMF data type is omfi:Positiom32Array the 32-bit 'positions' in the array are zero-padded to 64-bits.
//	On entry the nMaxElements argument holds the maximum size of caller's buffer as measured in elements.
//	On exit the ULONG pointed to by pnActualElements receives the actual size of the array as measured in elements.
//	If the caller's buffer is too small to hold the array then this method will fail with OMF_E_INSUFFICIENT_BUFFER.
//	The elements are automatically byte-swapped when necessary.
//	This method has optional call-twice semantics. You can make the first call with pArrayBase set to NULL to get
//	the required size (measured in elements), and then call again with an appropriately sized buffer to retrieve
//	the full array.
	OMFOOAPI ReadPosition64Array(__in LPCSTR pszPropertyName,
										__in ULONG nMaxElements,
											__out_opt PUINT64 pArrayBase,
												__out PULONG pnActualElements)= 0;

//	Miscellaneous structure retrieval routines.
//	The structures are defined in "Omfoo_Structures.h"
//	Structure members are automatically byte-swapped when necessary.
//	The OMF data type is strictly enforced.
	OMFOOAPI ReadGuid(__in LPCSTR pszPropertyName, __out LPGUID pGuid)= 0;
	OMFOOAPI ReadMobID(__in LPCSTR pszPropertyName, __out POMF_MOB_ID pMobID)= 0;
	OMFOOAPI ReadPoint(__in LPCSTR pszPropertyName, __out POMF_POINT pPoint)= 0;
	OMFOOAPI ReadProductVersion(__in LPCSTR pszPropertyName, __out POMF_PRODUCT_VERSION pVersion)= 0;
	OMFOOAPI ReadVersionType(__in LPCSTR pszPropertyName, __out POMF_VERSIONTYPE pVersionType)= 0;

//	Routines to read omfi:Rational and omfi:ExactEditRate.
	OMFOOAPI ReadRational(__in LPCSTR pszPropertyName, __out POMF_RATIONAL pRational)= 0;
	OMFOOAPI ReadRationalAsFloat(__in LPCSTR pszPropertyName, __out PFLOAT pFloat)= 0;
	OMFOOAPI ReadRationalAsDouble(__in LPCSTR pszPropertyName, __out DOUBLE* pDouble)= 0;

//	Routines to read omfi:Timestamp.
	OMFOOAPI ReadTimeStamp(__in LPCSTR pszPropertyName, __out POMF_TIMESTAMP pTimeStamp)= 0;
	OMFOOAPI ReadTimeStampAsFileTime(__in LPCSTR pszPropertyName, __out LPFILETIME pFileTime)= 0;

//	Routines to read omfi:CompCodeArray and omfi:CompSizeArray.
//	These "arrays" are stored in the file like a variable-length null-terminated string,
//	but they can never have more than eight elements (including the terminator).
//	See the OMF Interchange Specification Version 2.1. Appendix B, Table 9, at page 220.
//	For simplicity, Omfoo always treats these as a fixed-length array of eight elements.
//	Any unused elements at the end are always padded with zeros.
//	If the payload is malformed (i.e. not properly terminated) then these routines will return OMF_E_BAD_ARRAY.
//	On exit the 'array' is guaranteed to be null-terminated.
	OMFOOAPI ReadCompCodeArray(__in LPCSTR pszPropertyName, __out POMF_COMP_CODE_ARRAY pResult)= 0;
	OMFOOAPI ReadCompSizeArray(__in LPCSTR pszPropertyName, __out POMF_COMP_SIZE_ARRAY pResult)= 0;

//	Populates a OMF_DATA_VALUE structure with information about the property specified by pszPropertyName.
//	This includes the physical size of the payload (as measured in bytes), along with the physical file position
//	where the raw payload begins (or, if the size is four bytes or less the payload itself). 
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
	OMFOOAPI QueryDataValue(__in LPCSTR pszPropertyName, __out POMF_DATA_VALUE pDataValue)= 0;

//	Fills the caller's buffer with the payload of the property specified by pszPropertyName.
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
//	This method has optional call-twice semantics. You can make the first call with pBuffer set to NULL to get
//	the required size, and then call again with an appropriately sized buffer to retrieve the full payload.
	OMFOOAPI ReadDataValueAsRawBytes(__in LPCSTR pszPropertyName, __in ULONG cbBuffer,
													__out_opt PVOID pBuffer, __out PULONG pcbBuffer)= 0;

//	Allocates memory with the Win32 GlobalAlloc() function and fills it with the property specified by pszPropertyName.
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
//	The uFlags argument is passed directly to GlobalAlloc(). If zero is specified the default is GMEM_FIXED.
//	If this method succeeds the HGLOBAL it returns must be freed by calling the Win32 API function GlobalFree().
	OMFOOAPI CreateHGlobalOnDataValue(__in LPCSTR pszPropertyName, __in UINT uFlags, __out HGLOBAL* phGlobal)= 0;

//	Creates a standard Windows Stream object on the payload of the property specified by pszPropertyName.
//	The Stream it returns is read-only. The supported interfaces are IUnknown, ISequentialStream, and IStream.
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
	OMFOOAPI CreateStreamOnDataValue(__in LPCSTR pszPropertyName, __in REFIID riid, __out PVOID *ppvOut)= 0;

//	Reads an 80-bit SANE (Standard Apple Numerics Environment) IEEE 754 extended precision floating-point
//	number, and returns it as a 64-bit DOUBLE. Note that the byte-order is always assumed to be big-endian.
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
	OMFOOAPI ReadExtended80AsDouble(__in LPCSTR pszPropertyName, __out DOUBLE* pDouble)= 0;

//	Instantiates the ObjRef specified by pszPropertyName and returns the interface specified by riid.
//	The OMF data type must be omfi:ObjRef or this will fail with the return code OMF_E_TYPE_SURPRISE.
	OMFOOAPI InstantiateObjRef(__in LPCSTR pszPropertyName, __in REFIID riid, __out PVOID *ppvOut)= 0;

//	Creates an iterator to instantiate the objects in the specified object array.
//	The OMF data type must be omfi:ObjRefArray or this will fail with the return code OMF_E_TYPE_SURPRISE.
	OMFOOAPI IterateObjRefArray(__in LPCSTR pszPropertyName, __out IOmfooIterator **ppIterator)= 0;

//	OMF1 only - Creates an iterator to instantiate the objects in the specified mob index array.
//	The OMF data type must be omfi:MobIndex or this method will fail with the return code OMF_E_TYPE_SURPRISE.
	OMFOOAPI IterateMobIndexArray(__in LPCSTR pszPropertyName, __out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfHeader
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Header Class object (HEAD).
//	See OMF Interchange Specification 2.1. Appendix A at page 152.
//**********************************************************s**********************************************************
struct __declspec(uuid("BE412E00-447C-442a-A3E9-EFA1C9B7B0E8")) IOmfHeader;
interface IOmfHeader : public IUnknown
{
//	OMF1 - Retrieves the OMFI:ByteOrder property.
//	OMF2 - Retrieves the OMFI:HEAD:ByteOrder property.
	OMFOOAPI GetByteOrder(__out PWORD pwByteOrder)= 0;

//	OMF1 - Retrieves the OMFI:Version property.
//	OMF2 - Retrieves the OMFI:HEAD:Version property.
//	Note that the bMinor member of the OMF_VERSIONTYPE structure tends to always be zero.
//	If fVerbatim is TRUE this will return the version property exactly as it is stored in the file. 
//	If fVerbatim is FALSE then this method will attempt to provide more meaningful value for bMinor.
//	The current implementation can differentiate between OMF versions 1.0, 1.5, 2.0, and 2.1.
	OMFOOAPI GetVersion(__in BOOL fVerbatim, __out POMF_VERSIONTYPE pVersionType)= 0;

//	OMF1 - Retrieves the OMFI:ToolkitVersion property.
//	OMF2 - Retrieves the OMFI:HEAD:ToolkitVersion property.
	OMFOOAPI GetToolkitVersion(__out POMF_PRODUCT_VERSION pVersion)= 0;

//	OMF1 - Retrieves the OMFI:LastModified property in a Win32 FILETIME structure.
//	OMF2 - Retrieves the OMFI:HEAD:LastModified property in a Win32 FILETIME structure.
	OMFOOAPI GetLastModified(__out LPFILETIME pFileTime)= 0;
	
//	OMF1 & OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:IdentificationList array.
	OMFOOAPI IterateIdentificationList(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:ClassDictionary array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:ClassDictionary array.
	OMFOOAPI IterateClassDictionary(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:MediaData array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:MediaData array.
	OMFOOAPI IterateMediaData(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Searches the OMFI:MediaData array for the media data object (MDAT) specified by pMobID.
//	OMF2 - Searches the OMFI:HEAD:MediaData array for the media data object (MDAT) specified by pMobID.
	OMFOOAPI FindMediaData(__in POMF_MOB_ID pMobID, __in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:ExternalFiles array.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2 because there is no similar property in OMF2.
	OMFOOAPI IterateExternalFiles(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:CompositionMobs array.
//		   Note that despite its name, this array actually holds composition mobs _and_ master mobs.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2 because there is no similar property in OMF2.
	OMFOOAPI IterateCompositionMobs(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:SourceMobs array.
//	OMF2 - Creates an iterator to instantiate all source mobs in the container.
	OMFOOAPI IterateSourceMobs(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:ObjectSpine array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:Mobs array.
	OMFOOAPI IterateMobSpine(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Returns OMF_E_NOT_IN_OMF1 because there is no similar property in OMF1.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:DefinitionObjects array.
	OMFOOAPI IterateDefinitionObjects(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Returns OMF_E_NOT_IN_OMF1 because there is no similar property in OMF1.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:HEAD:PrimaryMobs array.
	OMFOOAPI IteratePrimaryMobs(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 & OMF2 - Instantiates the mob with the specified OMFI:MOBJ:MobID property,
//	and then queries it for the interface specified by riid.
	OMFOOAPI FindMobByID(__in POMF_MOB_ID pMobID, __in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 & OMF2 - Instantiates the Nth mob with the specified name and queries it for the interface specified by riid.
//	The string comparison is not case-sensitive.
//	The nInstance argument is zero-based, so a value of zero will return the first mob whose name matches pszMobName.
//	The dwClassFourCC argument is optional. If it's non-zero then it should be CMOB, MMOB, SMOB, or MOBJ.
//	Returns OMF_E_MOB_NOT_FOUND if there is no match.
	OMFOOAPI FindNthMobByName(__in LPCSTR pszMobName, __in ULONG nInstance,
										__in_opt DWORD dwClassFourCC, __in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 & OMF2 - Finds the top level composition mob.
//	Chapter 5 of the OMF Specification explains how to parse the top level composition mob
//	but it never explains how to find the top level composition mob in the first place!
//	This routine does that for you in a way that is	compatible with OMF1 and OMF2.
//	TIP OF THE DAY: DO NOT query for IOmfCompositionMob or IOmfMasterMob!
//	Instead query for IOmfCommonMob, and then once this method succeeds call IOmfCommonMob::IsACompositionMob()
//	or IOmfCommonMob::IsAMasterMob() to discover the actual type.
	OMFOOAPI FindTopLevelMob(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfClassDictionaryEntry
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Class Dictionary Entry Class object (CLSD).
//	See OMF Interchange Specification 2.1. Appendix A at page 117.
//*********************************************************************************************************************
struct __declspec(uuid("C0516BB2-4018-4d4c-BCB4-55E92A6C20A2")) IOmfClassDictionaryEntry;
interface IOmfClassDictionaryEntry : public IUnknown
{
//	OMF1 - Retrieves the OMFI:CLSD:ClassID property as a 32-bit DWORD.
//	OMF2 - Retrieves the OMFI:CLSD:Class property as a 32-bit DWORD.
	OMFOOAPI GetClassFourCC(__out PDWORD pdwClassFourCC)= 0;

//	OMF1 & OMF2 - Instantiates the ObjRef specified by the OMFI:CLSD:ParentClass property.
//	You will normally query for its IOmfClassDictionaryEntry or IOmfObject interface.
	OMFOOAPI GetParentEntry(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfDataDefinition
//	Available in OMF2 only.
//	This is a specialized interface for accessing an OMF Data Definition Class object (DDEF).
//	See OMF Interchange Specification 2.1. Appendix A at page 132.
//*********************************************************************************************************************
struct __declspec(uuid("6C361C58-5ECD-4391-BC91-4BDCFC5F1DC7")) IOmfDataDefinition;
interface IOmfDataDefinition : public IUnknown
{
//	Retrieves the OMFI:DDEF:DataKindID property (a omfi:UniqueName) as a wide character (UTF-16) string.
	OMFOOAPI GetDataKindIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Reads the OMFI:DDEF:DataKindID property and maps it to one of the OMF_DATA_KIND enumerations.
//	This saves you from having to read the string and then call strcmp() repeatedly until you find a match.
//	Moreover, it does it in a way that is backward-compatible with the OMF1 OMFI:CPNT:TrackKind property.
//	Note that these strings can be anything (as long as they begin with "omfi:data:") and so there is no guarantee
//	that this routine will always succeed. See the list of all known data kind names in "Omfoo_Enumerated_Types.h".
//	Returns OMF_E_DDEF_SURPRISE if there is no match for the string in omfi:UniqueName.
	OMFOOAPI GetDataKindIDWord(__out PWORD pwDataKindID)= 0;
};

//*********************************************************************************************************************
//	IOmfEffectDefinition
//	Available in OMF2 only.
//	This is a specialized interface for accessing an OMF Effect Definition Class object (EDEF).
//	See OMF Interchange Specification 2.1. Appendix A at page 143.
//*********************************************************************************************************************
struct __declspec(uuid("DF3C7CA1-131D-460b-B08D-4722EE4B6B18")) IOmfEffectDefinition;
interface IOmfEffectDefinition : public IUnknown
{
//	Retrieves the OMFI:EDEF:EffectID property (an omfi:UniqueName) as a wide character (UTF-16) string.
	OMFOOAPI GetEffectIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Reads the OMFI:EDEF:EffectID property and maps it to one of the OMF_EFFECT_KIND enumerations.
//	This method saves you from having to read the string and then call strcmp() yourself.
//	See "Omfoo_Enumerated_Types.h"
//	Note that these strings are 'wild and woolly' and can be anything (as long as they begin with "omfi:effect:"),
//	so there is no guarantee that this routine will always succeed.
//	Returns OMF_E_EDEF_SURPRISE if there is no match for the string in omfi:UniqueName.
	OMFOOAPI GetEffectIDWord(__out PWORD pwEffectID)= 0;

//	Retrieves the OMFI:EDEF:EffectName property as a wide character (UTF-16) string.
	OMFOOAPI GetEffectName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:EDEF:EffectDescription property as a wide character (UTF-16) string.
	OMFOOAPI GetDescription(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:EDEF:Bypass, omfi:ArgIDType.
	OMFOOAPI GetBypassArgID(__out PLONG pnArgID)= 0;

//	Returns S_OK if the OMFI:EDEF:IsTimeWarp property (an omfi:Boolean) is TRUE, or S_FALSE if it is FALSE. 
	OMFOOAPI IsTimeWarp(void)= 0;
}; 

//*********************************************************************************************************************
//	IOmfEffectSlot
//	Available in OMF2 only.
//	This is a specialized interface for accessing an OMF Effect Slot Class object (ESLT).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 149.
//*********************************************************************************************************************
struct __declspec(uuid("55244FDC-27F8-43e4-979C-815484F2C666")) IOmfEffectSlot;
interface IOmfEffectSlot : public IUnknown
{
//	OMFI:ESLT:ArgID
	OMFOOAPI GetArgID(__out PDWORD pdwArgID)= 0;

//	OMFI:ESLT:ArgValue
	OMFOOAPI GetArgValue(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfIdentity
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Identification Class object (IDNT).
//	Provides information about the application that created or modified the OMF file.
//	See OMF Interchange Specification 2.1. Appendix A at page 156.
//*********************************************************************************************************************
struct __declspec(uuid("6F8E0D9F-6319-4a3a-BA32-5EC669890CBD")) IOmfIdentity;
interface IOmfIdentity : public IUnknown
{
//	Retrieves the OMFI:IDNT:CompanyName property as a wide character (UTF-16) string.
	OMFOOAPI GetCompanyName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:IDNT:ProductName property as a wide character (UTF-16) string.
	OMFOOAPI GetProductName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:IDNT:ProductVersion property in a OMF_PRODUCT_VERSION structure.
	OMFOOAPI GetProductVersion(__out POMF_PRODUCT_VERSION pVersion)= 0;

//	Retrieves the OMFI:IDNT:ProductVersionString property as a wide character (UTF-16) string.
	OMFOOAPI GetProductVersionString(__in ULONG cchBuffer,
													__out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:IDNT:ProductID property, which is identification number assigned to the application and vendor.
	OMFOOAPI GetProductID(__out PULONG pnProductID)= 0;

//	Retrieves the OMFI:IDNT:Date property in a Win32 FILETIME structure.
	OMFOOAPI GetDate(__out LPFILETIME pFileTime)= 0;

//	Retrieves the version number of the OMF Toolkit library.
	OMFOOAPI GetToolkitVersion(__out POMF_PRODUCT_VERSION pToolkitVersion)= 0;

//	Retrieves the platform on which the application ran, as a wide character (UTF-16) string.
//	Examples: "Windows", "Windows NT", "WIN32", "win32", "Mac", "MacOS", "Macintosh", "Unspecified", etc.
	OMFOOAPI GetPlatform(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfAttribute
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Attribute class object. (ATTB)
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 114.
//*********************************************************************************************************************
struct __declspec(uuid("1596539B-C11E-4607-A360-43575DF6CBD6")) IOmfAttribute;
interface IOmfAttribute : public IUnknown
{
//	Retrieves the OMFI:ATTB:Name property.
//	The ANSI version returns the string exactly as it is stored in the file.
//	The wide character version calls MultiByteToWideChar() using the working code page.
	OMFOOAPI GetAttributeNameA(__in ULONG cchBuffer, __out_opt LPSTR pBuffer, __out_opt PULONG pcchRequired)= 0;
	OMFOOAPI GetAttributeNameW(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:ATTB:Kind property.
//	The attribute kinds are AK_NULL (0), AK_INT32 (1), AK_STRING (2), AK_OBJECT (3), and AK_DATAVALUE (4).
//	See the OMF_ATTRIBUTE_KIND enumerations in "Omfoo_Enumerated_Types.h"
	OMFOOAPI GetAttributeKind(__out PWORD pwAttributeKind)= 0;

//	Retrieves the OMFI:ATTB:IntAttribute property.
//	Use this when the OMFI:ATTB:Kind property is AK_NULL or AK_INT32.
	OMFOOAPI ReadIntAttribute(__out PINT32 pn32)= 0;

//	Retrieves the OMFI:ATTB:StringAttribute property.
//	Use this when the OMFI:ATTB:Kind property is AK_NULL or AK_STRING.
//	The ANSI version returns the string exactly as it is stored in the file.
//	The wide character version calls MultiByteToWideChar() using the working code page.
	OMFOOAPI ReadStringAttributeA(__in ULONG cchBuffer, __out_opt LPSTR pBuffer, __out_opt PULONG pcchRequired)= 0;
	OMFOOAPI ReadStringAttributeW(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Instantiates the ObjRef specified by OMFI:ATTB:ObjAttribute and returns the interface specified by riid.
//	Use this when the OMFI:ATTB:Kind property is AK_NULL or AK_OBJECT.
	OMFOOAPI ReadObjAttribute(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Retrieves the OMFI:ATTB:BobSize property. This sometimes accompanies the OMFI:ATTB:BobData property.
//	Use this when the OMFI:ATTB:Kind property is AK_DATAVALUE.
	OMFOOAPI ReadBobSizeAttribute(__out PULONG pcbBobSize)= 0;

//	Reads and decodes the OMFI:ATTB:BobData property as a null-terminated wide character (UTF-16) string.
//	The OMFI:ATTB:Kind must be AK_NULL (0) or AK_DATAVALUE (4).
//	The payload must be encoded as ASCII, UTF-8, or with the working ANSI code page.
//	Returns OMF_E_BAD_TEXT if the payload is not a null-terminated string.
	OMFOOAPI ReadBobDataAsString(__in ULONG cchBuffer,
											__out_opt PWCHAR pBuffer,
												__out_opt PULONG pcchRequired)= 0;

//	Fills caller's buffer with the payload of the the OMFI:ATTB:BobData or OMFI:ATTB:DataAttribute properties,
//	or any other property whose OMFI:ATTB:Kind is AK_NULL (0) or AK_DATAVALUE (4).
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
	OMFOOAPI ReadBobDataAsRawBytes(__in ULONG cbBuffer, __out_opt PVOID pBuffer, __out_opt PULONG pcbRequired) = 0;

//	Allocates memory with GlobalAlloc() and fills it with the OMFI:ATTB:BobData or OMFI:ATTB:DataAttribute
//	properties, or any other property whose OMFI:ATTB:Kind is AK_NULL (0) or AK_DATAVALUE (4).
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
//	The uFlags argument is passed directly to GlobalAlloc(). If zero is specified the default is GMEM_FIXED.
//	If this method succeeds the HGLOBAL it returns must be freed by calling the Win32 API function GlobalFree().
	OMFOOAPI CreateHGlobalOnDataValue(__in UINT uFlags, __out HGLOBAL* phGlobal)= 0;

//	Creates a standard Windows Stream object on the OMFI:ATTB:BobData or OMFI:ATTB:DataAttribute properties,
//	or any other property whose OMFI:ATTB:Kind is AK_NULL (0) or AK_DATAVALUE (4).
//	The OMF data type must be omfi:DataValue or omfi:VarLenBytes, or this will fail with OMF_E_TYPE_SURPRISE.
//	The Stream it returns is read-only. The supported interfaces are IUnknown, ISequentialStream, and IStream.
	OMFOOAPI CreateStreamOnDataValue(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfAttributeArray
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Attribute Array class object. (ATTR)
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 116.
//*********************************************************************************************************************
struct __declspec(uuid("6B56B5A3-B6D3-4c01-B340-FEC29DC9DF81")) IOmfAttributeArray;
interface IOmfAttributeArray : public IUnknown
{
//	Searches the OMFI:ATTR:AttrRefs array for an attribute by its category name.
//	If it finds a match it instantiates the ATTB and queries it for the interface specified by riid. 
//	In the current version of Omfoo the ATTRs always expose IOmfObject and IOmfAttribute.
//	Future versions of Omfoo may expose additional interfaces.
	OMFOOAPI FindAttbByName(__in LPCSTR pszAttributeName, __in REFIID riid, __out PVOID *ppvOut)= 0;

//	Creates an iterator to examine all members of the OMFI:ATTR:AttrRefs array.
	OMFOOAPI IterateMembers(__out IOmfooIterator **ppIterator)= 0;

//	Searches the OMFI:ATTR:AttrRefs array for an attribute by its category name,
//	and then retrieves its OMFI:ATTB:IntAttribute property (if it exists).
	OMFOOAPI FindIntAttributeByName(__in LPCSTR pszAttributeName, __out PINT32 pn32)= 0;

//	Searches the OMFI:ATTR:AttrRefs array for an attribute by its category name,
//	and retrieves its OMFI:ATTB:StringAttribute property (if it exists).
//	The ANSI version returns the string exactly as it is stored in the payload. The wide character version calls
//	MultiByteToWideChar() on the payload using the working code page, and then returns the result.
//	Note that the pszAttributeName argument is always an LPCSTR.
	OMFOOAPI FindStringAttributeByNameA(__in LPCSTR pszAttributeName, __in ULONG cchBuffer,
												__out_opt LPSTR pBuffer, __out_opt PULONG pcchRequired)= 0;
	OMFOOAPI FindStringAttributeByNameW(__in LPCSTR pszAttributeName, __in ULONG cchBuffer,
												__out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Searches the OMFI:ATTR:AttrRefs array for an attribute by its category name,
//	and instantiates its OMFI:ATTB:ObjAttribute property (if it exists).
//	Then it queries it for the interface specified by riid.
//	Returns E_NOINTERFACE if the result does not expose the requested interface.
	OMFOOAPI FindObjectAttributeByName(__in LPCSTR pszAttributeName, __in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfCommonMob
//	Available in OMF1 and OMF2.
//	This is a common interface for accessing all OMF Mob class objects (MOBJ).
//	Note that in OMF2 the MOBJ class is an abstract class that inherits OOBJ. But in OMF1 the MOBJ class is a concrete
//	class that inherits TRKG. There are no real SMOBs, MMOBs, or CMOBs in OMF1 - they are all just MOBJs!
//	Omfoo uses the same rules that omfToolkit.dll used when deciding if an OMF1 MOBJ is a composition mob, master mob,
//	or source mob: It tests the OMFI:MOBJ:UsageCode and OMFI:MOBJ:PhysicalMedia properties and casts them accordingly.
//*********************************************************************************************************************
struct __declspec(uuid("6A377CB1-17FC-422b-A77E-B9FB47E26716")) IOmfCommonMob;
interface IOmfCommonMob : public IUnknown
{
//	OMF2 only - Returns S_OK if the mob is listed in the Header's OMFI:HEAD:PrimaryMobs array, or S_FALSE if not.
	OMFOOAPI IsAPrimaryMob(void)= 0;

//	These methods return S_OK if the mob is an XXXX mob, or S_FALSE if it is not.
	OMFOOAPI IsACompositionMob(void)= 0;
	OMFOOAPI IsAMasterMob(void)= 0;
	OMFOOAPI IsASourceMob(void)= 0;
	OMFOOAPI IsAFileMob(void)= 0;	// a 'file mob' is a source mob whose OMFI:SMOB:MediaDescription is an MDFL.
	OMFOOAPI IsATapeMob(void)= 0;	// a 'tape mob' is a source mob whose OMFI:SMOB:MediaDescription is an MDTP.
	OMFOOAPI IsAFilmMob(void)= 0;	// a 'film mob' is a source mob whose OMFI:SMOB:MediaDescription is an MDFM.
	OMFOOAPI IsANagraMob(void)= 0;	// a 'nagra mob' is a source mob whose OMFI:SMOB:MediaDescription is an MDNG.

//	OMF1 & OMF2 - Retrieves the OMFI:MOBJ:MobID property.
	OMFOOAPI GetMobID(__out POMF_MOB_ID pMobID)= 0;

//	OMF1 - Retrieves the OMFI:MOBJ:UsageCode property.
//	OMF2 - Retrieves the OMFI:MOBJ:AppCode property.
	OMFOOAPI GetAppCode(__out PDWORD pdwAppCode)= 0;

//	OMF1 - Retrieves the OMFI:CPNT:Name property as a wide character (UTF-16) string.
//	OMF2 - Retrieves the OMFI:MOBJ:Name property as a wide character (UTF-16) string.
	OMFOOAPI GetMobName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:MOBJ:CreationTime property as a Win32 FILETIME structure.
	OMFOOAPI GetCreationTime(__out LPFILETIME pFileTime)= 0;

//	Retrieves the OMFI:MOBJ:LastModified property as a Win32 FILETIME structure.
	OMFOOAPI GetLastModified(__out LPFILETIME pFileTime)= 0;

//	OMF1 - OMFI:CPNT:Attributes
//	OMF2 - OMFI:MOBJ:UserAttributes
	OMFOOAPI GetUserAttributeList(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:TRKG:Tracks array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:MOBJ:Slots array.
	OMFOOAPI IterateSlots(__out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfCompositionMob
//	Inherits IOmfCommonMob
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Composition Mob Class object (CMOB).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 126.
//*********************************************************************************************************************
struct __declspec(uuid("9C490B29-A1F6-479f-91A7-1BA989AB1586")) IOmfCompositionMob;
interface IOmfCompositionMob : public IOmfCommonMob
{
//	OMFI:CMOB:DefFadeLength
	OMFOOAPI GetDefFadeLength(__out PUINT64 pLength)= 0;

//	OMFI:CMOB:DefFadeType
	OMFOOAPI GetDefFadeType(__out PWORD pwFadeType)= 0;

//	OMFI:CMOB:DefFadeEditUnit
	OMFOOAPI GetDefFadeEditUnit(__out POMF_RATIONAL pEditUnit)= 0;
	OMFOOAPI GetDefFadeEditUnitAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetDefFadeEditUnitAsDouble(__out DOUBLE* pDouble)= 0;
};

//*********************************************************************************************************************
//	IOmfMasterMob
//	Inherits IOmfCommonMob
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Master Mob Class object (MMOB).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 162.
//*********************************************************************************************************************
struct __declspec(uuid("88704A29-10A8-4201-834B-5066B63BCA73")) IOmfMasterMob;
interface IOmfMasterMob : public IOmfCommonMob
{
	// (No additional methods.)
};

//*********************************************************************************************************************
//	IOmfSourceMob
//	Inherits IOmfCommonMob
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Source Mob Class object (SMOB).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 200.
//*********************************************************************************************************************
struct __declspec(uuid("13DD8EDF-AC9C-45fb-9615-D1008F0A67C9")) IOmfSourceMob;
interface IOmfSourceMob : public IOmfCommonMob
{
//	OMF1 - Instantiates the ObjRef specified by the OMFI:MOBJ:PhysicalMedia property.
//	OMF2 - Instantiates the ObjRef specified by the OMFI:SMOB:MediaDescription property.
//	The result should always expose IOmfObject and IOmfMediaDescriptor.
	OMFOOAPI GetMediaDescriptor(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Instantiates the mob's Media Data object (MDAT) and then queries for the interface specified by riid.
//	Returns E_NOINTERFACE if the result does not expose the requested interface.
//	The result should always expose IOmfObject and IOmfMediaData.
	OMFOOAPI GetMediaData(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	If the media described by this Source Mob has been derived from a previous generation of media, then
//	this routine instantiates the previous generation's mob - which is typically a Tape Mob or a Film Mob.
//	Returns E_NOINTERFACE if the result does not expose the requested interface.
//	See also IOmfSourceClipSegment::ResolveRef().
	OMFOOAPI GetPreviousGenerationMob(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfMobSlot
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing OMF1 Track objects (TRAK) and OMF2 Mob Slot objects (MSLT).
//*********************************************************************************************************************
struct __declspec(uuid("5EEA0E6C-BF24-433c-A07D-BB6B71467BBB")) IOmfMobSlot;
interface IOmfMobSlot : public IUnknown
{
//	OMF1 - Retrieves the OMF class fourCC of the ObjRef specified by the OMFI:TRAK:TrackComponent property.
//	OMF2 - Retrieves the OMF class fourCC of the ObjRef specified by the OMFI:MSLT:Segment property.
	OMFOOAPI GetSegmentClass(__out PDWORD pdwFourCC)= 0;

//	OMF1 - Instantiates the ObjRef specified by the OMFI:TRAK:TrackComponent property.
//	OMF2 - Instantiates the ObjRef specified by the OMFI:MSLT:Segment property.
	OMFOOAPI GetSegment(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Retrieves the track component's OMFI:CPNT:EditRate property.
//	OMF2 - Retrieves the OMFI:MSLT:EditRate property.
	OMFOOAPI GetEditRate(__out POMF_RATIONAL pEditRate)= 0;
	OMFOOAPI GetEditRateAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetEditRateAsDouble(__out DOUBLE* pDouble)= 0;

//	OMF1 - Returns OMF_E_NOT_IN_OMF1 because there are no Track Descriptions (TRKD) in OMF1.
//	OMF2 - Instantiates the ObjRef specified by the OMFI:MSLT:TrackDesc property.
//		   Note that all of the TRKD's properties can be accessed indirectly right here in IOmfMobSlot.
//		   This is by design so that Omfoo can provide a uniform workflow across OMF1 and OMF2.
	OMFOOAPI GetTrackDesc(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Retrieves the OMFI:MOBJ:StartPosition property from the MOBJ that owns this track.
//	OMF2 - Retrieves the track descriptor's OMFI:TRKD:Origin property.
	OMFOOAPI GetOrigin(__out PINT64 pOrigin)= 0;

//	OMF1 - Retrieves the OMFI:TRAK:LabelNumber property.
//	OMF2 - Retrieves the track descriptor's OMFI:TRKD:TrackID property.
	OMFOOAPI GetTrackID(__out PDWORD pdwTrackID)= 0;

//	OMF1 - Returns the TRAK's one-based position in the owning mob's OMFI:TRKG:Tracks array.
//	OMF2 - Returns the MSLT's one-based position in the owning mob's OMFI:MOBJ:Slots array.
	OMFOOAPI GetCompatibleTrackID(__out PDWORD pdwTrackID)= 0;

//	OMF1 - Returns OMF_E_NOT_IN_OMF1.
//	OMF2 - Retrieves the track's name from the track descriptor's OMFI:TRKD:TrackName property.
	OMFOOAPI GetTrackName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMF1 - Returns OMF_E_NOT_IN_OMF1.
//	OMF2 - Retrieves the track descriptor's OMFI:TRKD:PhysicalTrack property.
	OMFOOAPI GetPhysicalTrack(__out PDWORD pdwPhysicalTrack)= 0;

//	OMF1 - Instantiates the ObjRef specified by the OMFI:TRAK:FillerProxy property.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2.
	OMFOOAPI GetFillerProxy(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Instantiates the ObjRef specified by the OMFI:TRAK:Attributes property.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2.
	OMFOOAPI GetTrackAttributeList(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Instantiates the ObjRef specified by the OMFI:TRAK:SessionAttrs property.
//	OMF2 - Returns OMF_E_NOT_IN_OMF2.
	OMFOOAPI GetSessionAttributeList(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfTrackDescription
//	Available in OMF2 only.
//	This is a specialized interface for accessing an OMF Track Description Class object (TRKD).
//	See OMF Interchange Specification 2.1. Appendix A at page 207.
//	Note that all of the TRKD's properties can also be retrieved indirectly from IOmfMobSlot.
//*********************************************************************************************************************
struct __declspec(uuid("E9002D78-9606-46b5-B9CF-8BD8F9622D58")) IOmfTrackDescription;
interface IOmfTrackDescription : public IUnknown
{
//	Retrieves the OMFI:TRKD:Origin property.
	OMFOOAPI GetOrigin(__out PINT64 pOrigin)= 0;

//	Retrieves the OMFI:TRKD:TrackID property.
	OMFOOAPI GetTrackID(__out PDWORD pdwTrackID)= 0;

//	Retrieves the OMFI:TRKD:PhysicalTrack property.
	OMFOOAPI GetPhysicalTrack(__out PDWORD pdwPhysicalTrack)= 0;

//	Retrieves the OMFI:TRKD:TrackName property as a wide character (UTF-16) string.
	OMFOOAPI GetTrackName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfComponent
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Component Class object (CPNT).
//	In OMF1 the standard component classes are:
//		ATCP, CLIP, ECCP, FILL, MASK, MOBJ, REPT, SCLP, SEQU, SLCT, SPED, TCCP, TRAN, TRKG, TRKR, WARP.
//	In OMF2 the standard component classes are:
//		CVAL, ECCP, ERAT, EFFE, FILL, MGRP, NEST, PDWN, SREF, SLCT, SEQU, SCLP, TCCP, TRAN, VVAL.
//*********************************************************************************************************************
struct __declspec(uuid("4348411D-1EBE-4828-BFB0-55AE07F886F6")) IOmfComponent;
interface IOmfComponent : public IUnknown
{
//	OMF1 only - Retrieves the OMFI:CPNT:Name property.
	OMFOOAPI GetCpntName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Returns S_OK if the component is a transition (TRAN) or S_FALSE if it is not.
	OMFOOAPI IsATransition(void)= 0;

//	OMF1 - Retrieves the component's OMFI:CPNT:EffectID property as a wide character (UTF-16) string.
//	OMF2 - Retrieves the OMFI:DDEF:DataKindID string from the Data Definition object (DDEF) - which is
//		   specified in the component's OMFI:CPNT:DataKind property - as a wide character (UTF-16) string.
	OMFOOAPI GetDataKindIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the component's data kind. See the OMF_DATAKIND enumeration for details.
//	OMF1 - This comes directly from the OMFI:CPNT:TrackKind property.
//	OMF2 - This comes indirectly from the OMFI:CPNT:DataKind property.
	OMFOOAPI GetDataKindIDWord(__out PWORD pwDataKind)= 0;

//	OMF2 only - Instantiates the ObjRef specified by the OMFI:CPNT:DataKind property.
//	Note that you can retrieve all of the Data Definition object's properties indirectly using this
//	(the IOmfComponent) interface. Nevertheless if you ever really want access to the Data Definition
//	object (DDEF) you can get its IOmfDataDefinition interface by calling this.
	OMFOOAPI GetDataDefinitionObject(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 only - Retrieves the OMFI:CPNT:EditRate property.
	OMFOOAPI GetEditRate(__out POMF_RATIONAL pEditRate)= 0;
	OMFOOAPI GetEditRateAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetEditRateAsDouble(__out DOUBLE* pDouble)= 0;

//	OMF1 - If the component is a CLIP then this retrieves the OMFI:CLIP:Length property.
//		   If the component is a TRKG then this retrieves the OMFI:TRKG:GroupLength property.
//		   If the component is a SEQU then the length is calculated at runtime.
//	OMF2 - Retrieves the OMFI:CPNT:Length property.
	OMFOOAPI GetLength(__out PUINT64 pLength)= 0;

//	OMF1 only - Instantiates the OMFI:CPNT:Precomputed object.
	OMFOOAPI GetPrecomputed(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 - Instantiates the OMFI:CPNT:Attributes object and queries for the interface specified by riid.
//	OMF2 - Instantiates the OMFI:CPNT:UserAttributes object and queries for the interface specified by riid.
//	The attributes object is typically an Attribute Array Class object (ATTR).
	OMFOOAPI GetCpntAttributeList(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMF1 only - Instantiates the OMFI:CPNT:SessionAttrs object and queries for the interface specified by riid.
//	The attributes object is typically an Attribute Array Class object (ATTR).
	OMFOOAPI GetSessionAttributeList(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfTransition
//	Inherits IOmfComponent
//	This is a specialized interface for accessing an OMF Transition Class object (TRAN).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 209.
//*********************************************************************************************************************
struct __declspec(uuid("96CCEFE2-77AE-4497-93CD-CC6056A72183")) IOmfTransition;
interface IOmfTransition : public IOmfComponent
{
//	Retrieves the OMFI:TRAN:CutPoint property.
	OMFOOAPI GetCutPoint(__out PINT64 pCutPoint)= 0;

//	Instantiates the ObjRef specified by the OMFI:TRAN:Effect property.
	OMFOOAPI GetEffect(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfSegment
//	Inherits IOmfComponent
//	OMF1 - This is a specialized interface for accessing an OMF Clip Class object (CLIP).
//	OMF2 - This is a specialized interface for accessing an OMF Segment Class object (SEGM).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 192.
//*********************************************************************************************************************
struct __declspec(uuid("CCD568C6-4A1B-46a6-A365-172B7A3F02C8")) IOmfSegment;
interface IOmfSegment : public IOmfComponent
{
	// This is an abstract class. There are no additional properties or methods.
};

//	In OMF1 this class was called a Clip component (CLIP).
//	CLIP and SEGM are essentially the same thing.
typedef IOmfSegment IOmfClipComponent;

//*********************************************************************************************************************
//	IOmfAttributesClip
//	Inherits IOmfClipComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Attributes Clip Class object (ATCP).
//*********************************************************************************************************************
struct __declspec(uuid("6EA85610-6802-46ef-9768-9E9282D6417B")) IOmfAttributesClip;
interface IOmfAttributesClip : public IOmfClipComponent
{
//	Instantiates the ObjRef specified by the OMFI:ATCP:Attributes property.
//	This may be an Attribute Array Class object (ATTR) or an Attribute Class object (ATTB). 
	OMFOOAPI GetAtcpAttributeList(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfTrackGroupComponent
//	Inherits IOmfComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Track Group Class object (TRKG).
//*********************************************************************************************************************
struct __declspec(uuid("4147F588-352B-4cf9-BAA9-8B23CEDF2011")) IOmfTrackGroupComponent;
interface IOmfTrackGroupComponent : public IOmfComponent
{
//	Note that the OMFI:TRKG:GroupLength property is retrieved with IOmfComponent::GetLength().

//	Creates an iterator to examine members of the OMFI:TRKG:Tracks property.
	OMFOOAPI IterateTracks(__out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfTrackReference
//	Inherits IOmfClipComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Track Reference Class object (TRKR).
//*********************************************************************************************************************
struct __declspec(uuid("FDA5F8A6-B573-4de6-B8DE-E78F0BF12696")) IOmfTrackReference;
interface IOmfTrackReference : public IOmfClipComponent
{
//	OMFI:TRKR:RelativeScope
	OMFOOAPI GetRelativeScope(__out PWORD pwRelativeScope)= 0;

//	OMFI:TRKR:RelativeTrack
	OMFOOAPI GetRelativeTrack(__out PWORD pwRelativeTrack)= 0;
};

//*********************************************************************************************************************
//	IOmfTimeWarpComponent
//	Inherits IOmfTrackGroupComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Time Warp Class object (WARP).
//*********************************************************************************************************************
struct __declspec(uuid("1CDE0D2B-2195-41cd-9AEF-CBFF8A1BF38E")) IOmfTimeWarpComponent;
interface IOmfTimeWarpComponent : public IOmfTrackGroupComponent
{
//	OMFI:WARP:EditRate
	OMFOOAPI GetWarpEditRate(__out POMF_RATIONAL pEditRate)= 0;
	OMFOOAPI GetWarpEditRateAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetWarpEditRateAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:WARP:PhaseOffset
	OMFOOAPI GetPhaseOffset(__out PWORD pwOffset)= 0;
};

//*********************************************************************************************************************
//	IOmfVideoMaskComponent
//	Inherits IOmfTimeWarpComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Video Frame Mask Class object (MASK).
//*********************************************************************************************************************
struct __declspec(uuid("BB96A79D-1F35-4a64-83EB-ED168974F61B")) IOmfVideoMaskComponent;
interface IOmfVideoMaskComponent : public IOmfTimeWarpComponent
{
//	OMFI:MASK:IsDouble
	OMFOOAPI IsDouble(void)= 0;

//	OMFI:MASK:MaskBits
	OMFOOAPI GetMaskBits(__out PDWORD pdwMaskBits)= 0;
};

//*********************************************************************************************************************
//	IOmfVideoRepeatComponent
//	Inherits IOmfTimeWarpComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Video Repeat Class object (REPT).
//*********************************************************************************************************************
struct __declspec(uuid("434038F8-11D6-465c-9F85-D57413307540")) IOmfVideoRepeatComponent;
interface IOmfVideoRepeatComponent : public IOmfTimeWarpComponent
{
	// No additional properties or methods.
};

//*********************************************************************************************************************
//	IOmfVideoSpeedComponent
//	Inherits IOmfTimeWarpComponent
//	Available in OMF1 only.
//	This is a specialized interface for accessing an OMF Video Speed Class object (SPED).
//*********************************************************************************************************************
struct __declspec(uuid("16066F59-2A75-4898-B2C5-10AD0B5AB1EE")) IOmfVideoSpeedComponent;
interface IOmfVideoSpeedComponent : public IOmfTimeWarpComponent
{
//	OMFI:SPED:IsFreezeFrame
	OMFOOAPI IsFreezeFrame(void)= 0;

//	OMFI:SPED:Numerator / OMFI:SPED:Denominator
	OMFOOAPI GetSpeedAsRational(__out POMF_RATIONAL pSpeed)= 0;
	OMFOOAPI GetSpeedAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetSpeedAsDouble(__out DOUBLE* pDouble)= 0;
};

//*********************************************************************************************************************
//	IOmfConstantValueSegment
//	Inherits IOmfSegment
//	Available in OMF2 only.
//	This is a specialized interface for accessing an OMF Constant Value Class object (CVAL).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 128.
//*********************************************************************************************************************
struct __declspec(uuid("C0FCA657-8343-410a-9E30-3BFB08123B1E")) IOmfConstantValueSegment;
interface IOmfConstantValueSegment : public IOmfSegment
{
//	Retrieves the OMFI:CVAL:ParameterID	property (only in OMF v2.0, removed in OMF v2.1).
	OMFOOAPI GetParameterID(__out LPGUID pGuid)= 0;

//	The methods below retrieve the OMFI:CVAL:Value property - whose data type is always omfi:DataValue.
//	Note that CVAL inherits SEGM, which in turn inherits CPNT. So call IOmfComponent::GetDataKind() first to determine
//	which routine to use. See the OMF_DATAKIND enumerated values in "Omfoo_Enumerated_Types.h" for details.
//	Note that not all data kinds can be supported because there are an (almost) infinite number of possibilities.
//	Any application using omfToolkit.dll can create its own data kind strings on the fly.
//	If you need to retrieve an unsupported data kind then you can call IOmfComponent::GetDataKindIDString()
//	and try to make sense of it yourself in your own code, or present it to your user and let them decide. 

//	Call this when the OMFI:CPNT:DataKind is DK_STRING (omfi:data:String).
	OMFOOAPI GetValueAsString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_INT8 (omfi:data:Int8).
	OMFOOAPI GetValueAsByte(__out PBYTE pb)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_INT16 (omfi:data:Int16).
	OMFOOAPI GetValueAsInt16(__out PINT16 pn16)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_INT32 (omfi:data:Int32).
	OMFOOAPI GetValueAsInt32(__out PINT32 pn32)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_INT64 (omfi:data:Int64).
	OMFOOAPI GetValueAsInt64(__out PINT64 pn64)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_UINT16 (omfi:data:UInt16).
	OMFOOAPI GetValueAsUInt16(__out PUINT16 pu16)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_UINT32 (omfi:data:UInt32).
	OMFOOAPI GetValueAsUInt32(__out PUINT32 pu32)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_UINT64 (omfi:data:UInt64).
	OMFOOAPI GetValueAsUInt64(__out PUINT64 pu64)= 0;

//	Call this when the OMFI:CPNT:DataKind is DK_POINT (omfi:data:Point).
	OMFOOAPI GetValueAsPoint(__out POMF_POINT pPoint)= 0;

//	OMF2 - Call any of these when the OMFI:CPNT:DataKind is DK_RATIONAL (omfi:data:Rational).
	OMFOOAPI GetValueAsRational(__out POMF_RATIONAL pRational)= 0;
	OMFOOAPI GetRationalAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetRationalAsDouble(__out DOUBLE* pDouble)= 0;

//	Default helpers for when the OMFI:CPNT:DataKind property is unknown or none of the above.
	OMFOOAPI GetValueAsDataValue(__out POMF_DATA_VALUE pDataValue)= 0;
	OMFOOAPI CreateHGlobalOnDataValue(__in UINT uFlags, __out HGLOBAL* phGlobal)= 0;
	OMFOOAPI CreateStreamOnDataValue(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfEdgecodeSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Edgecode Class object (ECCP).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 139.
//*********************************************************************************************************************
struct __declspec(uuid("D71A1C1D-4302-47e2-8265-02C5C83400AB")) IOmfEdgecodeSegment;
interface IOmfEdgecodeSegment : public IOmfSegment
{
//	OMFI:ECCP:Start, OMFI:ECCP:StartEC
	OMFOOAPI GetStart(__out PINT64 pStart)= 0;
	
//	OMFI:ECCP:FilmKind
	OMFOOAPI GetFilmKind(__out PWORD pwFilmKind)= 0;

//	OMFI:ECCP:CodeFormat
	OMFOOAPI GetCodeFormat(__out PWORD pwCodeFormat)= 0;

//	OMFI:ECCP:Header
	OMFOOAPI GetHeader(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfEffectSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Effect Invocation Class object (EFFE).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 145.
//*********************************************************************************************************************
struct __declspec(uuid("E8B5A1B0-60EB-4ad9-B75B-A40962209F54")) IOmfEffectSegment;
interface IOmfEffectSegment : public IOmfSegment
{
//	Retrieves the OMFI:EDEF:EffectID string from the Effect Definition Class object (EDEF) specified
//	in the OMFI:EFFE:EffectKind property as a wide character (UTF-16) string.
	OMFOOAPI GetEffectIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Reads the OMFI:EDEF:EffectID string from the Effect Definition Class object (EDEF) specified in
//	the OMFI:EFFE:EffectKind property, and maps it to one of the OMF_EFFECT_KINDs in "Omfoo_Enumerated_Types.h"
	OMFOOAPI GetEffectIDWord(__out PWORD pwEffectID) =0;

//	Retrieves the OMFI:EDEF:EffectName string from the Effect Definition Class object (EDEF) specified
//	in the OMFI:EFFE:EffectKind property as a wide character (UTF-16) string.
	OMFOOAPI GetEffectName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:EDEF:EffectDescription string from the Effect Definition Class object (EDEF) specified
//	in the OMFI:EFFE:EffectKind property as a wide character (UTF-16) string.
	OMFOOAPI GetEffectDescription(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the OMFI:EDEF:IsTimeWarp property from the Effect Definition Class object (EDEF) specified in
//	the OMFI:EFFE:EffectKind property. Returns S_OK if the omfi:Boolean is TRUE or S_FALSE if it is FALSE. 
	OMFOOAPI IsTimeWarp(void)= 0;

//	Retrieves the OMFI:EDEF:Bypass property from the Effect Definition Class object (EDEF) specified in
//	the OMFI:EFFE:EffectKind property. 
	OMFOOAPI GetBypassArgID(__out PLONG pnArgID)= 0;

//	Instantiates the ObjRef specified by the OMFI:EFFE:EffectKind property.
//	You will normally query for the IOmfEffectDefinition interface.
//	Note that you can access all of the Effect Definition Class object's properties indirectly using
//	this (the IOmfEffectSegment) interface. Nevertheless if you ever really need it here's how to get it.
	OMFOOAPI GetEffectDefinitionObject(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMFI:EFFE:BypassOverride
	OMFOOAPI GetBypassOverride(__out PLONG pnBypassOverride)= 0;

//	OMFI:EFFE:EffectSlots
	OMFOOAPI IterateEffectSlots(__out IOmfooIterator **ppIterator)= 0;

//	OMFI:EFFE:FinalRendering
	OMFOOAPI GetFinalRendering(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMFI:EFFE:WorkingRendering
	OMFOOAPI GetWorkingRendering(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfEditRateSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Edit Rate Converter Class object (ERAT).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 141.
//*********************************************************************************************************************
struct __declspec(uuid("201836E7-87A9-4e86-89A9-E5452D8BC06B")) IOmfEditRateSegment;
interface IOmfEditRateSegment : public IOmfSegment
{
//	Instantiates the ObjRef specified by the OMFI:ERAT:InputSegment property.
	OMFOOAPI GetInputSegment(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMFI:ERAT:InputEditRate
	OMFOOAPI GetInputEditRate(__out POMF_RATIONAL pEditRate)= 0;
	OMFOOAPI GetInputEditRateAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetInputEditRateAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:ERAT:InputOffset
	OMFOOAPI GetInputOffset(__out PINT64 pInputOffset)= 0;

//	OMFI:ERAT:ResultOffset
	OMFOOAPI GetResultOffset(__out PINT64 pResultOffset)= 0;
};

//*********************************************************************************************************************
//	IOmfFillerSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Filler Class object (FILL).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 151.
//*********************************************************************************************************************
struct __declspec(uuid("55C3F540-B1F0-4a72-BCB2-1C6E8A7B59AA")) IOmfFillerSegment;
interface IOmfFillerSegment : public IOmfSegment
{
	// No additional properties.
};

//*********************************************************************************************************************
//	IOmfMediaGroupSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Media Group Class object (MGRP).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 171.
//*********************************************************************************************************************
struct __declspec(uuid("2B01D7E0-F59F-4a58-8A77-6C0F124A3C17")) IOmfMediaGroupSegment;
interface IOmfMediaGroupSegment : public IOmfSegment
{
//	Creates an iterator to instantiate the objects in the OMFI:MGRP:Choices array.
	OMFOOAPI IterateChoices(__out IOmfooIterator **ppIterator)= 0;

//	Instantiates the ObjRef specified by the OMFI:MGRP:StillFrame property.
	OMFOOAPI GetStillFrame(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfNestedScopeSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Nested Scope Class object (NEST).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 179.
//*********************************************************************************************************************
struct __declspec(uuid("CA08FE16-D803-42dd-8FEC-EE31B2A100EC")) IOmfNestedScopeSegment;
interface IOmfNestedScopeSegment : public IOmfSegment
{
//	Creates an iterator to instantiate the members of the OMFI:NEST:Slots array.
	OMFOOAPI IterateSlots(__out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfPulldownSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Pulldown Class object (PDWN).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 183.
//*********************************************************************************************************************
struct __declspec(uuid("554750A0-76C8-4501-B782-5E1748C2FD20")) IOmfPulldownSegment;
interface IOmfPulldownSegment : public IOmfSegment
{
//	OMFI:PDWN:InputSegment
	OMFOOAPI GetInputSegment(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMFI:PDWN:PhaseFrame
	OMFOOAPI GetPhaseFrame(__out PWORD pwPhaseFrame)= 0;

//	OMFI:PDWN:PulldownDirection
	OMFOOAPI GetPulldownDirection(__out PWORD pwDirection)= 0;

//	OMFI:PDWN:PulldownKind
	OMFOOAPI GetPulldownKind(__out PWORD pwPulldownKind)= 0;
};

//*********************************************************************************************************************
//	IOmfSourceClipSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Source Clip Class object (SCLP).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 197.
//*********************************************************************************************************************
struct __declspec(uuid("4E57B67B-1E4D-42e0-B40C-2CD581BD1463")) IOmfSourceClipSegment;
interface IOmfSourceClipSegment : public IOmfSegment
{
//	Retrieves the OMFI:SCLP:SourceID property.
	OMFOOAPI GetSourceID(__out POMF_MOB_ID pMobID)= 0;

//	OMF1 - Retrieves the OMFI:SCLP:SourceTrack property.
//	OMF2 - Retrieves the OMFI:SCLP:SourceTrackID property.
//	Returns S_FALSE and 0x00000000 when the OMFI:SCLP:SourceID property is 0-0-0.
	OMFOOAPI GetSourceTrackID(__out PDWORD pdwTrackID)= 0;

//	Instantiates the Mob Class object (MOBJ) specified by the OMFI:SCLP:SourceID property.
//	Returns S_FALSE (and *ppv == NULL) if this Source Clip describes the original source.
//	See also IOmfSourceMob::GetPreviousGenerationMob().
	OMFOOAPI ResolveRef(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	OMFI:SCLP:SourcePosition, OMFI:SCLP:StartTime
	OMFOOAPI GetStartTime(__out PINT64 pStartTime)= 0;

//	OMFI:SCLP:FadeInLength
	OMFOOAPI GetFadeInLength(__out PUINT64 pLength)= 0;

//	OMFI:SCLP:FadeInType
	OMFOOAPI GetFadeInType(__out PDWORD pdwFadeType)= 0;

//	OMFI:SCLP:FadeOutLength
	OMFOOAPI GetFadeOutLength(__out PUINT64 pLength)= 0;

//	OMFI:SCLP:FadeOutType
	OMFOOAPI GetFadeOutType(__out PDWORD pdwFadeType)= 0;
};

//*********************************************************************************************************************
//	IOmfSequenceSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Sequence Class object (SEQU).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 195.
//*********************************************************************************************************************
struct __declspec(uuid("08722344-4947-4389-9320-63CFC7AC0393")) IOmfSequenceSegment;
interface IOmfSequenceSegment : public IOmfSegment
{
//	OMF1 - Creates an iterator to instantiate the objects in the OMFI:SEQU:Sequence array.
//	OMF2 - Creates an iterator to instantiate the objects in the OMFI:SEQU:Components array.
	OMFOOAPI IterateComponents(__out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfSelectorSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Selector Class object (SLCT).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 193.
//*********************************************************************************************************************
struct __declspec(uuid("6CA79656-3591-4d92-8075-5E9BBD81EEAE")) IOmfSelectorSegment;
interface IOmfSelectorSegment : public IOmfSegment
{
//	OMF1 only - Returns S_OK if the OMFI:SLCT:IsGanged property is TRUE or S_FALSE if the property is FALSE.
	OMFOOAPI IsGanged(void)= 0;

//	OMF1 only - Retrieves the OMFI:SLCT:SelectedTrack property.
	OMFOOAPI GetSelectedTrack(__out PWORD pwTrack)= 0;

//	OMF2  - Creates an iterator to instantiate the objects in the OMFI:TRKG:Tracks array.
//	OMF2  - Creates an iterator to instantiate the objects in the OMFI:SLCT:Alternates array.
	OMFOOAPI IterateAlternates(__out IOmfooIterator **ppIterator)= 0;

//	OMF1 - Instantiates the ObjRef specified by the OMFI:SLCT:SelectedTrack property.
//	OMF2 - Instantiates the ObjRef specified by the OMFI:SLCT:Selected property.
	OMFOOAPI GetSelected(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfScopeReferenceSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Scope Reference Class object (SREF).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 190.
//*********************************************************************************************************************
struct __declspec(uuid("6DEC612B-0751-4b71-A745-0CCBD1C9C72D")) IOmfScopeReferenceSegment;
interface IOmfScopeReferenceSegment : public IOmfSegment
{
//	OMFI:SREF:RelativeScope
	OMFOOAPI GetRelativeScope(__out PDWORD pdwRelativeScope)= 0;

//	OMFI:SREF:RelativeSlot
	OMFOOAPI GetRelativeSlot(__out PDWORD pdwRelativeSlot)= 0;
};

//*********************************************************************************************************************
//	IOmfTimecodeSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Timecode Class object (TCCP).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 206.
//*********************************************************************************************************************
struct __declspec(uuid("7115FCEF-679C-4934-AF4F-75CCF54D4450")) IOmfTimecodeSegment;
interface IOmfTimecodeSegment : public IOmfSegment
{
//	Returns S_OK if the timecode is drop, or S_FALSE if it is non-drop.
//	OMF1 - This comes from the OMFI:TCCP:Flags property.
//	OMF2 - This comes from the OMFI:TCCP:Drop property.
	OMFOOAPI IsDrop(void)= 0;

//	Retrieves the OMFI:TCCP:FPS property.
	OMFOOAPI GetFramesPerSec(__out PWORD pwFPS)= 0;

//	Retrieves the timecode at the beginning of the segment. This is the number of frames since midnight.
//	OMF1 - This comes from the OMFI:TCCP:StartTC property.
//	OMF2 - This comes from the OMFI:TCCP:Start property.
	OMFOOAPI GetStart(__out PINT64 pStart)= 0;
};

//*********************************************************************************************************************
//	IOmfVaryingValueSegment
//	Inherits IOmfSegment
//	This is a specialized interface for accessing an OMF Varying Value Class object (VVAL).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 212.
//*********************************************************************************************************************
struct __declspec(uuid("08679E6C-B5E8-43ed-A916-87AD9B59A292")) IOmfVaryingValueSegment;
interface IOmfVaryingValueSegment : public IOmfSegment
{
//	OMFI:VVAL:ParameterID
	OMFOOAPI GetParameterID(__out LPGUID pGuid)= 0;

//	OMFI:VVAL:Interpolation
	OMFOOAPI GetInterpolationKind(__out PWORD pwInterpKind)= 0;

//	OMFI:VVAL:Extrapolation
	OMFOOAPI GetExtrapolationKind(__out PWORD pwExtrapKind)= 0;

//	OMFI:VVAL:FieldCount
	OMFOOAPI GetFieldCount(__out PWORD pwFieldCount)= 0;

//	OMFI:VVAL:PointList
	OMFOOAPI IterateControlPoints(__out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfControlPoint
//	This is a specialized interface for accessing an OMF Control Point object (CTLP).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 130.
//*********************************************************************************************************************
struct __declspec(uuid("A626169B-94FC-4420-AEAC-1A83B5A946B0")) IOmfControlPoint;
interface IOmfControlPoint : public IUnknown
{
//	Maps the Data Definition object (DDEF) specified in the OMFI:CTLP:DataKind property to one of the OMF_DATAKIND
//	enumerations defined in "Omfoo_Enumerated_values.h"
//	Returns OMF_E_DDEF_SURPRISE if the Data Definition object's OMFI:DDEF:DataKindID string is not recognized as
//	one of the DK_ strings listed in "Omfoo_Enumerated_values.h"
	OMFOOAPI GetDataKind(__out PWORD pwDataKind)= 0;

//	Retrieves the OMFI:DDEF:DataKindID string from the Data Definition object (DDEF) specified
//	in the control point 's OMFI:CTLP:DataKind property as a wide character (UTF-16) string.
	OMFOOAPI GetDataKindIDString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:CTLP:EditHint
	OMFOOAPI GetEditHintType(__out PWORD pwHintType)= 0;

//	OMFI:CTLP:Time
	OMFOOAPI GetTime(__out POMF_RATIONAL pRational)= 0;
	OMFOOAPI GetTimeAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetTimeAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:CTLP:PointProperties
	OMFOOAPI IteratePointProperties(__out IOmfooIterator **ppIterator)= 0;

//	The methods below retrieve the OMFI:CTLP:Value property.
//	Call IOmfControlPoint::GetDataKind() first to determine which routine to use.
//	For example if wDataKind is DK_UINT16 then call IOmfControlPoint::GetValueAsUInt16() to retrieve the value.
//	Note that there are over forty known data kind strings! Omfoo cannot guarantee to be able to read all of them.
//	If you need to retrieve an unsupported data kind then you can ...
//	1) Call GetValueAsDataValue() to discover where the payload is stored in the OMF File.
//	2) Query IOmfControlPoint for its IOmfObject and use IOmfObject::ReadRawBytes() to retrieve the data.
	OMFOOAPI GetValueAsString(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;
	OMFOOAPI GetValueAsByte(__out PBYTE pb)= 0;
	OMFOOAPI GetValueAsInt16(__out PINT16 pn16)= 0;
	OMFOOAPI GetValueAsInt32(__out PINT32 pn32)= 0;
	OMFOOAPI GetValueAsInt64(__out PINT64 pn64)= 0;
	OMFOOAPI GetValueAsUInt16(__out PUINT16 pu16)= 0;
	OMFOOAPI GetValueAsUInt32(__out PUINT32 pu32)= 0;
	OMFOOAPI GetValueAsUInt64(__out PUINT64 pu64)= 0;
	OMFOOAPI GetValueAsPoint(__out POMF_POINT pPoint)= 0;
	OMFOOAPI GetValueAsRational(__out POMF_RATIONAL pRational)= 0;
	OMFOOAPI GetRationalAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetRationalAsDouble(__out DOUBLE* pDouble)= 0;
};

//*********************************************************************************************************************
//	IOmfMediaDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Media Descriptor Class object (MDES).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 165.
//*********************************************************************************************************************
struct __declspec(uuid("97826459-50F1-40ae-B730-95BAB9FEE68A")) IOmfMediaDescriptor;
interface IOmfMediaDescriptor : public IUnknown
{
//	OMFI:MDES:MobKind
	OMFOOAPI GetPhysicalMobKind(__out PWORD pwMobKind)= 0;

//	OMFI:MDES:KitCodecID - as a wide character (UTF-16) string.
	OMFOOAPI GetKitCodecID(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:MDES:KitCodecDesc - as a wide character (UTF-16) string.
	OMFOOAPI GetKitCodecDesc(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Creates an iterator to instantiate the LOCRs in the OMFI:MDES:Locator array.
//	The members will normally expose IOmfObject and IOmfLocator.
	OMFOOAPI IterateLocators(__out IOmfooIterator **ppIterator)= 0;
};

//*********************************************************************************************************************
//	IOmfLocator
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing OMF Locator Class objects (LOCR) and subclasses that inherit it.
//	This includes ...
//
//		Domain Locator Class (DOML)
//		DOS Locator Class (DOSL)
//		Mac Locator Class (MACL)
//		Mac Directory Locator Class (MACD)
//		Avid Media File Manager Locator Class (MFML)
//		Avid Media Stream Manager Locator Class (MSML)
//		Network Locator Class (NETL)
//		Sound Designer II Stream Locator Class (SDSL)
//		Text Locator Class (TXTL)
//		UNIX Locator Class (UNXL)
//		URL Locator Class (URLL)
//		Windows Locator Class (WINL)
//
//	All methods return wide (UTF-16) strings and have call-twice semantics.
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 160.
//*********************************************************************************************************************
struct __declspec(uuid("F941FFE3-0F75-4237-93BD-E875E0FB4F0A")) IOmfLocator;
interface IOmfLocator : public IUnknown
{
//	Retrieves the locator's main class-specific location property as a wide character (UTF-16) string.
//	If the locator is a DOS Locator (DOSL) this will retrieve the OMFI:DOSL:PathName property.
//	If the locator is a Network Locator (NETL) this will retrieve the OMFI:NETL:URLString property.
//	If the locator is a Text Locator (TXTL) this will retrieve the OMFI:TXTL:Name property.
//	If the locator is a UNIX Locator (UNXL) this will retrieve the OMFI:UNXL:PathName property.
//	If the locator is a Windows Locator (WINL) this will retrieve the OMFI:WINL:PathName property.
	OMFOOAPI GetRawLocation(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves tha filename if the locator class provides one.
//	Note that some locator classes are designed to locate directories, folders, volumes, hard drives, etc.,
//	and so they have no filename component. In that case this routine will return OMF_E_LOCR_NOT_A_FILE.
	OMFOOAPI GetFileName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	This method fixes-up the raw location string (if there is one) and returns an absolute path that can be passed
//	to the Windows file APIs. The path it returns is not guaranteed to exist. This method simply assembles
//	a healthy-looking string that complies with the standard Windows file and path naming conventions.
	OMFOOAPI GetFullPath(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the directory root. This can be the file system drive designator (for example "C:\") if the
//	underlying locator provides one, or the Universal Naming Convention (UNC) server and share name components.
	OMFOOAPI GetPathRoot(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the name of the file system volume if the locator class provides one.
	OMFOOAPI GetVolumeName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	Retrieves the machine name, host name, server name, NetBIOS name, etc. if the locator class provides one.
	OMFOOAPI GetServerName(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfFilmDescriptor
//	Inherits IOmfMediaDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Media Film Descriptor Class object (MDFM).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 169.
//*********************************************************************************************************************
struct __declspec(uuid("2AB3E848-A5D9-484d-B8CA-0644AD9E3EEB")) IOmfFilmDescriptor;
interface IOmfFilmDescriptor : public IOmfMediaDescriptor
{
//	OMFI:MDFM:FilmAspectRatio
	OMFOOAPI GetAspectRatio(__out POMF_RATIONAL pAspectRatio)= 0;
	OMFOOAPI GetAspectRatioAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetAspectRatioAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:MDFM:FilmFormat
	OMFOOAPI GetFilmFormat(__out PWORD pwFormat)= 0;

//	OMFI:MDFM:FrameRate
	OMFOOAPI GetFrameRate(__out PULONG pnFrameRate)= 0;

//	OMFI:MDFM:Manufacturer - as a wide character (UTF-16) string.
	OMFOOAPI GetManufacturer(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:MDFM:Model - as a wide character (UTF-16) string.
	OMFOOAPI GetModel(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:MDFM:PerforationsPerFrame
	OMFOOAPI GetPerforationsPerFrame(__out PBYTE pbPerforations)= 0;
};

//*********************************************************************************************************************
//	IOmfTapeDescriptor
//	Inherits IOmfMediaDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Media Tape Descriptor Class object (MDTP).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 173.
//*********************************************************************************************************************
struct __declspec(uuid("5A5B0B0C-34BB-4c1a-A5E1-D2C23231FFF8")) IOmfTapeDescriptor;
interface IOmfTapeDescriptor : public IOmfMediaDescriptor
{
//	OMFI:MDTP:FormFactor
	OMFOOAPI GetFormFactor(__out PWORD pwFormFactor)= 0;

//	OMFI:MDTP:Length
	OMFOOAPI GetLengthInMinutes(__out PULONG pnLength)= 0;

//	OMFI:MDTP:Manufacturer - as a wide character (UTF-16) string.
	OMFOOAPI GetManufacturer(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:MDTP:Model - as a wide character (UTF-16) string.
	OMFOOAPI GetModel(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:MDTP:TapeFormat
	OMFOOAPI GetTapeFormat(__out PWORD pwTapeFormat)= 0;

//	OMFI:MDTP:VideoSignal
	OMFOOAPI GetVideoSignal(__out PWORD pwVideoSignal)= 0;
};

//*********************************************************************************************************************
//	IOmfGmflDescriptor
//	Inherits IOmfMediaDescriptor
//	GMFL is one of Avid's private/undocumented object classes.
//	GMFLs are found in the OMFI:SMOB:MediaDescription property of some SMOBs (where an MDES normally lives).
//	GMFLs have just one property, which is OMFI:MDES:Locator.
//	GMFL might stand for "General Media File" or "Generic Media File" because the files that its locators 'locate'
//	tend to have non-standard filename extensions. For example *.pict, *.mp3, *.cda, *.mxf.
//*********************************************************************************************************************
struct __declspec(uuid("E579365E-46DA-4967-8FD0-55DA2D8059AF")) IOmfGmflDescriptor;
interface IOmfGmflDescriptor : public IOmfMediaDescriptor
{
	// The OMFI:MDES:Locator property is accessible from IOmfMediaDescriptor.
	// There are no additional properties or methods.
};

//*********************************************************************************************************************
//	IOmfFileDescriptor
//	Inherits IOmfMediaDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Media File Descriptor Class object (MDFL).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 167.
//*********************************************************************************************************************
struct __declspec(uuid("08C22D6A-DC42-4720-BA2E-D525B1D00D18")) IOmfFileDescriptor;
interface IOmfFileDescriptor : public IOmfMediaDescriptor
{
//	OMFI:MDFL:IsOMFI, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsOMFI(void)= 0;

//	OMFI:MDFL:SampleRate
	OMFOOAPI GetSampleRate(__out POMF_RATIONAL pSampleRate)= 0;
	OMFOOAPI GetSampleRateAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetSampleRateAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:MDFL:Length
//	This is the duration of the media in sample units.
	OMFOOAPI GetLength(__out PUINT64 pLength)= 0;

//	OMFI:MDFL:dataOffset
//	The physical file position where the data begins in the file.
//	This is typically the same value as the file position in the corresponding MDAT’s omfi:DataValue.
//	See OMFI:AIFC:AudioData, OMFI:IDAT:ImageData, OMFI:TIFF:ImageData, OMFI:WAVE:AudioData, etc.
//	See also OMFI:DIDD:DataOffset.
	OMFOOAPI GetDataOffset(__out PUINT64 pcbOffset)= 0;
};

//*********************************************************************************************************************
//	IOmfAifcDescriptor
//	Inherits IOmfFileDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF AIFC Audio Descriptor Class object (AIFD).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 113.
//*********************************************************************************************************************
struct __declspec(uuid("D569828F-949B-4271-87F3-E141AA662795")) IOmfAifcDescriptor;
interface IOmfAifcDescriptor : public IOmfFileDescriptor
{
//	OMFI:AIFD:Summary
//	Retrieves the payload as an array of raw bytes.
	OMFOOAPI GetSummaryAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfDiddDescriptor
//	Inherits IOmfFileDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Digital Image Descriptor Class object (DIDD).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 133.
//*********************************************************************************************************************
struct __declspec(uuid("B4E26EAA-D537-48c4-97EA-6F6F3A1328B1")) IOmfDiddDescriptor;
interface IOmfDiddDescriptor : public IOmfFileDescriptor
{
//	OMFI:DIDD:DIDCompressMethod
	OMFOOAPI GetDIDCompressMethod(__out PDWORD pdwCompressionFourCC)= 0;

//	OMFI:DIDD:Compression - as a wide character (UTF-16) string.
	OMFOOAPI GetCompression(__in ULONG cchBuffer, __out_opt PWCHAR pBuffer, __out_opt PULONG pcchRequired)= 0;

//	OMFI:DIDD:DIDResolutionID
	OMFOOAPI GetDIDResolutionID(__out PDWORD pdwResolutionID)= 0;

//	OMFI:DIDD:Uniformness, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsUniform(void)= 0;

//	OMFI:DIDD:StoredHeight
	OMFOOAPI GetStoredHeight(__out PULONG pnStoredHeight)= 0;

//	OMFI:DIDD:StoredWidth
	OMFOOAPI GetStoredWidth(__out PULONG pnStoredWidth)= 0;

//	OMFI:DIDD:SampledHeight
	OMFOOAPI GetSampledHeight(__out PULONG pnSampledHeight)= 0;

//	OMFI:DIDD:SampledWidth
	OMFOOAPI GetSampledWidth(__out PULONG pnSampledWidth)= 0;

//	OMFI:DIDD:SampledXOffset
	OMFOOAPI GetSampledXOffset(__out PLONG pnXOffset)= 0;

//	OMFI:DIDD:SampledYOffset
	OMFOOAPI GetSampledYOffset(__out PLONG pnYOffset)= 0;

//	OMFI:DIDD:DisplayHeight
	OMFOOAPI GetDisplayHeight(__out PULONG pnHeight)= 0;

//	OMFI:DIDD:DisplayWidth
	OMFOOAPI GetDisplayWidth(__out PLONG pnWidth)= 0;

//	OMFI:DIDD:DisplayXOffset
	OMFOOAPI GetDisplayXOffset(__out PLONG pnXOffset)= 0;

//	OMFI:DIDD:DisplayYOffset
	OMFOOAPI GetDisplayYOffset(__out PLONG pnYOffset)= 0;

//	OMFI:DIDD:FrameLayout
	OMFOOAPI GetFrameLayout(__out PWORD pwLayoutType)= 0;

//	OMFI:DIDD:VideoLineMap
	OMFOOAPI GetVideoLineMap(__in ULONG nMaxElements,
									__out_opt PUINT32 pArrayBase,
										__out PULONG pnActualElements)= 0;

//	OMFI:DIDD:ImageAspectRatio
	OMFOOAPI GetImageAspectRatio(__out POMF_RATIONAL pAspectRatio)= 0;
	OMFOOAPI GetImageAspectRatioAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetImageAspectRatioAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:DIDD:AlphaTransparency
	OMFOOAPI GetAlphaTransparency(__out PULONG pnAlphaValue)= 0;

//	OMFI:DIDD:ImageAlignmentFactor
	OMFOOAPI GetAlignmentFactor(__out PULONG pnAlignment)= 0;

//	OMFI:DIDD:Gamma
	OMFOOAPI GetGamma(__out POMF_RATIONAL pGamma)= 0;
	OMFOOAPI GetGammaAsFloat(__out PFLOAT pFloat)= 0;
	OMFOOAPI GetGammaAsDouble(__out DOUBLE* pDouble)= 0;

//	OMFI:DIDD:ClientFillStart
	OMFOOAPI GetClientFillStart(__out PULONG pnFillStart)= 0;

//	OMFI:DIDD:ClientFillEnd
	OMFOOAPI GetClientFillEnd(__out PULONG pnFillEnd)= 0;

//	OMFI:DIDD:DataOffset
//	If this property is absent then this will attempt to retrieve OMFI:MDFL:dataOffset.
	OMFOOAPI GetDataOffset(__out PUINT64 pcbOffset)= 0;

//	OMFI:DIDD:OffsetToRLEFrameIndexes
	OMFOOAPI GetOffsetToRLEFrameIndexes(__out PULONG pcbFrameIndexOffset)= 0;

//	OMFI:DIDD:FrameIndexByteOrder
//	Returns S_OK if the byte order is big-endian, or S_FALSE if it is little-endian.
	OMFOOAPI IsFrameIndexBigEndian(void)= 0;

//	OMFI:DIDD:FirstFrameOffset
//	If this property is absent then this will attempt to retrieve OMFI:DIDD:DIDDFirstFrameOffset.
	OMFOOAPI GetFirstFrameOffset(__out PULONG pcbFirstFrameOffset)= 0;

//	OMFI:DIDD:FrameSampleSize
//	If this property is absent then this will attempt to retrieve OMFI:DIDD:DIDDFrameSampleSize.
	OMFOOAPI GetFrameSampleSize(__out PULONG pcbSampleSize)= 0;

//	OMFI:DIDD:DIDImageSize
//	If this property is absent then this will attempt to retrieve OMFI:DIDD:DIDDImageSize.
	OMFOOAPI GetDIDDImageSize(__out PULONG pcbImageSize)= 0;

//	OMFI:DIDD:NextDIDDesc
//	If this property is absent then this will attempt to retrieve OMFI:DIDD:NextDIDDescriptor.
	OMFOOAPI NextDIDDescriptor(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfSd2fDescriptor
//	Inherits IOmfFileDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing a Sound Designer II Audio Descriptor Class object (SD2D).
//	SD2D extends (inherits, is a subclass of) MDFL. It is not one of the standard object classes defined
//	in the OMF specification.
//*********************************************************************************************************************
struct __declspec(uuid("C6EC2C46-EC2F-4c68-A23A-A0476F355316")) IOmfSd2fDescriptor;
interface IOmfSd2fDescriptor : public IOmfFileDescriptor
{
//	OMFI:SD2D:BitsPerSample
	OMFOOAPI GetBitsPerSample(__out PWORD pwBitsPerSample)= 0;

//	OMFI:SD2D:NumOfChannels
	OMFOOAPI GetNumChannels(__out PWORD pwChannels)= 0;
};

//*********************************************************************************************************************
//	IOmfTiffDescriptor
//	Inherits IOmfFileDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF TIFF Image Descriptor Class object (TIFD).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 204.
//*********************************************************************************************************************
struct __declspec(uuid("150DC076-2708-449c-8CA9-F666ECE8C56B")) IOmfTiffDescriptor;
interface IOmfTiffDescriptor : public IOmfFileDescriptor
{
//	OMFI:TIFD:IsContiguous, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsContiguous(void)= 0;

//	OMFI:TIFD:IsUniform, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsUniform(void)= 0;

//	OMFI:TIFD:LeadingLines
	OMFOOAPI GetLeadingLines(__out PULONG pnLeadingLines)= 0;

//	OMFI:TIFD:TrailingLines
	OMFOOAPI GetTrailingLinesLines(__out PULONG pnTrailingLines)= 0;

//	OMFI:TIFD:JPEGTableID
	OMFOOAPI GetJPEGTableID(__out PDWORD pdwTableID)= 0;

//	OMFI:TIFD:FP16QTables
//	Retrieves the payload as an array of raw bytes.
	OMFOOAPI GetFP16QTablesAsRawBytes(__in ULONG cbBuffer,
											__out_opt PVOID pBuffer,
												__out_opt PULONG pcbRequired)= 0;

//	OMFI:TIFD:Summary
//	Retrieves the payload as an array of raw bytes.
	OMFOOAPI GetSummaryAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfWaveDescriptor
//	Inherits IOmfFileDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF WAVE Audio Descriptor Class object (WAVD). 
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 217.
//*********************************************************************************************************************
struct __declspec(uuid("4FC97297-7FBA-48df-B2A7-1DD79D1734C2")) IOmfWaveDescriptor;
interface IOmfWaveDescriptor : public IOmfFileDescriptor
{
//	OMFI:WAVD:Summary
//	Retrieves the payload as an array of raw bytes.
	OMFOOAPI GetSummaryAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfCdciDescriptor
//	Inherits IOmfDiddDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Color Difference Component Image Descriptor Class object (CDCI).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 119.
//*********************************************************************************************************************
struct __declspec(uuid("9BE5AACE-9BD6-45d2-A286-D3C6EDB7D5A9")) IOmfCdciDescriptor;
interface IOmfCdciDescriptor : public IOmfDiddDescriptor
{
//	OMFI:CDCI:ComponentWidth
	OMFOOAPI GetComponentWidth(__out PULONG pnWidth)= 0;

//	OMFI:CDCI:HorizontalSubsampling
	OMFOOAPI GetHorizSubsampling(__out PULONG pnHorizSubsampling)= 0;

//	OMFI:CDCI:VerticalSubsampling
	OMFOOAPI GetVertSubsampling(__out PULONG pnVertSubsampling)= 0;

//	OMFI:CDCI:ColorSiting, omfi:ColorSitingType (16 bits) 
	OMFOOAPI GetColorSiting(__out PWORD pwColorSiting)= 0;

//	OMFI:CDCI:BlackReferenceLevel
	OMFOOAPI GetBlackReferenceLevel(__out PULONG pnBlackLevel)= 0;

//	OMFI:CDCI:WhiteReferenceLevel
	OMFOOAPI GetWhiteReferenceLevel(__out PULONG pnWhiteLevel)= 0;

//	OMFI:CDCI:ColorRange
	OMFOOAPI GetColorRange(__out PULONG pnColorRange)= 0;

//	OMFI:CDCI:PaddingBits
	OMFOOAPI GetPaddingBits(__out PWORD pwPaddingBits)= 0;

//	OMFI:CDCI:OffsetToFrameIndexes
	OMFOOAPI GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)= 0;
};

//*********************************************************************************************************************
//	IOmfQtmdDescriptor
//	Inherits IOmfDiddDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing a Quicktime Image Descriptor Class object (QTMD).
//	QTMD extends (inherits, is a subclass of) DIDD. It is not one of the standard object classes defined
//	in the OMF specification.
//*********************************************************************************************************************
struct __declspec(uuid("7D042F26-3258-456f-A7CB-58B9B2497377")) IOmfQtmdDescriptor;
interface IOmfQtmdDescriptor : public IOmfDiddDescriptor
{
//	OMFI:QTMD:fType
	OMFOOAPI GetfTypeAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfRgbaDescriptor
//	Inherits IOmfDiddDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF RGBA Component Image Descriptor Class object (RGBA).
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 186.
//*********************************************************************************************************************
struct __declspec(uuid("7A940AD1-8B32-4653-A2AA-0A1D696A6B65")) IOmfRgbaDescriptor;
interface IOmfRgbaDescriptor : public IOmfDiddDescriptor
{
//	OMFI:RGBA:Palette
	OMFOOAPI GetPaletteAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)= 0;
//	OMFI:RGBA:PaletteLayout
	OMFOOAPI GetPaletteLayout(__out POMF_COMP_CODE_ARRAY pResult)= 0;

//	OMFI:RGBA:PaletteStructure
	OMFOOAPI GetPaletteStructure(__out POMF_COMP_SIZE_ARRAY pResult)= 0;

//	OMFI:RGBA:PixelLayout
//	On exit the OMF_COMP_CODE_ARRAY is guaranteed to be null-terminated.
	OMFOOAPI GetPixelLayout(__out POMF_COMP_CODE_ARRAY pResult)= 0;

//	OMFI:RGBA:PixelStructure
//	On exit the OMF_COMP_SIZE_ARRAY is guaranteed to be null-terminated.
	OMFOOAPI GetPixelStructure(__out POMF_COMP_SIZE_ARRAY pResult)= 0;
};

//*********************************************************************************************************************
//	IOmfRleiDescriptor
//	Inherits IOmfDiddDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing a Run-Length Encoded Image Descriptor Class object (RLED).
//	RLED extends (inherits, is a subclass of) DIDD. It is not one of the standard object classes defined
//	in the OMF specification.
//*********************************************************************************************************************
struct __declspec(uuid("6D88EE0A-925B-4f57-9DCE-9B4D1CF3B36D")) IOmfRleiDescriptor;
interface IOmfRleiDescriptor : public IOmfDiddDescriptor
{
//	Retrieves the OMFI:RLED:FrameIndex property as an array of UINT32s.
//	The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	OMFOOAPI ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)= 0;

//	Retrieves the OMFI:RLED:FrameIndex property as an array of UINT64s.
//	The data type can be omfi:Position32Array or omfi:Position64Array.
//	If the data type is omfi:Position32Array then the 32-bit values are zero-padded to 64 bits.
	OMFOOAPI ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)= 0;

//	OMFI:RLED:OffsetToFrameIndexes
	OMFOOAPI GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)= 0;
};

//*********************************************************************************************************************
//	IOmfJpegDescriptor
//	Inherits IOmfCdciDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an JPED Image Descriptor Class object (JPED).
//	JPED extends (inherits, is a subclass of) CDCI. It is not one of the standard object classes defined
//	in the OMF specification.
//*********************************************************************************************************************
struct __declspec(uuid("9F95EE29-124E-466f-B565-3CA868351109")) IOmfJpegDescriptor;
interface IOmfJpegDescriptor : public IOmfCdciDescriptor
{
//	OMFI:JPED:JPEDImageStartAlignment
	OMFOOAPI GetJPEDImageStartAlignment(__out PULONG pnAlignment)= 0;

//	OMFI:JPED:JPEGTag
	OMFOOAPI GetJPEGTag(__out PDWORD pdwJpegTag)= 0;

//	OMFI:JPED:OffsetToFrameIndexes
	OMFOOAPI GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)= 0;

//	OMF2 - OMFI:JPED:QntznTblLen
	OMFOOAPI GetQntznTblLen(__out PULONG pcbTableLength)= 0;

//	OMF1 - OMFI:JPED:QuantizationTables
//	OMF2 - OMFI:JPED:QntznTbl
	OMFOOAPI GetQntznTblAsRawBytes(__in ULONG cbBuffer,
										__out_opt PVOID pBuffer,
											__out_opt PULONG pcbRequired)= 0;
};

//*********************************************************************************************************************
//	IOmfMpegDescriptor
//	Inherits IOmfCdciDescriptor
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an MPEG Bitstream Image Descriptor Class object (MPGI).
//	MPGI extends (inherits, is a subclass of) CDCI. It is not one of the standard object classes defined
//	in the OMF specification.
//*********************************************************************************************************************
struct __declspec(uuid("1830A046-11F3-4784-8018-E5151445F394")) IOmfMpegDescriptor;
interface IOmfMpegDescriptor : public IOmfCdciDescriptor
{
//	OMFI:MPGI:GOPStructure
	OMFOOAPI GetGOPStructure(__out PBYTE pbResult)= 0;

//	OMFI:MPGI:isMPEG1, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsMPEG1(void)= 0;

//	OMFI:MPGI:LeadingDiscard, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsLeadingDiscard(void)= 0;

//	OMFI:MPGI:TrailingDiscard, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsTrailingDiscard(void)= 0;

//	OMFI:MPGI:MPEGStreamType
	OMFOOAPI GetMPEGStreamType(__out PBYTE pbType)= 0;

//	OMFI:MPGI:MPEGVersion
	OMFOOAPI GetMPEGVersion(__out PBYTE pbVersion)= 0;

//	OMFI:MPGI:OffsetToFrameIndexes
	OMFOOAPI GetOffsetToFrameIndexes(__out PUINT64 pcbOffset)= 0;

//	OMFI:MPGI:MinGOP
//	If this property is absent then this will attempt to retrieve OMFI:MPGI:omMPGIMinGOPLength.
	OMFOOAPI GetMinGOPLength(__out PWORD pwMinGOP)= 0;

//	OMFI:MPGI:MaxGOP
//	If this property is absent then this will attempt to retrieve OMFI:MPGI:omMPGIMaxGOPLength.
	OMFOOAPI GetMaxGOPLength(__out PWORD pwMaxGOP)= 0;

//	OMFI:MPGI:ProfileAndLevel
	OMFOOAPI GetProfileAndLevel(__out PBYTE pbProfAndLevel)= 0;

//	OMFI:MPGI:RandomAccess, omfi:Boolean
//	Returns S_OK if the boolean is TRUE, or S_FALSE if the boolean is FALSE.
	OMFOOAPI IsRandomAccess(void)= 0;

//	OMFI:MPGI:SequenceHdr
	OMFOOAPI GetSequenceHdrAsRawBytes(__in ULONG cbBuffer,
											__out_opt PVOID pBuffer,
												__out_opt PULONG pcbRequired)= 0;
//	OMFI:MPGI:StreamType
	OMFOOAPI GetStreamType(__out PBYTE pbStmType)= 0;
};

//*********************************************************************************************************************
//	IOmfooExtractCallback
//	This is similar to the DirectShow IAMCopyCaptureFileProgress interface.
//	It enables your application to get callback notifications during your calls to IOmfMediaData::ExtractDataToFile().
//	It allows you to cancel the operation in progress. This object is optional. Omfoo does not provide this.
//	If you want to use this feature you must implement this interface yourself - and pass it to ExtractDataToFile().
//	See IOmfMediaData::ExtractDataToFile() below for more info.
//*********************************************************************************************************************
struct __declspec(uuid("2619D227-6F9C-4471-89F4-51A8FC16432D")) IOmfooExtractCallback;
interface IOmfooExtractCallback : public IUnknown
{
//	Called periodically so your application can update its GUI (or whatever).
//	The callback interval is NOT measured in time, it is MEASURED IN 4096-BYTE UNITS.
//	IOmfMediaData calls this from your calling thread after issuing overlapped asynchronous read/write requests,
//	but before they are completed. Then it calls it once more after all read/write operations are done.
//	Your implmentation should return S_OK to continue extracting the embedded media file, or return E_ABORT to
//	cancel all pending read/write operations and to delete the incomplete file.
	OMFOOAPI UpdateProgress(__in UINT64 cbCompleted, __in UINT64 cbTotal)= 0;
};

//*********************************************************************************************************************
//	IOmfMediaData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Media Data Class object (MDAT).
//	It retrieves various MDAT properties, and provides access to the payload of all subclasses that inherit MDAT.
//	For example you can reach OMFI:AIFC:AudioData, OMFI:IDAT:ImageData, OMFI:TIFF:ImageData, OMFI:WAVE:AudioData, etc.
//	See the OMF Interchange Specification Version 2.1. Appendix A, page 164.
//*********************************************************************************************************************
struct __declspec(uuid("096173ED-22FF-4a8d-BA74-14C6174BADE2")) IOmfMediaData;
interface IOmfMediaData : public IUnknown
{
//	Returns S_OK only when the subclass's payload contains just audio (no video) or S_FALSE otherwise.
	OMFOOAPI IsAudio(void)= 0;

//	Returns S_OK if the subclass's payload contains video (and perhaps audio too) or S_FALSE if it does not.
	OMFOOAPI IsVideo(void)= 0;

//	OMF1 - Retrieves the OMFI:xxxx:MobID property (where xxxx is the class fourCC of the MDAT's subclass).
//	OMF2 - Retrieves the OMFI:MDAT:MobID property.
	OMFOOAPI GetMobID(__out POMF_MOB_ID pMobID)= 0;

//	Instantiates the Source Mob (SMOB) associated with this Media Data, and then queries it for the interface
//	specified by riid. Returns E_NOINTERFACE if the result does not expose the requested interface.
	OMFOOAPI GetSourceMob(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Instantiates the Media Descriptor (MDES) that describes this data, and then queries it for the interface
//	specified by riid. Returns E_NOINTERFACE if the result does not expose the requested interface.
	OMFOOAPI GetMediaDescriptor(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Retrieves the internal file position and length of the MDAT's payload and (optionally) duplicates the OMF
//	file's opened file handle. This method allows you to make your own low-level direct calls to ReadFile().
//	It's made possible because IOmfooReader::Load() opens the OMF file with read-only/share access rights.
//	The handle this method returns also has read-only/share access rights.
//	You MUST call CloseHandle() when you are done using the handle.
	OMFOOAPI GetRawFileParams(__out PUINT64 pOffset, __out PUINT64 pLength, __out_opt PHANDLE pHandle)= 0;

//	Extracts the embedded data to a file.
//	If a file with the specifed name already exists and fOverwrite is TRUE then that file will be overwritten.
//	The pCallback and nPagesPerCallback arguments are optional. If pCallback is non-NULL then its UpdateProgress()
//	method will be called periodically in nPagesPerCallback intervals (where each memory page is 4096 bytes).
//	To use this feature the nPagesPerCallback argument must be in the range of 1~7200 inclusive, or this will
//	return E_INVALIDARG. If pCallback is NULL then nPagesPerCallback is ignored.
//	See IOmfooExtractCallback (above) for more details.
//
//	Works with OMFI:AIFC:AudioData, OMFI:IDAT:ImageData, OMFI:TIFF:ImageData, OMFI:WAVE:AudioData, etc.
	OMFOOAPI ExtractDataToFile(__in PCWSTR pwzFullPathNewFile,
									__in_opt IOmfooExtractCallback *pCallback,
										__in_opt ULONG nPagesPerCallback,
											__in BOOL fOverwrite)= 0;

//	Creates a filename that can be used use with ExtractDataToFile(). (See above ^^^.)
//	The content/format/style/syntax of the string is determined as follows:
//	SYNTAX	DESCRIPTION
//	  0		Return a filename extension only, including the leading dot.
//			If Omfoo cannot provide a meaningful filename extension the default will return ".raw"
//
//	  1		Create a filename based on the MDAT's category ('audio', 'video', or 'media') with an instance number.
//			For example "Audio File #12.wav", "Video File #04.dv", or "Media File #01.raw".
//
//	  2		Return the MDAT's OMFI:MDAT:MobID property in hex, followed by the filename extension.
//			For example "75B0C5A4_B0F5A485_F39EDE2C.wav".
//
//	  3		Same as #2 above, but includes a prefix derived from the name of the loaded OMF file.
//			For example if the OMF file is named "Wizard of Oz.omf" then the result may look something
//			like "Wizard_75B0C5A4_B0F5A485_F39EDE2C.wav".
//
//	  4		Same as #3 above, but include a prefix derived from the name of the top level composition mob.
//			For example if the top level composition mob is named "TripToTheMoon" then the name may look
//			something like "Trip_75B0C5A4_B0F5A485_F39EDE2C.wav"
//
//	  5		Create a filename using the name of the Source Mob (SMOB) associated with the MDAT.
//			This will only succeed if all Source Mobs have unique names; and only if every name consists
//			entirely of valid Win32 filename characters. Otherwise this will return OMF_E_CANT_COMPLETE.
	OMFOOAPI CreateFileNameForExport(__in DWORD dwSyntax,
											__in ULONG cchBuffer,
												__out_opt PWCHAR pBuffer,
													__out_opt PULONG pcchRequired)= 0;

//	Creates a Windows Stream object on the MDAT's embedded payload and queries it for the interface specified by riid.
//	This implementation exposes IStream and ISequentialStream.
	OMFOOAPI CreateStreamOnData(__in REFIID riid, __out PVOID *ppvOut)= 0;

//	Creates a DirectShow source filter on the subclass's payload that can be used with the DirectShow API.
//	The filter exposes IBaseFilter, and will typically have at least one output pin that exposes IAsyncReader.
	OMFOOAPI CreateDShowSourceFilterOnData(__in REFIID riid, __out PVOID *ppvOut)= 0;
};

//*********************************************************************************************************************
//	IOmfAifcData
//	Inherits IOmfMediaData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF AIFC Audio Data Class object (AIFC).
//	See OMF Interchange Specification 2.1. Appendix A at page 112.
//*********************************************************************************************************************
struct __declspec(uuid("39CD6629-31F0-4634-98FB-FAD4F22FCA9F")) IOmfAifcData;
interface IOmfAifcData : public IOmfMediaData
{
//	Note that the OMFI:AIFC:AudioData property is accessible from IOmfMediaData (see above).
//	There are no additional methods.
};

//*********************************************************************************************************************
//	IOmfIdatData
//	Inherits IOmfMediaData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF Image Data Class object (IDAT).
//	See OMF Interchange Specification 2.1. Appendix A at page 158.
//*********************************************************************************************************************
struct __declspec(uuid("890655C2-4168-4baa-A48B-21FB55377E60")) IOmfIdatData;
interface IOmfIdatData : public IOmfMediaData
{
//	Note that the OMFI:IDAT:ImageData property is accessible from IOmfMediaData (see above).
//	There are no additional methods.
};

//*********************************************************************************************************************
//	IOmfTiffData
//	Inherits IOmfMediaData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF TIFF Image Data Class object (TIFF).
//	See OMF Interchange Specification 2.1. Appendix A at page 203.
//*********************************************************************************************************************
struct __declspec(uuid("727A4AA5-CF6E-4124-A365-54C4D25244E1")) IOmfTiffData;
interface IOmfTiffData : public IOmfMediaData
{
//	Note that the OMFI:TIFF:ImageData property is accessible from IOmfMediaData (see above).
//	There are no additional methods.
};

//*********************************************************************************************************************
//	IOmfWaveData
//	Inherits IOmfMediaData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF WAVE Audio Data Class object (WAVE).
//	See OMF Interchange Specification 2.1. Appendix A at page 216.
//*********************************************************************************************************************
struct __declspec(uuid("93D96C57-69DF-4aea-AB32-B65AB6865371")) IOmfWaveData;
interface IOmfWaveData : public IOmfMediaData
{
//	Note that the OMFI:WAVE:AudioData property is accessible from IOmfMediaData (see above).
//	There are no additional methods.
};

//*********************************************************************************************************************
//	IOmfSd2fData
//	Inherits IOmfMediaData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing a Sound Designer II Media Data object (SD2M).
//	SD2M extends (inherits, is a subclass of) MDAT. It is not one of the standard object classes defined
//	in the OMF specification.
//*********************************************************************************************************************
struct __declspec(uuid("D10187C7-870E-4e55-B116-373B13847648")) IOmfSd2fData;
interface IOmfSd2fData : public IOmfMediaData
{
//	Note that the OMFI:SD2M:Data (aka OMFI:SD2M:AudioData) property is accessible from IOmfMediaData (see above).
//	There are no additional methods.
};

//*********************************************************************************************************************
//	IOmfJpegData
//	Inherits IOmfIdatData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an OMF JPEG Image Data Class object (JPEG).
//	See OMF Interchange Specification 2.1. Appendix A at page 159.
//	Note that its OMFI:IDAT:ImageData property is accessible from IOmfMediaData (see above).
//*********************************************************************************************************************
struct __declspec(uuid("CD9AADB5-49A8-4bdd-BB57-8C9FD398E06F")) IOmfJpegData;
interface IOmfJpegData : public IOmfIdatData
{
//	Retrieves the OMFI:JPEG:FrameIndex property as an array of UINT32s.
//	The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	OMFOOAPI ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)= 0;

//	Retrieves the OMFI:JPEG:FrameIndexExt or OMFI:JPEG:FrameIndex property as an array of UINT64s.
//	The data type can be omfi:Position32Array or omfi:Position64Array.
//	If the data type is omfi:Position32Array then the 32-bit values are zero-padded to 64 bits.
	OMFOOAPI ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)= 0;
};

//*********************************************************************************************************************
//	IOmfMpegData
//	Inherits IOmfIdatData
//	Available in OMF2 only.
//	This is a specialized interface for accessing an MPEG Image Data Class object (MPEG).
//	MPEG extends (inherits, is a subclass of) IDAT. It is not one of the standard object classes defined
//	in the OMF specification. Note that its OMFI:IDAT:ImageData property is accessible from IOmfMediaData (see above).
//*********************************************************************************************************************
struct __declspec(uuid("2D83CF38-ECEB-4d20-B581-438D5ED933A2")) IOmfMpegData;
interface IOmfMpegData : public IOmfIdatData
{
//	Retrieves the OMFI:MPEG:FrameIndex property as an array of UINT32s.
//	The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
	OMFOOAPI ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)= 0;

//	Retrieves the OMFI:MPEG:MPEGFrameIndex property as an array of UINT64s.
//	The data type can be omfi:Position32Array or omfi:Position64Array.
//	If the data type is omfi:Position32Array then the 32-bit values are zero-padded to 64 bits.
	OMFOOAPI ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)= 0;
};

//*********************************************************************************************************************
//	IOmfRleiData
//	Inherits IOmfIdatData
//	Available in OMF1 and OMF2.
//	This is a specialized interface for accessing an RLE Image Data Class object ('RLE ').
//	'RLE ' extends (inherits, is a subclass of) IDAT. It is not one of the standard object classes defined
//	in the OMF specification. Note that its OMFI:IDAT:ImageData property is accessible from IOmfMediaData (see above).
//*********************************************************************************************************************
struct __declspec(uuid("BD04B2C3-2094-4859-8AFF-0C159A743E76")) IOmfRleiData;
interface IOmfRleiData : public IOmfIdatData
{
//	Retrieves the OMFI:RLED:FrameIndex property as an array of UINT32s.
//	Note that in some files this property lives in the corresponding 'RLED' object, which is a type of DIDD.
//	The data type must be omfi:Position32Array or this will return OMF_E_TYPE_SURPRISE.
//	Note that the "RLED" in name "OMFI:RLED:FrameIndex" is not a typo - it's just the way they named the property.
//	It's the class fourCC of the corresponding media descriptor.
//	This violates the OMF naming convention. It is not a typo - it's just the way they named the property.
	OMFOOAPI ReadFrameIndexArray32(__in ULONG nMaxElements,
										__out_opt PUINT32 pArrayBase,
											__out PULONG pnActualElements)= 0;

//	Retrieves the OMFI:RLED:FrameIndex property as an array of UINT64s.
//	The data type can be omfi:Position32Array or omfi:Position64Array.
//	If the data type is omfi:Position32Array then the 32-bit values are zero-padded to 64 bits.
	OMFOOAPI ReadFrameIndexArray64(__in ULONG nMaxElements,
										__out_opt PUINT64 pArrayBase,
											__out PULONG pnActualElements)= 0;
};

#endif	//  __OMFOO_INTERFACES_H__



