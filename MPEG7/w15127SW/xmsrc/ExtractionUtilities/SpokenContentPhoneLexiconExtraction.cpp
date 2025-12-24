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
// Applicable File Name:  SpokenContentPhoneLexiconExtraction.cpp
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


const UUID SpokenContentPhoneLexiconExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLexiconExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentPhoneLexiconExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLexiconExtractionTool::myName = "SpokenContentPhoneLexicon Type Descriptor Extractor";

const UUID SpokenContentPhoneLexiconExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentPhoneLexiconExtractionInterfaceABC::SpokenContentPhoneLexiconExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentPhoneLexiconExtractionInterface::SpokenContentPhoneLexiconExtractionInterface(
  SpokenContentPhoneLexiconExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconExtractionInterface::~SpokenContentPhoneLexiconExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLexiconExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentPhoneLexiconExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptorInterfaceABC* SpokenContentPhoneLexiconExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconExtractionInterface::
SetDescriptorInterface(SpokenContentPhoneLexiconDescriptorInterfaceABC
		       *aSpokenContentPhoneLexiconDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentPhoneLexiconDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentPhoneLexiconExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentPhoneLexiconExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentPhoneLexiconExtractionTool::SpokenContentPhoneLexiconExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentPhoneLexiconExtractionTool::
SpokenContentPhoneLexiconExtractionTool(SpokenContentPhoneLexiconDescriptorInterfaceABC *SpokenContentPhoneLexicon):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentPhoneLexicon) {
	  SpokenContentPhoneLexiconDescriptor *descriptor = new SpokenContentPhoneLexiconDescriptor();
	  SpokenContentPhoneLexicon=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentPhoneLexicon);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconExtractionTool::~SpokenContentPhoneLexiconExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentPhoneLexiconExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentPhoneLexiconExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentPhoneLexiconExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptorInterfaceABC* SpokenContentPhoneLexiconExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconExtractionTool::
SetDescriptorInterface(SpokenContentPhoneLexiconDescriptorInterfaceABC
		       *aSpokenContentPhoneLexiconDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentPhoneLexiconDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentPhoneLexiconDescriptorInterface;
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
unsigned long SpokenContentPhoneLexiconExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentPhoneLexiconExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentPhoneLexiconExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentPhoneLexicon Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the name attribute - the default is "sampa"
  string nm;
  if(m_Media->GetTextAttribute(string("phoneticAlphabet"),nm) != 0) {
    m_DescriptorInterface->SetPhonesetName("sampa");
  }
  else {
    if((nm != "sampa")&&(nm != "ipaSymbol")&&(nm != "ipaNumber")&&
       (nm != "other")) {
      fprintf(stderr,"%s unknown name\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetPhonesetName(nm.c_str());
  }

  // Get the phones
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("Token"));
  if(list.size() == 0) {
    fprintf(stderr,"%s no phones specified\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNPhones(list.size());
  for(unsigned int i=0;i<list.size();i++) {
    string tmp;
    if(list[i].GetTextValue(tmp) != 0) {
      fprintf(stderr,"%s missing contents\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetPhone(i,tmp.c_str());
  }

  // Now do the lexicon component
  SpokenContentLexiconDescriptor* theLexDescriptor;
  SpokenContentLexiconDescriptorInterfaceABC* theLexDescriptorInterface;
  SpokenContentLexiconExtractionTool* theLexExtractionTool;
  SpokenContentLexiconExtractionInterfaceABC* theLexExtractionInterface;
  theLexDescriptor = new SpokenContentLexiconDescriptor();
  theLexDescriptor->addref();
  theLexDescriptorInterface = theLexDescriptor->GetInterface();
  theLexExtractionTool = new SpokenContentLexiconExtractionTool(NULL);
  theLexExtractionInterface = theLexExtractionTool->GetInterface();

  theLexExtractionInterface->SetSourceMedia(m_Media);
  theLexExtractionInterface->SetDescriptorInterface(theLexDescriptorInterface);
  theLexExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetLexicon(theLexDescriptor);
  theLexDescriptor->release();

  // Now do a sanity check

  // First, if numOriginalEntries is given it should be >= size
  unsigned long no=m_DescriptorInterface->GetLexicon()->GetLexiconNumOriginalEntries();
  unsigned long np=m_DescriptorInterface->GetNPhones();
  if((no != 0) && (no < np)) {
    fprintf(stderr,"%s numOriginalEntries & number of phones inconsistent\n",routine);
    return(failure);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLexiconExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconExtractionInterfaceABC *SpokenContentPhoneLexiconExtractionTool::GetInterface(void)
{ return &m_Interface; }

