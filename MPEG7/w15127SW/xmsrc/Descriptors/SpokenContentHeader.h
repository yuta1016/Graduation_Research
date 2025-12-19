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
// Applicable File Name:  SpokenContentHeader.h
#ifndef __SPOKENCONTENTHEADER_TYPE__
#define __SPOKENCONTENTHEADER_TYPE__

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
class SpokenContentHeaderDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentHeaderDescriptorInterfaceABC();
        virtual ~SpokenContentHeaderDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	static const UUID myID;

  // Get the contents of the class
  virtual const unsigned long GetNWordLexicon(void) const = 0;
  virtual SpokenContentWordLexiconDescriptor* GetWordLexicon(const unsigned long idx) = 0;
  virtual void SetNWordLexicon(const unsigned long n) = 0;
  virtual void SetWordLexicon(const unsigned long idx,
                              SpokenContentWordLexiconDescriptor* l) = 0;
  virtual const unsigned long GetNPhoneLexicon(void) const = 0;
  virtual SpokenContentPhoneLexiconDescriptor* GetPhoneLexicon(const unsigned long idx) = 0;
  virtual void SetNPhoneLexicon(const unsigned long n) = 0;
  virtual void SetPhoneLexicon(const unsigned long idx,
                              SpokenContentPhoneLexiconDescriptor* l) = 0;
  virtual const unsigned long GetNSpeaker(void) const = 0;
  virtual SpokenContentSpeakerDescriptor* GetSpeaker(const unsigned long idx) = 0;
  virtual void SetNSpeaker(const unsigned long n) = 0;

  virtual void SetNConfusions(const unsigned long n) = 0;
  virtual unsigned long GetNConfusions(void) const = 0;
  virtual void SetConfusion(const unsigned long idx,
                            SpokenContentConfusionInfoDescriptor* l) = 0;
  virtual long FindConfusion(const char* refid) const = 0;
  virtual SpokenContentConfusionInfoDescriptor* GetConfusion(const unsigned long idx) = 0;

  virtual void SetSpeaker(const unsigned long idx,
                              SpokenContentSpeakerDescriptor* s) = 0;
  virtual long FindSpeaker(const char* refid) const = 0;
  virtual long FindWordLexicon(const char* refid) const = 0;
  virtual long FindPhoneLexicon(const char* refid) const = 0;

  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class SpokenContentHeaderDescriptor;

//=============================================================================
class SpokenContentHeaderDescriptorInterface: public SpokenContentHeaderDescriptorInterfaceABC
{
public:
	SpokenContentHeaderDescriptorInterface(SpokenContentHeaderDescriptor *aHisto);
	virtual ~SpokenContentHeaderDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  virtual const unsigned long GetNWordLexicon(void) const;
  virtual SpokenContentWordLexiconDescriptor* GetWordLexicon(const unsigned long idx);
  virtual void SetNWordLexicon(const unsigned long n);
  virtual void SetWordLexicon(const unsigned long idx,
                              SpokenContentWordLexiconDescriptor* l);
  virtual const unsigned long GetNPhoneLexicon(void) const;
  virtual SpokenContentPhoneLexiconDescriptor* GetPhoneLexicon(const unsigned long idx);
  virtual void SetNPhoneLexicon(const unsigned long n);
  virtual void SetPhoneLexicon(const unsigned long idx,
                              SpokenContentPhoneLexiconDescriptor* l);
  virtual const unsigned long GetNSpeaker(void) const;
  virtual SpokenContentSpeakerDescriptor* GetSpeaker(const unsigned long idx);
  virtual void SetNSpeaker(const unsigned long n);
  virtual void SetSpeaker(const unsigned long idx,
                              SpokenContentSpeakerDescriptor* s);
  virtual long FindSpeaker(const char* refid) const;
  virtual long FindWordLexicon(const char* refid) const;
  virtual long FindPhoneLexicon(const char* refid) const;

  virtual unsigned long GetNConfusions(void) const;
  virtual void SetNConfusions(const unsigned long n);
  virtual void SetConfusion(const unsigned long idx,
                            SpokenContentConfusionInfoDescriptor* l);
  virtual long FindConfusion(const char* refid) const;
  virtual SpokenContentConfusionInfoDescriptor* GetConfusion(const unsigned long idx);

  virtual void Reset(void);
  virtual void Display(void) const;


	static const UUID myID;
	static const char *myName;


private:
	SpokenContentHeaderDescriptor *m_SpokenContentHeaderDescriptor;
};

//=============================================================================
class SpokenContentHeaderDescriptor: public Descriptor
{
friend class SpokenContentHeaderDescriptorInterface;
public:
	SpokenContentHeaderDescriptor();

        // This does the copy constructor
	SpokenContentHeaderDescriptor(SpokenContentHeaderDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentHeaderDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

  // Get the contents of the class
  virtual const unsigned long GetNWordLexicon(void) const;
  virtual SpokenContentWordLexiconDescriptor* GetWordLexicon(const unsigned long idx);
  virtual void SetNWordLexicon(const unsigned long n);
  virtual void SetWordLexicon(const unsigned long idx,
                              SpokenContentWordLexiconDescriptor* l);
  virtual const unsigned long GetNPhoneLexicon(void) const;
  virtual SpokenContentPhoneLexiconDescriptor* GetPhoneLexicon(const unsigned long idx);
  virtual void SetNPhoneLexicon(const unsigned long n);
  virtual void SetPhoneLexicon(const unsigned long idx,
                              SpokenContentPhoneLexiconDescriptor* l);
  virtual const unsigned long GetNSpeaker(void) const;
  virtual SpokenContentSpeakerDescriptor* GetSpeaker(const unsigned long idx);
  virtual void SetNSpeaker(const unsigned long n);
  virtual void SetSpeaker(const unsigned long idx,
                              SpokenContentSpeakerDescriptor* s);
  virtual long FindSpeaker(const char* refid) const;
  virtual long FindWordLexicon(const char* refid) const;
  virtual long FindPhoneLexicon(const char* refid) const;

  virtual unsigned long GetNConfusions(void) const;
  virtual void SetNConfusions(const unsigned long n);
  virtual void SetConfusion(const unsigned long idx,
                            SpokenContentConfusionInfoDescriptor* l);
  virtual long FindConfusion(const char* refid) const;
  virtual SpokenContentConfusionInfoDescriptor* GetConfusion(const unsigned long idx);

  virtual void Reset(void);
  virtual void Display(void) const;

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentHeaderDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentHeaderDescriptor();
	
	unsigned long m_refcount;

	SpokenContentHeaderDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


  // ------------------------------------------------------------------
  unsigned long nwordlexicon;
  unsigned long nphonelexicon;
  unsigned long nspeaker;
  unsigned long nconfusions;

  SpokenContentWordLexiconDescriptor** wordlexicon;
  SpokenContentPhoneLexiconDescriptor** phonelexicon;
  SpokenContentSpeakerDescriptor** speaker;
  SpokenContentConfusionInfoDescriptor** confusions;

};

};

//#endif // USEXML
#endif //__SPOKENCONTENTHEADER_TYPE__
