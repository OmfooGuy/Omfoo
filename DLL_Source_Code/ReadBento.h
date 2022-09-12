// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadBento.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#pragma once
#include "ReadableFile.h"

//	Number of standard Bento property IDs.
//	See Apple's Bento Specification Revision 1.0d5, "Appendix D: Standard Object IDs and Global Names" at page 99.
#define BENTO_NUM_HARDCODED_PROPERTIES	(15)

//	Number of standard Bento data type IDs.
//	See Apple's Bento Specification Revision 1.0d5, "Appendix D: Standard Object IDs and Global Names" at page 99.
#define BENTO_NUM_HARDCODED_DATA_TYPES	(9)

//	Maximum number of 7-bit ASCII characters in a Bento Globally Unique Name.
//	The Bento spec says names must comply with ISO/IEC 9070, but there is no mention of any name-length limitation.
//	However, there was a hard-coded limitation of 64 characters (including the null-terminator) in omfToolkit.dll.
//	See OMUNIQUENAME_SIZE in omDefs.h, and omfUniqueName_t in omTypes.h in Avid's OMF SDK.
#define BENTO_STRMAX_UNIQUE_NAME		(64)

//*********************************************************************************************************************
//	These constants represent the "stream elements" in a Bento version 1.0d5 Table Of Contents (TOC).
//	We cannot change these. Their literal values are hard-coded into the Bento spec.
//	See page 65 of the Bento Specification Rev 1.0d5 for details.
//*********************************************************************************************************************
class NsBentoStreamElements
{
protected:
	enum
	{
	//	Code				Value	Meaning										Bytes consumed
	//---------------------------------------------------------------------------------------------
		eNewObject			= 1,	// object + property + type                 12
		eNewProperty		= 2,	// property + type                           8
		eNewType			= 3,	// type                                      4
		eExplicitGen		= 4,	// generation                                4
		eOffset4Len4		= 5,	// 4-byte offset + 4-byte length             8
		eContdOffset4Len4	= 6,	// continued 4-byte offset + 4-byte length   8
		eOffset8Len4		= 7,	// 8-byte offset + 4-byte length            12
		eContdOffset8Len4	= 8,	// continued 8-byte offset + 4-byte length  12
		eImmediate0			= 9,	// immediate (length = 0)                    0
		eImmediate1			= 10,	// immediate (length = 1)                    4
		eImmediate2			= 11,	// immediate (length = 2)                    4
		eImmediate3			= 12,	// immediate (length = 3)                    4
		eImmediate4			= 13,	// immediate (length = 4)                    4
		eContdImmediate4	= 14,	// continued immediate (length = 4)          4
		eReferenceListID	= 15,	// references recording object ID            4
		eReserved4b			= 16,	// reserved for future use of 4 byte entry   4
		eReserved4c			= 17,	//     "     "     "    "  "  "   "    "     4
		eReserved8a			= 18,	// reserved for future use of 8 byte entry   8
		eReserved8b			= 19,	//     "     "     "    "  "  "   "    "     8
		eReserved8c			= 20,	//     "     "     "    "  "  "   "    "     8
		eReserved12a		= 21,	// reserved for future use of 12 byte entry 12
		eReserved12b		= 22,	//     "     "     "    "  "   "   "    "   12
		eReserved12c		= 23,	//     "     "     "    "  "   "   "    "   12
		eEndOfBuffer		= 24,	// end of current buffer, go to next         0
		eOffset8Len8		= 25,	// 8-byte offset + 8-byte length            16
		eContdOffset8Len8	= 26,	// continued 8-byte offset + 8-byte length  16
		eNOP				= 0xFF,	// filler, skipped							 0
	};

	enum
	{
		// This bitmask has bit N set if stream element N holds an immediate value.
		// This group includes eImmediate0, eImmediate1, eImmediate2, eImmediate3, eImmediate4, and eContdImmediate4.
		dwElemBitMaskImmediate	= 0x00007e00,

		// This bitmask has bit N set if stream element N is continued.
		// This group includes eContdOffset4Len4, eContdOffset8Len4, eContdOffset8Len8, and eContdImmediate4.
		dwElemBitMaskContinued	= 0x04004140,

		// This bitmask has bit N set if stream element N is immediate but not continued.
		// This group includes eImmediate0, eImmediate1, eImmediate2, eImmediate3, and eImmediate4.
		dwElemBitMaskImmNotCont	= 0x00003e00,

		// This bitmask has bit N set if stream element N is an offset.
		// This group includes eOffset4Len4, eContdOffset4Len4, eOffset8Len4, eContdOffset8Len4, eOffset8Len8,
		// and eContdOffset8Len8.
		dwElemBitMaskOffsetAll	= 0x060001e0,

		// This bitmask has bit N set if stream element N is eOffset4Len4 or eContdOffset4Len4.
		dwElemBitMaskOffset4x4	= 0x00000060,

		// This bitmask has bit N set if stream element N is eOffset8Len4 or eContdOffset8Len4.
		dwElemBitMaskOffset8x4	= 0x00000180,

		// This bitmask has bit N set if stream element N is eOffset8Len8 or eContdOffset8Len8.
		dwElemBitMaskOffset8x8	= 0x06000000,
	};
};

//*********************************************************************************************************************
//	These constants represent Bento's standard predefined Object IDs, Property IDs, and Datatype IDs.
//	We cannot change these. Their literal values are hard-coded into the Bento spec.
//	Notice that many of them also have corresponding global unique names (all beginning with "Apple:").
//	See "Appendix D: Standard Object IDs and Global Names" on page 99 of the Bento Specification Rev 1.0d5.
//*********************************************************************************************************************
class NsBentoStandardObjectIDs
{
protected:
enum
{
	CM_StdObjID_TOC					= 1,	// TOC object ID                         (object)
	CM_StdObjID_TOC_Seed			= 2,	// Apple:Global_Name_TOC_Seed          (property)
	CM_StdObjID_TOC_MinSeed			= 3,	// Apple:Global_Name_TOC_MinSeed       (property)
	CM_StdObjID_TOC_Object			= 4,	// Apple:Global_Name_TOC_Object        (property)
	CM_StdObjID_TOC_Container		= 5,	// Apple:Global_Name_TOC_Container     (property)
	CM_StdObjID_TOC_Deleted			= 6,	// Apple:Global_Name_TOC_Deleted       (property)
	CM_StdObjID_TOC_Free			= 7,	// Apple:Global_Name_TOC_Free          (property)
	CM_StdObjID_TOC_NewValuesTOC	= 8,	// Apple:Global_Name_TOC_NewValuesTOC  (property)
	CM_StdObjID_TOC_Target			= 9,	// Apple:Global_Name_TOC_Target        (property)
	CM_StdObjID_TOC_DeleteList		= 10,	// Apple:Global_Name_TOC_DeleteList    (property)

	CM_StdObjID_TOC_Property15		= 15,	// undocumented                        (property)

	CM_StdObjID_TOC_Type			= 19,	// Apple:Global_Name_TOC_Type              (type)
	CM_StdObjID_Writable			= 20,	// Predefined IDs less than this are
											// written to the container.
											// Predefined IDs greater than this
											// are NOT written.

	CM_StdObjID_7BitASCII			= 21,	// 7-bit ASCII                             (type)
	CM_StdObjID_8BitASCII			= 22,	// 8-bit ASCII                             (type)
	CM_StdObjID_GlobalTypeName		= 23,	// Global type name                    (property)
	CM_StdObjID_GlobalPropName		= 24,	// Global property name                (property)
	CM_StdObjID_DynamicValues		= 25,	// Apple:Dynamic_Value                 (property)
	CM_StdObjID_BaseTypes			= 26,	// Apple:Type_BaseType                 (property)
	CM_StdObjID_BaseTypeList		= 27,	// Apple:Type_BaseTypeList                 (type)
	CM_StdObjID_TargetContainer		= 28,	// Apple:Target_Container		           (type)
	CM_StdObjID_ValueUpdates		= 29,	// Apple:Type_ValueUpdates             (property)
	CM_StdObjID_UpdatesData			= 30,	// Apple:Type_UpdatesData                  (type)
	CM_StdObjID_ObjReferences		= 31,	// Referenced objects from an object   (property)
	CM_StdObjID_ObjRefData			= 32,	// Reference data                          (type)

	CM_StdObjID_32BitImmValue		= 40,	// 32-bit immediate data value             (type)
	CM_StdObjID_16BitImmValue		= 41,	// 16-bit immediate data value             (type)
	CM_StdObjID_8BitImmValue		= 42,	//  8-bit immediate data value             (type)

	CM_StdObjID_MinGeneralID		= 100,	// Minimum dynamic object ID for general standard objects
	CM_StdObjID_MinUserID	= 0x00010000,	// Minimum dynamic object ID for user objects
};
};	// NsBentoStandardObjectIDs

//	These are our enumerated values for the bStorageMode member of our TOCX_ITEM structure.
class NsBentoTocStorageModes
{
protected:
enum
{
	SM_UNKNOWN		= 0, // Uninitialized/invalid. Each of our TOCX_ITEM structures is zeroed when it is allocated.
	SM_REFLISTID	= 1, // The ID of a bookkeeping object associated with this value. See page 66 of Bento Spec.
	SM_IMMEDIATE	= 2, // The payload is four bytes or less, and lives immediately inside the TOCITEM structure.
	SM_OFFSET		= 3, // The payload lives somewhere in the file; it is not stored in the TOCITEM structure.
	SM_CACHED		= 4, // The payload has been read/copied/retrieved from its original location, and is now cached.
						 // The technique used to cache it depends on the TOCX_ITEM's OMF data type.
						 // It is beyond the scope of CReadBento. Suffice to say it will make use ob the 
};
};	// NsBentoTocStorageModes

//*********************************************************************************************************************
//	This structure is the footer that lives at the end of every Bento (and therefore every OMF) file.
//	Total size is 24 bytes.
//	See Apple's Bento Specification Revision 1.0d5.
//	Chapter 7: "Format Definition"
//	Subheading "Container Label Format"
//	Begininng at page 61.
//*********************************************************************************************************************
#pragma pack(push, 1)
typedef struct
{
	QWORD	qwMagicBytes;	// 64-bit magic byte signature. Always 0xD7726448A54D43A4. Always little-endian.
	WORD	wFlags;			// Page 26 of the Bento spec (7/15/1993) says "No container flags are currently defined."
							// But some time later (8/26/1994) Apple's OpenDoc team added kCMLittleEndian (0x0001)
							// and kCMLittleEndianTwin (0x0101) to indicate little-endian byte ordering.
							// As a result (apparently all?) v1.0d5 OMF files store 0x0101 here when the byte order
							// is little-endian. But this whole issue is moot:
							// We can safely ignore wFlags because the byte order can be inferred from wMajorVersion.
	union {
	WORD	wEncoding;		// Version 1.0d4 : 'MM' = Motorola (big-endian), 'II' = Intel (little-endian)
	WORD	wBlockSize;		// Version 1.0d5 : the size of one TOC block / 1024. (Always 0x0001 ????)
	};
	WORD	wMajorVersion;	// Valid values are 0x0001, 0x0100, 0x0002, and 0x0200. Corresponds with 1.0d4 and 1.0d5.
	WORD	wMinorVersion;	// Valid values are 0x0001, 0x0100, and 0x0000.
	UINT32	cbTocOffset32;	// 32-bit physical file position where the TOC begins.
	UINT32	cbTocSize32;	// 32-bit length of the TOC. Zero indicates the presence of an extended label. (See below.)
} BENTO_STANDARD_LABEL, BENTO_STANDARD_FOOTER;
#pragma pack(pop)

//*********************************************************************************************************************
//	This structure is a backwards-compatible 64-bit extension to the BENTO_STANDARD_LABEL.
//	It's not documented in any of the Bento or OMF specs but it is codded into some later builds of omfToolkit.dll.
//	See readLabel_Handler() and writeLabel_Handler() in <<OMFAnsiC.c>> from Avid's OMF Toolkit SDK v2.1.2.
//*********************************************************************************************************************
#pragma pack(push, 1)
typedef struct
{
	QWORD	qwExtraMagicBytes;	// 64-bit extra magic byte signature. Always 0xD7726448A54D43A5. Always little-endian.
	UINT64	cbTocOffset64;		// 64-bit physical file position where the TOC begins.
	UINT64	cbTocSize64;		// 64-bit total byte size of the TOC.
} BENTO_EXTENDED_LABEL;
#pragma pack(pop)

//*********************************************************************************************************************
//	This structure represents the last 48 bytes of a Bento/OMF file assuming it has the extra label.
//	If the file does not have the extra label then the bytes in m_extLabel are unrelated and might be anything.
//*********************************************************************************************************************
#pragma pack(push, 1)
typedef struct
{
	BENTO_EXTENDED_LABEL	m_extLabel;		// optional
	BENTO_STANDARD_LABEL	m_stdLabel;		// required
} BENTO_EXTENDED_FOOTER, *PBENTO_EXTENDED_FOOTER;
#pragma pack(pop)

//*********************************************************************************************************************
//	The Bento Table-of-Contents (TOC) is an array of TOC items.
//	Each TOC item contains information about one property for one object.
//	Although the Bento specification allows a single property to have multiple types and a value for each type,
//	the OMF specification only allows a single type and value for each property in an object.
//
//	This structure represents one TOC entry in a Bento v1.0d4 file.
//	This was used in OMF versions 1.0 through 1.5 inclusive (note that some OMF 1.5 files used Bento v1.0d5).
//	This is exactly how the data is stored in the file (tho it is subject to byte-swapping).
//	The size of this structure is always 24 bytes.
//*********************************************************************************************************************
#pragma pack(push, 1)
typedef struct
{
	DWORD	dwObject;	// the 32-bit Bento persistent-ID of the OMF object. Unique only inside this Bento file.
	DWORD	dwProperty;	// the 32-bit Bento persistent-ID of the OMF property. Unique only inside this Bento file.
	DWORD	dwDataType;	// the 32-bit Bento persistent-ID of the OMF data type. Unique only inside this Bento file.

	// The way we interpret the union below depends on bit #0 of the wFlags member at the end of this structure.
	union
	{
		// These are just three different ways of accessing the same four immediate bytes from C++.
		DWORD	dwImmediateDword;
		WORD	aImmediateWords[2];
		BYTE	aImmediateBytes[4];

		// 32-bit physical file position where data begins when the storage mode is NOT immediate.
		UINT32	cbOffset;
	};
	UINT32	cbLength;		// length of the data including zero. Always less than five when the data is immediate.
	WORD	wGeneration;	// always 0 or 1. Note that Bento's generation tracking feature was never used in OMF.
	WORD	wFlags;			// If bit #0 is set the payload is immediate. If bit #1 is set the payload is continued.
} TOC1_ITEM, *PTOC1_ITEM;	// Note that there is no such thing as a "TOC2_ITEM" structure. (But see TOCX_ITEM below.)
#pragma pack(pop)

//*********************************************************************************************************************
//	This structure represents one TOC entry as it is used internally by CReadBento.
//	It is similar to a TOC1_ITEM but it is our own invention. It is not a Bento structure or an OMF structure.
//	When CReadBento ingests an OMF file it converts/expands all TOC entries into this common format.
//	Think of it as a TOC1_ITEM structure on steroids.
//	The size of this structure is always 32 bytes.
//*********************************************************************************************************************
#pragma pack(push, 1)
typedef struct 
{
	DWORD	dwObject;		// the 32-bit Bento persistent-ID of the OMF object. Unique only inside this Bento file.
	DWORD	dwProperty;		// the 32-bit Bento persistent-ID of the OMF property. Unique only inside this Bento file.
	DWORD	dwDataType;		// the 32-bit Bento persistent-ID of the OMF data type. Unique only inside this Bento file.
	WORD	wGeneration;	// always 0 or 1. Note that Bento's generation tracking feature was never used in OMF.
	BOOLEAN	fContinued;		// TRUE if the property is fragmented and continues into the next TOCX_ITEM structure.
	BYTE	bStorageMode;	// one of our storage mode enumerations.

	// The way we interpret the union(s) below depends on our bStorageMode member (see above).
	union {
		struct {
			union {
				// When bStorageMode is SM_REFLISTID this is the 32-bit Bento persistent-ID of a "bookkeeping" object.
				// See pages 63, 66, and 68, of Bento Specification, Revision 1.0d5.
				DWORD	dwRefList;

				// When bStorageMode is SM_IMMEDIATE the payload lives in this four-byte area.
				// These are just three different ways of accessing the same four immediate bytes from C++.
				DWORD	dwImmediateDword;
				WORD	aImmediateWords[2];
				BYTE	aImmediateBytes[4];

				// When bStorageMode is SM_OFFSET this is the physical file position where the property's payload begins.
				// Note that Bento revision 1.0d5 supports 64-bit offsets and lengths!
				UINT64	cbOffset64;
				struct {
					UINT32	cbOffsetLo;		// lower 32 bits of cbOffset64
					UINT32	cbOffsetHi;		// upper 32 bits of cbOffset64
				};// struct
			};// union

			// This is the length of the payload including zero.
			// It is valid with SM_REFLISTID, SM_IMMEDIATE, and SM_OFFSET.
			// However it is NOT valid when bStorageMode is SM_CACHED!
			// Note that when bStorageMode is SM_IMMEDIATE this value can never be greater than four.
			// Also note that Bento v1.0d4 doesn't support payloads larger than 2^32, however v1.0d5 does.
			union {
				UINT64	cbLength64;			// full 64-bit length of payload
				struct {
					UINT32	cbLengthLo;		// lower 32 bits of cbLength64
					UINT32	cbLengthHi;		// upper 32 bits of cbLength64
				};// struct
			};// union
		};// struct

		// When bStorageMode is SM_CACHED everything lives in this sixteen-byte area below.
		// SM_CACHED is not an actual native Bento storage format. It's our own wrinkle.
		// This is just five different ways of accessing the same sixteen immediate bytes from C++.
		// This area can store anything as long as the total size does not exceed sixteen bytes.
		// For example in CReadOmf we use it to cache the omfi:UID data type (which is always three DWORDs).
		union {
			QWORD	aCachedQwords[2];
			DWORD	aCachedDwords[4];
			WORD	aCachedWords[8];
			BYTE	aCachedBytes[16];

			// In CReadOmf we use this to save pointers to omfi:Strings that have been read into a shared memory pool.
			// Remember that LPCSTRs are 4 or 8 bytes depending on x86 or x64 builds.
			LPCSTR	pCachedString;
		};// union
	};// union
} TOCX_ITEM, *PTOCX_ITEM, *PEXPANDED_TOC;
#pragma pack(pop)

//*********************************************************************************************************************
//	This structure is our own concoction that CReadBento uses internally.
//	It isn't a a Bento structure or an OMF structure.
//	It associates a Bento Globally Unique Name with its corresponding 32-bit Bento Object ID.
//	It also holds a 32-bit hash of the string to speed up searching.
//*********************************************************************************************************************
#pragma pack(push, 4)
typedef struct
{
	DWORD	dwObject;
	DWORD	dwHash;
	CHAR	szUniqueName[BENTO_STRMAX_UNIQUE_NAME];
} BENTO_BINDING, *PBENTO_BINDING;
#pragma pack(pop)

//*********************************************************************************************************************
//	CReadBento.
//	The class hierarchy is CReadableFile >> CReadBento >> CReadOmf ...
//	Most of the routines are for bootstrapping.
//*********************************************************************************************************************
class CReadBento
	: public CReadableFile
	, protected NsBentoStreamElements
	, protected NsBentoStandardObjectIDs
	, protected NsBentoTocStorageModes
	, protected BENTO_EXTENDED_FOOTER
{
protected:
			CReadBento(void);
	virtual	~CReadBento(void);
	HRESULT	OpenBentoFile(__in PCWSTR pwzFileName);

private:
	// Private helpers called once per lifetime from within OpenBentoFile().
	HRESULT	ReadBentoLabel(void);
	HRESULT	ReadBentoToc(void);
	HRESULT	ReadV1Toc(void);
	HRESULT	ReadV2Toc(void);
	HRESULT	VerifyToc(void);
	HRESULT	BuildPropertyNameTable(void);
	HRESULT	BuildDataTypeNameTable(void);

	// Private helpers for ReadV2Toc().
	HRESULT	CountV2TocItems(PBYTE pbCompressedToc);
	HRESULT	ExpandV2TocBE(PBYTE pbCompressedToc);
	HRESULT	ExpandV2TocLE(PBYTE pbCompressedToc);

protected:
	// Returns TRUE if pszUniqueName is a Bento-compliant "unique name".
	static BOOL	__stdcall CheckUniqueNameSyntax(__in PCSTR pszUniqueName);

	// Inventory/Inquiry.
	DWORD	FindGreatestBentoID(void);
	ULONG	CountPropertyInstancesByID(__in DWORD dwProperty);
	ULONG	CountDataTypeInstancesByID(__in DWORD dwDataType);

	// Returns TRUE if dwProperty is one of the standard Bento properties.
	// See "Appendix D: Standard Object IDs and Global Names" in the Bento Specification Revision 1.0d5.
	BOOL	IsAStandardBentoProperty(__in DWORD dwProperty);

	// Returns TRUE if dwDataType one of the standard Bento data types.
	// See "Appendix D: Standard Object IDs and Global Names" in the Bento Specification Revision 1.0d5.
	BOOL	IsAStandardBentoDataType(__in DWORD dwDataType);

	// String-to-ID conversions.
	// Returns the 32-bit Bento persistent-ID of the property or data type.
	// The value is only meaningful inside this Bento file.
	DWORD	LookupPropertyIDByName(__in LPCSTR pszPropertyName);
	DWORD	LookupDataTypeIDByName(__in LPCSTR pszDataTypeName);

	// ID-to-string conversions.
	// Returns the globally unique name for the property or data type.
	// The buffer pointed to by pchResult must be large enough to hold BENTO_STRMAX_UNIQUE_NAME characters.
	HRESULT	LookupPropertyNameFromID(__in DWORD dwProperty, __out PCHAR pchResult);
	HRESULT	LookupDataTypeNameFromID(__in DWORD dwDataType, __out PCHAR pchResult);

	// This is our lowest-level read routine.
	HRESULT	ReadItemBytes(__in ULONG iToc, __in ULONG cbBuffer, __out PVOID pBuffer);

	// If the return value is positive then it's an index to the first TOCITEM whose object ID matches dwObject.
	// If the return value is negative then it's an HRESULT with the error code OMF_E_OOBJ_NOT_FOUND.
	HRESULT	FindTocIndexForObject(__in DWORD dwObject);

	// If the return value is positive then it's an index to the first TOCITEM whose object ID matches dwObject,
	// and whose corresponding property ID matches dwProperty.
	// If the return value is negative it's either OMF_E_OOBJ_NOT_FOUND or OMF_E_PROP_NOT_FOUND.
	HRESULT	FindTocIndexForObjectWithProperty(__in DWORD dwObject, __in DWORD dwProperty);

	// If the return value is positive then it's an index to the first TOCITEM whose object ID matches dwObject,
	// and whose corresponding property ID matches dwProperty, and whose corresponding data type ID matches dwDataType.
	// If the return value is negative it's either OMF_E_OOBJ_NOT_FOUND, OMF_E_PROP_NOT_FOUND, or OMF_E_TYPE_NOT_FOUND.
	HRESULT	FindTocIndexForObjectWithPropertyAndType(__in DWORD dwObject, __in DWORD dwProperty, __in DWORD dwDataType);

	// If the return value is positive then it's an index to the first TOCITEM whose object ID matches dwObject,
	// and whose corresponding data type ID matches dwDataType.
	// If the return value is negative it's either OMF_E_OOBJ_NOT_FOUND, or OMF_E_TYPE_NOT_FOUND.
	HRESULT	FindTocIndexForObjectWithType(__in DWORD dwObject, __in DWORD dwDataType);

protected:
	PEXPANDED_TOC	m_aToc;			// array of TOCX_ITEM structures.
	ULONG	m_nTocItems;			// number of TOC_ITEMs in m_aToc[].
	BOOL	m_fBentoBigEndian;		// TRUE if the Bento layer is big endian. (Motorola)
	BOOL	m_fBentoHasExtraLabel;	// TRUE if this file has an extended label.

	DWORD	m_dwStdObjTocSeed;		// the Bento CM_StdObjID_TOC_Seed property.

	UINT64	m_cbLabelOffset64;		// file offset where the BENTO_STANDARD_LABEL (or BENTO_EXTENDED_LABEL) begins.
	UINT64	m_cbTocOffset64;		// copied from our BENTO_STANDARD_LABEL (or BENTO_EXTENDED_LABEL) structure.
	UINT32	m_cbTocSize32;			// copied from our BENTO_STANDARD_LABEL (or BENTO_EXTENDED_LABEL) structure.

	PBENTO_BINDING	m_aPropertyNames;	// an array of BENTO_BINDING structures - to manage property names.
	ULONG			m_nPropertyNames;	// number of elements in the array.

	PBENTO_BINDING	m_aDataTypeNames;	// an array of BENTO_BINDING structures - to manage data type names.
	ULONG			m_nDataTypeNames;	// number of elements in the array.

private:
	// Lookup table to convert a BentoStreamElement (0~26 inclusive) into a byte count (0, 4, 8, 12, or 16 bytes).
	const static BYTE m_aStreamElementByteCountTable[27];
};

