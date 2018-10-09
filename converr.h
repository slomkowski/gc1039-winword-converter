/*----------------------------------------------------------------------------
	%%File: CONVERR.H
	%%Unit: CORE
	%%Contact: smueller

	Conversions error return values.
----------------------------------------------------------------------------*/

#ifndef CONVERR_H
#define CONVERR_H


typedef short FCE;

// File Conversion Errors
#define fceNoErr    		0		// success

#define fceOpenInFileErr	(-1)	// could not open input file
#define fceReadErr			(-2)	// error during read
#define fceOpenConvErr		(-3)	// error opening conversion file
#define fceWriteErr			(-4)	// error during write
#define fceInvalidFile		(-5)	// invalid data in conversion file
#define fceOpenExceptErr	(-6)	// error opening exception file
#define fceWriteExceptErr	(-7)	// error writing exception file
#define fceNoMemory			(-8)	// out of memory
#define fceInvalidDoc		(-9)	// invalid document
#define fceDiskFull			(-10)	// out of space on output
#define fceDocTooLarge		(-11)	// conversion document too large for target
#define fceOpenOutFileErr	(-12)	// could not open output file
#define fceUserCancel		(-13)	// conversion cancelled by user
#define fceWrongFileType	(-14)	// wrong file type for this converter


#if defined(WIN32) || defined(MACPPC)
// These errors are obsolete.  Map all usages to smaller, current set.
#undef  fceOpenConvErr
#define fceOpenConvErr		fceOpenInFileErr
#undef  fceOpenExceptErr
#define fceOpenExceptErr 	fceOpenInFileErr
#undef  fceWriteExceptErr
#define fceWriteExceptErr	fceWriteErr
#undef  fceInvalidDoc
#define fceInvalidDoc		fceInvalidFile
#undef  fceDiskFull
#define fceDiskFull			fceWriteErr
#undef  fceDocTooLarge
#define fceDocTooLarge		fceWriteErr
#endif


#endif // CONVERR_H

