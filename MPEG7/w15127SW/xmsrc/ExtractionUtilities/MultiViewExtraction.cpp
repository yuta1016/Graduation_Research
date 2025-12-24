///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2000-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiViewExtraction.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <fstream>
#include <math.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>

#include "Descriptors/Descriptors.h"                     // Naming Convention
#include "ExtractionUtilities/ExtractionUtilities.h"


using namespace XM;

const int COEFFSIZE=16;

//=============================================================================

const UUID MultiViewExtractionInterface::myID = UUID();
//sth no space and use correct name
const char *MultiViewExtractionInterface::myName = "Streaming Component Control Interface";

const UUID MultiViewExtractionTool::myID = UUID();
const char *MultiViewExtractionTool::myName = "MultiView Descriptor Extractor";

const UUID MultiViewExtractionInterfaceABC::myID = UUID();

//=============================================================================
MultiViewExtractionInterfaceABC::MultiViewExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MultiViewExtractionInterface::MultiViewExtractionInterface(MultiViewExtractionTool* aTool)
: m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiViewExtractionInterface::~MultiViewExtractionInterface()
{
}

//----------------------------------------------------------------------------
int MultiViewExtractionInterface::
SetDescriptorInterface(MultiViewDescriptorInterfaceABC
		       *aMultiViewDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aMultiViewDescriptorInterface);
}


//----------------------------------------------------------------------------
const UUID& MultiViewExtractionInterface::GetInterfaceID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *MultiViewExtractionInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
void MultiViewExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int MultiViewExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC **media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
MultiViewDescriptorInterfaceABC* MultiViewExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
SUB_EXTRACTION_INTERFACE_ABC* MultiViewExtractionInterface::Get2DExtractionInterface()
{
	return m_ExtractionTool->Get2DExtractionInterface();
}

//----------------------------------------------------------------------------
void MultiViewExtractionInterface::Set2DExtractionInterface( SUB_EXTRACTION_INTERFACE_ABC*
																											shapeExtraction )
{
	m_ExtractionTool->Set2DExtractionInterface( shapeExtraction );
}

//----------------------------------------------------------------------------
unsigned long MultiViewExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
MultiViewExtractionTool::MultiViewExtractionTool():
m_Interface(this),
m_pMultiViewDescriptorInterface(0),
m_p2DExtractionInterface(0),
m_Media(0)
{

	m_Media = new MultiMediaInterfaceABC*[TotNumberOfViews];

	
	SUB_EXTRACTION_TOOL *m_2DExtraction;

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	m_2DExtraction = new SUB_EXTRACTION_TOOL();
	// connect sub extraction tools with 3D extraction tool
	if (m_2DExtraction)
		Set2DExtractionInterface(m_2DExtraction->GetInterface());

  SetExposedInterface(&m_Interface);

}

//----------------------------------------------------------------------------
MultiViewExtractionTool::MultiViewExtractionTool(MultiViewDescriptorInterfaceABC *MultiView):
m_Interface(this),
m_pMultiViewDescriptorInterface(0),
m_p2DExtractionInterface(0),
m_Media(0)
{

	m_Media = new MultiMediaInterfaceABC*[TotNumberOfViews];
	
	SUB_EXTRACTION_TOOL *m_2DExtraction;

#ifdef VERBOSE
        fprintf(stderr,"Connect descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!MultiView) {
	  MultiViewDescriptor *descriptor =  new MultiViewDescriptor(0);
	  MultiView = descriptor->GetInterface();
	}

	// connect descritors with extraction tool
	SetDescriptorInterface(MultiView);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	if (MultiView->Get2DDescriptorInterface(0)) 
		// create sub coding schemes
	  //sth SUB_EXTRACTION_TOOL potental conflict with other components
		m_2DExtraction = new SUB_EXTRACTION_TOOL(MultiView->Get2DDescriptorInterface(0));
	else 
		m_2DExtraction = new SUB_EXTRACTION_TOOL();

	// connect sub coding schemes with 3D coding scheme
	if (m_2DExtraction)
		Set2DExtractionInterface(m_2DExtraction->GetInterface());

	SetExposedInterface(&m_Interface);

}

//----------------------------------------------------------------------------
MultiViewExtractionTool::~MultiViewExtractionTool()
{
  // destroy sub coding scheme
	if (m_p2DExtractionInterface) m_p2DExtractionInterface->destroy();

	// release descriptor
	if (m_pMultiViewDescriptorInterface)  
		m_pMultiViewDescriptorInterface->release();

	if(m_Media) delete [] m_Media;
}

//----------------------------------------------------------------------------
bool MultiViewExtractionTool::SupportsPush(void)
{
  return true;
}

//----------------------------------------------------------------------------
bool MultiViewExtractionTool::SupportsPull(void)
{
  return false;
}

//----------------------------------------------------------------------------
int MultiViewExtractionTool::SetSourceMedia(MultiMediaInterfaceABC **media)
{
  if (!media) return -1;
  if (!media[0]) return -1;

	m_p2DExtractionInterface->SetSourceMedia(media[0]);
	for( int i = 0; i < TotNumberOfViews; i++ ) 
		m_Media[i] = media[i];

  return 0;
}

//----------------------------------------------------------------------------
MultiViewDescriptorInterfaceABC* MultiViewExtractionTool::
GetDescriptorInterface(void)
{
        return m_pMultiViewDescriptorInterface;
}

//----------------------------------------------------------------------------
int MultiViewExtractionTool::
SetDescriptorInterface(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface)
{
	// check if new value is different from old value
	if (m_pMultiViewDescriptorInterface == aMultiViewDescriptorInterface) 
		return 0;
	
	// release old interface
	if (m_pMultiViewDescriptorInterface) 
		m_pMultiViewDescriptorInterface->release();
	
	// add new interface
	m_pMultiViewDescriptorInterface = aMultiViewDescriptorInterface;
	if (m_pMultiViewDescriptorInterface)
	{
		m_pMultiViewDescriptorInterface->addref();
		
		// conect sub descriptors with sub extraction tools
		if ((m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(0)) &&	m_p2DExtractionInterface)
				m_p2DExtractionInterface->
					SetDescriptorInterface(m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(0));
	}
	else 
	{
	  // if no descriptor, release also descriptor interfaces from sub coding schemes
		m_p2DExtractionInterface->SetDescriptorInterface(0);

		return -1;
	}

	return 0;
}


//----------------------------------------------------------------------------
SUB_EXTRACTION_INTERFACE_ABC *MultiViewExtractionTool::Get2DExtractionInterface()
{
	return m_p2DExtractionInterface;
}

//----------------------------------------------------------------------------
void MultiViewExtractionTool::
Set2DExtractionInterface(SUB_EXTRACTION_INTERFACE_ABC  *a2DExtractionInterface)
{
  m_p2DExtractionInterface = a2DExtractionInterface;
}

//----------------------------------------------------------------------------
unsigned long MultiViewExtractionTool::StartExtracting(void)

{
	// check for descriptor
  if (!m_pMultiViewDescriptorInterface) return (unsigned long)-1;

	if (strcmp(m_pMultiViewDescriptorInterface->GetName(), "MultiView Interface") != 0)
	{
	  return (unsigned long)-1;
	}

	for( int i = 0; i < TotNumberOfViews; i++ )
	{
		// check for 2D descriptor
		if (!m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(i))
			return (unsigned long)-1;
		// check for media
		if (!m_Media[i]) return (unsigned long)-1;
		// set media
		m_p2DExtractionInterface->SetSourceMedia(m_Media[i]);
		// link 2D descriptor[i] with extraction tool
		m_p2DExtractionInterface->SetDescriptorInterface(m_pMultiViewDescriptorInterface->
			Get2DDescriptorInterface(i));

		m_p2DExtractionInterface->StartExtracting();
		std::cout << " " << i << std::flush;

	}

	return 0;
}



//----------------------------------------------------------------------------
const UUID& MultiViewExtractionTool::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *MultiViewExtractionTool::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
MultiViewExtractionInterfaceABC *MultiViewExtractionTool::GetInterface(void)
{
  return &m_Interface;
}

//----------------------------------------------------------------------------
