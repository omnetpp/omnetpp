/*
 * tkImgPNG.c --
 *
 *		A Tk photo image file handler for PNG files.  Requires zlib.
 *
 * Copyright (c) 2005 Michael Kirkham <mikek@muonics.com> & Muonics
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tkImgPNG.c,v 1.6 2005/03/26 22:22:13 mikek Exp $
 */

#include <stdlib.h>
#include <memory.h>
#include <limits.h>

#include <math.h>
#include "tcl.h"
#include "tk.h"

#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <6
#include <zlib.h>

/*
 * Change by Andras:
 *
 * Handle Tk_PhotoPutBlock signature change in Tk 8.5:
 * "Added interp argument to these functions and made them return
 * a standard Tcl result, with error indicating memory allocation
 * failure instead of panic()ing."
 * See Tk ChangeLog entry from 2003-03-06 by Donal K. Fellows
 * --
 */

#if TK_MAJOR_VERSION>8 || (TK_MAJOR_VERSION==8 && TK_MINOR_VERSION>=5)
#define INTERP_IF_TK85 interp,
#else
#define INTERP_IF_TK85
#endif

/* Every PNG image starts with the following 8-byte signature */

static const Byte	gspPNGSignature[]	= { 137, 80, 78, 71, 13, 10, 26, 10 };
#define	gsSigSz	8

static const int	gspStartLine[8] = {0, 0, 0, 4, 0, 2, 0, 1};

#define	PNG_LONG(a,b,c,d)	((a << 24) | (b << 16) | (c << 8) | d)

#define	PNG_BLOCK_SZ		1024		/* Process up to 1k at a time */

/* Chunk type flags */

#define PNG_CF_ANCILLARY	0x10000000L	/* Non-critical chunk (can ignore) */
#define PNG_CF_PRIVATE		0x00100000L	/* Application-specific chunk */
#define PNG_CF_RESERVED		0x00001000L	/* Not used */
#define PNG_CF_COPYSAFE		0x00000010L	/* Opaque data safe for copying */

/* Chunk types */

#define CHUNK_IDAT	PNG_LONG(  73,  68,  65,  84 )	/* Pixel data */
#define CHUNK_IEND	PNG_LONG(  73,  69,  78,  68 )	/* End of Image */
#define CHUNK_IHDR	PNG_LONG(  73,  72,  68,  82 )	/* Header */
#define CHUNK_PLTE	PNG_LONG(  80,  76,  84,  69 )	/* Palette */
#define CHUNK_bKGD	PNG_LONG(  98,  75,  71,  68 )	/* Background Color */
#define CHUNK_cHRM	PNG_LONG(  99,  72,  82,  77 )
#define CHUNK_gAMA	PNG_LONG( 103,  65,  77,  65 )	/* Gamma */
#define CHUNK_hIST	PNG_LONG( 104,  73,  83,  84 )	/* Histogram */
#define CHUNK_iCCP	PNG_LONG( 105,  67,  67,  80 )
#define CHUNK_iTXt	PNG_LONG( 105,  84,  88, 116 )	/* Text (comments etc.) */
#define CHUNK_oFFs	PNG_LONG( 111,  70,  70, 115 )
#define CHUNK_pCAL	PNG_LONG( 112,  67,  65,  76 )
#define CHUNK_pHYs	PNG_LONG( 112,  72,  89, 115 )
#define CHUNK_sBIT	PNG_LONG( 115,  66,  73,  84 )
#define CHUNK_sCAL	PNG_LONG( 115,  67,  65,  76 )
#define CHUNK_sPLT	PNG_LONG( 115,  80,  76,  84 )
#define CHUNK_sRGB	PNG_LONG( 115,  82,  71,  66 )
#define CHUNK_tEXt	PNG_LONG( 116,  69,  88, 116 )	/* More text */
#define CHUNK_tIME	PNG_LONG( 116,  73,  77,  69 )	/* Time stamp */
#define CHUNK_tRNS	PNG_LONG( 116,  82,  78,  83 )	/* Transparency */
#define CHUNK_zTXt	PNG_LONG( 122,  84,  88, 116 )	/* More text */

/* Color flags */

#define PNG_COLOR_INDEXED		1
#define PNG_COLOR_USED			2
#define PNG_COLOR_ALPHA			4

/* Actual color types */

#define PNG_COLOR_GRAY			0
#define PNG_COLOR_RGB			(PNG_COLOR_USED)
#define PNG_COLOR_PLTE			(PNG_COLOR_USED | PNG_COLOR_INDEXED)
#define PNG_COLOR_GRAYALPHA		(PNG_COLOR_GRAY | PNG_COLOR_ALPHA)
#define PNG_COLOR_RGBA			(PNG_COLOR_USED | PNG_COLOR_ALPHA)

/* Compression Methods */

#define PNG_COMPRESS_DEFLATE	0

/* Filter Methods */

#define PNG_FILTMETH_STANDARD	0

/* Interlacing Methods */

#define	PNG_INTERLACE_NONE		0
#define PNG_INTERLACE_ADAM7		1

#define	PNG_ENCODE	0
#define PNG_DECODE	1

typedef struct
{
	Byte	mRed;
	Byte	mGrn;
	Byte	mBlu;
	Byte	mAlpha;
} PNG_RGBA;

/* State information */

typedef struct
{
	/* PNG Data Source/Destination channel/object/byte array */

	Tcl_Channel		mFile;			/* Channel for from-file reads */
	Tcl_Obj*		mpObjData;
	Byte*			mpStrData;		/* Raw source data for from-string reads */
	int				mStrDataSz;		/* Length of source data */
	Byte*			mpBase64Data;	/* base64 encoded string data */
	Byte			mBase64Bits;	/* Remaining bits from last base64 read */
	Byte			mBase64State;	/* Current state of base64 decoder */

	uLong			mChunks;		/* Number of chunks read */

	/* State information for zlib compression/decompression */

	z_stream		mZStream;		/* Zlib inflate/deflate stream state */
	int				mZStreamInit;	/* Stream has been initialized */
	int				mZStreamDir;	/* PNG_ENCODE/PNG_DECODE */

	/* Image Header Information */

	uLong			mWidth;			/* Width of the PNG image in pixels */
	uLong			mHeight;		/* Height of the PNG image in pixels */
	Byte			mBitDepth;		/* Number of bits per pixel */
	Byte			mColorType;		/* Grayscale, TrueColor, etc. */
	Byte			mCompression;	/* Compression Mode (always zlib) */
	Byte			mFilter;		/* Filter mode (0 - 3) */
	Byte			mInterlace;		/* Type of interlacing (if any) */

	Byte			mChannels;		/* Number of channels per pixel */
	Byte			mPixelDepth;	/* Number of total bits per pixel */
	Byte			mBPP;			/* Bytes per pixel in scan line */

	uLong			mCurrLine;		/* Current line being unfiltered */
	Byte			mPhase;			/* Interlacing phase (0..6) */

	Tk_PhotoImageBlock	mBlock;
	uLong				mBlockSz;		/* Number of bytes in Tk image pixels */

	/* PLTE Palette and tRNS Transparency Entries */

	int				mPalEntries;	/* Number of PLTE entries (1..256) */
	int				mUseTRNS;
	PNG_RGBA		mpPalette[256];	/* Palette RGB/Transparency table */
	Byte			mpTrans[6];		/* Fully-transparent RGB/Gray Value */

	/* PNG and Tk Photo pixel data */

	Byte*			mpLastLine;		/* Last line of pixels, for unfiltering */
	Byte*			mpThisLine;		/* Current line of pixels to process */
	uLong			mLineSz;		/* Number of bytes in a PNG line */
	uLong			mPhaseSz;		/* Number of bytes/line in current phase */
} PNGImage;


/*
 *----------------------------------------------------------------------
 *
 * PNGZAlloc --
 *
 *		This function is invoked by zlib to allocate memory it needs.
 *
 * Results:
 *		A pointer to the allocated buffer or NULL if allocation fails.
 *
 * Side effects:
 *		Memory is allocated.
 *
 *----------------------------------------------------------------------
 */

static
voidpf PNGZAlloc(voidpf opaque, uInt items, uInt itemSz)
{
	uLong	blockSz	= items * itemSz;
	void*	pBlock	= attemptckalloc(blockSz);

	if (!pBlock)
		return Z_NULL;

	memset(pBlock, 0, blockSz);

	return pBlock;
}


/*
 *----------------------------------------------------------------------
 *
 * PNGZFree --
 *
 *		This function is invoked by zlib to free memory it previously
 *		allocated using PNGZAlloc.
 *
 * Results:
 *		None.
 *
 * Side effects:
 *		Memory is freed.
 *
 *----------------------------------------------------------------------
 */

static
void PNGZFree(voidpf opaque, voidpf ptr)
{
	if (ptr) ckfree((char *)ptr);
}


/*
 *----------------------------------------------------------------------
 *
 * PNGInit --
 *
 *		This function is invoked by each of the Tk image handler
 *		procs (MatchStringProc, etc.) to initialize state information
 *		used during the course of encoding or decoding an PNG image.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if initialization failed.
 *
 * Side effects:
 *		The reference count of the -data Tcl_Obj*, if any, is
 *		incremented.
 *
 *----------------------------------------------------------------------
 */

static int
PNGInit(Tcl_Interp* interp, PNGImage* pPNG,
	Tcl_Channel chan, Tcl_Obj* pObj, int dir)
{
	int ret;

	memset(pPNG, 0, sizeof(*pPNG));

	pPNG -> mFile = chan;

	/*
	 * If decoding from a -data string object, increment its reference
	 * count for the duration of the decode and get its length and
	 * byte array for reading with PNGRead().
	 */

	if (pObj)
	{
		Tcl_IncrRefCount(pObj);
		pPNG -> mpObjData = pObj;
		pPNG -> mpStrData = Tcl_GetByteArrayFromObj(pObj,
			&pPNG->mStrDataSz);
	}

	/* Initialize the palette transparency table to fully opaque */

	memset(pPNG -> mpPalette, 0xff, sizeof(pPNG -> mpPalette));

	/* Initialize Zlib inflate/deflate stream */

	pPNG -> mZStream.zalloc	= PNGZAlloc;	/* Memory allocation */
	pPNG -> mZStream.zfree	= PNGZFree;		/* Memory deallocation */

	if (PNG_DECODE == dir) {
		ret = inflateInit(&pPNG -> mZStream);
	} else {
		ret = deflateInit(&pPNG -> mZStream, Z_DEFAULT_COMPRESSION);
	}

	/* Make sure that Zlib stream initialization was successful */

	if (Z_OK != ret)
	{
		if (pPNG -> mZStream.msg)
			Tcl_SetResult(interp, pPNG -> mZStream.msg, TCL_VOLATILE);
		else
			Tcl_SetResult(interp, "zlib initialization failed", TCL_STATIC);

		return TCL_ERROR;
	}

	pPNG -> mZStreamInit = 1;

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * PNGCleanup --
 *
 *		This function is invoked by each of the Tk image handler
 *		procs (MatchStringProc, etc.) prior to returning to Tcl
 *		in order to clean up any allocated memory and call other
 *		other cleanup handlers such as zlib's inflateEnd/deflateEnd.
 *
 * Results:
 *		None.
 *
 * Side effects:
 *		The reference count of the -data Tcl_Obj*, if any, is
 *		decremented.  Buffers are freed, zstreams are closed.
 *
 *----------------------------------------------------------------------
 */

static void
PNGCleanup(PNGImage* pPNG)
{
	/* Don't need the object containing the -data .. data anymore. */

	if (pPNG -> mpObjData)
	{
		Tcl_DecrRefCount(pPNG -> mpObjData);
	}
	
	/* Discard pixel buffer */

	if (pPNG -> mZStreamInit)
	{
		if (PNG_ENCODE == pPNG -> mZStreamDir)
			inflateEnd(&pPNG -> mZStream);
		else
			deflateEnd(&pPNG -> mZStream);
	}

	if (pPNG -> mBlock.pixelPtr)
		ckfree((char *)pPNG -> mBlock.pixelPtr);
	if (pPNG -> mpThisLine)
		ckfree((char *)pPNG -> mpThisLine);
	if (pPNG -> mpLastLine)
		ckfree((char *)pPNG -> mpLastLine);
}


/*
 *----------------------------------------------------------------------
 *
 * char64 --
 *
 *		This procedure converts a base64 ascii character into its binary
 *		equivalent.  This code is a slightly modified version of the
 *		char64 proc in N. Borenstein's metamail decoder.
 *
 * Results:
 *		The binary value, or an error code.
 *
 * Side effects:
 *		None.
 *----------------------------------------------------------------------
 */

#define PNG64_SPECIAL     0x80
#define PNG64_SPACE       0x80
#define PNG64_PAD         0x81
#define PNG64_DONE        0x82
#define PNG64_BAD         0x83

static Byte gspFrom64[] =
{
	0x82, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x80, 0x80, 0x83,
	0x80, 0x80, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x80, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x3e, 0x83, 0x83, 0x83, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x83, 0x83,
	0x83, 0x81, 0x83, 0x83, 0x83, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
	0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
	0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
	0x31, 0x32, 0x33, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83
};


/*
 *----------------------------------------------------------------------
 *
 * PNGRead --
 *
 *		This function is invoked to read the specified number of bytes
 *		from the image file or data.  It is a wrapper around the
 *		choice of byte array Tcl_Obj or Tcl_Channel which depends on
 *		whether the image data is coming from a file or -data.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs.
 *
 * Side effects:
 *		The file position will change.  The running CRC is updated
 *		if a pointer to it is provided.
 *
 *----------------------------------------------------------------------
 */

static int
PNGRead(Tcl_Interp* interp, PNGImage* pPNG,
	Byte* pDest, uLong destSz, uLong* pCRC)
{
	if (pPNG -> mpBase64Data)
	{
		while (destSz && pPNG -> mStrDataSz)
		{
			Byte	c;
			Byte	c64 = gspFrom64[*pPNG -> mpStrData++];

			pPNG -> mStrDataSz--;

			if (PNG64_SPACE == c64)
				continue;

			if (c64 & PNG64_SPECIAL)
			{
				c = pPNG -> mBase64Bits;
			}
			else
			{
				if (0 == pPNG -> mBase64State)
				{
					pPNG -> mBase64Bits = c64 << 2;
					pPNG -> mBase64State++;
					continue;
				}

				switch (pPNG -> mBase64State++)
				{
				case 1:
					c = (Byte)(pPNG -> mBase64Bits | (c64 >> 4));
					pPNG -> mBase64Bits = (c64 & 0xF) << 4;
					break;
				case 2:
					c = (Byte)(pPNG -> mBase64Bits | (c64 >> 2));
					pPNG -> mBase64Bits = (c64 & 0x3) << 6;
					break;
				case 3:
					c = (Byte)(pPNG -> mBase64Bits | c64);
					pPNG -> mBase64State = 0;
					pPNG -> mBase64Bits = 0;
					break;
				}
			}

			if (pCRC)
				*pCRC = crc32(*pCRC, &c, 1);

			if (pDest)
				*pDest++ = c;

			destSz--;

			if (c64 & PNG64_SPECIAL)
				break;
		}

		if (destSz)
		{
			Tcl_SetResult(interp, "Unexpected end of image data", TCL_STATIC);
			return TCL_ERROR;
		}
	}
	else
	if (pPNG -> mpStrData)
	{
		if (pPNG -> mStrDataSz < destSz)
		{
			Tcl_SetResult(interp, "Unexpected end of image data", TCL_STATIC);
			return TCL_ERROR;
		}

		if (pDest)
			memcpy(pDest, pPNG -> mpStrData, destSz);

		pPNG -> mpStrData		+= destSz;
		pPNG -> mStrDataSz	-= destSz;

		if (pCRC)
			*pCRC = crc32(*pCRC, pDest, destSz);
	}
	else
	{
		while (destSz)
		{
			int	 blockSz = Tcl_Read(pPNG -> mFile, (char *)pDest, destSz);

			if (blockSz < 0)
			{
				/* TODO: failure info... */
				Tcl_SetResult(interp, "Channel read failed", TCL_STATIC);
				return TCL_ERROR;
			}

			if (blockSz)
			{
				if (pCRC)
					*pCRC = crc32(*pCRC, pDest, blockSz);

				pDest += blockSz;
				destSz -= blockSz;
			}

			if (destSz && Tcl_Eof(pPNG -> mFile))
			{
				Tcl_SetResult(interp, "Unexpected end of file ", TCL_STATIC);
				return TCL_ERROR;
			}
		}
	}

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * PNGReadLong --
 *
 *		This function is invoked to read a 4-byte long integer in
 *		network byte order from the image data and return the value in
 *		host byte order.
 *
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs.
 *
 * Side effects:
 *		The file position will change.  The running CRC is updated
 *		if a pointer to it is provided.
 *
 *----------------------------------------------------------------------
 */

static int
PNGReadLong(Tcl_Interp* interp, PNGImage* pPNG, uLong* pResult, uLong* pCRC)
{
	Byte p[4];

	if (PNGRead(interp, pPNG, p, 4, pCRC) == TCL_ERROR)
		return TCL_ERROR;

	*pResult = PNG_LONG(p[0],p[1],p[2],p[3]);

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * CheckCRC --
 *
 *		This function is reads the final 4-byte integer CRC from a
 *		chunk and compares it to the running CRC calculated over the
 *		chunk type and data fields.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error or CRC mismatch occurs.
 *
 * Side effects:
 *		The file position will change.
 *
 *----------------------------------------------------------------------
 */

static
int CheckCRC(Tcl_Interp* interp, PNGImage* pPNG, uLong calculated)
{
	uLong	chunked;

	/* Read the CRC field at the end of the chunk */

	if (PNGReadLong(interp, pPNG, &chunked, NULL) == TCL_ERROR)
		return TCL_ERROR;

	/* Compare the read CRC to what we calculate to make sure they match. */

	if (calculated != chunked)
	{
//		Tcl_SetResult(interp, "CRC check failed", TCL_STATIC);
//		return TCL_ERROR;
	}

	/* Done parsing this chunk. Reset the CRC counter for the next chunk. */

	pPNG -> mChunks++;

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * SkipChunk --
 *
 *		This function is used to skip a PNG chunk that is not used
 *		by this implementation.  Given the input stream has had the
 *		chunk length and chunk type fields already read, this function
 *		will read the number of bytes indicated by the chunk length,
 *		plus four for the CRC, and will verify that CRC is
 *		correct for the skipped data.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error or CRC mismatch occurs.
 *
 * Side effects:
 *		The file position will change.
 *
 *----------------------------------------------------------------------
 */

static int
SkipChunk(Tcl_Interp* interp, PNGImage* pPNG, uLong chunkSz, uLong crc)
{
	Byte	pBuffer[PNG_BLOCK_SZ];

	/*
	 * Skip data in blocks until none is left.  Read up to PNG_BLOCK_SZ
	 * bytes at a time, rather than trusting the claimed chunk size,
	 * which may not be trustworthy.
	 */

	while (chunkSz)
	{
		int blockSz = (chunkSz > PNG_BLOCK_SZ) ? PNG_BLOCK_SZ : chunkSz;

		if (PNGRead(interp, pPNG, pBuffer, blockSz, &crc) == TCL_ERROR)
			return TCL_ERROR;

		chunkSz -= blockSz;
	}

	if (CheckCRC(interp, pPNG, crc) == TCL_ERROR)
		return TCL_ERROR;

	return TCL_OK;
}


/*
4.3. Summary of standard chunks

This table summarizes some properties of the standard chunk types.

   Critical chunks (must appear in this order, except PLTE
					is optional):

		   Name  Multiple  Ordering constraints
				   OK?

		   IHDR	No	  Must be first
		   PLTE	No	  Before IDAT
		   IDAT	Yes	 Multiple IDATs must be consecutive
		   IEND	No	  Must be last

   Ancillary chunks (need not appear in this order):

		   Name  Multiple  Ordering constraints
				   OK?

		   cHRM	No	  Before PLTE and IDAT
		   gAMA	No	  Before PLTE and IDAT
		   iCCP	No	  Before PLTE and IDAT
		   sBIT	No	  Before PLTE and IDAT
		   sRGB	No	  Before PLTE and IDAT
		   bKGD	No	  After PLTE; before IDAT
		   hIST	No	  After PLTE; before IDAT
		   tRNS	No	  After PLTE; before IDAT
		   pHYs	No	  Before IDAT
		   sPLT	Yes	 Before IDAT
		   tIME	No	  None
		   iTXt	Yes	 None
		   tEXt	Yes	 None
		   zTXt	Yes	 None

	[From the PNG specification.]
*/


/*
 *----------------------------------------------------------------------
 *
 * ReadChunkHeader --
 *
 *		This function is used at the start of each chunk to extract
 *		the four-byte chunk length and four-byte chunk type fields.
 *		It will continue reading until it finds a chunk type that is
 *		handled by this implementation, checking the CRC of any chunks
 *		it skips.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs or an unknown
 *		critical chunk type is encountered.
 *
 * Side effects:
 *		The file position will change.  The running CRC is updated.
 *
 *----------------------------------------------------------------------
 */

static int
ReadChunkHeader(Tcl_Interp* interp, PNGImage* pPNG,
	uLong* pSize, uLong* pType, uLong* pCRC)
{
	uLong	chunkType	= 0;
	uLong	chunkSz		= 0;
	uLong	crc;
	Byte	pc[4];

	/* Continue until finding a chunk type that is handled. */

	while (!chunkType)
	{
		int i;

		/*
		 * Read the 4-byte length field for the chunk, which is not
		 * included in the CRC calculation.
		 */

		if (PNGReadLong(interp, pPNG, &chunkSz, NULL) == TCL_ERROR)
			return TCL_ERROR;

		/* Read the 4-byte chunk type */

		crc = crc32(0, NULL, 0);

		if (PNGRead(interp, pPNG, pc, 4, &crc) == TCL_ERROR)
			return TCL_ERROR;

		/* Convert it to a host-order long for integer comparison */

		chunkType = PNG_LONG(pc[0], pc[1], pc[2], pc[3]);

		/*
		 * Check to see if this is a known/supported chunk type.  Note
		 * that the PNG specs require non-critical (i.e., ancillary)
		 * chunk types that are not recognized to be ignored, rather
		 * than be treated as an error.  It does, however, recommend
		 * that an unknown critical chunk type be treated as a failure.
		 *
		 * This switch/loop acts as a filter of sorts for undesired
		 * chunk types.  The chunk type should still be checked
		 * elsewhere for determining it is in the correct order.
		 */

		switch (chunkType)
		{
		case CHUNK_IDAT:
		case CHUNK_IEND:
		case CHUNK_IHDR:
		case CHUNK_PLTE:
		case CHUNK_tRNS:
			break;

		/*
		 * These chunk types are part of the standard, but are not used by
		 * this implementation (at least not yet).  Note that these are
		 * all ancillary chunks (nowercase first letter).
		 */

		case CHUNK_bKGD:
		case CHUNK_cHRM:
		case CHUNK_gAMA:
		case CHUNK_hIST:
		case CHUNK_iCCP:
		case CHUNK_iTXt:
		case CHUNK_oFFs:
		case CHUNK_pCAL:
		case CHUNK_pHYs:
		case CHUNK_sBIT:
		case CHUNK_sCAL:
		case CHUNK_sPLT:
		case CHUNK_sRGB:
		case CHUNK_tEXt:
		case CHUNK_tIME:
		case CHUNK_zTXt:
			/* TODO: might want to check order here. */
			if (SkipChunk(interp, pPNG, chunkSz, crc) == TCL_ERROR)
				return TCL_ERROR;

			chunkType = 0;
			break;

		default:
			/* Unknown chunk type. If it's critical, we can't continue. */

			if (!(chunkType & PNG_CF_ANCILLARY))
			{
				Tcl_SetResult(interp,
					"Encountered an unsupported criticial chunk type",
					TCL_STATIC);
				return TCL_ERROR;
			}

			/* Check to see if the chunk type has legal bytes */

			for (i = 0 ; i < 4 ; i++)
			{
				if ((pc[i] < 65) || (pc[i] > 122) ||
					((pc[i] > 90) && (pc[i] < 97)))
				{
					Tcl_SetResult(interp, "Invalid chunk type", TCL_STATIC);
					return TCL_ERROR;
				}
			}

			/*
			 * It seems to be an otherwise legally labelled ancillary chunk
			 * that we don't want, so skip it after at least checking its CRC.
			 */

			if (SkipChunk(interp, pPNG, chunkSz, crc) == TCL_ERROR)
				return TCL_ERROR;

			chunkType = 0;
		}
	}

	/*
	 * Found a known chunk type that's handled, albiet possibly not in
	 * the right order.  Send back the chunk type (for further checking
	 * or handling), the chunk size and the current CRC for the rest
	 * of the calculation.
	 */

	*pType	= chunkType;
	*pSize	= chunkSz;
	*pCRC	= crc;

	return TCL_OK;
}

static int
PNGCheckColor(Tcl_Interp* interp, PNGImage* pPNG)
{
	int result = TCL_OK;
	int	offset;

	/* Verify the color type is valid and the bit depth is allowed */

//	if (pPNG -> mBitDepth == 16)
//		return TCL_ERROR;

	switch (pPNG -> mColorType)
	{
	case PNG_COLOR_GRAY:
		pPNG -> mChannels = 1;
		if ((1 != pPNG->mBitDepth) && (2 != pPNG->mBitDepth) &&
			(4 != pPNG->mBitDepth) && (8 != pPNG->mBitDepth) &&
			(16 != pPNG -> mBitDepth))
			result = TCL_ERROR;
		break;

	case PNG_COLOR_RGB:
		pPNG -> mChannels = 3;
		if ((8 != pPNG->mBitDepth) && (16 != pPNG->mBitDepth))
			result = TCL_ERROR;
		break;

	case PNG_COLOR_PLTE:
		pPNG -> mChannels = 1;
		if ((1 != pPNG->mBitDepth) && (2 != pPNG->mBitDepth) &&
			(4 != pPNG->mBitDepth) && (8 != pPNG->mBitDepth))
				result = TCL_ERROR;
		break;

	case PNG_COLOR_GRAYALPHA:
		pPNG -> mChannels = 2;
		if ((8 != pPNG->mBitDepth) && (16 != pPNG->mBitDepth))
			result = TCL_ERROR;
		break;

	case PNG_COLOR_RGBA:
		pPNG -> mChannels = 4;
		if ((8 != pPNG->mBitDepth) && (16 != pPNG->mBitDepth))
			result = TCL_ERROR;
		break;

	default:
		Tcl_SetResult(interp, "Unknown Color Type field", TCL_STATIC);
		return TCL_ERROR;
	}

	if (TCL_ERROR == result)
	{
		Tcl_SetResult(interp, "Bit depth is not allowed for given color type",
			TCL_STATIC);
		return TCL_ERROR;
	}

	pPNG -> mPixelDepth = pPNG -> mChannels * pPNG -> mBitDepth;

	/*
	 * Set up the Tk photo block's pixel size and channel offsets.
	 * offset array elements should already be 0 from the memset
	 * during PNGInit().
	 */

	offset = (pPNG -> mBitDepth > 8) ? 2 : 1;

	if (pPNG -> mColorType & PNG_COLOR_USED)
	{
		pPNG -> mBlock.pixelSize	= offset * 4;
		pPNG -> mBlock.offset[1]	= offset;
		pPNG -> mBlock.offset[2]	= offset * 2;
		pPNG -> mBlock.offset[3]	= offset * 3;
	}
	else
	{
		pPNG -> mBlock.pixelSize	= offset * 2;
		pPNG -> mBlock.offset[3]	= offset;
	}

	pPNG -> mBlock.pitch= pPNG -> mBlock.pixelSize * pPNG -> mBlock.width;
	pPNG -> mBlockSz	= pPNG -> mBlock.height * pPNG -> mBlock.pitch;

	switch (pPNG -> mColorType)
	{
	case PNG_COLOR_GRAY:
		pPNG -> mBPP = (pPNG -> mBitDepth > 8) ? 2 : 1;
		break;
	case PNG_COLOR_RGB:
		pPNG -> mBPP = (pPNG -> mBitDepth > 8) ? 6 : 3;
		break;
	case PNG_COLOR_PLTE:
		pPNG -> mBPP = 1;
		break;
	case PNG_COLOR_GRAYALPHA:
		pPNG -> mBPP = (pPNG -> mBitDepth > 8) ? 4 : 2;
		break;
	case PNG_COLOR_RGBA:
		pPNG -> mBPP = (pPNG -> mBitDepth > 8) ? 8 : 4;
		break;
	default:
		Tcl_SetResult(interp, "internal error - unknown color type",
			TCL_STATIC);
		return TCL_ERROR;
	}

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * ReadIHDR --
 *
 *		This function reads the PNG header from the beginning of a
 *		PNG file and returns the dimensions of the image.
 *
 * Results:
 *		The return value is 1 if file "f" appears to start with
 *		a valid PNG header, 0 otherwise.  If the header is valid,
 *		then *widthPtr and *heightPtr are modified to hold the
 *		dimensions of the image.
 *
 * Side effects:
 *		The access position in f advances.
 *
 *----------------------------------------------------------------------
 */

static int
ReadIHDR(Tcl_Interp* interp, PNGImage* pPNG)
{
	Byte	pSig[gsSigSz];
	uLong	chunkType;
	uLong	chunkSz;
	uLong	crc;
	int		mismatch;

	/* Read the appropriate number of bytes for the PNG signature */

	if (PNGRead(interp, pPNG, pSig, gsSigSz, NULL) == TCL_ERROR)
		return TCL_ERROR;

	/* Compare the read bytes to the expected signature. */

	mismatch = memcmp(pSig, gspPNGSignature, gsSigSz);

	/* If reading from string, reset position and try base64 decode */

	if (mismatch && pPNG -> mpStrData)
	{
		pPNG -> mpStrData = Tcl_GetByteArrayFromObj(pPNG -> mpObjData,
			&pPNG -> mStrDataSz);
		pPNG -> mpBase64Data = pPNG -> mpStrData;

		if (PNGRead(interp, pPNG, pSig, gsSigSz, NULL) == TCL_ERROR)
			return TCL_ERROR;

		mismatch = memcmp(pSig, gspPNGSignature, gsSigSz);
	}

	if (mismatch)
	{
		Tcl_SetResult(interp, "Data stream does not have a PNG signature",
			TCL_STATIC);
		return TCL_ERROR;
	}

	if (ReadChunkHeader(interp, pPNG, &chunkSz, &chunkType,
			&crc) == TCL_ERROR)
		return TCL_ERROR;

	/* Read in the IHDR (header) chunk for width, height, etc. */
	/* The first chunk in the file must be the IHDR (headr) chunk */

	if (chunkType != CHUNK_IHDR)
	{
		Tcl_SetResult(interp, "Expected IHDR chunk type", TCL_STATIC);
		return TCL_ERROR;
	}

	if (chunkSz != 13)
	{
		Tcl_SetResult(interp, "Invalid IHDR chunk size", TCL_STATIC);
		return TCL_ERROR;
	}

	/* Read and verify the image width to be sure we can handle it */

	if (PNGReadLong(interp, pPNG, &pPNG->mWidth, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (!pPNG -> mWidth || (pPNG -> mWidth > INT_MAX))
	{
		Tcl_SetResult(interp,
			"Image width is invalid or out of supported range",
			TCL_STATIC);
		return TCL_ERROR;
	}

	/* Read and verify the image height to be sure we can handle it */

	if (PNGReadLong(interp, pPNG, &pPNG->mHeight, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (!pPNG -> mHeight || (pPNG -> mHeight > INT_MAX))
	{
		Tcl_SetResult(interp,
			"Image height is invalid or out of supported range",
			TCL_STATIC);
		return TCL_ERROR;
	}

	/* Set height and width for the Tk photo block */

	pPNG -> mBlock.width = (int)pPNG -> mWidth;
	pPNG -> mBlock.height = (int)pPNG -> mHeight;

	/* Read and the Bit Depth and Color Type */

	if (PNGRead(interp, pPNG, &pPNG->mBitDepth, 1, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (PNGRead(interp, pPNG, &pPNG->mColorType, 1, &crc) == TCL_ERROR)
		return TCL_ERROR;

	/*
	 * Verify that the color type is valid, the bit depth is allowed
	 * for the color type, and calculate the number of channels and
	 * pixel depth (bits per pixel * channels).  Also set up offsets
	 * and sizes in the Tk photo block for the pixel data.
	 */

	if (PNGCheckColor(interp, pPNG) == TCL_ERROR)
		return TCL_ERROR;

	/* Only one compression method is currently defined by the standard */

	if (PNGRead(interp, pPNG, &pPNG->mCompression, 1, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (PNG_COMPRESS_DEFLATE != pPNG -> mCompression)
	{
		Tcl_SetResult(interp, "Unknown compression method", TCL_STATIC);
		return TCL_ERROR;
	}

	/*
	 * Only one filter method is currently defined by the standard; the
	 * method has five actual filter types associated with it.
	 */

	if (PNGRead(interp, pPNG, &pPNG->mFilter, 1, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (PNG_FILTMETH_STANDARD != pPNG -> mFilter)
	{
		Tcl_SetResult(interp, "Unknown filter method", TCL_STATIC);
		return TCL_ERROR;
	}

	if (PNGRead(interp, pPNG, &pPNG->mInterlace, 1, &crc) == TCL_ERROR)
		return TCL_ERROR;

	switch (pPNG -> mInterlace)
	{
	case PNG_INTERLACE_NONE:
	case PNG_INTERLACE_ADAM7:
		break;

	default:
		Tcl_SetResult(interp, "Unknown interlace method", TCL_STATIC);
		return TCL_ERROR;
	}

	return CheckCRC(interp, pPNG, crc);
}


/*
 *----------------------------------------------------------------------
 *
 * ReadPLTE --
 *
 *		This function reads the PLTE (indexed color palette) chunk
 *		data from the PNG file and populates the palette table in the
 *		PNGImage structure.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs or the PLTE
 *		chunk is invalid.
 *
 * Side effects:
 *		The access position in f advances.
 *
 *----------------------------------------------------------------------
 */

static int
ReadPLTE(Tcl_Interp* interp, PNGImage* pPNG, uLong chunkSz,
	uLong chunkType, uLong crc)
{
	Byte	pBuffer[768];
	int		i, c;

	/* This chunk is mandatory for color type 3 and forbidden for 2 and 6 */

	switch (pPNG -> mColorType)
	{
	case PNG_COLOR_GRAY:
	case PNG_COLOR_GRAYALPHA:
		Tcl_SetResult(interp, "PLTE chunk type forbidden for grayscale",
			TCL_STATIC);
		return TCL_ERROR;

	default:
		break;
	}

	/*
	 * The palette chunk contains from 1 to 256 palette entries.
	 * Each entry consists of a 3 byte RGB value.  It must therefore
	 * contain a non-zero multiple of 3 bytes, up to 768.
	 */

	if (!chunkSz || (chunkSz > 768) || (chunkSz % 3))
	{
		Tcl_SetResult(interp, "Invalid palette chunk size", TCL_STATIC);
		return TCL_ERROR;
	}

	/* Read the palette contents and stash them for later, possibly */

	if (PNGRead(interp, pPNG, pBuffer, chunkSz, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (CheckCRC(interp, pPNG, crc) == TCL_ERROR)
		return TCL_ERROR;

	/* Stash away the number of palette entries and start the next chunk. */

	pPNG -> mPalEntries = chunkSz / 3;

	for (i = 0, c = 0 ; c < chunkSz ; i++)
	{
		pPNG -> mpPalette[i].mRed = pBuffer[c++];
		pPNG -> mpPalette[i].mGrn = pBuffer[c++];
		pPNG -> mpPalette[i].mBlu = pBuffer[c++];
	}

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * ReadtRNS --
 *
 *		This function reads the tRNS (transparency) chunk data from the
 *		PNG file and populates the alpha field of the palette table in
 *		the PNGImage structure or the single color transparency, as
 *		appropriate for the color type.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs or the tRNS
 *		chunk is invalid.
 *
 * Side effects:
 *		The access position in f advances.
 *
 *----------------------------------------------------------------------
 */

static int
ReadtRNS(Tcl_Interp* interp, PNGImage* pPNG, uLong chunkSz, uLong crc)
{
	Byte	pBuffer[256];
	int		i;

	/* First a sanity check to prevent buffer overrun */

	if (chunkSz > 256)
	{
		Tcl_SetResult(interp, "Invalid tRNS chunk size", TCL_STATIC);
		return TCL_ERROR;
	}

	/* Read in the raw transparency information */

	if (PNGRead(interp, pPNG, pBuffer, chunkSz, &crc) == TCL_ERROR)
		return TCL_ERROR;

	if (CheckCRC(interp, pPNG, crc) == TCL_ERROR)
		return TCL_ERROR;

	if (pPNG -> mColorType & PNG_COLOR_ALPHA)
	{
		Tcl_SetResult(interp,
			"tRNS chunk not allowed color types with a full alpha channel",
			TCL_STATIC);
		return TCL_ERROR;
	}

	switch (pPNG -> mColorType)
	{
	case PNG_COLOR_GRAYALPHA:
	case PNG_COLOR_RGBA:
		break;

	case PNG_COLOR_PLTE:
		/*
		 * The number of tRNS entries must be less than or equal to
		 * the number of PLTE entries, and consists of a single-byte
		 * alpha level for the corresponding PLTE entry.
		 */
		if ((chunkSz / 3) > pPNG -> mPalEntries)
		{
			Tcl_SetResult(interp,
				"Size of tRNS chunk is too large for the palette",
				TCL_STATIC);
			return TCL_ERROR;
		}

		for (i = 0 ; i < chunkSz ; i++)
		{
			pPNG -> mpPalette[i].mAlpha = pBuffer[i];
		}
		break;

	case PNG_COLOR_GRAY:
		/*
		 * Grayscale uses a single 2-byte gray level, which we'll
		 * store in palette index 0, since we're not using the palette.
		 */

		if (chunkSz != 2)
		{
			Tcl_SetResult(interp,
				"Invalid tRNS chunk size - must 2 bytes for grayscale",
				TCL_STATIC);
			return TCL_ERROR;
		}

		/*
		 * According to the PNG specs, if the bit depth is less than 16,
		 * then only the lower byte is used.
		 */

		if (16 == pPNG -> mBitDepth)
		{
			pPNG -> mpTrans[0] = pBuffer[0];
			pPNG -> mpTrans[1] = pBuffer[1];
		}
		else
		{
			pPNG -> mpTrans[0] = pBuffer[1];
		}
		pPNG -> mUseTRNS = 1;
		break;

	case PNG_COLOR_RGB:
		/* TrueColor uses a single RRGGBB triplet. */

		if (chunkSz != 6)
		{
			Tcl_SetResult(interp,
				"Invalid tRNS chunk size - must 2 bytes for grayscale",
				TCL_STATIC);
			return TCL_ERROR;
		}

		/*
		 * According to the PNG specs, if the bit depth is less than 16,
		 * then only the lower byte is used.
		 */

		if (16 == pPNG -> mBitDepth)
		{
			memcpy(pPNG -> mpTrans, pBuffer, 6);
		}
		else
		{
			pPNG -> mpTrans[0] = pBuffer[1];
			pPNG -> mpTrans[1] = pBuffer[3];
			pPNG -> mpTrans[2] = pBuffer[5];
		}
		pPNG -> mUseTRNS = 1;
		break;
	}

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * DecodeLine --
 *
 *
 * Results:
 *
 * Side effects:
 *
 *----------------------------------------------------------------------
 */

#define	PNG_FILTER_NONE		0
#define	PNG_FILTER_SUB		1
#define	PNG_FILTER_UP		2
#define	PNG_FILTER_AVG		3
#define	PNG_FILTER_PAETH	4

static Byte
Paeth(int a, int b, int c)
{
	int		pa	= abs(b - c);
	int		pb	= abs(a - c);
	int		pc	= abs(a + b - c - c);

	if ((pa <= pb) && (pa <= pc))
		return (Byte)a;

	if (pb <= pc)
		return (Byte)b;

	return (Byte)c;
}

static int
UnfilterLine(Tcl_Interp* interp, PNGImage* pPNG)
{
	switch (*pPNG->mpThisLine)
	{
	case PNG_FILTER_NONE:	/* Nothing to do */
		break;
	case PNG_FILTER_SUB:	/* Sub(x) = Raw(x) - Raw(x-bpp) */
		{
			Byte*	pRaw_bpp= pPNG -> mpThisLine + 1;
			Byte*	pRaw	= pRaw_bpp + pPNG -> mBPP;
			Byte*	pEnd	= pPNG->mpThisLine + pPNG->mPhaseSz;

			while (pRaw < pEnd)
			{
				*pRaw++ += *pRaw_bpp++;
			}
		}
		break;
	case PNG_FILTER_UP:		/* Up(x) = Raw(x) - Prior(x) */
		if (pPNG -> mCurrLine > gspStartLine[pPNG -> mPhase])
		{
			Byte*	pPrior	= pPNG -> mpLastLine + 1;
			Byte*	pRaw	= pPNG -> mpThisLine + 1;
			Byte*	pEnd	= pPNG->mpThisLine + pPNG->mPhaseSz;

			while (pRaw < pEnd)
			{
				*pRaw++ += *pPrior++;
			}
		}
		break;
	case PNG_FILTER_AVG:
		/* Avg(x) = Raw(x) - floor((Raw(x-bpp)+Prior(x))/2) */
		if (pPNG -> mCurrLine > gspStartLine[pPNG -> mPhase])
		{
			Byte*	pPrior	= pPNG -> mpLastLine + 1;
			Byte*	pRaw_bpp= pPNG -> mpThisLine + 1;
			Byte*	pRaw	= pRaw_bpp;
			Byte*	pEnd	= pPNG->mpThisLine + pPNG->mPhaseSz;
			Byte*	pEnd2	= pRaw + pPNG -> mBPP;

			while ((pRaw < pEnd2) && (pRaw < pEnd))
			{
				*pRaw++ += (*pPrior++/2);
			}

			while (pRaw < pEnd)
			{
				*pRaw++ += (Byte)(((int)*pRaw_bpp++ + (int)*pPrior++)/2);
			}
		}
		else
		{
			Byte*	pRaw_bpp= pPNG -> mpThisLine + 1;
			Byte*	pRaw	= pRaw_bpp + pPNG -> mBPP;
			Byte*	pEnd	= pPNG->mpThisLine + pPNG->mPhaseSz;

			while (pRaw < pEnd)
			{
				*pRaw++ += (*pRaw_bpp++/2);
			}
		}
		break;
	case PNG_FILTER_PAETH:
		/* Paeth(x) = Raw(x) - PaethPredictor(Raw(x-bpp), Prior(x),
				Prior(x-bpp)) */
		if (pPNG -> mCurrLine > gspStartLine[pPNG -> mPhase])
		{
			Byte*	pPrior_bpp	= pPNG -> mpLastLine + 1;
			Byte*	pPrior		= pPrior_bpp;
			Byte*	pRaw_bpp	= pPNG -> mpThisLine + 1;
			Byte*	pRaw		= pRaw_bpp;
			Byte*	pEnd		= pPNG->mpThisLine + pPNG->mPhaseSz;
			Byte*	pEnd2		= pRaw_bpp + pPNG -> mBPP;

			while ((pRaw < pEnd) && (pRaw < pEnd2))
			{
				*pRaw++ += *pPrior++;
			}

			while (pRaw < pEnd)
			{
				*pRaw++ += Paeth(*pRaw_bpp++, *pPrior++, *pPrior_bpp++);
			}
		}
		else
		{
			Byte*	pRaw_bpp= pPNG -> mpThisLine + 1;
			Byte*	pRaw	= pRaw_bpp + pPNG -> mBPP;
			Byte*	pEnd	= pPNG->mpThisLine + pPNG->mPhaseSz;

			while (pRaw < pEnd)
			{
				*pRaw++ += *pRaw_bpp++;
			}
		}
		break;
	default:
		Tcl_SetResult(interp, "Invalid filter type", TCL_STATIC);
		return TCL_ERROR;
	}

	return TCL_OK;
}

static int
DecodeLine(Tcl_Interp* interp, PNGImage* pPNG)
{
	Byte*	pixelPtr	= pPNG -> mBlock.pixelPtr;
	int		bitScale	= 1;	/* Scale factor for RGB/Gray bit depths < 8 */
	uLong	colNum		= 0;	/* Current pixel column */
	Byte	chan		= 0;	/* Current channel (0..3) = (R, G, B, A) */
	Byte	readByte	= 0;	/* Current scan line byte */
	int		haveBits	= 0;	/* Number of bits remaining in current byte */
	Byte	pixBits		= 0;	/* Extracted bits for current channel */
	int		shifts		= 0;	/* Number of channels extracted from byte */
	uLong	offset		= 0;	/* Current offset into pixelPtr */
	Byte	colStep		= 1;	/* Column increment each pass */
	Byte	pixStep		= 0;	/* extra pixelPtr increment each pass */
	Byte	pLastPixel[6];
	Byte*	p			= pPNG -> mpThisLine + 1;

	if (UnfilterLine(interp, pPNG) == TCL_ERROR)
		return TCL_ERROR;

	/*
	 * Calculate scale factor for bit depths less than 8, in order to
	 * adjust them to a minimum of 8 bits per pixel in the Tk image.
	 */

	if (pPNG -> mBitDepth < 8)
		bitScale = 255/(pow(2, pPNG -> mBitDepth)-1);

	if (pPNG -> mInterlace)
	{
		switch (pPNG -> mPhase)
		{
		case 1:				/* Phase 1: */
			colStep	= 8;	/* 1 pixel per block of 8 per line */
			break;			/* Start at column 0 */
		case 2:				/* Phase 2: */
			colStep	= 8;	/* 1 pixels per block of 8 per line */
			colNum	= 4;	/* Start at column 4 */
			break;
		case 3:				/* Phase 3: */
			colStep	= 4;	/* 2 pixels per block of 8 per line */
			break;			/* Start at column 0 */
		case 4:				/* Phase 4: */
			colStep	= 4;	/* 2 pixels per block of 8 per line */
			colNum	= 2;	/* Start at column 2 */
			break;
		case 5:				/* Phase 5: */
			colStep	= 2;	/* 4 pixels per block of 8 per line */
			break;			/* Start at column 0 */
		case 6:				/* Phase 6: */
			colStep	= 2;	/* 4 pixels per block of 8 per line */
			colNum	= 1;	/* Start at column 1 */
			break;
							/* Phase 7: */
							/* 8 pixels per block of 8 per line */
							/* Start at column 0 */
		}
	}

	/* Calculate offset into pixelPtr for the first pixel of the line */

	offset	= pPNG -> mCurrLine * pPNG -> mBlock.pitch;

	/* Adjust up for the starting pixel of the line */

	offset += colNum * pPNG -> mBlock.pixelSize;

	/* Calculate the extra number of bytes to skip between columns */

	pixStep = (colStep - 1) * pPNG -> mBlock.pixelSize;

	for ( ; colNum < pPNG -> mWidth ; colNum += colStep)
	{
		if (haveBits < (pPNG -> mBitDepth * pPNG -> mChannels))
			haveBits = 0;

		for (chan = 0 ; chan < pPNG -> mChannels ; chan++)
		{
			if (!haveBits)
			{
				shifts = 0;

				readByte = *p++;

				haveBits += 8;
			}

			if (16 == pPNG -> mBitDepth)
			{
				pPNG->mBlock.pixelPtr[offset++] = readByte;

				if (pPNG -> mUseTRNS)
					pLastPixel[chan * 2] = readByte;

				readByte = *p++;

				if (pPNG -> mUseTRNS)
					pLastPixel[(chan * 2) + 1] = readByte;

				pPNG->mBlock.pixelPtr[offset++] = readByte;

				haveBits = 0;
				continue;
			}

			switch (pPNG -> mBitDepth)
			{
			case 1:
				pixBits = (readByte >> (7 - shifts)) & 0x01;
				break;
			case 2:
				pixBits = (readByte >> (6 - shifts*2)) & 0x03;
				break;
			case 4:
				pixBits = (readByte >> (4 - shifts*4)) & 0x0f;
				break;
			case 8:
				pixBits = readByte;
				break;
			}

			if (PNG_COLOR_PLTE == pPNG -> mColorType)
			{
				pixelPtr[offset++] = pPNG -> mpPalette[pixBits].mRed;
				pixelPtr[offset++] = pPNG -> mpPalette[pixBits].mGrn;
				pixelPtr[offset++] = pPNG -> mpPalette[pixBits].mBlu;
				pixelPtr[offset++] = pPNG -> mpPalette[pixBits].mAlpha;
				chan += 2;
			}
			else
			{
				pixelPtr[offset++] = pixBits * bitScale;

				if (pPNG -> mUseTRNS)
					pLastPixel[chan] = pixBits;
			}

			haveBits -= pPNG -> mBitDepth;
			shifts++;
		}

		/*
		 * Apply boolean transparency via tRNS data if necessary
		 * (where necessary means a tRNS chunk was provided and
		 * we're not using an alpha channel or indexed alpha).
		 */

		if ((PNG_COLOR_PLTE != pPNG -> mColorType) &&
			((pPNG -> mColorType & PNG_COLOR_ALPHA) == 0))
		{
			Byte alpha;

			if (pPNG -> mUseTRNS)
			{
				if (memcmp(pLastPixel, pPNG -> mpTrans, pPNG -> mBPP) == 0)
					alpha = 0x00;
				else
					alpha = 0xff;
			}
			else
			{
				alpha = 0xff;
			}

			pixelPtr[offset++] = alpha;

			if (16 == pPNG -> mBitDepth)
				pixelPtr[offset++] = alpha;
		}

		offset += pixStep;
	}

	if (pPNG -> mInterlace)
	{
		/* Skip lines */

		switch (pPNG -> mPhase)
		{
		case 1: case 2: case 3:
			pPNG -> mCurrLine += 8;
			break;
		case 4: case 5:
			pPNG -> mCurrLine += 4;
			break;
		case 6: case 7:
			pPNG -> mCurrLine += 2;
			break;
		}

		/* Start the next phase if there are no more lines to do */

		if (pPNG -> mCurrLine >= pPNG -> mHeight)
		{
			uLong pixels	= 0;

			while ((!pixels || (pPNG -> mCurrLine >= pPNG -> mHeight)) &&
				(pPNG->mPhase<7))
			{
				pPNG -> mPhase++;

				switch (pPNG -> mPhase)
				{
				case 2:
					pixels = (pPNG -> mWidth + 3) >> 3;
					pPNG -> mCurrLine	= 0;
					break;
				case 3:
					pixels = (pPNG -> mWidth + 3) >> 2;
					pPNG -> mCurrLine	= 4;
					break;
				case 4:
					pixels = (pPNG -> mWidth + 1) >> 2;
					pPNG -> mCurrLine	= 0;
					break;
				case 5:
					pixels = (pPNG -> mWidth + 1) >> 1;
					pPNG -> mCurrLine	= 2;
					break;
				case 6:
					pixels = (pPNG -> mWidth) >> 1;
					pPNG -> mCurrLine	= 0;
					break;
				case 7:
					pPNG -> mCurrLine	= 1;
					pixels				= pPNG -> mWidth;
					break;
				}
			}

			if (16 == pPNG -> mBitDepth)
			{
				pPNG -> mPhaseSz = 1 + (pPNG -> mChannels * pixels * 2);
			}
			else
			{
				pPNG -> mPhaseSz = 1 + (((pPNG -> mChannels * pixels *
					pPNG -> mBitDepth) + 7) >> 3);
			}
		}
	}
	else
	{
		pPNG -> mCurrLine++;
	}

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * ReadIDAT --
 *
 *		This function reads the IDAT (pixel data) chunk from the
 *		PNG file to build the image.  It will continue reading until
 *		all IDAT chunks have been processed or an error occurs.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs or an IDAT
 *		chunk is invalid.
 *
 * Side effects:
 *		The access position in f advances.  Memory may be allocated
 *		by zlib through PNGZAlloc.
 *
 *----------------------------------------------------------------------
 */

static int
ReadIDAT(Tcl_Interp* interp, PNGImage* pPNG, uLong chunkSz, uLong crc)
{
	Byte	pInput[PNG_BLOCK_SZ];
	uLong	lineCount	= 0;

	/* Process IDAT contents until there is no more in this chunk */

	while (chunkSz)
	{
		int blockSz = (chunkSz > PNG_BLOCK_SZ) ? PNG_BLOCK_SZ : chunkSz;
		int ret;

		/* Read the next bit of IDAT chunk data, up to read buffer size */

		if (PNGRead(interp, pPNG, pInput, blockSz, &crc) == TCL_ERROR)
			return TCL_ERROR;

		chunkSz -= blockSz;

		/* Run inflate() until output buffer is not full. */

		pPNG -> mZStream.avail_in = blockSz;
		pPNG -> mZStream.next_in = pInput;

		do {
			ret = inflate(&pPNG -> mZStream, Z_NO_FLUSH);

			switch (ret)
			{
			case Z_STREAM_ERROR:
				break;
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				Tcl_SetResult(interp, "zlib inflation failed", TCL_STATIC);
				return TCL_ERROR;
			}

			/* Process pixels when a full scan line has been obtained */

			if (!pPNG -> mZStream.avail_out)
			{
				Byte*	temp;
				lineCount++;

				if (pPNG -> mPhase > 7)
				{
					Tcl_SetResult(interp,
						"Extra data after final scan line of final phase",
						TCL_STATIC);
					return TCL_ERROR;
				}

				if (DecodeLine(interp, pPNG) == TCL_ERROR)
					return TCL_ERROR;

				/*
				 * Swap the current/last lines so that we always have the last
				 * line processed available, which is necessary for filtering.
				 */

				temp = pPNG -> mpLastLine;
				pPNG -> mpLastLine = pPNG -> mpThisLine;
				pPNG -> mpThisLine = temp;

				/* Next pass through, inflate into the new current line */

				pPNG -> mZStream.avail_out	= pPNG -> mPhaseSz;
				pPNG -> mZStream.next_out	= pPNG -> mpThisLine;
			}
		} while (pPNG -> mZStream.avail_in);

		/* Check for end of zlib stream */

		if (ret == Z_STREAM_END)
		{
			if (chunkSz)
			{
				Tcl_SetResult(interp, "Extra data after end of zlib stream",
					TCL_STATIC);
				return TCL_ERROR;
			}

			break;
		}
	}

	if (CheckCRC(interp, pPNG, crc) == TCL_ERROR)
		return TCL_ERROR;

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * PNGDecode --
 *
 *		This function handles the entirety of reading a PNG file (or
 *		data) from the first byte to the last.
 *
 * Results:
 *		TCL_OK, or TCL_ERROR if an I/O error occurs or any problems
 *		are detected in the PNG file.
 *
 * Side effects:
 *		The access position in f advances.  Memory may be allocated
 *		and image dimensions and contents may change.
 *
 *----------------------------------------------------------------------
 */

static int
PNGDecode(Tcl_Interp* interp, PNGImage* pPNG, Tcl_Obj* format,
	Tk_PhotoHandle imageHandle, int destX, int destY)
{
	double		alpha		= 1.0;
	Tcl_Obj**	objv		= NULL;
	int			objc		= 0;
	uLong		chunkType;
	uLong		chunkSz;
	uLong		crc;

	static const char* fmtOptions[] = {
		"png", "-alpha", (char *)NULL
	};

	enum fmtOptions {
		OPT_PNG, OPT_ALPHA
	};

	/* Parse the PNG signature and IHDR (header) chunk */

	if (ReadIHDR(interp, pPNG) == TCL_ERROR)
		return TCL_ERROR;

	if (format && (Tcl_ListObjGetElements(interp, format,
			&objc, &objv) == TCL_ERROR))
		return TCL_ERROR;

	while (objc)
	{
    	int optIndex;

        if (Tcl_GetIndexFromObj(interp, objv[0], fmtOptions, "option", 0,
				&optIndex) == TCL_ERROR)
            return TCL_ERROR;

		if (OPT_PNG == optIndex)
		{
			objc--; objv++;
			continue;
		}

    	if (objc < 2)
    	{
        	Tcl_WrongNumArgs(interp, 1, objv, "value");
        	return TCL_ERROR;
    	}

		objc--; objv++;

		switch ((enum fmtOptions) optIndex)
		{
		case OPT_PNG:
			break;

		case OPT_ALPHA:
			if (Tcl_GetDoubleFromObj(interp, objv[0], &alpha) == TCL_ERROR)
				return TCL_ERROR;

			if ((alpha < 0.0) || (alpha > 1.0))
			{
				Tcl_SetResult(interp,
					"-alpha value must be between 0.0 and 1.0",
					TCL_STATIC);
				return TCL_ERROR;
			}
			break;
		}

		objc--; objv++;
	}

	/*
	 * The next chunk may either be a PLTE (Palette) chunk or the first
	 * of at least one IDAT (data) chunks.  It could also be one of
	 * a number of ancillary chunks, but those are skipped by for us by
	 * the switch in ReadChunkHeader().
	 *
	 * This chunk is mandatory for color type 3 and forbidden for 2 and 6
	 */

	if (ReadChunkHeader(interp, pPNG, &chunkSz, &chunkType,
			&crc) == TCL_ERROR)
		return TCL_ERROR;

	if (CHUNK_PLTE == chunkType)
	{
		/* Finish parsing the PLTE chunk */

		if (ReadPLTE(interp, pPNG, chunkSz, chunkType, crc) == TCL_ERROR)
			return TCL_ERROR;

		/* Begin the next chunk */

		if (ReadChunkHeader(interp, pPNG, &chunkSz, &chunkType,
				&crc) == TCL_ERROR)
			return TCL_ERROR;
	}
	else
	if (PNG_COLOR_PLTE == pPNG -> mColorType)
	{
		Tcl_SetResult(interp, "PLTE chunk required for indexed color",
			TCL_STATIC);
		return TCL_ERROR;
	}

	/*
	 * The next chunk may be a tRNS (palette transparency) chunk,
	 * depending on the color type. It must come after the PLTE
	 * chunk and before the IDAT chunk, but can be present if there
	 * is no PLTE chunk because it can be used for Grayscale and
	 * TrueColor in lieu of an alpha channel.
	 */

	if (CHUNK_tRNS == chunkType)
	{
		if (ReadtRNS(interp, pPNG, chunkSz, crc) == TCL_ERROR)
			return TCL_ERROR;

		if (ReadChunkHeader(interp, pPNG, &chunkSz, &chunkType,
				&crc) == TCL_ERROR)
			return TCL_ERROR;
	}

	/*
	 * Other ancillary chunk types could appear here, but for now we're
	 * only interested in IDAT.  The others should have been skipped.
	 */

	if (CHUNK_IDAT != chunkType)
	{
		Tcl_SetResult(interp, "At least one IDAT chunk is required",
			TCL_STATIC);
		return TCL_ERROR;
	}

	/*
	 * Expand the photo size (if not set by the user) to provide enough
	 * space for the image being parsed.
	 */

	Tk_PhotoExpand(INTERP_IF_TK85 imageHandle, destX + pPNG -> mWidth,
		destY + pPNG -> mHeight);

	/*
	 * A scan line consists of one byte for a filter type, plus
	 * the number of bits per color sample times the number of
	 * color samples per pixel.
	 */

	if (16 == pPNG -> mBitDepth)
	{
		pPNG -> mLineSz = 1 + (pPNG -> mChannels * pPNG -> mWidth * 2);
	}
	else
	{
		pPNG -> mLineSz = 1 + ((pPNG -> mChannels * pPNG -> mWidth) /
			(8 / pPNG -> mBitDepth));
		if (pPNG -> mWidth % (8 / pPNG -> mBitDepth))
			pPNG -> mLineSz++;
	}

	/* Allocate space for decoding the scan lines */

	pPNG -> mpLastLine	= (Byte*) attemptckalloc(pPNG -> mLineSz);
	pPNG -> mpThisLine	= (Byte*) attemptckalloc(pPNG -> mLineSz);
	pPNG -> mBlock.pixelPtr	= (Byte*) attemptckalloc(pPNG -> mBlockSz);

	if (!pPNG -> mpLastLine || !pPNG -> mpThisLine || !pPNG -> mBlock.pixelPtr)
	{
		Tcl_SetResult(interp, "Memory allocation failed", TCL_STATIC);
		return TCL_ERROR;
	}

	if (pPNG -> mInterlace)
	{       uLong pixels;
		/* Only one pixel per block of 8 per line in the first phase */
		pPNG -> mPhase		= 1;

		pixels = (pPNG -> mWidth + 7) >> 3;

		if (16 == pPNG -> mBitDepth)
		{
			pPNG -> mPhaseSz = 1 + (pPNG -> mChannels * pixels * 2);
		}
		else
		{
			pPNG -> mPhaseSz = 1 +
				(((pPNG -> mChannels * pixels * pPNG -> mBitDepth) + 7) >> 3);
		}
	}
	else
	{
		pPNG -> mPhaseSz = pPNG -> mLineSz;
	}
	pPNG -> mZStream.avail_out	= pPNG -> mPhaseSz;
	pPNG -> mZStream.next_out	= pPNG -> mpThisLine;

	/* All of the IDAT (data) chunks must be consecutive */

	while (CHUNK_IDAT == chunkType)
	{
		if (ReadIDAT(interp, pPNG, chunkSz, crc) == TCL_ERROR)
			return TCL_ERROR;

		if (ReadChunkHeader(interp, pPNG, &chunkSz, &chunkType,
				&crc) == TCL_ERROR)
			return TCL_ERROR;
	}

	/* Now skip the remaining chunks which we're also not interested in. */

	while (CHUNK_IEND != chunkType)
	{
		if (SkipChunk(interp, pPNG, chunkSz, crc) == TCL_ERROR)
			return TCL_ERROR;

		if (ReadChunkHeader(interp, pPNG, &chunkSz, &chunkType,
				&crc) == TCL_ERROR)
			return TCL_ERROR;
	}

	/* Got the IEND (end of image) chunk.  Do some final checks... */

	if (chunkSz)
	{
		Tcl_SetResult(interp, "IEND chunk contents must be empty", TCL_STATIC);
		return TCL_ERROR;
	}

	/* Check the CRC on the IEND chunk */

	if (CheckCRC(interp, pPNG, crc) == TCL_ERROR)
		return TCL_ERROR;

	/*
	 * TODO: verify that nothing else comes after the IEND chunk, or do
	 * we really care?
	 */

#if 0
	if (PNGRead(interp, pPNG, &c, 1, NULL) != TCL_ERROR)
	{
		Tcl_SetResult(interp, "Extra data following IEND chunk", TCL_STATIC);
		return TCL_ERROR;
	}
#endif

	/* Apply overall image alpha if specified */

	if (alpha != 1.0)
	{
		unsigned char*	p		= pPNG -> mBlock.pixelPtr;
		unsigned char*	pEnd	= p + pPNG -> mBlockSz;
		int				offset	= pPNG -> mBlock.offset[3];

		p += offset;

		if (16 == pPNG -> mBitDepth)
		{
			int channel;

			while (p < pEnd)
			{
				channel = ((p[0] << 8) | p[1]) * alpha;

				*p++ = channel >> 8;
				*p++ = channel & 0xff;

				p += offset;
			}
		}
		else
		{
			while (p < pEnd)
			{
				*p++ *= alpha;

				p += offset;
			}
		}
	}

	Tk_PhotoPutBlock(INTERP_IF_TK85 imageHandle, &pPNG -> mBlock, destX, destY,
		pPNG -> mBlock.width, pPNG -> mBlock.height, TK_PHOTO_COMPOSITE_SET);

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * FileMatchPNG --
 *
 *		This function is invoked by the photo image type to see if
 *		a file contains image data in PNG format.
 *
 * Results:
 *		The return value is 1 if the first characters in file f look
 *		like PNG data, and 0 otherwise.
 *
 * Side effects:
 *		The access position in f may change.
 *
 *----------------------------------------------------------------------
 */

static int
FileMatchPNG(chan, fileName, format, widthPtr, heightPtr, interp)
	Tcl_Channel chan;			/* The image file, open for reading. */
	CONST char *fileName;		/* The name of the image file. */
	Tcl_Obj *format;			/* User-specified format object, or NULL. */
	int *widthPtr, *heightPtr;	/* The dimensions of the image are
								 * returned here if the file is a valid
								 * raw PNG file. */
	Tcl_Interp *interp;			/* not used */
{
	PNGImage		png;
	int				match = 0;
	Tcl_SavedResult	sya;

	Tcl_SaveResult(interp, &sya);

	PNGInit(interp, &png, chan, NULL, PNG_DECODE);

	if (ReadIHDR(interp, &png) == TCL_OK)
	{
		*widthPtr	= (int)png.mWidth;
		*heightPtr	= (int)png.mHeight;
		match		= 1;
	}

	PNGCleanup(&png);
	Tcl_RestoreResult(interp, &sya);

	return match;
}


/*
 *----------------------------------------------------------------------
 *
 * FileReadPNG --
 *
 *		This function is called by the photo image type to read
 *		PNG format data from a file and write it into a given
 *		photo image.
 *
 * Results:
 *		A standard TCL completion code.  If TCL_ERROR is returned
 *		then an error message is left in the interp's result.
 *
 * Side effects:
 *		The access position in file f is changed, and new data is
 *		added to the image given by imageHandle.
 *
 *----------------------------------------------------------------------
 */

static int
FileReadPNG(interp, chan, fileName, format, imageHandle, destX, destY,
		width, height, srcX, srcY)
	Tcl_Interp *interp;			/* Interpreter to use for reporting errors. */
	Tcl_Channel chan;			/* The image file, open for reading. */
	CONST char *fileName;		/* The name of the image file. */
	Tcl_Obj *format;			/* User-specified format object, or NULL. */
	Tk_PhotoHandle imageHandle;	/* The photo image to write into. */
	int destX, destY;			/* Coordinates of top-left pixel in
								 * photo image to be written to. */
	int width, height;			/* Dimensions of block of photo image to
								 * be written to. */
	int srcX, srcY;				/* Coordinates of top-left pixel to be used
								 * in image being read. */
{
	PNGImage		png;
	int				result		= TCL_ERROR;

	result = PNGInit(interp, &png, chan, NULL, PNG_DECODE);

	if (TCL_OK == result)
		result = PNGDecode(interp, &png, format, imageHandle, destX, destY);

	PNGCleanup(&png);
	return result;
}


/*
 *----------------------------------------------------------------------
 *
 * StringMatchPNG --
 *
 *  This function is invoked by the photo image type to see if
 *  an object contains image data in PNG format.
 *
 * Results:
 *  The return value is 1 if the first characters in the data are
 *  like PNG data, and 0 otherwise.
 *
 * Side effects:
 *  the size of the image is placed in widthPre and heightPtr.
 *
 *----------------------------------------------------------------------
 */

static int
StringMatchPNG(dataObj, format, widthPtr, heightPtr, interp)
	Tcl_Obj *dataObj;			/* the object containing the image data */
	Tcl_Obj *format;			/* the image format object, or NULL */
	int *widthPtr;				/* where to put the string width */
	int *heightPtr;				/* where to put the string height */
	Tcl_Interp *interp;			/* not used */
{
	PNGImage		png;
	int				match		= 0;
	Tcl_SavedResult	sya;

	Tcl_SaveResult(interp, &sya);
	PNGInit(interp, &png, (Tcl_Channel)NULL, dataObj, PNG_DECODE);

	png.mpStrData = Tcl_GetByteArrayFromObj(dataObj, &png.mStrDataSz);

	if (ReadIHDR(interp, &png) == TCL_OK)
	{
		*widthPtr	= (int)png.mWidth;
		*heightPtr	= (int)png.mHeight;
		match		= 1;
	}

	PNGCleanup(&png);
	Tcl_RestoreResult(interp, &sya);

	return match;
}


/*
 *----------------------------------------------------------------------
 *
 * StringReadPNG -- --
 *
 *		This function is called by the photo image type to read
 *		PNG format data from an object and give it to the photo image.
 *
 * Results:
 *		A standard TCL completion code.  If TCL_ERROR is returned
 *		then an error message is left in the interp's result.
 *
 * Side effects:
 *		new data is added to the image given by imageHandle.
 *
 *----------------------------------------------------------------------
 */

static int
StringReadPNG(interp, dataObj, format, imageHandle,
		destX, destY, width, height, srcX, srcY)
	Tcl_Interp *interp;				/* interpreter for reporting errors in */
	Tcl_Obj *dataObj;				/* object containing the image */
	Tcl_Obj *format;				/* format object, or NULL */
	Tk_PhotoHandle imageHandle;		/* the image to write this data into */
	int destX, destY;				/* The rectangular region of the  */
	int width, height;				/*   image to copy */
	int srcX, srcY;
{
	PNGImage	png;
	int			result		= TCL_ERROR;

	result = PNGInit(interp, &png, (Tcl_Channel)NULL,
		dataObj, PNG_DECODE);

	if (TCL_OK == result)
		result = PNGDecode(interp, &png, format, imageHandle, destX, destY);

	PNGCleanup(&png);
	return result;
}

/* Write is far from done yet.  Don't enable this code. */
#if 0
static int
WriteByte(Tcl_Interp* interp, PNGImage* pPNG, Byte c, uLong* pCRC)
{
	return WriteBytes(interp, pPNG, &c, 1, pCRC);
}

static int
WriteBytes(Tcl_Interp* interp, PNGImage* pPNG,
	const void* pSrc, uLong srcSz, uLong* pCRC)
{
	if (pCRC && pSrc && srcSz)
		*pCRC = crc32(*pCRC, pSrc, srcSz);

	/*
	 * TODO: is Tcl_AppendObjToObj faster here? i.e., does Tcl join
	 * the objects immediately or store them in a multi-object rep?
	 */

	if (pPNG -> mpObjData)	/* ugh, wanna check result, not panic! */
		Tcl_AppendToObj(pPNG -> mpObjData, pSrc, srcSz);
	else
	if (Tcl_WriteChars(pPNG -> mChannel, pSrc, srcSz) < 0)
	{
		/* TODO: reason */

		Tcl_SetResult(interp, "Write to channel failed", TCL_STATIC);
		return TCL_ERROR;
	}

	return TCL_OK;
}

static int
WriteLong(Tcl_Interp* interp, PNGImage* pPNG, uLong L, uLong* pCRC)
{
	Byte	pc[4];

	pc[0] = (Byte)((L & 0xff000000) >> 24);
	pc[1] = (Byte)((L & 0x00ff0000) >> 16);
	pc[2] = (Byte)((L & 0x0000ff00) >> 8);
	pc[3] = (Byte)((L & 0x000000ff) >> 0);

	return WriteBytes(interp, pPNG, pc, 4, pCRC);
}

static int
WritePNG(Tcl_Interp* interp, Tk_PhotoImageBlock *blockPtr, PNGImage* pPNG)
{
	/* Write out the PNG Signature */

	if (WriteBytes(interp, pPNG, gspPNGSignature, gsSigSz, NULL) == TCL_ERROR)
		return TCL_ERROR;

	if (WriteIHDR(interp, pPNG, blockPtr) == TCL_ERROR)
		return TCL_ERROR;

	if (WriteChunk(interp, pPNG, CHUNK_IDAT, NULL, 0) == TCL_ERROR)
		return TCL_ERROR;

	return WriteIEND(interp, pPNG);;
}

static int
WriteIHDR(Tcl_Interp* interp, PNGImage* pPNG, Tk_PhotoImageBlock *blockPtr)
{
	uLong	crc		= crc32(0, NULL, 0);
	int		result	= TCL_OK;

	/* The IHDR (header) chunk has a fixed size of 13 bytes */

	result = WriteLong(interp, pPNG, 13, NULL);

	/* Write the IHDR Chunk Type */

	if (TCL_OK == result)
		result = WriteLong(interp, pPNG, CHUNK_IHDR, &crc);

	/* Write the image width, height */

	if (TCL_OK == result)
		result = WriteLong(interp, pPNG, blockPtr->width, &crc);

	if (TCL_OK == result)
		result = WriteLong(interp, pPNG, blockPtr->height, &crc);

	/*
	 * TODO: Figure out appropriate bit depth, color type.  For now,
	 * just going to say it's 8bpp TrueColor.
	 */

	if (TCL_OK == result)
		result = WriteByte(interp, pPNG, 8, &crc);

	if (TCL_OK == result)
		result = WriteByte(interp, pPNG, PNG_COLOR_RGBA, &crc);

	if (TCL_OK == result)
		result = WriteByte(interp, pPNG, PNG_COMPRESS_DEFLATE, &crc);

	if (TCL_OK == result)
		result = WriteByte(interp, pPNG, PNG_FILTMETH_STANDARD, &crc);

	/* TODO: support interlace through -format */

	if (TCL_OK == result)
		result = WriteByte(interp, pPNG, PNG_INTERLACE_NONE, &crc);

	/* Write out the CRC at the end of the chunk */

	if (TCL_OK == result)
		result = WriteLong(interp, pPNG, crc, NULL);

	return result;
}

static int
WriteIEND(Tcl_Interp* interp, PNGImage* pPNG)
{
	return WriteChunk(interp, pPNG, CHUNK_IEND, NULL, 0);
}

static int
WriteChunk(Tcl_Interp* interp, PNGImage* pPNG,
	uLong chunkType, const void* pData, uLong dataSz)
{
	uLong	crc		= crc32(0, NULL, 0);
	int		result	= TCL_OK;

	/* Write the length field for the chunk */

	result = WriteLong(interp, pPNG, dataSz, NULL);

	/* Write the Chunk Type */

	if (TCL_OK == result)
		result = WriteLong(interp, pPNG, chunkType, &crc);

	/* Write the contents (if any) */

	if (TCL_OK == result)
		result = WriteBytes(interp, pPNG, pData, dataSz, &crc);

	/* Write out the CRC at the end of the chunk */

	if (TCL_OK == result)
		result = WriteLong(interp, pPNG, crc, NULL);

	return result;
}

static int
FileWritePNG(Tcl_Interp* interp, CONST char* filename, Tcl_Obj* format,
	Tk_PhotoImageBlock *blockPtr)
{
	Tcl_Channel	chan;
	PNGImage	png;
	int			result		= TCL_ERROR;

	/* Open a Tcl file channel where the image data will be stored. */

	/* TODO: Support other channel types through format option? */
	/* TODO: Support file mode through format option? */

	/* This is silly.  image photo ought to take care of this stuff. */

	chan = Tcl_OpenFileChannel(interp, filename, "w", 0644);

	if (!chan)
		return TCL_ERROR;

	PNGInit(interp, &png, chan, NULL, PNG_ENCODE);

	/*
     * Set the translation mode to binary so that CR and LF are not
	 * to the platform's EOL sequence.
	 */

	if (Tcl_SetChannelOption(interp, chan, "-translation", "binary") != TCL_OK)
		goto cleanup;

	if (Tcl_SetChannelOption(interp, chan, "-encoding", "binary") != TCL_OK)
		goto cleanup;

	/* Write the raw PNG data out to the file */

	result = WritePNG(interp, blockPtr, &png);

cleanup:
	Tcl_Close(interp, chan);
	PNGCleanup(&png);
	return result;
}

static int
StringWritePNG(Tcl_Interp* interp, Tcl_Obj* format,
	Tk_PhotoImageBlock *blockPtr)
{
	Tcl_Obj*	pObjResult	= Tcl_NewObj();
	PNGImage	png;
	int			result		= TCL_ERROR;

	PNGInit(interp, &png, NULL, pObjResult, PNG_ENCODE);

	/*
	 * Write the raw PNG data into the prepared Tcl_Obj buffer
	 * Set the result back to the interpreter if successful.
	 */

	result = WritePNG(interp, blockPtr, &png);

	if (TCL_OK == result)
		Tcl_SetObjResult(interp, png.mpObjData);

	PNGCleanup(&png);
	return result;
}

#endif

/* The format record for the PNG file format: */

Tk_PhotoImageFormat tkImgFmtPNG = {
	"png",				/* name */
	FileMatchPNG,		/* fileMatchProc */
	StringMatchPNG,		/* stringMatchProc */
	FileReadPNG,		/* fileReadProc */
	StringReadPNG,		/* stringReadProc */
	NULL, /* FileWritePNG	*/	/* fileWriteProc */
	NULL  /* StringWritePNG	*/	/* stringWriteProc */
};

#endif