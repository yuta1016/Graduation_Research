//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshio Kamei, NEC Corporation.
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
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  AdvancedFaceRecognitionSearch.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"

//=============================================================================

using namespace XM;

const UUID AdvancedFaceRecognitionSearchInterface::myID = UUID("");
const char *AdvancedFaceRecognitionSearchInterface::myName = "AdvancedFaceRecognitionMatchingInterface";

const UUID AdvancedFaceRecognitionSearchTool::myID = UUID("");
const char *AdvancedFaceRecognitionSearchTool::myName = "AdvancedFaceRecognitionMatchingTool";

const UUID AdvancedFaceRecognitionSearchInterfaceABC::myID = UUID();

#include "SearchUtilities/AdvancedFaceRecognition/FourierFeatureWeight.h"
#include "SearchUtilities/AdvancedFaceRecognition/CentralFourierFeatureWeight.h"
#include "SearchUtilities/AdvancedFaceRecognition/CompositeFeatureWeight.h"
#include "SearchUtilities/AdvancedFaceRecognition/CentralCompositeFeatureWeight.h"

//=============================================================================
AdvancedFaceRecognitionSearchInterfaceABC::AdvancedFaceRecognitionSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
AdvancedFaceRecognitionSearchInterface::AdvancedFaceRecognitionSearchInterface(AdvancedFaceRecognitionSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionSearchInterface::~AdvancedFaceRecognitionSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionSearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionSearchInterface::destroy(void)
{ delete m_SearchTool; }


//----------------------------------------------------------------------------
int AdvancedFaceRecognitionSearchInterface::
SetRefDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
		       *RefAdvancedFaceRecognitionDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefAdvancedFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC* AdvancedFaceRecognitionSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionSearchInterface::
SetQueryDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
		       *QueryAdvancedFaceRecognitionDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryAdvancedFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
double AdvancedFaceRecognitionSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}


//=============================================================================
AdvancedFaceRecognitionSearchTool::AdvancedFaceRecognitionSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionSearchTool::
AdvancedFaceRecognitionSearchTool
(AdvancedFaceRecognitionDescriptorInterfaceABC *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
  
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    AdvancedFaceRecognitionDescriptor *descriptor =
      new AdvancedFaceRecognitionDescriptor();
    aQueryDescriptorInterface=descriptor->GetInterface();
  }
  
  /* connect descritor with search tool */
  SetQueryDescriptorInterface(aQueryDescriptorInterface);
  
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionSearchTool::~AdvancedFaceRecognitionSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionSearchTool::
SetRefDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
			  *aRefDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefDescriptorInterface) return 0;
	
  /* release old Interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new Interface*/
  m_RefDescriptorInterface = 
    aRefDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();
    
    /* connect sub descriptors with sub search tools*/
    /* not available*/
  }
  else {
    /* if no descriptor, release also descriptor Interfaces from
       sub search tools*/
    /* not available*/
    return -1;
  }
  
  return 0;
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC*
AdvancedFaceRecognitionSearchTool::GetQueryDescriptorInterface(void)
{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  AdvancedFaceRecognitionSearchTool::
SetQueryDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
			    *aQueryDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryDescriptorInterface) return 0;
	
  /* release old Interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new Interface*/
  m_QueryDescriptorInterface = 
    aQueryDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();
    
    /* conect sub descriptors with sub search tools*/
    /* not available*/
  }
  else {
    /* if no descriptor, release also descriptor Interfaces from
       sub search tools*/
    /* not available*/
    return -1;
  }
  
  return 0;
}

//----------------------------------------------------------------------------
double AdvancedFaceRecognitionSearchTool::GetDistance()
{

  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "AdvancedFaceRecognitionDescriptorInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "AdvancedFaceRecognitionDescriptorInterface") != 0) return(DBL_MAX);

  AdvancedFRD	*pLocal_Face_Ref;
  AdvancedFRD	*pLocal_Face_Query;
  int iw;
  long dl;
  int hfdim,cfdim,hcdim,ccdim;
  double	dist;
#define FACE_PARAM_FILE
#ifdef   FACE_PARAM_FILE
  static int face_param_flag = 0;
  char FaceParamFile[1024];
#endif

  hfdim = atoi(getpara("numOfFourierFeatureSearch","24"));
  cfdim = atoi(getpara("numOfCentralFourierFeatureSearch","24"));
  hcdim = atoi(getpara("numOfCompositeFeatureSearch","0"));
  ccdim = atoi(getpara("numOfCentralCompositeFeatureSearch","0"));

  pLocal_Face_Ref   = m_RefDescriptorInterface->GetAdvancedFaceRecognition();
  pLocal_Face_Query = m_QueryDescriptorInterface->GetAdvancedFaceRecognition();

  /* check parameters */
  if ( pLocal_Face_Ref->numOfFourierFeature < hfdim )
    fprintf(stderr,"error: pLocal_Face_Ref->numOfFourierFeature(%d) < hfdim(%d)\n",
	    pLocal_Face_Ref->numOfFourierFeature,hfdim);

  if ( pLocal_Face_Query->numOfFourierFeature < hfdim )
    fprintf(stderr,"error: pLocal_Face_Query->numOfFourierFeature(%d) < hfdim(%d)\n",
	    pLocal_Face_Query->numOfFourierFeature,hfdim);

  if ( pLocal_Face_Ref->numOfCentralFourierFeature < cfdim )
    fprintf(stderr,"error: pLocal_Face_Ref->numOfCentralFourierFeature(%d) < cfdim(%d)\n",
	    pLocal_Face_Ref->numOfCentralFourierFeature,cfdim);
  
  if ( pLocal_Face_Query->numOfCentralFourierFeature < cfdim )
    fprintf(stderr,"error: pLocal_Face_Query->numOfCentralFourierFeature(%d) < cfdim(%d)\n",
	    pLocal_Face_Query->numOfCentralFourierFeature,cfdim);

  /* check parameters ( for extended part ) */
  if ( pLocal_Face_Ref->extensionFlag == 1 ){
    if ( pLocal_Face_Ref->numOfCompositeFeature < hcdim )
      fprintf(stderr,"error: pLocal_Face_Ref->numOfCompositeFeature(%d) < hcdim(%d)\n",
	      pLocal_Face_Ref->numOfCompositeFeature,hcdim);

    if ( pLocal_Face_Ref->numOfCentralCompositeFeature < ccdim )
      fprintf(stderr,"error: pLocal_Face_Ref->numOfCentralCompositeFeature(%d) < ccdim(%d)\n",
	      pLocal_Face_Ref->numOfCentralCompositeFeature,ccdim);
  }
  else{
    if ( hcdim > 0 )
      fprintf(stderr,"error: pLocal_Face_Ref->extensionFlag(%d) ,  hcdim(%d)\n",
	      pLocal_Face_Ref->extensionFlag,hcdim);
    if ( ccdim > 0 )
      fprintf(stderr,"error: pLocal_Face_Ref->extensionFlag(%d) ,  ccdim(%d)\n",
	      pLocal_Face_Ref->extensionFlag,ccdim);
  }
  
  if ( pLocal_Face_Query->extensionFlag == 1 ){
    if ( pLocal_Face_Query->numOfCompositeFeature < hcdim )
      fprintf(stderr,"error: pLocal_Face_Query->numOfCompositeFeature(%d) < hcdim(%d)\n",
	      pLocal_Face_Query->numOfCompositeFeature,hcdim);

    if ( pLocal_Face_Query->numOfCentralCompositeFeature < ccdim )
      fprintf(stderr,"error: pLocal_Face_Query->numOfCentralCompositeFeature(%d) < ccdim(%d)\n",
	      pLocal_Face_Query->numOfCentralCompositeFeature,ccdim);
  }
  else{
    if ( hcdim > 0 )
      fprintf(stderr,"error: pLocal_Face_Query->extensionFlag(%d) ,  hcdim(%d)\n",
	      pLocal_Face_Query->extensionFlag,hcdim);
    if ( ccdim > 0 )
      fprintf(stderr,"error: pLocal_Face_Query->extensionFlag(%d) ,  ccdim(%d)\n",
	      pLocal_Face_Query->extensionFlag,ccdim);
  }

#ifdef FACE_PARAM_FILE
  /* set weighting parameters from file */
  if (( testpara("FaceParamFile") == 1 )&&(face_param_flag == 0 )){
    mf_read_FaceParamFile(getpara("FaceParamFile","AdvancedFaceRecognition.prm"));
    face_param_flag = 1;
  }
#endif
  
  dist = 0.0;
  
  for( iw = 0 ; iw < hfdim ; iw++ ){
    dl = pLocal_Face_Ref->FourierFeature[iw] 
      - pLocal_Face_Query->FourierFeature[iw];
    dl *= dl;
    dist += (double)(FourierFeatureWeight[iw]*dl);
  }
  
  for( iw = 0 ; iw < cfdim; iw++ ){
    dl = pLocal_Face_Ref->CentralFourierFeature[iw] 
      - pLocal_Face_Query->CentralFourierFeature[iw];
    dl *= dl;
    dist += (double)(CentralFourierFeatureWeight[iw]*dl);
  }

  for( iw = 0 ; iw < hcdim; iw++ ){
    dl = pLocal_Face_Ref->CompositeFeature[iw] 
      - pLocal_Face_Query->CompositeFeature[iw];
    dl *= dl;
    dist += (double)(CompositeFeatureWeight[iw]*dl);
  }
  
  for( iw = 0 ; iw < ccdim; iw++ ){
    dl = pLocal_Face_Ref->CentralCompositeFeature[iw] 
      - pLocal_Face_Query->CentralCompositeFeature[iw];
    dl *= dl;
    dist += (double)(CentralCompositeFeatureWeight[iw]*dl);
  }
  
  // All done
  return(dist);
}


//-----------------------------------------------------------------------
AdvancedFaceRecognitionSearchInterfaceABC *AdvancedFaceRecognitionSearchTool::GetInterface(void)
{ return &m_Interface; }

#ifdef FACE_PARAM_FILE
int AdvancedFaceRecognitionSearchTool::mf_read_param1D( FILE *fp, void *buffer )
{
  int nitem;
  int size;
  fread(&nitem,sizeof(int),1,fp);
  fread(&size,sizeof(int),1,fp);
  fread(buffer,size,nitem,fp);

  return 0;
}

int AdvancedFaceRecognitionSearchTool::mf_read_param2D( FILE *fp, void *buffer )
{
  int dy,dx;
  int size;

  fread(&dy,sizeof(int),1,fp);
  fread(&dx,sizeof(int),1,fp);
  fread(&size,sizeof(int),1,fp);
  fread(buffer,size,dx*dy,fp);

  return 0;
}

int AdvancedFaceRecognitionSearchTool::mf_read_FaceParamFile( char * path )
{
  FILE *fp;
  double dummy[200000];
  /* fprintf(stderr,"mf_read_FaceParamFile(): %s\n",path); */
  fp = fopen(path,"r");
  if ( fp == NULL ){
    fprintf(stderr,"error: fopen(\"%s\",\"r\")\n",path);
    exit(1);
  }
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);

  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);

  mf_read_param1D(fp,(void *)FourierFeatureWeight);
  mf_read_param1D(fp,(void *)CentralFourierFeatureWeight);

  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);  mf_read_param1D(fp,(void *)dummy);

  mf_read_param2D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  
  
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  mf_read_param2D(fp,(void *)dummy);
  
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  
  mf_read_param2D(fp,(void *)dummy);
  
  mf_read_param1D(fp,(void *)CompositeFeatureWeight);
  mf_read_param1D(fp,(void *)CentralCompositeFeatureWeight);
  
  fclose(fp);

  return 0;
}

#endif


