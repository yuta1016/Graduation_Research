//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  SpokenContentConfusionInfoExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID SpokenContentConfusionInfoExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentConfusionInfoExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentConfusionInfoExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentConfusionInfoExtractionTool::myName = "SpokenContentConfusionInfo Type Descriptor Extractor";

const UUID SpokenContentConfusionInfoExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentConfusionInfoExtractionInterfaceABC::SpokenContentConfusionInfoExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentConfusionInfoExtractionInterface::SpokenContentConfusionInfoExtractionInterface(
  SpokenContentConfusionInfoExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoExtractionInterface::~SpokenContentConfusionInfoExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentConfusionInfoExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentConfusionInfoExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentConfusionInfoExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptorInterfaceABC* SpokenContentConfusionInfoExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoExtractionInterface::
SetDescriptorInterface(SpokenContentConfusionInfoDescriptorInterfaceABC
		       *aSpokenContentConfusionInfoDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentConfusionInfoDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentConfusionInfoExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentConfusionInfoExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentConfusionInfoExtractionTool::SpokenContentConfusionInfoExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentConfusionInfoExtractionTool::
SpokenContentConfusionInfoExtractionTool(SpokenContentConfusionInfoDescriptorInterfaceABC *SpokenContentConfusionInfo):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentConfusionInfo) {
	  SpokenContentConfusionInfoDescriptor *descriptor = new SpokenContentConfusionInfoDescriptor();
	  SpokenContentConfusionInfo=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentConfusionInfo);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoExtractionTool::~SpokenContentConfusionInfoExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentConfusionInfoExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentConfusionInfoExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentConfusionInfoExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptorInterfaceABC* SpokenContentConfusionInfoExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoExtractionTool::
SetDescriptorInterface(SpokenContentConfusionInfoDescriptorInterfaceABC
		       *aSpokenContentConfusionInfoDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentConfusionInfoDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentConfusionInfoDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long SpokenContentConfusionInfoExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentConfusionInfoExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentConfusionInfoExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentConfusionInfo Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the size attribute. This is required
  string sz;
  if(m_Media->GetTextAttribute(string("numOfDimensions"),sz) != 0) {
    fprintf(stderr,"%s Omitted dim attribute\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetSize(static_cast<short> (atol(sz.c_str())));
  
  // Get the id attribute. This is required
  if(m_Media->GetTextAttribute(string("id"),sz) != 0) {
    fprintf(stderr,"%s Omitted id attribute\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetConfusionID(sz.c_str());
  
  ProbabilityVectorDescriptor* thePVecDescriptor;
  ProbabilityVectorDescriptorInterfaceABC* thePVecDescriptorInterface;
  ProbabilityVectorExtractionTool* thePVecExtractionTool;
  ProbabilityVectorExtractionInterfaceABC* thePVecExtractionInterface;
  vector<GenericDS> list;

#if 0
  // Read in the occurance prob
  list=m_Media->GetAllDescriptions(string("PhoneOccuranceProb"));
  if(list.size() != 1) {
    fprintf(stderr,"%s Omitted occurance probabilities\n",routine);
    return(failure);
  }

  thePVecDescriptor = new ProbabilityVectorDescriptor();
  thePVecDescriptor->addref();
  thePVecDescriptorInterface = thePVecDescriptor->GetInterface();
  thePVecExtractionTool = new ProbabilityVectorExtractionTool(NULL);
  thePVecExtractionInterface = thePVecExtractionTool->GetInterface();

  thePVecExtractionInterface->SetSourceMedia(&(list[0]));
  thePVecExtractionInterface->SetDescriptorInterface(thePVecDescriptorInterface);
  thePVecExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetOccuranceProb(thePVecDescriptor);
  thePVecDescriptor->release();
#endif 

  // Read in the insertion prob
  list=m_Media->GetAllDescriptions(string("Insertion"));
  if(list.size() != 1) {
    fprintf(stderr,"%s Omitted insertion probabilities\n",routine);
    return(failure);
  }

#if 0
  // Get the insertion attribute
  string uncond;
  if(list[0].GetTextAttribute(string("unconditional"),uncond) != 0) {
    fprintf(stderr,"%s Omitted unconditional attribute\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetUnconditionalInsertionProb(atof(uncond.c_str()));
#endif

  thePVecDescriptor = new ProbabilityVectorDescriptor();
  thePVecDescriptor->addref();
  thePVecDescriptorInterface = thePVecDescriptor->GetInterface();
  thePVecExtractionTool = new ProbabilityVectorExtractionTool(NULL);
  thePVecExtractionInterface = thePVecExtractionTool->GetInterface();

  thePVecExtractionInterface->SetSourceMedia(&(list[0]));
  thePVecExtractionInterface->SetDescriptorInterface(thePVecDescriptorInterface);
  thePVecExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetInsertionProb(thePVecDescriptor);
  thePVecDescriptor->release();

   // Read in the omission prob
  list=m_Media->GetAllDescriptions(string("Deletion"));
  if(list.size() != 1) {
    fprintf(stderr,"%s Omitted omission probabilities\n",routine);
    return(failure);
  }

  thePVecDescriptor = new ProbabilityVectorDescriptor();
  thePVecDescriptor->addref();
  thePVecDescriptorInterface = thePVecDescriptor->GetInterface();
  thePVecExtractionTool = new ProbabilityVectorExtractionTool(NULL);
  thePVecExtractionInterface = thePVecExtractionTool->GetInterface();

  thePVecExtractionInterface->SetSourceMedia(&(list[0]));
  thePVecExtractionInterface->SetDescriptorInterface(thePVecDescriptorInterface);
  thePVecExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetOmissionProb(thePVecDescriptor);
  thePVecDescriptor->release();

   // Read in the confusion prob
  list=m_Media->GetAllDescriptions(string("Substitution"));
  if(list.size() != 1) {
    fprintf(stderr,"%s Omitted confusion probabilities\n",routine);
    return(failure);
  }

  ProbabilityMatrixDescriptor* thePMatDescriptor;
  ProbabilityMatrixDescriptorInterfaceABC* thePMatDescriptorInterface;
  ProbabilityMatrixExtractionTool* thePMatExtractionTool;
  ProbabilityMatrixExtractionInterfaceABC* thePMatExtractionInterface;

  thePMatDescriptor = new ProbabilityMatrixDescriptor();
  thePMatDescriptor->addref();
  thePMatDescriptorInterface = thePMatDescriptor->GetInterface();
  thePMatExtractionTool = new ProbabilityMatrixExtractionTool(NULL);
  thePMatExtractionInterface = thePMatExtractionTool->GetInterface();

  thePMatExtractionInterface->SetSourceMedia(&(list[0]));
  thePMatExtractionInterface->SetDescriptorInterface(thePMatDescriptorInterface);
  thePMatExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetConfusionProb(thePMatDescriptor);
  thePMatDescriptor->release();

  // All done
  return 0;
}


//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentConfusionInfoExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentConfusionInfoExtractionInterfaceABC *SpokenContentConfusionInfoExtractionTool::GetInterface(void)
{ return &m_Interface; }

