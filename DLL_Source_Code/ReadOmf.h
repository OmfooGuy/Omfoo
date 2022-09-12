// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadOmf.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************

#pragma once
#include "ReadBento.h"
#include "ReadOmf_ClassTables.h"
#include "ReadOmf_StringTables.h"
#include "ReadOmf_StringOrdinals.h"
#include "Omfoo_Alpha_Header.h"

//*********************************************************************************************************************
//	OMF has three different ways of representing a Toolkit version number.
//	They are omfi:VersionType, omfi:Int32Array, and omfi:ProductVersion.
//	These are reported by the HEAD's OMFI:ToolkitVersion (OMF1) and OMFI:HEAD:ToolkitVersion (OMF2) properties,
//	and by the IDNT's OMFI:IDNT:ToolkitVersion property.
//	This union is the way that we deal with them internally.
//*********************************************************************************************************************
#pragma pack(push, 1)	// align structure members to byte boundaries
typedef union {
	DWORD	dwVersion;	// as a 32-bit value for quick copies and compares.
	struct {
	BYTE	bPatch;		// the isolated patch number is in bits 00~07.
	BYTE	bTertiary;	// the isolated tertiary version is in bits 08~15.
	BYTE	bMinor;		// the isolated minor version is in bits 16~23.
	BYTE	bMajor;		// the isolated major version is in bits 24~31.
	};
} OMF_TKV, *POMF_TKV;
#pragma pack(pop)		// restore compiler's previous alignment settings

//*********************************************************************************************************************
//	Undocumented Bento/OMF array structures.
//	These are exactly how array data is physically stored in an OMF file.
//*********************************************************************************************************************
#pragma pack(push, 1)	// align structure members to byte boundaries

//	Physical layout for the data type omfi:UID.
//typedef struct {
//	LONG	dwPrefix;		// a 32-bit company-specific ID issued by the Avid Developer's Desk.
//	DWORD	dwMajor;		// the time of creation, as measured in seconds since January 1, 1970.
//	DWORD	dwMinor;		// the accumulated cpu cycles of the application. Typically GetTickCount().
//} OMF_MOB_ID, *POMF_MOB_ID;

//	Physical layout of one element in an OMF1_MOB_INDEX_ARRAY. (See the full array definition below.)
typedef struct {
	OMF_MOB_ID	mobID;
	DWORD		dwObject;
	DWORD		dwKey;
} OMF1_MOB_INDEX_ENTRY, *POMF1_MOB_INDEX_ENTRY;

//	Physical layout for the data type omfi:MobIndex.`This only exists in OMF1 files.
//	It is a variable-length structure (because it represents a variable-length array).
//	There can never be more than 65535 elements (because the element count is stored in the first WORD).
//	It is defined here having only one element. Note that when there are zero elements there is
//	usually no wElements member either (making the entire payload size zero).
typedef struct {
	WORD					wElements;
	OMF1_MOB_INDEX_ENTRY	a[ANYSIZE_ARRAY];
} OMF1_MOB_INDEX_ARRAY, *POMF1_MOB_INDEX_ARRAY;

//	Physical layout of one element in an OMF1_OBJREF_ARRAY. (See the full array definition below.)
typedef struct {
	DWORD		dwObject;
	DWORD		dwKey;
} OMF1_OBJREF_ENTRY, *POMF1_OBJREF_ENTRY;

//	Physical layout for the data type omfi:ObjRefArray in an OMF1 file.
//	It is a variable-length structure (because it represents a variable-length array).
//	There can never be more than 65535 elements because the element count is stored in the first WORD.
//	It is defined here having only one element. Note that when there are zero elements there is
//	usually no wElements member either (making the entire payload size zero).
typedef struct {
	WORD				wElements;
	OMF1_OBJREF_ENTRY	a[ANYSIZE_ARRAY];
} OMF1_OBJREF_ARRAY, *POMF1_OBJREF_ARRAY;

//	Physical layout of one element in an OMF2_OBJREF_ARRAY. (See the full array definition below.)
typedef struct {
	DWORD				dwObject;
} OMF2_OBJREF_ENTRY, *POMF2_OBJREF_ENTRY;

//	Physical layout for the data type omfi:ObjRefArray in an OMF2 file.
//	It is a variable-length structure (because it represents a variable-length array).
//	There can never be more than 65535 elements because the element count is stored in the first WORD.
//	It is defined here having only one element. Note that when there are zero elements there is
//	usually no wElements member either (making the entire payload size zero).
typedef struct {
	WORD				wElements;
	OMF2_OBJREF_ENTRY	a[ANYSIZE_ARRAY];
} OMF2_OBJREF_ARRAY, *POMF2_OBJREF_ARRAY;

//	Physical layout for the data type omfi:Position32Array in OMF1 and OMF2 files.
//	It is a variable-length structure (because it represents a variable-length array).
//	There can never be more than 65535 elements because the element count is stored in the first WORD.
//	It is defined here having the maximum number of elements. Note that when there are zero elements there is
//	usually no wElements member either (making the entire payload size zero).
typedef struct {
	WORD			wElements;
	UINT32			a[0x0000FFFF];
} OMF_POSITION32_ARRAY, *POMF_POSITION32_ARRAY;

//	Physical layout for the data type omfi:Position64Array in an OMF1 and OMF2 file.
//	It is a variable-length structure (because it represents a variable-length array).
//	There can never be more than 65535 elements (because the element count is stored in the first WORD).
///	It is defined here having the maximum number of elements. Note that when there are zero elements there is
//	usually no wElements member either (making the entire payload size zero).
typedef struct {
	WORD			wElements;
	UINT64			a[0x0000FFFF];
} OMF_POSITION64_ARRAY, *POMF_POSITION64_ARRAY;

//	This is our own structure that we use internally. It's not a 'Bento' structure or an 'OMF' structure.
//	We use it to hold the most important infomation about an OMF object.
//	It's like a shortcut that allows us to find values and properties fast, without having to step though
//	our entire array of TOCX_ITEM structures.
//
//	CReadOmf::m_aBlopTable[] is an array of these.
//
//	The name "Blop" comes from Apple's patent (which is expired).
//	"Data is stored as a plurality of Blops, each of which has a list of Properties associated with it.
//	Each Property contains zero or more elements. Elements are Values, each of which has a Basic Type
//	and consists of a variable length sequence of bytes. The Basic Type defines the format of the Value,
//	as well as meta-information such as compression and encryption."
//	- US Patent #5857207, "Storage manager for computer system" April 21, 1998 - Apple Computer Co.
typedef struct {
	DWORD	dwObject;		// The Bento object ID for this object (byte-swapped when necessary).
	DWORD	dwFourCC;		// The OMF class fourCC (or zero if this is not an OMF class object).
	ULONG	iFirstItem;		// Index in CReadOmf::m_aToc[] where our first TOCX_ITEM lives.
	WORD	wTotalItems;	// Number of contiguous TOCX_ITEM structures for this blop.
	union {
	struct {
	BYTE	bMobjAppCode;	// MOBJs only - Cache of the mob's the OMFI:MOBJ:UsageCode or OMFI:MOBJ:AppCode property.
	BOOLEAN	fMobjHasOwner;	// MOBJs only - TRUE if this mob is referenced by an OMFI:SCLP:SourceID property.
	};
	struct {
	BYTE	bReserved1;		// Reserved for future use by other OOBJ classes.
	BYTE	bReserved2;		// Reserved for future use by other OOBJ classes.
	};
	WORD	wMdatIdx;		// MDATs only - The MDAT's position in the OMFI:MediaData or OMFI:HEAD:MediaData array (plus one).
	};// union
} BENTO_BLOP, *PBENTO_BLOP;
#pragma pack(pop)			// restore compiler's previous alignment settings

//	We use this structure to cache the values of the most commonly used Property IDs and DataType IDs.
//	This saves us from calling OrdinalToPropertyID() and OrdinalToDataTypeID() every time we need them.
//  We acquire them (and populate this struct) at startup time.
struct CReadOmf_PropTypeCache
{
protected:
									// OMF2							OMF1
	DWORD	m_dwPropHeadModified;	// OMFI:HEAD:LastModified		OMFI:LastModified
	DWORD	m_dwPropHeadByteOrder;	// OMFI:HEAD:ByteOrder			OMFI:ByteOrder
	DWORD	m_dwPropHeadVersion;	// OMFI:HEAD:Version			OMFI:Version
	DWORD	m_dwPropHeadMobs;		// OMFI:HEAD:Mobs				n/a
	DWORD	m_dwPropObjectSpine;	// n/a							OMFI:ObjectSpine

	DWORD	m_dwPropClass;			// OMFI:OOBJ:ObjClass			OMFI:ObjID
	DWORD	m_dwPropClsdClass;		// OMFI:CLSD:Class				OMFI:CLSD:ClassID
	DWORD	m_dwPropAppCode;		// OMFI:MOBJ:AppCode			OMFI:MOBJ:UsageCode
	DWORD	m_dwPropMobjName;		// OMFI:MOBJ:Name				OMFI:CPNT:Name
	DWORD	m_dwPropMediaDesc;		// OMFI:SMOB:MediaDescription	OMFI:MOBJ:PhysicalMedia
	DWORD	m_dwPropMobjMobID;		// OMFI:MOBJ:MobID				OMFI:MOBJ:MobID
	DWORD	m_dwPropNextDIDDesc;	// OMFI:DIDD:NextDIDDescriptor	OMFI:DIDD:NextDIDDesc
	DWORD	m_dwPropDataOffset;		// OMFI:DIDD:DataOffset			OMFI:MDFL:dataOffset

	DWORD	m_dwTypeClass;			// omfi:ClassID					omfi:ObjectTag
	DWORD	m_dwTypeMobIndex;		// n/a							omfi:MobIndex
	DWORD	m_dwTypeObjRef;			// omfi:ObjRef					omfi:ObjRef
	DWORD	m_dwTypeObjRefArray;	// omfi:ObjRefArray				omfi:ObjRefArray
	DWORD	m_dwTypeString;			// omfi:String					omfi:String
	DWORD	m_dwTypeTimeStamp;		// omfi:TimeStamp				omfi:TimeStamp
	DWORD	m_dwTypeUID;			// omfi:UID						omfi:UID
	DWORD	m_dwTypeUniqueName;		// omfi:UniqueName				n/a

	DWORD	m_dwTypeLong;			// n/a							omfi:Long
	DWORD	m_dwTypeULong;			// n/a							omfi:Ulong
	DWORD	m_dwTypeInt32;			// omfi:Int32					omfi:Int32
	DWORD	m_dwTypeUInt32;			// omfi:UInt32					n/a

	DWORD	m_dwTypePosition32;		// omfi:Position32				n/a
	DWORD	m_dwTypePosition64;		// omfi:Position64				n/a
	DWORD	m_dwTypeLength32;		// omfi:Length32				n/a
	DWORD	m_dwTypeLength64;		// omfi:Length64				n/a

	DWORD	m_dwTypePos32Array;		// omfi:Position32Array			omfi:Position32Array
	DWORD	m_dwTypePos64Array;		// omfi:Position64Array			omfi:Position64Array
	DWORD	m_dwTypeInt32Array;		// omfi:Int32Array				omfi:Int32Array
	DWORD	m_dwTypeInt64Array;		// omfi:Int64Array				omfi:Int64Array

	DWORD	m_dwTypeDataValue;		// omfi:DataValue				omfi:DataValue
	DWORD	m_dwTypeVarLenBytes;	// n/a							omfi:VarLenBytes
};

//*********************************************************************************************************************
//	CReadOmf
//*********************************************************************************************************************
class CReadOmf
	: public CReadBento
	, protected CReadOmf_ClassTables
	, protected CReadOmf_StringOrdinals
	, protected CReadOmf_PropTypeCache
	, private CReadOmf_StringTables
{
protected:
	// Various string length limitations found in the sourcecode of Avid's omfToolkit.dll v2.1.2.
	// These 'limitations' are not part of the OMF or Bento specs, but they are a reality that we need to be aware of.
	// they exist in the reference source code that was distributed with Avid's "OMF Interchange Toolkit 2.1.2 SDK".
	enum {

	// Maximum number of characters (including null-terminator) for the data type omfi:UniqueName.
	// See omfsReadUniqueName(), OMUNIQUENAME_SIZE, and omfUniqueName_t.
	OMF_STRMAX_UNIQUE_NAME		= 64,

	// Maximum number of characters (including null-terminator) in an OMF property name.
	// See omfiGetPropertyName(), OMUNIQUENAME_SIZE, and omfUniqueName_t.
	OMF_STRMAX_PROPERTY_NAME	= 64,

	// Maximum number of characters (including null-terminator) in an OMF data type name.
	// See omfiGetPropertyTypename(), OMUNIQUENAME_SIZE, and omfUniqueName_t.
	OMF_STRMAX_DATATYPE_NAME	= 64,

	// Maximum number of characters (including null-terminator) in an OMF data kind name. [OMFI:DDEF:DataKindID]
	// See omfiDatakindGetName(() in 'omMobGet.c'.
	OMF_STRMAX_DATAKIND_NAME	= 64,

	// Maximum number of characters (including null-terminator) in an OMF effect ID string. [OMFI:EDEF:EffectID]
	// See omfiEffectDefGetInfo() in 'omMobGet.c'.
	OMF_STRMAX_EFFECT_ID		= 64,

	// Maximum number of chars (including null-terminator) in an OMF effect definition name. [OMFI:EDEF:EffectName]
	// See VerifyEffects() in 'MkSimpFX.c' aka 'WriteSimple.c'.
	 OMF_STRMAX_EFFECT_NAME		= 64,

	// Maximum number of characters (including null-terminator) in an OMF mob name. [OMFI:MOBJ:Name]
	// See OMMOBNAME_SIZE in 'omDefs.h'.
	// Warning! OMMOBNAME_SIZE is indeed 64, but I've seen mob names as long as 76 characters!
	OMF_STRMAX_MOBJ_NAME		= 64,

	// Maximum number of characters (including null-terminator) in OMFI:MDES:KitCodecDesc.
	OMF_STRMAX_MDES_CODEC_NAME	= 64,

	// Maximum number of characters (including null-terminator) in OMFI:MDES:KitCodecID.
	OMF_STRMAX_MDES_CODEC_ID	= 64,

	// Maximum number of characters (including null-terminator) in an attribute name.
	// See omfiMobAppendComment() in 'omMobBld.c'.
	OMF_STRMAX_ATTB_NAME		= 64,

	// Maximum number of characters (including null-terminator) in a locator filename.
	// See openLocator() and omfmLocatorGetInfo().
	OMF_STRMAX_LOCR_FILENAME	= 256,

	// Maximum number of characters (including null-terminator) for the data type omfi:String.
	// I've found no hard-coded limitation, however ...
	// The legacy 68k Macintosh operating system (and Apple's original Macintosh compilers) used
	// 'pascal strings' which limit the maximum string length to 255 characters (256 bytes total).
	OMF_STRMAX_GENERIC_STRING	= 256,
	};

protected:
			CReadOmf(void);
	virtual	~CReadOmf(void);
	HRESULT	OpenOmfFile(__in PCWSTR pwzFileName);

	// Private initialization routine called once per lifetime during OpenOmfFile().
private:
	HRESULT	DetectMajorVersion(void);
	HRESULT	DetectByteOrder(void);
	HRESULT	GetToolkitVersionFromHead(void);
	HRESULT	GetToolkitVersionsFromIdnts(void);
	HRESULT	DetectMinorVersion(void);
	HRESULT	CacheCommonPropertyIDs(void);
	HRESULT	CacheCommonDataTypeIDs(void);
	HRESULT	FixupBentoTocSeed(void);
	HRESULT	BuildBlopTable(void);

protected:
	BENTO_BLOP&	GetBlop(DWORD dwObject);	// always succeeds, even when dwObject is invalid or zero.

	HRESULT	FindTocItemForProperty(BENTO_BLOP& rBlop, DWORD dwProperty, PTOCX_ITEM& rpTocItem);
	HRESULT	FindTocItemForPropertyAndType(BENTO_BLOP& rBlop, DWORD dwProperty, DWORD dwDataType, PTOCX_ITEM& rpTocItem);
	HRESULT	FindTocItemForType(BENTO_BLOP& rBlop, DWORD dwDataType, PTOCX_ITEM& rpTocItem);

public:
	BOOL	IsClassDefined(__in DWORD dwClassFourCC);
	BOOL	HasObjectOfClass(__in DWORD dwClassFourCC);
	ULONG	CountObjectsOfClass(__in DWORD dwClassFourCC);
	DWORD	GetObjectClassFourCC(__in DWORD dwObject);

	BOOL	HasPropertyInstances(__in DWORD dwProperty);
	BOOL	HasDataTypeInstances(__in DWORD dwDataType);

//	BOOL	IsAFamiliarProperty(__in DWORD dwProperty);
//	BOOL	IsAFamiliarDataType(__in DWORD dwDataType);

	DWORD	NameToPropertyID(__in LPCSTR pszPropertyName);
	DWORD	OrdinalToPropertyID(__in PropOrdinal eProperty);
//	INT		PropertyIDToOrdinal(DWORD dwProperty);

	DWORD	NameToDataTypeID(__in LPCSTR pszDataTypeName);
	DWORD	OrdinalToDataTypeID(__in DataTypeOrdinal eType);
//	INT		DataTypeIDToOrdinal(__in DWORD dwDataType);

	LPCSTR	OrdinalToPropertyName(__in PropOrdinal eProperty);
	LPCSTR	OrdinalToDataTypeName(__in DataTypeOrdinal eDataType);
	LPCSTR	OrdinalToDataKindName(__in DataKindOrdinal eDataKind);
	LPCSTR	OrdinalToEffectIDString(__in EffectOrdinal eEffect);

	HRESULT	DataKindIDStringToWord(__in LPCSTR pszDataKindID, __out PWORD pwDataKindID);
	HRESULT	EffectIDStringToWord(__in LPCSTR pszEffectID, __out PWORD pwEffectID);

	//
	BOOL	IsProperlyFormedPropertyString(__in LPCSTR pszPropertyName);
	BOOL	IsProperlyFormedDataTypeString(__in LPCSTR pszDataTypeName);
	BOOL	IsProperlyFormedDataKindString(__in LPCSTR pszDataKindID);
	BOOL	IsProperlyFormedEffectIDString(__in LPCSTR pszEffectID);

public:
	BOOL	m_fOmfVer1;			// TRUE if the file is OMF version 1.x. FALSE if it is version 2.x.
	BOOL	m_fOmfBigEndian;	// TRUE if the OMF byte order is big endian.
	ULONG	m_nBlops;			// number of elements in m_aBlopTable (see below).

protected:
	BENTO_BLOP*	m_aBlopTable;	// our master array of BENTO_BLOP structures.

	// This array holds the version numbers of the OMF Toolkit libraries that touched or created this file.
	// These are reported by the HEAD's OMFI:ToolkitVersion (OMF1) and OMFI:HEAD:ToolkitVersion (OMF2) properties,
	// and by the IDNT's OMFI:IDNT:ToolkitVersion properties.
	OMF_TKV	m_tkv[4];
	ULONG	m_ntkv;		// the number of populated (non-zero) elements in m_tkv[].

	// This is our own reconstruction of what the HEAD's OMFI:Version or OMFI:HEAD:Version property
	// _SHOULD_ look like if things weren't so overly-complicated and twisted.
	// It is NOT a verbatim copy of those properties.
	// See our comments in DetectMinorVersion().
	OMF_VERSIONTYPE	m_omfVersion;

private:
	// This is a special-case BENTO_BLOP structure. It is always all zeroes!
	// We use this to guarantee that our GetBlop() routine will always succeed.
	// It's not a member of m_aBlopTable[]. It is part of our deferred error handling scheme.
	// We return a reference to this BENTO_BLOP in GetBlop(dwObject) when the dwObject argument is invalid or zero.
	// The invalid dwObject will eventually be detected in the first call to a method that tries to access the blop.
	BENTO_BLOP	m_oEmptyBlop;

	// Cache of Bento property IDs.
	// Once we look up a property ID we save a copy here - see OrdinalToPropertyID().
	DWORD	m_aPropertyIDCache[eMaxProperty];

	// Cache of Bento data type IDs.
	// Once we look up a data type ID we save a copy here - see OrdinalToDataTypeID().
	DWORD	m_aDataTypeIDCache[eMaxDataType];
};
