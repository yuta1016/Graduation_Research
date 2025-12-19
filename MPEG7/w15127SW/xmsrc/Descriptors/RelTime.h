///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, J. Heuer
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  RelTime.h
#ifndef __RelTime__
#define __RelTime__
#include "Duration.h"
#include "TimePoint.h"

typedef Duration RelTime;
typedef DurationAttributes RelTimeAttributes;

namespace XM
{

//=============================================================================
class RelTimeDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	RelTimeDescriptorInterfaceABC();
	virtual ~RelTimeDescriptorInterfaceABC() {}

	virtual RelTime GetDescriptionRelTime(void) = 0;
	virtual unsigned long SetDescriptionRelTime(RelTime) = 0;
	virtual int GetRelTimeElement(char index) = 0;
	virtual unsigned long SetRelTimeElement(char index,
					       int value) = 0;
	virtual int GetAttribute(char index);
	virtual unsigned long SetAttribute(char index,
					       int value);

	virtual TimePointDescriptorInterfaceABC *GetTimeBaseInterface(void);
	virtual unsigned long SetTimeBaseInterface(TimePointDescriptorInterfaceABC *);
	static const UUID myID;
};

class RelTimeDescriptor;

//=============================================================================
class RelTimeDescriptorInterface: public RelTimeDescriptorInterfaceABC
{
public:
	RelTimeDescriptorInterface(RelTimeDescriptor *aRelTime);
	virtual ~RelTimeDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual RelTime GetDescriptionRelTime(void);
	virtual unsigned long SetDescriptionRelTime(RelTime);	
	virtual int GetRelTimeElement(char index);
	virtual unsigned long SetRelTimeElement(char index,
					       int value);
	virtual int GetAttribute(char index);
	virtual unsigned long SetAttribute(char index,
					       int value);

	virtual TimePointDescriptorInterfaceABC *GetTimeBaseInterface(void);
	virtual unsigned long SetTimeBaseInterface(TimePointDescriptorInterfaceABC *);
	static const UUID myID;
	static const char *myName;


private:
	RelTimeDescriptor *m_RelTimeDescriptor;	
};

//=============================================================================
class RelTimeDescriptor: public Descriptor
{
public:
	RelTimeDescriptor();
	virtual ~RelTimeDescriptor();


	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual RelTimeDescriptorInterfaceABC *GetInterface(void);

	// accessor methods

	virtual RelTime GetDescriptionRelTime(void);
	virtual unsigned long SetDescriptionRelTime(RelTime);	

	virtual int GetRelTimeElement(char index);
	virtual unsigned long SetRelTimeElement(char index,
					       int value);
	virtual int GetAttribute(char index);
	virtual unsigned long SetAttribute(char index,
					       int value);

	virtual TimePointDescriptorInterfaceABC *GetTimeBaseInterface(void);
	virtual unsigned long SetTimeBaseInterface(TimePointDescriptorInterfaceABC *);

private:

	RelTimeDescriptorInterface m_Interface;
	
	//const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	RelTime  m_DescriptionRelTime;
	RelTimeAttributes m_attributesRelTime;
	TimePointDescriptorInterfaceABC *m_TimeBaseInterface;
};

};

#endif //__RelTime__
