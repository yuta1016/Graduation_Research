///////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  HarmonicInstrumentTimbre.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID HarmonicInstrumentTimbreDescriptorInterface::myID = UUID("");
const char *HarmonicInstrumentTimbreDescriptorInterface::myName = 
"HarmonicInstrumentTimbreDescriptionInterface";

const UUID HarmonicInstrumentTimbreDescriptor::myID = UUID("");
const char *HarmonicInstrumentTimbreDescriptor::myName = "HarmonicInstrumentTimbreDescriptor";

const UUID HarmonicInstrumentTimbreDescriptor::valID = UUID("");
const char *HarmonicInstrumentTimbreDescriptor::valName = "";

const UUID HarmonicInstrumentTimbreDescriptorInterfaceABC::myID = UUID();

HarmonicInstrumentTimbreDescriptorInterfaceABC::HarmonicInstrumentTimbreDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
HarmonicInstrumentTimbreDescriptorInterface::
HarmonicInstrumentTimbreDescriptorInterface(HarmonicInstrumentTimbreDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterface::~HarmonicInstrumentTimbreDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreDescriptorInterface::GetName(void)
{ return myName; }
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptorInterface::GetHarmonicSpectralCentroid(void)
{
    return m_Descriptor->GetHarmonicSpectralCentroid();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::SetHarmonicSpectralCentroid(double harmonic_spectral_centroid)
{
    return m_Descriptor->SetHarmonicSpectralCentroid(harmonic_spectral_centroid);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptorInterface::GetHarmonicSpectralDeviation(void)
{
    return m_Descriptor->GetHarmonicSpectralDeviation();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::SetHarmonicSpectralDeviation(double harmonic_spectral_deviation)
{
    return m_Descriptor->SetHarmonicSpectralDeviation(harmonic_spectral_deviation);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptorInterface::GetHarmonicSpectralSpread(void)
{
    return m_Descriptor->GetHarmonicSpectralSpread();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::SetHarmonicSpectralSpread(double harmonic_spectral_spread)
{
    return m_Descriptor->SetHarmonicSpectralSpread(harmonic_spectral_spread);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptorInterface::GetHarmonicSpectralVariation(void)
{
    return m_Descriptor->GetHarmonicSpectralVariation();
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::SetHarmonicSpectralVariation(double harmonic_spectral_variation)
{
    return m_Descriptor->SetHarmonicSpectralVariation(harmonic_spectral_variation);
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptorInterface::GetLogAttackTime(void)
{
    return m_Descriptor->GetLogAttackTime();
}

//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptorInterface::SetLogAttackTime(double log_attack_time)
{
    return m_Descriptor->SetLogAttackTime(log_attack_time);
}

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
HarmonicInstrumentTimbreDescriptor::HarmonicInstrumentTimbreDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_HarmonicSpectralCentroid(0),
m_HarmonicSpectralDeviation(0),
m_HarmonicSpectralSpread(0),
m_HarmonicSpectralVariation(0),
m_LogAttackTime(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptor::~HarmonicInstrumentTimbreDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"HarmonicInstrumentTimbre delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* HarmonicInstrumentTimbreDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool HarmonicInstrumentTimbreDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptor::GetHarmonicSpectralCentroid(void)
{ return m_HarmonicSpectralCentroid; }
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::SetHarmonicSpectralCentroid(double harmonic_spectral_centroid)
{
    m_HarmonicSpectralCentroid = harmonic_spectral_centroid;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptor::GetHarmonicSpectralDeviation(void)
{ return m_HarmonicSpectralDeviation; }
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::SetHarmonicSpectralDeviation(double harmonic_spectral_deviation)
{
    m_HarmonicSpectralDeviation = harmonic_spectral_deviation;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptor::GetHarmonicSpectralSpread(void)
{ return m_HarmonicSpectralSpread; }
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::SetHarmonicSpectralSpread(double harmonic_spectral_spread)
{
    m_HarmonicSpectralSpread = harmonic_spectral_spread;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptor::GetHarmonicSpectralVariation(void)
{
    return m_HarmonicSpectralVariation;
}
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::SetHarmonicSpectralVariation(double harmonic_spectral_variation)
{
    m_HarmonicSpectralVariation = harmonic_spectral_variation;
}
//----------------------------------------------------------------------------
double HarmonicInstrumentTimbreDescriptor::GetLogAttackTime(void)
{ return m_LogAttackTime; }
//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::SetLogAttackTime(double log_attack_time)
{
    m_LogAttackTime = log_attack_time;
}
//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
    //GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  GenericDS l_AudioSegment;
  l_AudioSegment = aParentDescription->CreateChild("AudioDS");
  l_AudioSegment.SetAttribute("xsi:type", "HarmonicInstrumentTimbreType");

  GenericDSInterfaceABC* l_AudioSegmentInterface = l_AudioSegment.GetInterface();

  // LogAttackTime
  GenericDS l_LogAttackTime = l_AudioSegmentInterface->CreateChild("LogAttackTime");
  GenericDSInterfaceABC* l_LogAttackTimeInterface = l_LogAttackTime.GetInterface();

  l_LogAttackTimeInterface->SetValue(m_LogAttackTime);

  // HarmonicSpectralCentroid
  GenericDS l_HarmonicSpectralCentroid = 
      l_AudioSegmentInterface->CreateChild("HarmonicSpectralCentroid");
  GenericDSInterfaceABC* l_HarmonicSpectralCentroidInterface = 
      l_HarmonicSpectralCentroid.GetInterface();

  l_HarmonicSpectralCentroidInterface->SetValue(m_HarmonicSpectralCentroid);

  // HarmonicSpectralDeviation
  GenericDS l_HarmonicSpectralDeviation = 
      l_AudioSegmentInterface->CreateChild("HarmonicSpectralDeviation");
  GenericDSInterfaceABC* l_HarmonicSpectralDeviationInterface = 
      l_HarmonicSpectralDeviation.GetInterface();

  //l_DDLDescription.SetAttribute("HarmonicSpectralDeviation", m_HarmonicSpectralDeviation);
  l_HarmonicSpectralDeviationInterface->SetValue(m_HarmonicSpectralDeviation);

  // HarmonicSpectralSpread
  GenericDS l_HarmonicSpectralSpread = 
      l_AudioSegmentInterface->CreateChild("HarmonicSpectralSpread");
  GenericDSInterfaceABC* l_HarmonicSpectralSpreadInterface = 
      l_HarmonicSpectralSpread.GetInterface();
  
  //l_DDLDescription.SetAttribute("HarmonicSpectralSpread",    m_HarmonicSpectralSpread);
  l_HarmonicSpectralSpreadInterface->SetValue(m_HarmonicSpectralSpread);


  // HarmonicSpectralVariation
  GenericDS l_HarmonicSpectralVariation = 
      l_AudioSegmentInterface->CreateChild("HarmonicSpectralVariation");
  GenericDSInterfaceABC* l_HarmonicSpectralVariationInterface = 
      l_HarmonicSpectralVariation.GetInterface();

  //l_DDLDescription.SetAttribute("HarmonicSpectralVariation", m_HarmonicSpectralVariation);
  l_HarmonicSpectralVariationInterface->SetValue(m_HarmonicSpectralVariation);

  return 0;
}

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  double value = 0;
  string xsitype("");

  GenericDS l_AudioSegment;
  GenericDSInterfaceABC *l_AudioSegmentInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;



  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "AudioDS") 
  {
      aDescription->GetTextAttribute("xsi:type",xsitype);
      if (xsitype == "HarmonicInstrumentTimbreType") 
      {
	  l_AudioSegmentInterface=aDescription;
      }
  }

  /* else search for HarmonicInstrumentTimbre as a child*/
  if (!l_AudioSegmentInterface) 
  {
      l_AudioSegment=aDescription->GetDescription("AudioDS");
    
      /* search for correct xsi type*/
      while (!l_AudioSegment.isNull()) 
      {
	  l_AudioSegment.GetTextAttribute("xsi:type", xsitype);
	  if(xsitype=="HarmonicInstrumentTimbreType") 
	      break;
	  l_AudioSegment=
	      l_AudioSegment.GetNextSibling("AudioDS");
      }

    /* HarmonicInstrumentTimbre not found*/
    if (l_AudioSegment.isNull()) 
    {
	cerr << " HarmonicInstrumentTimbreType not found !!! " << endl;
	return (unsigned long)-1;
    }
    /* HarmonicInstrumentTimbre found*/
    l_AudioSegmentInterface=
	l_AudioSegment.GetInterface();
  }

  // Get Log Attack Time
  GenericDS l_LogAttackTime = l_AudioSegmentInterface->GetDescription("LogAttackTime");
  if (! l_LogAttackTime.isNull())
  {
      GenericDSInterfaceABC* l_LogAttackTimeInterface = l_LogAttackTime.GetInterface();
      l_LogAttackTimeInterface->GetRealValue(value);
      SetLogAttackTime(value);
  }


  // Get Harmonic Spectral Centroid
  GenericDS l_HarmonicSpectralCentroid = 
      l_AudioSegmentInterface->GetDescription("HarmonicSpectralCentroid");
  if (! l_HarmonicSpectralCentroid.isNull())
  {
      GenericDSInterfaceABC* l_HarmonicSpectralCentroidInterface = 
	  l_HarmonicSpectralCentroid.GetInterface();
      l_HarmonicSpectralCentroidInterface->GetRealValue(value);
      SetHarmonicSpectralCentroid(value);
  }

  // Get Harmonic Spectral Deviation
  GenericDS l_HarmonicSpectralDeviation = 
      l_AudioSegmentInterface->GetDescription("HarmonicSpectralDeviation");
  if (! l_HarmonicSpectralDeviation.isNull())
  {
      GenericDSInterfaceABC* l_HarmonicSpectralDeviationInterface = 
	  l_HarmonicSpectralDeviation.GetInterface();
      l_HarmonicSpectralDeviationInterface->GetRealValue(value);
      SetHarmonicSpectralDeviation(value);
  }

  // Get Harmonic Spectral Spread
  GenericDS l_HarmonicSpectralSpread = 
      l_AudioSegmentInterface->GetDescription("HarmonicSpectralSpread");
  if (! l_HarmonicSpectralSpread.isNull())
  {
      GenericDSInterfaceABC* l_HarmonicSpectralSpreadInterface = 
	  l_HarmonicSpectralSpread.GetInterface();
      l_HarmonicSpectralSpreadInterface->GetRealValue(value);
      SetHarmonicSpectralSpread(value);
  }

  // Get Harmonic Spectral Variation
  GenericDS l_HarmonicSpectralVariation = 
      l_AudioSegmentInterface->GetDescription("HarmonicSpectralVariation");
  if (! l_HarmonicSpectralVariation.isNull())
  {
      GenericDSInterfaceABC* l_HarmonicSpectralVariationInterface = 
	  l_HarmonicSpectralVariation.GetInterface();
      l_HarmonicSpectralVariationInterface->GetRealValue(value);
      SetHarmonicSpectralVariation(value);
  }

  return 0;
}

//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC *HarmonicInstrumentTimbreDescriptor::GetInterface(void)
{ return &m_Interface; }


