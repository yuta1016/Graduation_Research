//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Dulce Ponceleon (Almaden Research Center, IBM, USA)
// Jan H. Pieper (Almaden Research Center, IBM, USA)
//
// (contributing organizations names)
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
// Applicable File Name:  SeqSummaryKeywordsExtraction.cpp
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <fstream>


#ifndef _VISUALC_
#include <sys/param.h>
#include <unistd.h>
#include <netinet/in.h>
#else
#include <io.h>
#include <direct.h>
//#include <strstrea.h>
#include <strstream>
#endif

#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#define FPWIN 1
#endif

#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "Utilities/readparam.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"              /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"


using namespace XM;

//===========================================================================
void makefullfilepathname(char *full_or_partial_filepathname,
			  char *extension = NULL)
{
#ifdef WIN32
	char drive[1024];
	char dir[1024], name[1024], ext[1024];
	char newname[1024];

	_splitpath(full_or_partial_filepathname, drive, dir, name, ext);
	if(!drive[0] || !dir[0])
	{
		_getcwd(newname, 1024);
		strcat(newname, "\\");
		if (extension)
		{
			strcat(newname, name);
			strcat(newname, extension);
		}
		else
		  strcat(newname, full_or_partial_filepathname);
		strcpy(full_or_partial_filepathname, newname);
	}
#else
	char newname[MAXPATHLEN];
	char *a;

	realpath(full_or_partial_filepathname, newname);
	if (extension) {
	  a=strrchr(newname, '.');
	  if (a) *a=0;
	  strncat(newname, extension, MAXPATHLEN-strlen(newname)-1);
	}
	strcpy(full_or_partial_filepathname, newname);
#endif
}


//----------------------------------------------------------------------------
int exit_err(int err, char *message = NULL)
{
	if (NULL != message)
	{
		printf("%s\n", message);
	}

	exit(err);
	return err;
}


//----------------------------------------------------------------------------
//returns false if success
bool read_line(FILE *file, char *buf)
{
	//this is tolerant of cross-platform non-native-EOL files
	//e.g. if you FTP a text file as binary, this avoids EOL and/or EOF problems
	int	c = fgetc(file);
	if (c < 0)
	{
		return true;	//nothing at all to read
	}

	do
	{
		if (0 != c && 13 != c && 10 != c)	//not an EOL char
		{
			*(buf++) = c;
		}	else	//eat all combinations of the entire CRLFnul EOL sequence
		{
			int	c2 = fgetc(file);
			if (c2 >= 0)
			{
				if (0 != c2 && 13 != c2 && 10 != c2 || c2 == c)	//not EOL or same EOL char
				{
					ungetc(c2, file);
				} else
				{
					int	c3 = fgetc(file);
					if (c3 >= 0)
					{
						if (0 != c3 && 13 != c3 && 10 != c3 || c3 == c || c3 == c2)	//not EOL or same EOL char
						{
							ungetc(c3, file);
						}
					}
				}
			}
			break;
		}
	}	while ((c = fgetc(file)) >= 0);

	*buf = 0;
	return false;
}


//----------------------------------------------------------------------------
//strips out non-alpha characters (e.g. apostrophies) and makes the alphas uppercase
void make_upper_alpha(char *string)
{
	if (NULL != string)
	{
		char	c, *dest = string;
		while (0 != (c = *string++))
		{
			if (c >= 'a' && c <= 'z')
			{
				*(dest++) = (c + 'A' - 'a');
			}	else if (c >= 'A' && c <= 'Z')
			{
				*(dest++) = c;
			}
		}
		*dest = 0;
	}
}


//----------------------------------------------------------------------------
struct word_elem
{
	word_elem *next;
	int instances;
	bool is_stopword;
	float timeStamps[1000];  //timeStamps for the occurrences of the word	
	char word[1];					   //the [1] makes room for terminator if you malloc(sizeof(stopword)+strlen(word))
};


struct word_list
{
	word_elem *head;
	word_elem *tail;
};


//----------------------------------------------------------------------------
void free_word_list(word_elem *wordList)
{ 
	word_elem *listPtr;
	word_elem *word;

	for (listPtr = wordList; listPtr != NULL;)
	{
		word = listPtr;
		strcpy(word->word, "\0");
		listPtr = word->next;
		word->next = NULL;
		free(word);
	}
}


//----------------------------------------------------------------------------
//returns NULL if not found
word_elem* find_word(word_elem *words, char *word)
{
	if (NULL != word)
	{
		char	first_char = *(word++);
		if (0 != first_char)
		{
			for (; NULL != words; words = words->next)
			{
				char	*words_word = words->word;
				if (first_char == *(words_word++))	//	quick-check first char
				{
					if (!strcmp(word, words_word))	//	compare the rest
					{
						return words;
					}
				}
			}
		}
	}

	return NULL;
}
	
	
//----------------------------------------------------------------------------
//This routine is used to insert a new CONTENT word in a wordlist
//The new word is inserted at the head of the list
//Returns pointer to updated wordlist

//@param wordlist List of words to insert new word into
//@param word word to be inserted
//@param seconds timeStamp for inserted word
word_elem* insert_new_word(word_elem *wordList, char *word, float seconds)
{
	int	len = strlen(word);
	word_elem *new_word; 
	if (NULL == (new_word = (word_elem*)malloc(sizeof(word_elem) + len)))
	{
		printf("ERROR: failed to allocate space for  a new CONTENT word\n");
		return NULL;
	} else
	{
		new_word->is_stopword = false; 
		new_word->instances = 1;
		new_word->timeStamps[0] = seconds;
		strcpy(new_word->word, word);
		new_word->next = wordList;
		wordList = new_word;
	}
	return wordList;
} 


//----------------------------------------------------------------------------
//This routine is used to extract the MOST FREQUENT word in the list of contentWords
//and insert it into the list of topWord. At the end, the latter will have the top K
//words of the transcript in decreasing order
//Returns pointer to EXTRACTED word
//@param prev_word pointer to the word previous to the one we want to extract
word_elem* remove_word(word_elem *prev_word)
{
	word_elem *extractedWord = prev_word->next;
	prev_word->next = extractedWord->next;
	extractedWord->next = NULL;
	return extractedWord;
}


//----------------------------------------------------------------------------
//@param topWordList List of top content words
//@param word word to be inserted
void transfer_word(word_list *topWordList, word_elem *word)
{
	if (topWordList->head == NULL) // Empty List, insert first word to be trasfered
	{
		topWordList->head = topWordList->tail = word;
	} else // Insert it at the end if the list
	{
		topWordList->tail->next = word; 
		topWordList->tail = word; 
	}
	word->next = NULL;
}


//----------------------------------------------------------------------------
//This routine finds a pointer to the FIRST word that has the 
//MAX instances in the contentWord list.
//It returns a pointer to the prev_word or the MAX word so that we can remove it from the list
word_elem* find_max_word(word_elem *contentWords)
{
	word_elem *word;  // pointer to word previous to the word being tested
	word_elem *currentWord;
	word_elem *maxWord = NULL;
	int max = 0;

	if (NULL == contentWords->next) // ContentWords List Conatins only dummy element, hence it is empty
	{
		return maxWord;
	}

	for (word = contentWords; NULL != word->next; word = word->next)
	{
		currentWord = word->next;
		if (currentWord->instances > max )
		{
			max = currentWord->instances;
			maxWord = word;  // save pointer to word previous to maxWord;
		} 
	}
	return maxWord;
}

//=============================================================================

const UUID SeqSummaryKeywordsExtractionInterface::myID = UUID("");
const char *SeqSummaryKeywordsExtractionInterface::myName = "SeqSummaryKeywordsExtractionInterface";

const UUID SeqSummaryKeywordsExtractionTool::myID = UUID("");
const char *SeqSummaryKeywordsExtractionTool::myName = "SeqSummaryKeywordsExtraction";

const UUID SeqSummaryKeywordsExtractionInterfaceABC::myID = UUID();

//=============================================================================
SeqSummaryKeywordsExtractionInterfaceABC::SeqSummaryKeywordsExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SeqSummaryKeywordsExtractionInterface::SeqSummaryKeywordsExtractionInterface(SeqSummaryKeywordsExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SeqSummaryKeywordsExtractionInterface::~SeqSummaryKeywordsExtractionInterface()
{
}

//----------------------------------------------------------------------------
void SeqSummaryKeywordsExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//----------------------------------------------------------------------------
int SeqSummaryKeywordsExtractionInterface::SetSourceMediaFilename(char *aFilename)
{
  return m_ExtractionTool->SetSourceMediaFilename(aFilename);
}

//----------------------------------------------------------------------------
int SeqSummaryKeywordsExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsExtractionInterface::NewSequence(void)

{
  return m_ExtractionTool->NewSequence();
}

//----------------------------------------------------------------------------
const UUID& SeqSummaryKeywordsExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SeqSummaryKeywordsExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
SeqSummaryKeywordsExtractionTool::SeqSummaryKeywordsExtractionTool():
m_Interface(this),

m_MediaFilename(0)

{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SeqSummaryKeywordsExtractionTool::
SeqSummaryKeywordsExtractionTool(GenericDSInterface *theParent):
m_Interface(this),

m_MediaFilename(0)

{
  SetDescriptorInterface(theParent);
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SeqSummaryKeywordsExtractionTool::~SeqSummaryKeywordsExtractionTool()
{

}

//----------------------------------------------------------------------------
bool SeqSummaryKeywordsExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SeqSummaryKeywordsExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
/*
int SeqSummaryKeywordsExtractionTool::SetSourceMedia(MomVop *media)
{
  if (!media) return -1;

  m_Media=media;
  return 0;
}

//----------------------------------------------------------------------------
int SeqSummaryKeywordsExtractionTool::
SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  m_FrameNumber=FrameNo;
  return 0;
}
*/
//----------------------------------------------------------------------------
int SeqSummaryKeywordsExtractionTool::SetSourceMediaFilename(char *aFilename)

{
  m_MediaFilename=aFilename;
  return 0;
}

//----------------------------------------------------------------------------
int SeqSummaryKeywordsExtractionTool::SetDescriptorInterface(
     GenericDSInterfaceABC *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptorInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsExtractionTool::InitExtracting(void)

{
#define MAXFPFILENAMELEN 256
//	char filename[MAXFPFILENAMELEN];


	printf("Init\n");

  /* read parameters from parameter file*/
	m_stopwordFilename=getpara("stopwordFilename", NULL);
	m_printAllTimestamps=strcmp("true", getpara("printAllTimestamps", "false"));
	m_numTopK=atoi(getpara("numTopKeywords", "10"));

	/* create basic structure of description*/
	NewSequence();
	

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsExtractionTool::StartExtracting(void)
{
	printf("StartExtracting\n");

	unsigned long returnval=0;
	bool stop_words;

	char	*filenames[] = {m_stopwordFilename, m_MediaFilename};
	int num_files = 2;

	FILE	**files = (FILE**) calloc(3, sizeof(FILE*));
	int	fileStart;
	int i;
	char	buffer[2048];

	if (m_stopwordFilename != NULL)
	{
		fileStart=0;
		stop_words=true;
	} else //do not read stopwords
	{
		fileStart=1;
		stop_words = false;
		std::cout<<"no stopwordfile specified."<<std::endl;
	}

	for (i=fileStart; i<num_files; i++)
	{
		buffer[0] = 0;
		strcpy(buffer, filenames[i]);
		makefullfilepathname(buffer);
		FILE	*file = fopen(buffer, "rb");
		if (NULL == file)
		{
			char	message[2100];
			sprintf(message, "cannot open file: %s", buffer);
			exit_err(2, message);
		}
		files[i] = file;
	}

	word_elem	*stopWords = NULL;

	if (stop_words)
	{
		//	scan through files[0] and make a linked list of all 
		//  the stopwords, converted to upper case and punctuation removed
		while (!read_line(files[0], buffer))
		{
			char	*s = strchr(buffer, ' ');
			if (NULL != s)	//	ignore stuff after the word
			{
				*s = 0;
			}

			make_upper_alpha(buffer);

			int	len = strlen(buffer);
			if (len > 0)
			{
				word_elem	*new_stopword = (word_elem*) malloc(sizeof(word_elem) + len);
				if (NULL != new_stopword)
				{
					new_stopword->instances = 1;
					strcpy(new_stopword->word, buffer);
					new_stopword->next = stopWords;
					stopWords = new_stopword;
				} else 
				{
					printf("ERROR; failure to allocate space for new STOP WORD\n");
					return exit_err(2);
				}
			}
		}
	}

	word_elem	*contentWords = NULL;
	for (float prev_seconds = -.1; !read_line(files[1], buffer);)
	{
//
//		Sample Line from Text File containing the output of the VIAVOICE Speech Recognition Engine
//
//		cell     1		0.27    0.38           36  FUNDING,-11,FIND,-3,AND,-7,WIN,-6,AND,-6,WHEN,-7,AND,-6,FIND,-3,HIM,-10,OPEN,-14,FIND,-3,THEM,-11,FUNDING,-11,TO,-11,FUNDING,-11,OF,-13,FIND,-3,AND,-7,TO,-11,WIN,-6,AND,-6,TO,-11,FIND,-3,AND,-7,OF,-13,WHEN,-7,AND,-6,TO,-11,FIND,-3,HIM,-10,TO,-11,WIN,-6,AND,-6,OF,-13,OPEN,-14,TO,-11,
//
//		where  
//				TimeStamp = 0.27  where the word FUNDING occurred
//				ConfidenceScore = 0.38 
//				Best-1 Word = FUNDING
//				List of Alternate words = FIND, AND, WIN, AND, WHEN, AND, FIND, ...
//				

		char	*s = buffer;
		s = strtok(s, " \t");	//	"cell"
		if (NULL == s || !*s)	//	if no token found, skip to process next line
		{
			continue;
		}
		do	{	}	while (*(s++));//skip past \0 at end of token (equivalent to s+=(1+strlen(s))

		s = strtok(s, " \t");	//	"1"
		if (NULL == s || !*s)
		{
			continue;
		}
		do	{	}	while (*(s++));

		s = strtok(s, " \t");	//	"0.27"
		if (NULL == s || !*s)
		{
			continue;
		}
		float seconds = (float)atof(s);
	
		do	{	}	while (*(s++));

		s = strtok(s, " \t");	//	"0.38"
		if (NULL == s || !*s)
		{
			continue;
		}
		do	{	}	while (*(s++));

		s = strtok(s, " \t");	//	"36"
		if (NULL == s || !*s)
		{
			continue;
		}
		do	{	}	while (*(s++));

		s = strtok(s, " \t,");	//	"FUNDING"
		if (NULL == s || !*s)
		{
			continue;
		}

		char	*word = s;
		make_upper_alpha(word);
//		int	len = strlen(word);

		if (seconds < prev_seconds || seconds < 0)
		{
			printf("ERROR: The time stamps in the speech transcript should be in increasing order\n");
			printf("	   TimeStamp = %10.3f is smaller than the previous timeStamp\n", seconds);
			return exit_err(1);
		} else if (seconds == prev_seconds)
		{
			printf("WARNING: TIME STAMPS SHOULD BE STRCITLY INCREASING\n");
			printf("Two equal time stamps where found %f \n", seconds);
			//return exit_err(1);
		}
		prev_seconds = seconds;


		//Look in the contentWord list. Assuming that this list is shorter than the stop words 
		//list it is more efficient to look here first. Also high frequency contentWords will be found
		//quickly
		word_elem	*found_word = find_word(contentWords, word);

		//cout<<"*** "<<word<<"... "<<seconds<<endl; //FOR DEBUGGING PURPOSES 

		if (NULL != found_word)
		{
			// It is a contentWord that we have found earlier in the transcript
			found_word->timeStamps[found_word->instances] = seconds;
			found_word->instances++;
		}	else if (!stop_words)
		{
			contentWords = insert_new_word(contentWords, word, seconds);	
		}	else if (NULL == find_word(stopWords, word))
		{
			//Not a stopword, i.e. Found first occurence of contentWord, so insert it
			contentWords = insert_new_word(contentWords, word, seconds);
		}
	}

	for (i=fileStart; i<num_files; i++)
	{
		fclose(files[i]);
	}

	//Compute MAX occurence of contentWord
//	word_elem   *max_word = NULL;
	word_elem	*word;
	int	max = 0; //int counter =0;
//	int total_words = 0;

	
	//FOR DEBUGGING , just check how many total contentWord do we get
	/*
	for (word = contentWords; NULL != word; word = word->next) total_words++;
	{
		printf("max ===> %d TOTAL WORDS %d\n", max, total_words);
	}
	*/

	//Insert dummy first element in contentWords, it assumes ONE instance
	//we set it -1 so that this first element is NEVER the MAX found
	contentWords = insert_new_word(contentWords, "my_dummy_word", 0.1);
	contentWords->instances = -1;

	word_list *topWords; topWords = new(word_list); 
	topWords->head = topWords->tail = NULL;

	word_elem *prev_word;
	word_elem *extractedWord;
	int topK = m_numTopK;
	int topWordCounter;

	for (topWordCounter=0; topWordCounter<topK; )
	{
		
		if (NULL == (prev_word = find_max_word(contentWords)))
		{
			printf("Requested number of top words is greater than available in transcript\n");
			printf("Only %d words have been found\n", topWordCounter);
			break;
		}
		
		if (NULL == prev_word->next)
		{
			printf("This should NEVER happen\n");
			return exit_err(3);
		}
		max = prev_word->next->instances;

		while ((NULL != prev_word->next) && (topWordCounter < topK))
		{
			if(prev_word->next->instances == max)
			{
				extractedWord = remove_word(prev_word); 
				transfer_word(topWords, extractedWord);
				//if (prev_word->next != NULL) {printf("WORD = %s\n", prev_word->next->word);} else {printf("NULL\n");} //FOR DEBUGGING PURPOSES 
				topWordCounter++;  //counter for extracted words
			} else 
			{
				prev_word = prev_word->next;
			}
		} 
	}

  
//	int extracted = topWordCounter;

	char temp[200];
	bool firstRun = true;

 	for (word=topWords->head; NULL!=word; word=word->next)
	{
		//cout<<word->word<<"  "<<word->instances<<"  "<<word->timeStamps[0]<<"  "<<word->timeStamps[word->instances-1]<<endl; //FOR DEBUGGING PURPOSES 

		#ifdef USEXML 
			GenericDS l_TextProperty=m_SequentialSummaryInterface->CreateChild("TextProperty");
			GenericDSInterfaceABC *l_TextPropertyInterface=l_TextProperty.GetInterface();
		
			GenericDS l_FreeText=l_TextPropertyInterface->CreateChild("FreeText");
			GenericDSInterfaceABC *l_FreeTextInterface=l_FreeText.GetInterface();

			sprintf(temp,"%s (%i)", word->word, word->instances);
			l_FreeTextInterface->SetValue(temp);
			l_FreeTextInterface->SetAttribute("xml:lang", "eng-us");

			GenericDS l_RefTime=l_TextPropertyInterface->CreateChild("RefTime");
			GenericDSInterfaceABC *l_RefTimeInterface=l_RefTime.GetInterface();

			strcpy(temp, "PT");
			ms2time(((int)(word->timeStamps[0] * 1000.0)), temp);
			if (firstRun) 
			{
				//this needs only to be done once
				firstRun=false;
				strcat(temp, "1000F");
				l_RefTimeInterface->CreateChild("RelMediaTimePoint").GetInterface()->SetValue(temp).SetAttribute("TimeBase", "RefLocator (1)");
			} else
			{
				l_RefTimeInterface->CreateChild("RelMediaTimePoint").GetInterface()->SetValue(temp);
			}

			int timestampPos;
			if (m_printAllTimestamps == 0)
			{
				timestampPos=1;
			} else 
			{
				timestampPos=word->instances-1;
			}

			for ( ; timestampPos<word->instances; timestampPos++)
			{
				l_RefTime=l_TextPropertyInterface->CreateChild("RefTime");
				l_RefTimeInterface=l_RefTime.GetInterface();

				strcpy(temp, "PT");
				ms2time(((int)(word->timeStamps[timestampPos] * 1000.0)), temp);
				l_RefTimeInterface->CreateChild("RelMediaTimePoint").GetInterface()->SetValue(temp);
			}
		#endif
	}

	free_word_list(contentWords);
	free_word_list(stopWords);
	free_word_list(topWords->head);
	topWords->head = topWords->tail = NULL;
	free(topWords);

	return returnval;
}


//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsExtractionTool::NewSequence(void)

{
	if (!m_DescriptorInterface)  return (unsigned long)-1;

	char filename[200];

	/* create sequential description*/
	m_SequentialSummary=m_DescriptorInterface->CreateChild("SequentialSummary");
	m_SequentialSummaryInterface=m_SequentialSummary.GetInterface();

	/* create AudioSegmentLocator if file name is available*/
	if (m_MediaFilename) {
	  GenericDS l_RefLocator=m_SequentialSummaryInterface->CreateChild("RefLocator");
	  GenericDSInterfaceABC *m_RefLocatorInterface=l_RefLocator.GetInterface();

	  GenericDS l_MediaURL=m_RefLocatorInterface->CreateChild("MediaURL");
	  GenericDSInterfaceABC *l_MediaURLInterface=l_MediaURL.GetInterface();

		strcpy(filename,m_MediaFilename);
		strcat(filename, ".mpg");

		l_MediaURLInterface->SetValue(filename);
	}
	return 0;
}

//----------------------------------------------------------------------------

/** convert a given time from milliseconds to a MediaTime string (xxxHxxMxxSxxxN).
 *	The result will be appended (!) to the given result string. This method
 *  uses milliseconds as unit for N.
 *
 *  @param ms  time in milliseconds
 *  @param res char* to receive the result of the method. The MediaTime string will be appended to the given string.
 */
void SeqSummaryKeywordsExtractionTool::ms2time(int ms, char* res)
{
	char time[200];
	char str[200];

	int temp = ms;
	int msec, sec, min, h;

	msec=temp%1000;
	temp=(int) temp/1000;
	sec=temp%60;
	temp=(int) temp/60;
	min=temp%60;
	h=(int) temp/60;

	strcpy(time, res);

	if (h>0)
	{
		sprintf(str,"%iH", h);
		strcat(time, str);
	}

	if (min>0)
	{
		sprintf(str,"%iM", min);
		strcat(time, str);
	}

	if (sec>0)
	{
		sprintf(str,"%iS", sec);
		strcat(time, str);
	}
	
	if (ms>0)
	{
		sprintf(str,"%iN", msec);
		strcat(time, str);
	}

	strcpy(res, time);
}
//----------------------------------------------------------------------------
const UUID& SeqSummaryKeywordsExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SeqSummaryKeywordsExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SeqSummaryKeywordsExtractionInterfaceABC *SeqSummaryKeywordsExtractionTool::GetInterface(void)
{ return &m_Interface; }


