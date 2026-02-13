/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  STmsg.h

Description:
  Message texts for the TSP ST routines.

Author / revision:
  P. Kabal
  $Revision: 1.5 $  $Date: 2023/03/23 20:19:49 $

----------------------------------------------------------------------*/

#ifndef STmsg_h_
#define STmsg_h_

/* Error messages */

/* Warning messages */
#define STM_DataErr     "Data format/value error"
#define STM_BadIdent    "Invalid identifier"
#define STM_EmptyData   "Empty data string"
#define STM_ExtraChars  "Extra characters follow parameter"
#define STM_InvVal      "Invalid data value"
#define STM_StrTrunc    "String truncated"
#define STM_TooFewData  "Too few data values"

/* Informational messages */
#define STM_ExtraData   "Extra data ignored"
#define STM_SmallVal    "Value too small, set to zero"
#define STM_UnBalQuote  "Unbalanced quotes"

#endif  /* STmsg_h_ */
