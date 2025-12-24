//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Takehiro FUJITA
// Hitachi, Ltd.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TimeSeriesExtraction.cpp
//

#include <stdio.h>
#include <math.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID TimeSeriesExtractionInterface::myID = UUID("");
const char *TimeSeriesExtractionInterface::myName =
"TimeSeriesDescriptorExtractorInterface";

const UUID TimeSeriesExtractionTool::myID = UUID("");
const char *TimeSeriesExtractionTool::myName = "TimeSeriesDescriptorExtractor";

const UUID TimeSeriesExtractionInterfaceABC::myID = UUID();

//=============================================================================
TimeSeriesExtractionInterfaceABC::TimeSeriesExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
TimeSeriesExtractionInterface::
TimeSeriesExtractionInterface(TimeSeriesExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeSeriesExtractionInterface::~TimeSeriesExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TimeSeriesExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void TimeSeriesExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int TimeSeriesExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC* TimeSeriesExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TimeSeriesExtractionInterface::
SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *aTimeSeriesDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aTimeSeriesDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
/*
unsigned long TimeSeriesExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}
*/
//----------------------------------------------------------------------------
unsigned long TimeSeriesExtractionInterface::StartExtracting(MediaDB *ramDB)

{
  return m_ExtractionTool->StartExtracting(ramDB);
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
TimeSeriesExtractionTool::TimeSeriesExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{ 
  m_FrameCnt=0;
  SetExposedInterface(&m_Interface);
}

//=============================================================================
TimeSeriesExtractionTool::
TimeSeriesExtractionTool(TimeSeriesDescriptorInterfaceABC
			*TimeSeries):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{
  // create descriptor if it does not exist*/
  if (!TimeSeries) {
    TimeSeriesDescriptor *descriptor =
      new TimeSeriesDescriptor();
    TimeSeries=descriptor->GetInterface();
  }

  // connect Descritors with Extractions*/
  SetDescriptorInterface(TimeSeries);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeSeriesExtractionTool::~TimeSeriesExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool TimeSeriesExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool TimeSeriesExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int TimeSeriesExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC* TimeSeriesExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int TimeSeriesExtractionTool::
SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *aTimeSeriesDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aTimeSeriesDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aTimeSeriesDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  } else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long TimeSeriesExtractionTool::InitExtracting(void)

{
  /* example for initializing extraction paramters*/
  m_DummyExtractionParameter=atoi(getpara("ParameterNameInFile","1"/*default
								     value*/));
  m_FrameCnt=0;

  return(0UL);
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesExtractionTool::StartExtracting(MediaDB *ramDB)
{
  // content descriptor ID
  short     DescId;
  // TimeSeries Type
  short     SeriesType;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "TimeSeriesDescriptionInterface") != 0) return (unsigned long)-1;

  DescId = m_DescriptorInterface->GetContentDescriptorId();
  SeriesType = m_DescriptorInterface->GetSeriesType();
  switch (DescId) {
  case ID_ColorLayout:			// Color Layout Descriptor
    StartColorLayoutExtracting(ramDB);
    if (SeriesType == TYPE_IRREGULAR_SERIES) {
      StartIrregularSeriesColorLayoutExtracting();
    }
    break;
  // other descriptors will be added.
  default:
	  break;
  }
  // All done
  return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesExtractionTool::StartColorLayoutExtracting(MediaDB *ramDB)
{
	int nEntries=ramDB->GetNoOfElements();
	/* image media*/
	//MomVop *media;
	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;


	/* descriptor */
	ColorLayoutDescriptorInterfaceABC	*theDescriptorInterface;
	ColorLayoutDescriptor				*theDescriptor;
	/* extraction tool*/
	ColorLayoutExtractionTool *theExtractionTool;
	ColorLayoutExtractionInterfaceABC	*theExtractionInterface;

	int		cnt;

	/* get color layout extraction interface*/
	theExtractionTool = new ColorLayoutExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();
	/* get color layout interface*/
	theDescriptorInterface= theExtractionInterface->GetDescriptorInterface();

    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

	ramDB->SetMedia(theMediaInterface);
	theExtractionInterface->SetSourceMedia(theMediaInterface);

	// extract Regular TimeSeries
	for (cnt = 0; cnt < nEntries; cnt++) {
		/* malloc media and load input data*/
		if(!ramDB->DBLoadImage())
			break;

		/* reset layout values*/
		theExtractionInterface->InitExtracting();
		/* extract descriptor form media*/
		theExtractionInterface->StartExtracting();
			/* if you would like to use other combination of NumberOfCoeff,
			execute StartExtracting(NumberOfYCoeff, NumberOfCCoeff); */

        // copy the extracted descriptor
        theDescriptor = new ColorLayoutDescriptor();
        CopyColorLayoutDescriptor(theDescriptor, theDescriptorInterface);

        // contain the extracted descriptor
        m_DescriptorInterface->SetContentDescriptor(cnt, theDescriptor);

    }
    theExtractionInterface->destroy();

	theMediaInterface->destroy();

    // All done
    return 0;
}


//----------------------------------------------------------------------------
unsigned long TimeSeriesExtractionTool::StartIrregularSeriesColorLayoutExtracting(void)
{
	int		theNumOfRegularD;
	int		theNumOfIrregularD;
	ColorLayoutDescriptor	*theDescriptor;			// Regular Series
    ColorLayoutDescriptor	**workDescriptor;
    ColorLayoutDescriptor	*pIrregularD;
    long	interval;
    float	distance;
    float	threshold = (float)(THRESHOLD_COLOR_LAYOUT);
    int		cnt;									// counter
	long		maxInterval = 0;

    theNumOfRegularD = m_DescriptorInterface->GetNumOfContents();
	
    // alloc working (temporally) descriptors
    workDescriptor = new ColorLayoutDescriptor *[theNumOfRegularD];
    for (cnt = 0; cnt < theNumOfRegularD; cnt++) {
        workDescriptor[cnt] = new ColorLayoutDescriptor();
	workDescriptor[cnt]->addref(); //sth please check
	}

    //----- Irregular TimeSeries extraction ---------------------------
    // assign the first element of the Irregular TimeSeries
    theDescriptor = (ColorLayoutDescriptor *)m_DescriptorInterface->GetContentDescriptor(0);
    pIrregularD = workDescriptor[0];
    CopyColorLayoutDescriptor(pIrregularD, theDescriptor);
    theNumOfIrregularD = 1;
    interval = 1L;

    for (cnt = 1; cnt < theNumOfRegularD; cnt++) {
        theDescriptor = (ColorLayoutDescriptor *)m_DescriptorInterface->GetContentDescriptor(cnt);
        // check the distance between the descriptors
        distance = CompareDescriptors(pIrregularD, theDescriptor);
        if (distance < threshold) {		// almost the same descriptors
            interval++;
		} else {					// different descriptors
            // set the extracted interval
			maxInterval = ((maxInterval < interval)? interval : maxInterval);
            m_DescriptorInterface->SetContentInterval(theNumOfIrregularD-1, interval);
            // add a new element
            CopyColorLayoutDescriptor(workDescriptor[theNumOfIrregularD], theDescriptor);
            pIrregularD = workDescriptor[theNumOfIrregularD];
            theNumOfIrregularD++;
            interval = 1L;
		}
	}
    // set the extracted interval (the last one)
    m_DescriptorInterface->SetContentInterval(theNumOfIrregularD-1, interval);

	maxInterval = ((maxInterval < interval)? interval : maxInterval);
    m_DescriptorInterface->SetMaxInterval(maxInterval);

    // copy the Descriptors
    for (cnt = 0; cnt < theNumOfIrregularD; cnt++) {
        theDescriptor = (ColorLayoutDescriptor *)m_DescriptorInterface->GetContentDescriptor(cnt);
        CopyColorLayoutDescriptor(theDescriptor, workDescriptor[cnt]);
		m_DescriptorInterface->SetContentDescriptor(cnt, theDescriptor);
	}
    // delete the rest descriptor elements
    for (cnt = theNumOfIrregularD; cnt < theNumOfRegularD; cnt++) {
        m_DescriptorInterface->DeleteOneContentDescriptor(cnt);
	}

	// update the number of content descriptors
	m_DescriptorInterface->SetNumOfContents(theNumOfIrregularD);

    // free working (temporally) descriptors
    for (cnt = 0; cnt < theNumOfRegularD; cnt++) {
        //sth delete workDescriptor[cnt];
      workDescriptor[cnt]->release();//sth please check
	}
    delete []workDescriptor;

    // All done
    return 0;
}

//----------------------------------------------------------------------------
void TimeSeriesExtractionTool::CopyColorLayoutDescriptor(ColorLayoutDescriptor *theD, ColorLayoutDescriptorInterfaceABC *theDInterface)
{
	int NumberOfYCoeff, NumberOfCCoeff;
	int *y_coeff, *cb_coeff, *cr_coeff;

	NumberOfYCoeff = theDInterface->GetNumberOfYCoeff();
	NumberOfCCoeff = theDInterface->GetNumberOfCCoeff();

	theD->SetNumberOfYCoeff(NumberOfYCoeff);
	theD->SetNumberOfCCoeff(NumberOfCCoeff);

	y_coeff = theDInterface->GetYCoeff();
	cb_coeff = theDInterface->GetCbCoeff();
	cr_coeff = theDInterface->GetCrCoeff();

	theD->SetYCoeff(y_coeff);
	theD->SetCbCoeff(cb_coeff);
	theD->SetCrCoeff(cr_coeff);
}

//----------------------------------------------------------------------------
void TimeSeriesExtractionTool::CopyColorLayoutDescriptor(ColorLayoutDescriptor *dst, ColorLayoutDescriptor *src)
{
	int NumberOfYCoeff, NumberOfCCoeff;
	int *y_coeff, *cb_coeff, *cr_coeff;

	NumberOfYCoeff = src->GetNumberOfYCoeff();
	NumberOfCCoeff = src->GetNumberOfCCoeff();

	dst->SetNumberOfYCoeff(NumberOfYCoeff);
	dst->SetNumberOfCCoeff(NumberOfCCoeff);

	y_coeff = src->GetYCoeff();
	cb_coeff = src->GetCbCoeff();
	cr_coeff = src->GetCrCoeff();

	dst->SetYCoeff(y_coeff);
	dst->SetCbCoeff(cb_coeff);
	dst->SetCrCoeff(cr_coeff);
}

float TimeSeriesExtractionTool::CompareDescriptors(ColorLayoutDescriptor *d1, ColorLayoutDescriptor *d2)
{
	float	distance;
	int		dcy1, dccb1, dccr1;		// DC coefficients of the descriptor 1
	int		dcy2, dccb2, dccr2;		// DC coefficients of the descriptor 2
	int		ddcy, ddccb, ddccr;		// difference between DC coefficients
	int		distY, distCb, distCr;

	// extract DC coefficients
	dcy1 = *(d1->GetYCoeff());
	dccb1 = *(d1->GetCbCoeff());
	dccr1 = *(d1->GetCrCoeff());
	dcy2 = *(d2->GetYCoeff());
	dccb2 = *(d2->GetCbCoeff());
	dccr2 = *(d2->GetCrCoeff());

	//----- calculate the difference -----
	/////////////////////////// Y
	ddcy = dcy1 - dcy2;
	distY = ddcy * ddcy;
	/////////////////////////// Cb
	ddccb = dccb1 - dccb2;
	distCb = ddccb * ddccb;
	/////////////////////////// Cr
	ddccr = dccr1 - dccr2;
	distCr = ddccr * ddccr;

	distance = (float)(sqrt((double)(distY + distCb + distCr)));
	return (distance);
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long TimeSeriesExtractionTool::PostExtracting(void)

{
  // In this specific case you could als write the number of frames here
  //m_DescriptorInterface->SetDummyContents(m_FrameCnt*
  //					  m_DummyExtractionParameter);

  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& TimeSeriesExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TimeSeriesExtractionInterfaceABC *TimeSeriesExtractionTool::GetInterface(void)
{ return &m_Interface; }

