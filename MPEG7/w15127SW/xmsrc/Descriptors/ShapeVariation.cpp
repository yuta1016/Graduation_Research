///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// Applicable File Name:  ShapeVariation.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"


using namespace XM;

const UUID ShapeVariationDescriptorInterface::myID = UUID("");
const char *ShapeVariationDescriptorInterface::myName = "ShapeVariationDescriptorInterface";

const UUID ShapeVariationDescriptor::myID = UUID("");
const char *ShapeVariationDescriptor::myName = "ShapeVariationDescriptor";

const UUID ShapeVariationDescriptor::valID = UUID("");
const char *ShapeVariationDescriptor::valName = "ShapeVariationDescriptor";

const UUID ShapeVariationDescriptorInterfaceABC::myID = UUID();

//=============================================================================
// Quantization and Inverse quantization tables

double ShapeVariationDescriptor::QuantTable[17] = { 0.000000000, 0.003073263, 0.006358638, 0.009887589, 0.013699146, 0.017842545, 0.022381125, 0.027398293, 0.033007009, 0.039365646, 0.046706155, 0.055388134, 0.066014017, 0.079713163, 0.099021026, 0.132028034, 1.000000000 };
double ShapeVariationDescriptor::IQuantTable[16] =     {0.001511843, 0.004687623, 0.008090430, 0.011755242, 0.015725795, 0.020057784, 0.024823663, 0.030120122, 0.036080271, 0.042894597, 0.050849554, 0.060405301, 0.072372655, 0.088395142, 0.112720172, 0.165035042 };

//=============================================================================
ShapeVariationDescriptorInterfaceABC::ShapeVariationDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ShapeVariationDescriptorInterface::
ShapeVariationDescriptorInterface(ShapeVariationDescriptor *aHisto):
m_Descriptor(aHisto)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeVariationDescriptorInterface::~ShapeVariationDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void ShapeVariationDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void ShapeVariationDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& ShapeVariationDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationDescriptorInterface::GetName(void)
{ return myName; }

bool ShapeVariationDescriptorInterface::SetLVMElement(char p, char r, char value)
{
	return m_Descriptor->SetLVMElement(p, r, value);
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptorInterface::SetLVMElement(char p, char r, double value)
{
	return m_Descriptor->SetLVMElement(p, r, value);
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptorInterface::GetLVMElement(char p, char r)
{
	return m_Descriptor->GetLVMElement(p, r);
}

//----------------------------------------------------------------------------
double ShapeVariationDescriptorInterface::GetLVMValue(char p, char r)
{
	return m_Descriptor->GetLVMValue(p, r);
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptorInterface::SetHVMElement(char p, char r, char value)
{
	return m_Descriptor->SetHVMElement(p, r, value);
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptorInterface::SetHVMElement(char p, char r, double value)
{
	return m_Descriptor->SetHVMElement(p, r, value);
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptorInterface::GetHVMElement(char p, char r)
{
	return m_Descriptor->GetHVMElement(p, r);
}

//----------------------------------------------------------------------------
double ShapeVariationDescriptorInterface::GetHVMValue(char p, char r)
{
	return m_Descriptor->GetHVMValue(p, r);
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptorInterface::SetSTDElement(char p, char r, char value)
{
	return m_Descriptor->SetSTDElement(p, r, value);
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptorInterface::SetSTDElement(char p, char r, double value)
{
	return m_Descriptor->SetSTDElement(p, r, value);
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptorInterface::GetSTDElement(char p, char r)
{
	return m_Descriptor->GetSTDElement(p, r);
}

//----------------------------------------------------------------------------
double ShapeVariationDescriptorInterface::GetSTDValue(char p, char r)
{
	return m_Descriptor->GetSTDValue(p, r);
}

//----------------------------------------------------------------------------
unsigned long ShapeVariationDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long ShapeVariationDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
ShapeVariationDescriptor::ShapeVariationDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
	for(int i = 0 ; i < ANGULAR ; i++)
	{
		for(int j = 0 ; j < RADIAL ; j++)
		{
			m_StaticShapeVariation[i][j] = 0;
			m_DynamicShapeVariation[i][j] = 0;
			m_StatisticalVariation[i][j] = 0;
		}
	}

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeVariationDescriptor::~ShapeVariationDescriptor()
{
#ifdef VERBOSE
	fprintf(stderr,"ShapeVariation delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& ShapeVariationDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ShapeVariationDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ShapeVariationDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::SetLVMElement(char p, char r, char value)
{
	if ( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return false;

	m_StaticShapeVariation[(int)p][(int)r] = value;
	return true;
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::SetLVMElement(char p, char r, double value)
{
	if ( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return false;

	m_StaticShapeVariation[(int)p][(int)r] = Quantize(value);
	return true;
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptor::GetLVMElement(char p, char r)
{
	if( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return -1;

	return m_StaticShapeVariation[(int)p][(int)r];	
}

//----------------------------------------------------------------------------
double ShapeVariationDescriptor::GetLVMValue(char p, char r)
{
	if( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return -1;

	return IQuantTable[(int)m_StaticShapeVariation[(int)p][(int)r]];
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::SetHVMElement(char p, char r, char value)
{
	if ( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return false;

	m_DynamicShapeVariation[(int)p][(int)r] = value;
	return true;
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::SetHVMElement(char p, char r, double value)
{
	if ( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return false;

	m_DynamicShapeVariation[(int)p][(int)r] = Quantize(value);
	return true;
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptor::GetHVMElement(char p, char r)
{
	if( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return -1;

	return m_DynamicShapeVariation[(int)p][(int)r];
}

//----------------------------------------------------------------------------
double ShapeVariationDescriptor::GetHVMValue(char p, char r)
{
	if( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return -1;

	return IQuantTable[(int)m_DynamicShapeVariation[(int)p][(int)r]];
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::SetSTDElement(char p, char r, char value)
{
	if ( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return false;

	m_StatisticalVariation[(int)p][(int)r] = value;
	return true;
}

//----------------------------------------------------------------------------
bool ShapeVariationDescriptor::SetSTDElement(char p, char r, double value)
{
	if ( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return false;

	m_StatisticalVariation[(int)p][(int)r] = Quantize(value);
	return true;
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptor::GetSTDElement(char p, char r)
{
	if( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return -1;

	return m_StatisticalVariation[(int)p][(int)r];	
}

//----------------------------------------------------------------------------
double ShapeVariationDescriptor::GetSTDValue(char p, char r)
{
	if( p < 0 || p >= ANGULAR || r < 0 || r >= RADIAL)
		return -1;

	return IQuantTable[(int)m_StatisticalVariation[(int)p][(int)r]];
}

//----------------------------------------------------------------------------
char ShapeVariationDescriptor::Quantize(double val)
{
	// Quantization
 	char high = sizeof(QuantTable)/sizeof(double);
	char low = 0;
	char middle;

	while( high-low > 1)
	{
		middle = (high+low)/2;

		if(QuantTable[(int)middle] < val)
			low = middle;
		else
			high = middle;
	}

	return low;
}

//----------------------------------------------------------------------------
unsigned long ShapeVariationDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	
	if (!aParentDescription) return (unsigned long) -1;

	GenericDS l_DDLDescription;
	l_DDLDescription = aParentDescription->CreateChild("Descriptor");
	l_DDLDescription.SetAttribute("xsi:type", "ShapeVariationType");
	
	GenericDS LVMElement = l_DDLDescription.CreateChild("StaticShapeVariation");
	GenericDS HVMElement = l_DDLDescription.CreateChild("DynamicShapeVariation");
	GenericDS STDElement = l_DDLDescription.CreateChild("StatisticalVariation");
	
	vector<int> LVME(35);
	vector<int> HVME(35);
	vector<int> STDE(35);
	
	int p, r;
	int n = 0;
	for( p = 0 ; p < ANGULAR ; p++ )	
	{
		for( r = 0 ; r < RADIAL ; r++ )
		{
			if( p != 0 || r != 0 )
			{
				LVME[n] = GetLVMElement( p, r);
				HVME[n] = GetHVMElement( p, r);
				STDE[n] = GetSTDElement( p, r);
				n++;
			}
		}
	}

	LVMElement.SetValue(LVME);
	HVMElement.SetValue(HVME);
	STDElement.SetValue(STDE);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ShapeVariationDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
	if (!aDescription) return (unsigned long) -1;
	
	GenericDS l_DDLDescription;
	GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
	
	string xsitype;
	if (aDescription->GetDSName() == "Descriptor")
	{
		aDescription->GetTextAttribute("xsi:type", xsitype);
		if (xsitype == "ShapeVariationType")
		{
			l_DDLDescriptionInterface = aDescription;
		}
	}
	
	if (!l_DDLDescriptionInterface)
	{
		l_DDLDescription = aDescription->GetDescription("Descriptor");
		
		while (!l_DDLDescription.isNull())
		{
			l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
			if (xsitype == "ShapeVariationType")
				break;
			l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
		}
		
		if (l_DDLDescription.isNull())
			return (unsigned long)-1;
		
		l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
	}
	
	GenericDS LVMElement = l_DDLDescriptionInterface->GetDescription("StaticShapeVariation");
	GenericDS HVMElement = l_DDLDescriptionInterface->GetDescription("DynamicShapeVariation");
	GenericDS STDElement = l_DDLDescriptionInterface->GetDescription("StatisticalVariation");

	vector<int> LVME;
	vector<int> HVME;
	vector<int> STDE;
	
	LVMElement.GetIntVector(LVME);
	HVMElement.GetIntVector(HVME);
	STDElement.GetIntVector(STDE);

	int p, r;
	int n = 0;
	for( p = 0 ; p < ANGULAR ; p++ )	
	{
		for( r = 0 ; r < RADIAL ; r++ )
		{
			if( p != 0 || r != 0 )
			{
				SetLVMElement( (char)p, (char)r, (char)LVME[n] );
				SetHVMElement( (char)p, (char)r, (char)HVME[n] );
				SetSTDElement( (char)p, (char)r, (char)STDE[n] );
				n++;
			}
		}
	}
	
	return 0;
}

//----------------------------------------------------------------------------
void ShapeVariationDescriptor::addref(void)

{
	m_refcount++;
}

//----------------------------------------------------------------------------
void ShapeVariationDescriptor::release(void)

{
	if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC *ShapeVariationDescriptor::GetInterface(void)
{ return &m_Interface; }


