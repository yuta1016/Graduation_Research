//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
// Applicable File Name:  ScalableColorSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/readparam.h"

//=============================================================================
void histo_3d_inversion4(int tabelle[5][255],int tablae,int *histogram,
int h_size,int s_size,int v_size,int hist_nr,int ivert)
{
int i,sum,dif,x1,y1,x2,y2;
int ind1,ind2,index;
int matrix[16][16];

for(i=0;i<h_size*s_size*v_size;++i)
   matrix[i%(h_size)][i/(h_size)]=histogram[i];

ind1=0;
ind2=-1;
for(i=tablae-1;i>=0;--i)
  {
  if(tabelle[4][i]==hist_nr) {ind1=i+1; break;}
  y1=tabelle[0][i];
  x1=tabelle[1][i];
  y2=tabelle[2][i];
  x2=tabelle[3][i];
  sum=matrix[y1][x1];
  dif=matrix[y2][x2];

  matrix[y1][x1]=(sum-dif)/2;
  matrix[y2][x2]=(sum+dif)/2;
  /*printf("%5d %5d %5d %5d %5d %5d %5d\n",
  i,y1,x1,y2,x2,sum,dif);*/
  if((ind2==-1)&(tabelle[i][4]==hist_nr/2)) ind2=i; 
  }

index=0;
//printf("******************************************\n");
//printf("ind1:%5d ind2:%5d\n",ind1,ind2);
for(i=ind1;i<=ind2;++i)
  {
  y1=tabelle[0][i];
  x1=tabelle[1][i];
  y2=tabelle[2][i];
  x2=tabelle[3][i];
  //printf("%5d  %5d\n",index,matrix[y1][x1]);
  //printf("%5d  %5d\n",index+1,matrix[y2][x2]);
  index=index+2;
  }
  if(ivert==0)
	{
    index=0;
    for(i=ind1;i<=ind2;++i)
	{
    y1=tabelle[0][i];
    x1=tabelle[1][i];
    y2=tabelle[2][i];
    x2=tabelle[3][i];
    histogram[index]=matrix[y1][x1];
    histogram[index+1]=matrix[y2][x2];
    index=index+2;
	}
	}

  if(ivert!=0)
	for(i=0;i<h_size*s_size*v_size;++i)
     histogram[i]=matrix[i%(h_size)][i/(h_size)];

//for(i=0;i<h_size*s_size*v_size;++i)
  //histogram[i]= matrix[i%(h_size)][i/(h_size)];

}


void hsv_hir_quant_lin4(int *histogram)
{
int i,wert,maxwert;
for(i=0;i<256;++i)
  {
  maxwert=XM::scalableColorQuantValues[i][2];
  wert=histogram[i]-XM::scalableColorQuantValues[i][0];
//printf("i:%5d wert:%5d maxwert:%5d\n",i,wert,maxwert);
  if(wert>maxwert) wert=maxwert;
  if(wert< -maxwert) wert=-maxwert;
  histogram[i]=wert;
  }
}

void red_bits_pro_bin4(int *histogram,
int anzred,int ivert)
{
int i,j,wert,wert1,bits_pro_bin,bits_pro_bild;
int max_bits_pro_bin,anzkof;
if(anzred==0) return;

bits_pro_bild=0;
max_bits_pro_bin=0;
anzkof=0;
if(anzred>0)
   {
   for(i=0;i<256;++i)
     {
     bits_pro_bin=XM::scalableColorQuantValues[i][1]-anzred;
     if(bits_pro_bin<2)
       {
       wert=histogram[i];
       if(wert>=0) histogram[i]=0;
       if(wert<0 ) histogram[i]=1;
       bits_pro_bild=bits_pro_bild+1;
       }
     if(bits_pro_bin>=2)
       {
       wert=histogram[i];
       wert1=wert;
       if(wert<0) wert=-wert;
       bits_pro_bild=bits_pro_bild+bits_pro_bin;
       if(bits_pro_bin>max_bits_pro_bin) max_bits_pro_bin=bits_pro_bin;
       anzkof=anzkof+1;

       for(j=0;j<anzred;++j)
         wert=wert>>1;
 
       //if((wert==0)&&(wert1>=0)) histogram[i]=0; //JH: separation btw sign and value?
       //if((wert==0)&&(wert1<0)) histogram[i]=1;  //JH: separation btw sign and value?
       if(wert1<0) wert=-wert;
       histogram[i]=wert;
 
      }
   }
  } 

/*if(ivert==0)
printf("+++maxbit:%5d anzbit:%5d anzkof:%5d\n",
max_bits_pro_bin,bits_pro_bild,anzkof);*/
}




using namespace XM;

const UUID ScalableColorSearchInterface::myID = UUID("");
const char *ScalableColorSearchInterface::myName = "ScalableColorMatchingInterface";

const UUID ScalableColorSearchTool::myID = UUID("");
const char *ScalableColorSearchTool::myName = "ScalableColorMatchingTool";

const UUID ScalableColorSearchInterfaceABC::myID = UUID();

//=============================================================================
ScalableColorSearchInterfaceABC::ScalableColorSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ScalableColorSearchInterface::ScalableColorSearchInterface(ScalableColorSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ScalableColorSearchInterface::~ScalableColorSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ScalableColorSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ScalableColorSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int ScalableColorSearchInterface::
SetRefDescriptorInterface(ScalableColorDescriptorInterfaceABC
		       *RefScalableColorDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefScalableColorDescriptorInterface);
}

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC* ScalableColorSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int ScalableColorSearchInterface::
SetQueryDescriptorInterface(ScalableColorDescriptorInterfaceABC
		       *QueryScalableColorDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryScalableColorDescriptorInterface);
}

//----------------------------------------------------------------------------
double ScalableColorSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
ScalableColorSearchTool::ScalableColorSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ScalableColorSearchTool::
ScalableColorSearchTool(ScalableColorDescriptorInterfaceABC
			 *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  ScalableColorDescriptor *descriptor =
	    new ScalableColorDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	/* no sub matching tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ScalableColorSearchTool::~ScalableColorSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ScalableColorSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ScalableColorSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ScalableColorSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ScalableColorSearchTool::
SetRefDescriptorInterface(ScalableColorDescriptorInterfaceABC
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
ScalableColorDescriptorInterfaceABC* ScalableColorSearchTool::
GetQueryDescriptorInterface(void)

{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  ScalableColorSearchTool::
SetQueryDescriptorInterface(ScalableColorDescriptorInterfaceABC
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
double ScalableColorSearchTool::GetDistance(void)

{
  int h_size,s_size,v_size,*query_hist,*targ_hist,max_color=256;
  int RecHistogram=0;
  double distance,sqrerr,preverr;

  int j,tablae=255,hist_nr=256,anzred=0;


  static int tabelle[5][255]={
    {0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,
     4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,
     0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,
     4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,
     0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,0,4,8,12,0,4,8,12,0,4,8,12,0,4,8,12,0,4,
     8,12,0,4,8,12,0,4,8,12,0,8,0},

    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,
     2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,
     5,5,5,5,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,
     8,8,8,8,8,8,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,
     11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,13,13,13,13,13,13,
     13,13,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,0,0,0,0,
     0,0,0,0,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,6,6,
     6,6,6,6,6,6,8,8,8,8,8,8,8,8,10,10,10,10,10,10,10,10,
     12,12,12,12,12,12,12,12,14,14,14,14,14,14,14,14,0,0,0,0,0,0,
     0,0,2,2,2,2,2,2,2,2,8,8,8,8,8,8,8,8,10,10,10,10,
     10,10,10,10,0,0,0,0,2,2,2,2,8,8,8,8,10,10,10,10,0,0,
     0,0,8,8,8,8,0,0,0,0,0,0,0},

    {1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,
     13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,
     9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,
     5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,
     1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,
     13,15,1,3,5,7,9,11,13,15,1,3,5,7,9,11,13,15,0,2,4,6,
     8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,
     4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,
     0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,
     12,14,0,2,4,6,8,10,12,14,0,2,4,6,8,10,12,14,0,2,4,6,
     8,10,12,14,2,6,10,14,2,6,10,14,2,6,10,14,2,6,10,14,0,4,
     8,12,0,4,8,12,0,4,8,12,4,12,8},

    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,
     2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,
     5,5,5,5,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,
     8,8,8,8,8,8,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,
     11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,13,13,13,13,13,13,
     13,13,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,1,1,1,1,
     1,1,1,1,3,3,3,3,3,3,3,3,5,5,5,5,5,5,5,5,7,7,
     7,7,7,7,7,7,9,9,9,9,9,9,9,9,11,11,11,11,11,11,11,11,
     13,13,13,13,13,13,13,13,15,15,15,15,15,15,15,15,4,4,4,4,4,4,
     4,4,6,6,6,6,6,6,6,6,12,12,12,12,12,12,12,12,14,14,14,14,
     14,14,14,14,0,0,0,0,2,2,2,2,8,8,8,8,10,10,10,10,2,2,
     2,2,10,10,10,10,8,8,8,8,0,0,0},

    {128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
     128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,64,64,64,64,
     64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
     64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
     64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,32,32,32,32,32,32,
     32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
     32,32,32,32,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,8,8,
     8,8,8,8,8,8,4,4,4,4,2,2,1}
  };

  static int sorttab[256]= {
    0, 4, 8, 12, 32, 36, 40, 44, 128, 132, 136, 140, 160, 164, 168, 172,
    2, 6, 10, 14, 34, 38, 42, 46, 130, 134, 138, 142, 162, 166, 170, 174,
    64, 66, 68, 70, 72, 74, 76, 78, 96, 98, 100, 102, 104, 106, 108, 110, 192, 194, 196, 198, 200, 202, 204, 206, 224, 226, 228, 230, 232, 234, 236, 238,
    16, 18, 20, 22, 24, 26, 28, 30, 48, 50, 52, 54, 56, 58, 60, 62, 80, 82, 84, 86, 88, 90, 92, 94, 112, 114, 116, 118, 120, 122, 124, 126, 144, 146, 148, 150, 152, 154, 156, 158, 176, 178, 180, 182, 184, 186, 188, 190, 208, 210, 212, 214, 216, 218, 220, 222, 240, 242, 244, 246, 248, 250, 252, 254,
    1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143, 145, 147, 149, 151, 153, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255};

  h_size=16;
  s_size=4;
  v_size=4;
  hist_nr=atoi(getpara("NumberOfCoefficients","256"));
  anzred=atoi(getpara("NumberOfBitplanesDiscarded","0"));
  //RecHistogram=atoi(getpara("RecHistogram",0));
  RecHistogram=0;
  //printf("+++++hist_nr:%5d rec:%5d anzred:%5d\n",
  //hist_nr,RecHistogram,anzred);

  m_QueryDescriptorInterface->
    SetNumberOfCoefficients(hist_nr);

  query_hist=(int *)calloc(max_color, sizeof(int));
  targ_hist=(int *)calloc(max_color, sizeof(int));

  //printf("vor check for descriptors\n");
  /* check for descriptors*/
  if (!m_RefDescriptorInterface) return DBL_MAX;
  if (!m_QueryDescriptorInterface) return DBL_MAX;
  //printf("nach check for descriptors\n");

  if (strcmp(m_RefDescriptorInterface->GetName(),
	     "ScalableColorDescriptionInterface") != 0) {
    return DBL_MAX;
  }
  if (strcmp(m_QueryDescriptorInterface->GetName(),
	     "ScalableColorDescriptionInterface") != 0) {
    return DBL_MAX;
  }

  /*printf("query:%5d ref:%5d\n",m_QueryDescriptorInterface->
    GetNumberOfCoefficients(), 
    m_RefDescriptorInterface->
    GetNumberOfCoefficients());*/ 

  /* check if histogramms have same size*/
  if (m_RefDescriptorInterface->GetNumberOfCoefficients() !=
      m_QueryDescriptorInterface->GetNumberOfCoefficients()) {
    fprintf(stderr,"*************** ERROR *****************\n");
    fprintf(stderr,"NumberOfCoefficients(Query):%5ld\n",
	    m_QueryDescriptorInterface->
	      GetNumberOfCoefficients());

    fprintf(stderr,"NumberOfCoefficients(Ref):  %5ld\n",
	    m_RefDescriptorInterface->
	    GetNumberOfCoefficients());
    return DBL_MAX;
  }

  /* check if histogramms have NumberOfBitplanesDiscarded*/
  if (m_RefDescriptorInterface->GetNumberOfBitplanesDiscarded() !=
      m_QueryDescriptorInterface->GetNumberOfBitplanesDiscarded()) {
    fprintf(stderr,"*************** ERROR *****************\n");
    fprintf(stderr,"NumberOfBitplanesDiscarded(Query):%5ld\n",
	    m_QueryDescriptorInterface->
	    GetNumberOfBitplanesDiscarded());

    fprintf(stderr,"NumberOfBitplanesDiscarded(Ref):  %5ld\n",
	    m_RefDescriptorInterface->
	    GetNumberOfBitplanesDiscarded());
    return DBL_MAX;
  }

  /*fprintf(stderr,"xtrct: %p (%ld)/t",
    m_DescriptorInterface()->
    GetScalableHistogram(),
    m_DescriptorInterface()->
    GetScalableHistogramSize());*/



  /* perform matching*/
  sqrerr=0;
  preverr=0;


  for (unsigned int i=0; i< m_QueryDescriptorInterface->
	 GetNumberOfCoefficients(); i++) {
    targ_hist[i]=m_RefDescriptorInterface->
      GetCoefficient(i);

    query_hist[i]=m_QueryDescriptorInterface->
      GetCoefficient(i);
    //printf("i:%5d query:%5d targ:%5d\n",i,query_hist[i],targ_hist[i]);
  }

  if(RecHistogram==0) {
    for(j=0;j<hist_nr;++j) {
      distance=(double)query_hist[j]-(double)targ_hist[j];
      if(distance<0.0) distance=-distance;
      sqrerr += distance;
    }
  }

  if(RecHistogram==1) {
    int  local_query_hist[256];
    int  local_targ_hist[256];
    //reorder from storage order to coefficient index
    for(j=0;j<256;j++){
      local_query_hist[sorttab[j]]=query_hist[j];
      local_targ_hist[sorttab[j]]=targ_hist[j];
    }
    for(j=0;j<256;++j) {
      query_hist[j]=local_query_hist[j]+XM::scalableColorQuantValues[j][0];
      targ_hist[j]=local_targ_hist[j]+XM::scalableColorQuantValues[j][0];
    }
    histo_3d_inversion4(tabelle,tablae,query_hist,h_size,s_size,v_size,hist_nr,0);
    histo_3d_inversion4(tabelle,tablae,targ_hist,h_size,s_size,v_size,hist_nr,0);

    for(j=0;j<hist_nr;++j) {
      distance=(double)query_hist[j]-(double)targ_hist[j];
      if(distance<0.0) distance=-distance;
      sqrerr += distance;
    }
  }

  if(RecHistogram==2) {
    red_bits_pro_bin4(query_hist,anzred,0);
    red_bits_pro_bin4(targ_hist,anzred,0);

    //get_sort_differences(tabelle,tablae,query_hist,
    //h_size,s_size,v_size,hist_nr);

    //get_sort_differences(tabelle,tablae,targ_hist,
    //h_size,s_size,v_size,hist_nr);

    for(j=0;j<hist_nr;++j) {
      distance=(double)query_hist[j]-(double)targ_hist[j];
      if(distance<0.0) distance=-distance;
      sqrerr += distance;
    }
  }

  free(query_hist);
  free(targ_hist);
  return sqrerr;
}



//----------------------------------------------------------------------------
ScalableColorSearchInterfaceABC *ScalableColorSearchTool::GetInterface(void)
{ return &m_Interface; }

