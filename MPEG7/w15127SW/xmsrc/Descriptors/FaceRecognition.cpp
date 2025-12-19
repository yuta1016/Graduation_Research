///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Applicable File Name:  FaceRecognition.cpp
//
#include <stdio.h>
#include <memory.h>

#include "Descriptors/Descriptors.h"
#include "FaceRecognition.h"

//#include <vector>

using namespace XM;

const UUID FaceRecognitionDescriptorInterface::myID = UUID("");
const char *FaceRecognitionDescriptorInterface::myName = "FaceRecognition Descriptor Interface";

const UUID FaceRecognitionDescriptor::myID = UUID("");
const char *FaceRecognitionDescriptor::myName = "FaceRecognition Descriptor";

const UUID FaceRecognitionDescriptor::valID = UUID("");
const char *FaceRecognitionDescriptor::valName = "";

const UUID FaceRecognitionDescriptorInterfaceABC::myID = UUID();


FaceRecognitionDescriptorInterfaceABC::FaceRecognitionDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
FaceRecognitionDescriptorInterface::FaceRecognitionDescriptorInterface(FaceRecognitionDescriptor *aFaceRecognition):
m_FaceRecognitionDescriptor(aFaceRecognition)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterface::~FaceRecognitionDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void FaceRecognitionDescriptorInterface::addref(void)
{ m_FaceRecognitionDescriptor->addref(); }

//----------------------------------------------------------------------------
void FaceRecognitionDescriptorInterface::release(void)
{ m_FaceRecognitionDescriptor->release(); }

//----------------------------------------------------------------------------
void FaceRecognitionDescriptorInterface::
SetValuesFromParameterFile(void)
{

}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
FRD*	FaceRecognitionDescriptorInterface::GetFaceRecognition()
{
	return	m_FaceRecognitionDescriptor->GetFaceRecognition();	
}

//----------------------------------------------------------------------------
void	FaceRecognitionDescriptorInterface::
SetFaceRecognition(FRD*	pFaceRecognition)
{
	m_FaceRecognitionDescriptor->SetFaceRecognition(pFaceRecognition);	
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_FaceRecognitionDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long FaceRecognitionDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_FaceRecognitionDescriptor->ImportDDL(aDescription); }


//=============================================================================
FaceRecognitionDescriptor::FaceRecognitionDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
	m_pFaceRecognition=NULL;
	SetExposedInterface(&m_Interface);
	m_pFaceRecognition = new	FRD[1];
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptor::~FaceRecognitionDescriptor()
{
	if( m_pFaceRecognition )
		delete [] m_pFaceRecognition;
#ifdef VERBOSE
       fprintf(stderr,"FaceRecognition delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& FaceRecognitionDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* FaceRecognitionDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool FaceRecognitionDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void FaceRecognitionDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"FaceRecognition connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void FaceRecognitionDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"FaceRecognition release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
FRD*	FaceRecognitionDescriptor::GetFaceRecognition()
{
	return	m_pFaceRecognition;
}

//----------------------------------------------------------------------------
void	FaceRecognitionDescriptor::SetFaceRecognition(FRD*	pFaceRecognition)
{
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	memcpy(m_pFaceRecognition->eigenfeature, pFaceRecognition->eigenfeature,
	       48*sizeof(int) );
//	memcpy(m_pFaceRecognition->eigenfeature, pFaceRecognition->eigenfeature,
//	       49*sizeof(int) );
}


//----------------------------------------------------------------------------
unsigned long FaceRecognitionDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  
	if (!aParentDescription) return (unsigned long) -1;
	//sth please use parent node as input => first CreateChild

	GenericDS FaceRecognitionDDLDescription;

	FaceRecognitionDDLDescription = aParentDescription->CreateChild("Descriptor");
	FaceRecognitionDDLDescription.SetAttribute("xsi:type", "FaceRecognitionType");
	
	int i;
	FRD* FRDDL;
	vector<int> features(48);

	FRDDL=GetFaceRecognition();
	for(i=0; i<48; i++) features[i]=FRDDL->eigenfeature[i];

	GenericDS FRFeatureDDLDescription = FaceRecognitionDDLDescription.CreateChild("Feature");
	FRFeatureDDLDescription.SetValue(features);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
	int i;
	string xsitype;
	FRD* FRDDL;

	if(!aDescription) return (unsigned long)-1;
	GenericDS l_DDLDescription;
	GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

	if (aDescription->GetDSName() == "Descriptor")
		{
		aDescription->GetTextAttribute("xsi:type", xsitype);
		if (xsitype == "FaceRecognitionType")
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
			if (xsitype == "FaceRecognitionType") break;
			l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
			}

		if (l_DDLDescription.isNull())	return (unsigned long)-1;

		l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
		}


	GenericDS FaceRecognitionDDLDescription = l_DDLDescriptionInterface->GetDescription("Feature");

	vector<int> features;
	FRDDL=new FRD;

	FaceRecognitionDDLDescription.GetIntVector(features);
	for(i=0; i<48; i++) FRDDL->eigenfeature[i]=features[i];
	SetFaceRecognition(FRDDL);

	delete	[] FRDDL;
	return 0;
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterfaceABC *FaceRecognitionDescriptor::GetInterface(void)
{ return &m_Interface; }
