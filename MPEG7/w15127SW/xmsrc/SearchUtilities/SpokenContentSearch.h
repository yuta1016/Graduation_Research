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
// Applicable File Name:  SpokenContentSearch.h
//
#ifndef __SPOKENCONTENTTYPESEARCH_H__
#define __SPOKENCONTENTTYPESEARCH_H__

// Only include if using XML
//#ifdef USEXML

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class SpokenContentSearchTool;

//=============================================================================
class SpokenContentSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	SpokenContentSearchInterfaceABC();
	virtual ~SpokenContentSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (SpokenContentDescriptorInterfaceABC
	   *aSpokenContentDescriptorInterface) = 0;
	virtual SpokenContentDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
	  (SpokenContentDescriptorInterfaceABC
	   *aSpokenContentDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class SpokenContentSearchInterface: 
  public SpokenContentSearchInterfaceABC
{
public:
	SpokenContentSearchInterface(SpokenContentSearchTool* aTool);
	virtual ~SpokenContentSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (SpokenContentDescriptorInterfaceABC
	   *aSpokenContentDescriptorInterface);
	virtual SpokenContentDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (SpokenContentDescriptorInterfaceABC
	   *aSpokenContentDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	SpokenContentSearchTool *m_SearchTool;
};

//=============================================================================
class SpokenContentSearchTool: public Search
{
friend class SpokenContentSearchInterface;
public:
	SpokenContentSearchTool();
	SpokenContentSearchTool(SpokenContentDescriptorInterfaceABC
				 *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual SpokenContentSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (SpokenContentDescriptorInterfaceABC
	   *aSpokenContentDescriptorInterface);
	virtual SpokenContentDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (SpokenContentDescriptorInterfaceABC
	   *aSpokenContentDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
  // Unravel all the phone ngrams in the query lattice. This is done
  // by unravelling the lattice from each node. Only unique ngrams are
  // retained. If the lattices in the query and ref are of different
  // languages no ngrams are returned
  void GetNGramList(
    SpokenContentDescriptorInterfaceABC* r,
    SpokenContentDescriptorInterfaceABC* q,
    const unsigned long      length,
    vector<vector<string> >& qphone_ngrams,
    vector<float>&           qphone_probs,
    bool&                    phone_index_flag);


  // 1.0 is a complete miss, 0.0 is a perfect hit
  void GetPhoneMatchScore(
    SpokenContentDescriptorInterfaceABC* r,
    SpokenContentDescriptorInterfaceABC* q,
    const unsigned long                  length,
    const vector<string>&                qngram,
    const float                          qprob,
    const bool                           phoneindex_flag,
    vector<vector<float> >&              results);

  // This performs the word based matching on the list of words
  // supplied
  void GetWordMatchScore(SpokenContentDescriptorInterfaceABC* r,
                         SpokenContentDescriptorInterfaceABC* q,
                         const string& qword,
                         const float   qprob,
                         const bool    wordindex_flag,
                         vector<vector<float> >& results);

         void GetWordList(
	     SpokenContentDescriptorInterfaceABC* r, // the reference
	     SpokenContentDescriptorInterfaceABC* q, // The query
	     vector<string>& qwords,
	     vector<float>&  qprobs,
             bool& word_index_flag,
             unsigned long& nqw_original);

  // This takes a node and block position within a lattice and obtains all
  // phone ngrams of the given length from that 
  void UnravelPhoneNGram(const unsigned long length,
    SpokenContentLatticeDescriptor* lattice,
    const unsigned short inode,
    const unsigned short iblock,
    vector<vector<string> >& phone_ngrams,
    vector<float>&           phone_probs);

	virtual ~SpokenContentSearchTool();

	SpokenContentSearchInterface m_Interface;
	SpokenContentDescriptorInterfaceABC *m_RefDescriptorInterface;
	SpokenContentDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};

//#endif // USEXML
#endif //__SPOKENCONTENTTYPESEARCH_H__
