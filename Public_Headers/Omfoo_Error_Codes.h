// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII.
// Original filename: Omfoo_Error_Codes.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo API. You should have received a
// copy of the Omfoo end-user license agreement (EULA) along with it.
// Please see OMFOO_EULA.TXT or send inquiries to OmfooGuy@gmail.com.
// ---------------------------------------------------------------------------
#ifndef __OMFOO_ERROR_CODES_H__
#define __OMFOO_ERROR_CODES_H__

#include <winerror.h>

// Macro to determine if an HRESULT is an Omfoo-specific error code.
#define IS_OMFOO_ERROR(hr)  ((ULONG(hr)>=0x80045B00)&&(ULONG(hr)<=0x80045BFF))

enum {
//	All methods in the Omfoo API return HRESULTs; which are a type of LONG, which is a signed 32-bit integer.
//	HRESULTs are documented in <winerror.h> and on the internet. They even have their own wikipedia page.
//	The codes below are just aliases for the standard/generic COM return values.
//	When Omfoo encounters an error it will try to use these codes first if they adequately describe the problem.
	OMF_S_FALSE			= S_FALSE,			// 0x00000001
	OMF_S_OK			= S_OK,				// Operation successful 0x00000000 
	OMF_E_ABORT			= E_ABORT,			// Operation aborted 0x80004004 
	OMF_E_ACCESSDENIED	= E_ACCESSDENIED,	// General access denied error 0x80070005 
	OMF_E_FAIL			= E_FAIL,			// Unspecified failure 0x80004005 
	OMF_E_HANDLE		= E_HANDLE,			// Handle that is not valid 0x80070006 
	OMF_E_INVALIDARG	= E_INVALIDARG,		// One or more arguments are not valid 0x80070057 
	OMF_E_NOINTERFACE	= E_NOINTERFACE,	// No such interface supported 0x80004002 
	OMF_E_NOTIMPL		= E_NOTIMPL,		// Not implemented 0x80004001 
	OMF_E_OUTOFMEMORY	= E_OUTOFMEMORY,	// Failed to allocate necessary memory 0x8007000E 
	OMF_E_POINTER		= E_POINTER,		// Pointer that is not valid 0x80004003 
	OMF_E_UNEXPECTED	= E_UNEXPECTED,		// Unexpected failure 0x8000FFFF

//	Whenever Omfoo makes an internal call to any of the Win32 APIs and that call fails, Omfoo will call GetLastError()
//	to retrieve the Win32 error code - and then it will return it using the HRESULT_FROM_WIN32() macro.
//	Here are some examples of error codes you may encounter.
//
//	HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION)
//	HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
//	HRESULT_FROM_WIN32(ERROR_OPEN_FAILED)
//	HRESULT_FROM_WIN32(ERROR_READ_FAULT)

//	The Omfoo API also has its own set of quasi-unique error codes.
//	They conform to Microsoft's HRESULT coding scheme described in <winerror.h>.
//	They use FACILITY_ITF and occupy 256 codes in the range of 0x5B00~0x5BFF.
//	As with all 3rd party HRESULTs there is always the chance that their literal
//	values might collide or conflict with other error codes in other 3rd party APIs.
	OMFOO_E_ASSERTION_FAILURE	= 0x80045B00,	// An internal self-test has unexpectedly failed.

	// Bento fatal/irrecoverable errors returned by IOmfooReader::Load().
	BENTO_E_FILE_TOO_SMALL		= 0x80045B10,	// The file is too small (less that 256 bytes) to be a valid Bento file.
	BENTO_E_BAD_SIGNATURE		= 0x80045B11,	// Incorrect magic byte sequence in the Bento label.
	BENTO_E_BAD_MAJOR_VERSION	= 0x80045B12,	// Unknown Bento major version.
	BENTO_E_BAD_MINOR_VERSION	= 0x80045B13,	// Unknown Bento minor version.
	BENTO_E_TOC_TOO_SMALL		= 0x80045B14,	// The Bento Table-of-Contents (TOC) is too small to be a valid TOC.
	BENTO_E_TOC_TOO_HUGE		= 0x80045B15,	// The Bento Table-of-Contents (TOC) is too big to read into memory.
	BENTO_E_TOC_OVERLAP			= 0x80045B16,	// The Bento Table-of-Contents (TOC) purports to overlap onto the label.
	BENTO_E_TOC_104_EXTRA_BYTES	= 0x80045B17,	// The size of the TOC isn't evenly divisible by 24. (Bento v1.04 only.)
	BENTO_E_TOO_FEW_ITEMS		= 0x80045B18,	// The Bento Table-of-Contents (TOC) has too few items to be understood.
	BENTO_E_TOO_MANY_ITEMS		= 0x80045B19,	// The Bento Table-of-Contents (TOC) has more than 67108736 items.
	BENTO_E_BAD_BYTE_CODE		= 0x80045B1A,	// Encountered an invalid compression token while expanding the TOC.
	BENTO_E_RUNAWAY_TOC			= 0x80045B1B,	// The compressed TOC exceeds the size reported in the label.
	BENTO_E_NO_SELF_DESC		= 0x80045B1C,	// The first object in the TOC is not CM_StdObjID_TOC.
	BENTO_E_MISSING_PROPS		= 0x80045B1D,	// One or more standard (required) properties are missing.
	BENTO_E_SELF_TEST_1			= 0x80045B1E,	// The CM_StdObjID_TOC_Object property disagrees with the TOC's size.
	BENTO_E_SELF_TEST_2			= 0x80045B1F,	// The CM_StdObjID_TOC_Container property disagrees with the file's size.

	// OMF fatal/irrecoverable errors returned by IOmfooReader::Load().
	OMF_E_BAD_MAJOR_VERSION		= 0x80045B40,	// Cannot determine the file's OMF major version.
	OMF_E_BAD_FILE_BYTE_ORDER	= 0x80045B41,	// Cannot determine the file's OMF byte order.
	OMF_E_REQUIRED_OOBJ_MISSING	= 0x80045B42,	// A required OMFI class object is not defined in the file.
	OMF_E_REQUIRED_PROP_MISSING	= 0x80045B43,	// A required OMFI property is not defined in the file.
	OMF_E_REQUIRED_TYPE_MISSING	= 0x80045B44,	// A required OMFI data type is not defined in the file.
	OMF_E_TOO_MANY_OOBJS		= 0x80044B45,	// The file contains more than one million objects.
	OMF_E_TOO_MANY_PROPS		= 0x80044B46,	// An object has more than 32767 properties.
	OMF_E_MOBJ_IDENTITY_CRISIS	= 0x80044B47,	// Two or more MOBJs have the same globally unique ID.
	OMF_E_MDAT_IDENTITY_CRISIS	= 0x80044B48,	// Two or more MDATs have the same globally unique ID.
	OMF_E_MDAT_SURPRISE			= 0x80044B49,	// An error occurred while building the MDAT table.

	// Non-fatal errors.
	OMF_E_NO_MORE_ITEMS			= 0x80045B50,	// Non-error! The iterator has simply traversed all items in the list.
	OMF_E_NOT_IN_OMF1			= 0x80045B51,	// This method requires an OMF2 file, but this is an OMF1 file.
	OMF_E_NOT_IN_OMF2			= 0x80045B52,	// This method requires an OMF1 file, but this is an OMF2 file.
	OMF_E_CANT_COMPLETE			= 0x80045B53,	// This method cannot complete for an unspecified reason.
	OMF_E_INSUFFICIENT_BUFFER	= 0x80045B54,	// The data area passed to an Omfoo call is too small.
	OMF_E_DIVIDE_BY_ZERO		= 0x80045B55,	// An internal math operation attempted to divide by zero.

	OMF_E_BAD_ARRAY				= 0x80045B69,	// An array is malformed.
	OMF_E_BAD_FOURCC			= 0x80045B6A,	// The "fourCC" is not composed of four printable ASCII characters.
	OMF_E_BAD_TIMESTAMP			= 0x80045B6B,	// The timestamp is invalid.
	OMF_E_BAD_TEXT				= 0x80045B6C,	// The payload is not text.
	OMF_E_BAD_PROP_BYTE_ORDER	= 0x80045B6D,	// Cannot determine a property's byte order.

	OMF_E_CLSD_NOT_DEFINED		= 0x80045B70,	// The specified object class is not defined in this file.
	OMF_E_PROP_NOT_DEFINED		= 0x80045B71,	// The specified property is not defined in this file.
	OMF_E_TYPE_NOT_DEFINED		= 0x80045B72,	// The specified data type is not defined in this file.
	OMF_E_OOBJ_NOT_FOUND		= 0x80045B73,	// The specified object was not found.
	OMF_E_PROP_NOT_FOUND		= 0x80045B74,	// The specified object doesn't have the specified property.
	OMF_E_TYPE_NOT_FOUND		= 0x80045B75,	// The specified object doesn't have the specified property with the specific type.
	OMF_E_TYPE_SURPRISE			= 0x80045B76,	// A property has an unexpected data type.
	OMF_E_STORAGE_SURPRISE		= 0x80045B77,	// Can't determine how the property's payload is stored in the file.
	OMF_E_SIZE_SURPRISE			= 0x80045B78,	// A property payload has an unexpected size.
	OMF_E_DATA_NONCONTIGUOUS	= 0x80045B79,	// Cannot complete because the payload is fragmented.

//	OMF_E_STRING_TRUNCATED		= 0xHHHHHHHH,	// A string cannot be read in its entirety because the buffer is too small.

	OMF_E_ATTB_NOT_FOUND		= 0x80045B80,	// The named attribute was not found in the OMFI:ATTR:AttrRefs array.
	OMF_E_ATTB_SURPRISE			= 0x80045B81,	// The attribute's OMFI:ATTB:Kind does not match the call.
	OMF_E_DDEF_SURPRISE			= 0x80045B82,	// A Data Definition object has an unknown OMFI:DDEF:DataKindID string.
	OMF_E_EDEF_SURPRISE			= 0x80045B83,	// An Effect Definition object has an unknown OMFI:EDEF:EffectID string.

	OMF_E_LOCR_UNKNOWN_SUBCLASS	= 0x80045B90,	// The locator has an unknown class fourCC.
	OMF_E_LOCR_NO_STRING		= 0x80045B91,	// The locator class does not (or can not) provide the requested string.
	OMF_E_LOCR_NOT_A_FILE		= 0x80045B92,	// The locator class is not a file locator (and so there is no filename).

//	OMF_E_JPEG_PROBLEM			= 0x80045B50L
//	OMF_E_TIFF_PROBLEM			= 0x80045B60L

	OMF_E_OOBJ_NO_FOURCC		= 0x80045BA0,	// An object does not have a OMFI:ObjID or OMFI:OOBJ:ObjClass property.
	OMF_E_CLASS_SURPRISE		= 0x80045BA1,	// An object belongs to an unexpected class.
	OMF_E_NO_INHERITANCE		= 0x80045BA2,	// The object does not inherit the specified class.
	OMF_E_MOB_NOT_FOUND			= 0x80045BA3,	// The mob was not found.

	OMF_E_MEDIA_IS_EMBEDDED		= 0x80045BB4,	// Can't complete because the media data is embedded (not external).
	OMF_E_MEDIA_IS_EXTERNAL		= 0x80045BB5,	// Can't complete because the media data is external (not embedded).
};

#endif	// __OMFOO_ERROR_CODES_H__