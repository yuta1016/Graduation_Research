///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Modified by Vincent Hsu, June 11, 2001
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
// Applicable File Name:  GoFGoPColor.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"
#include "Utilities/readparam.h"

using namespace XM;

const UUID GoFGoPColorDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76e");
const char *GoFGoPColorDescriptorInterface::myName = 
"GoFGoPColorDescriptionInterface";

const UUID GoFGoPColorDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76e");
const char *GoFGoPColorDescriptor::myName = "GoFGoPColorDescriptor";

const UUID GoFGoPColorDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76e");
const char *GoFGoPColorDescriptor::valName = "";

const UUID GoFGoPColorDescriptorInterfaceABC::myID = UUID();

GoFGoPColorDescriptorInterfaceABC::GoFGoPColorDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
GoFGoPColorDescriptorInterface::
GoFGoPColorDescriptorInterface(GoFGoPColorDescriptor *aDescriptor):
m_Descriptor(aDescriptor)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterface::~GoFGoPColorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void GoFGoPColorDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void GoFGoPColorDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& GoFGoPColorDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC
*GoFGoPColorDescriptorInterface::GetScalableColorDescriptorInterface(void)
{
  return m_Descriptor->GetScalableColorDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long GoFGoPColorDescriptorInterface::
SetScalableColorDescriptorInterface(ScalableColorDescriptorInterfaceABC
				    *aScalableColorDescriptorInterface)	
{
  return m_Descriptor->
    SetScalableColorDescriptorInterface(aScalableColorDescriptorInterface);
}

//----------------------------------------------------------------------------
TAggregation GoFGoPColorDescriptorInterface::GetAggregation(void)
{ return m_Descriptor->GetAggregation(); }

//----------------------------------------------------------------------------
void GoFGoPColorDescriptorInterface::SetAggregation(const TAggregation anAggregation)
{m_Descriptor->SetAggregation(anAggregation); }

//----------------------------------------------------------------------------
unsigned long GoFGoPColorDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long GoFGoPColorDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
GoFGoPColorDescriptor::GoFGoPColorDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ScalableColorDescriptorInterface(NULL),
m_Aggregation(Average)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptor::
GoFGoPColorDescriptor(ScalableColorDescriptorInterfaceABC
		    *aScalableColorInterface):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ScalableColorDescriptorInterface(NULL),
m_Aggregation(Average)
{
#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!aScalableColorInterface) {
    ScalableColorDescriptor *aScalableColor;
    aScalableColor= new ScalableColorDescriptor;
    aScalableColorInterface=aScalableColor->GetInterface();
  }
  if (aScalableColorInterface)
    SetScalableColorDescriptorInterface(aScalableColorInterface);
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptor::~GoFGoPColorDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"GoFGoPColor delete\n");
#endif
  if (m_ScalableColorDescriptorInterface)
    m_ScalableColorDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& GoFGoPColorDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GoFGoPColorDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GoFGoPColorDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GoFGoPColorDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC *GoFGoPColorDescriptor::
GetScalableColorDescriptorInterface(void)
{
  return m_ScalableColorDescriptorInterface;
}

//----------------------------------------------------------------------------
unsigned long GoFGoPColorDescriptor::SetScalableColorDescriptorInterface
        (ScalableColorDescriptorInterfaceABC *aScalableColor)	
{
  if (!aScalableColor) return (unsigned long) -1;
  if (m_ScalableColorDescriptorInterface == aScalableColor) return 0;
  if (m_ScalableColorDescriptorInterface)
    m_ScalableColorDescriptorInterface->release();
  m_ScalableColorDescriptorInterface = aScalableColor;
  if (m_ScalableColorDescriptorInterface)
    m_ScalableColorDescriptorInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
TAggregation GoFGoPColorDescriptor::GetAggregation(void)
{ return m_Aggregation; }

//----------------------------------------------------------------------------
void GoFGoPColorDescriptor::SetAggregation(const TAggregation anAggregation)
{m_Aggregation=anAggregation; }

//----------------------------------------------------------------------------
unsigned long GoFGoPColorDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","GoFGoPColorType");
  switch (m_Aggregation) {
  case Average:
    l_DDLDescription.SetAttribute("aggregation","Average");
    break;
  case Median:
    l_DDLDescription.SetAttribute("aggregation","Median");
    break;
  case Intersection:
    l_DDLDescription.SetAttribute("aggregation","Intersection");
    break;
  default:
    fprintf(stderr,"Error: Unknown Aggregation type in GoFGoP\n");
    exit (1);
  }

  if (m_ScalableColorDescriptorInterface) {
	  m_ScalableColorDescriptorInterface->SetGoFGoP(true);
    m_ScalableColorDescriptorInterface->
      ExportDDL(l_DDLDescription.GetInterface());
  }
  return 0;
}

//----------------------------------------------------------------------------
unsigned long GoFGoPColorDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string theAggregation;
  string xsitype;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "GoFGoPColorType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for GoFGoPColor as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DDLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="GoFGoPColorType") break;
      l_DDLDescription=
	l_DDLDescription.GetNextSibling("Descriptor");
    }


    /* GoFGoPColor not found*/
    if (l_DDLDescription.isNull()) return (unsigned long)-1;
    /* GoFGoPColor found*/
    l_DDLDescriptionInterface=
      l_DDLDescription.GetInterface();
  }

  l_DDLDescriptionInterface->GetTextAttribute("aggregation",theAggregation);
  SetAggregation(Reserved);
  if (theAggregation == "Average")
    SetAggregation(Average);
  else if (theAggregation == "Median")
    SetAggregation(Median);
  else if (theAggregation == "Intersection")
    SetAggregation(Intersection);

  if (GetAggregation() == Reserved) {
    fprintf(stderr,"Warning: unkown AggregationType in GoFGoPColor "
	    "XML decoding\n");
    SetAggregation(Reserved);
  }

  GenericDS SubDescription;
  if (m_ScalableColorDescriptorInterface) {
    SubDescription=l_DDLDescriptionInterface->GetChild("ScalableColor");
    if (!SubDescription.isNull()) {
      m_ScalableColorDescriptorInterface->
	ImportDDL(SubDescription.GetInterface());
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
void GoFGoPColorDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void GoFGoPColorDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC *GoFGoPColorDescriptor::GetInterface(void)
{ return &m_Interface; }


