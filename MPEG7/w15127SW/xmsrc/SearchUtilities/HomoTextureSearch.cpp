// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi
// in cooperation with UCSB, Hyundai, HHI.
// (contributing organizations names)
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  HomoTextureSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/readparam.h"

#define Nray			128		// Num of ray
#define Nview			180		// Num of view
#define NUMofFEATURE	62
#define Quant_level		255
#define RadialDivision  5
#define RadialDivision2 3
#define AngularDivision 6

//=============================================================================
	

using namespace XM;


const UUID HomogeneousTextureSearchInterface::myID = UUID("14813410-a39c-11d3-9dcd-00609791d5bb");
const char *HomogeneousTextureSearchInterface::myName = "HomogeneousTextureSearchInterface";

const UUID HomogeneousTextureSearchTool::myID = UUID("1d3e4e90-a39c-11d3-9dcd-00609791d5bb");
const char *HomogeneousTextureSearchTool::myName = "HomogeneousTextureSearchTool";

const UUID HomogeneousTextureSearchInterfaceABC::myID = UUID("fedadf00-a39b-11d3-9dcd-00609791d5bb");


//---yjyu - 010217
void dequantization(int *intFeature,float *floatFeature);
void ReduceRedundancy(float *feature);
void Normalization(float *feature);
//---


//=============================================================================
HomogeneousTextureSearchInterfaceABC::HomogeneousTextureSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
HomogeneousTextureSearchInterface::HomogeneousTextureSearchInterface(HomogeneousTextureSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HomogeneousTextureSearchInterface::~HomogeneousTextureSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void HomogeneousTextureSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int HomogeneousTextureSearchInterface::
SetRefDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aRefHomogeneousTextureDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(aRefHomogeneousTextureDescriptorInterface);
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC* HomogeneousTextureSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int HomogeneousTextureSearchInterface::
SetQueryDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aQueryHomogeneousTextureDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(aQueryHomogeneousTextureDescriptorInterface);
}

//----------------------------------------------------------------------------
double HomogeneousTextureSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
HomogeneousTextureSearchTool::HomogeneousTextureSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
HomogeneousTextureSearchTool::
HomogeneousTextureSearchTool(HomogeneousTextureDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    HomogeneousTextureDescriptor *descriptor =
      new HomogeneousTextureDescriptor;

    aQueryDescriptorInterface=descriptor->GetInterface();
  }

  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HomogeneousTextureSearchTool::~HomogeneousTextureSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool HomogeneousTextureSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool HomogeneousTextureSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int HomogeneousTextureSearchTool::
SetRefDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aRefHomogeneousTextureDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefHomogeneousTextureDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefHomogeneousTextureDescriptorInterface;
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC* HomogeneousTextureSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  HomogeneousTextureSearchTool::
SetQueryDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aQueryHomogeneousTextureDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryHomogeneousTextureDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryHomogeneousTextureDescriptorInterface;
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->addref();
  else 
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
double HomogeneousTextureSearchTool::GetDistance(void)
{
	//---yjyu - 010217
	int i,j,m,n,flag;
	int size2=Nray/2;
	double temp_distance, distance=0;
	int RefFeature[NUMofFEATURE] = {0,};
	int QueryFeature[NUMofFEATURE] = {0,};
	float fRefFeature[NUMofFEATURE] = {0.0,};
	float fQueryFeature[NUMofFEATURE] = {0.0,};

	double wm[RadialDivision]={0.42,1.00,1.00,0.08,1.00};
	double wd[RadialDivision]={0.32,1.00,1.00,1.00,1.00};
	double wdc=0.28;
	double wstd=0.22;

	double Num_pixel=0;

	for(i=0;i<Nview;i++)		// # of angular feature channel = 6
		for(j=0;j<size2;j++)
				Num_pixel++;
	//---

	
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "HomogeneousTextureDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "HomogeneousTextureDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------


//---yjyu - 010217
	memcpy(RefFeature, m_RefDescriptorInterface->GetHomogeneousTextureFeature(), sizeof(int)*NUMofFEATURE);
	memcpy(QueryFeature, m_QueryDescriptorInterface->GetHomogeneousTextureFeature(), sizeof(int)*NUMofFEATURE);
	
	dequantization(RefFeature,fRefFeature);
	dequantization(QueryFeature,fQueryFeature);
//	ReduceReducdancy(fRefFeature);
//	ReduceReducdancy(fQueryFeature);
	Normalization(fRefFeature);
	Normalization(fQueryFeature);

	distance =(wdc*fabs(fRefFeature[0]-fQueryFeature[0]));
	distance +=(wstd*fabs(fRefFeature[1]-fQueryFeature[1]));

	char* option = getpara("option", "n");	//yjyu - 010222 - get parameter option, default value is n
												// r - rotation invariant, s - scale invariant, rs - rotation & scale invariant
//    printf("%s\n", option);	
	flag = m_RefDescriptorInterface->GetHomogeneousTextureFeatureFlag() * m_QueryDescriptorInterface->GetHomogeneousTextureFeatureFlag();

	if (!strcmp(option, "r")){
	    double min = DBL_MAX;
	    for (int i=AngularDivision;i>0;i--) {
			temp_distance = 0.0;
			for(n=0;n<RadialDivision;n++)
				for(m=0;m<AngularDivision;m++) {
					if (m >= i) {
						temp_distance+=(wm[n]*fabs(fRefFeature[n*AngularDivision+m+2-i]-fQueryFeature[n*AngularDivision+m+2])) 
							            + flag*(wd[n]*fabs(fRefFeature[n*AngularDivision+m+30+2-i]-fQueryFeature[n*AngularDivision+m+30+2]));
					}
					else {
						temp_distance+=(wm[n]*fabs(fRefFeature[(n+1)*AngularDivision+m+2-i]-fQueryFeature[n*AngularDivision+m+2]))
							            + flag*(wd[n]*fabs(fRefFeature[(n+1)*AngularDivision+m+30+2-i]-fQueryFeature[n*AngularDivision+m+30+2]));
					}
				}

		if (temp_distance < min) min = temp_distance;
		}

	    distance +=min;
	    return distance;
	}
	
	else if (!strcmp(option, "s")) {
	    double min = DBL_MAX;
	    {
	      int i;
	          for (i=0;i<3;i++) {
			temp_distance =0.0;
			for(n=2;n<RadialDivision;n++)
				for(m=0;m<AngularDivision;m++) {
					temp_distance+=(wm[n]*fabs(fRefFeature[n*AngularDivision+m+2]-fQueryFeature[(n-i)*AngularDivision+m+2]))
									+ flag*(wd[n]*fabs(fRefFeature[n*AngularDivision+m+30+2]-fQueryFeature[(n-i)*AngularDivision+m+30+2]));
		    }

		if (temp_distance < min)
		  min = temp_distance;
	      }
		
	    for (i=1;i<3;i++) {
			temp_distance = 0.0;
			temp_distance =(wdc*fabs(fRefFeature[0]-fQueryFeature[0]));
			temp_distance +=(wstd*fabs(fRefFeature[1]-fQueryFeature[1]));
			for(n=2;n<RadialDivision;n++)
				for(m=0;m<AngularDivision;m++) {
					temp_distance+=(wm[n]*fabs(fRefFeature[(n-i)*AngularDivision+m+2]-fQueryFeature[n*AngularDivision+m+2]))
						            + flag*(wd[n]*fabs(fRefFeature[(n-i)*AngularDivision+m+30+2]-fQueryFeature[n*AngularDivision+m+30+2]));
				}

		if (temp_distance < min)
		  min = temp_distance;
	      }
	    distance += min;
	    return distance;
	    }
	}

	else if(!strcmp(option, "rs") || !strcmp(option,"sr"))
	{
	    double min = DBL_MAX;
	    {
	        int j;
		for (j=0;j<3;j++)
		  for (i=AngularDivision;i>0;i--)
		    {
		      temp_distance =0.0;
		      for(n=2;n<RadialDivision;n++)
				for(m=0;m<AngularDivision;m++) {	
					if (m >= i) {
						temp_distance+=(wm[n]*fabs(fRefFeature[n*AngularDivision+m+2-i]-fQueryFeature[(n-j)*AngularDivision+m+2]))
										+ flag*(wd[n]*fabs(fRefFeature[n*AngularDivision+m+30+2-i]-fQueryFeature[(n-j)*AngularDivision+m+30+2]));
					}
					else {
						temp_distance+=(wm[n]*fabs(fRefFeature[(n+1)*AngularDivision+m+2-i]-fQueryFeature[(n-j)*AngularDivision+m+2]))
										+ flag*(wd[n]*fabs(fRefFeature[(n+1)*AngularDivision+m+30+2-i]-fQueryFeature[(n-j)*AngularDivision+m+30+2]));
					}
				}				
		      if (temp_distance < min) min = temp_distance;
		  }

		for (j=1;j<3;j++)
		  for (i=AngularDivision;i>0;i--) {
			   temp_distance =0.0;
			   for(n=2;n<RadialDivision;n++)
			     for(m=0;m<AngularDivision;m++) {	
			         if (m >= i) {
					     temp_distance+=(wm[n]*fabs(fRefFeature[(n-j)*AngularDivision+m+2-i]-fQueryFeature[n*AngularDivision+m+2]))
						    			+ flag*(wd[n]*fabs(fRefFeature[(n-j)*AngularDivision+m+30+2-i]-fQueryFeature[n*AngularDivision+m+30+2]));
					 }
				     else {
						 temp_distance+=(wm[n]*fabs(fRefFeature[(n+1-j)*AngularDivision+m+2-i]-fQueryFeature[n*AngularDivision+m+2]))
										+ flag*(wd[n]*fabs(fRefFeature[(n+1-j)*AngularDivision+m+30+2-i]-fQueryFeature[n*AngularDivision+m+30+2]));
					 }
				 }

			   if (temp_distance < min) min = temp_distance;
		  }
		distance = min + distance;
		return distance;
	    }
	}

	else
	{	//default option is NULL ==> if (option == NULL)
	  for(n=0;n<RadialDivision;n++)
	    for(m=0;m<AngularDivision;m++) {
			distance+=(wm[n]*fabs(fRefFeature[n*AngularDivision+m+2]-fQueryFeature[n*AngularDivision+m+2]))
					   + flag*(wd[n]*fabs(fRefFeature[n*AngularDivision+m+30+2]-fQueryFeature[n*AngularDivision+m+30+2]));
		}			
	  return distance;
	  /////////////////////////////
	}
//---
}

//----------------------------------------------------------------------------
HomogeneousTextureSearchInterfaceABC *HomogeneousTextureSearchTool::GetInterface(void)
{ return &m_Interface; }



//---yjyu - 010217
//----------------------------------------------------------------------------
void dequantization(int *intFeature,float *floatFeature)
{

double dcmin=0.0, dcmax=255.0;
double stdmin=1.309462,stdmax=109.476530;

double mmax[5][6]=
{ {18.392888,18.014313,18.002143,18.083845,18.046575,17.962099},
  {19.368960,18.628248,18.682786,19.785603,18.714615,18.879544},
  {20.816939,19.093605,20.837982,20.488190,20.763511,19.262577},
  {22.298871,20.316787,20.659550,21.463502,20.159304,20.280403},
  {21.516125,19.954733,20.381041,22.129800,20.184864,19.999331}};

double mmin[5][6]=
{{ 6.549734, 8.886816, 8.885367, 6.155831, 8.810013, 8.888925},
 { 6.999376, 7.859269, 7.592031, 6.754764, 7.807377, 7.635503},
 { 8.299334, 8.067422, 7.955684, 7.939576, 8.518458, 8.672599},
 { 9.933642, 9.732479, 9.725933, 9.802238,10.076958,10.428015},
 {11.704927,11.690975,11.896972,11.996963,11.977944,11.944282}};

double dmax[5][6]=
{ {21.099482,20.749788,20.786944,20.847705,20.772294,20.747129},
  {22.658359,21.334119,21.283285,22.621111,21.773690,21.702166},
  {24.317046,21.618960,24.396872,23.797967,24.329333,21.688523},
  {25.638742,24.102725,22.687910,25.216958,22.334769,22.234942},
  {24.692990,22.978804,23.891302,25.244315,24.281915,22.699811}};

double dmin[5][6]=
{{ 9.052970,11.754891,11.781252, 8.649997,11.674788,11.738701},
 { 9.275178,10.386329,10.066189, 8.914539,10.292868,10.152977},
 {10.368594,10.196313,10.211122,10.112823,10.648101,10.801070},
 {11.737487,11.560674,11.551509,11.608201,11.897524,12.246614},
 {13.303207,13.314553,13.450340,13.605001,13.547492,13.435994}};


 
	int n,m;
	float dcstep,stdstep,mstep,dstep;

	dcstep=(dcmax-dcmin)/Quant_level;
	floatFeature[0]=(dcmin+intFeature[0]*dcstep);

	stdstep=(stdmax-stdmin)/Quant_level;
	floatFeature[1]=(stdmin+intFeature[1]*stdstep);

	for(n=0;n<RadialDivision;n++)
		for(m=0;m<AngularDivision;m++)
		{
			mstep=(mmax[n][m]-mmin[n][m])/Quant_level;
			floatFeature[n*AngularDivision+m+2]=(mmin[n][m]+intFeature[n*AngularDivision+m+2]*mstep);
		}
	for(n=0;n<RadialDivision;n++)
		for(m=0;m<AngularDivision;m++)
		{
			dstep=(dmax[n][m]-dmin[n][m])/Quant_level;
			floatFeature[n*AngularDivision+m+32]=(dmin[n][m]+intFeature[n*AngularDivision+m+32]*dstep);
		}
}

//----------------------------------------------------------------------------
void ReduceRedundancy(float *feature)
{
	double Num_pixel=64*180;
    	int n,m;
	float tm[RadialDivision2][AngularDivision],td[RadialDivision2][AngularDivision];
	float tdvec[RadialDivision2][AngularDivision];

	for(n=0;n<RadialDivision2;n++)
		for(m=0;m<AngularDivision;m++){
			tm[n][m]=exp(feature[n*AngularDivision+m+2])-1;
			td[n][m]=exp(feature[n*AngularDivision+m+32])-1;
			td[n][m]=td[n][m]*td[n][m]+tm[n][m]*tm[n][m];
			tm[n][m]*=Num_pixel;
			td[n][m]*=Num_pixel;
		}

	for(m=0;m<AngularDivision;m++){
		tm[0][m]+=(tm[1][m]+tm[2][m]);
		tm[0][m]/=Num_pixel;
		tdvec[0][m]=(td[0][m]+td[1][m]+td[2][m])/Num_pixel;
		tdvec[0][m]=log(1+sqrt(tdvec[0][m]-tm[0][m]*tm[0][m]));
	}

	for(n=0;n<RadialDivision2-1;n++)
		for(m=0;m<AngularDivision;m++)
			tdvec[n+1][m]=feature[(n+3)*AngularDivision+m+32];

	for(n=0;n<RadialDivision2;n++)
		for(m=0;m<AngularDivision;m++)
			feature[n*AngularDivision+m+32]=tdvec[n][m];
}


//----------------------------------------------------------------------------
void Normalization(float *feature)
{
	int n,m;

double dcnorm=122.331353;
double stdnorm=51.314701;
double mmean[RadialDivision][AngularDivision]=
{{13.948462, 15.067986, 15.077915, 13.865536, 15.031283, 15.145633},
 {15.557970, 15.172251, 15.357618, 15.166167, 15.414601, 15.414378},
 {17.212408, 16.173027, 16.742651, 16.913837, 16.911480, 16.582123},
 {17.911104, 16.761711, 17.065447, 17.867548, 17.250889, 17.050728},
 {17.942741, 16.891190, 17.101770, 18.032434, 17.295305, 17.202160}};
double dmean[RadialDivision][AngularDivision]=
{{16.544933, 17.845844, 17.849176, 16.484509, 17.803377, 17.928810},
 {18.054886, 17.617800, 17.862095, 17.627794, 17.935352, 17.887453},
 {19.771456, 18.512341, 19.240444, 19.410559, 19.373478, 18.962496},
 {20.192045, 18.763544, 19.202494, 20.098207, 19.399082, 19.032280},
 {19.857040, 18.514065, 18.831860, 19.984838, 18.971045, 18.863575}};

	feature[0]/=dcnorm;
	feature[1]/=stdnorm;

	for(n=0;n<RadialDivision;n++)
		for(m=0;m<AngularDivision;m++)
			feature[n*AngularDivision+m+2]/=mmean[n][m];
	for(n=0;n<RadialDivision;n++)
		for(m=0;m<AngularDivision;m++)
			feature[n*AngularDivision+m+32]/=dmean[n][m];
}

