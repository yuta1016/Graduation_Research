///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Takehiro FUJITA
// Hitachi, Ltd.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TimeSeries.h

#ifndef __TIMESERIES_H__
#define __TIMESERIES_H__

#include "DescriptionSchemes/DescriptionSchemes.h"

#include "Descriptors/VisualDescriptorID.h"
#define REP_DAYS	0x10
#define REP_HOURS	0x08
#define REP_MINUTES	0x04
#define REP_SECONDS	0x02
#define REP_FRACTIONS	0x01

//sth #include "media_time.h"
typedef struct { //sth propritary coding stype for narmative components??
	char		Representation;
	char		Sign;
	short		D;
	short		h;
	short		m;
	short		s;
	short		nn;
	short		nnFraction;
} MediaDuration;

typedef struct {
	long		value;
	MediaDuration	timeUnit;
	} MediaIncrDuration;

//#include "TimeSeriesSub.h"  //sth 

// a list of input frames (temporally file)
#define INPUT_LIST_FILE          "temp_InputList.txt"
 //sth what`s this??

// file name extensions
#define EXT_REGULAR_SERIES      "tsr"
//sth what`s this?
#define EXT_IRREGULAR_SERIES    "tsi"

// Indicators for Irregular or Regular TimeSeries
#define TYPE_REGULAR_SERIES     (0)
#define TYPE_IRREGULAR_SERIES   (1)

// default threshold value
#define THRESHOLD_COLOR_LAYOUT	(3)


namespace XM
{

//=============================================================================
class TimeSeriesDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  TimeSeriesDescriptorInterfaceABC();
  virtual ~TimeSeriesDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  virtual long GetDummyContents(void)=0;
  virtual void SetDummyContents(const long val)=0;

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

  //------------ content descriptor ----------
  virtual short GetContentDescriptorId(void)=0;
  virtual void SetContentDescriptorId(const short val)=0;

  virtual void GetTimeIncr(int &incr)=0;
  virtual void SetTimeIncr(int val)=0;

  virtual void GetOffset(int &incr, bool &b)=0;
  virtual void SetOffset(int val)=0;

  virtual double GetFrameRate(int &num, int &den)=0;
  virtual void SetFrameRate(int num, int den)=0;

  virtual short GetSeriesType(void)=0;
  virtual void SetSeriesType(const char *val)=0;

  virtual int GetNumOfContents(void)=0;
  virtual void SetNumOfContents(const int val)=0;

  virtual bool AllocContentDescriptors(void)=0;
  virtual void FreeContentDescriptors(void)=0;
  virtual void DeleteOneContentDescriptor(const int val)=0;

  virtual void *GetContentDescriptor(const int val)=0;
  virtual void SetContentDescriptor(const int val, void *content)=0;


  
  virtual long GetContentInterval(const int val)=0;
  virtual void SetContentInterval(const int val, const long interval)=0;

  virtual long GetMaxInterval(void)=0;
  virtual void SetMaxInterval(const long val)=0;
};

class TimeSeriesDescriptor;

//=============================================================================
class TimeSeriesDescriptorInterface: public TimeSeriesDescriptorInterfaceABC
{
public:
  TimeSeriesDescriptorInterface(TimeSeriesDescriptor
			       *aTimeSeriesDescriptor);
  virtual ~TimeSeriesDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual long GetDummyContents(void);
  virtual void SetDummyContents(const long val);

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

  //------------ content descriptor ----------
  virtual short GetContentDescriptorId(void);
  virtual void SetContentDescriptorId(const short val);

  virtual void GetTimeIncr(int &incr);
  virtual void SetTimeIncr(int val);

  virtual void GetOffset(int &incr, bool &b);
  virtual void SetOffset(int val);

  virtual double GetFrameRate(int &num, int &den);
  virtual void SetFrameRate(int num, int den);

  virtual short GetSeriesType(void);
  virtual void SetSeriesType(const char *val);

  virtual int GetNumOfContents(void);
  virtual void SetNumOfContents(const int val);

  virtual bool AllocContentDescriptors(void);
  virtual void FreeContentDescriptors(void);
  virtual void DeleteOneContentDescriptor(const int val);

  virtual void *GetContentDescriptor(const int val);
  virtual void SetContentDescriptor(const int val, void *content);

  virtual long GetContentInterval(const int val);
  virtual void SetContentInterval(const int val, const long interval);

  virtual long GetMaxInterval(void);
  virtual void SetMaxInterval(const long val);

private:
  TimeSeriesDescriptor *m_pTimeSeriesDescriptor;
};

//=============================================================================
class TimeSeriesDescriptor: public Descriptor
{
friend class TimeSeriesDescriptorInterface;
public:
  TimeSeriesDescriptor();

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual long GetDummyContents(void);
  virtual void SetDummyContents(const long val);

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;

  virtual TimeSeriesDescriptorInterfaceABC *GetInterface(void);

  //------------ content descriptor ----------
  virtual short GetContentDescriptorId(void);
  virtual void SetContentDescriptorId(const short val);

  virtual void GetTimeIncr(int &incr);
  virtual void SetTimeIncr(int val);

  virtual void GetOffset(int &incr, bool &b);
  virtual void SetOffset(int val);

  virtual double GetFrameRate(int &num, int &den);
  virtual void SetFrameRate(int num, int den);

  virtual short GetSeriesType(void);
  virtual void SetSeriesType(const char *val);

  virtual int GetNumOfContents(void);
  virtual void SetNumOfContents(const int val);

  virtual bool AllocContentDescriptors(void);
  virtual void FreeContentDescriptors(void);
  virtual void DeleteOneContentDescriptor(const int val);

  virtual void *GetContentDescriptor(const int val);
  virtual void SetContentDescriptor(const int val, void *content);

  virtual long GetContentInterval(const int val);
  virtual void SetContentInterval(const int val, const long interval);

  virtual long GetMaxInterval(void);
  virtual void SetMaxInterval(const long val);

private:
  virtual ~TimeSeriesDescriptor();
	
  unsigned long m_refcount;

  TimeSeriesDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

  virtual void readMediaDuration(const char *val, MediaDuration *mid);

  /* series type (regular or irregular) */
  short	m_SeriesType;

 // This is the actual data the D/DSType stores. In this particular
 // dummy example it's just a signed long called m_DummyContents
  long m_DummyContents;//sth TimeSeries has a normative component named DummyContents??

    /* common elements (RegularTimeSeries/IrregularTimeSeries) */
	short	m_DescriptorId;
	int		m_DescriptorNum;
	short	m_DescriptorLength;
	int		m_TimeIncr;
	bool	m_IsOffset;
	int 	m_Offset;

	int		m_FrameRateNum;
	int		m_FrameRateDen;

	/* VisualDescriptors */
	void **m_ContentDescriptor;

	/* IrregularTimeSeries elements */
	long	*m_Interval;
				/* CS --> 8-bits ("Interval") or 32-bits ("LongInterval") */
	long	m_maxInterval;
};

};

#endif //__TIMESERIES_H__
