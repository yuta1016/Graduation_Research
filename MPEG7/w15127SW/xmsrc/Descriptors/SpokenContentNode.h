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
// Applicable File Name:  SpokenContentNode.h
#ifndef __SPOKENCONTENTNODE_TYPE__
#define __SPOKENCONTENTNODE_TYPE__

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
class SpokenContentNodeDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentNodeDescriptorInterfaceABC();
        virtual ~SpokenContentNodeDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

  // Handle the contents of the class
  virtual const unsigned char GetNWordLinks(void) const = 0;
  virtual SpokenContentWordLinkDescriptor* GetWordLink(const unsigned char idx) = 0;
  virtual void SetNWordLinks(const unsigned char n) = 0;
  virtual void SetWordLink(const unsigned char idx,SpokenContentWordLinkDescriptor *l) = 0;
  virtual const unsigned char GetNPhoneLinks(void) const = 0;
  virtual SpokenContentPhoneLinkDescriptor* GetPhoneLink(const unsigned char idx) = 0;
  virtual void SetNPhoneLinks(const unsigned char n) = 0;
  virtual void SetPhoneLink(const unsigned char idx,SpokenContentPhoneLinkDescriptor *l) = 0;
  virtual SpokenContentPhoneLinkDescriptor* FindPhoneLink(const char* phone)=0;

  virtual const unsigned short GetNum(void) const = 0;
  virtual void SetNum(const unsigned short n) = 0;
  virtual const unsigned short GetTimeOffset(void) const = 0;
  virtual void SetTimeOffset(const unsigned short t) = 0;
  virtual const char* GetSpeaker(void) const = 0;
  virtual void SetSpeaker(const char* s) = 0;

  virtual const float GetWordProb(const char* w) const = 0;
  virtual const float GetPhoneProb(const char* p) const = 0;

  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class SpokenContentNodeDescriptor;

//=============================================================================
class SpokenContentNodeDescriptorInterface: public SpokenContentNodeDescriptorInterfaceABC
{
public:
	SpokenContentNodeDescriptorInterface(SpokenContentNodeDescriptor *aHisto);
	virtual ~SpokenContentNodeDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  virtual const unsigned char GetNWordLinks(void) const;
  virtual SpokenContentWordLinkDescriptor* GetWordLink(const unsigned char idx);
  virtual void SetNWordLinks(const unsigned char n);
  virtual void SetWordLink(const unsigned char idx,SpokenContentWordLinkDescriptor *l);
  virtual const unsigned char GetNPhoneLinks(void) const;
  virtual SpokenContentPhoneLinkDescriptor* GetPhoneLink(const unsigned char idx);
  virtual void SetNPhoneLinks(const unsigned char n);
  virtual void SetPhoneLink(const unsigned char idx,SpokenContentPhoneLinkDescriptor *l);
  virtual SpokenContentPhoneLinkDescriptor* FindPhoneLink(const char* phone);

  virtual const unsigned short GetNum(void) const;
  virtual void SetNum(const unsigned short n);
  virtual const unsigned short GetTimeOffset(void) const;
  virtual void SetTimeOffset(const unsigned short t);
  virtual const char* GetSpeaker(void) const;
  virtual void SetSpeaker(const char* s);
  virtual const float GetWordProb(const char* w) const;
  virtual const float GetPhoneProb(const char* p) const;

  virtual void Reset(void);
  virtual void Display(void) const;


	static const UUID myID;
	static const char *myName;


private:
	SpokenContentNodeDescriptor *m_SpokenContentNodeDescriptor;
};

//=============================================================================
class SpokenContentNodeDescriptor: public Descriptor
{
friend class SpokenContentNodeDescriptorInterface;
public:
	SpokenContentNodeDescriptor();

        // This does the copy constructor
	SpokenContentNodeDescriptor(SpokenContentNodeDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentNodeDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Contents of the class
  virtual const unsigned char GetNWordLinks(void) const;
  virtual SpokenContentWordLinkDescriptor* GetWordLink(const unsigned char idx);
  virtual void SetNWordLinks(const unsigned char n);
  virtual void SetWordLink(const unsigned char idx,SpokenContentWordLinkDescriptor *l);
  virtual const unsigned char GetNPhoneLinks(void) const;
  virtual SpokenContentPhoneLinkDescriptor* GetPhoneLink(const unsigned char idx);
  virtual void SetNPhoneLinks(const unsigned char n);
  virtual void SetPhoneLink(const unsigned char idx,SpokenContentPhoneLinkDescriptor *l);
  virtual SpokenContentPhoneLinkDescriptor* FindPhoneLink(const char* phone);

  virtual const unsigned short GetNum(void) const;
  virtual void SetNum(const unsigned short n);
  virtual const unsigned short GetTimeOffset(void) const;
  virtual void SetTimeOffset(const unsigned short t);
  virtual const char* GetSpeaker(void) const;
  virtual void SetSpeaker(const char* s);
  virtual const float GetWordProb(const char* w) const;
  virtual const float GetPhoneProb(const char* p) const;

  virtual void Reset(void);
  virtual void Display(void) const;

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentNodeDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentNodeDescriptor();
	
	unsigned long m_refcount;

	SpokenContentNodeDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  unsigned char nwordlinks;
  unsigned char nphonelinks;
  unsigned short num;
  unsigned short timeoffset;
  char* speaker;
  SpokenContentWordLinkDescriptor**  wordlinks;
  SpokenContentPhoneLinkDescriptor** phonelinks;

};

};

//#endif // USEXML
#endif //__SPOKENCONTENTNODE_TYPE__
