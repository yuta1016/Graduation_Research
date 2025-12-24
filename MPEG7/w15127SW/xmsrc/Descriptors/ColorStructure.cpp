////////////////////////////////////////////////////////////////////////
//
// ColorStructure.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2015.
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <assert.h>
#include "Descriptors/Descriptors.h"
#include "Utilities/readparam.h"

#include <string>	// GenericDS uses <string>, not <string.h>
#include <vector>
#ifdef WIN32
//#include <strstream.h>	// <strstream> conflicts with <stdio.h>
#include <strstream>
#else
#include <strstream>
#endif

using namespace XM;

const UUID ColorStructureDescriptorInterface::myID = UUID("");
const char *ColorStructureDescriptorInterface::myName = 
"ColorStructureDescriptionInterface";

const UUID ColorStructureDescriptor::myID = UUID("");
const char *ColorStructureDescriptor::myName = "ColorStructureDescriptor";

const UUID ColorStructureDescriptor::valID = UUID("");
const char *ColorStructureDescriptor::valName = "";

const UUID ColorStructureDescriptorInterfaceABC::myID = UUID();

ColorStructureDescriptorInterfaceABC::ColorStructureDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ColorStructureDescriptorInterface::
ColorStructureDescriptorInterface(ColorStructureDescriptor *aHisto):
m_Descriptor(aHisto)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorStructureDescriptorInterface::~ColorStructureDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void ColorStructureDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void ColorStructureDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& ColorStructureDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptorInterface::GetSize(void)
{ return m_Descriptor->GetSize(); }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptorInterface::
	SetSize(unsigned long size)	
{ return m_Descriptor->SetSize(size); }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptorInterface::
	GetElement(unsigned long index)
{ return m_Descriptor->GetElement(index); }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptorInterface::
	SetElement(unsigned long index, int value)
{ return m_Descriptor->SetElement(index, value); }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
ColorStructureDescriptor::ColorStructureDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_Data(0),
m_Size(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorStructureDescriptor::~ColorStructureDescriptor()
{
	if (m_Data)
		delete(m_Data);
}

//----------------------------------------------------------------------------
const UUID& ColorStructureDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorStructureDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorStructureDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ColorStructureDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::GetSize(void)
{ return m_Size; }

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::SetSize(unsigned long size)	
{
	if (size == 32 || size == 64 || size == 128 || size == 256)
	{
		if (m_Size == size)
			return 0;
		if (m_Data)
			delete m_Data;
		m_Data = new unsigned long[size];
		if (!m_Data)
		{
			std::cerr << "Allocation error SetSize()" << std::endl;
			exit(0);
		}

		m_Size = size; 
		return 0;
	}

	else
	{
		std::cerr << "Invalid SetSize(" << size << ")";
		exit(0);
		return (unsigned long) -1;
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::GetElement(unsigned long index)
{
	if (index < m_Size && m_Data)
		return m_Data[index];

	else
	{
		std::cerr << "Out of bounds GetElement(" << index << ")" << std::endl;
		exit(0);
		return (unsigned long) -1;
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::
SetElement(unsigned long index, int value)
{
	if (index < m_Size && m_Data)
	{
		m_Data[index] = value;
		return 0;
	}

	else
	{
		std::cerr << "Out of bounds SetElement(" << index << ",...)" << std::endl;
		exit(0);
		return (unsigned long) -1;
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	if(!aParentDescription) 
		return (unsigned long) -1;

	// Determine the coded size & color quantization setting
	int		v, size = GetSize();
	switch(size)
	{
		case  32:	v=1; break;
		case  64:	v=2; break;
		case 128:	v=3; break;
		case 256:	v=4; break;
		default:	v=0;
	}
	assert(v);

	
	GenericDS	l_DDLDescription;
	GenericDS	/*MultimediaContent, Image, VisualDescriptor,*/ Values;

	// changes according to m15122
	// Create repeating descriptor data
	//MultimediaContent = aParentDescription->CreateChild("MultimediaContent");
	//if (MultimediaContent.isNull())
	//	return (unsigned long) -1;
	//MultimediaContent.SetAttribute("xsi:type","ImageType");

	//Image = MultimediaContent.CreateChild("Image");
	//if (Image.isNull())
	//	return (unsigned long) -1;

	l_DDLDescription = aParentDescription->CreateChild("Descriptor");
	l_DDLDescription.SetAttribute("xsi:type","ColorStructureType");
	l_DDLDescription.SetAttribute("colorQuant", v);
	Values = l_DDLDescription.CreateChild("Values");

	/*VisualDescriptor = Image.CreateChild("VisualDescriptor");
	if (VisualDescriptor.isNull())
		return (unsigned long) -1;
	VisualDescriptor.SetAttribute("xsi:type","ColorStructureType");
	VisualDescriptor.SetAttribute("colorQuant", v);

	Values = VisualDescriptor.CreateChild("Values");
	if (Values.isNull())
		return (unsigned long) -1;
		*/

	// Build values vector
	vector<int> buf(size);
	for (int i = 0; i< size; i++)
		buf[i] = GetElement(i);

	// Set list value into element
	Values.SetValue(buf);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
        int /*v,*/ size;
	GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
	// m15122
	//GenericDS	Image, VisualDescriptor, Values;
	GenericDS	l_DDLDescription, Values;
	string xsitype;

	if(!aDescription) return (unsigned long) -1;

	//sth this looks different in DummyType!!
	// if aDescription is of correct type
	// m15122
	//if(aDescription->GetDSName() == "MultimediaContent") 
	if(aDescription->GetDSName() == "Descriptor") 
	{
		// m15122
		//string xsitype;
		aDescription->GetTextAttribute("xsi:type", xsitype);
		//if(xsitype == "ImageType")
		if(xsitype == "ColorStructureType") 
			l_DDLDescriptionInterface = aDescription;
	}

	// m15122
	/*
	// ImageType not found
	if(l_DDLDescriptionInterface->isNull())
	  return (unsigned long) -1;

	Image = l_DDLDescriptionInterface->GetChild(0);
	if (Image.isNull())
		return (unsigned long) -1;
	VisualDescriptor = Image.GetChild(0);
	if (VisualDescriptor.isNull())
		return (unsigned long) -1;
	Values = VisualDescriptor.GetChild(0);
	*/
 	// Descriptor not found
 	if(l_DDLDescriptionInterface->isNull()) {
 		// else search for ColorStructureType as a child
 		l_DDLDescription = aDescription->GetDescription("Descriptor");
 
 		// search for correct xsi type
 		while(!l_DDLDescription.isNull()) {
 			l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
 			if(xsitype == "ColorStructureType") break;
 			l_DDLDescription.GetNextSibling("Descriptor");
 		}
 		// ColorStructureType not found
 		if(!l_DDLDescription.isNull()) return (unsigned long) -1;
 		// ColorStructureType found
 		l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
 	}
 
 	Values = l_DDLDescriptionInterface->GetDescription("Values");
	
	if (Values.isNull())
		return (unsigned long) -1;

	// Set size into descriptor
	// m15122
	//VisualDescriptor.GetIntAttribute("colorQuant", size);
	l_DDLDescriptionInterface->GetIntAttribute("colorQuant", size);
	size = 1 << (size + 4);
	SetSize((unsigned long)size);

	// Get data
	vector<int> buf;
	Values.GetIntVector(buf);
	for (int i = 0; i < buf.size() && i < size; i++)
		SetElement((unsigned long)i,buf[i]);

	return 0;
}

//----------------------------------------------------------------------------
void ColorStructureDescriptor::addref(void)

{
	m_refcount++;
}

//----------------------------------------------------------------------------
void ColorStructureDescriptor::release(void)

{
	if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
ColorStructureDescriptorInterfaceABC *ColorStructureDescriptor::GetInterface(void)
{ return &m_Interface; }


