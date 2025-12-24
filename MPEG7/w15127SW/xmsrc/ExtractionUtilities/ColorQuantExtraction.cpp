//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
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
// Applicable File Name:  ColorQunatExtraction.cpp
//

#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <math.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

#define LQUANT_Y (((XM::ColorQuantizerDescriptorInterfaceABC*)_intra_infopt)->\
    GetNoOfBins(0))
#define LQUANT_U (((XM::ColorQuantizerDescriptorInterfaceABC*)_intra_infopt)->\
    GetNoOfBins(1))
#define LQUANT_V (((XM::ColorQuantizerDescriptorInterfaceABC*)_intra_infopt)->\
    GetNoOfBins(2))

void NonUniformExtraction(int range[][2], int *partial_index, TPixel *in, XM::ColorQuantizerDescriptorInterfaceABC *c_quant);
void GetPartialIndex(int *partial_index, XM::ColorQuantizerDescriptorInterfaceABC *pCQD);
int GetQuantIndexOfH(double H, int NUM_Q);
int	GetQuantIndexOfGray(double sum, int GrayLevels);
void SetHMMDQuantizerParameters(XM::ColorQuantizerDescriptorInterfaceABC *pS0);
int GetQuantIndexOfHMMD(double H, double sum, double diff);

int THRESHOLD_GRAY;	
int THRESHOLD[5];	    

int MAX_GRAY_LEVEL;	
int MAX_H_LEVEL[5];
int MAX_DIFF_LEVEL[5];
int MAX_SUM_LEVEL[5];
int SS_COLOR_CNT[5];
int LEVEL_CNT;

//----------------------------------------------------------------------------
void  QuantUniform(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  /* _intra_infopt   pointer to quantizer descriptor*/
  /* quantized values in channel,
     bin_no in ax_chan*/

  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y.i=(in[12].y.i*LQUANT_Y)/256;
  res->u.i=(in[12].u.i*LQUANT_U)/256;
  res->v.i=(in[12].v.i*LQUANT_V)/256;
  res->ax.i=((res->y.i*LQUANT_V)+res->v.i)*LQUANT_U+res->u.i;
}

//----------------------------------------------------------------------------
void  QuantNonUniform(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
	XM::ColorQuantizerDescriptorInterfaceABC *quantizer_interface;
	int index = 0;
	static int range[3][2];
	int i;

	range[0][0] = range[1][0] = range[2][0] = 0;
	range[0][1] = range[1][1] = range[2][1] = 256;

	quantizer_interface = (XM::ColorQuantizerDescriptorInterfaceABC*)_intra_infopt;
	// NonUniformExtraction(range, &partial_index, in,
	//   quantizer_interface);
	SetHMMDQuantizerParameters(quantizer_interface);
	index = GetQuantIndexOfHMMD((double)in[12].y.i,
				    (double)in[12].u.i, (double)in[12].v.i);

  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax.i=(long)index;
}

//----------------------------------------------------------------------------
void  QuantLookup(TPixel *res, TPixel *in,TCoor x, TCoor y)
{
  res->a=in[12].a; /*position 12 is central position of 5x5 2D input matrix*/
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;
}

//----------------------------------------------------------------------------
void NonUniformExtraction(int range[][2], int *partial_index, TPixel *in, XM::ColorQuantizerDescriptorInterfaceABC *c_quant)
{
  int comp;
  int type;
  float step;
  long pixel[3];
  int *boundary;
  int num_subspaces;
  int compmin, compmax;
  int i;
  int min, max;
  int current_index;

  pixel[0] = in[12].y.i;
  pixel[1] = in[12].u.i;
  pixel[2] = in[12].v.i;

  comp = c_quant->GetQuantizationComponent();
	
  type = c_quant->GetColorQuantizerType();
  num_subspaces = c_quant->GetNumSubspaces();
  compmin = range[comp][0];
  compmax = range[comp][1];
  if(type==Subspace_Uniform) {
    step = (compmax-compmin) / num_subspaces;
    for(i=0; i<num_subspaces; i++) {
      min = compmin + step * i;
      max = min + step;
      if(pixel[comp] >= min && pixel[comp] < max) {
	current_index = i;
	range[comp][0] = min;
	range[comp][1] = max;
	break;
      }
    }
    
  }
  else {
    boundary = c_quant->GetBoundary();
    for(i=0; i<num_subspaces; i++) {
      if(i==0) {
	min = compmin;
	max = boundary[i];
      }
      else if(i==num_subspaces-1) {
	min = boundary[i-1];
	max = compmax;
      }
      else {
	min = boundary[i-1];
	max = boundary[i];
      }
      if(pixel[comp] >= min && pixel[comp] < max) {
	current_index = i;
	range[comp][0] = min;
	range[comp][1] = max;
	break;
      }
    }
  }

  if(!c_quant->GetBottomFlag()) {
    XM::ColorQuantizerDescriptorInterfaceABC *pCQDInterfaceSubspace;

    for(i=0; i<current_index; i++) {
      pCQDInterfaceSubspace=c_quant->GetSubspaceInterface(i);
      GetPartialIndex(partial_index, pCQDInterfaceSubspace);
    }
    pCQDInterfaceSubspace=c_quant->GetSubspaceInterface(current_index);
    NonUniformExtraction(range, partial_index, in, pCQDInterfaceSubspace);
  }
  else {
    *partial_index += current_index;
  }
}

//----------------------------------------------------------------------------
void GetPartialIndex(int *partial_index, XM::ColorQuantizerDescriptorInterfaceABC *pCQD)
{
  int i;
  int num_subspaces;
  XM::ColorQuantizerDescriptorInterfaceABC *pCQDInterfaceSubspace;

  num_subspaces = pCQD->GetNumSubspaces();

  if(pCQD->GetBottomFlag())
    {
      *partial_index += num_subspaces;
    }
  else
    {

      for(i=0; i<num_subspaces; i++)
	{
	  pCQDInterfaceSubspace=pCQD->GetSubspaceInterface(i);
	  GetPartialIndex(partial_index, pCQDInterfaceSubspace);
	}
    }
}

//----------------------------------------------------------------------------
void SetHMMDQuantizerParameters(XM::ColorQuantizerDescriptorInterfaceABC *pS0)
{
  int i;
  int num_subspaces;
  int *boundary;

  // Diff Quantization
  num_subspaces = pS0->GetNumSubspaces();
  boundary = pS0->GetBoundary();
  THRESHOLD_GRAY = boundary[0];
  for(i=1; i<num_subspaces-1; i++)
    {
      THRESHOLD[i] = boundary[i];
    }
  for(i=num_subspaces-1; i<5; i++)
    {
      THRESHOLD[i] = 256;
      MAX_DIFF_LEVEL[i] = 0;
    }

  XM::ColorQuantizerDescriptorInterfaceABC *pS1;
  XM::ColorQuantizerDescriptorInterfaceABC *pS2;
  // Gray Quantization
  pS1 = pS0->GetSubspaceInterface(0);
  MAX_GRAY_LEVEL = pS1->GetNumSubspaces();

  for(i=1; i<num_subspaces; i++)
    {
      // Sum Quantization
      pS1 = pS0->GetSubspaceInterface(i);
      MAX_SUM_LEVEL[i]=pS1->GetNumSubspaces();
      // Hue Quantization
      pS2 = pS1->GetSubspaceInterface(0);
      MAX_H_LEVEL[i]=pS2->GetNumSubspaces();
      MAX_DIFF_LEVEL[i]=1;
    }
  for(i=num_subspaces; i<5; i++)
    {
      MAX_SUM_LEVEL[i]=0;
      MAX_H_LEVEL[i]=0;
      MAX_DIFF_LEVEL[i]=0;
    }

  SS_COLOR_CNT[0] =  MAX_GRAY_LEVEL;
  for(i=1; i<5; i++)
    {
      SS_COLOR_CNT[i] =  MAX_H_LEVEL[i]*MAX_DIFF_LEVEL[i]*MAX_SUM_LEVEL[i];
    }
  LEVEL_CNT = SS_COLOR_CNT[0]+SS_COLOR_CNT[1]+SS_COLOR_CNT[2]
    +SS_COLOR_CNT[3]+SS_COLOR_CNT[4];
}

//----------------------------------------------------------------------------
int GetQuantIndexOfHMMD(double H, double sum, double diff)
{
  int QuantIndex = -1, GrayIndex = -1;
  int HIndex = -1, MaxIndex = -1, MinIndex = -1;
  int DIndex = -1, MMDIndex = -1;

  if ((diff < THRESHOLD_GRAY) && (MAX_GRAY_LEVEL != 0)) //Subspace 0
    {
      GrayIndex = GetQuantIndexOfGray(sum, MAX_GRAY_LEVEL);
      QuantIndex = LEVEL_CNT - MAX_GRAY_LEVEL + GrayIndex;
    }	
  else if ((diff < THRESHOLD[1]) && (SS_COLOR_CNT[1] != 0)) //Subspace 1
    {   
      HIndex = GetQuantIndexOfH(H, MAX_H_LEVEL[1]);

      int SumIndex = -1;
      if (sum == 255.0 - 0.5*(double)THRESHOLD_GRAY)
	SumIndex = MAX_SUM_LEVEL[1] - 1;		
      else
	SumIndex = (int)floor( (sum-0.5*(double)THRESHOLD_GRAY)
			       *(double)MAX_SUM_LEVEL[1]
			       /(double)(255-THRESHOLD_GRAY));
      int DiffIndex;
      if (diff == THRESHOLD[1])
	DiffIndex =  MAX_DIFF_LEVEL[1] - 1;		
      else
	DiffIndex = (int)floor((diff-THRESHOLD_GRAY)*(double)MAX_DIFF_LEVEL[1]
			       /(double)(THRESHOLD[1]-THRESHOLD_GRAY));
      
      QuantIndex = SS_COLOR_CNT[2] + SS_COLOR_CNT[3] + SS_COLOR_CNT[4]
	+ HIndex*MAX_DIFF_LEVEL[1]*MAX_SUM_LEVEL[1]
	+ DiffIndex*MAX_SUM_LEVEL[1] + SumIndex;
      
    }
  else if ((diff < THRESHOLD[2]) && (SS_COLOR_CNT[2] != 0)) //Subspace 2
    {   
      HIndex = GetQuantIndexOfH(H, MAX_H_LEVEL[2]);
      
      int SumIndex = -1;
      if (sum == 255.0 - 0.5*(double)THRESHOLD[1])
	SumIndex = MAX_SUM_LEVEL[2] - 1;		
      else
	SumIndex = (int)floor( (sum-0.5*(double)THRESHOLD[1])
			       *(double)MAX_SUM_LEVEL[2]
			       /(double)(255-THRESHOLD[1]));
      int DiffIndex;
      if (diff == THRESHOLD[2])
	DiffIndex =  MAX_DIFF_LEVEL[2] - 1;		
      else
	DiffIndex = (int)floor((diff-THRESHOLD[1])*(double)MAX_DIFF_LEVEL[2]
			       /(double)(THRESHOLD[2]-THRESHOLD[1]));

      QuantIndex = SS_COLOR_CNT[3] + SS_COLOR_CNT[4]
	+ HIndex*MAX_DIFF_LEVEL[2]*MAX_SUM_LEVEL[2]
	+ DiffIndex*MAX_SUM_LEVEL[2] + SumIndex;

    }
  else if ((diff < THRESHOLD[3]) && (SS_COLOR_CNT[3] != 0)) //Subspace 3
    {   
      HIndex = GetQuantIndexOfH(H, MAX_H_LEVEL[3]);

      int SumIndex = -1;
      if (sum == 255.0 - 0.5*(double)THRESHOLD[2])
	SumIndex = MAX_SUM_LEVEL[3] - 1;		
      else
	SumIndex = (int)floor( (sum-0.5*(double)THRESHOLD[2])
			       *(double)MAX_SUM_LEVEL[3]
			       /(double)(255-THRESHOLD[2]));
      int DiffIndex;
      if (diff == THRESHOLD[3])
	DiffIndex =  MAX_DIFF_LEVEL[3] - 1;		
      else
	DiffIndex = (int)floor((diff-THRESHOLD[2])*(double)MAX_DIFF_LEVEL[3]
			       /(double)(THRESHOLD[3]-THRESHOLD[2]));

      QuantIndex = SS_COLOR_CNT[4]
	+ HIndex*MAX_DIFF_LEVEL[3]*MAX_SUM_LEVEL[3]
	+ DiffIndex*MAX_SUM_LEVEL[3] + SumIndex;

    }
  else if ((diff < THRESHOLD[4]) && (SS_COLOR_CNT[4] != 0)) //Subspace 4
    {   
      HIndex = GetQuantIndexOfH(H, MAX_H_LEVEL[4]);

      int SumIndex = -1;
      if (sum == 255.0 - 0.5*(double)THRESHOLD[3])
	SumIndex = MAX_SUM_LEVEL[4] - 1;		
      else
	SumIndex = (int)floor( (sum-0.5*(double)THRESHOLD[3])
			       *(double)MAX_SUM_LEVEL[4]
			       /(double)(255-THRESHOLD[3]));
      int DiffIndex;
      if (diff == THRESHOLD[4])
	DiffIndex =  MAX_DIFF_LEVEL[4] - 1;		
      else
	DiffIndex = (int)floor((diff-THRESHOLD[3])*(double)MAX_DIFF_LEVEL[4]
			       /(double)(THRESHOLD[4]-THRESHOLD[3]));

      QuantIndex = HIndex*MAX_DIFF_LEVEL[4]*MAX_SUM_LEVEL[4]
	+ DiffIndex*MAX_SUM_LEVEL[4] + SumIndex;

    }
  else
    {
      QuantIndex = -1;
    }

  if (QuantIndex >= LEVEL_CNT || QuantIndex < 0) 
    {
      printf("\n HMMD Quant Error!\n");
      exit(0);
    }

  return QuantIndex;
}

//----------------------------------------------------------------------------
int GetQuantIndexOfH(double H, int NUM_Q) {
  if(H == 360) return 0;	
  else 
    return (int)floor(H/360.0*(double)NUM_Q); 
}

//----------------------------------------------------------------------------
int	GetQuantIndexOfGray(double sum, int GrayLevels) {
  if(sum == 255.0) return GrayLevels -1;
  else
    return (int)floor(sum * (double)(GrayLevels)/(double)255.0);
}
