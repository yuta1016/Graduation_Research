/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecSrange(const char String[], int Tindex[2], const char *SymTab[])

Purpose:
  Decode a range specification for symbolic values

Description:
  This routine decodes a string specifying a range of symbolic values. The range
  is specified in the form "a" or "a:b". Optional white-space (as defined by
  isspace) can surround the values. For the case of a single value "a", this is
  equivalent to the range "a:a". The outputs are a pair of integer values which
  are the indices of the symbol values in a symbol table SymTable. If an error
  is encountered, neither output value is set.

Parameters:
  <-  int STdecSrange
      Error status, 0 for no error, 1 for error
   -> const char String[]
      Input string
  <-  int Tindex[2]
      Array containing the index for first and second symbolic values.
   -> const char *SymbTab[]
      Pointer array with pointers to the symbol strings. The end of the symbol
      table is signalled with a NULL pointer. Note that with ANSI C, if the
      actual parameter is not declared to have the const attribute, an explicit
      cast to (const char **) is required.

Author / revision:
  P. Kabal
  $Revision: 1.5 $  $Date: 2023/03/14 01:28:33 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23

#define DP_EMPTY  0
#define DP_LVAL   1
#define DP_DELIM  2
#define DP_RVAL   4


int
STdecSrange(const char String[], int Tindex[2], const char *SymTab[])

{
  int status;
  int Ti[2];

  /* Decode the range values */
  status = STdecSpair(String, ":", Ti, SymTab);

  if (status == DP_LVAL) {
    Tindex[0] = Ti[0];
    Tindex[1] = Ti[0];
    status = 0;
  }
  else if (status == (DP_LVAL + DP_DELIM + DP_RVAL)) {
    Tindex[0] = Ti[0];
    Tindex[1] = Ti[1];
    status = 0;
  }
  else {
    UTwarn("STdecSrange - %s: \"%s\"", STM_DataErr, STstrDots(String, MAXC));
    status = 1;
  }

  return status;
}
