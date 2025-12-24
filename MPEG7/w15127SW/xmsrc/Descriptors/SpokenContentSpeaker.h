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
// Applicable File Name:  SpokenContentSpeaker.h
#ifndef __SPOKENCONTENTSPEAKER_TYPE__
#define __SPOKENCONTENTSPEAKER_TYPE__

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
class SpokenContentSpeakerDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  SpokenContentSpeakerDescriptorInterfaceABC();
  virtual ~SpokenContentSpeakerDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  // Get the contents of the class
  // PNG virtual IndividualDescriptor* GetIndividual(void) = 0;
  // PNG virtual SpokenContentExtractionInfoDescriptor* GetExtractionInfo(void) = 0;
  // PNG virtual void SetExtractionInfo(SpokenContentExtractionInfoDescriptor* i) = 0;
  virtual const unsigned long GetNWordIndex(void) const = 0;
  virtual const unsigned long GetNPhoneIndex(void) const = 0;
  virtual SpokenContentWordIndexEntryDescriptor* GetWordIndex(const unsigned long idx) = 0;
  virtual SpokenContentPhoneIndexEntryDescriptor* GetPhoneIndex(const unsigned long idx) = 0;
  virtual const char* GetWordLexiconID(void) const = 0;
  virtual const char* GetPhoneLexiconID(void) const = 0;
  virtual const char* GetLang(void) const = 0;
  virtual const char* GetProvenance(void) const = 0;
  virtual SpokenContentWordIndexEntryDescriptor* FindWordIndexEntry(const unsigned char n,const char** x) = 0;
  virtual SpokenContentPhoneIndexEntryDescriptor* FindPhoneIndexEntry(const unsigned char n,const char** x) = 0;
  virtual const char* GetDefaultWordLatticeId(void) const = 0;
  virtual const char* GetDefaultPhoneLatticeId(void) const = 0;

  // Set the contents of the class
  //  virtual void SetIndividual(IndividualDescriptor* i) = 0;
  virtual void SetNWordIndex(const unsigned long) = 0;
  virtual void SetNPhoneIndex(const unsigned long) = 0;
  virtual void SetWordIndex(const unsigned long idx,SpokenContentWordIndexEntryDescriptor* w) = 0;
  virtual void SetPhoneIndex(const unsigned long idx,SpokenContentPhoneIndexEntryDescriptor* p) = 0;
  virtual void SetWordLexiconID(const char* d) = 0;
  virtual void SetPhoneLexiconID(const char* d) = 0;
  virtual void SetLang(const char* d) = 0;
  virtual void SetProvenance(const char* d) = 0;
  virtual void SetDefaultWordLatticeId(const char* i) = 0;
  virtual void SetDefaultPhoneLatticeId(const char* i) = 0;

  virtual void SetPhoneConfID(const char* i) = 0;
  virtual const char* GetPhoneConfID(void) const = 0;
  virtual void SetSpeakerID(const char* i) = 0;
  virtual const char* GetSpeakerID(void) const = 0;

  // Sundry
  virtual void Reset(void) = 0;
  virtual void Display(void) const = 0;


};

class SpokenContentSpeakerDescriptor;

//=============================================================================
class SpokenContentSpeakerDescriptorInterface: public SpokenContentSpeakerDescriptorInterfaceABC
{
public:
	SpokenContentSpeakerDescriptorInterface(SpokenContentSpeakerDescriptor *aHisto);
	virtual ~SpokenContentSpeakerDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
  virtual void release();

  // Get the contents of the class
  // PNG virtual SpokenContentExtractionInfoDescriptor* GetExtractionInfo(void);
  // PNG virtual void SetExtractionInfo(SpokenContentExtractionInfoDescriptor* i);
  // PNG virtual IndividualDescriptor* GetIndividual(void);
  virtual const unsigned long GetNWordIndex(void) const;
  virtual const unsigned long GetNPhoneIndex(void) const;
  virtual SpokenContentWordIndexEntryDescriptor* GetWordIndex(const unsigned long idx);
  virtual SpokenContentPhoneIndexEntryDescriptor* GetPhoneIndex(const unsigned long idx);
  virtual const char* GetWordLexiconID(void) const;
  virtual const char* GetPhoneLexiconID(void) const;
  virtual const char* GetLang(void) const;
  virtual const char* GetProvenance(void) const;
  virtual SpokenContentWordIndexEntryDescriptor* FindWordIndexEntry(const unsigned char n,const char** x);
  virtual SpokenContentPhoneIndexEntryDescriptor* FindPhoneIndexEntry(const unsigned char n,const char** x);
  virtual const char* GetDefaultWordLatticeId(void) const;
  virtual const char* GetDefaultPhoneLatticeId(void) const;

  // Set the contents of the class
  //  virtual void SetIndividual(IndividualDescriptor* i);
  virtual void SetNWordIndex(const unsigned long);
  virtual void SetNPhoneIndex(const unsigned long);
  virtual void SetWordIndex(const unsigned long idx,SpokenContentWordIndexEntryDescriptor* w);
  virtual void SetPhoneIndex(const unsigned long idx,SpokenContentPhoneIndexEntryDescriptor* p);
  virtual void SetWordLexiconID(const char* d);
  virtual void SetPhoneLexiconID(const char* d);
  virtual void SetLang(const char* d);
  virtual void SetProvenance(const char* d);
  virtual void SetDefaultWordLatticeId(const char* i);
  virtual void SetDefaultPhoneLatticeId(const char* i);

  virtual void SetPhoneConfID(const char* i);
  virtual const char* GetPhoneConfID(void) const;
  virtual void SetSpeakerID(const char* i);
  virtual const char* GetSpeakerID(void) const;

  // Sundry
  virtual void Reset(void);
  virtual void Display(void) const;


	static const UUID myID;
	static const char *myName;


private:
	SpokenContentSpeakerDescriptor *m_SpokenContentSpeakerDescriptor;
};

//=============================================================================
class SpokenContentSpeakerDescriptor: public Descriptor
{
friend class SpokenContentSpeakerDescriptorInterface;
public:
	SpokenContentSpeakerDescriptor();

        // This does the copy constructor
	SpokenContentSpeakerDescriptor(SpokenContentSpeakerDescriptor& data);

        // This is my own specialist constructor. It takes two \0 terminated
        // strings and stores them in the relevant components.
        SpokenContentSpeakerDescriptor(const char* con,const char* was);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
  virtual void release();

  // Get the contents of the class
  // PNG virtual SpokenContentExtractionInfoDescriptor* GetExtractionInfo(void);
  // PNG virtual void SetExtractionInfo(SpokenContentExtractionInfoDescriptor* i);
  // PNG virtual IndividualDescriptor* GetIndividual(void);
  virtual const unsigned long GetNWordIndex(void) const;
  virtual const unsigned long GetNPhoneIndex(void) const;
  virtual SpokenContentWordIndexEntryDescriptor* GetWordIndex(const unsigned long idx);
  virtual SpokenContentPhoneIndexEntryDescriptor* GetPhoneIndex(const unsigned long idx);
  virtual const char* GetWordLexiconID(void) const;
  virtual const char* GetPhoneLexiconID(void) const;
  virtual const char* GetLang(void) const;
  virtual const char* GetProvenance(void) const;
  virtual SpokenContentWordIndexEntryDescriptor* FindWordIndexEntry(const unsigned char n,const char** x);
  virtual SpokenContentPhoneIndexEntryDescriptor* FindPhoneIndexEntry(const unsigned char n,const char** x);
  virtual const char* GetDefaultWordLatticeId(void) const;
  virtual const char* GetDefaultPhoneLatticeId(void) const;

  // Set the contents of the class
  //  virtual void SetIndividual(IndividualDescriptor* i);
  virtual void SetNWordIndex(const unsigned long);
  virtual void SetNPhoneIndex(const unsigned long);
  virtual void SetWordIndex(const unsigned long idx,SpokenContentWordIndexEntryDescriptor* w);
  virtual void SetPhoneIndex(const unsigned long idx,SpokenContentPhoneIndexEntryDescriptor* p);
  virtual void SetWordLexiconID(const char* d);
  virtual void SetPhoneLexiconID(const char* d);
  virtual void SetLang(const char* d);
  virtual void SetProvenance(const char* d);
  virtual void SetDefaultWordLatticeId(const char* i);
  virtual void SetDefaultPhoneLatticeId(const char* i);

  virtual void SetPhoneConfID(const char* i);
  virtual const char* GetPhoneConfID(void) const;
  virtual void SetSpeakerID(const char* i);
  virtual const char* GetSpeakerID(void) const;

  // Sundry
  virtual void Reset(void) ;
  virtual void Display(void) const;


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual SpokenContentSpeakerDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~SpokenContentSpeakerDescriptor();
	
	unsigned long m_refcount;

	SpokenContentSpeakerDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
  static const UUID valID;


  // ------------------------------------------------------------------
  // A person name contains a langugage attribute and a vector of given
  // names, a vector of family names and a vector of person names.
  // PNG IndividualDescriptor* individual;
  // PNG SpokenContentExtractionInfoDescriptor* extractionInfo;
    unsigned long nwordindex;
    SpokenContentWordIndexEntryDescriptor** word_index;
    unsigned long nphoneindex;
    SpokenContentPhoneIndexEntryDescriptor** phone_index;
    char* defaultWordLatticeId;
    char* defaultPhoneLatticeId;
    char* phone_lexicon_id;
    char* word_lexicon_id;
    char* phone_conf_id;
    char* lang;
    char* provenance;
    char* speakerId;
};

};

//#endif // USEXML
#endif //__SPOKENCONTENTSPEAKER_TYPE__
