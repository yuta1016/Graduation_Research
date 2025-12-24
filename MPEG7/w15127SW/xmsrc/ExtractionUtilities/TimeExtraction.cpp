//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Patricia Chiang 	Siemens AG	12/05/2000
// Joerg Heuer          Siemens AG      30/06/2000
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
// Applicable File Name:  TimeExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */


//=============================================================================

using namespace XM;


const UUID TimeExtractionInterface::myID = UUID("");
const char *TimeExtractionInterface::myName = "TimeDSExtractionInterface";

const UUID TimeExtractionTool::myID = UUID("");
const char *TimeExtractionTool::myName = "TimeExtraction";
const int TimeExtractionTool::STRING_MAXSIZE= 4096;		// maximum size of the strings

const UUID TimeExtractionInterfaceABC::myID = UUID();


//=============================================================================
TimeExtractionInterfaceABC::TimeExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
TimeExtractionInterface::TimeExtractionInterface(TimeExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeExtractionInterface::~TimeExtractionInterface()
{
}

//----------------------------------------------------------------------------
void TimeExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int TimeExtractionInterface::SetSourceMedia(MomVop *mvFS, FILE *fpTree, FILE *fpTime, char *pcMediaName)
{
  return m_ExtractionTool->SetSourceMedia(mvFS, fpTree, fpTime, pcMediaName);
}

//----------------------------------------------------------------------------
int TimeExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long TimeExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long TimeExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& TimeExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
TimeExtractionTool::TimeExtractionTool():
m_Interface(this),
m_DescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeExtractionTool::TimeExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(0)
{

  SetDescriptionInterface(theParent);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeExtractionTool::~TimeExtractionTool()
{

}

//----------------------------------------------------------------------------
bool TimeExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool TimeExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int TimeExtractionTool::SetSourceMedia(MomVop *mvFS, FILE *fpTree, FILE *fpTime, char *pcMediaName) // sth use MultiMedia class
{
  
	m_mvFS			= mvFS;
	m_fpTree		= fpTree;
	m_fpTime		= fpTime;
	m_pcMediaName		= pcMediaName;
	return(0);
}

//----------------------------------------------------------------------------
int TimeExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeExtractionTool::StartExtracting(void)
{

	// Do not need this 

	return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeExtractionTool::InitExtracting(void)
{
	int *piFrameOccurenceList = NULL;		// Vector List of no. of frame occurence
	char **ppcGOPTimeCodeList = NULL;
	int *piGOPNumFramesList = NULL;
	int nnFract, iFrameRate_d;
	
	fprintf( stderr, "Extraction of Time \n");

	// ------------------------------------------------------------------------
	// Read Time Info from Time file
	// ------------------------------------------------------------------------
	int iGOPHeaders = ReadTimeInfo( m_fpTime, &piFrameOccurenceList, &ppcGOPTimeCodeList, &piGOPNumFramesList, 
		&nnFract, &iFrameRate_d);
	//Read Time Mode of Description from tree file
	ReadTimeModesFromTreeFile(m_fpTree);
	// Start parsing the tree file.
	m_bRootNode=1; m_bTimeUnit=0;
	RecursiveParse(1, m_DescriptionInterface , piFrameOccurenceList, ppcGOPTimeCodeList, nnFract, iFrameRate_d);

	// Release Memory
	if(piFrameOccurenceList != NULL) delete [] piFrameOccurenceList;
	if(piGOPNumFramesList != NULL) delete [] piGOPNumFramesList;
	if(ppcGOPTimeCodeList != NULL){
		for(int i=0; i<iGOPHeaders; ++i){
			delete [] ppcGOPTimeCodeList[i];
		}
		delete [] ppcGOPTimeCodeList;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
const UUID& TimeExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TimeExtractionInterfaceABC *TimeExtractionTool::GetInterface(void)
{ return &m_Interface; }


//----------------------------------------------------------------------------
int TimeExtractionTool::ReadTimeInfo( FILE *fpTime, int **ppiFrameOccurenceList, 
	char ***pppcGOPTimeCodeList, int **ppiGOPNumFrameList, int *pnnFract, int *piFrameRate_d)
{
	char identifier1[20], identifier2[20], identifier3[20];
	int iNumFrames, iGOPHeaders=0, iNumGOPFrames;
	int i,j, iFrameNum=0;
	char **ppcGOPTimeCodeList;
	int *piFrameOccurenceList, *piGOPNumFrameList;
	
	
	// Go to the beginning of the file
	fseek(fpTime, 0, SEEK_SET);
	
	while(fscanf(fpTime, "%s %d %d %s %d %s %d", identifier1, pnnFract, piFrameRate_d, 
		identifier2, &iGOPHeaders, identifier3, &iNumFrames)==7){

		if(strcmp(identifier1, "frame_rate")==0 && strcmp(identifier2, "nGOPHeaders")==0 && 
			strcmp(identifier3, "nFrames")==0){
			ppcGOPTimeCodeList = new char*[iGOPHeaders];
			piGOPNumFrameList = new int[iGOPHeaders];
			piFrameOccurenceList = new int[iNumFrames];
			i=0;
			while(fscanf(fpTime, "%s", identifier1) ==1 && i<iGOPHeaders){
				if(strcmp(identifier1, "GOP")==0){
					ppcGOPTimeCodeList[i] = new char[20];
					fscanf(fpTime, "%s %s %s %d", identifier2, ppcGOPTimeCodeList[i], identifier3, &iNumGOPFrames);
					for(j=0; j<iNumGOPFrames; ++j){
						fscanf(fpTime, "%d", &iFrameNum);
						fscanf(fpTime, "%d", &piFrameOccurenceList[iFrameNum]);
					}
				}				
				++i;
			}
		}
	}
	
	// Return Data
	(*pppcGOPTimeCodeList) = ppcGOPTimeCodeList;
	(*ppiFrameOccurenceList) = piFrameOccurenceList;
	(*ppiGOPNumFrameList) = piGOPNumFrameList;
	
	return iGOPHeaders;		
}

//----------------------------------------------------------------------------
int TimeExtractionTool::ReadTimeModesFromTreeFile( FILE *fpTree){

        // Go to the beggining of the file
        fseek( fpTree, 0, SEEK_SET); 

	if(fscanf(fpTree,"%d %d", &m_iChoiceStart, &m_iChoiceDuration)==2)
	  return 0;
	else{
	  fprintf(stderr,"ReadTimeModesFromTreeFile: Could not read Time Modes correctly");
	  return -1;
	}
}

//----------------------------------------------------------------------------
int TimeExtractionTool::ReadSegmentInfoTree( FILE *fpTree, int iSegmentID, char *pcSegmentType, 
	int*piSegmentNumChildren, int **ppiSegmentChildIDList, char *pcDecompType, char *pcOverlap, 
	char *pcGap, char *pcSegmentAnno)
{
	
	int iID;
	int iChild;
	
	// Go to the beggining of the file
	fseek( fpTree, 0, SEEK_SET); 

	// skip line of time modes 
	int c = getc(fpTree);
	while(c!='\n' && c!= EOF) {
	  c = getc(fpTree);
	}

	// Scan line by line until the SegmentID is found
	while (fscanf( fpTree, "%d %s %d", &iID, pcSegmentType, piSegmentNumChildren)==3) {

		if (iID==iSegmentID) {
			
			// Read children
			if ((*piSegmentNumChildren)==0) {
				(*ppiSegmentChildIDList) = NULL;
			} else {
				(*ppiSegmentChildIDList) = new int[(*piSegmentNumChildren)];
				for (iChild=0; iChild<(*piSegmentNumChildren); iChild++) {
					fscanf( fpTree, "%d", &(*ppiSegmentChildIDList)[iChild]);
				}

				// Read Decomposition Type
				fscanf( fpTree, " %s %s %s", pcDecompType, pcOverlap, pcGap);
			}
			
			// Read Annotation (rest of the line)
			fgets( pcSegmentAnno, STRING_MAXSIZE, fpTree);
			if (strlen(pcSegmentAnno)>0) {
				pcSegmentAnno[strlen(pcSegmentAnno)-1] = '\0';	// remove last newline
			}

			return(0);
			
		} else {
		
			// Move to the begining of the next line or the end of file
			c = getc(fpTree);
			while(c!='\n' && c!= EOF) {
				c = getc(fpTree);
			}
		}

	}

	// Couldn't read anymore, finish tree file
	return(-1);
}


//----------------------------------------------------------------------------
void TimeExtractionTool::ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID, 
	int *piSegmentNumFrames, int **ppiSegmentFramesList)
{

	int i,k;
	int iNumFrames;					// Number of frames 
	unsigned char *pucMediaPtr;		// Pointer to Segment row in the media FS file
	int *piFramesList;				// List of frames
	
	// Go to line iSegmentID of the media and read it
	if ( (iSegmentID-1)>=int(mediaFS->y_chan->y)) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Couldn't find the SegmentID in the FS file.");
		exit(-1);
	}
	
	// Count Number of Frames
	pucMediaPtr = (mediaFS->y_chan->data->u) + ((iSegmentID-1)*mediaFS->y_chan->x); 
	for (iNumFrames=0, i=0; i<int(mediaFS->y_chan->x); i++) {
		if (pucMediaPtr[i]>0) { iNumFrames++;}
	}
										
	// Check for no frames
	if (iNumFrames==0) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Found a Segment with No frames!!!");
		exit(-1);
	}

	// Create frame vector list
	piFramesList = new int[iNumFrames];	// frame vector list
					
	// 'i' is the x coordinate (correspond to frame number)
	// 'k' is the pointer inside piFrameList
	for (i=0,k=0; i<int(mediaFS->y_chan->x); i++) {
		if (pucMediaPtr[i]>0) { 
			piFramesList[k] = i;
			k++;
		}
	}
					
	// Return data
	(*piSegmentNumFrames)	= iNumFrames;
	(*ppiSegmentFramesList) = piFramesList;

}

//----------------------------------------------------------------------------
void TimeExtractionTool::RecursiveParse(int iSegmentID, GenericDSInterfaceABC *pDomParentInterface, int *piFrameOccurenceList, 
	char **ppcGOPTimeCodeList, int nnFract, int iFrameRate_d)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	char	pcSegmentType[STRING_MAXSIZE];	// Type of segment ("VideoSegment", "MovingRegion" or "StillRegion")
	int	iSegmentNumChildren;		// Number of Children
	int	*piSegmentChildIDList = NULL;	// Vector list with Children's ID

	char	pcDecompType[STRING_MAXSIZE];
	char	pcOverlap[STRING_MAXSIZE];
	char	pcGap[STRING_MAXSIZE];
	char	pcSegmentAnno[STRING_MAXSIZE];	// Segment AnnotationDS
	
	int i,j;	// guess what this is for... :-)

	int iSegmentNumFrames = 0;
	int *piSegmentFramesList = NULL;

	// ------------------------------------------------------------------------
	// Read Segment Info from the tree file
	// ------------------------------------------------------------------------
	if (ReadSegmentInfoTree( m_fpTree, iSegmentID, pcSegmentType, &iSegmentNumChildren, 
			&piSegmentChildIDList, 	pcDecompType, pcOverlap, pcGap, pcSegmentAnno)!=0) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Couldn't find SegmendID = %d in the tree file.", iSegmentID);
		exit(-1);
	}

	// ------------------------------------------------------------------------
	// Read Segment Info from FS file
	// ------------------------------------------------------------------------
	ReadSegmentInfoFS( m_mvFS, iSegmentID, &iSegmentNumFrames, 
		&piSegmentFramesList);
	
	
	// ------------------------------------------------------------------------
	// Create a new child of the parent node (for this Segment)
	// ------------------------------------------------------------------------
	char sID[STRING_MAXSIZE];
	if (strncmp("VideoSegment", pcSegmentType, 12)==0) {
		sprintf(sID, "VS%d", iSegmentID);
	} 
	else if (strncmp("MovingRegion", pcSegmentType,12)==0) {
		sprintf(sID, "MR%d", iSegmentID);
	} 
	else if (strncmp("StillRegion", pcSegmentType,11)==0) {
		sprintf(sID, "SR%d", iSegmentID);
	}

	GenericDS domSegmentElement = pDomParentInterface->CreateChild(pcSegmentType);
	GenericDSInterfaceABC *pDomSegmentInterface = domSegmentElement.GetInterface();
	pDomSegmentInterface->SetAttribute("id", sID);
		
	GenericDS domTimeElement = MediaTimeDOMExtraction(pDomSegmentInterface, piSegmentFramesList[0], 
		piSegmentFramesList[iSegmentNumFrames-1] +1 - piSegmentFramesList[0],
		ppcGOPTimeCodeList, nnFract, iFrameRate_d) ;
	 
	// ------------------------------------------------------------------------
	// Print the Result
	// ------------------------------------------------------------------------
	fprintf( stderr, "\nRead Segment %d, type \"%s\", with %d children.\n\n", 
		iSegmentID, pcSegmentType, iSegmentNumChildren);
	
	// ------------------------------------------------------------------------
	// Do the same for each children
	// ------------------------------------------------------------------------
	if (iSegmentNumChildren>0) {

		// Create a SegmentDS Type for Children
		
		GenericDS domSegmentDecompositionElement = pDomSegmentInterface->CreateChild("SegmentDecomposition");
		GenericDSInterfaceABC *pDomSegmentDecompositionInterface = domSegmentDecompositionElement.GetInterface();

		// Set Decomposition Type
		pDomSegmentDecompositionInterface->SetAttribute("DecompositionType", pcDecompType);
		pDomSegmentDecompositionInterface->SetAttribute("Overlap", pcOverlap);
		pDomSegmentDecompositionInterface->SetAttribute("Gap", pcGap);

		// Do the recursivity
		for (i=0; i<iSegmentNumChildren; i++) {
			RecursiveParse( piSegmentChildIDList[i], pDomSegmentDecompositionInterface, 
				piFrameOccurenceList, ppcGOPTimeCodeList, nnFract, iFrameRate_d);
		}

	}
	else if(iSegmentNumChildren==0){

		// count no of subsegments
		int iNumSubSegments = 1;
		for(i=0; i<iSegmentNumFrames-1; ++i){
			if(piSegmentFramesList[i+1] != (piSegmentFramesList[i]+1))
				++ iNumSubSegments;
		}
		if(iNumSubSegments>1){
			// Create a SegmentDS Type for SubSegments
			GenericDS domSegmentDecompositionElement = pDomSegmentInterface->CreateChild("SegmentDecomposition");
			GenericDSInterfaceABC *pDomSegmentDecompositionInterface = domSegmentDecompositionElement.GetInterface();

			// Set Decomposition Type
			pDomSegmentDecompositionInterface->SetAttribute("DecompositionType", "temporal");
			pDomSegmentDecompositionInterface->SetAttribute("Overlap", "false");
			pDomSegmentDecompositionInterface->SetAttribute("Gap", "true");
			
			int *piStartFrameNum, *piSegmentLength;
			piStartFrameNum = new int[iNumSubSegments];
			piSegmentLength = new int[iNumSubSegments];
			piStartFrameNum[0] = piSegmentFramesList[0];
			for(i=0, j=0; i<(iSegmentNumFrames-1); ++i){
				if(piSegmentFramesList[i+1] != (piSegmentFramesList[i]+1)){ 
					++j;
					piSegmentLength[j-1] = piSegmentFramesList[i] - piStartFrameNum[j-1] + 1;
					piStartFrameNum[j] = piSegmentFramesList[i+1];
				}
			}
			piSegmentLength[j] = piSegmentFramesList[iSegmentNumFrames-1] - piStartFrameNum[j] + 1;
			for(i=0; i<iNumSubSegments; ++i){
				GenericDS subSegmentElement = pDomSegmentDecompositionInterface->CreateChild(pcSegmentType);
				GenericDSInterfaceABC *pSubSegmentInterface = subSegmentElement.GetInterface();
				GenericDS subTimeElement = MediaTimeDOMExtraction(pSubSegmentInterface, piStartFrameNum[i],
					 piSegmentLength[i], ppcGOPTimeCodeList, nnFract, iFrameRate_d) ;
			}
			
			// Release Memory
			if(piStartFrameNum != NULL) delete [] piStartFrameNum;
			if(piSegmentLength != NULL) delete [] piSegmentLength;
		}
	}


	// ------------------------------------------------------------------------
	// Release memory
	// ------------------------------------------------------------------------
	if (piSegmentChildIDList!=NULL) {
		delete [] piSegmentChildIDList;
	}
	if (piSegmentFramesList!=NULL) {
		delete [] piSegmentFramesList;
	}

}

//----------------------------------------------------------------------------
GenericDS TimeExtractionTool::MediaTimeDOMExtraction(GenericDSInterfaceABC *pSegmentInterface, int iStartFrame,  int iFrameDuration,
	char **ppcGOPTimeCodeList, int nnFract, int iFrameRate_d) 
{
	// ------------------------------------------------------------------------
	// Variables
	// ------------------------------------------------------------------------

  //static int choiceStart = RELINCRTIMEPOINT;		//0=RelTime, 1=RelIncrTime, 2=TimePoint
  //static int choiceLength = DURATION;	//0=Duration, 1=IncrDuration

	GenericDS domTimeElement, startNode, lengthNode;
	GenericDSInterfaceABC *pDomTimeInterface;
	
	domTimeElement = pSegmentInterface->CreateChild("MediaTime");
	pDomTimeInterface = domTimeElement.GetInterface();
	
	/*if(m_bRootNode){
		m_iRefTime = iStartFrame;
		printf("Please enter choice of start time representation\n(0=RelTime, 1=RelIncrTime, 2=TimePoint): ");
		scanf("%d",&choiceStart);
		printf("Please enter choice of segment length representation\n(0=Duration, 1=IncrDuration): ");
		scanf("%d",&choiceLength);
		}*/

	switch(m_iChoiceStart){
		case RELTIMEPOINT:
			startNode = DOMEncodeMediaRelTimePoint(pDomTimeInterface, iStartFrame, nnFract, iFrameRate_d);
			break;
		case RELINCRTIMEPOINT:
			startNode = DOMEncodeMediaRelIncrTimePoint(pDomTimeInterface, iStartFrame, nnFract, iFrameRate_d);
			break;
		case TIMEPOINT:
			startNode = DOMEncodeMediaTimePoint(pDomTimeInterface, iStartFrame, ppcGOPTimeCodeList, nnFract, iFrameRate_d);
			break;
		default:
			startNode = DOMEncodeMediaRelIncrTimePoint(pDomTimeInterface, iStartFrame, nnFract, iFrameRate_d);
			break;
	}
		
	switch(m_iChoiceDuration){
		case DURATION:
			lengthNode = DOMEncodeMediaDuration(pDomTimeInterface, iFrameDuration, nnFract, iFrameRate_d);
			break;
		case INCRDURATION:
			lengthNode = DOMEncodeMediaIncrDuration(pDomTimeInterface, iFrameDuration, nnFract, iFrameRate_d);
			break;
		default:
			lengthNode = DOMEncodeMediaIncrDuration(pDomTimeInterface, iFrameDuration, nnFract, iFrameRate_d);
	}

	if(m_bRootNode){
		m_bRootNode = 0;
	}
		
	return domTimeElement;
}

//----------------------------------------------------------------------------
GenericDS TimeExtractionTool::DOMEncodeMediaTimePoint(GenericDSInterfaceABC *pTimeInterface, int iStartFrame, char **ppcGOPTimeCodeList, int nnFract, int iFrameRate_d)
{
	//DOM_Element timePointNode;
	GenericDS timePointNode;
	GenericDSInterfaceABC *pTimePointInterface;

	int dateY, dateM, dateD, time_h, time_m, time_s, time_nn;
	int cDateY, cDateM, cDateD, cTime_h, cTime_m, cTime_s;

	char timePoint[256];

	cDateY=cDateM=cDateD=cTime_h=cTime_m=cTime_s=0;
	sscanf(*ppcGOPTimeCodeList, "%d:%d:%d:%d", &time_h, &time_m, &time_s, &time_nn);

	/* to specific year, month and day */
	dateY = 2000;
	dateM = 1;
	dateD = 1;

	time_nn += (iStartFrame*iFrameRate_d)%nnFract;
	if(time_nn>=nnFract){
		 cTime_s = time_nn/nnFract;
		 time_nn = time_nn%nnFract;
	}
	
	time_s += (iStartFrame*iFrameRate_d)/nnFract + cTime_s;
	if(time_s>=60){
		cTime_m = time_s/60;
		time_s = time_s%60;
	}
	
	time_m += (iStartFrame*iFrameRate_d)/(nnFract*60) + cTime_m;
	if(time_m>=60){
		cTime_h = time_m/60;
		time_m = time_m%60;
	}
	
	time_h += (iStartFrame*iFrameRate_d)/(nnFract*60*60);
	if(time_h>=24){
		cDateD = time_h/24;
		time_h = time_h%24;
	}

	dateD += (iStartFrame*iFrameRate_d)/(nnFract*60*60*24) + cDateD;

	timePointNode = pTimeInterface->CreateChild("MediaTimePoint");
	pTimePointInterface = timePointNode.GetInterface();
	
	if(nnFract>1 && m_bRootNode){
	  sprintf(timePoint,"%d-%d-%dT%d:%d:%d:%dF%d",dateY,dateM,dateD,time_h,time_m,time_s,time_nn,nnFract);
	}
	else {
	 sprintf(timePoint,"%d-%d-%dT%d:%d:%d:%d",dateY,dateM,dateD,time_h,time_m,time_s,time_nn); 
	}
	pTimePointInterface->SetValue(timePoint);

	

	return timePointNode;
}

//----------------------------------------------------------------------------
GenericDS TimeExtractionTool::DOMEncodeMediaRelTimePoint(GenericDSInterfaceABC *pTimeInterface, int iStartFrame, int nnFract, int iFrameRate_d)
{
	GenericDS relTimeNode;
	GenericDSInterfaceABC *pRelTimeInterface;

	relTimeNode = pTimeInterface->CreateChild("MediaRelTimePoint");
	pRelTimeInterface = relTimeNode.GetInterface();
		
	if(m_pcMediaName!=NULL && m_bRootNode)
		pRelTimeInterface->SetAttribute("TimeBase", m_pcMediaName);

	int iRelStartFrame;
	if(m_bRootNode)
		iRelStartFrame = iStartFrame;
	else
		iRelStartFrame = iStartFrame - m_iRefTime;
		
	DOMEncodeTimePeriod(pRelTimeInterface, iRelStartFrame, nnFract, iFrameRate_d);
		
	return relTimeNode;
}

//----------------------------------------------------------------------------
GenericDS TimeExtractionTool::DOMEncodeMediaRelIncrTimePoint(GenericDSInterfaceABC *pTimeInterface, int iStartFrame, int nnFract, int iFrameRate_d)
{
	GenericDS relIncrTimeNode;
	GenericDSInterfaceABC *pRelIncrTimeInterface;

	int iRelStartFrame;
	if(m_bRootNode)
		iRelStartFrame = iStartFrame;
	else
		iRelStartFrame = iStartFrame - m_iRefTime;
		
	relIncrTimeNode = pTimeInterface->CreateChild("MediaRelIncrTimePoint");
	pRelIncrTimeInterface = relIncrTimeNode.GetInterface();
	pRelIncrTimeInterface->SetValue(iRelStartFrame);
	
	if(m_pcMediaName!=NULL && m_bRootNode)
		pRelIncrTimeInterface->SetAttribute("TimeBase", m_pcMediaName);

	DOMEncodeTimeUnit(pRelIncrTimeInterface, nnFract, iFrameRate_d);
		
	return relIncrTimeNode;
}

//----------------------------------------------------------------------------
GenericDS TimeExtractionTool::DOMEncodeMediaDuration(GenericDSInterfaceABC *pTimeInterface, int iFrameDuration, int nnFract, int iFrameRate_d)
{
	GenericDS durationNode;
	GenericDSInterfaceABC *pDurationInterface;
	
	durationNode = pTimeInterface->CreateChild("MediaDuration");
	pDurationInterface = durationNode.GetInterface();
	
	DOMEncodeTimePeriod(pDurationInterface, iFrameDuration, nnFract, iFrameRate_d);
		
	return durationNode;	
}

//----------------------------------------------------------------------------
GenericDS TimeExtractionTool::DOMEncodeMediaIncrDuration(GenericDSInterfaceABC *pTimeInterface, int iFrameDuration, int nnFract, int iFrameRate_d)
{
	GenericDS incrDurationNode;
	GenericDSInterfaceABC *pIncrDurationInterface;
	
	incrDurationNode = pTimeInterface->CreateChild("MediaIncrDuration");
	pIncrDurationInterface = incrDurationNode.GetInterface();
	pIncrDurationInterface->SetValue(iFrameDuration);
	
	DOMEncodeTimeUnit(pIncrDurationInterface, nnFract, iFrameRate_d);
	
	return incrDurationNode;
	
}

//----------------------------------------------------------------------------
int TimeExtractionTool::DOMEncodeTimePeriod(GenericDSInterfaceABC *pTimeInterface,  int iTimePeriod, int nnFract, int iFrameRate_d)
{
	int time_D, time_h, time_m, time_s, time_nn;
	char period[256];
	char *p_period;
	
	if(iTimePeriod<0){
	  sprintf(period,"-P");
	  iTimePeriod=-iTimePeriod;
	}
	else
	  sprintf(period,"P");

	time_D = (iTimePeriod*iFrameRate_d)/(nnFract*60*60*24);
	time_h = (iTimePeriod*iFrameRate_d)/(nnFract*60*60);
	time_m = (iTimePeriod*iFrameRate_d)/(nnFract*60);
	time_s = (iTimePeriod*iFrameRate_d)/nnFract;
	time_nn = (iTimePeriod*iFrameRate_d)%nnFract;
		


	if(time_D>0){
	  p_period=strchr(period,'\0');
	  sprintf(p_period,"%dD",time_D);
	}
	if(time_h>0){
	  p_period=strchr(period,'\0');
	  sprintf(p_period,"%dH",time_h);
	}
	if(time_m>0){
	  p_period=strchr(period,'\0');
	  sprintf(p_period,"%dM",time_m);
	}
	if(time_s>0){
	  p_period=strchr(period,'\0');
	  sprintf(p_period,"%dS",time_s);
	}
	if(time_nn>0 || iTimePeriod==0){
	  p_period=strchr(period,'\0');
	  sprintf(p_period,"%dN",time_nn);
	}

	if(nnFract>1 && m_bRootNode){
	  p_period=strchr(period,'\0');
	  sprintf(p_period,"%dF",nnFract);
	}

	
	pTimeInterface->SetValue(period);
	return 0;
}

//----------------------------------------------------------------------------
int TimeExtractionTool::DOMEncodeTimeUnit(GenericDSInterfaceABC *pTimeInterface, int nnFract, int iTimePeriod)
{
	int time_D, time_h, time_m, time_s, time_nn;
	char period[256];
	char *p_period; 

	if(iTimePeriod<0){
	  sprintf(period,"-P");
	  iTimePeriod=-iTimePeriod;
	}
	else
	  sprintf(period,"P");

	time_D = iTimePeriod/(nnFract*60*60*24);
	time_h = iTimePeriod/(nnFract*60*60);
	time_m = iTimePeriod/(nnFract*60);
	time_s = iTimePeriod/nnFract;
	time_nn = iTimePeriod%nnFract;

	if(!m_bTimeUnit || m_bRootNode){

	  if(time_D>0){
	    p_period=strchr(period,'\0');
	    sprintf(p_period,"%dD",time_D);
	  }
	  if(time_h>0){
	    p_period=strchr(period,'\0');
	    sprintf(p_period,"%dH",time_h);
	  }
	  if(time_m>0){
	    p_period=strchr(period,'\0');
	    sprintf(p_period,"%dM",time_m);
	  }
	  if(time_s>0){
	    p_period=strchr(period,'\0');
	    sprintf(p_period,"%dS",time_s);
	  }
	  if(time_nn>0){
	    p_period=strchr(period,'\0');
	    sprintf(p_period,"%dN",time_nn);
	  }
	  
	  if(nnFract>1 && m_bRootNode){
	    p_period=strchr(period,'\0');
	    sprintf(p_period,"%dF",nnFract);
	  }

	       		

	  if(m_bRootNode) m_bTimeUnit=1;

	  pTimeInterface->SetAttribute("timeunit",period);
	}
	return 0;
}
