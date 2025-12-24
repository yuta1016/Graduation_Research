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
// Applicable File Name:  SpokenContentLexiconExtraction.cpp
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


const UUID SpokenContentLexiconExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentLexiconExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentLexiconExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentLexiconExtractionTool::myName = "SpokenContentLexicon Type Descriptor Extractor";

const UUID SpokenContentLexiconExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentLexiconExtractionInterfaceABC::SpokenContentLexiconExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentLexiconExtractionInterface::SpokenContentLexiconExtractionInterface(
  SpokenContentLexiconExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLexiconExtractionInterface::~SpokenContentLexiconExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLexiconExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentLexiconExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentLexiconExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptorInterfaceABC* SpokenContentLexiconExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentLexiconExtractionInterface::
SetDescriptorInterface(SpokenContentLexiconDescriptorInterfaceABC
		       *aSpokenContentLexiconDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentLexiconDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentLexiconExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentLexiconExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentLexiconExtractionTool::SpokenContentLexiconExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentLexiconExtractionTool::
SpokenContentLexiconExtractionTool(SpokenContentLexiconDescriptorInterfaceABC *SpokenContentLexicon):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentLexicon) {
	  SpokenContentLexiconDescriptor *descriptor = new SpokenContentLexiconDescriptor();
	  SpokenContentLexicon=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentLexicon);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentLexiconExtractionTool::~SpokenContentLexiconExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentLexiconExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentLexiconExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentLexiconExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentLexiconDescriptorInterfaceABC* SpokenContentLexiconExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentLexiconExtractionTool::
SetDescriptorInterface(SpokenContentLexiconDescriptorInterfaceABC
		       *aSpokenContentLexiconDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentLexiconDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentLexiconDescriptorInterface;
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
unsigned long SpokenContentLexiconExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentLexiconExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentLexiconExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentLexicon Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the attributes - id is required but numOriginalEntries is optional
  string id,num;
  if(m_Media->GetTextAttribute(string("id"),id) != 0) {
    fprintf(stderr,"%s id was omitted\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetLexiconID(id.c_str());
  if(m_Media->GetTextAttribute(string("numOriginalEntries"),num) == 0) {
    unsigned long in=atol(num.c_str());
    m_DescriptorInterface->SetLexiconNumOriginalEntries(in);
  }

#if 0 // PNG
  // Get the decoder stuff - should be zero or one of these
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("Decoder"));
  if(list.size()==0) return(0); // All done
  if(list.size()>1) {
    fprintf(stderr,"%s Tow many decoder entries\n",routine);
    return(failure);
  }

  // Put the contents into the descriptor
  SpokenContentExtractionInfoDescriptor* theInfoDescriptor;
  SpokenContentExtractionInfoDescriptorInterfaceABC* theInfoDescriptorInterface;
  SpokenContentExtractionInfoExtractionTool* theExtractionTool;
  SpokenContentExtractionInfoExtractionInterfaceABC* theExtractionInterface;

  theInfoDescriptor = new SpokenContentExtractionInfoDescriptor();
  theInfoDescriptor->addref();
  theInfoDescriptorInterface = theInfoDescriptor->GetInterface();
  theExtractionTool = new SpokenContentExtractionInfoExtractionTool(NULL);
  theExtractionInterface = theExtractionTool->GetInterface();

  theExtractionInterface->SetSourceMedia(&(list[0]));
  theExtractionInterface->SetDescriptorInterface(theInfoDescriptorInterface);
  theExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetLexiconDecoder(theInfoDescriptor);
  theInfoDescriptor->release();
#endif
 
  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLexiconExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentLexiconExtractionInterfaceABC *SpokenContentLexiconExtractionTool::GetInterface(void)
{ return &m_Interface; }

