///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Spatial2DCoordinates.h
#ifndef __Spatial2DCoordinates__
#define __Spatial2DCoordinates__

#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{


//=============================================================================
class Spatial2DCoordinatesDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	Spatial2DCoordinatesDescriptorInterfaceABC();
	virtual ~Spatial2DCoordinatesDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent)=0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription)=0;

	// Accesssor methods
	virtual	char* Getid()=0;
	virtual	unsigned long Setid(char*)=0;
	virtual unsigned long GetxRepr()=0;
	virtual unsigned long SetxRepr(unsigned long)=0;
	virtual unsigned long GetyRepr()=0;
	virtual unsigned long SetyRepr(unsigned long)=0;
	virtual unsigned long GetxSrcSize()=0;
	virtual unsigned long SetxSrcSize(unsigned long)=0;
	virtual unsigned long GetySrcSize()=0;
	virtual unsigned long SetySrcSize(unsigned long)=0;

	virtual unsigned long GetUnitDefined()=0;
	virtual unsigned long SetUnitDefined(unsigned long)=0;
	virtual unsigned long GetUnit()=0;
	virtual unsigned long SetUnit(unsigned long)=0;

	virtual unsigned long GetLocalCoordinatesDefined()=0;
	virtual unsigned long SetLocalCoordinatesDefined(unsigned long)=0;
	virtual	unsigned long GetNameLength()=0;
	virtual	unsigned long SetNameLength(unsigned long)=0;
	virtual	char* Getname()=0;
	virtual	unsigned long Setname(char*)=0;
	virtual	unsigned long GetDataSetDefined()=0;
	virtual	unsigned long SetDataSetDefined(unsigned long)=0;
	virtual	unsigned long GetDataSetLength()=0;
	virtual	unsigned long SetDataSetLength(unsigned long)=0;
	virtual	char* GetdataSet()=0;
	virtual	unsigned long SetdataSet(char*)=0;

	virtual	unsigned long  GetNumOfPixels()=0;
	virtual	unsigned long  SetNumOfPixels(unsigned long)=0;
	virtual	unsigned long  GetsrcPixelX(unsigned long)=0;
	virtual	unsigned long  SetsrcPixelX(unsigned long,unsigned long)=0;
	virtual	unsigned long  GetsrcPixelY(unsigned long)=0;
	virtual	unsigned long  SetsrcPixelY(unsigned long,unsigned long)=0;
	virtual	unsigned long  GetCoord(unsigned long)=0;
	virtual	unsigned long  SetCoord(unsigned long,unsigned long)=0;
	virtual	unsigned long  GetPixelX(unsigned long)=0;
	virtual	unsigned long  SetPixelX(unsigned long,unsigned long)=0;
	virtual	unsigned long  GetPixelY(unsigned long)=0;
	virtual	unsigned long  SetPixelY(unsigned long,unsigned long)=0;
	virtual	double GetCoordPointX(unsigned long)=0;
	virtual	unsigned long  SetCoordPointX(unsigned long,double)=0;
	virtual	double GetCoordPointY(unsigned long)=0;
	virtual	unsigned long  SetCoordPointY(unsigned long,double)=0;

	virtual	unsigned long GetNumOfMappingFuncts()=0;
	virtual	unsigned long SetNumOfMappingFuncts(unsigned long)=0;
	virtual	unsigned long GetMappingFunctLength(unsigned long)=0;
	virtual	unsigned long SetMappingFunctLength(unsigned long,unsigned long)=0;
	virtual	char* GetMappingFunct(unsigned long)=0;
	virtual	unsigned long SetMappingFunct(unsigned long,char*)=0;

	virtual unsigned long  GetIntegratedCoordinatesDefined()=0;
	virtual unsigned long  SetIntegratedCoordinatesDefined(unsigned long)=0;
	virtual	unsigned long  GetmodelType()=0;
	virtual	unsigned long  SetmodelType(unsigned long)=0;
	virtual	double GetxOrigin()=0;
	virtual	unsigned long  SetxOrigin(double)=0;
	virtual	double GetyOrigin()=0;
	virtual	unsigned long  SetyOrigin(double)=0;
	virtual	unsigned long  GetnumOfMotionParameterSets()=0;
	virtual	unsigned long  SetnumOfMotionParameterSets(unsigned long)=0;
	virtual	unsigned long  GetTimeIncr(unsigned long)=0;
	virtual	unsigned long  SetTimeIncr(unsigned long,unsigned long)=0;
	virtual	double GetMotionParameters(unsigned long,unsigned long)=0;
	virtual	unsigned long  SetMotionParameters(unsigned long,unsigned long,double)=0;

	static const UUID myID;

};

class Spatial2DCoordinatesDescriptor;

//=============================================================================
class Spatial2DCoordinatesDescriptorInterface: public Spatial2DCoordinatesDescriptorInterfaceABC
{
public:
	Spatial2DCoordinatesDescriptorInterface(Spatial2DCoordinatesDescriptor
						*stemp);
	virtual ~Spatial2DCoordinatesDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// Accesssor methods
	virtual	char* Getid();
	virtual	unsigned long Setid(char*);
	virtual unsigned long GetxRepr();
	virtual unsigned long SetxRepr(unsigned long);
	virtual unsigned long GetyRepr();
	virtual unsigned long SetyRepr(unsigned long);
	virtual unsigned long GetxSrcSize();
	virtual unsigned long SetxSrcSize(unsigned long);
	virtual unsigned long GetySrcSize();
	virtual unsigned long SetySrcSize(unsigned long);

	virtual unsigned long GetUnitDefined();
	virtual unsigned long SetUnitDefined(unsigned long);
	virtual unsigned long GetUnit();
	virtual unsigned long SetUnit(unsigned long);

	virtual unsigned long GetLocalCoordinatesDefined();
	virtual unsigned long SetLocalCoordinatesDefined(unsigned long);
	virtual	unsigned long GetNameLength();
	virtual	unsigned long SetNameLength(unsigned long);
	virtual	char* Getname();
	virtual	unsigned long Setname(char*);
	virtual	unsigned long GetDataSetDefined();
	virtual	unsigned long SetDataSetDefined(unsigned long);
	virtual	unsigned long GetDataSetLength();
	virtual	unsigned long SetDataSetLength(unsigned long);
	virtual	char* GetdataSet();
	virtual	unsigned long SetdataSet(char*);

	virtual	unsigned long  GetNumOfPixels();
	virtual	unsigned long  SetNumOfPixels(unsigned long);
	virtual	unsigned long  GetsrcPixelX(unsigned long);
	virtual	unsigned long  SetsrcPixelX(unsigned long,unsigned long);
	virtual	unsigned long  GetsrcPixelY(unsigned long);
	virtual	unsigned long  SetsrcPixelY(unsigned long,unsigned long);
	virtual	unsigned long  GetCoord(unsigned long);
	virtual	unsigned long  SetCoord(unsigned long,unsigned long);
	virtual	unsigned long  GetPixelX(unsigned long);
	virtual	unsigned long  SetPixelX(unsigned long,unsigned long);
	virtual	unsigned long  GetPixelY(unsigned long);
	virtual	unsigned long  SetPixelY(unsigned long,unsigned long);
	virtual	double GetCoordPointX(unsigned long);
	virtual	unsigned long  SetCoordPointX(unsigned long,double);
	virtual	double GetCoordPointY(unsigned long);
	virtual	unsigned long  SetCoordPointY(unsigned long,double);

	virtual	unsigned long GetNumOfMappingFuncts();
	virtual	unsigned long SetNumOfMappingFuncts(unsigned long);
	virtual	unsigned long GetMappingFunctLength(unsigned long);
	virtual	unsigned long SetMappingFunctLength(unsigned long,unsigned long);
	virtual	char* GetMappingFunct(unsigned long);
	virtual	unsigned long SetMappingFunct(unsigned long,char*);

	virtual unsigned long  GetIntegratedCoordinatesDefined();
	virtual unsigned long  SetIntegratedCoordinatesDefined(unsigned long);
	virtual	unsigned long  GetmodelType();
	virtual	unsigned long  SetmodelType(unsigned long);
	virtual	double GetxOrigin();
	virtual	unsigned long  SetxOrigin(double);
	virtual	double GetyOrigin();
	virtual	unsigned long  SetyOrigin(double);
	virtual	unsigned long  GetnumOfMotionParameterSets();
	virtual	unsigned long  SetnumOfMotionParameterSets(unsigned long);
	virtual	unsigned long  GetTimeIncr(unsigned long);
	virtual	unsigned long  SetTimeIncr(unsigned long,unsigned long);
	virtual	double GetMotionParameters(unsigned long,unsigned long);
	virtual	unsigned long  SetMotionParameters(unsigned long,unsigned long,double);


	static const UUID myID;
	static const char *myName;


private:
	Spatial2DCoordinatesDescriptor *m_Spatial2DCoordinatesDescriptor;
};

//=============================================================================
class Spatial2DCoordinatesDescriptor: public Descriptor
{
friend class Spatial2DCoordinatesDescriptorInterface;
public:
	Spatial2DCoordinatesDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual Spatial2DCoordinatesDescriptorInterfaceABC *GetInterface(void);

	// Accesssor methods
	virtual	char* Getid();
	virtual	unsigned long Setid(char*);
	virtual unsigned long GetxRepr();
	virtual unsigned long SetxRepr(unsigned long);
	virtual unsigned long GetyRepr();
	virtual unsigned long SetyRepr(unsigned long);
	virtual unsigned long GetxSrcSize();
	virtual unsigned long SetxSrcSize(unsigned long);
	virtual unsigned long GetySrcSize();
	virtual unsigned long SetySrcSize(unsigned long);

	virtual unsigned long GetUnitDefined();
	virtual unsigned long SetUnitDefined(unsigned long);
	virtual unsigned long GetUnit();
	virtual unsigned long SetUnit(unsigned long);

	virtual unsigned long GetLocalCoordinatesDefined();
	virtual unsigned long SetLocalCoordinatesDefined(unsigned long);
	virtual	unsigned long GetNameLength();
	virtual	unsigned long SetNameLength(unsigned long);
	virtual	char* Getname();
	virtual	unsigned long Setname(char*);
	virtual	unsigned long GetDataSetDefined();
	virtual	unsigned long SetDataSetDefined(unsigned long);
	virtual	unsigned long GetDataSetLength();
	virtual	unsigned long SetDataSetLength(unsigned long);
	virtual	char* GetdataSet();
	virtual	unsigned long SetdataSet(char*);

	virtual	unsigned long  GetNumOfPixels();
	virtual	unsigned long  SetNumOfPixels(unsigned long);
	virtual	unsigned long  GetsrcPixelX(unsigned long);
	virtual	unsigned long  SetsrcPixelX(unsigned long,unsigned long);
	virtual	unsigned long  GetsrcPixelY(unsigned long);
	virtual	unsigned long  SetsrcPixelY(unsigned long,unsigned long);
	virtual	unsigned long  GetCoord(unsigned long);
	virtual	unsigned long  SetCoord(unsigned long,unsigned long);
	virtual	unsigned long  GetPixelX(unsigned long);
	virtual	unsigned long  SetPixelX(unsigned long,unsigned long);
	virtual	unsigned long  GetPixelY(unsigned long);
	virtual	unsigned long  SetPixelY(unsigned long,unsigned long);
	virtual	double GetCoordPointX(unsigned long);
	virtual	unsigned long  SetCoordPointX(unsigned long,double);
	virtual	double GetCoordPointY(unsigned long);
	virtual	unsigned long  SetCoordPointY(unsigned long,double);

	virtual	unsigned long GetNumOfMappingFuncts();
	virtual	unsigned long SetNumOfMappingFuncts(unsigned long);
	virtual	unsigned long GetMappingFunctLength(unsigned long);
	virtual	unsigned long SetMappingFunctLength(unsigned long,unsigned long);
	virtual	char* GetMappingFunct(unsigned long);
	virtual	unsigned long SetMappingFunct(unsigned long,char*);

	virtual unsigned long  GetIntegratedCoordinatesDefined();
	virtual unsigned long  SetIntegratedCoordinatesDefined(unsigned long);
	virtual	unsigned long  GetmodelType();
	virtual	unsigned long  SetmodelType(unsigned long);
	virtual	double GetxOrigin();
	virtual	unsigned long  SetxOrigin(double);
	virtual	double GetyOrigin();
	virtual	unsigned long  SetyOrigin(double);
	virtual	unsigned long  GetnumOfMotionParameterSets();
	virtual	unsigned long  SetnumOfMotionParameterSets(unsigned long);
	virtual	unsigned long  GetTimeIncr(unsigned long);
	virtual	unsigned long  SetTimeIncr(unsigned long,unsigned long);
	virtual	double GetMotionParameters(unsigned long,unsigned long);
	virtual	unsigned long  SetMotionParameters(unsigned long,unsigned long,double);


private:
	virtual ~Spatial2DCoordinatesDescriptor();

	unsigned long m_refcount;

	Spatial2DCoordinatesDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


	// actual values:
#define MaxMotionParameterSets 256
	char  m_id[256];
 	unsigned long m_xRepr;
 	unsigned long m_yRepr; 
	unsigned long m_xSrcSize;
	unsigned long m_ySrcSize;

	unsigned long m_UnitDefined;
	unsigned long  m_Unit;

	unsigned long m_LocalCoordinatesDefined;
	unsigned long m_NameLength;
	char  m_name[256];
	unsigned long m_DataSetDefined;
	unsigned long m_DataSetLength;
	char  m_dataSet[256];
	unsigned long m_NumOfPixels;
	unsigned long m_srcPixelX[5];
	unsigned long m_srcPixelY[5];
	unsigned long  m_Coord[5];
	unsigned long m_PixelX[5];
	unsigned long m_PixelY[5];
	double m_CoordPointX[5];
	double m_CoordPointY[5];
	unsigned long m_NumOfMappingFuncts;
	unsigned long m_MappingFunctLength[5];
	char  m_MappingFunct[5*256];

	unsigned long m_IntegratedCoordinatesDefined;
	unsigned long  m_modelType;
	double m_xOrigin;
	double m_yOrigin;
	unsigned long  m_numOfMotionParameterSets;
	unsigned long  m_TimeIncr[MaxMotionParameterSets];
	double m_MotionParameters[MaxMotionParameterSets*16];

};

};

#endif //__Spatial2DCoordinates__
