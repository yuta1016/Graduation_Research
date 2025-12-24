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
// Applicable File Name:  SpokenContentIndexEntry.h
#ifndef __SPOKENCONTENTINDEXENTRY_TYPE__
#define __SPOKENCONTENTINDEXENTRY_TYPE__

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
class SpokenContentIndexEntryDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentIndexEntryDescriptorInterfaceABC();
        virtual ~SpokenContentIndexEntryDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

  // Get the contents of the class
  virtual const unsigned short GetNode(void) const=0;
  virtual const unsigned long GetBlock(void) const=0;
  virtual const char* GetLattice(void) const=0;
  virtual void SetNode(const unsigned short n)=0;
  virtual void SetBlock(const unsigned long b)=0;
  virtual void SetLattice(const char* l)=0;
  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class SpokenContentIndexEntryDescriptor;

//=============================================================================
class SpokenContentIndexEntryDescriptorInterface: public SpokenContentIndexEntryDescriptorInterfaceABC
{
public:
	SpokenContentIndexEntryDescriptorInterface(SpokenContentIndexEntryDescriptor *aHisto);
	virtual ~SpokenContentIndexEntryDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const unsigned short GetNode(void) const;
  virtual const unsigned long GetBlock(void) const;
  virtual const char* GetLattice(void) const;
  virtual void SetNode(const unsigned short n);
  virtual void SetBlock(const unsigned long b);
  virtual void SetLattice(const char* l);
  virtual void Reset(void);
  virtual void Display(void) const;

	static const UUID myID;
	static const char *myName;


private:
	SpokenContentIndexEntryDescriptor *m_SpokenContentIndexEntryDescriptor;
};

//=============================================================================
class SpokenContentIndexEntryDescriptor: public Descriptor
{
friend class SpokenContentIndexEntryDescriptorInterface;
public:
	SpokenContentIndexEntryDescriptor();

        // This does the copy constructor
	SpokenContentIndexEntryDescriptor(SpokenContentIndexEntryDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentIndexEntryDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const unsigned short GetNode(void) const;
  virtual const unsigned long GetBlock(void) const;
  virtual const char* GetLattice(void) const;
  virtual void SetNode(const unsigned short n);
  virtual void SetBlock(const unsigned long b);
  virtual void SetLattice(const char* l);
  virtual void Reset(void);
  virtual void Display(void) const;

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentIndexEntryDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentIndexEntryDescriptor();
	
	unsigned long m_refcount;

	SpokenContentIndexEntryDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  // A person name contains a langugage attribute and a vector of given
  // names, a vector of family names and a vector of person names.
        unsigned short node;
        unsigned long block;
        char* lattice;
};

};

//#endif // USEXML
#endif //__SPOKENCONTENTINDEXENTRY_TYPE__
