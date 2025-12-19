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
/* Filename :    $RCSfile: lifo.c,v $
   Version  :    Revision: 1.7
   Last Edit:    Date: 1999/11/03 14:12:28
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
// Applicable File Name:  lifo.c
//
*/

/******* INCLUDES ************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <momusys.h>
#include <stdlib.h>
#include "address.h"
#include "nhood.h"
#include "lifo.h"


/******* DEFINES *************************************************************/
#define STACKSIZE 65536
#define uchar unsigned char


/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
TChan _lifo_regin1=0,_lifo_regin2=0,_lifo_regin3=0,_lifo_regin4=0;
TChan _lifo_regout1=0,_lifo_regout2=0,_lifo_regout3=0,_lifo_regout4=0;
void *_lifo_infopt=0;

/******* FUNCTIONS ***********************************************************/

/******* Code ****************************************************************/

/* central function for connected area related functions (recursive approach)*/
void lifo_proc(MomVop *resvop, MomVop *invop,MomImage *aux,MomImage *marker,
		  int reschanels,int inchanels,
		  int connect,int scanmode,TPos *list,int listsize,
		  TChan ahold,TChan yhold,TChan uhold,TChan vhold,TChan axhold,
		  int (*startcrit)(TPixel *in,TCoor x,TCoor y),
		  int (*neighcrit)(TPixel *in,int pos,TCoor x,TCoor y),
		  void (*pushop)(TPixel *res, TPixel *in,TCoor x,TCoor y),
		  void (*popoper)(TPixel *res, TPixel *in,TCoor x,TCoor y))
{
  static uchar *stackbuffer=0;
  uchar *stackpos;
  uchar valid,pos;
  int nextdir=4;
  int dir=0,
    enddir=0,
    dirinc=0;
  TNhood regs;
  int listpos;
  int startpixel=0;
/*  TSizeConv noconv=NOCONV;*/

  ConstructNhood(&regs,resvop,invop,aux,marker,1,
	      reschanels,inchanels,connect,scanmode,
	      ahold,yhold,uhold,vhold,axhold,
	      -1,-1,0,0,0,0,size1to1);
  if (regs.error) {
    DestructNhood(&regs);
    return;
  }

  if (connect==CON_4) dirinc=2;
  else dirinc=1;

  /*init stack*/
  if (!stackbuffer) {
    stackbuffer=(uchar *)malloc(STACKSIZE);
    if (!stackbuffer) {
      fprintf(stderr,"Warning: Out of memory, Skipping connecteted area\n");
      DestructNhood(&regs);
      return;
    }
  }
  stackpos=stackbuffer;


  /* loop for imagescan*/
  switch (regs._n_scanmode) {
  case SCAN_HM:
    /* horizontal mander scan*/
    dir=dirinc;
    enddir=0;
    
/*    regs._n_ia.u=regs.stia.u;
    regs._n_iy.u=regs.stiy.u;
    regs._n_iu.u=regs.stiu.u;
    regs._n_iv.u=regs.stiv.u;
    regs._n_m.u=regs.stm.u;
    regs._n_ax.u=regs.stax.u;
    regs._n_ra.u=regs.stra.u;
    regs._n_ry.u=regs.stry.u;
    regs._n_ru.u=regs.stru.u;
    regs._n_rv.u=regs.strv.u;*/
    
    LoadIfUnload3X3(&regs,12);
/*    matrix[12].a=_n_iaget(regs._n_ia);
    matrix[12].y=_n_iyget(regs._n_iy);
    matrix[12].u=_n_iuget(regs._n_iu);
    matrix[12].v=_n_ivget(regs._n_iv);
    matrix[12].m=_n_mget(regs._n_m);
    matrix[12].ax=_n_axget(regs._n_ax);*/
    

    dir=4;

    while (1) {
      
      /* execute embedded function*/
      if (regs.matrix[12].m==MARKER_LOADED) {
	valid=startcrit(&regs.matrix[12],regs._n_cntx,regs._n_cnty);
      }
      else valid=0;


      if (valid) {
	/*set marker*/
	regs.matrix[12].m=MARKER_CRIT_TRUE;
/*	regs._n_mset(regs._n_m,regs.matrix[12].m);*/

	/*push to stack*/
	*stackpos=dir;
	dir=enddir=(dir+4)&7;
	stackpos++;

	/*do recursiv adressing*/
	do {
	  /*next direction*/
	  dir+=dirinc;
	  dir&=7;

	  if (dir == enddir) {
	    
	    if (popoper) {
	      popoper(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
	      if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	      if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	      if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	      if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	      if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);
	      regs.matrix[12].ax=regs.result.ax;
	    }
	    /* set marker*/
	    regs.matrix[12].m=MARKER_POPED;
	    regs._n_mset(regs._n_m,regs.matrix[12].m);

	    
	    
	    /* mv pointers*/
	    if ((stackbuffer+1) < stackpos) {
	      Shift(&regs,dir);
	    }
	    
	    /* pop from stack*/
	    stackpos--;
	    dir=(*stackpos)&7;
	    if (stackpos> stackbuffer) {
	      stackpos--;
	      enddir=((*stackpos)+4)&7; /* ^4*/
	      stackpos++;
	    }
/*	    regs.LoadIfUnload3X3(12);*/
	  }
	  else {
	    switch (dir) {
	    case 0: pos=11;
	      break;
	    case 1: pos=6;
	      break;
	    case 2: pos=7;
	      break;
	    case 3: pos=8;
	      break;
	    case 4: pos=13;
	      break;
	    case 5: pos=18;
	      break;
	    case 6: pos=17;
	      break;
	    case 7: pos=16;
	      break;
	    default: pos=12;
	      fprintf(stderr,"Error: Internal error in lifo_proc\n");
	      break;
	    }
	    
	    LoadIfUnload3X3(&regs,(int)pos);
	    if (regs.matrix[pos].m==MARKER_LOADED) {
	      if (neighcrit(regs.matrix,pos,regs._n_cntx,regs._n_cnty)) {
	      
		/*set marker regs._n_pare to push*/
		regs.matrix[pos].m=MARKER_CRIT_TRUE;

		/* pushoperation*/
		
		if (pushop) {
		  pushop(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);
		  regs.matrix[12].ax=regs.result.ax;
		}

	      
		/* check for push*/
		if (stackbuffer+STACKSIZE == stackpos) {
		  fprintf(stderr,"Warning: Stackoverflow in connected area\n");
		  regs.matrix[pos].m=MARKER_LOADED;
		}
		else {
		
		  regs.matrix[12].m=MARKER_PUSHED;
		  regs._n_mset(regs._n_m,regs.matrix[12].m);
		  /*move pointers*/
		  Shift(&regs,dir);
		  
		  /*set marker*/
		  regs.matrix[12].m=MARKER_CRIT_TRUE;

		  
		  /*push*/
		  *stackpos=dir;
		  dir=enddir=(dir+4)&7;
		  stackpos++;
		}
	      }
	    }
	  }
	} while (stackpos > stackbuffer); /*stack not empty*/
/*	dir-=dirinc;
	dir&=7;*/
      }
      
      /* goto next pixel*/
      if ((dir==4) && (regs._n_cntx== (regs.width-1))) {
	nextdir=0;
	dir=6;
      }
      if (!dir && !regs._n_cntx) {
	nextdir=4;
	dir=6;
      }
      if (dir==6) {
	if (regs._n_cnty==(regs.height-1)) break;
	Shift(&regs,dir);
	dir=nextdir;
      }
      else Shift(&regs,dir);
    }
    break;
  case SCAN_LIST:
    /* horizontal mander scan*/
    dir=dirinc;
    enddir=0;
    
/*    dir=4;  can be fixed*/
    listpos=0;

    while (listpos < listsize) {
      
      /* load pixel from list*/
      Goto(&regs,list->x,list->y);
      for (pos=0; pos<25;regs.matrix[pos++].m=0);
      list++;
      listpos++;
      LoadIfUnload3X3(&regs,12);
      
      /* execute embedded function*/
      if (regs.matrix[12].m==MARKER_LOADED) {
	valid=startcrit(&regs.matrix[12],regs._n_cntx,regs._n_cnty);
      }
      else valid=0;

      if (valid) {
	/*set marker*/
	regs.matrix[12].m=MARKER_CRIT_TRUE;
/*	regs._n_mset(regs._n_m,regs.matrix[12].m);*/

	/*push to stack*/
	*stackpos=4;
/*	enddir=(dir+4)&7;
	dir=(dir+4-dirinc)&7; can be fixed*/
	enddir=0;
	dir=(-dirinc)&7;
	stackpos++;
	startpixel=1;


	/*do recursiv adressing*/
	do {
	  /*next direction*/
	  dir+=dirinc;
	  dir&=7;

	  if (!startpixel && (dir == enddir)) {
	    
	    if (popoper) {
	      popoper(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
	      if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	      if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	      if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	      if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	      if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);
	      regs.matrix[12].ax=regs.result.ax;
	    }
	    /* set marker*/
	    regs.matrix[12].m=MARKER_POPED;
	    regs._n_mset(regs._n_m,regs.matrix[12].m);

	    
	    
	    /* mv pointers*/
	    if ((stackbuffer+1) < stackpos) {
	      Shift(&regs,dir);
	    }
	    
	    /* pop from stack*/
	    stackpos--;
	    dir=(*stackpos)&7;
	    if (stackpos> stackbuffer) {
	      stackpos--;
	      enddir=((*stackpos)+4)&7; /* ^4*/
	      stackpos++;
	    }
/*	    regs.LoadIfUnload3X3(12);*/
	  }
	  else {
	    switch (dir) {
	    case 0: pos=11;
	      break;
	    case 1: pos=6;
	      break;
	    case 2: pos=7;
	      break;
	    case 3: pos=8;
	      break;
	    case 4: pos=13;
	      break;
	    case 5: pos=18;
	      break;
	    case 6: pos=17;
	      break;
	    case 7: pos=16;
	      break;
	    default: pos=12;
	      fprintf(stderr,"Error: Internal error in lifo_proc\n");
	      break;
	    }
	    
	    LoadIfUnload3X3(&regs,(int)pos);
	    if (regs.matrix[pos].m==MARKER_LOADED) {
	      if (neighcrit(regs.matrix,pos,regs._n_cntx,regs._n_cnty)) {
	      
		/*set marker regs._n_pare to push*/
		regs.matrix[pos].m=MARKER_CRIT_TRUE;

		/* pushoperation*/
		
		if (pushop) {
		  pushop(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);
		  regs.matrix[12].ax=regs.result.ax;
		}

	      
		/* check for push*/
		if (stackbuffer+STACKSIZE == stackpos) {
		  fprintf(stderr,"Warning: Stackoverflow in connected area\n");
		  regs.matrix[pos].m=MARKER_LOADED;
		}
		else {
		
		  regs.matrix[12].m=MARKER_PUSHED;
		  regs._n_mset(regs._n_m,regs.matrix[12].m);
		  /*move pointers*/
		  Shift(&regs,dir);
		  
		  /*set marker*/
		  regs.matrix[12].m=MARKER_CRIT_TRUE;

		  
		  /*push*/
		  *stackpos=dir;
		  dir=enddir=(dir+4)&7;
		  stackpos++;
		}
	      }
	    }
	  }
	  startpixel=0;
	} while (stackpos > stackbuffer); /*stack not empty*/
/*	dir-=dirinc;
	dir&=7;*/
      }
    }
    break;
  default:
    fprintf(stderr,"Warning: Unsupported scane mode in connected area\n");
    DestructNhood(&regs);
    /*free(stackbuffer);  stack is static!*/
    return;
  }
  DestructNhood(&regs);
  /* free(stackbuffer);  stack is static!*/
  return;
}

