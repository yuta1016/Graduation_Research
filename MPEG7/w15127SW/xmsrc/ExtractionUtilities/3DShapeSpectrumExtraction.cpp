//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
// (contributing organizations names)
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
// Applicable File Name:  3DShapeSpectrumExtraction.cpp
//

#include <stdio.h>

#include "Media/Media.h" 
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "ExtractionUtilities/3d_shape_spectrum_extraction.h"

using namespace XM;

const UUID ShapeSpectrumExtractionInterface::myID = UUID("");
const char *ShapeSpectrumExtractionInterface::myName = "3D Shape Spectrum Extraction Interface";

const UUID ShapeSpectrumExtractionTool::myID = UUID("");
const char *ShapeSpectrumExtractionTool::myName = "3D Shape Spectrum Extraction Descriptor Extractor";

const UUID ShapeSpectrumExtractionInterfaceABC::myID = UUID("");

//=============================================================================
ShapeSpectrumExtractionInterfaceABC::ShapeSpectrumExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ShapeSpectrumExtractionInterface::ShapeSpectrumExtractionInterface(ShapeSpectrumExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeSpectrumExtractionInterface::~ShapeSpectrumExtractionInterface()
{
}

//----------------------------------------------------------------------------
void ShapeSpectrumExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int ShapeSpectrumExtractionInterface::
SetSourceMedia(MediaThreeD *Media)
{
  return m_ExtractionTool->SetSourceMedia(Media);
}

//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC 
*ShapeSpectrumExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ShapeSpectrumExtractionInterface::
SetDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *aShapeSpectrumDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aShapeSpectrumDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ShapeSpectrumExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long ShapeSpectrumExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
ShapeSpectrumExtractionTool::ShapeSpectrumExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media3D(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeSpectrumExtractionTool::
ShapeSpectrumExtractionTool(
			     ShapeSpectrumDescriptorInterfaceABC
			     *aDescriptorInterface):
m_Interface(this),
m_DescriptorInterface(0),
m_Media3D(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aDescriptorInterface) {
	  ShapeSpectrumDescriptor *descriptor =
	    new ShapeSpectrumDescriptor();
	  aDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(aDescriptorInterface);

	/* create sub extraction tools and
	   connect them with descriptor if availabe*/
	/* no sub extraction tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeSpectrumExtractionTool::~ShapeSpectrumExtractionTool()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool ShapeSpectrumExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ShapeSpectrumExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ShapeSpectrumExtractionTool::SetSourceMedia(MediaThreeD *Media)
{
  if (!Media) return -1;

  m_Media3D = Media; 
  return 0;
}


//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC*
ShapeSpectrumExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ShapeSpectrumExtractionTool::
SetDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *aDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == 
	    aDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
	  aDescriptorInterface;
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
unsigned long ShapeSpectrumExtractionTool::InitExtracting(void)

{
  /* example for initializing extraction paramters*/
  m_ShapeSpectrumSize=(unsigned long )atoi(getpara("NoOfBins","100"/*default
								     value*/));
  m_DescriptorInterface->SetShapeSpectrumSize(m_ShapeSpectrumSize);
  //  m_DescriptorInterface->SetNoOfBins(m_ShapeSpectrumSize);

  m_ShapeSpectrumNoOfBits=(unsigned long)atoi(getpara("NoOfBits","12"/*default
								     value*/));
  m_DescriptorInterface->SetNoOfBits(m_ShapeSpectrumNoOfBits);

  m_FrameCnt=0;//TOBEMODIFIED in you Application

  return(0UL);
}

//----------------------------------------------------------------------------
unsigned long ShapeSpectrumExtractionTool::StartExtracting(void)

{
  Int i;
  Int NoOfBins;
  Int NoOfBits;
  
   /* check for descriptor*/
  if (!m_DescriptorInterface) return (unsigned long)-1;
 
  if (strcmp(m_DescriptorInterface->GetName(),
	     "Shape Spectrum Description Interface") != 0) {
    return (unsigned long)-1;
  }
 
  /* check for media*/
  
  if (!m_Media3D) {
    return (unsigned long)-1;
  }
  

  NoOfBins = m_DescriptorInterface->GetShapeSpectrumSize();
  NoOfBits = m_DescriptorInterface->GetNoOfBits();
   
  unsigned long *ulss = new unsigned long [NoOfBins+2];
  
  for (i=0;i<NoOfBins;i++)
    ulss[i] = 0; 
  ulss[i++] = 1; ulss[i] = 0; /* initialisation */
 
  ShapeSpectrumEstimation(m_Media3D, NoOfBins, NoOfBits, 2, ulss);
 
  for (i=0;i<NoOfBins;i++)
    {
      //      fprintf(stderr, "StartExtracting: i = %d  ulss[i] = %d\n", i, ulss[i]); 
      m_DescriptorInterface->SetElement(i, ulss[i]);
    }
  
  m_DescriptorInterface->SetPlanarElement(ulss[i]); i++;
  m_DescriptorInterface->SetSingularElement(ulss[i]);
  

 if (ulss) delete [] ulss; 
 return 0;
}

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ShapeSpectrumExtractionInterfaceABC *ShapeSpectrumExtractionTool::GetInterface(void)
{ return &m_Interface; }

