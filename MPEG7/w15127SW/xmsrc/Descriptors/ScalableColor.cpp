///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ScalablrColor.cpp
//
#include <vector>
#include <stdio.h>
#include "Descriptors/Descriptors.h"
#include "Utilities/readparam.h"

using namespace XM;

const UUID ScalableColorDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *ScalableColorDescriptorInterface::myName = "ScalableColorDescriptionInterface";

const UUID ScalableColorDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *ScalableColorDescriptor::myName = "ScalableColorDescriptor";

const UUID ScalableColorDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *ScalableColorDescriptor::valName = "";

const UUID ScalableColorDescriptorInterfaceABC::myID = UUID();

ScalableColorDescriptorInterfaceABC::ScalableColorDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ScalableColorDescriptorInterface::ScalableColorDescriptorInterface(ScalableColorDescriptor *aHisto):
m_ScalableColorDescriptor(aHisto)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ScalableColorDescriptorInterface::~ScalableColorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void ScalableColorDescriptorInterface::addref(void)
{ m_ScalableColorDescriptor->addref(); }

//----------------------------------------------------------------------------
void ScalableColorDescriptorInterface::release(void)
{ m_ScalableColorDescriptor->release(); }

//----------------------------------------------------------------------------
void ScalableColorDescriptorInterface::SetValuesFromParameterFile(void)

{
	m_ScalableColorDescriptor->SetNumberOfBitplanesDiscarded(
		atoi(getpara("GetNumberOfBitplanesDiscarded","0")));
}

//----------------------------------------------------------------------------
unsigned long *ScalableColorDescriptorInterface::GetScalableHistogram(void)
{
	return m_ScalableColorDescriptor->GetScalableHistogram();
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::GetNumberOfCoefficients(void)
{
	return m_ScalableColorDescriptor->GetNumberOfCoefficients();
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::SetNumberOfCoefficients(unsigned long size)	
{
	return m_ScalableColorDescriptor->SetNumberOfCoefficients(size);
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
GetNumberOfBitplanesDiscarded(void)
{
	return m_ScalableColorDescriptor->GetNumberOfBitplanesDiscarded();
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
SetNumberOfBitplanesDiscarded(unsigned long factor)
{
	return m_ScalableColorDescriptor->
	  SetNumberOfBitplanesDiscarded(factor);
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
SetCoefficient(unsigned long index, int value)
{
	return m_ScalableColorDescriptor->SetCoefficient(index, value);
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
SetCoeffValue(unsigned long index, int value)
{
	return m_ScalableColorDescriptor->SetCoeffValue(index, value);
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
GetCoefficient(unsigned long index)
{
	return m_ScalableColorDescriptor->GetCoefficient(index);
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
GetCoeffSign(unsigned long index)
{
	return m_ScalableColorDescriptor->GetCoeffSign(index);
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
SetCoeffSign(unsigned long index, int value)
{
	return m_ScalableColorDescriptor->SetCoeffSign(index, value);
}

//----------------------------------------------------------------------------
const UUID& ScalableColorDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorDescriptorInterface::GetName(void)
{ return myName; }


//-----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  return m_ScalableColorDescriptor->ExportDDL(aParentDescription);
}

//-----------------------------------------------------------------------------
unsigned long ScalableColorDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_ScalableColorDescriptor->ImportDDL(aDescription);
}

//-----------------------------------------------------------------------------
void ScalableColorDescriptorInterface::SetGoFGoP(bool set)
{
	m_ScalableColorDescriptor->SetGoFGoP(set);
}

//=============================================================================
ScalableColorDescriptor::ScalableColorDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ScalableHistogram(0),
m_NumberOfCoefficients(0),
m_NumberOfBitplanesDiscarded(0),
m_isGoFGop(false)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ScalableColorDescriptor::~ScalableColorDescriptor()
{

	if (m_ScalableHistogram) delete [] m_ScalableHistogram;

#ifdef VERBOSE
       fprintf(stderr,"ScalableColor delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& ScalableColorDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ScalableColorDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ScalableColorDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ScalableColorDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void ScalableColorDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ScalableColor connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void ScalableColorDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ScalableColor release\n");
#endif
       if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
unsigned long* ScalableColorDescriptor::GetScalableHistogram(void)
{
	return m_ScalableHistogram;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetNumberOfCoefficients(void)
{
	return m_NumberOfCoefficients;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::
SetNumberOfCoefficients(unsigned long newSize)
{
	if (m_NumberOfCoefficients == newSize) return 0;

	if (m_ScalableHistogram) {
	  delete [] m_ScalableHistogram;
	  delete [] m_Sign;
	}
	m_ScalableHistogram = new unsigned long [newSize];
	m_Sign = new unsigned long [newSize];

	m_NumberOfCoefficients = newSize;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::
   SetNumberOfBitplanesDiscarded(unsigned long aRenorm)
{
	if(aRenorm<0 || aRenorm==5 || aRenorm==7 || aRenorm>8)
	  return (unsigned long)-1;
	m_NumberOfBitplanesDiscarded = aRenorm;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetNumberOfBitplanesDiscarded()
{
	return m_NumberOfBitplanesDiscarded;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetCoefficient(unsigned long index, int value)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;
	m_ScalableHistogram[index] = value;
	m_Sign[index] = value<0?0:1;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::SetCoeffValue(unsigned long index, int value)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;
	m_ScalableHistogram[index] = value;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetCoefficient(unsigned long index)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;

	return m_ScalableHistogram[index];
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::GetCoeffSign(unsigned long index)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;

	return m_Sign[index];
}

//----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::
SetCoeffSign(unsigned long index, int value)
{
	if (index < 0 || index >= m_NumberOfCoefficients)
	  return (unsigned long)-1;
	m_Sign[index]=value<0?0:1;
	return 0;
}

//-----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  if (!aParentDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  if(m_isGoFGop)
  {
	  l_DDLDescription = aParentDescription->CreateChild("ScalableColor");
  }
  else
  {
	  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
	  l_DDLDescription.SetAttribute("xsi:type", "ScalableColorType");
  }
//////////////////////////

	int NumberOfCoefficients=atoi(getpara("NumberOfCoefficients","256"));
	
	if((NumberOfCoefficients!=16)&&(NumberOfCoefficients!=32)&&
	   (NumberOfCoefficients!=64)&&(NumberOfCoefficients!=128)&&
		 (NumberOfCoefficients!=256))
	return (unsigned long)-1;
	
	l_DDLDescription.SetAttribute("numOfCoeff", NumberOfCoefficients);
	
	/*put NumberOfRedBits*/
	int NumberOfBitplanesDiscarded = GetNumberOfBitplanesDiscarded();
	l_DDLDescription.SetAttribute("numOfBitplanesDiscarded", NumberOfBitplanesDiscarded);
	
	GenericDS Coeff_element = l_DDLDescription.CreateChild("Coeff");

	vector<int> element(NumberOfCoefficients);
	for (int i=0; i<NumberOfCoefficients; i++) 
	{
		int no_of_bits=XM::scalableColorQuantValues[i][1]-1-NumberOfBitplanesDiscarded;

		if(no_of_bits<=0) no_of_bits = 1;

		element[i] = GetCoefficient(i);// >> (sizeof(long) - no_of_bits)		
	}

	Coeff_element.SetValue(element);

  return 0;
}

//-----------------------------------------------------------------------------
unsigned long ScalableColorDescriptor::ImportDDL(GenericDSInterfaceABC 
																								 *aDescription)
{
  if (!aDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  string xsitype;
  if (aDescription->GetDSName() == "Descriptor")
  {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if (xsitype == "ScalableColorType")
    {
      l_DDLDescriptionInterface = aDescription;
    }
  }
  else if(aDescription->GetDSName() == "ScalableColor")
  {
	  m_isGoFGop = true;
	  l_DDLDescriptionInterface = aDescription;
  }

  if (!l_DDLDescriptionInterface)
  {
    l_DDLDescription = aDescription->GetDescription("Descriptor");

    while (!l_DDLDescription.isNull())
    {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if (xsitype == "ScalableColorType")
        break;
      l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
    }

    if (l_DDLDescription.isNull())
      return (unsigned long)-1;

    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  int NumberOfCoefficients=-1;
  l_DDLDescriptionInterface->GetIntAttribute("numOfCoeff", 
					     NumberOfCoefficients);
	
  if((NumberOfCoefficients!=16)&&(NumberOfCoefficients!=32)&&
	  (NumberOfCoefficients!=64)&&(NumberOfCoefficients!=128)&&
	  (NumberOfCoefficients!=256))
	  return (unsigned long)-1;

  SetNumberOfCoefficients(NumberOfCoefficients); 

  /*get normalization*/
  int NumberOfBitplanesDiscarded=0;
 
  l_DDLDescriptionInterface->GetIntAttribute("numOfBitplanesDiscarded", 
					     NumberOfBitplanesDiscarded);
  
  if((NumberOfBitplanesDiscarded != 0)
	  && (NumberOfBitplanesDiscarded != 1)
	  && (NumberOfBitplanesDiscarded != 2)
	  && (NumberOfBitplanesDiscarded != 3)
	  && (NumberOfBitplanesDiscarded != 4)
	  && (NumberOfBitplanesDiscarded != 6)
	  && (NumberOfBitplanesDiscarded != 8)
	  )
	  return  (unsigned long)-1;

  SetNumberOfBitplanesDiscarded(NumberOfBitplanesDiscarded);

  GenericDS Coeff_element = l_DDLDescriptionInterface->
    GetDescription("Coeff");
  vector<int> element;
  Coeff_element.GetIntVector(element);
	
  for (int i=0; i<NumberOfCoefficients; i++) 
    {
//		int no_of_bits = 
//			XM::scalableColorQuantValues[i][1]-1-NumberOfBitplanesDiscarded;
      int coeff = element[i]; // << (sizeof(long) - no_of_bits)
      SetCoefficient(i,coeff);
    }

  return 0;
}
//----------------------------------------------------------------------------
void ScalableColorDescriptor::SetGoFGoP(bool set)
{
	m_isGoFGop = set;
}

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC *ScalableColorDescriptor::
  GetInterface(void)
{ return &m_Interface; }



