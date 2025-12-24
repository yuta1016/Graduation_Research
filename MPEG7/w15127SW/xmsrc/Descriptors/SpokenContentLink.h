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
// Applicable File Name:  SpokenContentLink.h
#ifndef __SPOKENCONTENTLINK_TYPE__
#define __SPOKENCONTENTLINK_TYPE__

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
class SpokenContentLinkDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentLinkDescriptorInterfaceABC();
        virtual ~SpokenContentLinkDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

  // Get the contents of the class
  virtual const float GetProbability(void) const =0;
  virtual const unsigned short GetNodeOffset(void) const=0;
  virtual void SetProbability(const float p)=0;
  virtual void SetNodeOffset(const unsigned short n)=0;
  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class SpokenContentLinkDescriptor;

//=============================================================================
class SpokenContentLinkDescriptorInterface: public SpokenContentLinkDescriptorInterfaceABC
{
public:
	SpokenContentLinkDescriptorInterface(SpokenContentLinkDescriptor *aHisto);
	virtual ~SpokenContentLinkDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const float GetProbability(void) const;
  virtual const unsigned short GetNodeOffset(void) const;
  virtual void SetProbability(const float p);
  virtual void SetNodeOffset(const unsigned short n);
  virtual void Reset(void);

  virtual void Display(void) const;

	static const UUID myID;
	static const char *myName;


private:
	SpokenContentLinkDescriptor *m_SpokenContentLinkDescriptor;
};

//=============================================================================
class SpokenContentLinkDescriptor: public Descriptor
{
friend class SpokenContentLinkDescriptorInterface;
public:
	SpokenContentLinkDescriptor();

        // This does the copy constructor
	SpokenContentLinkDescriptor(SpokenContentLinkDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentLinkDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const float GetProbability(void) const;
  virtual const unsigned short GetNodeOffset(void) const;
  virtual void SetProbability(const float p);
  virtual void SetNodeOffset(const unsigned short n);
  virtual void Reset(void);

  virtual void Display(void) const;

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentLinkDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentLinkDescriptor();
	
	unsigned long m_refcount;

	SpokenContentLinkDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  float probability;
  unsigned short node;
};

};

//#endif // USEXML
#endif //__SPOKENCONTENTLINK_TYPE__
