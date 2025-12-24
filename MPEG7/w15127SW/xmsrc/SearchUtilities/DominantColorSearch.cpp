//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski, LG-Elite.
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorSearch.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <float.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

#define Td2 255
//#define BINNUMDEF  "64"
#define VARPRESDEF  "1"

static double twopi=2.0*3.14159265358979323846;
static float VAR_RECL=60.0;
static float VAR_RECH=90.0;
//=============================================================================
	

namespace XM {
//#define DEBUG

const UUID DominantColorSearchInterface::myID = UUID("");
const char *DominantColorSearchInterface::myName = "Dominant Color Matching Interface";

const UUID DominantColorSearchTool::myID = UUID("");
const char *DominantColorSearchTool::myName = "Dominant Color Matching Tool";

const UUID DominantColorSearchInterfaceABC::myID = UUID();

//=============================================================================
//----------------------------------------------------------------------------
DominantColorSearchInterfaceABC::DominantColorSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorSearchInterface::DominantColorSearchInterface(DominantColorSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorSearchInterface::~DominantColorSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& DominantColorSearchInterface::GetInterfaceID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorSearchInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
void DominantColorSearchInterface::destroy(void)
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int DominantColorSearchInterface::
SetRefDescriptorInterface(DominantColorDescriptorInterfaceABC
                          *RefDominantColorDescriptorInterface)
{
  return m_SearchTool->
    SetRefDescriptorInterface(RefDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC* DominantColorSearchInterface::
GetQueryDescriptorInterface(void)
{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int DominantColorSearchInterface::
SetQueryDescriptorInterface(DominantColorDescriptorInterfaceABC
                            *QueryDominantColorDescriptorInterface)
{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
double DominantColorSearchInterface::GetDistance(void)
{
  return m_SearchTool->GetDistance();
}

//=============================================================================
//----------------------------------------------------------------------------
DominantColorSearchTool::DominantColorSearchTool():
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorSearchTool::DominantColorSearchTool(DominantColorDescriptorInterfaceABC *aQueryDescriptorInterface):
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0)
{
#ifdef VERBOSE
  fprintf(stderr,"Connect descriptor\n");
#endif

  /* create descriptor if it does not exist */
  if(!aQueryDescriptorInterface) {
    DominantColorDescriptor *descriptor = new DominantColorDescriptor(0,0);
    aQueryDescriptorInterface = descriptor->GetInterface();
  }

  /* connect descriptor to coding scheme */
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorSearchTool::~DominantColorSearchTool()
{
  if(m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if(m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& DominantColorSearchTool::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorSearchTool::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
bool DominantColorSearchTool::SupportsPush(void)
{
  return true;
}

//----------------------------------------------------------------------------
bool DominantColorSearchTool::SupportsPull(void)
{
  return false;
}

//----------------------------------------------------------------------------
int DominantColorSearchTool::
SetRefDescriptorInterface(DominantColorDescriptorInterfaceABC
			  *aRefDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();

    /* connect sub descriptors with sub search tools*/
    /* not available*/
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub search tools*/
    /* not available*/
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC* DominantColorSearchTool::
GetQueryDescriptorInterface(void)
{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  DominantColorSearchTool::
SetQueryDescriptorInterface(DominantColorDescriptorInterfaceABC
			    *aQueryDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryDescriptorInterface) return 0;

  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();

    /* conect sub descriptors with sub coding schemes*/
    /* not available*/
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub extraction tools schemes*/
    /* not available*/
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double DominantColorSearchTool::GetDistance()
{
  dc_t *DC1, *DC2;
  int i, j, N1, N2;
  int sc1, sc2; //added by LG CIT
  float **color1_float, **color2_float, **vars1, **vars2;
  float *per1_float, *per2_float;
  double d, dist=0.0, dmax, Td, total;
  bool varpres1, varpres2;

  if (!m_RefDescriptorInterface) {
    fprintf(stderr, "Error(DC_ST_GD): RDI missing\n");
    return DBL_MAX;
  }
  if (!m_QueryDescriptorInterface) {
    fprintf(stderr, "Error(DC_ST_GD): QDI missing\n");
    return DBL_MAX;
  }
  if (strcmp(m_RefDescriptorInterface->GetName(),
             "Dominant Color Description Interface") != 0) {
    fprintf(stderr, "Error(DC_ST_GD): RDI bad name\n");
    return DBL_MAX;
  }
  if (strcmp(m_QueryDescriptorInterface->GetName(),
             "Dominant Color Description Interface") != 0) {
    fprintf(stderr, "Error(DC_ST_GD): QDI bad name\n");
    return DBL_MAX;
  }

  varpres1 = m_RefDescriptorInterface->GetVariancePresent();
  varpres2 = m_QueryDescriptorInterface->GetVariancePresent();

  /* dequantization and color space re-conversion */

  bool colspcpres = (bool)atoi(getpara("ColorSpacePresent","0"));
  bool colquantpres = (bool)atoi(getpara("ColorQuantizationPresent","0"));

  TPixel in[25];
  TPixel res;
  TCoor x, y;
  int max_h;
  int component0, component1, component2;
  char textBINNUMDEF[8];
  sprintf(textBINNUMDEF, "%d", DominantColorDescriptor::BINNUMDEF);
  int bin_number0 = atoi(getpara("BinNumber0", textBINNUMDEF));
  int bin_number1 = atoi(getpara("BinNumber1", textBINNUMDEF));
  int bin_number2 = atoi(getpara("BinNumber2", textBINNUMDEF));
  float *conversionmatrix;

  // LC-new: requantisation
  int mydiv = atoi(getpara("ColorValueDivisor", "1"));

  component0 = 0;
  component1 = 1;
  component2 = 2; // default RGB

  //bin_number0 = 32;
  //bin_number1 = 32;
  //bin_number2 = 32; // default 5 bits

  max_h = 256;

  N1 = m_RefDescriptorInterface->GetDominantColorsNumber();
  N2 = m_QueryDescriptorInterface->GetDominantColorsNumber();
#ifdef DEBUG
  fprintf(stderr, "Info(DC_ST_GD): rd size: %d, qd size: %d\n", N1, N2);
#endif

  int **icnts1=new int*[3*N1];
  for(i=0;i<N1;i++) icnts1[i] = new int[3];
  int **icnts2=new int*[3*N2];
  for(i=0;i<N2;i++) icnts2[i] = new int[3];

  if(colspcpres) {
    switch(m_RefDescriptorInterface->GetColorSpaceDescriptorInterface()->
	   GetDescriptionColorSpace())
      {
      case RGB:
	component0 = 0; // R
	component1 = 1; // G
	component2 = 2; // B
	break;
      case YCRCB:
	component0 = 3; // Y
	component1 = 4; // Cb
	component2 = 5; // Cr
	break;
      case HSV:
	component0 = 6; // H
	component1 = 7; // S
	component2 = 8; // V
	break;
      case HMMD:
	component0 = 6; // H
	component1 = 12; // Sum
	component2 = 11; // Diff
	max_h=360;
	break;
      default:
	break;
      }
  }

  if(colquantpres) {
    bin_number0 = m_RefDescriptorInterface->GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component0);
    bin_number1 = m_RefDescriptorInterface->GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component1);
    bin_number2 = m_RefDescriptorInterface->GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component2);
  }

  DC1 = m_RefDescriptorInterface->GetDominantColors();
  DC2 = m_QueryDescriptorInterface->GetDominantColors();

  for(i=0; i<m_RefDescriptorInterface->GetDominantColorsNumber(); i++) {
    in[12].y.i = (DC1[i].centroid[0]/mydiv)*mydiv*max_h/bin_number0;
    in[12].u.i = (DC1[i].centroid[1]/mydiv)*mydiv*256/bin_number1;
    in[12].v.i = (DC1[i].centroid[2]/mydiv)*mydiv*256/bin_number2;

    if(colspcpres) {
      switch(m_RefDescriptorInterface->GetColorSpaceDescriptorInterface()->
	     GetDescriptionColorSpace())
	{
	case RGB:
	  icnts1[i][0]=in[12].y.i;
	  icnts1[i][1]=in[12].u.i;
	  icnts1[i][2]=in[12].v.i;
	  break;
	case YCRCB:
	  YUV_To_RGB(&res,in,x,y);
	  icnts1[i][0]=res.v.i;
	  icnts1[i][1]=res.y.i;
	  icnts1[i][2]=res.u.i;
	  break;
	case HSV:
	  HSV_To_RGB(&res,in,x,y);
	  icnts1[i][0]=res.v.i;
	  icnts1[i][1]=res.y.i;
	  icnts1[i][2]=res.u.i;
	  break;
	case HMMD:
	  HMMD_To_RGB(&res,in,x,y);
	  icnts1[i][0]=res.v.i;
	  icnts1[i][1]=res.y.i;
	  icnts1[i][2]=res.u.i;
	  break;
	case LinearMatrix:
	  conversionmatrix = m_RefDescriptorInterface->
	    GetColorSpaceDescriptorInterface()->
	    GetColorSpaceMatrix();
	  InverseLinearTransform(&res,in,x,y,conversionmatrix);
	  icnts1[i][0]=res.y.i;
	  icnts1[i][1]=res.u.i;
	  icnts1[i][2]=res.v.i;
	  break;
	default:
	  break;
	}
    }
    else {
      icnts1[i][0]=in[12].y.i;
      icnts1[i][1]=in[12].u.i;
      icnts1[i][2]=in[12].v.i;
    }
  }

  for(i=0; i<m_QueryDescriptorInterface->GetDominantColorsNumber(); i++) {
    in[12].y.i=(DC2[i].centroid[0]/mydiv)*mydiv*max_h/bin_number0;
    in[12].u.i=(DC2[i].centroid[1]/mydiv)*mydiv*256/bin_number1;
    in[12].v.i=(DC2[i].centroid[2]/mydiv)*mydiv*256/bin_number2;

    if(colspcpres) {
      switch(m_QueryDescriptorInterface->GetColorSpaceDescriptorInterface()->
	     GetDescriptionColorSpace())
	{
	case RGB:
	  icnts2[i][0]=in[12].y.i;
	  icnts2[i][1]=in[12].u.i;
	  icnts2[i][2]=in[12].v.i;
	  break;
	case YCRCB:
	  YUV_To_RGB(&res,in,x,y);
	  icnts2[i][0]=res.v.i;
	  icnts2[i][1]=res.y.i;
	  icnts2[i][2]=res.u.i;
	  break;
	case HSV:
	  HSV_To_RGB(&res,in,x,y);
	  icnts2[i][0]=res.v.i;
	  icnts2[i][1]=res.y.i;
	  icnts2[i][2]=res.u.i;
	  break;
	case HMMD:
	  HMMD_To_RGB(&res,in,x,y);
	  icnts2[i][0]=res.v.i;
	  icnts2[i][1]=res.y.i;
	  icnts2[i][2]=res.u.i;
	  break;
	case LinearMatrix:
	  conversionmatrix = m_QueryDescriptorInterface->
	    GetColorSpaceDescriptorInterface()->
	    GetColorSpaceMatrix();
	  InverseLinearTransform(&res,in,x,y,conversionmatrix);
	  icnts2[i][0]=res.y.i;
	  icnts2[i][1]=res.u.i;
	  icnts2[i][2]=res.v.i;
	  break;
	default:
	  break;
	}
    }
    else {
      icnts2[i][0]=in[12].y.i;
      icnts2[i][1]=in[12].u.i;
      icnts2[i][2]=in[12].v.i;
    }
  }

  per1_float = new float[N1];
  color1_float = new float*[N1];
  for( i=0; i<N1; ++i )
    color1_float[i] = new float[3];
  per2_float = new float[N2];
  color2_float = new float*[N2];
  for( i=0; i<N2; ++i )
    color2_float[i] = new float[3];

  for (i=0,total=0.0; i<N1; i++) {
    rgb2luv(icnts1[i], color1_float[i], 3);
    per1_float[i] = ((float)DC1[i].weight+0.5)/31.9999;
    total += per1_float[i];
  }
  for( i=0; i<N1; i++ )
    per1_float[i] /= total;

  for (i=0,total=0.0;i<N2;i++) {
    rgb2luv(icnts2[i], color2_float[i], 3);
    per2_float[i] = ((float)DC2[i].weight+0.5)/31.9999;
    total += per2_float[i];
  }
  for (i=0;i<N2;i++)
    per2_float[i] /= total;

#ifdef DEBUG
  int tmpsize=N1>N2?N2:N1;
  for(i=0; i<tmpsize; i++) {
    //fprintf(stderr, "Info(DC_ST_GD): %d: rd per0: %f, qd per0: %f\n",
    //        i, per1_float[i], per2_float[i]);
    //fprintf(stderr, "Info(DC_ST_GD): %d: rd val0: %f, qd val0: %f\n",
    //        i, color1_float[i][0], color2_float[i][0]);
    //fprintf(stderr, "Info(DC_ST_GD): %d: rd val1: %f, qd val1: %f\n",
    //      i, color1_float[i][1], color2_float[i][1]);
    //fprintf(stderr, "Info(DC_ST_GD): %d: rd val2: %f, qd val2: %f\n",
    //        i, color1_float[i][2], color2_float[i][2]);
    if(per1_float[i]==per2_float[i]
       && color1_float[i][0]==color2_float[i][0]
       && color1_float[i][1]==color2_float[i][1]
       && color1_float[i][2]==color2_float[i][2])
      fprintf(stderr, "Info(DC_ST_GD): %d: DCs identical!\n", i);

  }
#endif

  if(varpres1 == 0 || varpres2 == 0)
    varpres1 = varpres2 = (bool)atoi(getpara("VariancePresent", "0"));
  // !!!LC-temp!!!
  //varpres1 = varpres2 = false;

  if( varpres1 && varpres2 ) {
    vars1 = new float*[N1];
    for(i=0; i<N1; ++i) {
      vars1[i] = new float[3];
      vars1[i][0] = (DC1[i].variance[0]>0)?VAR_RECH:VAR_RECL;
      vars1[i][1] = (DC1[i].variance[1]>0)?VAR_RECH:VAR_RECL;
      vars1[i][2] = (DC1[i].variance[2]>0)?VAR_RECH:VAR_RECL;
    }
    vars2 = new float*[N2];
    for(i=0; i<N2; ++i) {
      vars2[i] = new float[3];
      vars2[i][0] = (DC2[i].variance[0]>0)?VAR_RECH:VAR_RECL;
      vars2[i][1] = (DC2[i].variance[1]>0)?VAR_RECH:VAR_RECL;
      vars2[i][2] = (DC2[i].variance[2]>0)?VAR_RECH:VAR_RECL;
    }
    dist = GetDistanceVariance(per1_float, color1_float, vars1, N1,
                               per2_float, color2_float, vars2, N2);
    for(i=0;i<N1;i++)
      delete [] vars1[i];
    delete [] vars1;
    for(i=0;i<N2;i++)
      delete [] vars2[i];
    delete [] vars2;
  }
  else {
    Td = sqrt((double)Td2);
    dmax=1.2*Td;

    // check correctness of descriptors
#ifdef DEBUG
    for( i=0; i<N1; i++ ) {
      for( j=i+1; j<N1; j++ ) {
        d = sqrt(sqr(color1_float[i][0]-color1_float[j][0]) +
                 sqr(color1_float[i][1]-color1_float[j][1]) +
                 sqr(color1_float[i][2]-color1_float[j][2]));
        if(d < Td)
          fprintf(stderr, "Warning(DC_ST_GD): distr[%d,%d]: %f\n", i, j, d);
      }
    }
    for( i=0; i<N2; i++ ) {
      for( j=i+1; j<N2; j++ ) {
        d = sqrt(sqr(color2_float[i][0]-color2_float[j][0]) +
                 sqr(color2_float[i][1]-color2_float[j][1]) +
                 sqr(color2_float[i][2]-color2_float[j][2]));
        if(d < Td)
          fprintf(stderr, "Warning(DC_ST_GD): distq[%d,%d]: %f\n", i, j, d);
      }
    }
#endif

    for( i=0; i<N1; i++ )
      dist += sqr(per1_float[i]);
    for( i=0; i<N2; i++ ) 
      dist += sqr(per2_float[i]);
    for( i=0; i<N1; i++ ) {
      for( j=0; j<N2; j++ ) {
        d = sqrt(sqr(color1_float[i][0]-color2_float[j][0]) +
                 sqr(color1_float[i][1]-color2_float[j][1]) +
                 sqr(color1_float[i][2]-color2_float[j][2]));
	if( d < Td ) {
          dist -= 2*(1-d/dmax)*per1_float[i]*per2_float[j];
        }
      }
    }
    dist = sqrt(fabs(dist));
  }

#ifdef DEBUG
  fprintf(stderr, "Info(DC_ST_GD): dist: %f\n", dist);
#endif


  delete [] per1_float;
  for(i=0;i<N1;i++)
    delete [] color1_float[i];
  delete [] color1_float;
  delete [] per2_float;
  for(i=0;i<N2;i++)
    delete [] color2_float[i];
  delete [] color2_float;

  for(i=0;i<N1;++i)
    delete [] icnts1[i];
  delete [] icnts1;
  for(i=0;i<N2;++i)
    delete [] icnts2[i];
  delete [] icnts2;

  sc1 = m_RefDescriptorInterface->GetSpatialCoherency();
  sc2 = m_QueryDescriptorInterface->GetSpatialCoherency();

  bool spatialcoher = (bool)atoi(getpara("SpatialCoherency","0"));
  if(!spatialcoher) sc1 = sc2 = 0;

  if((sc1 != 0) && (sc2 != 0)) {
    float fsc1=float(sc1)/31.0, fsc2=float(sc2)/31.0;
    if(varpres1) {
      //dist = sqrt(dist);  // variance-based distance is squared
      dist = 0.6*dist*fabs(fsc1 - fsc2) + 0.4*dist;
    }
    else
      dist = 0.3*dist*fabs(fsc1 - fsc2) + 0.7*dist;
  }

  return 1000000000.0*dist;
  //return dist;
}

//----------------------------------------------------------------------------
double DominantColorSearchTool::GetDistanceVariance(
  float *per1, float **color1, float **var1, int size1,
  float *per2, float **color2, float **var2, int size2)
{
  int     i1, i2;
  double  d0, d1, d2, v0, v1, v2, arg1, arg2;
  double  tmp, val=0.0;

  /* the overall formula is:
     Integral of ( sum_ij f_i*f_j + sum_ij g_*g_j - 2*sum_ij f_j*g_j ) */

  /* loop for f_i*f_j */
  for( i1=0; i1<size1; i1++ ) {
    for( i2=0; i2<size1; i2++ ) {
      d0 = color1[i1][0] - color1[i2][0];  v0 = var1[i1][0] + var1[i2][0];
      d1 = color1[i1][1] - color1[i2][1];  v1 = var1[i1][1] + var1[i2][1];
      d2 = color1[i1][2] - color1[i2][2];  v2 = var1[i1][2] + var1[i2][2];
      arg1 = (d0*d0/v0 + d1*d1/v1 + d2*d2/v2)/2.0;
      arg2 = twopi*sqrt(twopi*v0*v1*v2);
      tmp = per1[i1]*per1[i2]*exp(-arg1)/arg2;
      val += tmp;
    }
  }

  /* loop for g_i*g_j */
  for( i1=0; i1<size2; i1++ ) {
    for( i2=0; i2<size2; i2++ ) {
      d0 = color2[i1][0] - color2[i2][0];  v0 = var2[i1][0] + var2[i2][0];
      d1 = color2[i1][1] - color2[i2][1];  v1 = var2[i1][1] + var2[i2][1];
      d2 = color2[i1][2] - color2[i2][2];  v2 = var2[i1][2] + var2[i2][2];
      arg1 = (d0*d0/v0 + d1*d1/v1 + d2*d2/v2)/2.0;
      arg2 = twopi*sqrt(twopi*v0*v1*v2);
      tmp = per2[i1]*per2[i2]*exp(-arg1)/arg2;
      val += tmp;
    }
  }

  /* loop for f_i*g_j */
  for( i1=0; i1<size1; i1++ ) {
    for( i2=0; i2<size2; i2++ ) {
      d0 = color1[i1][0] - color2[i2][0];  v0 = var1[i1][0] + var2[i2][0];
      d1 = color1[i1][1] - color2[i2][1];  v1 = var1[i1][1] + var2[i2][1];
      d2 = color1[i1][2] - color2[i2][2];  v2 = var1[i1][2] + var2[i2][2];
      arg1 = (d0*d0/v0 + d1*d1/v1 + d2*d2/v2)/2.0;
      arg2 = twopi*sqrt(twopi*v0*v1*v2);
      tmp = per1[i1]*per2[i2]*exp(-arg1)/arg2;
      val -= 2.0*tmp;
    }
  }

  return val;

} /* GetDistanceVariance */


//----------------------------------------------------------------------------
DominantColorSearchInterfaceABC* DominantColorSearchTool::GetInterface(void)
{
  return &m_Interface;
}

//#undef DEBUG
};
