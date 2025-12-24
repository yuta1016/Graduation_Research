//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Vincent Hsu, June 11, 2001
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GoFGoPColorExtraction.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

//static int initextractionwascalled=0;
extern "C" int GOF_ulongcmp(const void *arg1, const void *arg2);

//----------------------------------------------------------------------------
int GOF_ulongcmp(const void *arg1, const void *arg2)
{
  if (*((unsigned long*)arg1) < *((unsigned long*)arg2)) return -1;
  if (*((unsigned long*)arg1) > *((unsigned long*)arg2)) return 1;
  return 0;
}
//=============================================================================
	

using namespace XM;


const UUID GoFGoPColorExtractionInterface::myID =
UUID("290d7900-db6e-11d2-affe-0080c7e1e76e");
const char *GoFGoPColorExtractionInterface::myName =
"GoFGoPColorDescriptorExtractorInterface";

const UUID GoFGoPColorExtractionTool::myID =
UUID("bff2b340-db31-11d2-affe-0080c7e1e76e");
const char *GoFGoPColorExtractionTool::myName = "GoFGoPColorDescriptorExtractor";

const UUID GoFGoPColorExtractionInterfaceABC::myID = UUID();


//=============================================================================
GoFGoPColorExtractionInterfaceABC::GoFGoPColorExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
GoFGoPColorExtractionInterface::
GoFGoPColorExtractionInterface(GoFGoPColorExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GoFGoPColorExtractionInterface::~GoFGoPColorExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& GoFGoPColorExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void GoFGoPColorExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int GoFGoPColorExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC* GoFGoPColorExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int GoFGoPColorExtractionInterface::
SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *aGoFGoPColorDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aGoFGoPColorDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long GoFGoPColorExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long GoFGoPColorExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long GoFGoPColorExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
GoFGoPColorExtractionTool::GoFGoPColorExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_Aggregation(0),
m_NoOfHistograms(0),
m_HistogramList(0)

{
  /* create sub Extractions and
     connect them with descriptor if availabe*/
  ScalableColorExtractionTool
    *aScalableColorExtraction= new ScalableColorExtractionTool();

  /* connect sub Extractions with GoFGoPColor Extraction*/
  if (m_ScalableColorExtraction)
    m_ScalableColorExtraction=aScalableColorExtraction->GetInterface();
  //    SetScalableColorExtractionInterface(m_ScalableColorExtraction->
  //					GetInterface());
  
  SetExposedInterface(&m_Interface);
}

//=============================================================================
GoFGoPColorExtractionTool::
GoFGoPColorExtractionTool(GoFGoPColorDescriptorInterfaceABC
			*aGoFGoPColor):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_Aggregation(0),
m_NoOfHistograms(0),
m_HistogramList(0)

{
  /* create descriptor if it does not exist*/
  if (!aGoFGoPColor) {
    GoFGoPColorDescriptor *descriptor =
      new GoFGoPColorDescriptor(0); /* one argument for each sub-component*/
      aGoFGoPColor=descriptor->GetInterface();
  }

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub Extraction and
     connect them with descriptor if availabe*/
  {
    ScalableColorExtractionTool *aScalableColorExtraction;

    if (aGoFGoPColor->GetScalableColorDescriptorInterface()) {
      aScalableColorExtraction= new
	ScalableColorExtractionTool(aGoFGoPColor->
				    GetScalableColorDescriptorInterface());
    }
    else { 
      aScalableColorExtraction= new ScalableColorExtractionTool();
    }
    if (aScalableColorExtraction) {
      m_ScalableColorExtraction=aScalableColorExtraction->GetInterface();
      //      SetScalableColorExtractionInterface(m_ScalableColorExtraction);
    }
  }

  /* connect descritors with Extraction*/
  SetDescriptorInterface(aGoFGoPColor);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GoFGoPColorExtractionTool::~GoFGoPColorExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
  if (m_ScalableColorExtraction) m_ScalableColorExtraction->destroy();
  while (m_HistogramList) {
    /* destroy list*/
    TGOFHISTLIST *next;

    next=m_HistogramList->next;
    if (m_HistogramList->histogram) free(m_HistogramList->histogram);
    free(m_HistogramList);
    m_HistogramList=next;
  }
}

//----------------------------------------------------------------------------
bool GoFGoPColorExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool GoFGoPColorExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int GoFGoPColorExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  if (m_ScalableColorExtraction)
    m_ScalableColorExtraction->SetSourceMedia(media);
  return 0;
}


//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC* GoFGoPColorExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int GoFGoPColorExtractionTool::
SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *aGoFGoPColorDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aGoFGoPColorDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aGoFGoPColorDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

    /* conect sub descriptors with sub Extraction*/
    if ((m_DescriptorInterface->
	 GetScalableColorDescriptorInterface()) &&
	m_ScalableColorExtraction) {
      m_ScalableColorExtraction->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetScalableColorDescriptorInterface());
    }
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub Extraction*/
    m_ScalableColorExtraction->SetDescriptorInterface(0);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long GoFGoPColorExtractionTool::InitExtracting(void)

{
  /* example for initializing extraction paramters*/
  m_Aggregation=atoi(getpara("AggregationMode","0"/*default value*/));

  /* init extraction of ScalableColor description*/
  // write extracted data to Descriptor
  switch (m_Aggregation) {
  case 0:
    m_DescriptorInterface->SetAggregation(Average);
    break;
  case 1:
    m_DescriptorInterface->SetAggregation(Median);
    break;
  case 2:
    m_DescriptorInterface->SetAggregation(Intersection);
    break;
  default:
    fprintf(stderr,"Warning: Unkown Aggregation type\n");
    m_DescriptorInterface->SetAggregation(Reserved);
  }

  m_ScalableColorExtraction->InitExtracting();

  while (m_HistogramList) {
    /* destroy list*/
    TGOFHISTLIST *next;

    next=m_HistogramList->next;
    if (m_HistogramList->histogram) free(m_HistogramList->histogram);
    free(m_HistogramList);
    m_HistogramList=next;
  }

  m_NoOfHistograms=0;
  
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long GoFGoPColorExtractionTool::StartExtracting(void)

{
  // select media type you require and remove comment
  MomVop *ImageMedia;
  unsigned long no_of_bins;
  unsigned long *SC_histogram, *histogram;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "GoFGoPColorDescriptionInterface") != 0) return (unsigned long)-1;


  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;

  /* extraction of ScalableColor description*/
  SC_histogram=m_ScalableColorExtraction->ComputeNormalizedHistogram();
  if (!SC_histogram) {
    fprintf(stderr,"Error: (GoF) did not get the histogram from "
	    "ScalableColor\n");
    return (unsigned long) -1;
  }

  no_of_bins=
    m_ScalableColorExtraction->
    GetDescriptorInterface()->
    GetNumberOfCoefficients();

  if (!m_HistogramList) {
    
    m_HistogramList=(TGOFHISTLIST*)malloc(sizeof(TGOFHISTLIST));
    if (!m_HistogramList) {
      fprintf(stderr,"Error: Out of memory\n");
      return (unsigned long) -1;
    }
    /* create new copy of histogram and add histogram to linked list*/
    histogram=(unsigned long*)malloc(no_of_bins*sizeof(unsigned long));
    if (!histogram) {
      fprintf(stderr,"Error: Out of memory\n");
      return (unsigned long) -1;
    }
    memcpy(histogram,SC_histogram,no_of_bins*sizeof(unsigned long));
    m_HistogramList->histogram=histogram;
    m_HistogramList->next=0;
    m_NoOfHistograms=1;
  }
  else {
    unsigned long i;
    switch (m_Aggregation) {
    case 0://average
      histogram=m_HistogramList->histogram;
      for (i=0; i<no_of_bins; i++) {
	*histogram+=*SC_histogram;
	histogram++;
	SC_histogram++;
      }
      break;
    case 1://median
      {
	/* create new histogram copy and put it as first element in the list*/
	TGOFHISTLIST *newhist;

	newhist=(TGOFHISTLIST*)malloc(sizeof(TGOFHISTLIST));
	if (!newhist) {
	  fprintf(stderr,"Error: Out of memory\n");
	  return (unsigned long) -1;
	}
	/* create new copy of histogram and add histogram to linked list*/
	histogram=(unsigned long*)malloc(no_of_bins*sizeof(unsigned long));
	if (!histogram) {
	  fprintf(stderr,"Error: Out of memory\n");
	  return (unsigned long) -1;
	}
	memcpy(histogram,SC_histogram,no_of_bins*sizeof(unsigned long));
	//modified according to m15122
	//newhist->histogram=histogram;
	//newhist->next=m_HistogramList;
	//newhist=m_HistogramList;
	TGOFHISTLIST *nexthist, *currenthist;
	nexthist=m_HistogramList;
	do
	{
		currenthist=nexthist;
		nexthist=currenthist->next;
	} while (nexthist);
	currenthist->next=newhist;
	newhist->histogram=histogram;
	newhist->next=0;
      }
      break;
    case 2://intersection
      histogram=m_HistogramList->histogram;
      for (i=0; i<no_of_bins; i++) {
	if (*histogram>*SC_histogram) *histogram=*SC_histogram;
	histogram++;
	SC_histogram++;
      }
      break;
    default:
      fprintf(stderr,"Warning: Unkown Aggregation type\n");
    }
    m_NoOfHistograms++;
  }
  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long GoFGoPColorExtractionTool::PostExtracting(void)

{
  unsigned long no_of_bins,i;
  unsigned long *onebin=0;
  unsigned long *histogram=0;

  no_of_bins=
    m_ScalableColorExtraction->
    GetDescriptorInterface()->
    GetNumberOfCoefficients();

  if (m_Aggregation==1) {
    onebin=(unsigned long*)malloc(m_NoOfHistograms*sizeof(unsigned long));
    if (!onebin) {
      fprintf(stderr,"Error: Out of memory\n");
      return (unsigned long) -1;
    }
  }
    
  histogram=m_HistogramList->histogram;

  switch (m_Aggregation) {
  case 0://average
    for (i=0; i<no_of_bins; i++) {
      *histogram=(unsigned long)
	(((double)*histogram/(double)m_NoOfHistograms)+0.5);
      histogram++;
    }
    break;
  case 1://median
    for (i=0; i<no_of_bins; i++) {
      int j;
      TGOFHISTLIST *currhist;

      currhist=m_HistogramList;
      for (j=0; j<m_NoOfHistograms; j++) {
#ifdef DEBUG
	if (!currhist) {
	  fprintf(stderr,"Error: internal Error in GoFGoPColor\n");
	  exit (1);
	}
#endif
	onebin[j]=currhist->histogram[i];
	currhist=currhist->next;
      }
      qsort(onebin,m_NoOfHistograms,sizeof(unsigned long),
	    &GOF_ulongcmp);
      //*histogram=onebin[m_NoOfHistograms>>1];
      // modified according to m15122
      // fixed: if m_NoOfHistograms is even: arithmethic mean of the two middle values
      unsigned long half = m_NoOfHistograms>>1;
      unsigned long median;
      if (half*2 == m_NoOfHistograms) 
      {
         unsigned long sum;
         sum = onebin[half-1]+onebin[half];
         median = (unsigned long) double(sum)*0.5;
      }
      else
      {
         median = onebin[half];  // fixed m15122
      }
	  *histogram=median;
      histogram++;
    }
    break;
  case 2://intersection
    break;
  default:
    fprintf(stderr,"Warning: Unkown Aggregation type\n");
  }
  
  m_ScalableColorExtraction->QuantizeHistogram(m_HistogramList->histogram);
  m_ScalableColorExtraction->HaarTransform(0);

  if (onebin) free(onebin);

  /* post extraction of ScalableColor description*/
  //  m_ScalableColorExtraction->PostExtracting();
  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& GoFGoPColorExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
GoFGoPColorExtractionInterfaceABC *GoFGoPColorExtractionTool::GetInterface(void)
{ return &m_Interface; }

