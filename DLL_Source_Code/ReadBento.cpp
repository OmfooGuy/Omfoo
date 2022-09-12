// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: ReadBento.cpp
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo Source Code Project.
// You should have received a copy of the source code license with this file.
// Please see OMFOO_SOURCECODE_LICENSE.TXT or send inquiries to OmfooGuy@gmail.com.
//*********************************************************************************************************************
#include "stdafx.h"
#include <shlwapi.h>
#include "ReadBento.h"
#include "Omfoo_Alpha_Header.h"
#include "DllMain.h"

//	CReadBento is a C++ class for reading the Bento layer of OMF files.
//	It has been tested with Bento version 1.0d4 (as implemented in OMF 1.0 through 1.4 inclusive)
//	and with Bento version 1.0d5 (as implemented in OMF 1.5, 2.0, and 2.1).

//	This is maximum number of TOC items that this implementation of CReadBento can possibly handle.
//	We need this number because we expand the entire TOC into contiguous memory. We call VirtualAlloc()
//	to allocate that memory - which it does on 64KB boundaries (even though the page granularity is 4KB).
//	In most real-life situations we will _safely_ run out of memory long before we reach here.
//	This liberally/naively assumes that an entire contiguous 4GB is available.
//	Note that the sizeof(TOCX_ITEM) is 32 bytes.
//
//	  0x100000000	(4294967296) four gibibytes
//	- 0x000010000	(65536)
//	----------------------------
//	  0xFFFF0000	(4294901760) maxumum amount of memory that VirtualAlloc() can ever possibly allocate. 
//	/ 0x00000020	(32)		 sizeof(TOCX_ITEM)
//	----------------------------
//	  0x07FFF800	(134215680)
//
#define BENTO_MAX_TOC_ITEMS	((0x100000000-65536)/sizeof(TOCX_ITEM))


//*********************************************************************************************************************
//	Constructor
//	WARNING: This class is not meant to be instantiated on the stack!
//	It assumes that its C++ operator new() has already zeroed all of its memory!
//*********************************************************************************************************************
CReadBento::CReadBento(void)
{
	//SIZE_T cbTocItem = sizeof(TOCX_ITEM);
	//if (cbTocItem != 32)
	//{
	//	__debugbreak();
	//}
	//else
	//{
	//	__debugbreak();
	//}
}

//*********************************************************************************************************************
//	Destructor
//*********************************************************************************************************************
CReadBento::~CReadBento()
{
	// Free these arrays in the reverse order from which they were allocated. Null pointers are okay.
	MemFree(m_aDataTypeNames);
	MemFree(m_aPropertyNames);
	VirtualFree(m_aToc, SIZE_T(0), MEM_RELEASE);
}

//*********************************************************************************************************************
//	This is our main initialization routine. Call it once per lifetime.
//*********************************************************************************************************************
HRESULT CReadBento::OpenBentoFile(PCWSTR pwzFileName)
{
	CHECK(OpenReadableFile(pwzFileName));
	CHECK(ReadBentoLabel());
	CHECK(ReadBentoToc());
	CHECK(VerifyToc());
	CHECK(BuildPropertyNameTable());
	CHECK(BuildDataTypeNameTable());
	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenBentoFile().
//	Bento files are not identified by a 'header', they are identified by a 'footer'.
//	They are identified by a BENTO_STANDARD_LABEL structure that lives in the last 24 bytes of the file.
//	The BENTO_STANDARD_LABEL structure is also known as a BENTO_STANDARD_FOOTER.
//	For official info on the BENTO_STANDARD_LABEL see the Bento Specification 1.0d5, chapter 7, "Format Definition"
//	under the subheading "Container Label Format", beginning on page 61.
//
//	This method does the following:
//	1) Reads the last 48 bytes of the file into our m_extLabel and m_stdLabel structures, which are contiguous.
//	2) Verifies the magic byte sequence in m_stdLabel.
//	3) Detects the Bento byte order and initializes m_fBentoBigEndian to TRUE or FALSE.
//	4) Tests for the presence of a BENTO_EXTENDED_LABEL in m_extLabel.
//	5) If an extended label existes then sets m_fBentoHasExtraLabel to TRUE.
//	5) Byteswaps all members of both structures (but not the magic bytes because they're technically a byte array).
//*********************************************************************************************************************
HRESULT CReadBento::ReadBentoLabel(void)
{
	UINT64	cbLabelOffset64	= 0;
	UINT64	cbTocOffset64	= 0;
	UINT64	cbTocSize64		= 0;

	// If the file is smaller than 256 bytes then fail.
	// Note that we test m_cbVirtualEndOfFile64 (and not m_cbPhysicalEndOfFile64) so
	// that our code can read Bento/OMF files that are embedded inside other, outer, larger containers.
	if (m_cbVirtualEndOfFile64 < 256)
	{
		BREAK_IF_DEBUG
		return BENTO_E_FILE_TOO_SMALL;
	}

	// Calculate the offset to the Bento extended label structure (assuming one exists).
	cbLabelOffset64 = m_cbVirtualEndOfFile64 - sizeof(BENTO_EXTENDED_FOOTER);	// 24 + 24 = 48 bytes

	// Seek to 48 bytes before the end of the file and read those 48 bytes into our BENTO_EXTENDED_FOOTER structure.
	CHECK(SeekRead(cbLabelOffset64, PBENTO_EXTENDED_FOOTER(this), sizeof(BENTO_EXTENDED_FOOTER)));

	// Test for the presence of the standard Bento eight-byte magic byte sequence.
	// The Bento spec describes it as an array of eight bytes, but we can treat it as a 64-bit QWORD.
	if (m_stdLabel.qwMagicBytes != 0xD7726448A54D43A4)
	{
		BREAK_IF_DEBUG
		return BENTO_E_BAD_SIGNATURE;
	}

	// Just for curiosity.
	// Just while debugging.
	// Is m_stdLabel.wFlags ever anything other than 0x0000 or 0x0101?
	#ifdef _DEBUG
	if ((m_stdLabel.wFlags != 0x0000) && (m_stdLabel.wFlags != 0x0101))
	{
		__debugbreak();
	}
	#endif

	// In Bento 1.0d4 the label's wEncoding member was the official way for determining the byte order.
	//
	//		if (m_stdLabel.wEncoding == 'II')	{...}	// little endian
	//		if (m_stdLabel.wEncoding == 'MM')	{...}	// big endian
	//
	// However in Bento 1.0d5 the wEncoding member was replaced with the wBlockSize member.

	// Note that the version number refers to the footer/label version, and NOT to the actual Bento version.
	// We can always easily determine the Bento version (and it's byte order) by testing wMajorVersion.
	// The only valid values are 0x0001, 0x0100, 0x0002, and 0x0200.
	if ((m_stdLabel.wMajorVersion == 0x0001) || (m_stdLabel.wMajorVersion == 0x0002))
	{
		m_fBentoBigEndian = FALSE;
	}
	else if ((m_stdLabel.wMajorVersion == 0x0100) || (m_stdLabel.wMajorVersion == 0x0200))
	{
		m_fBentoBigEndian			= TRUE;
		m_stdLabel.wFlags			= Endian16(m_stdLabel.wFlags);
		m_stdLabel.wBlockSize		= Endian16(m_stdLabel.wBlockSize);
		m_stdLabel.wMajorVersion	= Endian16(m_stdLabel.wMajorVersion);
		m_stdLabel.wMinorVersion	= Endian16(m_stdLabel.wMinorVersion);
		m_stdLabel.cbTocOffset32	= Endian32(m_stdLabel.cbTocOffset32);
		m_stdLabel.cbTocSize32		= Endian32(m_stdLabel.cbTocSize32);
	}
	else
	{
		BREAK_IF_DEBUG
		return BENTO_E_BAD_MAJOR_VERSION;
	}

	// Valid minor versions are zero and one.
	if (m_stdLabel.wMinorVersion > 1)
	{
		BREAK_IF_DEBUG
		return BENTO_E_BAD_MINOR_VERSION;
	}

	// Get the TOC offset and TOC size from our BENTO_STANDARD_LABEL structure and promote them to 64 bits.
	cbTocOffset64	= m_stdLabel.cbTocOffset32;
	cbTocSize64		= m_stdLabel.cbTocSize32;

	// Test for the presence of a BENTO_EXTRA_LABEL structure immediately before the BENTO_STANDARD_LABEL.
	// All three conditions must be TRUE.
	if ((m_stdLabel.cbTocSize32 == 0) &&
		(m_stdLabel.wMinorVersion == 1) &&
		(m_extLabel.qwExtraMagicBytes == 0xD7726448A54D43A5))
	{
		m_fBentoHasExtraLabel = TRUE;
		cbLabelOffset64 = m_cbVirtualEndOfFile64 - sizeof(BENTO_EXTENDED_FOOTER);

		// Replace/update cbTocOffset64 and cbTocSize64 with their 64-bit counterparts.
		// Get them from the BENTO_EXTRA_LABEL structure.
		if (m_fBentoBigEndian)
		{
			cbTocOffset64	= Endian64(m_extLabel.cbTocOffset64);
			cbTocSize64		= Endian64(m_extLabel.cbTocSize64);
		}
		else
		{
			cbTocOffset64	= m_extLabel.cbTocOffset64;
			cbTocSize64		= m_extLabel.cbTocSize64;
		}
	}
	else
	{
		// Fix cbLabelOffset64 because now we know that there is no BENTO_EXTRA_LABEL.
		cbLabelOffset64 = m_cbVirtualEndOfFile64 - sizeof(BENTO_STANDARD_LABEL);

		// Fill m_extLabel with 'ones' to remind ourself that it's invalid and doesn't exist.
		m_extLabel.qwExtraMagicBytes	= -1;
		m_extLabel.cbTocOffset64		= -1;
		m_extLabel.cbTocSize64			= -1;
	}

	// If the TOC is smaller than 96 bytes then something is seriously messed up.
	if (cbTocSize64 < 96)
	{
		BREAK_IF_DEBUG
		return BENTO_E_TOC_TOO_SMALL;
	}

	// Our current implementation reads the entire TOC into memory.
	// We do not suppport TOCs greater than or equal to one gibibyte.
 	if (cbTocSize64 > 0x3FFFFFFFF)
	{
		BREAK_IF_DEBUG
		return BENTO_E_TOC_TOO_HUGE;
	}

	// Make sure that the TOC does not purport to be so large that it overlaps and clobbers onto the Bento label(s).
	if ((cbTocOffset64 >= cbLabelOffset64) ||
		(cbTocSize64 > cbLabelOffset64) ||
		((cbTocOffset64 + cbTocSize64) > cbLabelOffset64))
	{
		BREAK_IF_DEBUG
		return BENTO_E_TOC_OVERLAP;
	}

	// All is good. Save these.
	m_cbLabelOffset64	= cbLabelOffset64;
	m_cbTocOffset64		= cbTocOffset64;
	m_cbTocSize32		= UINT32(cbTocSize64);	// safe to truncate because we know cbTocSize64 is less than one gig.

	// Done!
	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenBentoFile().
//	Allocates and populates our m_aToc[] member - which is an array of TOCX_ITEM structures.
//*********************************************************************************************************************
HRESULT CReadBento::ReadBentoToc()
{
	// Fork to the appropriate routine.
	if (m_stdLabel.wMajorVersion == 0x0001)
	{
		// If the label's major version is 0x0001 then this is a Bento revision 1.0d4 file.
		// Each TOC entry is exactly 24 bytes.
		return ReadV1Toc();
	}

	if (m_stdLabel.wMajorVersion == 0x0002)
	{
		// If the label's major version is 0x0002 then this is a Bento revision 1.0d5 file.
		// Each TOC entry is variable-length compressed.
		return ReadV2Toc();
	}

	// We don't recognize the Bento version!
	// But wait! Something is horribly wrong! We already validated this in ReadBentoLabel()!
	// So how did we even get here?
	BREAK_IF_DEBUG
	return OMFOO_E_ASSERTION_FAILURE;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during ReadBentoToc().
//	In Bento version 1.0d4 the TOC is stored on disk as an array of fixed-length 24-byte TOC1_ITEM structures.
//	This routine converts those TOC1_ITEM structures into 32-byte TOCX_ITEM structures.
//	The conversion is done in-place using one big chunk of memory.
//	On exit that array is stored as our m_aToc[] member, and the number of items is stored in our m_nTocItems member.
//	The array persists throughout our lifetime and the memory is finally released in our destructor.
//*********************************************************************************************************************
HRESULT CReadBento::ReadV1Toc()
{
	PTOC1_ITEM	pSrc	= NULL;
	PTOCX_ITEM	pDst	= NULL;
	PBYTE		pMem	= NULL;
	ULONG		nItems	= 0;
	UINT32		cbAlloc	= 0;
	HRESULT		hr		= E_UNEXPECTED;

	// Calculate the number of items in the TOC.
	nItems = m_cbTocSize32 / sizeof(TOC1_ITEM);

	// Reality check.
	// m_cbTocSize32 must be evenly divisible by sizeof(TOC1_ITEM).
	if ((nItems * sizeof(TOC1_ITEM)) != m_cbTocSize32)
	{
		BREAK_IF_DEBUG
		hr = BENTO_E_TOC_104_EXTRA_BYTES;
		goto L_CleanUpExit;
	}

	// Make sure nItems isn't huge.
	if (nItems > BENTO_MAX_TOC_ITEMS)
	{
		BREAK_IF_DEBUG
		hr = BENTO_E_TOO_MANY_ITEMS;
		goto L_CleanUpExit;
	}

	// Allocate permanent memory for the internal TOC.
	cbAlloc = nItems * sizeof(TOCX_ITEM);

	// Include 24 extra bytes of slack-space for Bento v1.04.
	cbAlloc	+= sizeof(TOC1_ITEM);

	// Note that memory is automatically zeroed.
	pMem = PBYTE(VirtualAlloc(NULL, cbAlloc, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
	if (NULL == pMem)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_CleanUpExit;
	}

	// Save the pointer for use elsewhere.
	m_aToc	= PTOCX_ITEM(pMem);

	// Initialize the destination pointer.
	pDst	= PTOCX_ITEM(pMem);

	// Initialize the source pointer.
	pSrc	= PTOC1_ITEM(&pMem[cbAlloc-m_cbTocSize32]);

	// Read in the file's TOC verbatim - exactly as it is stored on disk.
	if (FAILED(hr = SeekRead(m_cbTocOffset64, pSrc, m_cbTocSize32)))
	{
		BREAK_IF_DEBUG
		goto L_CleanUpExit;
	}

	// These loops convert each TOC1_ITEM structure into a TOCX_ITEM structure.
	if (m_fBentoBigEndian)
	{
		// Use this loop if the byte order is big endian. (Motorola)
		for (UINT iCvt = 0; iCvt < nItems; iCvt++)
		{
			// Wipe the entire destination structure.
			pDst->cbOffset64	= 0;
			pDst->cbLength64	= 0;

			// Copy the object ID, property ID, data type ID, generation, and byteswap as we go.
			pDst->dwObject		= Endian32(pSrc->dwObject);
			pDst->dwProperty	= Endian32(pSrc->dwProperty);
			pDst->dwDataType	= Endian32(pSrc->dwDataType);
			pDst->wGeneration	= Endian16(pSrc->wGeneration);

			// Initialize the fContinued flag to TRUE or FALSE.
			// Bit 9 is set if the data is continued.
			pDst->fContinued	= BOOLEAN((pSrc->wFlags & 0x0200) != 0);

			// Is the data stored immediately?
			// Bit 8 is set if the data is stored immediately.
			if (pSrc->wFlags & 0x0100)
			{
				// Initialize the storage mode.
				pDst->bStorageMode = SM_IMMEDIATE;

				// Copy the four data bytes.
				// We do not byteswap the data because we don't know what it is.
				// Traditionally we would use memcpy() to do this, but instead we treat the data as a DWORD and move
				// it four bytes at a time.
				pDst->dwImmediateDword = pSrc->dwImmediateDword;

				// Byteswap the 32-bit data length and promote it to 64 bits.
				// Note that the TOC1_ITEM structure can only accommodate immediate data that is 4 bytes long or less.
				// If the structure says that the length is longer (and it could) then the structure must be corrupt.
				// My point is that we may be copying a dangerous value here, but we are going to copy it anyhow.
				// It is the responsibility of our data-reading routines to test this length before we use it and
				// return an error code if it is invalid.
				// Note too, that all of this is just speculative prevention: I have never encountered a malformed file like this.
				pDst->cbLength64 = Endian32(pSrc->cbLength);
			}
			else
			{
				// If the data portion is not 'immediate' then it must be an offset.
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;

				// Copy and convert offset and length.
				pDst->cbOffset64	= Endian32(pSrc->cbOffset);	// promote 32-bit offset to a 64-bit offset
				pDst->cbLength64	= Endian32(pSrc->cbLength);	// promote 32-bit length to a 64-bit length
			}

			++pSrc;		// inc source pointer to the next TOC1_ITEM structure
			++pDst;		// inc destination pointer to the next TOCX_ITEM structure
		}
	}
	else
	{
		// Use this loop if the byte order is little endian. (Intel)
		for (UINT iCvt = 0; iCvt < nItems; iCvt++)
		{
			// Wipe the entire destination structure.
			pDst->cbOffset64	= 0;
			pDst->cbLength64	= 0;

			// Copy the object ID, property ID, data type ID, and generation.
			pDst->dwObject		= pSrc->dwObject;
			pDst->dwProperty	= pSrc->dwProperty;
			pDst->dwDataType	= pSrc->dwDataType;
			pDst->wGeneration	= pSrc->wGeneration;

			// Initialize the fContinued flag to TRUE or FALSE.
			// Bit #1 is set if the data is continued.
			pDst->fContinued	= BOOLEAN((pSrc->wFlags & 0x0002) != 0);

			// Is the data stored immediately?
			// Bit #0 is set if the data is stored immediately.
			if (pSrc->wFlags & 0x0001)
			{
				// Initialize the storage mode.
				pDst->bStorageMode = SM_IMMEDIATE;

				// Copy the four data bytes.
				// We could use memcpy() to do this, but instead we treat it as a DWORD and move four bytes at a time.
				pDst->dwImmediateDword = pSrc->dwImmediateDword;

				// Promote the 32-bit data length to a 64-bit length.
				// Note that the TOC1_ITEM structure can only accommodate immediate data that is 4 bytes long or less.
				// If the structure says that the length is longer (and it could) then the structure must be corrupt.
				// My point is that we may be copying a dangerous value here, but we are going to copy it anyhow.
				// It is the responsibility of our data-reading routines to test this length before we use it and
				// return an error code if it is invalid.
				// Note too, that all of this is just paranoia since I have never encountered a file with this problem.
				pDst->cbLength64	= pSrc->cbLength;
			}
			else
			{
				// If the data portion is not 'immediate' then it must be an offset.
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;

				// Copy and convert offset and length.
				pDst->cbOffset64	= pSrc->cbOffset;	// promote 32-bit offset to a 64-bit offset
				pDst->cbLength64	= pSrc->cbLength;	// promote 32-bit length to a 64-bit length
			}

			++pSrc;		// inc source pointer to the next TOC1_ITEM structure
			++pDst;		// inc destination pointer to the next TOCX_ITEM structure
		}
	}

	// Save item count.
	// This member was originally initialized to zero. We save this last as a precautionary measure,
	// so that if we fail our other code will simply think there are no items.
	m_nTocItems = nItems;

	// Set return code.
	hr = S_OK;

L_CleanUpExit:
	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during ReadBentoToc().
//	In Bento version 1.0d5 the TOC is stored on disk as a compressed, variable-length, stream of bytes.
//	This routine parses that stream and transforms it into an array of fixed-length 32-byte structures we call TOC_ITEMs.
//	The conversion is done using two memory pools.
//	On exit that array is stored as our m_aToc[] member, and the number of items is stored in our m_nTocItems member.
//	The array persists throughout our lifetime, and memory is finally released in our destructor.
//*********************************************************************************************************************
HRESULT CReadBento::ReadV2Toc()
{
	HRESULT	hr	= E_UNEXPECTED;
	PBYTE	pbCompressedToc = NULL;

	// Allocate temporary memory for the compressed TOC.
	// Note that memory is automatically zeroed.
	// We will free it before this method exits.
	pbCompressedToc = PBYTE(VirtualAlloc(NULL, m_cbTocSize32, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
	if (NULL == pbCompressedToc)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_CleanUpExit;
	}

	// Read the entire compressed TOC into the temporary memory.
	if (FAILED(hr = SeekRead(m_cbTocOffset64, pbCompressedToc, m_cbTocSize32)))
	{
		BREAK_IF_DEBUG
		goto L_CleanUpExit;
	}

	// Call helper routine to calculate the nunber of items in the compressed TOC.
	// It saves the result in m_nTocItems.
	if (FAILED(hr = CountV2TocItems(pbCompressedToc)))
	{
		BREAK_IF_DEBUG
		goto L_CleanUpExit;
	}

	// If CountV2TocItems() succeeded then we can be sure that the 'stream element byte codes' are all valid,
	// and that all of the bytes in the compressed byte stream are accounted for.
	// Now that we know how many items there are we can calculate memory requirements for the expanded TOC.
	UINT32 cbExpandedToc = m_nTocItems * sizeof(TOCX_ITEM);

	// Make sure the resulting size doesn't exceed m_cbMaxTocMem.
	//if (cbExpandedToc > m_cbMaxTocMem)
	//{
	//	BREAK_IF_DEBUG
	//	hr = BENTO_E_BAD_TOC;
	//	goto L_CleanUpExit;
	//}

	// Allocate permanent memory for the expanded TOC.
	// Note that memory is automatically zeroed.
	m_aToc = PEXPANDED_TOC(VirtualAlloc(NULL, cbExpandedToc, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
	if (NULL == m_aToc)
	{
		BREAK_IF_DEBUG
		hr = E_OUTOFMEMORY;
		goto L_CleanUpExit;
	}

	// We detected the endianess in ReadBentoLabel().
	if (m_fBentoBigEndian)
	{
		hr = ExpandV2TocBE(pbCompressedToc);
	}
	else
	{
		hr = ExpandV2TocLE(pbCompressedToc);
	}

L_CleanUpExit:

	if (pbCompressedToc)
	{
		// Release the temporary memory that we used to hold the compressed TOC. A NULL pointer is ok.
		VirtualFree(pbCompressedToc, SIZE_T(0), MEM_RELEASE);
		pbCompressedToc = NULL;
	}

	return hr;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during ReadV2Toc().
//	Calculate the nunber of items in pbCompressedToc.
//	Save the result in m_nTocItems.
//	Return S_OK, BENTO_E_BAD_BYTE_CODE, or BENTO_E_RUNAWAY_TOC.
//*********************************************************************************************************************
HRESULT CReadBento::CountV2TocItems(PBYTE pbCompressedToc)
{
	// Convert m_stdLabel.wBlockSize into an actual byte count.
	UINT32	cbBlockSize	= UINT32(m_stdLabel.wBlockSize) << 10;
	UINT32	iCurrentBytePos	= 0;

	// 
	for (;;)
	{
		// Fetch the next stream element byte code. Do NOT post-increment the position index.
		BYTE bTocElementByteCode = pbCompressedToc[iCurrentBytePos];

		// First we detect the special case for the no-op/filler/skip code (0xFF).
		// This is mentioned in "Chapter 7 : Format Definition" of the Bento Spec at page 65.
		// The idea is that you can 'remove' and item without having to completely rebuild the TOC.
		// All you have to do is fill it with 0xFFs.
		if (bTocElementByteCode == eNOP)
		{
			// WARNING! THIS CODE IS UNTESTED!
			// I DON’T HAVE ANY TEST FILES WITH THIS FEATURE, AND I'M NOT SURE SUCH FILES EVEN EXIST!
			BREAK_IF_DEBUG

			// Calculate the next byte position.
			iCurrentBytePos++;

			// Is iCurrentBytePos less than m_cbTocSize?
			if (iCurrentBytePos < m_cbTocSize32)
			{
				// More bytes are available for processing.
				// Continue looping and advance to the next byte code.
				continue;
			}

			// We are done processing bytes in this for(;;) loop!
			// This is a normal/expected way of exiting this loop.
			break;
		}

		// Verify that bTocElementByteCode is valid code.
		if ((bTocElementByteCode == 0) || (bTocElementByteCode > 26))
		{
			BREAK_IF_DEBUG
			return BENTO_E_BAD_BYTE_CODE;
		}

		// Have we reached the end of the current block?
		if (bTocElementByteCode == eEndOfBuffer)
		{
			// Calculate the next byte position.
			iCurrentBytePos = ((iCurrentBytePos/cbBlockSize)+1) * cbBlockSize;

			// Is iCurrentBytePos less than m_cbTocSize?
			if (iCurrentBytePos < m_cbTocSize32)
			{
				// More bytes are available for processing.
				// Continue looping and advance to the beginning of the next block.
				continue;
			}

			// We are done processing bytes in this for(;;) loop!
			// This is a normal/expected way of exiting this loop.
			break;
		}

		// Increment the item counter if bTocElementByteCode represents the 'value' portion.
		if (bTocElementByteCode > eExplicitGen)
		{
			++m_nTocItems;

			// Make sure m_nTocItems isn't huge.
			// BENTO_MAX_TOC_ITEMS = 67108862
			// Anything greater than this will result in a to call VirtualAlloc() for more than 0x7FFFFFC0 bytes.
			if (m_nTocItems > BENTO_MAX_TOC_ITEMS)
			{
				BREAK_IF_DEBUG
				return BENTO_E_TOO_MANY_ITEMS;
			}
		}

		// Calculate the next byte position.
		// Use our lookup table to get a corresponding byte count for this code.
		iCurrentBytePos = 1 + iCurrentBytePos + m_aStreamElementByteCountTable[bTocElementByteCode];

		// Are more bytes available for processing?
		if (iCurrentBytePos < m_cbTocSize32)
		{
			// Yes! Go back to the top of the loop.
			continue;
		}

		// No more bytes are available.
		// Is iCurrentBytePos greater than m_cbTocSize?
		if (iCurrentBytePos > m_cbTocSize32)
		{
			// WTF? Something went wrong.
			// iCurrentBytePos should match m_cbTocSize32.
			// Did we miscalculate an item size?
			BREAK_IF_DEBUG
			return BENTO_E_RUNAWAY_TOC;
		}

		// Success!
		// iCurrentBytePos is an exact match for m_cbTocSize!
		// This is a normal/expected way of exiting this loop.
		break;
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during ReadV2Toc().
//	This is the big-endian version of ExpandV2TocLE().
//	pbCompressedToc is our only argument (because it's temporary),
//	but we also access m_aToc, m_stdLabel, and m_cbTocSize32.
//*********************************************************************************************************************
HRESULT CReadBento::ExpandV2TocBE(PBYTE pbCompressedToc)
{
	// Temporary structure for extracting TOC_ITEMs.
	// It is essentially the top half of our TOCX_ITEM structure.
	#pragma pack(push, 4)	// align structure members to DWORD boundaries
	struct {
		DWORD	dwObject;
		DWORD	dwProperty;
		DWORD	dwDataType;
		WORD	wGeneration;
		WORD	wUnused;
	} wip = {0};
	#pragma pack(pop)		// restore compiler's previous settings

	PTOCX_ITEM	pDst		= m_aToc;
	UINT32	cbBlockSize		= UINT32(m_stdLabel.wBlockSize) << 10;
	UINT32	iCurrentBytePos	= 0;

	// Use this loop if the byte order is big endian. (Motorola)
	while (iCurrentBytePos < m_cbTocSize32)
	{
		// Get the next compression token. We already know that it is within range.
		BYTE bTocElementByteCode = pbCompressedToc[iCurrentBytePos];

		// Is this a filler? (a no-op?)
		if (bTocElementByteCode == eNOP)
		{
			iCurrentBytePos++;
			continue;
		}

		// Have we reached the end of the block?
		if (bTocElementByteCode == eEndOfBuffer)
		{
			// Calculate the next byte position.
			iCurrentBytePos = ((iCurrentBytePos/cbBlockSize)+1) * cbBlockSize;
			continue;
		}

		// Increment the current byte position; move it past the token.
		++iCurrentBytePos;

		// Calculate source address and cast it as a pointer to a DWORD.
		// We cast it as a pointer to a DWORD because we tend to move four bytes at a time.
		PDWORD pdw = PDWORD(PBYTE(&pbCompressedToc[iCurrentBytePos]));

		// Use our lookup table to get a corresponding byte count for this token.
		UINT32 cbCurrentEntry = m_aStreamElementByteCountTable[bTocElementByteCode];

		switch (bTocElementByteCode)
		{
		case eNewObject:	// object + property + type
			wip.dwObject	= Endian32(pdw[0]);
			wip.dwProperty	= Endian32(pdw[1]);
			wip.dwDataType	= Endian32(pdw[2]);
			wip.wGeneration	= 0;
			break;

		case eNewProperty:	// property + type
			wip.dwProperty	= Endian32(pdw[0]);
			wip.dwDataType	= Endian32(pdw[1]);
			wip.wGeneration	= 0;
			break;

		case eNewType:		// type
			wip.dwDataType	= Endian32(pdw[0]);
			wip.wGeneration	= 0;
			break;

		case eExplicitGen:	// 32-bit generation
			{
				DWORD dwGeneration	= Endian32(pdw[0]);
				wip.wGeneration	= (dwGeneration > 0x0000FFFF) ? 0xFFFF : WORD(dwGeneration);
			}
			break;

		case eContdImmediate4:
			// Initialize the storage mode.
			pDst->bStorageMode	= SM_IMMEDIATE;

			// Set the continued flag.
			pDst->fContinued	= TRUE;

			// Copy object + property + type + generation from temporary structure.
			// Note that these values are already byteswapped.
			pDst->dwObject		= wip.dwObject;
			pDst->dwProperty	= wip.dwProperty;	
			pDst->dwDataType	= wip.dwDataType;
			pDst->wGeneration	= wip.wGeneration;

			// Move the data as an opaque DWORD four bytes at a time.
			// Note that we do not byteswap the data because we don't know what the data is.
			pDst->dwImmediateDword = pdw[0];

			// Initialize the length, which in this case is always a constant.
			pDst->cbLength64 = sizeof(DWORD);

			// Increment destination pointer to next TOCX_ITEM structure in m_aToc.
			++pDst;

			// Done with eContdImmediate4.
			break;

		case eReferenceListID:
			// Initialize the storage mode.
			pDst->bStorageMode = SM_REFLISTID;

			// Clear the continued flag.
			pDst->fContinued = FALSE;

			// Copy object + property + type + generation from temporary structure.
			// Note that these values are already byteswapped.
			pDst->dwObject		= wip.dwObject;
			pDst->dwProperty	= wip.dwProperty;	
			pDst->dwDataType	= wip.dwDataType;
			pDst->wGeneration	= wip.wGeneration;

			// Copy the reference list object ID and byteswap as we go.
			pDst->dwRefList = Endian32(pdw[0]);

			// Initialize the length to zero because the reference list object ID is not data.
			pDst->cbLength64 = 0;

			// Increment destination pointer to next TOCX_ITEM structure in m_aToc.
			++pDst;

			// Done with eReferenceListID.
			break;

		default:
			// Copy object + property + type + generation from temporary structure.
			// Note that these values are already byteswapped.
			pDst->dwObject		= wip.dwObject;
			pDst->dwProperty	= wip.dwProperty;	
			pDst->dwDataType	= wip.dwDataType;
			pDst->wGeneration	= wip.wGeneration;

			// Convert bTocElementByteCode (1~26 inclusive) into a bit mask.
			DWORD dwCurrentByteCodeAsBitMask = 1 << bTocElementByteCode;

			// Initialize the 'continued' flag to TRUE or FALSE depending on the storage code.
			pDst->fContinued = (dwCurrentByteCodeAsBitMask & dwElemBitMaskContinued) != 0;

			// Does the storage code belong to the "immediate but not continued" group?
			// This group includes eImmediate0, eImmediate1, eImmediate2, eImmediate3, and eImmediate4.
			if (dwCurrentByteCodeAsBitMask & dwElemBitMaskImmNotCont)
			{
				// Initialize the storage mode.
				pDst->bStorageMode = SM_IMMEDIATE;

				// Move the data as an opaque DWORD four bytes at a time.
				// This is safe to do because the compressed bytestream always reserves four bytes
				// for immediate data even when the bytecount is less than that.
				// Note that we do not byteswap the data because we don't know what that data is (yet).
				pDst->dwImmediateDword = pdw[0];

				// Calculate the bytecount and save it.
				// We can use simple subtraction because eImmediate0 <= bTocElementByteCode <= eImmediate4.
				pDst->cbLength64 = bTocElementByteCode - eImmediate0;
			}

			// Does the storage code belong to the "32-bit offset and 32-bit length" group?
			// This group includes eOffset4Len4 and eContdOffset4Len4.
			else if (dwCurrentByteCodeAsBitMask & dwElemBitMaskOffset4x4)
			{
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;	

				// Byteswap the 32-bit offset and promote it to a 64-bit offset.
				pDst->cbOffset64	= Endian32(pdw[0]);

				// Byteswap the 32-bit length and promote it to a 64-bit length.
				pDst->cbLength64	= Endian32(pdw[1]);
			}

			// Does the storage code belong to the "64-bit offset and 32-bit length" group?
			// This group includes eOffset8Len4 and eContdOffset8Len4.
			else if (dwCurrentByteCodeAsBitMask & dwElemBitMaskOffset8x4)
			{
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;	

				// Copy the 64-bit offset and byteswap as we go.
				// Note that we cast pdw to a PUINT64.
				pDst->cbOffset64	= Endian64(PUINT64(pdw)[0]);

				// Byteswap the 32-bit length and promote it to a 64-bit length.
				pDst->cbLength64	= Endian32(pdw[2]);
			}

			// Does the storage code belong to the "64-bit offset and 64-bit length" group?
			// This group includes eOffset8Len8 and eContdOffset8Len8.
			else if (dwCurrentByteCodeAsBitMask & dwElemBitMaskOffset8x8)
			{
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;	

				// Copy the 64-bit offset and byteswap as we go.
				// Note that we cast pdw to a PUINT64.
				pDst->cbOffset64	= Endian64(PUINT64(pdw)[0]);

				// Copy the 64-bit length and byteswap as we go.
				// Note that we cast pdw to a PUINT64.
				pDst->cbLength64	= Endian64(PUINT64(pdw)[1]);
			}

			else
			{
				// WTF?
				// Something went wrong.
				// Why are we here?
				// Invalid compression token!
				// What is the value of bTocElementByteCode?
				BREAK_IF_DEBUG
			}

			// Increment destination pointer to next TOCX_ITEM structure in m_aToc.
			++pDst;
			break;
		}

		// Calculate the next byte position.
		iCurrentBytePos += cbCurrentEntry;
	}

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during ReadV2Toc().
//	This is the little-endian version of ExpandV2TocBE().
//	pbCompressedToc is our only argument (because it's temporary),
//	but we also access m_aToc, m_stdLabel, and m_cbTocSize32.
//*********************************************************************************************************************
HRESULT CReadBento::ExpandV2TocLE(PBYTE pbCompressedToc)
{
	// Temporary structure for extracting TOC_ITEMs.
	// It is essentially the top half of our TOCX_ITEM structure.
	#pragma pack(push, 4)	// align structure members to DWORD boundaries
	struct {
		DWORD	dwObject;
		DWORD	dwProperty;
		DWORD	dwDataType;
		WORD	wGeneration;
		WORD	wUnused;
	} wip = {0};
	#pragma pack(pop)		// restore compiler's previous settings

	PTOCX_ITEM	pDst		= m_aToc;
	UINT32	cbBlockSize		= UINT32(m_stdLabel.wBlockSize) << 10;
	UINT32	iCurrentBytePos	= 0;

	// Use this loop if the byte order is little endian. (Intel)
	while (iCurrentBytePos < m_cbTocSize32)
	{
		// Get the next compression token. We already know that it is within range.
		// Note that the token is a BYTE but we promote it to a DWORD.
		BYTE bTocElementByteCode = pbCompressedToc[iCurrentBytePos];

		// Is this a filler? (a no-op?)
		if (bTocElementByteCode == eNOP)
		{
			iCurrentBytePos++;
			continue;
		}

		// Have we reached the end of the block?
		if (bTocElementByteCode == eEndOfBuffer)
		{
			// Calculate the next byte position.
			iCurrentBytePos = ((iCurrentBytePos/cbBlockSize)+1) * cbBlockSize;
			continue;
		}

		// Increment the current byte position; move it past the token.
		++iCurrentBytePos;

		// Calculate source address and cast it as a pointer to a DWORD.
		// We cast it as a pointer to a DWORD because we tend to move four bytes at a time.
		PDWORD pdw = PDWORD(PBYTE(&pbCompressedToc[iCurrentBytePos]));

		// Use our lookup table to get a corresponding byte count for this token.
		UINT32 cbCurrentEntry = m_aStreamElementByteCountTable[bTocElementByteCode];

		switch (bTocElementByteCode)
		{
		case eNewObject:	// object + property + type
			wip.dwObject	= pdw[0];
			wip.dwProperty	= pdw[1];
			wip.dwDataType	= pdw[2];
			wip.wGeneration	= 0;
			break;

		case eNewProperty:	// property + type
			wip.dwProperty	= pdw[0];
			wip.dwDataType	= pdw[1];
			wip.wGeneration	= 0;
			break;

		case eNewType:		// type
			wip.dwDataType	= pdw[0];
			wip.wGeneration	= 0;
			break;

		case eExplicitGen:	// 32-bit generation
			{
				DWORD dwGeneration	= pdw[0];
				wip.wGeneration		= (dwGeneration > 0x0000FFFF) ? 0xFFFF : WORD(dwGeneration);
			}
			break;

		case eContdImmediate4:
			// Initialize the storage mode.
			pDst->bStorageMode	= SM_IMMEDIATE;

			// Set the continued flag.
			pDst->fContinued	= TRUE;

			// Copy object + property + type + generation from temporary structure.
			pDst->dwObject		= wip.dwObject;
			pDst->dwProperty	= wip.dwProperty;	
			pDst->dwDataType	= wip.dwDataType;
			pDst->wGeneration	= wip.wGeneration;

			// Move the data as an opaque DWORD four bytes at a time.
			pDst->dwImmediateDword = pdw[0];

			// Initialize the length, which in this case is always a constant.
			pDst->cbLength64	= sizeof(DWORD);

			// Increment destination pointer to next TOCX_ITEM structure in m_aToc.
			++pDst;

			// Done with eContdImmediate4.
			break;

		case eReferenceListID:
			// Initialize the storage mode.
			pDst->bStorageMode	= SM_REFLISTID;

			// Clear the continued flag.
			pDst->fContinued	= FALSE;

			// Copy object + property + type + generation from temporary structure.
			pDst->dwObject		= wip.dwObject;
			pDst->dwProperty	= wip.dwProperty;	
			pDst->dwDataType	= wip.dwDataType;
			pDst->wGeneration	= wip.wGeneration;

			// Copy the reference list object ID.
			pDst->dwRefList		= pdw[0];

			// Initialize the length to zero because the reference list object ID is not data.
			pDst->cbLength64	= 0;

			// Increment destination pointer to next TOCX_ITEM structure in m_aToc.
			++pDst;

			// Done with eReferenceListID.
			break;

		default:
			// Copy object + property + type + generation from temporary structure.
			pDst->dwObject		= wip.dwObject;
			pDst->dwProperty	= wip.dwProperty;	
			pDst->dwDataType	= wip.dwDataType;
			pDst->wGeneration	= wip.wGeneration;

			// Convert bTocElementByteCode (1~26 inclusive) into a bit mask.
			DWORD dwCurrentByteCodeAsBitMask = 1 << bTocElementByteCode;

			// Initialize the 'continued' flag to TRUE or FALSE depending on the storage code.
			pDst->fContinued = (dwCurrentByteCodeAsBitMask & dwElemBitMaskContinued) != 0;

			// Does the storage code belong to the "immediate but not continued" group?
			// This group includes eImmediate0, eImmediate1, eImmediate2, eImmediate3, and eImmediate4.
			if (dwCurrentByteCodeAsBitMask & dwElemBitMaskImmNotCont)
			{
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_IMMEDIATE;

				// Move the data as an opaque DWORD four bytes at a time.
				// This is safe to do because the compressed bytestream always reserves four bytes
				// for immediate data even when the bytecount is less than four.
				pDst->dwImmediateDword = pdw[0];

				// Calculate the bytecount and save it.
				// We can use simple subtraction because eImmediate0 <= bTocElementByteCode <= eImmediate4.
				pDst->cbLength64 = bTocElementByteCode - eImmediate0;
			}

			// Does the storage code belong to the "32-bit offset and 32-bit length" group?
			// This group includes eOffset4Len4 and eContdOffset4Len4.
			else if (dwCurrentByteCodeAsBitMask & dwElemBitMaskOffset4x4)
			{
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;	

				// Promote the 32-bit offset to a 64-bit offset.
				pDst->cbOffset64	= pdw[0];

				// Promote the 32-bit length to a 64-bit length.
				pDst->cbLength64	= pdw[1];
			}

			// Does the storage code belong to the "64-bit offset and 32-bit length" group?
			// This group includes eOffset8Len4 and eContdOffset8Len4.
			else if (dwCurrentByteCodeAsBitMask & dwElemBitMaskOffset8x4)
			{
				// Initialize the storage mode.
				pDst->bStorageMode	= SM_OFFSET;	

				// Copy the 64-bit offset.
				// Note that we cast pdw to a PUINT64.
				pDst->cbOffset64	= PUINT64(pdw)[0];

				// Promote the 32-bit length to a 64-bit length.
				pDst->cbLength64	= pdw[2];
			}

			// Does the storage code belong to the "64-bit offset and 64-bit length" group?
			// This group includes eOffset8Len8 and eContdOffset8Len8.
			else if (dwCurrentByteCodeAsBitMask & dwElemBitMaskOffset8x8)
			{
				// Initialize the storage mode.
				pDst->bStorageMode = SM_OFFSET;

				// Copy the 64-bit offset.
				// Note that we cast pdw to a PUINT64.
				pDst->cbOffset64 = PUINT64(pdw)[0];

				// Copy the 64-bit length.
				// Note that we cast pdw to a PUINT64.
				pDst->cbLength64 = PUINT64(pdw)[1];
			}
			else
			{
				// WTF?
				// Something went wrong.
				// Why are we here?
				// Invalid compression token!
				// What is the value of bTocElementByteCode?
				BREAK_IF_DEBUG
			}

			// Increment destination pointer to next TOCX_ITEM structure.
			++pDst;
			break;
		}

		// Calculate the next byte position.
		iCurrentBytePos += cbCurrentEntry;

	}	// while (iCurrentBytePos < m_cbTocSize32)

	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenBentoFile().
//	This method is a self-test, verification, reality check.
//
//	Here we do this:
//	1)	Verify that CReadBento::m_aToc[] exists and that CReadBento::m_nTocItems is greater than or equal to three.
//	2)	Verify that the Bento self-description object - aka CM_StdObjID_TOC, is the first object in the TOC.
//	3)	Find its TOC self-description property - aka CM_StdObjID_TOC_Object.
//	4)	Compare that property's offset and size against the corresponding values in CReadBento::m_stdLabel.
//	5)	Fail if they don't match.
//	6)	Find the self-description object's Container self-description property - aka CM_StdObjID_TOC_Container.
//	7)	Test that property's offset for zero, and compare its size against CReadableFile::m_cbVirtualEndOfFile64.
//	8)	Fail if they don't match.
//	9)	Verify that the TOC's CM_StdObjID_TOC_Seed property exists, and save a copy in m_dwStdObjTocSeed.
//
//	If these tests pass then we can be more confident that our file is valid and that our code is reliable!
//	For more info on the Bento self-description object see the following sections in the Bento Specification 1.0d5:
//		Chapter 8	"Format Usage", subheading "The TOC Itself", page 71.
//		Appendix D	"Standard Object IDs and Global Names", subheading "Standard Objects", page 99.
//*********************************************************************************************************************
HRESULT CReadBento::VerifyToc()
{
	// We intend to update hr with a more-meaningful error code before we exit.
	// If we don't update hr with another error code then its because we goofed up somewhere in this code.
	HRESULT	hr = OMFOO_E_ASSERTION_FAILURE;

	// We must have an TOCX_ITEM array.
	if (m_aToc == NULL)
	{
		// Who called us?
		// How did we even get here?
		BREAK_IF_DEBUG
		hr = OMFOO_E_ASSERTION_FAILURE;
		goto L_CleanupExit;
	}

	// Our TOCX_ITEM array must have at least three properties.
	// CM_StdObjID_TOC_Seed, CM_StdObjID_TOC_Object, and CM_StdObjID_TOC_Container.
	if (m_nTocItems < 3)
	{
		BREAK_IF_DEBUG
		hr = BENTO_E_TOO_FEW_ITEMS;
		goto L_CleanupExit;
	}

	// The Bento "self-description" object must always come first.
	if (CM_StdObjID_TOC != m_aToc[0].dwObject)
	{
		BREAK_IF_DEBUG
		hr = BENTO_E_NO_SELF_DESC;
		goto L_CleanupExit;
	}

	// Each bit in this DWORD corresponds one-to-one with the predefined Bento properties that belong to CM_StdObjID_TOC.
	// We do not attempt to discover all of them. We quit searching when we've found the ones we're looking for.
	DWORD dwDiscoveredBits = 0;

	// These bitmasks represent the properties that we are looking for.
	const DWORD dwTocSeedBit	= (1 << CM_StdObjID_TOC_Seed);
	const DWORD dwTocObjectBit	= (1 << CM_StdObjID_TOC_Object);
	const DWORD dwContainerBit	= (1 << CM_StdObjID_TOC_Container);

	// This bitmask represents the totality of all properties that we are looking for.
	const DWORD dwRequiredBits = dwTocSeedBit|dwTocObjectBit|dwContainerBit;

	// Loop through all properties in object 0x00000001.
	for (ULONG i = 0; i < m_nTocItems; i++)
	{
		// Does the current TOCX_ITEM (still) belong to the Bento self-description object?
		// If not then we are done traversing the TOC.
		// If the object ID is no longer 0x00000001 then break.
		if (CM_StdObjID_TOC != m_aToc[i].dwObject)
		{
			// This is a good thing!
			// It's the first of two natural/expected ways to exit this loop.
			break;
		}

		// If dwProperty is greater than 31 then that means it's not one of the predefined Bento properties.
		if (m_aToc[i].dwProperty > 31)
		{
			// This is a good thing!
			// It's the second of two natural/expected ways to exit this loop.
			break;
		}

		// Set the appropriate "property exists" bit.
		dwDiscoveredBits |= (1 << m_aToc[i].dwProperty);

		// Is this the Bento TOC object starting seed property?
		if (m_aToc[i].dwProperty == CM_StdObjID_TOC_Seed)
		{
			// The starting seed property is always immediate.
			// But if it isn't then don't fail here.
			// Note that the result in m_dwStdObjTocSeed may need additional bytewapping if the OMF layer is big endian.
			if (m_aToc[i].bStorageMode == SM_IMMEDIATE)
			{
				if (m_fBentoBigEndian)
				{
					m_dwStdObjTocSeed = Endian32(m_aToc[i].dwImmediateDword);
				}
				else
				{
					m_dwStdObjTocSeed = m_aToc[i].dwImmediateDword;
				}
			}
			else
			{
				// WTF?
				BREAK_IF_DEBUG
			}
		}

		// Is this the Bento TOC self-description property?
		if (m_aToc[i].dwProperty == CM_StdObjID_TOC_Object)
		{
			// The TOC self-description property is never immediate!
			// The TOC self-description property never has a reference list ID!
			// The TOC self-description property is always stored as an offset & length!
			if (m_aToc[i].bStorageMode != SM_OFFSET)
			{
				// WTF?
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_1;
				goto L_CleanupExit;
			}

			// The TOC self-description property is never fragmented!
			if (m_aToc[i].fContinued)
			{
				// WTF?
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_1;
				goto L_CleanupExit;
			}

			// The TOC self-description property offset must match the corresponding offset stored in the label.
			if (m_aToc[i].cbOffset64 != m_cbTocOffset64)
			{
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_1;
				goto L_CleanupExit;
			}

			// The TOC self-description property length must match the corresponding length stored in the label.
			if (m_aToc[i].cbLength64 != m_cbTocSize32)
			{
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_1;
				goto L_CleanupExit;
			}

			// The TOC self-description property tests okay.
			// Continue testing. Back to the top of this loop.
			continue;
		}

		// Is this the Bento Container self-description property?
		if (m_aToc[i].dwProperty == CM_StdObjID_TOC_Container)
		{
			// The Container self-description property is never immediate!
			// The Container self-description property never has a reference list ID!
			// The Container self-description property is always stored at an offset!
			if (m_aToc[i].bStorageMode != SM_OFFSET)
			{
				// WTF?
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_2;
				goto L_CleanupExit;
			}

			// The Container self-description property is never fragmented!
			if (m_aToc[i].fContinued)
			{
				// WTF?
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_2;
				goto L_CleanupExit;
			}

			// The Container self-description property offset must always be zero.
			if (m_aToc[i].cbOffset64)
			{
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_2;
				goto L_CleanupExit;
			}

			// The Container self-description property length must match the size of our file.
			if (m_aToc[i].cbLength64 != m_cbVirtualEndOfFile64)
			{
				BREAK_IF_DEBUG
				hr = BENTO_E_SELF_TEST_2;
				goto L_CleanupExit;
			}

			// Back to the top of the loop.
			continue;
		}

		// We can test for additional standard Bento properties right here if necessary, like this:
	//	if (m_aToc[iToc].dwProperty == CM_StdObjID_???)
	//	{
	//		continue;
	//	}
	}

	// Okay we are done looping.
	// Have we detected (and verified) all of the required TOC-related properties?
	if ((dwDiscoveredBits & dwRequiredBits) == dwRequiredBits)
	{
		// Success!
		// We have found and verified all of the properties we are testing for.
		hr = S_OK;
	}
	else
	{
		BREAK_IF_DEBUG
		hr = BENTO_E_MISSING_PROPS;
	}

L_CleanupExit:
	return hr;
}

//*********************************************************************************************************************
//	Survey all objectIDs, propertyIDs, and datatypeIDs, and return the greatest ID value.
//	I wrote this as temporary code while I was experimenting and debugging, and learning about CM_StdObjID_TOC_Seed.
//	But now we never call it, and don't really need it.
//*********************************************************************************************************************
DWORD CReadBento::FindGreatestBentoID(void)
{
	DWORD dwResult = 0;
	for (ULONG i = 0; i < m_nTocItems; i++)
	{
		if (m_aToc[i].dwObject > dwResult)
		{
			dwResult = m_aToc[i].dwObject;
		}
		if (m_aToc[i].dwProperty > dwResult)
		{
			dwResult = m_aToc[i].dwProperty;
		}
		if (m_aToc[i].dwDataType > dwResult)
		{
			dwResult = m_aToc[i].dwDataType;
		}
	}
	return dwResult;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenBentoFile().
//	Allocates and populates our m_aPropertyNames[] member, which is an array of BENTO_BINDING structures.
//	On exit m_nPropertyNames contains the number of elements in the m_aPropertyNames[] array.
//*********************************************************************************************************************
HRESULT CReadBento::BuildPropertyNameTable(void)
{
	PTOCX_ITEM		pSrc	= NULL;
	PBENTO_BINDING	pDst	= NULL;

	ULONG		cEstimatedNames	= 0;
	ULONG		cActualNames	= 0;

	// We always have at least 15 global property names.
	// We know this because there are 15 standard Bento properties.
	// See Apple's Bento Specification Revision 1.0d5, "Appendix D: Standard Object IDs and Global Names" at page 99.
	cEstimatedNames = BENTO_NUM_HARDCODED_PROPERTIES;

	// Scan the TOC for all instances of the CM_StdObjID_GlobalPropName property.
	// This will tell us how many standard and non-standard properties are defined in the file.
	cEstimatedNames += CountPropertyInstancesByID(CM_StdObjID_GlobalPropName);

	// Calculate memory requirements and allocate the memory.
	// Remember - memory allocated with our allocator is always zeroed.
	m_aPropertyNames = PBENTO_BINDING(MemAlloc(cEstimatedNames * sizeof(BENTO_BINDING)));

	// Reality check.
	if (NULL == m_aPropertyNames)
	{
		return E_OUTOFMEMORY;
	}

	// Initialize pSrc to point to the very first TOCX_ITEM structure in our m_aToc[] array.
	pSrc = m_aToc;

	// Initialize pDst to point to the very first BIND_PROPERTY structure in our m_aPropertyNames[] array.
	pDst = m_aPropertyNames;

	// Pre-decrement pSrc before we enter the loop (because we advance it immediately inside the loop).
	--pSrc;

	for (ULONG iToc = 0; iToc < m_nTocItems; iToc++)
	{
		// Advance source pointer to the next TOCX_ITEM structure in our m_aToc[] array.
		++pSrc;

		// Is this a property name?
		if (pSrc->dwProperty != CM_StdObjID_GlobalPropName)
		{
			// If it is not CM_StdObjID_GlobalPropName then we don't care what it is.
			continue;
		}

		// Is this object one of the standard pre-defined objects described in Appendix D of the Bento Spec?
		// See "Global Names" listed on page 100~101 of the Bento Spec Rev 1.0d5.
		if (IsAStandardBentoProperty(pSrc->dwObject))
		{
			// We don't care about standard properties (because their object ID is etched in stone).
			continue;
		}

		// Reality check: Is it an ASCII string?
		if (pSrc->dwDataType != CM_StdObjID_7BitASCII)
		{
			// Something is messed up.
			// So just ignore this property.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Reality check: Is the storage mode SM_OFFSET?
		if (pSrc->bStorageMode != SM_OFFSET)
		{
			// Something is messed up.
			// So just ignore this property.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Reality check: Is the value continued?
		if (pSrc->fContinued)
		{
			// Something is messed up.
			// So just ignore this property.
			BREAK_IF_DEBUG
			continue;
		}

		// Reality check.
		// The string must be small enough to fit in our BENTO_BINDING structure - including its null-terminator.
		if (pSrc->cbLength64 >= BENTO_STRMAX_UNIQUE_NAME)
		{
			// Something is messed up.
			// So just ignore this property.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Copy the object ID into our BENTO_BINDING structure.
		pDst->dwObject = pSrc->dwObject;

		// Read the property name string into our BENTO_BINDING structure.
		// The string will become null-terminated even if it wasn't terminated in the file.
		CHECK(SeekRead(pSrc->cbOffset64, pDst->szUniqueName, pSrc->cbLengthLo));

		// Advance destination pointer to the next BENTO_BINDING structure in our m_aPropertyNames[] array.
		++pDst;

		// Increment the number of names successfully processed.
		++cActualNames;
	}

	// Reset the pDst pointer to the first BENTO_BINDING structure in our m_aPropertyNames[] array.
	pDst = m_aPropertyNames;

	// Loop through each BENTO_BINDING structure and create a 32-bit hash on the string.
	// Save the hash in the structure.
	for (UINT iName = 0; iName < cActualNames; iName++)
	{
		DWORD	dwHash		= 0;
		DWORD	dwChar		= 0;
		LPCSTR	psz			= pDst->szUniqueName;

		while (dwChar = DWORD(*psz++))
		{
			dwHash = _rotl(dwHash,11)^(dwChar*4246609);
		}
		pDst->dwHash = dwHash;
		++pDst;
	}

	// Save count permanently.
	m_nPropertyNames = cActualNames;
	return S_OK;
}

//*********************************************************************************************************************
//	Private initialization routine called once per lifetime during OpenBentoFile().
//	Allocates and populates our m_aDataTypeNames[] member, which is an array of BENTO_BINDING structures.
//	On exit m_nDataTypeNames contains the number of elements in the m_aDataTypeNames[] array.
//*********************************************************************************************************************
HRESULT CReadBento::BuildDataTypeNameTable(void)
{
	PTOCX_ITEM		pSrc	= NULL;
	PBENTO_BINDING	pDst	= NULL;

	ULONG		cEstimatedNames	= 0;
	ULONG		cActualNames	= 0;

	// We always have at least nine global data type names.
	// We know this because there are nine standard Bento data types.
	// See Apple's Bento Specification Revision 1.0d5, "Appendix D: Standard Object IDs and Global Names" at page 99.
	cEstimatedNames = BENTO_NUM_HARDCODED_DATA_TYPES;

	// Scan the TOC for all instances of the CM_StdObjID_GlobalTypeName property.
	// This will tell us how many standard and non-standard data types are defined in the file.
	cEstimatedNames += CountPropertyInstancesByID(CM_StdObjID_GlobalTypeName);

	// Calculate memory requirements and allocate the memory.
	// Remember - memory allocated with our allocator is always zeroed.
	m_aDataTypeNames = PBENTO_BINDING(MemAlloc(cEstimatedNames * sizeof(BENTO_BINDING)));

	// Reality check.
	if (NULL == m_aDataTypeNames)
	{
		return E_OUTOFMEMORY;
	}

	// Initialize the pSrc pointer to the first TOCX_ITEM structure in our m_aToc[] array.
	pSrc = m_aToc;

	// Initialize the pDst pointer to the first BENTO_BINDING structure in our m_aDataTypeNames[] array.
	pDst = m_aDataTypeNames;

	// Pre-decrement pSrc before we enter the loop (because we advance it immediately inside the loop).
	--pSrc;

	for (UINT iToc = 0; iToc < m_nTocItems; iToc++)
	{
		// Advance source pointer to the next TOCX_ITEM structure in our m_aToc[] array.
		++pSrc;

		// Is this a data type name?
		if (pSrc->dwProperty != CM_StdObjID_GlobalTypeName)
		{
			// If it is not CM_StdObjID_GlobalTypeName then we don't care what it is.
			continue;
		}

		// Is this object one of the standard pre-defined objects described in Appendix D of the Bento Spec?
		// See "Global Names" listed on page 100~101 of the Bento Spec Rev 1.0d5.
		if (IsAStandardBentoDataType(pSrc->dwObject))
		{
			// We don't care about standard data types (because their object ID is etched in stone).
			continue;
		}

		// Reality check: Is it an ASCII string?
		if (pSrc->dwDataType != CM_StdObjID_7BitASCII)
		{
			// Something is messed up.
			// So just ignore this data type.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Reality check: Is the storage mode SM_OFFSET?
		if (pSrc->bStorageMode != SM_OFFSET)
		{
			// Something is messed up.
			// So just ignore this data type.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Reality check: Is the value continued?
		if (pSrc->fContinued)
		{
			// Something is messed up.
			// So just ignore this data type.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Reality check.
		// The string must be small enough to fit in our BENTO_BINDING structure - including its null-terminator.
		if (pSrc->cbLength64 >= BENTO_STRMAX_UNIQUE_NAME)
		{
			// Something is messed up.
			// So just ignore this data type.
			BREAK_IF_DEBUG	// has this ever happened?
			continue;
		}

		// Copy the object ID into our BENTO_BINDING structure.
		pDst->dwObject = pSrc->dwObject;

		// Read the data type name string into our BENTO_BINDING structure.
		// The string will become null-terminated even if it wasn't terminated in the file.
		CHECK(SeekRead(pSrc->cbOffset64, pDst->szUniqueName, pSrc->cbLengthLo));

		// Advance destination pointer to the next BENTO_BINDING structure in our m_aDataTypeNames[] array.
		++pDst;

		// Increment the number of names successfully processed.
		++cActualNames;
	}

	// Reset the pDst pointer to the first BENTO_BINDING structure in our m_aDataTypeNames[] array.
	pDst = m_aDataTypeNames;

	// Loop through each BENTO_BINDING structure and create a 32-bit hash on the string.
	// Save the hash value in the structure.
	for (UINT iName = 0; iName < cActualNames; iName++)
	{
		DWORD	dwHash		= 0;
		DWORD	dwChar		= 0;
		LPCSTR	psz			= pDst->szUniqueName;

		while (dwChar = DWORD(*psz++))
		{
			dwHash = _rotl(dwHash,11)^(dwChar*4246609);
		}
		pDst->dwHash = dwHash;
		++pDst;
	}

	// Save count permanently.
	m_nDataTypeNames = cActualNames;
	return S_OK;
}

//*********************************************************************************************************************
//	What we do here is ... 
//	1) Perform boilerplate Win32 string tests.
//	2) Test for printable alphanumeric ASCII characters and colons.
//	3) Look for at least one colon and prohibit two consecutive colons.
//	4) Make sure the string is longer than seven characters but less than 64 characters.
//	5) Make sure there are more alphanumeric characters than colons.
//	6) Make sure there are at least three alphanumeric characters before the first colon.
//*********************************************************************************************************************
BOOL CReadBento::CheckUniqueNameSyntax(PCSTR pszUniqueName)
{
	PBYTE	pByte			= PBYTE(pszUniqueName);
	BOOL	fInAlphaNumeric	= FALSE;
	ULONG	cchAlphaNumeric	= 0;
	ULONG	cchColons		= 0;
	BYTE	ch				= 0;

	// Unique names should be no longer than 63 characters plus a null-terminator.
	// This isn't a requirement of the Bento spec, but it's a limitation of the implementation in 'omfToolkit.dll'.
	if (IsBadStringPtrA(pszUniqueName, BENTO_STRMAX_UNIQUE_NAME))
	{
		return FALSE;
	}

	// IsBadStringPtrA() will return FALSE if pszUniqueName is not null-terminated within the first 64 characters.
	// So walk through the string ourselves until we find a terminator.
	ULONG cch = 0;
	do {
		if (pByte[cch] == 0) {
			break;
		}
	} while (++cch < BENTO_STRMAX_UNIQUE_NAME);

	// If loop completed without finding a null-terminator then fail.
	if (cch == BENTO_STRMAX_UNIQUE_NAME)
	{
		return FALSE;
	}

	// The shortest string we should expect is "omfi:UID".
	if (cch < 8)
	{
		return FALSE;
	}

	// Note that this for() loop has several exit points.
	for (ULONG i = 0; i < BENTO_STRMAX_UNIQUE_NAME; i++)
	{
		// Fetch next character in string and treat it as unsigned.
		ch = pByte[i];

		// Is this the null-terminator?
		if (0 == ch)
		{
			if ((i >= 7) &&						// Have we traversed at least seven characters?
				(cchColons > 0) &&				// Was there at least one colon?
				(cchAlphaNumeric > cchColons))	// Were there more characters than colons?
			{
				// Pass!
				return TRUE;
			}
			else
			{
				// Invalid unique name.
				return FALSE;
			}
		}

		// Is this character printable ascii?
		if ((ch < 0x20) || (ch > 0x7E))
		{
			// Invalid character in unique name.
			return FALSE;
		}

		// Is this character alpha-numeric?
		if (((ch >= 'A') && (ch <= 'Z')) ||
			((ch >= 'a') && (ch <= 'z')) ||
			((ch >= '0') && (ch <= '9')))
		{
			// Increment our alphanumeric counter.
			cchAlphaNumeric++;

			// Set the flag that tells us that previous character was alphanumeric.
			fInAlphaNumeric = TRUE;

			// Back to the top of this loop.
			continue;
		}

		// Is this a namespace delimiter?
		if (ch == ':')
		{
			// Have we already traversed at least three alphanumeric characters?
			if (i < 3)
			{
				// Not a namespace.
				return FALSE;
			}

			// We don't allow two adjacent punctuation characters.
			// If the previous character was not alphanumeric then fail.
			if (!fInAlphaNumeric)
			{
				return FALSE;
			}

			// Clear the alphanumeric flag.
			fInAlphaNumeric = FALSE;

			// Increment our colon counter.
			cchColons++;

			// Back to the top of this loop.
			continue;
		}

		// If we've traversed at least one colon then we also allow underscore, dot, and minus.
		if (cchColons)
		{
			if ((ch == '_') ||
				(ch == '.') ||
				(ch == '-'))
			{
				// Clear the alphanumeric flag.
				fInAlphaNumeric = FALSE;

				// Back to the top of this loop.
				continue;
			}
		}

		// If it's not any of the above then it is not a unique name.
		// Exit this loop and return FALSE.
		break;
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Public inquiry method.
//	Count the total number of times the specified property ID appears in the TOC.
//*********************************************************************************************************************
ULONG CReadBento::CountPropertyInstancesByID(DWORD dwProperty)
{
	ULONG cProps = 0;
	if (dwProperty)
	{
		// Loop through each item in m_aToc[]. 
		for (UINT iToc = 0; iToc < m_nTocItems; iToc++)
		{
			if ((m_aToc[iToc].dwProperty == dwProperty) &&
				(!m_aToc[iToc].fContinued) &&
				(m_aToc[iToc].bStorageMode != SM_REFLISTID))
			{
				++cProps;
			}
		}
	}
	return cProps;
}

//*********************************************************************************************************************
//	Public inquiry method.
//	Count the total number of times the specified data type ID appears in the TOC.
//*********************************************************************************************************************
ULONG CReadBento::CountDataTypeInstancesByID(DWORD dwDataType)
{
	ULONG nTypes = 0;
	if (dwDataType)
	{
		// Loop through each item in m_aToc[].
		for (UINT iToc = 0; iToc < m_nTocItems; iToc++)
		{
			if ((m_aToc[iToc].dwDataType == dwDataType) &&
				(!m_aToc[iToc].fContinued) &&
				(m_aToc[iToc].bStorageMode != SM_REFLISTID))
			{
				++nTypes;
			}
		}
	}
	return nTypes;
}

//*********************************************************************************************************************
//	Protected helper.
//	Extracts raw property data.
//	The data is either retrieved from RAM (directly from a TOCX_ITEM structure) or from the file.
//	This method is forgiving. The number of bytes to read must be less than or equal to the size of the data,
//	or this routine returns OMF_E_SIZE_SURPRISE.
//*********************************************************************************************************************
HRESULT CReadBento::ReadItemBytes(__in ULONG iToc, __in ULONG cbBuffer, __out PVOID pBuffer)
{
	// Check caller's pointer.
	if (IsBadWritePointer(pBuffer, cbBuffer))
		return E_POINTER;

	// Wipe caller's buffer.
	SecureZeroMemory(pBuffer, cbBuffer);

	// Validate caller's index - check array bounds.
	if (iToc >= m_nTocItems)
		return E_INVALIDARG;

	// Number of bytes to read must match the length of the data.
	if (m_aToc[iToc].cbLength64 != cbBuffer)
		return OMF_E_SIZE_SURPRISE;

	// Is data stored immediately in the TOC?
	if (m_aToc[iToc].bStorageMode == SM_IMMEDIATE)
	{
		// This storage mode can only accommodate immediate data that is four bytes long or less.
		// If our TOCX_ITEM says that the length is longer than that, then something is horribly wrong!
		if (m_aToc[iToc].cbLength64 > 4)
		{
			return OMF_E_STORAGE_SURPRISE;
		}

		// Copy it from RAM.
		CopyMemory(pBuffer, &m_aToc[iToc].aImmediateBytes, cbBuffer);
		return S_OK;
	}

	if (m_aToc[iToc].bStorageMode == SM_OFFSET)
	{
		// Read it from disk.
		return SeekRead(m_aToc[iToc].cbOffset64, pBuffer, cbBuffer);
	}

	// We can't deal with the data because we don't know how it is stored ...
	BREAK_IF_DEBUG
	return OMF_E_STORAGE_SURPRISE;
}

//*********************************************************************************************************************
//	Is dwProperty one of the standard Bento properties listed in appendix D of Bento Specification Rev 1.0d5?
//*********************************************************************************************************************
BOOL CReadBento::IsAStandardBentoProperty(DWORD dwProperty)
{
	// Is dwProperty one of the standard properties listed in Appendix D of Apple's Bento Specification?
	// These values are predefined, fixed, and etched in stone.
	// They are not assigned dynamically - they have the same literal value in every Bento file.
	if ((dwProperty == CM_StdObjID_TOC_Seed) ||			// (2)  Apple:Global_Name_TOC_Seed
		(dwProperty == CM_StdObjID_TOC_MinSeed) ||		// (3)  Apple:Global_Name_TOC_MinSeed
		(dwProperty == CM_StdObjID_TOC_Object) ||		// (4)  Apple:Global_Name_TOC_Object
		(dwProperty == CM_StdObjID_TOC_Container) ||	// (5)  Apple:Global_Name_TOC_Container
		(dwProperty == CM_StdObjID_TOC_Deleted) ||		// (6)  Apple:Global_Name_TOC_Deleted
		(dwProperty == CM_StdObjID_TOC_Free) ||			// (7)  Apple:Global_Name_TOC_Free
		(dwProperty == CM_StdObjID_TOC_NewValuesTOC) ||	// (8)  Apple:Global_Name_TOC_NewValuesTOC
		(dwProperty == CM_StdObjID_TOC_Target) ||		// (9)  Apple:Global_Name_TOC_Target
		(dwProperty == CM_StdObjID_TOC_DeleteList) ||	// (10) Apple:Global_Name_TOC_DeleteList
		(dwProperty == CM_StdObjID_GlobalTypeName) ||	// (23) Global type name
		(dwProperty == CM_StdObjID_GlobalPropName) ||	// (24) Global property name
		(dwProperty == CM_StdObjID_DynamicValues) ||	// (25) Apple:Dynamic_Value
		(dwProperty == CM_StdObjID_BaseTypes) ||		// (26) Apple:Type_BaseType
		(dwProperty == CM_StdObjID_ValueUpdates) ||		// (29) Apple:Type_ValueUpdates
		(dwProperty == CM_StdObjID_ObjReferences))		// (31) Referenced objects from an object
	{
		return TRUE;
	}

	return FALSE;
}

//*********************************************************************************************************************
//	Is dwDataType one of the standard Bento data types listed in appendix D of Bento Specification Rev 1.0d5?
//*********************************************************************************************************************
BOOL CReadBento::IsAStandardBentoDataType(DWORD dwDataType)
{
	// Is dwDataType one of the ten standard data types listed in Appendix D of Apple's Bento Specification?
	// These values are predefined, fixed, and etched in stone.
	// They are not assigned dynamically - they have the same literal value in every Bento file.
	if ((dwDataType == CM_StdObjID_TOC_Type) ||			// (19) Apple:Global_Name_TOC_Type
		(dwDataType == CM_StdObjID_7BitASCII) ||		// (21) 7-bit ASCII
		(dwDataType == CM_StdObjID_8BitASCII) ||		// (22) 8-bit ASCII
		(dwDataType == CM_StdObjID_BaseTypeList) ||		// (27) Apple:Type_BaseTypeList
		(dwDataType == CM_StdObjID_TargetContainer) ||	// (28) Apple:Target_Container
		(dwDataType == CM_StdObjID_UpdatesData) ||		// (30) Apple:Type_UpdatesData
		(dwDataType == CM_StdObjID_ObjRefData) ||		// (32) Reference data
		(dwDataType == CM_StdObjID_32BitImmValue) ||	// (40) 32-bit immediate data value
		(dwDataType == CM_StdObjID_16BitImmValue) ||	// (41) 16-bit immediate data value
		(dwDataType == CM_StdObjID_8BitImmValue))		// (42)  8-bit immediate data value
	{
		return TRUE;
	}

	return FALSE;
}

//*********************************************************************************************************************
//	String-to-ID conversion.
//	Given a property name retrieve its property ID.
//	Returns zero if not found.
//*********************************************************************************************************************
DWORD CReadBento::LookupPropertyIDByName(LPCSTR pszPropertyName)
{
	if (IsBadStringPtrA(pszPropertyName, BENTO_STRMAX_UNIQUE_NAME))
	{
		return 0;
	}
	else
	{
		DWORD	dwProperty	= 0;
		DWORD	dwHash		= 0;
		DWORD	dwChar		= 0;
		LPCSTR	psz			= pszPropertyName;
		LPCSTR	pEnd		= &psz[BENTO_STRMAX_UNIQUE_NAME];

		while ((psz < pEnd) && (dwChar = DWORD(*psz++)))
		{
			dwHash = _rotl(dwHash,11)^(dwChar*4246609);
		}

		// If the string was properly terminated then dwChar will be zero.
		if (dwChar == 0)
		{
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
				}
			}
		}

		return dwProperty;
	}
}

//*********************************************************************************************************************
//	String-to-ID conversion.
//	Given a data type name retrieve its data type ID.
//	Returns zero if not found.
//*********************************************************************************************************************
DWORD CReadBento::LookupDataTypeIDByName(LPCSTR pszDataTypeName)
{
	if (IsBadStringPtrA(pszDataTypeName, BENTO_STRMAX_UNIQUE_NAME))
	{
		return 0;
	}
	else
	{
		DWORD	dwDataType	= 0;
		DWORD	dwHash		= 0;
		DWORD	dwChar		= 0;
		LPCSTR	psz			= pszDataTypeName;
		LPCSTR	pEnd		= &psz[BENTO_STRMAX_UNIQUE_NAME];

		while ((psz < pEnd) && (dwChar = DWORD(*psz++)))
		{
			dwHash = _rotl(dwHash,11)^(dwChar*4246609);
		}

		// If the string was properly terminated then dwChar will be zero.
		if (dwChar == 0)
		{
			for (UINT i = 0; i < m_nDataTypeNames; i++)
			{
				if (dwHash == m_aDataTypeNames[i].dwHash)
				{
					if (lstrcmpA(pszDataTypeName, m_aDataTypeNames[i].szUniqueName) == 0)
					{
						// Found it!
						dwDataType = m_aDataTypeNames[i].dwObject;
						break;
					}
				}
			}
		}

		return dwDataType;
	}
}

//*********************************************************************************************************************
//	ID-to-string conversion routine.
//	Given a property ID, retrieves the property's name.
//	The buffer that pchResult points to must be at large enough to hold BENTO_STRMAX_UNIQUE_NAME characters.
//*********************************************************************************************************************
HRESULT CReadBento::LookupPropertyNameFromID(DWORD dwProperty, PCHAR pchResult)
{
	if (IsBadWritePointer(pchResult, BENTO_STRMAX_UNIQUE_NAME))
		return E_POINTER;

	ZeroMemory(pchResult, BENTO_STRMAX_UNIQUE_NAME);

	if (0 == dwProperty)
		return E_INVALIDARG;

	for (UINT i = 0; i < m_nPropertyNames; i++)
	{
		if (m_aPropertyNames[i].dwObject == dwProperty)
		{
			lstrcpyA(pchResult, m_aPropertyNames[i].szUniqueName);
			return S_OK;
		}
	}

	return OMF_E_PROP_NOT_DEFINED;
}

//*********************************************************************************************************************
//	ID-to-string conversion routine.
//	Given a data type ID, retrieves the data type's name.
//	The buffer that pchResult points to must be at large enough to hold BENTO_STRMAX_UNIQUE_NAME characters.
//*********************************************************************************************************************
HRESULT CReadBento::LookupDataTypeNameFromID(DWORD dwDataType, PCHAR pchResult)
{
	if (IsBadWritePointer(pchResult, BENTO_STRMAX_UNIQUE_NAME))
		return E_POINTER;

	ZeroMemory(pchResult, BENTO_STRMAX_UNIQUE_NAME);

	if (dwDataType == 0)
		return E_INVALIDARG;

	for (UINT i = 0; i < m_nDataTypeNames; i++)
	{
		if (m_aDataTypeNames[i].dwObject == dwDataType)
		{
			lstrcpyA(pchResult, m_aDataTypeNames[i].szUniqueName);
			return S_OK;
		}
	}

	return OMF_E_TYPE_NOT_DEFINED;
}

//*********************************************************************************************************************
//	Finds the index of the first TOCX_ITEM struture in m_aToc[] whose object ID matches caller's dwObject.
//	If this method succeeds the index is returned in hr. If it fails it returns OMF_E_OOBJ_NOT_FOUND.
//*********************************************************************************************************************
HRESULT CReadBento::FindTocIndexForObject(DWORD dwObject)
{
	if (dwObject)
	{
		ULONG i = 0;
		do
		{
			if (m_aToc[i].dwObject == dwObject)
			{
				// Cast i as an HRESULT and return it.
				return HRESULT(i);
			}
		} while (++i < m_nTocItems);
	}
	return OMF_E_OOBJ_NOT_FOUND;
}

//*********************************************************************************************************************
//	Finds the index of the TOCX_ITEM struture in m_aToc[] whose object ID and property ID matches the caller's
//	dwObject and dwProperty arguments. We use this for bootstrapping.
//	If this method succeeds then hr is a success code that holds the TOC index.
//	If it fails it returns OMF_E_OOBJ_NOT_FOUND or OMF_E_PROP_NOT_FOUND.
//*********************************************************************************************************************
HRESULT CReadBento::FindTocIndexForObjectWithProperty(DWORD dwObject, DWORD dwProperty)
{
	// Validate caller's dwObject argument.
	if (dwObject == 0)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Validate caller's dwProperty argument.
	if (dwProperty == 0)
	{
		return OMF_E_PROP_NOT_DEFINED;
	}

	ULONG i = 0;
	do
	{
		if (m_aToc[i].dwObject == dwObject)
		{
			do
			{
				if ((m_aToc[i].dwProperty == dwProperty) && (m_aToc[i].bStorageMode != SM_REFLISTID))
				{
					// Cast i as a success code (an HRESULT) and return it.
					return HRESULT(i);
				}
			} while ((++i < m_nTocItems) && (m_aToc[i].dwObject == dwObject));

			return OMF_E_PROP_NOT_FOUND;
		}
	} while (++i < m_nTocItems);

	return OMF_E_OOBJ_NOT_FOUND;
}

//*********************************************************************************************************************
//	Finds the index of the TOCX_ITEM struture in m_aToc[] whose object ID, property ID, and data type ID
//	matches the caller's dwObject, dwProperty, and dwDataType arguments.
//	If this method succeeds then hr is a success code that holds the TOC index.
//	If it fails it returns OMF_E_OOBJ_NOT_FOUND, OMF_E_PROP_NOT_FOUND, or OMF_E_TYPE_NOT_FOUND.
//*********************************************************************************************************************
HRESULT CReadBento::FindTocIndexForObjectWithPropertyAndType(DWORD dwObject, DWORD dwProperty, DWORD dwDataType)
{
	// Validate caller's dwObject argument.
	if (dwObject == 0)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Validate caller's dwProperty argument.
	if (dwProperty == 0)
	{
		return OMF_E_PROP_NOT_DEFINED;
	}

	// Validate caller's dwDataType argument.
	if (dwDataType == 0)
	{
		return OMF_E_TYPE_NOT_DEFINED;
	}

	ULONG i = 0;
	do
	{
		if (m_aToc[i].dwObject == dwObject)
		{
			do
			{
				if ((m_aToc[i].dwProperty == dwProperty) && (m_aToc[i].bStorageMode != SM_REFLISTID))
				{
					do
					{
						if (m_aToc[i].dwDataType == dwDataType)
						{
							// Cast i as an HRESULT and return it.
							return HRESULT(i);
						}
					} while ((++i < m_nTocItems)
							&& (m_aToc[i].dwObject == dwObject)
							&& (m_aToc[i].dwProperty == dwProperty));

					return OMF_E_TYPE_NOT_FOUND;
				}
			} while ((++i < m_nTocItems) && (m_aToc[i].dwObject == dwObject));	
			return OMF_E_PROP_NOT_FOUND;
		}
	} while (++i < m_nTocItems);

	return OMF_E_OOBJ_NOT_FOUND;
}

//*********************************************************************************************************************
//	Finds the index of the TOCX_ITEM struture in m_aToc[] whose object ID and data type ID matches the caller's
//	dwObject and dwDataType arguments.
//	If this method succeeds then hr is a success code that holds the TOC index.
//	If it fails it returns OMF_E_OOBJ_NOT_FOUND or OMF_E_TYPE_NOT_FOUND.
//*********************************************************************************************************************
HRESULT CReadBento::FindTocIndexForObjectWithType(__in DWORD dwObject, __in DWORD dwDataType)
{
	// Validate caller's dwObject argument.
	if (dwObject == 0)
	{
		return OMF_E_OOBJ_NOT_FOUND;
	}

	// Validate caller's dwDataType argument.
	if (dwDataType == 0)
	{
		return OMF_E_TYPE_NOT_DEFINED;
	}

	ULONG i = 0;
	do
	{
		if (m_aToc[i].dwObject == dwObject)
		{
			do
			{
				if ((m_aToc[i].dwDataType == dwDataType) && (m_aToc[i].bStorageMode != SM_REFLISTID))
				{
					// Cast i as an HRESULT and return it.
					return HRESULT(i);
				}
			} while ((++i < m_nTocItems) && (m_aToc[i].dwObject == dwObject));
			return OMF_E_TYPE_NOT_FOUND;
		}
	} while (++i < m_nTocItems);

	return OMF_E_OOBJ_NOT_FOUND;
}

//*********************************************************************************************************************
//	The lookup table below converts a BentoStreamElement (0~26 inclusive) into a byte count (0, 4, 8, 12, or 16).
//	Note that the byte count is the maximum size of bytes reserved on disk, and not necessarily the size of the data.
//	The actual data value - once it is expanded, may use fewer bytes.
//*********************************************************************************************************************
const BYTE CReadBento::m_aStreamElementByteCountTable[27] =
{
	0,		// Unused
	12,		// NewObject
	8,		// NewProperty
	4,		// NewType
	4,		// ExplicitGen
	8,		// Offset4Len4
	8,		// ContdOffset4Len4
	12,		// Offset8Len4
	12,		// ContdOffset8Len4
	0,		// Immediate0
	4,		// Immediate1
	4,		// Immediate2
	4,		// Immediate3
	4,		// Immediate4
	4,		// ContdImmediate4
	4,		// ReferenceListID
	4,		// Reserved4b
	4,		// Reserved4c
	8,		// Reserved8a
	8,		// Reserved8b
	8,		// Reserved8c
	12,		// Reserved12a
	12,		// Reserved12b
	12,		// Reserved12c
	0,		// EndOfBufr
	16,		// Offset8Len8
	16,		// ContdOffset8Len8
};
