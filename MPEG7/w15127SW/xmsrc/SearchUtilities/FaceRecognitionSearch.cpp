//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Applicable File Name:  FaceRecognitionSearch.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>

#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"


//=============================================================================
	

using namespace XM;


const UUID FaceRecognitionSearchInterface::myID = UUID("");
const char *FaceRecognitionSearchInterface::myName = "FaceRecognition Matching Interface";

const UUID FaceRecognitionSearchTool::myID = UUID("");
const char *FaceRecognitionSearchTool::myName = "FaceRecognition Matching Tool";

const UUID FaceRecognitionSearchInterfaceABC::myID = UUID();

//=============================================================================
FaceRecognitionSearchInterfaceABC::FaceRecognitionSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
FaceRecognitionSearchInterface::FaceRecognitionSearchInterface(FaceRecognitionSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
FaceRecognitionSearchInterface::~FaceRecognitionSearchInterface()
{
}

//----------------------------------------------------------------------------
void FaceRecognitionSearchInterface::destroy(void)
{ delete m_SearchTool; }


//----------------------------------------------------------------------------
int FaceRecognitionSearchInterface::
SetRefDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
		       *RefFaceRecognitionDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterfaceABC* FaceRecognitionSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int FaceRecognitionSearchInterface::
SetQueryDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
		       *QueryFaceRecognitionDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
double FaceRecognitionSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
FaceRecognitionSearchTool::FaceRecognitionSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
	int i;

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int qstep[48];
	double weight1[48];
	for (i=0; i<48; i++)
		qstep[i]=8;
	for (i=0; i<16; i++)
		weight1[i] = qstep[i];
	for (i=16;i<24; i++)
		weight1[i] = 2*qstep[i];
	for (i=24;i<48;i++)
		weight1[i] = 4*qstep[i];
	//weight1[48] = 0;

	/*
	int qstep[49];
	double weight1[49];
	//mark's new proposal
	for (i=0; i<49; i++)
		qstep[i]=8;
	for (i=0; i<16; i++)
		weight1[i] = qstep[i];
	for (i=16;i<24; i++)
		weight1[i] = 2*qstep[i];
	for (i=24;i<49;i++)
		weight1[i] = 4*qstep[i];
	weight1[48] = 0;
	*/

	/*
	for (i=0;i<5;i++)
		qstep[i]=4;
	for (i=5;i<17;i++)
		qstep[i]=4;
	for (i=17;i<25;i++)
		qstep[i]=8;
	for (i=25;i<49;i++)
		qstep[i]=16;
	for (i=0;i<5;i++)
		weight1[i]=(28-i*2)*qstep[i]*qstep[i];
	for (i=2;i<3;i++)
		weight1[i]=11*qstep[i]*qstep[i];
	for (i=11;i<49;i++)
		weight1[i]=1.05*qstep[i]*qstep[i];
	for (i=5;i<11;i++)
		weight1[i]=3.5*qstep[i]*qstep[i];
	for (i=3;i<4;i++)
		weight1[i]=5*qstep[i]*qstep[i];
	*/
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
FaceRecognitionSearchTool::
FaceRecognitionSearchTool(FaceRecognitionDescriptorInterfaceABC 
				 *aQueryDescriptorInterface):
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0)
{
	int i;

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int qstep[48];
	double weight1[48];
	for (i=0; i<48; i++)
		qstep[i]=8;
	for (i=0; i<16; i++)
		weight1[i] = qstep[i];
	for (i=16;i<24; i++)
		weight1[i] = 2*qstep[i];
	for (i=24;i<48;i++)
		weight1[i] = 4*qstep[i];
	//weight1[48] = 0;

/*	int qstep[49];
	double weight1[49];
	//mark's new proposal
	for (i=0; i<49; i++)
		qstep[i]=8;
	for (i=0; i<16; i++)
		weight1[i] = qstep[i];
	for (i=16;i<24; i++)
		weight1[i] = 2*qstep[i];
	for (i=24;i<49;i++)
		weight1[i] = 4*qstep[i];
	weight1[48] = 0;
	*/

	/*
	for (i=0;i<5;i++)
		qstep[i]=4;
	for (i=5;i<17;i++)
		qstep[i]=4;
	for (i=17;i<25;i++)
		qstep[i]=8;
	for (i=25;i<49;i++)
		qstep[i]=16;
	for (i=0;i<5;i++)
		weight1[i]=(28-i*2)*qstep[i]*qstep[i];
	for (i=2;i<3;i++)
		weight1[i]=11*qstep[i]*qstep[i];
	for (i=11;i<49;i++)
		weight1[i]=1.05*qstep[i]*qstep[i];
	for (i=5;i<11;i++)
		weight1[i]=3.5*qstep[i]*qstep[i];
	for (i=3;i<4;i++)
		weight1[i]=5*qstep[i]*qstep[i];
	*/
	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  FaceRecognitionDescriptor *descriptor =
	    new FaceRecognitionDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect descritor with search tool */
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
FaceRecognitionSearchTool::~FaceRecognitionSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
int FaceRecognitionSearchTool::
SetRefDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
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
FaceRecognitionDescriptorInterfaceABC*
FaceRecognitionSearchTool::GetQueryDescriptorInterface(void)
{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  FaceRecognitionSearchTool::
SetQueryDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
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
double FaceRecognitionSearchTool::GetDistance()
{

	if (!m_RefDescriptorInterface) return (double)-1;
	if (!m_QueryDescriptorInterface) return (double)-1;
  	if (strcmp(m_RefDescriptorInterface->GetName(),
		"FaceRecognition Descriptor Interface") != 0) {
		return (double)-1;
  	}
  	if (strcmp(m_QueryDescriptorInterface->GetName(),
		"FaceRecognition Descriptor Interface") != 0) {
		return (double)-1;
  	}

	FRD	*pLocal_Face_Ref;
	FRD	*pLocal_Face_Query;
	int i;
	
  	double	dist;

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int qstep[48];
	double weight1[48];
	for (i=0; i<48; i++)
		qstep[i]=8;
	for (i=0; i<16; i++)
		weight1[i] = qstep[i];
	for (i=16;i<24; i++)
		weight1[i] = 2*qstep[i];
	for (i=24;i<48;i++)
		weight1[i] = 4*qstep[i];
	//weight1[48] = 0;
	
	/*
	//mark's new proposal
	for (i=0; i<49; i++)
		qstep[i]=8;
	for (i=0; i<16; i++)
		weight1[i] = qstep[i];
	for (i=16;i<24; i++)
		weight1[i] = 2*qstep[i];
	for (i=24;i<49;i++)
		weight1[i] = 4*qstep[i];
	weight1[48] = 0;
	*/
	/*
	for (i=0;i<5;i++)
		qstep[i]=4;
	for (i=5;i<17;i++)
		qstep[i]=4;
	for (i=17;i<25;i++)
		qstep[i]=8;
	for (i=25;i<49;i++)
		qstep[i]=16;
	for (i=0;i<5;i++)
		weight1[i]=(28-i*2)*qstep[i]*qstep[i];
	for (i=2;i<3;i++)
		weight1[i]=11*qstep[i]*qstep[i];
	for (i=11;i<49;i++)
		weight1[i]=1.05*qstep[i]*qstep[i];
	for (i=5;i<11;i++)
		weight1[i]=3.5*qstep[i]*qstep[i];
	for (i=3;i<4;i++)
		weight1[i]=5*qstep[i]*qstep[i];
	*/
//	for (i=0;i<5;i++)
//		weight1[i]=0;
	
	pLocal_Face_Ref=m_RefDescriptorInterface->GetFaceRecognition();
	pLocal_Face_Query=m_QueryDescriptorInterface->GetFaceRecognition();
	
	dist = 0.0;
	
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	for( i=0; i < 48; i++){
//	for( i=0; i < 49; i++){
	  // Global(5)+Semi_Global(65) 
	  dist += weight1[i]*(pLocal_Face_Ref->eigenfeature[i] - pLocal_Face_Query->eigenfeature[i])*(pLocal_Face_Ref->eigenfeature[i] - pLocal_Face_Query->eigenfeature[i]);
	}
	return 	dist;
}


//----------------------------------------------------------------------------
FaceRecognitionSearchInterfaceABC *FaceRecognitionSearchTool::GetInterface(void)
{ return &m_Interface; }

