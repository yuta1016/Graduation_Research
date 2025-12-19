///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich Univerisity of Tehcnology
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
// Applicable File Name:  MultiImage.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID MultiImageDescriptorInterface::myID = UUID("");
const char *MultiImageDescriptorInterface::myName = 
"MultiImageDescriptionInterface";

const UUID MultiImageDescriptor::myID = UUID("");
const char *MultiImageDescriptor::myName = "MultiImageDescriptor";

const UUID MultiImageDescriptor::valID = UUID("");
const char *MultiImageDescriptor::valName = "";

const UUID MultiImageDescriptorInterfaceABC::myID = UUID();

MultiImageDescriptorInterfaceABC::MultiImageDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
MultiImageDescriptorInterface::
MultiImageDescriptorInterface(MultiImageDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiImageDescriptorInterface::~MultiImageDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void MultiImageDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void MultiImageDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& MultiImageDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC
*MultiImageDescriptorInterface::GetScalableColorDescriptorInterface(void)
{
  return m_Descriptor->GetScalableColorDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptorInterface::
SetScalableColorDescriptorInterface(ScalableColorDescriptorInterfaceABC
				 *aScalableColorDescriptorInterface)	
{
  return m_Descriptor->
    SetScalableColorDescriptorInterface(aScalableColorDescriptorInterface);
}

//----------------------------------------------------------------------------
ContourShapeDescriptorInterfaceABC
*MultiImageDescriptorInterface::GetContourShapeDescriptorInterface(void)
{
  return m_Descriptor->GetContourShapeDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptorInterface::
SetContourShapeDescriptorInterface(ContourShapeDescriptorInterfaceABC
				 *aContourShapeDescriptorInterface)	
{
  return m_Descriptor->
    SetContourShapeDescriptorInterface(aContourShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
MultiImageDescriptor::MultiImageDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ScalableColorDescriptorInterface(NULL),
m_ContourShapeDescriptorInterface(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MultiImageDescriptor::
MultiImageDescriptor(ScalableColorDescriptorInterfaceABC
		     *aScalableColorInterface,
		     ContourShapeDescriptorInterfaceABC
		     *aContourShapeInterface):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ScalableColorDescriptorInterface(NULL),
m_ContourShapeDescriptorInterface(NULL)
{
#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!aScalableColorInterface) {
    ScalableColorDescriptor *aScalableColor;
    aScalableColor= new ScalableColorDescriptor();
    aScalableColorInterface=aScalableColor->GetInterface();
  }
  if (aScalableColorInterface)
    SetScalableColorDescriptorInterface(aScalableColorInterface);

  if (!aContourShapeInterface) {
    ContourShapeDescriptor *aContourShape;
    aContourShape= new ContourShapeDescriptor;
    aContourShapeInterface=aContourShape->GetInterface();
  }
  if (aContourShapeInterface)
    SetContourShapeDescriptorInterface(aContourShapeInterface);
}

//----------------------------------------------------------------------------
MultiImageDescriptor::~MultiImageDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"MultiImage delete\n");
#endif
  if (m_ScalableColorDescriptorInterface)
    m_ScalableColorDescriptorInterface->release();
  if (m_ContourShapeDescriptorInterface)
    m_ContourShapeDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& MultiImageDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MultiImageDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MultiImageDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MultiImageDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC *MultiImageDescriptor::
GetScalableColorDescriptorInterface(void)
{
  return m_ScalableColorDescriptorInterface;
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptor::SetScalableColorDescriptorInterface
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
ContourShapeDescriptorInterfaceABC *MultiImageDescriptor::
GetContourShapeDescriptorInterface(void)
{
  return m_ContourShapeDescriptorInterface;
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptor::SetContourShapeDescriptorInterface
        (ContourShapeDescriptorInterfaceABC *aContourShape)	
{
  if (!aContourShape) return (unsigned long) -1;
  if (m_ContourShapeDescriptorInterface == aContourShape) return 0;
  if (m_ContourShapeDescriptorInterface)
    m_ContourShapeDescriptorInterface->release();
  m_ContourShapeDescriptorInterface = aContourShape;
  if (m_ContourShapeDescriptorInterface)
    m_ContourShapeDescriptorInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS MultiImageDDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  MultiImageDDLDescription=aParentDescription->CreateChild("MultiImage");

  if (m_ScalableColorDescriptorInterface) {
    m_ScalableColorDescriptorInterface->
      ExportDDL(MultiImageDDLDescription.GetInterface());
  }
  if (m_ContourShapeDescriptorInterface) {
    m_ContourShapeDescriptorInterface->
      ExportDDL(MultiImageDDLDescription.GetInterface());
  }
  return 0;
}

//----------------------------------------------------------------------------
unsigned long MultiImageDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  if (!aDescription) return (unsigned long) -1;
  
  GenericDS SubDescription;
  if (m_ScalableColorDescriptorInterface) {
    SubDescription=aDescription->GetDescription("ScalableColor",0);
    if (!SubDescription.isNull()) {
      m_ScalableColorDescriptorInterface->
	ImportDDL(SubDescription.GetInterface());
    }
  }
  if (m_ContourShapeDescriptorInterface) {
    SubDescription=aDescription->GetDescription("ContourShape",0);
    if (!SubDescription.isNull()) {
      m_ContourShapeDescriptorInterface->
      	ImportDDL(SubDescription.GetInterface());
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
void MultiImageDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void MultiImageDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC *MultiImageDescriptor::GetInterface(void)
{ return &m_Interface; }


