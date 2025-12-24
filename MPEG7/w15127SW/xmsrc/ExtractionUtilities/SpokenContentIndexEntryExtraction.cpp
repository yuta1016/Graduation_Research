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
// Applicable File Name:  SpokenContentIndexEntryExtraction.cpp
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


const UUID SpokenContentIndexEntryExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentIndexEntryExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentIndexEntryExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentIndexEntryExtractionTool::myName = "SpokenContentIndexEntry Type Descriptor Extractor";

const UUID SpokenContentIndexEntryExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentIndexEntryExtractionInterfaceABC::SpokenContentIndexEntryExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentIndexEntryExtractionInterface::SpokenContentIndexEntryExtractionInterface(
  SpokenContentIndexEntryExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryExtractionInterface::~SpokenContentIndexEntryExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentIndexEntryExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentIndexEntryExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentIndexEntryExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptorInterfaceABC* SpokenContentIndexEntryExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryExtractionInterface::
SetDescriptorInterface(SpokenContentIndexEntryDescriptorInterfaceABC
		       *aSpokenContentIndexEntryDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentIndexEntryDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentIndexEntryExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentIndexEntryExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentIndexEntryExtractionTool::SpokenContentIndexEntryExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentIndexEntryExtractionTool::
SpokenContentIndexEntryExtractionTool(SpokenContentIndexEntryDescriptorInterfaceABC *SpokenContentIndexEntry):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentIndexEntry) {
	  SpokenContentIndexEntryDescriptor *descriptor = new SpokenContentIndexEntryDescriptor();
	  SpokenContentIndexEntry=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentIndexEntry);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryExtractionTool::~SpokenContentIndexEntryExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentIndexEntryExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentIndexEntryExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentIndexEntryExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptorInterfaceABC* SpokenContentIndexEntryExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryExtractionTool::
SetDescriptorInterface(SpokenContentIndexEntryDescriptorInterfaceABC
		       *aSpokenContentIndexEntryDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentIndexEntryDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentIndexEntryDescriptorInterface;
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
unsigned long SpokenContentIndexEntryExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentIndexEntryExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentIndexEntryExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentIndexEntry Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the attributes
  string node,block,lattice;
  unsigned short inode;
  unsigned long iblock;
  if(m_Media->GetTextAttribute(string("node"),node) != 0) {
    fprintf(stderr,"%s Node attribute omitted\n",routine);
    return(failure);
  }
  inode=atol(node.c_str());
  if(inode>65535) {
    fprintf(stderr,"%s Node value out of range\n",routine);
    return(failure);
  }
  if(m_Media->GetTextAttribute(string("block"),block) != 0) {
    fprintf(stderr,"%s Block attribute omitted\n",routine);
    return(failure);
  }
  iblock=atol(block.c_str());

  // The lattice is optional
  if(m_Media->GetTextAttribute(string("lattice"),lattice) != 0) lattice="";

  // Put the values into the descriptor
  m_DescriptorInterface->SetNode(inode);
  m_DescriptorInterface->SetBlock(iblock);
  if(lattice=="") m_DescriptorInterface->SetLattice(NULL);
  else            m_DescriptorInterface->SetLattice(lattice.c_str());
 
  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentIndexEntryExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentIndexEntryExtractionInterfaceABC *SpokenContentIndexEntryExtractionTool::GetInterface(void)
{ return &m_Interface; }

