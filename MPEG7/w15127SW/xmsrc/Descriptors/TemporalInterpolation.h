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
// Applicable File Name:  TemporalInterpolation.h


#ifndef __TemporalInterpolation__
#define __TemporalInterpolation__

#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{


//=============================================================================
class TemporalInterpolationDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	TemporalInterpolationDescriptorInterfaceABC();
	virtual ~TemporalInterpolationDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent)=0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription)=0;

	// Accesssor methods
	virtual unsigned long  GetNumOfKeyPoint()=0;
	virtual unsigned long  SetNumOfKeyPoint(unsigned long)=0;
	virtual unsigned long  GetDimension()=0;
	virtual unsigned long  SetDimension(unsigned long)=0;
	virtual unsigned long  GetConstantTimeInterval()=0;
	virtual unsigned long  SetConstantTimeInterval(unsigned long)=0;
	virtual unsigned long  GetWholeIntervalDataType()=0;
	virtual unsigned long  SetWholeIntervalDataType(unsigned long)=0;
	virtual double GetMediaDuration()=0;
	virtual unsigned long  SetMediaDuration(double)=0;

	virtual unsigned long  GetMediaTimePointDataType()=0;
	virtual unsigned long  SetMediaTimePointDataType(unsigned long)=0;
	virtual double GetMediaTimePoint(unsigned long)=0;
	virtual unsigned long  SetMediaTimePoint(unsigned long,double)=0;
	virtual double GetKeyValue(unsigned long,unsigned long)=0;
	virtual unsigned long  SetKeyValue(unsigned long,unsigned long,double)=0;

	virtual unsigned long  GetDefaultFunction(unsigned long)=0;
	virtual unsigned long  SetDefaultFunction(unsigned long,unsigned long)=0;
	virtual unsigned long  GetType(unsigned long,unsigned long)=0;
	virtual unsigned long  SetType(unsigned long,unsigned long,unsigned long)=0;
	virtual double GetParam(unsigned long,unsigned long)=0;
	virtual unsigned long  SetParam(unsigned long,unsigned long,double)=0;

	static const UUID myID;

};

class TemporalInterpolationDescriptor;

//=============================================================================
class TemporalInterpolationDescriptorInterface: public TemporalInterpolationDescriptorInterfaceABC
{
public:
	TemporalInterpolationDescriptorInterface(TemporalInterpolationDescriptor *stemp);
	virtual ~TemporalInterpolationDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// Accesssor methods
	virtual unsigned long  GetNumOfKeyPoint();
	virtual unsigned long  SetNumOfKeyPoint(unsigned long);
	virtual unsigned long  GetDimension();
	virtual unsigned long  SetDimension(unsigned long);
	virtual unsigned long  GetConstantTimeInterval();
	virtual unsigned long  SetConstantTimeInterval(unsigned long);
	virtual unsigned long  GetWholeIntervalDataType();
	virtual unsigned long  SetWholeIntervalDataType(unsigned long);
	virtual double GetMediaDuration();
	virtual unsigned long  SetMediaDuration(double);

	virtual unsigned long  GetMediaTimePointDataType();
	virtual unsigned long  SetMediaTimePointDataType(unsigned long);
	virtual double GetMediaTimePoint(unsigned long);
	virtual unsigned long  SetMediaTimePoint(unsigned long,double);
	virtual double GetKeyValue(unsigned long,unsigned long);
	virtual unsigned long  SetKeyValue(unsigned long,unsigned long,double);

	virtual unsigned long  GetDefaultFunction(unsigned long);
	virtual unsigned long  SetDefaultFunction(unsigned long,unsigned long);
	virtual unsigned long  GetType(unsigned long,unsigned long);
	virtual unsigned long  SetType(unsigned long,unsigned long,unsigned long);
	virtual double GetParam(unsigned long,unsigned long);
	virtual unsigned long  SetParam(unsigned long,unsigned long,double);

	static const UUID myID;
	static const char *myName;

private:
	TemporalInterpolationDescriptor *m_TemporalInterpolationDescriptor;
};

//=============================================================================
class TemporalInterpolationDescriptor: public Descriptor
{
friend class TemporalInterpolationDescriptorInterface;
public:
	TemporalInterpolationDescriptor();

	virtual void addref();
	virtual void release();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual TemporalInterpolationDescriptorInterfaceABC *GetInterface(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	// Accesssor methods
	virtual unsigned long  GetNumOfKeyPoint();
	virtual unsigned long  SetNumOfKeyPoint(unsigned long);
	virtual unsigned long  GetDimension();
	virtual unsigned long  SetDimension(unsigned long);
	virtual unsigned long  GetConstantTimeInterval();
	virtual unsigned long  SetConstantTimeInterval(unsigned long);
	virtual unsigned long  GetWholeIntervalDataType();
	virtual unsigned long  SetWholeIntervalDataType(unsigned long);
	virtual double GetMediaDuration();
	virtual unsigned long  SetMediaDuration(double);

	virtual unsigned long  GetMediaTimePointDataType();
	virtual unsigned long  SetMediaTimePointDataType(unsigned long);
	virtual double GetMediaTimePoint(unsigned long);
	virtual unsigned long  SetMediaTimePoint(unsigned long,double);
	virtual double GetKeyValue(unsigned long,unsigned long);
	virtual unsigned long  SetKeyValue(unsigned long,unsigned long,double);

	virtual unsigned long  GetDefaultFunction(unsigned long);
	virtual unsigned long  SetDefaultFunction(unsigned long,unsigned long);
	virtual unsigned long  GetType(unsigned long,unsigned long);
	virtual unsigned long  SetType(unsigned long,unsigned long,unsigned long);
	virtual double GetParam(unsigned long,unsigned long);
	virtual unsigned long  SetParam(unsigned long,unsigned long,double);

private:
	virtual ~TemporalInterpolationDescriptor();
	unsigned long m_refcount;

	TemporalInterpolationDescriptorInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	unsigned long  GetMediaDurationString(char* s);
	unsigned long  GetMediaTimePointString(int i,char* s);
	unsigned long  ExtractHours(unsigned long second);
	unsigned long  ExtractMinutes(unsigned long second);
	unsigned long  ExtractSeconds(unsigned long second);
	unsigned long  ExtractSeconds(string s);


	// actual values:
#define MaxNumOfKeyPoint 256
	unsigned long  m_NumOfKeyPoint;
	unsigned long  m_Dimension;
	unsigned long  m_ConstantTimeInterval;
	unsigned long  m_WholeIntervalDataType;
	double m_MediaDuration;

	unsigned long m_MediaTimePointDataType;
	double m_MediaTimePoint[MaxNumOfKeyPoint];

	unsigned long  m_DefaultFunction[16];
	double m_KeyValue[16*MaxNumOfKeyPoint];
	unsigned long  m_Type[16*MaxNumOfKeyPoint];
	double m_Param[16*MaxNumOfKeyPoint];


};

};

#endif //__TemporalInterpolation__
