//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
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
// Applicable File Name:  SegmentRelGraphExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */


//=============================================================================

using namespace XM;


const UUID SegmentRelGraphExtractionInterface::myID = UUID("");
const char *SegmentRelGraphExtractionInterface::myName = "SegmentRelGraphDSExtractionInterface";

const UUID SegmentRelGraphExtractionTool::myID = UUID("");
const char *SegmentRelGraphExtractionTool::myName = "SegmentRelGraphExtraction";
const int	SegmentRelGraphExtractionTool::STRING_MAXSIZE= 4096;	// maximum size of the strings

const UUID SegmentRelGraphExtractionInterfaceABC::myID = UUID();


//=============================================================================
SegmentRelGraphExtractionInterfaceABC::SegmentRelGraphExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
SegmentRelGraphExtractionInterface::SegmentRelGraphExtractionInterface(SegmentRelGraphExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SegmentRelGraphExtractionInterface::~SegmentRelGraphExtractionInterface()
{
}

//----------------------------------------------------------------------------
void SegmentRelGraphExtractionInterface::destroy()
{
  delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int SegmentRelGraphExtractionInterface::SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, 
	char *pcMasksName)
{
  return m_ExtractionTool->SetSourceMedia(mvFS, fp, pcFramesName, pcMasksName);
}

//----------------------------------------------------------------------------
int SegmentRelGraphExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& SegmentRelGraphExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentRelGraphExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
SegmentRelGraphExtractionTool::SegmentRelGraphExtractionTool():
m_Interface(this),
m_DescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SegmentRelGraphExtractionTool::
SegmentRelGraphExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(0)
{
        SetDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SegmentRelGraphExtractionTool::~SegmentRelGraphExtractionTool()
{

}

//----------------------------------------------------------------------------
bool SegmentRelGraphExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SegmentRelGraphExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SegmentRelGraphExtractionTool::SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, 
	char *pcMasksName)
{
  
	m_mvFS			= mvFS;
	m_fp			= fp;
	m_pcFramesName	= pcFramesName;
	m_pcMasksName	= pcMasksName;

	return(0);
}

//----------------------------------------------------------------------------
int SegmentRelGraphExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphExtractionTool::InitExtracting(void)
{

	/* Do not need this */

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphExtractionTool::StartExtracting(void)
{

	fprintf( stderr, "Extraction of Segment relation Graph\n");

	/* Start parsing the tree file and extract segment relation graph information */
	ExtractSegmentRelGraph(m_DescriptionInterface);

	return 0;
}

//----------------------------------------------------------------------------
const UUID& SegmentRelGraphExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentRelGraphExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SegmentRelGraphExtractionInterfaceABC *SegmentRelGraphExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int SegmentRelGraphExtractionTool::ReadSegmentInfoTree( FILE *fp, int iSegmentID, char *pcSegmentType, 
	int*piSegmentNumChildren, int **ppiSegmentChildIDList, char *pcDecompType, char *pcOverlap, 
	char *pcGap, char *pcSegmentAnno)
{
	
	int iID;
	int iChild;
	
	/* Go to the beggining of the file */
	fseek( fp, 0, SEEK_SET); 

	/* Scan line by line until the SegmentRelGraphID is found */
	while (fscanf( fp, "%d %s %d", &iID, pcSegmentType, piSegmentNumChildren) == 3) {

		if (iID==iSegmentID) {
			
			/* Read children */
			if ((*piSegmentNumChildren)==0) {
				(*ppiSegmentChildIDList) = NULL;
			} else {
				(*ppiSegmentChildIDList) = new int[(*piSegmentNumChildren)];
				for (iChild=0; iChild<(*piSegmentNumChildren); iChild++) {
					fscanf( fp, "%d", &(*ppiSegmentChildIDList)[iChild]);
				}

				/* Read Decomposition Type */
				fscanf( fp, " %s %s %s", pcDecompType, pcOverlap, pcGap);
			}
			
			/* Read Annotation (rest of the line) */
			fgets( pcSegmentAnno, STRING_MAXSIZE, fp);
			if (strlen(pcSegmentAnno)>0) {
				pcSegmentAnno[strlen(pcSegmentAnno)-1] = '\0';	/* remove last newline */
			}

			return(0);
			
		} else {
		
			/* Move to the begining of the next line or the end of file */
			int c = getc(fp);
			while(c!='\n' && c!= EOF) {
				c = getc(fp);
			}
		}

	}

	/* Couldn't read anymore, finish tree file */
	return(-1);
	

}

//----------------------------------------------------------------------------
void SegmentRelGraphExtractionTool::ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID, 
	char *pcSegmentType, int *piSegmentNumFrames, int **ppiSegmentFramesList, 
	int **ppiSegmentMasksList)
{

	int i,k;
	int iNumFrames;					// Number of frames 
	unsigned char *pucMediaPtr;		// Pointer to segment row in the media FS file
	int *piFramesList;				// List of frames
	int *piMasksList;				// List of Masks
	
	/* Go to line iSegmentID of the media and read it */
	if ( (iSegmentID-1)>=int(mediaFS->y_chan->y)) {
		fprintf( stderr, "ERROR(SegmentRelGraphExtractionTool): Couldn't find the SegmentD in the FS file.\n");
		exit(-1);
	}

	/* TESTING Print the image channel values in a file: y.txt!! */
	FILE *fa = fopen("y.txt", "w");
	pucMediaPtr = mediaFS->y_chan->data->u;
	for (i=0; i<mediaFS->y_chan->x*mediaFS->y_chan->y; i++) {
		fprintf(fa, "%d ", pucMediaPtr[i]);
		if ((i+1)%mediaFS->y_chan->x == 0) {
			fprintf(fa, "\n");
		}
	}
	fclose(fa);
	fa = fopen("u.txt", "w");
	pucMediaPtr = mediaFS->u_chan->data->u;
	for (i=0; i<mediaFS->y_chan->x*mediaFS->y_chan->y; i++) {
		fprintf(fa, "%d ", pucMediaPtr[i]);
		if ((i+1)%mediaFS->y_chan->x == 0) {
			fprintf(fa, "\n");
		}
	}
	fclose(fa);
	fa = fopen("v.txt", "w");
	pucMediaPtr = mediaFS->v_chan->data->u;
	for (i=0; i<mediaFS->y_chan->x*mediaFS->y_chan->y; i++) {
		fprintf(fa, "%d ", pucMediaPtr[i]);
		if ((i+1)%mediaFS->y_chan->x == 0) {
			fprintf(fa, "\n");
		}
	}
	fclose(fa);


	/* Count Number of Frames */
	pucMediaPtr = (mediaFS->y_chan->data->u) + ((iSegmentID-1)*mediaFS->y_chan->x); 
	for (iNumFrames=0, i=0; i<int(mediaFS->y_chan->x); i++) {
		if (pucMediaPtr[i]!=0) { iNumFrames++;}
	}

	/* Check for no frames */
	if (iNumFrames==0) {
		fprintf( stderr, "ERROR(SegmentRelGraphExtractionTool): Found a Segment with No frames!!!\n");
		//exit(-1);
		return;
	}

	/* Create frame vector list */
	piFramesList = new int[iNumFrames];	/* frame vector list */
	piMasksList  = new int[iNumFrames];	/* mask vector list */
					
	/* 'i' is the x coordinate (correspond to frame number) */
	/* 'k' is the pointer inside piFrameList */
	for (i=0,k=0; i<int(mediaFS->y_chan->x); i++) {
		if (pucMediaPtr[i]>0) { 
			piFramesList[k] = i;
			if (pucMediaPtr[i]==255) {
				piMasksList[k] = iSegmentID;
			} else {
				piMasksList[k] = int(pucMediaPtr[i]);
			}
			k++;
		}
	}
					
	/* Return data */
	(*piSegmentNumFrames)	= iNumFrames;
	(*ppiSegmentFramesList) = piFramesList;
	(*ppiSegmentMasksList)	= piMasksList;

}

//----------------------------------------------------------------------------
void SegmentRelGraphExtractionTool::ExtractSegmentRelGraph(GenericDSInterfaceABC *parentDescriptionInterface)
{
	/* NOTES ExtractSegmentRelGraph() method of the SegmentRelGraphExtractionTool
	 
	   One temporal graph is defined for all the segments
	*/

	/* ------------------------------------------------------------------------
	   Variables declaration
	   ------------------------------------------------------------------------ */

	int		iSegmentNumChildren;			/* Number of Children */
	int		*piSegmentChildIDList = NULL;	/* Vector list with Children's ID */
	char	pcDecompType[STRING_MAXSIZE];
	char	pcOverlap[STRING_MAXSIZE];
	char	pcGap[STRING_MAXSIZE];
	char	pcSegmentAnno[STRING_MAXSIZE];	/* Segment AnnotationDS */

	int iNumSegments = 0;
	int *piSegmentNumFrameList = NULL;
	int **ppiSegmentFramesLists = NULL;
	int **ppiSegmentMasksLists = NULL;

	char pcSegmentType[STRING_MAXSIZE];	/* Type of segment ("VideoSegment", "MovingRegion" or "StillRegion") */
	int i;

	char **ppcSegmentRelationsList;	/* A relation for each pair of segments */
	int iNumRelations;


	/* ------------------------------------------------------------------------
	   Find out the number of segments in Info Tree file
	   Segments are assumed to have IDs from 0 to iNumSegments-1
	   ------------------------------------------------------------------------ */
	//fprintf(stderr, "Dimensions of FS image: %d %d\n", m_mvFS->y_chan->y, m_mvFS->y_chan->x);
	for (i=0; i<int(m_mvFS->y_chan->y); i++) {
		//fprintf(stderr, "Looking for segment: %d\n", i+1);
		if (ReadSegmentInfoTree( m_fp, i+1, pcSegmentType, &iSegmentNumChildren, 
				&piSegmentChildIDList, 	pcDecompType, pcOverlap, pcGap, pcSegmentAnno) != 0) {
			
			break;
		} 
	}	
	iNumSegments = i;
	fprintf(stderr, "Number of segments: %d\n", i);


	/* ------------------------------------------------------------------------
	   Read Segment Info from FS file for all the Segments
	   ------------------------------------------------------------------------ */

	piSegmentNumFrameList = new int[iNumSegments];
	ppiSegmentFramesLists = new int*[iNumSegments];
	ppiSegmentMasksLists = new int*[iNumSegments];
	for (i=0; i<iNumSegments; i++) {
		ReadSegmentInfoFS( m_mvFS, i+1, pcSegmentType, &piSegmentNumFrameList[i], 
			&ppiSegmentFramesLists[i], &ppiSegmentMasksLists[i]);
		//fprintf(stderr, "Segment %d - Number of frames: %d\n", i+1, piSegmentNumFrameList[i]);
	}


	/* ------------------------------------------------------------------------
	   Extract temporal relations, one for each pair of segments, among the
	   following: Before, After, Start.
	   ------------------------------------------------------------------------ */

	SegmentRelationsExtraction(iNumSegments, ppiSegmentFramesLists, 
									&ppcSegmentRelationsList, &iNumRelations);


	/* ------------------------------------------------------------------------
	   Create the Segment Graph DS
	   ------------------------------------------------------------------------ */

	SegmentRelGraphEncoding(parentDescriptionInterface, iNumSegments, ppcSegmentRelationsList, 
								iNumRelations);

	/* ------------------------------------------------------------------------
	   Print the Result
	   ------------------------------------------------------------------------ */
/*	fprintf( stderr, "Read SegmentRelGraph %d, type \"%s\", with %d children.\n\n", 
		iSegmentID, pcSegmentType, iSegmentNumChildren);
*/

	/* ------------------------------------------------------------------------
	   Release memory
	   ------------------------------------------------------------------------ */
	if (piSegmentChildIDList!=NULL) {
		delete [] piSegmentChildIDList;
	}
	if (ppiSegmentMasksLists!=NULL) {
		for (i=0; i<iNumSegments; i++) {
			if (ppiSegmentMasksLists[i] != NULL) {
				delete [] ppiSegmentMasksLists[i];
			}
		}
		delete [] ppiSegmentMasksLists;
	}
	if (ppiSegmentFramesLists!=NULL) {
		for (i=0; i<iNumSegments; i++) {
			if (ppiSegmentFramesLists[i] != NULL) {
				delete [] ppiSegmentFramesLists[i];
			}
		}
		delete [] ppiSegmentFramesLists;
	}
	if (piSegmentNumFrameList!=NULL) {
		delete [] piSegmentNumFrameList;
	}
	if (ppcSegmentRelationsList!=NULL) {
		for (int i=0; i<iNumRelations; i++) {
			if (ppcSegmentRelationsList[i]!=NULL) {
				delete [] ppcSegmentRelationsList[i];
			}
		}
		delete [] ppcSegmentRelationsList;
	}
}

//----------------------------------------------------------------------------
void SegmentRelGraphExtractionTool::SegmentRelationsExtraction(int iNumSegments,
					int **ppiSegmentFramesLists, char ***pppcSegRelsList, int *piNumRels) 
{
	int iNumRelations;
	char **ppcSegmentRelationsList;

	int i, j, k;


	/* ------------------------------------------------------------------------
	   Discover a temporal relation for each pair of segments among the
	   following: Before, After, Start.
	   N segments => N*(N-1)/2 relations.
	   Only look at the start time of each segment to generate these.
	   In the future, other relations can be considered.
	   ------------------------------------------------------------------------ */

	iNumRelations = iNumSegments*(iNumSegments-1)/2;

	/* Allocate memory for segment relations */
	ppcSegmentRelationsList = new char*[iNumRelations];
	for (i=0; i<iNumRelations; i++) {
		ppcSegmentRelationsList[i] = new char[STRING_MAXSIZE];
	}

	/* For each pair of segments, obtain relation */
	for (i=0, k=0; i<iNumSegments; i++) {
		for (j=i+1; j<iNumSegments; j++, k++) {
			if (ppiSegmentFramesLists[i][0] < ppiSegmentFramesLists[j][0]) {
				/* Case: Segment i "Before" Segment j */
				strcpy(ppcSegmentRelationsList[k], "before");

			} else if (ppiSegmentFramesLists[i][0] == ppiSegmentFramesLists[j][0]) {
				/* Case: Segment i "Start" Segment j */
				strcpy(ppcSegmentRelationsList[k], "starts");
			} else {
				/* Case: Segment i "After" Segment j */
				strcpy(ppcSegmentRelationsList[k], "after");
			}
		}
	}

	/* Print relations */
	for (i=0, k=0; i<iNumSegments; i++) {
		for (j=i+1; j<iNumSegments; j++, k++) {
			fprintf(stderr, "Segment %d  \"%s\"  Segment %d\n", i+1, ppcSegmentRelationsList[k], j+1);
		}
	}


	/* Values passed by reference */
	(*pppcSegRelsList) = ppcSegmentRelationsList;
	(*piNumRels) = iNumRelations;


	return;
}

//----------------------------------------------------------------------------
void SegmentRelGraphExtractionTool::SegmentRelGraphEncoding(
		GenericDSInterfaceABC *parentDescriptionInterface, int iNumSegments, 
		char **ppcSegmentRelationsList, int iNumRelations) 
{
	int j, k, i;
	char str[STRING_MAXSIZE];

	/* ------------------------------------------------------------------------
	   Create the Segment Graph DS
	   ------------------------------------------------------------------------ */

	GenericDS SegmentRelGraphDS = parentDescriptionInterface->CreateChild("Graph");
	GenericDSInterfaceABC *pSegmentRelGraphDSInterface = SegmentRelGraphDS.GetInterface();

	pSegmentRelGraphDSInterface->SetAttribute("id", "SRG0");
	pSegmentRelGraphDSInterface->SetAttribute("type", "Temporal");

	// Namespaces
	pSegmentRelGraphDSInterface->SetAttribute("xmlns", "http://www.mpeg7.org/2001/MPEG-7_Schema");
	pSegmentRelGraphDSInterface->SetAttribute("xmlns:xsi", 
		"http://www.w3.org/2000/10/XMLSchema-instance");

	for (i=0, k=0; i<iNumSegments; i++) {
		for (j=i+1; j<iNumSegments; j++, k++) {

			/* Relation */
			GenericDS RelationDS = pSegmentRelGraphDSInterface->CreateChild("Relation");
			GenericDSInterfaceABC *pRelationDSInterface = RelationDS.GetInterface();

			/* Relation xsi:type */
			pRelationDSInterface->SetAttribute("xsi:type", "BinaryTemporalSegmentRelationType");

			/* Relation name */
			pRelationDSInterface->SetAttribute("name", ppcSegmentRelationsList[k]);

			/* Source of relation */
			sprintf(str, "#SEGMENT%d", i+1);
			pRelationDSInterface->SetAttribute("source", str);

			/* Target of relation */
			sprintf(str, "#SEGMENT%d", j+1);
			pRelationDSInterface->SetAttribute("target", str);
		}
	}


	return;
}

