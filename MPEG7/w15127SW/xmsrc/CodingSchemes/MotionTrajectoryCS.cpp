///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Philips Research USA, Sylvie Jeannin
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MotionTrajectoryCS.cpp
//

// Remarks: this module is not in its final state, as it needs to make use of:
//	- global motion interfaces & coding/decoding routines
//	- time interfaces & coding/decoding routines
//	- coordinates interfaces & coding/decoding routines
// to be added where it is mentioned when available
// In the current status:
// Temporary solutions are provided for coding/decoding times and coordinates.
// Global motion is never coded/decoded.


#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif


using namespace XM;

const UUID MotionTrajectoryCSInterface::myID = UUID("");
const char *MotionTrajectoryCSInterface::myName =
                         "MotionTrajectoryCodingEngineInterface";

const UUID MotionTrajectoryCS::myID = UUID("");
const char *MotionTrajectoryCS::myName = "MotionTrajectoryCodingScheme";

const UUID MotionTrajectoryCS::valID = UUID("");
const char *MotionTrajectoryCS::valName = "";

const UUID MotionTrajectoryCSInterfaceABC::myID = UUID("");

MotionTrajectoryCSInterfaceABC::MotionTrajectoryCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
MotionTrajectoryCSInterface::MotionTrajectoryCSInterface(MotionTrajectoryCS *aMotionTrajectory):
m_MotionTrajectoryCS(aMotionTrajectory)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionTrajectoryCSInterface::~MotionTrajectoryCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MotionTrajectoryCSInterface::destroy(void)
{ delete m_MotionTrajectoryCS; }

//----------------------------------------------------------------------------
BitBuffer *MotionTrajectoryCSInterface::GetEncoderStreamBuffer(void)
{
        return m_MotionTrajectoryCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int MotionTrajectoryCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_MotionTrajectoryCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *MotionTrajectoryCSInterface::GetDecoderStreamBuffer(void)
{
        return m_MotionTrajectoryCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int MotionTrajectoryCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_MotionTrajectoryCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC
*MotionTrajectoryCSInterface::GetDescriptorInterface(void)
{
        return m_MotionTrajectoryCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int MotionTrajectoryCSInterface::
SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *aMotionTrajectoryDescriptorInterface)
{
        return m_MotionTrajectoryCS->
	  SetDescriptorInterface(aMotionTrajectoryDescriptorInterface);
}

//----------------------------------------------------------------------------
//COORD SpatialCoordSysCSInterfaceABC *MotionTrajectoryCSInterface::
//COORD GetSpatialCoordSysCSInterface(void)
//COORD {
//COORD         return m_MotionTrajectoryCS->GetSpatialCoordSysCSInterface();
//COORD }

//----------------------------------------------------------------------------
//COORD int MotionTrajectoryCSInterface::
//COORD SetSpatialCoordSysCSInterface(SpatialCoordSysCSInterfaceABC *aSpatialCoordSysCSInterface)
//COORD {
//COORD         return m_MotionTrajectoryCS->
//COORD 	  SetSpatialCoordSysCSInterface(aSpatialCoordSysCSInterface);
//COORD }

//----------------------------------------------------------------------------
TemporalInterpolationCSInterfaceABC *MotionTrajectoryCSInterface::
GetTemporalInterpolationCSInterface(void)
{
        return m_MotionTrajectoryCS->GetTemporalInterpolationCSInterface();
}

//----------------------------------------------------------------------------
int MotionTrajectoryCSInterface::
SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC *aTemporalInterpolationCSInterface)
{
        return m_MotionTrajectoryCS->
	  SetTemporalInterpolationCSInterface(aTemporalInterpolationCSInterface);
}

//----------------------------------------------------------------------------
int MotionTrajectoryCSInterface::StartEncode(void)
{
        return m_MotionTrajectoryCS->StartEncode();
}

//----------------------------------------------------------------------------
int MotionTrajectoryCSInterface::StartDecode(void)
{
        return m_MotionTrajectoryCS->StartDecode();
}

//=============================================================================

MotionTrajectoryCS::MotionTrajectoryCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_MotionTrajectoryDescriptorInterface(0),
//COORD m_SpatialCoordSysCSInterface(0),
m_TemporalInterpolationCSInterface(0)
{
//COORD         SpatialCoordSysCS *m_SpatialCoordSysCS;
        TemporalInterpolationCS *m_TemporalInterpolationCS;

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	/* create sub coding schemes */

//COORD 	m_SpatialCoordSysCS= new SpatialCoordSysCS();
	m_TemporalInterpolationCS= new TemporalInterpolationCS();

	/* connect sub coding schemes with motion trajectory coding scheme*/
//COORD 	if (m_SpatialCoordSysCS)
//COORD 	  SetSpatialCoordSysCSInterface(m_SpatialCoordSysCS->GetInterface());
	if (m_TemporalInterpolationCS)
	  SetTemporalInterpolationCSInterface(m_TemporalInterpolationCS->GetInterface());

	SetExposedInterface(&m_Interface);
}

//=============================================================================

MotionTrajectoryCS::MotionTrajectoryCS(MotionTrajectoryDescriptorInterfaceABC
				   *motiontrajectory):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_MotionTrajectoryDescriptorInterface(0),
//COORD m_SpatialCoordSysCSInterface(0),
m_TemporalInterpolationCSInterface(0)
{

   //COORD  SpatialCoordSysCS *m_SpatialCoordSysCS;
   TemporalInterpolationCS *m_TemporalInterpolationCS;

#ifdef VERBOSE
   fprintf(stderr,"Connect descriptors\n");
#endif

   /* create descriptor if it does not exist*/
   if (!motiontrajectory) {
   //COORD  MotionTrajectoryDescriptor *descriptor =new MotionTrajectoryDescriptor(0,0);
	MotionTrajectoryDescriptor *descriptor = new MotionTrajectoryDescriptor(0); //COORD: changed to 1 parameter
	motiontrajectory=descriptor->GetInterface();
   }

   /* connect descritors with coding schemes*/
   SetDescriptorInterface(motiontrajectory);

   /* create sub coding schemes and connect them with descriptor if availabe*/
   //COOR  if (motiontrajectory->GetSpatialCoordSysDescriptorInterface()) {
   //COORD   m_SpatialCoordSysCS= new SpatialCoordSysCS(motiontrajectory->
   //COORD 					 GetSpatialCoordSysDescriptorInterface());
   //COORD }
   //COORD else {
   //COORD   m_SpatialCoordSysCS= new SpatialCoordSysCS();
   //COORD }

   if (motiontrajectory->GetTemporalInterpolationDescriptorInterface()) {
	m_TemporalInterpolationCS = new TemporalInterpolationCS(motiontrajectory->GetTemporalInterpolationDescriptorInterface());
   }
   else {
	m_TemporalInterpolationCS= new TemporalInterpolationCS();
   }

   /* connect sub coding schemes with motion trajectory coding scheme*/
   //COORD  if (m_SpatialCoordSysCS)
   //COORD 	 SetSpatialCoordSysCSInterface(m_SpatialCoordSysCS->GetInterface());
   if (m_TemporalInterpolationCS)
	SetTemporalInterpolationCSInterface(m_TemporalInterpolationCS->GetInterface());

   SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionTrajectoryCS::~MotionTrajectoryCS()
{
        /* destroy sub coding schemes*/
//COORD         if (m_SpatialCoordSysCSInterface) m_SpatialCoordSysCSInterface->destroy();
	if (m_TemporalInterpolationCSInterface) m_TemporalInterpolationCSInterface->destroy();

	/* release descriptor*/
	if (m_MotionTrajectoryDescriptorInterface)
	  m_MotionTrajectoryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MotionTrajectoryCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MotionTrajectoryCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer *MotionTrajectoryCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int MotionTrajectoryCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

        /* if sub coding schemes exist apply stream also to them*/
//COORD         if (m_SpatialCoordSysCSInterface) m_SpatialCoordSysCSInterface->
//COORD 				      SetEncoderStreamBuffer(aBuffer);
        if (m_TemporalInterpolationCSInterface) m_TemporalInterpolationCSInterface->
				      SetEncoderStreamBuffer(aBuffer);

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer *MotionTrajectoryCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int MotionTrajectoryCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	/* connect stream buffer also with sub coding schemes*/
//COORD 	if (m_SpatialCoordSysCSInterface) m_SpatialCoordSysCSInterface->
//COORD 				      SetDecoderStreamBuffer(aBuffer);
	if (m_TemporalInterpolationCSInterface) m_TemporalInterpolationCSInterface->
				       SetDecoderStreamBuffer(aBuffer);

	return 0;
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC *MotionTrajectoryCS::
GetDescriptorInterface(void)
{
       return m_MotionTrajectoryDescriptorInterface;
}

//----------------------------------------------------------------------------
int MotionTrajectoryCS::
SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *aMotionTrajectoryDescriptorInterface)
{
  if (!aMotionTrajectoryDescriptorInterface)
     return -1;

  /* check if new value is different from old value */
  if (m_MotionTrajectoryDescriptorInterface == aMotionTrajectoryDescriptorInterface)
     return 0;

  /* release old interface if exists */
  if (m_MotionTrajectoryDescriptorInterface)
     m_MotionTrajectoryDescriptorInterface->release();

  /* add new interface */
  /*********************/

  m_MotionTrajectoryDescriptorInterface = aMotionTrajectoryDescriptorInterface;

  if (m_MotionTrajectoryDescriptorInterface) {
     m_MotionTrajectoryDescriptorInterface->addref();
     /* connect sub descriptors with sub coding schemes*/
     //COORD  if ((m_MotionTrajectoryDescriptorInterface-GetSpatialCoordSysDescriptorInterface())
     //COORD     &&   m_SpatialCoordSysCSInterface) {
     //COORD     m_SpatialCoordSysCSInterface->SetDescriptorInterface(
     //COORD        m_MotionTrajectoryDescriptorInterface->GetSpatialCoordSysDescriptorInterface());
     //COORD  }
     if ((m_MotionTrajectoryDescriptorInterface->GetTemporalInterpolationDescriptorInterface())
       && m_TemporalInterpolationCSInterface) {
            m_TemporalInterpolationCSInterface->SetDescriptorInterface(
                m_MotionTrajectoryDescriptorInterface->GetTemporalInterpolationDescriptorInterface());
     }
  }
  else {
     /* if no descriptor, release also descriptor interfaces from sub coding schemes*/
     //COORD   m_SpatialCoordSysCSInterface->SetDescriptorInterface(0);
     m_TemporalInterpolationCSInterface->SetDescriptorInterface(0);
     return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
//COORD SpatialCoordSysCSInterfaceABC *MotionTrajectoryCS::GetSpatialCoordSysCSInterface(void)
//COORD
//COORD {
//COORD         return m_SpatialCoordSysCSInterface;
//COORD }

//----------------------------------------------------------------------------
//COORD int MotionTrajectoryCS::SetSpatialCoordSysCSInterface(SpatialCoordSysCSInterfaceABC
//COORD 						*aSpatialCoordSysCSInterface)
//COORD
//COORD {
//COORD         if (!aSpatialCoordSysCSInterface) return -1;
//COORD
//COORD         m_SpatialCoordSysCSInterface = aSpatialCoordSysCSInterface;
//COORD 	return 0;
//COORD }

//----------------------------------------------------------------------------
TemporalInterpolationCSInterfaceABC *MotionTrajectoryCS::GetTemporalInterpolationCSInterface(void)

{
        return m_TemporalInterpolationCSInterface;
}

//----------------------------------------------------------------------------
int MotionTrajectoryCS::SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC
						*aTemporalInterpolationCSInterface)

{
        if (!aTemporalInterpolationCSInterface) return -1;

        m_TemporalInterpolationCSInterface = aTemporalInterpolationCSInterface;
	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryCS::StartEncode(void)
{
  long d;

  if (!m_EncoderBuffer) {
     return -1;
  }

  /* check if descriptor and stream buffer are linked to coding scheme*/
  if (!m_EncoderBuffer || !m_MotionTrajectoryDescriptorInterface) {
     return -1;
  }

  /* check values for SpatialCoordSys and if it is linked to traj */
  //COORD  if (!m_MotionTrajectoryDescriptorInterface->
  //COORD    GetSpatialCoordSysDescriptorInterface()) return -1;
  //COORD  /* check if SpatialCoordSysD and streambuffer are linked to SpatialCoordSys CS */
  //COORD  if (!m_SpatialCoordSysCSInterface->GetDescriptorInterface() ||
  //COORD      !m_SpatialCoordSysCSInterface->GetEncoderStreamBuffer()) return -1;
  //COORD  /* check if SpatialCoordSysD attached to traj = SpatialCoordSysD attached to SpatialCoordSys CS */
  //COORD  if (m_SpatialCoordSysCSInterface->GetDescriptorInterface() !=
  //COORD      m_MotionTrajectoryDescriptorInterface->
  //COORD  GetSpatialCoordSysDescriptorInterface()) return -1;

  /* check values for TemporalInterpolation and if it is linked to traj */
  if (!m_MotionTrajectoryDescriptorInterface->GetTemporalInterpolationDescriptorInterface())
     return -1;
  /* check if TemporalInterpolationD and streambuffer are linked to TemporalInterpolationCS */
  if (!m_TemporalInterpolationCSInterface->GetEncoderStreamBuffer())
     return -1;
  if (!m_TemporalInterpolationCSInterface->GetDescriptorInterface())
     return -1;
  /* check if TemporalInterpolationD attached to traj = TemporalInterpolationD attached to TemporalInterpolationCS */
  if (m_TemporalInterpolationCSInterface->GetDescriptorInterface() !=
      m_MotionTrajectoryDescriptorInterface->GetTemporalInterpolationDescriptorInterface())
     return -1;

  /* encode motion trajectory */
  /****************************/

  /* put cameraFollows  (2 bits) */
  d=m_MotionTrajectoryDescriptorInterface->GetCameraFollows();
  m_EncoderBuffer->PutBitsLong(d,2);

  /* put coordFlag  (1 bit) */
  d=m_MotionTrajectoryDescriptorInterface->GetCoordFlag();
  m_EncoderBuffer->PutBitsLong(d,1);

  if (m_MotionTrajectoryDescriptorInterface->GetCoordFlag()==1)  {
      /* put coordinates reference */
      //? should call the coding function for the reference type ?

      /* put spatialReference  (1 bit) */
      d=m_MotionTrajectoryDescriptorInterface->GetSpatialReference();
      m_EncoderBuffer->PutBitsLong(d,1);
  }
  else  {
      /* put Units  (2 bits) */
      d=m_MotionTrajectoryDescriptorInterface->GetUnits();
      m_EncoderBuffer->PutBitsLong(d,2);

      /* put coordCodingLength  (1 bit) */
      d=m_MotionTrajectoryDescriptorInterface->GetCoordCodingLength();
      m_EncoderBuffer->PutBitsLong(d,1);

	  if (m_MotionTrajectoryDescriptorInterface->GetCoordCodingLength()==1)  {
          /* put XRepr  (1 bit) */
          d=m_MotionTrajectoryDescriptorInterface->GetXRepr();
          m_EncoderBuffer->PutBitsLong(d,8);

          /* put YRepr  (1 bit) */
          d=m_MotionTrajectoryDescriptorInterface->GetYRepr();
          m_EncoderBuffer->PutBitsLong(d,8);
	  }
  }

  /* encode temporal interp description*/
  // XRepr, YRepr should be passed as variables to this function, with quantization range: 
  // to add when TemporalInterpolationCS updated accordingly
  m_TemporalInterpolationCSInterface->StartEncode();

  return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryCS::StartDecode(void)
{

  long d;

  if (!m_DecoderBuffer) {
     return -1;
  }

  /* check if descriptor and stream buffer are linked to coding scheme*/
  if (!m_DecoderBuffer || !m_MotionTrajectoryDescriptorInterface) {
     return -1;
  }

  /* check values for SpatialCoordSys and if it is linked to traj */
  //COORD  if (!m_MotionTrajectoryDescriptorInterface->
  //COORD     GetSpatialCoordSysDescriptorInterface()) return -1;
  //COORD  /* check if SpatialCoordSysD and streambuffer are linked to SpatialCoordSysCS */
  //COORD  if (!m_SpatialCoordSysCSInterface->GetDescriptorInterface() ||
  //COORD     !m_SpatialCoordSysCSInterface->GetDecoderStreamBuffer()) return -1;
  //COORD  /* check if SpatialCoordSysD attached to traj = SpatialCoordSysD attached to SpatialCoordSysCS */
  //COORD  if (m_SpatialCoordSysCSInterface->GetDescriptorInterface() !=
  //COORD     m_MotionTrajectoryDescriptorInterface->
  //COORD     GetSpatialCoordSysDescriptorInterface()) return -1;

  /* check values for TemporalInterpolation and if it is linked to traj */
  if (!m_MotionTrajectoryDescriptorInterface->GetTemporalInterpolationDescriptorInterface())
     return -1;
  /* check if TemporalInterpolationD and streambuffer are linked to TemporalInterpolationCS */
  if (!m_TemporalInterpolationCSInterface->GetDescriptorInterface() ||
      !m_TemporalInterpolationCSInterface->GetDecoderStreamBuffer())
     return -1;
  /* check if TemporalInterpolationD attached to traj = TemporalInterpolationD attached to TemporalInterpolationCS */
  if (m_TemporalInterpolationCSInterface->GetDescriptorInterface() !=
      m_MotionTrajectoryDescriptorInterface->GetTemporalInterpolationDescriptorInterface())
     return -1;

  /* decode motion trajectory */
  /****************************/

  /* get Camera_follows_object flag (2 bit) */
  m_DecoderBuffer->GetBitsLong(&d,2);
  m_DecoderBuffer->TakeBits(2);
  m_MotionTrajectoryDescriptorInterface->SetCameraFollows(d);

  /* get coordFlag  (1 bit) */
  m_DecoderBuffer->GetBitsLong(&d,1);
  m_DecoderBuffer->TakeBits(1);
  m_MotionTrajectoryDescriptorInterface->SetCoordFlag(d);

  if (m_MotionTrajectoryDescriptorInterface->GetCoordFlag()==1)  {
      /* get coordinates reference */
      //?call decoder of reference type?
	  /*access and decode coordinates*/
	  /*m_Spatial2DCoordinatesCSInterface->StartDecode();*/

      /* get spatialReference  (1 bit) */
      m_DecoderBuffer->GetBitsLong(&d,1);
      m_DecoderBuffer->TakeBits(1);
      m_MotionTrajectoryDescriptorInterface->SetSpatialReference(d);
  }
  else  {
      /* get Units  (2 bits) */
      m_DecoderBuffer->GetBitsLong(&d,2);
      m_DecoderBuffer->TakeBits(2);
      m_MotionTrajectoryDescriptorInterface->SetUnits(d);

      /* get coordCodingLength  (1 bit) */
      m_DecoderBuffer->GetBitsLong(&d,1);
      m_DecoderBuffer->TakeBits(1);
      m_MotionTrajectoryDescriptorInterface->SetCoordCodingLength(d);

	  if (m_MotionTrajectoryDescriptorInterface->GetCoordCodingLength()==1)  {
          /* get XRepr  (1 bit) */
          m_DecoderBuffer->GetBitsLong(&d,1);
          m_DecoderBuffer->TakeBits(1);
          m_MotionTrajectoryDescriptorInterface->SetXRepr(d);

          /* get YRepr  (1 bit) */
          m_DecoderBuffer->GetBitsLong(&d,1);
          m_DecoderBuffer->TakeBits(1);
          m_MotionTrajectoryDescriptorInterface->SetYRepr(d);
	  }
  }

  /* decode temporal positions description*/
  // XRepr, YRepr should be passed as variables to this function, with quantization range: 
  // to add when TemporalInterpolationCS updated accordingly
  m_TemporalInterpolationCSInterface->StartDecode();

  return 0;
}

//----------------------------------------------------------------------------
MotionTrajectoryCSInterfaceABC *MotionTrajectoryCS::GetInterface(void)
{
return &m_Interface;
}



