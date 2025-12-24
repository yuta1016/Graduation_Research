// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// Applicable File Name:  HomoTextureDescriptor.cpp
//

#include <stdio.h>
#include <memory.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID HomogeneousTextureDescriptorInterface::myID = UUID("71434f90-8299-11d3-9daf-00609791d5bb");
const char *HomogeneousTextureDescriptorInterface::myName = 
"HomogeneousTextureDescriptionInterface";

const UUID HomogeneousTextureDescriptor::myID = UUID("28544460-6719-11d3-9d88-00609791d5bb");
const char *HomogeneousTextureDescriptor::myName = "HomogeneousTextureDescriptor";

const UUID HomogeneousTextureDescriptor::valID = UUID("");
const char *HomogeneousTextureDescriptor::valName = "";

const UUID HomogeneousTextureDescriptorInterfaceABC::myID = UUID();

HomogeneousTextureDescriptorInterfaceABC::HomogeneousTextureDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
HomogeneousTextureDescriptorInterface::
HomogeneousTextureDescriptorInterface(HomogeneousTextureDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterface::~HomogeneousTextureDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void HomogeneousTextureDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void HomogeneousTextureDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureDescriptorInterface::GetName(void)
{ return myName; }

//---yjyu - 010222 ychoi - 010601
//----------------------------------------------------------------------------
void HomogeneousTextureDescriptorInterface::
SetHomogeneousTextureFeature( int f, int* p )
{
	this->m_Descriptor->SetHomogeneousTextureFeature( f, p ) ;
}


//----------------------------------------------------------------------------
int* HomogeneousTextureDescriptorInterface::GetHomogeneousTextureFeature( ) 
{
	return this->m_Descriptor->GetHomogeneousTextureFeature() ;
}

//----------------------------------------------------------------------------
int HomogeneousTextureDescriptorInterface::GetHomogeneousTextureFeatureFlag( ) 
{
	return this->m_Descriptor->GetHomogeneousTextureFeatureFlag() ;
}
//---


//----------------------------------------------------------------------------
unsigned long HomogeneousTextureDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }

//=============================================================================
HomogeneousTextureDescriptor::HomogeneousTextureDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
  SetExposedInterface(&m_Interface);
}
//----------------------------------------------------------------------------
HomogeneousTextureDescriptor::~HomogeneousTextureDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"HomogeneousTexture delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* HomogeneousTextureDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool HomogeneousTextureDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//---yjyu - 010222 ychoi - 010601
//----------------------------------------------------------------------------------------------
void HomogeneousTextureDescriptor ::
SetHomogeneousTextureFeature( int f, int* pHomogeneousTextureFeature ) 
{
	energydeviationflag = f;
	memcpy(feature, pHomogeneousTextureFeature, sizeof (feature));
}

//----------------------------------------------------------------------------
int* HomogeneousTextureDescriptor::GetHomogeneousTextureFeature(void)
{
	if (feature ==NULL) return NULL;
	return feature;
}

//----------------------------------------------------------------------------
int HomogeneousTextureDescriptor::GetHomogeneousTextureFeatureFlag(void)
{

	return energydeviationflag;
}
//---

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","HomogeneousTextureType");

  vector<int> energy(30), energydeviation(30);

  GenericDS AverageDescription = l_DDLDescription.CreateChild("Average");
  AverageDescription.SetValue(GetHomogeneousTextureFeature()[0]);

  GenericDS StandardDeviationDescription = l_DDLDescription.CreateChild("StandardDeviation");
  StandardDeviationDescription.SetValue(GetHomogeneousTextureFeature()[1]);

  GenericDS EnergyDescription = l_DDLDescription.CreateChild("Energy");
  for (int i=0;i<30;i++)
	  energy[i] = GetHomogeneousTextureFeature()[i+2];
  EnergyDescription.SetValue(energy);

  if (GetHomogeneousTextureFeatureFlag() == 1)
  {
		GenericDS EnergyDeviationDescription = l_DDLDescription.CreateChild("EnergyDeviation");
		for (int i=0;i<30;i++)
			energydeviation[i] = GetHomogeneousTextureFeature()[i+32];  
        EnergyDeviationDescription.SetValue(energydeviation);
  }	
  return 0;
}

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;


  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "HomogeneousTextureType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for HomogeneousTextureType as a child*/
  if (!l_DDLDescriptionInterface) { 
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="HomogeneousTextureType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* HomogeneousTextureType not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* HomogeneousTextureType found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  int energydeviationflag, average, standarddeviation;
  int feature[62];
  vector<int> energy, energydeviation;
  
  GenericDS AverageDescription = l_DDLDescriptionInterface->GetDescription("Average");
  AverageDescription.GetIntValue(average);
  feature[0] = average;
  
  GenericDS StandardDeviationDescription = l_DDLDescriptionInterface->GetDescription("StandardDeviation");
  StandardDeviationDescription.GetIntValue(standarddeviation);
  feature[1] = standarddeviation;

  GenericDS EnergyDescription = l_DDLDescriptionInterface->GetDescription("Energy");
  EnergyDescription.GetIntVector(energy);
  for (int i=0;i<30;i++) 
	  feature[i+2] = energy[i];

  GenericDS EnergyDeviationDescription = l_DDLDescriptionInterface->GetDescription("EnergyDeviation");
  if (!EnergyDeviationDescription.isNull()) {
	 energydeviationflag = 1;
     EnergyDeviationDescription.GetIntVector(energydeviation);
	 for (int i=0;i<30;i++) feature[i+32] = energydeviation[i];
  }
  else {
	  energydeviationflag = 0;
	  for (int i=0;i<30;i++) feature[i+32] = 0;
  }
  SetHomogeneousTextureFeature(energydeviationflag, feature);
  return 0;
}

//----------------------------------------------------------------------------
void HomogeneousTextureDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void HomogeneousTextureDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC *HomogeneousTextureDescriptor::GetInterface(void)
{ return &m_Interface; }



