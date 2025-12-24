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
// Applicable File Name:  SpokenContentExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID SpokenContentExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentExtractionTool::myName = "SpokenContent Type Descriptor Extractor";

const UUID SpokenContentExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentExtractionInterfaceABC::SpokenContentExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentExtractionInterface::SpokenContentExtractionInterface(
  SpokenContentExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentExtractionInterface::~SpokenContentExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC* SpokenContentExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentExtractionInterface::
SetDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *aSpokenContentDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentExtractionTool::SpokenContentExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentExtractionTool::
SpokenContentExtractionTool(SpokenContentDescriptorInterfaceABC *SpokenContent):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContent) {
	  SpokenContentDescriptor *descriptor = new SpokenContentDescriptor();
	  SpokenContent=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContent);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentExtractionTool::~SpokenContentExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC* SpokenContentExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentExtractionTool::
SetDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *aSpokenContentDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();

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
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long SpokenContentExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentExtractionTool::StartExtracting()
{
  // To help debugging
  const char* routine="SpokenContentExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContent Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------
  vector<GenericDS> list;

  // Get the header - must be only one
  list=m_Media->GetAllDescriptions(string("SpokenContentHeader"));
  if(list.size()!=1) {
    fprintf(stderr,"%s must contain (only) one header\n",routine);
    return(failure);
  }

  SpokenContentHeaderDescriptor* theHeadDescriptor;
  SpokenContentHeaderDescriptorInterfaceABC* theHeadDescriptorInterface;
  SpokenContentHeaderExtractionTool* theHeadExtractionTool;
  SpokenContentHeaderExtractionInterfaceABC* theHeadExtractionInterface;
  theHeadDescriptor = new SpokenContentHeaderDescriptor();
  theHeadDescriptor->addref();
  theHeadDescriptorInterface = theHeadDescriptor->GetInterface();
  theHeadExtractionTool = new SpokenContentHeaderExtractionTool(NULL);
  theHeadExtractionInterface = theHeadExtractionTool->GetInterface();

  theHeadExtractionInterface->SetSourceMedia(&(list[0]));
  theHeadExtractionInterface->SetDescriptorInterface(theHeadDescriptorInterface);
  theHeadExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetHeader(theHeadDescriptor);
  theHeadDescriptor->release();

  // Get the lattices - must be at least one
  list=m_Media->GetAllDescriptions(string("SpokenContentLattice"));
  if(list.size()==0) {
    fprintf(stderr,"%s must contain at least one lattice\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNLattice(list.size());
  for(unsigned int i=0;i<list.size();i++) {
    SpokenContentLatticeDescriptor* theLatDescriptor;
    SpokenContentLatticeDescriptorInterfaceABC* theLatDescriptorInterface;
    SpokenContentLatticeExtractionTool* theLatExtractionTool;
    SpokenContentLatticeExtractionInterfaceABC* theLatExtractionInterface;
    theLatDescriptor = new SpokenContentLatticeDescriptor();
    theLatDescriptor->addref();
    theLatDescriptorInterface = theLatDescriptor->GetInterface();
    theLatExtractionTool = new SpokenContentLatticeExtractionTool(NULL);
    theLatExtractionInterface = theLatExtractionTool->GetInterface();

    theLatExtractionInterface->SetSourceMedia(&(list[i]));
    theLatExtractionInterface->SetDescriptorInterface(theLatDescriptorInterface);
    theLatExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetLattice(i,theLatDescriptor);
    theLatDescriptor->release();
  }

	// -------------------- Post-Processing to Resolve Lexicon Indices --------------

	SpokenContentHeaderDescriptor* const headerDescriptor = m_DescriptorInterface->GetHeader();
	SpokenContentHeaderDescriptorInterfaceABC* const headerInterface = headerDescriptor->GetInterface();
  
	// resolve all speakerinfo wordindexentries and phoneindexentries
	const int numSpeakers = headerInterface->GetNSpeaker();
	for(int speaker = 0; speaker < numSpeakers; ++speaker)
	{
		SpokenContentSpeakerDescriptor* speakerDescriptor = headerInterface->GetSpeaker(speaker);
		SpokenContentSpeakerDescriptorInterfaceABC* speakerInterface = speakerDescriptor->GetInterface();

		// get the phone lexicon for this speaker
		long phoneLexicon = -1;
		if(speakerInterface->GetPhoneLexiconID() == 0
			|| (phoneLexicon = headerInterface->FindPhoneLexicon(speakerInterface->GetPhoneLexiconID())) == -1)
		{
			fprintf(stderr, "%s: unknown phone lexicon ref for speaker %s.\n", routine, speakerInterface->GetSpeakerID());
			return failure;
		}
		
		SpokenContentPhoneLexiconDescriptor* phoneLexiconDescriptor = headerInterface->GetPhoneLexicon(phoneLexicon);
		SpokenContentPhoneLexiconDescriptorInterfaceABC* phoneLexiconInterface = phoneLexiconDescriptor->GetInterface();

		// loop through each phone index entry
		int numPhoneIndexes = speakerInterface->GetNPhoneIndex();
		for(int phoneIndex = 0; phoneIndex < numPhoneIndexes; ++phoneIndex)
		{
			SpokenContentPhoneIndexEntryDescriptor* indexDescriptor = speakerInterface->GetPhoneIndex(phoneIndex);
			SpokenContentPhoneIndexEntryDescriptorInterfaceABC* indexInterface = indexDescriptor->GetInterface();

			// loop through each key element
			int numKeyElements = indexInterface->GetNKey();
			for(int element = 0; element < numKeyElements; ++element)
			{
				long index = indexInterface->GetKeyIndex(element);
				if(index < 0 || index >= phoneLexiconInterface->GetNPhones())
				{
					fprintf(stderr, "%s: invalid phone index %d in key.\n", routine, index);
					return failure;
				}
				// set the actual phone
				/*printf("PhoneIndexEntry key %d -> %s (S, P, E = %d, %d, %d)\n", 
					index, phoneLexiconInterface->GetPhone(index), speaker, phoneIndex, element);*/
				indexInterface->SetKey(element, phoneLexiconInterface->GetPhone(index));
			}
		}

		// get the word lexicon for this speaker
		long wordLexicon = -1;
		if(speakerInterface->GetWordLexiconID() == 0
			|| (wordLexicon = headerInterface->FindWordLexicon(speakerInterface->GetWordLexiconID())) == -1)
		{
			fprintf(stderr, "%s: unknwon word lexicon ref for speaker %s.\n", routine, speakerInterface->GetSpeakerID());
			return failure;
		}

		SpokenContentWordLexiconDescriptor* wordLexiconDescriptor = headerInterface->GetWordLexicon(wordLexicon);
		SpokenContentWordLexiconDescriptorInterfaceABC* wordLexiconInterface = wordLexiconDescriptor->GetInterface();

		// loop through each word index entry
		int numWordIndexes = speakerInterface->GetNWordIndex();
		for(int wordIndex = 0; wordIndex < numWordIndexes; ++wordIndex)
		{
			SpokenContentWordIndexEntryDescriptor* indexDescriptor = speakerInterface->GetWordIndex(wordIndex);
			SpokenContentWordIndexEntryDescriptorInterfaceABC* indexInterface = indexDescriptor->GetInterface();

			// loop through each key element
			int numKeyElements = indexInterface->GetNKey();
			for(int element = 0; element < numKeyElements; ++element)
			{
				long index = indexInterface->GetKeyIndex(element);
				if(index < 0 || index >= wordLexiconInterface->GetNWords())
				{
					fprintf(stderr, "%s: invalid word index %d in key.\n", routine, index);
					return failure;
				}
				// set the actual phone
				/*printf("WordIndexEntry key %d -> %s (S, P, E = %d, %d, %d)\n", 
					index, wordLexiconInterface->GetWord(index), speaker, wordIndex, element);*/
				indexInterface->SetKey(element, wordLexiconInterface->GetWord(index));
			}
		}
	}

	// --- Post-processing to convert confusion stats -> probabilities ---
	long numConfusionInfos = headerInterface->GetNConfusions();
	for(long confusionInfo = 0; confusionInfo < numConfusionInfos; ++confusionInfo)
	{
		SpokenContentConfusionInfoDescriptor* confDescriptor = headerInterface->GetConfusion(confusionInfo);
		SpokenContentConfusionInfoDescriptorInterfaceABC* confInterface = confDescriptor->GetInterface();

		ProbabilityMatrixDescriptor* subDescriptor = confInterface->GetConfusionProb();
		ProbabilityMatrixDescriptorInterfaceABC* sub = subDescriptor->GetInterface();
		ProbabilityVectorDescriptor* insDescriptor = confInterface->GetInsertionProb();
		ProbabilityVectorDescriptorInterfaceABC* ins = insDescriptor->GetInterface();
		ProbabilityVectorDescriptor* delDescriptor = confInterface->GetOmissionProb();
		ProbabilityVectorDescriptorInterfaceABC* del = delDescriptor->GetInterface();

		long r, c, rows = sub->GetRows(), cols = sub->GetCols();
		
		/*printf("Substitution Counts:\n");
		for(r = 0; r < rows; ++r)
		{
			for(c = 0; c < cols; ++c)
				printf("%3.0f ", sub->GetValue(r, c));
			printf("\n");
		}*/

		// compute N[i] = occurrances of phone i in the canonical source phone-string
		float* N = new float[rows];
		for(r = 0; r < rows; ++r)
		{
			N[r] = del->GetValue(r);
			for(c = 0; c < cols; ++c)
				N[r] += sub->GetValue(r, c);
		}

		// compute I = total number of insertions
		float I = 0.0f;
		for(c = 0; c < cols; ++c)
		{
			I += ins->GetValue(c);
		}

		// P(p|d) = sub{i,j} / N{i}
		for(r = 0; r < rows; ++r)
		{
			for(c = 0; c < cols; ++c)
			{
				if(N[r] != 0.0f)
					sub->SetValue(sub->GetValue(r, c) / N[r], r, c);
			}
		}

		// P(p=0|d) = del{i} / N{i}
		for(r = 0; r < rows; ++r)
		{
			if(N[r] != 0.0f)
				del->SetValue(del->GetValue(r) / N[r], r);
		}

		// P(p|d=0) = ins{i} / I
		if(I != 0.0f)
		{
			for(c = 0; c < rows; ++c)
				ins->SetValue(ins->GetValue(c) / I, c);
		}

		/*printf("Substitution Probabilities:\n");
		for(r = 0; r < rows; ++r)
		{
			for(c = 0; c < cols; ++c)
				printf("%5.2f ", sub->GetValue(r, c));
			printf("\n");
		}*/
	}

	// --- Post-processing to resolve all lattice wordlink and phonelinks into words and phones	----
	const int numLattices = m_DescriptorInterface->GetNLattice();
	for(int lattice = 0; lattice < numLattices; ++lattice)
	{
		SpokenContentLatticeDescriptor* const latticeDescriptor = m_DescriptorInterface->GetLattice(lattice);
		SpokenContentLatticeDescriptorInterfaceABC* const latticeInterface = latticeDescriptor->GetInterface();

		const int numBlocks = latticeInterface->GetNBlocks();
		for(int block = 0; block < numBlocks; ++block)
		{
			SpokenContentBlockDescriptor* const blockDescriptor = latticeInterface->GetBlock(block);
			SpokenContentBlockDescriptorInterfaceABC* const blockInterface = blockDescriptor->GetInterface();

			// get the default speaker for this block
			const long defaultSpeaker = headerInterface->FindSpeaker(blockInterface->GetDefaultSpeaker());
			if(defaultSpeaker == -1)
			{
				fprintf(stderr, "%s: unknown default speaker in lattice %d block %d\n", routine, lattice, block);
				return failure;
			}
			SpokenContentSpeakerDescriptor* const defaultSpeakerDescriptor = headerInterface->GetSpeaker(defaultSpeaker);
			SpokenContentSpeakerDescriptorInterfaceABC* const defaultSpeakerInterface = defaultSpeakerDescriptor->GetInterface();

			const int numNodes = blockInterface->GetNNodes();
			for(int node = 0; node < numNodes; ++node)
			{
				SpokenContentNodeDescriptor* const nodeDescriptor = blockInterface->GetNode(node);
				SpokenContentNodeDescriptorInterfaceABC* const nodeInterface = nodeDescriptor->GetInterface();

				// find the speaker if this node isn't using the default speaker
				SpokenContentSpeakerDescriptorInterfaceABC* speakerInterface = defaultSpeakerInterface;
				if(nodeInterface->GetSpeaker() != 0)
				{
					const long speaker = headerInterface->FindSpeaker(nodeInterface->GetSpeaker());
					if(speaker == -1)
					{
						fprintf(stderr, "%s: unknown speaker in lattice %d block %d node %d\n", routine, lattice, block, node);
						return failure;
					}
					SpokenContentSpeakerDescriptor* const speakerDescriptor = headerInterface->GetSpeaker(speaker);
					speakerInterface = speakerDescriptor->GetInterface();
				}

				// resolve the word links
				const int numWordLinks = nodeInterface->GetNWordLinks();
				if(numWordLinks != 0)
				{
					// get the word lexicon
					const char* const lexicon = speakerInterface->GetWordLexiconID();
					long lexiconIndex = -1;
					if(lexicon == 0 || (lexiconIndex = headerInterface->FindWordLexicon(lexicon)) == -1)
					{
						fprintf(stderr, "%s: failed to find word lexicon for speaker %s\n", routine, speakerInterface->GetSpeakerID());
						return failure;
					}
					SpokenContentWordLexiconDescriptor* lexiconDescriptor = headerInterface->GetWordLexicon(lexiconIndex);
					SpokenContentWordLexiconDescriptorInterfaceABC* lexiconInterface = lexiconDescriptor->GetInterface();

					// resolve each word index with the lexicon
					for(int wordLink = 0; wordLink < numWordLinks; ++wordLink)
					{
						SpokenContentWordLinkDescriptor* const wordLinkDescriptor = nodeInterface->GetWordLink(wordLink);
						SpokenContentWordLinkDescriptorInterfaceABC* const wordLinkInterface = wordLinkDescriptor->GetInterface();

						const long wordIndex = wordLinkInterface->GetWordIndex();
						if(0 > wordIndex || wordIndex >= lexiconInterface->GetNWords())
						{
							fprintf(stderr, "%s: word index %d invalid for lattice %d block %d node %d wordlink %d", 
								routine, wordIndex, lattice, block, node, wordLink);
							return failure;
						}
						/*printf("WordLink %d -> %s (L, B, N, W = %d, %d, %d, %d)\n", 
							wordIndex, lexiconInterface->GetWord(wordIndex), lattice, block, node, wordLink);*/
						wordLinkInterface->SetWord(lexiconInterface->GetWord(wordIndex));
					}
				}

				// resolve the phone links
				const int numPhoneLinks = nodeInterface->GetNPhoneLinks();
				if(numPhoneLinks != 0)
				{
					// get the phone lexicon
					const char* const lexicon = speakerInterface->GetPhoneLexiconID();
					long lexiconIndex = -1;
					if(lexicon == 0 || (lexiconIndex = headerInterface->FindPhoneLexicon(lexicon)) == -1)
					{
						fprintf(stderr, "%s: failed to find phone lexicon for speaker %s\n", routine, speakerInterface->GetSpeakerID());
						return failure;
					}
					SpokenContentPhoneLexiconDescriptor* lexiconDescriptor = headerInterface->GetPhoneLexicon(lexiconIndex);
					SpokenContentPhoneLexiconDescriptorInterfaceABC* lexiconInterface = lexiconDescriptor->GetInterface();

					// resolve each phone index with the lexicon
					for(int phoneLink = 0; phoneLink < numPhoneLinks; ++phoneLink)
					{
						SpokenContentPhoneLinkDescriptor* const phoneLinkDescriptor = nodeInterface->GetPhoneLink(phoneLink);
						SpokenContentPhoneLinkDescriptorInterfaceABC* phoneLinkInterface = phoneLinkDescriptor->GetInterface();

						const long phoneIndex = phoneLinkInterface->GetPhoneIndex();
						if(0 > phoneIndex || phoneIndex >= lexiconInterface->GetNPhones())
						{
							fprintf(stderr, "%s: phone index %d invalid for lattice %d block %d node %d phonelink %d", 
								routine, phoneIndex, lattice, block, node, phoneLink);
							return failure;
						}
						/*printf("PhoneLink %d -> %s (L, B, N, W = %d, %d, %d, %d)\n", 
							phoneIndex, lexiconInterface->GetPhone(phoneIndex), lattice, block, node, phoneLink);*/
						phoneLinkInterface->SetPhone(lexiconInterface->GetPhone(phoneIndex));
					}
				}
			}
		}
	}

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentExtractionInterfaceABC *SpokenContentExtractionTool::GetInterface(void)
{ return &m_Interface; }

