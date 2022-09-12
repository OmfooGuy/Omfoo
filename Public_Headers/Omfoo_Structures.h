// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII.
// Original filename: Omfoo_Structures.h
// Copyright (C) 2022 David Miller, all rights reserved.
// This file is part of the Omfoo API. You should have received a
// copy of the Omfoo end-user license agreement (EULA) along with it.
// Please see OMFOO_EULA.TXT or send inquiries to OmfooGuy@gmail.com.
// ---------------------------------------------------------------------------
#ifndef __OMFOO_STRUCTURES_H__
#define __OMFOO_STRUCTURES_H__

//	OMF STRUCTURES
//	See the OMF Interchange Specification Version 2.1. Appendix B, "Data Types", beginning on page 219.
#pragma pack(push, 1)		// pack structure members to byte boundaries

//	omfi:UID
typedef struct {
	DWORD	dwPrefix;		// sometimes this is a 32-bit company-specific ID issued by the Avid Developer's Desk.
	DWORD	dwMajor;		// sometimes this is the time of creation, as measured in seconds since January 1, 1970.
	DWORD	dwMinor;		// sometimes this is a value returned by GetTickCount().
} OMF_MOB_ID, *POMF_MOB_ID;

//	omfi:Rational, omfi:ExactEditRate
typedef struct {
	LONG	nNumerator;		// signed
	LONG	nDenominator;	// signed
} OMF_RATIONAL, *POMF_RATIONAL;

//	omfi:data:Point
typedef struct {
	OMF_RATIONAL	x;
	OMF_RATIONAL	y;
} OMF_POINT, *POMF_POINT;

//	omfi:ProductVersion
typedef struct {
	WORD	wMajor;
	WORD	wMinor;
	WORD	wTertiary;
	WORD	wPatch;
	WORD	wDetail;
} OMF_PRODUCT_VERSION, *POMF_PRODUCT_VERSION;

//	omfi:VersionType
typedef struct {
	BYTE	bMajor;		// major version
	BYTE	bMinor;		// minor revision
} OMF_VERSIONTYPE, *POMF_VERSIONTYPE;

//	omfi:TimeStamp
typedef struct {
	DWORD		dwTimeValue;	// seconds since January 1, 1970
	BOOLEAN		fIsGMT;			// TRUE if dwTimeValue is based on Greenwich Mean Time (GMT)
} OMF_TIMESTAMP, *POMF_TIMESTAMP;

//	omfi:DataValue
//	omfi:VarLenBytes
typedef struct {
	INT64	cbOffset;	// if cbOffset is positive then this is the physical file position where the payload begins.
						// if cbOffset is negative then the payload is stored immediately in its lower four bytes.
	UINT64	cbLength;	// The physical length of the payload as measured in bytes.
} OMF_DATA_VALUE, *POMF_DATA_VALUE;

//	omfi:CompCodeArray
//	This "array" has a fixed-length of eight CHARs.
//	Any unused CHARs (at the end) are padded with zeros.
typedef CHAR OMF_COMP_CODE_ARRAY[8], *POMF_COMP_CODE_ARRAY;

//	omfi:CompSizeArray
//	This "array" has a fixed-length of eight BYTEs.
//	Any unused BYTEs (at the end) are padded with zeros.
typedef BYTE OMF_COMP_SIZE_ARRAY[8], *POMF_COMP_SIZE_ARRAY;

#pragma pack(pop)		// restore compiler's previous structure alignment settings

#endif	//  __OMFOO_STRUCTURES_H__
