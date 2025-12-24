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
// Applicable File Name:  TemporalInterpolation.cpp
//

#include <cstring>

#include "Descriptors/Descriptors.h"                                    /* Naming Convention */

using namespace XM;

const UUID TemporalInterpolationDescriptorInterface::myID = UUID("");
const char *TemporalInterpolationDescriptorInterface::myName = "TemporalInterpolationDescriptorInterface";

const UUID TemporalInterpolationDescriptor::myID = UUID("");
const char *TemporalInterpolationDescriptor::myName = "TemporalInterpolationDescriptor";

const UUID TemporalInterpolationDescriptor::valID = UUID("");
const char *TemporalInterpolationDescriptor::valName = "TemporalInterpolation";

const UUID TemporalInterpolationDescriptorInterfaceABC::myID = UUID();


//=============================================================================
TemporalInterpolationDescriptorInterfaceABC::
TemporalInterpolationDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TemporalInterpolationDescriptorInterface::
TemporalInterpolationDescriptorInterface(TemporalInterpolationDescriptor
					 *aTemporalInterpolation):
m_TemporalInterpolationDescriptor(aTemporalInterpolation)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterface::
~TemporalInterpolationDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void TemporalInterpolationDescriptorInterface::addref(void)
{ m_TemporalInterpolationDescriptor->addref(); }

//----------------------------------------------------------------------------
void TemporalInterpolationDescriptorInterface::release(void)
{ m_TemporalInterpolationDescriptor->release(); }

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::GetNumOfKeyPoint() {
	return m_TemporalInterpolationDescriptor->GetNumOfKeyPoint();   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
SetNumOfKeyPoint(unsigned long points)
{
	return m_TemporalInterpolationDescriptor->SetNumOfKeyPoint(points);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::GetDimension() {
	return m_TemporalInterpolationDescriptor->GetDimension();   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
SetDimension(unsigned long dim) {
	return m_TemporalInterpolationDescriptor->SetDimension(dim);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
GetConstantTimeInterval() {
	return m_TemporalInterpolationDescriptor->GetConstantTimeInterval();   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
SetConstantTimeInterval(unsigned long inter) {
	return m_TemporalInterpolationDescriptor->SetConstantTimeInterval(inter);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
GetWholeIntervalDataType() {
	return m_TemporalInterpolationDescriptor->GetWholeIntervalDataType();   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
SetWholeIntervalDataType(unsigned long inter) {
	return m_TemporalInterpolationDescriptor->SetWholeIntervalDataType(inter);   
}

//----------------------------------------------------------------------------
double  TemporalInterpolationDescriptorInterface::GetMediaDuration() {
	return m_TemporalInterpolationDescriptor->GetMediaDuration();   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
SetMediaDuration(double dur) {
	return m_TemporalInterpolationDescriptor->SetMediaDuration(dur);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
GetMediaTimePointDataType()
{
  return m_TemporalInterpolationDescriptor->GetMediaTimePointDataType();   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::SetMediaTimePointDataType(unsigned long type) {
	return m_TemporalInterpolationDescriptor->SetMediaTimePointDataType(type);   
}

//----------------------------------------------------------------------------
double TemporalInterpolationDescriptorInterface::GetMediaTimePoint(unsigned long m) {
	return m_TemporalInterpolationDescriptor->GetMediaTimePoint(m);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::SetMediaTimePoint(unsigned long m ,double pointT) {
	return m_TemporalInterpolationDescriptor->SetMediaTimePoint(m ,pointT);   
}

//----------------------------------------------------------------------------
double TemporalInterpolationDescriptorInterface::GetKeyValue(unsigned long d,unsigned long n) {
	return m_TemporalInterpolationDescriptor->GetKeyValue(d, n);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::SetKeyValue(unsigned long d,unsigned long n,double point) {
	return m_TemporalInterpolationDescriptor->SetKeyValue(d, n, point);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
GetDefaultFunction(unsigned long d) {
	return m_TemporalInterpolationDescriptor->GetDefaultFunction(d);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::
SetDefaultFunction(unsigned long d, unsigned long def) {
	return m_TemporalInterpolationDescriptor->SetDefaultFunction(d, def);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::GetType(unsigned long d,unsigned long m) {
	return m_TemporalInterpolationDescriptor->GetType(d,m);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::SetType(unsigned long d, unsigned long n, unsigned long id) {
	return m_TemporalInterpolationDescriptor->SetType(d, n, id);   
}

//----------------------------------------------------------------------------
double TemporalInterpolationDescriptorInterface::GetParam(unsigned long d, unsigned long n) {
	return m_TemporalInterpolationDescriptor->GetParam(d,n);   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptorInterface::SetParam(unsigned long d, unsigned long n,double value) {
	return m_TemporalInterpolationDescriptor->SetParam(d, n, value);   
}



//----------------------------------------------------------------------------
const UUID& TemporalInterpolationDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TemporalInterpolationDescriptorInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void TemporalInterpolationDescriptorInterface::SetValuesFromParameterFile(void)
{
}


//----------------------------------------------------------------------------
unsigned long TemporalInterpolationDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *theParent)
{
  return m_TemporalInterpolationDescriptor->ExportDDL(theParent);
}
//----------------------------------------------------------------------------
unsigned long TemporalInterpolationDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *theParent)
{
  return m_TemporalInterpolationDescriptor->ImportDDL(theParent);
}


//=============================================================================
TemporalInterpolationDescriptor::TemporalInterpolationDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_NumOfKeyPoint(0),
m_Dimension(0),
m_ConstantTimeInterval(0),
m_WholeIntervalDataType(0),
m_MediaDuration(0)
{
	SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
TemporalInterpolationDescriptor::~TemporalInterpolationDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"TemporalInterpolation delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TemporalInterpolationDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TemporalInterpolationDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TemporalInterpolationDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void TemporalInterpolationDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"TemporalInterpolationRegion connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void TemporalInterpolationDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"TemporalInterpolationRegion release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC
*TemporalInterpolationDescriptor::GetInterface(void)
{
	return &m_Interface; 
}


//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetNumOfKeyPoint() {
	return m_NumOfKeyPoint;   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetNumOfKeyPoint(unsigned long points) {
	m_NumOfKeyPoint = points;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetDimension() {
	return m_Dimension;   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::
SetDimension(unsigned long dim) {
	m_Dimension = dim;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetConstantTimeInterval() {
	return m_ConstantTimeInterval;   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::
SetConstantTimeInterval(unsigned long inter) {
	m_ConstantTimeInterval = inter;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetWholeIntervalDataType() {
	return m_WholeIntervalDataType;   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::
SetWholeIntervalDataType(unsigned long inter) {
	m_WholeIntervalDataType = inter;   
	return 0;
}

//----------------------------------------------------------------------------
double  TemporalInterpolationDescriptor::GetMediaDuration() {
	return m_MediaDuration;   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetMediaDuration(double dur)
{
	m_MediaDuration = dur;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetMediaTimePointDataType() {
	return m_MediaTimePointDataType;   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetMediaTimePointDataType(unsigned long type) {
	m_MediaTimePointDataType = type;   
	return 0;
}

//----------------------------------------------------------------------------
double TemporalInterpolationDescriptor::GetMediaTimePoint(unsigned long n) {
	if( m_ConstantTimeInterval==0 ){
		return m_MediaTimePoint[n];   
	} else {
		return (float)m_MediaDuration/(float)(m_NumOfKeyPoint-1)*(float)n;
	}
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetMediaTimePoint(unsigned long n ,double pointT) {
	m_MediaTimePoint[n] = pointT;   
	return 0;
}

//----------------------------------------------------------------------------
double TemporalInterpolationDescriptor::GetKeyValue(unsigned long d, unsigned long n) {
	return m_KeyValue[n*16+d];   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetKeyValue(unsigned long d,unsigned long n,double point) {
	m_KeyValue[n*16+d] = point;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::
GetDefaultFunction(unsigned long d) {
	return m_DefaultFunction[d];   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::
SetDefaultFunction(unsigned long d, unsigned long def) {
	m_DefaultFunction[d] = def;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetType(unsigned long d, unsigned long n) {
	return m_Type[n*16+d];   
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetType(unsigned long d, unsigned long n,unsigned long id) {
	m_Type[n*16+d] = id;   
	return 0;
}

//----------------------------------------------------------------------------
double TemporalInterpolationDescriptor::GetParam(unsigned long d, unsigned long n) {
	if( m_Type[n*16+d]!=2 || m_DefaultFunction[d]==1 ) return 0.0;
	else return m_Param[n*16+d];
	return m_Param[n*16+d];	
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::SetParam(unsigned long d, unsigned long n,double value) {
	m_Param[n*16+d] = value;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetMediaDurationString(char* s) {
	unsigned long ul = GetMediaDuration();
	if(ExtractHours(ul)){
		sprintf(s,"PT%ldH%ldM%ldS", ExtractHours(ul), ExtractMinutes(ul), ExtractSeconds(ul) );
	} else if(ExtractMinutes(ul)){
		sprintf(s,"PT%ldM%ldS", ExtractMinutes(ul), ExtractSeconds(ul) );
	} else if(ExtractMinutes(ul)){
		sprintf(s,"PT%ldS", ExtractSeconds(ul) );
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::GetMediaTimePointString(int i,char* s) {
	unsigned long ul = (unsigned long)GetMediaTimePoint(i);
	sprintf(s,"T%02ld:%02ld:%02ld", ExtractHours(ul), ExtractMinutes(ul),
		ExtractSeconds(ul) );
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::ExtractHours(unsigned long second) {
	unsigned long hrs=second/3600;
	return hrs%24;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::ExtractMinutes(unsigned long second) {
	unsigned long mts=second/60;
	return mts%60;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::ExtractSeconds(unsigned long second) {
	return second%60;
}

//----------------------------------------------------------------------------
unsigned long  TemporalInterpolationDescriptor::ExtractSeconds(string s) {
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
unsigned long TemporalInterpolationDescriptor::
ExportDDL(GenericDSInterfaceABC *theParent)
{
	// Get the XML DOM Node from the GenericDSInterface
	unsigned long a, Nkey, Dim;
	double d;
	int i, j;
	char s[256];

	GenericDS DS_Ele0=theParent->CreateChild("Descriptor");
	GenericDSInterfaceABC *DS_Ele=DS_Ele0.GetInterface();

	DS_Ele->SetAttribute("xsi:type","TemporalInterpolation");

	Nkey = GetNumOfKeyPoint();

	a = GetConstantTimeInterval();
	if (a==1) {			// WholeInterval
		GenericDS WI_Ele0=DS_Ele->CreateChild("WholeInterval");
		GenericDSInterfaceABC *WI_Ele=WI_Ele0.GetInterface();
	a = GetWholeIntervalDataType();
		if(a==0){
			GenericDS Inter_Ele0=WI_Ele->CreateChild("MediaDuration");
			GenericDSInterfaceABC *Inter_Ele=Inter_Ele0.GetInterface();
//			d = GetMediaDuration();
//			Inter_Ele->SetValue(d);
			GetMediaDurationString(s);			
			Inter_Ele->SetValue(s);
		} else {
			GenericDS Inter_Ele0=WI_Ele->CreateChild("MediaIncrDuration");
			GenericDSInterfaceABC *Inter_Ele=Inter_Ele0.GetInterface();
//			d = GetMediaDuration();
//			Inter_Ele->SetValue(d);
			GetMediaDurationString(s);			
			Inter_Ele->SetValue(s);
		}
	} else {				// KeyTimePoint
		GenericDS KT_Ele0=DS_Ele->CreateChild("KeyTimePoint");
		GenericDSInterfaceABC *KT_Ele=KT_Ele0.GetInterface();
		for (i=0; i<Nkey; i++) {
			GenericDS PointT_Ele0=KT_Ele->CreateChild("MediaTimePoint");
			GenericDSInterfaceABC *PointT_Ele=PointT_Ele0.GetInterface();
//			d = GetMediaTimePoint(i);
//			PointT_Ele->SetValue(d);			
			GetMediaTimePointString(i,s);
			PointT_Ele->SetValue(s);			
		}
	}

	Dim = GetDimension();

	for (i=0; i<Dim; i++) {
		GenericDS Func_Ele0=DS_Ele->CreateChild("InterpolationFunctions");
		GenericDSInterfaceABC *Func_Ele=Func_Ele0.GetInterface();
		if( GetDefaultFunction(i)==0 ){		// not default functions

			for (j=0; j<Nkey; j++) {	// KeyValue
				GenericDS Point_Ele0=Func_Ele->CreateChild("KeyValue");
				GenericDSInterfaceABC *Point_Ele=Point_Ele0.GetInterface();
				d = GetKeyValue(i,j);
				Point_Ele->SetValue(d);	
				
				if (j==0) { Point_Ele->SetAttribute("type", "startPoint"); }
				else if (GetDefaultFunction(i)==1) { Point_Ele->SetAttribute("type", "firstOrder"); }
				else if (GetType(i,j-1)==1) { Point_Ele->SetAttribute("type", "firstOrder"); }
				else {	
					Point_Ele->SetAttribute("type", "secondOrder"); 
					Point_Ele->SetAttribute("param", GetParam(i,j-1));
				}
			}

		} else {		// default functions

			for (j=0; j<Nkey; j++) {	// KeyValue
				GenericDS Point_Ele0=Func_Ele->CreateChild("RestrictedKeyValue");
				GenericDSInterfaceABC *Point_Ele=Point_Ele0.GetInterface();
				d = GetKeyValue(i,j);
				Point_Ele->SetValue(d);	
			}

		}
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long TemporalInterpolationDescriptor::
ImportDDL(GenericDSInterfaceABC *theDescription)
{
	// Get the XML DOM Node from the GenericDSInterface
	int Nkey, Dim;
	double d;
	int i, j;
//	int c;
//	char s[256];
	string str, xsitype;
	GenericDS Child;
	vector<GenericDS>	vDS, vDSc;

	GenericDS DLLD;
	GenericDSInterfaceABC *DS_Ele=NULL;

  	if (!theDescription) return (unsigned long) -1;

	// if aDescriptor is of correct type
	if (theDescription->GetDSName() == "Descriptor") {
		theDescription->GetTextAttribute("xsi:type",xsitype);
		if (xsitype == "TemporalInterpolation") {
			DS_Ele=theDescription;
		}
	}

	// else search for DummyType as a child
	if (!DS_Ele) {
		DLLD=theDescription->GetDescription("Descriptor");

		// search for correct xsi type
		while (!DLLD.isNull()) {
			DLLD.GetTextAttribute("xsi:type", xsitype);
			if(xsitype=="TemporalInterpolation") break;
			DLLD = DLLD.GetNextSibling("Descriptor");
		}

		// DummyType not found
		if (DLLD.isNull()) { return (unsigned long)-1; }
		// DummyType found
		DS_Ele = DLLD.GetInterface();
	}

	Child = DS_Ele->GetFirstChild();
	str = Child.GetDSName();
	if( str=="WholeInterval" ){	// WholeInterval
		Nkey = 0;
		vDS=Child.GetAllDescriptions("MediaDuration");
		if (vDS.size()>0) {
//			vDS[0].GetRealValue(d);
			vDS[0].GetTextValue(str);
			d = ExtractSeconds(str);
			SetMediaDuration(d);
		}
		vDS=Child.GetAllDescriptions("MediaIncrDuration");
		if (vDS.size()>0) {
//			vDS[0].GetRealValue(d);
			vDS[0].GetTextValue(str);
			d = ExtractSeconds(str);
			SetMediaDuration(d);
		}
		SetConstantTimeInterval(1);
	} else if( str=="KeyTimePoint" ){	// MediaTimePoint
		vDS=Child.GetAllDescriptions("MediaTimePoint");
		if( vDS.size()>0 ){
			for (i=0; i<vDS.size(); i++) {
//				vDS[i].GetRealValue(d);
				vDS[i].GetTextValue(str);
				d = ExtractSeconds(str);
				SetMediaTimePoint(i,d);
			}
			Nkey = vDS.size();
			SetNumOfKeyPoint(Nkey);
		}
		vDS=Child.GetAllDescriptions("MediaRelTimePoint");
		if( vDS.size()>0 ){
			for (i=0; i<vDS.size(); i++) {
//				vDS[i].GetRealValue(d);
				vDS[i].GetTextValue(str);
				d = ExtractSeconds(str);
				SetMediaTimePoint(i,d);
			}
			Nkey = vDS.size();
			SetNumOfKeyPoint(Nkey);
		}
		vDS=Child.GetAllDescriptions("MediaRelIncrTimePoint");
		if( vDS.size()>0 ){
			for (i=0; i<vDS.size(); i++) {
//				vDS[i].GetRealValue(d);
				vDS[i].GetTextValue(str);
				d = ExtractSeconds(str);
				SetMediaTimePoint(i,d);
			}
			Nkey = vDS.size();
			SetNumOfKeyPoint(Nkey);
		}
	} else {
		return 1;
	}

	vDS = DS_Ele->GetAllDescriptions( "InterpolationFunctions" );
	if(vDS.size()>0){
		m_Dimension = Dim = vDS.size();
	} else {
		return 1;
	}

	// InterpolationFunctions
	for (i=0; i<Dim; i++) {
		vDSc=vDS[i].GetAllDescriptions("KeyValue");
		if( vDSc.size()>0 ){	// KeyValue

			if( Nkey == 0 ){
				Nkey = vDSc.size();
				SetNumOfKeyPoint(Nkey);
			}
			
			SetDefaultFunction(i, 0);	
			for( j=0; j<Nkey; j++ ) {
				vDSc[j].GetRealValue(d);			
				SetKeyValue(i,j,d);	
				
				string str;
				vDSc[j].GetTextAttribute("type", str);
				if (j==0) { SetType(i,0,0); }
				else if (str=="firstOrder") { SetType(i,j-1,1);}
				else { 
					SetType(i,j-1,2); 
					vDSc[j].GetRealAttribute("param", d);
					SetParam(i,j-1,d);
				}
			}

		}
		
		vDSc=vDS[i].GetAllDescriptions("RestrictedKeyValue");
		if( vDSc.size()>0 ){	// RestrictedKeyValue

			if( Nkey == 0 ){
				Nkey = vDSc.size();
				SetNumOfKeyPoint(Nkey);
			}
			
			SetDefaultFunction(i, 1);	
			for( j=0; j<Nkey; j++ ) {
				vDSc[j].GetRealValue(d);			
				SetKeyValue(i,j,d);	
				
				if (j==0) { SetType(i,0,0); }
				else { SetType(i,j-1,1); }
			}

		}

	}

	return 0;
}

