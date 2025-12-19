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
// Applicable File Name:  Spatial2DCoordinates.cpp
//

#include <cstring>
#include "Descriptors/Descriptors.h"                                    /* Naming Convention */

using namespace XM;

const UUID Spatial2DCoordinatesDescriptorInterface::myID = UUID("");
const char *Spatial2DCoordinatesDescriptorInterface::myName = "Spatial2DCoordinatesDescriptorInterface";

const UUID Spatial2DCoordinatesDescriptor::myID = UUID("");
const char *Spatial2DCoordinatesDescriptor::myName = "Spatial2DCoordinatesDescriptor";

const UUID Spatial2DCoordinatesDescriptor::valID = UUID("");
const char *Spatial2DCoordinatesDescriptor::valName = "Spatial2DCoordinates";

const UUID Spatial2DCoordinatesDescriptorInterfaceABC::myID = UUID();


//=============================================================================
Spatial2DCoordinatesDescriptorInterfaceABC::Spatial2DCoordinatesDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
Spatial2DCoordinatesDescriptorInterface::Spatial2DCoordinatesDescriptorInterface(Spatial2DCoordinatesDescriptor *aSpatial2DCoordinates):
m_Spatial2DCoordinatesDescriptor(aSpatial2DCoordinates)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesDescriptorInterface::~Spatial2DCoordinatesDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void Spatial2DCoordinatesDescriptorInterface::addref(void)
{ m_Spatial2DCoordinatesDescriptor->addref(); }

//----------------------------------------------------------------------------
void Spatial2DCoordinatesDescriptorInterface::release(void)
{ m_Spatial2DCoordinatesDescriptor->release(); }


//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptorInterface::Getid() {  
	return m_Spatial2DCoordinatesDescriptor->Getid();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::Setid(char* id) {  
	return m_Spatial2DCoordinatesDescriptor->Setid(id) ;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetxRepr() {
	return m_Spatial2DCoordinatesDescriptor->GetxRepr();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetxRepr(unsigned long r) {
	return m_Spatial2DCoordinatesDescriptor->SetxRepr(r);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetyRepr() {
	return m_Spatial2DCoordinatesDescriptor->GetyRepr();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetyRepr(unsigned long r) {
	return m_Spatial2DCoordinatesDescriptor->SetyRepr(r);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetxSrcSize() {
	return m_Spatial2DCoordinatesDescriptor->GetxSrcSize();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetxSrcSize(unsigned long r) {
	return m_Spatial2DCoordinatesDescriptor->SetxSrcSize(r);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetySrcSize() {
	return m_Spatial2DCoordinatesDescriptor->GetySrcSize();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetySrcSize(unsigned long r) {
	return m_Spatial2DCoordinatesDescriptor->SetySrcSize(r);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetUnitDefined() {
	return m_Spatial2DCoordinatesDescriptor->GetUnitDefined();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetUnitDefined(unsigned long UnitDef) {
	return m_Spatial2DCoordinatesDescriptor->SetUnitDefined(UnitDef);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetUnit() {
	return m_Spatial2DCoordinatesDescriptor->GetUnit();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetUnit(unsigned long Unit) {
	return m_Spatial2DCoordinatesDescriptor->SetUnit(Unit);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetLocalCoordinatesDefined() { 
	return m_Spatial2DCoordinatesDescriptor->GetLocalCoordinatesDefined();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetLocalCoordinatesDefined(unsigned long LocalDef) {  
	return m_Spatial2DCoordinatesDescriptor->SetLocalCoordinatesDefined(LocalDef);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetNameLength() {  
	return m_Spatial2DCoordinatesDescriptor->GetNameLength();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetNameLength(unsigned long NameL) {  
	return m_Spatial2DCoordinatesDescriptor->SetNameLength(NameL);   
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptorInterface::Getname() {  
	return m_Spatial2DCoordinatesDescriptor->Getname();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::Setname(char* Name) {  
	return m_Spatial2DCoordinatesDescriptor->Setname(Name) ;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetDataSetDefined() {  
	return m_Spatial2DCoordinatesDescriptor->GetDataSetDefined();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetDataSetDefined(unsigned long DataSetDef) {  
	return m_Spatial2DCoordinatesDescriptor->SetDataSetDefined(DataSetDef);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetDataSetLength() {  
	return m_Spatial2DCoordinatesDescriptor->GetDataSetLength();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetDataSetLength(unsigned long DataSetL) {  
	return m_Spatial2DCoordinatesDescriptor->SetDataSetLength(DataSetL);   
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptorInterface::GetdataSet() {  
	return m_Spatial2DCoordinatesDescriptor->GetdataSet();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetdataSet(char* DataSet) {  
	return m_Spatial2DCoordinatesDescriptor->SetdataSet(DataSet);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::GetNumOfPixels() {
	return m_Spatial2DCoordinatesDescriptor->GetNumOfPixels();   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetNumOfPixels(unsigned long NumOfPixels) {
	return m_Spatial2DCoordinatesDescriptor->SetNumOfPixels(NumOfPixels);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetPixelX(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetPixelX(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetPixelX(unsigned long n,unsigned long x) {
	return m_Spatial2DCoordinatesDescriptor->SetPixelX(n, x);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetPixelY(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetPixelY(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetPixelY(unsigned long n,unsigned long y) {
	return m_Spatial2DCoordinatesDescriptor->SetPixelY(n, y) ;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::GetCoord(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetCoord(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetCoord(unsigned long n,unsigned long src) {
	return m_Spatial2DCoordinatesDescriptor->SetCoord(n, src);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetsrcPixelX(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetsrcPixelX(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetsrcPixelX(unsigned long n,unsigned long x) {
	return m_Spatial2DCoordinatesDescriptor->SetsrcPixelX(n, x);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetsrcPixelY(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetsrcPixelY(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetsrcPixelY(unsigned long n,unsigned long y) {
	return m_Spatial2DCoordinatesDescriptor->SetsrcPixelY(n, y);   
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptorInterface::GetCoordPointX(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetCoordPointX(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetCoordPointX(unsigned long n,double x) {
	return m_Spatial2DCoordinatesDescriptor->SetCoordPointX(n, x);   
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptorInterface::GetCoordPointY(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetCoordPointY(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetCoordPointY(unsigned long n,double y) {
	return m_Spatial2DCoordinatesDescriptor->SetCoordPointY(n, y);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetNumOfMappingFuncts() {
	return m_Spatial2DCoordinatesDescriptor->GetNumOfMappingFuncts();   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetNumOfMappingFuncts(unsigned long map) {
	return m_Spatial2DCoordinatesDescriptor->SetNumOfMappingFuncts(map);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::GetMappingFunctLength(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetMappingFunctLength(n);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetMappingFunctLength(unsigned long n,unsigned long FuncLen) {
	return m_Spatial2DCoordinatesDescriptor->SetMappingFunctLength(n, FuncLen);   
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptorInterface::GetMappingFunct(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetMappingFunct(n);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::SetMappingFunct(unsigned long n,char* Func) {
	return m_Spatial2DCoordinatesDescriptor->SetMappingFunct(n, Func);   
}


//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::GetIntegratedCoordinatesDefined() {
	return m_Spatial2DCoordinatesDescriptor->GetIntegratedCoordinatesDefined();   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetIntegratedCoordinatesDefined(unsigned long Inte) {
	return m_Spatial2DCoordinatesDescriptor->SetIntegratedCoordinatesDefined(Inte);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::GetmodelType() {
	return m_Spatial2DCoordinatesDescriptor->GetmodelType();   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetmodelType(unsigned long Type)  {
	return m_Spatial2DCoordinatesDescriptor->SetmodelType(Type) ;   
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptorInterface::GetxOrigin() {
	return m_Spatial2DCoordinatesDescriptor->GetxOrigin();   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetxOrigin(double x) {
	return m_Spatial2DCoordinatesDescriptor->SetxOrigin(x);   
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptorInterface::GetyOrigin() {
	return m_Spatial2DCoordinatesDescriptor->GetyOrigin();   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetyOrigin(double y) {
	return m_Spatial2DCoordinatesDescriptor->SetyOrigin(y);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::GetnumOfMotionParameterSets() {
	return m_Spatial2DCoordinatesDescriptor->GetnumOfMotionParameterSets();   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetnumOfMotionParameterSets(unsigned long No) {
	return m_Spatial2DCoordinatesDescriptor->SetnumOfMotionParameterSets(No);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::GetTimeIncr(unsigned long n) {
	return m_Spatial2DCoordinatesDescriptor->GetTimeIncr(n);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetTimeIncr(unsigned long n,unsigned long Time) {
	return m_Spatial2DCoordinatesDescriptor->SetTimeIncr(n,Time);   
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptorInterface::GetMotionParameters(unsigned long NSet,unsigned long Type) {
	return m_Spatial2DCoordinatesDescriptor->GetMotionParameters(NSet,Type);   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptorInterface::SetMotionParameters(unsigned long NSet,unsigned long Type,double Para) {
	return m_Spatial2DCoordinatesDescriptor->SetMotionParameters(NSet, Type, Para);   
}

//----------------------------------------------------------------------------
const UUID& Spatial2DCoordinatesDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Spatial2DCoordinatesDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void Spatial2DCoordinatesDescriptorInterface::SetValuesFromParameterFile(void)
{
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::ExportDDL(GenericDSInterfaceABC *theParent)
{
  return m_Spatial2DCoordinatesDescriptor->ExportDDL(theParent);
}
//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptorInterface::ImportDDL(GenericDSInterfaceABC *theParent)
{
  return m_Spatial2DCoordinatesDescriptor->ImportDDL(theParent);
}


//=============================================================================
Spatial2DCoordinatesDescriptor::Spatial2DCoordinatesDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_UnitDefined(0),
m_LocalCoordinatesDefined(0),
m_IntegratedCoordinatesDefined(0)
{

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesDescriptor::~Spatial2DCoordinatesDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"Spatial2DCoordinates delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& Spatial2DCoordinatesDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Spatial2DCoordinatesDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& Spatial2DCoordinatesDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* Spatial2DCoordinatesDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool Spatial2DCoordinatesDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void Spatial2DCoordinatesDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"Spatial2DCoordinatesRegion connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void Spatial2DCoordinatesDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"Spatial2DCoordinatesRegion release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesDescriptorInterfaceABC *Spatial2DCoordinatesDescriptor::GetInterface(void)
{
	return &m_Interface; 
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptor::Getid() {  
	return m_id;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::Setid(char* id) {  
	strcpy(m_id,id) ;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetxRepr() {
	return m_xRepr;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetxRepr(unsigned long xRepr) {
	m_xRepr = xRepr;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetyRepr() {
	return m_yRepr;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetyRepr(unsigned long yRepr) {
	m_yRepr = yRepr;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetxSrcSize() {
	return m_xSrcSize;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetxSrcSize(unsigned long x) {
	m_xSrcSize = x;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetySrcSize() {
	return m_ySrcSize;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetySrcSize(unsigned long y) {
	m_ySrcSize = y;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetUnitDefined() {
	return m_UnitDefined;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetUnitDefined(unsigned long UnitDef) {
	m_UnitDefined = UnitDef;   
	return 0;
}


//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetUnit() {
	return m_Unit;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetUnit(unsigned long Unit) {
	m_Unit = Unit;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetLocalCoordinatesDefined() { 
	return m_LocalCoordinatesDefined;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetLocalCoordinatesDefined(unsigned long LocalDef) {  
	m_LocalCoordinatesDefined = LocalDef;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetNameLength() {  
	return m_NameLength;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetNameLength(unsigned long NameL) {  
	m_NameLength = NameL;   
	return 0;
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptor::Getname() {  
	return m_name;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::Setname(char* Name) {  
	strcpy(m_name,Name) ;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetDataSetDefined() {  
	return m_DataSetDefined;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetDataSetDefined(unsigned long DataSetDef) {  
	m_DataSetDefined = DataSetDef;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetDataSetLength() {  
	return m_DataSetLength;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetDataSetLength(unsigned long DataSetL) {  
	m_DataSetLength = DataSetL;   
	return 0;
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptor::GetdataSet() {  
	return m_dataSet;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetdataSet(char* DataSet) {  
	strcpy(m_dataSet, DataSet);   
	return 0;
}


//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::GetNumOfPixels() {
	return m_NumOfPixels;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetNumOfPixels(unsigned long NumOfPixels) {
	m_NumOfPixels = NumOfPixels;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetPixelX(unsigned long n) {
	return m_PixelX[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetPixelX(unsigned long n,unsigned long x) {
	m_PixelX[n] = x;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetPixelY(unsigned long n) {
	return m_PixelY[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetPixelY(unsigned long n,unsigned long y) {
	m_PixelY[n] = y ;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::GetCoord(unsigned long n) {
	return m_Coord[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetCoord(unsigned long n,unsigned long coord) {
	m_Coord[n] = coord;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetsrcPixelX(unsigned long n) {
	return m_srcPixelX[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetsrcPixelX(unsigned long n,unsigned long x) {
	m_srcPixelX[n] = x;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetsrcPixelY(unsigned long n) {
	return m_srcPixelY[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetsrcPixelY(unsigned long n,unsigned long y) {
	m_srcPixelY[n] = y;   
	return 0;
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptor::GetCoordPointX(unsigned long n) {
	return m_CoordPointX[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetCoordPointX(unsigned long n,double x) {
	m_CoordPointX[n] = x;   
	return 0;
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptor::GetCoordPointY(unsigned long n) {
	return m_CoordPointY[n];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetCoordPointY(unsigned long n,double y) {
	m_CoordPointY[n] = y;   
	return 0;
}


//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetNumOfMappingFuncts() {
	return m_NumOfMappingFuncts;   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetNumOfMappingFuncts(unsigned long map) {
	m_NumOfMappingFuncts = map;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::GetMappingFunctLength(unsigned long n) {
	return m_MappingFunctLength[n];   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetMappingFunctLength(unsigned long n,unsigned long FuncLen) {
	m_MappingFunctLength[n] = FuncLen;   
	return 0;
}

//----------------------------------------------------------------------------
char* Spatial2DCoordinatesDescriptor::GetMappingFunct(unsigned long n) {
	return m_MappingFunct+(n*256);   
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::SetMappingFunct(unsigned long n,char* Func) {
	strcpy(m_MappingFunct+(n*256), Func);   
	return 0;
}


//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::GetIntegratedCoordinatesDefined() {
	return m_IntegratedCoordinatesDefined;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetIntegratedCoordinatesDefined(unsigned long Inte) {
	m_IntegratedCoordinatesDefined = Inte;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::GetmodelType() {
	return m_modelType;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetmodelType(unsigned long Type)  {
	m_modelType = Type ;   
	return 0;
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptor::GetxOrigin() {
	return m_xOrigin;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetxOrigin(double x) {
	m_xOrigin = x;   
	return 0;
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptor::GetyOrigin() {
	return m_yOrigin;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetyOrigin(double y) {
	m_yOrigin = y;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::GetnumOfMotionParameterSets() {
	return m_numOfMotionParameterSets;   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetnumOfMotionParameterSets(unsigned long No) {
	m_numOfMotionParameterSets = No;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::GetTimeIncr(unsigned long NSet) {
	return m_TimeIncr[NSet];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetTimeIncr(unsigned long NSet,unsigned long Time) {
	m_TimeIncr[NSet] = Time;   
	return 0;
}

//----------------------------------------------------------------------------
double Spatial2DCoordinatesDescriptor::GetMotionParameters(unsigned long NSet, unsigned long Type) {
	return m_MotionParameters[NSet*16+Type];   
}

//----------------------------------------------------------------------------
unsigned long  Spatial2DCoordinatesDescriptor::SetMotionParameters(unsigned long NSet, unsigned long Type,double Para) {
	m_MotionParameters[NSet*16+Type] = Para;   
	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::ExportDDL(GenericDSInterfaceABC *theParent)
{
	// Get the XML DOM Node from the GenericDSInterface
	int k, l;
	string str;
	vector<int> vec;
	vector<double> vecD;

//	GenericDS DS_Ele0=theParent->CreateChild("Spatial2DCoordinateSystem");
	GenericDS DS_Ele0=theParent->CreateChild("Descriptor");
	GenericDSInterfaceABC *DS_Ele=DS_Ele0.GetInterface();
//	GenericDSInterfaceABC *DS_Ele=theParent;

	DS_Ele->SetAttribute("xsi:type","Spatial2DCoordinateSystem");

	str = m_id;
	DS_Ele->SetAttribute("id",str);
	DS_Ele->SetAttribute("xRepr",(int)m_xRepr);
	DS_Ele->SetAttribute("yRepr",(int)m_yRepr);
	DS_Ele->SetAttribute("xSrcSize",(int)m_xSrcSize);
	DS_Ele->SetAttribute("ySrcSize",(int)m_ySrcSize);

	// Unit
	if (m_UnitDefined != 0) {
		GenericDS Unit_Ele0=DS_Ele->CreateChild("Unit");
		GenericDSInterfaceABC *Unit_Ele=Unit_Ele0.GetInterface();
		if (m_Unit==0) { str = "pixel"; }
		else if (m_Unit==1) { str = "meter"; }
		else if (m_Unit==4) { str = "pictureHeight"; }
		else if (m_Unit==5) { str = "pictureWidth"; }
		else if (m_Unit==6) { str = "pictureWidthandHeight"; }
		else { str = ""; }
		Unit_Ele->SetValue(str);
	}

	// LocalCoordinateSystem
	if (m_LocalCoordinatesDefined != 0) {
		GenericDS Local_Ele0=DS_Ele->CreateChild("LocalCoordinateSystem");
		GenericDSInterfaceABC *Local_Ele=Local_Ele0.GetInterface();
	
		str = m_name;
		Local_Ele->SetAttribute("name",str);
		if (m_dataSet[0]!=0) {
			str = m_dataSet;
			Local_Ele->SetAttribute("dataSet",str);
		}

		for (k=0; k<m_NumOfPixels; k++) {
			// SrcPixel
			GenericDS SP_Ele0=Local_Ele->CreateChild("SrcPixel");
			GenericDSInterfaceABC *SP_Ele=SP_Ele0.GetInterface();
			vec.clear();	vec.resize(2);
			vec[0] = m_srcPixelX[k];	vec[1] = m_srcPixelY[k];
			SP_Ele->SetValue(vec);

			if (m_Coord[k] == 0) {		// Pixel
				GenericDS P_Ele0=Local_Ele->CreateChild("Pixel");
				GenericDSInterfaceABC *P_Ele=P_Ele0.GetInterface();
				vec.clear();	vec.resize(2);
				vec[0] = m_PixelX[k];	vec[1] = m_PixelY[k];
				P_Ele->SetValue(vec);

			} else {					// CoordPoint
				GenericDS P_Ele0=Local_Ele->CreateChild("CoordPoint");
				GenericDSInterfaceABC *P_Ele=P_Ele0.GetInterface();
				vecD.clear();	vecD.resize(2);
				vecD[0] = m_CoordPointX[k];		vecD[1] = m_CoordPointY[k];
				P_Ele->SetValue(vecD);

			}
		}
		// MappingFunct
		for (k=0; k<m_NumOfMappingFuncts; k++) {
			GenericDS M_Ele0=Local_Ele->CreateChild("MappingFunct");
			GenericDSInterfaceABC *M_Ele=M_Ele0.GetInterface();
			M_Ele->SetAttribute("dimension",(int)m_MappingFunctLength[k]);
			str = m_MappingFunct+(k*256);
			M_Ele->SetValue(str);
		}
		
	}	// End of LocalCoordinateSystem

	// IntegrateCoordinateSystem
	if (m_IntegratedCoordinatesDefined != 0) {
		GenericDS Int_Ele0=DS_Ele->CreateChild("IntegrateCoordinateSystem");
		GenericDSInterfaceABC *Int_Ele=Int_Ele0.GetInterface();
		
		Int_Ele->SetAttribute("modelType",(int)m_modelType);
		Int_Ele->SetAttribute("xOrigin",(int)m_xOrigin);
		Int_Ele->SetAttribute("yOrigin",(int)m_yOrigin);
		Int_Ele->SetAttribute("numOfMotionParameterSets",(int)m_numOfMotionParameterSets);
		
		for (k=0; k<m_numOfMotionParameterSets; k++) {
			GenericDS Time_Ele0=Int_Ele->CreateChild("TimeIncr");
			GenericDSInterfaceABC *Time_Ele=Time_Ele0.GetInterface();
			Time_Ele->SetValue((int)m_TimeIncr[k]);
			
			for (l=0; l<m_modelType; l++) {
				GenericDS P_Ele0=Int_Ele->CreateChild("MotionParameters");
				GenericDSInterfaceABC *P_Ele=P_Ele0.GetInterface();			
				P_Ele->SetValue(m_MotionParameters[k*16+l]);

			}
		}

	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long Spatial2DCoordinatesDescriptor::ImportDDL(GenericDSInterfaceABC *theDescription)
{
	// Get the XML DOM Node from the GenericDSInterface
	int k, l;
	string str, xsitype;
	vector<int> vec;
	vector<double> vecD;
	vector<GenericDS> vDS;

//	GenericDSInterfaceABC *DS_Ele=theDescription;
	GenericDS DLLD;
	GenericDSInterfaceABC *DS_Ele=NULL;

	if (!theDescription) { return (unsigned long) -1; }
// 	if (theDescription->GetDSName() != "Spatial2DCoordinateSystem") return (unsigned long) -1;

	// if aDescriptor is of correct type
	if (theDescription->GetDSName() == "Descriptor") {
		theDescription->GetTextAttribute("xsi:type",xsitype);
		if (xsitype == "Spatial2DCoordinateSystem") {
			DS_Ele=theDescription;
		}
	}

	// else search for DummyType as a child
	if (!DS_Ele) {
		DLLD=theDescription->GetDescription("Descriptor");

		// search for correct xsi type
		while (!DLLD.isNull()) {
			DLLD.GetTextAttribute("xsi:type", xsitype);
			if(xsitype=="Spatial2DCoordinateSystem") break;
			DLLD = DLLD.GetNextSibling("Descriptor");
		}

		// DummyType not found
		if (DLLD.isNull()) { return (unsigned long)-1; }
		// DummyType found
		DS_Ele = DLLD.GetInterface();
	}


	DS_Ele->GetTextAttribute("id", str);
	strcpy(m_id, str.c_str());
	DS_Ele->GetIntAttribute("xRepr",(int&)m_xRepr);
	DS_Ele->GetIntAttribute("yRepr",(int&)m_yRepr);
	DS_Ele->GetIntAttribute("xSrcSize",(int&)m_xSrcSize);
	DS_Ele->GetIntAttribute("ySrcSize",(int&)m_ySrcSize);

	// Unit
	GenericDS C_Ele0=DS_Ele->GetFirstChild();
	GenericDSInterfaceABC *C_Ele=C_Ele0.GetInterface();
	str = C_Ele->GetDSName();

	// Unit
	if (str.compare("Unit") == 0) {		
		C_Ele->GetTextValue(str);
		if (str=="pixel") { m_Unit=0; }
		else if (str == "meter" ) { m_Unit=1; }
		else if (str == "pictureHeight" ) { m_Unit=4; }
		else if (str == "pictureWidth" ) { m_Unit=5; }
		else if (str == "pictureWidthandHeight" ) { m_Unit=6; }
		else { m_Unit=0; }

		C_Ele0=C_Ele->GetNextSibling();
		C_Ele=C_Ele0.GetInterface();
		str = C_Ele->GetDSName();
	}

	// LocalCoordinateSystem
	if (str.compare("LocalCoordinateSystem") == 0) {		
		C_Ele->GetTextAttribute("name",str);
		strcpy(m_name, str.c_str());
		C_Ele->GetTextAttribute("dataSet",str);
		strcpy(m_dataSet, str.c_str());

		vDS = C_Ele->GetAllDescriptions("SrcPixel");
		int Nsp = vDS.size();
		vDS = C_Ele->GetAllDescriptions("MappingFunct");
		int Nmf = vDS.size();

		GenericDS SP_Ele0=C_Ele->GetFirstChild();
		GenericDSInterfaceABC *SP_Ele=SP_Ele0.GetInterface();

		for (k=0; k<Nsp; k++) {
			// SrcPixel	
			if (SP_Ele->GetDSName() != "SrcPixel") { break; }
			m_NumOfPixels = k+1;
			SP_Ele->GetIntVector(vec);
			m_srcPixelX[k] = vec[0];	m_srcPixelY[k] = vec[1];

			SP_Ele0 = SP_Ele->GetNextSibling();
			SP_Ele=SP_Ele0.GetInterface();

			if (SP_Ele->GetDSName() == "Pixel") {		// Pixel
				m_Coord[k] = 0;
				SP_Ele->GetIntVector(vec);
				m_PixelX[k] = vec[0];	m_PixelY[k] = vec[1];

			} else {									// CoordPoint
				m_Coord[k] = 1;
				SP_Ele->GetRealVector(vecD);
				m_CoordPointX[k] = vecD[0];	m_CoordPointY[k] = vecD[1];
			}

			SP_Ele0 = SP_Ele->GetNextSibling();
			SP_Ele=SP_Ele0.GetInterface();
		}

		// MappingFunct
		for (k=0; k<Nmf; k++) {
			if (SP_Ele->GetDSName() != "MappingFunct") { break; }
			m_NumOfMappingFuncts = k+1;
			SP_Ele->GetIntAttribute("dimension",(int&)m_MappingFunctLength[k]);
			SP_Ele->GetTextValue(str);
			strcpy(m_MappingFunct+(k*256), str.c_str());

			SP_Ele0 = SP_Ele->GetNextSibling();
			SP_Ele=SP_Ele0.GetInterface();
		}
		
		C_Ele0=C_Ele->GetNextSibling();
		C_Ele=C_Ele0.GetInterface();
		str = C_Ele->GetDSName();
	}

	// IntegrateCoordinateSystem
	if (str.compare("IntegrateCoordinateSystem") == 0) {		
		C_Ele->GetIntAttribute("modelType",(int&)m_modelType);
		C_Ele->GetIntAttribute("xOrigin",(int&)m_xOrigin);
		C_Ele->GetIntAttribute("yOrigin",(int&)m_yOrigin);
		C_Ele->GetIntAttribute("numOfMotionParameterSets",(int&)m_numOfMotionParameterSets);

		
		GenericDS P_Ele0=C_Ele->GetFirstChild();
		GenericDSInterfaceABC *P_Ele=P_Ele0.GetInterface();

		for (k=0; k<m_numOfMotionParameterSets; k++) {
			if (P_Ele->GetDSName() != "TimeIncr") { break; }
			P_Ele->GetIntValue((int&)m_TimeIncr[k]);

			P_Ele0 = P_Ele->GetNextSibling();
			P_Ele=P_Ele0.GetInterface();

			for (l=0; l<m_modelType; l++) {
				if (P_Ele->GetDSName() != "MotionParameters") { break; }
				P_Ele->GetRealValue(m_MotionParameters[k*16+l]);

				P_Ele0 = P_Ele->GetNextSibling();
				P_Ele=P_Ele0.GetInterface();
			}
		}

	}

	return 0;
}

