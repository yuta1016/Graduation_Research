///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Zon-Yin shae and Xiping Wang	- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation InkSegment.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation InkSegment tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  InkSegmentSearch.cpp
//
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

/* number of directions chain-coded; max allowable = 255.
   these codes begin with 0 representing 360/NUMDIRS degrees centered
   around rightbound horizontal, then successive codes representing
   directions going counter-clockwise. */
#define DEFAULTNUMDIRS 16

/* type for chain-code data -- want unsigned bytes */
typedef unsigned char     t_Code;
/* special code indicating no move */
#define NOMOVE ((t_Code)0x77)

/* type for x & y co-ordinates.  Don't use unsigned because want to
   subtract them without having to coerce type. */
typedef short    t_Coord;
/* for marking a bogus coordinate */
#define DUD -32768

/* for any text line */
#define MAXLINELENGTH 256

#define DEFAULTCHAINALLOC 10000
#define DEFAULTNBEST 10

typedef struct chaincode {
  t_Code   NumDirs;
  float    *DirThresh;       /* direction thresholds of delta-y/delta-x */
} t_ChainCode;

static int ReadChainFile(char *filename, int *pnumpts, t_Code **pcodearray);
static int StkToChain(char *filename, t_Code **pcodearray, int *ppointsalloc, int *numpts, t_ChainCode ChainCode);
static int Distance(t_Code *codearray, int numpts, t_Code *templates, int templatelength);
static void recognizer(char *stroke, char *szPath, int nbest, int ndirection);

//=============================================================================

using namespace XM;

const UUID InkSegmentSearchInterface::myID = UUID("");
const char *InkSegmentSearchInterface::myName = "InkSegment Interface";

const UUID InkSegmentSearchTool::myID = UUID("");
const char *InkSegmentSearchTool::myName = "InkSegment Tool";

const UUID InkSegmentSearchInterfaceABC::myID = UUID();

//=============================================================================
InkSegmentSearchInterfaceABC::InkSegmentSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
InkSegmentSearchInterface::InkSegmentSearchInterface(InkSegmentSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
InkSegmentSearchInterface::~InkSegmentSearchInterface()
{
}

//----------------------------------------------------------------------------
void InkSegmentSearchInterface::destroy()
{  delete m_SearchTool; }


//----------------------------------------------------------------------------
int InkSegmentSearchInterface::SetInkSegmentInterface(GenericDSInterfaceABC
														   *aGenericDSInterface)
{
  return m_SearchTool->SetInkSegmentInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int InkSegmentSearchInterface::SetDescriptionInterface(GenericDSInterfaceABC
														   *aGenericDSInterface)
{
  return m_SearchTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int InkSegmentSearchInterface::SetInkSegmentParms(const char* packagefile, const char* descriptionfile)
{
	return m_SearchTool->SetInkSegmentParms(packagefile, descriptionfile);
}

//----------------------------------------------------------------------------
unsigned long InkSegmentSearchInterface::StartPackaging(void)
{
	return m_SearchTool->StartPackaging();
}

//----------------------------------------------------------------------------
const UUID& InkSegmentSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *InkSegmentSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
InkSegmentSearchTool::InkSegmentSearchTool():
m_Interface(this),
m_DescriptionInterface(0),
m_InkSegmentfile(0),
m_Descriptionfile(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
InkSegmentSearchTool::InkSegmentSearchTool(GenericDSInterfaceABC *theInkSegmentInterface,
									 GenericDSInterfaceABC *theDescriptionInterface) :
m_Interface(this),
m_InkSegmentInterface(0),
m_DescriptionInterface(0),
m_InkSegmentfile(0),
m_Descriptionfile(0)
{
	SetInkSegmentInterface(theInkSegmentInterface);
	SetDescriptionInterface(theDescriptionInterface);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
InkSegmentSearchTool::~InkSegmentSearchTool()
{
	delete [] m_InkSegmentfile;
	delete [] m_Descriptionfile;
}

//----------------------------------------------------------------------------
const UUID& InkSegmentSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *InkSegmentSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool InkSegmentSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool InkSegmentSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int InkSegmentSearchTool::SetInkSegmentInterface(GenericDSInterfaceABC
													  *aGenericDSInterface)
{
  m_InkSegmentInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int InkSegmentSearchTool::SetDescriptionInterface(GenericDSInterfaceABC
													  *aGenericDSInterface)
{
  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int InkSegmentSearchTool::SetInkSegmentParms(const char* packagefile, const char* descriptionfile)
{
	delete [] m_InkSegmentfile;
	if (packagefile) {
		m_InkSegmentfile = new char[strlen(packagefile)+1];
		strcpy(m_InkSegmentfile, packagefile);
	}
	else m_InkSegmentfile = 0;

	delete [] m_Descriptionfile;
	if (descriptionfile) {
		m_Descriptionfile = new char[strlen(descriptionfile)+1];
		strcpy(m_Descriptionfile, descriptionfile);
	}
	else m_Descriptionfile = 0;

return 0;

}


//----------------------------------------------------------------------------
unsigned long InkSegmentSearchTool::StartPackaging(void)
{
	recognizer(m_InkSegmentfile, m_Descriptionfile, 1, 16);
	
	return 0;
}

//----------------------------------------------------------------------------
InkSegmentSearchInterfaceABC *InkSegmentSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
static void recognizer(char *stroke, char *codebook, int nbest, int ndirection)
{
  //sth don't perform fileIO here, use the XML parser

	char   tempstring[MAXLINELENGTH], tempstringalt[MAXLINELENGTH];
	t_Code **templates, **templateptr;  /* for array of templates=seq of codes */
	char   **categories, **categoryptr; /* matching array of writer IDs=strings */
	int    *templatelengths, *templatelengthptr; /* array of code-seq lengths */
	int    numtemplates = 0, /*templatecounter = 0,*/
	  numnbest = DEFAULTNBEST, numans;
	char   *tempstrptr;
	t_ChainCode ChainCode;
	double pi2 = (double) 2.0 * acos((double)-1);
	float *ccptr;
	int numpts, pointsalloc /*, *intptr*/;
	int i,j,k,l;

	FILE *fp;
	t_Code *codearray;
	int score, flag, *bestscores;
	char **bestanswers;

	ChainCode.NumDirs = (t_Code) DEFAULTNUMDIRS;
	numnbest = nbest;
	i = ndirection;

	/* form N-best arrays */

	/* form chain code -- array of the cotangents thresholding highest angle
     (counting counter-clockwise from +x-axis) in each bin.  Bin 0 is centered
     around +x-axis.  So if numcodes=6, thresholds are 30, 90, 150, 210, 270 &
     330 degrees, and array is sqrt(3), 0, -sqrt(3),sqrt(3), 0, -sqrt(3).
     When using the chain code, the y>0 half-plane and the y<0 half-plane must
     be handled separately, as cotangent goes to infinity on the x-axis. */

	ccptr = ChainCode.DirThresh = (float *) malloc(ChainCode.NumDirs * sizeof(float));
	for(i = 0; i < ChainCode.NumDirs; i++, ccptr++) {
		if ((float) i != ((float) (ChainCode.NumDirs-1))/2.0)
			*ccptr = 1.0F / (float) tan(pi2*(double)((((float)i)+0.5)/(float)ChainCode.NumDirs));
		else
		  *ccptr = (float) -999999;  /* approx. negative infinity; should no be used anyway */
	}

	/* count templates */
	if ((fp = fopen(codebook, "r")) == NULL) {
		fprintf(stderr, "Error:  cannot open %s for reading.\n", codebook);
		exit(1);
	}

	while (fgets(tempstring, MAXLINELENGTH, fp) != NULL) numtemplates++;

	rewind(fp);

	if (NULL == (templates = (t_Code **) malloc(numtemplates * sizeof(t_Code *)))) {
		fprintf(stderr, "Error:  cannot allocate array for templates.\n");
		exit(1);
	}

	if (NULL == (templatelengths = (int *) malloc(numtemplates * sizeof(int)))) {
		fprintf(stderr, "Error:  cannot allocate array for template lengths.\n");
		exit(1);
	}

	if (NULL == (categories = (char **) malloc(numtemplates * sizeof(char *)))) {
		fprintf(stderr, "Error:  cannot allocate array for writer ID strings.\n");
		exit(1);
	}

	templatelengthptr = templatelengths;
	templateptr = templates;
	categoryptr = categories;

	for (i = 0; i < numtemplates; i++, templatelengthptr++, templateptr++, categoryptr++) {
		fgets(tempstring, MAXLINELENGTH, fp);
		tempstrptr = strtok(tempstring, " ");
		if (NULL == (*categoryptr = (char *)malloc((strlen(tempstrptr)+1)*sizeof(char)))) {
			fprintf(stderr, "Error:  allocation failure reading templates.\n");
			exit(1);
		}
		strcpy(*categoryptr,tempstrptr);
		tempstrptr = strtok(NULL, "\n");  /* cut the newline character off */

		if (0 != (ReadChainFile(tempstrptr, templatelengthptr, templateptr))) {
			fprintf(stderr, "Error:  reading template %s.\n", tempstrptr);
			exit(1);
		}
	}

	fclose(fp);

//	printf("Loaded %d templates; beginning recognition.\n", numtemplates);
	printf("Searching database in progress ...\n");

	pointsalloc = (int) DEFAULTCHAINALLOC;

	if (NULL == (codearray = (t_Code *) malloc(pointsalloc * sizeof(t_Code)))) {
		fprintf(stderr, "Error:  cannot allocate array for chain code.\n");
		exit(1);
	}

	if (NULL == (bestscores = (int *) malloc(numnbest * sizeof(int)))) {
		fprintf(stderr, "Error:  cannot allocate array for N-best scores.\n");
		exit(1);
	}

	if (NULL == (bestanswers = (char **) malloc(numnbest * sizeof(char *)))) {
		fprintf(stderr, "Error:  cannot allocate array for N-best answers.\n");
		exit(1);
	}

	strcpy(tempstring, stroke);
//	strtok(tempstring,"\n");  /* cut the newline character off */
	sprintf(tempstringalt, tempstring);

	if (1 == StkToChain(tempstringalt, &codearray, &pointsalloc, &numpts, ChainCode)) {
		fprintf(stderr, "Error converting %s to chain code; quitting.\n", tempstringalt);
		exit(1);
	}

	numans = 0; /* number of answers we already have on the stack */

	for (i = 0, templateptr = templates, templatelengthptr = templatelengths; i < numtemplates; i++, templateptr++, templatelengthptr++) {
		flag = 0; /* flag: -1 = current answer will not add to stack (may replace an entry
				   on stack though); 1 = current answer will add to stack (currently
				   unused); 0 = undetermined */

		/* score = negative distance, so that higher score is good */
		score = -Distance(codearray, numpts, *templateptr, *templatelengthptr);
		/* Make N-best list of DISTINCT answers.
		4+ possibilities for what the new score is:
		1.  First answer -- initialize stack with it.
		2.  Belongs somewhere within stack.
		A.  Displaces existing lower-scoring version of same word.  Shift portion of
		   stack between old & new score.
		B.  New word to stack; stack below limit --> grow stack, shift portion below
		   new word.
		C.  Stack is at limit -- push something else off bottom & shift portion below
		   new word.
		3.  Ditch due to existing higher-scoring version of same word.
		4.  Worse than whole stack
		A.  Stack can grow -- put new word at bottom.
		B.  Stack at limit -- ditch. */
		if (numans == 0) {     /* case 1 */
			numans = 1;
			bestscores[0] = score;
			bestanswers[0] = categories[i];
		} else {
			for(j = 0; j < numans; j++) {
				if (score > bestscores[j])
					break;       /* case 2 */
				if (!strcmp(categories[i], bestanswers[j]))	{
					flag = -1; /* case 3 */
					break;
				}
			}
			if (flag == 0 && j < numans) {
				/* case 2 */
				for(k = j; k < numans; k++)
					if (!strcmp(categories[i], bestanswers[k]))
						break; /* case 2A */
					if (k == numans) {
						if (numans < numnbest) {
							numans++; /* case 2B */
							flag = 1;
						} else
							k--; /* case 2C; clobber bottom of stack */
					}
					/* shift down everything between old & new rank for this wd, or below
					 rank for this word if it is a new word -- all the case 2s */
					for(l = k; l > j; l--){
						bestanswers[l] = bestanswers[l-1];
						bestscores[l] = bestscores[l-1];
					}
					/* insert new score & word */
					bestscores[j] = score;
					bestanswers[j] = categories[i];
			} else if (j == numans && numans < numnbest) {
				bestscores[numans] = score;     /* case 4A */
				bestanswers[numans] = categories[i];
				numans++;
			}
		}
	}

//	printf("Input %s: recognized category %s\n", tempstring, bestanswers[0]);

	printf("Best match : %s\n", bestanswers[0]);

//	if (numnbest > 1) {
//		printf("%d best:\n",numnbest);
//		for(i = 0;i < numans; i++)
//			printf("Answer %d Score %f Text %s\n", i+1, fScore[i], bestanswers[i]);
//	}

}

#define NUMCATEGORIES 4 /* # scoring categories */
#define INS_INDEX     0 
#define DEL_INDEX     1 
#define SUB_INDEX     2 
#define COR_INDEX     3 

static int MinIndex(int *pArray, int lNumIndices)
{
	int Min;
	int i, Index;

	Min = pArray[0];
	Index = 0;

	for (i = 1; i < lNumIndices; i++) {
		if (Min > pArray[i]) {
			Min   = pArray[i];
			Index = i;
		}
	}

	return(Index);
}

/**********************************************************
Trellis for matching strings codearray and template :
                   insertion
                  --------->
		  |\
		  | \
		  |  \
		  |   \
		  |    \
		  |     \
		  v      v
	      deletion   correct(or)substitution
*******************************************************/
		  
static int Distance(t_Code *codearray, int numpts, t_Code *templates, int templatelength)
{ 
	int  CostofDeletion, CostofInsertion, CostofSubstitution, CostofCorrect;
	int  **ppScoreBestPath, **ppScoreOfPath, **ppBestPath;
	int  lMaxPathLength;
	int  i, j, k, iLoop, oLoop;
	int  BestArc;
	int  Score;

	CostofDeletion = CostofInsertion = 5;
	CostofCorrect = 0;
	
	/* CostofSubstitution is computed as the distance between the codes */

	lMaxPathLength = numpts+templatelength + 4;

	/***************ALLOCATE MEMORY FOR BESTPATH *****************/
	/* ppBestPath contains the ID of the best path at any point in the Trellis */
	if ((ppBestPath = (int **)calloc(lMaxPathLength, sizeof(int *))) == NULL) {
		fprintf(stderr, "Error:  cannot allocate array for ppBestPath.\n");
		exit(1);
	}

	/* ppScoreBestPath contains the score of the best path at any point in the Trellis */
	if ((ppScoreBestPath = (int **)calloc(lMaxPathLength, sizeof(int *))) == NULL) {
		fprintf(stderr, "Error:  cannot allocate memory for ppScoreBestPath. \n");
		exit(1);
	}
  

	for (i = 0; i < lMaxPathLength; i ++) {
		if ((ppScoreBestPath[i] = (int *)calloc(lMaxPathLength, sizeof(int))) == NULL) {
			fprintf(stderr, "Error:  cannot allocate array for ppScoreBestPath[%d].\n", i);
			exit(1);
		}

		if ((ppBestPath[i] = (int *)calloc(lMaxPathLength, sizeof(int))) == NULL)	{
			fprintf(stderr, "Error:  cannot allocate array for ppBestPath[%d].\n", i);
			exit(1);
		}
	}

	/******************* ALLOCATE MEMORY FOR BESTPATH *********************/

	/******************* ALLOCATE MEMORY FOR ALLPATHS *********************/
	/* ppScoreBestPath contains the score of all possible path at any point in the Trellis */
	if ((ppScoreOfPath = (int **)calloc(lMaxPathLength, sizeof(int *))) == NULL) {
		fprintf(stderr, "Error:  cannot allocate array for templates.\n");
		exit(1);
	}

	for (i = 0; i < templatelength+1; i ++) {
		if ((ppScoreOfPath[i] = (int *)calloc(((numpts+1)*NUMCATEGORIES), sizeof(int))) == NULL) {
			fprintf(stderr, "Error:  cannot allocate array for templates.\n");
			exit(1);
		}
	}

	for (i = 0; i < templatelength+1; i ++) {
		for (j = 0; j < numpts+1; j ++) {
			/* max out the Score for all partial paths */
			/* remember we're interested in the minimum Score (cost) */
			for (k = 0; k < NUMCATEGORIES; k ++)
				ppScoreOfPath[i][j*NUMCATEGORIES + k] = 10000;
		}
	}

	/*************** ALLOCATE MEMORY FOR ALLPATHS ***************/

	/************************ TRELLIS **************************/
	for (oLoop = 0; oLoop < templatelength + 1; oLoop ++) {
		for (iLoop = 0; iLoop < numpts + 1; iLoop ++) {
			/* special case oLoop=0 only one path possible: series of deletions. */
			if (oLoop == 0) {
				if (iLoop > 0)
					ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + DEL_INDEX] =
					CostofDeletion + (ppScoreBestPath)[oLoop][iLoop-1];
			} else {
				if (iLoop == 0)   /* top row of trellis only Insertions possible */
					ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + INS_INDEX] =
					CostofInsertion + (ppScoreBestPath)[oLoop-1][iLoop];
				else      /* all other paths */
				{         /* substitution and correct look alike */
						  /* i.e. they take the same path on the trellis */
					ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + INS_INDEX] =
					CostofInsertion + (ppScoreBestPath)[oLoop-1][iLoop];

					if ((int)(codearray[iLoop-1]) == (int)(templates[oLoop-1])) {
						ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + COR_INDEX] =
						CostofCorrect + (ppScoreBestPath)[oLoop-1][iLoop-1];
					} else {
						CostofSubstitution = (abs(codearray[iLoop-1] - templates[oLoop-1]))%16;
						ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + SUB_INDEX] =
						CostofSubstitution + (ppScoreBestPath)[oLoop-1][iLoop-1];
					}

					ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + DEL_INDEX] =
					CostofDeletion + (ppScoreBestPath)[oLoop][iLoop-1];
				}
			}

			if (oLoop != 0 || iLoop != 0) {
				BestArc = (ppBestPath)[oLoop][iLoop] =
						MinIndex(&ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES],
						NUMCATEGORIES);
				(ppScoreBestPath)[oLoop][iLoop] =
				ppScoreOfPath[oLoop][iLoop*NUMCATEGORIES + BestArc];
			}
		} /* loop over input codes */
	}  /* loop over output codes */

	/***************** TRELLIS ***************************/
	Score = ppScoreBestPath[oLoop-1][iLoop-1];

	/******************** MEMFREE ************************/
	for (i = 0; i < lMaxPathLength; i ++) {
		free(ppScoreBestPath[i]);
		free(ppBestPath[i]);
	}
  
	free(ppScoreBestPath);
	free(ppBestPath);
  
	for (i = 0; i < templatelength+1; i ++)
		free(ppScoreOfPath[i]);
  
	free(ppScoreOfPath);

	/********************** MEMFREE ************************/
  
	return(Score);
}

static int ReadChainFile(char *filename, int *pnumpts, t_Code **pcodearray)
{
	FILE *fp;
	int num;
	t_Code *codearray;

	if ((fp = fopen(filename,"rb")) == NULL) {
		fprintf(stderr, "Error:  cannot open %s for reading.\n", filename);
		exit(1);
	}

	num = fread(pnumpts, sizeof(int), 1, fp);

	if (NULL == (codearray = (t_Code *) malloc(*pnumpts * sizeof(t_Code)))) {
		fprintf(stderr, "Error:  cannot allocate array for chain code while reading file %s.\n", filename);
		exit(1);
	}

	*pcodearray = codearray;
	fread(codearray, sizeof(t_Code), *pnumpts, fp);

	fclose(fp);

	return 0;
}

#define STRLEN 256

/* convert a .stk file into a chain-code binary file.  For an N-direction
   chain code, the codes x = 0 through N-1 are in counter-clockwise succession
   in 360/N-degree increments, with 0 centered around the horizontal.
   Mirroring those are codes 255 minus x which refer to same direction but
   spanning a stroke break; that is, the direction from the last point in
   one stroke to the first in another.  Finally, 128 represents consecutive
   points with the same co-ordinates.  So the length of the chain code =
   the number of points in the stk file minus 1. */

/* determine chain code for a pair of points (x1,y1) followed by (x2,y2).
   presumes they are not the same point */

t_Code PointsToChain(t_Coord x1, t_Coord y1, t_Coord x2, t_Coord y2, t_ChainCode ChainCode)
{
	t_Code Code;
	float invslope;

	if (y1 == y2) {   /* horizontal */
		if (x1 < x2) return (t_Code) 0;
		else if (x1 > x2) return (t_Code) ChainCode.NumDirs/2;
		else return (t_Code) NOMOVE;
	} else  {
		invslope = (x2-x1)/(float)(y2-y1);
		if (y1 < y2) {/* up-going */
			for(Code = 0; Code < (t_Code) ChainCode.NumDirs/2; Code++)
				if (ChainCode.DirThresh[Code] < invslope) break;
			return Code;
		} else {/* down-going */
			for(Code = ChainCode.NumDirs - 1; Code >= (t_Code) (ChainCode.NumDirs+1)/2; Code--)
				if (ChainCode.DirThresh[Code] > invslope) break;
					Code++;
				if (Code == ChainCode.NumDirs) Code = (t_Code) 0;
				return Code;
		}
	}
}
	
static int StkToChain(char *filename, t_Code **pcodearray, int *ppointsalloc, int *numpts, t_ChainCode ChainCode)
{
	FILE *fp;
	int strokesleft,pointsleftthisstroke;
	int i, pointsfound = 0, newstroke = 0;
	char tempstring[STRLEN];
	t_Coord oldx, oldy, newx, newy;
	t_Code *codearray;

	if (!(fp = fopen(filename, "r"))) {
		fprintf(stderr, "Error:  could not open %s for reading.\n", filename);
		exit(1);
	}

	if (NULL == fgets(tempstring, 256, fp)) goto error;/* get stroke-count line */
	strtok(tempstring, " ");
	strokesleft = (int) atoi(strtok(NULL, " "));/* stroke count = 2nd field */
	if (NULL == fgets(tempstring, 256, fp)) goto error;/* ignore text line */
	pointsleftthisstroke = 0;
	oldx = DUD;
	oldy = DUD;

	codearray = *pcodearray;

	/* get lines containing stroke headers and points */
	while (NULL != (fgets(tempstring, 256, fp))) {
		if (strokesleft == 0 && pointsleftthisstroke == 0) goto error;
		if (pointsleftthisstroke == 0) {      /* read a stroke header line */
			newstroke = 1;
			strtok(tempstring, " ");  /* point count in stroke = 2nd field */
			pointsleftthisstroke = (int) atoi(strtok(NULL, " "));
			if (pointsleftthisstroke + pointsfound > *ppointsalloc) {
				/* multiply allocation by 1.3, then make sure it's enough */
				*ppointsalloc = (int) (1.3 * (float) *ppointsalloc);
				if (pointsleftthisstroke + pointsfound > *ppointsalloc)
					*ppointsalloc = pointsleftthisstroke + pointsfound;
				codearray = (t_Code *) realloc(codearray, *ppointsalloc * sizeof(t_Code)); 
				if (NULL == codearray) {
					fprintf(stderr, "Error:  cannot allocate sufficient array for chain code.\n");
					exit(1);
				}
				*pcodearray = codearray;
			}
			strokesleft--;
		} else {    /* read a line with co-ordinates */
			for (i = pointsleftthisstroke; i > pointsleftthisstroke - 8 && i > 0; i--) {
				if (i==pointsleftthisstroke)
					newx = (t_Coord) atoi(strtok(tempstring, " "));
				else newx = (t_Coord) atoi(strtok(NULL, " "));
				newy = (t_Coord) atoi(strtok(NULL, " "));
				if (oldx != DUD && oldy != DUD) {
					codearray[pointsfound] = PointsToChain(oldx, oldy, newx, newy, ChainCode);
					if (newstroke == 1)
						codearray[pointsfound] = (t_Code) 255 - codearray[pointsfound];
					pointsfound++;
				}
				newstroke = 0;
				oldx = newx;
				oldy = newy;
			}
			pointsleftthisstroke = i;
		}
	}

	*numpts = pointsfound;
	if (pointsleftthisstroke == 0 && strokesleft == 0) return 0;

	error:
	fprintf(stderr, "Error:  malformed file %s; quitting.\n", filename);
	fclose(fp);

	return 1;
}
