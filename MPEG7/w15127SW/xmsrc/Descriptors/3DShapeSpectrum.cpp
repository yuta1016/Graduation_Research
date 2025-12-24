///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
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
// Applicable File Name:  3DShapeSpectrum.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"
#include "Utilities/readparam.h"

using namespace XM;

const UUID ShapeSpectrumDescriptorInterface::myID = UUID("");
const char *ShapeSpectrumDescriptorInterface::myName = "Shape Spectrum Description Interface";

const UUID ShapeSpectrumDescriptor::myID = UUID("");
const char *ShapeSpectrumDescriptor::myName = "Shape Spectrum  Descriptor";
const UUID ShapeSpectrumDescriptor::valID = UUID("");
const char *ShapeSpectrumDescriptor::valName = "";

const UUID ShapeSpectrumDescriptorInterfaceABC::myID = UUID();

ShapeSpectrumDescriptorInterfaceABC::ShapeSpectrumDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ShapeSpectrumDescriptorInterface::ShapeSpectrumDescriptorInterface(ShapeSpectrumDescriptor *aHisto):
m_ShapeSpectrumDescriptor(aHisto)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterface::~ShapeSpectrumDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void ShapeSpectrumDescriptorInterface::addref(void)
{ m_ShapeSpectrumDescriptor->addref(); }

//----------------------------------------------------------------------------
void ShapeSpectrumDescriptorInterface::release(void)
{ m_ShapeSpectrumDescriptor->release(); }


//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::
          SetNoOfBins(int NoOfBins)

{
  return m_ShapeSpectrumDescriptor->SetNoOfBins(NoOfBins);
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::GetNoOfBins(void)
{  
  return m_ShapeSpectrumDescriptor->GetNoOfBins();
}


//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::
          SetNoOfBits(int NoOfBits)

{
  return m_ShapeSpectrumDescriptor->SetNoOfBits(NoOfBits);
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::GetNoOfBits(void)
{  
  return m_ShapeSpectrumDescriptor->GetNoOfBits();
}


//----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptorInterface::SetShapeSpectrumSize(unsigned long newSize)
{
  return m_ShapeSpectrumDescriptor->SetShapeSpectrumSize(newSize);   
}

//----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptorInterface::GetShapeSpectrumSize(void)
{
  return m_ShapeSpectrumDescriptor->GetShapeSpectrumSize();   
}


//----------------------------------------------------------------------------
unsigned long *ShapeSpectrumDescriptorInterface::GetShapeSpectrum(void)
{
  return m_ShapeSpectrumDescriptor->GetShapeSpectrum();
}


//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::SetElement(unsigned long index, unsigned long value)
{
  return m_ShapeSpectrumDescriptor->SetElement(index, value);  
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::GetElement(unsigned long index)
{
  return m_ShapeSpectrumDescriptor->GetElement(index);  
}


//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::SetPlanarElement(unsigned long value)
{  
  return m_ShapeSpectrumDescriptor->SetPlanarElement(value);  
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::GetPlanarElement(void)
{   
  return m_ShapeSpectrumDescriptor->GetPlanarElement();  
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::SetSingularElement(unsigned long value)
{  
  return m_ShapeSpectrumDescriptor->SetSingularElement(value);  
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptorInterface::GetSingularElement(void)
{ 
  return m_ShapeSpectrumDescriptor->GetSingularElement();
}

//----------------------------------------------------------------------------
void ShapeSpectrumDescriptorInterface::SetValuesFromParameterFile(void)

{   
  int no_of_bins = atoi(getpara("NoOfBins","100"));

  m_ShapeSpectrumDescriptor->SetShapeSpectrumSize(no_of_bins);
  m_ShapeSpectrumDescriptor->SetNoOfBins(no_of_bins);
  m_ShapeSpectrumDescriptor->SetNoOfBits(atoi(getpara("NoOfBits","12"))); 
  
  #ifdef DEBUG
  fprintf(stderr,"Using: ShapeSpectrum Size = %d  NoOfBits = %d\n",(int)
	  m_ShapeSpectrumDescriptor->GetNoOfBins(), (int)m_ShapeSpectrumDescriptor->GetNoOfBits());
  #endif
  
  }


//----------------------------------------------------------------------------
const UUID& ShapeSpectrumDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumDescriptorInterface::GetName(void)
{ return myName; }


//-----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  return m_ShapeSpectrumDescriptor->ExportDDL(aParentDescription);
}

//-----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_ShapeSpectrumDescriptor->ImportDDL(aDescription);
}

//-----------------------------------------------------------------------------

//=============================================================================
ShapeSpectrumDescriptor::ShapeSpectrumDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_ShapeSpectrumSize(0),
m_ShapeSpectrumNoOfBits(0),
m_ShapeSpectrum(0),
m_PlanarSurfaces(0),
m_SingularSurfaces(0)
  
{
	SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
ShapeSpectrumDescriptor::~ShapeSpectrumDescriptor()
{
  if (m_ShapeSpectrum) delete [] m_ShapeSpectrum;

#ifdef VERBOSE
       fprintf(stderr," Shape Spectrum delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ShapeSpectrumDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ShapeSpectrumDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void ShapeSpectrumDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ShapeSpectrum connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void ShapeSpectrumDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ShapeSpectrum release\n");
#endif
       if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC *ShapeSpectrumDescriptor::GetInterface(void)
{ return &m_Interface; }



//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::SetNoOfBins(int NoOfBins)
{
  int error=0;

  m_ShapeSpectrumSize = (unsigned long)NoOfBins;
 
  if (NoOfBins <= 0) {
    m_ShapeSpectrumSize = 0;
    error=-1;
  } 

  return error;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::GetNoOfBins(void)
{  
  return m_ShapeSpectrumSize;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::SetNoOfBits(int NoOfBits)
{
  int error=0;

  m_ShapeSpectrumNoOfBits = (unsigned long)NoOfBits;
 
  if (NoOfBits <= 0) {
    m_ShapeSpectrumNoOfBits = 0;
    error=-1;
  } 

  return error;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::GetNoOfBits(void)
{  
  return m_ShapeSpectrumNoOfBits;
}

//----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptor::SetShapeSpectrumSize(unsigned long newSize)
{
  if (m_ShapeSpectrumSize == newSize) return 0;
  if (m_ShapeSpectrum) delete [] m_ShapeSpectrum;
  
  m_ShapeSpectrum = new unsigned long [newSize];
  m_ShapeSpectrumSize = newSize;
 
  return 0;
}

//----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptor::GetShapeSpectrumSize(void)
{
	return m_ShapeSpectrumSize;
}


//----------------------------------------------------------------------------
unsigned long *ShapeSpectrumDescriptor::GetShapeSpectrum(void)
{
	return m_ShapeSpectrum;
}


//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::SetElement(unsigned long index, unsigned long value)
{
  if (index < 0 || index >= m_ShapeSpectrumSize)
    return -1;
  m_ShapeSpectrum[index] = value;
  
  return 0;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::GetElement(unsigned long index)
{
  if (index < 0 || index >= m_ShapeSpectrumSize)
    return (unsigned long)-1;
  
  return (int)m_ShapeSpectrum[index];
}


//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::SetPlanarElement(unsigned long value)
{  
  m_PlanarSurfaces = value;
  
  return 0;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::GetPlanarElement(void)
{    
  return (int)m_PlanarSurfaces;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::SetSingularElement(unsigned long value)
{  
  m_SingularSurfaces = value;
  
  return 0;
}

//----------------------------------------------------------------------------
int ShapeSpectrumDescriptor::GetSingularElement(void)
{    
  return (int)m_SingularSurfaces;
}

// UPDATE for DLL representation


//----------------------------------------------------------------------------
unsigned long ShapeSpectrumDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;

  int i;
  GenericDS l_Spectrum, l_Size, l_NoOfBits, l_Planar, l_Singular;
  int *p_spectrum = new int[m_ShapeSpectrumSize]; 
  for(i=0;i<m_ShapeSpectrumSize;i++)    
    p_spectrum[i] = (int)GetElement((unsigned long)i); 

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","Shape3DType");
  l_DDLDescription.SetAttribute("bitsPerBin",(int)m_ShapeSpectrumNoOfBits);
  
  l_Spectrum = l_DDLDescription.CreateChild("Spectrum");
  vector<int> Spectrum(&p_spectrum[0],&p_spectrum[m_ShapeSpectrumSize]);  
  l_Spectrum.SetValue(Spectrum); // m_ShapeSpectrum
 
  l_Planar = l_DDLDescription.CreateChild("PlanarSurfaces");
  l_Planar.SetValue((int)m_PlanarSurfaces);

  l_Singular = l_DDLDescription.CreateChild("SingularSurfaces");
  l_Singular.SetValue((int)m_SingularSurfaces);

  if(p_spectrum) delete p_spectrum; 

  return 0;
}




//----------------------------------------------------------------------------ShapeSpectrumDescriptor
unsigned long ShapeSpectrumDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int i, NBins, NBits, Plan, Sing; 
  vector<int> Spectrum;

  string xsitype;

  GenericDS l_Spectrum, l_Size, l_NoOfBits, l_Planar, l_Singular;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "Shape3DType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for ShapeSpectrum as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="Shape3DType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* ShapeSpectrum not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* ShapeSpectrum found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  /* import the content */ 
  l_DDLDescriptionInterface->GetIntAttribute("bitsPerBin", NBits);
  SetNoOfBits(NBits); 

  l_Spectrum = l_DDLDescriptionInterface->GetDescription("Spectrum");
  l_Spectrum.GetIntVector(Spectrum);
  NBins = Spectrum.size();
  for (i=0;i<NBins;i++)
    SetElement(i, Spectrum[i]);

  l_Planar = l_DDLDescriptionInterface->GetDescription("PlanarSurfaces");
  l_Planar.GetIntValue(Plan); SetPlanarElement(Plan); 

  l_Singular = l_DDLDescriptionInterface->GetDescription("SingularSurfaces");
  l_Singular.GetIntValue(Sing); SetSingularElement(Sing); 

  //  l_DDLDescriptionInterface->GetIntAttribute("ShapeSpectrumContents",val);
  //  SetShapeSpectrumContents((long)val);


  /*
  fprintf(stderr, "HERE IMPORTING\n");
  fprintf(stderr, "ImportDDL: NBins = %d\n", GetShapeSpectrumSize());
  fprintf(stderr, "ImportDDL: NBits = %d\n", GetNoOfBits() );
  fprintf(stderr, "ImportDDL: \n");
  for (i=0;i<NBins;i++)
    fprintf(stderr, "%d ", GetElement(i));   
  fprintf(stderr, "\nImportDDL: Planar = %d\n", GetPlanarElement());
  fprintf(stderr, "ImportDDL: Singular = %d\n", GetSingularElement());
  fprintf(stderr, "ImportDDL: \n");
  
  */
  return 0;
}

