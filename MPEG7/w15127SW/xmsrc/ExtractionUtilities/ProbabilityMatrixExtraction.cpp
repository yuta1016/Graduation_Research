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
// Applicable File Name:  ProbabilityMatrixExtraction.cpp
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


const UUID ProbabilityMatrixExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *ProbabilityMatrixExtractionInterface::myName = "Streaming Component Control Interface";

const UUID ProbabilityMatrixExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *ProbabilityMatrixExtractionTool::myName = "ProbabilityMatrix Type Descriptor Extractor";

const UUID ProbabilityMatrixExtractionInterfaceABC::myID = UUID();

//=============================================================================
ProbabilityMatrixExtractionInterfaceABC::ProbabilityMatrixExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ProbabilityMatrixExtractionInterface::ProbabilityMatrixExtractionInterface(
  ProbabilityMatrixExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ProbabilityMatrixExtractionInterface::~ProbabilityMatrixExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityMatrixExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ProbabilityMatrixExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int ProbabilityMatrixExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
ProbabilityMatrixDescriptorInterfaceABC* ProbabilityMatrixExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ProbabilityMatrixExtractionInterface::
SetDescriptorInterface(ProbabilityMatrixDescriptorInterfaceABC
		       *aProbabilityMatrixDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aProbabilityMatrixDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ProbabilityMatrixExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long ProbabilityMatrixExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
ProbabilityMatrixExtractionTool::ProbabilityMatrixExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ProbabilityMatrixExtractionTool::
ProbabilityMatrixExtractionTool(ProbabilityMatrixDescriptorInterfaceABC *ProbabilityMatrix):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!ProbabilityMatrix) {
	  ProbabilityMatrixDescriptor *descriptor = new ProbabilityMatrixDescriptor();
	  ProbabilityMatrix=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(ProbabilityMatrix);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ProbabilityMatrixExtractionTool::~ProbabilityMatrixExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool ProbabilityMatrixExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ProbabilityMatrixExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int ProbabilityMatrixExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
ProbabilityMatrixDescriptorInterfaceABC* ProbabilityMatrixExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ProbabilityMatrixExtractionTool::
SetDescriptorInterface(ProbabilityMatrixDescriptorInterfaceABC
		       *aProbabilityMatrixDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aProbabilityMatrixDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aProbabilityMatrixDescriptorInterface;
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
unsigned long ProbabilityMatrixExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long ProbabilityMatrixExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="ProbabilityMatrixExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "ProbabilityMatrix Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the dimensions
  string dims;
  if(m_Media->GetTextAttribute(string("dim"),dims) != 0) {
    fprintf(stderr,"%s dim was omitted\n",routine);
    return(failure);
  }

  // Should only contain +ve integers or whitespace
  unsigned int lr,lc,ld; // Last is a dummy to check for >2 parameters
  if(sscanf(dims.c_str(),"%u%u%u",&lr,&lc,&ld) != 2) {
    fprintf(stderr,"%s dim must be two whitespace separated ints\n",routine);
    return(failure);
  }
  if((lr<0) || (lc<0)) {
    fprintf(stderr,"%s dims must be positive\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetSize(lr,lc);

  // Get the contents
  vector<double> v;
  if(m_Media->GetRealVector(v) != 0) {
    fprintf(stderr,"%s missing contents\n",routine);
    return(failure);
  }
  if(v.size() != lr*lc) {
    fprintf(stderr,"%s contents incorrect length\n",routine);
    return(failure);
  }

  // Copy the data across
  for(unsigned long r=0;r<lr;r++) {
    for(unsigned long c=0;c<lc;c++) {
      m_DescriptorInterface->SetValue(static_cast<float>(v[c+lc*r]),r,c);
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityMatrixExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ProbabilityMatrixExtractionInterfaceABC *ProbabilityMatrixExtractionTool::GetInterface(void)
{ return &m_Interface; }

