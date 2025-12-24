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
// Applicable File Name:  SpatioTemporalLocatorExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/Utilities.h"

#include <math.h>


//----------------------------------------------------------------------------

//=============================================================================
	

using namespace XM;


const UUID SpatioTemporalLocatorExtractionInterface::myID = UUID("");
const char *SpatioTemporalLocatorExtractionInterface::myName = "SpatioTemporalLocatorExtractionInterface";

const UUID SpatioTemporalLocatorExtractionTool::myID = UUID("");
const char *SpatioTemporalLocatorExtractionTool::myName = "SpatioTemporalLocatorExtractor";

const UUID SpatioTemporalLocatorExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpatioTemporalLocatorExtractionInterfaceABC::SpatioTemporalLocatorExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpatioTemporalLocatorExtractionInterface::SpatioTemporalLocatorExtractionInterface(SpatioTemporalLocatorExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorExtractionInterface::~SpatioTemporalLocatorExtractionInterface()
{
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorExtractionInterface::
SetSourceMedia(MultiMediaInterfaceABC *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterfaceABC
*SpatioTemporalLocatorExtractionInterface::GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorExtractionInterface::
SetDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *aSpatioTemporalLocatorDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aSpatioTemporalLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorExtractionInterface::PostExtraction(void)

{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpatioTemporalLocatorExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//=============================================================================
SpatioTemporalLocatorExtractionTool::SpatioTemporalLocatorExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	SetExposedInterface(&m_Interface);
	m_counter = 0;

	int i;
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {
		TIEx[i] = new TemporalInterpolationExtractionTool;
	}
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorExtractionTool::
SpatioTemporalLocatorExtractionTool(
SpatioTemporalLocatorDescriptorInterfaceABC *SpatioTemporalLocator):
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	if (!SpatioTemporalLocator) {
	  SpatioTemporalLocatorDescriptor *descriptor =
	    new SpatioTemporalLocatorDescriptor(0,0);
	  SpatioTemporalLocator = descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(SpatioTemporalLocator);

	SetExposedInterface(&m_Interface);
	m_counter = 0;

	int i;
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {
		TIEx[i] = new TemporalInterpolationExtractionTool;
	}
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorExtractionTool::~SpatioTemporalLocatorExtractionTool()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();

	int i;
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {
		TIExI = TIEx[i]->GetInterface();
		TIExI->destroy();
	}
}

//----------------------------------------------------------------------------
bool SpatioTemporalLocatorExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpatioTemporalLocatorExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SpatioTemporalLocatorExtractionTool::
SetSourceMedia(MultiMediaInterfaceABC *media)
{
    if (!media) return -1;

    m_Media=media;

    return 0;
}



//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterfaceABC
*SpatioTemporalLocatorExtractionTool::GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorExtractionTool::
SetDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *aSpatioTemporalLocatorDescriptorInterface)

{
    /* check if new value is different from old value*/
    if (m_DescriptorInterface == 
	    aSpatioTemporalLocatorDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpatioTemporalLocatorDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();

	}
	else {
	  return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorExtractionTool::StartExtracting(void)

{
	int Result, i;

	/* check for descriptor*/
	if (!m_DescriptorInterface) return (unsigned long)-1;

	if (strcmp(m_DescriptorInterface->GetName(),
		   "SpatioTemporalLocatorDescriptorInterface") != 0) {
	  return (unsigned long)-1;
	}

	/* check for media*/
	if (!m_Media) return (unsigned long)-1;

	MomVop *media;
	media = m_Media->GetImage();
	
	if (strcmp(getpara("Trajectory","FigureTrajectory"),
		   "FigureTrajectory")==0) {
		m_FP = 0;
	} else if (strcmp(getpara("Trajectory","FigureTrajectory"),
		   "ParameterTrajectory")==0) {
		m_FP = 1;
	} else {
		m_FP = 2;
	}
	m_sf = atoi(getpara("StartFrame","0"));
	m_ef = atoi(getpara("EndFrame","1000"));

	Pdb.gBuf = media->y_chan->data->u;
	Pdb.VerN = atoi(getpara("VerN","12"));
	Pdb.rawW = media->width;
	Pdb.rawH = media->height;
	Pdb.m_Poly.N_Frame = m_ef - m_sf;
	Pdb.m_Poly.Frame = m_counter;

	if( m_counter < m_sf || m_ef < m_counter ){
		m_counter ++;
		return 0;
	}

	if (m_FP == 0) {
		// FigureTrajectory Extracting
		Result = Pdb.MakeFpoly();
		if ( Result <= 0) return 0;
		Pdb.MakeNPoly(Pdb.VerN);

		int i;
		Pdb.m_Poly.VSize = Pdb.m_tPoly.VSize;
		if (Pdb.m_Poly.VSize > MAXNVertices) { Pdb.m_Poly.VSize = MAXNVertices; } 
		for (i=0; i<Pdb.m_Poly.VSize; i++) {
			Pdb.m_Poly.X[i] = Pdb.m_tPoly.x[i];
			Pdb.m_Poly.Y[i] = Pdb.m_tPoly.y[i];
		}
		if (m_counter != 0) {
			Pdb.LMinShift(Pmedia_back);
		}
		Pmedia = &Pdb.m_Poly;
		Pmedia_back = *Pmedia;
		for (i=0; i<Pmedia->VSize; i++) {
			Pmedia_back.X[i] = Pmedia->X[i];
			Pmedia_back.Y[i] = Pmedia->Y[i];
		}


		// FigureTrajectory Data Setting
		m_DescriptorInterface->SetCoordFlag(0);
		m_DescriptorInterface->SetNumberOfRefernceRegions(1);

		m_DescriptorInterface->SetTypeOfTrajectory(0, 0);
		                                 // N=0 : FigureTrajectory
		m_DescriptorInterface->SetMediaTimePoint(0, m_sf);
		m_DescriptorInterface->SetMediaDuration(0, m_ef-m_sf);
//		m_DescriptorInterface->SetMediaDuration(0, m_sf);
		m_DescriptorInterface->Settype(0, Pmedia->VSize);

		// TI
		TemporalInterpolationDescriptorInterfaceABC *TIDI;
		for (i=0; i<Pmedia->VSize; i++) {
			TIDI = m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface(0,i);
			TIExI = TIEx[i]->GetInterface();
			TIExI->SetDescriptorInterface(TIDI);

			MultiMedia *Media = new MultiMedia;
			MultiMediaInterfaceABC *MediaI;
			MediaI=Media->GetInterface();

			TnDKeyPoint keypoint;
			keypoint.p = new float [3];
			keypoint.dim = 2;
			keypoint.time = m_counter-m_sf;
			keypoint.p[0]  = Pmedia->X[i];
			keypoint.p[1]  = Pmedia->Y[i];	

			MediaI->AddKeyPointsList(m_counter);
			MediaI->AddKeyPoint(&keypoint);		

			TIExI->SetSourceMedia(MediaI);
			TIExI->StartExtracting();

			delete [] keypoint.p;
			MediaI->destroy();
		}

	} else 	if (m_FP == 1) {
		// PT 
		double Param[6], RegionX[4], RegionY[4];

		m_DescriptorInterface->SetCoordFlag(0);
		m_DescriptorInterface->SetNumberOfRefernceRegions(1);

		m_DescriptorInterface->SetTypeOfTrajectory(0, 1);
                                      		// N=0 : FigureTrajectory
		m_DescriptorInterface->SetMediaTimePoint(0, m_sf);
		m_DescriptorInterface->SetMediaDuration(0, m_ef-m_sf);
//		m_DescriptorInterface->SetMediaDuration(0, m_sf);
#ifdef STL_NEW
		m_DescriptorInterface->SetmotionModel(0, 3);		// AFFINE
#else
		m_DescriptorInterface->SetmotionModel(0, 2);		// AFFINE
#endif
		m_DescriptorInterface->SetellipseFlag(0, 0);	

		if (m_counter == 0) {   // first time

			Result = ExtractMP.Initialize(
				Pdb.gBuf, Pdb.rawW, Pdb.rawH,1, m_counter);
			if ( Result <= 0) return 0;
			ExtractMP.GetInitRegion(RegionX, RegionY);
			// RegionLocator
			RegionLocatorDescriptorInterfaceABC *RLDI;
			RLDI = m_DescriptorInterface->GetRegionLocatorDescriptorInterface(0);
			unsigned long *vertices=
			  (unsigned long *)malloc(6*sizeof(unsigned long));
			vertices[0]=(unsigned long)RegionX[0];
			vertices[1]=(unsigned long)RegionY[0];
			vertices[2]=(unsigned long)RegionX[1];
			vertices[3]=(unsigned long)RegionY[1];
			vertices[4]=(unsigned long)RegionX[2];
			vertices[5]=(unsigned long)RegionY[2];
			RLDI->SetRepresentation(16,16);
			RLDI->SetLocator(0,1,3,(unsigned int *)vertices);
			free(vertices);

		} 
		Result = ExtractMP.ExtractMotionParameters( Param ,	Pdb.gBuf, m_counter);
		// for Interp Func.
		if ( Result <= 0) return 0;

		// TI
		TemporalInterpolationDescriptorInterfaceABC *TIDI;

		TIDI = m_DescriptorInterface->
		  GetTemporalInterpolationDescriptorInterface(0,0);
		TIExI = TIEx[0]->GetInterface();
		TIExI->SetDescriptorInterface(TIDI);

		MultiMedia *Media = new MultiMedia;
		MultiMediaInterfaceABC *MediaI;
		MediaI=Media->GetInterface();

		TnDKeyPoint keypoint;
		keypoint.p = new float [7];
		keypoint.dim = 6;
		keypoint.time = m_counter-m_sf;
		for (i=0; i<6; i++) {  keypoint.p[i]  = Param[i];	}

		MediaI->AddKeyPointsList(m_counter);
		MediaI->AddKeyPoint(&keypoint);		

		TIExI->SetSourceMedia(MediaI);
		TIExI->StartExtracting();

		delete [] keypoint.p;
		MediaI->destroy();

	} else {

		m_DescriptorInterface->SetCoordFlag(0);
		m_DescriptorInterface->SetNumberOfRefernceRegions(1);
		m_DescriptorInterface->SetMediaTimePoint(0, m_sf);
		m_DescriptorInterface->SetMediaDuration(0, m_ef-m_sf);

	}

	// 3D
	double depth = atof(getpara("Depth","-1.0"));
	if (depth==-1.0) {
		m_DescriptorInterface->SetDepthFlag(0,0);
	} else {
		m_DescriptorInterface->SetDepthFlag(0,1);

		TemporalInterpolationDescriptorInterfaceABC *TIDI;
		TIDI = m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface(0,MAXNRefrence*MAXNVertices+1);
		TIExI = TIEx[MAXNRefrence*MAXNVertices+1]->GetInterface();
		TIExI->SetDescriptorInterface(TIDI);

		MultiMedia *Media = new MultiMedia;
		MultiMediaInterfaceABC *MediaI;
		MediaI=Media->GetInterface();

		TnDKeyPoint keypoint;
		keypoint.p = new float [7];
		keypoint.dim = 1;
		keypoint.time = m_counter-m_sf;
		keypoint.p[0]  = depth;

		MediaI->AddKeyPointsList(m_counter);
		MediaI->AddKeyPoint(&keypoint);		

		TIExI->SetSourceMedia(MediaI);
		TIExI->StartExtracting();

		delete [] keypoint.p;
		MediaI->destroy();

	}

	m_counter++;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorExtractionTool::PostExtraction(void)
{

	int i;
	TemporalInterpolationDescriptorInterfaceABC *TIDI;

	if (m_FP == 0) {			// FT
		for( i=0; i<Pmedia->VSize; i++ ) {
			TIDI = m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface(0,i);
			TIExI = TIEx[i]->GetInterface();
			TIExI->SetDescriptorInterface(TIDI);
			TIExI->PostExtraction();
		}
	} else if (m_FP == 1) {			// PT
	  TIDI = m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface(0,0);
	  TIExI = TIEx[0]->GetInterface();
	  TIExI->SetDescriptorInterface(TIDI);
	  TIExI->PostExtraction();
	}

	// 3D
	if (m_DescriptorInterface->GetDepthFlag(0) == 1) {
	  TIDI = m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface(0,MAXNRefrence*MAXNVertices+1);
	  TIExI = TIEx[MAXNRefrence*MAXNVertices+1]->GetInterface();
	  TIExI->SetDescriptorInterface(TIDI);
	  TIExI->PostExtraction();
	}


	return 0;
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpatioTemporalLocatorExtractionInterfaceABC *SpatioTemporalLocatorExtractionTool::GetInterface(void)
{ return &m_Interface; }



