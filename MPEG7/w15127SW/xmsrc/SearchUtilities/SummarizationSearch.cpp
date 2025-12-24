//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// Yousri Abdeljaoued (EPFL Switzerland)
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
// Applicable File Name:  SummarizationSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string>
#include "Utilities/readparam.h"
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "CodingSchemes/CodingSchemes.h"
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID SummarizationSearchInterface::myID = UUID("");
const char *SummarizationSearchInterface::myName = "Summarization Matching Interface";

const UUID SummarizationSearchTool::myID = UUID("");
const char *SummarizationSearchTool::myName = "Summarization Matching Tool";

const UUID SummarizationSearchInterfaceABC::myID = UUID();

//=============================================================================
SummarizationSearchInterfaceABC::SummarizationSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SummarizationSearchInterface::SummarizationSearchInterface(SummarizationSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SummarizationSearchInterface::~SummarizationSearchInterface()
{
}

//----------------------------------------------------------------------------
void SummarizationSearchInterface::destroy()

{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int SummarizationSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC *aDescription)

{
  return m_SearchTool->SetRefDescriptionInterface(aDescription);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC *SummarizationSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int SummarizationSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC *aDescription)

{
  return m_SearchTool->SetQueryDescriptionInterface(aDescription);
}

//----------------------------------------------------------------------------
int SummarizationSearchInterface::SetMedia(MultiMediaInterfaceABC *aMedia)

{
  return m_SearchTool->SetMedia(aMedia);
}

//----------------------------------------------------------------------------
int SummarizationSearchInterface::InitSearch(void)

{
  return m_SearchTool->InitSearch();
}

//----------------------------------------------------------------------------
int SummarizationSearchInterface::StartSearch(void)

{
  return m_SearchTool->StartSearch();
}

//----------------------------------------------------------------------------
double SummarizationSearchInterface::PostSearch(void)

{
  return m_SearchTool->PostSearch();
}

//----------------------------------------------------------------------------
double SummarizationSearchInterface::ActivityTreshold(void)

{
  return m_SearchTool->ActivityTreshold();
}

//----------------------------------------------------------------------------
const UUID& SummarizationSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SummarizationSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
SummarizationSearchTool::SummarizationSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0),
m_writeall(0),
m_ActivityThreshold(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SummarizationSearchTool::
SummarizationSearchTool(GenericDSInterfaceABC *aDescription):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0),
m_writeall(0),
m_ActivityThreshold(0)
{
  SetQueryDescriptionInterface(aDescription);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SummarizationSearchTool::~SummarizationSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& SummarizationSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SummarizationSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SummarizationSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SummarizationSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SummarizationSearchTool::SetRefDescriptionInterface(GenericDSInterfaceABC
						       *aDescription)

{
  m_RefDescriptionInterface = aDescription;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC *SummarizationSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int SummarizationSearchTool::SetQueryDescriptionInterface(GenericDSInterfaceABC
							 *aDescription)

{
  GenericDS Child;
  GenericDSInterfaceABC *ChildInterface;
  GenericDS Root;
  GenericDSInterfaceABC *RootInterface;
  
  RootInterface=m_QueryDescriptionInterface=aDescription;
  m_ActivityThreshold=0;
  if (!m_QueryDescriptionInterface) return -1;
  if (m_QueryDescriptionInterface->isNull()) return -1;

  if (m_QueryDescriptionInterface->GetDSName() == "FrameChangeValue") {
    //    fprintf(stderr,"FrameChangeValue should be child of query\n!");
    m_QueryDescriptionInterface->GetRealValue(m_ActivityThreshold);
    return 1;
  } 

  Child=RootInterface->GetChild(0);
  ChildInterface=Child.GetInterface();

  /* check if root node has name Query*/
  if (ChildInterface->GetDSName() == "Query") {
    Root=Child;
    RootInterface=Root.GetInterface();
  }

  /* find FrameChangeValue in all children*/
  Child=RootInterface->GetDescription("FrameChangeValue",0);
  if (Child.isNull()) {
    fprintf (stderr,"Warning: FrameChangeValue not found in query\n");
    return -1;
  }

  Child.GetRealValue(m_ActivityThreshold);
    fprintf (stderr,"FrameChangeValue threshold= %f\n",m_ActivityThreshold);
    return -1;
  return 0;
}

//----------------------------------------------------------------------------
int  SummarizationSearchTool::SetMedia(MultiMediaInterfaceABC
						 *aMedia)

{
  m_Media=aMedia;
  return 0;
}

//----------------------------------------------------------------------------
int SummarizationSearchTool::InitSearch(void)

{
  GenericDS Child;

  m_writeall=atoi(getpara("WriteAllFramesToDDL","0"));

  /* find frame property*/
  /* check if RefDescription is set*/
  if (!m_RefDescriptionInterface) return -1;
  //  if (!m_RefDescriptionInterface->isNull()) return -1;

  /********************************************/
  /* check if RefDescription has Summarization*/

  /* if ref is Summarization*/
  if (m_RefDescriptionInterface->GetDSName()=="SequentialSummary") {
    m_SummarizationInterface=m_RefDescriptionInterface;
  }
  else {
    /* else get first Summarization*/
    m_Summarization=m_RefDescriptionInterface->
      GetDescription("SequentialSummary",0);
    /* Summarization not found*/
    if (m_Summarization.isNull()) return -1;
    /* Summarization found*/
    m_SummarizationInterface=m_Summarization.GetInterface();
  }

  /***********************************************************************/
  /* check if RefDescription has FrameProperty belonging to current frame*/
  m_FrameProperty=m_SummarizationInterface->GetDescription("FrameProperty",0);

  return 0;
}

//----------------------------------------------------------------------------
int SummarizationSearchTool::StartSearch(void)

{
  GenericDS MediaTime,
    RelIncTime,
    FrameChangeValue;
  int FrameTime;
  int returnval;

  /* check for activity*/
  if (!m_ActivityThreshold) return -1;

  if (!m_writeall) {
    returnval=1;
  }
  else {
    returnval=-1;
  }

  while (!m_FrameProperty.isNull()) {
    MediaTime=m_FrameProperty.GetDescription("MediaTime",0);
    if (MediaTime.isNull()) {
      fprintf(stderr,"FrameProperty without MediaTime in SequentialSummary\n");
      return returnval;
    }

    RelIncTime=MediaTime.GetDescription("RelIncTime",0);
    if (RelIncTime.isNull()) {
      fprintf(stderr,"MediaTime without RelIncTime in SequentialSummary\n");
      return returnval;
    }

    RelIncTime.GetIntValue(FrameTime);

    if (FrameTime>m_Media->GetImageTime()) {
      return returnval;
    }
    if (FrameTime==m_Media->GetImageTime()) break;

    m_FrameProperty=m_FrameProperty.GetNextSibling("FrameProperty");
  }


  if (m_FrameProperty.isNull()) {
    /* if only key frames in SequentialSummary*/
    if (!m_writeall) {
      return 1;
    }
    return returnval;
  }

  /* search for FrameChangeValue in current*/
  FrameChangeValue=m_FrameProperty.GetDescription("FrameChangeValue",0);
  if (FrameChangeValue.isNull()) {
    fprintf(stderr,"FrameProperty without FrameChangeValue in "
	    "SequentialSummary\n");
    return returnval;
  }

  /* if FrameChangeValue for current image was found*/
  /* if only key frames in SequentialSummary*/
  if (!m_writeall) {
    return 2;
  }


  /* if all frames in SequentialSummary*/
  FrameChangeValue.GetRealValue(m_Activity);
  m_SumActivity+=m_Activity;
  if (m_SumActivity>m_ActivityThreshold) {
    m_SumActivity-=m_ActivityThreshold;
    return 2;
  }
  return 0;
}

//----------------------------------------------------------------------------
double SummarizationSearchTool::PostSearch(void)

{
  return m_ActivityThreshold;
}

//----------------------------------------------------------------------------
double SummarizationSearchTool::ActivityTreshold(void)

{
  return m_ActivityThreshold;
}

//----------------------------------------------------------------------------
SummarizationSearchInterfaceABC *SummarizationSearchTool::GetInterface(void)
{ return &m_Interface; }

