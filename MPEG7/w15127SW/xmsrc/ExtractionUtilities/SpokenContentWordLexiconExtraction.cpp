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
// Applicable File Name:  SpokenContentWordLexiconExtraction.cpp
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


const UUID SpokenContentWordLexiconExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLexiconExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentWordLexiconExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLexiconExtractionTool::myName = "SpokenContentWordLexicon Type Descriptor Extractor";

const UUID SpokenContentWordLexiconExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentWordLexiconExtractionInterfaceABC::SpokenContentWordLexiconExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentWordLexiconExtractionInterface::SpokenContentWordLexiconExtractionInterface(
  SpokenContentWordLexiconExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconExtractionInterface::~SpokenContentWordLexiconExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLexiconExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentWordLexiconExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentWordLexiconExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptorInterfaceABC* SpokenContentWordLexiconExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconExtractionInterface::
SetDescriptorInterface(SpokenContentWordLexiconDescriptorInterfaceABC
		       *aSpokenContentWordLexiconDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentWordLexiconDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentWordLexiconExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentWordLexiconExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentWordLexiconExtractionTool::SpokenContentWordLexiconExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentWordLexiconExtractionTool::
SpokenContentWordLexiconExtractionTool(SpokenContentWordLexiconDescriptorInterfaceABC *SpokenContentWordLexicon):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentWordLexicon) {
	  SpokenContentWordLexiconDescriptor *descriptor = new SpokenContentWordLexiconDescriptor();
	  SpokenContentWordLexicon=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentWordLexicon);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconExtractionTool::~SpokenContentWordLexiconExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentWordLexiconExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentWordLexiconExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentWordLexiconExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptorInterfaceABC* SpokenContentWordLexiconExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconExtractionTool::
SetDescriptorInterface(SpokenContentWordLexiconDescriptorInterfaceABC
		       *aSpokenContentWordLexiconDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentWordLexiconDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentWordLexiconDescriptorInterface;
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
unsigned long SpokenContentWordLexiconExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentWordLexiconExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentWordLexiconExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentWordLexicon Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the words
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("Token"));
  m_DescriptorInterface->SetNWords(list.size());
  for(unsigned int i=0;i<list.size();i++) {
    string tmp;
    if(list[i].GetTextValue(tmp) != 0) {
      fprintf(stderr,"%s missing contents\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetWord(i,tmp.c_str());
  }

  // Now do the lexicon component
  SpokenContentLexiconDescriptor* theDescriptor;
  SpokenContentLexiconDescriptorInterfaceABC* theDescriptorInterface;
  SpokenContentLexiconExtractionTool* theExtractionTool;
  SpokenContentLexiconExtractionInterfaceABC* theExtractionInterface;
  theDescriptor = new SpokenContentLexiconDescriptor();
  theDescriptor->addref();
  theDescriptorInterface = theDescriptor->GetInterface();
  theExtractionTool = new SpokenContentLexiconExtractionTool(NULL);
  theExtractionInterface = theExtractionTool->GetInterface();

  theExtractionInterface->SetSourceMedia(m_Media);
  theExtractionInterface->SetDescriptorInterface(theDescriptorInterface);
  theExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetLexicon(theDescriptor);
  theDescriptor->release();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLexiconExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentWordLexiconExtractionInterfaceABC *SpokenContentWordLexiconExtractionTool::GetInterface(void)
{ return &m_Interface; }

