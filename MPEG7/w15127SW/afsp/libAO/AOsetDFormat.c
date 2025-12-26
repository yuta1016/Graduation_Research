/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetDFormat(struct AO_FOpar *FO, AFILE *AFp[], int Nf)

Purpose:
  Determine a compatible output data format for the output file type

Description:
  File Type:
  This routine first determines an output file type. If the file type is set in
  the structure FO, that value is used. Otherwise the file type is determined by
  the output file name extension. If the file name extension is non-standard,
  and the data format is text, the file type is set to text audio. Finally if
  the file type is still not set, it is set to a default file type.

  The mapping from file name extensions to file types is as follows. The
  asterisk indicates that a shortened form can be used.
     .wav*e        - FTW_WAVE, Wave file
     .au           - FTW_AU, AU audio file
     .aif*f        - FTW_AIFF, AIFF sound file
     .afc or .aifc - FTW_AIFF_C, AIFF-C sound file
     .raw, .dat, or .nh - FTW_NH_NATIVE, headerless file
     .txt or .text - FTW_TXAUD, text file (with header)

  Data Format:
  The determination of the data format has three modes. The first mode is used
  for programs that do not use data from input files. This is signalled by
  setting Nf=0. The AFp argument is not used and can be set to NULL. If the data
  format is set, that value is used. Otherwise, the data format is set to a
  default format.

  If the program uses data from input files, the second mode is used to test the
  compatibility of the data format and the file type for the output file before
  the input files are opened. This mode is triggered by having AFp set to NULL
  and having Nf > 0. If an incompatibility is found, an error message is printed
  and execution is halted. If the data format is not set, it is left unchanged.
  A second pass using Mode 3 after the input files are open will resolve the
  data format for the output file.

  The third mode is used to find a data format for the output file compatible
  with the data format in the input file(s) if the output data format has not
  been set. First the data formats are assigned a precision. The precision of
  the input data is the largest of the precision of the input files. The output
  file data format will have a precision at least as great as the precision of
  the input data.

  For use in error messages, the program name should be set using the routine
  UTsetProg.

Parameters:
  <-  void AOsetDFormat
  <-  enum AF_FD_T AOsetDFormat
      Output code for the data format
   -> const struct AO_FOpar *FO
      Output file parameters. The file type (FO->FtypeW) can be FTW_UNDEF. The
      data format (FO->DFormat.Format) can be FD_UNDEF.
   -> const AFILE *AFp[]
      Audio file pointers for the input files (Nf values). Set this pointer to
      NULL for Mode 1 or 2 operation
   -> int Nf
      Number of input audio files  (can be zero)

Author / revision:
  P. Kabal
  $Revision: 1.21 $  $Date: 2023/04/04 19:51:34 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>  /* FILENAME_MAX */

#include <AO.h>

#define ROUTINE       "AOsetFormat"
#define PGM           ((UTgetProg())[0] == '\0' ? ROUTINE : UTgetProg())
#define MAXV(a, b)    (((a) > (b)) ? (a) : (b))
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Resulting precision ranking for data formats. Float is considered to be more
   precise than 32-bit integer. This is done so that integer inputs will be
   represented with integer data formats on output. The minimum precision for
   the output format is that of 16-bit integer. Thus mu-law, A-law, and the
   one-byte formats will be promoted to 16-bit precision.
     0 - undefined,
     1 - 8-bit unsigned or integer
     2 - mu-law or A-law
     3 - 16-bit integer
     4 - 24-bit integer
     5 - 32-bit integer
     6 - 32-bit float, text16, text
     7 - 64-bit double

   The text formats are floating point formats (using exponents which are
   multiples 10) with 7 significant digits of precision. TEXT16 will generally
   give a more compact representation than TEXT.

      Format    0    1      2      3      4   5   6   7   8  9  10   11  12
              undef A-law mu-law mu-lawR u8  i8 i16 i24 i32 f4  f8  t16   t
      Precision 0    2      2       2     1   1   3   4   5  6   7    6   6
*/

/* Canonical data formats for each of the precision values */
/* FD_TEXT16 and FD_TEXT do not appear in this list. However, these will be
   used if explicitly selected, or if the file type is TXAUD. */

static const enum AF_FD_T Prec2FD[8] = {
  FD_UNDEF, FD_INT16, FD_INT16, FD_INT16, FD_INT24, FD_INT32,
  FD_FLOAT32, FD_FLOAT64};
static const int PrecD[] = {0, 2, 2, 2, 1, 1, 3, 4, 5, 6, 7, 6, 6};

/* For the error message */
static const char *AF_FTWN[] = {
  "UNDEFINED",
  "AU file",
  "WAVE file",
  "AIFF file",
  "AIFF-C file",
  "AIFF-C/sowt file",
  "WAVE-EX file",
  "WAVE file",  /* WAVE-NOEX */
  "Headerless file",
  "Headerless file",
  "Headerless file",
  "Headerless file",
  "Text audio file"
};

static const enum AF_FD_T *
AO_AllowFD(enum AF_FTW_T FtypeW);


void
AOsetDFormat(struct AO_FOpar *FO, AFILE *AFp[], int Nf)

{
  int i, PrecI, PrecO;
  enum AF_FD_T Format;
  const enum AF_FD_T *AllowFD;

  assert(FTW_TXAUD+1 == NELEM(AF_FTWN));
  assert(NELEM(PrecD) == AF_NFD);

  /* Set the output file type if not set */
  AOsetFtypeW(FO);

  Format = FO->DFormat.Format;

  /* Special case */
  if (Format == FD_UNDEF && FO->FtypeW == FTW_TXAUD)
    Format = FD_TEXT16;

  /* Set up allowable data formats for the output file type */
  AllowFD = AO_AllowFD(FO->FtypeW);

  if (Format == FD_UNDEF) {
    if (Nf <= 0)
      /* No input files, use the default format */
      Format = AllowFD[AO_DFORMATO_DEFAULT];
    else if (AFp != NULL) {
      /* Choose the output data format based on the input data formats */
      PrecO = PrecD[FD_INT16];        /* At least precision for int16 */
      for (i = 0; i < Nf; ++i) {
        PrecI = PrecD[AFp[i]->Format];
        PrecO = MAXV(PrecO, PrecI);
      }
      Format = Prec2FD[PrecO];
    }
  }

  /* Check for compatibility of the data format and file type */
  if (Format != FD_UNDEF && Format != AllowFD[Format])
    UThalt(AOMF_DataFType, PGM, AF_DTN[Format], AF_FTWN[FO->FtypeW]);

  /* Set the Format */
  FO->DFormat.Format = Format;
}

static const enum AF_FD_T *
AO_AllowFD(enum AF_FTW_T FtypeW)

{
  const enum AF_FD_T *AllowFD;

/* Conversion tables to allowable data formats for file types */
  static const enum AF_FD_T Allow_AU[] = {FD_UNDEF,
          FD_ALAW8, FD_MULAW8, FD_UNDEF, FD_UNDEF, FD_INT8, FD_INT16,
          FD_INT24, FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_UNDEF, FD_UNDEF};
  static const enum AF_FD_T Allow_WAVE[] = {FD_UNDEF,
          FD_ALAW8, FD_MULAW8, FD_UNDEF, FD_UINT8, FD_UNDEF, FD_INT16,
          FD_INT24, FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_UNDEF, FD_UNDEF};
  static const enum AF_FD_T Allow_AIFF[] = {FD_UNDEF,
          FD_UNDEF, FD_UNDEF, FD_UNDEF, FD_UNDEF, FD_INT8, FD_INT16,
          FD_INT24, FD_INT32, FD_INT16, FD_INT16, FD_UNDEF, FD_UNDEF};
  static const enum AF_FD_T Allow_AIFF_C[] = {FD_UNDEF,
          FD_ALAW8, FD_MULAW8, FD_UNDEF, FD_UNDEF, FD_INT8, FD_INT16,
          FD_INT24, FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_UNDEF, FD_UNDEF};
  static const enum AF_FD_T Allow_NH[] = {FD_UNDEF,
          FD_ALAW8, FD_MULAW8, FD_MULAWR8, FD_UINT8, FD_INT8, FD_INT16,
          FD_INT24, FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_TEXT16, FD_TEXT};
  static const enum AF_FD_T Allow_TXAUD[] = {FD_UNDEF,
          FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT16,
          FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT};

  assert(NELEM(Allow_AU)   == AF_NFD && NELEM(Allow_WAVE)   == AF_NFD &&
         NELEM(Allow_AIFF) == AF_NFD && NELEM(Allow_AIFF_C) == AF_NFD &&
         NELEM(Allow_NH)   == AF_NFD && NELEM(Allow_TXAUD)  == AF_NFD);

  switch (FtypeW) {
  case FTW_AU:
    AllowFD = Allow_AU;
    break;
  case FTW_WAVE:
  case FTW_WAVE_EX:
  case FTW_WAVE_NOEX:
    AllowFD = Allow_WAVE;
    break;
  case FTW_AIFF:
    AllowFD = Allow_AIFF;
    break;
  case FTW_AIFF_C:
  case FTW_AIFF_C_SOWT:
    AllowFD = Allow_AIFF_C;
    break;
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
  case FTW_NH_EL:
  case FTW_NH_EB:
    AllowFD = Allow_NH;
    break;
  case FTW_TXAUD:
    AllowFD = Allow_TXAUD;
    break;
  default:
    AllowFD = NULL;
    assert(0);
  }

  return AllowFD;
}
