///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim, Du-Sik Park, 
// Yanglim Choi
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
// Applicable File Name:  CTBrowsingCS.cpp
//

#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID CTBrowsingCSInterface::myID = UUID("");
const char *CTBrowsingCSInterface::myName =
                         "CTBrowsingCodingSchemeInterface";

// create a UUID for your type
const UUID CTBrowsingCS::myID = UUID("to be created with xx");
const char *CTBrowsingCS::myName = "CTBrowsingCodingScheme";

const UUID CTBrowsingCS::valID = UUID("");
const char *CTBrowsingCS::valName = "";

const UUID CTBrowsingCSInterfaceABC::myID = UUID("");

CTBrowsingCSInterfaceABC::CTBrowsingCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
CTBrowsingCSInterface::CTBrowsingCSInterface(CTBrowsingCS *aCTBrowsing):
m_CTBrowsingCS(aCTBrowsing)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
CTBrowsingCSInterface::~CTBrowsingCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void CTBrowsingCSInterface::destroy(void)
{ delete m_CTBrowsingCS; }

//----------------------------------------------------------------------------
EncoderFileIO *CTBrowsingCSInterface::GetEncoderStreamBuffer(void)
{
  return m_CTBrowsingCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int CTBrowsingCSInterface::SetEncoderStreamBuffer(EncoderFileIO *aBuffer)
{
  return m_CTBrowsingCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
DecoderFileIO *CTBrowsingCSInterface::GetDecoderStreamBuffer(void)
{
  return m_CTBrowsingCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int CTBrowsingCSInterface::SetDecoderStreamBuffer(DecoderFileIO *aBuffer)
{
  return m_CTBrowsingCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC
*CTBrowsingCSInterface::GetDescriptorInterface(void)

{
  return m_CTBrowsingCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int CTBrowsingCSInterface::
SetDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *aCTBrowsingDescriptorInterface)
{
  return m_CTBrowsingCS->
    SetDescriptorInterface(aCTBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
int CTBrowsingCSInterface::StartEncode(void)
{
  return m_CTBrowsingCS->StartEncode();
}

//----------------------------------------------------------------------------
int CTBrowsingCSInterface::StartDecode(void)
{
  return m_CTBrowsingCS->StartDecode();
}
//=============================================================================
CTBrowsingCS::CTBrowsingCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CTBrowsingCS::CTBrowsingCS(CTBrowsingDescriptorInterfaceABC* aCTBrowsing):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aCTBrowsing) {
    CTBrowsingDescriptor *descriptor =
    new CTBrowsingDescriptor;
    aCTBrowsing=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aCTBrowsing);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CTBrowsingCS::~CTBrowsingCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& CTBrowsingCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* CTBrowsingCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool CTBrowsingCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
EncoderFileIO * CTBrowsingCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int CTBrowsingCS::SetEncoderStreamBuffer(EncoderFileIO *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
DecoderFileIO * CTBrowsingCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int CTBrowsingCS::SetDecoderStreamBuffer(DecoderFileIO *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC *CTBrowsingCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int CTBrowsingCS::
SetDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *aCTBrowsingDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aCTBrowsingDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aCTBrowsingDescriptorInterface;
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
int CTBrowsingCS::StartEncode(void)
{

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"CTBrowsingCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"CTBrowsingCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  long* temp;
  int* pctbc;
  temp=new long[2];
  pctbc=new int[2];
  
  pctbc=m_DescriptorInterface->GetCTBrowsing_Component();

  temp[0] = (long)pctbc[0];
  temp[1] = (long)pctbc[1];

  //the scheme stores the 2 components of the PCTBC(8bits)
  m_EncoderBuffer->PutBitsLong(temp[0],2);
  m_EncoderBuffer->PutBitsLong(temp[1],6);

  delete [] pctbc;
  delete [] temp;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int CTBrowsingCS::StartDecode(void)
{

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"CTBrowsingCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"CTBrowsingCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  int* PCTBC;
  long temp;

  PCTBC = new int[2];

  m_DecoderBuffer->GetBitsLong(&temp,2);
  m_DecoderBuffer->TakeBits(2);
  PCTBC[0]=temp;

  m_DecoderBuffer->GetBitsLong(&temp,6);
  m_DecoderBuffer->TakeBits(6);
  PCTBC[1]=temp;

  // Put the data into the D/DSType
  GetDescriptorInterface()->SetCTBrowsing_Component(PCTBC);

  delete [] PCTBC;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
CTBrowsingCSInterfaceABC *CTBrowsingCS::GetInterface(void)
{ return &m_Interface; }



