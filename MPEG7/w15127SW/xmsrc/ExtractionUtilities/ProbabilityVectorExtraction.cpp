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
// Applicable File Name:  ProbabilityVectorExtraction.cpp
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


const UUID ProbabilityVectorExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *ProbabilityVectorExtractionInterface::myName = "Streaming Component Control Interface";

const UUID ProbabilityVectorExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *ProbabilityVectorExtractionTool::myName = "ProbabilityVector Type Descriptor Extractor";

const UUID ProbabilityVectorExtractionInterfaceABC::myID = UUID();

//=============================================================================
ProbabilityVectorExtractionInterfaceABC::ProbabilityVectorExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ProbabilityVectorExtractionInterface::ProbabilityVectorExtractionInterface(
  ProbabilityVectorExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ProbabilityVectorExtractionInterface::~ProbabilityVectorExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityVectorExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ProbabilityVectorExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int ProbabilityVectorExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptorInterfaceABC* ProbabilityVectorExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ProbabilityVectorExtractionInterface::
SetDescriptorInterface(ProbabilityVectorDescriptorInterfaceABC
		       *aProbabilityVectorDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aProbabilityVectorDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ProbabilityVectorExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long ProbabilityVectorExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
ProbabilityVectorExtractionTool::ProbabilityVectorExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ProbabilityVectorExtractionTool::
ProbabilityVectorExtractionTool(ProbabilityVectorDescriptorInterfaceABC *ProbabilityVector):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!ProbabilityVector) {
	  ProbabilityVectorDescriptor *descriptor = new ProbabilityVectorDescriptor();
	  ProbabilityVector=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(ProbabilityVector);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ProbabilityVectorExtractionTool::~ProbabilityVectorExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool ProbabilityVectorExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ProbabilityVectorExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int ProbabilityVectorExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
ProbabilityVectorDescriptorInterfaceABC* ProbabilityVectorExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ProbabilityVectorExtractionTool::
SetDescriptorInterface(ProbabilityVectorDescriptorInterfaceABC
		       *aProbabilityVectorDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aProbabilityVectorDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aProbabilityVectorDescriptorInterface;
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
unsigned long ProbabilityVectorExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long ProbabilityVectorExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="ProbabilityVectorExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "ProbabilityVector Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the dimension - this is optional
  string dim="";
  m_Media->GetTextAttribute(string("dim"),dim);

  // Get the contents
  vector<double> v;
  if(m_Media->GetRealVector(v) != 0) {
    fprintf(stderr,"%s missing contents\n",routine);
    return(failure);
  }
  if(dim != "") {
    if(v.size() != atol(dim.c_str())) {
     fprintf(stderr,"%s contents incorrect length\n",routine);
     return(failure);
    }
  }
  m_DescriptorInterface->SetSize(v.size());

  // Copy the data across
  for(unsigned long i=0;i<v.size();i++) {
    m_DescriptorInterface->SetValue(static_cast<float>(v[i]),i);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityVectorExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ProbabilityVectorExtractionInterfaceABC *ProbabilityVectorExtractionTool::GetInterface(void)
{ return &m_Interface; }

