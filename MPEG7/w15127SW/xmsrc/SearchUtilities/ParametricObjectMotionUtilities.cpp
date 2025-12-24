///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
// 
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
// Applicable File Name:  ParametricObjectMotionUtilities.cpp
//

/*
 * Description: utilities for global motion query
 * -- ComputeSpeedField
 * -- distFOTotal
 * Modified:
 *	18/01/2000, INT 
 * 

 ***********************************************************HeaderEnd*********/

/************************    INCLUDE FILES    ********************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "momusys.h"
#include "ExtractionUtilities/gme_for_iso.h"
#include "ExtractionUtilities/imgutil.h"
#include "SearchUtilities/ParametricObjectMotionUtilities.h"


void OpticalFlow(int **alpha,double *param,double **vector_x, double **vector_y,int h,int w,double xOrigin,double yOrigin,double xg, double yg, int modelCode);


/***********************************************************CommentBegin******
 *
 * --    ComputeSpeedField
 *
 * Author : INT
 *
 * Created : 17-12-99
 *
 * Purpose :   
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/
void ComputeSpeedField(double *param1, double *param2, 
		       double **vectorx_FM1, double **vectory_FM1, 
		       double **vectorx_FM2, double **vectory_FM2, 
		       int sizex, int sizey, double xOrig1,double yOrig1,double xOrig2,double yOrig2,
		       double xg1, double yg1, double xg2, double yg2,int mCodeR,int mCodeQ)
{

  int i, j; 
  double xc, yc;
  double xFM1, yFM1, xFM2, yFM2; 
  
  
  xc=(double)sizex/2; yc=(double)sizey/2;
      
  for(i=0;i<sizex;i++)
    for(j=0;j<sizey;j++){
      xFM1=(double)((double)i-xOrig1-xc+xg1);
      yFM1=(double)((double)j-yOrig1-yc+yg1);
            
      xFM2=(double)((double)i-xOrig2-xc+xg2);
      yFM2=(double)((double)j-yOrig2-yc+yg2);	

      switch (mCodeR){
      case Translational:
	vectorx_FM1[i][j]=param1[0];
	vectory_FM1[i][j]=param1[1];
	break;
      case Rotation_Scale:
	vectorx_FM1[i][j]=param1[2]*xFM1+param1[3]*yFM1+param1[0];
	vectory_FM1[i][j]=-param1[3]*xFM1+param1[2]*yFM1+param1[1];
	break;
      case Affine:
	vectorx_FM1[i][j]=param1[2]*xFM1+param1[3]*yFM1+param1[0];
	vectory_FM1[i][j]=param1[4]*xFM1+param1[5]*yFM1+param1[1];
	break;
      case Perspective:
	vectorx_FM1[i][j]=(param1[2]*xFM1+param1[3]*yFM1+param1[0])/(param1[6]*xFM1+param1[7]*yFM1+1);
	vectory_FM1[i][j]=(param1[4]*xFM1+param1[5]*yFM1+param1[1])/(param1[6]*xFM1+param1[7]*yFM1+1);
	break;
      case Quadric:
	vectorx_FM1[i][j]=param1[2]*xFM1+param1[3]*yFM1+param1[6]*xFM1*yFM1+
	  param1[8]*xFM1*xFM1+param1[9]*yFM1*yFM1+param1[0];
	vectory_FM1[i][j]=param1[4]*xFM1+param1[5]*yFM1+param1[7]*xFM1*yFM1+
	  param1[10]*xFM1*xFM1+param1[11]*yFM1*yFM1+param1[1];

	break;
      default:
	fprintf(stderr,"Not a known model code");
      }

      switch (mCodeQ){
      case Translational:
	vectorx_FM2[i][j]=param2[0];
	vectory_FM2[i][j]=param2[1];
	break;
      case Rotation_Scale:
	vectorx_FM2[i][j]=param2[2]*xFM2+param2[3]*yFM2+param2[0];
	vectory_FM2[i][j]=-param2[3]*xFM2+param2[2]*yFM2+param2[1];
	break;
      case Affine:
	vectorx_FM2[i][j]=param2[2]*xFM2+param2[3]*yFM2+param2[0];
	vectory_FM2[i][j]=param2[4]*xFM2+param2[5]*yFM2+param2[1];
	break;
      case Perspective:
	vectorx_FM2[i][j]=(param2[2]*xFM2+param2[3]*yFM2+param2[0])/(param2[6]*xFM2+param2[7]*yFM2+1);
	vectory_FM2[i][j]=(param2[4]*xFM2+param2[5]*yFM2+param2[1])/(param2[6]*xFM2+param2[7]*yFM2+1);
	break;
      case Quadric:
	vectorx_FM2[i][j]=param2[2]*xFM2+param2[3]*yFM2+param2[6]*xFM2*yFM2+
	  param2[8]*xFM2*xFM2+param2[9]*yFM2*yFM2+param2[0];
	vectory_FM2[i][j]=param2[4]*xFM2+param2[5]*yFM2+param2[7]*xFM2*yFM2+
	  param2[10]*xFM2*xFM2+param2[11]*yFM2*yFM2+param2[1];
	break;
      default:
	fprintf(stderr,"Not a known model code");
      }

    }
}


unsigned char MemAllocINT(int **alpha,int height,int width){
	int i;
	alpha=(int **)malloc(height*sizeof(int*));
	if (alpha==NULL){
		fprintf(stderr, "error - memory allocation");
		return 0;
	}
	for (i=0;i<height;i++)
	alpha[i]=(int *)malloc(width*sizeof(int));
	if (alpha[i]==NULL){
		fprintf(stderr, "error - memory allocation");
		return 0;
	}
	return 1;
}
unsigned char MemAllocDOUBLE(double **alpha,int height,int width){
	int i;
	alpha=(double **)malloc(height*sizeof(double*));
	if (alpha==NULL){
		fprintf(stderr, "\nerror - memory allocation");
		return 0;
	}
	for (i=0;i<height;i++)
	alpha[i]=(double *)malloc(width*sizeof(double));
	if (alpha[i]==NULL){
		fprintf(stderr, "\nerror - memory allocation");
		return 0;
	}
	return 1;
}

/***********************************************************CommentBegin******
 *
 * --    distFOTotal
 *
 * Author : INT
 *
 * Created : 17-12-99
 *
 * Purpose :   
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/

double distFOTotal(double **fo1_vx, double **fo1_vy, double **fo2_vx, double **fo2_vy, int hh, int ww, int NormType) 
{
  double dist = 0.0;  double disttemp; 
  int i, j;
  double norm_v1, norm_v2; 
  
  for(i=0;i<hh;i++)
    for(j=0;j<ww;j++){

	if((fo1_vx[i][j])||(fo1_vy[i][j])||(fo2_vx[i][j])||(fo2_vy[i][j])){
      switch (NormType){
      case L1Norm:
	dist += fabs(fo1_vx[i][j] - fo2_vx[i][j]);
	dist += fabs(fo1_vy[i][j] - fo2_vy[i][j]);
	break;
      case L2Norm:
	dist += ( (fo1_vx[i][j] - fo2_vx[i][j])*(fo1_vx[i][j] - fo2_vx[i][j]) );
	dist += ( (fo1_vy[i][j] - fo2_vy[i][j])*(fo1_vy[i][j] - fo2_vy[i][j]) );
	break;
      case Correlation:
	disttemp=(fo1_vx[i][j]*fo2_vx[i][j]);
	disttemp+=(fo1_vy[i][j]*fo2_vy[i][j]);
	disttemp=fabs(disttemp);
	norm_v1=sqrt(fo1_vx[i][j]*fo1_vx[i][j]+fo1_vy[i][j]*fo1_vy[i][j]);
	norm_v2=sqrt(fo2_vx[i][j]*fo2_vx[i][j]+fo2_vy[i][j]*fo2_vy[i][j]);
	if((norm_v1<1e-6)||((norm_v2<1e-6)))
	  disttemp = 1.0;
	else		
	  disttemp /= (norm_v1*norm_v2);

	dist += (1.0 - disttemp);
	break;
      case Amplitude:
	dist+=fabs(sqrt(fo1_vx[i][j]*fo1_vx[i][j]+fo1_vy[i][j]*fo1_vy[i][j])-
		sqrt(fo2_vx[i][j]*fo2_vx[i][j]+fo2_vy[i][j]*fo2_vy[i][j]));
	break;
      default:
	fprintf(stderr,"\n This norm type doesn't exist in XM documentation");

      }
    }
    }
  return dist;
}
/***********************************************************CommentBegin******
 *
 * --    GetXYCenter
 *
 * Author : INT
 *
 * Created : 17-12-99
 *
 * Purpose :   
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/

void  GetXYCenter(MomVop *ref_vop,double *xcenter,double *ycenter)
{

UChar *UCref, *UCref_alpha;
SInt *ref, *ref_alpha;
Int ref_width, ref_height;

  Int i, j, ref_pel,xo,yo,noOfP,x,y;


 UCref = (UChar *) GetMomImageData(GetMomVopY(ref_vop));
 UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(ref_vop));

 ref_width = (GetMomVopWidth(ref_vop));
 ref_height = (GetMomVopHeight(ref_vop));

 ref        = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));

 ref_pel = 0;
xo=0;yo=0;noOfP=0;

 for(y=0; y<ref_height; y++)
   for(x=0; x<ref_width; x++, ref_pel++)
     {
       ref       [ref_pel] = (SInt)UCref[ref_pel];
       ref_alpha [ref_pel] = (SInt)UCref_alpha [ref_pel]; 
	if (ref_alpha[ref_pel]) {
	xo+=x;
	yo+=y;
	noOfP++;
	}
 }
*xcenter=(double)xo/noOfP;
*ycenter=(double)yo/noOfP;
}




void ComputeSpeedFieldAllMask(double *param1,double *param2,
		  	 MomVop *m_RefVop,MomVop *m_QueryVop,
			 double **foSelected_vxD,double **foSelected_vyD,
			 double **fo_vxD,double **fo_vyD,int mCodeR,int mCodeQ, double xOrigin1, double yOrigin1
			, double xOrigin2, double yOrigin2)
{
	int i,j;
	UChar *UCquery_alpha, *UCref_alpha;
	SInt  *query_alpha, *ref_alpha;
	int  ref_width,ref_height,query_width, query_height;

	int **alphaSelected;
	int **alpha_generic;
	int **alpha;


	UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(m_RefVop));
	UCquery_alpha = (UChar *) GetMomImageData(GetMomVopA(m_QueryVop)); 

	ref_width = (GetMomVopWidth(m_RefVop));
	ref_height = (GetMomVopHeight(m_RefVop));
	query_width = (GetMomVopWidth(m_QueryVop));
	query_height = (GetMomVopHeight(m_QueryVop));

 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 query_alpha = (SInt *)malloc(query_width*query_height*sizeof(SInt));


 int query_pel = 0,x,y;
 int xgaS=0,ygaS=0,npw=0,h,w;
 int xgaG=0,ygaG=0;

 for(y=0; y<ref_height; y++)
   for(x=0; x<ref_width; x++, query_pel++)
	ref_alpha [query_pel] = (SInt)UCref_alpha [query_pel]; 

query_pel=0;
 for(y=0; y<query_height; y++)
   for(x=0; x<query_width; x++, query_pel++)
     	query_alpha[query_pel] = (SInt)UCquery_alpha[query_pel];

 	h=MAX(ref_height,query_height);
	w=MAX(ref_width,query_width);

	alphaSelected=new int* [query_height];
	 for(i=0;i<query_height;i++)
	  alphaSelected[i]=new int [query_width];
	alpha_generic=new int* [ref_height];
	 for(i=0;i<ref_height;i++)
	  alpha_generic[i]=new int [ref_width];

	alpha=new int* [h];
	for(i=0;i<h;i++){

	alpha[i]=new int [w];
	}

	int p=0;
	for (i=0;i<query_height;i++)
	for (j=0;j<query_width;j++,p++)	
	alphaSelected[i][j]=(int)query_alpha[p];
	p=0;
	for (i=0;i<ref_height;i++)
	for (j=0;j<ref_width;j++,p++)
	alpha_generic[i][j]=(int)ref_alpha[p];

        for(i=0; i<query_height; i++)
	  for(j=0; j<query_width; j++){
	  if (alphaSelected[i][j]) {
	  xgaS+=j;
	  ygaS+=i;
	  npw++;
	  }
	}
	  xgaS=(int)(xgaS/npw);
	  ygaS=(int)(ygaS/npw);
	npw=0;

       for(i=0; i<ref_height; i++)
	  for(j=0; j<ref_width; j++)
	  if (alpha_generic[i][j]) {
	  xgaG+=j;
	  ygaG+=i;
	  npw++;
	  }
	  xgaG=(int)(xgaG/npw);
	  ygaG=(int)(ygaG/npw);



int a1,a2;

       for(i=0; i<h; i++)
	  for(j=0; j<w; j++){

	  a1=0;a2=0;
	  if ( (i-(int)(h/2)+ygaS>=0)&&(i-(int)(h/2)+ygaS<query_height)&&(j-(int)(w/2)+xgaS>=0)&&(j-(int)(w/2)+xgaS<query_width) )
	  a1=alphaSelected[i-(int)(h/2)+ygaS][j-(int)(w/2)+xgaS];
	  if ( (i-(int)(h/2)+ygaG>=0)&&(i-(int)(h/2)+ygaG<ref_height)&&(j-(int)(w/2)+xgaG>=0)&&(j-(int)(w/2)+xgaG<ref_width) )
	  a2=alpha_generic[i-(int)(h/2)+ygaG][j-(int)(w/2)+xgaG];
	  if ((a1)||(a2)) alpha[i][j]=255;
	  else alpha[i][j]=0;
	  }

	

    /************************ MOTION ***************************/ 


    OpticalFlow(alpha,param2,foSelected_vxD, foSelected_vyD,h,w,xOrigin2,yOrigin2,(double)xgaS,(double)ygaS,mCodeQ);
    OpticalFlow(alpha,param1,fo_vxD, fo_vyD,h,w,xOrigin1,yOrigin1,(double)xgaG,(double)ygaG,mCodeR);

	for(i=0;i<query_height;i++)
	  delete [] alphaSelected[i];
	 delete [] alphaSelected;
	for(i=0;i<ref_height;i++)
	  delete [] alpha_generic[i];
	 delete [] alpha_generic;
	for(i=0;i<h;i++)
	  delete [] alpha[i];
	 delete [] alpha;

 free(ref_alpha); 
 free(query_alpha); 


   }

   
void OpticalFlow(int **alpha,double *param,double **vector_x, double **vector_y,int h,int w,double xOrig,double yOrig,double xg, double yg, int mCode){

  int i, j; 
  double xc, yc;
  double x, y; 
  xc=(double)w/2; yc=(double)h/2;
      


  for(i=0;i<h;i++)
    for(j=0;j<w;j++)
	if (alpha[i][j]){
        x=(double)((double)j-xOrig-xc+xg);
        y=(double)((double)i-yOrig-yc+yg);

      switch (mCode){
      case Translational:
	vector_x[i][j]=param[0];
	vector_y[i][j]=param[1];

	break;
      case Rotation_Scale:
	vector_x[i][j]=param[2]*x+param[3]*y+param[0];
	vector_y[i][j]=-param[3]*x+param[2]*y+param[1];

	break;
      case Affine:
	vector_x[i][j]=param[2]*x+param[3]*y+param[0];
	vector_y[i][j]=param[4]*x+param[5]*y+param[1];

	break;
      case Perspective:
	vector_x[i][j]=(param[2]*x+param[3]*y+param[0])/(param[6]*x+param[7]*y+1);
	vector_y[i][j]=(param[4]*x+param[5]*y+param[1])/(param[6]*x+param[7]*y+1);

	break;
      case Quadric:
	vector_x[i][j]=param[2]*x+param[3]*y+param[6]*x*y+
	  param[8]*x*x+param[9]*y*y+param[0];
	vector_y[i][j]=param[4]*x+param[5]*y+param[7]*x*y+
	  param[10]*x*x+param[11]*y*y+param[1];

	break;
      default:
	fprintf(stderr,"Not a known model code");
      }
    }
else {
vector_y[i][j]= 0.0;
vector_x[i][j]= 0.0;
}
	
}


