//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Warsaw University of Technology, Karol Wnukowicz, Wladys³aw Skarbek
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
// Copyright (c) 1998-2000.
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
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

#define DCT_START 40.0
#define DCT_STOP  600.0

//static double twopi=2.0*3.14159265358979323846;
//static float VAR_RECL=60.0;
//static float VAR_RECH=90.0;

int getReciprocalCT(int R, int G, int B, double *ct);
void sortCTValues(double *ct_vals, double *perc_vals, int num);
int getDCTDistribution(double *ct, double *perc, int num, double *distr_ct, double *distr_perc);

//=============================================================================


using namespace XM;


const UUID DominantColorTemperatureSearchInterface::myID = UUID("");
const char *DominantColorTemperatureSearchInterface::myName = "Dominant Color Temperature Matching Interface";

const UUID DominantColorTemperatureSearchTool::myID = UUID("");
const char *DominantColorTemperatureSearchTool::myName = "Dominant Color Temperature Matching Tool";

const UUID DominantColorTemperatureSearchInterfaceABC::myID = UUID();

//=============================================================================
//----------------------------------------------------------------------------
DominantColorTemperatureSearchInterfaceABC::DominantColorTemperatureSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorTemperatureSearchInterface::DominantColorTemperatureSearchInterface( DominantColorTemperatureSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorTemperatureSearchInterface::~DominantColorTemperatureSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& DominantColorTemperatureSearchInterface::GetInterfaceID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorTemperatureSearchInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
void DominantColorTemperatureSearchInterface::destroy(void)
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int DominantColorTemperatureSearchInterface::
SetRefDescriptorInterface(DominantColorDescriptorInterfaceABC
                          *RefDominantColorDescriptorInterface)
{
  return m_SearchTool->
    SetRefDescriptorInterface(RefDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC* DominantColorTemperatureSearchInterface::
GetQueryDescriptorInterface(void)
{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int DominantColorTemperatureSearchInterface::
SetQueryDescriptorInterface(DominantColorDescriptorInterfaceABC
                            *QueryDominantColorDescriptorInterface)
{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
double DominantColorTemperatureSearchInterface::GetDistance(void)
{
  return m_SearchTool->GetDistance();
}

//=============================================================================
//----------------------------------------------------------------------------
DominantColorTemperatureSearchTool::DominantColorTemperatureSearchTool():
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorTemperatureSearchTool::DominantColorTemperatureSearchTool(DominantColorDescriptorInterfaceABC *aQueryDescriptorInterface):
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
DominantColorTemperatureSearchTool::~DominantColorTemperatureSearchTool()
{
  if(m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if(m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& DominantColorTemperatureSearchTool::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorTemperatureSearchTool::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
bool DominantColorTemperatureSearchTool::SupportsPush(void)
{
  return true;
}

//----------------------------------------------------------------------------
bool DominantColorTemperatureSearchTool::SupportsPull(void)
{
  return false;
}

//----------------------------------------------------------------------------
int DominantColorTemperatureSearchTool::
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
DominantColorDescriptorInterfaceABC* DominantColorTemperatureSearchTool::
GetQueryDescriptorInterface(void)
{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  DominantColorTemperatureSearchTool::
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
double DominantColorTemperatureSearchTool::GetDistance()
{

  dc_t *DC1, *DC2;
  int i, N1, N2;
  double *ct1, *ct2;
  double *per1, *per2;
  double dist=0.0, total;

  // if this flag is false then black colors are ignored,
  // in that case, for example, red image and black-red image
  // are regarded to be the same
  bool NonCTincl=false;


  if (!m_RefDescriptorInterface)
    return (unsigned long)-1;
  if (!m_QueryDescriptorInterface)
    return (unsigned long)-1;
  if (strcmp(m_RefDescriptorInterface->GetName(),
             "Dominant Color Description Interface") != 0)
    return (unsigned long)-1;
  if (strcmp(m_QueryDescriptorInterface->GetName(),
             "Dominant Color Description Interface") != 0)
    return (unsigned long)-1;

  /* dequantization and color space re-conversion */

  bool colspcpres = (bool)atoi(getpara("ColorSpacePresent","0"));
  bool colquantpres = (bool)atoi(getpara("ColorQuantizationPresent","0"));

  TPixel in[25];
  TPixel res;
  TCoor x, y;
  int max_h;
  int component0, component1, component2;
  int bin_number0, bin_number1, bin_number2;
  float *conversionmatrix;

  // LC-new: requantisation
  int mydiv = atoi(getpara("ColorValueDivisor", "1"));

  component0 = 0;
  component1 = 1;
  component2 = 2; // default RGB

  bin_number0 = 32;
  bin_number1 = 32;
  bin_number2 = 32; // default 5 bits

  max_h = 256;

  N1 = m_RefDescriptorInterface->GetDominantColorsNumber();
  N2 = m_QueryDescriptorInterface->GetDominantColorsNumber();

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

  per1 = new double[N1];
  ct1 = new double[N1];
  per2 = new double[N2];
  ct2 = new double[N2];
  int N1_valid=0;
  int N2_valid=0;


  //int dbg1=1;

  for (i=0,total=0.0; i<N1; i++) {
    int ctvalid = getReciprocalCT(icnts1[i][0], icnts1[i][1], icnts1[i][2], &ct1[N1_valid]);
    if(ctvalid != 0){
      per1[N1_valid] = ((float)DC1[i].weight+0.5)/31.9999;
      total += per1[N1_valid];
      N1_valid++;
    } else {
	  if(NonCTincl){
	    total += ((float)DC1[i].weight+0.5)/31.9999;;
	  }
	}
  }
  for( i=0; i<N1_valid; i++ )
    per1[i] /= total;

  sortCTValues(ct1, per1, N1_valid);

  double *distr_ct1=new double[N1_valid+2];
  double *distr_per1=new double[N1_valid+2];
  int size1=getDCTDistribution(ct1, per1, N1_valid, distr_ct1, distr_per1);

  /*if(dbg1){
    for (i=0; i<size1; i++) {
      printf("%d: CT=%f; perc=%f;\n",i,  distr_ct1[i], distr_per1[i]);
	}
  }*/

  for (i=0,total=0.0;i<N2;i++) {
    int ctvalid = getReciprocalCT(icnts2[i][0], icnts2[i][1], icnts2[i][2], &ct2[N2_valid]);
    if(ctvalid != 0){
      per2[N2_valid] = ((float)DC2[i].weight+0.5)/31.9999;
      total += per2[N2_valid];
      N2_valid++;
    } else {
	  if(NonCTincl){
	    total += ((float)DC2[i].weight+0.5)/31.9999;;
	  }
	}
  }
  for (i=0;i<N2_valid;i++)
    per2[i] /= total;

  sortCTValues(ct2, per2, N2_valid);

  double *distr_ct2=new double[N2_valid+2];
  double *distr_per2=new double[N2_valid+2];
  int size2=getDCTDistribution(ct2, per2, N2_valid, distr_ct2, distr_per2);

  /*if(dbg1){
    for (i=0; i<size2; i++) {
      printf("%d: CT=%f; perc=%f;\n",i,  distr_ct2[i], distr_per2[i]);
	}
  }*/

  int pos1=1;
  int pos2=1;
  int cur1=0;
  int cur2=0;
  double x1=DCT_START;
  double x2=DCT_START;
  bool listMin1=false;  // which list have smaller T value

  while(true){
	// be care with this stop condition...
	if(pos1>=size1) break;
	if(pos2>=size2) break;

    if((distr_ct1[pos1] < distr_ct2[pos2]) && (pos1<size1) &&(!(pos2==size2))){
		x1=distr_ct1[pos1];
	    cur1=pos1;
	    pos1++;
	    listMin1=true;
	} else {
		x1=distr_ct2[pos2];
	    cur2=pos2;
	    pos2++;
	    listMin1=false;
	}

	if(x1==x2) continue;

	double y1=0;
	double y2=0;
	if(listMin1){
		y1=distr_per2[cur2];
		y2=distr_per1[cur1-1];
	} else {
	    y1 = distr_per2[cur2-1];
		y2 = distr_per1[cur1];
	}
	dist += fabs(x2-x1) * fabs(y2-y1);
	x2=x1;
	// after loop
	//cout << "Dist=" << dist << ", X1=" << x1 << ", X2=" << x2;
	//cout << ", Y1=" << y1 << ", Y2=" << y2 	<< endl;

  }

  delete [] per1;
  delete [] ct1;
  delete [] per2;
  delete [] ct2;

  delete [] distr_per1;
  delete [] distr_ct1;
  delete [] distr_per2;
  delete [] distr_ct2;

  for(i=0;i<N1;++i)
    delete [] icnts1[i];
  delete [] icnts1;
  for(i=0;i<N2;++i)
    delete [] icnts2[i];
  delete [] icnts2;

  //cout << "Dist=" << dist << endl;

  return dist;
}

// to be deleted
//----------------------------------------------------------------------------
double DominantColorTemperatureSearchTool::GetDistanceVariance(float* per1_float,
        float** color1_float, float** vars1, int N1, float* per2_float, float** color2_float,
        float** vars2, int N2 ){
  return 0.0;
}


//----------------------------------------------------------------------------
DominantColorTemperatureSearchInterfaceABC* DominantColorTemperatureSearchTool::GetInterface(void)
{
  return &m_Interface;
}

//-- sRGB -> XYZ conversion matrix
// from CTBrowsing_Extraction.cpp
double	SRGB2XYZ1[3][3] =
	{	0.4124,0.3576,0.1805,
		0.2126,0.7152,0.0722,
		0.0193,0.1192,0.9505};

#define CT_Y_TRESHOLD 0.05

//----------------------------------------------------------------------------
int getReciprocalCT(int R, int G, int B, double *ct){
  double r_srgb, g_srgb, b_srgb;
  double px, py;
  double xyz[3];
  int ctemp;
  inversegamma(R, G, B, &r_srgb, &g_srgb, &b_srgb);
  //printf("R: %d; G: %d; B: %d \n", R, G, B);
  //printf("r: %g; g: %g; b: %g \n", r_srgb, g_srgb, b_srgb);

  xyz[0] = SRGB2XYZ1[0][0]*r_srgb+SRGB2XYZ1[0][1]*g_srgb+SRGB2XYZ1[0][2]*b_srgb;
  xyz[1] = SRGB2XYZ1[1][0]*r_srgb+SRGB2XYZ1[1][1]*g_srgb+SRGB2XYZ1[1][2]*b_srgb;
  xyz[2] = SRGB2XYZ1[2][0]*r_srgb+SRGB2XYZ1[2][1]*g_srgb+SRGB2XYZ1[2][2]*b_srgb;

  if(xyz[1] < CT_Y_TRESHOLD) return 0;

  /*if((xyz[0]<0)||(xyz[1]<0)||(xyz[2]<0)) {
    xyz[0] = 0.0;
    xyz[1] = 0.0;
    xyz[2] = 0.0;
  }

  if((xyz[0]>1.0)||(xyz[1]>1.0)||(xyz[2]>1.0))
  {
    xyz[0] = 1.0;
    xyz[1] = 1.0;
    xyz[2] = 1.0;
  }*/

  if((xyz[0] + xyz[1] + xyz[2]) == 0) {
    px = 0;
    py = 0;
  } else {
    px = xyz[0] / (xyz[0] + xyz[1] + xyz[2]);
    py = xyz[1] / (xyz[0] + xyz[1] + xyz[2]);
  }

  convert_xy2temp(px, py, &ctemp);
  //printf(" %d: \n", ctemp);
  *ct=1000000.0/ctemp;
  return 1;
}

//----------------------------------------------------------------------------
void sortCTValues(double *ct_vals, double *perc_vals, int num){

  // sorting reciprocal color temperature values
  for(int i=0; i<num; i++){
    double cur_min=ct_vals[i];
    int cur_min_idx=i;
    for (int j = (i+1); j < num; j++) {
      if (ct_vals[j] < cur_min) {
        cur_min = ct_vals[j];
        cur_min_idx = j;
      }
    }
	if(cur_min_idx == i) continue;
    // exchange values: current <-> min
    double tct = ct_vals[i];
    double tperc = perc_vals[i];
    ct_vals[i] = ct_vals[cur_min_idx];
    perc_vals[i] = perc_vals[cur_min_idx];
    ct_vals[cur_min_idx] = tct;
    perc_vals[cur_min_idx] = tperc;
  }
}

//----------------------------------------------------------------------------
int getDCTDistribution(double *ct, double *perc, int num, double *distr_ct, double *distr_perc){
  //int distr_size=num;
  int i;
  double quant_dct_stop = DCT_STOP - 1.0; //

  // Creating distribution table
  int pos=0;
  double perc_sum=0.0;
  if(num == 0 || ct[0] > DCT_START){
    distr_ct[pos]=DCT_START;
    distr_perc[pos]=0;
  }
  if(num>0){
    double oldval=DCT_START;;
    for(i = 0; i < num; i++){
      if(ct[i] != oldval){
        pos++;
      }
      distr_ct[pos] = ct[i];
      perc_sum += perc[i];
      distr_perc[pos] = perc_sum*100;
      oldval=ct[i];
    }
  }
  pos++;
  if(num == 0 || ct[num-1] < quant_dct_stop){
    distr_ct[pos]=DCT_STOP;
    distr_perc[pos]=100;
	pos++;
  }
  /*
  for(int i=0; i < pos; i++){
    cout << i << ": " << distr_ct[i] << " (" << distr_perc[i] << "); ";
  }
  cout << endl;
  */
  return pos;
}
