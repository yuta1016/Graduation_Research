///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Philips Research USA, S. Jeannin
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
// Applicable File Name: MotionTrajectory.cpp
//
#include "Descriptors/Descriptors.h"                           /* Naming Convention */

using namespace XM;

const UUID MotionTrajectoryDescriptorInterface::myID = UUID("");
const char *MotionTrajectoryDescriptorInterface::myName = "MotionTrajectoryDescriptorInterface";

const UUID MotionTrajectoryDescriptor::myID = UUID("");
const char *MotionTrajectoryDescriptor::myName = "MotionTrajectoryDescriptor";

const UUID MotionTrajectoryDescriptor::valID = UUID("");
const char *MotionTrajectoryDescriptor::valName = "?";

const UUID MotionTrajectoryDescriptorInterfaceABC::myID = UUID();


MotionTrajectoryDescriptorInterfaceABC::MotionTrajectoryDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
MotionTrajectoryDescriptorInterface::MotionTrajectoryDescriptorInterface(MotionTrajectoryDescriptor *aTraj):
m_MotionTrajectoryDescriptor(aTraj)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterface::~MotionTrajectoryDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void MotionTrajectoryDescriptorInterface::addref(void) {
 m_MotionTrajectoryDescriptor->addref();
}

//----------------------------------------------------------------------------
void MotionTrajectoryDescriptorInterface::release(void) {
m_MotionTrajectoryDescriptor->release();
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetCameraFollows(void)
{
	return m_MotionTrajectoryDescriptor->GetCameraFollows();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetCameraFollows(int is_followed)
{
	return m_MotionTrajectoryDescriptor->SetCameraFollows(is_followed);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetCoordFlag(void)
{
	return m_MotionTrajectoryDescriptor->GetCoordFlag();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetCoordFlag(int coord_flag)
{
	return m_MotionTrajectoryDescriptor->SetCoordFlag(coord_flag);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetCoordRef(void)
{
	return m_MotionTrajectoryDescriptor->GetCoordRef();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetCoordRef(int coord_ref)
{
	return m_MotionTrajectoryDescriptor->SetCoordRef(coord_ref);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetSpatialReference(void)
{
	return m_MotionTrajectoryDescriptor->GetSpatialReference();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetSpatialReference(int spatial_ref)
{
	return m_MotionTrajectoryDescriptor->SetSpatialReference(spatial_ref);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetUnits(void)
{
	return m_MotionTrajectoryDescriptor->GetUnits();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetUnits(int the_units)
{
	return m_MotionTrajectoryDescriptor->SetUnits(the_units);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetCoordCodingLength(void)
{
	return m_MotionTrajectoryDescriptor->GetCoordCodingLength();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetCoordCodingLength(int coord_coding_length)
{
	return m_MotionTrajectoryDescriptor->SetCoordCodingLength(coord_coding_length);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetXRepr(void)
{
	return m_MotionTrajectoryDescriptor->GetXRepr();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetXRepr(int x_repr)
{
	return m_MotionTrajectoryDescriptor->SetXRepr(x_repr);
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptorInterface::GetYRepr(void)
{
	return m_MotionTrajectoryDescriptor->GetYRepr();
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::SetYRepr(int y_repr)
{
	return m_MotionTrajectoryDescriptor->SetYRepr(y_repr);
}

//----------------------------------------------------------------------------
//COORD SpatialCoordSysDescriptorInterfaceABC
//COORD *MotionTrajectoryDescriptorInterface::GetSpatialCoordSysDescriptorInterface(void)
//COORD {
//COORD 	return m_MotionTrajectoryDescriptor->GetSpatialCoordSysDescriptorInterface();
//COORD }

//----------------------------------------------------------------------------
//COORD unsigned long MotionTrajectoryDescriptorInterface::
//COORD SetSpatialCoordSysDescriptorInterface(SpatialCoordSysDescriptorInterfaceABC
//COORD 				 *aSpatialCoordSysDescriptorInterface)
//COORD {
//COORD 	return m_MotionTrajectoryDescriptor->
//COORD 	  SetSpatialCoordSysDescriptorInterface(aSpatialCoordSysDescriptorInterface);
//COORD }

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC
*MotionTrajectoryDescriptorInterface::GetTemporalInterpolationDescriptorInterface(void)
{
	return m_MotionTrajectoryDescriptor->GetTemporalInterpolationDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::
SetTemporalInterpolationDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
				 *aTemporalInterpolationDescriptorInterface)
{
	return m_MotionTrajectoryDescriptor->
	  SetTemporalInterpolationDescriptorInterface(aTemporalInterpolationDescriptorInterface);
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MotionTrajectoryDescriptorInterface::SetValuesFromParameterFile(void)
{
//TBD: needed functions from others not available - or is it because they are virtual?
  // Read CameraFollows and others
  // GetSpatialCoordSysDescriptorInterface()->SetValuesFromParameterFile();

  GetTemporalInterpolationDescriptorInterface()->SetValuesFromParameterFile();
  // should also use own parameters?
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *theParent)
{
  return m_MotionTrajectoryDescriptor->ExportDDL(theParent);
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *theParent)
{
  return m_MotionTrajectoryDescriptor->ImportDDL(theParent);
}

//=============================================================================
MotionTrajectoryDescriptor::MotionTrajectoryDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_CameraFollows(0),
m_CoordFlag(0),
m_CoordRef(0),
m_SpatialReference(0),
m_Units(0),
m_CoordCodingLength(0),
m_XRepr(0),
m_YRepr(0),
//m_SpatialCoordSysDescriptor(0),
//m_TemporalInterpolationDescriptor(0),
//COORD m_SpatialCoordSysInterface(0),
m_TemporalInterpolationInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptor::
MotionTrajectoryDescriptor(//COORD SpatialCoordSysDescriptorInterfaceABC *coordSys,
			   TemporalInterpolationDescriptorInterfaceABC *interp):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_CameraFollows(0),
m_CoordFlag(0),
m_CoordRef(0),
m_SpatialReference(0),
m_Units(0),
m_CoordCodingLength(0),
m_XRepr(0),
m_YRepr(0),
//COORD m_SpatialCoordSysInterface(0),
m_TemporalInterpolationInterface(0)
{
//COORD         SpatialCoordSysDescriptor *m_SpatialCoordSysDescriptor;
        TemporalInterpolationDescriptor *m_TemporalInterpolationDescriptor;

#ifdef VERBOSE
        fprintf(stderr,"Create sub descriptors\n");
#endif

//COORD 	if (!coordSys) {
//COORD 	  m_SpatialCoordSysDescriptor= new SpatialCoordSysDescriptor;
//COORD 	  coordSys=m_SpatialCoordSysDescriptor->GetInterface();
//COORD 	}
//COORD 	if (coordSys) SetSpatialCoordSysDescriptorInterface(coordSys);

	if (!interp) {
	  m_TemporalInterpolationDescriptor= new TemporalInterpolationDescriptor;
	  interp=m_TemporalInterpolationDescriptor->GetInterface();
	}
	if (interp) SetTemporalInterpolationDescriptorInterface(interp);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptor::~MotionTrajectoryDescriptor()
{
//COORD   if (m_SpatialCoordSysInterface)
//COORD     m_SpatialCoordSysInterface->release();
  if (m_TemporalInterpolationInterface)
    m_TemporalInterpolationInterface->release();
#ifdef VERBOSE
       fprintf(stderr,"MotionTrajectory delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MotionTrajectoryDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MotionTrajectoryDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void MotionTrajectoryDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"MotionTrajectory connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void MotionTrajectoryDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"MotionTrajectory release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetCameraFollows(void)
{
	return m_CameraFollows;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetCameraFollows(int is_followed)
{
        m_CameraFollows = is_followed;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetCoordFlag(void)
{
	return m_CoordFlag;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetCoordFlag(int coord_flag)
{
        m_CoordFlag = coord_flag;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetCoordRef(void)
{
	return m_CoordRef;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetCoordRef(int coord_ref)
{
        m_CoordRef = coord_ref;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetSpatialReference(void)
{
	return m_SpatialReference;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetSpatialReference(int spatial_ref)
{
        m_SpatialReference = spatial_ref;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetUnits(void)
{
	return m_Units;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetUnits(int the_units)
{
        m_Units = the_units;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetCoordCodingLength(void)
{
	return m_CoordCodingLength;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetCoordCodingLength(int coord_coding_length)
{
        m_CoordCodingLength = coord_coding_length;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetXRepr(void)
{
	return m_XRepr;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetXRepr(int x_repr)
{
        m_XRepr = x_repr;

	return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryDescriptor::GetYRepr(void)
{
	return m_YRepr;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetYRepr(int y_repr)
{
        m_YRepr = y_repr;

	return 0;
}

//----------------------------------------------------------------------------
//COORD SpatialCoordSysDescriptorInterfaceABC * MotionTrajectoryDescriptor::
//COORD GetSpatialCoordSysDescriptorInterface(void)
//COORD {
//COORD 	return m_SpatialCoordSysInterface;
//COORD }

//----------------------------------------------------------------------------
//COORD unsigned long MotionTrajectoryDescriptor::SetSpatialCoordSysDescriptorInterface
//COORD 	(SpatialCoordSysDescriptorInterfaceABC *coordSys)
//COORD {
//COORD        if (!coordSys) return (unsigned long) -1;
//COORD        if (m_SpatialCoordSysInterface == coordSys) return 0;
//COORD        if (m_SpatialCoordSysInterface) m_SpatialCoordSysInterface->release();
//COORD        m_SpatialCoordSysInterface = coordSys;
//COORD        if (m_SpatialCoordSysInterface) m_SpatialCoordSysInterface->addref();
//COORD        else return (unsigned long) -1;
//COORD
//COORD        return 0;
//COORD }

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC * MotionTrajectoryDescriptor::
GetTemporalInterpolationDescriptorInterface(void)
{
	return m_TemporalInterpolationInterface;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::SetTemporalInterpolationDescriptorInterface
	(TemporalInterpolationDescriptorInterfaceABC *interp)
{
       if (!interp) return (unsigned long) -1;
       if (m_TemporalInterpolationInterface == interp) return 0;
       if (m_TemporalInterpolationInterface) m_TemporalInterpolationInterface->release();
       m_TemporalInterpolationInterface = interp;
       if (m_TemporalInterpolationInterface) m_TemporalInterpolationInterface->addref();
       else return (unsigned long) -1;

       return 0;
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC *MotionTrajectoryDescriptor::GetInterface(void)
{
	return &m_Interface;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::
ExportDDL(GenericDSInterfaceABC *theParent)
{

  int					tempInt;
  GenericDS				l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface;
  GenericDS				CoordRef, CoordDef, TrajParams;
  
  if(!theParent) return (unsigned long) -1;

  l_DDLDescription = theParent->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","MotionTrajectoryType");

  if ( GetCameraFollows() ) {	
	tempInt = GetCameraFollows();
	if (tempInt == 2)	l_DDLDescription.SetAttribute("cameraFollows","false");
	if (tempInt == 3)	l_DDLDescription.SetAttribute("cameraFollows","true");
  }

  if (GetCoordFlag()==1)  {
	CoordRef = l_DDLDescription.CreateChild("TrajCoordRef");
	// reference to coordinates
	//CoordRef.SetAttribute("coordRef", (unsigned short int)GetCoordRef());
	// spatial references
	if ( GetSpatialReference() )
		CoordRef.SetAttribute("spatialReference","true");
	else CoordRef.SetAttribute("spatialReference","false");
  }
  else {
	CoordDef = l_DDLDescription.CreateChild("TrajCoordDef");
	// Units 
	switch(GetUnits())
	{
		case 0:	CoordDef.SetAttribute("Units", "pictureWidthAndHeight");	break;
		case 1:	CoordDef.SetAttribute("Units", "pictureHeight");			break;
		case 2:	CoordDef.SetAttribute("Units", "pictureWidth");				break;
		case 3:	CoordDef.SetAttribute("Units", "meter");					break;
		default: break;
	}
	// X-Repr, Y-Repr
	if ( GetCoordCodingLength()==1 ) {		
		CoordDef.SetAttribute("xRepr",GetXRepr());
		CoordDef.SetAttribute("yRepr",GetYRepr());
	}
  }

  //TrajParams = l_DDLDescription.CreateChild("TrajParams"); // ? specify name?
  l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  m_TemporalInterpolationInterface->ExportDDL(l_DDLDescriptionInterface); 
  
  return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryDescriptor::
ImportDDL(GenericDSInterfaceABC *theDescription)
{

  string				xsitype;
  GenericDS				l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL, *TrajParamsInterface;
  string				tempText;
  GenericDS				CoordRef, CoordDef, TrajParams;
  int					isAttribute, tempInt;
  bool					tempBool;

  if(!theDescription) return (unsigned long) -1;

  // if theDescription is of correct type
  if(theDescription->GetDSName() == "Descriptor") {
    theDescription->GetTextAttribute("xsi:type", xsitype);
    if((xsitype == "MotionTrajectoryType") || (xsitype == "MotionTrajectoryTypeType")) {
      l_DDLDescriptionInterface = theDescription;
    }
  }

  // else search for MotionTrajectoryType as a child
  if(!l_DDLDescriptionInterface) {
    l_DDLDescription = theDescription->GetDescription("Descriptor");
    // search for correct xsi type
    while(!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if((xsitype == "MotionTrajectoryType") || (xsitype == "MotionTrajectoryTypeType")) break;
      l_DDLDescription.GetNextSibling("Descriptor");
	}
    // MotionTrajectoryType not found
    if(!l_DDLDescription.isNull()) return (unsigned long) -1;
    // MotionTrajectoryType found
    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  // cameraFollows
  isAttribute = l_DDLDescriptionInterface->GetBoolAttribute("cameraFollows",tempBool);
  if (isAttribute == -2) {	// NO_SUCH_ATTR
    SetCameraFollows(0);
  }
  else {
    if(tempBool)	SetCameraFollows(3);
    else			SetCameraFollows(2);
  }

  CoordRef = l_DDLDescriptionInterface->GetDescription("TrajCoordRef");
  if(!CoordRef.isNull()) {
	SetCoordFlag (1);
    // reference to coordinates
    //l_CoordRef.GetIntValue( CoordRef );
	//SetCoordRef( CoordRef );
    // spatial references
    CoordRef.GetBoolAttribute("spatialReference",tempBool);
	if (tempBool) SetSpatialReference(1);
	else SetSpatialReference(0);
  }

  CoordDef = l_DDLDescriptionInterface->GetDescription("TrajCoordDef");
  if(!CoordDef.isNull()) {
	SetCoordFlag (0);

	// Units 
	CoordDef.GetTextAttribute("Units", tempText);
	if (tempText=="pictureWidthAndHeight")	SetUnits(0);
	else if (tempText=="pictureHeight")		SetUnits(1);
	else if (tempText=="pictureWidth")		SetUnits(2);
	else if (tempText=="meter")				SetUnits(3);

	// X-Repr, Y-Repr, Z-Repr
    isAttribute = CoordDef.GetIntAttribute("xRepr",tempInt);
    if (isAttribute == -2) {  // NO_SUCH_ATTR
      SetCoordCodingLength(0);
	}
	else {
	  SetCoordCodingLength(1);
	  SetXRepr(tempInt);
	  CoordDef.GetIntAttribute("yRepr",tempInt);
	  SetYRepr(tempInt);
	}
  }

  // 2013/07/24 - KW fix - make consistent with ExportDDL
  TrajParams = l_DDLDescriptionInterface->GetDescription("Descriptor");
  if(!TrajParams.isNull()) {
	  TrajParams.GetTextAttribute("xsi:type", xsitype);
	  if(xsitype == "TemporalInterpolation")
	  {
		  TrajParamsInterface = TrajParams.GetInterface();
		  m_TemporalInterpolationInterface->ImportDDL(TrajParamsInterface);
	  }
  }

  return 0;
}

