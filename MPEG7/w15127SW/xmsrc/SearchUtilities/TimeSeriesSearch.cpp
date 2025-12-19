//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Takehiro FUJITA
// Hitachi, Ltd.
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
// Applicable File Name:  TimeSeriesSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID TimeSeriesSearchInterface::myID = UUID("");
const char *TimeSeriesSearchInterface::myName = "TimeSeriesMatchingInterface";

const UUID TimeSeriesSearchTool::myID = UUID("");
const char *TimeSeriesSearchTool::myName = "TimeSeriesMatchingTool";

const UUID TimeSeriesSearchInterfaceABC::myID = UUID();

//=============================================================================
TimeSeriesSearchInterfaceABC::TimeSeriesSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
TimeSeriesSearchInterface::TimeSeriesSearchInterface(TimeSeriesSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeSeriesSearchInterface::~TimeSeriesSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TimeSeriesSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void TimeSeriesSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int TimeSeriesSearchInterface::
SetRefDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *RefTimeSeriesDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefTimeSeriesDescriptorInterface);
}

//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC* TimeSeriesSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int TimeSeriesSearchInterface::
SetQueryDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *QueryTimeSeriesDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryTimeSeriesDescriptorInterface);
}

//----------------------------------------------------------------------------
double TimeSeriesSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
TimeSeriesSearchTool::TimeSeriesSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
  /* create sub Searchs and
     connect them with descriptor if availabe*/
  SubTimeSeriesASearch *m_SubTimeSeriesASearch= new SubTimeSeriesASearch();
  SubTimeSeriesBSearch *m_SubTimeSeriesBSearch= new SubTimeSeriesBSearch();

  /* connect sub Search with TimeSeriesSearch */
  if (m_SubTimeSeriesASearch)
    SetSubTimeSeriesASearchInterface(m_SubTimeSeriesASearch->GetInterface());
  if (m_SubTimeSeriesBSearch)
    SetSubTimeSeriesBSearchInterface(m_SubTimeSeriesBSearch->GetInterface());

#endif /* __HasSubTypes*/
  SetExposedInterface(&m_Interface);
}

//=============================================================================
TimeSeriesSearchTool::
TimeSeriesSearchTool(TimeSeriesDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    TimeSeriesDescriptor *descriptor =
      new TimeSeriesDescriptor();
    aQueryDescriptorInterface=descriptor->GetInterface();
  }

  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub Search and
     connect them with descriptor if availabe*/
  if (TimeSeries->GetSubTimeSeriesADescriptorInterface()) {
    m_SubTimeSeriesASearch= new
      SubTimeSeriesASearch(TimeSeries->GetSubTimeSeriesADescriptorInterface());
  }
  else { 
    m_SubTimeSeriesASearch= new SubTimeSeriesASearch();
  }

  if (TimeSeries->GetSubTimeSeriesBDescriptorInterface()) {
    m_SubTimeSeriesBSearch= new
      SubTimeSeriesBSearch(TimeSeries->GetSubTimeSeriesBDescriptorInterface());
  }
  else { 
    m_SubTimeSeriesBSearch= new SubTimeSeriesBSearch();
  }

  if (m_SubTimeSeriesASearch)
    SetSubTimeSeriesASearchInterface(m_SubTimeSeriesASearch->GetInterface());
  if (m_SubTimeSeriesBSearch)
    SetSubTimeSeriesBSearchInterface(m_SubTimeSeriesBSearch->GetInterface());
#endif /* __HasSubTypes*/

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeSeriesSearchTool::~TimeSeriesSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
  if (m_SubTimeSeriesASearch) m_SubTimeSeriesASearch->destroy();
  if (m_SubTimeSeriesBSearch) m_SubTimeSeriesBSearch->destroy();
#endif /* __HasSubTypes*/
}

//----------------------------------------------------------------------------
const UUID& TimeSeriesSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool TimeSeriesSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool TimeSeriesSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int TimeSeriesSearchTool::
SetRefDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *aRefTimeSeriesDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefTimeSeriesDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefTimeSeriesDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();

 #ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
    /* conect sub descriptors with sub Searchs*/
    if ((m_RefDescriptorInterface->
	 GetSubTimeSeriesADescriptorInterface()) &&
	m_SubTimeSeriesASearchInterface) {
      m_SubTimeSeriesASearchInterface->
	SetDescriptorInterface(m_RefDescriptorInterface->
			       GetSubTimeSeriesADescriptorInterface());
    }
    if ((m_RefDescriptorInterface->
	 GetSubTimeSeriesBDescriptorInterface()) &&
	m_SubTimeSeriesBSearchInterface) {
      m_SubTimeSeriesBSearchInterface->
	SetDescriptorInterface(m_RefDescriptorInterface->
			       GetSubTimeSeriesBDescriptorInterface());
    }
#endif /* __HasSubTypes*/

  } else {
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
    /* if no descriptor, release also descriptor interfaces from
       sub Searchs*/
    m_SubTimeSeriesASearchInterface->SetDescriptorInterface(0);
    m_SubTimeSeriesBSearchInterface->SetDescriptorInterface(0);
#endif /* __HasSubTypes*/
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC* TimeSeriesSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  TimeSeriesSearchTool::
SetQueryDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *aQueryTimeSeriesDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryTimeSeriesDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryTimeSeriesDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
    /* conect sub descriptors with sub Searchs*/
    if ((m_QueryDescriptorInterface->
	 GetSubTimeSeriesADescriptorInterface()) &&
	m_SubTimeSeriesASearchInterface) {
      m_SubTimeSeriesASearchInterface->
	SetDescriptorInterface(m_QueryDescriptorInterface->
			       GetSubTimeSeriesADescriptorInterface());
    }
    if ((m_QueryDescriptorInterface->
	 GetSubTimeSeriesBDescriptorInterface()) &&
	m_SubTimeSeriesBSearchInterface) {
      m_SubTimeSeriesBSearchInterface->
	SetDescriptorInterface(m_QueryDescriptorInterface->
			       GetSubTimeSeriesBDescriptorInterface());
    }
#endif /* __HasSubTypes*/
  } else {
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
    /* if no descriptor, release also descriptor interfaces from
       sub Searchs*/
    m_SubTimeSeriesASearchInterface->SetDescriptorInterface(0);
    m_SubTimeSeriesBSearchInterface->SetDescriptorInterface(0);
#endif /* __HasSubTypes*/
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double TimeSeriesSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "TimeSeriesDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "TimeSeriesDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------
  // For this dummy type all I want is the abs difference between the values
  const long ref_val=m_RefDescriptorInterface->GetDummyContents();
  const long qur_val=m_QueryDescriptorInterface->GetDummyContents();

  double dist=static_cast<double> (ref_val-qur_val);
  if(dist<0.0) dist=-dist;

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
TimeSeriesSearchInterfaceABC *TimeSeriesSearchTool::GetInterface(void)
{ return &m_Interface; }

