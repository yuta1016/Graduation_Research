/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann                                                 */
/*               TU Muenchen-LIS                                             */
/* based on   :  Momusys VM                                                  */
/*****************************************************************************/

/******* SCCSInfo ************************************************************/
/* Filename :    %M%
   Version  :    %I%
   Last Edit:    %E% %U%
   Released :    %D% %T%   */
/******* SCCSInfo ************************************************************/

/******* INCLUDES ************************************************************/
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <vopio.h>

/******* DEFINES *************************************************************/

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/

/******* FUNCTIONS ***********************************************************/
void pix_diff(TPixel *res,TPixel *in1,TPixel *in2);
void pix_hvfir121(TPixel *res,TPixel *in, TCoor x, TCoor y);
int fifo_lable_schk(TPixel *in,TCoor x,TCoor y);
int fifo_lable_nchk(TPixel *in,int pos,TCoor x,TCoor y);
void fifo_expand_dequeue(TPixel *res,TPixel *in,TCoor x,TCoor y);

/******* Code ****************************************************************/

