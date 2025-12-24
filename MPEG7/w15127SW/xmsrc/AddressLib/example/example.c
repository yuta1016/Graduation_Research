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
#include <stdio.h>
#include <string.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <vopio.h>
#include "example.h"

/******* DEFINES *************************************************************/
#define FLOATTEST
#ifdef FLOATTEST
#define TYPE FLOAT_TYPE
#define CASTFLOAT (float)
#define CASTPFLOAT (float *)
#define DIVHUNDRET /100.0
#define UNIONTYPE f
#else
#define TYPE UCHAR_TYPE   /* see momusys.h*/
#define CASTFLOAT
#define CASTPFLOAT
#define DIVHUNDRET
#define UNIONTYPE i
#endif
#define INTYPE UCHAR_TYPE   /* see momusys.h*/
#define ATYPE SHORT_TYPE  /* see momusys.h*/
#define ORIGX 4
#define ORIGY 4
#define MARKER 0          /* fifo_proc provides own status filed*/      

/******* MAKROS  *************************************************************/
#define vopdiff(out,in1,in2)  inter_proc(out,in1,in2,Y_CH, \
                                Y_CH, Y_CH,SCAN_HO, \
                                -1,-1,0,0,0,0,0,0,&pix_diff)

#define firhv121(out,in) intra_proc(out,in,0,Y_CH,Y_CH,CON_8,SCAN_HM, \
				  -1,-1,-1,-1,0, \
				  -1,-1,0,0,0,0,size1to1,&pix_hvfir121)

/* should use hierarchical fifos which will be provided soon*/
#define watershed(vop) fifo_proc(vop,vop,0,MARKER, \
				  A_CH,A_CH|Y_CH, \
				  CON_8,SCAN_HM|SCAN_COLLECT,QOUT_PROC, \
				  0,0,0,0,0,fifo_lable_schk, \
				  fifo_lable_nchk,fifo_expand_dequeue)



/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/

/******* FUNCTIONS ***********************************************************/
void loadfile(char *filename,MomVop *vop);
void dispimg(char *title,MomImage *img);

/******* Code ****************************************************************/



int main(int argc, char **argv)

{
  MomVop *vop1, *vop2,  /* see momusys.h*/
    *diff, *filtered;
  MomImageData a;       /* see momusys.h*/


  /* first malloc VOP`s with only Y- and alpha channel*/
  vop1=initvop(ORIGX,ORIGY,_400_,ATYPE,INTYPE,INTYPE,INTYPE);
  vop2=initvop(ORIGX,ORIGY,_400_,ATYPE,INTYPE,INTYPE,INTYPE);
  diff=initvop(ORIGX,ORIGY,_400_,ATYPE,TYPE,TYPE,TYPE);
  filtered=initvop(ORIGX,ORIGY,_400_,ATYPE,TYPE,TYPE,TYPE);

  /* load input data from file*/
  loadfile("nofile1.yuv",vop1);
  /* print result*/
  dispimg("vop1",vop1->y_chan);

  loadfile("nofile1.yuv",vop2);
  /* print result*/
  dispimg("vop2",vop2->y_chan);


  /* compute vop1 - vop2*/
  vopdiff(diff,vop1,vop2);   /* see macro!*/

  /* print result*/
  dispimg("vop1 - vop2",diff->y_chan);

  /* filter vop1 and write result to vop2*/
  firhv121(filtered,diff);

  /* print result*/
  dispimg("after filtering",filtered->y_chan);

  /* perform watershed on vop2*/
  /* set markers 1 and 2 in alpha mask*/
  a.u=xydata(filtered->a_chan,0,0);  
  setdata(a,filtered->a_chan->type,1);
  a.u=xydata(filtered->a_chan,vop2->width-1,vop2->height-1);  
  setdata(a,filtered->a_chan->type,2);
  /* print result*/
  dispimg("start mask",filtered->a_chan);
  watershed(filtered); /* should use hierarchical fifos which will
                      be provided soon*/

  /* print result*/
  dispimg("after watershed",filtered->a_chan);

  /* free memory of VOPs*/
  freevop(vop1);
  freevop(vop2);
  freevop(diff);
  freevop(filtered);
  return 0;
}


/* loadfile ******************************************************/
void loadfile(char *filename,MomVop *vop)

{
/* sorry no file IO*/
  static int run=0;
  MomImageData data;
  int size;
  int pixels,i;
  int file1[16]={255,255,255,129,
		 255,255, 90, 10,
		 255, 90, 10, 10,
		 129, 10, 10, 10};
  int file2[16]={  0,  0,  0,  0,
		   0,  0,  0, 10,
		   0,  0, 10, 10,
		   0, 10, 10, 10};

  
  pixels=vop->width*vop->height;

  /* read y_channel*/
  size=datasize(vop->y_chan);
  data.u=xydata(vop->y_chan,0,0);

  if (!run) {
    for (i=0; i<pixels;i++) {
      setdata(data,vop->y_chan->type,(TChan)file1[i]);
      data.u+=size;
    }
    run=1;
  }
  else {
    for (i=0; i<pixels;i++) {
      setdata(data,vop->y_chan->type,(TChan)file2[i]);
      data.u+=size;
    }
  }

  /* read a_channel*/
  size=datasize(vop->a_chan);
  data.u=xydata(vop->a_chan,0,0);
  if (data.u) memset(data.u,0,size*pixels);

  /* read u_channel*/
  size=datasize(vop->u_chan);
  data.u=xydata(vop->u_chan,0,0);
  if (data.u) memset(data.u,0,size*pixels);

  /* read v_channel*/
  size=datasize(vop->v_chan);
  data.u=xydata(vop->v_chan,0,0);
  if (data.u) memset(data.u,0,size*pixels);

}

/* dispimg ******************************************************/
void dispimg(char *title,MomImage *img)

{
  int xsize,x;
  int ysize,y;
  int size;
  MomImageData data;
  TChan tchandata;

  printf("\n%s\n",title);
  xsize=img->x;
  ysize=img->y;
  size=datasize(img);
  data.u=xydata(img,0,0);
  for (y=0; y<ysize; y++) {
    for (x=0; x< xsize; x++) {
      if (img->type== FLOAT_TYPE) {
	tchandata=getdata(data,img->type);
	printf("%8.2f",tchan_to_float(tchandata));
      }
      else {
	printf("%5ld",getdata(data,img->type));
      }
      data.u+=size;
    }
    printf("\n");
  }
  printf("\n");
}


/* pix_diff **************************************************/
void pix_diff(TPixel *res,TPixel *in1,TPixel *in2)

{
  res->a.i=(in1->a.i-in2->a.i);
  res->y.UNIONTYPE=(CASTFLOAT(in1->y.i-in2->y.i))DIVHUNDRET;
  res->u.UNIONTYPE=CASTFLOAT(in1->u.i-in2->u.i);
  res->v.UNIONTYPE=CASTFLOAT(in1->v.i-in2->v.i);
}


/* pix_hvfir121 **************************************************/

void pix_hvfir121(TPixel *res,TPixel *in, TCoor x, TCoor y)

{
  res->a.i=in[12].a.i;
#ifdef FLOATTEST
  res->y.UNIONTYPE=(in[6].y.UNIONTYPE + in[8].y.UNIONTYPE +
		    in[16].y.UNIONTYPE + in[18].y.UNIONTYPE +
		    2* (in[7].y.UNIONTYPE + in[11].y.UNIONTYPE+
			in[13].y.UNIONTYPE + in[17].y.UNIONTYPE) +
		    4 * in[12].y.UNIONTYPE)/ 16;
#else
  res->y.UNIONTYPE=(in[6].y.UNIONTYPE + in[8].y.UNIONTYPE +
		    in[16].y.UNIONTYPE + in[18].y.UNIONTYPE +
		    ((in[7].y.UNIONTYPE + in[11].y.UNIONTYPE+
			in[13].y.UNIONTYPE + in[17].y.UNIONTYPE)<<1) +
		    (in[12].y.UNIONTYPE << 2) + 8 )/ 16;
#endif
  res->u.UNIONTYPE=in[12].u.UNIONTYPE;
  res->v.UNIONTYPE=in[12].v.UNIONTYPE;
  res->ax.UNIONTYPE=in[12].ax.UNIONTYPE;
}

/* functions for watershed *************************************/

int fifo_lable_schk(TPixel *in,TCoor x,TCoor y)

{
  if (in[12].a.i != 0) return 0;
  if (in[6].a.i != 0)  return 1;
  if (in[7].a.i != 0)  return 1;
  if (in[8].a.i != 0)  return 1;
  if (in[11].a.i != 0) return 1;
  if (in[13].a.i != 0) return 1;
  if (in[16].a.i != 0) return 1;
  if (in[17].a.i != 0) return 1;
  if (in[18].a.i != 0) return 1;
  return 0;
}

int fifo_lable_nchk(TPixel *in,int pos,TCoor x,TCoor y)

{
  if (in[pos].a.i !=  0) return 0;
  return 1;
}

void fifo_expand_dequeue(TPixel *res,TPixel *in,TCoor x,TCoor y)

{
  int dir,pos;
#ifdef FLOATTEST
  float diff,mindiff=256;
#else
  int diff,mindiff=256;
#endif

  for (dir=0; dir < 8; dir++) {
    pos=dirtopos[dir];     /* defined in nhhod.h*/
    if (in[pos].a.i ==  0) continue;

#ifdef FLOATTEST
    diff=in[pos].y.UNIONTYPE-in[12].y.UNIONTYPE;
    if (diff < 0) diff=-diff;
#else
    diff=abs(in[pos].y.UNIONTYPE-in[12].y.UNIONTYPE);
#endif
    if (mindiff <= diff ) continue;

    mindiff=diff;
    res->a= in[pos].a;
  }
    
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;
}
