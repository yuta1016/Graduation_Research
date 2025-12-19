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
// Applicable File Name:  TimeSeries.cpp
//

#include <stdio.h>
#include <cstring>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID TimeSeriesDescriptorInterface::myID = UUID("");
const char *TimeSeriesDescriptorInterface::myName = 
"TimeSeriesDescriptionInterface";

const UUID TimeSeriesDescriptor::myID = UUID("");
const char *TimeSeriesDescriptor::myName = "TimeSeriesDescriptor";

const UUID TimeSeriesDescriptor::valID = UUID("");
const char *TimeSeriesDescriptor::valName = "";

const UUID TimeSeriesDescriptorInterfaceABC::myID = UUID();

TimeSeriesDescriptorInterfaceABC::TimeSeriesDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TimeSeriesDescriptorInterface::
TimeSeriesDescriptorInterface(TimeSeriesDescriptor *aHisto):
m_pTimeSeriesDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeSeriesDescriptorInterface::~TimeSeriesDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::addref(void)
{ m_pTimeSeriesDescriptor->addref(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::release(void)
{ m_pTimeSeriesDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& TimeSeriesDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
long TimeSeriesDescriptorInterface::GetDummyContents(void)
{ return m_pTimeSeriesDescriptor->GetDummyContents(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetDummyContents(const long val)
{m_pTimeSeriesDescriptor->SetDummyContents(val); }

//----------------------------------------------------------------------------
short TimeSeriesDescriptorInterface::GetContentDescriptorId(void)
{ return m_pTimeSeriesDescriptor->GetContentDescriptorId(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetContentDescriptorId(const short val)
{m_pTimeSeriesDescriptor->SetContentDescriptorId(val); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::GetTimeIncr(int &incr)
{
	m_pTimeSeriesDescriptor->GetTimeIncr(incr);
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetTimeIncr(int val)
{
	m_pTimeSeriesDescriptor->SetTimeIncr(val);
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::GetOffset(int &incr, bool &b)
{
	m_pTimeSeriesDescriptor->GetOffset(incr, b);
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetOffset(int val)
{
	m_pTimeSeriesDescriptor->SetOffset(val);
}

double TimeSeriesDescriptorInterface::GetFrameRate(int &num, int &den)
{
	return m_pTimeSeriesDescriptor->GetFrameRate(num, den);
}
  
void TimeSeriesDescriptorInterface::SetFrameRate(int num, int den)
{
	m_pTimeSeriesDescriptor->SetFrameRate(num, den);
}

//----------------------------------------------------------------------------
short TimeSeriesDescriptorInterface::GetSeriesType(void)
{ return m_pTimeSeriesDescriptor->GetSeriesType(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetSeriesType(const char *val)
{m_pTimeSeriesDescriptor->SetSeriesType(val); }

//----------------------------------------------------------------------------
int TimeSeriesDescriptorInterface::GetNumOfContents(void)
{ return m_pTimeSeriesDescriptor->GetNumOfContents(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetNumOfContents(const int val)
{ m_pTimeSeriesDescriptor->SetNumOfContents(val); }

//----------------------------------------------------------------------------
bool TimeSeriesDescriptorInterface::AllocContentDescriptors(void)
{ return m_pTimeSeriesDescriptor->AllocContentDescriptors(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::FreeContentDescriptors(void)
{m_pTimeSeriesDescriptor->FreeContentDescriptors(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::DeleteOneContentDescriptor(const int val)
{m_pTimeSeriesDescriptor->DeleteOneContentDescriptor(val); }

//----------------------------------------------------------------------------
void *TimeSeriesDescriptorInterface::GetContentDescriptor(const int val)
{ return m_pTimeSeriesDescriptor->GetContentDescriptor(val); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetContentDescriptor(const int val, void *content)
{m_pTimeSeriesDescriptor->SetContentDescriptor(val, content); }

//----------------------------------------------------------------------------
long TimeSeriesDescriptorInterface::GetContentInterval(const int val)
{ return m_pTimeSeriesDescriptor->GetContentInterval(val); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetContentInterval(const int val, const long interval)
{m_pTimeSeriesDescriptor->SetContentInterval(val, interval); }

//----------------------------------------------------------------------------
long TimeSeriesDescriptorInterface::GetMaxInterval(void)
{ return m_pTimeSeriesDescriptor->GetMaxInterval(); }

//----------------------------------------------------------------------------
void TimeSeriesDescriptorInterface::SetMaxInterval(const long val)
{m_pTimeSeriesDescriptor->SetMaxInterval(val); }

//----------------------------------------------------------------------------
unsigned long TimeSeriesDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_pTimeSeriesDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long TimeSeriesDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_pTimeSeriesDescriptor->ImportDDL(aDescription); }

//=============================================================================
TimeSeriesDescriptor::TimeSeriesDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_DummyContents(0),
m_maxInterval(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeSeriesDescriptor::~TimeSeriesDescriptor()
{
#ifdef VERBOSE
  fprintf(stderr,"TimeSeries delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& TimeSeriesDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TimeSeriesDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TimeSeriesDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TimeSeriesDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
long TimeSeriesDescriptor::GetDummyContents(void)
{ return m_DummyContents; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetDummyContents(const long val)
{m_DummyContents=val; }

//----------------------------------------------------------------------------
short TimeSeriesDescriptor::GetContentDescriptorId(void)
{ return m_DescriptorId; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetContentDescriptorId(const short val)
{m_DescriptorId=val; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::GetTimeIncr(int &incr)
{ incr = m_TimeIncr; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetTimeIncr(int val)
{
	m_TimeIncr = val;
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::GetOffset(int &incr, bool &b)
{ incr = m_Offset; b = m_IsOffset; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetOffset(int val)
{
	if (val == 0) {
		m_IsOffset = false;
	} else {
		m_IsOffset = true;
	}

	m_Offset = val;
}

double TimeSeriesDescriptor::GetFrameRate(int &num, int &den)
{
	num = m_FrameRateNum;
	den = m_FrameRateDen;
	if(den > 0)
		return num/den;
	else
		return 25;	// the default
}
  
void TimeSeriesDescriptor::SetFrameRate(int num, int den)
{
	m_FrameRateNum = num;
	m_FrameRateDen = den;
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::readMediaDuration(const char *val, MediaDuration *mid)
{
	MediaDuration	*md;

	md = mid;

	md->Sign = 0x00;

	sscanf(val, "P%dDT%dH%dM%dS%dN%dF", &md->D, &md->h, &md->m, &md->s, &md->nn, &md->nnFraction);

	md->Representation = 0x00;
	if (md->D != 0) {
		md->Representation |= 0x10;
	}
	if (md->h != 0) {
		md->Representation |= 0x08;
	}
	if (md->m != 0) {
		md->Representation |= 0x04;
	}
	if (md->s != 0) {
		md->Representation |= 0x02;
	}
	if (md->nn != 0) {
		md->Representation |= 0x01;
	}

	return;
}

//----------------------------------------------------------------------------
short TimeSeriesDescriptor::GetSeriesType(void)
{ return m_SeriesType; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetSeriesType(const char *val)
{
	if (strcmp(val, "Regular") == 0) {
		m_SeriesType = TYPE_REGULAR_SERIES;
	} else if (strcmp(val, "Irregular") == 0) {
		m_SeriesType = TYPE_IRREGULAR_SERIES;
	} else {
		fprintf(stderr, "TimeSeriesDescriptor::SetSeriesType: strange type\n");
		exit (3);
	}
}

//----------------------------------------------------------------------------
int TimeSeriesDescriptor::GetNumOfContents(void)
{ return m_DescriptorNum; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetNumOfContents(const int val)
{
	m_DescriptorNum = val;
}

//----------------------------------------------------------------------------
long TimeSeriesDescriptor::GetMaxInterval(void)
{ return m_maxInterval; }

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetMaxInterval(const long val)
{m_maxInterval = val; }

//----------------------------------------------------------------------------
bool TimeSeriesDescriptor::AllocContentDescriptors(void)
{
	int		i;

	if (m_DescriptorNum <= 0) return (false);

	// alloc descriptor pointers
	m_ContentDescriptor = new void *[m_DescriptorNum];
	if (m_ContentDescriptor == NULL) return (false);
	for(i=0; i < m_DescriptorNum; i++) m_ContentDescriptor[i]=0;
	// alloc intervals
	m_Interval = new long [m_DescriptorNum];
	if (m_Interval == NULL) return (false);
	for(i=0; i < m_DescriptorNum; i++) m_Interval[i]=0;

	return (true);
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::FreeContentDescriptors(void)
{
	int		i;

	if (m_DescriptorNum != 0) {
		if (m_ContentDescriptor != NULL) {
			for (i = 0; i < m_DescriptorNum; i++) {
				if (m_ContentDescriptor[i] != NULL) {
					delete m_ContentDescriptor[i];
					m_ContentDescriptor[i] = 0;
				}
			}
			delete []m_ContentDescriptor;
			m_ContentDescriptor = 0;
		}
		if (m_Interval != NULL) {
			delete []m_Interval;
			m_Interval = 0;
		}
		m_DescriptorNum = 0;
	}
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::DeleteOneContentDescriptor(const int val)
{
	if (val >= m_DescriptorNum) return;
	if (m_ContentDescriptor != NULL) {
		delete m_ContentDescriptor[val];
		m_ContentDescriptor[val] = 0;
	}
	if (m_Interval != NULL) {
		m_Interval[val] = 0L;
	}
	return;
}

//----------------------------------------------------------------------------
void *TimeSeriesDescriptor::GetContentDescriptor(const int val)
{
	if (val > m_DescriptorNum) return (0);
	return m_ContentDescriptor[val];
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetContentDescriptor(const int val, void *content)
{
	if (val > m_DescriptorNum) return;
	m_ContentDescriptor[val] = content;
}

//----------------------------------------------------------------------------
long TimeSeriesDescriptor::GetContentInterval(const int val)
{
	if (val > m_DescriptorNum) return (0);
	return m_Interval[val];
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::SetContentInterval(const int val, const long interval)
{
	if (val > m_DescriptorNum) return;
	m_Interval[val] = interval;
}

extern unsigned long  MTT_GetMediaDurationString(unsigned long ul,char* s);

//----------------------------------------------------------------------------
unsigned long TimeSeriesDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  int val, i;
  char time_buf[64];

  if (!aParentDescription) return (unsigned long) -1;
  //if (aParentDescription->GetDSName() != "VisualTimeSeriesDescriptor")
	  //return (unsigned long) -1;

  int SeriesType = GetSeriesType();
  int nEntries = GetNumOfContents();

  if(SeriesType == TYPE_REGULAR_SERIES)
  {
	  unsigned long incr_millis;
	  int timeIncr;

	  aParentDescription->SetAttribute("xsi:type", "RegularVisualTimeSeriesType");

	  GetTimeIncr(timeIncr);
	  incr_millis = 1000* timeIncr * m_FrameRateDen / m_FrameRateNum;
	  MTT_GetMediaDurationString(incr_millis, time_buf);


	  GenericDS theTimeIncr = aParentDescription->CreateChild("TimeIncr");
	  theTimeIncr.SetValue(std::string(time_buf));
	  for (i = 0; i < nEntries; i++)
	  {
		  ColorLayoutDescriptor *theDescriptor = (ColorLayoutDescriptor *)GetContentDescriptor(i);
		  theDescriptor->ExportDDL(aParentDescription);
	  }
  }
  else
  {
	  aParentDescription->SetAttribute("xsi:type", "IrregularVisualTimeSeriesType");
	  // NOT implemented !
  }
  
  return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
	return 0;
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::addref(void)

{
  m_refcount++;
}

//----------------------------------------------------------------------------
void TimeSeriesDescriptor::release(void)

{
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC *TimeSeriesDescriptor::GetInterface(void)
{ return &m_Interface; }

