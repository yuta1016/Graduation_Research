//////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  SpokenContentSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "SpokenContentSearch.h"

//=============================================================================
	

using namespace XM;


const UUID SpokenContentSearchInterface::myID = UUID("");
const char *SpokenContentSearchInterface::myName = "SpokenContent Type Matching Interface";

const UUID SpokenContentSearchTool::myID = UUID("");
const char *SpokenContentSearchTool::myName = "SpokenContent Type Matching Tool";

const UUID SpokenContentSearchInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentSearchInterfaceABC::SpokenContentSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentSearchInterface::SpokenContentSearchInterface(SpokenContentSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentSearchInterface::~SpokenContentSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int SpokenContentSearchInterface::
SetRefDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *RefSpokenContentDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefSpokenContentDescriptorInterface);
}

//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC* SpokenContentSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentSearchInterface::
SetQueryDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *QuerySpokenContentDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QuerySpokenContentDescriptorInterface);
}

//----------------------------------------------------------------------------
double SpokenContentSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
SpokenContentSearchTool::SpokenContentSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentSearchTool::
SpokenContentSearchTool(SpokenContentDescriptorInterfaceABC
			 *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  SpokenContentDescriptor *descriptor =
	    new SpokenContentDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentSearchTool::~SpokenContentSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& SpokenContentSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SpokenContentSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SpokenContentSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SpokenContentSearchTool::
SetRefDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *aRefSpokenContentDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_RefDescriptorInterface == 
	    aRefSpokenContentDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
	  m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface = 
	  aRefSpokenContentDescriptorInterface;
	if (m_RefDescriptorInterface) {
	  m_RefDescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC* SpokenContentSearchTool::
GetQueryDescriptorInterface(void)

{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  SpokenContentSearchTool::
SetQueryDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *aQuerySpokenContentDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_QueryDescriptorInterface == 
	    aQuerySpokenContentDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
	  m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface = 
	  aQuerySpokenContentDescriptorInterface;
	if (m_QueryDescriptorInterface) {
	  m_QueryDescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
// 1.0 is a complete miss, 0.0 is a perfect hit
double SpokenContentSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  assert(m_RefDescriptorInterface   != NULL);
  assert(m_QueryDescriptorInterface != NULL);

  if(strcmp(m_RefDescriptorInterface->GetName(),
      "SpokenContent Type Description Interface") != 0) return(1.0);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
      "SpokenContent Type Description Interface") != 0) return(1.0);

  SpokenContentDescriptorInterfaceABC* r=m_RefDescriptorInterface;
  SpokenContentDescriptorInterfaceABC* q=m_QueryDescriptorInterface;

  // -------------------- Valid query check ----------------------------
  // Ok, the first thing it to ensure that the query is valid. It must
  // be a single lexicon and a single lattice
  SpokenContentHeaderDescriptor* qheader=q->GetHeader();
  const unsigned long nqwordlexicons=qheader->GetNWordLexicon();
  const unsigned long nqphonelexicons=qheader->GetNPhoneLexicon();
  const unsigned long nrlattice=r->GetNLattice();

  if(nqphonelexicons + nqwordlexicons != 1) {
    fprintf(stderr,"SpokenContentSearchTool::GetDistance: Require just "
      "one phone lexicon or one word - got %ld, %ld\n",
      nqphonelexicons,nqwordlexicons);
    return(1.0); 
  }
  if(q->GetNLattice()           != 1) {
    fprintf(stderr,"SpokenContentSearchTool::GetDistance: Query has %ld "
                   "lattices\n",q->GetNLattice());
    return(1.0);
  }

  // -------------------- Get the query elements -----------------------
  // Get the list of words in the query that are valid
  vector<string> qword_text;
  vector<float>  qword_probs;
  bool word_index_flag;
  unsigned long nqw_original;
  GetWordList(r,q,qword_text,qword_probs,word_index_flag,nqw_original);

  // Get the list of phone n-grams in the query 
  vector<vector<string> > qphone_ngrams;
  vector<float>           qphone_probs;
  bool phone_index_flag;
  const unsigned long phone_ngram_length=3;
  GetNGramList(r,q,phone_ngram_length,qphone_ngrams,qphone_probs,
               phone_index_flag);

  // Make up some storage for the results and zero it
  vector<vector<float> > prob_results(nrlattice);
  unsigned long il,ib;
  for(il=0;il<nrlattice;il++) {
    prob_results[il]=vector<float>(r->GetLattice(il)->GetNBlocks());
    for(ib=0;ib<r->GetLattice(il)->GetNBlocks();ib++) {
      prob_results[il][ib]=0.0f;
    }
  }

  // Get renormalaiser if both words and phones used
  float renorm=1.0f;
  if((qphone_ngrams.size() != 0) && (qword_text.size() != 0)) renorm = 0.5f;

  // Loop over the phone ngrams and include the score from these
  for(unsigned int ip=0;ip<qphone_ngrams.size();ip++) {
    float val=qphone_probs[ip]*renorm/qphone_ngrams.size();
    GetPhoneMatchScore(r,q,phone_ngram_length,qphone_ngrams[ip],val,
                       phone_index_flag,prob_results);
  }

  // Loop over the words and include score from these
  for(unsigned int iw=0;iw<qword_text.size();iw++) {
    float val=qword_probs[iw]*renorm/nqw_original;
    GetWordMatchScore(r,q,qword_text[iw],val,word_index_flag,prob_results);
  }

  // Ok, now find the best one
  float best=0.0f;
  for(il=0;il<prob_results.size();il++) {
    for(ib=0;ib<prob_results[il].size();ib++) {
      if(prob_results[il][ib] != 0.0f) {

        fprintf(stderr,"lattice=%4ld block=%4ld distance=%12.6f Mediatime=[%s]\n",
               il,ib,1.0f-prob_results[il][ib],
               r->GetLattice(il)->GetBlock(ib)->GetMediaTime());
        if(prob_results[il][ib]>best) {
          best=prob_results[il][ib];
	}
      }
    }
  }

  // That's all we have
  return(static_cast<double> (1.0f-best));
}


// ------------------------------------------------------------------------
// Unravel all the phone ngrams in the query lattice. This is done
// by unravelling the lattice from each node. Only unique ngrams are
// retained. If the lattices in the query and ref are of different
// languages no ngrams are returned
void SpokenContentSearchTool::GetNGramList(
  SpokenContentDescriptorInterfaceABC* r,
  SpokenContentDescriptorInterfaceABC* q,
  const unsigned long      length,
  vector<vector<string> >& qphone_ngrams,
  vector<float>&           qphone_probs,
  bool&                    phone_index_flag)
{
  // If length is zero do nothing
  if(length==0) return;

  // Create assorted variables - currently treat as if just one language
  SpokenContentHeaderDescriptor* rheader=r->GetHeader();
  SpokenContentHeaderDescriptor* qheader=q->GetHeader();
  SpokenContentLatticeDescriptor* qlattice=q->GetLattice(0);
  const unsigned long qnphonelexicons=qheader->GetNPhoneLexicon();
  const unsigned long rnphonelexicons=rheader->GetNPhoneLexicon();
  if((qnphonelexicons != 1) || (rnphonelexicons != 1)) return;

  SpokenContentPhoneLexiconDescriptor* qlexicon=qheader->GetPhoneLexicon(0);
  SpokenContentSpeakerDescriptor* qspeaker=qheader->GetSpeaker(0);
  const string qlang=qspeaker->GetLang();
  const string qphoneset=qlexicon->GetPhonesetName();

  // Loop over all the speakers in the ref
  bool foundmatch=false;
  phone_index_flag=true;
  for(unsigned long ispeaker=0;ispeaker<qheader->GetNSpeaker();ispeaker++) {
    SpokenContentSpeakerDescriptor* rspeaker=rheader->GetSpeaker(ispeaker);
    if(qlang != rspeaker->GetLang()) continue;

    // Find the phone lexicon for this speaker if there is one
    const char* plex_id=rspeaker->GetPhoneLexiconID();
    if(plex_id==NULL) continue; // No phone lexicon here
    long is=rheader->FindPhoneLexicon(plex_id);
    if(is == -1) {
      fprintf(stderr,"SpokenContentSearchTool::GetNGramList: "
                "lexicon id not found: %s\n",plex_id);
      return;
    }
    if(qphoneset!=rheader->GetPhoneLexicon(is)->GetPhonesetName()) continue;

    // Ok it all matches
    foundmatch=true;

    // Does this speaker have a phone index?
    if(rspeaker->GetNPhoneIndex()==0) phone_index_flag=false;
  }

  // If there's nothing to match return now
  if(foundmatch==false) return;

  // Ok, loop over the nodes in the query
  const unsigned short qnblocks=qlattice->GetNBlocks();
  for(unsigned short qiblock=0;qiblock<qnblocks;qiblock++) {

    // Loop over all nodes
    SpokenContentBlockDescriptor* qblock=qlattice->GetBlock(qiblock);
    const unsigned short qnnodes=qblock->GetNNodes();
    for(unsigned short qinodes=0;qinodes<qnnodes;qinodes++) {

      // Get all the ngrams from here
      vector<vector<string> > t_ngram;
      vector<float>           t_prob;
      UnravelPhoneNGram(length,qlattice,qinodes,qiblock,t_ngram,t_prob);

      // Add to our list
      for(unsigned int i=0;i<t_ngram.size();i++) {
        qphone_ngrams.push_back(t_ngram[i]);
        qphone_probs.push_back(t_prob[i]);
      }
    } // end node loop
  } // End block loop

  // All done - display the ngram
  /*
  for(int i=0;i<qphone_ngrams.size();i++) {
    for(int j=0;j<qphone_ngrams[i].size();j++) {
      cout << qphone_ngrams[i][j] << " ";
    }
    cout << endl;
  }
  */
}

// ------------------------------------------------------------------------
// This takes a node and block position within a lattice and obtains all
// phone ngrams of the given length from that 
void SpokenContentSearchTool::UnravelPhoneNGram(const unsigned long length,
  SpokenContentLatticeDescriptor* lattice,
  const unsigned short inode,
  const unsigned short iblock,
  vector<vector<string> >& phone_ngrams,
  vector<float>&           phone_probs)
{
  // If length==0 then do nothing
  if(length==0) return;

  // Get the node
  SpokenContentBlockDescriptor* qblock=lattice->GetBlock(iblock);
  SpokenContentNodeDescriptor*  qnode=qblock->GetNode(inode);
 
  // Loop over the phone links
  const unsigned char nlinks=qnode->GetNPhoneLinks();
  for(unsigned char i=0;i<nlinks;i++) {
    // Get the details
    SpokenContentPhoneLinkDescriptor* plink=qnode->GetPhoneLink(i);
    const float prob=plink->GetLink()->GetProbability();
    const string phone=string(plink->GetPhone());

    // If this is the end of the line just use these phones
    if(length==1) {
      vector<string> t_ngrams;
      t_ngrams.push_back(phone);
      phone_ngrams.push_back(t_ngrams);
      phone_probs.push_back(1.0f);
    }
    else {
      // Sort out the target
      const unsigned short noffset=plink->GetLink()->GetNodeOffset();
      unsigned short targetnode=noffset+inode;
      unsigned short targetblock=iblock;
      if(targetnode>=qblock->GetNNodes()) {
        targetnode -= qblock->GetNNodes();
        targetblock++;
      }
      //      SpokenContentBlockDescriptor* tblock=lattice->GetBlock(targetblock);
      //      SpokenContentNodeDescriptor*  tnode=tblock->GetNode(targetnode);
      
      // Get the list from this node
      vector<vector<string> > t_ngrams;
      vector<float>           t_probs;
      UnravelPhoneNGram(length-1,lattice,targetnode,targetblock,
                        t_ngrams,t_probs);

      // Add this to the new one with the current phone
      vector<string> first_phone;
      first_phone.push_back(phone);
      for(unsigned int i=0;i<t_ngrams.size();i++) {
        phone_ngrams.push_back(first_phone);
        vector<string>& v=phone_ngrams[phone_ngrams.size()-1];
        for(unsigned int j=0;j<t_ngrams[i].size();j++) {
          v.push_back(t_ngrams[i][j]);
	}
        phone_probs.push_back(prob*t_probs[i]);
      } // End loop over partial ngrams
    } // end if length==1
  } // end loop over phone links
      
  // All done
}

//--------------------------------------------------------------------------
// Make up a list of all the words and their probabilities in the 
// query. Strip out any which do not appear in any word lexicon and
// are not of the right language.
void SpokenContentSearchTool::GetWordList(
	     SpokenContentDescriptorInterfaceABC* r, // the reference
	     SpokenContentDescriptorInterfaceABC* q, // The query
	     vector<string>& qwords,
	     vector<float>&  qprobs,
             bool& word_index_flag,
             unsigned long& nqw_original)
{
  // Set up assorted variables
  SpokenContentLatticeDescriptor* qlattice=q->GetLattice(0);
  SpokenContentHeaderDescriptor* qheader=q->GetHeader();
  SpokenContentHeaderDescriptor* rheader=r->GetHeader();
  const unsigned long nrwordlexicons=rheader->GetNWordLexicon();
  const unsigned long nqwordlexicons=qheader->GetNWordLexicon();
  if((nrwordlexicons==0) || (nqwordlexicons == 0)) return;

  const unsigned long nrspeakers=rheader->GetNSpeaker();

  // --------- Make a list of all query words and their probs  ------------
  // Loop over all blocks
  const unsigned short qnblocks=qlattice->GetNBlocks();
  for(unsigned short qiblock=0;qiblock<qnblocks;qiblock++) {

    // Loop over all nodes
    SpokenContentBlockDescriptor* qblock=qlattice->GetBlock(qiblock);
    const unsigned short qnnodes=qblock->GetNNodes();
    for(unsigned short qinodes=0;qinodes<qnnodes;qinodes++) {

      // Loop over all word links
      SpokenContentNodeDescriptor* qnode=qblock->GetNode(qinodes);
      const unsigned char qnwlinks=qnode->GetNWordLinks();
      for(unsigned char qiwlinks=0;qiwlinks<qnwlinks;qiwlinks++) {

	// Add these words to the list
        SpokenContentWordLinkDescriptor* qwlink=qnode->GetWordLink(qiwlinks);
        qwords.push_back(string(qwlink->GetWord()));
        qprobs.push_back(qwlink->GetLink()->GetProbability());
      } // End link loop
    } // end node loop
  } // End block loop
  assert(qwords.size()==qprobs.size());
  if(qwords.size()==0) return; // No words to match!

  // Display this list of query words and probs
  unsigned int iqw;
  nqw_original=qwords.size();

  // ----------------------------------------------------------------------
  // Strip out any words which do not appear in the ref word lexicons in
  // the same language
  word_index_flag=true;
  const char* qlanguage=qheader->GetSpeaker(0)->GetLang();
  for(iqw=0;iqw<qwords.size();) {
    const char* qs=qwords[iqw].c_str();
    
    // Loop over the speakers
    bool found_flag=false;
    for(unsigned long irs=0;irs<nrspeakers;irs++) {
      SpokenContentSpeakerDescriptor* rspeaker=rheader->GetSpeaker(irs);

      // Check the language - must be same
      if(strcmp(rspeaker->GetLang(),qlanguage) != 0) continue;

      // Now see whether word is in this speaker's word lexicon
      const char* wlex_id=rspeaker->GetWordLexiconID();
      if(wlex_id==NULL) continue; // No word lexicon here

      // Does this speaker have a word index?
      if(rspeaker->GetNWordIndex()==0) word_index_flag=false;

      long is=rheader->FindWordLexicon(wlex_id);
      if(is == -1) {
        fprintf(stderr,"SpokenContentSearchTool::GetDistance: "
                "lexicon id not found: %s\n",wlex_id);
        return;
      }
      if(rheader->GetWordLexicon(is)->Exists(qs)) {
        found_flag=true;
        break;
      }
    }

    // If word doesn't exist in lexicon then bin it
    if(found_flag==false) {
      vector<string>::iterator ps=qwords.begin();
      ps += iqw;
      qwords.erase(ps);
      vector<float>::iterator pb=qprobs.begin();
      pb += iqw;
      qprobs.erase(pb);
      //      printf("binning %s\n",qs);
    }
    else {
      iqw++;
    }
  } // Onto the next word
}

//-------------------------------------------------------------------------
// 1.0 is a complete miss, 0.0 is a perfect hit
void SpokenContentSearchTool::GetPhoneMatchScore(
  SpokenContentDescriptorInterfaceABC* r,
  SpokenContentDescriptorInterfaceABC* q,
  const unsigned long                  length,
  const vector<string>&                qngram,
  const float                          qprob,
  const bool                           phoneindex_flag,
  vector<vector<float> >&              results)
{
  // Set up assorted conts and pointers
  SpokenContentHeaderDescriptor* rheader=r->GetHeader();
  SpokenContentHeaderDescriptor* qheader=q->GetHeader();
  const unsigned long nrphonelexicons=rheader->GetNPhoneLexicon();
  const unsigned long nrspeakers=rheader->GetNSpeaker();
  const unsigned long rnlattice=r->GetNLattice();
  const char* qlanguage=qheader->GetSpeaker(0)->GetLang();
  if(nrphonelexicons==0) return;
  SpokenContentPhoneLexiconDescriptor* qlexicon=qheader->GetPhoneLexicon(0);
  const char* qphoneset=qlexicon->GetPhonesetName();

  // ----------------------------------------------------------------------
  // Scores will be given by the percentage of the ngrams found in a block.
  // A word occuring 5 times in a block has the same importance as one found
  // once in block. Time ordering is unimportant
  assert(rnlattice == results.size());
  vector<vector<float> > tmp_block_scores(rnlattice);
  for(unsigned long rilattice=0;rilattice<rnlattice;rilattice++) {
    const unsigned long rnblock=r->GetLattice(rilattice)->GetNBlocks();
    assert(results[rilattice].size()==rnblock);
    tmp_block_scores[rilattice]=vector<float>(rnblock);
    for(unsigned long i=0;i<rnblock;i++) {
      tmp_block_scores[rilattice][i] = 0.0f;
    }
  }

  // Make a char** representation of the phonengram
  const char** qs=new const char*[qngram.size()];
  for(unsigned int i=0;i<qngram.size();i++) {
    qs[i]=qngram[i].c_str();
  }

  // ----------------------------------------------------------------------
  // If there's a phone index search that. If not then search all the 
  // lattices. This is much slower.
  if(phoneindex_flag) { // There are indexes so use them
    // Loop over the speakers
    for(unsigned long irs=0;irs<nrspeakers;irs++) {
      SpokenContentSpeakerDescriptor* rspeaker=rheader->GetSpeaker(irs);

      // Check the language - must be same
      if(strcmp(rspeaker->GetLang(),qlanguage) != 0) continue;

      // Get the default lattice
      const char* defaultLattice=rspeaker->GetDefaultPhoneLatticeId();
      if(defaultLattice==NULL) continue; // No index

      // Now see whether phone lexicons are the same
      const char* plex_id=rspeaker->GetPhoneLexiconID();
      if(plex_id==NULL) continue; // No phone lexicon here

      long is=rheader->FindPhoneLexicon(plex_id);
      if(is == -1) {
        fprintf(stderr,"SpokenContentSearchTool::GetPhoneMatchScore: "
                "lexicon id not found: %s\n",plex_id);
        return;
      }

      // Check that the phone sets are the same
      const char* rphoneset=rheader->GetPhoneLexicon(is)->GetPhonesetName();
      if(strcmp(qphoneset,rphoneset) != 0) continue; // Not the same

      // Is this in the phone index?
      SpokenContentPhoneIndexEntryDescriptor* 
                              e=rspeaker->FindPhoneIndexEntry(length,qs);
      if(e==NULL) continue;

      // Ok, add all these entries to the list
      const unsigned long nentries=e->GetNEntries();
      for(unsigned long ie=0;ie<nentries;ie++) {
        SpokenContentIndexEntryDescriptor* i=e->GetEntry(ie);
        const unsigned long ib=i->GetBlock();
        const unsigned short in=i->GetNode();
        long il;
        if(i->GetLattice()!=NULL) il=r->FindLattice(i->GetLattice());
        else                      il=r->FindLattice(defaultLattice);
        assert(il != -1);
        SpokenContentNodeDescriptor* n=
                               r->GetLattice(il)->GetBlock(ib)->GetNode(in);
          
        // Get the score for this entry
        const float p=qprob*(n->GetPhoneProb(qs[0]));
        if(p>tmp_block_scores[il][ib]) tmp_block_scores[il][ib]=p;
      }
    }
  }
  else { // Ok, no indexes so search whole lattice! - this will be slow
    // Loop over all the reference lattices
    for(unsigned long rilattice=0;rilattice<rnlattice;rilattice++) {
  
      // Loop over all the blocks
      SpokenContentLatticeDescriptor* rlattice=r->GetLattice(rilattice);
      const unsigned short rnblocks=rlattice->GetNBlocks();

      // Initialise the scores
      for(unsigned short riblocks=0;riblocks<rnblocks;riblocks++) {

	// Identify the default speaker's language
        SpokenContentBlockDescriptor* rblock=rlattice->GetBlock(riblocks);
        const long dSpeaker=rheader->FindSpeaker(rblock->GetDefaultSpeaker());
        if(dSpeaker==-1) {
          fprintf(stderr,"unknown default speaker#1: %s\n",
                  rblock->GetDefaultSpeaker());
          exit(1);
	}
        const char* defaultLanguage=rheader->GetSpeaker(dSpeaker)->GetLang();
        
        // Loop over all the nodes
        const unsigned short rnnodes=rblock->GetNNodes();
        for(unsigned short rinodes=0;rinodes<rnnodes;rinodes++) {

	  // Is this node in the same language as the query?
          SpokenContentNodeDescriptor* rnode=rblock->GetNode(rinodes);
          if(rnode->GetSpeaker()==NULL) {
            if(strcmp(defaultLanguage,qlanguage)!=0) continue;
	  }
          else {
            const long nsp=rheader->FindSpeaker(rnode->GetSpeaker());
            if(nsp==-1) {
              fprintf(stderr,"unknown default speaker#1: %s\n",
                      rblock->GetDefaultSpeaker());
              exit(1);
	    }
            const char* nlang=rheader->GetSpeaker(nsp)->GetLang();
            if(strcmp(nlang,qlanguage)!=0) continue;
	  }

	  // Is the first phone of the ngram at this node?
          if(rnode->FindPhoneLink(qs[0]) == NULL) continue;

	  // Get all the ngrams which come from here
          vector<vector<string> > t_ngram;
          vector<float>           t_prob;
          UnravelPhoneNGram(length,rlattice,rinodes,riblocks,t_ngram,t_prob);
          
	  // Loop over the ngrams and see if any match
          for(unsigned int i=0;i<t_ngram.size();i++) {

            if(t_ngram[i] == qngram) {
              const float t=qprob*t_prob[i];
              if(t>tmp_block_scores[rilattice][riblocks]) {
                tmp_block_scores[rilattice][riblocks]=t;
	      }
	    }
	  }
	} // End loop over nodes
      } // End loop over blocks
    } // End loop over lattices
  } // End if index

  // Add these to the total score
  for(unsigned long itl=0;itl<tmp_block_scores.size();itl++) {
    for(unsigned long itb=0;itb<tmp_block_scores[itl].size();itb++) {
      results[itl][itb] += tmp_block_scores[itl][itb];
    }
  }
}

//-------------------------------------------------------------------------
// 1.0 is a complete miss, 0.0 is a perfect hit
void SpokenContentSearchTool::GetWordMatchScore(
  SpokenContentDescriptorInterfaceABC* r,
  SpokenContentDescriptorInterfaceABC* q,
  const string& qword,
  const float   qprob,
  const bool    wordindex_flag,
  vector<vector<float> >& results)
{
  // Set up assorted conts and pointers
  SpokenContentHeaderDescriptor* rheader=r->GetHeader();
  SpokenContentHeaderDescriptor* qheader=q->GetHeader();
  const unsigned long nrwordlexicons=rheader->GetNWordLexicon();
  const unsigned long nrspeakers=rheader->GetNSpeaker();
  const unsigned long rnlattice=r->GetNLattice();
  const char* qlanguage=qheader->GetSpeaker(0)->GetLang();
  if(nrwordlexicons==0) return;

  // ----------------------------------------------------------------------
  // Scores will be given by the percentage of the words found in a block.
  // A word occuring 5 times in a block has the same importance as one found
  // once in block. 
  assert(rnlattice == results.size());
  vector<vector<float> > tmp_block_scores(rnlattice);
  for(unsigned long rilattice=0;rilattice<rnlattice;rilattice++) {
    const unsigned long rnblock=r->GetLattice(rilattice)->GetNBlocks();
    assert(results[rilattice].size()==rnblock);
    tmp_block_scores[rilattice]=vector<float>(rnblock);
    for(unsigned long i=0;i<rnblock;i++) {
      tmp_block_scores[rilattice][i] = 0.0f;
    }
  }

  // ----------------------------------------------------------------------
  // If there's a word index search that. If not then search all the 
  // lattices. This is much slower.
  if(wordindex_flag) { // There are indexes so use them

    const char* qs=qword.c_str();

    // Loop over the speakers
    for(unsigned long irs=0;irs<nrspeakers;irs++) {
      SpokenContentSpeakerDescriptor* rspeaker=rheader->GetSpeaker(irs);

      // Check the language - must be same
      if(strcmp(rspeaker->GetLang(),qlanguage) != 0) continue;

      // Get the default word lattice
      const char* defaultLattice=rspeaker->GetDefaultWordLatticeId();
      if(defaultLattice==NULL) continue; // No word index     

      // Is this in the word index?
      const char** tqs=&qs;
      SpokenContentWordIndexEntryDescriptor* 
                                       e=rspeaker->FindWordIndexEntry(1,tqs);
      if(e==NULL) continue;

      // Ok, add all these entries to the list
      const unsigned long nentries=e->GetNEntries();
      for(unsigned long ie=0;ie<nentries;ie++) {
        SpokenContentIndexEntryDescriptor* i=e->GetEntry(ie);
        const unsigned long ib=i->GetBlock();
        const unsigned short in=i->GetNode();
        long il=-1;
        if(i->GetLattice() != NULL) il=r->FindLattice(i->GetLattice());
        else                        il=r->FindLattice(defaultLattice);
        assert(il != -1);
        SpokenContentNodeDescriptor* n=
                                r->GetLattice(il)->GetBlock(ib)->GetNode(in);
          
        // Get the score for this entry
        const float p=qprob*(n->GetWordProb(qs));
        if(p>tmp_block_scores[il][ib]) tmp_block_scores[il][ib]=p;
      }
    }
  }
  else { // Ok, no indexes so search whole lattice!

    // Loop over all the reference lattices
    for(unsigned long rilattice=0;rilattice<rnlattice;rilattice++) {
  
      // Loop over all the blocks
      SpokenContentLatticeDescriptor* rlattice=r->GetLattice(rilattice);
      const unsigned short rnblocks=rlattice->GetNBlocks();

      // Initialise the scores
      for(unsigned short riblocks=0;riblocks<rnblocks;riblocks++) {

	// Identify the default speaker's language
        SpokenContentBlockDescriptor* rblock=rlattice->GetBlock(riblocks);
        const long dSpeaker=rheader->FindSpeaker(rblock->GetDefaultSpeaker());
        if(dSpeaker==-1) {
          fprintf(stderr,"unknown default speaker#1: %s\n",
                  rblock->GetDefaultSpeaker());
          exit(1);
	}
        const char* defaultLanguage=rheader->GetSpeaker(dSpeaker)->GetLang();
        
        // Loop over all the nodes
        const unsigned short rnnodes=rblock->GetNNodes();
        for(unsigned short rinodes=0;rinodes<rnnodes;rinodes++) {

	  // Is this node in the same language as the query?
          SpokenContentNodeDescriptor* rnode=rblock->GetNode(rinodes);
          if(rnode->GetSpeaker()==NULL) {
            if(strcmp(defaultLanguage,qlanguage)!=0) continue;
	  }
          else {
            const long nsp=rheader->FindSpeaker(rnode->GetSpeaker());
            if(nsp==-1) {
              fprintf(stderr,"unknown default speaker#1: %s\n",
                      rblock->GetDefaultSpeaker());
              exit(1);
	    }
            const char* nlang=rheader->GetSpeaker(nsp)->GetLang();
            if(strcmp(nlang,qlanguage)!=0) continue;
	  }

  	  // Loop over all the word links
          const unsigned char rnwlinks=rnode->GetNWordLinks();
          for(unsigned char riwlinks=0;riwlinks<rnwlinks;riwlinks++) {
        
	    // Is this word in the query set?
            SpokenContentWordLinkDescriptor* rwlink=rnode->GetWordLink(riwlinks);
            const char* rword=rwlink->GetWord();
            if(strcmp(rword,qword.c_str())==0) {
              const float t=qprob*rwlink->GetLink()->GetProbability();
              if(t>tmp_block_scores[rilattice][riblocks]) {
                tmp_block_scores[rilattice][riblocks]=t;
	      }
	    }
	  } // End loop over links
        } // End loop over nodes
      } // End loop over blocks
    } // End loop over lattice
  } // End if-index

  // Add these to the total score
  for(unsigned long itl=0;itl<tmp_block_scores.size();itl++) {
    for(unsigned long itb=0;itb<tmp_block_scores[itl].size();itb++) {
      results[itl][itb] += tmp_block_scores[itl][itb];
    }
  }
}

//----------------------------------------------------------------------------
SpokenContentSearchInterfaceABC *SpokenContentSearchTool::GetInterface(void)
{ return &m_Interface; }

