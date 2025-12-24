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
// Applicable File Name:  SpokenContentBlock.h
#ifndef __SPOKENCONTENTBlock_TYPE__
#define __SPOKENCONTENTBlock_TYPE__

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
class SpokenContentBlockDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentBlockDescriptorInterfaceABC();
        virtual ~SpokenContentBlockDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

        virtual void SetMediaTime(const char* t) = 0;
        virtual const char* GetMediaTime(void) const = 0;
        virtual void SetDefaultSpeaker(const char* s) = 0;
        virtual const char* GetDefaultSpeaker(void) const = 0;
        virtual void SetNNodes(const unsigned short n) = 0;
        virtual const unsigned short GetNNodes(void) const = 0;
        virtual void SetNum(const unsigned short n) = 0;
        virtual const unsigned short GetNum(void) const = 0;
        virtual const char* GetAudio(void) const = 0;
        virtual void SetAudio(const char* a) = 0;
        virtual void SetNode(const unsigned short idx,
                             SpokenContentNodeDescriptor* n) = 0;
        virtual SpokenContentNodeDescriptor* GetNode(const unsigned short idx) = 0;

        virtual void Reset(void) = 0;
        virtual void Display(void) const = 0;
};

class SpokenContentBlockDescriptor;

//=============================================================================
class SpokenContentBlockDescriptorInterface: public SpokenContentBlockDescriptorInterfaceABC
{
public:
	SpokenContentBlockDescriptorInterface(SpokenContentBlockDescriptor *aHisto);
	virtual ~SpokenContentBlockDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

        virtual void SetMediaTime(const char* t);
        virtual const char* GetMediaTime(void) const;
        virtual void SetDefaultSpeaker(const char* s);
        virtual const char* GetDefaultSpeaker(void) const;
        virtual void SetNNodes(const unsigned short n);
        virtual const unsigned short GetNNodes(void) const;
        virtual void SetNum(const unsigned short n);
        virtual const unsigned short GetNum(void) const;
        virtual const char* GetAudio(void) const;
        virtual void SetAudio(const char* a);
        virtual void SetNode(const unsigned short idx,
                             SpokenContentNodeDescriptor* n);
        virtual SpokenContentNodeDescriptor* GetNode(const unsigned short idx);
        virtual void Reset(void);
        virtual void Display(void) const;


	static const UUID myID;
	static const char *myName;


private:
	SpokenContentBlockDescriptor *m_SpokenContentBlockDescriptor;
};

//=============================================================================
class SpokenContentBlockDescriptor: public Descriptor
{
friend class SpokenContentBlockDescriptorInterface;
public:
	SpokenContentBlockDescriptor();

        // This does the copy constructor
	SpokenContentBlockDescriptor(SpokenContentBlockDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentBlockDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

        virtual void SetMediaTime(const char* t);
        virtual const char* GetMediaTime(void) const;
        virtual void SetDefaultSpeaker(const char* s);
        virtual const char* GetDefaultSpeaker(void) const;
        virtual void SetNNodes(const unsigned short n);
        virtual const unsigned short GetNNodes(void) const;
        virtual void SetNum(const unsigned short n);
        virtual const unsigned short GetNum(void) const;
        virtual const char* GetAudio(void) const;
        virtual void SetAudio(const char* a);
        virtual void SetNode(const unsigned short idx,
                             SpokenContentNodeDescriptor* n);
        virtual SpokenContentNodeDescriptor* GetNode(const unsigned short idx);
        virtual void Reset(void);
        virtual void Display(void) const;


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentBlockDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentBlockDescriptor();
	
	unsigned long m_refcount;

	SpokenContentBlockDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  char*          mediatime;
  unsigned short nnodes;
  unsigned short num;
  char*          audio;
  SpokenContentNodeDescriptor** nodes;
  char*          defaultSpeaker;
};

};

//#endif // USEXML
#endif //__SPOKENCONTENTBlock_TYPE__
