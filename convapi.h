/*----------------------------------------------------------------------------
	%%File: CONVAPI.H
	%%Unit: CORE
	%%Contact: smueller@microsoft.com

	This header is distributed as part of the 32-bit Conversions SDK.

	Changes to this header file should be sent to smueller@microsoft.com

	Revision History: (Current=1.02)

	3/14/97 tidy up and remove references to derived types
	3/16/97 MacPPC callback prototype fix
	4/21/97 explicit packing, MacPPC prototype fixes, C++ friendliness
----------------------------------------------------------------------------*/

#ifndef CONVAPI_H
#define CONVAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN16
#include "crmgr.h"	// for PFN_CRMGR
#endif

#include "converr.h"


// Rtf to foreign Percent ConVersion compleTe
#pragma pack(2)
typedef struct _PCVT
	{
	short cbpcvt;				// size of this structure
	short wVersion;				// version # for determining size of struct
	short wPctWord;				// current %-complete according to Word
	short wPctConvtr;			// current %-complete according to the converter
	} PCVT;
#pragma pack()


// Rtf to Foreign User OPTions
#pragma pack(2)
typedef struct _RFUOPT
	{
	union
		{
		short rgf;
		struct
			{
			short fPicture : 1;		// converter wants pictures (binary implied)
			short fLayoutInfo : 1;	// converter wants (slow) layout info
			short fPctComplete : 1;	// converter will provide percent complete
			short : 13;
			};
		};
	} RFUOPT;
#pragma pack()

// Rtf to foreign user options, again
#define fOptPicture				0x0001
#define fOptLayoutInfo			0x0002
#define fOptPctComplete			0x0004



// RegisterApp input flags
#pragma pack(2)
typedef struct _LFREGAPP
	{
	union
		{
		struct
			{
			unsigned long fAcceptPctComp : 1;
			unsigned long fNoBinary : 1;
			unsigned long fPreview : 1;
			unsigned long fDontNeedOemConvert : 1;
			unsigned long fIndexing : 1;
			unsigned long unused : 27;
			};
		unsigned long lfRegApp;
		};
	} LFREGAPP;
#pragma pack()

// RegisterApp input flags, again
#define fRegAppPctComp			0x00000001	// app is prepared to accept percent complete numbers in export
#define fRegAppNoBinary			0x00000002	// app is not prepared to accept binary-encoded picture and other data
#define fRegAppPreview			0x00000004	// converter should display no message boxes or dialogs
#define fRegAppSupportNonOem	0x00000008	// app is prepared to provide non-OEM character set filenames
#define fRegAppIndexing			0x00000010	// converter can omit non-content Rtf


// REGister APP structure (received from client)
#pragma pack(1)
typedef struct _REGAPP {
	short cbStruct;			// size of this structure
	char rgbOpcodeData[];
	} REGAPP;
#pragma pack()

// REGAPP byte opcodes
#define RegAppOpcodeFilename	0x80	// true final filename of exported file
#define RegAppOpcodeInterimPath	0x81	// path being exported to is not the final location


// REGister APP RETurn structure (passed to client)
#pragma pack(1)
typedef struct _REGAPPRET
	{
	short cbStruct;			// size of this structure

	// Does this converter understand docfiles and/or non-docfiles?
	char cbSizefDocfile;
	char opcodefDocfile;
	union
		{
		struct
			{
			short fDocfile : 1;
			short fNonDocfile : 1;
			short : 14;
			};
		short grfType;
		};

	// Version of Word for which converter's Rtf is compliant
	char cbSizeVer;		// == sizeof(char)+sizeof(char)+sizeof(short)+sizeof(short)
	char opcodeVer;
	short verMajor;		// Major version of Word for which Rtf is compliant
	short verMinor;		// Minor version of Word for which Rtf is compliant

#if defined(WIN16) || defined(WIN32)
	// What character set do we want all filenames to be in.
	char cbSizeCharset;
	char opcodeCharset;
	char charset;
#endif

	char opcodesOptional[];		// optional additional stuff
	} REGAPPRET;
#pragma pack()

// REGAPPRET byte opcodes
#define RegAppOpcodeVer				0x01	// Rtf version converter is prepared to accept
#define RegAppOpcodeDocfile			0x02	// converter's ability to handle regular and docfiles
#define RegAppOpcodeCharset 		0x03	// converter is prepared to handle filenames in this character set
#define RegAppOpcodeReloadOnSave	0x04	// app should reload document after exporting
#define RegAppOpcodePicPlacehold	0x05	// app should send placeholder pictures (with size info) for includepicture \d fields
#define RegAppOpcodeFavourUnicode	0x06	// app should output Unicode RTF whenever possible, especially for DBCS; \uc0 is good
#define RegAppOpcodeNoClassifyChars	0x07	// app should not break text runs by character set clasification



// Principal converter entrypoints
#ifdef WIN16

// callback type
typedef PFN_CRMGR PFN_RTF;

void PASCAL GetIniEntry(HANDLE ghIniName, HANDLE ghIniExt);
HGLOBAL PASCAL RegisterApp(unsigned long lFlags, void FAR *lpFuture);
FCE  PASCAL IsFormatCorrect(HANDLE ghszFile, HANDLE ghszDescrip);
FCE  PASCAL ForeignToRtf(HANDLE ghszFile, HANDLE ghBuff, HANDLE ghszDescrip, HANDLE ghszSubset, PFN_RTF lpfnOut);
FCE  PASCAL RtfToForeign(HANDLE ghszFile, HANDLE ghBuff, HANDLE ghszDescrip, PFN_RTF lpfnIn);

#elif defined(WIN32)

// callback type
typedef long (PASCAL *PFN_RTF)();

long PASCAL InitConverter32(HANDLE hWnd, char *szModule);
void PASCAL UninitConverter(void);
void PASCAL GetReadNames(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt);
void PASCAL GetWriteNames(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt);
HGLOBAL PASCAL RegisterApp(unsigned long lFlags, void FAR *lpFuture);
FCE  PASCAL IsFormatCorrect32(HANDLE ghszFile, HANDLE ghszClass);
FCE  PASCAL ForeignToRtf32(HANDLE ghszFile, void *pstgForeign, HANDLE ghBuff, HANDLE ghszClass, HANDLE ghszSubset, PFN_RTF lpfnOut);
FCE  PASCAL RtfToForeign32(HANDLE ghszFile, void *pstgForeign, HANDLE ghBuff, HANDLE ghshClass, PFN_RTF lpfnIn);
long PASCAL CchFetchLpszError(long fce, char FAR *lpszError, long cb);
long PASCAL FRegisterConverter(HANDLE hkeyRoot);

#elif defined(MAC)

typedef struct _GFIB	// Graphics File Information Block.
	{
	short	fh;			// File handle to the open file.
	long	fcSrc;		// FC where the WPG Data will reside.
	long	lcbSrc;		// Count of bytes for WPG Data.
	} GFIB;

typedef struct _PINFO
	{
	Rect	box;		// Dimensions of the binding rectangle for the picture.
	short	inch;		// Units/Inch in which these dimensions are given.
	} PINFO;
typedef PINFO **HPINFO;

// grfs for wOleFlags
#define grfOleDocFile 		0x0100
#define grfOleNonDocFile 	0x0200
#define grfOleInited		0x0001

#ifdef MAC68K
// function type of Rtf callback function
typedef short (PASCAL * PFNRTFXFER)(short, unsigned short);

typedef struct _FIC
	{
	short icr;					/* Index to the converter routine */
	union
		{
		char **hstFileName;		/* File Name */
		long **hrgTyp;			/* Types of files known to this converter */
		GFIB **hgfib;			/* Graphics File Info Block */
		void *lpFuture;			// for RegisterApp()
		} hun;
	short vRefNum;				/* vRefNum for the file */
	short refNum;				/* Path for file */
	union
		{
		long ftg;
		unsigned long lFlags;	/* for RegisterApp */
		};
	char **hszDescrip;			/* Description of file */
	PFNRTFXFER pfn;				/* Pointer into Word of function to
								   call for more RTF or to convert RTF */
	union
		{
		Handle hBuffer;			/* Buffer through which RTF will be
								   passed. */
		Handle hRegAppRet;		/* handle to return RegAppRet structure,
								   NULL if couldn't be allocated */
		};
	short wReturn;				/* Code returned by converter */

	// Following are new to Mac Word 6.0
	short  wVersion;
	Handle hszClass;
	Handle hszSubset;
	HPINFO hpinfo;				/* Handle to PINFO Struct for Graphics */
	union
		{
		struct
			{
			char fDocFile : 1;
			char fNonDocFile : 1;
			char : 6;
			char fOleInited : 1;
			char : 7;
			};
		short wOleFlags;
		};
	} FIC;
	
typedef FIC *PFIC;
typedef PFIC *HFIC;
#define cbFIC sizeof(FIC)
#define cbFIC5 offsetof(FIC, wVersion) /* size of a Word 5 FIC */

/* Constants for Switch routine */
#define icrInitConverter    0
#define icrIsFormatCorrect  1
#define icrGetReadTypes     2
#define icrGetWriteTypes    3
#define icrForeignToRtf     4
#define icrRtfToForeign     5
#define icrRegisterApp		6
#define icrConverterForeignToRTF	7

void _pascal CodeResourceEntry(long *plUsr, FIC **hfic);
#endif

#ifdef MACPPC
// function type of Rtf callback function
typedef short (* PFNRTFXFER)(short, unsigned short);

long InitConverter(long *plw);
void UninitConverter(void);
Handle RegisterApp(unsigned long lFlags, void *lpFuture);
void GetReadNames(Handle hszClass, Handle hszDescrip, Handle hrgTyp);
void GetWriteNames(Handle hszClass, Handle hszDescrip, Handle hrgTyp);
FCE IsFormatCorrect(FSSpecPtr, Handle);
FCE ForeignToRtf(FSSpecPtr, void *, Handle, Handle, Handle, PFNRTFXFER);
FCE RtfToForeign(FSSpecPtr, void *, Handle, Handle, PFNRTFXFER);
#endif

#else
#error Unknown platform.
#endif

#ifdef __cplusplus
}
#endif

#endif // CONVAPI_H
