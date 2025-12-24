///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips, France
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name: CameraMotion.cpp
//
#include "Descriptors/Descriptors.h"                           /* Naming Convention */
#include "media_time_tool.h"

using namespace XM;

const UUID CameraMotionDescriptorInterface::myID = UUID("30da1680-87ac-11d3-82cc-0050041cc4a9");
const char *CameraMotionDescriptorInterface::myName = "Camera Motion Descriptor Interface";

const UUID CameraMotionDescriptor::myID = UUID("30da1681-87ac-11d3-82cc-0050041cc4a9");
const char *CameraMotionDescriptor::myName = "Camera Motion Descriptor";

const UUID CameraMotionDescriptor::valID = UUID("4e93a560-87ac-11d3-82cc-0050041cc4a9");
const char *CameraMotionDescriptor::valName = "";

const UUID CameraMotionDescriptorInterfaceABC::myID = UUID();


CameraMotionDescriptorInterfaceABC::CameraMotionDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
CameraMotionDescriptorInterface::CameraMotionDescriptorInterface(CameraMotionDescriptor *aCameraMotion):
m_CameraMotionDescriptor(aCameraMotion)
{
	SetInterfaceID(myID);
}
 
//----------------------------------------------------------------------------
CameraMotionDescriptorInterface::~CameraMotionDescriptorInterface()
{
}
//----------------------------------------------------------------------------
const UUID& CameraMotionDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------

void CameraMotionDescriptorInterface::addref(void)
{ m_CameraMotionDescriptor->addref(); }
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CameraMotionDescriptorInterface::release(void)
{ m_CameraMotionDescriptor->release(); }

//----------------------------------------------------------------------------
long CameraMotionDescriptorInterface::GetNumSegments(void)
{
	return m_CameraMotionDescriptor->GetNumSegments();
}
//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptorInterface::SetNumSegments(long num_segments)
{
	return m_CameraMotionDescriptor->SetNumSegments(num_segments);
}

//----------------------------------------------------------------------------
bool CameraMotionDescriptorInterface::GetDescriptionMode(void)
{
	return m_CameraMotionDescriptor->GetDescriptionMode();
}
//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptorInterface::SetDescriptionMode(bool desc_mode)
{
	return m_CameraMotionDescriptor->SetDescriptionMode(desc_mode);
}

 //----------------------------------------------------------------------------
SegmentedCameraMotion_t* CameraMotionDescriptorInterface::GetCameraMotionInfo(void)
{
	return m_CameraMotionDescriptor->GetCameraMotionInfo();
}

//----------------------------------------------------------------------------
SegmentedCameraMotion_t CameraMotionDescriptorInterface::GetCameraMotionSegmentInfo(long segment_index)
{
	return m_CameraMotionDescriptor->GetCameraMotionSegmentInfo(segment_index);
}

//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptorInterface::SetCameraMotionSegmentInfo(SegmentedCameraMotion_t camera_motion_segment, long segment_index)
{
	return m_CameraMotionDescriptor->SetCameraMotionSegmentInfo(camera_motion_segment, segment_index);
}

//----------------------------------------------------------------------------
SegmentedCameraMotion_t CameraMotionDescriptorInterface::CreateCameraMotionInfo(long start_time,
	                                                   long duration, 
	                                                   FractionalPresence_t presence, 
	                                                   AmountOfMotion_t speeds ) 
{	
	return m_CameraMotionDescriptor->CreateCameraMotionInfo(start_time,duration,presence,speeds) ;
}	

//----------------------------------------------------------------------------
SegmentedCameraMotion_t CameraMotionDescriptorInterface::CreateCameraMotionInfo(long start_time,
	                                                   long duration, 
	                                                   FractionalPresence_t presence, 
	                                                   AmountOfMotion_t speeds,	
	                                                   double x_FoeFoc = 0,	
	                                                   double y_FoeFoc = 0) 
{	
	return m_CameraMotionDescriptor->CreateCameraMotionInfo(start_time,duration,presence,speeds,x_FoeFoc,y_FoeFoc) ;
}	            
                                 
//----------------------------------------------------------------------------				//0719-Sylvie
unsigned long CameraMotionDescriptorInterface::ExportDDL(GenericDSInterfaceABC *theParent)	//0719-Sylvie
{																							//0719-Sylvie
  return m_CameraMotionDescriptor->ExportDDL(theParent);									//0719-Sylvie
}																							//0719-Sylvie

//----------------------------------------------------------------------------				//0719-Sylvie
unsigned long CameraMotionDescriptorInterface::ImportDDL(GenericDSInterfaceABC *theParent)	//0719-Sylvie
{																							//0719-Sylvie
  return m_CameraMotionDescriptor->ImportDDL(theParent);									//0719-Sylvie
}																							//0719-Sylvie

//=============================================================================
CameraMotionDescriptor::CameraMotionDescriptor():
m_Interface(this),
m_refcount(0),
m_isProprietary(false),
m_NumSegments(0),
m_DescriptionMode(MIXTURE_MODE),
m_CameraMotionInfo(NULL)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CameraMotionDescriptor::~CameraMotionDescriptor()
{
  delete [] m_CameraMotionInfo;
}

//----------------------------------------------------------------------------
const UUID& CameraMotionDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& CameraMotionDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* CameraMotionDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool CameraMotionDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void CameraMotionDescriptor::addref(void)
{
#ifdef VERBOSE
       fprintf(stderr,"CameraMotion connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void CameraMotionDescriptor::release(void)
{
#ifdef VERBOSE
       fprintf(stderr,"CameraMotion release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
long CameraMotionDescriptor::GetNumSegments(void)
{
	return m_NumSegments;
}
//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptor::SetNumSegments(long num_segments)
{
   m_NumSegments = num_segments;
   
   /* Memory allocation */
   if (m_CameraMotionInfo)
      delete [] m_CameraMotionInfo ;
      
   m_CameraMotionInfo = new SegmentedCameraMotion_t[num_segments] ;
   
	return 0;
}

//----------------------------------------------------------------------------
bool CameraMotionDescriptor::GetDescriptionMode(void)
{
	return m_DescriptionMode;
}
//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptor::SetDescriptionMode(bool description_mode)
{
	m_DescriptionMode = description_mode;
	return 0;
}
//----------------------------------------------------------------------------
SegmentedCameraMotion_t* CameraMotionDescriptor::GetCameraMotionInfo(void)
{
	return m_CameraMotionInfo ;
}
//----------------------------------------------------------------------------
SegmentedCameraMotion_t CameraMotionDescriptor::GetCameraMotionSegmentInfo(long segment_index)
{
	return m_CameraMotionInfo[segment_index] ;
}
//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptor::SetCameraMotionSegmentInfo(SegmentedCameraMotion_t camera_motion_info, long segment_index)
{
	m_CameraMotionInfo[segment_index] = (SegmentedCameraMotion_t) camera_motion_info ;
	return 0 ;
}
//----------------------------------------------------------------------------
SegmentedCameraMotion_t CameraMotionDescriptor::CreateCameraMotionInfo(long start_time,
	                                                   long duration, 
	                                                   FractionalPresence_t presence, 
	                                                   AmountOfMotion_t speeds,	
	                                                   double x_FoeFoc = 0,	
	                                                   double y_FoeFoc = 0) 
{	
	SegmentedCameraMotion_t* SegmentedCameraMotion ;
	SegmentedCameraMotion = new SegmentedCameraMotion_t ;
	
	SegmentedCameraMotion->startTime = start_time ;
	SegmentedCameraMotion->duration = duration ;
	SegmentedCameraMotion->FocusOfExpansionFlag = 1 ;
	
	{
	int i;
	for (i=0; i<NB_CAMERA_OPERATIONS; i++)
		SegmentedCameraMotion->presence[i] = presence[i] ;
	}

	{
	int i;
	for (i=0; i<NB_CAMERA_OPERATIONS-1; i++)
		SegmentedCameraMotion->speeds[i]   = speeds[i] ;
	}
		
	SegmentedCameraMotion->x_FoeFoc = x_FoeFoc ;
	SegmentedCameraMotion->y_FoeFoc = y_FoeFoc ;
	
	return (*SegmentedCameraMotion) ;
}	            

//----------------------------------------------------------------------------
SegmentedCameraMotion_t CameraMotionDescriptor::CreateCameraMotionInfo(long start_time,
	                                                   long duration, 
	                                                   FractionalPresence_t presence, 
	                                                   AmountOfMotion_t speeds) 
{	
	SegmentedCameraMotion_t* SegmentedCameraMotion ;
	SegmentedCameraMotion = new SegmentedCameraMotion_t ;
	
	SegmentedCameraMotion->startTime = start_time ;
	SegmentedCameraMotion->duration = duration ;
	SegmentedCameraMotion->FocusOfExpansionFlag = 0 ;
	
	{
	int i;
	for (i=0; i<NB_CAMERA_OPERATIONS; i++)
		SegmentedCameraMotion->presence[i] = presence[i] ;
	}

	{
	int i;
	for (i=0; i<NB_CAMERA_OPERATIONS-1; i++)
		SegmentedCameraMotion->speeds[i]   = speeds[i] ;
	}
		
	SegmentedCameraMotion->x_FoeFoc = 0 ;
	SegmentedCameraMotion->y_FoeFoc = 0 ;
	
	return (*SegmentedCameraMotion) ;
}	            

//----------------------------------------------------------------------------
CameraMotionDescriptorInterfaceABC *CameraMotionDescriptor::GetInterface(void)
{ return &m_Interface; }


char *CMNames[NB_CAMERA_OPERATIONS] = { 
	"TrackLeft",
	"TrackRight",
	"BoomDown",
	"BoomUp",
	"DollyForward",
	"DollyBackward",
	"PanLeft",
	"PanRight",
	"TiltUp",
	"TiltDown",
	"RollClockwise",
	"RollAnticlockwise",
	"ZoomIn",
	"ZoomOut",
	"Fixed"
};

// Below this line, all added by: //0719-Sylvie
//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptor::ExportDDL(GenericDSInterfaceABC *theParent)
{

  int					seg;
  GenericDS				l_DDLDescription;
//  GenericDSInterfaceABC *l_DDLDescriptionInterface;
  GenericDS				Segment, 
						Time, 
						FocusOfExpansion, 
						FractionalPresence, 
						AmountOfMotion,
						HorizontalPosition,
						VerticalPosition;
  GenericDS				TrackLeft,
						TrackRight,
						TiltUp,
						TiltDown,
						BoomUp,
						BoomDown,
						PanRight,
						PanLeft,
						RollClockwise,
						RollAnticlockwise,
						ZoomIn,
						ZoomOut,
						DollyForward,
						DollyBackward,
						Fixed;
  GenericDS				MediaTimePoint,
						MediaDuration;
  long start_time, duration;
  char tstr[32];

  if(!theParent) return (unsigned long) -1;

  l_DDLDescription = theParent->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","CameraMotionType");

  for (seg=0; seg<GetNumSegments(); seg++) {
	Segment = l_DDLDescription.CreateChild("Segment");
	if (GetDescriptionMode() == MIXTURE_MODE)
		Segment.SetAttribute("xsi:type","MixtureCameraMotionSegmentType");
	else	//NON_MIXTURE_MODE
		Segment.SetAttribute("xsi:type","NonMixtureCameraMotionSegmentType");

	//time
	// KW - 2014/03 partial implemenation of MediaTime
	Time = Segment.CreateChild("MediaTime");
	//Time.SetAttribute("xsi:type","MediaTimeType");
	// fill when time SW available
	MediaTimePoint = Time.CreateChild("MediaTimePoint");
	start_time = GetCameraMotionSegmentInfo(seg).startTime;
	MTT_GetMediaTimePointString(start_time, tstr);
	MediaTimePoint.SetValue(string(tstr));
	MediaDuration = Time.CreateChild("MediaDuration");
	duration = GetCameraMotionSegmentInfo(seg).duration;
	MTT_GetMediaDurationString(duration, tstr);
	MediaDuration.SetValue(string(tstr));

	//FOE
	if (GetCameraMotionSegmentInfo(seg).FocusOfExpansionFlag == true)  {
		FocusOfExpansion = Segment.CreateChild("FocusOfExpansion");
		//FocusOfExpansion.SetAttribute("xsi:type","FocusOfExpansionType");
		//HorizontalPosition
		HorizontalPosition = FocusOfExpansion.CreateChild("HorizontalPosition");
		HorizontalPosition.SetValue(GetCameraMotionSegmentInfo(seg).x_FoeFoc);
		//VerticalPosition
		VerticalPosition = FocusOfExpansion.CreateChild("VerticalPosition");
		VerticalPosition.SetValue(GetCameraMotionSegmentInfo(seg).y_FoeFoc);
	}

	//extension, depending on Type
	if (GetDescriptionMode() == MIXTURE_MODE) {
		// Fractional Presence
		FractionalPresence = Segment.CreateChild("FractionalPresence");
		//FractionalPresence.SetAttribute("xsi:type","FractionalPresenceType");
		if (GetCameraMotionSegmentInfo(seg).presence[0])  {	//TRACK_LEFT
			TrackLeft = FractionalPresence.CreateChild(CMNames[TRACK_LEFT]);
			TrackLeft.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[0]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[1])  {	//TRACK_RIGHT
			TrackRight = FractionalPresence.CreateChild(CMNames[TRACK_RIGHT]);
			TrackRight.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[1]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[2])  {	//BOOM_DOWN
			BoomDown = FractionalPresence.CreateChild(CMNames[BOOM_DOWN]);
			BoomDown.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[2]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[3])  {	//BOOM_UP
			BoomUp = FractionalPresence.CreateChild(CMNames[BOOM_UP]);
			BoomUp.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[3]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[4])  {	//DOLLY_FORWARD
			DollyForward = FractionalPresence.CreateChild(CMNames[DOLLY_FORWARD]);
			DollyForward.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[4]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[5])  {	//DOLLY_BACKWARD
			DollyBackward = FractionalPresence.CreateChild(CMNames[DOLLY_BACKWARD]);
			DollyBackward.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[5]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[6])  {	//PAN_LEFT
			PanLeft = FractionalPresence.CreateChild(CMNames[PAN_LEFT]);
			PanLeft.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[6]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[7])  {	//PAN_RIGHT
			PanRight = FractionalPresence.CreateChild(CMNames[PAN_RIGHT]);
			PanRight.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[7]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[8])  {	//TILT_UP
			TiltDown = FractionalPresence.CreateChild(CMNames[TILT_UP]);
			TiltDown.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[8]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[9])  {	//TILT_DOWN
			TiltUp = FractionalPresence.CreateChild(CMNames[TILT_DOWN]);
			TiltUp.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[9]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[10])  {	//ROLL_CLOCKWISE
			RollClockwise = FractionalPresence.CreateChild(CMNames[ROLL_CLOCKWISE]);
			RollClockwise.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[10]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[11])  {	//ROLL_ANTICLOCKWISE
			RollAnticlockwise = FractionalPresence.CreateChild(CMNames[ROLL_ANTICLOCKWISE]);
			RollAnticlockwise.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[11]));
		}
		
		if (GetCameraMotionSegmentInfo(seg).presence[12])  {	//ZOOM_IN
			ZoomIn = FractionalPresence.CreateChild(CMNames[ZOOM_IN]);
			ZoomIn.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[12]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[13])  {	//ZOOM_OUT
			ZoomOut = FractionalPresence.CreateChild(CMNames[ZOOM_OUT]);
			ZoomOut.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[13]));
		}
		if (GetCameraMotionSegmentInfo(seg).presence[14])  {	//FIXED
			Fixed = FractionalPresence.CreateChild(CMNames[FIXED]);
			Fixed.SetValue((int) (127 * GetCameraMotionSegmentInfo(seg).presence[14]));
		}

		//AmountOfMotion
		AmountOfMotion = Segment.CreateChild("AmountOfMotion"); 
		AmountOfMotion.SetAttribute("xsi:type","MixtureAmountOfMotionType");
		if (GetCameraMotionSegmentInfo(seg).speeds[0])  {	//TRACK_LEFT
			TrackLeft = AmountOfMotion.CreateChild(CMNames[TRACK_LEFT]);
			TrackLeft.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[0]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[1])  {	//TRACK_RIGHT
			TrackRight = AmountOfMotion.CreateChild(CMNames[TRACK_RIGHT]);
			TrackRight.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[1]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[2])  {	//BOOM_DOWN
			BoomDown = AmountOfMotion.CreateChild(CMNames[BOOM_DOWN]);
			BoomDown.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[2]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[3])  {	//BOOM_UP
			BoomUp = AmountOfMotion.CreateChild(CMNames[BOOM_UP]);
			BoomUp.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[3]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[4])  {	//DOLLY_FORWARD
			DollyForward = AmountOfMotion.CreateChild(CMNames[DOLLY_FORWARD]);
			DollyForward.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[4]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[5])  {	//DOLLY_BACKWARD
			DollyBackward = AmountOfMotion.CreateChild(CMNames[DOLLY_BACKWARD]);
			DollyBackward.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[5]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[6])  {	//PAN_LEFT
			PanLeft = AmountOfMotion.CreateChild(CMNames[PAN_LEFT]);
			PanLeft.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[6]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[7])  {	//PAN_RIGHT
			PanRight = AmountOfMotion.CreateChild(CMNames[PAN_RIGHT]);
			PanRight.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[7]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[8])  {	//TILT_DOWN
			TiltDown = AmountOfMotion.CreateChild(CMNames[TILT_DOWN]);
			TiltDown.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[8]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[9])  {	//TILT_UP
			TiltUp = AmountOfMotion.CreateChild(CMNames[TILT_UP]);
			TiltUp.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[9]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[10])  {	//ROLL_CLOCKWISE
			RollClockwise = AmountOfMotion.CreateChild(CMNames[ROLL_CLOCKWISE]);
			RollClockwise.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[10]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[11])  {	//ROLL_ANTICLOCKWISE
			RollAnticlockwise = AmountOfMotion.CreateChild(CMNames[ROLL_ANTICLOCKWISE]);
			RollAnticlockwise.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[11]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[12])  {	//ZOOM_IN
			ZoomIn = AmountOfMotion.CreateChild(CMNames[ZOOM_IN]);
			ZoomIn.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[12]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[13])  {	//ZOOM_OUT
			ZoomOut = AmountOfMotion.CreateChild(CMNames[ZOOM_OUT]);
			ZoomOut.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[13]));
		}
	}
	else {	//NON_MIXTURE_MODE
		AmountOfMotion = Segment.CreateChild("AmountOfMotion"); 
		// only one, with a choice - test null
		if (GetCameraMotionSegmentInfo(seg).speeds[0])  {	//TRACK_LEFT
			TrackLeft = AmountOfMotion.CreateChild(CMNames[TRACK_LEFT]);
			TrackLeft.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[0]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[1])  {	//TRACK_RIGHT
			TrackRight = AmountOfMotion.CreateChild(CMNames[TRACK_RIGHT]);
			TrackRight.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[1]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[2])  {	//BOOM_DOWN
			BoomDown = AmountOfMotion.CreateChild(CMNames[BOOM_DOWN]);
			BoomDown.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[2]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[3])  {	//BOOM_UP
			BoomUp = AmountOfMotion.CreateChild(CMNames[BOOM_UP]);
			BoomUp.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[3]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[4])  {	//DOLLY_FORWARD
			DollyForward = AmountOfMotion.CreateChild(CMNames[DOLLY_FORWARD]);
			DollyForward.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[4]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[5])  {	//DOLLY_BACKWARD
			DollyBackward = AmountOfMotion.CreateChild(CMNames[DOLLY_BACKWARD]);
			DollyBackward.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[5]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[6])  {	//PAN_LEFT
			PanLeft = AmountOfMotion.CreateChild(CMNames[PAN_LEFT]);
			PanLeft.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[6]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[7])  {	//PAN_RIGHT
			PanRight = AmountOfMotion.CreateChild(CMNames[PAN_RIGHT]);
			PanRight.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[7]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[8])  {	//TILT_DOWN
			TiltDown = AmountOfMotion.CreateChild(CMNames[TILT_DOWN]);
			TiltDown.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[8]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[9])  {	//TILT_UP
			TiltUp = AmountOfMotion.CreateChild(CMNames[TILT_UP]);
			TiltUp.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[9]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[10])  {	//ROLL_CLOCKWISE
			RollClockwise = AmountOfMotion.CreateChild(CMNames[ROLL_CLOCKWISE]);
			RollClockwise.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[10]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[11])  {	//ROLL_ANTICLOCKWISE
			RollAnticlockwise = AmountOfMotion.CreateChild(CMNames[ROLL_ANTICLOCKWISE]);
			RollAnticlockwise.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[11]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[12])  {	//ZOOM_IN
			ZoomIn = AmountOfMotion.CreateChild(CMNames[ZOOM_IN]);
			ZoomIn.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[12]));
		}
		if (GetCameraMotionSegmentInfo(seg).speeds[13])  {	//ZOOM_OUT
			ZoomOut = AmountOfMotion.CreateChild(CMNames[ZOOM_OUT]);
			ZoomOut.SetValue((int) (2047 * GetCameraMotionSegmentInfo(seg).speeds[13]));
		}
		Fixed = AmountOfMotion.CreateChild(CMNames[FIXED]);
	}
  }
  return 0;
}

//----------------------------------------------------------------------------
unsigned long CameraMotionDescriptor::ImportDDL(GenericDSInterfaceABC *theDescription)
{

  int					seg=0, tempInt;
  long					start_time, duration;
  string				xsitype;
  GenericDS				l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
  string				tempText;
  GenericDS				Segment, 
						Time, 
						FocusOfExpansion, 
						FractionalPresence, 
						AmountOfMotion,
						HorizontalPosition,
						VerticalPosition;
  GenericDS				TrackLeft,
						TrackRight,
						TiltUp,
						TiltDown,
						BoomUp,
						BoomDown,
						PanRight,
						PanLeft,
						RollClockwise,
						RollAnticlockwise,
						ZoomIn,
						ZoomOut,
						DollyForward,
						DollyBackward,
						Fixed;
   FractionalPresence_t read_presence;
   AmountOfMotion_t     read_speeds;
   SegmentedCameraMotion_t camera_segment;
   double				read_x_FoeFoc;
   double				read_y_FoeFoc;
   bool					FOE_flag;
   GenericDS			MediaTimePoint,
						MediaDuration;

  if(!theDescription) return (unsigned long) -1;

  // if theDescription is of correct type
  if(theDescription->GetDSName() == "Descriptor") {
    theDescription->GetTextAttribute("xsi:type", xsitype);
    if(xsitype == "CameraMotionType") {
      l_DDLDescriptionInterface = theDescription;
    }
  }

  // else search for CameraMotionType as a child
  if(!l_DDLDescriptionInterface) {
    l_DDLDescription = theDescription->GetDescription("Descriptor");
    // search for correct xsi type
    while(!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype == "CameraMotionType") break;
      l_DDLDescription.GetNextSibling("Descriptor");
	}
    // CameraMotionType not found
    if(!l_DDLDescription.isNull()) return (unsigned long) -1;
    // CameraMotionType found
    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  Segment = l_DDLDescriptionInterface->GetDescription("Segment");

  // KW: fix 2013/07/23 - set segments
  if(!Segment.isNull())
  {
		seg=1;
		GenericDS Segment1 = Segment.GetNextSibling("Segment");
		while(!Segment1.isNull()) {
			seg++;
			Segment1 = Segment1.GetNextSibling("Segment");
		}
	  SetNumSegments(seg);
	  seg = 0;
  }


  while(!Segment.isNull()) {
     Segment.GetTextAttribute("xsi:type", xsitype);
	  if(xsitype == "MixtureCameraMotionSegmentType")
		  SetDescriptionMode(MIXTURE_MODE);
	  else if(xsitype == "NonMixtureCameraMotionSegmentType")
		  SetDescriptionMode(NON_MIXTURE_MODE);

	  for (int j=0; j<NB_CAMERA_OPERATIONS-1; j++)
	  {
			read_presence[j] = 0 ;
			read_speeds[j] = 0 ;
	  }
	  read_presence[NB_CAMERA_OPERATIONS-1] = 0 ;

	  read_x_FoeFoc = 0;
	  read_y_FoeFoc = 0;
	  start_time = 0;
	  duration = 0;

	  //time
	  Time = Segment.GetDescription("MediaTime");
	  if(!Time.isNull()) {
		  std::string v;
		  MediaTimePoint = Time.GetDescription("MediaTimePoint");
		  if(!MediaTimePoint.isNull())
		  {
			  MediaTimePoint.GetTextValue(v);
			  start_time = MTT_GetMediaTimeMilliseconds(v);
		  }
		  MediaDuration = Time.GetDescription("MediaDuration");
		  if(!MediaDuration.isNull())
		  {
			  MediaDuration.GetTextValue(v);
			  duration = MTT_GetMediaDurationMilliseconds(v);
		  }
	  }

	  //FOE
	  FocusOfExpansion = Segment.GetDescription("FocusOfExpansion");
	  if(!FocusOfExpansion.isNull()) {
		FOE_flag = true;
		HorizontalPosition = FocusOfExpansion.GetDescription("HorizontalPosition");
		HorizontalPosition.GetRealValue(read_x_FoeFoc);
		VerticalPosition = FocusOfExpansion.GetDescription("VerticalPosition");
		VerticalPosition.GetRealValue(read_y_FoeFoc);
	  }
	  else 
		FOE_flag = false;

	  //extension, depending on Type
	  if (GetDescriptionMode() == MIXTURE_MODE) {
		// Fractional Presence
		FractionalPresence = Segment.GetDescription("FractionalPresence");
		TrackLeft = FractionalPresence.GetDescription(CMNames[TRACK_LEFT]);
		if (!TrackLeft.isNull()) {
			TrackLeft.GetIntValue(tempInt);
			read_presence[0] = (double) tempInt / 127.0;
		}
		TrackRight = FractionalPresence.GetDescription(CMNames[TRACK_RIGHT]);
		if (!TrackRight.isNull()) {
			TrackRight.GetIntValue(tempInt);
			read_presence[1] = (double) tempInt / 127.0;
		}
		BoomDown = FractionalPresence.GetDescription(CMNames[BOOM_DOWN]);
		if (!BoomDown.isNull()) {
			BoomDown.GetIntValue(tempInt);
			read_presence[2] = (double) tempInt / 127.0;
		}
		BoomUp = FractionalPresence.GetDescription(CMNames[BOOM_UP]);
		if (!BoomUp.isNull()) {
			BoomUp.GetIntValue(tempInt);
			read_presence[3] = (double) tempInt / 127.0;
		}
		DollyForward = FractionalPresence.GetDescription(CMNames[DOLLY_FORWARD]);
		if (!DollyForward.isNull()) {
			DollyForward.GetIntValue(tempInt);
			read_presence[4] = (double) tempInt / 127.0;
		}
		DollyBackward = FractionalPresence.GetDescription(CMNames[DOLLY_BACKWARD]);
		if (!DollyBackward.isNull()) {
			DollyBackward.GetIntValue(tempInt);
			read_presence[5] = (double) tempInt / 127.0;
		}
		PanLeft = FractionalPresence.GetDescription(CMNames[PAN_LEFT]);
		if (!PanLeft.isNull()) {
			PanLeft.GetIntValue(tempInt);
			read_presence[6] = (double) tempInt / 127.0;
		}
		PanRight = FractionalPresence.GetDescription(CMNames[PAN_RIGHT]);
		if (!PanRight.isNull()) {
			PanRight.GetIntValue(tempInt);
			read_presence[7] = (double) tempInt / 127.0;
		}
		TiltDown = FractionalPresence.GetDescription(CMNames[TILT_DOWN]);
		if (!TiltDown.isNull()) {
			TiltDown.GetIntValue(tempInt);
			read_presence[8] = (double) tempInt / 127.0;
		}
		TiltUp = FractionalPresence.GetDescription(CMNames[TILT_UP]);
		if (!TiltUp.isNull()) {
			TiltUp.GetIntValue(tempInt);
			read_presence[9] = (double) tempInt / 127.0;
		}
		RollClockwise = FractionalPresence.GetDescription(CMNames[ROLL_CLOCKWISE]);
		if (!RollClockwise.isNull()) {
			RollClockwise.GetIntValue(tempInt);
			read_presence[10] = (double) tempInt / 127.0;
		}
		RollAnticlockwise = FractionalPresence.GetDescription(CMNames[ROLL_ANTICLOCKWISE]);
		if (!RollAnticlockwise.isNull()) {
			RollAnticlockwise.GetIntValue(tempInt);
			read_presence[11] = (double) tempInt / 127.0;
		}
		ZoomIn = FractionalPresence.GetDescription(CMNames[ZOOM_IN]);
		if (!ZoomIn.isNull()) {
			ZoomIn.GetIntValue(tempInt);
			read_presence[12] = (double) tempInt / 127.0;
		}
		ZoomOut = FractionalPresence.GetDescription(CMNames[ZOOM_OUT]);
		if (!ZoomOut.isNull()) {
			ZoomOut.GetIntValue(tempInt);
			read_presence[13] = (double) tempInt / 127.0;
		}
		Fixed = FractionalPresence.GetDescription(CMNames[FIXED]);
		if (!Fixed.isNull()) {
			Fixed.GetIntValue(tempInt);
			read_presence[14] = (double) tempInt / 127.0;
		}

		// Amount of Motion
		AmountOfMotion = Segment.GetDescription("AmountOfMotion");
		TrackLeft = AmountOfMotion.GetDescription(CMNames[TRACK_LEFT]);
		if (!TrackLeft.isNull()) {
			TrackLeft.GetIntValue(tempInt);
			read_speeds[0] = (double) tempInt / 2047.0;
		}
		TrackRight = AmountOfMotion.GetDescription(CMNames[TRACK_RIGHT]);
		if (!TrackRight.isNull()) {
			TrackRight.GetIntValue(tempInt);
			read_speeds[1] = (double) tempInt / 2047.0;
		}
		BoomDown = AmountOfMotion.GetDescription(CMNames[BOOM_DOWN]);
		if (!BoomDown.isNull()) {
			BoomDown.GetIntValue(tempInt);
			read_speeds[2] = (double) tempInt / 2047.0;
		}
		BoomUp = AmountOfMotion.GetDescription(CMNames[BOOM_UP]);
		if (!BoomUp.isNull()) {
			BoomUp.GetIntValue(tempInt);
			read_speeds[3] = (double) tempInt / 2047.0;
		}
		DollyForward = AmountOfMotion.GetDescription(CMNames[DOLLY_FORWARD]);
		if (!DollyForward.isNull()) {
			DollyForward.GetIntValue(tempInt);
			read_speeds[4] = (double) tempInt / 2047.0;
		}
		DollyBackward = AmountOfMotion.GetDescription(CMNames[DOLLY_BACKWARD]);
		if (!DollyBackward.isNull()) {
			DollyBackward.GetIntValue(tempInt);
			read_speeds[5] = (double) tempInt / 2047.0;
		}
		PanLeft = AmountOfMotion.GetDescription(CMNames[PAN_LEFT]);
		if (!PanLeft.isNull()) {
			PanLeft.GetIntValue(tempInt);
			read_speeds[6] = (double) tempInt / 2047.0;
		}
		PanRight = AmountOfMotion.GetDescription(CMNames[PAN_RIGHT]);
		if (!PanRight.isNull()) {
			PanRight.GetIntValue(tempInt);
			read_speeds[7] = (double) tempInt / 2047.0;
		}
		TiltDown = AmountOfMotion.GetDescription(CMNames[TILT_DOWN]);
		if (!TiltDown.isNull()) {
			TiltDown.GetIntValue(tempInt);
			read_speeds[8] = (double) tempInt / 2047.0;
		}
		TiltUp = AmountOfMotion.GetDescription(CMNames[TILT_UP]);
		if (!TiltUp.isNull()) {
			TiltUp.GetIntValue(tempInt);
			read_speeds[9] = (double) tempInt / 2047.0;
		}
		RollClockwise = AmountOfMotion.GetDescription(CMNames[ROLL_CLOCKWISE]);
		if (!RollClockwise.isNull()) {
			RollClockwise.GetIntValue(tempInt);
			read_speeds[10] = (double) tempInt / 2047.0;
		}
		RollAnticlockwise = AmountOfMotion.GetDescription(CMNames[ROLL_ANTICLOCKWISE]);
		if (!RollAnticlockwise.isNull()) {
			RollAnticlockwise.GetIntValue(tempInt);
			read_speeds[11] = (double) tempInt / 2047.0;
		}
		ZoomIn = AmountOfMotion.GetDescription(CMNames[ZOOM_IN]);
		if (!ZoomIn.isNull()) {
			ZoomIn.GetIntValue(tempInt);
			read_speeds[12] = (double) tempInt / 2047.0;
		}
		ZoomOut = AmountOfMotion.GetDescription(CMNames[ZOOM_OUT]);
		if (!ZoomOut.isNull()) {
			ZoomOut.GetIntValue(tempInt);
			read_speeds[13] = (double) tempInt / 2047.0;
		}
	  }
	  else {	//NON_MIXTURE_MODE
		// Amount of Motion
		AmountOfMotion = Segment.GetDescription("AmountOfMotion");
		TrackLeft = AmountOfMotion.GetDescription(CMNames[TRACK_LEFT]);
		if (!TrackLeft.isNull()) {
			TrackLeft.GetIntValue(tempInt);
			read_speeds[0] = (double) tempInt / 2047.0;
		}
		TrackRight = AmountOfMotion.GetDescription(CMNames[TRACK_RIGHT]);
		if (!TrackRight.isNull()) {
			TrackRight.GetIntValue(tempInt);
			read_speeds[1] = (double) tempInt / 2047.0;
		}
		BoomDown = AmountOfMotion.GetDescription(CMNames[BOOM_DOWN]);
		if (!BoomDown.isNull()) {
			BoomDown.GetIntValue(tempInt);
			read_speeds[2] = (double) tempInt / 2047.0;
		}
		BoomUp = AmountOfMotion.GetDescription(CMNames[BOOM_UP]);
		if (!BoomUp.isNull()) {
			BoomUp.GetIntValue(tempInt);
			read_speeds[3] = (double) tempInt / 2047.0;
		}
		DollyForward = AmountOfMotion.GetDescription(CMNames[DOLLY_FORWARD]);
		if (!DollyForward.isNull()) {
			DollyForward.GetIntValue(tempInt);
			read_speeds[4] = (double) tempInt / 2047.0;
		}
		DollyBackward = AmountOfMotion.GetDescription(CMNames[DOLLY_BACKWARD]);
		if (!DollyBackward.isNull()) {
			DollyBackward.GetIntValue(tempInt);
			read_speeds[5] = (double) tempInt / 2047.0;
		}
		PanLeft = AmountOfMotion.GetDescription(CMNames[PAN_LEFT]);
		if (!PanLeft.isNull()) {
			PanLeft.GetIntValue(tempInt);
			read_speeds[6] = (double) tempInt / 2047.0;
		}
		PanRight = AmountOfMotion.GetDescription(CMNames[PAN_RIGHT]);
		if (!PanRight.isNull()) {
			PanRight.GetIntValue(tempInt);
			read_speeds[7] = (double) tempInt / 2047.0;
		}
		TiltDown = AmountOfMotion.GetDescription(CMNames[TILT_DOWN]);
		if (!TiltDown.isNull()) {
			TiltDown.GetIntValue(tempInt);
			read_speeds[8] = (double) tempInt / 2047.0;
		}
		TiltUp = AmountOfMotion.GetDescription(CMNames[TILT_UP]);
		if (!TiltUp.isNull()) {
			TiltUp.GetIntValue(tempInt);
			read_speeds[9] = (double) tempInt / 2047.0;
		}
		RollClockwise = AmountOfMotion.GetDescription(CMNames[ROLL_CLOCKWISE]);
		if (!RollClockwise.isNull()) {
			RollClockwise.GetIntValue(tempInt);
			read_speeds[10] = (double) tempInt / 2047.0;
		}
		RollAnticlockwise = AmountOfMotion.GetDescription(CMNames[ROLL_ANTICLOCKWISE]);
		if (!RollAnticlockwise.isNull()) {
			RollAnticlockwise.GetIntValue(tempInt);
			read_speeds[11] = (double) tempInt / 2047.0;
		}
		ZoomIn = AmountOfMotion.GetDescription(CMNames[ZOOM_IN]);
		if (!ZoomIn.isNull()) {
			ZoomIn.GetIntValue(tempInt);
			read_speeds[12] = (double) tempInt / 2047.0;
		}
		ZoomOut = AmountOfMotion.GetDescription(CMNames[ZOOM_OUT]);
		if (!ZoomOut.isNull()) {
			ZoomOut.GetIntValue(tempInt);
			read_speeds[13] = (double) tempInt / 2047.0;
		}
	  }

	  if (FOE_flag)
		camera_segment = CreateCameraMotionInfo(start_time, duration, read_presence, read_speeds, read_x_FoeFoc, read_y_FoeFoc);
	  else
		camera_segment = CreateCameraMotionInfo(start_time, duration, read_presence, read_speeds, 0, 0); // 2013/07/23 fix

	  SetCameraMotionSegmentInfo(camera_segment, seg);
	  seg++;
	  // 2013/07/23 fix
	  Segment = Segment.GetNextSibling("Segment");
	  //l_DDLDescription.GetNextSibling("Segment");
   }
   // 2013/07/23 - bug, this was moved to allocate segments before they are accessed.
  //SetNumSegments(seg);
    
  return 0;
}
