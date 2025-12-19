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
// Applicable File Name:  TimeSeriesCS.cpp
//

#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID TimeSeriesCSInterface::myID = UUID("");
const char *TimeSeriesCSInterface::myName =
                         "TimeSeriesCodingSchemeInterface";

// create a UUID for your type
const UUID TimeSeriesCS::myID = UUID("to be created with xx");
const char *TimeSeriesCS::myName = "TimeSeriesCodingScheme";

const UUID TimeSeriesCS::valID = UUID("");
const char *TimeSeriesCS::valName = "";

const UUID TimeSeriesCSInterfaceABC::myID = UUID("");

TimeSeriesCSInterfaceABC::TimeSeriesCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TimeSeriesCSInterface::TimeSeriesCSInterface(TimeSeriesCS *aTimeSeries):
m_TimeSeriesCS(aTimeSeries)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeSeriesCSInterface::~TimeSeriesCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& TimeSeriesCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void TimeSeriesCSInterface::destroy(void)
{ delete m_TimeSeriesCS; }

//----------------------------------------------------------------------------
BitBuffer *TimeSeriesCSInterface::GetEncoderStreamBuffer(void)
{
  return m_TimeSeriesCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_TimeSeriesCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *TimeSeriesCSInterface::GetDecoderStreamBuffer(void)
{
  return m_TimeSeriesCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_TimeSeriesCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC
*TimeSeriesCSInterface::GetDescriptorInterface(void)

{
  return m_TimeSeriesCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::
SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *aTimeSeriesDescriptorInterface)
{
  return m_TimeSeriesCS->
    SetDescriptorInterface(aTimeSeriesDescriptorInterface);
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::StartEncode()
{
  return m_TimeSeriesCS->StartEncode();
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::StartDecode(void)
{
  return m_TimeSeriesCS->StartDecode();
}

//=============================================================================
TimeSeriesCS::TimeSeriesCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeSeriesCS::TimeSeriesCS(TimeSeriesDescriptorInterfaceABC* TimeSeries):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!TimeSeries) {
    TimeSeriesDescriptor *descriptor =
    new TimeSeriesDescriptor;
    TimeSeries=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(TimeSeries);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeSeriesCS::~TimeSeriesCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& TimeSeriesCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TimeSeriesCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TimeSeriesCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TimeSeriesCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * TimeSeriesCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int TimeSeriesCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * TimeSeriesCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int TimeSeriesCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
TimeSeriesDescriptorInterfaceABC *TimeSeriesCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int TimeSeriesCS::
SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
		       *aTimeSeriesDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aTimeSeriesDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aTimeSeriesDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  } else {
    return -1;
  }

  return 0;
}

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
//----------------------------------------------------------------------------
SubTimeSeriesACSInterfaceABC *TimeSeriesCS::GetSubTimeSeriesACSInterface(void)
{
  return m_SubTimeSeriesACS;
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::
SetSubTimeSeriesACSInterface(SubTimeSeriesACSInterfaceABC
			    *aSubTimeSeriesACSInterface)
{
  if (!aSubTimeSeriesACSInterface) return -1;

  m_SubTimeSeriesACS=aSubTimeSeriesACSInterface;

  return 0;
}

//----------------------------------------------------------------------------
SubTimeSeriesBCSInterfaceABC *TimeSeriesCS::GetSubTimeSeriesBCSInterface(void)
{
  return m_SubTimeSeriesBCS;
}

//----------------------------------------------------------------------------
int TimeSeriesCSInterface::
SetSubTimeSeriesBCSInterface(SubTimeSeriesBCSInterfaceABC
			    *aSubTimeSeriesBCSInterface)
{
  if (!aSubTimeSeriesBCSInterface) return -1;

  m_SubTimeSeriesBCS=aSubTimeSeriesBCSInterface;

  return 0;
}
#endif /* __HasSubTypes*/

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int TimeSeriesCS::StartEncode()
{
//  /* stream data base*/
//  BitBuffer theStreamBuffer(0xFFFF);
  void			*theDescriptor;
  // content descriptor ID
  short		DescId;
  // TimeSeries Type
  short     SeriesType;
  long		nEntries;
  // DescriptorLength
  int		descLen;		// the length is specified as the number of bytes
  int		descLenBits;	// the length is specified as the number of bits
  int		res;
  int		bitstuffNum;
  // Offset
  bool		isOffset;
  int		offset;
  // TimeIncr
  int		timeIncr;
  // 
  int		maxInterval;
  long		interval;
  int		bits, bitsMD;
  int		i;

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"TimeSeriesCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"TimeSeriesCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  //----------------- encoding ------------------
  // Descriptor ID
  DescId = m_DescriptorInterface->GetContentDescriptorId();
  m_EncoderBuffer->PutBitsLong((long)DescId, 10UL);
  bits = 10;

  // DescriptorNum: the number of descriptors
  nEntries = m_DescriptorInterface->GetNumOfContents();
  m_EncoderBuffer->PutBitsLong((long)nEntries, 32UL);
  bits += 32;

  // DescriptorLength
  switch (DescId) {
  case ID_ColorLayout:			// Color Layout Descriptor
    descLenBits = BitLengthOfColorLayout();
	break;
  // other descriptors will be added.
  default:
	  break;
  }
  res = descLenBits % 8;
  if (res == 0) {
	  descLen = descLenBits / 8;
	  bitstuffNum = 0;
  } else {
	  descLen = (descLenBits / 8) + 1;
	  bitstuffNum = 8 - res;
  }
  m_EncoderBuffer->PutBitsLong((long)descLen, 16UL);
  bits += 16;

  // Regular or Irregular
  SeriesType = m_DescriptorInterface->GetSeriesType();
  if (SeriesType == TYPE_IRREGULAR_SERIES) {
  // Irregular Time Series
    // IsShortInterval
    maxInterval = m_DescriptorInterface->GetMaxInterval();
	if (maxInterval > 255) {	// Long interval
		m_EncoderBuffer->PutBitsLong(0, 1UL);
	} else {					// Short interval
		m_EncoderBuffer->PutBitsLong(1, 1UL);
    }
	bits += 1;

	// IsOffset
	m_DescriptorInterface->GetOffset(offset, isOffset);
	if (isOffset == true) {
		m_EncoderBuffer->PutBitsLong(1, 1UL);
		m_EncoderBuffer->PutBitsLong(offset, 32UL);	// Needs fixing!
		// Offset
		//bitsMD = PutMediaDuration(offset);
		//bits += (bitsMD + 1);
    } else {
		m_EncoderBuffer->PutBitsLong(0, 1UL);
		bits += 1;
    }

	// TimeIncr
	m_DescriptorInterface->GetTimeIncr(timeIncr);
	m_EncoderBuffer->PutBitsLong(timeIncr, 32UL);	// Needs fixing!
	//bitsMD = PutMediaDuration(timeIncr);
	//bits += bitsMD;


	// bit-stuffing
	res = bits % 8;
	if (res != 0) {
		int		tmp;
		tmp = 8 - res;
		for (i = 0; i < tmp; i++) {
			m_EncoderBuffer->PutBitsLong(1, 1UL);
		}
	}

	// Descriptors and Intervals
	for (i = 0; i < nEntries; i++) {
	  // one descriptor
	  theDescriptor = GetDescriptorInterface()->GetContentDescriptor(i);
      switch (DescId) {
      case ID_ColorLayout:			// Color Layout Descriptor
        OneColorLayoutEncoding(m_EncoderBuffer, theDescriptor, bitstuffNum);
	    break;
        // other descriptors will be added.
      default:
	    break;
	  }
      // bit-stuffing
      if (bitstuffNum != 0) {
		for (i = 0; i < bitstuffNum; i++) {
			m_EncoderBuffer->PutBitsLong(1, 1UL);
		}
	  }

	  // interval
	  interval = GetDescriptorInterface()->GetContentInterval(i);
	  if (maxInterval > 255) {	// Long interval
        m_EncoderBuffer->PutBitsLong(interval, 32UL);
	  } else {					// Short interval
        m_EncoderBuffer->PutBitsLong(interval, 8UL);
	  }	
	}
  } else {
  // Regular Time Series
	// TimeIncr
	m_DescriptorInterface->GetTimeIncr(timeIncr);
	m_EncoderBuffer->PutBitsLong(timeIncr, 32UL);	// Needs fixing!

	//bitsMD = PutMediaDuration(timeIncr);
	//bits += bitsMD;

	// IsOffset
	m_DescriptorInterface->GetOffset(offset, isOffset);
	if (isOffset == true) {
		m_EncoderBuffer->PutBitsLong(1, 1UL);
		m_EncoderBuffer->PutBitsLong(offset, 32UL);	// Needs fixing!
		// Offset
		//bitsMD = PutMediaDuration(offset);
		//bits += (bitsMD + 1);
    } else {
		m_EncoderBuffer->PutBitsLong(0, 1UL);
		bits += 1;
    }

	// bit-stuffing
	res = bits % 8;
	if (res != 0) {
		int		tmp;
		tmp = 8 - res;
		for (i = 0; i < tmp; i++) {
			m_EncoderBuffer->PutBitsLong(1, 1UL);
		}
	}

	// Descriptors and Intervals
	for (i = 0; i < nEntries; i++) {
	  // one descriptor
	  theDescriptor = GetDescriptorInterface()->GetContentDescriptor(i);
      switch (DescId) {
      case ID_ColorLayout:			// Color Layout Descriptor
		  OneColorLayoutEncoding(m_EncoderBuffer, theDescriptor, bitstuffNum);
	    break;
        // other descriptors will be added.
      default:
	    break;
	  }
      // bit-stuffing
      if (bitstuffNum != 0) {
		for (i = 0; i < bitstuffNum; i++) {
			m_EncoderBuffer->PutBitsLong(1, 1UL);
		}
	  }
    }
  }
 
  // All done
  return 0;
}

//----------------------------------------------------------------------------
int TimeSeriesCS::PutMediaDuration(const MediaDuration dur)
{
	char	rep;
	int		bits;

	m_EncoderBuffer->PutBitsLong((long)dur.Representation, 5UL);
	m_EncoderBuffer->PutBitsLong((long)dur.Sign, 1UL);
	bits = 6;

	rep = dur.Representation;
	if (rep & 0x10) {		// days
		m_EncoderBuffer->PutBitsLong((long)dur.D, 16UL);
		bits += 16;
	}
	if (rep & 0x08) {		// hours
		m_EncoderBuffer->PutBitsLong((long)dur.h, 10UL);
		bits += 10;
	}
	if (rep & 0x04) {		// minutes
		m_EncoderBuffer->PutBitsLong((long)dur.m, 10UL);
		bits += 10;
	}
	if (rep & 0x02) {		// seconds
		m_EncoderBuffer->PutBitsLong((long)dur.s, 10UL);
		bits += 10;
	}
	if (rep & 0x01) {		// fractions
		m_EncoderBuffer->PutBitsLong((long)dur.nn, 16UL);
		m_EncoderBuffer->PutBitsLong((long)dur.nnFraction, 16UL);
		bits += 32;
	}

	return (bits);
}

//----------------------------------------------------------------------------
int TimeSeriesCS::OneColorLayoutEncoding(BitBuffer *theBuf, void *theD, int bitstuffNum)
{
	/* descriptor */
	ColorLayoutDescriptor *theDescriptor;
	ColorLayoutDescriptorInterfaceABC	*theDescriptorInterface;
	/* coding schemes*/
	ColorLayoutCS *theCS;
	ColorLayoutCSInterfaceABC *theCSInterface;

	theDescriptor = (ColorLayoutDescriptor *)theD;
	/* get color layout interface*/
	theDescriptorInterface= theDescriptor->GetInterface();
	/* connect descriptor to coding scheme*/
	theCS = new ColorLayoutCS(theDescriptorInterface);
	theCSInterface=theCS->GetInterface();

	/* connect stream to coding scheme*/
	theCSInterface->SetEncoderStreamBuffer(theBuf);
//	theDescriptorInterface->SetValuesFromParameterFile();

	/* code descriptor data*/
	theCSInterface->StartEncode();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
int TimeSeriesCS::BitLengthOfColorLayout(void)
{
	int		bits;
	/* descriptor */
	ColorLayoutDescriptor *theDescriptor;
	int nY, nC;

	theDescriptor = (ColorLayoutDescriptor *)GetDescriptorInterface()->GetContentDescriptor(0);
	nY = theDescriptor->GetNumberOfYCoeff();
	nC = theDescriptor->GetNumberOfCCoeff();

	if (nY == 6 && nC == 3) {
		bits = 1;
	} else {
		bits = 2;
	}

	bits += (3 * 6);
	bits += ((nY-1) + (nC-1) * 2) * 5;

	if (nY != 6 || (nC != 3 && nC != 6)) {
		bits += 6;
	}

	return (bits);
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int TimeSeriesCS::StartDecode(void)
{
  // content descriptor ID
  short		DescId;

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"TimeSeriesCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"TimeSeriesCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  DescId = m_DescriptorInterface->GetContentDescriptorId();
  switch (DescId) {
  case ID_ColorLayout:			// Color Layout Descriptor
    StartColorLayoutDecoding();
	break;
  // other descriptors will be added.
  default:
	  break;
  }
  // All done
  return 0;
}

//----------------------------------------------------------------------------
int TimeSeriesCS::StartColorLayoutDecoding(void)
{
	/* stream data base*/
	
	DecoderFileIO theStreamBuffer(0xFFFF);
	/* descriptor */
	ColorLayoutDescriptor *theDescriptor;
	ColorLayoutDescriptorInterfaceABC	*theDescriptorInterface;
	/* coding schemes*/
	ColorLayoutCS *theCS;
	ColorLayoutCSInterfaceABC *theCSInterface;

	int		nEntries;

	int		cnt;

    // Create a database of frame images to be processed
	theStreamBuffer.OpenBitStream("TimeSeries.mp7");

	nEntries = m_DescriptorInterface->GetNumOfContents();
	for (cnt = 0; cnt < nEntries; cnt++) {
		theDescriptor = (ColorLayoutDescriptor *)GetDescriptorInterface()->GetContentDescriptor(cnt);
		/* get color layout interface*/
		theDescriptorInterface= theDescriptor->GetInterface();
		/* connect descriptor to coding scheme*/
		theCS = new ColorLayoutCS(theDescriptorInterface);
		theCSInterface=theCS->GetInterface();

		/* connect stream to coding scheme*/
		theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
//		theDescriptorInterface->SetValuesFromParameterFile();

		/* code descriptor data*/
		theCSInterface->StartEncode();
		theCSInterface->destroy();
	}

	theStreamBuffer.CloseBitStream();

	return 0;
}

//----------------------------------------------------------------------------
TimeSeriesCSInterfaceABC *TimeSeriesCS::GetInterface(void)
{ return &m_Interface; }



