///////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  Perceptual3DShapeCS.cpp
//

#include "Descriptors/Descriptors.h"
#include "CodingSchemes.h"
#include "Encoders/Encoders.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID Perceptual3DShapeCSInterface::myID = UUID("");
const char *Perceptual3DShapeCSInterface::myName =
                         "Perceptual3DShapeCodingSchemeInterface";

// create a UUID for your type
const UUID Perceptual3DShapeCS::myID = UUID("");
const char *Perceptual3DShapeCS::myName = "Perceptual3DShapeCodingScheme";

const UUID Perceptual3DShapeCS::valID = UUID("");
const char *Perceptual3DShapeCS::valName = "";

const UUID Perceptual3DShapeCSInterfaceABC::myID = UUID("");

Perceptual3DShapeCSInterfaceABC::Perceptual3DShapeCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
Perceptual3DShapeCSInterface::Perceptual3DShapeCSInterface(Perceptual3DShapeCS *aPerceptual3DShape):
m_Perceptual3DShapeCS(aPerceptual3DShape)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
Perceptual3DShapeCSInterface::~Perceptual3DShapeCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void Perceptual3DShapeCSInterface::destroy(void)
{ delete m_Perceptual3DShapeCS; }

//----------------------------------------------------------------------------
EncoderFileIO *Perceptual3DShapeCSInterface::GetEncoderStreamBuffer(void)
{
  return m_Perceptual3DShapeCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCSInterface::SetEncoderStreamBuffer(EncoderFileIO *aBuffer)
{
  return m_Perceptual3DShapeCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
DecoderFileIO *Perceptual3DShapeCSInterface::GetDecoderStreamBuffer(void)
{
  return m_Perceptual3DShapeCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCSInterface::SetDecoderStreamBuffer(DecoderFileIO *aBuffer)
{
  return m_Perceptual3DShapeCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC
*Perceptual3DShapeCSInterface::GetDescriptorInterface(void)

{
  return m_Perceptual3DShapeCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCSInterface::
SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *aPerceptual3DShapeDescriptorInterface)
{
  return m_Perceptual3DShapeCS->
    SetDescriptorInterface(aPerceptual3DShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCSInterface::StartEncode(void)
{
  return m_Perceptual3DShapeCS->StartEncode();
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCSInterface::StartDecode(void)
{
  return m_Perceptual3DShapeCS->StartDecode();
}
//=============================================================================
Perceptual3DShapeCS::Perceptual3DShapeCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Perceptual3DShapeCS::Perceptual3DShapeCS(Perceptual3DShapeDescriptorInterfaceABC* aPerceptual3DShape):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aPerceptual3DShape) {
    Perceptual3DShapeDescriptor *descriptor = new Perceptual3DShapeDescriptor();
      aPerceptual3DShape=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aPerceptual3DShape);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Perceptual3DShapeCS::~Perceptual3DShapeCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* Perceptual3DShapeCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool Perceptual3DShapeCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
EncoderFileIO * Perceptual3DShapeCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCS::SetEncoderStreamBuffer(EncoderFileIO *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
DecoderFileIO * Perceptual3DShapeCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCS::SetDecoderStreamBuffer(DecoderFileIO *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC *Perceptual3DShapeCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int Perceptual3DShapeCS::
SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
		       *aPerceptual3DShapeDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aPerceptual3DShapeDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aPerceptual3DShapeDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int Perceptual3DShapeCS::StartEncode(void)
{
  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"Perceptual3DShapeCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"Perceptual3DShapeCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  int i;
  long nNodes = m_DescriptorInterface->GetPerceptual3DShapeNNodes();
  m_EncoderBuffer->PutBitsLong(nNodes,8);
  long nBits = m_DescriptorInterface->GetPerceptual3DBitsPerAttribute();
  m_EncoderBuffer->PutBitsLong(nBits,4);

  long* pAdj = m_DescriptorInterface->GetPerceptual3DShapeIsConnected();
  for(i=0;i<nNodes*(nNodes-1)/2;i++) m_EncoderBuffer->PutBitsLong(pAdj[i],1);

  P3DS_COMPONENTS* pComp = m_DescriptorInterface->GetPerceptual3DShapeNodesAll();
  for(i=0;i<nNodes;i++)
  {
	  m_EncoderBuffer->PutBitsLong(pComp[i].Volume,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Center_X,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Center_Y,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Center_Z,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].PCA_Axis_1X,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].PCA_Axis_1Y,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].PCA_Axis_1Z,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].PCA_Axis_2X,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].PCA_Axis_2Y,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].PCA_Axis_2Z,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Max_1,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Max_2,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Max_3,nBits);
	  m_EncoderBuffer->PutBitsLong(pComp[i].Convexity,nBits);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int Perceptual3DShapeCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"Perceptual3DShapeCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"Perceptual3DShapeCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  int i;
  long nNodes, nBits, tmp;
  m_DecoderBuffer->GetBitsLong(&nNodes,8);m_DecoderBuffer->TakeBits(8);
  m_DescriptorInterface->SetPerceptual3DShapeNNodes(nNodes);

  m_DecoderBuffer->GetBitsLong(&nBits,4);m_DecoderBuffer->TakeBits(4);
  m_DescriptorInterface->SetPerceptual3DBitsPerAttribute(nBits);

  long* pAdj = m_DescriptorInterface->GetPerceptual3DShapeIsConnected();
  for(i=0;i<nNodes*(nNodes-1)/2;i++)
  {m_DecoderBuffer->GetBitsLong(pAdj+i,1);m_DecoderBuffer->TakeBits(1);}

  P3DS_COMPONENTS* pComp = m_DescriptorInterface->GetPerceptual3DShapeNodesAll();
  for(i=0;i<nNodes;i++)
  {
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Volume=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Center_X=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Center_Y=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Center_Z=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].PCA_Axis_1X=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].PCA_Axis_1Y=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].PCA_Axis_1Z=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].PCA_Axis_2X=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].PCA_Axis_2Y=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].PCA_Axis_2Z=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Max_1=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Max_2=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Max_3=tmp;
	m_DecoderBuffer->GetBitsLong(&tmp,nBits);m_DecoderBuffer->TakeBits(nBits);pComp[i].Convexity=tmp;
  }
  return 0;// All done
}

//----------------------------------------------------------------------------
Perceptual3DShapeCSInterfaceABC *Perceptual3DShapeCS::GetInterface(void)
{ return &m_Interface; }



