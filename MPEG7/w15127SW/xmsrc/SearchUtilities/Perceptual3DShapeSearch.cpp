//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
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
// Applicable File Name:  Perceptual3DShapeSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#pragma warning(disable : 4786)

#include "Descriptors/Descriptors.h"
#include "SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID Perceptual3DShapeSearchInterface::myID = UUID("");
const char *Perceptual3DShapeSearchInterface::myName = "Perceptual3DShapeMatchingInterface";

const UUID Perceptual3DShapeSearchTool::myID = UUID("");
const char *Perceptual3DShapeSearchTool::myName = "Perceptual3DShapeMatchingTool";

const UUID Perceptual3DShapeSearchInterfaceABC::myID = UUID();

//=============================================================================
Perceptual3DShapeSearchInterfaceABC::Perceptual3DShapeSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
Perceptual3DShapeSearchInterface::Perceptual3DShapeSearchInterface(Perceptual3DShapeSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
Perceptual3DShapeSearchInterface::~Perceptual3DShapeSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void Perceptual3DShapeSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int Perceptual3DShapeSearchInterface::
SetRefDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *RefPerceptual3DShapeDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefPerceptual3DShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC* Perceptual3DShapeSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int Perceptual3DShapeSearchInterface::
SetQueryDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *QueryPerceptual3DShapeDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryPerceptual3DShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
double Perceptual3DShapeSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
Perceptual3DShapeSearchTool::Perceptual3DShapeSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
Perceptual3DShapeSearchTool::
Perceptual3DShapeSearchTool(Perceptual3DShapeDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect Perceptual3DShape Searching Tool\n");
#endif
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    Perceptual3DShapeDescriptor *descriptor = new Perceptual3DShapeDescriptor;
    aQueryDescriptorInterface=descriptor->GetInterface();
  }

  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Perceptual3DShapeSearchTool::~Perceptual3DShapeSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool Perceptual3DShapeSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool Perceptual3DShapeSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int Perceptual3DShapeSearchTool::
SetRefDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *aRefPerceptual3DShapeDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefPerceptual3DShapeDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefPerceptual3DShapeDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC* Perceptual3DShapeSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  Perceptual3DShapeSearchTool::
SetQueryDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *aQueryPerceptual3DShapeDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryPerceptual3DShapeDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryPerceptual3DShapeDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

#include "Media/Media.h"
#include "Descriptors/Perceptual3DShape_Aux.h"

//----------------------------------------------------------------------------
double Perceptual3DShapeSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "Perceptual3DShapeDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "Perceptual3DShapeDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------
//convert the Ref and Query DescriptorInterface into CGraph object.
	CGraph *pModelGraph = new CGraph(m_RefDescriptorInterface);
	CGraph *pQueryGraph = new CGraph(m_QueryDescriptorInterface);

	double dist = pQueryGraph->MatchFastEMDApart(pModelGraph,NULL,NULL,1.0,0.5);
  //fprintf(stderr,"matching result: %f\n", dist);

	if(pQueryGraph!=NULL) delete pQueryGraph; 
	if(pModelGraph!=NULL) delete pModelGraph;

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
Perceptual3DShapeSearchInterfaceABC *Perceptual3DShapeSearchTool::GetInterface(void)
{ return &m_Interface; }

