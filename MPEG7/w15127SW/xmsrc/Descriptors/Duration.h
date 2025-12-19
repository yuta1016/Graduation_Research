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
// Applicable File Name:  Duration.h
#ifndef __Duration__
#define __Duration__

typedef struct {
  bool negative;
  int No_D;
  int h;
  int m;
  int s;
  int nn;
} Duration;

typedef struct {
  int *nnFraction;
  int *TZD;
} DurationAttributes;

namespace XM
{

//=============================================================================
class DurationDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	DurationDescriptorInterfaceABC();
	virtual ~DurationDescriptorInterfaceABC() {}

	virtual Duration GetDescriptionDuration(void) = 0;
	virtual unsigned long SetDescriptionDuration(Duration) = 0;
	virtual int GetDurationElement(char index) = 0;
	virtual unsigned long SetDurationElement(char index,
					       int value) = 0;
	virtual int GetAttribute(char index);
	virtual unsigned long SetAttribute(char index,
					       int value);
	static const UUID myID;
};

class DurationDescriptor;

//=============================================================================
class DurationDescriptorInterface: public DurationDescriptorInterfaceABC
{
public:
	DurationDescriptorInterface(DurationDescriptor *aDuration);
	virtual ~DurationDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual Duration GetDescriptionDuration(void);
	virtual unsigned long SetDescriptionDuration(Duration);	
	virtual int GetDurationElement(char index);
	virtual unsigned long SetDurationElement(char index,
					       int value);
	virtual int GetAttribute(char index);
	virtual unsigned long SetAttribute(char index,
					       int value);
	static const UUID myID;
	static const char *myName;


private:
	DurationDescriptor *m_DurationDescriptor;	
};

//=============================================================================
class DurationDescriptor: public Descriptor
{
public:
	DurationDescriptor();
	virtual ~DurationDescriptor();


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

	virtual DurationDescriptorInterfaceABC *GetInterface(void);

	// accessor methods

	virtual Duration GetDescriptionDuration(void);
	virtual unsigned long SetDescriptionDuration(Duration);	

	virtual int GetDurationElement(char index);
	virtual unsigned long SetDurationElement(char index,
					       int value);
	virtual int GetAttribute(char index);
	virtual unsigned long SetAttribute(char index,
					       int value);
private:

	DurationDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	Duration  m_DescriptionDuration;
	DurationAttributes m_attributesDuration;
};

};

#endif //__Duration__
