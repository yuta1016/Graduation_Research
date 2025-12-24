///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim, Du-Sik Park, 
// Yanglim Choi
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
// Applicable File Name:  CTBrowsing.cpp
//

#include <stdio.h>
#include "Descriptors.h"

using namespace XM;

const UUID CTBrowsingDescriptorInterface::myID = UUID("");
const char *CTBrowsingDescriptorInterface::myName = 
"CTBrowsingDescriptionInterface";

const UUID CTBrowsingDescriptor::myID = UUID("");
const char *CTBrowsingDescriptor::myName = "CTBrowsingDescriptor";

const UUID CTBrowsingDescriptor::valID = UUID("");
const char *CTBrowsingDescriptor::valName = "CTBrowsing";

const UUID CTBrowsingDescriptorInterfaceABC::myID = UUID();

CTBrowsingDescriptorInterfaceABC::CTBrowsingDescriptorInterfaceABC()
{ 
	SetInterfaceID(myID);
}

//=============================================================================
CTBrowsingDescriptorInterface::
CTBrowsingDescriptorInterface(CTBrowsingDescriptor *aCTBrowse):
m_CTBrowsingDescriptor(aCTBrowse)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
CTBrowsingDescriptorInterface::~CTBrowsingDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void CTBrowsingDescriptorInterface::addref()
{ m_CTBrowsingDescriptor->addref(); }

//----------------------------------------------------------------------------
void CTBrowsingDescriptorInterface::release()
{ m_CTBrowsingDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& CTBrowsingDescriptorInterface::GetInterfaceID()
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingDescriptorInterface::GetName()
{ return myName; }

//----------------------------------------------------------------------------
int* CTBrowsingDescriptorInterface::GetCTBrowsing_Component()
{ return m_CTBrowsingDescriptor->GetCTBrowsing_Component(); }

//----------------------------------------------------------------------------
void CTBrowsingDescriptorInterface::SetCTBrowsing_Component(int* PBC)
{ m_CTBrowsingDescriptor->SetCTBrowsing_Component(PBC); }

//----------------------------------------------------------------------------
unsigned long CTBrowsingDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_CTBrowsingDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long CTBrowsingDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_CTBrowsingDescriptor->ImportDDL(aDescription); }

//=============================================================================
CTBrowsingDescriptor::CTBrowsingDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CTBrowsingDescriptor::~CTBrowsingDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"CTBrowsing delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingDescriptor::GetObjectID()
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingDescriptor::GetName()
{ return myName; }

//----------------------------------------------------------------------------
const UUID& CTBrowsingDescriptor::GetValueID()
{ return valID; }

//----------------------------------------------------------------------------
const char* CTBrowsingDescriptor::GetValueName()
{ return valName; }

//----------------------------------------------------------------------------
bool CTBrowsingDescriptor::IsProprietary()
{ return m_isProprietary; }

//----------------------------------------------------------------------------
int* CTBrowsingDescriptor::GetCTBrowsing_Component()
{ return (m_CTBrowsing_Component); }

//----------------------------------------------------------------------------
void CTBrowsingDescriptor::SetCTBrowsing_Component(int* PBC)
{
	int i;
	m_CTBrowsing_Component = new int [2];
	for(i=0;i<2;i++){
		this->m_CTBrowsing_Component[i]=PBC[i];
	}
}

//----------------------------------------------------------------------------
unsigned long CTBrowsingDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  int temp;
  GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","ColorTemperatureType");

  GenericDS CategoryDescription = l_DDLDescription.CreateChild("BrowsingCategory");
  temp = GetCTBrowsing_Component()[0];
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
  temp = GetCTBrowsing_Component()[1];
  SubRangeIndexDescription.SetValue(temp);

  return 0;
}

//----------------------------------------------------------------------------
unsigned long CTBrowsingDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "ColorTemperatureType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for CTBrowsing as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="ColorTemperatureType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* CTBrowsing not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* CTBrowsing found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  int feature[2];
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

  SetCTBrowsing_Component(feature);
  
  return 0;
}

//----------------------------------------------------------------------------
void CTBrowsingDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void CTBrowsingDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC *CTBrowsingDescriptor::GetInterface(void)
{ return &m_Interface; }


