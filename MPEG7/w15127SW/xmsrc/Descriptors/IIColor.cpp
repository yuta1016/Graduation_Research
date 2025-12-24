///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  IIColor.cpp
//

#include <stdio.h>
#include "Descriptors.h"

using namespace XM;

const UUID IIColorDescriptorInterface::myID = UUID("");
const char *IIColorDescriptorInterface::myName = 
"IIColorDescriptionInterface";

const UUID IIColorDescriptor::myID = UUID("");
const char *IIColorDescriptor::myName = "IIColorDescriptor";

const UUID IIColorDescriptor::valID = UUID("");
const char *IIColorDescriptor::valName = "IIColor";

const UUID IIColorDescriptorInterfaceABC::myID = UUID();

IIColorDescriptorInterfaceABC::IIColorDescriptorInterfaceABC()
{ 
	SetInterfaceID(myID);
}

//=============================================================================
IIColorDescriptorInterface::
IIColorDescriptorInterface(IIColorDescriptor *aCTBrowse):
m_IIColorDescriptor(aCTBrowse)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
IIColorDescriptorInterface::~IIColorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void IIColorDescriptorInterface::addref()
{ m_IIColorDescriptor->addref(); }

//----------------------------------------------------------------------------
void IIColorDescriptorInterface::release()
{ m_IIColorDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& IIColorDescriptorInterface::GetInterfaceID()
{ return myID; }

//----------------------------------------------------------------------------
const char *IIColorDescriptorInterface::GetName()
{ return myName; }

//----------------------------------------------------------------------------
int* IIColorDescriptorInterface::GetIIColor_Component()
{ return m_IIColorDescriptor->GetIIColor_Component(); }

//----------------------------------------------------------------------------
void IIColorDescriptorInterface::SetIIColor_Component(int* PBC)
{ m_IIColorDescriptor->SetIIColor_Component(PBC); }

//----------------------------------------------------------------------------
unsigned long IIColorDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_IIColorDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long IIColorDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_IIColorDescriptor->ImportDDL(aDescription); }

//=============================================================================
IIColorDescriptor::IIColorDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
IIColorDescriptor::~IIColorDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"IIColor delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& IIColorDescriptor::GetObjectID()
{ return myID; }

//----------------------------------------------------------------------------
const char *IIColorDescriptor::GetName()
{ return myName; }

//----------------------------------------------------------------------------
const UUID& IIColorDescriptor::GetValueID()
{ return valID; }

//----------------------------------------------------------------------------
const char* IIColorDescriptor::GetValueName()
{ return valName; }

//----------------------------------------------------------------------------
bool IIColorDescriptor::IsProprietary()
{ return m_isProprietary; }

//----------------------------------------------------------------------------
int* IIColorDescriptor::GetIIColor_Component()
{ return (m_IIColor_Component); }

//----------------------------------------------------------------------------
void IIColorDescriptor::SetIIColor_Component(int* PBC)
{
	int i;
	m_IIColor_Component = new int [2];
	for(i=0;i<2;i++){
		this->m_IIColor_Component[i]=PBC[i];
	}
}

//----------------------------------------------------------------------------
unsigned long IIColorDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  int temp;
  GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

/*  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","IIColorType");

  GenericDS CategoryDescription = l_DDLDescription.CreateChild("BrowsingCategory");
  temp = GetIIColor_Component()[0];
  if (temp == 0)
	  CategoryDescription.SetValue("hot");
  else if (temp == 1)
	  CategoryDescription.SetValue("warm");
  else if (temp == 2)
	  CategoryDescription.SetValue("moderate");
  else if (temp == 3)
	  CategoryDescription.SetValue("cool");
  else {
	  printf("Unable to export to DDL, Wrong Value\n");
	  exit(0);
  }

  GenericDS SubRangeIndexDescription = l_DDLDescription.CreateChild("SubRangeIndex");
  temp = GetIIColor_Component()[1];
  SubRangeIndexDescription.SetValue(temp); */

  return 0;
}

//----------------------------------------------------------------------------
unsigned long IIColorDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "IIColorType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for IIColor as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="IIColorType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* IIColor not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* IIColor found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

 /* int feature[2];
  string temp;
  int tmp;

  GenericDS CategoryDescription = l_DDLDescriptionInterface->GetDescription
	  ("BrowsingCategory");
  CategoryDescription.GetTextValue(temp);
  if (temp == "hot") feature[0] = 0;
  else if (temp == "warm") feature[0] = 1;
  else if (temp == "moderate") feature[0] = 2;
  else if (temp == "cool") feature[0] = 3;
  else printf("Can't read the XML file, Wrong element value\n");

  GenericDS SubRangeIndexDescription = l_DDLDescriptionInterface->GetDescription
	  ("SubRangeIndex");
  SubRangeIndexDescription.GetIntValue(tmp);
  feature[1] = tmp;

  SetIIColor_Component(feature); */
  
  return 0;
}

//----------------------------------------------------------------------------
void IIColorDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void IIColorDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
IIColorDescriptorInterfaceABC *IIColorDescriptor::GetInterface(void)
{ return &m_Interface; }


