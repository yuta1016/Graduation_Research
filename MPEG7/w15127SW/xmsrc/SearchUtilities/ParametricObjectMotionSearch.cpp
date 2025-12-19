//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - usage of parameter file to set search parameters
//   definition of new menber function SetValuesFromParameterFile()
// - bugfixes: initialization, memory management
// - update according to LaBaule CD-text (coordinates, duration)
// - normalization of translation to frame size for parameter-based search
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
// Applicable File Name:  ParametricObjectMotionSearch.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "momusys.h"
#include "ExtractionUtilities/gme_for_iso.h"
#include "ParametricObjectMotionUtilities.h"
#include "ExtractionUtilities/imgutil.h"
#include "Utilities/readparam.h"		// Aljoscha 12/13/00
#define Td2 255


//=============================================================================
	

using namespace XM;


const UUID ParametricObjectMotionSearchInterface::myID = UUID("");
const char *ParametricObjectMotionSearchInterface::myName = "Parametric Object Motion Matching Interface";

const UUID ParametricObjectMotionSearchTool::myID = UUID("");
const char *ParametricObjectMotionSearchTool::myName = "Parametric Object Motion Matching Tool";

const UUID ParametricObjectMotionSearchInterfaceABC::myID = UUID();

//=============================================================================
ParametricObjectMotionSearchInterfaceABC::ParametricObjectMotionSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ParametricObjectMotionSearchInterface::ParametricObjectMotionSearchInterface(ParametricObjectMotionSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ParametricObjectMotionSearchInterface::~ParametricObjectMotionSearchInterface()
{
}

//----------------------------------------------------------------------------
void ParametricObjectMotionSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int ParametricObjectMotionSearchInterface::
SetRefDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *RefParametricObjectMotionDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefParametricObjectMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC*
ParametricObjectMotionSearchInterface::GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int ParametricObjectMotionSearchInterface::
SetQueryDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *QueryParametricObjectMotionDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryParametricObjectMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
void ParametricObjectMotionSearchInterface::// Aljoscha 12/13/00 start
SetValuesFromParameterFile(void)
{
  m_SearchTool->SetQueryMethod(atoi(getpara("QueryMethod","0")));
  /* default = 0 = velocity based*/

  m_SearchTool->SetQueryNorm(atoi(getpara("QueryNorm","0")));
  /* default = 0*/

  m_SearchTool->SetSquareDim(atoi(getpara("SquareDim","20")));
  /* default = 20*/


#ifdef DEBUG
  fprintf(stderr,"Using: Query Method %d\n",(int)
	  m_SearchTool->
	  GetQueryMethod());
  fprintf(stderr,"Using: Query Norm %d\n",(int)
	  m_SearchTool->
	  GetQueryNorm());
  fprintf(stderr,"Using: Square Dim %d\n",(int)
	  m_SearchTool->
	  GetSquareDim());
#endif
}	      // Aljoscha 12/13/00 end

//----------------------------------------------------------------------------
double ParametricObjectMotionSearchInterface::GetDistance(MomVop *rvop,MomVop *qvop,int qm,int qn,int sd)

{
  return m_SearchTool->GetDistance(rvop,qvop,qm,qn,sd);
}


//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionSearchInterface::SetQueryMethod(int queryMeth){
  return m_SearchTool->SetQueryMethod(queryMeth);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionSearchInterface::GetQueryMethod(void){
return  m_SearchTool->GetQueryMethod();
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionSearchInterface::SetQueryNorm(int queryNorm){
 return m_SearchTool->SetQueryNorm(queryNorm);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionSearchInterface::GetQueryNorm(void){
return  m_SearchTool->GetQueryNorm();
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionSearchInterface::SetSquareDim(int squareDim){
 return m_SearchTool->SetSquareDim(squareDim);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionSearchInterface::GetSquareDim(void){
return  m_SearchTool->GetSquareDim();
}


//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
ParametricObjectMotionSearchTool::ParametricObjectMotionSearchTool():
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0),
  m_QueryMethod(0),
  m_QueryNorm(0),
  m_SquareDim(20)		// Aljoscha 12/13/00, initialize
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionSearchTool::
ParametricObjectMotionSearchTool(ParametricObjectMotionDescriptorInterfaceABC 
				 *aQueryDescriptorInterface):
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0),
  m_QueryMethod(0),
  m_QueryNorm(0),
  m_SquareDim(20)		// Aljoscha 12/13/00, initialize
{
	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  ParametricObjectMotionDescriptor *descriptor =
	    new ParametricObjectMotionDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect descritor with search tool */
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionSearchTool::~ParametricObjectMotionSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------

int ParametricObjectMotionSearchTool::
SetRefDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
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
ParametricObjectMotionDescriptorInterfaceABC*
ParametricObjectMotionSearchTool::GetQueryDescriptorInterface(void)

{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  ParametricObjectMotionSearchTool::
SetQueryDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
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

	  /* conect sub descriptors with sub search tools*/
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
double ParametricObjectMotionSearchTool::GetDistance(MomVop *m_RefVop,MomVop *m_QueryVop,int QueryMethod, int QueryNorm,int squareDimension)
{

//SpatialRegion  *curr_alpha, *ref_alpha;

  int i,mCodeRef,mCodeQuery;
  int DurationRef,DurationQuery;	// Aljoscha 12/13/00
  double *paramRef,*paramQuery;
  int noofParamRef,noofParamQuery;
  double xOriginRef,yOriginRef,xOriginQuery,yOriginQuery;
  double dist;

  if (!m_RefDescriptorInterface) return (double)-1;
  if (!m_QueryDescriptorInterface) return (double)-1;
  if (strcmp(m_RefDescriptorInterface->GetName(),
	     "Parametric Object Motion Description Interface") != 0) {
    return (double)-1;
  }
  if (strcmp(m_QueryDescriptorInterface->GetName(),
	     "Parametric Object Motion Description Interface") != 0) {
    return (double)-1;
  }
	
  // Aljoscha 12/13/00 start
  if ( m_RefDescriptorInterface->GetCoordFlag() ||
       m_QueryDescriptorInterface->GetCoordFlag() )
    {
      fprintf(stderr,
	      "\nMatching only implemented for embedded coordinates\n");
      return (double)-1;
    }
  else
    {
      xOriginRef=m_RefDescriptorInterface->GetXOrigin();
      yOriginRef=m_RefDescriptorInterface->GetXOrigin();
      xOriginQuery=m_QueryDescriptorInterface->GetXOrigin();
      yOriginQuery=m_QueryDescriptorInterface->GetYOrigin();
    }

  DurationRef=m_RefDescriptorInterface->GetDuration();
  DurationQuery=m_QueryDescriptorInterface->GetDuration();
  // Aljoscha 12/13/00 end

  mCodeRef=m_RefDescriptorInterface->GetModelCode();
  mCodeQuery=m_QueryDescriptorInterface->GetModelCode();

  switch (mCodeRef){
  case Translational:
    noofParamRef=2;
    break;
  case Rotation_Scale:
    noofParamRef=4;
    break;
  case Affine:
    noofParamRef=6;
    break;
  case Perspective:
    noofParamRef=8;
    break;
  case Quadric:
    noofParamRef=12;
    break;
  default:
    return DBL_MAX;
  }

  switch (mCodeQuery){
  case Translational:
    noofParamQuery=2;
    break;
  case Rotation_Scale:
    noofParamQuery=4;
    break;
  case Affine:
    noofParamQuery=6;
    break;
  case Perspective:
    noofParamQuery=8;
    break;
  case Quadric:
    noofParamQuery=12;
    break;
  default:
    return DBL_MAX;
  }

  // Aljoscha 12/13/00
  paramRef=m_RefDescriptorInterface->GetMotionParameters();
  paramQuery=m_QueryDescriptorInterface->GetMotionParameters();
  // removed mallocs
  // pointing to nowhere  

  int h,w;
  
  switch (QueryMethod) {


  case VelocityBasedRegion:


/************this is used for AssociatedRegion
	curr_alpha=m_RefDescriptorInterface->GetAssociatedRegion();
	ref_alpha=m_RefDescriptorInterface->GetAssociatedRegion();
*******************************/

    int  ref_width,ref_height,curr_width, curr_height;

    ref_width = (GetMomVopWidth(m_RefVop));
    ref_height = (GetMomVopHeight(m_RefVop));

    curr_width = (GetMomVopWidth(m_QueryVop));
    curr_height = (GetMomVopHeight(m_QueryVop));

    h=MAX(ref_height,curr_height);
    w=MAX(ref_width,curr_width);


    double  **foSelected_vxD,**foSelected_vyD,**fo_vxD,**fo_vyD;


    foSelected_vxD = new double* [h];
    foSelected_vyD = new double* [h];
    fo_vxD = new double* [h];
    fo_vyD = new double* [h];
    for (i=0;i<h;i++){
      foSelected_vxD[i]=  new double [w];
      foSelected_vyD[i]=  new double [w];
      fo_vxD[i]=  new double [w];
      fo_vyD[i]=  new double [w];
    }

    if (foSelected_vxD==NULL) {printf("\n no mem");exit(1);}
    if (foSelected_vyD==NULL) {printf("\n no mem");exit(1);}
    if (fo_vxD==NULL) {printf("\n no mem");exit(1);}
    if (fo_vyD==NULL) {printf("\n no mem");exit(1);}


    ComputeSpeedFieldAllMask(paramRef,paramQuery,m_RefVop,m_QueryVop,
			     foSelected_vxD,foSelected_vyD,fo_vxD,fo_vyD,
			     mCodeRef,mCodeQuery,xOriginRef,yOriginRef,
			     xOriginQuery,yOriginQuery);

    dist = distFOTotal(foSelected_vxD,foSelected_vyD,fo_vxD,fo_vyD,h,w,
		       QueryNorm);


    for (i=0;i<h;i++){
      delete [] foSelected_vxD[i];
      delete [] foSelected_vyD[i];
      delete [] fo_vxD[i];
      delete [] fo_vyD[i];
    }
    delete [] foSelected_vxD;
    delete [] foSelected_vyD;
    delete [] fo_vxD;
    delete [] fo_vyD;

    break;


  case VelocityBasedLittleSquare:

    double dist_F;

    h=squareDimension;w=h;

    double **vectorx_FM1,**vectory_FM1;
    double **vectorx_FM2,**vectory_FM2;
    double xGravityRef,yGravityRef,xGravityQuery,yGravityQuery;
    vectorx_FM1 = new double* [h];
    vectory_FM1 = new double* [h];
    vectorx_FM2 = new double* [h];
    vectory_FM2 = new double* [h];
    for(i=0;i<h;++i){
      vectorx_FM1[i]=new double [w];
      vectory_FM1[i]=new double [w];
      vectorx_FM2[i]=new double [w];
      vectory_FM2[i]=new double [w];
    }

    GetXYCenter(m_RefVop,&xGravityRef,&yGravityRef);
    GetXYCenter(m_QueryVop,&xGravityQuery,&yGravityQuery);
    ComputeSpeedField(paramRef,paramQuery,
 		      vectorx_FM1, vectory_FM1,vectorx_FM2, vectory_FM2, 
		      h,w,xOriginRef,yOriginRef,xOriginQuery,yOriginQuery,
		      xGravityRef,yGravityRef,xGravityQuery,yGravityQuery,
		      mCodeRef,mCodeQuery);

    dist_F = distFOTotal(vectorx_FM1, vectory_FM1, vectorx_FM2, vectory_FM2,
			 h, w,QueryNorm);
    dist = dist_F;
    break;


  case ParameterBased:
    dist=0;
    if (noofParamRef!=noofParamQuery) {
      fprintf(stderr,"\n For this query method the motion parameters "
	      "should have the sane type\n");
      return (double)-1;// Aljoscha 12/13/00, new return value (not 0.0)
    }

    double *wight=(double*)malloc(noofParamRef*sizeof(double));
    if (wight==NULL) return (double)-1;


    for (i=0;i<noofParamRef;i++)
      wight[i]=1.0;

    // Aljoscha 12/13/00
    ref_width  = (GetMomVopWidth(m_RefVop));
    curr_width = (GetMomVopWidth(m_QueryVop));

    // Aljoscha 12/13/00, normalization of translation to frame width
    for (i=0;i<2;i++)
      dist+=wight[i]*fabs(paramRef[i]/ref_width-paramQuery[i]/curr_width);

    for (i=2;i<noofParamRef;i++)
      dist+=wight[i]*fabs(paramRef[i]-paramQuery[i]);

    free(wight);			// Aljoscha 12/13/00

    break;
  }
  return dist;
}


//----------------------------------------------------------------------------
unsigned long  ParametricObjectMotionSearchTool::SetQueryMethod(int queryMeth){
 return m_QueryMethod=queryMeth;
}

//----------------------------------------------------------------------------
int  ParametricObjectMotionSearchTool::GetQueryMethod(void){
return   m_QueryMethod;
}

//----------------------------------------------------------------------------
unsigned long  ParametricObjectMotionSearchTool::SetQueryNorm(int queryNorm){
return  m_QueryNorm=queryNorm;
}

//----------------------------------------------------------------------------
int  ParametricObjectMotionSearchTool::GetQueryNorm(void){
return  m_QueryNorm;
}

//----------------------------------------------------------------------------
unsigned long  ParametricObjectMotionSearchTool::SetSquareDim(int squareDim){
 return m_SquareDim=squareDim;
}

//----------------------------------------------------------------------------
int  ParametricObjectMotionSearchTool::GetSquareDim(void){
return  m_SquareDim;
}

//----------------------------------------------------------------------------
ParametricObjectMotionSearchInterfaceABC* ParametricObjectMotionSearchTool::GetInterface(void)
{ return &m_Interface; }
