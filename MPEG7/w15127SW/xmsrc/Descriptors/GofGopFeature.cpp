///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Akio Yamada
// NEC Corp.
// Soo-Jun Park (ETRI), Dong Kwon Park (Dongguk Univ)
// Leszek Cieplinski (Mitsubishi Electric ITE-VIL)
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
// Copyright (c) 1998-2004.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GofGopFeature.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID GofGopFeatureDescriptorInterface::myID = UUID("");
const char *GofGopFeatureDescriptorInterface::myName = 
"GofGopFeatureTypeDescriptionInterface";

const UUID GofGopFeatureDescriptor::myID = UUID("");
const char *GofGopFeatureDescriptor::myName = "GofGopFeatureDescriptor";

const UUID GofGopFeatureDescriptor::valID = UUID("");
const char *GofGopFeatureDescriptor::valName = "";

const UUID GofGopFeatureDescriptorInterfaceABC::myID = UUID();

GofGopFeatureDescriptorInterfaceABC::GofGopFeatureDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
GofGopFeatureDescriptorInterface::
GofGopFeatureDescriptorInterface(GofGopFeatureDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GofGopFeatureDescriptorInterface::~GofGopFeatureDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void GofGopFeatureDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void GofGopFeatureDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& GofGopFeatureDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GofGopFeatureDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ColorLayoutDescriptorInterfaceABC
*GofGopFeatureDescriptorInterface::GetColorLayoutDescriptorInterface(void)
{
  return m_Descriptor->GetColorLayoutDescriptorInterface();
}

// added by dkpark -----------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC
*GofGopFeatureDescriptorInterface::GetEdgeHistogramDescriptorInterface(void)
{
  return m_Descriptor->GetEdgeHistogramDescriptorInterface();
}

// added by LC -----------------------------------------------------------
DominantColorDescriptorInterfaceABC
*GofGopFeatureDescriptorInterface::GetDominantColorDescriptorInterface(void)
{
  return m_Descriptor->GetDominantColorDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long GofGopFeatureDescriptorInterface::
SetColorLayoutDescriptorInterface(ColorLayoutDescriptorInterfaceABC
				    *aColorLayoutDescriptorInterface)	
{
  return m_Descriptor->
    SetColorLayoutDescriptorInterface(aColorLayoutDescriptorInterface);
}

// added by dkpark --------------------------------------------------------
unsigned long GofGopFeatureDescriptorInterface::
SetEdgeHistogramDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
				    *aEdgeHistogramDescriptorInterface)	
{
  return m_Descriptor->
    SetEdgeHistogramDescriptorInterface(aEdgeHistogramDescriptorInterface);
}

// added by LC --------------------------------------------------------
unsigned long GofGopFeatureDescriptorInterface::
SetDominantColorDescriptorInterface(DominantColorDescriptorInterfaceABC
				    *aDominantColorDescriptorInterface)	
{
  return m_Descriptor->
    SetDominantColorDescriptorInterface(aDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
long GofGopFeatureDescriptorInterface::GetAggregation(void)
{ return m_Descriptor->GetAggregation(); }

//----------------------------------------------------------------------------
void GofGopFeatureDescriptorInterface::SetAggregation(const long val)
{m_Descriptor->SetAggregation(val); }

//----------------------------------------------------------------------------
unsigned long GofGopFeatureDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long GofGopFeatureDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
GofGopFeatureDescriptor::GofGopFeatureDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ColorLayoutDescriptorInterface(NULL),
m_EdgeHistogramDescriptorInterface(NULL), // added by dkpark
m_DominantColorDescriptorInterface(NULL), // added by LC
m_Aggregation(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GofGopFeatureDescriptor::
GofGopFeatureDescriptor(ColorLayoutDescriptorInterfaceABC
		    *aColorLayoutInterface):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ColorLayoutDescriptorInterface(NULL),
m_Aggregation(0)
{
#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!aColorLayoutInterface) {
    ColorLayoutDescriptor *aColorLayout;
    aColorLayout= new ColorLayoutDescriptor/* or (0,0..0) if subsub Descriptors*/;
    aColorLayoutInterface=aColorLayout->GetInterface();
  }
  if (aColorLayoutInterface)
    SetColorLayoutDescriptorInterface(aColorLayoutInterface);

}

// added by dkpark ---------------------------------------------------------
GofGopFeatureDescriptor::
GofGopFeatureDescriptor(EdgeHistogramDescriptorInterfaceABC
		    *aEdgeHistogramInterface):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_EdgeHistogramDescriptorInterface(NULL),
m_Aggregation(0)
{
#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!aEdgeHistogramInterface) {
    EdgeHistogramDescriptor *aEdgeHistogram;
    aEdgeHistogram= new EdgeHistogramDescriptor/* or (0,0..0) if subsub Descriptors*/;
    aEdgeHistogramInterface=aEdgeHistogram->GetInterface();
  }
  if (aEdgeHistogramInterface)
    SetEdgeHistogramDescriptorInterface(aEdgeHistogramInterface);
}

// added by LC ---------------------------------------------------------
GofGopFeatureDescriptor::
GofGopFeatureDescriptor(DominantColorDescriptorInterfaceABC
		    *aDominantColorInterface):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_DominantColorDescriptorInterface(NULL),
m_Aggregation(0)
{
#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!aDominantColorInterface) {
    DominantColorDescriptor *aDominantColor;
    aDominantColor= new DominantColorDescriptor/* or (0,0..0) if subsub Descriptors*/;
    aDominantColorInterface=aDominantColor->GetInterface();
  }
  if (aDominantColorInterface)
    SetDominantColorDescriptorInterface(aDominantColorInterface);
}

// added by dkpark and LC ---------------------------------------------------------
GofGopFeatureDescriptor::
GofGopFeatureDescriptor(ColorLayoutDescriptorInterfaceABC
		    *aColorLayoutInterface, EdgeHistogramDescriptorInterfaceABC
		    *aEdgeHistogramInterface, DominantColorDescriptorInterfaceABC
		    *aDominantColorInterface):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ColorLayoutDescriptorInterface(NULL),
m_EdgeHistogramDescriptorInterface(NULL),
m_DominantColorDescriptorInterface(NULL),
m_Aggregation(0)
{
#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!aColorLayoutInterface) {
    ColorLayoutDescriptor *aColorLayout;
    aColorLayout= new ColorLayoutDescriptor/* or (0,0..0) if subsub Descriptors*/;
    aColorLayoutInterface=aColorLayout->GetInterface();
  }
  if (aColorLayoutInterface)
    SetColorLayoutDescriptorInterface(aColorLayoutInterface);

  if (!aEdgeHistogramInterface) {
    EdgeHistogramDescriptor *aEdgeHistogram;
    aEdgeHistogram= new EdgeHistogramDescriptor/* or (0,0..0) if subsub Descriptors*/;
    aEdgeHistogramInterface=aEdgeHistogram->GetInterface();
  }
  if (aEdgeHistogramInterface)
    SetEdgeHistogramDescriptorInterface(aEdgeHistogramInterface);

  if (!aDominantColorInterface) {
    DominantColorDescriptor *aDominantColor;
    aDominantColor= new DominantColorDescriptor/* or (0,0..0) if subsub Descriptors*/;
    aDominantColorInterface=aDominantColor->GetInterface();
  }
  if (aDominantColorInterface)
    SetDominantColorDescriptorInterface(aDominantColorInterface);
}

//----------------------------------------------------------------------------
GofGopFeatureDescriptor::~GofGopFeatureDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"GofGopFeatureType delete\n");
#endif
  if (m_ColorLayoutDescriptorInterface)
    m_ColorLayoutDescriptorInterface->release();

  if (m_EdgeHistogramDescriptorInterface) // added by dkpark
    m_EdgeHistogramDescriptorInterface->release();

  if (m_DominantColorDescriptorInterface) // added by dkpark
    m_DominantColorDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& GofGopFeatureDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GofGopFeatureDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GofGopFeatureDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GofGopFeatureDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GofGopFeatureDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
ColorLayoutDescriptorInterfaceABC *GofGopFeatureDescriptor::
GetColorLayoutDescriptorInterface(void)
{
  return m_ColorLayoutDescriptorInterface;
}

// added by dkpark -----------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC *GofGopFeatureDescriptor::
GetEdgeHistogramDescriptorInterface(void)
{
  return m_EdgeHistogramDescriptorInterface;
}

// added by LC -----------------------------------------------------------
DominantColorDescriptorInterfaceABC *GofGopFeatureDescriptor::
GetDominantColorDescriptorInterface(void)
{
  return m_DominantColorDescriptorInterface;
}

//----------------------------------------------------------------------------
unsigned long GofGopFeatureDescriptor::SetColorLayoutDescriptorInterface
        (ColorLayoutDescriptorInterfaceABC *aColorLayout)	
{
  if (!aColorLayout) return (unsigned long) -1;
  if (m_ColorLayoutDescriptorInterface == aColorLayout) return 0;
  if (m_ColorLayoutDescriptorInterface)
    m_ColorLayoutDescriptorInterface->release();
  m_ColorLayoutDescriptorInterface = aColorLayout;
  if (m_ColorLayoutDescriptorInterface)
    m_ColorLayoutDescriptorInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

// added by dkpark -----------------------------------------------------------
unsigned long GofGopFeatureDescriptor::SetEdgeHistogramDescriptorInterface
        (EdgeHistogramDescriptorInterfaceABC *aEdgeHistogram)	
{
  if (!aEdgeHistogram) return (unsigned long) -1;
  if (m_EdgeHistogramDescriptorInterface == aEdgeHistogram) return 0;
  if (m_EdgeHistogramDescriptorInterface)
    m_EdgeHistogramDescriptorInterface->release();
  m_EdgeHistogramDescriptorInterface = aEdgeHistogram;
  if (m_EdgeHistogramDescriptorInterface)
    m_EdgeHistogramDescriptorInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

// added by LC -----------------------------------------------------------
unsigned long GofGopFeatureDescriptor::SetDominantColorDescriptorInterface
        (DominantColorDescriptorInterfaceABC *aDominantColor)	
{
  if (!aDominantColor) return (unsigned long) -1;
  if (m_DominantColorDescriptorInterface == aDominantColor) return 0;
  if (m_DominantColorDescriptorInterface)
    m_DominantColorDescriptorInterface->release();
  m_DominantColorDescriptorInterface = aDominantColor;
  if (m_DominantColorDescriptorInterface)
    m_DominantColorDescriptorInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
long GofGopFeatureDescriptor::GetAggregation(void)
{ return m_Aggregation; }

//----------------------------------------------------------------------------
void GofGopFeatureDescriptor::SetAggregation(const long val)
{m_Aggregation=val; }

//----------------------------------------------------------------------------
unsigned long GofGopFeatureDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS GofGopFeatureTypeDDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  GofGopFeatureTypeDDLDescription=aParentDescription->CreateChild("GofGopFeatureType");
  //GofGopFeatureTypeDDLDescription.SetAttribute("Aggregation",(int)m_Aggregation);
  // added by LC
  string aggrstr;
  if(m_Aggregation < 1 && m_Aggregation > 3)
    return 999;
  if(m_Aggregation == 1)
    aggrstr = "Average";
  else if(m_Aggregation == 2)
    aggrstr = "Median";
  else if(m_Aggregation == 3)
    aggrstr = "SplitMerge";
  GofGopFeatureTypeDDLDescription.SetAttribute("Aggregation", aggrstr);

  if (m_ColorLayoutDescriptorInterface) {
    m_ColorLayoutDescriptorInterface->
      ExportDDL(GofGopFeatureTypeDDLDescription.GetInterface());
  }
  if (m_EdgeHistogramDescriptorInterface) { // added by dkpark
    m_EdgeHistogramDescriptorInterface->
      ExportDDL(GofGopFeatureTypeDDLDescription.GetInterface());
  }
  if (m_DominantColorDescriptorInterface) { // added by LC
    m_DominantColorDescriptorInterface->
      ExportDDL(GofGopFeatureTypeDDLDescription.GetInterface());
  }
  return 0;
}

//----------------------------------------------------------------------------
unsigned long GofGopFeatureDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int val;

  if (!aDescription) return (unsigned long) -1;
  if (aDescription->GetDSName() != "GofGopFeatureType") return (unsigned long) -1;
  
  //aDescription->GetIntAttribute("Aggregation",val);
  //SetAggregation((long)val);
  // added by LC
  string aggrstr;
  aDescription->GetTextAttribute("Aggregation", aggrstr);
  if(aggrstr == "Average")
    SetAggregation(1L);
  else if(aggrstr == "Median")
    SetAggregation(2L);
  if(aggrstr == "SplitMerge")
    SetAggregation(3L);
  else
    return 999;

  GenericDS SubDescription;
  if (m_ColorLayoutDescriptorInterface) {
    SubDescription=aDescription->GetDescription("ColorLayout",0);
    if (!SubDescription.isNull()) {
      m_ColorLayoutDescriptorInterface->
	ImportDDL(SubDescription.GetInterface());
    }
  }
  if (m_EdgeHistogramDescriptorInterface) { // added by dkpark
    SubDescription=aDescription->GetDescription("EdgeHistogram",0);
    if (!SubDescription.isNull()) {
      m_EdgeHistogramDescriptorInterface->
	ImportDDL(SubDescription.GetInterface());
    }
  }
  if (m_DominantColorDescriptorInterface) { // added by LC
    SubDescription=aDescription->GetDescription("DominantColor",0);
    if (!SubDescription.isNull()) {
      m_DominantColorDescriptorInterface->
	ImportDDL(SubDescription.GetInterface());
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
void GofGopFeatureDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void GofGopFeatureDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
GofGopFeatureDescriptorInterfaceABC *GofGopFeatureDescriptor::GetInterface(void)
{ return &m_Interface; }


