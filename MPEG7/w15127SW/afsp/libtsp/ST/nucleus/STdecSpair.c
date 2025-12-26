/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecSpair(const char String[], const char Delim[], int Tindex[2],
                 const char *SymTab[])

Purpose:
  Decode a data specification for a pair of symbolic values

Description:
  This routine decodes a string specifying a pair of symbolic values. The pair
  is specified as "a", "a:", ":b", or "a:". The delimiter is shown as ":" in the
  example, but a general delimiter string can be specified in the argument list.
  Optional white-space (as defined by isspace) can surround the values. The
  outputs are a pair of integer values which are the indices of the symbol
  values in a symbol table SymTable.

Parameters:
  <-  int STdecSpair
      Return code, negative for errors.
        0 - Empty string
        1 - Left value (no delimiter)
        2 - Delimiter alone
        3 - Left value + delimiter
        6 - Delimiter + right value
        7 - Left value + delimiter + right value
   -> const char String[]
      Input string
   -> const char Delim[];
      Delimiter string. This value is used in STfindToken to separate the two
      values.
  <-  int Tindex[2]
      Array of indices for the first and second symbolic value.
   -> const char *SymbTab[]
      Pointer array with pointers to the symbol strings. The end of the symbol
      table is signalled with a NULL pointer. Note that with ANSI C, if the
      actual parameter is not declared to have the const attribute, an explicit
      cast to (const char **) is required.

Author / revision:
  P. Kabal
  $Revision: 1.10 $  $Date: 2023/04/04 19:06:19 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define NCBUF    80

#define DP_BADV  -2
#define DP_BADF  -1
#define DP_EMPTY  0
#define DP_LVAL   1
#define DP_DELIM  2
#define DP_RVAL   4

#define WS_CHAR   0
#define WS_TRIM   1
#define WS_DELIM  2

int
STdecSpair(const char String[], const char Delim[], int Tindex[2],
           const char *SymTab[])

{
  const char *p;
  char *token;
  int nc, status, I1, I2;
  char cbuf[NCBUF+1];

  /* Allocate temporary space */
  nc = (int) strlen(String);
  token = cbuf;
  if (nc > NCBUF)
    token = (char *) UTmalloc(nc + 1);

  I1 = 0;   /* Keep compiler happy */
  I2 = 0;
  status = DP_EMPTY;

  /* Separate the input into two parts; token has left part, p the right part */
  p = STfindToken(String, Delim, "", token, WS_DELIM, nc);
  if (p != NULL && *(p-1) == ' ')   /* Check for an embedded whitespace char */
    status = DP_BADF;

  /* Pick up the first value */
  if (status >= 0 && token[0] != '\0') {
    status += DP_LVAL;          /* Left value present */
    I1 = STkeyMatch(token, SymTab);
    if (I1 < 0)
      status = DP_BADV;
  }

  if (status >= 0 && p != NULL) {   /* No error and delimiter  */

    /* Pick up the next token */
    p = STfindToken(p, Delim, "", token, WS_DELIM, nc);
    if (p != NULL)
      status = DP_BADF;         /* Trailing delimiter (whitespace or /) */
    else
      status += DP_DELIM;       /* Delimiter present */
  }

  /* Pick up the second value */
  if (status >= DP_DELIM && token[0] != '\0') {
    status += DP_RVAL;        /* Right value present */
    I2 = STkeyMatch(token, SymTab);
    if (I2 < 0)
      status = DP_BADV;
  }

  /* Deallocate storage */
  if (nc > NCBUF)
    UTfree((void *) token);

  if (status > 0) {
    if (status | DP_LVAL)
      Tindex[0] = I1;
    if (status | DP_RVAL)
      Tindex[1] = I2;
  }

  return status;
}
