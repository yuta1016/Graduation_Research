/****** CopyRightBegin *******************************************************/
/*                                                                           */
/* Copyright (C) 1996 TU-Muenchen LIS All Rights Reserved.                   */
/*                                                                           */
/****** CopyRightEnd *********************************************************/

/*****************************************************************************/
/* CREATED BY :  S. Herrmann  26/2/96                                        */
/*               TU Muenchen-LIS                                             */
/* based on   :  Momusys VM                                                  */
/*****************************************************************************/

/**** LISSCCSInfo ************************************************************/
/* Filename :    $RCSfile: fifo.c,v $
   Version  :    Revision: 1.14
   Last Edit:    Date: 2002/01/15 17:37:05
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
// Applicable File Name:  fifo.c
//
*/

/******* INCLUDES ************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <momusys.h>
#include "vopio.h"
#include "fifo.h"
#include "nhood.h"
#ifdef EVAL
  #include "evaladdr.h"
  #include "eval.h"
#endif
#ifdef PPTRACE
  #include "pptrace.h"
#endif


/******* DEFINES *************************************************************/
#define HIERLEVELS 256
#define uchar unsigned char
#define MULTI_SIGLE_QUEUEIN  ~SCAN_MULIN


/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
TChan _fifo_regin1=0,_fifo_regin2=0,_fifo_regin3=0,_fifo_regin4=0,
  _fifo_regin5=0;
TChan _fifo_regout1=0,_fifo_regout2=0,_fifo_regout3=0,_fifo_regout4=0;
short _fifo_level,_fifo_distlevel;
void *_fifo_infopt;
/* pointer to a database with segment features*/

static short dirtoxoff[8]={-1,-1,0,1,1,1,0,-1};
static short dirtoyoff[8]={0,-1,-1,-1,0,1,1,1};

static THFifoEntry hfifobuffer[FIFOSIZE];
static THFifoEntry *fifofirst; /* queue is empty if pointer = 0*/
static THFifoEntry *fifolast; /* queue is empty if pointer = 0*/
static THFifoEntry *stophlifo[HIERLEVELS];
static THFifoEntry *freelistpos; /* used to get free entries in freelistinc*/
static THFifoEntry *freeliststop; /* to of free list stack*/
static THFifoEntry *startliststop; /* for LIST mode*/
static short freelistinc;

#ifdef EVAL
static int eval_NCResult, eval_NCCrit;
#endif

#ifdef CNTFIFOSIZE
int fifosize, maxcallfifosize,maxfifosize=0;
#endif

/******* FUNCTIONS ***********************************************************/

/******* Code ****************************************************************/

/* central function for connected area related functions (fifo approach)*/
void fifo_proc(MomVop *resvop, MomVop *in1,
	       MomImage *aux,MomImage *marker,
	       int reschannels,
	       int in1channels,
	       int connect,int scanmode,int processmode,
	       TChan ahold,TChan yhold,TChan uhold,TChan vhold,TChan axhold,
	       int (*startcrit)(TPixel *in,TCoor x,TCoor y),
	       int (*neighcrit)(TPixel *in,int pos,TCoor x,TCoor y),
	       void (*operation)(TPixel *res, TPixel *in,TCoor x,TCoor y))

{
  TNhood regs;
  THFifoEntry *fifopos;              /* needed for startlist*/
  static TPos fifobuffer[FIFOSIZE]; /* not shared memory area with
				       hfifobuffer and startpixel list*/
  TPos *fifoend;                    /* first position after fifobuffer 
				       (= wrap arround point)*/
  TPos *fifostart, *fifostop;
  TPos *lasthfifoindistlevel;
  short lastindistlevel;
  short deniqueuein;
  short valid;
  short startdir,dirinc,dir;
  TCoor lineend;
  TCoor stx,sty;                    /* to store position of pixel scan*/
  MomImageData stia,stiy,stiu,stiv,stax,stm,stra,stry,stru,strv;
  void (*defaultdir)(TNhood *);
  void (*pre)(TNhood *nhood,TCoor dx,TCoor dy);
  short pos;

#ifdef EVAL
  eval_BeginMeasureDur();
  eval_BeginAddrMode(SECTION_FIFO);
  eval_BeginRecMode(in1,resvop,in1channels,reschannels);
  eval_PictSize(in1,NULL,resvop,aux);
  eval_BeginScan(scanmode);
  eval_ScanWindow(-1,-1,0,0,0,0,0,0);
  eval_BeginCon(connect);
  eval_BeginConSC(connect);
  eval_BeginProc(processmode);
  eval_BeginOpr((void*)operation);
  eval_BeginSCrit((void*)startcrit);
  eval_BeginNCrit((void*)neighcrit);
  eval_StartAddr();
#endif

#ifdef PPTRACE
  PPTrace_StartOfTool();
  PPTrace_LogType((void*)operation,reschannels|M_CH,connect);
  PPTrace_LogFifoReg(1);
#endif

  /* check input parameters*/
  ConstructNhood(&regs,resvop,in1,aux,marker,1,
	      reschannels,in1channels,connect,scanmode,
	      ahold,yhold,uhold,vhold,axhold,
	      -1,-1,0,0,0,0,size1to1);
  if (regs.error) {
    DestructNhood(&regs);
    return;
  }

  /* set direction incrementer for neighborhood*/
  if (connect==CON_4) {
    startdir=0;
    dirinc=2;
    pre=&Preload4;  
  }
  else if (connect < CON_H2) {
    startdir=0;
    dirinc=1;
    pre=&Preload8;
  }
  else {
    if (connect < CON_V2) {
      startdir=0;
    }
    else {
      startdir=2;
    }
    dirinc=4;
    pre=&Preload4;  
  }


  /*init start and stop pointer for fifos*/
  fifostart=fifobuffer;
  fifostop=fifobuffer;
  fifoend=fifostart+FIFOSIZE; /*position is outside of fifo!!*/

  if (!(regs._n_scanmode & SCAN_LIST)) {
    freelistpos=hfifobuffer;
    freeliststop=0;
    startliststop=0;
    memset(stophlifo,0,HIERLEVELS*sizeof(THFifoEntry *));
    freelistinc=1;
#ifdef CNTFIFOSIZE
    fifosize=maxcallfifosize=0;
#endif
  }

#ifdef RESETLIST
    fifoposin=fifobuffer;
    for (i=1; i<FIFOSIZE; i++) {
      fifoposin->next=fifoposin+1;
      fifoposin++;
    }
    fifoposin->next=0;
    stophfifo[0]=fifoposin;
    fifoposout=fifobuffer;
    starthfifo[0]=fifoposout;
#endif

    
  /* set comparision value for queue in, > queued or > dequeued*/
  if (regs._n_scanmode & SCAN_MULIN) {
    deniqueuein=MARKER_DEQUEUED;
  }
  else {
    deniqueuein=MARKER_QUEUED;
  }
  /* loop for imagescan*/
  switch (regs._n_scanmode & MULTI_SIGLE_QUEUEIN) {
  case SCAN_HO:
    /* noncollected horizontal scan*/

#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;

    while (1) {

#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
  
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid?1:0);
#endif

        
	if (valid) {
#ifdef EVAL
	  eval_LogSCResult(2);
#endif
	  /* queue in*/
	  fifostop++;

	  if (fifostop==fifoend) fifostop=fifobuffer;
	  if (fifostop==fifostart) {
	    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	    exit(1);
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifostop->x=regs._n_cntx;
	  fifostop->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /* save current position*/
	  stx=regs._n_cntx;
	  sty=regs._n_cnty;

	  stia.u=regs._n_ia.u;
	  stiy.u=regs._n_iy.u;
	  stiu.u=regs._n_iu.u;
	  stiv.u=regs._n_iv.u;
	  stax.u=regs._n_ax.u;
	  stm.u=regs._n_m.u;
	  stra.u=regs._n_ra.u;
	  stry.u=regs._n_ry.u;
	  stru.u=regs._n_ru.u;
	  strv.u=regs._n_rv.u;
	  
	  /* initialize values for internal distance counter*/
	  lasthfifoindistlevel=fifostop;
	  lastindistlevel=_fifo_distlevel=0;

	  /*do recursiv adressing*/
	  while (fifostart!=fifostop) {  /*while fifo nor empty*/

	    /* dequeue from queue(level)*/
	    fifostart++;

	    if (fifostart==fifoend) fifostart=fifobuffer;

	    /*compute distlevel for hierarchy*/
	    if (lastindistlevel) {
	      _fifo_distlevel++;
	      lasthfifoindistlevel=fifostop;
	      lastindistlevel=0;
#ifdef EVAL
	      eval_LogDist();
#endif
	    }
	    if (fifostart == lasthfifoindistlevel) {
	      lastindistlevel=1;
	    }

#ifdef CNTFIFOSIZE
	    fifosize--;
#endif
#ifdef EVAL
	    eval_LogNCPos(fifostart->x,fifostart->y);
#endif
	    /* goto current position*/
	    pre(&regs,fifostart->x,fifostart->y);

	    /* check if pixel was dequeued in multiqueuein-mode*/
	    if (regs.matrix[12].m==MARKER_DEQUEUED) {
#ifdef EVAL
        eval_LogNCResult(0,0);
#endif
	      continue;
	    }

	    /*set marker*/
	    regs.matrix[12].m=MARKER_DEQUEUED;

	    /* pixel operation*/
	    if (operation) {
#ifdef PPTRACE
	      PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
				  regs._n_cnty, 0, _fifo_distlevel);
#endif
	      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	      PPTrace_LogFifoOPOut(regs.result);
#endif

	      if (processmode == QOUT_PROC) {
		/* store results*/
		if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		/* store recursiv results to matrix*/
		/* regs.matrix[12].ax=regs.result.ax;*/
	      }

	    }

	    /*write marker*/
	    regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
      eval_NCResult = 1;
      eval_NCCrit = 0;
#endif
	    /* queue in neighbors*/
	    dir=startdir;
	    while (dir < 8) {

	      /* check neighbor */
	      pos=dirtopos[dir];

	      if (regs.matrix[pos].m && (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		if (eval_NCResult==1)
		  eval_NCResult = 2;
#endif
#ifdef PPTRACE
		PPTrace_LogFifoNCIn(regs.matrix);
#endif
		valid=(short)neighcrit(regs.matrix,
				       (int)pos,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		PPTrace_LogFifoNCOut(pos, valid?1:0);
#endif

		if (valid) {

#ifdef EVAL
		  eval_NCResult = 3;
		  eval_NCCrit |= (1<<dir);
#endif
		  /* queue in*/
		  fifostop++;

		  if (fifostop==fifoend) fifostop=fifobuffer;
		  if (fifostop==fifostart) {
		    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
		    exit(1);
		  }

#ifdef CNTFIFOSIZE
		  fifosize++;
		  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif


		  /* new fifo values*/
		  fifostop->x=regs._n_cntx+dirtoxoff[dir];
		  fifostop->y=regs._n_cnty+dirtoyoff[dir];

		  regs.result.m=MARKER_QUEUED;
		  if (processmode == QOUT_PROC) {
		    MUpdate(&regs,pos);
		  }
		  else {
		    UpdatePos(&regs,pos);
		  }
		}
	      }
	      /* next direction*/
	      dir += dirinc;
	    }
#ifdef EVAL
	    eval_LogNCResult(eval_NCResult, eval_NCCrit);
#endif

	  }

	  /*goto prev position*/
	  regs._n_cntx=regs._n_icntx=stx;
	  regs._n_cnty=regs._n_icnty=sty;

	  regs._n_ia.u=stia.u;
	  regs._n_iy.u=stiy.u;
	  regs._n_iu.u=stiu.u;
	  regs._n_iv.u=stiv.u;
	  regs._n_ax.u=stax.u;
	  regs._n_m.u =stm.u;
	  regs._n_ra.u=stra.u;
	  regs._n_ry.u=stry.u;
	  regs._n_ru.u=stru.u;
	  regs._n_rv.u=strv.u;

	  /* load matrix with previous values*/
	  Preload(&regs);
	}
#ifdef EVAL
	else
	  eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif


      /* goto next pixel */
      if (regs._n_cntx==lineend) {

        /* check for frame end */
        if (regs._n_cnty == (regs._n_maxy)) break;

        /* go to next result line */
        regs._n_cnty++;

        regs.stra.u+=regs._n_rals;
        regs.stry.u+=regs._n_ryls;
        regs.stru.u+=regs._n_ruls;
        regs.strv.u+=regs._n_rvls;
        regs.stax.u+=regs._n_axls;
        regs.stm.u+=regs._n_mls;

        /* go to input line start  */
        regs._n_icnty++;

        regs.stia.u+=regs._n_ials;
        regs.stiy.u+=regs._n_iyls;
        regs.stiu.u+=regs._n_iuls;
        regs.stiv.u+=regs._n_ivls;

        /* reset x counter and set pointer to linestart */
        regs._n_cntx=0;
        regs._n_icntx=0;

        regs._n_ia.u=regs.stia.u;
        regs._n_iy.u=regs.stiy.u;
        regs._n_iu.u=regs.stiu.u;
        regs._n_iv.u=regs.stiv.u;
        regs._n_ax.u=regs.stax.u;
        regs._n_m.u=regs.stm.u;
        regs._n_ra.u=regs.stra.u;
        regs._n_ry.u=regs.stry.u;
        regs._n_ru.u=regs.stru.u;
        regs._n_rv.u=regs.strv.u;
        
        /* preload linestart */
#ifdef EVAL
        eval_LogPreload();
#endif
        Preload(&regs);
      }
      else {
        ShiftRightLineLoad(&regs);
      }
    }
    break;
    
  case SCAN_HO|SCAN_COLLECT:
    /* noncollected horizontal meander scan*/
#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid?1:0);
#endif

	if (valid) {

#ifdef EVAL
	  eval_LogSCResult(2);
#endif
	  /* queue in*/
	  fifostop++;

	  if (fifostop==fifoend) fifostop=fifobuffer;
	  if (fifostop==fifostart) {
	    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	    exit(1);
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifostop->x=regs._n_cntx;
	  fifostop->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
	else
	  eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel */
      if (regs._n_cntx==lineend) {
        
        /* check for frame end */
        if (regs._n_cnty == (regs._n_maxy)) break;

        /* go to next result line */
        regs._n_cnty++;

        regs.stra.u+=regs._n_rals;
        regs.stry.u+=regs._n_ryls;
        regs.stru.u+=regs._n_ruls;
        regs.strv.u+=regs._n_rvls;
        regs.stax.u+=regs._n_axls;
        regs.stm.u+=regs._n_mls;

        /* go to input line start  */
        regs._n_icnty++;

        regs.stia.u+=regs._n_ials;
        regs.stiy.u+=regs._n_iyls;
        regs.stiu.u+=regs._n_iuls;
        regs.stiv.u+=regs._n_ivls;

        /* reset x counter and set pointer to linestart */
        regs._n_cntx=0;
        regs._n_icntx=0;

        regs._n_ia.u=regs.stia.u;
        regs._n_iy.u=regs.stiy.u;
        regs._n_iu.u=regs.stiu.u;
        regs._n_iv.u=regs.stiv.u;
        regs._n_ax.u=regs.stax.u;
        regs._n_m.u=regs.stm.u;
        regs._n_ra.u=regs.stra.u;
        regs._n_ry.u=regs.stry.u;
        regs._n_ru.u=regs.stru.u;
        regs._n_rv.u=regs.strv.u;
        
        /* preload linestart */
#ifdef EVAL
        eval_LogPreload();
#endif
        Preload(&regs);
      }
      else {
        ShiftRightLineLoad(&regs);
      }
    }

    /* initialize values for internal distance counter*/
    lasthfifoindistlevel=fifostop;
    lastindistlevel=_fifo_distlevel=0;

    /*do recursiv adressing*/
    while (fifostart!=fifostop) {  /*while fifo nor empty*/

      /* dequeue from queue(level)*/
      fifostart++;

      if (fifostart==fifoend) fifostart=fifobuffer;

      /*compute distlevel for hierarchy*/
      if (lastindistlevel) {
	_fifo_distlevel++;
	lasthfifoindistlevel=fifostop;
	lastindistlevel=0;
#ifdef EVAL
	eval_LogDist();
#endif
      }
      if (fifostart == lasthfifoindistlevel) {
	lastindistlevel=1;
      }

#ifdef CNTFIFOSIZE
      fifosize--;
#endif
#ifdef EVAL
      eval_LogNCPos(fifostart->x,fifostart->y);
#endif

      /* goto current position*/
      pre(&regs,fifostart->x,fifostart->y);

      /* check if pixel was dequeued in multiqueuein-mode*/
      if (regs.matrix[12].m==MARKER_DEQUEUED) {
#ifdef EVAL
        eval_LogNCResult(0,0);
#endif
        continue;
      }

      /*set marker*/
      regs.matrix[12].m=MARKER_DEQUEUED;

      /* pixel operation*/
      if (operation) {
#ifdef PPTRACE
        PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
                            regs._n_cnty, 0, _fifo_distlevel);
#endif
	operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoOPOut(regs.result);
#endif

	if (processmode == QOUT_PROC) {
	  /* store results*/
	  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	  /* store recursiv results to matrix*/
	  /* regs.matrix[12].ax=regs.result.ax;*/
	}

      }

      /*write marker*/
      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
      eval_NCResult = 1;
      eval_NCCrit = 0;
#endif
      /* queue in neighbors*/
      dir=startdir;
      while (dir < 8) {

	/* check neighbor */
	pos=dirtopos[dir];

	if (regs.matrix[pos].m &&
	    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	  if (eval_NCResult==1)
	    eval_NCResult = 2;
#endif
          
#ifdef PPTRACE
          PPTrace_LogFifoNCIn(regs.matrix);
#endif
          valid=(short)neighcrit(regs.matrix,(int)pos,
				 regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
          PPTrace_LogFifoNCOut(pos, valid?1:0);
#endif
	  if (valid) {

#ifdef EVAL
	    eval_NCResult = 3;
	    eval_NCCrit |= (1<<dir);
#endif
	    /* queue in*/
	    fifostop++;

	    if (fifostop==fifoend) fifostop=fifobuffer;
	    if (fifostop==fifostart) {
	      addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	      exit(1);
	    }

#ifdef CNTFIFOSIZE
	    fifosize++;
	    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	    /* new fifo values*/
	    fifostop->x=regs._n_cntx+dirtoxoff[dir];
	    fifostop->y=regs._n_cnty+dirtoyoff[dir];

	    /* set marker*/
	    regs.result.m=MARKER_QUEUED;
	    if (processmode == QOUT_PROC) {
	      MUpdate(&regs,pos);
	    }
	    else {
	      UpdatePos(&regs,pos);
	    }
	  }
	}

	/* next direction*/
	dir += dirinc;

      }
#ifdef EVAL
      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
    }

    break;

  case SCAN_HM:
    /* noncollected horizontal meander scan*/

#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;

    while (1) {

#ifdef EVAL
	eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
  
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid?1:0);
#endif

        
	if (valid) {
#ifdef EVAL
	  eval_LogSCResult(2);
#endif
	  /* queue in*/
	  fifostop++;

	  if (fifostop==fifoend) fifostop=fifobuffer;
	  if (fifostop==fifostart) {
	    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	    exit(1);
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifostop->x=regs._n_cntx;
	  fifostop->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /* save current position*/
	  stx=regs._n_cntx;
	  sty=regs._n_cnty;

	  stia.u=regs._n_ia.u;
	  stiy.u=regs._n_iy.u;
	  stiu.u=regs._n_iu.u;
	  stiv.u=regs._n_iv.u;
	  stax.u=regs._n_ax.u;
	  stm.u=regs._n_m.u;
	  stra.u=regs._n_ra.u;
	  stry.u=regs._n_ry.u;
	  stru.u=regs._n_ru.u;
	  strv.u=regs._n_rv.u;
	  
	  /* initialize values for internal distance counter*/
	  lasthfifoindistlevel=fifostop;
	  lastindistlevel=_fifo_distlevel=0;

	  /*do recursiv adressing*/
	  while (fifostart!=fifostop) {  /*while fifo nor empty*/

	    /* dequeue from queue(level)*/
	    fifostart++;

	    if (fifostart==fifoend) fifostart=fifobuffer;

	    /*compute distlevel for hierarchy*/
	    if (lastindistlevel) {
	      _fifo_distlevel++;
	      lasthfifoindistlevel=fifostop;
	      lastindistlevel=0;
#ifdef EVAL
	      eval_LogDist();
#endif
	    }
	    if (fifostart == lasthfifoindistlevel) {
	      lastindistlevel=1;
	    }

#ifdef CNTFIFOSIZE
	    fifosize--;
#endif
#ifdef EVAL
	    eval_LogNCPos(fifostart->x,fifostart->y);
#endif
	    /* goto current position*/
	    pre(&regs,fifostart->x,fifostart->y);

	    /* check if pixel was dequeued in multiqueuein-mode*/
	    if (regs.matrix[12].m==MARKER_DEQUEUED) {
#ifdef EVAL
	      eval_LogNCResult(0,0);
#endif
	      continue;
	    }

	    /*set marker*/
	    regs.matrix[12].m=MARKER_DEQUEUED;

	    /* pixel operation*/
	    if (operation) {
#ifdef PPTRACE
	      PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
				  regs._n_cnty, 0, _fifo_distlevel);
#endif
	      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	      PPTrace_LogFifoOPOut(regs.result);
#endif

	      if (processmode == QOUT_PROC) {
		/* store results*/
		if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		/* store recursiv results to matrix*/
		/* regs.matrix[12].ax=regs.result.ax;*/
	      }

	    }

	    /*write marker*/
	    regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	    eval_NCResult = 1;
	    eval_NCCrit = 0;
#endif
	    /* queue in neighbors*/
	    dir=startdir;
	    while (dir < 8) {

	      /* check neighbor */
	      pos=dirtopos[dir];

	      if (regs.matrix[pos].m && (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		if (eval_NCResult==1)
		  eval_NCResult = 2;
#endif
#ifdef PPTRACE
		PPTrace_LogFifoNCIn(regs.matrix);
#endif
		valid=(short)neighcrit(regs.matrix,(int)pos,
				       regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		PPTrace_LogFifoNCOut(pos, valid?1:0);
#endif

		if (valid) {

#ifdef EVAL
		  eval_NCResult = 3;
		  eval_NCCrit |= (1<<dir);
#endif
		  /* queue in*/
		  fifostop++;

		  if (fifostop==fifoend) fifostop=fifobuffer;
		  if (fifostop==fifostart) {
		    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
		    exit(1);
		  }

#ifdef CNTFIFOSIZE
		  fifosize++;
		  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif


		  /* new fifo values*/
		  fifostop->x=regs._n_cntx+dirtoxoff[dir];
		  fifostop->y=regs._n_cnty+dirtoyoff[dir];

		  regs.result.m=MARKER_QUEUED;
		  if (processmode == QOUT_PROC) {
		    MUpdate(&regs,pos);
		  }
		  else {
		    UpdatePos(&regs,pos);
		  }
		}
	      }
	      /* next direction*/
	      dir += dirinc;
	    }
#ifdef EVAL
	    eval_LogNCResult(eval_NCResult, eval_NCCrit);
#endif

	  }

	  /*goto prev position*/
	  regs._n_cntx=regs._n_icntx=stx;
	  regs._n_cnty=regs._n_icnty=sty;

	  regs._n_ia.u=stia.u;
	  regs._n_iy.u=stiy.u;
	  regs._n_iu.u=stiu.u;
	  regs._n_iv.u=stiv.u;
	  regs._n_ax.u=stax.u;
	  regs._n_m.u =stm.u;
	  regs._n_ra.u=stra.u;
	  regs._n_ry.u=stry.u;
	  regs._n_ru.u=stru.u;
	  regs._n_rv.u=strv.u;

	  /* load matrix with previous values*/
	  Preload(&regs);
	}
#ifdef EVAL
	else
	  eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif


      /* goto next pixel*/
      if (regs._n_cntx==lineend) {
	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
	ShiftDownLineLoad(&regs);

	if (defaultdir==ShiftRightLineLoad) {
	  lineend=0;
	  defaultdir=ShiftLeftLineLoad;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=ShiftRightLineLoad;
	}
      }
      else {
	defaultdir(&regs);
      }
    }
    break;
    
  case SCAN_HM|SCAN_COLLECT:
    /* noncollected horizontal meander scan*/
#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid?1:0);
#endif

	if (valid) {

#ifdef EVAL
	  eval_LogSCResult(2);
#endif
	  /* queue in*/
	  fifostop++;

	  if (fifostop==fifoend) fifostop=fifobuffer;
	  if (fifostop==fifostart) {
	    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	    exit(1);
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifostop->x=regs._n_cntx;
	  fifostop->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
	else
	  eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel*/
      if (regs._n_cntx==lineend) {

	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
	ShiftDownLineLoad(&regs);

	if (defaultdir==ShiftRightLineLoad) {
	  lineend=0;
	  defaultdir=ShiftLeftLineLoad;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=ShiftRightLineLoad;
	}
      }
      else {
	defaultdir(&regs);
      }
    }

    /* initialize values for internal distance counter*/
    lasthfifoindistlevel=fifostop;
    lastindistlevel=_fifo_distlevel=0;

    /*do recursiv adressing*/
    while (fifostart!=fifostop) {  /*while fifo nor empty*/

      /* dequeue from queue(level)*/
      fifostart++;

      if (fifostart==fifoend) fifostart=fifobuffer;

      /*compute distlevel for hierarchy*/
      if (lastindistlevel) {
	_fifo_distlevel++;
	lasthfifoindistlevel=fifostop;
	lastindistlevel=0;
#ifdef EVAL
	eval_LogDist();
#endif
      }
      if (fifostart == lasthfifoindistlevel) {
	lastindistlevel=1;
      }

#ifdef CNTFIFOSIZE
      fifosize--;
#endif
#ifdef EVAL
      eval_LogNCPos(fifostart->x,fifostart->y);
#endif

      /* goto current position*/
      pre(&regs,fifostart->x,fifostart->y);

      /* check if pixel was dequeued in multiqueuein-mode*/
      if (regs.matrix[12].m==MARKER_DEQUEUED) {
#ifdef EVAL
        eval_LogNCResult(0,0);
#endif
        continue;
      }

      /*set marker*/
      regs.matrix[12].m=MARKER_DEQUEUED;

      /* pixel operation*/
      if (operation) {
#ifdef PPTRACE
        PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
                            regs._n_cnty, 0, _fifo_distlevel);
#endif
	operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoOPOut(regs.result);
#endif

	if (processmode == QOUT_PROC) {
	  /* store results*/
	  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	  /* store recursiv results to matrix*/
	  /* regs.matrix[12].ax=regs.result.ax;*/
	}

      }

      /*write marker*/
      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
      eval_NCResult = 1;
      eval_NCCrit = 0;
#endif
      /* queue in neighbors*/
      dir=startdir;
      while (dir < 8) {

	/* check neighbor */
	pos=dirtopos[dir];

	if (regs.matrix[pos].m &&
	    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	  if (eval_NCResult==1)
	    eval_NCResult = 2;
#endif
          
#ifdef PPTRACE
          PPTrace_LogFifoNCIn(regs.matrix);
#endif
          valid=(short)neighcrit(regs.matrix,(int)pos,
				 regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
          PPTrace_LogFifoNCOut(pos, valid?1:0);
#endif
	  if (valid) {

#ifdef EVAL
	    eval_NCResult = 3;
	    eval_NCCrit |= (1<<dir);
#endif
	    /* queue in*/
	    fifostop++;

	    if (fifostop==fifoend) fifostop=fifobuffer;
	    if (fifostop==fifostart) {
	      addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	      exit(1);
	    }

#ifdef CNTFIFOSIZE
	    fifosize++;
	    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	    /* new fifo values*/
	    fifostop->x=regs._n_cntx+dirtoxoff[dir];
	    fifostop->y=regs._n_cnty+dirtoyoff[dir];

	    /* set marker*/
	    regs.result.m=MARKER_QUEUED;
	    if (processmode == QOUT_PROC) {
	      MUpdate(&regs,pos);
	    }
	    else {
	      UpdatePos(&regs,pos);
	    }
	  }
	}

	/* next direction*/
	dir += dirinc;

      }
#ifdef EVAL
      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
    }

    break;

  case SCAN_LIST:

    /* reset matrix*/
/*    for (pos=0; pos<25;regs.matrix[(int)pos++].m=0);*/

    while (startliststop) {

#ifdef EVAL
      eval_LogPreload();
      eval_LogSCPos(startliststop->x,startliststop->y);
#endif
    
      /* load pixel from list*/
      pre(&regs,startliststop->x,startliststop->y);

      /* goto next startpixel*/
      fifopos=startliststop;
      startliststop=startliststop->next;

      /* check in to emtpy list*/
      fifopos->next=freeliststop;
      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
      fifosize--;
#endif

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid?1:0);
#endif
        
	if (valid) {

#ifdef EVAL
	  eval_LogSCResult(2);
#endif
	  /* queue in*/
	  fifostop++;

	  if (fifostop==fifoend) fifostop=fifobuffer;
	  if (fifostop==fifostart) {
	    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	    exit(1);
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifostop->x=regs._n_cntx;
	  fifostop->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /* initialize values for internal distance counter*/
	  lasthfifoindistlevel=fifostop;
	  lastindistlevel=_fifo_distlevel=0;

	  /*do recursiv adressing*/
	  while (fifostart!=fifostop) {  /*while fifo nor empty*/

	    /* dequeue from queue(level)*/
	    fifostart++;

	    if (fifostart==fifoend) fifostart=fifobuffer;

	    /*compute distlevel for hierarchy*/
	    if (lastindistlevel) {
	      _fifo_distlevel++;
	      lasthfifoindistlevel=fifostop;
	      lastindistlevel=0;
#ifdef EVAL
	      eval_LogDist();
#endif
	    }
	    if (fifostart == lasthfifoindistlevel) {
	      lastindistlevel=1;
	    }

#ifdef CNTFIFOSIZE
	    fifosize--;
#endif
#ifdef EVAL
      eval_LogNCPos(fifostart->x,fifostart->y);
#endif
	    /* goto current position*/
	    pre(&regs,fifostart->x,fifostart->y);

	    /* check if pixel was dequeued in multiqueuein-mode*/
	    if (regs.matrix[12].m==MARKER_DEQUEUED) {
#ifdef EVAL
	      eval_LogNCResult(0,0);
#endif
	      continue;
	    }

	    /*set marker*/
	    regs.matrix[12].m=MARKER_DEQUEUED;

	    /* pixel operation*/
	    if (operation) {
#ifdef PPTRACE
	      PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
				  regs._n_cnty, 0, _fifo_distlevel);
#endif
	      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	      PPTrace_LogFifoOPOut(regs.result);
#endif

	      if (processmode == QOUT_PROC) {
		/* store results*/
		if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		/* store recursiv results to matrix*/
		/* regs.matrix[12].ax=regs.result.ax;*/
	      }

	    }

	    /*write marker*/
	    regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	    eval_NCResult = 1;
	    eval_NCCrit = 0;
#endif	    /* queue in neighbors*/
	    dir=startdir;
	    while (dir < 8) {

	      /* check neighbor */
	      pos=dirtopos[dir];

	      if (regs.matrix[pos].m &&
		  (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		if (eval_NCResult==1)
		  eval_NCResult = 2;
#endif
#ifdef PPTRACE
		PPTrace_LogFifoNCIn(regs.matrix);
#endif
		valid=(short)neighcrit(regs.matrix,
				       (int)pos,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		PPTrace_LogFifoNCOut(pos, valid?1:0);
#endif
          
		if (valid) {
#ifdef EVAL
		  eval_NCResult = 3;
		  eval_NCCrit |= (1<<dir);
#endif
		  /* queue in*/
		  fifostop++;

		    if (fifostop==fifoend) fifostop=fifobuffer;
		  if (fifostop==fifostart) {
		    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
		    exit(1);
		  }

#ifdef CNTFIFOSIZE
		  fifosize++;
		  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

		  /* new fifo values*/
		  fifostop->x=regs._n_cntx+dirtoxoff[dir];
		  fifostop->y=regs._n_cnty+dirtoyoff[dir];

		  /* set marker*/
		  regs.result.m=MARKER_QUEUED;
		  if (processmode == QOUT_PROC) {
		    MUpdate(&regs,pos);
		  }
		  else {
		    UpdatePos(&regs,pos);
		  }
		}
	      }

	      /* next direction*/
	      dir += dirinc;

	    }
#ifdef EVAL
	    eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
	  }
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
    }

    break;

  case SCAN_LIST|SCAN_COLLECT:
/*    for (pos=0; pos<25;regs.matrix[pos++].m=0);*/

    while (startliststop) {

#ifdef EVAL
      eval_LogPreload();
      eval_LogSCPos(startliststop->x,startliststop->y);
#endif
      /* load pixel from list*/
      pre(&regs,startliststop->x,startliststop->y);

      /* goto next startpixel*/
      fifopos=startliststop;
      startliststop=startliststop->next;

      /* check in to emtpy list*/
      fifopos->next=freeliststop;
      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
      fifosize--;
#endif

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid?1:0);
#endif
        
	if (valid) {

#ifdef EVAL
	  eval_LogSCResult(2);
#endif

	  /* queue in*/
	  fifostop++;

	  if (fifostop==fifoend) fifostop=fifobuffer;
	  if (fifostop==fifostart) {
	    addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	    exit(1);
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifostop->x=regs._n_cntx;
	  fifostop->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
    }

    /* initialize values for internal distance counter*/
    lasthfifoindistlevel=fifostop;
    lastindistlevel=_fifo_distlevel=0;

    /*do recursiv adressing*/
    while (fifostart!=fifostop) {  /*while fifo nor empty*/

      /* dequeue from queue(level)*/
      fifostart++;

      if (fifostart==fifoend) fifostart=fifobuffer;

      /*compute distlevel for hierarchy*/
      if (lastindistlevel) {
	_fifo_distlevel++;
	lasthfifoindistlevel=fifostop;
	lastindistlevel=0;
#ifdef EVAL
	eval_LogDist();
#endif
      }
      if (fifostart == lasthfifoindistlevel) {
	lastindistlevel=1;
      }

#ifdef CNTFIFOSIZE
      fifosize--;
#endif
#ifdef EVAL
      eval_LogNCPos(fifostart->x,fifostart->y);
#endif

      /* goto current position*/
      pre(&regs,fifostart->x,fifostart->y);

      /* check if pixel was dequeued in multiqueuein-mode*/
      if (regs.matrix[12].m==MARKER_DEQUEUED) {
#ifdef EVAL
        eval_LogNCResult(0,0);
#endif
        continue;
      }

      /*set marker*/
      regs.matrix[12].m=MARKER_DEQUEUED;

      /* pixel operation*/
      if (operation) {
#ifdef PPTRACE
        PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
                            regs._n_cnty, 0, _fifo_distlevel);
#endif
	      operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoOPOut(regs.result);
#endif

	if (processmode == QOUT_PROC) {
	  /* store results*/
	  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	  /* store recursiv results to matrix*/
	  /* regs.matrix[12].ax=regs.result.ax;*/
	}

      }

      /*write marker*/
      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
      eval_NCResult = 1;
      eval_NCCrit = 0;
#endif
      /* queue in neighbors*/
      dir=startdir;
      while (dir < 8) {

	/* check neighbor */
	pos=dirtopos[dir];

	if (regs.matrix[pos].m &&
	    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	  if (eval_NCResult==1)
	    eval_NCResult = 2;
#endif

#ifdef PPTRACE
          PPTrace_LogFifoNCIn(regs.matrix);
#endif
          valid=(short)neighcrit(regs.matrix,(int)pos,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
          PPTrace_LogFifoNCOut(pos, valid?1:0);
#endif
	  if (valid) {

#ifdef EVAL
	    eval_NCResult = 3;
	    eval_NCCrit |= (1<<dir);
#endif
	    /* queue in*/
	    fifostop++;

	    if (fifostop==fifoend) fifostop=fifobuffer;
	    if (fifostop==fifostart) {
	      addlib_stop_here("Warning: Fifo overflow in fifo\n",0,0);
	      exit(1);
	    }

#ifdef CNTFIFOSIZE
	    fifosize++;
	    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	    /* new fifo values*/
	    fifostop->x=regs._n_cntx+dirtoxoff[dir];
	    fifostop->y=regs._n_cnty+dirtoyoff[dir];

	    /* set marker*/
	    regs.result.m=MARKER_QUEUED;
	    if (processmode == QOUT_PROC) {
	      MUpdate(&regs,pos);
	    }
	    else {
	      UpdatePos(&regs,pos);
	    }
	  }
	}

	/* next direction*/
	dir += dirinc;

      }
#ifdef EVAL
      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
    }

    break;
  default:
    addlib_stop_here("Warning: Unsupported scan mode in fifo addressing\n",
		     0,0);
    DestructNhood(&regs);
    /*free(fifobuffer);  fifo is static!*/
    return;
  }
  DestructNhood(&regs);
  /* free(fifobuffer);  fifo is static!*/

#ifdef PPTRACE
  PPTrace_LogFifoReg(0);
#endif

#ifdef EVAL
  eval_EndMeasureDur();
  eval_EndAddr();
  eval_EndNCrit();
  eval_EndSCrit();
  eval_EndOpr();
  eval_EndProc();
  eval_EndConSC();
  eval_EndCon();
  eval_EndScan();
  eval_EndRecMode();
  eval_EndAddrMode();
#endif


#ifdef CNTFIFOSIZE
  if (maxcallfifosize > maxfifosize) maxfifosize=maxcallfifosize;
#endif

  return;
}

/******* hfifo_proc ******************************************************/
/* central function for connected area related functions (hfifo approach)*/
void hfifo_proc(MomVop *resvop, MomVop *in1,
		MomImage *aux,MomImage *marker,
		int reschannels,
		int in1channels,
		int connect,int scanmode,int processmode,
		short startlevel,short stoplevel,short initfifo,
		TChan ahold,TChan yhold,TChan uhold,TChan vhold,TChan axhold,
		int (*startcrit)(TPixel *in,TCoor x,TCoor y),
		int (*neighcrit)(TPixel *in,int pos,TCoor x,TCoor y),
		void (*operation)(TPixel *res, TPixel *in,TCoor x,TCoor y))

{
  THFifoEntry *fifopos;
  THFifoEntry *lasthfifoindistlevel=0;
  THFifoEntry **dequeuepointer;
  short lastindistlevel;
  short deniqueuein;
  TNhood regs;
  short valid;
  short startdir,dirinc,dir;
  TCoor lineend;
  TCoor stx,sty;                   /* to store position of pixel scan*/
  MomImageData stia,stiy,stiu,stiv,stax,stm,stra,stry,stru,strv;
  void (*defaultdir)(TNhood *);
  void (*pre)(TNhood *nhood,TCoor dx,TCoor dy);
  uchar pos;

#ifdef EVAL
  eval_BeginMeasureDur();
  eval_BeginAddrMode(SECTION_HFIFO);
  eval_BeginRecMode(in1,resvop,in1channels,reschannels);
  eval_PictSize(in1,NULL,resvop,aux);
  eval_BeginScan(scanmode);
  eval_ScanWindow(-1,-1,0,0,0,0,0,0);
  eval_BeginCon(connect);
  eval_BeginConSC(connect);
  eval_BeginProc(processmode);
  eval_BeginOpr((void*)operation);
  eval_BeginSCrit((void*)startcrit);
  eval_BeginNCrit((void*)neighcrit);
  eval_StartAddr();
#endif

#ifdef PPTRACE
  PPTrace_StartOfTool();
  PPTrace_LogType((void*)operation,reschannels|M_CH,connect);
  PPTrace_LogFifoReg(1);
#endif

  /* check input parameters*/
  ConstructNhood(&regs,resvop,in1,aux,marker,1,
	      reschannels,in1channels,connect,scanmode,
	      ahold,yhold,uhold,vhold,axhold,
	      -1,-1,0,0,0,0,size1to1);
  if (regs.error) {
    DestructNhood(&regs);
    return;
  }

  /* set direction incrementer for neighborhood*/
  if (connect==CON_4) {
    startdir=0;
    dirinc=2;
    pre=&Preload4;  
  }
  else if (connect < CON_H2) {
    startdir=0;
    dirinc=1;
    pre=&Preload8;
  }
  else {
    if (connect < CON_V2) {
      startdir=0;
    }
    else {
      startdir=2;
    }
    dirinc=4;
    pre=&Preload4;  
  }

  /* check level parameters*/
  if (startlevel> stoplevel) {
    _fifo_level=startlevel;
    startlevel=stoplevel;
    stoplevel=_fifo_level;
  }
  
  if (startlevel < 0) {
    startlevel=0;
    fprintf(stderr,"WARNING : hierachical fifo: startlevel must be between "
	    "0 and %d\n",HIERLEVELS-1);
  }
  if (startlevel >= HIERLEVELS) {
    startlevel=HIERLEVELS-1;
    fprintf(stderr,"WARNING : hierachical fifo: startlevel must be between "
	    "0 and %d\n",HIERLEVELS-1);
  }
  if (stoplevel < startlevel) {
    stoplevel=startlevel;
    fprintf(stderr,"WARNING : hierachical fifo: stoplevel must be between "
	    "%d and %d\n",startlevel,HIERLEVELS-1);
  }
  if (stoplevel >= HIERLEVELS) {
    stoplevel=HIERLEVELS-1;
    fprintf(stderr,"WARNING : hierachical fifo: stoplevel must be between "
	    "%d and %d\n",startlevel,HIERLEVELS-1);
  }

  /*init start and stop pointer for fifos*/
#ifdef DEBUG
  if ((regs._n_scanmode & SCAN_LIST) && initfifo) {
    fprintf(stderr,"WARNING : Reset FIFO should not happen with LIST mode\n");
    initfifo=0;
  }
#endif
  if (initfifo) {
    freelistpos=hfifobuffer;
    freeliststop=0;
    startliststop=0;
    memset(stophlifo,0,HIERLEVELS*sizeof(THFifoEntry *));
    freelistinc=1;
#ifdef CNTFIFOSIZE
    fifosize=maxcallfifosize=0;
#endif
  
#ifdef RESETLIST
    fifoposin=hfifobuffer;
    for (i=1; i<FIFOSIZE; i++) {
      fifoposin->next=fifoposin+1;
      fifoposin++;
    }
    fifoposin->next=0;
    stophlifo[0]=fifoposin;
    fifoposout=hfifobuffer;
    starthfifo[0]=fifoposout;
#endif
  }

  /* set comparision value for queue in, > queued or > dequeued*/
  if (regs._n_scanmode & SCAN_MULIN) {
    deniqueuein=MARKER_DEQUEUED;
  }
  else {
    deniqueuein=MARKER_QUEUED;
  }
  /* loop for imagescan*/
  switch (regs._n_scanmode & MULTI_SIGLE_QUEUEIN) {
  case SCAN_HO:
    /* noncollected horizontal meander scan*/

#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;
    _fifo_level=startlevel;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
  
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
#ifdef DEBUG
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
#endif
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    fifopos=freeliststop;
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /* save current position*/
	  stx=regs._n_cntx;
	  sty=regs._n_cnty;

	  stia.u=regs._n_ia.u;
	  stiy.u=regs._n_iy.u;
	  stiu.u=regs._n_iu.u;
	  stiv.u=regs._n_iv.u;
	  stax.u=regs._n_ax.u;
	  stm.u=regs._n_m.u;
	  stra.u=regs._n_ra.u;
	  stry.u=regs._n_ry.u;
	  stru.u=regs._n_ru.u;
	  strv.u=regs._n_rv.u;

	  /*do recursiv adressing*/
	  for (; _fifo_level <= stoplevel;
	       _fifo_level ++) {

	    /* get value from lifo*/
	    if ((fifopos=stophlifo[_fifo_level])) {
	      dequeuepointer=&stophlifo[_fifo_level];
	    }
	    else {

	      /*if lifo empty, try fifo*/
	      if ((fifopos=fifofirst)) {
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
	      }
	      else {
#ifdef EVAL
          eval_LogSkipLevel(_fifo_level);
#endif
          continue;
	      }
	    }
      
#ifdef EVAL
	    {
	      THFifoEntry *pTmp;
	      int ElemInHrch;

	      ElemInHrch=0;
	      for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
		ElemInHrch++;

	      eval_LogStartLevel(_fifo_level,ElemInHrch);
	    }
#endif
      
	    /*initialize distance counter*/
	    lastindistlevel=_fifo_distlevel=0;

	    while (fifopos) {  /*while fifo nor empty*/

	      /*compute distlevel for hierarchy*/
	      if (!stophlifo[_fifo_level]) {
		if (lastindistlevel) {
		  _fifo_distlevel++;
		  lasthfifoindistlevel=fifolast;
		  lastindistlevel=0;
#ifdef EVAL
		  eval_LogDist();
#endif
		}
		if (fifopos == lasthfifoindistlevel) {
		  lastindistlevel=1;
		}
	      }
        
#ifdef EVAL
	      eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	      /* goto current position*/
	      pre(&regs,fifopos->x,fifopos->y);

	      /* dequeue from queue(level)*/
	      *dequeuepointer=fifopos->next;

	      if (!(*dequeuepointer)) {
		/*switch from hierarchical lifo to fifo*/
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
		_fifo_distlevel=0;
	      }		

	      /* check in to emtpy list*/
	      fifopos->next=freeliststop;
	      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	      fifosize--;
#endif

	      /* check if pixel was dequeued in multiqueuein-mode*/
	      if (regs.matrix[12].m==MARKER_DEQUEUED) {
		/* set fifo to next element*/
		fifopos=*dequeuepointer;
#ifdef EVAL
		eval_LogNCResult(0,0);
#endif
		continue;
	      }

	      /*set marker*/
	      regs.matrix[12].m=MARKER_DEQUEUED;

	      /* pixel operation*/
	      if (operation) {
#ifdef PPTRACE
		PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
				    regs._n_cnty, _fifo_level,
				    _fifo_distlevel);
#endif
		operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		PPTrace_LogFifoOPOut(regs.result);
#endif

		if (processmode == QOUT_PROC) {
		  /* store results*/
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		  /* store recursiv results to matrix*/
		  /* regs.matrix[12].ax=regs.result.ax;*/
		}

	      }

	      /*write marker*/
	      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	      eval_NCResult = 1;
	      eval_NCCrit = 0;
#endif
	      /* queue in neighbors*/
	      dir=startdir;
	      while (dir < 8) {

		/* check neighbor */
		pos=dirtopos[dir];

		if (regs.matrix[pos].m &&
		    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		  if (eval_NCResult==1)
		    eval_NCResult = 2;
#endif

#ifdef PPTRACE
		  PPTrace_LogFifoNCIn(regs.matrix);
#endif
		  valid=(short)neighcrit(regs.matrix,(int)pos,
					 regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		  PPTrace_LogFifoNCOut(pos, valid);
#endif

		  if (valid) {
#ifdef EVAL
		    eval_NCResult = 3;
		    eval_NCCrit |= (1<<dir);
#endif

		    if (valid > HIERLEVELS) {
		      fprintf(stderr,"WARNING : hierachical fifo: "
			      "valid level must be between 1 and %d\n",
			      HIERLEVELS);
		      valid=HIERLEVELS;
		    }

		    /* queue in*/

		    /* get free entry*/
		    if (freelistinc) { 
		      /* emty fifo queue not completely initialised*/
		      fifopos=freelistpos;
		      freelistpos++;
		      fifopos=freelistpos;
		      if (freelistpos==(hfifobuffer+FIFOSIZE)) {
			freelistinc=0;
		      }
		    }
		    else {
		      /* check out from emtpy list*/
		      fifopos=freeliststop;
		      if (!fifopos) {
			addlib_stop_here("Warning: Fifo overflow in "
				"hierarchical fifo\n",0,0);
			exit(1);
		      }
		      freeliststop=freeliststop->next;
		    }
		    
		    /* check in to hierachical queue*/
		    valid--; /* allow hfifo0; old value for 
				valid=0 => invalid*/

		    /* to current fifo*/
		    if (valid<=_fifo_level) {
#ifdef EVAL
		      eval_LogNCLevel(_fifo_level);
#endif
		      if (fifofirst) { /* fifo not empty*/
			fifolast->next=fifopos;
		      }
		      else { /*fifo empty*/
			fifofirst=fifopos;
		      }
		      fifolast=fifopos;
		      fifolast->next=0;
		    }

		    /*to queue with lower priority*/
		    else {
#ifdef EVAL
		      eval_LogNCLevel(valid);
#endif
		      fifopos->next=stophlifo[valid];
		      stophlifo[valid]=fifopos;
		    }

#ifdef CNTFIFOSIZE
		    fifosize++;
		    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

		    /* new fifo values*/
		    fifopos->x=regs._n_cntx+dirtoxoff[dir];
		    fifopos->y=regs._n_cnty+dirtoyoff[dir];

		    /* set marker*/
		    regs.result.m=MARKER_QUEUED;
		    if (processmode == QOUT_PROC) {
		      MUpdate(&regs,pos);
		    }
		    else {
		      UpdatePos(&regs,pos);
		    }
		  }
		}
		/* next direction*/
		dir += dirinc;
	      }
#ifdef EVAL
	      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
	
	      /* set fifo to next element*/
	      fifopos=*dequeuepointer;

	    }
	  }

	  /* reset startlevel for next  startpixel*/
	  _fifo_level=startlevel;

	  /*goto prev position*/
	  regs._n_cntx=regs._n_icntx=stx;
	  regs._n_cnty=regs._n_icnty=sty;
	     
	  regs._n_ia.u=stia.u;
	  regs._n_iy.u=stiy.u;
	  regs._n_iu.u=stiu.u;
	  regs._n_iv.u=stiv.u;
	  regs._n_ax.u=stax.u;
	  regs._n_m.u =stm.u;
	  regs._n_ra.u=stra.u;
	  regs._n_ry.u=stry.u;
	  regs._n_ru.u=stru.u;
	  regs._n_rv.u=strv.u;

	  /* load matrix with previous values*/
	  Preload(&regs);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel */
      if (regs._n_cntx==lineend) {
        
        /* check for frame end */
        if (regs._n_cnty == (regs._n_maxy)) break;

        /* go to next result line */
        regs._n_cnty++;

        regs.stra.u+=regs._n_rals;
        regs.stry.u+=regs._n_ryls;
        regs.stru.u+=regs._n_ruls;
        regs.strv.u+=regs._n_rvls;
        regs.stax.u+=regs._n_axls;
        regs.stm.u+=regs._n_mls;

        /* go to input line start  */
        regs._n_icnty++;

        regs.stia.u+=regs._n_ials;
        regs.stiy.u+=regs._n_iyls;
        regs.stiu.u+=regs._n_iuls;
        regs.stiv.u+=regs._n_ivls;

        /* reset x counter and set pointer to linestart */
        regs._n_cntx=0;
        regs._n_icntx=0;

        regs._n_ia.u=regs.stia.u;
        regs._n_iy.u=regs.stiy.u;
        regs._n_iu.u=regs.stiu.u;
        regs._n_iv.u=regs.stiv.u;
        regs._n_ax.u=regs.stax.u;
        regs._n_m.u=regs.stm.u;
        regs._n_ra.u=regs.stra.u;
        regs._n_ry.u=regs.stry.u;
        regs._n_ru.u=regs.stru.u;
        regs._n_rv.u=regs.strv.u;
        
        /* preload linestart */
#ifdef EVAL
        eval_LogPreload();
#endif
        Preload(&regs);
      }
      else {
        ShiftRightLineLoad(&regs);
      }

    }
    break;
  case SCAN_HO|SCAN_COLLECT:
    /* noncollected horizontal meander scan*/
#ifdef EVAL
    eval_LogPreload();
#endif

    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;
    _fifo_level=startlevel;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: valid level must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    fifopos=freeliststop;
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical queue*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/

	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel */
      if (regs._n_cntx==lineend) {
        
        /* check for frame end */
        if (regs._n_cnty == (regs._n_maxy)) break;

        /* go to next result line */
        regs._n_cnty++;

        regs.stra.u+=regs._n_rals;
        regs.stry.u+=regs._n_ryls;
        regs.stru.u+=regs._n_ruls;
        regs.strv.u+=regs._n_rvls;
        regs.stax.u+=regs._n_axls;
        regs.stm.u+=regs._n_mls;

        /* go to input line start  */
        regs._n_icnty++;

        regs.stia.u+=regs._n_ials;
        regs.stiy.u+=regs._n_iyls;
        regs.stiu.u+=regs._n_iuls;
        regs.stiv.u+=regs._n_ivls;

        /* reset x counter and set pointer to linestart */
        regs._n_cntx=0;
        regs._n_icntx=0;

        regs._n_ia.u=regs.stia.u;
        regs._n_iy.u=regs.stiy.u;
        regs._n_iu.u=regs.stiu.u;
        regs._n_iv.u=regs.stiv.u;
        regs._n_ax.u=regs.stax.u;
        regs._n_m.u=regs.stm.u;
        regs._n_ra.u=regs.stra.u;
        regs._n_ry.u=regs.stry.u;
        regs._n_ru.u=regs.stru.u;
        regs._n_rv.u=regs.strv.u;
        
        /* preload linestart */
#ifdef EVAL
        eval_LogPreload();
#endif
        Preload(&regs);
      }
      else {
        ShiftRightLineLoad(&regs);
      }
    }
    
    /*do recursiv adressing*/
    for (; _fifo_level <= stoplevel; _fifo_level ++) {

      /* get value from lifo*/
      if ((fifopos=stophlifo[_fifo_level])) {
	dequeuepointer=&stophlifo[_fifo_level];
      }
      else {

	/*if lifo empty, try fifo*/
	if ((fifopos=fifofirst)) {
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	}
	else {
#ifdef EVAL
          eval_LogSkipLevel(_fifo_level);
#endif
          continue;
	}
      }

#ifdef EVAL
      {
        THFifoEntry *pTmp;
        int ElemInHrch;
        
        ElemInHrch=0;
        for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
          ElemInHrch++;
        
        eval_LogStartLevel(_fifo_level,ElemInHrch);
      }
#endif
      
      /*initialize distance counter*/
      lastindistlevel=_fifo_distlevel=0;

      while (fifopos) {  /*while fifo nor empty*/
	
	/*compute distlevel for hierarchy*/
	if (!stophlifo[_fifo_level]) {
	  if (lastindistlevel) {
	    _fifo_distlevel++;
	    lasthfifoindistlevel=fifolast;
	    lastindistlevel=0;
#ifdef EVAL
	    eval_LogDist();
#endif
	  }
	  if (fifopos == lasthfifoindistlevel) {
	    lastindistlevel=1;
	  }
	}

#ifdef EVAL
	eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	/* goto current position*/
	pre(&regs,fifopos->x,fifopos->y);

	/* dequeue from queue(level)*/
	*dequeuepointer=fifopos->next;

	if (!(*dequeuepointer)) {
	  /*switch from hierarchical lifo to fifo*/
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	  _fifo_distlevel=0;
	}		

	/* check in to emtpy list*/
	fifopos->next=freeliststop;
	freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	fifosize--;
#endif

	/* check if pixel was dequeued in multiqueuein-mode*/
	if (regs.matrix[12].m==MARKER_DEQUEUED) {
	  /* set fifo to next element*/
	  fifopos=*dequeuepointer;
#ifdef EVAL
	  eval_LogNCResult(0,0);
#endif
	  continue;
	}

	/*set marker*/
	regs.matrix[12].m=MARKER_DEQUEUED;

	/* pixel operation*/
	if (operation) {
#ifdef PPTRACE
	  PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
			      regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
	  operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	  PPTrace_LogFifoOPOut(regs.result);
#endif

	  if (processmode == QOUT_PROC) {
	    /* store results*/
	    if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	    if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	    if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	    if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	    if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	    /* store recursiv results to matrix*/
	    /* regs.matrix[12].ax=regs.result.ax;*/
	  }

	}

	/*write marker*/
	regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	eval_NCResult = 1;
	eval_NCCrit = 0;
#endif
	/* queue in neighbors*/
	dir=startdir;
	while (dir < 8) {

	  /* check neighbor */
	  pos=dirtopos[dir];

	  if (regs.matrix[pos].m &&
	      (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	    if (eval_NCResult==1)
	      eval_NCResult = 2;
#endif
      
#ifdef PPTRACE
	    PPTrace_LogFifoNCIn(regs.matrix);
#endif
	    valid=(short)neighcrit(regs.matrix,(int)pos,
				   regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	    PPTrace_LogFifoNCOut(pos, valid);
#endif
          
	    if (valid) {
#ifdef EVAL
	      eval_NCResult = 3;
	      eval_NCCrit |= (1<<dir);;
#endif
        
	      if (valid > HIERLEVELS) {
		fprintf(stderr,"WARNING : hierachical fifo: "
			"valid level must be between 1 and %d\n",
			HIERLEVELS);
		valid=HIERLEVELS;
	      }

	      /* queue in*/

	      /* get free entry*/
	      if (freelistinc) { 
		/* emty fifo queue not completely initialised*/
		fifopos=freelistpos;
		freelistpos++;
		fifopos=freelistpos;
		if (freelistpos==(hfifobuffer+FIFOSIZE)) {
		  freelistinc=0;
		}
	      }
	      else {
		/* check out from emtpy list*/
		fifopos=freeliststop;
		if (!fifopos) {
		  addlib_stop_here("Warning: Fifo overflow in "
			  "hierarchical fifo\n",0,0);
		  exit(1);
		}
		freeliststop=freeliststop->next;
	      }

	      /* check in to hierachical queue*/
	      valid--; /* allow hfifo0; old value for 
			  valid=0 => invalid*/

	      /* to current fifo*/
	      if (valid<=_fifo_level) {
#ifdef EVAL
		eval_LogNCLevel(_fifo_level);
#endif
		if (fifofirst) { /* fifo not empty*/
		  fifolast->next=fifopos;
		}
		else { /*fifo empty*/
		  fifofirst=fifopos;
		}
		fifolast=fifopos;
		fifolast->next=0;
	      }

	      /*to queue with lower priority*/
	      else {
#ifdef EVAL
		eval_LogNCLevel(valid);
#endif
		fifopos->next=stophlifo[valid];
		stophlifo[valid]=fifopos;
	      }

#ifdef CNTFIFOSIZE
	      fifosize++;
	      if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	      /* new fifo values*/
	      fifopos->x=regs._n_cntx+dirtoxoff[dir];
	      fifopos->y=regs._n_cnty+dirtoyoff[dir];

	      /* set marker*/
	      regs.result.m=MARKER_QUEUED;
	      if (processmode == QOUT_PROC) {
		MUpdate(&regs,pos);
	      }
	      else {
		UpdatePos(&regs,pos);
	      }
	    }
	  }
	  /* next direction*/
	  dir += dirinc;
	}
#ifdef EVAL
	eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	/* set fifo to next element*/
	fifopos=*dequeuepointer;

      }
    }

    break;

  case SCAN_HM:
    /* noncollected horizontal meander scan*/

#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;
    _fifo_level=startlevel;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
  
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
#ifdef DEBUG
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
#endif
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    fifopos=freeliststop;
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /* save current position*/
	  stx=regs._n_cntx;
	  sty=regs._n_cnty;

	  stia.u=regs._n_ia.u;
	  stiy.u=regs._n_iy.u;
	  stiu.u=regs._n_iu.u;
	  stiv.u=regs._n_iv.u;
	  stax.u=regs._n_ax.u;
	  stm.u=regs._n_m.u;
	  stra.u=regs._n_ra.u;
	  stry.u=regs._n_ry.u;
	  stru.u=regs._n_ru.u;
	  strv.u=regs._n_rv.u;

	  /*do recursiv adressing*/
	  for (; _fifo_level <= stoplevel;
	       _fifo_level ++) {

	    /* get value from lifo*/
	    if ((fifopos=stophlifo[_fifo_level])) {
	      dequeuepointer=&stophlifo[_fifo_level];
	    }
	    else {

	      /*if lifo empty, try fifo*/
	      if ((fifopos=fifofirst)) {
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
	      }
	      else {
#ifdef EVAL
		eval_LogSkipLevel(_fifo_level);
#endif
		continue;
	      }
	    }
      
#ifdef EVAL
	    {
	      THFifoEntry *pTmp;
	      int ElemInHrch;

	      ElemInHrch=0;
	      for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
		ElemInHrch++;

	      eval_LogStartLevel(_fifo_level,ElemInHrch);
	    }
#endif
      
	    /*initialize distance counter*/
	    lastindistlevel=_fifo_distlevel=0;

	    while (fifopos) {  /*while fifo nor empty*/

	      /*compute distlevel for hierarchy*/
	      if (!stophlifo[_fifo_level]) {
		if (lastindistlevel) {
		  _fifo_distlevel++;
		  lasthfifoindistlevel=fifolast;
		  lastindistlevel=0;
#ifdef EVAL
		  eval_LogDist();
#endif
		}
		if (fifopos == lasthfifoindistlevel) {
		  lastindistlevel=1;
		}
	      }
        
#ifdef EVAL
	      eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	      /* goto current position*/
	      pre(&regs,fifopos->x,fifopos->y);

	      /* dequeue from queue(level)*/
	      *dequeuepointer=fifopos->next;

	      if (!(*dequeuepointer)) {
		/*switch from hierarchical lifo to fifo*/
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
		_fifo_distlevel=0;
	      }		

	      /* check in to emtpy list*/
	      fifopos->next=freeliststop;
	      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	      fifosize--;
#endif

	      /* check if pixel was dequeued in multiqueuein-mode*/
	      if (regs.matrix[12].m==MARKER_DEQUEUED) {
		/* set fifo to next element*/
		fifopos=*dequeuepointer;
#ifdef EVAL
		eval_LogNCResult(0,0);
#endif
		continue;
	      }

	      /*set marker*/
	      regs.matrix[12].m=MARKER_DEQUEUED;

	      /* pixel operation*/
	      if (operation) {
#ifdef PPTRACE
		PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
				    regs._n_cnty, _fifo_level,
				    _fifo_distlevel);
#endif
		operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		PPTrace_LogFifoOPOut(regs.result);
#endif

		if (processmode == QOUT_PROC) {
		  /* store results*/
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		  /* store recursiv results to matrix*/
		  /* regs.matrix[12].ax=regs.result.ax;*/
		}

	      }

	      /*write marker*/
	      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	      eval_NCResult = 1;
	      eval_NCCrit = 0;
#endif
	      /* queue in neighbors*/
	      dir=startdir;
	      while (dir < 8) {

		/* check neighbor */
		pos=dirtopos[dir];

		if (regs.matrix[pos].m &&
		    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		  if (eval_NCResult==1)
		    eval_NCResult = 2;
#endif

#ifdef PPTRACE
		  PPTrace_LogFifoNCIn(regs.matrix);
#endif
		  valid=(short)neighcrit(regs.matrix,(int)pos,
					 regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		  PPTrace_LogFifoNCOut(pos, valid);
#endif

		  if (valid) {
#ifdef EVAL
		    eval_NCResult = 3;
		    eval_NCCrit |= (1<<dir);
#endif

		    if (valid > HIERLEVELS) {
		      fprintf(stderr,"WARNING : hierachical fifo: "
			      "valid level must be between 1 and %d\n",
			      HIERLEVELS);
		      valid=HIERLEVELS;
		    }

		    /* queue in*/

		    /* get free entry*/
		    if (freelistinc) { 
		      /* emty fifo queue not completely initialised*/
		      fifopos=freelistpos;
		      freelistpos++;
		      fifopos=freelistpos;
		      if (freelistpos==(hfifobuffer+FIFOSIZE)) {
			freelistinc=0;
		      }
		    }
		    else {
		      /* check out from emtpy list*/
		      fifopos=freeliststop;
		      if (!fifopos) {
			addlib_stop_here("Warning: Fifo overflow in "
				"hierarchical fifo\n",0,0);
			exit(1);
		      }
		      freeliststop=freeliststop->next;
		    }
		    
		    /* check in to hierachical queue*/
		    valid--; /* allow hfifo0; old value for 
				valid=0 => invalid*/

		    /* to current fifo*/
		    if (valid<=_fifo_level) {
#ifdef EVAL
		      eval_LogNCLevel(_fifo_level);
#endif
		      if (fifofirst) { /* fifo not empty*/
			fifolast->next=fifopos;
		      }
		      else { /*fifo empty*/
			fifofirst=fifopos;
		      }
		      fifolast=fifopos;
		      fifolast->next=0;
		    }

		    /*to queue with lower priority*/
		    else {
#ifdef EVAL
		      eval_LogNCLevel(valid);
#endif
		      fifopos->next=stophlifo[valid];
		      stophlifo[valid]=fifopos;
		    }

#ifdef CNTFIFOSIZE
		    fifosize++;
		    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

		    /* new fifo values*/
		    fifopos->x=regs._n_cntx+dirtoxoff[dir];
		    fifopos->y=regs._n_cnty+dirtoyoff[dir];

		    /* set marker*/
		    regs.result.m=MARKER_QUEUED;
		    if (processmode == QOUT_PROC) {
		      MUpdate(&regs,pos);
		    }
		    else {
		      UpdatePos(&regs,pos);
		    }
		  }
		}
		/* next direction*/
		dir += dirinc;
	      }
#ifdef EVAL
	      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	      /* set fifo to next element*/
	      fifopos=*dequeuepointer;

	    }
	  }

	  /* reset startlevel for next  startpixel*/
	  _fifo_level=startlevel;

	  /*goto prev position*/
	  regs._n_cntx=regs._n_icntx=stx;
	  regs._n_cnty=regs._n_icnty=sty;
	     
	  regs._n_ia.u=stia.u;
	  regs._n_iy.u=stiy.u;
	  regs._n_iu.u=stiu.u;
	  regs._n_iv.u=stiv.u;
	  regs._n_ax.u=stax.u;
	  regs._n_m.u =stm.u;
	  regs._n_ra.u=stra.u;
	  regs._n_ry.u=stry.u;
	  regs._n_ru.u=stru.u;
	  regs._n_rv.u=strv.u;

	  /* load matrix with previous values*/
	  Preload(&regs);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel*/
      if (regs._n_cntx==lineend) {
	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
	ShiftDownLineLoad(&regs);

	if (defaultdir==ShiftRightLineLoad) {
	  lineend=0;
	  defaultdir=ShiftLeftLineLoad;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=ShiftRightLineLoad;
	}
      }
      else {
	defaultdir(&regs);
      }
    }
    break;
  case SCAN_HM|SCAN_COLLECT:
    /* noncollected horizontal meander scan*/
#ifdef EVAL
    eval_LogPreload();
#endif

    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;
    _fifo_level=startlevel;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    fifopos=freeliststop;
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical queue*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/

	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel*/
      if (regs._n_cntx==lineend) {
	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
	ShiftDownLineLoad(&regs);

	if (defaultdir==ShiftRightLineLoad) {
	  lineend=0;
	  defaultdir=ShiftLeftLineLoad;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=ShiftRightLineLoad;
	}
      }
      else {
	defaultdir(&regs);
      }
    }

    /*do recursiv adressing*/
    for (; _fifo_level <= stoplevel; _fifo_level ++) {

      /* get value from lifo*/
      if ((fifopos=stophlifo[_fifo_level])) {
	dequeuepointer=&stophlifo[_fifo_level];
      }
      else {

	/*if lifo empty, try fifo*/
	if ((fifopos=fifofirst)) {
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	}
	else {
#ifdef EVAL
          eval_LogSkipLevel(_fifo_level);
#endif
          continue;
	}
      }

#ifdef EVAL
      {
        THFifoEntry *pTmp;
        int ElemInHrch;
        
        ElemInHrch=0;
        for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
          ElemInHrch++;
        
        eval_LogStartLevel(_fifo_level,ElemInHrch);
      }
#endif
      
      /*initialize distance counter*/
      lastindistlevel=_fifo_distlevel=0;

      while (fifopos) {  /*while fifo nor empty*/
	
	/*compute distlevel for hierarchy*/
	if (!stophlifo[_fifo_level]) {
	  if (lastindistlevel) {
	    _fifo_distlevel++;
	    lasthfifoindistlevel=fifolast;
	    lastindistlevel=0;
#ifdef EVAL
	    eval_LogDist();
#endif
	  }
	  if (fifopos == lasthfifoindistlevel) {
	    lastindistlevel=1;
	  }
	}

#ifdef EVAL
	eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	/* goto current position*/
	pre(&regs,fifopos->x,fifopos->y);

	/* dequeue from queue(level)*/
	*dequeuepointer=fifopos->next;

	if (!(*dequeuepointer)) {
	  /*switch from hierarchical lifo to fifo*/
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	  _fifo_distlevel=0;
	}		

	/* check in to emtpy list*/
	fifopos->next=freeliststop;
	freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	fifosize--;
#endif

	/* check if pixel was dequeued in multiqueuein-mode*/
	if (regs.matrix[12].m==MARKER_DEQUEUED) {
	  /* set fifo to next element*/
	  fifopos=*dequeuepointer;
#ifdef EVAL
	  eval_LogNCResult(0,0);
#endif
	  continue;
	}

	/*set marker*/
	regs.matrix[12].m=MARKER_DEQUEUED;

	/* pixel operation*/
	if (operation) {
#ifdef PPTRACE
	  PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
			      regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
	  operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	  PPTrace_LogFifoOPOut(regs.result);
#endif

	  if (processmode == QOUT_PROC) {
	    /* store results*/
	    if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	    if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	    if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	    if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	    if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	    /* store recursiv results to matrix*/
	    /* regs.matrix[12].ax=regs.result.ax;*/
	  }

	}

	/*write marker*/
	regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	eval_NCResult = 1;
	eval_NCCrit = 0;
#endif
	/* queue in neighbors*/
	dir=startdir;
	while (dir < 8) {

	  /* check neighbor */
	  pos=dirtopos[dir];

	  if (regs.matrix[pos].m &&
	      (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	    if (eval_NCResult==1)
	      eval_NCResult = 2;
#endif
      
#ifdef PPTRACE
	    PPTrace_LogFifoNCIn(regs.matrix);
#endif
	    valid=(short)neighcrit(regs.matrix,(int)pos,
				   regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	    PPTrace_LogFifoNCOut(pos, valid);
#endif
          
	    if (valid) {
#ifdef EVAL
	      eval_NCResult = 3;
	      eval_NCCrit |= (1<<dir);;
#endif
        
	      if (valid > HIERLEVELS) {
		fprintf(stderr,"WARNING : hierachical fifo: "
			"valid level must be between 1 and %d\n",
			HIERLEVELS);
		valid=HIERLEVELS;
	      }

	      /* queue in*/

	      /* get free entry*/
	      if (freelistinc) { 
		/* emty fifo queue not completely initialised*/
		fifopos=freelistpos;
		freelistpos++;
		fifopos=freelistpos;
		if (freelistpos==(hfifobuffer+FIFOSIZE)) {
		  freelistinc=0;
		}
	      }
	      else {
		/* check out from emtpy list*/
		fifopos=freeliststop;
		if (!fifopos) {
		  addlib_stop_here("Warning: Fifo overflow in "
			  "hierarchical fifo\n",0,0);
		  exit(1);
		}
		freeliststop=freeliststop->next;
	      }

	      /* check in to hierachical queue*/
	      valid--; /* allow hfifo0; old value for 
			  valid=0 => invalid*/

	      /* to current fifo*/
	      if (valid<=_fifo_level) {
#ifdef EVAL
		eval_LogNCLevel(_fifo_level);
#endif
		if (fifofirst) { /* fifo not empty*/
		  fifolast->next=fifopos;
		}
		else { /*fifo empty*/
		  fifofirst=fifopos;
		}
		fifolast=fifopos;
		fifolast->next=0;
	      }

	      /*to queue with lower priority*/
	      else {
#ifdef EVAL
		eval_LogNCLevel(valid);
#endif
		fifopos->next=stophlifo[valid];
		stophlifo[valid]=fifopos;
	      }

#ifdef CNTFIFOSIZE
	      fifosize++;
	      if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	      /* new fifo values*/
	      fifopos->x=regs._n_cntx+dirtoxoff[dir];
	      fifopos->y=regs._n_cnty+dirtoyoff[dir];

	      /* set marker*/
	      regs.result.m=MARKER_QUEUED;
	      if (processmode == QOUT_PROC) {
		MUpdate(&regs,pos);
	      }
	      else {
		UpdatePos(&regs,pos);
	      }
	    }
	  }
	  /* next direction*/
	  dir += dirinc;
	}
#ifdef EVAL
	eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	/* set fifo to next element*/
	fifopos=*dequeuepointer;

      }
    }

    break;

  case SCAN_LIST:

/*    for (pos=0; pos<25;regs.matrix[(int)pos++].m=0);*/
    _fifo_level=startlevel;

    while (startliststop) {
      
#ifdef EVAL
      eval_LogPreload();
      eval_LogSCPos(startliststop->x,startliststop->y);
#endif

      /* load pixel from list*/
      pre(&regs,startliststop->x,startliststop->y);

      /* goto next startpixel*/
      fifopos=startliststop;
      startliststop=startliststop->next;

      /* check in to emtpy list*/
      fifopos->next=freeliststop;
      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
      fifosize--;
#endif

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    /*fifopos=freeliststop; already set from list!!*/
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /*do recursiv adressing*/
	  for (; _fifo_level <= stoplevel;
	       _fifo_level ++) {

	    /* get value from lifo*/
	    if ((fifopos=stophlifo[_fifo_level])) {
	      dequeuepointer=&stophlifo[_fifo_level];
	    }
	    else {

	      /*if lifo empty, try fifo*/
	      if ((fifopos=fifofirst)) {
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
	      }
	      else {
#ifdef EVAL
		eval_LogSkipLevel(_fifo_level);
#endif
		continue;
	      }
	    }

#ifdef EVAL
	    {
	      THFifoEntry *pTmp;
	      int ElemInHrch;
        
	      ElemInHrch=0;
	      for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
		ElemInHrch++;
        
	      eval_LogStartLevel(_fifo_level,ElemInHrch);
	    }
#endif
	    /*initialize distance counter*/
	    lastindistlevel=_fifo_distlevel=0;

	    while (fifopos) {  /*while fifo nor empty*/

	      /*compute distlevel for hierarchy*/
	      if (!stophlifo[_fifo_level]) {
		if (lastindistlevel) {
		  _fifo_distlevel++;
		  lasthfifoindistlevel=fifolast;
		  lastindistlevel=0;
#ifdef EVAL
		  eval_LogDist();
#endif
		}
		if (fifopos == lasthfifoindistlevel) {
		  lastindistlevel=1;
		}
	      }

#ifdef EVAL
        eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	      /* goto current position*/
	      pre(&regs,fifopos->x,fifopos->y);

	      /* dequeue from queue(level)*/
	      *dequeuepointer=fifopos->next;

	      if (!(*dequeuepointer)) {
		/*switch from hierarchical lifo to fifo*/
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
		_fifo_distlevel=0;
	      }		

	      /* check in to emtpy list*/
	      fifopos->next=freeliststop;
	      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	      fifosize--;
#endif

	      /* check if pixel was dequeued in multiqueuein-mode*/
	      if (regs.matrix[12].m==MARKER_DEQUEUED) {
		/* set fifo to next element*/
		fifopos=*dequeuepointer;
#ifdef EVAL
		eval_LogNCResult(0,0);
#endif
		continue;
	      }

	      /*set marker*/
	      regs.matrix[12].m=MARKER_DEQUEUED;

	      /* pixel operation*/
	      if (operation) {
#ifdef PPTRACE
		PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
				    regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
		operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		PPTrace_LogFifoOPOut(regs.result);
#endif

		if (processmode == QOUT_PROC) {
		  /* store results*/
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		  /* store recursiv results to matrix*/
		  /* regs.matrix[12].ax=regs.result.ax;*/
		}

	      }

	      /*write marker*/
	      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	      eval_NCResult = 1;
	      eval_NCCrit = 0;
#endif
	      /* queue in neighbors*/
	      dir=startdir;
	      while (dir < 8) {

		/* check neighbor */
		pos=dirtopos[dir];

		if (regs.matrix[pos].m &&
		    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		  if (eval_NCResult==1)
		    eval_NCResult = 2;
#endif
      
#ifdef PPTRACE
		  PPTrace_LogFifoNCIn(regs.matrix);
#endif
		  valid=(short)neighcrit(regs.matrix,(int)pos,
					 regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		  PPTrace_LogFifoNCOut(pos, valid);
#endif
          
		  if (valid) {
#ifdef EVAL
		    eval_NCResult = 3;
		    eval_NCCrit |= (1<<dir);
#endif
        
		    if (valid > HIERLEVELS) {
		      fprintf(stderr,"WARNING : hierachical fifo: "
			      "valid level must be between 1 and %d\n",
			      HIERLEVELS);
		      valid=HIERLEVELS;
		    }

		    /* queue in*/

		    /* get free entry*/
		    if (freelistinc) { 
		      /* emty fifo queue not completely initialised*/
		      fifopos=freelistpos;
		      freelistpos++;
		      fifopos=freelistpos;
		      if (freelistpos==(hfifobuffer+FIFOSIZE)) {
			freelistinc=0;
		      }
		    }
		    else {
		      /* check out from emtpy list*/
		      fifopos=freeliststop;
		      if (!fifopos) {
			addlib_stop_here("Warning: Fifo overflow in "
				"hierarchical fifo\n",0,0);
			exit(1);
		      }
		      freeliststop=freeliststop->next;
		    }

		    /* check in to hierachical queue*/
		    valid--; /* allow hfifo0; old value for 
				valid=0 => invalid*/

		    /* to current fifo*/
		    if (valid<=_fifo_level) {
#ifdef EVAL
		      eval_LogNCLevel(_fifo_level);
#endif
		      if (fifofirst) { /* fifo not empty*/
			fifolast->next=fifopos;
		      }
		      else { /*fifo empty*/
			fifofirst=fifopos;
		      }
		      fifolast=fifopos;
		      fifolast->next=0;
		    }

		    /*to queue with lower priority*/
		    else {
#ifdef EVAL
		      eval_LogNCLevel(valid);
#endif
		      fifopos->next=stophlifo[valid];
		      stophlifo[valid]=fifopos;
		    }

#ifdef CNTFIFOSIZE
		    fifosize++;
		    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

		    /* new fifo values*/
		    fifopos->x=regs._n_cntx+dirtoxoff[dir];
		    fifopos->y=regs._n_cnty+dirtoyoff[dir];

		    /* set marker*/
		    regs.result.m=MARKER_QUEUED;
		    if (processmode == QOUT_PROC) {
		      MUpdate(&regs,pos);
		    }
		    else {
		      UpdatePos(&regs,pos);
		    }
		  }
		}
		/* next direction*/
		dir += dirinc;
	      }
#ifdef EVAL
	      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
	    }

	    /* set fifo to next element*/
	    fifopos=*dequeuepointer;

	  }

	  /* reset startlevel for next  startpixel*/
	  _fifo_level=startlevel;
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
    }

    break;

  case SCAN_LIST|SCAN_COLLECT:

/*    for (pos=0; pos<25;regs.matrix[pos++].m=0);*/
    _fifo_level=startlevel;

    while (startliststop) {

#ifdef EVAL
      eval_LogPreload();
      eval_LogSCPos(startliststop->x,startliststop->y);
#endif
      /* load pixel from list*/
      pre(&regs,startliststop->x,startliststop->y);

      /* goto next startpixel*/
      fifopos=startliststop;
      startliststop=startliststop->next;

      /* check in to emtpy list*/
      fifopos->next=freeliststop;
      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
      fifosize--;
#endif

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogFifoSCIn(regs.matrix, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif
        
	if (valid) {

	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    /*fifopos=freeliststop; already set from list!!*/
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
    }

    /*do recursiv adressing*/
    for (_fifo_level=startlevel; _fifo_level <= stoplevel; _fifo_level ++) {

      /* get value from lifo*/
      if ((fifopos=stophlifo[_fifo_level])) {
	dequeuepointer=&stophlifo[_fifo_level];
      }
      else {

	/*if lifo empty, try fifo*/
	if ((fifopos=fifofirst)) {
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	}
	else {
#ifdef EVAL
	  eval_LogSkipLevel(_fifo_level);
#endif
	  continue;
	}
      }

#ifdef EVAL
      {
        THFifoEntry *pTmp;
        int ElemInHrch;
        
        ElemInHrch=0;
        for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
          ElemInHrch++;
        
        eval_LogStartLevel(_fifo_level,ElemInHrch);
      }
#endif
      
      /*initialize distance counter*/
      lastindistlevel=_fifo_distlevel=0;

      while (fifopos) {  /*while fifo nor empty*/

	/*compute distlevel for hierarchy*/
	if (!stophlifo[_fifo_level]) {
	  if (lastindistlevel) {
	    _fifo_distlevel++;
	    lasthfifoindistlevel=fifolast;
	    lastindistlevel=0;
#ifdef EVAL
	    eval_LogDist();
#endif
	  }
	  if (fifopos == lasthfifoindistlevel) {
	    lastindistlevel=1;
	  }
	}

#ifdef EVAL
	eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	/* goto current position*/
	pre(&regs,fifopos->x,fifopos->y);

	/* dequeue from queue(level)*/
	*dequeuepointer=fifopos->next;

	if (!(*dequeuepointer)) {
	  /*switch from hierarchical lifo to fifo*/
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	  _fifo_distlevel=0;
	}		

	/* check in to emtpy list*/
	fifopos->next=freeliststop;
	freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	fifosize--;
#endif

	/* check if pixel was dequeued in multiqueuein-mode*/
	if (regs.matrix[12].m==MARKER_DEQUEUED) {
	  /* set fifo to next element*/
	  fifopos=*dequeuepointer;
#ifdef EVAL
	  eval_LogNCResult(0,0);
#endif
	  continue;
	}

	/*set marker*/
	regs.matrix[12].m=MARKER_DEQUEUED;

	/* pixel operation*/
	if (operation) {
#ifdef PPTRACE
	  PPTrace_LogFifoOPIn(regs.matrix, regs._n_cntx,
			      regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
	  operation(&regs.result,regs.matrix,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	  PPTrace_LogFifoOPOut(regs.result);
#endif

	  if (processmode == QOUT_PROC) {
	    /* store results*/
	    if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	    if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	    if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	    if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	    if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	    /* store recursiv results to matrix*/
	    /* regs.matrix[12].ax=regs.result.ax;*/
	  }

	}

	/*write marker*/
	regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	eval_NCResult = 1;
	eval_NCCrit = 0;
#endif
	/* queue in neighbors*/
	dir=startdir;
	while (dir < 8) {

	  /* check neighbor */
	  pos=dirtopos[dir];

	  if (regs.matrix[pos].m &&
	      (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	    if (eval_NCResult==1)
	      eval_NCResult = 2;
#endif

#ifdef PPTRACE
	    PPTrace_LogFifoNCIn(regs.matrix);
#endif
	    valid=(short)neighcrit(regs.matrix,(int)pos,
				   regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
	    PPTrace_LogFifoNCOut(pos, valid);
#endif
          
	    if (valid) {
#ifdef EVAL
	      eval_NCResult = 3;
	      eval_NCCrit |= (1<<dir);;
#endif

	      if (valid > HIERLEVELS) {
		fprintf(stderr,"WARNING : hierachical fifo: "
			"valid level must be between 1 and %d\n",
			HIERLEVELS);
		valid=HIERLEVELS;
	      }

	      /* queue in*/

	      /* get free entry*/
	      if (freelistinc) { 
		/* emty fifo queue not completely initialised*/
		fifopos=freelistpos;
		freelistpos++;
		fifopos=freelistpos;
		if (freelistpos==(hfifobuffer+FIFOSIZE)) {
		  freelistinc=0;
		}
	      }
	      else {
		/* check out from emtpy list*/
		fifopos=freeliststop;
		if (!fifopos) {
		  addlib_stop_here("Warning: Fifo overflow in "
			  "hierarchical fifo\n",0,0);
		  exit(1);
		}
		freeliststop=freeliststop->next;
	      }

	      /* check in to hierachical queue*/
	      valid--; /* allow hfifo0; old value for 
			  valid=0 => invalid*/

	      /* to current fifo*/
	      if (valid<=_fifo_level) {
#ifdef EVAL
		eval_LogNCLevel(_fifo_level);
#endif
		if (fifofirst) { /* fifo not empty*/
		  fifolast->next=fifopos;
		}
		else { /*fifo empty*/
		  fifofirst=fifopos;
		}
		fifolast=fifopos;
		fifolast->next=0;
	      }

	      /*to queue with lower priority*/
	      else {
#ifdef EVAL
		eval_LogNCLevel(valid);
#endif
		fifopos->next=stophlifo[valid];
		stophlifo[valid]=fifopos;
	      }

	      /* new fifo values*/
	      fifopos->x=regs._n_cntx+dirtoxoff[dir];
	      fifopos->y=regs._n_cnty+dirtoyoff[dir];

	      /* set marker*/
	      regs.result.m=MARKER_QUEUED;
	      if (processmode == QOUT_PROC) {
		MUpdate(&regs,pos);
	      }
	      else {
		UpdatePos(&regs,pos);
	      }
	    }
	  }
	  /* next direction*/
	  dir += dirinc;
	}
#ifdef EVAL
	eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	/* set fifo to next element*/
	fifopos=*dequeuepointer;

      }
    }

    break;
  default:
    fprintf(stderr,"Warning: Unsupported scan mode in fifo addressing\n");
    DestructNhood(&regs);
    /*free(hfifobuffer);  fifo is static!*/
    return;
  }
  DestructNhood(&regs);
  /* free(hfifobuffer);  fifo is static!*/

#ifdef PPTRACE
  PPTrace_LogFifoReg(0);
#endif

#ifdef EVAL
  eval_EndMeasureDur();
  eval_EndAddr();
  eval_EndNCrit();
  eval_EndSCrit();
  eval_EndOpr();
  eval_EndProc();
  eval_EndConSC();
  eval_EndCon();
  eval_EndScan();
  eval_EndRecMode();
  eval_EndAddrMode();
#endif

#ifdef CNTFIFOSIZE
  if (maxcallfifosize > maxfifosize) maxfifosize=maxcallfifosize;
#endif

  return;
}

/******* interhfifo_proc *************************************************/
/* central function for connected area related functions (hfifo approach)
 with inter frame combinations*/
void interhfifo_proc(MomVop *resvop, MomVop *in1, MomVop *in2,
		     MomImage *aux,MomImage *marker,
		     int reschannels,
		     int in1channels,int in2channels,
		     int connect,int scanmode,int processmode,
		     short startlevel,short stoplevel,short initfifo,
		     TChan ahold,TChan yhold,TChan uhold,
		     TChan vhold,TChan axhold,
		     int (*startcrit)(TPixel *in1,TPixel *in2,TCoor x,TCoor y),
		     int (*neighcrit)(TPixel *in1,TPixel *in2,
				      int pos,TCoor x,TCoor y),
		     void (*operation)(TPixel *res, TPixel *in1,TPixel *in2,
				       TCoor x,TCoor y))

{
  THFifoEntry *fifopos;
  THFifoEntry *lasthfifoindistlevel=0;
  THFifoEntry **dequeuepointer;
  short lastindistlevel;
  short deniqueuein;
  TNhood regs;
  short valid;
  short startdir,dirinc,dir;
  TCoor lineend;
  TCoor stx,sty;                   /* to store position of pixel scan*/
  MomImageData stia,stiy,stiu,stiv,stax,stm,stra,stry,stru,strv;
  void (*defaultdir)(TNhood *);
  void (*pre)(TNhood *nhood,TCoor dx,TCoor dy);
  uchar pos;
  TCoor _i_in2width=0,_i_in2height=0; /* input2 image size*/
  MomImageData  _i_i2a,_i_i2y,_i_i2u,_i_i2v;
  MomImageData fsti2a,fsti2y,fsti2u,fsti2v;  /* points to frame start*/
  MomImageData sti2a,sti2y,sti2u,sti2v;      /* points to stack*/
  int _i_i2as,_i_i2ys,_i_i2us,_i_i2vs;
  int _i_i2als,_i_i2yls,_i_i2uls,_i_i2vls;
  short _i_i2ash,_i_i2ysh,_i_i2ush,_i_i2vsh; /* shift value for offset*/
  long i2offset;
  int _i_in2channels;
  MomImage *in2_ch;
  TChan	(*_i_i2aget)(MomImageData data);
  TChan	(*_i_i2yget)(MomImageData data);
  TChan	(*_i_i2uget)(MomImageData data);
  TChan	(*_i_i2vget)(MomImageData data);
  TPixel _f_in2;
  short i;

#ifdef EVAL
  eval_BeginMeasureDur();
  eval_BeginAddrMode(SECTION_HFIFO_INTER);
  eval_BeginRecMode(in1,resvop,in1channels,reschannels);
  eval_PictSize(in1,in2,resvop,aux);
  eval_BeginScan(scanmode);
  eval_ScanWindow(-1,-1,0,0,0,0,0,0);
  eval_BeginCon(connect);
  eval_BeginConSC(connect);
  eval_BeginProc(processmode);
  eval_BeginOpr((void*)operation);
  eval_BeginSCrit((void*)startcrit);
  eval_BeginNCrit((void*)neighcrit);
  eval_StartAddr();
#endif

#ifdef PPTRACE
  PPTrace_StartOfTool();
  PPTrace_LogType((void*)operation,reschannels|M_CH,connect);
  PPTrace_LogFifoReg(1);
#endif

  /* check input parameters*/
  ConstructNhood(&regs,resvop,in1,aux,marker,1,
	      reschannels,in1channels,connect,scanmode,
	      ahold,yhold,uhold,vhold,axhold,
	      -1,-1,0,0,0,0,size1to1);
  if (regs.error) {
    DestructNhood(&regs);
    return;
  }

  /* check parameters for initer channel*/
  _i_in2width=_i_in2height=0;
  _i_i2a.u=_i_i2y.u=_i_i2u.u=_i_i2v.u=0;
  _i_i2as=_i_i2ys=_i_i2us=_i_i2vs=0;
  _i_i2als=_i_i2yls=_i_i2uls=_i_i2vls=0;
  _i_i2ash=_i_i2ysh=_i_i2ush=_i_i2vsh=0;
  _i_in2channels=in2channels;
  _i_i2aget= &getunused;
  _i_i2yget= &getunused;
  _i_i2uget= &getunused;
  _i_i2vget= &getunused;

  i=1;
  while (i<16) {
    if (in1) {
      if (_i_in2channels & i) {
  	switch (i) {
  	case A_CH:
  	  in2_ch=in2->a_chan;
  	  break;
  	case Y_CH:
  	  in2_ch=in2->y_chan;
  	  break;
  	case U_CH:
  	  in2_ch=in2->u_chan;
  	  break;
  	case V_CH:
  	  in2_ch=in2->v_chan;
  	  break;
  	default:
  	  fprintf(stderr,"ERROR : Internal error in inter_proc\n");
  	  return;
  	}
  
  	if (!in2_ch) {
  	  fprintf(stderr,"WARNING : color component does not exist in "
  		  "inter_proc\n");
  	  _i_in2channels-=i;
  	}
  	else if (!(in2_ch->x && in2_ch->y)) {
  	  fprintf(stderr,"WARNING : color component has 0-size in "
  		  "inter_proc\n");
  	  _i_in2channels-=i;
  	}
  	else {
  	  switch (i) {
  	  case A_CH:
  	    _i_i2as=datasize(in2_ch);
  	    _i_i2als=_i_i2as*in2_ch->x;
  	    _i_i2ash=_typesize[_i_i2as];
  	    fsti2a.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2aget);
  	    break;
  	  case Y_CH:
  	    _i_i2ys=datasize(in2_ch);
  	    _i_i2yls=_i_i2ys*in2_ch->x;
  	    _i_i2ysh=_typesize[_i_i2ys];
  	    fsti2y.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2yget);
  	    break;
  	  case U_CH:
  	    _i_i2us=datasize(in2_ch);
  	    _i_i2uls=_i_i2us*in2_ch->x;
  	    _i_i2ush=_typesize[_i_i2us];
  	    fsti2u.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2uget);
  	    break;
  	  case V_CH:
  	    _i_i2vs=datasize(in2_ch);
  	    _i_i2vls=_i_i2vs*in2_ch->x;
  	    _i_i2vsh=_typesize[_i_i2vs];
  	    fsti2v.u=in2_ch->data->u;
  	    iref(in2_ch->type,&_i_i2vget);
  	    break;
  	  }
  	  if (in2_ch) {
  	    if (!_i_in2width) _i_in2width=in2_ch->x;
  	    else {
  	      if (_i_in2width != in2_ch->x) {
  		fprintf(stderr,"Warning can only process colorformat 444\n");
  		return;
  	      }
  	    }
  	    if (!_i_in2height) _i_in2height=in2_ch->y;
  	    else {
  	      if (_i_in2height != in2_ch->y) {
  		fprintf(stderr,"Warning can only process colorformat 444\n");
  		return;
  	      }
  	    }
  	  }
  	}
      }
    }
    else {
      _i_in2channels=0;
      break;
    }
  }

  if (_i_in2channels) {
    if ((_i_in2width != regs.width) || (_i_in2height != regs.height)) {
      fprintf(stderr,"WARNING : imagessizes does not match in "
	      "interhfido_proc\n");
      return;
    }
  }


  /* set direction incrementer for neighborhood*/
  if (connect==CON_4) {
    startdir=0;
    dirinc=2;
    pre=&Preload4;  
  }
  else if (connect < CON_H2) {
    startdir=0;
    dirinc=1;
    pre=&Preload8;
  }
  else {
    if (connect < CON_V2) {
      startdir=0;
    }
    else {
      startdir=2;
    }
    dirinc=4;
    pre=&Preload4;  
  }

  /* check level parameters*/
  if (startlevel> stoplevel) {
    _fifo_level=startlevel;
    startlevel=stoplevel;
    stoplevel=_fifo_level;
  }
  
  if (startlevel < 0) {
    startlevel=0;
    fprintf(stderr,"WARNING : hierachical fifo: startlevel must be between "
	    "0 and %d\n",HIERLEVELS-1);
  }
  if (startlevel >= HIERLEVELS) {
    startlevel=HIERLEVELS-1;
    fprintf(stderr,"WARNING : hierachical fifo: startlevel must be between "
	    "0 and %d\n",HIERLEVELS-1);
  }
  if (stoplevel < startlevel) {
    stoplevel=startlevel;
    fprintf(stderr,"WARNING : hierachical fifo: stoplevel must be between "
	    "%d and %d\n",startlevel,HIERLEVELS-1);
  }
  if (stoplevel >= HIERLEVELS) {
    stoplevel=HIERLEVELS-1;
    fprintf(stderr,"WARNING : hierachical fifo: stoplevel must be between "
	    "%d and %d\n",startlevel,HIERLEVELS-1);
  }

  /*init start and stop pointer for fifos*/
#ifdef DEBUG
  if ((regs._n_scanmode & SCAN_LIST) && initfifo) {
    fprintf(stderr,"WARNING : Reset FIFO should not happen with LIST mode\n");
    initfifo=0;
  }
#endif
  if (initfifo) {
    freelistpos=hfifobuffer;
    freeliststop=0;
    startliststop=0;
    memset(stophlifo,0,HIERLEVELS*sizeof(THFifoEntry *));
    freelistinc=1;
#ifdef CNTFIFOSIZE
    fifosize=maxcallfifosize=0;
#endif
  
#ifdef RESETLIST
    fifoposin=hfifobuffer;
    for (i=1; i<FIFOSIZE; i++) {
      fifoposin->next=fifoposin+1;
      fifoposin++;
    }
    fifoposin->next=0;
    stophlifo[0]=fifoposin;
    fifoposout=hfifobuffer;
    starthfifo[0]=fifoposout;
#endif
  }

  /* set comparision value for queue in, > queued or > dequeued*/
  if (regs._n_scanmode & SCAN_MULIN) {
    deniqueuein=MARKER_DEQUEUED;
  }
  else {
    deniqueuein=MARKER_QUEUED;
  }
  /* loop for imagescan*/
  switch (regs._n_scanmode & MULTI_SIGLE_QUEUEIN) {
  case SCAN_HM:
    /* noncollected horizontal meander scan*/

    _i_i2a.u=fsti2a.u;
    _i_i2y.u=fsti2y.u;
    _i_i2u.u=fsti2u.u;
    _i_i2v.u=fsti2v.u;

#ifdef EVAL
    eval_LogPreload();
#endif
    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;
    _fifo_level=startlevel;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
      
      /* load intra channel*/
      if (_i_i2as) {
	_f_in2.a.i=_i_i2aget(_i_i2a);
      }
      if (_i_i2ys) {
	_f_in2.y.i=_i_i2yget(_i_i2y);
      }
      if (_i_i2us) {
	_f_in2.u.i=_i_i2uget(_i_i2u);
      }
      if (_i_i2vs) {
	_f_in2.v.i=_i_i2vget(_i_i2v);
      }

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogIFifoSCIn(regs.matrix, &_f_in2, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,&_f_in2,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
#ifdef DEBUG
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
#endif
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    fifopos=freeliststop;
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /* save current position*/
	  stx=regs._n_cntx;
	  sty=regs._n_cnty;

	  stia.u=regs._n_ia.u;
	  stiy.u=regs._n_iy.u;
	  stiu.u=regs._n_iu.u;
	  stiv.u=regs._n_iv.u;
	  sti2a.u=_i_i2a.u;
	  sti2y.u=_i_i2y.u;
	  sti2u.u=_i_i2u.u;
	  sti2v.u=_i_i2v.u;
	  stax.u=regs._n_ax.u;
	  stm.u=regs._n_m.u;
	  stra.u=regs._n_ra.u;
	  stry.u=regs._n_ry.u;
	  stru.u=regs._n_ru.u;
	  strv.u=regs._n_rv.u;

	  /*do recursiv adressing*/
	  for (; _fifo_level <= stoplevel;
	       _fifo_level ++) {

	    /* get value from lifo*/
	    if ((fifopos=stophlifo[_fifo_level])) {
	      dequeuepointer=&stophlifo[_fifo_level];
	    }
	    else {

	      /*if lifo empty, try fifo*/
	      if ((fifopos=fifofirst)) {
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
	      }
	      else {
#ifdef EVAL
		eval_LogSkipLevel(_fifo_level);
#endif
		continue;
	      }
	    }

#ifdef EVAL
	    {
	      THFifoEntry *pTmp;
	      int ElemInHrch;
        
	      ElemInHrch=0;
	      for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
		ElemInHrch++;
        
	      eval_LogStartLevel(_fifo_level,ElemInHrch);
	    }
#endif
      
      /*initialize distance counter*/
	    lastindistlevel=_fifo_distlevel=0;

	    while (fifopos) {  /*while fifo nor empty*/

	      /*compute distlevel for hierarchy*/
	      if (!stophlifo[_fifo_level]) {
		if (lastindistlevel) {
		  _fifo_distlevel++;
		  lasthfifoindistlevel=fifolast;
		  lastindistlevel=0;
#ifdef EVAL
		  eval_LogDist();
#endif
		}
		if (fifopos == lasthfifoindistlevel) {
		  lastindistlevel=1;
		}
	      }

#ifdef EVAL
        eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	      /* goto current position*/
	      pre(&regs,fifopos->x,fifopos->y);

	      /* load inter channel*/
	      i2offset=fifopos->x+fifopos->y*_i_in2width;
	      if (_i_i2as) {
		_i_i2a.u=fsti2a.u+(i2offset<<_i_i2ash);
		_f_in2.a.i=_i_i2aget(_i_i2a);
	      }
	      if (_i_i2ys) {
		_i_i2y.u=fsti2y.u+(i2offset<<_i_i2ysh);
		_f_in2.y.i=_i_i2yget(_i_i2y);
	      }
	      if (_i_i2us) {
		_i_i2u.u=fsti2u.u+(i2offset<<_i_i2ush);
		_f_in2.u.i=_i_i2uget(_i_i2u);
	      }
	      if (_i_i2vs) {
		_i_i2v.u=fsti2v.u+(i2offset<<_i_i2vsh);
		_f_in2.v.i=_i_i2vget(_i_i2v);
	      }
	      
	      /* dequeue from queue(level)*/
	      *dequeuepointer=fifopos->next;

	      if (!(*dequeuepointer)) {
		/*switch from hierarchical lifo to fifo*/
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
		_fifo_distlevel=0;
	      }		

	      /* check in to emtpy list*/
	      fifopos->next=freeliststop;
	      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	      fifosize--;
#endif

	      /* check if pixel was dequeued in multiqueuein-mode*/
	      if (regs.matrix[12].m==MARKER_DEQUEUED) {
		/* set fifo to next element*/
		fifopos=*dequeuepointer;
#ifdef EVAL
		eval_LogNCResult(0,0);
#endif
		continue;
	      }

	      /*set marker*/
	      regs.matrix[12].m=MARKER_DEQUEUED;

	      /* pixel operation*/
	      if (operation) {
#ifdef PPTRACE
		PPTrace_LogIFifoOPIn(regs.matrix, &_f_in2, regs._n_cntx,
				     regs._n_cnty, _fifo_level,
				     _fifo_distlevel);
#endif
		operation(&regs.result,regs.matrix,&_f_in2,
			  regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
          PPTrace_LogFifoOPOut(regs.result);
#endif

		if (processmode == QOUT_PROC) {
		  /* store results*/
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		  /* store recursiv results to matrix*/
		  /* regs.matrix[12].ax=regs.result.ax;*/
		}

	      }

	      /*write marker*/
	      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	      eval_NCResult = 1;
	      eval_NCCrit = 0;
#endif
	      /* queue in neighbors*/
	      dir=startdir;
	      while (dir < 8) {

		/* check neighbor */
		pos=dirtopos[dir];

		if (regs.matrix[pos].m &&
		    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		  if (eval_NCResult==1)
		    eval_NCResult = 2;
#endif

#ifdef PPTRACE
		  PPTrace_LogIFifoNCIn(regs.matrix,&_f_in2);
#endif
		  valid=(short)neighcrit(regs.matrix,&_f_in2,
					 (int)pos,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
		  PPTrace_LogFifoNCOut(pos, valid);
#endif
         
		  if (valid) {
#ifdef EVAL
		    eval_NCResult = 3;
		    eval_NCCrit |= (1<<dir);
#endif

		    if (valid > HIERLEVELS) {
		      fprintf(stderr,"WARNING : hierachical fifo: "
			      "valid level must be between 1 and %d\n",
			      HIERLEVELS);
		      valid=HIERLEVELS;
		    }

		    /* queue in*/

		    /* get free entry*/
		    if (freelistinc) { 
		      /* emty fifo queue not completely initialised*/
		      fifopos=freelistpos;
		      freelistpos++;
		      fifopos=freelistpos;
		      if (freelistpos==(hfifobuffer+FIFOSIZE)) {
			freelistinc=0;
		      }
		    }
		    else {
		      /* check out from emtpy list*/
		      fifopos=freeliststop;
		      if (!fifopos) {
			addlib_stop_here("Warning: Fifo overflow in "
				"hierarchical fifo\n",0,0);
			exit(1);
		      }
		      freeliststop=freeliststop->next;
		    }
		    
		    /* check in to hierachical queue*/
		    valid--; /* allow hfifo0; old value for 
				valid=0 => invalid*/

		    /* to current fifo*/
		    if (valid<=_fifo_level) {
#ifdef EVAL
          eval_LogNCLevel(_fifo_level);
#endif
		      if (fifofirst) { /* fifo not empty*/
			fifolast->next=fifopos;
		      }
		      else { /*fifo empty*/
			fifofirst=fifopos;
		      }
		      fifolast=fifopos;
		      fifolast->next=0;
		    }

		    /*to queue with lower priority*/
		    else {
#ifdef EVAL
          eval_LogNCLevel(valid);
#endif
		      fifopos->next=stophlifo[valid];
		      stophlifo[valid]=fifopos;
		    }

#ifdef CNTFIFOSIZE
		    fifosize++;
		    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

		    /* new fifo values*/
		    fifopos->x=regs._n_cntx+dirtoxoff[dir];
		    fifopos->y=regs._n_cnty+dirtoyoff[dir];

		    /* set marker*/
		    regs.result.m=MARKER_QUEUED;
		    if (processmode == QOUT_PROC) {
		      MUpdate(&regs,pos);
		    }
		    else {
		      UpdatePos(&regs,pos);
		    }
		  }
		}
		/* next direction*/
		dir += dirinc;
	      }
#ifdef EVAL
	      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	      /* set fifo to next element*/
	      fifopos=*dequeuepointer;

	    }
	  }

	  /* reset startlevel for next  startpixel*/
	  _fifo_level=startlevel;

	  /*goto prev position*/
	  regs._n_cntx=regs._n_icntx=stx;
	  regs._n_cnty=regs._n_icnty=sty;
	     
	  regs._n_ia.u=stia.u;
	  regs._n_iy.u=stiy.u;
	  regs._n_iu.u=stiu.u;
	  regs._n_iv.u=stiv.u;
	  _i_i2a.u=sti2a.u;
	  _i_i2y.u=sti2y.u;
	  _i_i2u.u=sti2u.u;
	  _i_i2v.u=sti2v.u;
	  regs._n_ax.u=stax.u;
	  regs._n_m.u =stm.u;
	  regs._n_ra.u=stra.u;
	  regs._n_ry.u=stry.u;
	  regs._n_ru.u=stru.u;
	  regs._n_rv.u=strv.u;

	  /* load matrix with previous values*/
	  Preload(&regs);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel*/
      if (regs._n_cntx==lineend) {

	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
	ShiftDownLineLoad(&regs);
	_i_i2a.u+=_i_i2als;
	_i_i2y.u+=_i_i2yls;
	_i_i2u.u+=_i_i2uls;
	_i_i2v.u+=_i_i2vls;

	/* invert default direction*/
	if (defaultdir==ShiftRightLineLoad) {
	  lineend=0;
	  defaultdir=ShiftLeftLineLoad;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=ShiftRightLineLoad;
	}
	_i_i2as=-_i_i2as;
	_i_i2ys=-_i_i2ys;
	_i_i2us=-_i_i2us;
	_i_i2vs=-_i_i2vs;

      }
      else {
	defaultdir(&regs);
	_i_i2a.u+=_i_i2as;
	_i_i2y.u+=_i_i2ys;
	_i_i2u.u+=_i_i2us;
	_i_i2v.u+=_i_i2vs;

      }
    }
    break;
  case SCAN_HM|SCAN_COLLECT:
    /* noncollected horizontal meander scan*/
#ifdef EVAL
    eval_LogPreload();
#endif

    _i_i2a.u=fsti2a.u;
    _i_i2y.u=fsti2y.u;
    _i_i2u.u=fsti2u.u;
    _i_i2v.u=fsti2v.u;

    Preload(&regs);

    lineend=regs._n_maxx;
    defaultdir=ShiftRightLineLoad;
    _fifo_level=startlevel;

    while (1) {
      
#ifdef EVAL
      eval_LogSCPos(regs._n_cntx,regs._n_cnty);
#endif
       /* load intra channel*/
      if (_i_i2as) {
	_f_in2.a.i=_i_i2aget(_i_i2a);
      }
      if (_i_i2ys) {
	_f_in2.y.i=_i_i2yget(_i_i2y);
      }
      if (_i_i2us) {
	_f_in2.u.i=_i_i2uget(_i_i2u);
      }
      if (_i_i2vs) {
	_f_in2.v.i=_i_i2vget(_i_i2v);
      }

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogIFifoSCIn(regs.matrix, &_f_in2, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,&_f_in2,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif

	if (valid) {

	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    fifopos=freeliststop;
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical queue*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/

	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
      
      /* goto next pixel*/
      if (regs._n_cntx==lineend) {

	/*check for frame end*/
	if (regs._n_cnty == (regs._n_maxy)) break;
	ShiftDownLineLoad(&regs);
	_i_i2a.u+=_i_i2als;
	_i_i2y.u+=_i_i2yls;
	_i_i2u.u+=_i_i2uls;
	_i_i2v.u+=_i_i2vls;

	/* invert default direction*/
	if (defaultdir==ShiftRightLineLoad) {
	  lineend=0;
	  defaultdir=ShiftLeftLineLoad;
	}
	else {
	  lineend=regs._n_maxx;
	  defaultdir=ShiftRightLineLoad;
	}
	_i_i2as=-_i_i2as;
	_i_i2ys=-_i_i2ys;
	_i_i2us=-_i_i2us;
	_i_i2vs=-_i_i2vs;

      }
      else {
	defaultdir(&regs);
	_i_i2a.u+=_i_i2as;
	_i_i2y.u+=_i_i2ys;
	_i_i2u.u+=_i_i2us;
	_i_i2v.u+=_i_i2vs;
      }
    }

    /*do recursiv adressing*/
    for (; _fifo_level <= stoplevel; _fifo_level ++) {

      /* get value from lifo*/
      if ((fifopos=stophlifo[_fifo_level])) {
	dequeuepointer=&stophlifo[_fifo_level];
      }
      else {

	/*if lifo empty, try fifo*/
	if ((fifopos=fifofirst)) {
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	}
	else {
#ifdef EVAL
          eval_LogSkipLevel(_fifo_level);
#endif
          continue;
	}
      }

#ifdef EVAL
      {
        THFifoEntry *pTmp;
        int ElemInHrch;
        
        ElemInHrch=0;
        for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
          ElemInHrch++;
        
        eval_LogStartLevel(_fifo_level,ElemInHrch);
      }
#endif
      
      /*initialize distance counter*/
      lastindistlevel=_fifo_distlevel=0;



      while (fifopos) {  /*while fifo nor empty*/
	
	/*compute distlevel for hierarchy*/
	if (!stophlifo[_fifo_level]) {
	  if (lastindistlevel) {
	    _fifo_distlevel++;
	    lasthfifoindistlevel=fifolast;
	    lastindistlevel=0;
#ifdef EVAL
	    eval_LogDist();
#endif
	  }
	  if (fifopos == lasthfifoindistlevel) {
	    lastindistlevel=1;
	  }
	}

#ifdef EVAL
  eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	/* goto current position*/
	pre(&regs,fifopos->x,fifopos->y);

	/* load inter channel*/
	i2offset=fifopos->x+fifopos->y*_i_in2width;
	if (_i_i2as) {
	  _i_i2a.u=fsti2a.u+(i2offset<<_i_i2ash);
	  _f_in2.a.i=_i_i2aget(_i_i2a);
	}
	if (_i_i2ys) {
	  _i_i2y.u=fsti2y.u+(i2offset<<_i_i2ysh);
	  _f_in2.y.i=_i_i2yget(_i_i2y);
	}
	if (_i_i2us) {
	  _i_i2u.u=fsti2u.u+(i2offset<<_i_i2ush);
	  _f_in2.u.i=_i_i2uget(_i_i2u);
	}
	if (_i_i2vs) {
	  _i_i2v.u=fsti2v.u+(i2offset<<_i_i2vsh);
	  _f_in2.v.i=_i_i2vget(_i_i2v);
	}

	/* dequeue from queue(level)*/
	*dequeuepointer=fifopos->next;

	if (!(*dequeuepointer)) {
	  /*switch from hierarchical lifo to fifo*/
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	  _fifo_distlevel=0;
	}		

	/* check in to emtpy list*/
	fifopos->next=freeliststop;
	freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	fifosize--;
#endif

	/* check if pixel was dequeued in multiqueuein-mode*/
	if (regs.matrix[12].m==MARKER_DEQUEUED) {
	  /* set fifo to next element*/
	  fifopos=*dequeuepointer;
#ifdef EVAL
	  eval_LogNCResult(0,0);
#endif
	  continue;
	}

	/*set marker*/
	regs.matrix[12].m=MARKER_DEQUEUED;

	/* pixel operation*/
	if (operation) {
#ifdef PPTRACE
	  PPTrace_LogIFifoOPIn(regs.matrix, &_f_in2, regs._n_cntx,
                         regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
	  operation(&regs.result,regs.matrix,&_f_in2,
		    regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
    PPTrace_LogFifoOPOut(regs.result);
#endif

	  if (processmode == QOUT_PROC) {
	    /* store results*/
	    if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	    if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	    if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	    if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	    if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	    /* store recursiv results to matrix*/
	    /* regs.matrix[12].ax=regs.result.ax;*/
	  }

	}

	/*write marker*/
	regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	eval_NCResult = 1;
	eval_NCCrit = 0;
#endif
	/* queue in neighbors*/
	dir=startdir;
	while (dir < 8) {

	  /* check neighbor */
	  pos=dirtopos[dir];

	  if (regs.matrix[pos].m &&
	      (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	    if (eval_NCResult==1)
	      eval_NCResult = 2;
#endif

#ifdef PPTRACE
	    PPTrace_LogIFifoNCIn(regs.matrix,&_f_in2);
#endif
	    valid=(short)neighcrit(regs.matrix,&_f_in2,(int)pos,
				   regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogFifoNCOut(pos, valid);
#endif
      
 	    if (valid) {
#ifdef EVAL
	      eval_NCResult = 3;
	      eval_NCCrit |= (1<<dir);;
#endif

	      if (valid > HIERLEVELS) {
		fprintf(stderr,"WARNING : hierachical fifo: "
			"valid level must be between 1 and %d\n",
			HIERLEVELS);
		valid=HIERLEVELS;
	      }

	      /* queue in*/

	      /* get free entry*/
	      if (freelistinc) { 
		/* emty fifo queue not completely initialised*/
		fifopos=freelistpos;
		freelistpos++;
		fifopos=freelistpos;
		if (freelistpos==(hfifobuffer+FIFOSIZE)) {
		  freelistinc=0;
		}
	      }
	      else {
		/* check out from emtpy list*/
		fifopos=freeliststop;
		if (!fifopos) {
		  addlib_stop_here("Warning: Fifo overflow in "
			  "hierarchical fifo\n",0,0);
		  exit(1);
		}
		freeliststop=freeliststop->next;
	      }

	      /* check in to hierachical queue*/
	      valid--; /* allow hfifo0; old value for 
			  valid=0 => invalid*/

	      /* to current fifo*/
	      if (valid<=_fifo_level) {
#ifdef EVAL
          eval_LogNCLevel(_fifo_level);
#endif
		if (fifofirst) { /* fifo not empty*/
		  fifolast->next=fifopos;
		}
		else { /*fifo empty*/
		  fifofirst=fifopos;
		}
		fifolast=fifopos;
		fifolast->next=0;
	      }

	      /*to queue with lower priority*/
	      else {
#ifdef EVAL
          eval_LogNCLevel(valid);
#endif
		fifopos->next=stophlifo[valid];
		stophlifo[valid]=fifopos;
	      }

#ifdef CNTFIFOSIZE
	      fifosize++;
	      if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	      /* new fifo values*/
	      fifopos->x=regs._n_cntx+dirtoxoff[dir];
	      fifopos->y=regs._n_cnty+dirtoyoff[dir];

	      /* set marker*/
	      regs.result.m=MARKER_QUEUED;
	      if (processmode == QOUT_PROC) {
		MUpdate(&regs,pos);
	      }
	      else {
		UpdatePos(&regs,pos);
	      }
	    }
	  }
	  /* next direction*/
	  dir += dirinc;
	}
#ifdef EVAL
	eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	/* set fifo to next element*/
	fifopos=*dequeuepointer;

      }
    }

    break;

  case SCAN_LIST:

/*    for (pos=0; pos<25;regs.matrix[(int)pos++].m=0);*/
    _fifo_level=startlevel;

    while (startliststop) {
      
#ifdef EVAL
      eval_LogPreload();
      eval_LogSCPos(startliststop->x,startliststop->y);
#endif

      /* load pixel from list*/
      pre(&regs,startliststop->x,startliststop->y);

      /* load inter channel*/
      i2offset=startliststop->x+startliststop->y*_i_in2width;
      if (_i_i2as) {
	_i_i2a.u=fsti2a.u+(i2offset<<_i_i2ash);
	_f_in2.a.i=_i_i2aget(_i_i2a);
      }
      if (_i_i2ys) {
	_i_i2y.u=fsti2y.u+(i2offset<<_i_i2ysh);
	_f_in2.y.i=_i_i2yget(_i_i2y);
      }
      if (_i_i2us) {
	_i_i2u.u=fsti2u.u+(i2offset<<_i_i2ush);
	_f_in2.u.i=_i_i2uget(_i_i2u);
      }
      if (_i_i2vs) {
	_i_i2v.u=fsti2v.u+(i2offset<<_i_i2vsh);
	_f_in2.v.i=_i_i2vget(_i_i2v);
      }

      /* goto next startpixel*/
      fifopos=startliststop;
      startliststop=startliststop->next;

      /* check in to emtpy list*/
      fifopos->next=freeliststop;
      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
      fifosize--;
#endif

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogIFifoSCIn(regs.matrix, &_f_in2, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,&_f_in2,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif
        
	if (valid) {

#ifdef EVAL
	  eval_LogSCResult(2);
#endif
	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    /*fifopos=freeliststop; already set from list!!*/
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);

	  /*do recursiv adressing*/
	  for (; _fifo_level <= stoplevel;
	       _fifo_level ++) {

	    /* get value from lifo*/
	    if ((fifopos=stophlifo[_fifo_level])) {
	      dequeuepointer=&stophlifo[_fifo_level];
	    }
	    else {

	      /*if lifo empty, try fifo*/
	      if ((fifopos=fifofirst)) {
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
	      }
	      else {
#ifdef EVAL
		eval_LogSkipLevel(_fifo_level);
#endif
		continue;
	      }
	    }

#ifdef EVAL
	    {
	      THFifoEntry *pTmp;
	      int ElemInHrch;
        
	      ElemInHrch=0;
	      for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
		ElemInHrch++;
        
	      eval_LogStartLevel(_fifo_level,ElemInHrch);
	    }
#endif
	    /*initialize distance counter*/
	    lastindistlevel=_fifo_distlevel=0;

	    while (fifopos) {  /*while fifo nor empty*/

	      /*compute distlevel for hierarchy*/
	      if (!stophlifo[_fifo_level]) {
		if (lastindistlevel) {
		  _fifo_distlevel++;
		  lasthfifoindistlevel=fifolast;
		  lastindistlevel=0;
#ifdef EVAL
		  eval_LogDist();
#endif
		}
		if (fifopos == lasthfifoindistlevel) {
		  lastindistlevel=1;
		}
	      }

#ifdef EVAL
        eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	      /* goto current position*/
	      pre(&regs,fifopos->x,fifopos->y);

	      /* load inter channel*/
	      i2offset=fifopos->x+fifopos->y*_i_in2width;
	      if (_i_i2as) {
		_i_i2a.u=fsti2a.u+(i2offset<<_i_i2ash);
		_f_in2.a.i=_i_i2aget(_i_i2a);
	      }
	      if (_i_i2ys) {
		_i_i2y.u=fsti2y.u+(i2offset<<_i_i2ysh);
		_f_in2.y.i=_i_i2yget(_i_i2y);
	      }
	      if (_i_i2us) {
		_i_i2u.u=fsti2u.u+(i2offset<<_i_i2ush);
		_f_in2.u.i=_i_i2uget(_i_i2u);
	      }
	      if (_i_i2vs) {
		_i_i2v.u=fsti2v.u+(i2offset<<_i_i2vsh);
		_f_in2.v.i=_i_i2vget(_i_i2v);
	      }

	      /* dequeue from queue(level)*/
	      *dequeuepointer=fifopos->next;

	      if (!(*dequeuepointer)) {
		/*switch from hierarchical lifo to fifo*/
		dequeuepointer=&fifofirst;
		lasthfifoindistlevel=fifolast;
		_fifo_distlevel=0;
	      }		

	      /* check in to emtpy list*/
	      fifopos->next=freeliststop;
	      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	      fifosize--;
#endif

	      /* check if pixel was dequeued in multiqueuein-mode*/
	      if (regs.matrix[12].m==MARKER_DEQUEUED) {
		/* set fifo to next element*/
		fifopos=*dequeuepointer;
#ifdef EVAL
		eval_LogNCResult(0,0);
#endif
		continue;
	      }

	      /*set marker*/
	      regs.matrix[12].m=MARKER_DEQUEUED;

	      /* pixel operation*/
	      if (operation) {
#ifdef PPTRACE
		PPTrace_LogIFifoOPIn(regs.matrix, &_f_in2, regs._n_cntx,
                               regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
		operation(&regs.result,regs.matrix,&_f_in2,
			  regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
          PPTrace_LogFifoOPOut(regs.result);
#endif

		if (processmode == QOUT_PROC) {
		  /* store results*/
		  if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
		  if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
		  if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
		  if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
		  if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

		  /* store recursiv results to matrix*/
		  /* regs.matrix[12].ax=regs.result.ax;*/
		}

	      }

	      /*write marker*/
	      regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	      eval_NCResult = 1;
	      eval_NCCrit = 0;
#endif
	      /* queue in neighbors*/
	      dir=startdir;
	      while (dir < 8) {

		/* check neighbor */
		pos=dirtopos[dir];

		if (regs.matrix[pos].m &&
		    (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
		  if (eval_NCResult==1)
		    eval_NCResult = 2;
#endif

#ifdef PPTRACE
		  PPTrace_LogIFifoNCIn(regs.matrix,&_f_in2);
#endif
		  valid=(short)neighcrit(regs.matrix,&_f_in2,(int)pos,
					 regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogFifoNCOut(pos, valid);
#endif
      
 		  if (valid) {
#ifdef EVAL
		    eval_NCResult = 3;
		    eval_NCCrit |= (1<<dir);
#endif

		    if (valid > HIERLEVELS) {
		      fprintf(stderr,"WARNING : hierachical fifo: "
			      "valid level must be between 1 and %d\n",
			      HIERLEVELS);
		      valid=HIERLEVELS;
		    }

		    /* queue in*/

		    /* get free entry*/
		    if (freelistinc) { 
		      /* emty fifo queue not completely initialised*/
		      fifopos=freelistpos;
		      freelistpos++;
		      fifopos=freelistpos;
		      if (freelistpos==(hfifobuffer+FIFOSIZE)) {
			freelistinc=0;
		      }
		    }
		    else {
		      /* check out from emtpy list*/
		      fifopos=freeliststop;
		      if (!fifopos) {
			addlib_stop_here("Warning: Fifo overflow in "
				"hierarchical fifo\n",0,0);
			exit(1);
		      }
		      freeliststop=freeliststop->next;
		    }
		    
		    /* check in to hierachical queue*/
		    valid--; /* allow hfifo0; old value for 
				valid=0 => invalid*/

		    /* to current fifo*/
		    if (valid<=_fifo_level) {
#ifdef EVAL
          eval_LogNCLevel(_fifo_level);
#endif
		      if (fifofirst) { /* fifo not empty*/
			fifolast->next=fifopos;
		      }
		      else { /*fifo empty*/
			fifofirst=fifopos;
		      }
		      fifolast=fifopos;
		      fifolast->next=0;
		    }

		    /*to queue with lower priority*/
		    else {
#ifdef EVAL
          eval_LogNCLevel(valid);
#endif
		      fifopos->next=stophlifo[valid];
		      stophlifo[valid]=fifopos;
		    }

#ifdef CNTFIFOSIZE
		    fifosize++;
		    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

		    /* new fifo values*/
		    fifopos->x=regs._n_cntx+dirtoxoff[dir];
		    fifopos->y=regs._n_cnty+dirtoyoff[dir];

		    /* set marker*/
		    regs.result.m=MARKER_QUEUED;
		    if (processmode == QOUT_PROC) {
		      MUpdate(&regs,pos);
		    }
		    else {
		      UpdatePos(&regs,pos);
		    }
		  }
		}
		/* next direction*/
		dir += dirinc;
	      }
#ifdef EVAL
	      eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif
	    }

	    /* set fifo to next element*/
	    fifopos=*dequeuepointer;

	  }

	  /* reset startlevel for next  startpixel*/
	  _fifo_level=startlevel;
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
    }

    break;

  case SCAN_LIST|SCAN_COLLECT:

/*    for (pos=0; pos<25;regs.matrix[pos++].m=0);*/
    _fifo_level=startlevel;

    while (startliststop) {

#ifdef EVAL
      eval_LogPreload();
      eval_LogSCPos(startliststop->x,startliststop->y);
#endif
      /* load pixel from list*/
      pre(&regs,startliststop->x,startliststop->y);

      /* load inter channel*/
      i2offset=startliststop->x+startliststop->y*_i_in2width;
      if (_i_i2as) {
	_i_i2a.u=fsti2a.u+(i2offset<<_i_i2ash);
	_f_in2.a.i=_i_i2aget(_i_i2a);
      }
      if (_i_i2ys) {
	_i_i2y.u=fsti2y.u+(i2offset<<_i_i2ysh);
	_f_in2.y.i=_i_i2yget(_i_i2y);
      }
      if (_i_i2us) {
	_i_i2u.u=fsti2u.u+(i2offset<<_i_i2ush);
	_f_in2.u.i=_i_i2uget(_i_i2u);
      }
      if (_i_i2vs) {
	_i_i2v.u=fsti2v.u+(i2offset<<_i_i2vsh);
	_f_in2.v.i=_i_i2vget(_i_i2v);
      }

      /* goto next startpixel*/
      fifopos=startliststop;
      startliststop=startliststop->next;

      /* check in to emtpy list*/
      fifopos->next=freeliststop;
      freeliststop=fifopos;

#ifdef CNTFIFOSIZE
      fifosize--;
#endif

      /* execute referenced function*/
      if (regs.matrix[12].m && (regs.matrix[12].m<deniqueuein)) {

#ifdef PPTRACE
        PPTrace_LogIFifoSCIn(regs.matrix, &_f_in2, regs._n_cntx, regs._n_cnty);
#endif
        valid=(short)startcrit(regs.matrix,&_f_in2,regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
        PPTrace_LogFifoSCOut(valid);
#endif
        
	if (valid) {

	  if (valid > HIERLEVELS) {
	    fprintf(stderr,"WARNING : hierachical fifo: validlevel must be "
		    "between 1 and %d\n",HIERLEVELS);
	    valid=HIERLEVELS;
	  }

	  /* queue in*/

	  /* get free entry*/
	  if (freelistinc) { 
	    /* emty fifo queue not completely initialised*/
	    fifopos=freelistpos;
	    freelistpos++;
	    fifopos=freelistpos;
	    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	      freelistinc=0;
	    }
	  }
	  else {
	    /* check out from emtpy list*/
	    /*fifopos=freeliststop; already set from list!!*/
	    if (!fifopos) {
	      addlib_stop_here("Warning: Fifo overflow in hierarchical "
		      "fifo\n",0,0);
	      exit(1);
	    }
	    freeliststop=freeliststop->next;
	  }

	  /* check in to hierachical stack*/
	  valid--; /* allow hfifo0; old value for valid=0 => invalid*/
	  
	  /* to current fifo*/
	  if (valid<=_fifo_level) {
#ifdef EVAL
	    eval_LogSCLevel(_fifo_level);
	    eval_LogSCResult(2);
#endif
	    if (fifofirst) { /* fifo not empty*/
	      fifolast->next=fifopos;
	    }
	    else { /*fifo empty*/
	      fifofirst=fifopos;
	    }
	    fifolast=fifopos;
	    fifolast->next=0;
	  }

	  /*to queue with lower priority*/
	  else {
#ifdef EVAL
	    eval_LogSCLevel(valid);
	    eval_LogSCResult(2);
#endif
	    fifopos->next=stophlifo[valid];
	    stophlifo[valid]=fifopos;
	  }

#ifdef CNTFIFOSIZE
	  fifosize++;
	  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

	  /* new fifo values*/
	  fifopos->x=regs._n_cntx;
	  fifopos->y=regs._n_cnty;

	  /*set marker*/
	  regs.matrix[12].m=MARKER_QUEUED;
	  regs._n_mset(regs._n_m,regs.matrix[12].m);
	}
#ifdef EVAL
  else
    eval_LogSCResult(1);
#endif
      }
#ifdef EVAL
      else
        eval_LogSCResult(0);
#endif
    }

    /*do recursiv adressing*/
    for (_fifo_level=startlevel; _fifo_level <= stoplevel; _fifo_level ++) {

      /* get value from lifo*/
      if ((fifopos=stophlifo[_fifo_level])) {
	dequeuepointer=&stophlifo[_fifo_level];
      }
      else {

	/*if lifo empty, try fifo*/
	if ((fifopos=fifofirst)) {
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	}
	else {
#ifdef EVAL
	  eval_LogSkipLevel(_fifo_level);
#endif
	  continue;
	}
      }

#ifdef EVAL
      {
        THFifoEntry *pTmp;
        int ElemInHrch;
        
        ElemInHrch=0;
        for(pTmp=fifopos; pTmp!=NULL; pTmp=pTmp->next)
          ElemInHrch++;
        
        eval_LogStartLevel(_fifo_level,ElemInHrch);
      }
#endif
      
      /*initialize distance counter*/
      lastindistlevel=_fifo_distlevel=0;

      while (fifopos) {  /*while fifo nor empty*/

	/*compute distlevel for hierarchy*/
	if (!stophlifo[_fifo_level]) {
	  if (lastindistlevel) {
	    _fifo_distlevel++;
	    lasthfifoindistlevel=fifolast;
	    lastindistlevel=0;
#ifdef EVAL
	    eval_LogDist();
#endif
	  }
	  if (fifopos == lasthfifoindistlevel) {
	    lastindistlevel=1;
	  }
	}

#ifdef EVAL
  eval_LogNCPos(fifopos->x,fifopos->y);
#endif
	/* goto current position*/
	pre(&regs,fifopos->x,fifopos->y);

	/* load inter channel*/
	i2offset=fifopos->x+fifopos->y*_i_in2width;
	if (_i_i2as) {
	  _i_i2a.u=fsti2a.u+(i2offset<<_i_i2ash);
	  _f_in2.a.i=_i_i2aget(_i_i2a);
	}
	if (_i_i2ys) {
	  _i_i2y.u=fsti2y.u+(i2offset<<_i_i2ysh);
	  _f_in2.y.i=_i_i2yget(_i_i2y);
	}
	if (_i_i2us) {
	  _i_i2u.u=fsti2u.u+(i2offset<<_i_i2ush);
	  _f_in2.u.i=_i_i2uget(_i_i2u);
	}
	if (_i_i2vs) {
	  _i_i2v.u=fsti2v.u+(i2offset<<_i_i2vsh);
	  _f_in2.v.i=_i_i2vget(_i_i2v);
	}

	/* dequeue from queue(level)*/
	*dequeuepointer=fifopos->next;

	if (!(*dequeuepointer)) {
	  /*switch from hierarchical lifo to fifo*/
	  dequeuepointer=&fifofirst;
	  lasthfifoindistlevel=fifolast;
	  _fifo_distlevel=0;
	}		

	/* check in to emtpy list*/
	fifopos->next=freeliststop;
	freeliststop=fifopos;

#ifdef CNTFIFOSIZE
	fifosize--;
#endif

	/* check if pixel was dequeued in multiqueuein-mode*/
	if (regs.matrix[12].m==MARKER_DEQUEUED) {
	  /* set fifo to next element*/
	  fifopos=*dequeuepointer;
#ifdef EVAL
	  eval_LogNCResult(0,0);
#endif
	  continue;
	}

	/*set marker*/
	regs.matrix[12].m=MARKER_DEQUEUED;

	/* pixel operation*/
	if (operation) {
#ifdef PPTRACE
	  PPTrace_LogIFifoOPIn(regs.matrix, &_f_in2, regs._n_cntx,
                         regs._n_cnty, _fifo_level, _fifo_distlevel);
#endif
	  operation(&regs.result,regs.matrix,&_f_in2,
		    regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
    PPTrace_LogFifoOPOut(regs.result);
#endif

	  if (processmode == QOUT_PROC) {
	    /* store results*/
	    if (regs._n_raes) regs._n_raset(regs._n_ra,regs.result.a.i);
	    if (regs._n_ryes) regs._n_ryset(regs._n_ry,regs.result.y.i);
	    if (regs._n_rues) regs._n_ruset(regs._n_ru,regs.result.u.i);
	    if (regs._n_rves) regs._n_rvset(regs._n_rv,regs.result.v.i);
	    if (regs._n_axs) regs._n_axset(regs._n_ax,regs.result.ax.i);

	    /* store recursiv results to matrix*/
	    /* regs.matrix[12].ax=regs.result.ax;*/
	  }

	}

	/*write marker*/
	regs._n_mset(regs._n_m,regs.matrix[12].m);

#ifdef EVAL
	eval_NCResult = 1;
	eval_NCCrit = 0;
#endif
	/* queue in neighbors*/
	dir=startdir;
	while (dir < 8) {

	  /* check neighbor */
	  pos=dirtopos[dir];

	  if (regs.matrix[pos].m &&
	      (regs.matrix[pos].m<deniqueuein)) {
#ifdef EVAL
	    if (eval_NCResult==1)
	      eval_NCResult = 2;
#endif

#ifdef PPTRACE
	    PPTrace_LogIFifoNCIn(regs.matrix,&_f_in2);
#endif
	    valid=(short)neighcrit(regs.matrix,&_f_in2,(int)pos,
				   regs._n_cntx,regs._n_cnty);
#ifdef PPTRACE
      PPTrace_LogFifoNCOut(pos, valid);
#endif
      
 	    if (valid) {
#ifdef EVAL
	      eval_NCResult = 3;
	      eval_NCCrit |= (1<<dir);;
#endif

	      if (valid > HIERLEVELS) {
		fprintf(stderr,"WARNING : hierachical fifo: "
			"valid level must be between 1 and %d\n",
			HIERLEVELS);
		valid=HIERLEVELS;
	      }

	      /* queue in*/

	      /* get free entry*/
	      if (freelistinc) { 
		/* emty fifo queue not completely initialised*/
		fifopos=freelistpos;
		freelistpos++;
		fifopos=freelistpos;
		if (freelistpos==(hfifobuffer+FIFOSIZE)) {
		  freelistinc=0;
		}
	      }
	      else {
		/* check out from emtpy list*/
		fifopos=freeliststop;
		if (!fifopos) {
		  addlib_stop_here("Warning: Fifo overflow in "
			  "hierarchical fifo\n",0,0);
		  exit(1);
		}
		freeliststop=freeliststop->next;
	      }

	      /* check in to hierachical queue*/
	      valid--; /* allow hfifo0; old value for 
			  valid=0 => invalid*/

	      /* to current fifo*/
	      if (valid<=_fifo_level) {
#ifdef EVAL
          eval_LogNCLevel(_fifo_level);
#endif
		if (fifofirst) { /* fifo not empty*/
		  fifolast->next=fifopos;
		}
		else { /*fifo empty*/
		  fifofirst=fifopos;
		}
		fifolast=fifopos;
		fifolast->next=0;
	      }

	      /*to queue with lower priority*/
	      else {
#ifdef EVAL
          eval_LogNCLevel(valid);
#endif
		fifopos->next=stophlifo[valid];
		stophlifo[valid]=fifopos;
	      }

	      /* new fifo values*/
	      fifopos->x=regs._n_cntx+dirtoxoff[dir];
	      fifopos->y=regs._n_cnty+dirtoyoff[dir];

	      /* set marker*/
	      regs.result.m=MARKER_QUEUED;
	      if (processmode == QOUT_PROC) {
		MUpdate(&regs,pos);
	      }
	      else {
		UpdatePos(&regs,pos);
	      }
	    }
	  }
	  /* next direction*/
	  dir += dirinc;
	}
#ifdef EVAL
	eval_LogNCResult(eval_NCResult,eval_NCCrit);
#endif

	/* set fifo to next element*/
	fifopos=*dequeuepointer;

      }
    }

    break;
  default:
    fprintf(stderr,"Warning: Unsupported scan mode in fifo addressing\n");
    DestructNhood(&regs);
    /*free(hfifobuffer);  fifo is static!*/
    return;
  }
  DestructNhood(&regs);
  /* free(hfifobuffer);  fifo is static!*/

#ifdef PPTRACE
  PPTrace_LogFifoReg(0);
#endif

#ifdef EVAL
  eval_EndMeasureDur();
  eval_EndAddr();
  eval_EndNCrit();
  eval_EndSCrit();
  eval_EndOpr();
  eval_EndProc();
  eval_EndConSC();
  eval_EndCon();
  eval_EndScan();
  eval_EndRecMode();
  eval_EndAddrMode();
#endif

#ifdef CNTFIFOSIZE
  if (maxcallfifosize > maxfifosize) maxfifosize=maxcallfifosize;
#endif

  return;
}

/******* fillstartlist **************************************************/
/* put starpixel list in hfifobuffer address space*/

void fillstartlist(TPos *list,int listsize, short initfifo)

{
  unsigned short listpos;
  THFifoEntry *fifopos=0;            /*init only to prevent warnings*/

  /* initialize fifo*/
  if (initfifo) {
    freelistpos=hfifobuffer;
    freeliststop=0;
    startliststop=0;
    memset(stophlifo,0,HIERLEVELS*sizeof(THFifoEntry *));
    freelistinc=1;
#ifdef CNTFIFOSIZE
    fifosize=maxcallfifosize=0;
#endif
  
#ifdef RESETLIST
    fifoposin=hfifobuffer;
    for (i=1; i<FIFOSIZE; i++) {
      fifoposin->next=fifoposin+1;
      fifoposin++;
    }
    fifoposin->next=0;
    stophlifo[0]=fifoposin;
    fifoposout=hfifobuffer;
    starthfifo[0]=fifoposout;
#endif
  }

  for (listpos=0; listpos < listsize; listpos++) {

    /* queue in*/

    /* get free entry*/
    if (freelistinc) { 
      /* emty fifo queue not completely initialised*/
      fifopos=freelistpos;
      freelistpos++;
      fifopos=freelistpos;
      if (freelistpos==(hfifobuffer+FIFOSIZE)) {
	freelistinc=0;
      }
    }
    else {
      /* check out from emtpy list*/
      fifopos=freeliststop;
      if (!fifopos) {
	addlib_stop_here("Warning: Fifo overflow in hierarchical "
		"fifo\n",0,0);
	exit(1);
      }
      freeliststop=freeliststop->next;
    }

    /*check in to startlist*/
    fifopos->next=startliststop;
    startliststop=fifopos;

#ifdef CNTFIFOSIZE
    fifosize++;
    if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif


#ifdef CNTFIFOSIZE
    if (maxcallfifosize > maxfifosize) maxfifosize=maxcallfifosize;
#endif

    /* new fifo values*/
    fifopos->x=list[listpos].x;
    fifopos->y=list[listpos].y;
  }
}

/******* addstartpixel **************************************************/
/* put starpixel in hfifobuffer address space
   do not use in pixelprocessing functions */

void addstartpixel(TCoor x, TCoor y)

{
  THFifoEntry *fifopos;

  /* queue in*/

  /* get free entry*/
  if (freelistinc) { 
    /* emty fifo queue not completely initialised*/
    fifopos=freelistpos;
    freelistpos++;
    fifopos=freelistpos;
    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
      freelistinc=0;
    }
  }
  else {
    /* check out from emtpy list*/
    fifopos=freeliststop;
    if (!fifopos) {
      addlib_stop_here("Warning: Fifo overflow in hierarchical "
	      "fifo\n",0,0);
      exit(1);
    }
    freeliststop=freeliststop->next;
  }

  /*check in to startlist*/
  fifopos->next=startliststop;
  startliststop=fifopos;

#ifdef CNTFIFOSIZE
  fifosize++;
  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif


#ifdef CNTFIFOSIZE
  if (maxcallfifosize > maxfifosize) maxfifosize=maxcallfifosize;
#endif

  /* new fifo values*/
  fifopos->x=x;
  fifopos->y=y;
}

/******* proc_addstartpixel ************************************************/
/* put starpixel in hfifobuffer address space
   same as addstartpixel but
   for use in pixel processing function*/

void proc_addstartpixel(TCoor x, TCoor y)

{
  THFifoEntry *fifopos;

  /* queue in*/
#ifdef PPTRACE
  PPTrace_LogAddQI(x,y,257);  /* 257 = startpixellist*/
#endif

  /* get free entry*/
  if (freelistinc) { 
    /* emty fifo queue not completely initialised*/
    fifopos=freelistpos;
    freelistpos++;
    fifopos=freelistpos;
    if (freelistpos==(hfifobuffer+FIFOSIZE)) {
      freelistinc=0;
    }
  }
  else {
    /* check out from emtpy list*/
    fifopos=freeliststop;
    if (!fifopos) {
      addlib_stop_here("Warning: Fifo overflow in hierarchical "
	      "fifo\n",0,0);
      exit(1);
    }
    freeliststop=freeliststop->next;
  }

  /*check in to startlist*/
  fifopos->next=startliststop;
  startliststop=fifopos;

#ifdef CNTFIFOSIZE
  fifosize++;
  if (fifosize > maxcallfifosize) maxcallfifosize=fifosize;
#endif

#ifdef CNTFIFOSIZE
  if (maxcallfifosize > maxfifosize) maxfifosize=maxcallfifosize;
#endif

  /* new fifo values*/
  fifopos->x=x;
  fifopos->y=y;
}

/******* dumpstartlist **************************************************/
/* get starpixel list from hfifobuffer address space*/

void dumpstartlist(TPos *list,int *listsize, short initfifo)

{
  unsigned short listpos;
  THFifoEntry *fifopos=0;            /*init only to prevent warnings*/

  fifopos=startliststop;
  listpos=0;
  while (fifopos && (listpos < *listsize)) {
    list[listpos].x=fifopos->x;
    list[listpos].y=fifopos->y;
    listpos++;
    fifopos=fifopos->next;
  }
  *listsize=listpos;

  /* initialize fifo*/
  if (initfifo) {
    freelistpos=hfifobuffer;
    freeliststop=0;
    startliststop=0;
    memset(stophlifo,0,HIERLEVELS*sizeof(THFifoEntry *));
    freelistinc=1;
#ifdef CNTFIFOSIZE
    fifosize=maxcallfifosize=0;
#endif
  
#ifdef RESETLIST
    fifoposin=hfifobuffer;
    for (i=1; i<FIFOSIZE; i++) {
      fifoposin->next=fifoposin+1;
      fifoposin++;
    }
    fifoposin->next=0;
    stophlifo[0]=fifoposin;
    fifoposout=hfifobuffer;
    starthfifo[0]=fifoposout;
#endif

  }
}

