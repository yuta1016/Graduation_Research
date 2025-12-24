///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California Santa Barbara.
// Samsung, Yun Ju Yu, Yang Lim Choi.
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
// Applicable File Name:  TextureBrowsing.cpp
//
#include <stdio.h>
#include <string.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID TextureBrowsingDescriptorInterface::myID = UUID("");
const char *TextureBrowsingDescriptorInterface::myName = 
"TextureBrowsingDescriptionInterface";

const UUID TextureBrowsingDescriptor::myID = UUID("");
const char *TextureBrowsingDescriptor::myName = "TextureBrowsingDescriptor";

const UUID TextureBrowsingDescriptor::valID = UUID("");
const char *TextureBrowsingDescriptor::valName = "";

const UUID TextureBrowsingDescriptorInterfaceABC::myID = UUID();

TextureBrowsingDescriptorInterfaceABC::TextureBrowsingDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TextureBrowsingDescriptorInterface::
TextureBrowsingDescriptorInterface(TextureBrowsingDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterface::~TextureBrowsingDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void TextureBrowsingDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void TextureBrowsingDescriptorInterface::release(void)
{ m_Descriptor->release(); }


//----------------------------------------------------------------------------
const UUID& TextureBrowsingDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingDescriptorInterface::GetName(void)
{ return myName; }

//---yjyu - 010218
//----------------------------------------------------------------------------
int TextureBrowsingDescriptorInterface::GetComponentNumberFlag()
{
	return m_Descriptor->GetComponentNumberFlag();
}

//----------------------------------------------------------------------------
int* TextureBrowsingDescriptorInterface::GetBrowsing_Component()	
{
	return m_Descriptor->GetBrowsing_Component();
}

//-----------------------------------------------------------------------------
void TextureBrowsingDescriptorInterface::
SetComponentNumberFlag(int ComponentNumber)
{
    m_Descriptor->
      SetComponentNumberFlag(ComponentNumber);
}
//----------------------------------------------------------------------------
void TextureBrowsingDescriptorInterface::SetBrowsing_Component(int* PBC)
{
    m_Descriptor->SetBrowsing_Component(PBC);
}

//----------------------------------------------------------------------------
unsigned long TextureBrowsingDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long TextureBrowsingDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
TextureBrowsingDescriptor::TextureBrowsingDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
TextureBrowsingDescriptor::~TextureBrowsingDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"TextureBrowsing delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& TextureBrowsingDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TextureBrowsingDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TextureBrowsingDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TextureBrowsingDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
unsigned long TextureBrowsingDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	// 2013/07/26 modified (XML Names)
  int temp;
  GenericDS l_DDLDescription;
  
  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","TextureBrowsingType");

  GenericDS RegularityDescription = l_DDLDescription.CreateChild("Regularity");
  temp = GetBrowsing_Component()[0]; 
  if (temp == 1)
	RegularityDescription.SetValue("irregular");
  else if (temp == 2)
	RegularityDescription.SetValue("slightlyRegular");
  else if (temp == 3)
	RegularityDescription.SetValue("regular");
  else if (temp == 4)
	RegularityDescription.SetValue("highlyRegular");
  else {
	  printf("Unable to export to DDL, Wrong Value\n");
	  exit (0);
  }

  GenericDS FirstDirectionDescription = l_DDLDescription.CreateChild("Direction");
  temp = GetBrowsing_Component()[1]; 
  if (temp == 0)
	FirstDirectionDescription.SetValue("NoDirectionality");
  else if (temp == 1) 
	FirstDirectionDescription.SetValue("0Degree");
  else if (temp == 2)
	FirstDirectionDescription.SetValue("30Degree");
  else if (temp == 3)
	FirstDirectionDescription.SetValue("60Degree");
  else if (temp == 4) 
	FirstDirectionDescription.SetValue("90Degree");
  else if (temp == 5) 
	FirstDirectionDescription.SetValue("120Degree");
  else if (temp == 6)
	FirstDirectionDescription.SetValue("150Degree");
  else {
	printf("Unable to export to DDL, Wrong Value\n");
	exit (0);
  }

  GenericDS FirstScaleDescription = l_DDLDescription.CreateChild("Scale");
  temp = GetBrowsing_Component()[2]; 
  if (temp == 1)
	FirstScaleDescription.SetValue("fine");
  else if (temp == 2)
	FirstScaleDescription.SetValue("medium");
  else if (temp == 3)
	FirstScaleDescription.SetValue("coarse");
  else if (temp == 4)
	FirstScaleDescription.SetValue("veryCoarse");
  else {
	printf("Unable to export to DDL, Wrong Value\n");
	exit (0);
  }

  if ( GetComponentNumberFlag() != 0) {

	GenericDS SecondDirectionDescription = l_DDLDescription.CreateChild("Direction");
	temp = GetBrowsing_Component()[3]; 
	if (temp == 0)
	  SecondDirectionDescription.SetValue("NoDirectionality");
    else if (temp == 1)
      SecondDirectionDescription.SetValue("0Degree");
    else if (temp == 2)
	  SecondDirectionDescription.SetValue("30Degree");
    else if (temp == 3)
	  SecondDirectionDescription.SetValue("60Degree");
    else if (temp == 4)
	  SecondDirectionDescription.SetValue("90Degree");
    else if (temp == 5)
	  SecondDirectionDescription.SetValue("120Degree");
    else if (temp == 6)
	  SecondDirectionDescription.SetValue("150Degree");
    else {
	  printf("Unable to export to DDL, Wrong Value\n");
	  exit (0);
	}

    GenericDS SecondScaleDescription = l_DDLDescription.CreateChild("Scale");
    temp = GetBrowsing_Component()[4]; 
    if (temp == 1)
	  SecondScaleDescription.SetValue("fine");
    else if (temp == 2)
	  SecondScaleDescription.SetValue("medium");
    else if (temp == 3)
	  SecondScaleDescription.SetValue("coarse");
    else if (temp == 4)
	  SecondScaleDescription.SetValue("veryCoarse");
    else {
	  printf("Unable to export to DDL, Wrong Value\n");
	  exit (0);
	}
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long TextureBrowsingDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
	// 2013/07/26 modified (XML Names)
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "TextureBrowsingType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

    /* else search for TextureBrowingType as a child*/

  if (!l_DDLDescriptionInterface) { 
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="HomogeneousTextureType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      
    /* TextureBrowsingType not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* HomogeneousTextureType found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  int component_number_flag, feature[5];
  string temp; 

  GenericDS RegularityDescription = l_DDLDescriptionInterface->GetDescription("Regularity");
  RegularityDescription.GetTextValue(temp);
  if (temp =="irregular") feature[0] = 1;
  else if (temp =="slightlyRegular") feature[0]= 2;
  else if (temp =="regular") feature[0]= 3;
  else if (temp =="highlyRegular") feature[0]= 4;
  else printf(" Can't read the XML file, Wrong element value\n");
  
  GenericDS firstdirectionDescription = l_DDLDescriptionInterface->GetDescription("Direction");
  firstdirectionDescription.GetTextValue(temp);
  if (temp =="No directionality") feature[1] = 0;
  else if (temp =="0Degree") feature[1]= 1;
  else if (temp =="30Degree") feature[1]= 2;
  else if (temp =="60Degree") feature[1]= 3;
  else if (temp =="90Degree") feature[1]= 4;
  else if (temp =="120Degree") feature[1]= 5;
  else if (temp =="150Degree") feature[1]= 6;
  else printf(" Can't read the XML file, Wrong element value\n");

  GenericDS firstscaleDescription = l_DDLDescriptionInterface->GetDescription("Scale");
  firstscaleDescription.GetTextValue(temp);
  if (temp =="fine") feature[2] = 1;
  else if (temp =="medium") feature[2]= 2;
  else if (temp =="coarse") feature[2]= 3;
  else if (temp =="veryCoarse") feature[2]= 4;
  else printf(" Can't read the XML file, Wrong element value\n");
  
  GenericDS seconddirectionDescription = l_DDLDescriptionInterface->GetDescription("Direction",1);

  if (!seconddirectionDescription.isNull()) {

    component_number_flag = 1;

	seconddirectionDescription.GetTextValue(temp);
	if (temp =="NoDirectionality") feature[3] = 0;
    else if (temp =="0Degree") feature[3]= 1;
    else if (temp =="30Degree") feature[3]= 2;
    else if (temp =="60Degree") feature[3]= 3;
    else if (temp =="90Degree") feature[3]= 4;
    else if (temp =="120Degree") feature[3]= 5;
    else if (temp =="150Degree") feature[3]= 6;
    else printf(" Can't read the XML file, Wrong element value\n");

    GenericDS secondscaleDescription = l_DDLDescriptionInterface->GetDescription("Scale",1);
    secondscaleDescription.GetTextValue(temp);
    if (temp =="fine") feature[4] = 1;
    else if (temp =="medium") feature[4]= 2;
    else if (temp =="coarse") feature[4]= 3;
    else if (temp =="veryCoarse") feature[4]= 4;
    else printf(" Can't read the XML file, Wrong element value\n");
  }
  else {
	 component_number_flag = 0;
	 feature[3] = 0;
	 feature[4] = 0;
  }
  SetComponentNumberFlag(component_number_flag);
  SetBrowsing_Component(feature);

  return 0;
}

//----------------------------------------------------------------------------
void TextureBrowsingDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void TextureBrowsingDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}

//---yjyu - 010218
//----------------------------------------------------------------------------
int TextureBrowsingDescriptor::GetComponentNumberFlag()
{
	return (m_ComponentNumberFlag);
}
//----------------------------------------------------------------------------
int* TextureBrowsingDescriptor::GetBrowsing_Component()
{
	return (m_Browsing_Component);
}
//----------------------------------------------------------------------------
void TextureBrowsingDescriptor::
SetComponentNumberFlag(int ComponentNumber)
{
	m_ComponentNumberFlag=ComponentNumber;
}
//----------------------------------------------------------------------------
void TextureBrowsingDescriptor::SetBrowsing_Component(int *PBC)
{
  int i; 
  m_Browsing_Component=new int [5];
  for(i=0;i<5;++i){ 
  this->m_Browsing_Component[i]=PBC[i];
  }
}
//---

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC *TextureBrowsingDescriptor::GetInterface(void)
{ return &m_Interface; }


