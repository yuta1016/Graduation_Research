/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdecInfoDDU(const char String[], double Dval[2],
                   const char *UnitsTable[], int *Key)
  int AFdecInfoIIU(const char String[], double Ival[2],
                   const char *UnitsTable[], int *Key)

Purpose:
  Decode numeric values and a corresponding units specification

Description:
  This routine decodes values of the following forms:
    (1) single value, e.g. "32000"
    (2) pair of values, e.g. "20000/2"
    (3) single value with a units specification, e.g. "19.98 kHz"
    (4) pair of values with a units specification, e.g. "20/3 kHz"

Parameters:
  <-  int AFdecInfoDDU / int AFdecInfoIIU
      Status, 0 for no error.
   -> const char String[]
      Input string
  <-  double Dval[2] / int Ival[2]
      Data values. If the return value is zero, these have been set.
   -> const char *UnitsTable[]
      Pointer array with pointers to the units specification keyword strings.
      The end of the keyword table is signalled with a NULL pointer. Note that
      with ANSI C, if the actual parameter is not declared to have the const
      attribute, an explicit cast to (const char **) is required. A valid units
      specification must be included as part of the input String unless one of
      the units keyword entries is the empty string "".
  <-  int *Key
      Index of the matched units keyword. This value is set to -1 if no match
      is found. Key can be NULL, otherwise the value is as follows.
       -1 - No match
        0 - Match to the first keyword
        1 - Match to the second keyword
       ...

Author / revision:
P. Kabal
$Revision: 1.9 $  $Date: 2023/04/04 19:31:02 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFinfo.h>

#define WS_DELIM    2      /* Use white-space as a delimiter */
#define NCBUF       100
#define NOUNITS     0
#define UNITS       1
#define SVCODE_7x7  119 /* V1 must be positive, without an explicit + sign;
                           V2 must be positive, without an explicit + sign */

static int
AF_parseUnits(const char String[], const char Delim[], const char *UnitsTable[],
              int *NcV);


int
AFdecInfoDDU(const char String[], double Dval[2], const char *UnitsTable[],
             int *Key)

{
  char cbuf[NCBUF+1];
  char *string;
  int nc, status, key, NcV;

  /* Allocate temp storage */
  nc = (int) strlen(String);
  string = cbuf;
  if (nc > NCBUF)
    string = (char *) UTmalloc(nc+1);

  status = 1;
  key = AF_parseUnits(String, "/", UnitsTable, &NcV);
  if (key >= 0) {
    STcopyNMax(String, string, NcV, nc);
    status = STdecDfrac(string, SVCODE_7x7, Dval);
  }

  if (Key != NULL)
    *Key = key;

  if (nc > NCBUF)
    UTfree((void *) string);

  return status;
}

int
AFdecInfoIIU(const char String[], int Ival[2], const char *UnitsTable[],
             int *Key)

{
  char cbuf[NCBUF+1];
  char *string;
  int nc, status, key, NcV;

  /* Allocate temp storage */
  nc = (int) strlen(String);
  string = cbuf;
  if (nc > NCBUF)
    string = (char *) UTmalloc(nc+1);

  status = 1;
  key = AF_parseUnits(String, "/", UnitsTable, &NcV);
  if (key >= 0) {
    STcopyNMax(String, string, NcV, nc);
    status = STdecIfrac(string, SVCODE_7x7, Ival);
  }

  if (Key != NULL)
    *Key = key;

  if (nc > NCBUF)
    UTfree((void *) string);

  return status;
}

static int
AF_parseUnits(const char String[], const char Delim[], const char *UnitsTable[],
              int *NcV)

{
  char *token;
  const char *p;
  char cbuf[NCBUF+1];
  int nc, Flag, key;

  /* Allocate temporary space */
  nc = (int) strlen(String);
  if (nc > NCBUF)
    token = (char *) UTmalloc(nc+1);
  else
    token = cbuf;

/* There are several (correct) cases:
     123 Unit
     123/456 Unit (/ may be surrounded by whitespace)
     123
     123/456      (/ may be surrounded by whitespace)
   The goal is to identify the end of the numeric part and the beginning of the
   Unit part (if any).

   1. Identify the presence of a Delim using p=STfindToken(String, Delim, ...).
      The delimiter is either a character from Delim / or whitespace. The
      returned token string is ignored.
      (a) If no delimiter was found (p == NULL), the Units part is missing.
      (b) If the search terminated at whitespace (p-1) marks the end of the
          numeric part.
      (c) If the search terminated at a character from Delim, process the string
          starting at p using only whitespace as a delimiter.
          (i) If this second pass terminates at whitespace (p != NULL), (p-1)
              marks the end of the numeric part.
          (ii) If no delimiter was found (p == NULL), the Units part is missing.
   2. (a) If the Units part is not present, treat the Units as an empty string.
      (b) If the Units part is present, process the string using STfindToken
          starting after the whitespace delimiter.
          (i) If a further whitespace terminator is found, the Units part is
              not acceptable.
          (ii) Otherwise go to Step 3,
   3. The token string (possibly empty) is compared to the Units keywords.
*/
  p = STfindToken(String, Delim, "", token, WS_DELIM, nc);   /* Delim + WS */
  if (p == NULL)
    Flag = NOUNITS;
  else if (strchr(Delim, *(p-1)) == NULL)  /* Ended with whitespace */
    Flag = UNITS;
  else {                                   /* Ended with Delim */
    p = STfindToken(p, "", "", token, WS_DELIM, nc);       /* WS delimiter */
    if (p == NULL)
      Flag = NOUNITS;
    else
      Flag = UNITS;
  }

  /* Process/decode the Units values */
  if (Flag == NOUNITS) {
    *NcV = nc;                            /* Length of the value string */
    key = STkeyMatch("", UnitsTable);     /* Check if empty Units is OK */
  }
  else {
    *NcV = (int) ((p-1)-String);          /* Length of value string */
    p = STfindToken(p, "", "", token, WS_DELIM, nc);
    if (p != NULL)   /* Invalid, trailing non-whitespace characters */
      key = -1;
    else
      key = STkeyMatch(token, UnitsTable);
  }

  /* De-allocate memory */
  if (token != cbuf)
    UTfree(token);

  return key;
}
