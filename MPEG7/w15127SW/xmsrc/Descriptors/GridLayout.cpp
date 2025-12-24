///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Akio YAMADA NEC Corp.
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
// Applicable File Name:  GridLayout.cpp
//

#include <stdio.h>
#include <cstring>

#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID GridLayoutDescriptorInterface::myID = UUID("");
const char *GridLayoutDescriptorInterface::myName =
                         "GridLayoutDescriptionInterface";

const UUID GridLayoutDescriptor::myID = UUID("");
const char *GridLayoutDescriptor::myName = "GridLayoutDescriptor";

const UUID GridLayoutDescriptor::valID = UUID("");
const char *GridLayoutDescriptor::valName = "GridLayout";

const UUID GridLayoutDescriptorInterfaceABC::myID = UUID();

GridLayoutDescriptorInterfaceABC::GridLayoutDescriptorInterfaceABC()
{ SetInterfaceID(myID); }

//=============================================================================
GridLayoutDescriptorInterface::
GridLayoutDescriptorInterface(GridLayoutDescriptor *aGrid):
m_Descriptor(aGrid)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GridLayoutDescriptorInterface::~GridLayoutDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void GridLayoutDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void GridLayoutDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& GridLayoutDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GridLayoutDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
GL_SUB_DESCRIPTOR_INTERFACE *GridLayoutDescriptorInterface::
GetContents(const int x_pos, const int y_pos)
{ return m_Descriptor->GetContents(x_pos, y_pos); }

//----------------------------------------------------------------------------
void GridLayoutDescriptorInterface::
DeleteContents(const int x_pos, const int y_pos)
{ m_Descriptor->DeleteContents(x_pos, y_pos); }

//----------------------------------------------------------------------------
int GridLayoutDescriptorInterface::
SetContents(const int x_pos, const int y_pos, GL_SUB_DESCRIPTOR_INTERFACE *content)
{ return m_Descriptor->SetContents(x_pos, y_pos, content); }

//----------------------------------------------------------------------------
int GridLayoutDescriptorInterface::GetPartNumberH(void)
{ return m_Descriptor->GetPartNumberH(); }

//----------------------------------------------------------------------------
int GridLayoutDescriptorInterface::GetPartNumberV(void)
{ return m_Descriptor->GetPartNumberV(); }

//----------------------------------------------------------------------------
void GridLayoutDescriptorInterface::
SetPartNumber(const int part_number_h, const int part_number_v)
{ m_Descriptor->SetPartNumber(part_number_h, part_number_v); }

//----------------------------------------------------------------------------
char *GridLayoutDescriptorInterface::GetContentName()
{ return m_Descriptor->GetContentName(); }


//----------------------------------------------------------------------------
unsigned long GridLayoutDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long GridLayoutDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
GridLayoutDescriptor::GridLayoutDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ContentDescriptors(0),
m_part_number_h(0),
m_part_number_v(0)
{
  SetExposedInterface(&m_Interface);
  strcpy(m_content_name, "");
}

//----------------------------------------------------------------------------
GridLayoutDescriptor::~GridLayoutDescriptor()
{
	SetPartNumber(0, 0);
#ifdef VERBOSE
  fprintf(stderr,"GridLayoutType delete (LaBale Type)\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& GridLayoutDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GridLayoutDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GridLayoutDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GridLayoutDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GridLayoutDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
GL_SUB_DESCRIPTOR_INTERFACE *GridLayoutDescriptor::GetContents(const int x_pos, const int y_pos)
{ 
    if(x_pos>=m_part_number_h) return 0;
    if(y_pos>=m_part_number_v) return 0;
    return m_ContentDescriptors[x_pos+y_pos*m_part_number_h];
}

//----------------------------------------------------------------------------
void GridLayoutDescriptor::DeleteContents(const int x_pos, const int y_pos)
{
    if(x_pos>=m_part_number_h) return;
    if(y_pos>=m_part_number_v) return;
	
	if(m_ContentDescriptors[x_pos+y_pos*m_part_number_h])
		m_ContentDescriptors[x_pos+y_pos*m_part_number_h]->release();
}

//----------------------------------------------------------------------------
int GridLayoutDescriptor::
	SetContents(const int x_pos, const int y_pos, GL_SUB_DESCRIPTOR_INTERFACE *content)
{
    // check the partion position
    if(x_pos>=m_part_number_h) return 1;
    if(y_pos>=m_part_number_v) return 1;

    //const char *name=content->GetName();
	if(strcmp(m_content_name, "")==0) strcpy(m_content_name, GL_SUB_DESCRIPTOR_TYPE);
    //else if(strcmp(name, m_content_name)) return 2;

	m_ContentDescriptors[x_pos+y_pos*m_part_number_h]=content;
	content->addref();

	return 0;
}

//----------------------------------------------------------------------------
int GridLayoutDescriptor::GetPartNumberH(void)
{ return m_part_number_h; }

//----------------------------------------------------------------------------
int GridLayoutDescriptor::GetPartNumberV(void)
{ return m_part_number_v; }

//----------------------------------------------------------------------------
void GridLayoutDescriptor::
SetPartNumber(const int part_number_h, const int part_number_v)
{
    if(m_ContentDescriptors)
	{
		int i;
		for(i = 0; i < (m_part_number_h*m_part_number_v); i++)
		{
			if(m_ContentDescriptors[i])
				m_ContentDescriptors[i]->release();
		}
		delete m_ContentDescriptors;
		m_ContentDescriptors = 0;
	}
	m_part_number_h=part_number_h;
	m_part_number_v=part_number_v;
	if(m_part_number_h > 0 && m_part_number_v > 0)
	{
		m_ContentDescriptors = new GL_SUB_DESCRIPTOR_INTERFACE *[m_part_number_h*m_part_number_v];
		for(int i=0; i<m_part_number_h*m_part_number_v; i++) m_ContentDescriptors[i]=0;
	}
}

//----------------------------------------------------------------------------
char *GridLayoutDescriptor::GetContentName()
{
    return m_content_name;
}

//----------------------------------------------------------------------------
void GridLayoutDescriptor::addref(void)
{
  m_refcount++;
}

//----------------------------------------------------------------------------
void GridLayoutDescriptor::release(void)
{
  if (!(--m_refcount))
	  delete this;
}


//----------------------------------------------------------------------------
GridLayoutDescriptorInterfaceABC *GridLayoutDescriptor::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
unsigned long GridLayoutDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	//GenericDS l_DDLDescription;

	if (!aParentDescription) return (unsigned long) -1;

	int part_y = GetPartNumberV();
	int part_x = GetPartNumberH();
	aParentDescription->SetAttribute("numOfPartX", part_x);
	aParentDescription->SetAttribute("numOfPartY", part_y);

	bool use_mask = false;	// use the mask only if needed
	char *descriptor_mask=new char[part_x*part_y+1];
	int x, y;
	for(y=0; y<part_y; y++){
		for(x=0; x<part_x; x++){
			if(GetContents(x, y)){
				descriptor_mask[y*part_y+x]='1';
			} else {
				use_mask = true;
				descriptor_mask[y*part_x+x]='0';
			}
		}
	}
	if(use_mask)
	{
		descriptor_mask[part_x*part_y]='\0';
		string mask(descriptor_mask);
		aParentDescription->SetAttribute("descriptorMask", mask);
	}
	delete []descriptor_mask;

	// node: It is strongly requested to harmonize the name of each descriptor; Descriptor::valName
	char *content_name=GetContentName();
	int descriptor_id=0xffff;
	if(strcmp(content_name, "DominantColorType")==0) descriptor_id=ID_DominantColor;
	else if(strcmp(content_name, "ScalableColorType")==0) descriptor_id=ID_ScalableColor;
	else if(strcmp(content_name, "ColorLayoutType")==0) descriptor_id=ID_ColorLayout;
	else if(strcmp(content_name, "ColorStructureType")==0) descriptor_id=ID_ColorStructure;
	else if(strcmp(content_name, "GoFColorType")==0) descriptor_id=ID_GoFGoPColor;
	else if(strcmp(content_name, "HomogeneousTextureType")==0) descriptor_id=ID_HomogeneousTexture;
	else if(strcmp(content_name, "TextureBrowsingType")==0) descriptor_id=ID_TextureBrowsing;
	else if(strcmp(content_name, "EdgeHistogramType")==0) descriptor_id=ID_EdgeHistogram;
	else if(strcmp(content_name, "RegionShapeType")==0) descriptor_id=ID_RegionShape;
	else if(strcmp(content_name, "ContourShapeType")==0) descriptor_id=ID_ContourShape;
	else if(strcmp(content_name, "Shape3DType")==0) descriptor_id=ID_Shape3D;
	else {
		fprintf(stderr, "Unknown name(%s)\n", content_name);
		exit(0);
	}

	for(y=0; y<part_y; y++){
		for(x=0; x<part_x; x++){
			if(GetContents(x, y)){
				GetContents(x, y)->ExportDDL(aParentDescription);
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long GridLayoutDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int val1, val2;
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "GridLayout") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for DummyType as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="GridLayout") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* DummyType not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* DummyType found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  l_DDLDescriptionInterface->GetIntAttribute("parNumX",val1);
  l_DDLDescriptionInterface->GetIntAttribute("parNumY",val2);
  SetPartNumber(val1, val2);

  string mask;
  l_DDLDescriptionInterface->GetTextAttribute("descriptorMask", mask);
  int x, y;
  int entry_flag=1;
  for(y=0; y<val2; y++){
    for(x=0; x<val1; x++){
      if(mask[y*val1+x]=='1'){
	GenericDS theDDLDescription;
        if(entry_flag){
	  theDDLDescription=l_DDLDescriptionInterface->GetDescription("Descriptor");
	  // akio's memo: this should be descriptors, however, it is not support XM framework
	  entry_flag=0;
	} else {
	  theDDLDescription=l_DDLDescriptionInterface->GetNextSibling("Descriptor");
	}
	theDDLDescription.GetTextAttribute("xsi:type", xsitype);
	if(xsitype==GL_SUB_DESCRIPTOR_TYPE){
		GL_SUB_DESCRIPTOR *ptr= new GL_SUB_DESCRIPTOR();
		ptr->ImportDDL(theDDLDescription.GetInterface());
		SetContents(x, y, ptr->GetInterface());
	} else {
	  // when you would like to support other Ds, please refer the above sample  
	  fprintf(stderr, "DDL I/O functions support only %s\n", GL_SUB_DESCRIPTOR_TYPE);
	  return 1;
	}
      }
    }
  }
  return 0;
}


