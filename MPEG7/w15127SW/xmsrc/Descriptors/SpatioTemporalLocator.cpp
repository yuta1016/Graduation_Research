///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Applicable File Name:  SpatioTemporalLocator.cpp
//

#include <cstring>
#include "Descriptors/Descriptors.h"                                    /* Naming Convention */

using namespace XM;

const UUID SpatioTemporalLocatorDescriptorInterface::myID = UUID("");
const char *SpatioTemporalLocatorDescriptorInterface::myName = "SpatioTemporalLocatorDescriptorInterface";

const UUID SpatioTemporalLocatorDescriptor::myID = UUID("");
const char *SpatioTemporalLocatorDescriptor::myName = "SpatioTemporalLocatorDescriptor";

const UUID SpatioTemporalLocatorDescriptor::valID = UUID("");
const char *SpatioTemporalLocatorDescriptor::valName = "SpatioTemporalLocator";

const UUID SpatioTemporalLocatorDescriptorInterfaceABC::myID = UUID();


//=============================================================================
SpatioTemporalLocatorDescriptorInterfaceABC::
SpatioTemporalLocatorDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpatioTemporalLocatorDescriptorInterface::
SpatioTemporalLocatorDescriptorInterface(SpatioTemporalLocatorDescriptor
					 *aSpatioTemporalLocator):
m_SpatioTemporalLocatorDescriptor(aSpatioTemporalLocator)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterface::
~SpatioTemporalLocatorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpatioTemporalLocatorDescriptorInterface::addref(void)
{ m_SpatioTemporalLocatorDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpatioTemporalLocatorDescriptorInterface::release(void)
{ m_SpatioTemporalLocatorDescriptor->release(); }

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::GetCoordFlag(void)
{	
	return m_SpatioTemporalLocatorDescriptor->GetCoordFlag();   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
SetCoordFlag(unsigned long f)
{
	return m_SpatioTemporalLocatorDescriptor->SetCoordFlag(f);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::GetRef(void)
{	
	return m_SpatioTemporalLocatorDescriptor->GetRef();   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::SetRef(unsigned long r)
{
	return m_SpatioTemporalLocatorDescriptor->SetRef(r);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::GetspatialRef(void)
{	
	return m_SpatioTemporalLocatorDescriptor->GetspatialRef();   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::SetspatialRef(unsigned long r)
{
	return m_SpatioTemporalLocatorDescriptor->SetspatialRef(r);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::GetNumberOfRefernceRegions(void)
{	
	return m_SpatioTemporalLocatorDescriptor->GetNumberOfRefernceRegions();   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::SetNumberOfRefernceRegions(unsigned long NumberOfRefernceRegions)
{
	return m_SpatioTemporalLocatorDescriptor->SetNumberOfRefernceRegions(NumberOfRefernceRegions);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::GetTypeOfTrajectory(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->GetTypeOfTrajectory(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::SetTypeOfTrajectory(unsigned long i, unsigned long Type)
{
	return m_SpatioTemporalLocatorDescriptor->SetTypeOfTrajectory(i, Type);
}

// Trajectory
//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
GetMediaTimePoint(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->GetMediaTimePoint(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
SetMediaTimePoint(unsigned long i, unsigned long Time)
{
	return m_SpatioTemporalLocatorDescriptor->SetMediaTimePoint(i, Time);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
GetMediaDuration(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->GetMediaDuration(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
SetMediaDuration(unsigned long i, unsigned long Time)
{
	return m_SpatioTemporalLocatorDescriptor->SetMediaDuration(i, Time);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
Gettype(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->Gettype(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
Settype(unsigned long i, unsigned long Type)
{
	return m_SpatioTemporalLocatorDescriptor->Settype(i, Type);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
GetmotionModel(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->GetmotionModel(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
SetmotionModel(unsigned long i, unsigned long Model)
{
	return m_SpatioTemporalLocatorDescriptor->SetmotionModel(i, Model);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
GetellipseFlag(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->GetellipseFlag(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::
SetellipseFlag(unsigned long i, unsigned long F)
{
	return m_SpatioTemporalLocatorDescriptor->SetellipseFlag(i, F);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::GetDepthFlag(unsigned long i)
{
	return m_SpatioTemporalLocatorDescriptor->GetDepthFlag(i);
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::SetDepthFlag(unsigned long i, unsigned long F)
{
	return m_SpatioTemporalLocatorDescriptor->SetDepthFlag(i, F);
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC*
SpatioTemporalLocatorDescriptorInterface::
GetTemporalInterpolationDescriptorInterface(unsigned long i, unsigned long j)
{
	return m_SpatioTemporalLocatorDescriptor->GetTemporalInterpolationDescriptorInterface(i, j);
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorDescriptorInterface::
SetTemporalInterpolationDescriptorInterface(unsigned long i, unsigned long j,
			       TemporalInterpolationDescriptorInterfaceABC
			       *aTemporalInterpolationDescriptorInterface)	
{
  return m_SpatioTemporalLocatorDescriptor->
    SetTemporalInterpolationDescriptorInterface(i,j,
	       aTemporalInterpolationDescriptorInterface);
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC*
SpatioTemporalLocatorDescriptorInterface::
GetRegionLocatorDescriptorInterface(unsigned long i)
{
  return m_SpatioTemporalLocatorDescriptor->
    GetRegionLocatorDescriptorInterface(i);
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorDescriptorInterface::
SetRegionLocatorDescriptorInterface(unsigned long i
				    , RegionLocatorDescriptorInterfaceABC
				    *aRegionLocatorDescriptorInterface)	
{
  return m_SpatioTemporalLocatorDescriptor->
    SetRegionLocatorDescriptorInterface(i, aRegionLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpatioTemporalLocatorDescriptorInterface::SetValuesFromParameterFile(void)
{
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::ExportDDL(GenericDSInterfaceABC *theParent)
{
  return m_SpatioTemporalLocatorDescriptor->ExportDDL(theParent);
}
//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptorInterface::ImportDDL(GenericDSInterfaceABC *theParent)
{
  return m_SpatioTemporalLocatorDescriptor->ImportDDL(theParent);
}


//=============================================================================
SpatioTemporalLocatorDescriptor::SpatioTemporalLocatorDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_NumberOfRefernceRegions(0)
{
	int i;

	for (i=0; i<MAXNRefrence; i++) { m_RegionLocatorDescriptorInterface[i] = 0; }
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {m_TemporalInterpolationDescriptorInterface[i] = 0; }

#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  RegionLocatorDescriptor *m_RegionLocator[MAXNRefrence];
	TemporalInterpolationDescriptor *m_TemporalInterpolation[MAXNRefrence*MAXNVertices+2];
	
	// SetRLInterface
	for (i=0; i<MAXNRefrence; i++) {
		m_RegionLocator[i] = new RegionLocatorDescriptor;
	    SetRegionLocatorDescriptorInterface(i,m_RegionLocator[i]->GetInterface());
	}
	// SetTIInterface
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {
		m_TemporalInterpolation[i] = new TemporalInterpolationDescriptor;
		SetTemporalInterpolationDescriptorInterface(0,i,m_TemporalInterpolation[i]->GetInterface());
	}

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptor::SpatioTemporalLocatorDescriptor(
		TemporalInterpolationDescriptorInterfaceABC** TemporalInterpolation,
		RegionLocatorDescriptorInterfaceABC** RegionLocator):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_NumberOfRefernceRegions(0)
{
	// Toshiba : I don't know how to use a lot of sub-Desctiptors.
	int i;

#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

    for (i=0; i<MAXNRefrence; i++) { m_RegionLocatorDescriptorInterface[i] = 0; }
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {m_TemporalInterpolationDescriptorInterface[i] = 0; }

	RegionLocatorDescriptor *m_RegionLocator[MAXNRefrence];
	TemporalInterpolationDescriptor *m_TemporalInterpolation[MAXNRefrence*MAXNVertices+2];	

    if (!RegionLocator) {	
		RegionLocator = new RegionLocatorDescriptorInterfaceABC*[MAXNRefrence]; 
	    for (i=0; i<MAXNRefrence; i++) { RegionLocator[i] = 0; }
	}
    if (!TemporalInterpolation) { 
		TemporalInterpolation = new TemporalInterpolationDescriptorInterfaceABC*[MAXNRefrence*MAXNVertices+2]; 
		for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {TemporalInterpolation[i] = 0; }
	}

	// SetRLInterface
	for (i=0; i<MAXNRefrence; i++) {
	    if (!RegionLocator[i]) {
			m_RegionLocator[i] = new RegionLocatorDescriptor;
			RegionLocator[i] = m_RegionLocator[i]->GetInterface();
		}
		if (RegionLocator[i]) SetRegionLocatorDescriptorInterface(i,RegionLocator[i]);
	}
	// SetTIInterface
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {
	    if (!TemporalInterpolation[i]) {
			m_TemporalInterpolation[i] = new TemporalInterpolationDescriptor;
			TemporalInterpolation[i] = m_TemporalInterpolation[i]->GetInterface();
		}
		if (TemporalInterpolation[i]) SetTemporalInterpolationDescriptorInterface(0,i,TemporalInterpolation[i]);
	}

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptor::~SpatioTemporalLocatorDescriptor()
{
	int i;
	
	for (i=0; i<MAXNRefrence; i++) {
//		if (m_RegionLocatorInterface[i])
//		   m_RegionLocatorInterface[i]->release();		// release not support	
	}
	for (i=0; i<MAXNRefrence*MAXNVertices+2; i++) {
		if (m_TemporalInterpolationDescriptorInterface[i])
			m_TemporalInterpolationDescriptorInterface[i]->release();	
	}
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpatioTemporalLocatorDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpatioTemporalLocatorDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void SpatioTemporalLocatorDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"SpatioTemporalLocator connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpatioTemporalLocatorDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"SpatioTemporalLocator release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterfaceABC 
*SpatioTemporalLocatorDescriptor::GetInterface(void)
{
	return &m_Interface; 
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetCoordFlag(void)
{	
	return m_CoordFlag;   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetCoordFlag(unsigned long f)
{
	m_CoordFlag = f;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetRef(void)
{	
	return m_Ref;   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetRef(unsigned long r)
{
	m_Ref = r;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetspatialRef(void)
{	
	return m_spatialRef;   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetspatialRef(unsigned long r)
{
	m_spatialRef = r;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetNumberOfRefernceRegions(void)
{	
	return m_NumberOfRefernceRegions;   
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetNumberOfRefernceRegions(unsigned long NumberOfRefernceRegions)
{
	m_NumberOfRefernceRegions = NumberOfRefernceRegions;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetTypeOfTrajectory(unsigned long i)
{
	return m_TypeOfTrajectory[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetTypeOfTrajectory(unsigned long i, unsigned long Type)
{
	m_TypeOfTrajectory[i] = Type;
	return 0;
}

// FigureTrajectory
//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::
GetMediaTimePoint(unsigned long i)
{
	return m_MediaTimePoint[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetMediaTimePoint(unsigned long i,
							    unsigned long Time)
{
	m_MediaTimePoint[i] = Time;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::
GetMediaDuration(unsigned long i)
{
	return m_MediaDuration[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetMediaDuration(unsigned long i,
							    unsigned long Time)
{
	m_MediaDuration[i] = Time;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::Gettype(unsigned long i)
{
	return m_type[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::
Settype(unsigned long i, unsigned long Type)
{
	m_type[i] = Type;
	return 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC*
SpatioTemporalLocatorDescriptor::
GetTemporalInterpolationDescriptorInterface(unsigned long i, unsigned long j)
{
	return m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j];
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorDescriptor::
SetTemporalInterpolationDescriptorInterface (unsigned long i, unsigned long j,
	  TemporalInterpolationDescriptorInterfaceABC *aTemporalInterpolation)
{
  if (!aTemporalInterpolation) return (unsigned long) -1;
  if (m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j] == aTemporalInterpolation) return 0;
  if (m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j])
    m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j]->release();
  m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j] = aTemporalInterpolation;
  if (m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j])
    m_TemporalInterpolationDescriptorInterface[i*MAXNVertices+j]->addref();
  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC*
	SpatioTemporalLocatorDescriptor::
GetRegionLocatorDescriptorInterface(unsigned long i)
{
	return m_RegionLocatorDescriptorInterface[i];
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorDescriptor::SetRegionLocatorDescriptorInterface
   (unsigned long i, RegionLocatorDescriptorInterfaceABC *aRegionLocator)
{
  if (!aRegionLocator) return (unsigned long) -1;
  if (m_RegionLocatorDescriptorInterface[i] == aRegionLocator) return 0;
//  if (m_RegionLocatorDescriptorInterface[i])
//    m_RegionLocatorDescriptorInterface[i]->release();		// release not support
  m_RegionLocatorDescriptorInterface[i] = aRegionLocator;
//  if (m_RegionLocatorDescriptorInterface[i])
//    m_RegionLocatorDescriptorInterface[i]->addref();		// addref not support
//  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetmotionModel(unsigned long i)
{
	return m_motionModel[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetmotionModel(unsigned long i, unsigned long Model)
{
	m_motionModel[i] = Model;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetellipseFlag(unsigned long i)
{
	return m_ellipseFlag[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetellipseFlag(unsigned long i, unsigned long F)
{
	m_ellipseFlag[i] = F;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::GetDepthFlag(unsigned long i)
{
	return m_DepthFlag[i];
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::SetDepthFlag(unsigned long i, unsigned long F)
{
	m_DepthFlag[i] = F;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  SpatioTemporalLocatorDescriptor::GetMediaTimePointString(int i,char* s) {
	unsigned long ul = GetMediaTimePoint(i);
	sprintf(s,"T%02ld:%02ld:%02ld", ExtractHours(ul), ExtractMinutes(ul),
		ExtractSeconds(ul) );
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  SpatioTemporalLocatorDescriptor::GetMediaDurationString(unsigned long ul,char* s) {
	if(ExtractHours(ul)){
		sprintf(s,"PT%ldH%ldM%ldS", ExtractHours(ul), ExtractMinutes(ul),
			ExtractSeconds(ul) );
	} else if(ExtractMinutes(ul)){
		sprintf(s,"PT%ldM%ldS", ExtractMinutes(ul), ExtractSeconds(ul) );
	} else if(ExtractMinutes(ul)){
		sprintf(s,"PT%ldS", ExtractSeconds(ul) );
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  SpatioTemporalLocatorDescriptor::ExtractHours(unsigned long second) {
	unsigned long hrs=second/3600;
	return hrs%24;
}

//----------------------------------------------------------------------------
unsigned long  SpatioTemporalLocatorDescriptor::ExtractMinutes(unsigned long second) {
	unsigned long mts=second/60;
	return mts%60;
}

//----------------------------------------------------------------------------
unsigned long  SpatioTemporalLocatorDescriptor::ExtractSeconds(unsigned long second) {
	return second%60;
}

//----------------------------------------------------------------------------
unsigned long  SpatioTemporalLocatorDescriptor::ExtractSeconds(string s) {
	int n;
	unsigned int second=0;
	char c[64], *token;
	n = s.find('P');
	if(n==0){	// Duration
		strcpy(c,s.data());
		n = s.find('T');
		if(n>0){
			token = strtok(c,"T");
			n = s.find('H');
			if(n>0){
				token = strtok(NULL,"H");
				second+=atoi(token)*3600;
			}
			n = s.find('M');
			if(n>0){
				token = strtok(NULL,"M");
				second+=atoi(token)*60;
			}
			n = s.find('S');
			if(n>0){
				token = strtok(NULL,"S");
				second+=atoi(token);
			}
			n = s.find('N');
			if(n>0){
				token = strtok(NULL,"N");
				double N=atof(token);
				token = strtok(NULL,"F");
				double F=atof(token);
				second+=(unsigned int)(N/F);
			}
		}
	} else {	// TimePoint
		n=1;
		strncpy(c,s.data()+n,2);
		second+=atoi(c)*3600;
		n+=3;
		strncpy(c,s.data()+n,2);
		second+=atoi(c)*60;
		n+=3;
		strncpy(c,s.data()+n,2);
		second+=atoi(c);
	}
	return second;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::ExportDDL(GenericDSInterfaceABC *theParent)
{
	// Get the XML DOM Node from the GenericDSInterface

	GenericDS DS_Ele0=theParent->CreateChild("SpatioTemporalLocator");
	GenericDSInterfaceABC *DS_Ele=DS_Ele0.GetInterface();

	if (GetCoordFlag()!=0) {
		GenericDS STL_Ele0=DS_Ele->CreateChild("CoordRef");
		GenericDSInterfaceABC *STL_Ele=STL_Ele0.GetInterface();
		STL_Ele->SetAttribute("ref", (int)GetRef());
		if (GetspatialRef()!=0) { STL_Ele->SetAttribute("spatialRef", "true"); }
		else { STL_Ele->SetAttribute("spatialRef", "false"); }
	}

	long i, j;
	for (i=0; i<GetNumberOfRefernceRegions(); i++) {

		if (GetTypeOfTrajectory(i)==0) {		//  FigureTrajectory

			GenericDS FT_Ele0=DS_Ele->CreateChild("FigureTrajectory");
			GenericDSInterfaceABC *FT_Ele=FT_Ele0.GetInterface();
			
			switch( Gettype(i) ){
			case 0: return 1; // forbidden
			case 1: FT_Ele->SetAttribute("type", "rectangle"); break;
			case 2: FT_Ele->SetAttribute("type", "ellipse"); break;
			default: FT_Ele->SetAttribute("type", "polygon");
			}

			GenericDS C_Ele0=FT_Ele->CreateChild("MediaTime");
			GenericDSInterfaceABC *C_Ele=C_Ele0.GetInterface();

			char s[256];
//			int d;
			GenericDS MT_Ele0=C_Ele->CreateChild("MediaTimePoint");
			GenericDSInterfaceABC *MT_Ele=MT_Ele0.GetInterface();
			GetMediaTimePointString(i,s);
			MT_Ele->SetValue(s);

			GenericDS MD_Ele0=C_Ele->CreateChild("MediaDuration");
			GenericDSInterfaceABC *MD_Ele=MD_Ele0.GetInterface();
			GetMediaDurationString((unsigned long)(GetMediaDuration(i)),s);
			MD_Ele->SetValue(s);


			// TI
		    int Nv = Gettype(i);
		    if (Nv<=2) { Nv = 3; }
		    for (j=0; j<Nv; j++) {
				GetTemporalInterpolationDescriptorInterface(i,j)->ExportDDL(FT_Ele);
			}
			
			// Depth
			if (GetDepthFlag(i)==1) {
				GetTemporalInterpolationDescriptorInterface(i,MAXNRefrence*MAXNVertices+1)->ExportDDL(FT_Ele);
			}

		} else if (GetTypeOfTrajectory(i)==1) {		//  ParameterTrajectory

			GenericDS PT_Ele0=DS_Ele->CreateChild("ParameterTrajectory");
			GenericDSInterfaceABC *PT_Ele=PT_Ele0.GetInterface();
			
			switch( GetmotionModel(i) ){
			case 0: PT_Ele->SetAttribute("motionModel", "still"); break;
			case 1: PT_Ele->SetAttribute("motionModel", "translation"); break;
			case 2: PT_Ele->SetAttribute("motionModel", "rotationOrScaling"); break;
			case 3: PT_Ele->SetAttribute("motionModel", "affine"); break;
			case 4: PT_Ele->SetAttribute("motionModel", "perspective"); break;
			case 5: PT_Ele->SetAttribute("motionModel", "parabolic"); break;
			default: return 1;
			}
			if( GetellipseFlag(i) ){ PT_Ele->SetAttribute("ellipseFlag", "true"); }
			else {PT_Ele->SetAttribute("ellipseFlag", "false"); }

			char s[256];
			// Time
			GenericDS C_Ele0=PT_Ele->CreateChild("MediaTime");
			GenericDSInterfaceABC *C_Ele=C_Ele0.GetInterface();

			GenericDS MT_Ele0=C_Ele->CreateChild("MediaTimePoint");
			GenericDSInterfaceABC *MT_Ele=MT_Ele0.GetInterface();
			GetMediaTimePointString(i,s);
			MT_Ele->SetValue(s);

			GenericDS MD_Ele0=C_Ele->CreateChild("MediaDuration");
			GenericDSInterfaceABC *MD_Ele=MD_Ele0.GetInterface();
			GetMediaDurationString((unsigned long)(GetMediaDuration(i)),s);
			MD_Ele->SetValue(s);


			// Location
			GetRegionLocatorDescriptorInterface(i)->ExportDDL(PT_Ele);

			// Parameters
			GetTemporalInterpolationDescriptorInterface(i,0)->ExportDDL(PT_Ele);

			// Depth
			if (GetDepthFlag(i)==1) {
				GetTemporalInterpolationDescriptorInterface(i,MAXNRefrence*MAXNVertices+1)->ExportDDL(PT_Ele);
			}

		} else {	// MediaTime

			char s[256];
			GenericDS T_Ele0=DS_Ele->CreateChild("MediaTime");
			GenericDSInterfaceABC *T_Ele=T_Ele0.GetInterface();

			GenericDS MT_Ele0=T_Ele->CreateChild("MediaTimePoint");
			GenericDSInterfaceABC *MT_Ele=MT_Ele0.GetInterface();
			GetMediaTimePointString(i,s);
			MT_Ele->SetValue(s);

			GenericDS MD_Ele0=T_Ele->CreateChild("MediaDuration");
			GenericDSInterfaceABC *MD_Ele=MD_Ele0.GetInterface();
			GetMediaDurationString((unsigned long)(GetMediaDuration(i)),s);
			MD_Ele->SetValue(s);

		}
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorDescriptor::ImportDDL(GenericDSInterfaceABC *theDescription)
{
	// Get the XML DOM Node from the GenericDSInterface
	int i, j, a;
	bool b;
	string str, xsitype;
	vector<string> vstr;
	vector<GenericDS>	vDS;

	GenericDS DLLD;
	GenericDSInterfaceABC *DS_Ele=NULL;

  	if (!theDescription) return (unsigned long) -1;

	// if aDescriptor is of correct type
	if (theDescription->GetDSName() == "SpatioTemporalLocator") 
	{
		DS_Ele=theDescription;
	}
	else
	{
		DLLD = DLLD.GetNextSibling("SpatioTemporalLocator");
		while (!DLLD.isNull()) {
			if (theDescription->GetDSName() == "SpatioTemporalLocator") 
			{
				DS_Ele=theDescription;
				break;
			}
			DLLD = DLLD.GetNextSibling("SpatioTemporalLocator");
		}
		// DummyType not found
		if (DLLD.isNull()) { return (unsigned long)-1; }
		// DummyType found
		DS_Ele = DLLD.GetInterface();
	}


	SetCoordFlag(0);
	SetRef(0);
	SetspatialRef(0);

	int NRef;
	vDS=DS_Ele->GetAllDescriptions("FigureTrajectory");
	NRef = vDS.size();
	vDS=DS_Ele->GetAllDescriptions("ParameterTrajectory");
	NRef += vDS.size();
	vDS=DS_Ele->GetAllDescriptions("MediaTime");
	NRef += vDS.size();
	
	GenericDS T_Ele0=DS_Ele->GetFirstChild();
	GenericDSInterfaceABC *T_Ele=T_Ele0.GetInterface();

	SetNumberOfRefernceRegions(NRef);

	if( T_Ele->GetName()=="CoordRef" ){
		SetCoordFlag(1);
		T_Ele->GetIntAttribute("ref", (int&)m_CoordFlag);
		T_Ele->GetBoolAttribute("spatialRef", b);
		SetCoordFlag(1);
		if( b ){ SetspatialRef(1); } 
		else { SetspatialRef(0); }

		T_Ele0 = T_Ele->GetNextSibling();
		T_Ele=T_Ele0.GetInterface();
	}

	for (i=0; i<NRef; i++) {

		if (T_Ele->GetDSName()=="FigureTrajectory") {		//  FigureTrajectory

			T_Ele->GetTextAttribute("type", str);
			if( str=="rectangle" ){
				Settype(i, 1);
			} else if( str=="ellipse" ){
				Settype(i, 2);
			} else if( str=="polygon" ){
				vDS = T_Ele->GetAllDescriptions("Descriptor");
				Settype(i, vDS.size()-1 );	// size=vertex+depth
			} else {
				SetTypeOfTrajectory(i,0);
			}

			// MediaTime
			GenericDS C_Ele0=T_Ele->GetFirstChild();
			GenericDSInterfaceABC *C_Ele=C_Ele0.GetInterface();

			GenericDS MT_Ele0=C_Ele->GetFirstChild();
			GenericDSInterfaceABC *MT_Ele=MT_Ele0.GetInterface();
			MT_Ele->GetTextValue(str);
			SetMediaTimePoint(i,ExtractSeconds(str));
			GenericDS MD_Ele0=MT_Ele->GetNextSibling();
			GenericDSInterfaceABC *MD_Ele=MD_Ele0.GetInterface();
			MD_Ele->GetTextValue(str);
			SetMediaDuration(i,ExtractSeconds(str));


			C_Ele0 = C_Ele->GetNextSibling();
			C_Ele=C_Ele0.GetInterface();

			// TI
		    int Nv = Gettype(i);
		    if (Nv<=2) { Nv = 3; }
		    for (j=0; j<Nv; j++) {	
				GetTemporalInterpolationDescriptorInterface(i,j)->ImportDDL(C_Ele);

				C_Ele0 = C_Ele->GetNextSibling();
				C_Ele=C_Ele0.GetInterface();
			}

			// Depth
			C_Ele->GetTextAttribute("xsi:type", xsitype );
			if (xsitype=="TemporalInterpolation") {
				SetDepthFlag(i,1);
				GetTemporalInterpolationDescriptorInterface(i,MAXNRefrence*MAXNVertices+1)->ImportDDL(C_Ele);

				C_Ele0 = C_Ele->GetNextSibling();
				C_Ele=C_Ele0.GetInterface();
			} else {
				SetDepthFlag(i,0);
			}


		} else if (T_Ele->GetDSName()=="ParameterTrajectory") {		//  ParameterTrajectory

			SetTypeOfTrajectory(i,1);
			T_Ele->GetTextAttribute("motionModel", str);
			if( str=="still" ){
				SetmotionModel(i, 0);
			} else if( str=="translation" ){
				SetmotionModel(i, 1);
			} else if( str=="scalingAndRotation" ){
				SetmotionModel(i, 2);
			} else if( str=="affine" ){
				SetmotionModel(i, 3);
			} else if( str=="perspective" ){
				SetmotionModel(i, 4);
			} else if( str=="parabolic" ){
				SetmotionModel(i, 5);
			} 

			T_Ele->GetBoolAttribute("ellipseFlag", b);
			if( b ){ SetellipseFlag(i, 1); } 
			else { SetellipseFlag(i, 0); }
			
			// MediaTime
			GenericDS C_Ele0=T_Ele->GetFirstChild();
			GenericDSInterfaceABC *C_Ele=C_Ele0.GetInterface();

			GenericDS MT_Ele0=C_Ele->GetFirstChild();
			GenericDSInterfaceABC *MT_Ele=MT_Ele0.GetInterface();
			MT_Ele->GetTextValue(str);
			SetMediaTimePoint(i,ExtractSeconds(str));
			GenericDS MD_Ele0=MT_Ele->GetNextSibling();
			GenericDSInterfaceABC *MD_Ele=MD_Ele0.GetInterface();
			MD_Ele->GetTextValue(str);
			SetMediaDuration(i,ExtractSeconds(str));

			C_Ele0 = C_Ele->GetNextSibling();
			C_Ele=C_Ele0.GetInterface();

			// InitialRegion
			GetRegionLocatorDescriptorInterface(i)->ImportDDL(C_Ele);

			C_Ele0 = C_Ele->GetNextSibling();
			C_Ele=C_Ele0.GetInterface();

			// Parameters
			if( GetmotionModel(i)>0 )
				GetTemporalInterpolationDescriptorInterface(i,0)->ImportDDL(C_Ele);

			C_Ele0 = C_Ele->GetNextSibling();
			C_Ele=C_Ele0.GetInterface();

			// Depth
			C_Ele->GetTextAttribute("xsi:type", xsitype );
			if (xsitype=="TemporalInterpolation") {	
				SetDepthFlag(i,1);
				GetTemporalInterpolationDescriptorInterface(i,MAXNRefrence*MAXNVertices+1)->ImportDDL(C_Ele);

				C_Ele0 = C_Ele->GetNextSibling();
				C_Ele=C_Ele0.GetInterface();
			} else {
				SetDepthFlag(i,0);
			}

		} else {	// MediaTime

			SetTypeOfTrajectory(i,2);
			
			// MediaTime
			GenericDS C_Ele0=T_Ele->GetFirstChild();
			GenericDSInterfaceABC *C_Ele=C_Ele0.GetInterface();

			GenericDS MT_Ele0=C_Ele->GetFirstChild();
			GenericDSInterfaceABC *MT_Ele=MT_Ele0.GetInterface();
			MT_Ele->GetTextValue(str);
			SetMediaTimePoint(i,ExtractSeconds(str));
			GenericDS MD_Ele0=MT_Ele->GetNextSibling();
			GenericDSInterfaceABC *MD_Ele=MD_Ele0.GetInterface();
			MD_Ele->GetTextValue(str);
			SetMediaDuration(i,ExtractSeconds(str));

		}

		T_Ele0 = T_Ele->GetNextSibling();
		T_Ele=T_Ele0.GetInterface();
	}

	return 0;
}
