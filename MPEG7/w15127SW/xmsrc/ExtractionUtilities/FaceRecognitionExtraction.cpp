//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Applicable File Name:  FaceRecognitionExtraction.cpp
//

#include <stdio.h>
#include <math.h>
//#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

//=============================================================================

using namespace XM;


const UUID FaceRecognitionExtractionInterface::myID = UUID("");
const char *FaceRecognitionExtractionInterface::myName = "FaceRecognition Extraction Interface";

const UUID FaceRecognitionExtractionTool::myID = UUID("");
const char *FaceRecognitionExtractionTool::myName = "FaceRecognition Descriptor Extractor";

const UUID FaceRecognitionExtractionInterfaceABC::myID = UUID();

// Hard coded FaceRecognition eigenface and meanface parameters
#include "EigenFaceData.h"
//char FaceRecognitionExtractionTool::EigenfaceDataTable[2576*48] = {...};

//=============================================================================
FaceRecognitionExtractionInterfaceABC::FaceRecognitionExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
FaceRecognitionExtractionInterface::FaceRecognitionExtractionInterface(FaceRecognitionExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
FaceRecognitionExtractionInterface::~FaceRecognitionExtractionInterface()
{
}

//----------------------------------------------------------------------------
void FaceRecognitionExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int FaceRecognitionExtractionInterface::SetSourceMedia(MomVop *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterfaceABC* FaceRecognitionExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int FaceRecognitionExtractionInterface::
SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
		       *aFaceRecognitionDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
FaceRecognitionExtractionTool::FaceRecognitionExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	SetExposedInterface(&m_Interface);
}
using namespace XM;
//----------------------------------------------------------------------------
FaceRecognitionExtractionTool::
FaceRecognitionExtractionTool(FaceRecognitionDescriptorInterfaceABC
			      *aDescriptorInterface):
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif
	int i,j,k;

// Modified for hard code parameter producing

//	FILE *fp1, *fp2;
//	int featuresize, originalsize;
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
//	featuresize=48;
//	featuresize=49;
//	originalsize=2576;

//	if ((fp1=fopen("eigface48new.dat","r+b"))==NULL)
//	{
//		printf("eigface48new.dat file open error\n");
//		exit(1);
//	}

//	if ((fp2=fopen("meanface.dat","r+b"))==NULL)
//	{
//		printf("meanface.dat file open error\n");
//		exit(1);
//	}

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
//	unsigned char eigenfacecbc[2576*48];
//	unsigned char eigenfacecbc[2576*49];
//	unsigned char meanfacecbc[2576];

//	fread(eigenfacecbc,originalsize*featuresize,sizeof(unsigned char),fp1);
//	fread(meanfacecbc,originalsize,sizeof(unsigned char),fp2);
	for (i=0;i<56;i++)
		for (j=0;j<46;j++)
			meanface[i*46+j]=MeanfaceDataTable[i+j*56];

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	for (k=0;k<48;k++)
//	for (k=0;k<49;k++)
		for (i=0;i<56;i++)
			for (j=0;j<46;j++)
				eigenface[i*46+j+k*2576]=EigenfaceDataTable[i+j*56+k*2576];
//	fclose(fp1);
//	fclose(fp2);


	/* create descriptor if it does not exist*/
	if (!aDescriptorInterface) {
	  FaceRecognitionDescriptor *descriptor =
	    new FaceRecognitionDescriptor();
	  aDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(aDescriptorInterface);

	/* create sub extraction tools and
	   connect them with descriptor if availabe*/
	/* no sub extraction tools (see how FaceRecognitionCS if needed)*/

	SetExposedInterface(&m_Interface);

	
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	for (i=0; i<48; i++)
//	for (i=0; i<49; i++)
		qstep[i]=8;
	/*
	for (i=0;i<5;i++)
		qstep[i]=4;
	for (i=5;i<17;i++)
		qstep[i]=4;
	for (i=17;i<25;i++)
		qstep[i]=8;
	for (i=25;i<49;i++)
		qstep[i]=16;
	*/
}

//----------------------------------------------------------------------------
FaceRecognitionExtractionTool::~FaceRecognitionExtractionTool()
{
}

//----------------------------------------------------------------------------
bool FaceRecognitionExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool FaceRecognitionExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int FaceRecognitionExtractionTool::SetSourceMedia(MomVop *media)
{
  if (!media) return -1;

  m_Media=media;
  return 0;
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterfaceABC*
FaceRecognitionExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}


//----------------------------------------------------------------------------
int FaceRecognitionExtractionTool::
SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
		       *aFaceRecognitionDescriptorInterface)

{
  if (!aFaceRecognitionDescriptorInterface) return -1;

  m_DescriptorInterface=aFaceRecognitionDescriptorInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionExtractionTool::InitExtracting(void)
{
	return 0UL;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionExtractionTool::StartExtracting(void)

{
        /* check for descriptor*/
        if (!m_DescriptorInterface) return (unsigned long)-1;

	if (strcmp(m_DescriptorInterface->GetName(),
		   "FaceRecognition Descriptor Interface") != 0) {
	  return (unsigned long)-1;
	}

	/* check for media*/
	if (!m_Media) return (unsigned long)-1;

	FRD		*pFace;
	pFace = NULL;
	pFace = new	FRD;
	
	FaceRecognitionFeatureGeneration(m_Media->y_chan->data->u,m_Media->width,m_Media->height,pFace);	

	m_DescriptorInterface->SetFaceRecognition( pFace );
	
	delete	pFace;
	return 0;
}


//----------------------------------------------------------------------------
bool FaceRecognitionExtractionTool::
FaceRecognitionFeatureGeneration(unsigned char* pImage_Y, unsigned long image_width,
			unsigned long image_height, FRD* pFace)
{
	int i,j;
	double tmpfeature;
	unsigned char *Face;
	double step1st;
//	double step2nd;
	int featuresize, facesize;

	//unsigned char eigenface[2576];
	//unsigned char meanface[2576*49];

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	featuresize=48;
//	featuresize=49;
	facesize=2576;
	//Mark Pickering's proposal
	step1st = 1.6384e+004;
//  step2nd = 1.6695e+004;
	/*
	step1st=1.4398e+004;
	step2nd=1.6695e+004;
	*/   
	if ((image_width!=46)||(image_height!=56))
		return false;
	else
	{
		Face=pImage_Y;
		for (i=0;i<48;i++)//computing the eigenfeatures
//		for (i=0;i<49;i++)//computing the eigenfeatures
		{
			tmpfeature=0;
			for (j=0;j<2576;j++)
				tmpfeature+=(Face[j]-meanface[j])*eigenface[i*facesize+j];
			pFace->eigenfeature[i]=round(tmpfeature/step1st/qstep[i])+128/qstep[i];
		}
		
		for (i=0;i<featuresize;i++)
		{
			if (pFace->eigenfeature[i]<0)
				pFace->eigenfeature[i]=0;
			if (pFace->eigenfeature[i]>=256/qstep[i])
				pFace->eigenfeature[i]=256/qstep[i]-1;
		}

		return true;
	}
}

//-----------------------------------------------------------------------------------

//----------------------------------------------------------------------------
const UUID& FaceRecognitionExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
FaceRecognitionExtractionInterfaceABC *FaceRecognitionExtractionTool::GetInterface(void)
{ return &m_Interface; }


int FaceRecognitionExtractionTool::round(double x)
{
	if (x>0)
		return (int)(x+0.5);
	else if (x<0)
		return (int)(x-0.5);
	else
		return 0;
}
