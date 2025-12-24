///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  SpokenContentConfusionInfo.h
#ifndef __SPOKENCONTENTCONFUSIONINFO_TYPE__
#define __SPOKENCONTENTCONFUSIONINFO_TYPE__

// Only include if using xml
//#ifdef USEXML

#ifndef _STDIO_H
#include <stdio.h> 
#endif
#ifndef _STDLIB_H
#include <stdlib.h> 
#endif
#ifndef _ASSERT_H
#include <assert.h>
#endif

namespace XM
{

//=============================================================================
class SpokenContentConfusionInfoDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentConfusionInfoDescriptorInterfaceABC();
        virtual ~SpokenContentConfusionInfoDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

  // Get the contents of the class
  virtual const unsigned short GetSize(void) const = 0;
  virtual ProbabilityVectorDescriptor* GetOccuranceProb(void) = 0;
  virtual ProbabilityVectorDescriptor* GetInsertionProb(void) = 0;
  virtual const float GetUnconditionalInsertionProb(void) const = 0;
  virtual ProbabilityVectorDescriptor* GetOmissionProb(void) = 0;
  virtual ProbabilityMatrixDescriptor* GetConfusionProb(void) = 0;

  // Set the contents of the clas
  virtual void SetSize(const unsigned short n) = 0;
  virtual void SetOccuranceProb(ProbabilityVectorDescriptor* v) = 0;
  virtual void SetInsertionProb(ProbabilityVectorDescriptor* v) = 0;
  virtual void SetUnconditionalInsertionProb(const float p) = 0;
  virtual void SetOmissionProb(ProbabilityVectorDescriptor* v) = 0;
  virtual void SetConfusionProb(ProbabilityMatrixDescriptor* m) = 0;

  virtual const char* GetConfusionID(void) const = 0;
  virtual void SetConfusionID(const char* s) = 0;

  // Sundry
  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class SpokenContentConfusionInfoDescriptor;

//=============================================================================
class SpokenContentConfusionInfoDescriptorInterface: public SpokenContentConfusionInfoDescriptorInterfaceABC
{
public:
	SpokenContentConfusionInfoDescriptorInterface(SpokenContentConfusionInfoDescriptor *aHisto);
	virtual ~SpokenContentConfusionInfoDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const unsigned short GetSize(void) const;
  virtual ProbabilityVectorDescriptor* GetOccuranceProb(void);
  virtual ProbabilityVectorDescriptor* GetInsertionProb(void);
  virtual const float GetUnconditionalInsertionProb(void) const;
  virtual ProbabilityVectorDescriptor* GetOmissionProb(void);
  virtual ProbabilityMatrixDescriptor* GetConfusionProb(void);

  // Set the contents of the class
  virtual void SetSize(const unsigned short n);
  virtual void SetOccuranceProb(ProbabilityVectorDescriptor* v);
  virtual void SetInsertionProb(ProbabilityVectorDescriptor* v);
  virtual void SetUnconditionalInsertionProb(const float p);
  virtual void SetOmissionProb(ProbabilityVectorDescriptor* v);
  virtual void SetConfusionProb(ProbabilityMatrixDescriptor* m);

  virtual const char* GetConfusionID(void) const;
  virtual void SetConfusionID(const char* s);

  // Sundry
  virtual void Reset(void);
  virtual void Display(void) const;

	static const UUID myID;
	static const char *myName;


private:
	SpokenContentConfusionInfoDescriptor *m_SpokenContentConfusionInfoDescriptor;
};

//=============================================================================
class SpokenContentConfusionInfoDescriptor: public Descriptor
{
friend class SpokenContentConfusionInfoDescriptorInterface;
public:
	SpokenContentConfusionInfoDescriptor();

        // This does the copy constructor
	SpokenContentConfusionInfoDescriptor(SpokenContentConfusionInfoDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentConfusionInfoDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const unsigned short GetSize(void) const;
  virtual ProbabilityVectorDescriptor* GetOccuranceProb(void);
  virtual ProbabilityVectorDescriptor* GetInsertionProb(void);
  virtual const float GetUnconditionalInsertionProb(void) const;
  virtual ProbabilityVectorDescriptor* GetOmissionProb(void);
  virtual ProbabilityMatrixDescriptor* GetConfusionProb(void);

  // Set the contents of the clas
  virtual void SetSize(const unsigned short n);
  virtual void SetOccuranceProb(ProbabilityVectorDescriptor* v);
  virtual void SetInsertionProb(ProbabilityVectorDescriptor* v);
  virtual void SetUnconditionalInsertionProb(const float p);
  virtual void SetOmissionProb(ProbabilityVectorDescriptor* v);
  virtual void SetConfusionProb(ProbabilityMatrixDescriptor* m);

  virtual const char* GetConfusionID(void) const;
  virtual void SetConfusionID(const char* s);

  // Sundry
  virtual void Reset(void);
  virtual void Display(void) const;

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentConfusionInfoDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentConfusionInfoDescriptor();
	
	unsigned long m_refcount;

	SpokenContentConfusionInfoDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  unsigned short size;
  ProbabilityVectorDescriptor* occurance;
  ProbabilityVectorDescriptor* insertion;
  float unconditional;
  ProbabilityVectorDescriptor* omission;
  ProbabilityMatrixDescriptor* confusion;
  char* confusionId;

};

};

//#endif // USEXML
#endif //__SPOKENCONTENTCONFUSIONINFO_TYPE__
