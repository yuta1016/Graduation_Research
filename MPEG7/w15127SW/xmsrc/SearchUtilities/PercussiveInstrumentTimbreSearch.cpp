//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Tisserand Patrice
// IRCAM
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  PercussiveInstrumentTimbreSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/readparam.h"
//=============================================================================
	

using namespace XM;


const UUID PercussiveInstrumentTimbreSearchInterface::myID = UUID("");
const char *PercussiveInstrumentTimbreSearchInterface::myName = "PercussiveInstrumentTimbreMatchingInterface";

const UUID PercussiveInstrumentTimbreSearchTool::myID = UUID("");
const char *PercussiveInstrumentTimbreSearchTool::myName = "PercussiveInstrumentTimbreMatchingTool";

const UUID PercussiveInstrumentTimbreSearchInterfaceABC::myID = UUID();

//=============================================================================
PercussiveInstrumentTimbreSearchInterfaceABC::PercussiveInstrumentTimbreSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
PercussiveInstrumentTimbreSearchInterface::PercussiveInstrumentTimbreSearchInterface(PercussiveInstrumentTimbreSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreSearchInterface::~PercussiveInstrumentTimbreSearchInterface()
{
}

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchInterface::GetLogAttackTimeWeight()
{
    return m_SearchTool->GetLogAttackTimeWeight();
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchInterface::SetLogAttackTimeWeight(double log_attack_time_weight)
{
    return m_SearchTool->SetLogAttackTimeWeight(log_attack_time_weight);
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchInterface::GetSpectralCentroidWeight()
{
    return m_SearchTool->GetSpectralCentroidWeight();
}											
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchInterface::SetSpectralCentroidWeight(double spectral_centroid_weight)
{
    return m_SearchTool->SetSpectralCentroidWeight(spectral_centroid_weight);
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchInterface::GetTemporalCentroidWeight()
{
    return m_SearchTool->GetTemporalCentroidWeight();
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchInterface::SetTemporalCentroidWeight(double temporal_centroid_weight)
{
    return m_SearchTool->SetTemporalCentroidWeight(temporal_centroid_weight);
}

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreSearchInterface::
SetRefDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *RefPercussiveInstrumentTimbreDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefPercussiveInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC* PercussiveInstrumentTimbreSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreSearchInterface::
SetQueryDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *QueryPercussiveInstrumentTimbreDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryPercussiveInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
PercussiveInstrumentTimbreSearchTool::PercussiveInstrumentTimbreSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_LogAttackTimeWeight(-0.3),
m_SpectralCentroidWeight(-1e-4),
m_TemporalCentroidWeight(-0.6)
{
    m_LogAttackTimeWeight = atof(getpara("LogAttackTimeWeight","-0.3"));
    m_SpectralCentroidWeight = atof(getpara("SpectralCentroidWeight","-1e-4"));
    m_TemporalCentroidWeight = atof(getpara("TemporalCentroidWeight","-0.6"));

  SetExposedInterface(&m_Interface);
}

//=============================================================================
PercussiveInstrumentTimbreSearchTool::
PercussiveInstrumentTimbreSearchTool(PercussiveInstrumentTimbreDescriptorInterfaceABC
			   *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_LogAttackTimeWeight(-0.3),
m_SpectralCentroidWeight(-1e-4),
m_TemporalCentroidWeight(-0.6)
{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    PercussiveInstrumentTimbreDescriptor *descriptor =
	new PercussiveInstrumentTimbreDescriptor;

    aQueryDescriptorInterface=descriptor->GetInterface();
  }


  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

    m_LogAttackTimeWeight = atof(getpara("LogAttackTimeWeight","-0.3"));
    m_SpectralCentroidWeight = atof(getpara("SpectralCentroidWeight","-1e-4"));
    m_TemporalCentroidWeight = atof(getpara("TemporalCentroidWeight","-0.6"));

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreSearchTool::~PercussiveInstrumentTimbreSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool PercussiveInstrumentTimbreSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool PercussiveInstrumentTimbreSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreSearchTool::
SetRefDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *aRefPercussiveInstrumentTimbreDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefPercussiveInstrumentTimbreDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefPercussiveInstrumentTimbreDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();

  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC* PercussiveInstrumentTimbreSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  PercussiveInstrumentTimbreSearchTool::
SetQueryDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *aQueryPercussiveInstrumentTimbreDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryPercussiveInstrumentTimbreDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryPercussiveInstrumentTimbreDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();

  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "PercussiveInstrumentTimbreDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "PercussiveInstrumentTimbreDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------
  double dist;

  double distance_lat_tc     = GetPartDistanceLogAttackTimeTemporalCentroid();
  double distance_sc     = GetPartDistanceSpectralCentroid();

  // Full distance
  double sum_distance     = distance_lat_tc + distance_sc;

  dist = sqrt(sum_distance);

  // All done
  return(dist);
}
//----------------------------------------------------------------------------
PercussiveInstrumentTimbreSearchInterfaceABC *PercussiveInstrumentTimbreSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchTool::GetPartDistanceLogAttackTimeTemporalCentroid(void)
{
  // Compute Log Attack Time Distance
  const double ref_lat = m_RefDescriptorInterface->GetLogAttackTime();
  const double qur_lat = m_QueryDescriptorInterface->GetLogAttackTime();
  const double ref_tc  = m_RefDescriptorInterface->GetTemporalCentroid();
  const double qur_tc  = m_QueryDescriptorInterface->GetTemporalCentroid();

  // Difference between ref and query
  double diff_lat = ref_lat - qur_lat;
  double diff_tc  = ref_tc - qur_tc;

  // Weighting coefficient
  const double coeff_lat = GetLogAttackTimeWeight();
  const double coeff_tc  = GetTemporalCentroidWeight();
  
  double distance_lat_tc = coeff_lat * diff_lat + coeff_tc * diff_tc;

  distance_lat_tc = pow(distance_lat_tc, 2);

  return distance_lat_tc;
}

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchTool::GetPartDistanceSpectralCentroid(void)
{
  // Compute Harmonic Spectral Centroid Distance
  const double ref_sc = m_RefDescriptorInterface->GetSpectralCentroid();
  const double qur_sc = m_QueryDescriptorInterface->GetSpectralCentroid();
  // Difference between Reference and Query
  double diff_sc = ref_sc - qur_sc;
  // Weighting Coefficient
  const double coeff_sc = GetSpectralCentroidWeight();
  
  double distance_sc = coeff_sc * diff_sc;
  distance_sc = pow(distance_sc, 2);
  return distance_sc;
}

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchTool::GetLogAttackTimeWeight()
{
    return m_LogAttackTimeWeight;
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchTool::SetLogAttackTimeWeight(double log_attack_time_weight)
{
    m_LogAttackTimeWeight = log_attack_time_weight;
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchTool::GetSpectralCentroidWeight()
{
    return m_SpectralCentroidWeight;
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchTool::SetSpectralCentroidWeight(double spectral_centroid_weight)
{
    m_SpectralCentroidWeight = spectral_centroid_weight;
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreSearchTool::GetTemporalCentroidWeight()
{
    return m_TemporalCentroidWeight;
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreSearchTool::SetTemporalCentroidWeight(double temporal_centroid_weight)
{
    m_TemporalCentroidWeight = temporal_centroid_weight;
}

