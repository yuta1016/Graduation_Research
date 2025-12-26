/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTsetProg (const char Program[]);
  char *UTgetProg (void)

Purpose:
  Set the program name for messages
  Get the program name for messages

Description:
  This routine sets a program name string. This string is used by error
  reporting routines as an identifier in error message strings.

Parameters:
  <-  void UTsetProg
   -> const char Program[]
      Program name, normally without a path

  <-  char *UTgetProg
      Pointer to the program name. If the program name has not been set, this is
      a pointer to an empty string.

Author / revision:
  P. Kabal
  $Revision: 1.18 $  $Date: 2023/04/10 12:49:09 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>

static char *Pgm = NULL;


void
UTsetProg(const char Program[])

{
  int nc;

  UTfree((void *) Pgm);
  nc = (int) strlen(Program);
  if (nc > 0) {
    Pgm = (char *) UTmalloc(nc + 1);
    STcopyMax(Program, Pgm, nc);
  }
  else
    Pgm = NULL;
}



char *
UTgetProg (void)

{
  if (Pgm == NULL)
    return "";
  else
    return Pgm;
}
