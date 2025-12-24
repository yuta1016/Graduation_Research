//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
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
// Applicable File Name:  Perceptual3DShapeExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"

//typedef int BOOL;
#define TRUE 1
#define FALSE 0

#pragma warning(disable : 4786)

#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities.h"
#include "Utilities/readparam.h"

#include "math.h"

//=============================================================================
	

using namespace XM;


const UUID Perceptual3DShapeExtractionInterface::myID = UUID("");
const char *Perceptual3DShapeExtractionInterface::myName = "Perceptual3DShapeExtractorInterface";

const UUID Perceptual3DShapeExtractionTool::myID = UUID("");
const char *Perceptual3DShapeExtractionTool::myName = "Perceptual3DShapeDescriptorExtractor";

const UUID Perceptual3DShapeExtractionInterfaceABC::myID = UUID();

//=============================================================================
Perceptual3DShapeExtractionInterfaceABC::Perceptual3DShapeExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
Perceptual3DShapeExtractionInterface::
Perceptual3DShapeExtractionInterface(Perceptual3DShapeExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
Perceptual3DShapeExtractionInterface::~Perceptual3DShapeExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void Perceptual3DShapeExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int Perceptual3DShapeExtractionInterface::SetSourceMedia(MediaThreeD *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC* Perceptual3DShapeExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int Perceptual3DShapeExtractionInterface::
SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *aPerceptual3DShapeDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aPerceptual3DShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
Perceptual3DShapeExtractionTool::Perceptual3DShapeExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media3D(0),
m_FrameCnt(0)
{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
Perceptual3DShapeExtractionTool::
Perceptual3DShapeExtractionTool(Perceptual3DShapeDescriptorInterfaceABC
			*aPerceptual3DShape):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media3D(0),
m_FrameCnt(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect Perceptual 3D Shape descriptor\n");
#endif
  /* create descriptor if it does not exist*/
  if (!aPerceptual3DShape) {
    Perceptual3DShapeDescriptor *descriptor = new Perceptual3DShapeDescriptor;
      aPerceptual3DShape = descriptor->GetInterface();
  }

  /* connect descritors with Extraction*/
  SetDescriptorInterface(aPerceptual3DShape);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Perceptual3DShapeExtractionTool::~Perceptual3DShapeExtractionTool()
{
#ifdef VERBOSE
        fprintf(stderr,"Release Perceptual 3D Shape descriptor\n");
#endif
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool Perceptual3DShapeExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool Perceptual3DShapeExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int Perceptual3DShapeExtractionTool::SetSourceMedia(MediaThreeD *media)
{
  if(media==NULL) return -1;
  m_Media3D = media;
  return 0;
}


//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC* Perceptual3DShapeExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int Perceptual3DShapeExtractionTool::
SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *aPerceptual3DShapeDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aPerceptual3DShapeDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aPerceptual3DShapeDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

#include "Descriptors/Perceptual3DShape_Aux.h"
#include <time.h>

//#define FAST_TEST

/*
//this value is [0, 1], convert it to byte (0~255).
inline unsigned long Quantize(float val, int nParaBits)
{if(val>=1) return 255;return (unsigned long)floor(val*256);}
//this value is [-1, 1], convert it to byte (0~255).
inline unsigned long Quantize2(float val, int nParaBits)
{if(val>=1) return 255;return (unsigned long)floor((val+1)*128);}*/

//this value is [0, 1], convert it to byte (0~255).
inline unsigned long Quantize(float val, int nParaBits)
{if(val>=1) return nParaBits-1;return (unsigned long)floor(val*nParaBits);}
//this value is [-1, 1], convert it to byte (0~255).
inline unsigned long Quantize2(float val, int nParaBits)
{if(val>=1) return nParaBits-1;return (unsigned long)floor((val+1)*nParaBits/2);}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long Perceptual3DShapeExtractionTool::InitExtracting(void)
{
  //m_DummyExtractionParameter=atoi(getpara("ParameterNameInFile","1"/*defaultvalue*/));
  m_FrameCnt=0;//TOBEMODIFIED in your Application

//check ShapeSpectrumExtractionTool::StartExtracting() for samples.

//step 0: obtain the additional parameter from parameter files.
  int BitsPerAttribute = (unsigned long )atoi(getpara("bits_per_attribute","8"/*default
								     value*/));
  m_DescriptorInterface->SetPerceptual3DBitsPerAttribute(BitsPerAttribute);

//two things are needed:
/**
(1) perform extraction with m_Media3D.
(2) filling the result into the descriptor through m_DescriptorInterface.
*/

//step 1: perform extraction with m_Media3D. directly copied from main() in Hello.cpp.
	P3DS p3ds; p3ds.InitCGraph();

#ifndef FAST_TEST
	//////////////////////////////////
	// shape decomposition for Query
	CModel * pQueryVRML = new CModel;
	//pQueryVRML->Open("Test/Ant.wrl");	
	pQueryVRML->Open(m_Media3D);

	CSplit * pQuerySplit = new CSplit;
	MakeVoxel(pQueryVRML, pQuerySplit, 10000, FALSE);
	pQuerySplit->Initialize();
	pQuerySplit->PartSplit();
	pQuerySplit->PartMerge();	
	//pQuerySplit->SaveBinary("Test/Ant.bin");
	//pQuerySplit->SaveGray("Test/Ant.dec");
#endif	
	///////////////////////////////
	// graph generation for Query
	P3DS * pQueryP3DS = new P3DS;
#ifdef FAST_TEST
	pQueryP3DS->Load("Test/Ant.gph");
#else
	MakeGraph(pQuerySplit, pQueryP3DS);
//	pQueryP3DS->Save("Test/DragonFly.gph");
#endif

//	pQueryP3DS->Save_quantize("Test/DragonFly_quant.gph", 7);


//step 2: filling the result into the descriptor through m_DescriptorInterface.
	//the total size, also for allocation.
	int i, j, nNodes = pQueryP3DS->pNodeArray.size();
	m_DescriptorInterface->SetPerceptual3DShapeNNodes(nNodes);

	//convert the neighbouring relation to IsConnected array.
	long* pAdj = m_DescriptorInterface->GetPerceptual3DShapeIsConnected();
	int nSizeAdj = nNodes*(nNodes-1)/2;
	for(i=0;i<nSizeAdj;i++) pAdj[i] = 0;
	int nStartPos = 0;
	for(i=0;i<nNodes;i++)//generate pAdj for all nodes.
	{
	  for(j=0;j<pQueryP3DS->pNodeArray[i]->pNode.size();j++)//each neighbouring Node.
	  {
		int nNo = pQueryP3DS->pNodeArray[i]->pNode[j]->idx-1;//in original class the index is beginning with 1.
		int index;
		if(nNo>i) {index=nStartPos+(nNo-i-1);pAdj[index]=1;}
	  }
	  nStartPos += nNodes-1-i;
	}

	// Fixed: int nValueBits = pow(2, BitsPerAttribute);
	int nValueBits = (int)(0.5+pow((double)2, BitsPerAttribute));
	//now for every component.
	P3DS_COMPONENTS* pComps = m_DescriptorInterface->GetPerceptual3DShapeNodesAll();
	for(i=0;i<nNodes;i++)//for each component.
	{
		pComps[i].Volume = Quantize(pQueryP3DS->pNodeArray[i]->vol, nValueBits);
		pComps[i].Center_X = Quantize(pQueryP3DS->pNodeArray[i]->x, nValueBits);
		pComps[i].Center_Y = Quantize(pQueryP3DS->pNodeArray[i]->y, nValueBits);
		pComps[i].Center_Z = Quantize(pQueryP3DS->pNodeArray[i]->z, nValueBits);
		pComps[i].Max_1 = Quantize(pQueryP3DS->pNodeArray[i]->var_x, nValueBits);
		pComps[i].Max_2 = Quantize(pQueryP3DS->pNodeArray[i]->var_y, nValueBits);
		pComps[i].Max_3 = Quantize(pQueryP3DS->pNodeArray[i]->var_z, nValueBits);
		pComps[i].Convexity = Quantize(pQueryP3DS->pNodeArray[i]->convex, nValueBits);
		//the conversion from mat[9] into Transform_*.
		pComps[i].PCA_Axis_1X = Quantize2(pQueryP3DS->pNodeArray[i]->mat[0], nValueBits);
		pComps[i].PCA_Axis_1Y = Quantize2(pQueryP3DS->pNodeArray[i]->mat[3], nValueBits);
		pComps[i].PCA_Axis_1Z = Quantize2(pQueryP3DS->pNodeArray[i]->mat[6], nValueBits);
		pComps[i].PCA_Axis_2X = Quantize2(pQueryP3DS->pNodeArray[i]->mat[1], nValueBits);
		pComps[i].PCA_Axis_2Y = Quantize2(pQueryP3DS->pNodeArray[i]->mat[4], nValueBits);
		pComps[i].PCA_Axis_2Z = Quantize2(pQueryP3DS->pNodeArray[i]->mat[7], nValueBits);
	}

//destroy:
#ifndef FAST_TEST
	if(pQuerySplit!=NULL) delete pQuerySplit;
	if(pQueryVRML!=NULL) delete pQueryVRML; 
#endif
	if(pQueryP3DS!=NULL) delete pQueryP3DS; 

  return(0UL);
}

//in fact this descriptor needs only one extracting function, now I put all codes in InitExtracting.
unsigned long Perceptual3DShapeExtractionTool::StartExtracting(void)
{
  return 0;
}

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
Perceptual3DShapeExtractionInterfaceABC *Perceptual3DShapeExtractionTool::GetInterface(void)
{ return &m_Interface; }

