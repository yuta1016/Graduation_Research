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
// Applicable File Name:  TimeSeriesSAppl.h
//
#ifndef __TIME_SERIES_SERVER__
#define __TIME_SERIES_SERVER__


#include "BasicArchitecture/XMInterfaces.h" /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"    /* Naming Convention */

#include "Media/Media.h"
#include "Descriptors/Descriptors.h"

// additional parameters
typedef struct {
	short		nFrameRateCode;
	char		sTimeSeriesType[16];
	//char		sTimeIncr[32];
	//char		sOffset[32];
	int			nDuration;
	int			nTimeIncr;
	int			nOffset;
} AddtParams;

namespace XM
{

//=============================================================================
// This class creates the binary representation of the metadata for the 
// dummy type. 
// 
// For example purposes, each version of the dummy type consists of a
// signed long integer. Thus a library of such dtypes will consists of
// a set of files each containing one (textual representation of an) 
// integer. These are read in and stored in binary form. 
class TimeSeriesServer: public ServerApplication
{
public:
  // Null constructor
  TimeSeriesServer();

  // Destructor
  virtual ~TimeSeriesServer();

  // not used to create descriptor data base
  virtual unsigned long Open();

  // This appears to be the main processing task. It takes in
  // a list of files which should be processed and written out to the
  // file supplied by 'bitstream'.
  // 
  //sth interface violation
  //virtual unsigned long Start(short DescId, char *ListFile, char *Bitstream, char *AddtParamListFile);
  virtual unsigned long Start(char *ListFile, char *Bitstream);

  // not used to create descriptor data base
  virtual unsigned long Stop();

  // not used to create descriptor data base
  virtual unsigned long Close();

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  // Access is allowed only by class factories for this object.  This 
  // avoids having to duplicate the ID definition in multiple locations.
  // In the future, we may have to do this.  PLEASE DO NOT USE THESE
  // UNLESS YOU ARE IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;
private:
	// read additional parameters
	virtual bool ReadAdditionalParameters(char *parameterfile, AddtParams *a_params, int nEntries);

	// create a list of input frames
	virtual bool CreateInputList(TimeSeriesDescriptorInterfaceABC* theDescriptorInterface, AddtParams *a_param, const char *in_path, int *nEntries);


	unsigned long CLStart(int nEntries, char *Bitstream, MediaDB *ramDB);



	// the ID of the descriptor accomodated in the TimeSeries
	short	DescriptorId;

}; // End class definition

}; // End namespace definition

#endif //__TIME_SERIES_SERVER__
