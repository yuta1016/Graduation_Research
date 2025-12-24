///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips, France
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
// Applicable File Name:  CameraMotionCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifdef UNIX
#include <netinet/in.h>
#endif
 
using namespace XM;

const UUID CameraMotionCSInterface::myID = UUID("");
const char *CameraMotionCSInterface::myName = "Camera Motion Coding Engine Interface";

const UUID CameraMotionCS::myID = UUID("84f6ddc0-a1b5-11d3-82cc-0050041cc4a9");
const char *CameraMotionCS::myName = "Camera Motion Coding Scheme";

const UUID CameraMotionCS::valID = UUID("");
const char *CameraMotionCS::valName = "";

const UUID CameraMotionCSInterfaceABC::myID = UUID("");

static const unsigned int CAMERA_MOTION_MASKS[16] = 
{  TRACK_LEFT_MASK, TRACK_RIGHT_MASK, 
   BOOM_DOWN_MASK, BOOM_UP_MASK, 
   DOLLY_FORWARD_MASK,  DOLLY_BACKWARD_MASK, 
   PAN_LEFT_MASK, 	PAN_RIGHT_MASK, 
   TILT_DOWN_MASK, TILT_UP_MASK, 
   ROLL_CLOCK_MASK,  ROLL_ANTI_MASK, 
   ZOOM_IN_MASK, ZOOM_OUT_MASK, 
   FIXED_MASK, RESERVED_MASK  } ;

CameraMotionCSInterfaceABC::CameraMotionCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
CameraMotionCSInterface::CameraMotionCSInterface(CameraMotionCS
												 *aCameraMotion):
m_CameraMotionCS(aCameraMotion)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
CameraMotionCSInterface::~CameraMotionCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& CameraMotionCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
void CameraMotionCSInterface::destroy(void)
{ delete m_CameraMotionCS; }

//----------------------------------------------------------------------------
const char *CameraMotionCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *CameraMotionCSInterface::GetEncoderStreamBuffer(void)
{
        return m_CameraMotionCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int CameraMotionCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_CameraMotionCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *CameraMotionCSInterface::GetDecoderStreamBuffer(void)
{
        return m_CameraMotionCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int CameraMotionCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_CameraMotionCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
CameraMotionDescriptorInterfaceABC *CameraMotionCSInterface::GetDescriptorInterface(void)
{
        return m_CameraMotionCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int CameraMotionCSInterface::
SetDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface)
{
        return m_CameraMotionCS->SetDescriptorInterface(aCameraMotionDescriptorInterface);
}


//----------------------------------------------------------------------------
int CameraMotionCSInterface::StartEncode(void)
{
        return m_CameraMotionCS->StartEncode();
}
//----------------------------------------------------------------------------
int CameraMotionCSInterface::StartDecode(void)
{
        return m_CameraMotionCS->StartDecode();
}
//=============================================================================

CameraMotionCS::CameraMotionCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_CameraMotionDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CameraMotionCS::CameraMotionCS(CameraMotionDescriptorInterfaceABC *CameraMotion):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_CameraMotionDescriptorInterface(0)
{
	if (!CameraMotion) {
	  CameraMotionDescriptor *descriptor = new CameraMotionDescriptor();
	  CameraMotion = descriptor->GetInterface();
	}
	
	SetDescriptorInterface(CameraMotion);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CameraMotionCS::~CameraMotionCS()
{
	/* release descriptor*/
	if (m_CameraMotionDescriptorInterface)  m_CameraMotionDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& CameraMotionCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& CameraMotionCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* CameraMotionCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool CameraMotionCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * CameraMotionCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int CameraMotionCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
   if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * CameraMotionCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int CameraMotionCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
   if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
CameraMotionDescriptorInterfaceABC *CameraMotionCS::GetDescriptorInterface(void)
{
       return m_CameraMotionDescriptorInterface;
}

//----------------------------------------------------------------------------
int CameraMotionCS::SetDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface)
{
	if (m_CameraMotionDescriptorInterface ==  aCameraMotionDescriptorInterface)
	  return 0;

	if (m_CameraMotionDescriptorInterface)
	  m_CameraMotionDescriptorInterface->release();

	m_CameraMotionDescriptorInterface = aCameraMotionDescriptorInterface;

	if (m_CameraMotionDescriptorInterface)	  m_CameraMotionDescriptorInterface->addref();
	else  return -1;

	return 0;
}

//----------------------------------------------------------------------------
/*CameraMotionCSInterfaceABC *CameraMotionCS::GetCameraMotionCSInterface(void)
{
        return m_CameraMotionCSInterface;
}*/


//----------------------------------------------------------------------------
int CameraMotionCS::StartEncode(void)
{
   long a;
   int cpt, i ;
//   long num_segments ;
   int MTP_vector = 0 ;
   printf(" Start encoding...\n ") ;
	

   printf(" Segments : %ld \n",m_CameraMotionDescriptorInterface->GetNumSegments()) ;
   printf(" Mode : %d \n",m_CameraMotionDescriptorInterface->GetDescriptionMode() ) ;
   
	if (!m_EncoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_CameraMotionDescriptorInterface) {
	  return -1;
	}

	/* Encode camera motion */

	/* put num_segments */
	m_EncoderBuffer->PutBitsLong(m_CameraMotionDescriptorInterface->GetNumSegments(),16);

   /* put description mode */
   if (m_CameraMotionDescriptorInterface->GetDescriptionMode()) a=-1;
	else a=0;
	m_EncoderBuffer->PutBitsLong(a,1UL);
	
   /* put camera motion info */
	for (int seg=0; seg<m_CameraMotionDescriptorInterface->GetNumSegments(); seg++) 
	{
		printf(" Start : %ld \n",m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).startTime ) ;
		printf(" Duration : %ld \n",m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).duration ) ;

		printf(" Presence : ") ;
        for (i=0; i<NB_CAMERA_OPERATIONS; i++)
			if (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[i]>0)
	        printf(" %d : %.2f ",i,m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[i] ) ;
		printf("\n Speeds : ") ;
		for (i=0; i<NB_CAMERA_OPERATIONS-1; i++)
			if (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).speeds[i]>0)
	        printf(" %d : %.2f ",i,m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).speeds[i] ) ;
		
		printf("\n") ;

	  /* put start time */
	  a = m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).startTime ;
	  m_EncoderBuffer->PutBitsLong(a,16);
	  
	  /* put duration */
	  a = m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).duration ;
	  m_EncoderBuffer->PutBitsLong(a,16);
	  
	  if (m_CameraMotionDescriptorInterface->GetDescriptionMode() == MIXTURE_MODE)
	  {
         for (cpt=0; cpt<NB_CAMERA_OPERATIONS; cpt++)
		 {
  	        if  (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[cpt] > 0)
			{
  	           MTP_vector = ( MTP_vector | CAMERA_MOTION_MASKS[cpt] ) ;
			}
		 }

		  /*printf(" MTP_vector = %d - ",MTP_vector) ;
		  for (int b=0; b<15; b++)
		  {
			if  ( ( MTP_vector & ( 0x8000>>b ) ) == 0 )
			   printf("%d",0) ;
			else
			   printf("%d",1) ;
		  }*/

		  m_EncoderBuffer->PutBitsLong( (long) MTP_vector,15) ;

		 if (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).FocusOfExpansionFlag) a=-1;
	        else a=0;
	     m_EncoderBuffer->PutBitsLong(a,1UL);

		 for (cpt=0; cpt<NB_CAMERA_OPERATIONS; cpt++)
		 {
  	        if  (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[cpt] > 0)
			{
				m_EncoderBuffer->PutBitsLong( (long) ( 127 * m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[cpt]) ,7) ;  	           
			}
		 }

		 for (cpt=0; cpt<NB_CAMERA_OPERATIONS-1; cpt++)
		 {
  	        if  (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[cpt] > 0)
			{
				m_EncoderBuffer->PutBitsLong( (long) ( 2047 * m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).speeds[cpt]) ,11) ;  	           
			}
		 }
	  }
	  else // NON_MIXTURE_MODE
	  {
		 long motion_type_id = -1 ;
         for (int cpt=0; cpt<NB_CAMERA_OPERATIONS; cpt++)
		 {
  	        if  (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[cpt] > 0)
			{
  				if (motion_type_id != -1)
					fprintf(stderr," *** WARNING *** Cannot encode mixed motion type in the non-mixture mode !\n") ;
				motion_type_id =cpt ;
			}
		 }
		 m_EncoderBuffer->  PutBitsLong( motion_type_id , 4) ;

		 if (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).FocusOfExpansionFlag) a=-1;
	        else a=0;
	            m_EncoderBuffer->PutBitsLong(a,1UL);

		 if (motion_type_id != 14)
	        m_EncoderBuffer->PutBitsLong( (long) ( 204 * m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[motion_type_id]) ,11) ; 
	  }

	  if (  m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).FocusOfExpansionFlag == true )
	  {
		 m_EncoderBuffer->PutBitsLong( (long) ( 255 * m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).x_FoeFoc) ,8 ) ; 
		 m_EncoderBuffer->PutBitsLong( (long) ( 255 * m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).y_FoeFoc) ,8 ) ; 
		 printf(" FOE/FOC : (%.2f,%.2f)",m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).x_FoeFoc,m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).y_FoeFoc) ;
	  }
	}	

	printf(" Encoding finished.\n") ;
	return 0;
}

//----------------------------------------------------------------------------
int CameraMotionCS::StartDecode(void)
{
   long a;
   long seg; //added Sylvie-Jul27
   int cpt, i ;
   long num_segments ;
   FractionalPresence_t read_presence ;
   AmountOfMotion_t     read_speeds ;
   long start_time ;
   long duration ;
   double read_x_FoeFoc  ;
   double read_y_FoeFoc ;
   long MTP_vector = 0 ;
   bool FOE_flag ;

   printf(" Start decoding...\n") ;
   SegmentedCameraMotion_t camera_segment ;
	
	if (!m_DecoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_CameraMotionDescriptorInterface) {
	  return -1;
	}

	/* Decode camera motion */
	/* Get num_segments */
	m_DecoderBuffer->GetBitsLong(&num_segments,16);
	m_DecoderBuffer->TakeBits(16);
	m_CameraMotionDescriptorInterface->SetNumSegments( num_segments );
	
	/* Get description mode */
	a = 0 ;
	m_DecoderBuffer->GetBitsLong(&a,1UL);
	m_DecoderBuffer->TakeBits(1UL);
	m_CameraMotionDescriptorInterface->SetDescriptionMode( (a!=0) );
	
	/* Get camera motion info */
	for (seg=0; seg<m_CameraMotionDescriptorInterface->GetNumSegments(); seg++) 
	{

		{
	    int j;
	    for (int j=0; j<NB_CAMERA_OPERATIONS-1; j++) {
			read_presence[j] = 0 ;
			read_speeds[j] = 0 ;
		}
		}
	  
	  read_presence[NB_CAMERA_OPERATIONS-1] = 0 ;

	  read_x_FoeFoc = 0 ;
	  read_y_FoeFoc = 0 ;

	  /* get start time*/
	  m_DecoderBuffer->GetBitsLong(&start_time,16);
	  m_DecoderBuffer->TakeBits(16);
	  
	  /* get duration */
	  m_DecoderBuffer->GetBitsLong(&duration,16);
	  m_DecoderBuffer->TakeBits(16);

	  if (m_CameraMotionDescriptorInterface->GetDescriptionMode() == MIXTURE_MODE)
	  {
		m_DecoderBuffer->GetBitsLong(&MTP_vector,15) ;
		m_DecoderBuffer->TakeBits(15);

		m_DecoderBuffer->GetBitsLong(&a,1) ;
		m_DecoderBuffer->TakeBits(1) ;
		FOE_flag =  (a != 0) ;

		  for (cpt=0; cpt<NB_CAMERA_OPERATIONS; cpt++)
		  {
			if  ( ( MTP_vector & ( 0x8000>>cpt ) ) != 0 )
			{
				m_DecoderBuffer->GetBitsLong(&a,7);
				m_DecoderBuffer->TakeBits(7);
				read_presence[cpt] = ((double) a) / 127 ;
			}
		  }

		  for (cpt=0; cpt<NB_CAMERA_OPERATIONS; cpt++)
		  {
			if  ( ( ( MTP_vector & ( 0x8000>>cpt ) ) != 0 ) && ( cpt != NB_CAMERA_OPERATIONS-1 ) )
			{
				m_DecoderBuffer->GetBitsLong(&a,11);
				m_DecoderBuffer->TakeBits(11);
				read_speeds[cpt] = ((double) a) / 2047 ;
			}
		  }
	  }
	  else // NON-MIXTURE MODE
	  {
		  long motion_type_id = -1 ;
		  m_DecoderBuffer->GetBitsLong(&motion_type_id,4);

		  m_DecoderBuffer->GetBitsLong(&a,1UL) ;
		  m_DecoderBuffer->TakeBits(1) ;
		  FOE_flag =  (a != 0)  ;

		  m_DecoderBuffer->GetBitsLong(&a,7) ;
		  m_DecoderBuffer->TakeBits(7) ;
		  read_presence[motion_type_id] = ((double) a) / 127 ;

		  if (motion_type_id != NB_CAMERA_OPERATIONS-1)
		  {
		     m_DecoderBuffer->GetBitsLong(&a,11) ;
		     m_DecoderBuffer->TakeBits(11) ;
		     read_speeds[motion_type_id] = ((double) a) / 2047 ;
		  }

		  if (FOE_flag)
		  {
			 m_DecoderBuffer->GetBitsLong(&a,8) ;
		     m_DecoderBuffer->TakeBits(8) ;
		     read_x_FoeFoc = ((double) a) / 255 ;

			 m_DecoderBuffer->GetBitsLong(&a,8) ;
		     m_DecoderBuffer->TakeBits(8) ;
		     read_y_FoeFoc = ((double) a) / 255 ;
		  }

	  }
	  
	  if (FOE_flag)
		camera_segment = m_CameraMotionDescriptorInterface->CreateCameraMotionInfo(start_time,duration,read_presence,read_speeds,read_x_FoeFoc,read_y_FoeFoc) ;
	  else
		camera_segment = m_CameraMotionDescriptorInterface->CreateCameraMotionInfo(start_time,duration,read_presence,read_speeds) ;

	  m_CameraMotionDescriptorInterface->SetCameraMotionSegmentInfo( camera_segment, seg ) ;
	}

   printf(" Segments : %ld \n",m_CameraMotionDescriptorInterface->GetNumSegments()) ;
   printf(" Mode : %d \n",m_CameraMotionDescriptorInterface->GetDescriptionMode() ) ;

   	for (seg=0; seg<m_CameraMotionDescriptorInterface->GetNumSegments(); seg++) 
	{
		printf(" Start : %ld \n",m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).startTime ) ;
		printf(" Duration : %ld \n",m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).duration ) ;

		printf(" Presence : ") ;
        for (i=0; i<NB_CAMERA_OPERATIONS; i++)
			if (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[i]>0)
	        printf(" %d : %.2f ",i,m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).presence[i] ) ;
		printf("\n Speeds : ") ;
		for (i=0; i<NB_CAMERA_OPERATIONS-1; i++)
			if (m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).speeds[i]>0)
	        printf(" %d : %.2f ",i,m_CameraMotionDescriptorInterface->GetCameraMotionSegmentInfo(seg).speeds[i] ) ;
		
		printf("\n") ;
	}

	return 0;
}

//----------------------------------------------------------------------------
CameraMotionCSInterfaceABC *CameraMotionCS::GetInterface(void)
{ return &m_Interface; }
