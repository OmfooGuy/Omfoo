// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadOmf.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include <shlwapi.h>
#include "ReadOmf.h"

//	OMF makes heavy use of FOURCCs. 
//	There are various C/C++ macros that make it easier to declare FOURCC values in source code.
//	This one is barrowed from <Aviriff.h> in the Windows SDK.
#ifndef FCC
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))
#endif

//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack.
//	It assumes that its C++ operator new() has already zeroed all of its memory.
//*********************************************************************************************************************
CReadOmf::CReadOmf(void)
{
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CReadOmf::~CReadOmf(void)
{
	if (m_aBlopTable)
	{
		VirtualFree(m_aBlopTable, SIZE_T(0), MEM_RELEASE);
		m_aBlopTable = NULL;
	}
}

//*********************************************************************************************************************
//	Protected.
//*********************************************************************************************************************
HRESULT CReadOmf::OpenOmfFile(PCWSTR pwzFileName)
{
	CHECK(OpenBentoFile(pwzFileName));
	CHECK(DetectMajorVersion());
	CHECK(DetectByteOrder());
	CHECK(GetToolkitVersionFromHead());
	CHECK(GetToolkitVersionsFromIdnts());
	CHECK(CacheCommonPropertyIDs());
	CHECK(CacheCommonDataTypeIDs());
	CHECK(BuildBlopTable());
	CHECK(DetectMinorVersion());
	CHECK(FixupBentoTocSeed());

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//	Here we set CReadOmf::m_fOmfVer1 to TRUE or FALSE depending on if the major version is OMF1 or OMF2.
//	We detect the major version by testing for the presence (or absence) of certain properties in the HEAD object.
//	(And while we're at it we cache the property IDs so we don't have to find them again.)
//
//	We can't use the version property in the HEAD because 1) in OMF1 the "OMFI:Version" property was optional,
//	and 2) because in OMF2 the version property was named "OMFI:HEAD:Version" which required you to know the
//	OMF version before you could query for the OMF version. Duh!
//
//	Note that at this point we don't know the OMF byte order, but we DO know the Bento layer's byte order.
//	And all of those 32-bit object, property, and data type IDs have already been byte-swapped in CReadBento.
//*********************************************************************************************************************
HRESULT	CReadOmf::DetectMajorVersion()
{
	DWORD	dwPropHeadModified	= 0;
	DWORD	dwPropHeadByteOrder	= 0;
	DWORD	dwPropHeadVersion	= 0;
	DWORD	dwPropHeadMobs		= 0;
	DWORD	dwPropObjectSpine	= 0;

	// Detect OMF2 first. 
	// Test for the presence (or absence) of these OMF2-specific properties associated with the HEAD object.
	// We do not need to read or retrieve these properties right now.
	// We just test to see if they are defined in this OMF file.
	dwPropHeadModified	= OrdinalToPropertyID(ePropHeadLastModified);	// OMFI:HEAD:LastModified
	dwPropHeadByteOrder	= OrdinalToPropertyID(ePropHeadByteOrder);		// OMFI:HEAD:ByteOrder
	dwPropHeadVersion	= OrdinalToPropertyID(ePropHeadVersion);		// OMFI:HEAD:Version
	dwPropHeadMobs		= OrdinalToPropertyID(ePropHeadMobs);			// OMFI:HEAD:Mobs

	// If any of those properties are defined then this is an OMF2 file.
	// (Normally all or none will be defined.)
	if (dwPropHeadModified || dwPropHeadByteOrder || dwPropHeadVersion || dwPropHeadMobs)
	{
		// Set m_fOmfVer1 to FALSE because this is an OMF2 file.
		m_fOmfVer1	= FALSE;

		// Save the property IDs.
		// If some are zero that's okay.
		m_dwPropHeadModified	= dwPropHeadModified;
		m_dwPropHeadByteOrder	= dwPropHeadByteOrder;
		m_dwPropHeadVersion		= dwPropHeadVersion;
		m_dwPropHeadMobs		= dwPropHeadMobs;
		m_dwPropObjectSpine		= 0;

		// Done.
		return S_OK;
	}

	// Detect OMF1 after OMF2.
	// Test for the presence (or absence) of these OMF1-specific properties associated with the HEAD object.
	// We do not need to read or retrieve these properties right now.
	// We just test to see if they are defined in this OMF file.
	dwPropHeadModified	= OrdinalToPropertyID(ePropRev1LastModified);	// OMFI:LastModified
	dwPropHeadByteOrder	= OrdinalToPropertyID(ePropRev1ByteOrder);		// OMFI:ByteOrder
	dwPropHeadVersion	= OrdinalToPropertyID(ePropRev1Version);		// OMFI:Version
	dwPropObjectSpine	= OrdinalToPropertyID(ePropRev1ObjectSpine);	// OMFI:ObjectSpine

	// If any of those properties are defined then this is an OMF1 file.
	if (dwPropHeadModified || dwPropHeadByteOrder || dwPropHeadVersion || dwPropObjectSpine)
	{
		// Set m_fOmfVer1 to TRUE because this is an OMF1 file.
		m_fOmfVer1	= TRUE;

		// Save the property IDs.
		// If some are zero that's okay,
		m_dwPropHeadModified	= dwPropHeadModified;
		m_dwPropHeadByteOrder	= dwPropHeadByteOrder;
		m_dwPropHeadVersion		= dwPropHeadVersion;
		m_dwPropHeadMobs		= 0;
		m_dwPropObjectSpine		= dwPropObjectSpine;

		// Done.
		return S_OK;
	}

	// We can not detect the OMF major version!
	BREAK_IF_DEBUG	// has this ever happened?

	// Done.
	return OMF_E_BAD_MAJOR_VERSION;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//*********************************************************************************************************************
HRESULT	CReadOmf::DetectByteOrder()
{
	HRESULT	hr			= S_OK;
	WORD	wByteOrder	= 0;

	// We populated m_dwPropHeadByteOrder in DetectMajorVersion().
	if (m_dwPropHeadByteOrder == 0)
	{
		BREAK_IF_DEBUG	// has this ever happened?

		hr = OMF_E_REQUIRED_PROP_MISSING;
		goto L_Exit;
	}

	// Find the TOC index to the HEAD's byte order property.
	// Right now the HEAD is the only object we can interrogate because it's the only OMF object with a fixed ID.
	// If this method succeeds then hr is a positive success code that holds the TOC index.
	if (FAILED(hr = FindTocIndexForObjectWithProperty(CM_StdObjID_TOC, m_dwPropHeadByteOrder)))
	{
		BREAK_IF_DEBUG	// has this ever happened?

		hr = OMF_E_BAD_FILE_BYTE_ORDER;
		goto L_Exit;
	}

	// Pass hr as the TOC index to ReadItemBytes().
	if (FAILED(hr = ReadItemBytes(hr, sizeof(WORD), &wByteOrder)))
	{
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	// The value of the ByteOrder property must be either ’MM’ (0x4D4D) for 'Motorola' big-endian,
	// or ’II’ (0x4949) for 'Intel' little-endian byte order.
	if (wByteOrder == 'II')
	{
		m_fOmfBigEndian = FALSE;
	}
	else if (wByteOrder == 'MM')
	{
		m_fOmfBigEndian = TRUE;
	}
	else
	{
		// WTF?
		// The HEAD's byte order property exists, but its value is not 'II' or 'MM'!
		BREAK_IF_DEBUG	// has this ever happened?
		hr = OMF_E_BAD_FILE_BYTE_ORDER;
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//	Reads the HEAD's OMFI:ToolkitVersion (OMF1) or OMFI:HEAD:ToolkitVersion (OMF2).
//	Saves the result our m_tkv[] member.
//	The OMF1 implementation always succeeds even when there is no toolkit version property.
//	The OMF2 implmentation will fail only if it encounters a file system error. Otherwise it always succeeds.
//*********************************************************************************************************************
HRESULT	CReadOmf::GetToolkitVersionFromHead()
{
	// Assume we will succeed.
	HRESULT hr = S_OK;

	// Make sure there is room to append more values in our m_tkv[] array.
	if (m_ntkv >= ELEMS(m_tkv))
	{
		BREAK_IF_DEBUG
		// Not a fatal error.
		hr = S_OK;
		goto L_Exit;
	}

	if (m_fOmfVer1)
	{
		// OMFI:ToolkitVersion, omfi:VersionType
		DWORD dwProperty = OrdinalToPropertyID(ePropRev1ToolkitVersion);			
		if (dwProperty == 0)
		{
			// Not a fatal error.
			// In OMF1 this property was optional.
			goto L_Exit;
		}
		DWORD dwDataType = OrdinalToDataTypeID(eTypeVersionType);
		if (dwDataType == 0)
		{
			// Unexpected, but not a fatal error.
			BREAK_IF_DEBUG
			goto L_Exit;
		}
		// If this method succeeds then idx holds the TOC index to the property.
		LONG idx = FindTocIndexForObjectWithPropertyAndType(CM_StdObjID_TOC, dwProperty, dwDataType);
		if (SUCCEEDED(idx))
		{
			BYTE aVersion[2] = {0};
			if (SUCCEEDED(ReadItemBytes(idx, sizeof(aVersion), &aVersion)))
			{
				m_tkv[m_ntkv].bMajor	= aVersion[0];
				m_tkv[m_ntkv].bMinor	= aVersion[1];
				m_tkv[m_ntkv].bTertiary	= 0;	// n/a
				m_tkv[m_ntkv].bPatch	= 0;	// n/a

				// Increment the element counter.
				m_ntkv++;
			}
		}
	}
	else
	{
		// OMFI:HEAD:ToolkitVersion, omfi:Int32Array
		DWORD dwProperty = OrdinalToPropertyID(ePropHeadToolkitVersion);
		if (dwProperty == 0)
		{
			// Not a fatal error.
			// In OMF2 this property is optional.
			goto L_Exit;
		}
		DWORD dwDataType = OrdinalToDataTypeID(eTypeInt32Array);
		if (dwDataType == 0)
		{
			// Unexpected, but not a fatal error.
			BREAK_IF_DEBUG
			goto L_Exit;
		}
		// If this method succeeds then idx holds the TOC index to the property.
		LONG idx = FindTocIndexForObjectWithPropertyAndType(CM_StdObjID_TOC, dwProperty, dwDataType);
		if (SUCCEEDED(idx))
		{
			// In OMF2 the property is an omfi:Int32Array, which is an array of three 32-bit DWORDs.
			DWORD aToolkitVer[3] = {0};
			if (SUCCEEDED(hr = ReadItemBytes(idx, sizeof(aToolkitVer), &aToolkitVer)))
			{
				if (m_fOmfBigEndian)
				{
					aToolkitVer[0] = Endian32(aToolkitVer[0]);
					aToolkitVer[1] = Endian32(aToolkitVer[1]);
					aToolkitVer[2] = Endian32(aToolkitVer[2]);
				}

				// None of these 32-bit values should ever be greater than 255.
				// The major version should be 1 or 2.
				// The minor version should be less than 6.
				if ((aToolkitVer[0] > 0) &&
					(aToolkitVer[0] <= 2) &&
					(aToolkitVer[1] <= 5) &&
					(aToolkitVer[2] <= 255))
				{
					m_tkv[m_ntkv].bMajor	= BYTE(aToolkitVer[0]);
					m_tkv[m_ntkv].bMinor	= BYTE(aToolkitVer[1]);
					m_tkv[m_ntkv].bTertiary	= BYTE(aToolkitVer[2]);
					m_tkv[m_ntkv].bPatch	= 0;	// n/a

					// Increment the element counter.
					m_ntkv++;
				}
			}
		}
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//	Reads the OMFI:IDNT:ToolkitVersion property for each Identification Class (IDNT) object in the HEAD's
//	OMFI:HEAD:IdentificationList array and saves the result in m_tkv[] and then increments m_ntkv.
//*********************************************************************************************************************
HRESULT	CReadOmf::GetToolkitVersionsFromIdnts()
{
	// Not a fatal error if no properties exist.
	HRESULT	hr = S_OK;

	// OMFI:IDNT:ToolkitVersion, omfi:ProductVersion
	DWORD dwPropToolkitVersion = OrdinalToPropertyID(ePropIdntToolkitVersion);
	if (dwPropToolkitVersion == 0)
	{
		// Not a fatal error.
		// This property is optional.
		goto L_Exit;
	}
	DWORD dwTypeProductVersion = OrdinalToDataTypeID(eTypeProductVersion);
	if (dwTypeProductVersion == 0)
	{
		// Unexpected.
		// But not a fatal error.
		BREAK_IF_DEBUG
		goto L_Exit;
	}

	PTOCX_ITEM	pCurItem	= m_aToc;
	PTOCX_ITEM	pTocEnd		= &m_aToc[m_nTocItems];
	do
	{
		// Make sure there is room to append more values in our m_tkv[] array.
		if (m_ntkv >= ELEMS(m_tkv))
		{
			// This is a natural way to break out of this loop.
			// It's not a fatal error.
			// THis can happen when there are more IDNTs than there are elements in m_tkv[].
			BREAK_IF_DEBUG
			break;
		}

		if ((pCurItem->dwProperty	== dwPropToolkitVersion) &&
			(pCurItem->dwDataType	== dwTypeProductVersion) &&
			(pCurItem->bStorageMode	== SM_OFFSET) &&
			(pCurItem->cbLength64	== sizeof(OMF_PRODUCT_VERSION)))
		{
			OMF_PRODUCT_VERSION	omfProductVersion	= {0};
			if (FAILED(hr = SeekRead(pCurItem->cbOffset64, &omfProductVersion, sizeof(OMF_PRODUCT_VERSION))))
			{
				// SeekRead() can return a fatal file system error.
				goto L_Exit;
			}

			if (m_fOmfBigEndian)
			{
				omfProductVersion.wMajor	= Endian16(omfProductVersion.wMajor);
				omfProductVersion.wMinor	= Endian16(omfProductVersion.wMinor);
				omfProductVersion.wTertiary	= Endian16(omfProductVersion.wTertiary);
				omfProductVersion.wPatch	= Endian16(omfProductVersion.wPatch);
				omfProductVersion.wDetail	= Endian16(omfProductVersion.wDetail);
			}

			// The major version must be 1 or 2.
			// The minor version must be five or less.
			// The wTertiary and wPatch members should be 255 or less.
			// If the wTertiary or wPatch members are greater than 255 they get maxed.
			if ((omfProductVersion.wMajor > 0) &&
				(omfProductVersion.wMajor <= 2) &&
				(omfProductVersion.wMinor <= 5))
			{
				if (omfProductVersion.wTertiary > 0x00FF)
				{
					omfProductVersion.wTertiary = 0x00FF;
				}
				if (omfProductVersion.wPatch > 0x00FF)
				{
					omfProductVersion.wPatch = 0x00FF;
				}

				m_tkv[m_ntkv].bMajor	= BYTE(omfProductVersion.wMajor);
				m_tkv[m_ntkv].bMinor	= BYTE(omfProductVersion.wMinor);
				m_tkv[m_ntkv].bTertiary	= BYTE(omfProductVersion.wTertiary);
				m_tkv[m_ntkv].bPatch	= BYTE(omfProductVersion.wPatch);

				// Increment the element counter.
				m_ntkv++;
			}
		}
	} while (++pCurItem < pTocEnd);

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//*********************************************************************************************************************
HRESULT	CReadOmf::CacheCommonPropertyIDs()
{
	if (m_fOmfVer1)
	{
		// Cache these frequently-used OMF1 property IDs.
		// If any of these properties not defined in this file it is not considered a fatal error.
		m_dwPropAppCode				= OrdinalToPropertyID(ePropMobjUsageCode);			// OMFI:MOBJ:UsageCode
		m_dwPropClass				= OrdinalToPropertyID(ePropRev1ObjID);				// OMFI:ObjID
		m_dwPropClsdClass			= OrdinalToPropertyID(ePropClsdClassID);			// OMFI:CLSD:ClassID
		m_dwPropMobjName			= OrdinalToPropertyID(ePropCpntName);				// OMFI:CPNT:Name
		m_dwPropMediaDesc			= OrdinalToPropertyID(ePropMobjPhysicalMedia);		// OMFI:MOBJ:PhysicalMedia
		m_dwPropNextDIDDesc			= OrdinalToPropertyID(ePropDiddNextDIDDesc);		// OMFI:DIDD:NextDIDDesc
		m_dwPropDataOffset			= OrdinalToPropertyID(ePropMdfldataOffset);			// OMFI:MDFL:dataOffset
	}
	else
	{
		// Cache these frequently-used OMF2 property IDs.
		// If any of these properties are not defined in this file it is not considered a fatal error.
		m_dwPropAppCode				= OrdinalToPropertyID(ePropMobjAppCode);			// OMFI:MOBJ:AppCode
		m_dwPropClass				= OrdinalToPropertyID(ePropOobjObjClass);			// OMFI:OOBJ:ObjClass
		m_dwPropClsdClass			= OrdinalToPropertyID(ePropClsdClass);				// OMFI:CLSD:Class
		m_dwPropMobjName			= OrdinalToPropertyID(ePropMobjName);				// OMFI:MOBJ:Name
		m_dwPropMediaDesc			= OrdinalToPropertyID(ePropSmobMediaDescription);	// OMFI:SMOB:MediaDescription
		m_dwPropNextDIDDesc			= OrdinalToPropertyID(ePropDiddNextDIDDescriptor);	// OMFI:DIDD:NextDIDDescriptor
		m_dwPropDataOffset			= OrdinalToPropertyID(ePropDiddDataOffset);			// OMFI:DIDD:DataOffset
	}

	// Discover the OMFI:MOBJ:MobID property ID.
	// This property is identical in both OMF versions and unlike the ones above, IT IS REQUIRED!
	// It is the only property that MUST be defined in order for this routine to succeed!
	m_dwPropMobjMobID = OrdinalToPropertyID(ePropMobjMobID);
	if (m_dwPropMobjMobID == 0)
	{
		BREAK_IF_DEBUG
		return OMF_E_REQUIRED_PROP_MISSING;
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//*********************************************************************************************************************
HRESULT	CReadOmf::CacheCommonDataTypeIDs()
{
	if (m_fOmfVer1)
	{
		// Cache these frequently-used OMF1 data type IDs.
		m_dwTypeClass		= OrdinalToDataTypeID(eTypeObjectTag);		// omfi:ObjectTag
		m_dwTypeVarLenBytes	= OrdinalToDataTypeID(eTypeVarLenBytes);	// omfi:VarLenBytes
		m_dwTypeMobIndex	= OrdinalToDataTypeID(eTypeMobIndex);		// omfi:MobIndex

		// For OMF1 the omfi:MobIndex data type is always required.
		if (0 == m_dwTypeMobIndex)
		{
			BREAK_IF_DEBUG
			return OMF_E_REQUIRED_TYPE_MISSING;
		}
	}
	else
	{
		// Cache these frequently-used OMF2 data type IDs.
		m_dwTypeClass		= OrdinalToDataTypeID(eTypeClassID);		// omfi:ClassID
		m_dwTypePosition32	= OrdinalToDataTypeID(eTypePosition32);		// omfi:Position32
		m_dwTypePosition64	= OrdinalToDataTypeID(eTypePosition64);		// omfi:Position64
		m_dwTypeLength32	= OrdinalToDataTypeID(eTypeLength32);		// omfi:Length32
		m_dwTypeLength64	= OrdinalToDataTypeID(eTypeLength64);		// omfi:Length64
		m_dwTypeUniqueName	= OrdinalToDataTypeID(eTypeUniqueName);		// omfi:UniqueName
	}

	// Cache these frequently-used data type IDs. These are the same for OMF1 and OMF2.
	m_dwTypeDataValue		= OrdinalToDataTypeID(eTypeDataValue);			// omfi:DataValue
	m_dwTypeObjRef			= OrdinalToDataTypeID(eTypeObjRef);				// omfi:ObjRef
	m_dwTypeObjRefArray		= OrdinalToDataTypeID(eTypeObjRefArray);		// omfi:ObjRefArray
	m_dwTypeTimeStamp		= OrdinalToDataTypeID(eTypeTimeStamp);			// omfi:TimeStamp
	m_dwTypeUID				= OrdinalToDataTypeID(eTypeUID);				// omfi:UID
	m_dwTypeString			= OrdinalToDataTypeID(eTypeString);				// omfi:String
	m_dwTypeInt32			= OrdinalToDataTypeID(eTypeInt32);				// omfi:Int32
	m_dwTypeUInt32			= OrdinalToDataTypeID(eTypeUInt32);				// omfi:UInt32
	m_dwTypeInt32Array		= OrdinalToDataTypeID(eTypeInt32Array);			// omfi:Int32Array
	m_dwTypeInt64Array		= OrdinalToDataTypeID(eTypeInt64Array);			// omfi:Int64Array
	m_dwTypePos32Array		= OrdinalToDataTypeID(eTypePosition32Array);	// omfi:Position32Array
	m_dwTypePos64Array		= OrdinalToDataTypeID(eTypePosition64Array);	// omfi:Position64Array
	m_dwTypeLong			= OrdinalToDataTypeID(eTypeLong);				// omfi:Long
	m_dwTypeULong			= OrdinalToDataTypeID(eTypeUlong);				// omfi:Ulong

	// For our intents and purposes the omfi:VarLenBytes and omfi:DataValue data types are one and the same.
	// The only difference is that omfi:VarLenBytes only exists in OMF1 and supports SM_IMMEDIATE.
	// So here we guarantee that m_dwTypeVarLenBytes and m_dwTypeDataValue are always non-zero.
	// We do this to streamline our routines that read omfi:VarLenBytes and omfi:DataValue.
	if (m_dwTypeVarLenBytes == 0)
	{
		m_dwTypeVarLenBytes = m_dwTypeDataValue;
	}
	else if (m_dwTypeDataValue == 0)
	{
		m_dwTypeDataValue = m_dwTypeVarLenBytes; 
	}

	// Make sure these data types are defined. They all exist in OMF1 and OMF2. They are absolutely required!
	// If this OMF file doesn't define these types then we treat it as a fatal error, and abort here and now.
	if ((0 == m_dwTypeDataValue) ||
		(0 == m_dwTypeClass) ||
		(0 == m_dwTypeObjRef) ||
		(0 == m_dwTypeObjRefArray) ||
		(0 == m_dwTypeString) ||
		(0 == m_dwTypeUID))
	{
		BREAK_IF_DEBUG
		return OMF_E_REQUIRED_TYPE_MISSING;
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//	Allocates and populates our m_aBlopTable[] - which is an array of BENTO_BLOP structures.
//	On exit our m_nBlops member contains the number of elements in the m_aBlopTable[] array.
//
//	Here we loop through each object in m_aToc[] and collect the following info:
//	1) The Bento object ID for this object.
//	2) The OMF class ID (a FOURCC) for this blop.
//	3) The index of the first property (the first TOCX_ITEM structure) for this blop.
//	4) The number of properties (the number of contiguous TOCX_ITEM structures) for this blop.
//*********************************************************************************************************************
HRESULT	CReadOmf::BuildBlopTable()
{
	HRESULT	hr					= S_OK;			// assume success!
	ULONG	nBlops				= 0;			// number of blops (the number of actual objects)
	ULONG	i					= 0;			// current table index
	UINT32	cbAlloc				= 0;			// number of bytes required for the table
	DWORD	dwPreviousObject	= 0;			// initialize this with an illegal/impossible value
	PBENTO_BLOP	pDst			= NULL;			// pointer to current BENTO_BLOP to populate.

	// Loop through each item in CReadBento::m_aToc[] and count the number of discrete objects.
	for (i = 0; i < m_nTocItems; i++)
	{
		// Compare object IDS.
		// If they're different then we've traversed into the next object.
		if (m_aToc[i].dwObject != dwPreviousObject)
		{
			// Increment the object/blop count.
			nBlops++;

			// Save the current object ID so we can detect when we've encounterd a new one.
			dwPreviousObject = m_aToc[i].dwObject;
		}
	}

	// Reality check. We never allow more than one million blops!
	if (nBlops > 1000000)
	{
		BREAK_IF_DEBUG
		hr = OMF_E_TOO_MANY_OOBJS;
		goto L_Exit;
	}

	// Calculate the memory requirements so we can allocate our m_aBlopTable[].
	cbAlloc = nBlops * sizeof(BENTO_BLOP);

	// Allocate the array. Note that the memory is zeroed.
	m_aBlopTable = PBENTO_BLOP(VirtualAlloc(NULL, cbAlloc, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
	if (NULL == m_aBlopTable)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_Exit;
	}

	// Initialize our pointer to point to the very first BENTO_BLOP structure in our newly alocated array.
	pDst = m_aBlopTable;

	// We are almost ready to loop through m_aToc[] again.
	// However, we treat the very first item as a special case.
	// We initialize the first BENTO_BLOP structure outside of the for() loop.
	// Note that the very first item (the very first TOCX_ITEM structure) always belongs
	// to the Bento self-description object: aka CM_StdObjID_TOC - aka the OMF HEAD object.
	// And its property is always one of the standard CM_StdObjID_TOC_xxxx properties.

	// Copy the first object ID into the first blop - which is always CM_StdObjID_TOC (0x00000001).
	pDst->dwObject = m_aToc[0].dwObject;

	// Do the same for dwPreviousObject so we can detect when we've traversed into a new object.
	dwPreviousObject = m_aToc[0].dwObject;

	// Okay, now we loop through each remaining item in m_aToc[] beginning with the second item.
	for (i = 1; i < m_nTocItems; i++)
	{
		// Does this item belong to a new object?
		if (m_aToc[i].dwObject != dwPreviousObject)
		{
			// Yes! We've crossed over into the next object. This item belongs to a new object.
			// Now we can calculate the number of items for the previous object, and update its 'previous' entry.
			ULONG nItems = i - pDst->iFirstItem;

			// Limit the number of items (for the current blop) to 32767.
			if (nItems > 0x00007FFF)
			{
				BREAK_IF_DEBUG
				hr = OMF_E_TOO_MANY_PROPS;
				goto L_Exit;
			}

			pDst->wTotalItems = WORD(nItems);

			// Increment pointer to the next entry.
			++pDst;

			// Save current index in the new entry.
			pDst->iFirstItem = i;

			// Save dwObject in 'current' entry.
			pDst->dwObject = m_aToc[i].dwObject;

			// Update dwPreviousObject, so we can detect when we've traversed into the next object.
			dwPreviousObject = m_aToc[i].dwObject;
		}

		// Is the current item the class ID property for the current object?
		if ((m_aToc[i].dwProperty == m_dwPropClass) &&
			(m_aToc[i].dwDataType == m_dwTypeClass) &&
			(m_aToc[i].bStorageMode == SM_IMMEDIATE) &&
			(m_aToc[i].cbLength64 == sizeof(DWORD)))
		{
			// Get the OMF class FourCC. Save it in pDst->dwFourCC.
			// This property is always four bytes long, and it is always stored as an immediate value.
			// Technically it is an array of four ASCII characters and so we never need to byte-swap.
			pDst->dwFourCC = m_aToc[i].dwImmediateDword;
		}
	} // for (each toc item)

	// Update the final entry outside of the for() loop.
	ULONG nItems = i - pDst->iFirstItem;

	// Limit the number of items (for the final blop) to 65535.
	if (nItems > 0x0000FFFF)
	{
		BREAK_IF_DEBUG
		hr = OMF_E_TOO_MANY_PROPS;
	}
	else
	{
		pDst->wTotalItems = WORD(nItems);

		// Save count permanently.
		m_nBlops = nBlops;
	}

L_Exit:
	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//	In DetectMajorVersion() we detected the OMF major version (1 or 2) and saved the result in the BOOL m_fOmfVer1.
//	Now we attempt to find the minor revision with (or without) the help of the OMFI:HEAD:Version property.
//	We must call GetToolkitVersionFromHead() and GetToolkitVersionsFromIdnts() before calling here.
//	We save the results in our CReadOmf::m_omfVersion member.
//
//	The problem with the OMFI:HEAD:Version property is that the minor revision is usually a lie. (It's usually zero.)
//	The rationale for this deceit is explained in the OMF 2.1 Specification.
//	See in Appendix D "Changes in Version 2.1" at page 231.
//
//		"The changes introduced the OMF Interchange Specification Version 2.1 are minor enhancements to Version 2.0.
//		Consequently, the file version number stored in the Header object remains 2.0 and is not changed."
//
//	Fwiw the comments for omfsFileGetRev() in Avid's Toolkit 2.1.2 suggest that there were five revisions to OMF1.
//
//		Supported revisions are:
//			kOmfRev1x -- Rev 1.0-1.5 files
//			kOmfRevIMA -- Rev 1.5 files with Bento 1.0d5
//			kOmfRev2x -- Rev 2.0- files
//
//	Pardon my obsession with detecting the 'true' OMF minor version. This is a lot of code that does very little.
//	It was part of a reverse-engineering/learning experience, and so since this code exists, I’ll just leave it in.
//*********************************************************************************************************************
HRESULT	CReadOmf::DetectMinorVersion()
{
	HRESULT hr = S_OK;

	// Wipe these members (even tho they should be zero already).
	m_omfVersion.bMajor = 0;
	m_omfVersion.bMinor = 0;

	// Find the latest (greatest/most recent) Toolkit version number.
	// We collected these values in GetToolkitVersionFromHead() and GetToolkitVersionsFromIdnts().
	OMF_TKV tkv = {0};
	for (ULONG i = 0; i < m_ntkv; i++)
	{
		if (tkv.dwVersion < m_tkv[i].dwVersion)
		{
			tkv.dwVersion = m_tkv[i].dwVersion;
		}
	}

	// Counters that help us determine the minor revision.
	ULONG nCounterA = 0;
	ULONG nCounterB = 0;
	ULONG nCounterC = 0;

	// This holds the property ID for OMFI:ToolkitVersion (OMF1) or OMFI:HEAD:ToolkitVersion (OMF2).
	// We are not using this to detect the Toolkit version, we are using it to detect the OMF revision.
	// We just need to know if the property is defined.
	DWORD dwPropTkVersion = 0;

	// We will retrieve the OMFI:HEAD:Version (or OMFI:Version) property and manipulate it as a WORD (if it exists).
	WORD wStoredValue = 0;

	// If this method succeeds then idx holds the TOC index to the OMFI:HEAD:Version (or OMFI:Version) property.
	// But if it fails then idx is an HRESULT with a meaningful error code.
	LONG idx = FindTocIndexForObjectWithProperty(CM_StdObjID_TOC, m_dwPropHeadVersion);
	if (SUCCEEDED(idx))
	{
		// The omfi:VersionType is a tiny two-byte structure. It is NOT 16-bit WORD. It's essentially a two-byte array.
		// The byte at offset 0 should be the major version, and the byte at offset 1 should be the minor revision.
		// Therefore it should be immune from endianness issues. This is true regardless of the OMF byte order.
		// Nevertheless some (or maybe all ??) OMF1 files messed it up and apparently wrote it as a 16-bit WORD.
		// So this routine is coded so that it can't be fooled - even if the omfi:VersionType byte order is reversed.
		ReadItemBytes(idx, sizeof(wStoredValue), &wStoredValue);
	}
	else
	{
		// The OMFI:HEAD:Version (or OMFI:Version) property does not exist.
		// We do not treat this as a fatal error. In v1.x this property was optional.
		// Just proceed with wStoredValue = 0.
	}

	// The very first object in the Bento TOC is always CM_StdObjID_TOC (0x00000001).
	// Its very first property is always CM_StdObjID_TOC_Seed (0x00000002).
	// And its very first data type is always CM_StdObjID_TOC_Type (0x00000013).
	// Count the number of consecutive TOC properties whose data type is CM_StdObjID_TOC_Type.
	// We save the result in nCounterA.
	for (ULONG i = 0; i < m_nTocItems; i++)
	{
		if (m_aToc[i].dwDataType == CM_StdObjID_TOC_Type)
		{
			nCounterA++;
		}
		else
		{
			// This loop should always terminate prematurely right here.
			// Expected break points are 5 and 7.
			// Anything else is unexpected (but not a fatal error).
			break;
		}
	}
	// Set nCounterA aside. We'll come back to it during this if/else statement.

	// We already detected the major version in DetectMajorVersion().
	if (m_fOmfVer1)
	{
		// First we try the straightforward approach.
		// If the HEAD's OMFI:Version property is v1.1, v1.2, v1.3, v1.4, or v1.5 then we trust it.
		// But if the OMFI:Version property is anything else (it will usually be v1.0) then do NOT trust it!
		if (wStoredValue == 0x0101) 
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 1;
			goto L_Exit;
		}
		if ((wStoredValue == 0x0201)||(wStoredValue == 0x0102))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 2;
			goto L_Exit;
		}
		if ((wStoredValue == 0x0301)||(wStoredValue == 0x0103))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 3;
			goto L_Exit;
		}
		if ((wStoredValue == 0x0401)||(wStoredValue == 0x0104))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 4;
			goto L_Exit;
		}
		if ((wStoredValue == 0x0501)||(wStoredValue == 0x0105))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 5;
			goto L_Exit;
		}

		#ifdef _DEBUG
		// If wStoredValue is anything other than zero or 1 then break and take a look - just for curiosity.
		// Fwiw none of my test files ever break here - which is what we want and hope to expect.
		if ((wStoredValue != 0x0000)&&(wStoredValue != 0x0100)&&(wStoredValue != 0x0001))
		{
			// What is the value?
			// How did we get here?
			// What are we going to do about it?
			BREAK_IF_DEBUG
		}
		#endif

		// If the Bento Label's major version is 0x0002 (indicating Bento v1.0d5) then this is an OMF v1.5 file.
		// Avid's Toolkit 2.1 calls this version "kOmfRevIMA".
		if (m_stdLabel.wMajorVersion == 0x0002)
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 5;
			goto L_Exit;
		}

		// If the OMFI:CLSD:Version property is defined, then this is an OMF v1.5 file.
		// Most OMF1 files fall into this category.
		// if (OrdinalToPropertyID(ePropClsdVersion))
		if (NameToPropertyID("OMFI:CLSD:Version"))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 5;
			goto L_Exit;
		}

		// If this file was created (or modified) with Toolkit v2.0 or later then this is an OMF v1.5 file.
		if (tkv.bMajor == 2)
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 5;
			goto L_Exit;
		}

		// If the TKMN class is defined then this is a v1.0 file.
		// Afaik this class had no properties and was never used, but for whatever reason it was defined
		// with a OMFI:CLSD:ClassID/omfi:ObjectTag.
		if (IsClassDefined(FCC('TKMN')))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// Take inventory of these OMF1-specific HEAD-related properties.
		// OMFI:ContainerOffsetAtClose
		nCounterB += (0 != OrdinalToPropertyID(ePropRev1ContOffsetAtClose));

		// OMFI:DelBlobsSize
		nCounterB += (0 != OrdinalToPropertyID(ePropRev1DelBlobsSize));

		// OMFI:NumDelMobs
		nCounterB += (0 != OrdinalToPropertyID(ePropRev1NumDelMobs));

		// OMFI:TOCOffsetAtClose
		nCounterB += (0 != OrdinalToPropertyID(ePropRev1TOCOffsetAtClose));

		// OMFI:HEAD:IdentificationList
		nCounterC += (0 != OrdinalToPropertyID(ePropHeadIdentificationList));

		// OMFI:ExternalFiles
		nCounterC += (0 != OrdinalToPropertyID(ePropRev1ExternalFiles));

		// Analyze results.
		// If the TOC began with five (not seven) consecutive TOC properties whose data type is CM_StdObjID_TOC_Type,
		// and if NONE of the group B properties are defined; and if ALL of the group C properties are defined
		// then this is an OMF v1.5 file.
		if ((nCounterA == 5)&&(nCounterB == 0)&&(nCounterC == 2))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 5;
			goto L_Exit;
		}

		// If the TOC began with seven (not five) consecutive TOC properties whose data type is CM_StdObjID_TOC_Type,
		// and if ALL of the group B properties are defined, and if NONE of the group C properties are defined
		// then this is an OMF v1.0 file.
		if ((nCounterA == 7)&&(nCounterB == 4)&&(nCounterC == 0))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// Trouble ahead!
		// If we’ve reached here we're scraping the bottom of the barrel.
		// None of my test files ever reach here (unless I comment out the previous tests)!
		// I'm hoping that our previous tests will always succeed.
		// Nevertheless here are a few more tests that can detect v1.x files.
		// The order in which we perform these needs to be prioritized.
		BREAK_IF_DEBUG

		// The OMFI:MDES:KitCodecDesc and OMFI:MDES:KitCodecID properties were not defined in v1.0.
		if (OrdinalToPropertyID(ePropMdesKitCodecDesc)&&
			OrdinalToPropertyID(ePropMdesKitCodecID))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 5;
			goto L_Exit;
		}

		// The ATCP class and its OMFI:ATCP:Attributes property were not defined in v1.0.
		if (!IsClassDefined(FCC('ATCP')))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// If this file was created (or modified) with Toolkit v1.x then this is some type of OMF v1.x file.
		if ((tkv.bMajor == 1)&&(tkv.bMinor <= 5))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = tkv.bMinor;	// assume the toolkit revision number is also the OMF minor version.
			goto L_Exit;
		}

		// OMFI:MOBJ:_CreationTime
		// This property only exists in early v1.x files.
		// It was removed in later v1.x files.
		if (OrdinalToPropertyID(ePropMobj_CreationTime))
		{
			m_omfVersion.bMajor = 1;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// The HEAD of an early v1.x file has an 8-byte omfi:TimeStamp in its OMFI:LastModified property.
		DWORD dwPropLastModified = OrdinalToPropertyID(ePropRev1LastModified);
		LONG idx = FindTocIndexForObjectWithProperty(CM_StdObjID_TOC, dwPropLastModified);
		if (SUCCEEDED(idx))
		{
			if (m_aToc[idx].cbLength64 == 8)
			{
				// This is OMF v1.0.
				m_omfVersion.bMajor = 1;
				m_omfVersion.bMinor = 0;
				goto L_Exit;
			}
		}

		BREAK_IF_DEBUG
		// Worst-case scenario. We can't detect the OMF minor revision.
		// Remember that we can still rely on m_fOmfVer1 to discern the major version. 
		m_omfVersion.bMajor = 1;
		m_omfVersion.bMinor = 0;
		goto L_Exit;
	}
	else
	{
		// If OMFI:HEAD:Version identifies itself as v2.1 then we accept it with no questions asked.
		// However this is extremely unlikely (impossible?) since Toolkit v2.1 always saved the version as v2.0.
		// Also note that omfsInternOpenFile() in Avid's Toolkit v2.1 will reject everything except v2.0. 
		if ((wStoredValue == 0x0102)||(wStoredValue == 0x0201))
		{
			BREAK_IF_DEBUG				// Does this ever happen?
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 1;
			goto L_Exit;
		}

		// Take inventory of these OMF2-specific HEAD-related properties.
		// OMFI:HEAD:ContainerOffsetAtClose
		nCounterB += (0 != OrdinalToPropertyID(ePropHeadContOffsetAtClose));

		// OMFI:HEAD:TOCOffsetAtClose
		nCounterB += (0 != OrdinalToPropertyID(ePropHeadTOCOffsetAtClose));

		// OMFI:HEAD:NumDelMobs
		nCounterB += (0 != OrdinalToPropertyID(ePropHeadNumDelMobs));

		// OMFI:HEAD:Blobs
		nCounterB += (0 != OrdinalToPropertyID(ePropHeadBlobs));

		// OMFI:HEAD:DelBlobsSize
		nCounterB += (0 != OrdinalToPropertyID(ePropHeadDelBlobsSize));

		// OMFI:HEAD:ToolkitVersion
		dwPropTkVersion = OrdinalToPropertyID(ePropHeadToolkitVersion);

		// Analyze counters.
		if ((nCounterA == 7)&&(nCounterB == 5)&&(dwPropTkVersion == 0))
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		if ((nCounterA == 5)&&(nCounterB == 0)&&(dwPropTkVersion > 0))
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 1;
			goto L_Exit;
		}

		// Trouble ahead!
		// If we’ve reached here we're scraping the bottom of the barrel.
		// Does this ever happen?
		// None of my test files ever reach here (unless I comment out the previous tests)!
		// I'm hoping that our previous tests will always succeed.
		// Nevertheless here are a few more tests that can detect v2.x files.
		// The order in which we perform these needs to be prioritized.
		BREAK_IF_DEBUG

		// If the OMFI:ERAT:InputEditRate property is misspelled as "OMFI:ERAT:InputEditRage"
		// then we know that this is a v2.0 file. This typo was fixed in OMF v2.1.
		if (OrdinalToPropertyID(ePropEratInputEditRage))
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// Starting with OMF2 the OMFI:MOBJ:UsageCode property was renamed as "OMFI:MOBJ:AppCode".
		// And the corresponding omfi:UsageCodeType data type was replaced by the omfi:Int32 data type.
		// Nevertheless the omfi:UsageCodeType was still defined (even tho it was never used) in OMF v2.0 files!
		// Apparently someone forgot to remove the definition. It was finally removed in OMF v2.1.
		if (OrdinalToDataTypeID(eTypeUsageCodeType))
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// The OMFI:CVAL:ParameterID property is only defined in v2.0 files. It was removed in v2.1.
		if (OrdinalToPropertyID(ePropCvalParameterID))
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// In OMF v2.0 the WINL's path property was named "OMFI:WINL:FullPathName".
		// But in OMF v2.1 the WINL's path property was renamed "OMFI:WINL:PathName".
		if (OrdinalToPropertyID(ePropWinlFullPathName))
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		// If the Toolkit version is v2.1 or greater, then this is OMF v2.1.
		// Remember that this property is optional.
		if (tkv.dwVersion >= 0x02010000)
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 1;
			goto L_Exit;
		}

		// If the Toolkit version is v2.0 then this is OMF v2.0.
		// Remember that this property is optional.
		if (tkv.dwVersion >= 0x02000000)
		{
			m_omfVersion.bMajor = 2;
			m_omfVersion.bMinor = 0;
			goto L_Exit;
		}

		BREAK_IF_DEBUG
		// Worst-case scenario. We can't detect the OMF minor revision.
		m_omfVersion.bMajor = 2;
		m_omfVersion.bMinor = 0;
	}

L_Exit:	// Common exit point.

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenOmfFile().
//	In CReadBento::VerifyToc() we retrieved the TOC's CM_StdObjID_TOC_Seed property.
//	We byte-swapped it if m_fBentoBigEndian was TRUE, and then we saved it in m_dwStdObjTocSeed.
//	Now we need to byte-swap it again.
//*********************************************************************************************************************
HRESULT CReadOmf::FixupBentoTocSeed(void)
{
	if (!m_fBentoBigEndian)
	{
		if (m_fOmfBigEndian)
		{
			m_dwStdObjTocSeed = Endian32(m_dwStdObjTocSeed);
		}
	}
	return S_OK;
}

//*********************************************************************************************************************
//	Returns a reference to the BENTO_BLOP structure that corresponds to caller’s dwObject argument.
//	If dwObject is not a valid object then it returns a reference to a special-case BENTO_BLOP structure.
//	The special-case structure is always all zeroes, and can safely be passed to any of the BlopReadXXXX() routines.
//	And those routines, in turn, will always fail gracefully.
//*********************************************************************************************************************
BENTO_BLOP& CReadOmf::GetBlop(DWORD dwObject)
{
	for (UINT i = 0; i < m_nBlops; i++)
	{
		if (dwObject == m_aBlopTable[i].dwObject)
		{
			return m_aBlopTable[i];
		}
	}
	return m_oEmptyBlop;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
BOOL CReadOmf::IsProperlyFormedPropertyString(__in LPCSTR pszPropertyName)
{
	if (CheckUniqueNameSyntax(pszPropertyName))
	{
		// Avid's namespace for Open Media Framework property names. 
		if (0 == StrCmpNA(pszPropertyName, "OMFI:", 5))
		{
			return TRUE;
		}

		// Apple's namespace for Bento Globally Unique names (includes objects, properties, and data types.)
		if (0 == StrCmpNA(pszPropertyName, "Apple:", 6))
		{
			return TRUE;
		}

		// RFC-4122's UUID-based namespace (requires no registration).
		if (0 == StrCmpNA(pszPropertyName, "urn:uuid:", 9))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
BOOL CReadOmf::IsProperlyFormedDataTypeString(__in LPCSTR pszDataTypeName)
{
	if (CheckUniqueNameSyntax(pszDataTypeName))
	{
		// Avid's namespace for Open Media Framework data type names.
		if (0 == StrCmpNA(pszDataTypeName, "omfi:", 5))
		{
			return TRUE;
		}

		// Apple's namespace for Bento Globally Unique names (includes objects, properties, and data types.)
		if (0 == StrCmpNA(pszDataTypeName, "Apple:", 6))
		{
			return TRUE;
		}

		// RFC-4122's UUID-based namespace (requires no registration).
		if (0 == StrCmpNA(pszDataTypeName, "urn:uuid:", 9))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Test the syntax of caller's data kind string.
//	See the Data Definition Class object (DDEF), its OMFI:DDEF:DataKindID property, and the omfi:UniqueName data type,
//	in Appendix A of the OMF Interchange Specification 2.1, begining on page 132.
//	See also the description of 'Data Kind' at page 39.
//*********************************************************************************************************************
BOOL CReadOmf::IsProperlyFormedDataKindString(__in LPCSTR pszDataKindID)
{
	if (CheckUniqueNameSyntax(pszDataKindID))
	{
		if (0 == StrCmpNA(pszDataKindID, "omfi:data:", 10))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Test the syntax of caller's effect ID string.
//	in Appendix A of the OMF Interchange Specification 2.1, begining on page 143.
//	See also the description of 'Effect Invocations' at page 74.
//*********************************************************************************************************************
BOOL CReadOmf::IsProperlyFormedEffectIDString(__in LPCSTR pszEffectID)
{
	if (CheckUniqueNameSyntax(pszEffectID))
	{
		if (0 == StrCmpNA(pszEffectID, "omfi:effect:", 12))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Returns TRUE if we are familiar with the property associated with dwProperty.
//	See our list of all known OMF properties in <OmfooStringOrdinals.h>
//	Bento itself has fifteen 'standard' properties. Their property IDs are predefined, fixed, and etched in stone.
//	They are not assigned dynamically; they have the same literal value in every Bento file.
//	See the list of "Standard Object IDs and Global Names" in Appendix D of Bento Specification Rev 1.0d5.
//*********************************************************************************************************************
/*
BOOL CReadOmf::IsAFamiliarProperty(DWORD dwProperty)
{
	if (PropertyIDToOrdinal(dwProperty) >= 0)	// returns -1 if dwProperty isn't on our list.
	{
		return TRUE;
	}
	else
	{
		return IsAStandardBentoProperty(dwProperty);
	}
}
*/

//*********************************************************************************************************************
//	Returns TRUE if we are familiar with the data type associated with dwDataType.
//	See our list of all known OMF data types in <OmfooStringOrdinals.h>
//	Bento itself has ten 'standard' data types. Their data type IDs are predefined, fixed, and etched in stone.
//	They are not assigned dynamically; they have the same literal value in every Bento file.
//	See the list of "Standard Object IDs and Global Names" in Appendix D of Bento Specification Rev 1.0d.
//*********************************************************************************************************************
/*
BOOL CReadOmf::IsAFamiliarDataType(DWORD dwDataType)
{
	if (DataTypeIDToOrdinal(dwDataType) >= 0)	// returns -1 if dwDataType isn't on our list.
	{
		return TRUE;
	}
	else
	{
		return IsAStandardBentoDataType(dwDataType);
	}
}
*/

//*********************************************************************************************************************
//	String-to-ID conversion routine.
//	Given the full name of an OMF property, retrieves the corresponding property ID as a DWORD, or zero.
//	Here is an example >>> OMFI:MDFL:SampleRate.
//*********************************************************************************************************************
DWORD CReadOmf::NameToPropertyID(__in LPCSTR pszPropertyName)
{
	DWORD dwProperty = 0;

	if (IsProperlyFormedPropertyString(pszPropertyName))
	{
		DWORD	dwHash		= 0;
		DWORD	dwChar		= 0;
		ULONG	cch			= 0;
		LPCSTR	psz			= pszPropertyName;

		while (dwChar = DWORD(*psz++))
		{
			dwHash = _rotl(dwHash,11)^(dwChar*4246609);

			if (++cch == OMF_STRMAX_PROPERTY_NAME)
			{
				// Caller's string is to long to be any of the property names we're familiar with.
				return 0;
			}
		}

		for (UINT i = 0; i < m_nPropertyNames; i++)
		{
			if (dwHash == m_aPropertyNames[i].dwHash)
			{
				if (lstrcmpA(pszPropertyName, m_aPropertyNames[i].szUniqueName) == 0)
				{
					// Found it!
					dwProperty = m_aPropertyNames[i].dwObject;
					break;
				}
				else
				{
					// Hash collision! This is not a fatal error.
					// Consider changing the hash multiplier (currently 4246609) or shift count (currently 11).
					BREAK_IF_DEBUG
				}
			}
		}
	}

	return dwProperty;
}

//*********************************************************************************************************************
//	Given a PropOrdinal of an OMF property, retrieve its assigned property ID if it's defined in the file, or zero.
//	See the header file <OmfPropertyOrdinals.h> for a comprehensive list of OMF properties.
//*********************************************************************************************************************
DWORD CReadOmf::OrdinalToPropertyID(__in PropOrdinal eProperty)
{
	// Validate our caller's ordinal.
	if (eProperty >= eMaxProperty)
	{
 		return 0;	// Special case. Invalid argument. WTF! Return as if not found.
	}

	// Is the property ID already in our table?
	DWORD dwProperty = m_aPropertyIDCache[eProperty];

	// Test for a special case where the value is negative.
	// If dwProperty is -1 then that means that we've already looked for it, but we can't find it.
	if (dwProperty == DWORD(-1))
	{
		return 0;	// not found! not here! don't continue!
	}

	// If dwProperty is any other non-zero value then it's a bona fide property ID.
	// So just return it.
	if (dwProperty)
	{
		return dwProperty;
	}

	// Else look up the full property name using our table.
	LPCSTR pszPropertyName = LPCSTR(m_aPropertyStringData) + m_aPropertyStringOffsets[eProperty];

	// Get the pre-computed hash value from our lookup table.
	DWORD dwHash = m_aPropertyStringHashTable[eProperty];

	for (UINT i = 0; i < m_nPropertyNames; i++)
	{
		if (dwHash == m_aPropertyNames[i].dwHash)
		{
			if (lstrcmpA(pszPropertyName, m_aPropertyNames[i].szUniqueName) == 0)
			{
				// Found it!
				dwProperty = m_aPropertyNames[i].dwObject;

				// Save its assigned value in our lookup table.
				m_aPropertyIDCache[eProperty] = dwProperty;

				// Done!
				return dwProperty;
			}

			// Hash collision! This is not a fatal error.
			// Consider changing the hash multiplier or shift count.
			BREAK_IF_DEBUG
		}
	}

	// Now that we know for sure that this property is not defined in this file
	// we invalidate its corresponding entry in m_aPropertyIDCache[].
	m_aPropertyIDCache[DWORD(eProperty)] = DWORD(-1);

	//
	return 0;
}

//*********************************************************************************************************************
//	Returns -1 if not found.
//*********************************************************************************************************************
/*
INT CReadOmf::PropertyIDToOrdinal(DWORD dwProperty)
{
	INT	nResult	= -1;

	if (dwProperty >= CM_StdObjID_MinUserID)
	{
		for (UINT iName = 0; iName < m_nPropertyNames; iName++)
		{
			if (m_aPropertyNames[iName].dwObject == dwProperty)
			{
				DWORD dwHash = m_aPropertyNames[iName].dwHash;
				for (UINT iProperty = 0; iProperty < UINT(eMaxProperty); iProperty++)
				{
					if (dwHash == m_aPropertyStringHashTable[iProperty])
					{
						LPCSTR pszRomTableName = LPCSTR(m_aPropertyStringData) + m_aPropertyStringOffsets[iProperty];
						if (0 == lstrcmpA(pszRomTableName, m_aPropertyNames[iName].szUniqueName))
						{
							nResult = INT(iProperty);
							goto L_Exit;
						}
					}
				}
			}
		}
	}

L_Exit:
	return nResult;
}
*/

//*********************************************************************************************************************
//	String-to-ID conversion routine.
//	Given the full name of an OMF data type, retrieves the corresponding data type ID as a DWORD, or zero.
//	Here is an example >>> omfi:UInt32.
//*********************************************************************************************************************
DWORD CReadOmf::NameToDataTypeID(__in LPCSTR pszDataTypeName)
{
	DWORD dwDataType = 0;

	if (IsProperlyFormedDataTypeString(pszDataTypeName))
	{
		DWORD	dwHash		= 0;
		DWORD	dwChar		= 0;
		ULONG	cch			= 0;
		LPCSTR	psz			= pszDataTypeName;

		while (dwChar = DWORD(*psz++))
		{
			dwHash = _rotl(dwHash,11)^(dwChar*4246609);

			if (++cch == OMF_STRMAX_DATATYPE_NAME)
			{
				return 0;
			}
		}

		for (UINT i = 0; i < m_nDataTypeNames; i++)
		{
			if (dwHash == m_aDataTypeNames[i].dwHash)
			{
				if (strcmp(pszDataTypeName, m_aDataTypeNames[i].szUniqueName) == 0)
				{
					dwDataType = m_aDataTypeNames[i].dwObject;
					break;
				}
				else
				{
					// Hash collision! This is not a fatal error.
					// But consider changing the hash multiplier or shift count.
					BREAK_IF_DEBUG
				}
			}
		}
	}

	return dwDataType;
}

//*********************************************************************************************************************
//	Given a TypeOrdinal of an OMF data type, retrieve its assigned data type ID if it's defined in the file, or zero.
//	See the header file <OmfDataTypeOrdinals.h> for a comprehensive list of OMF data types.
//*********************************************************************************************************************
DWORD CReadOmf::OrdinalToDataTypeID(__in DataTypeOrdinal eDataType)
{
	// Validate our caller's ordinal.
	if (eDataType >= eMaxDataType)
	{
 		return 0;	// Special case. Invalid argument. WTF! Return as if not found.
	}

	// Is the data type ID already in our table?
	DWORD dwDataType = m_aDataTypeIDCache[eDataType];

	// Test for a special case where the value is negative.
	// If dwDataType is -1 then that means that we've already looked for it, but we can't find it.
	if (dwDataType == DWORD(-1))
	{
		return 0;	// not found! not here! don't continue!
	}

	// If dwDataType is any other non-zero value then it is a bona fide data type ID.
	// So just return it.
	if (dwDataType)
	{
		return dwDataType;
	}

	// Look up the full data type name using our table.
	LPCSTR	pszTypeName	= LPCSTR(m_aDataTypeStringData) + m_aDataTypeStringOffsets[eDataType];

	// Get the pre-computed hash value from our other table.
	DWORD	dwHash		= m_aDataTypeStringHashTable[eDataType];

	for (UINT i = 0; i < m_nDataTypeNames; i++)
	{
		if (dwHash == m_aDataTypeNames[i].dwHash)
		{
			if (lstrcmpA(pszTypeName, m_aDataTypeNames[i].szUniqueName) == 0)
			{
				// Found it!
				dwDataType = m_aDataTypeNames[i].dwObject;

				// Save its assigned value in our lookup table.
				m_aDataTypeIDCache[eDataType] = dwDataType;

				// Done!
				return dwDataType;
			}

			// Hash collision! This is not a fatal error.
			// Consider changing the hash multiplier or shift count.
			BREAK_IF_DEBUG
		}
	}

	// Now that we know for sure that this property is not defined in
	// this file we invalidate its corresponding entry in m_aDataTypeIDCache[].
	m_aDataTypeIDCache[DWORD(eDataType)] = DWORD(-1);

	//
	return 0;
}

//*********************************************************************************************************************
//	Returns -1 if not found.
//*********************************************************************************************************************
/*
INT CReadOmf::DataTypeIDToOrdinal(DWORD dwDataType)
{
	INT	nResult	= -1;

	if (dwDataType >= CM_StdObjID_MinUserID)
	{
		for (UINT iName = 0; iName < m_nDataTypeNames; iName++)
		{
			if (m_aDataTypeNames[iName].dwObject == dwDataType)
			{
				DWORD dwHash = m_aDataTypeNames[iName].dwHash;
				for (UINT iDataType = 0; iDataType < UINT(eMaxDataType); iDataType++)
				{
					if (dwHash == m_aDataTypeStringHashTable[iDataType])
					{
						LPCSTR pszRomTableName = LPCSTR(m_aDataTypeStringData) + m_aDataTypeStringOffsets[iDataType];
						if (0== lstrcmpA(pszRomTableName, m_aDataTypeNames[iName].szUniqueName))
						{
							nResult = INT(iDataType);
							goto L_Exit;
						}
					}
				}
			}
		}
	}

L_Exit:
	return nResult;
}
*/

//*********************************************************************************************************************
//	Returns a pointer to the constant null-terminated ascii string specified by eProperty.
//	For example "OMFI:MOBJ:Name".
//	The string lives somewhere in our m_aPropertyStringData[] array.
//*********************************************************************************************************************
LPCSTR CReadOmf::OrdinalToPropertyName(__in PropOrdinal eProperty)
{
	LPCSTR	pszUniqueName	= NULL;
	if (eProperty < eMaxProperty)
	{
		pszUniqueName	= LPCSTR(m_aPropertyStringData) + m_aPropertyStringOffsets[eProperty];
	}

	return pszUniqueName;
}

//*********************************************************************************************************************
//	Returns a pointer to the constant null-terminated ascii string specified by eDataType.
//	For example "omfi:UInt32".
//	The string lives somewhere in our m_aDataTypeStringData[] array.
//*********************************************************************************************************************
LPCSTR CReadOmf::OrdinalToDataTypeName(__in DataTypeOrdinal eDataType)
{
	LPCSTR	pszUniqueName	= NULL;

	if (eDataType < eMaxDataType)
	{
		pszUniqueName	= LPCSTR(m_aDataTypeStringData) + m_aDataTypeStringOffsets[eDataType];
	}

	return pszUniqueName;
}

//*********************************************************************************************************************
//	Returns a pointer to the constant null-terminated ascii string specified by eDataKind.
//	For example "omfi:data:Picture".
//	The string lives somewhere in our m_aDataKindStringData[] array.
//*********************************************************************************************************************
LPCSTR CReadOmf::OrdinalToDataKindName(__in DataKindOrdinal eDataKind)
{
	LPCSTR	pszName	= NULL;

	if (eDataKind < eMaxDataKind)
	{
		pszName	= LPCSTR(m_aDataKindStringData) + m_aDataKindStringOffsets[eDataKind];
	}

	return pszName;
}

//*********************************************************************************************************************
//	Returns a pointer to the constant null-terminated ascii string specified by eEffect.
//	For example "omfi:effect:SimpleMonoAudioDissolve".
//	The string lives somewhere in our m_aEffectIDStringData[] array.
//*********************************************************************************************************************
LPCSTR CReadOmf::OrdinalToEffectIDString(__in EffectOrdinal eEffect)
{
	LPCSTR	pszName	= NULL;

	if (eEffect < eMaxEffectIDString)
	{
		pszName	= LPCSTR(m_aEffectIDStringData) + m_aEffectIDStringOffsets[eEffect];
	}

	return pszName;
}

//*********************************************************************************************************************
//	OMFI:DDEF:DataKindID (omfi:UniqueName) >>> OMFI:CPNT:TrackKind (omfi:TrackType).
//	Map an OMF2 Data Definition Class object's OMFI:DDEF:DataKindID string to an OMF1 OMFI:CPNT:TrackKind.
//	The string must be one of the 'Data Kinds' listed in Table 10 of the OMF2 spec, or one of the handful
//	of the kinds that I've discovered by reverse engineering.
//	See the 'DataKindOrdinal' enumerations in <ReadOmf_StringOrdinals.h>
//*********************************************************************************************************************
HRESULT CReadOmf::DataKindIDStringToWord(__in LPCSTR pszDataKind, __out PWORD pwDataKindID)
{
	HRESULT hr = OMF_E_DDEF_SURPRISE;

	if (IsBadWritePointer(pwDataKindID, sizeof(WORD)))
	{
		return E_POINTER;
	}

	*pwDataKindID = 0;

	if (!IsProperlyFormedDataKindString(pszDataKind))
	{
		return E_INVALIDARG;
	}

	DWORD	dwHash		= 0;
	DWORD	dwChar		= 0;
	ULONG	cch			= 0;
	LPCSTR	psz			= pszDataKind;

	while (dwChar = DWORD(*psz++))
	{
		dwHash = _rotl(dwHash,11)^(dwChar*4246609);

		if (++cch == OMF_STRMAX_DATAKIND_NAME)
		{
			return OMF_E_DDEF_SURPRISE;
		}
	}

	for (UINT i = 0; i < UINT(eMaxDataKind); i++)
	{
		if (dwHash == m_aDataKindStringHashTable[i])
		{
			if (lstrcmpA(pszDataKind, LPCSTR(m_aDataKindStringData) + m_aDataKindStringOffsets[i]) == 0)
			{
				hr				= S_OK;
				WORD wDataKind	= WORD(i);

				// We currently reserve the fist three values immediately after TT_META to save room for
				// aditional undiscovered OMF1 TrackKinds.
				// The known TrackKinds are hard-coded into omfTookKit.dll.
				if (wDataKind > DK_META)
				{
					wDataKind += 3;
				}
				*pwDataKindID = wDataKind;
				break;
			}
			else
			{
				// Hash collision! This is not a fatal error.
				// But consider changing the hash multiplier or shift count.
				BREAK_IF_DEBUG
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//
//*********************************************************************************************************************
HRESULT CReadOmf::EffectIDStringToWord(__in LPCSTR pszEffectID, __out PWORD pwEffectID)
{
	HRESULT hr = OMF_E_EDEF_SURPRISE;

	if (IsBadWritePointer(pwEffectID, sizeof(WORD)))
	{
		return E_POINTER;
	}

	*pwEffectID = 0;

	if (!IsProperlyFormedEffectIDString(pszEffectID))
	{
		return E_INVALIDARG;
	}

	DWORD	dwHash		= 0;
	DWORD	dwChar		= 0;
	ULONG	cch			= 0;
	LPCSTR	psz			= pszEffectID;

	while (dwChar = DWORD(*psz++))
	{
		dwHash = _rotl(dwHash,11)^(dwChar*4246609);

		if (++cch == OMF_STRMAX_EFFECT_ID)
		{
			return E_INVALIDARG;
		}
	}

	for (UINT i = 0; i < UINT(eMaxEffectIDString); i++)
	{
		if (dwHash == m_aEffectIDStringHashTable[i])
		{
			if (lstrcmpA(pszEffectID, LPCSTR(m_aEffectIDStringData) + m_aEffectIDStringOffsets[i]) == 0)
			{
				hr			= S_OK;
				*pwEffectID = WORD(i);
				break;
			}
			else
			{
				// Hash collision!
				// Consider changing the hash multiplier or shift count.
				BREAK_IF_DEBUG
			}
		}
	}

	return hr;
}

//*********************************************************************************************************************
//	Returns TRUE if the class is defined in a Class Dictionary Entry (CLSD)

//	Prop:	OMFI:CLSD:ClassID
//	Type:	omfi:ObjectTag
//	Format:	Immediate (length = 4)
//	Value:	'TKMN'

//*********************************************************************************************************************
BOOL CReadOmf::IsClassDefined(__in DWORD dwClassFourCC)
{
	PTOCX_ITEM pCur	= m_aToc;
	PTOCX_ITEM pEnd	= &m_aToc[m_nTocItems];
	do
	{
		if ((pCur->dwProperty == m_dwPropClsdClass) &&	// OMFI:CLSD:Class
			(pCur->dwDataType == m_dwTypeClass) &&		// omfi:ClassID
			(pCur->cbLength64 == sizeof(DWORD)))
		{
			if (pCur->bStorageMode == SM_IMMEDIATE)
			{
				if (pCur->dwImmediateDword == dwClassFourCC)
				{
					return TRUE;
				}
			}
		}
	} while (++pCur < pEnd);

	return FALSE;
}

//*********************************************************************************************************************
//	Returns TRUE if any member of m_aBlopTable[] belongs to dwClassFourCC.
//	See also CountObjectsOfClass() below. 
//*********************************************************************************************************************
BOOL CReadOmf::HasObjectOfClass(__in DWORD dwClassFourCC)
{
	PBENTO_BLOP	pCur = m_aBlopTable;
	PBENTO_BLOP	pEnd = &m_aBlopTable[m_nBlops];
	do
	{
		if (pCur->dwFourCC == dwClassFourCC)
		{
			return TRUE;
		}
	} while (++pCur < pEnd);

	return FALSE;
}

//*********************************************************************************************************************
//	Returns the number of BENTO_BLOPs whose _dwClassFourCC member matches the dwClassFourCC argument.
//	See also HasObjectOfClass() above.
//*********************************************************************************************************************
ULONG CReadOmf::CountObjectsOfClass(__in DWORD dwClassFourCC)
{
	PBENTO_BLOP	pCur = m_aBlopTable;
	PBENTO_BLOP	pEnd = &m_aBlopTable[m_nBlops];
	ULONG		nCount	= 0;	
	do
	{
		nCount += BOOL(pCur->dwFourCC == dwClassFourCC);
	} while (++pCur < pEnd);

	return nCount;
}

//*********************************************************************************************************************
//	
//*********************************************************************************************************************
DWORD CReadOmf::GetObjectClassFourCC(__in DWORD dwObject)
{
	PBENTO_BLOP	pCur = m_aBlopTable;
	PBENTO_BLOP	pEnd = &m_aBlopTable[m_nBlops];

	do
	{
		if (pCur->dwObject == dwObject)
		{
			return pCur->dwFourCC;
		}
	} while (++pCur < pEnd);

	return 0;
}

//*********************************************************************************************************************
//	Just because an OMF property is defined in the file (via the Bento 'Global Property Name' declaration) it doesn't
//	necessarily mean that any object uses it.
//	So this method answers the question, "Does any object have the specified property?"
//	It scans the TOC - it stops at the first instance of the specified property and returns TRUE.
//	If no match is found it returns FALSE.
//*********************************************************************************************************************
BOOL CReadOmf::HasPropertyInstances(__in DWORD dwProperty)
{
	// Make sure caller passed a valid property ID.
	if ((dwProperty != 0x00000000)&&(dwProperty != 0xFFFFFFFF))
	{
		PTOCX_ITEM pCur	= m_aToc;
		PTOCX_ITEM pEnd	= &m_aToc[m_nTocItems];
		do
		{
			if (pCur->dwProperty == dwProperty)
			{
				return TRUE;
			}
		} while (++pCur < pEnd);
	}
	return FALSE;
}

//*********************************************************************************************************************
//	Just because an OMF data type is defined in the file (via the Bento 'Global Type Name' declaration) it doesn't
//	necessarily mean that any object~property uses it.
//	So this method answers the question, "Does any object have a property with the specified data type?"
//	It scans the TOC - it stops at the first instance of the specified data type and returns TRUE.
//	If no match is found it returns FALSE.
//*********************************************************************************************************************
BOOL CReadOmf::HasDataTypeInstances(__in DWORD dwDataType)
{
	// Make sure caller passed a valid data type ID.
	if ((dwDataType != 0x00000000)&&(dwDataType != 0xFFFFFFFF))
	{
		PTOCX_ITEM pCur	= m_aToc;
		PTOCX_ITEM pEnd	= &m_aToc[m_nTocItems];
		do
		{
			if (pCur->dwDataType == dwDataType)
			{
				return TRUE;
			}
		} while (++pCur < pEnd);
	}
	return FALSE;
}

//*********************************************************************************************************************
//	Protected helper.
//	This is a front end for CReadBento::FindTocIndexForObjectWithProperty().
//	It takes the object ID from a BENTO_BLOP and on exit sets rpTocItem to point to the corresponding TOCX_ITEM.
//	See CReadBento::FindTocIndexForObjectWithProperty().
//*********************************************************************************************************************
HRESULT CReadOmf::FindTocItemForProperty(BENTO_BLOP& rBlop, DWORD dwProperty, PTOCX_ITEM& rpTocItem)
{
	// If this method succeeds then hr is a success code that holds the TOC index.
	HRESULT	hr = FindTocIndexForObjectWithProperty(rBlop.dwObject, dwProperty);
	if (SUCCEEDED(hr))
	{
		rpTocItem = &m_aToc[hr];
		return S_OK;
	}
	else
	{
		rpTocItem = NULL;
		return hr;
	}
}

//*********************************************************************************************************************
//	Protected helper.
//	This is a front end for CReadBento::FindTocIndexForObjectWithPropertyAndType().
//	It takes the object ID from a BENTO_BLOP and on exit sets rpTocItem to point to the corresponding TOCX_ITEM.
//	See CReadBento::FindTocIndexForObjectWithPropertyAndType().
//*********************************************************************************************************************
HRESULT CReadOmf::FindTocItemForPropertyAndType(BENTO_BLOP& rBlop, DWORD dwProperty,
															DWORD dwDataType, PTOCX_ITEM& rpTocItem)
{
	// If this method succeeds then hr is a success code that holds the TOC index.
	HRESULT	hr = FindTocIndexForObjectWithPropertyAndType(rBlop.dwObject, dwProperty, dwDataType);
	if (SUCCEEDED(hr))
	{
		rpTocItem = &m_aToc[hr];
		return S_OK;
	}
	else
	{
		rpTocItem = NULL;
		return hr;
	}
}

//*********************************************************************************************************************
//	Protected helper.
//	This is a front end for CReadBento::FindTocIndexForObjectWithType().
//	It takes the object ID from a BENTO_BLOP and on exit sets rpTocItem to point to the corresponding TOCX_ITEM.
//	See CReadBento::FindTocIndexForObjectWithType().
//*********************************************************************************************************************
HRESULT CReadOmf::FindTocItemForType(BENTO_BLOP& rBlop, DWORD dwDataType, PTOCX_ITEM& rpTocItem)
{
	// If this method succeeds then hr is a success code that holds the TOC index.
	HRESULT	hr = FindTocIndexForObjectWithType(rBlop.dwObject, dwDataType);
	if (SUCCEEDED(hr))
	{
		rpTocItem = &m_aToc[hr];
		return S_OK;
	}
	else
	{
		rpTocItem = NULL;
		return hr;
	}
}
