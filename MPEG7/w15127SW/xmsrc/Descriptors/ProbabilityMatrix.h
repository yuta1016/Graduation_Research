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
// Applicable File Name:  ProbabilityMatrix.h
#ifndef __ProbabilityMatrix_TYPE__
#define __ProbabilityMatrix_TYPE__

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
class ProbabilityMatrixDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ProbabilityMatrixDescriptorInterfaceABC();
        virtual ~ProbabilityMatrixDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

  // Get the contents of the class
  virtual void SetSize(const unsigned long r,const unsigned long c) = 0;
  virtual void SetValue(const float p,const unsigned long r,const unsigned long c) = 0;
  virtual const unsigned long GetRows(void) const = 0;
  virtual const unsigned long GetCols(void) const = 0;
  virtual const float GetValue(const unsigned long r,const unsigned long c) const = 0;
  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class ProbabilityMatrixDescriptor;

//=============================================================================
class ProbabilityMatrixDescriptorInterface: public ProbabilityMatrixDescriptorInterfaceABC
{
public:
	ProbabilityMatrixDescriptorInterface(ProbabilityMatrixDescriptor *aHisto);
	virtual ~ProbabilityMatrixDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();


	static const UUID myID;
	static const char *myName;

  // Get the contents of the class
  virtual void SetSize(const unsigned long r,const unsigned long c);
  virtual void SetValue(const float p,const unsigned long r,const unsigned long c);
  virtual const unsigned long GetRows(void) const;
  virtual const unsigned long GetCols(void) const;
  virtual const float GetValue(const unsigned long r,const unsigned long c) const;
  virtual void Reset(void);
  virtual void Display(void) const;


private:
	ProbabilityMatrixDescriptor *m_ProbabilityMatrixDescriptor;
};

//=============================================================================
class ProbabilityMatrixDescriptor: public Descriptor
{
friend class ProbabilityMatrixDescriptorInterface;
public:
	ProbabilityMatrixDescriptor();

        // This does the copy constructor
	ProbabilityMatrixDescriptor(ProbabilityMatrixDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        ProbabilityMatrixDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual void SetSize(const unsigned long r,const unsigned long c);
  virtual void SetValue(const float p,const unsigned long r,const unsigned long c);
  virtual const unsigned long GetRows(void) const;
  virtual const unsigned long GetCols(void) const;
  virtual const float GetValue(const unsigned long r,const unsigned long c) const;
  virtual void Reset(void);
  virtual void Display(void) const;


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ProbabilityMatrixDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~ProbabilityMatrixDescriptor();
	
	unsigned long m_refcount;

	ProbabilityMatrixDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  // A person name contains a langugage attribute and a vector of given
  // names, a vector of family names and a vector of person names.
    unsigned long rows,cols;
    float* value;
};

};

//#endif // USEXML
#endif //__ProbabilityMatrix_TYPE__
