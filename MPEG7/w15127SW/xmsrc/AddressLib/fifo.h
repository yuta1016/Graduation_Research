/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann  26/2/96                                        */
/*               TU Muenchen-LIS  und  FhG-IFT                               */
/* based on   :  Momusys VM                                                  */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    $RCSfile: fifo.h,v $
   Version  :    Revision: 1.10
   Last Edit:    Date: 2001/08/02 07:54:42
   Released :    %D% %T%   */
/**** LISSCCSInfo ************************************************************/


/*
///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// S. Herrmann TU-Munich, Institute for Integrated Circuits
// (contributing organizations names)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  fifo.h
//
*/

#ifndef _FIFO_
#define _FIFO_

/******* INCLUDES ************************************************************/
#include<momusys.h>
#include"address.h"

/******* DEFINES *************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define FIFOSIZE 0x100000
#define RESET_HFIFO 1
#define KEEP_HFIFO  0

/* Treat naming conflict of MoMuSys and ImageMagick*/
#ifndef _MYMOMUSYS_
#ifndef _NAMECONFICTDEFS_
#define _NAMECONFICTDEFS_
#define MomImageType ImageType
#define MomImageData ImageData
#define MomImage Image
#define MomVopExtend VopExtend
#define MomVop Vop
#define momvop vop
#endif
#endif

#ifdef ADDPROF
#ifndef CNTFIFOSIZE
#define CNTFIFOSIZE
#endif
#endif
#ifdef EVAL
#ifndef CNTFIFOSIZE
#define CNTFIFOSIZE
#endif
#endif
#ifdef DEBUG
#ifndef CNTFIFOSIZE
#define CNTFIFOSIZE
#endif
#endif

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
extern TChan _fifo_regin1,_fifo_regin2,_fifo_regin3,_fifo_regin4,_fifo_regin5;
extern TChan _fifo_regout1,_fifo_regout2,_fifo_regout3,_fifo_regout4;
extern short _fifo_level,_fifo_distlevel;
/* global variable to fill the gap between high level algorithm and pixel
   processing function */
extern void *_fifo_infopt;
/* pointer to a database with segment features*/


#ifdef CNTFIFOSIZE
extern int fifosize, maxcallfifosize, maxfifosize;
#endif


/******* FUNCTIONS ***********************************************************/
void fifo_proc(MomVop *result, MomVop *in1,
			 MomImage *aux,MomImage *marker,
			 int reschannels,
			 int in1channels,
			 int connect,int scanmode,int processmode,
			 TChan ahold,TChan yhold,TChan uhold,
			 TChan vhold,TChan axhold,
			 int (*startcrit)(TPixel *in,TCoor x,TCoor y),
			 int (*neighcrit)(TPixel *in,int pos,TCoor x,TCoor y),
			 void (*operation)(TPixel *res, TPixel *in,
					   TCoor x,TCoor y));

void hfifo_proc(MomVop *resvop, MomVop *in1,
			  MomImage *aux,MomImage *marker,
			  int reschannels,
			  int in1channels,
			  int connect,int scanmode,int processmode,
			  short startlevel,short stoplevel,short initfifo,
			  TChan ahold,TChan yhold,TChan uhold,
			  TChan vhold,TChan axhold,
			  int (*startcrit)(TPixel *in,TCoor x,TCoor y),
			  int (*neighcrit)(TPixel *in,int pos,TCoor x,TCoor y),
			  void (*operation)(TPixel *res, TPixel *in,
					    TCoor x,TCoor y));

void interhfifo_proc(MomVop *resvop, MomVop *in1, MomVop *in2,
			       MomImage *aux,MomImage *marker,
			       int reschannels,
			       int in1channels,int in2channels,
			       int connect,int scanmode,int processmode,
			       short startlevel,short stoplevel,short initfifo,
			       TChan ahold,TChan yhold,TChan uhold,
			       TChan vhold,TChan axhold,
			       int (*startcrit)(TPixel *in1,TPixel *in2,
						TCoor x,TCoor y),
			       int (*neighcrit)(TPixel *in1,TPixel *in2,
						int pos,TCoor x,TCoor y),
			       void (*operation)(TPixel *res,
						 TPixel *in1,TPixel *in2,
						 TCoor x,TCoor y));

void fillstartlist(TPos *list,int listsize, short initfifo);
             /* put starpixel list in hfifobuffer address space*/
void addstartpixel(TCoor x, TCoor y);
             /* put one startpixel to startpixel list in hfifo address space
	        to be used in high level algorithm*/
void proc_addstartpixel(TCoor x, TCoor y);
             /* put one startpixel to startpixel list in hfifo address space
	        to be used in processing methodes*/
void dumpstartlist(TPos *list,int *listsize, short initfifo);
             /* get starpixel list from hfifobuffer address space*/


/* address routine for connected area fifo addressing mode
        all connected pixels which fulfill a certain homogenity
        criterion are processed
        a result at each pixel position is computet from the pixel
	itself and its neighbors
	windows  (res = f(in,N(in))*/
/* result  = reference (pointer) to result Vop
                 NULL pointer is allowewed*/
/* input   = references to input Vops
                 NULL pointer is allowewed*/
/* aux     = auxillary Image. If this image is used, it is an IO
             buffer (in oposite to result or input). I.e. results
	     are also written directly to the internal input matrix*/
/* marker  = marker is a special image which holds information about
             the "processing state" of the pixels. This image is needed
             for the interiour addressing routines. if a zero pointer is
             given this Image is greated by the addressing routine.
             In order to take this information from one address function
             call to the next it is possible to define a common marker
	     buffer which is referenced by this parameter
	     Since the marker values are give to the pixel operations
	     the values can be used in this functions:
             MARKER_INVALID 0     = pixel is outside of image
             MARKER_NOT_LOADED 0  = pixel is not loaded from image buffer
                                   (invalid value in matrix)
             MARKER_LOADED 1      = pixel is loaded from image buffer
             MARKER_CRIT_TRUE 2   = pixel fulfills homogenity criterion
             MARKER_PUSHED 3      = pixel is pushed to stack (lifo)
             MARKER_QUEUED 3      = pixel is queued to fifo (fifo)
             MARKER_POPED 4       = pixel was poped from stack (lifo)
             MARKER_DEQUEUED 4    = pixel was dequeued from fifo (fifo)
             MARKER_PROCESSED 5   = pixel is completely processed*/

/* reschannels= ored list of channels to be written to the result Vop
                e.g. A_CH | Y_CH | U_CH | V_CH when all channels are
                           computed*/
/* in1channels= ored list of channels to be read from image buffers
                           unused channels are set to zero values*/
/* connect= type of neighborhood
             CON_0 0  = center only
             CON_4 1  = center + 4 neighbours
             CON_8 2  = center + 8 neighbours
             CON_12 3 = center + 12 nearest pixels
             CON_24 4 = center + 24 nearest pixels
             CON_H3 5 = center + horizontal neighbours
             CON_H5 6 = center + 2 horizontal neighbours
             CON_V3 7 = center + vertical neighbours
             CON_V5 8 = center + 2 vertical neighbours

	     CON_4 and CON_8 are treated in the same way, since
	     this information is only used to preload the pixel matrix
	     The connectivity for pixel processing is then defined by the
	     pixel "operation"*/
            
/* scanmode= scheme in which order the pixels are processed
             SCAN_HM = scaning first line from left to right, second line
                       from right to left ... (horizontal meander)
             SCAN_LIST= scaning from a list of pixels*/

/* processmode= processing model (when results are written to the pixel)
              QOUT_PROC= process on queue out
	      QIN_PROC = process on queue in*/


/* list    = pointer to pixel list to be processed or NULL if no list is
             needed*/
/* listsize= if list is used then number of elements in list*/
/* startlevel= startlevel with hierchical fifo
           =  0 invalid (no processed hierarchy)
	   =  1 highest (first) hierarchy
	   =  max least (last) hierachy*/
/* stopevel= last proicessed level with hierchical fifo
           like startlevel, must graeter or equal then starthier*/
/* initfifo= flag if fifo and startpointers has to be reset*/
/* *hold   = values which are set to invalid pixel positions
           =  -1 doublicate pixel values from previous valid pixel
                 (or if no right or upper neighbour then take the value
                  from left or lower neighbor
           =  else take the given value   */
/* winx,winy= size of processed window
           = -1,-1 means total frame*/
/* resposx,resposy= top left corner of window location in Vop of
   inposx,inposy,             result and input*/

/* startcrit= user function to check criterion for startpixel of
              connected area (is called for every pixel in frame or list)
              reurnvalue 0 = criterion is not fulfilled
              else criterion is fulfilled*/
/* neighcrit= user function to check homogenity criterion for neighbors*/
/* operation= user function to process pixel after dequing */




/* complete list of defines for addressing routines*/
/* #define A_CH 1 */
/* #define Y_CH 2 */
/* #define U_CH 4 */
/* #define V_CH 8 */
/* #define AX_CH 16 */
/* #define M_CH 32 */

/* #define CON_0 0  *//* center only*/ 
/* #define CON_4 1  *//* center + 4 neighbours*/
/* #define CON_8 2  *//* center + 8 neighbours*/
/* #define CON_12 3 *//*center + 12 nearest pixels*/
/* #define CON_24 4 *//*center + 24 nearest pixels*/
/* #define CON_H2 5 *//*center + horizontal neighbours*/
/* #define CON_H4 6 *//*center + 2 horizontal neighbours*/
/* #define CON_V2 7 *//*center + vertical neighbours*/
/* #define CON_V4 8 *//*center + 2 vertical neighbours*/

/* #define SCAN_HO 0       *//* from top to buttom, from left to right*/
/* #define SCAN_VE 1       *//* from left to right, from top to buttom*/
/* #define SCAN_HM 2       *//* horizontal meander*/
/* #define SCAN_VM 3       *//* vertical meander*/
/* #define SCAN_LOOP 4     *//* loop with decrising radius*/
/* #define SCAN_LIST 5     *//* read positions from list*/
/* #define SCAN_COLLIST 21 *//* read positions from collected list*/
/* #define SCAN_COLLECT 16 *//* collect start pixels for fifo mode*/

/* #define	DIR_LEFT 0 */
/* #define	DIR_UP_LEFT 1 */
/* #define	DIR_UP 2 */
/* #define	DIR_UP_RIGHT 3 */
/* #define	DIR_RIGHT 4 */
/* #define	DIR_DOWN_RIGHT 5 */
/* #define	DIR_DOWN 6 */
/* #define	DIR_DOWN_LEFT 7 */

/* #define MARKER_INVALID 0 */
/* #define MARKER_NOT_LOADED 0 */
/* #define MARKER_LOADED 1 */
/* #define MARKER_CRIT_TRUE 2 */
/* #define MARKER_PUSHED 3 */
/* #define MARKER_QUEUED 3 */
/* #define MARKER_POPED 4 */
/* #define MARKER_DEQUEUED 4 */
/* #define MARKER_PROCESSED 5 */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
