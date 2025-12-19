//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TemporalInterpolationExtraction.cpp
//

#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

#include "Utilities/Utilities.h"

//=============================================================================


using namespace XM;


const UUID TemporalInterpolationExtractionInterface::myID = UUID("");
const char *TemporalInterpolationExtractionInterface::myName = "Temporal Interpolation Extraction Interface";

const UUID TemporalInterpolationExtractionTool::myID = UUID("");
const char *TemporalInterpolationExtractionTool::myName = "Temporal Interpolation Extractor";

const UUID TemporalInterpolationExtractionInterfaceABC::myID = UUID();

//=============================================================================
TemporalInterpolationExtractionInterfaceABC::TemporalInterpolationExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
TemporalInterpolationExtractionInterface::TemporalInterpolationExtractionInterface(TemporalInterpolationExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TemporalInterpolationExtractionInterface::~TemporalInterpolationExtractionInterface()
{
}


//----------------------------------------------------------------------------
int TemporalInterpolationExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC* TemporalInterpolationExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TemporalInterpolationExtractionInterface::
SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
		       *aTemporalInterpolationDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aTemporalInterpolationDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long TemporalInterpolationExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long TemporalInterpolationExtractionInterface::PostExtraction(void)
{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
TemporalInterpolationExtractionTool* TemporalInterpolationExtractionInterface::ExTool()
{
	return m_ExtractionTool;
}

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TemporalInterpolationExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void TemporalInterpolationExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//=============================================================================
TemporalInterpolationExtractionTool::TemporalInterpolationExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	SetExposedInterface(&m_Interface);

	InputPoints.Npoint = 0;
	InputPoints.Dimension = 2;
	InputPoints.Point.resize(1000);

	m_counter = 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationExtractionTool::TemporalInterpolationExtractionTool(
			TemporalInterpolationDescriptorInterfaceABC *TemporalInterpolation):
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{

	if (!TemporalInterpolation) {
	  TemporalInterpolationDescriptor *descriptor =
	    new TemporalInterpolationDescriptor;
	  TemporalInterpolation = descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(TemporalInterpolation);

	SetExposedInterface(&m_Interface);

	InputPoints.Npoint = 0;
	InputPoints.Dimension = 2;
	InputPoints.Point.resize(1000);

	m_counter = 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationExtractionTool::~TemporalInterpolationExtractionTool()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool TemporalInterpolationExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool TemporalInterpolationExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int TemporalInterpolationExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_Media=media;

  InputPoints.Dimension = m_Media->GetKeyPoint()->dim;
  InputPoints.Point[InputPoints.Npoint].PointT = m_Media->GetKeyPoint()->time;
  int i;
  for (i=0; i<InputPoints.Dimension; i++) {
	InputPoints.Point[InputPoints.Npoint].Point[i] = m_Media->GetKeyPoint()->p[i]; 
  }
  InputPoints.Npoint++;

  return 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC* TemporalInterpolationExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int TemporalInterpolationExtractionTool::
SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
		       *aTemporalInterpolationDescriptorInterface)

{
    /* check if new value is different from old value*/
    if (m_DescriptorInterface == 
	    aTemporalInterpolationDescriptorInterface) return 0;

	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
	  aTemporalInterpolationDescriptorInterface;
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
unsigned long TemporalInterpolationExtractionTool::StartExtracting(void)
{
	int i;
	double pData[16];
	char* c;

	// CInterp Init
	if (m_counter == 0) {
		// Read parameter file
		m_pMode = getpara("ExMode","ErrFix");
		for (i=0; i<16; i++) {			// Set Error Limit
			pErr[i] = atof(getpara("ErrLimit","10.0"));	
			
			char str[256];	sprintf(str, "ErrLimit%d", i);
			float el = atof(getpara(str,"-1"));
			if (el>=0) { pErr[i] = el; }

			c = getpara("DefaultFunction","false");	
			if(strcmp(c,"true")==0){
				pDeg[i] = 1;
				pUseDefaultFunction[i] = 1;	
			} else {
				pDeg[i] = atoi(getpara("Degree","2"));
				pUseDefaultFunction[i] = 0;
			}	
		}
		m_Interval = atof(getpara("Interval","10.0"));	
		int  NumOfKeyPoint = atoi(getpara("KeyPointNum","10"))-1;

		// Initialize
		if (strcmp(m_pMode, "ErrFix")==0) {			// Err Fix Mode
			Interp.Initialize(INTERP_SEQUENTIAL, InputPoints.Dimension,
				pDeg,  pErr, 0,0, 1000);

		} else if (strcmp(m_pMode, "KeyPointNumFix") == 0) {	// KeyPointNum Fix Mode
			Interp.Initialize(INTERP_FIXNKEYP, InputPoints.Dimension,
				pDeg,  NULL, NumOfKeyPoint,0, 1000);
			
		} else if (strcmp(m_pMode, "IntervalFix") == 0){							// Interval Fix Mode
			Interp.Initialize(INTERP_FIXLENGTH, InputPoints.Dimension,
				pDeg,  NULL, 0, m_Interval, 1000);
		} else { return 0; }
	}

	if (m_counter >= InputPoints.Npoint) { return 0; }


	for (i=0; i<InputPoints.Dimension; i++) {
		pData[i] = InputPoints.Point[m_counter].Point[i];
	}
	Interp.AddData(InputPoints.Point[m_counter].PointT, pData);

	m_counter++;

	return 0;
}

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TemporalInterpolationExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TemporalInterpolationExtractionInterfaceABC *TemporalInterpolationExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
unsigned long TemporalInterpolationExtractionTool::PostExtraction(void)
{
	int i,j;
	double  ta, tb, fa[16], fb[16], coeff[16];

	Interp.DataEnd();

	if (strcmp(m_pMode, "IntervalFix") == 0) {
		m_DescriptorInterface->SetConstantTimeInterval(0);
		m_DescriptorInterface->SetWholeIntervalDataType(0);
		m_DescriptorInterface->SetMediaDuration(m_Interval);
//		Interp.GetInterpFunc( 0, &ta, &tb, fa, fb, coeff );
	} else if(strcmp(m_pMode, "KeyPointNumFix") == 0){
		m_DescriptorInterface->SetConstantTimeInterval(1);
		Interp.GetInterpFunc( Interp.GetNKeyPoints()-2, &ta, &tb, fa, fb, coeff );
		m_Interval = tb;
		Interp.GetInterpFunc( 0, &ta, &tb, fa, fb, coeff );	
		m_Interval -= ta;
		m_DescriptorInterface->SetMediaDuration(m_Interval);
		Interp.GetInterpFunc( 0, &ta, &tb, fa, fb, coeff );	
	} else {
		m_DescriptorInterface->SetConstantTimeInterval(0);
		m_DescriptorInterface->SetWholeIntervalDataType(0);
		m_DescriptorInterface->SetMediaDuration(0);
	}

	m_DescriptorInterface->SetNumOfKeyPoint(Interp.GetNKeyPoints());				
	m_DescriptorInterface->SetDimension(InputPoints.Dimension);
	m_DescriptorInterface->SetMediaTimePointDataType(0);

	for( j=0; j<Interp.GetNKeyPoints()-1; j++ ){		// j: 0 - KeyPointNum
		Interp.GetInterpFunc( j, &ta, &tb, fa, fb, coeff );
		for (i=0; i<InputPoints.Dimension; i++) {	// i: 0 - Dimension
			m_DescriptorInterface->SetMediaTimePoint(j, ta);  
			m_DescriptorInterface->SetKeyValue(i, j, fa[i]);  

			m_DescriptorInterface->SetMediaTimePoint(Interp.GetNKeyPoints()-1, tb);
			m_DescriptorInterface->SetKeyValue(i, Interp.GetNKeyPoints()-1, fb[i]);

			m_DescriptorInterface->SetDefaultFunction(i, pUseDefaultFunction[i]);
			m_DescriptorInterface->SetType(i, j, pDeg[i]);
			m_DescriptorInterface->SetParam(i, j, coeff[i]);
		}
	}

	Interp.Reset();
	m_counter = 0;
	InputPoints.Npoint = 0;
	InputPoints.Dimension = 2;
	InputPoints.Point.clear();
	InputPoints.Point.resize(1000);

	return 0;
}
