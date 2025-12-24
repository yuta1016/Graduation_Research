//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Tisserand Patrice
// IRCAM
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
// Applicable File Name:  HarmonicInstrumentTimbreSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/readparam.h"
//=============================================================================
	

using namespace XM;


const UUID HarmonicInstrumentTimbreSearchInterface::myID = UUID("");
const char *HarmonicInstrumentTimbreSearchInterface::myName = "HarmonicInstrumentTimbreMatchingInterface";

const UUID HarmonicInstrumentTimbreSearchTool::myID = UUID("");
const char *HarmonicInstrumentTimbreSearchTool::myName = "HarmonicInstrumentTimbreMatchingTool";

const UUID HarmonicInstrumentTimbreSearchInterfaceABC::myID = UUID();

//=============================================================================
HarmonicInstrumentTimbreSearchInterfaceABC::HarmonicInstrumentTimbreSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
HarmonicInstrumentTimbreSearchInterface::HarmonicInstrumentTimbreSearchInterface(HarmonicInstrumentTimbreSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreSearchInterface::~HarmonicInstrumentTimbreSearchInterface()
{
}

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchInterface::GetLogAttackTimeWeight()
{
    return m_SearchTool->GetLogAttackTimeWeight();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchInterface::SetLogAttackTimeWeight(double log_attack_time_weight)
{
    return m_SearchTool->SetLogAttackTimeWeight(log_attack_time_weight);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchInterface::GetHarmonicSpectralCentroidWeight()
{
    return m_SearchTool->GetHarmonicSpectralCentroidWeight();
}											
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchInterface::SetHarmonicSpectralCentroidWeight(double harmonic_spectral_centroid_weight)
{
    return m_SearchTool->SetHarmonicSpectralCentroidWeight(harmonic_spectral_centroid_weight);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchInterface::GetHarmonicSpectralDeviationWeight()
{
    return m_SearchTool->GetHarmonicSpectralDeviationWeight();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchInterface::SetHarmonicSpectralDeviationWeight(double harmonic_spectral_deviation_weight)
{
    return m_SearchTool->SetHarmonicSpectralDeviationWeight(harmonic_spectral_deviation_weight);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchInterface::GetHarmonicSpectralSpreadWeight()
{
    return m_SearchTool->GetHarmonicSpectralSpreadWeight();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchInterface::SetHarmonicSpectralSpreadWeight(double harmonic_spectral_spread_weight)
{
    return m_SearchTool->SetHarmonicSpectralSpreadWeight(harmonic_spectral_spread_weight);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchInterface::GetHarmonicSpectralVariationWeight()
{
    return m_SearchTool->GetHarmonicSpectralVariationWeight();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchInterface::SetHarmonicSpectralVariationWeight(double harmonic_spectral_variation_weight)
{
    return m_SearchTool->SetHarmonicSpectralVariationWeight(harmonic_spectral_variation_weight);
}
//----------------------------------------------------------------------------

const UUID& HarmonicInstrumentTimbreSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreSearchInterface::
SetRefDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *RefHarmonicInstrumentTimbreDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefHarmonicInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC* HarmonicInstrumentTimbreSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreSearchInterface::
SetQueryDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *QueryHarmonicInstrumentTimbreDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryHarmonicInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
HarmonicInstrumentTimbreSearchTool::HarmonicInstrumentTimbreSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_LogAttackTimeWeight(8.0),
m_HarmonicSpectralCentroidWeight(3e-5),
m_HarmonicSpectralDeviationWeight(3e-4),
m_HarmonicSpectralSpreadWeight(10.0),
m_HarmonicSpectralVariationWeight(-60.0)
{
    m_LogAttackTimeWeight = atof(getpara("LogAttackTimeWeight","8.0"));
    m_HarmonicSpectralCentroidWeight = atof(getpara("HarmonicSpectralCentroidWeight","3e-5"));
    m_HarmonicSpectralDeviationWeight = atof(getpara("HarmonicSpectralDeviationWeight","3e-4"));
    m_HarmonicSpectralSpreadWeight = atof(getpara("HarmonicSpectralSpreadWeight","10.0"));
    m_HarmonicSpectralVariationWeight = atof(getpara("HarmonicSpectralVariationWeight","-60.0"));

  SetExposedInterface(&m_Interface);
}

//=============================================================================
HarmonicInstrumentTimbreSearchTool::
HarmonicInstrumentTimbreSearchTool(HarmonicInstrumentTimbreDescriptorInterfaceABC
			   *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_LogAttackTimeWeight(8.0),
m_HarmonicSpectralCentroidWeight(3e-5),
m_HarmonicSpectralDeviationWeight(3e-4),
m_HarmonicSpectralSpreadWeight(10.0),
m_HarmonicSpectralVariationWeight(-60.0)
{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    HarmonicInstrumentTimbreDescriptor *descriptor =
	new HarmonicInstrumentTimbreDescriptor;

    aQueryDescriptorInterface=descriptor->GetInterface();
  }


  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

    m_LogAttackTimeWeight = atof(getpara("LogAttackTimeWeight","8.0"));
    m_HarmonicSpectralCentroidWeight = atof(getpara("HarmonicSpectralCentroidWeight","3e-5"));
    m_HarmonicSpectralDeviationWeight = atof(getpara("HarmonicSpectralDeviationWeight","3e-4"));
    m_HarmonicSpectralSpreadWeight = atof(getpara("HarmonicSpectralSpreadWeight","10.0"));
    m_HarmonicSpectralVariationWeight = atof(getpara("HarmonicSpectralVariationWeight","-60.0"));

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreSearchTool::~HarmonicInstrumentTimbreSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool HarmonicInstrumentTimbreSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool HarmonicInstrumentTimbreSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreSearchTool::
SetRefDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *aRefHarmonicInstrumentTimbreDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefHarmonicInstrumentTimbreDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefHarmonicInstrumentTimbreDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();

  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC* HarmonicInstrumentTimbreSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  HarmonicInstrumentTimbreSearchTool::
SetQueryDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *aQueryHarmonicInstrumentTimbreDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryHarmonicInstrumentTimbreDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryHarmonicInstrumentTimbreDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "HarmonicInstrumentTimbreDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "HarmonicInstrumentTimbreDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------

  double distance_lat     = GetPartDistanceLogAttackTime();
  double distance_hsc     = GetPartDistanceHarmonicSpectralCentroid();
  double distance_hsd     = GetPartDistanceHarmonicSpectralDeviation();
  double distance_hss_hsv = GetPartDistanceHarmonicSpectralSpread();

  // Full distance
  double sum_distance     = distance_lat + distance_hsc + distance_hsd + distance_hss_hsv;

  double dist = sqrt(sum_distance);

  // All done
  return(dist);
}
//----------------------------------------------------------------------------
HarmonicInstrumentTimbreSearchInterfaceABC *HarmonicInstrumentTimbreSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetPartDistanceLogAttackTime(void)
{
  // Compute Log Attack Time Distance
  const double ref_lat = m_RefDescriptorInterface->GetLogAttackTime();
  const double qur_lat = m_QueryDescriptorInterface->GetLogAttackTime();
  // Difference between ref and query
  double diff_lat = ref_lat - qur_lat;

  // Weighting coefficient
  const double coeff_lat = GetLogAttackTimeWeight();
  
  double distance_lat = coeff_lat * pow(diff_lat, 2);

  return distance_lat;
}

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetPartDistanceHarmonicSpectralCentroid(void)
{
  // Compute Harmonic Spectral Centroid Distance
  const double ref_hsc = m_RefDescriptorInterface->GetHarmonicSpectralCentroid();
  const double qur_hsc = m_QueryDescriptorInterface->GetHarmonicSpectralCentroid();
  // Difference between Reference and Query
  double diff_hsc = ref_hsc - qur_hsc;

  // Weighting Coefficient
  const double coeff_hsc = GetHarmonicSpectralCentroidWeight();
  
  double distance_hsc = coeff_hsc * pow(diff_hsc, 2);
  return distance_hsc;
}

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetPartDistanceHarmonicSpectralDeviation(void)
{
  // Compute Harmonic Spectral Deviation Distance
  const double ref_hsd = m_RefDescriptorInterface->GetHarmonicSpectralDeviation();
  const double qur_hsd = m_QueryDescriptorInterface->GetHarmonicSpectralDeviation();
  // Difference between Reference and Query
  double diff_hsd = ref_hsd - qur_hsd;

  // Weighting Coefficient
  const double coeff_hsd = GetHarmonicSpectralDeviationWeight();
  
  double distance_hsd = coeff_hsd * pow(diff_hsd, 2);
  
  return distance_hsd;
}

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetPartDistanceHarmonicSpectralSpread(void)
{
    // Harmonic Spectral Spread Part 
  const double ref_hss = m_RefDescriptorInterface->GetHarmonicSpectralSpread();
  const double qur_hss = m_QueryDescriptorInterface->GetHarmonicSpectralSpread();
  // Difference between Reference and Query
  double diff_hss = ref_hss - qur_hss;

  // Weighting Coefficient
  const double coeff_hss = GetHarmonicSpectralSpreadWeight();
  
  double distance_hss = coeff_hss * diff_hss;

  // Harmonic Spectral Variation Part
  const double ref_hsv = m_RefDescriptorInterface->GetHarmonicSpectralVariation();
  const double qur_hsv = m_QueryDescriptorInterface->GetHarmonicSpectralVariation();
  // Difference between Reference and Query
  double diff_hsv = ref_hsv - qur_hsv;

  // Weighting Coefficient
  const double coeff_hsv = GetHarmonicSpectralVariationWeight();
  
  double distance_hsv = coeff_hsv * diff_hsv;
  
  double sum_distance_hss_hsv = distance_hss + distance_hsv;
  double distance_hss_hsv = pow(sum_distance_hss_hsv, 2);

  return distance_hss_hsv;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetLogAttackTimeWeight()
{
    return m_LogAttackTimeWeight;
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchTool::SetLogAttackTimeWeight(double log_attack_time_weight)
{
    m_LogAttackTimeWeight = log_attack_time_weight;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetHarmonicSpectralCentroidWeight()
{
    return m_HarmonicSpectralCentroidWeight;
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchTool::SetHarmonicSpectralCentroidWeight(double harmonic_spectral_centroid_weight)
{
    m_HarmonicSpectralCentroidWeight = harmonic_spectral_centroid_weight;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetHarmonicSpectralDeviationWeight()
{
    return m_HarmonicSpectralDeviationWeight;
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchTool::SetHarmonicSpectralDeviationWeight(double harmonic_spectral_deviation_weight)
{
    m_HarmonicSpectralDeviationWeight = harmonic_spectral_deviation_weight;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetHarmonicSpectralSpreadWeight()
{
    return m_HarmonicSpectralSpreadWeight;
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchTool::SetHarmonicSpectralSpreadWeight(double harmonic_spectral_spread_weight)
{
    m_HarmonicSpectralSpreadWeight = harmonic_spectral_spread_weight;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreSearchTool::GetHarmonicSpectralVariationWeight()
{
    return m_HarmonicSpectralVariationWeight;
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreSearchTool::SetHarmonicSpectralVariationWeight(double harmonic_spectral_variation_weight)
{
    m_HarmonicSpectralVariationWeight = harmonic_spectral_variation_weight;
}
//----------------------------------------------------------------------------

