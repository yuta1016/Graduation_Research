///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// (contributing organizations names)
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Descriptors.cpp
//
#include <stdio.h>
#include "Utilities/readparam.h"
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID ColorSpaceDescriptorInterface::myID = UUID("");
const char *ColorSpaceDescriptorInterface::myName =
                         "Color Space Description Interface";

const UUID ColorSpaceDescriptor::myID = UUID("");
const char *ColorSpaceDescriptor::myName = "Color Space Descriptor";

const UUID ColorSpaceDescriptor::valID = UUID("");
const char *ColorSpaceDescriptor::valName = "Color Space";

const UUID ColorSpaceDescriptorInterfaceABC::myID = UUID();

//=============================================================================
ColorSpaceDescriptorInterfaceABC::ColorSpaceDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ColorSpaceDescriptorInterface::ColorSpaceDescriptorInterface(ColorSpaceDescriptor *aColorSpace):
m_ColorSpaceDescriptor(aColorSpace)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorSpaceDescriptorInterface::~ColorSpaceDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& ColorSpaceDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorSpaceDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ColorSpaceDescriptorInterface::addref(void)
{ m_ColorSpaceDescriptor->addref(); }

//----------------------------------------------------------------------------
void ColorSpaceDescriptorInterface::release(void)
{ m_ColorSpaceDescriptor->release(); }

//----------------------------------------------------------------------------
void ColorSpaceDescriptorInterface::SetValuesFromParameterFile(void)
{
	int i, j;
	char tag[256];
	float value;

	m_ColorSpaceDescriptor->
		SetColorReferenceFlag((bool)atoi(getpara("ColorReferenceFlag","0")));

	m_ColorSpaceDescriptor->
		SetDescriptionColorSpace((ColorSpaceType)
		atoi(getpara("ColorSpace","0"))); // default = 0 (RGB)
	if(atoi(getpara("ColorSpace","0"))==LinearMatrix)
	{
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				sprintf(tag,"m%d%d",i,j);
				value = atof(getpara(tag,"1.0"));
				SetMatrixElement(i,j,value);
			}
		}
	}
#ifdef DEBUG
	fprintf(stderr,"Using: ColorSpace %d\n",(int)
	m_ColorSpaceDescriptor->
	GetDescriptionColorSpace());
#endif
}

//----------------------------------------------------------------------------
float *ColorSpaceDescriptorInterface::GetColorSpaceMatrix(void)
{
	return m_ColorSpaceDescriptor->GetColorSpaceMatrix();
}

//----------------------------------------------------------------------------
bool ColorSpaceDescriptorInterface::GetColorReferenceFlag(void)
{
	return m_ColorSpaceDescriptor->GetColorReferenceFlag();
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptorInterface::SetColorReferenceFlag(bool colorRefereceFlag)
{
	return m_ColorSpaceDescriptor->SetColorReferenceFlag(colorRefereceFlag);
}

//----------------------------------------------------------------------------
ColorSpaceType ColorSpaceDescriptorInterface::GetDescriptionColorSpace(void)
{
	return m_ColorSpaceDescriptor->GetDescriptionColorSpace();
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptorInterface::SetDescriptionColorSpace(ColorSpaceType colorspace)	
{
	return m_ColorSpaceDescriptor->SetDescriptionColorSpace(colorspace);
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptorInterface::SetMatrixElement(int xindex,
							      int yindex,
							      float value)
{
	return m_ColorSpaceDescriptor->SetMatrixElement(xindex, yindex, value);
}

//----------------------------------------------------------------------------
float ColorSpaceDescriptorInterface::GetMatrixElement(int xindex, int yindex)
{
	return m_ColorSpaceDescriptor->GetMatrixElement(xindex, yindex);
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_ColorSpaceDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_ColorSpaceDescriptor->ImportDDL(aDescription); }

//=============================================================================
ColorSpaceDescriptor::ColorSpaceDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ColorReferenceFlag(false),
m_DescriptionColorSpace(RGB),
m_ColorTransMatrix(/*{0,0,0},{0,0,0},{0,0,0}*/)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorSpaceDescriptor::~ColorSpaceDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"ColorSpace delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& ColorSpaceDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorSpaceDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorSpaceDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorSpaceDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ColorSpaceDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void ColorSpaceDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ColorSpace connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void ColorSpaceDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ColorSpace release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	int i, j;

	GenericDS ColSpc;
	vector<int> lin_mat(9);

	if(!aParentDescription) return (unsigned long) -1;

	ColSpc = aParentDescription->CreateChild("ColorSpace");
	switch(GetDescriptionColorSpace())
	{
		case RGB:	ColSpc.SetAttribute("type", "RGB");		break;
		case YCRCB:	ColSpc.SetAttribute("type", "YCbCr");	break;
		case HSV:	ColSpc.SetAttribute("type", "HSV");		break;
		case HMMD:	ColSpc.SetAttribute("type", "HMMD");	break;
		case LinearMatrix: ColSpc.SetAttribute("type", "LinearMatrix"); break;
		case Monochrome: ColSpc.SetAttribute("type", "Monochrome"); break;
		default: break;
	}

	if(GetColorReferenceFlag())
		ColSpc.SetAttribute("colorReferenceFlag","true");
	else
		ColSpc.SetAttribute("colorReferenceFlag","false");

	if(GetDescriptionColorSpace()==LinearMatrix)
	{
		GenericDS LinMat = ColSpc.CreateChild("ColorTransMat");
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				lin_mat[i*3+j] = (int)(16384.0*GetMatrixElement(i,j));
		LinMat.SetValue(lin_mat);
	}


	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
	int i, j;
	string text;
	GenericDS ColSpc;
	vector<int> lin_mat;

	if(!aDescription) return (unsigned long) -1;
	ColSpc = aDescription->GetDescription("ColorSpace");
	if(ColSpc.isNull()) return (unsigned long) -1;

	ColSpc.GetTextAttribute("type", text);
	if(text=="RGB")			SetDescriptionColorSpace((ColorSpaceType)RGB);
	else if(text=="YCbCr")	SetDescriptionColorSpace((ColorSpaceType)YCRCB);
	else if(text=="HSV")	SetDescriptionColorSpace((ColorSpaceType)HSV);
	else if(text=="HMMD")	SetDescriptionColorSpace((ColorSpaceType)HMMD);
	else if(text=="LinearMatrix") SetDescriptionColorSpace((ColorSpaceType)LinearMatrix);
	else if(text=="Monochrome") SetDescriptionColorSpace((ColorSpaceType)Monochrome);
	else					SetDescriptionColorSpace((ColorSpaceType)RGB);

	ColSpc.GetTextAttribute("colorReferenceFlag",text);
	if(text=="true") SetColorReferenceFlag(true);
	else SetColorReferenceFlag(false);

	if(GetDescriptionColorSpace()==LinearMatrix)
	{
		GenericDS LinMat = ColSpc.GetDescription("ColorTransMat");
		LinMat.GetIntVector(lin_mat);
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				SetMatrixElement(i,j,(float)lin_mat[i*3+j]/16384.0);
	}
	
	return 0;
}

//----------------------------------------------------------------------------
bool ColorSpaceDescriptor::GetColorReferenceFlag(void)
{
	return m_ColorReferenceFlag;
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptor::SetColorReferenceFlag(bool colorReferenceFlag)
{
	m_ColorReferenceFlag = colorReferenceFlag;

	return 0;
}

//----------------------------------------------------------------------------
ColorSpaceType ColorSpaceDescriptor::GetDescriptionColorSpace(void)
{
	return m_DescriptionColorSpace;
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptor::SetDescriptionColorSpace(ColorSpaceType colorspacetype)
{
        m_DescriptionColorSpace = colorspacetype;

	return 0;
}

//----------------------------------------------------------------------------
float *ColorSpaceDescriptor::GetColorSpaceMatrix(void)
{
	return &m_ColorTransMatrix[0][0];
}

//----------------------------------------------------------------------------
unsigned long ColorSpaceDescriptor::SetMatrixElement(int xindex, int yindex,
						     float value)
{
	if (xindex < 0 || xindex >= 3 || yindex < 0 || yindex >= 3)
		return (unsigned long)-1;
	m_ColorTransMatrix[xindex][yindex] = value;

	return 0;
}

//----------------------------------------------------------------------------
float ColorSpaceDescriptor::GetMatrixElement(int xindex,int yindex)
{
	if (xindex < 0 || xindex >= 3 || yindex < 0 || yindex >= 3)
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif

	return m_ColorTransMatrix[xindex][yindex];
}

//----------------------------------------------------------------------------
ColorSpaceDescriptorInterfaceABC *ColorSpaceDescriptor::GetInterface(void)
{ return &m_Interface; }



