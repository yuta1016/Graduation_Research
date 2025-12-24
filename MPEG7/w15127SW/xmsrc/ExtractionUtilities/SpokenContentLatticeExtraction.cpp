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
// Applicable File Name:  SpokenContentLatticeExtraction.cpp
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


const UUID SpokenContentLatticeExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentLatticeExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentLatticeExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentLatticeExtractionTool::myName = "SpokenContentLattice Type Descriptor Extractor";

const UUID SpokenContentLatticeExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentLatticeExtractionInterfaceABC::SpokenContentLatticeExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentLatticeExtractionInterface::SpokenContentLatticeExtractionInterface(
  SpokenContentLatticeExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLatticeExtractionInterface::~SpokenContentLatticeExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLatticeExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentLatticeExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentLatticeExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentLatticeDescriptorInterfaceABC* SpokenContentLatticeExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentLatticeExtractionInterface::
SetDescriptorInterface(SpokenContentLatticeDescriptorInterfaceABC
		       *aSpokenContentLatticeDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentLatticeDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentLatticeExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentLatticeExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentLatticeExtractionTool::SpokenContentLatticeExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentLatticeExtractionTool::
SpokenContentLatticeExtractionTool(SpokenContentLatticeDescriptorInterfaceABC *SpokenContentLattice):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentLattice) {
	  SpokenContentLatticeDescriptor *descriptor = new SpokenContentLatticeDescriptor();
	  SpokenContentLattice=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentLattice);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentLatticeExtractionTool::~SpokenContentLatticeExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentLatticeExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentLatticeExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentLatticeExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentLatticeDescriptorInterfaceABC* SpokenContentLatticeExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentLatticeExtractionTool::
SetDescriptorInterface(SpokenContentLatticeDescriptorInterfaceABC
		       *aSpokenContentLatticeDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentLatticeDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentLatticeDescriptorInterface;
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
unsigned long SpokenContentLatticeExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentLatticeExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentLatticeExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentLattice Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the id attribute
  string tmp;
  if(m_Media->GetTextAttribute(string("id"),tmp) != 0) {
    fprintf(stderr,"%s omitted id\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetLatticeID(tmp.c_str());

  // Get the blocks - must be 1 or more
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("Block"));
  if(list.size()==0) {
    fprintf(stderr,"%s must contain at least one block\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNBlocks(list.size());
  for(unsigned int i=0;i<list.size();i++) {
    SpokenContentBlockDescriptor* theDescriptor;
    SpokenContentBlockDescriptorInterfaceABC* theDescriptorInterface;
    SpokenContentBlockExtractionTool* theExtractionTool;
    SpokenContentBlockExtractionInterfaceABC* theExtractionInterface;
    theDescriptor = new SpokenContentBlockDescriptor();
    theDescriptor->addref();
    theDescriptorInterface = theDescriptor->GetInterface();
    theExtractionTool = new SpokenContentBlockExtractionTool(NULL);
    theExtractionInterface = theExtractionTool->GetInterface();

    theExtractionInterface->SetSourceMedia(&(list[i]));
    theExtractionInterface->SetDescriptorInterface(theDescriptorInterface);
    theExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetBlock(i,theDescriptor);
    theDescriptor->release();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLatticeExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentLatticeExtractionInterfaceABC *SpokenContentLatticeExtractionTool::GetInterface(void)
{ return &m_Interface; }

