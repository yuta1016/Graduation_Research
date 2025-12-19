//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshio Kamei, NEC Corporation.
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
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  AdvancedFaceRecognition.cpp
//

#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID AdvancedFaceRecognitionDescriptorInterface::myID = UUID("");
const char *AdvancedFaceRecognitionDescriptorInterface::myName = "AdvancedFaceRecognitionDescriptorInterface";

const UUID AdvancedFaceRecognitionDescriptor::myID = UUID("");
const char *AdvancedFaceRecognitionDescriptor::myName = "AdvancedFaceRecognitionDescriptor";

const UUID AdvancedFaceRecognitionDescriptor::valID = UUID("");
const char *AdvancedFaceRecognitionDescriptor::valName = "";

const UUID AdvancedFaceRecognitionDescriptorInterfaceABC::myID = UUID();


AdvancedFaceRecognitionDescriptorInterfaceABC::AdvancedFaceRecognitionDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
AdvancedFaceRecognitionDescriptorInterface::AdvancedFaceRecognitionDescriptorInterface(AdvancedFaceRecognitionDescriptor *aAdvancedFaceRecognition):
m_AdvancedFaceRecognitionDescriptor(aAdvancedFaceRecognition)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterface::~AdvancedFaceRecognitionDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionDescriptorInterface::addref(void)
{ m_AdvancedFaceRecognitionDescriptor->addref(); }

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionDescriptorInterface::release(void)
{ m_AdvancedFaceRecognitionDescriptor->release(); }

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionDescriptorInterface::
SetValuesFromParameterFile(void)
{

}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
AdvancedFRD*	AdvancedFaceRecognitionDescriptorInterface::GetAdvancedFaceRecognition()
{
	return	m_AdvancedFaceRecognitionDescriptor->GetAdvancedFaceRecognition();	
}

//----------------------------------------------------------------------------
void	AdvancedFaceRecognitionDescriptorInterface::
SetAdvancedFaceRecognition(AdvancedFRD*	pAdvancedFaceRecognition)
{
  m_AdvancedFaceRecognitionDescriptor->SetAdvancedFaceRecognition(pAdvancedFaceRecognition);	
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_AdvancedFaceRecognitionDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_AdvancedFaceRecognitionDescriptor->ImportDDL(aDescription); }


//=============================================================================
AdvancedFaceRecognitionDescriptor::AdvancedFaceRecognitionDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
  m_pAdvancedFaceRecognition=NULL;
  SetExposedInterface(&m_Interface);
  m_pAdvancedFaceRecognition = new	AdvancedFRD[1];
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptor::~AdvancedFaceRecognitionDescriptor()
{
  if( m_pAdvancedFaceRecognition )
    delete [] m_pAdvancedFaceRecognition;
}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* AdvancedFaceRecognitionDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionDescriptor::addref(void)

{
#ifdef VERBOSE
  fprintf(stderr,"AdvancedFaceRecognition connect\n");
#endif
  m_refcount++;
}

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionDescriptor::release(void)
{
#ifdef VERBOSE
  fprintf(stderr,"AdvancedFaceRecognition release\n");
#endif
  if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
AdvancedFRD*	AdvancedFaceRecognitionDescriptor::GetAdvancedFaceRecognition()
{
  return	m_pAdvancedFaceRecognition;
}

//----------------------------------------------------------------------------
void	AdvancedFaceRecognitionDescriptor::SetAdvancedFaceRecognition(AdvancedFRD*	pAdvancedFaceRecognition)
{
  int iw;

  m_pAdvancedFaceRecognition->numOfFourierFeature =  pAdvancedFaceRecognition->numOfFourierFeature;
  m_pAdvancedFaceRecognition->numOfCentralFourierFeature  =  pAdvancedFaceRecognition->numOfCentralFourierFeature;
  m_pAdvancedFaceRecognition->extensionFlag               =  pAdvancedFaceRecognition->extensionFlag;
  
  for ( iw = 0 ; iw < pAdvancedFaceRecognition->numOfFourierFeature ; iw++ )
    m_pAdvancedFaceRecognition->FourierFeature[iw] = pAdvancedFaceRecognition->FourierFeature[iw];
  for ( iw = 0 ; iw < pAdvancedFaceRecognition->numOfCentralFourierFeature ; iw++ )
    m_pAdvancedFaceRecognition->CentralFourierFeature[iw] = pAdvancedFaceRecognition->CentralFourierFeature[iw];

  if ( pAdvancedFaceRecognition->extensionFlag ){
    m_pAdvancedFaceRecognition->numOfCompositeFeature =  pAdvancedFaceRecognition->numOfCompositeFeature;
    m_pAdvancedFaceRecognition->numOfCentralCompositeFeature  =  pAdvancedFaceRecognition->numOfCentralCompositeFeature;
    for ( iw = 0 ; iw < pAdvancedFaceRecognition->numOfCompositeFeature  ; iw++ )
      m_pAdvancedFaceRecognition->CompositeFeature[iw] = pAdvancedFaceRecognition->CompositeFeature[iw];
    for ( iw = 0 ; iw < pAdvancedFaceRecognition->numOfCentralCompositeFeature ; iw++ )
      m_pAdvancedFaceRecognition->CentralCompositeFeature[iw] = pAdvancedFaceRecognition->CentralCompositeFeature[iw];
  }

}


//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  
  if (!aParentDescription) return (unsigned long) -1;
  //sth please use parent node as input => first CreateChild
  
  GenericDS AdvancedFaceRecognitionDDLDescription;
  
  AdvancedFaceRecognitionDDLDescription = aParentDescription->CreateChild("Descriptor");
  AdvancedFaceRecognitionDDLDescription.SetAttribute("xsi:type", "AdvancedFaceRecognitionType");
  
  int iw;
  AdvancedFRD* AdvancedFRDDL;
  
  AdvancedFRDDL=GetAdvancedFaceRecognition();

  vector<int> hffeatures(AdvancedFRDDL->numOfFourierFeature);
  vector<int> cffeatures(AdvancedFRDDL->numOfCentralFourierFeature);

  for(iw = 0; iw < AdvancedFRDDL->numOfFourierFeature; iw++)
    hffeatures[iw] = AdvancedFRDDL->FourierFeature[iw];
  
  for(iw = 0; iw < AdvancedFRDDL->numOfCentralFourierFeature; iw++)
    cffeatures[iw] = AdvancedFRDDL->CentralFourierFeature[iw];
  
  GenericDS HFFeatureDDLDescription = AdvancedFaceRecognitionDDLDescription.CreateChild("FourierFeature");
  HFFeatureDDLDescription.SetValue(hffeatures);
  
  GenericDS CFFeatureDDLDescription = AdvancedFaceRecognitionDDLDescription.CreateChild("CentralFourierFeature");
  CFFeatureDDLDescription.SetValue(cffeatures);

  if ( AdvancedFRDDL->extensionFlag != 0 ){
    /* CompositeFeature */
    vector<int> hcfeatures(AdvancedFRDDL->numOfCompositeFeature);
    for(iw = 0; iw < AdvancedFRDDL->numOfCompositeFeature; iw++)
      hcfeatures[iw]=AdvancedFRDDL->CompositeFeature[iw];
    if ( AdvancedFRDDL->numOfCompositeFeature > 0 ){
      GenericDS HCFeatureDDLDescription = AdvancedFaceRecognitionDDLDescription.CreateChild("CompositeFeature");
      HCFeatureDDLDescription.SetValue(hcfeatures);
    }

    /* CentralCompositeFeature */
    vector<int> ccfeatures(AdvancedFRDDL->numOfCentralCompositeFeature);
    for(iw = 0; iw < AdvancedFRDDL->numOfCentralCompositeFeature; iw++)
      ccfeatures[iw]=AdvancedFRDDL->CentralCompositeFeature[iw];
    if ( AdvancedFRDDL->numOfCentralCompositeFeature > 0 ){
      GenericDS CCFeatureDDLDescription = AdvancedFaceRecognitionDDLDescription.CreateChild("CentralCompositeFeature");
      CCFeatureDDLDescription.SetValue(ccfeatures);
    }
    
  }
  
  return 0;
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int iw;
  string xsitype;
  AdvancedFRD* AdvancedFRDDL;
  
  if(!aDescription) return (unsigned long)-1;
  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
  
  if (aDescription->GetDSName() == "Descriptor")
    {
      aDescription->GetTextAttribute("xsi:type", xsitype);
      if (xsitype == "AdvancedFaceRecognitionType")
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
	  if (xsitype == "AdvancedFaceRecognitionType") break;
	  l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
	}
      
      if (l_DDLDescription.isNull())	return (unsigned long)-1;
      
      l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
    }
  
  GenericDS AdvancedFaceRecognitionDDLDescription_FourierFeature 
    = l_DDLDescriptionInterface->GetDescription("FourierFeature");
  GenericDS AdvancedFaceRecognitionDDLDescription_CentralFourierFeature 
    = l_DDLDescriptionInterface->GetDescription("CentralFourierFeature");
  
  GenericDS AdvancedFaceRecognitionDDLDescription_CompositeFeature 
    = l_DDLDescriptionInterface->GetDescription("CompositeFeature");
  GenericDS AdvancedFaceRecognitionDDLDescription_CentralCompositeFeature
    = l_DDLDescriptionInterface->GetDescription("CentralCompositeFeature");
  
  vector<int> hffeatures;
  vector<int> cffeatures;
  vector<int> hcfeatures;
  vector<int> ccfeatures;
  AdvancedFRDDL=new AdvancedFRD;
  
  AdvancedFaceRecognitionDDLDescription_FourierFeature.GetIntVector(hffeatures);
  AdvancedFaceRecognitionDDLDescription_CentralFourierFeature.GetIntVector(cffeatures);
  AdvancedFaceRecognitionDDLDescription_CompositeFeature.GetIntVector(hcfeatures);
  AdvancedFaceRecognitionDDLDescription_CentralCompositeFeature.GetIntVector(ccfeatures);
    
  AdvancedFRDDL->numOfFourierFeature = hffeatures.size();
  for(iw=0; iw< AdvancedFRDDL->numOfFourierFeature; iw++)
    AdvancedFRDDL->FourierFeature[iw]=hffeatures[iw];
  
  AdvancedFRDDL->numOfCentralFourierFeature = cffeatures.size();
  for(iw=0; iw< AdvancedFRDDL->numOfCentralFourierFeature; iw++)
    AdvancedFRDDL->CentralFourierFeature[iw]=cffeatures[iw];
  
  if (( hcfeatures.size() == 0 )&&(ccfeatures.size()==0)){
    AdvancedFRDDL->extensionFlag = 0;
  }
  else{
    AdvancedFRDDL->extensionFlag = 1;
    
    AdvancedFRDDL->numOfCompositeFeature = hcfeatures.size();
    for(iw = 0; iw < AdvancedFRDDL->numOfCompositeFeature; iw++)
      AdvancedFRDDL->CompositeFeature[iw] = hcfeatures[iw];
    
    AdvancedFRDDL->numOfCentralCompositeFeature  = ccfeatures.size();
    for(iw = 0; iw < AdvancedFRDDL->numOfCentralCompositeFeature; iw++)
      AdvancedFRDDL->CentralCompositeFeature[iw] = ccfeatures[iw];
  }
  
  SetAdvancedFaceRecognition(AdvancedFRDDL);
  
  delete	[] AdvancedFRDDL;
  return 0;
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC *AdvancedFaceRecognitionDescriptor::GetInterface(void)
{ return &m_Interface; }



