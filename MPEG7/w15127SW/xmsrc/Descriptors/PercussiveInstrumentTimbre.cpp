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
// Applicable File Name:  PercussiveInstrumentTimbre.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID PercussiveInstrumentTimbreDescriptorInterface::myID = UUID("");
const char *PercussiveInstrumentTimbreDescriptorInterface::myName = 
"PercussiveInstrumentTimbreDescriptionInterface";

const UUID PercussiveInstrumentTimbreDescriptor::myID = UUID("");
const char *PercussiveInstrumentTimbreDescriptor::myName = "PercussiveInstrumentTimbreDescriptor";

const UUID PercussiveInstrumentTimbreDescriptor::valID = UUID("");
const char *PercussiveInstrumentTimbreDescriptor::valName = "";

const UUID PercussiveInstrumentTimbreDescriptorInterfaceABC::myID = UUID();

PercussiveInstrumentTimbreDescriptorInterfaceABC::PercussiveInstrumentTimbreDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
PercussiveInstrumentTimbreDescriptorInterface::
PercussiveInstrumentTimbreDescriptorInterface(PercussiveInstrumentTimbreDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterface::~PercussiveInstrumentTimbreDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreDescriptorInterface::GetSpectralCentroid(void)
{
    return m_Descriptor->GetSpectralCentroid();
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptorInterface::SetSpectralCentroid(double spectral_centroid)
{
    return m_Descriptor->SetSpectralCentroid(spectral_centroid);
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreDescriptorInterface::GetTemporalCentroid(void)
{
    return m_Descriptor->GetTemporalCentroid();
}
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptorInterface::SetTemporalCentroid(double temporal_centroid)
{
    return m_Descriptor->SetTemporalCentroid(temporal_centroid);
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreDescriptorInterface::GetLogAttackTime(void)
{
    return m_Descriptor->GetLogAttackTime();
}

//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptorInterface::SetLogAttackTime(double log_attack_time)
{
    return m_Descriptor->SetLogAttackTime(log_attack_time);
}

//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
PercussiveInstrumentTimbreDescriptor::PercussiveInstrumentTimbreDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_SpectralCentroid(0),
m_TemporalCentroid(0),
m_LogAttackTime(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptor::~PercussiveInstrumentTimbreDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"PercussiveInstrumentTimbre delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* PercussiveInstrumentTimbreDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool PercussiveInstrumentTimbreDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreDescriptor::GetSpectralCentroid(void)
{ return m_SpectralCentroid; }
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptor::SetSpectralCentroid(double spectral_centroid)
{
    m_SpectralCentroid = spectral_centroid;
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreDescriptor::GetTemporalCentroid(void)
{ return m_TemporalCentroid; }
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptor::SetTemporalCentroid(double temporal_centroid)
{
    m_TemporalCentroid = temporal_centroid;
}
//----------------------------------------------------------------------------
double PercussiveInstrumentTimbreDescriptor::GetLogAttackTime(void)
{ return m_LogAttackTime; }
//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptor::SetLogAttackTime(double log_attack_time)
{
    m_LogAttackTime = log_attack_time;
}
//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
    //GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  GenericDS l_AudioSegment;
  l_AudioSegment = aParentDescription->CreateChild("AudioDS");
  l_AudioSegment.SetAttribute("xsi:type","PercussiveInstrumentTimbreType");
  
  GenericDSInterfaceABC* l_AudioSegmentInterface = l_AudioSegment.GetInterface();

  // LogAttackTime
  GenericDS l_LogAttackTime = l_AudioSegmentInterface->CreateChild("LogAttackTime");
  GenericDSInterfaceABC* l_LogAttackTimeInterface = l_LogAttackTime.GetInterface();
  l_LogAttackTimeInterface->SetValue(m_LogAttackTime);
  
  // SpectralCentroid
  GenericDS l_SpectralCentroid = 
      l_AudioSegmentInterface->CreateChild("SpectralCentroid");
  GenericDSInterfaceABC* l_SpectralCentroidInterface = 
      l_SpectralCentroid.GetInterface();
  l_SpectralCentroidInterface->SetValue(m_SpectralCentroid);

  // TemporalCentroid
  GenericDS l_TemporalCentroid =
      l_AudioSegmentInterface->CreateChild("TemporalCentroid");
  GenericDSInterfaceABC* l_TemporalCentroidInterface =
      l_TemporalCentroid.GetInterface();
  l_TemporalCentroidInterface->SetValue(m_TemporalCentroid);

  return 0;
}

//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
    double value;
  string xsitype("");

  GenericDS l_AudioSegment;
  GenericDSInterfaceABC *l_AudioSegmentInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "AudioDS") 
  {
      aDescription->GetTextAttribute("xsi:type",xsitype);
      if (xsitype == "PercussiveInstrumentTimbreType") 
      {
	  l_AudioSegmentInterface=aDescription;
      }
  }

  /* else search for PercussiveInstrumentTimbre as a child*/
  if (!l_AudioSegmentInterface) 
  {
      l_AudioSegment=aDescription->GetDescription("AudioDS");
    
    /* search for correct xsi type*/
    while (!l_AudioSegment.isNull()) 
    {
      l_AudioSegment.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="PercussiveInstrumentTimbreType") 
	  break;
      l_AudioSegment=
	l_AudioSegment.GetNextSibling("Descriptor");
    }
      

    /* PercussiveInstrumentTimbre not found*/
    if (l_AudioSegment.isNull()) 
    {
	return (unsigned long)-1;
    }
    /* PercussiveInstrumentTimbre found*/
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


  // Get Spectral Centroid
  GenericDS l_SpectralCentroid = 
      l_AudioSegmentInterface->GetDescription("SpectralCentroid");
  if (! l_SpectralCentroid.isNull())
  {
      GenericDSInterfaceABC* l_SpectralCentroidInterface = l_SpectralCentroid.GetInterface();
      l_SpectralCentroidInterface->GetRealValue(value);
      SetSpectralCentroid(value);
  }

  // Get Temporal Centroid
  GenericDS l_TemporalCentroid = 
      l_AudioSegmentInterface->GetDescription("TemporalCentroid");
  if (! l_TemporalCentroid.isNull())
  {
      GenericDSInterfaceABC* l_TemporalCentroidInterface = 
	  l_TemporalCentroid.GetInterface();
      l_TemporalCentroidInterface->GetRealValue(value);
      SetTemporalCentroid(value);
  }

  return 0;
}

//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC *PercussiveInstrumentTimbreDescriptor::GetInterface(void)
{ return &m_Interface; }


