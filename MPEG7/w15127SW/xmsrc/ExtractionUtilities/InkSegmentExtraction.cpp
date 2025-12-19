//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Zon-Yin Shae and Xiping Wang - T. J. Watson Research Center
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
// Applicable File Name:  InkSegmentExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

static void ExtractMetaStrokeData(char *, int *, int *);

//=============================================================================

using namespace XM;


const UUID InkSegmentExtractionInterface::myID = UUID("");
const char *InkSegmentExtractionInterface::myName = "InkSegmentDSExtractionInterface";

const UUID InkSegmentExtractionTool::myID = UUID("");
const char *InkSegmentExtractionTool::myName = "InkSegmentExtraction";
const int	InkSegmentExtractionTool::STRING_MAXSIZE= 4096;	// maximum size of the strings

const UUID InkSegmentExtractionInterfaceABC::myID = UUID();


//=============================================================================
InkSegmentExtractionInterfaceABC::InkSegmentExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
InkSegmentExtractionInterface::InkSegmentExtractionInterface(InkSegmentExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
InkSegmentExtractionInterface::~InkSegmentExtractionInterface()
{
}

//----------------------------------------------------------------------------
void InkSegmentExtractionInterface::destroy()
{
  delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int InkSegmentExtractionInterface::SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, 
	char *pcMasksName)
{
  return m_ExtractionTool->SetSourceMedia(mvFS, fp, pcFramesName, pcMasksName);
}

//----------------------------------------------------------------------------
int InkSegmentExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long InkSegmentExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long InkSegmentExtractionInterface::StartExtracting(char *x, char *y)
{
  return m_ExtractionTool->StartExtracting(x, y);
}

//----------------------------------------------------------------------------
const UUID& InkSegmentExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *InkSegmentExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
InkSegmentExtractionTool::InkSegmentExtractionTool():
m_Interface(this),
m_DescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
InkSegmentExtractionTool::
InkSegmentExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(0)
{
        SetDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
InkSegmentExtractionTool::~InkSegmentExtractionTool()
{

}

//----------------------------------------------------------------------------
bool InkSegmentExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool InkSegmentExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int InkSegmentExtractionTool::SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, 
	char *pcMasksName)
{
  
	m_mvFS			= mvFS;
	m_fp			= fp;
	m_pcFramesName	= pcFramesName;
	m_pcMasksName	= pcMasksName;

	return(0);
}

//----------------------------------------------------------------------------
int InkSegmentExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long InkSegmentExtractionTool::InitExtracting(void)
{

	/* Do not need this */

	return 0;
}

//----------------------------------------------------------------------------
unsigned long InkSegmentExtractionTool::StartExtracting(char *stroke, char *mp7file)
{
  //sth use the XML parswer to create description
  //sth fileIO not allowed here!!
	char buffer[20480];
	int endComp, endUnit;

	memset(buffer, '\0', sizeof(buffer));

	strcat(buffer, "<?xml version = \"1.0\" encoding = \"ISO-8859-1\" ?>\n");
	strcat(buffer, "<InkSegment id = \"Signature_1 type = \"meta\">\n");
	
	strcat(buffer, "\t<MediaLocator>\n");
	strcat(buffer, "\t\t<MediaURL> ");
	strcat(buffer, stroke);
	strcat(buffer, " </MediaURL>\n");
	strcat(buffer, "\t</MediaLocator>\n");

	
////	
	strcat(buffer, "\t<InkMediaInfo>\n");
	strcat(buffer, "\t\t<InputDevice>\n");
	strcat(buffer, "\t\t\t<Name> PenDevice </Name>\n");
	strcat(buffer, "\t\t\t<PointsPerSecond> ");
	strcat(buffer, "133");
	strcat(buffer, " </PointsPerSecond>\n");
	strcat(buffer, "\t\t<Handness> Left </Handness>\n");
	strcat(buffer, "\t\t<Style> mixed </Style>\n");
	strcat(buffer, "\t</InkMediaInfo>\n");

	strcat(buffer, "\t<OrderedGroupDataMask>\n");
	strcat(buffer, "\t\t<SubInterval>\n");
	strcat(buffer, "\t\t\t<SetName> ");
	strcat(buffer, stroke);
	strcat(buffer, "#Signature_1");
	strcat(buffer, " </SetName>\n");
	strcat(buffer, "\t\t\t<StartComponent> 0 </StartComponent>\n");
	strcat(buffer, "\t\t\t<StartUnit> 0 </StartUnit>\n");

	ExtractMetaStrokeData(stroke, &endComp, &endUnit);

	char buf[256];

	strcat(buffer, "\t\t\t<EndComponent> ");
	memset(buf, sizeof(buf), '\0');
	sprintf(buf,"%d",endComp);
	strcat(buffer, buf);
	strcat(buffer, " </EndComponent>\n");
	strcat(buffer, "\t\t\t<EndUnit> ");
	memset(buf, sizeof(buf), '\0');
	sprintf(buf,"%d",endComp);
	strcat(buffer, buf);
	strcat(buffer, " </EndUnit>\n");
	strcat(buffer, "\t\t</SubInterval>\n");
	strcat(buffer, "\t</OrderedGroupDataMask>\n");
	strcat(buffer, "</InkSegment>");


	FILE *fp = NULL;
	if ((fp = fopen(mp7file, "w")) == NULL) {
		printf("failed to create file : %s", mp7file);
		exit(1);
	}

	fwrite(buffer, strlen(buffer), 1, fp);

	fclose(fp);

	return 0;
}

//----------------------------------------------------------------------------
const UUID& InkSegmentExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *InkSegmentExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
InkSegmentExtractionInterfaceABC *InkSegmentExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int InkSegmentExtractionTool::ReadSegmentInfoTree( FILE *fp, int iSegmentID, char *pcSegmentType, 
	int*piSegmentNumChildren, int **ppiSegmentChildIDList, char *pcDecompType, char *pcOverlap, 
	char *pcGap, char *pcSegmentAnno)
{

	return(-1);

}

//----------------------------------------------------------------------------
void InkSegmentExtractionTool::ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID, 
	char *pcSegmentType, int *piSegmentNumFrames, int **ppiSegmentFramesList, 
	int **ppiSegmentMasksList)
{

}

//----------------------------------------------------------------------------
void InkSegmentExtractionTool::ExtractInkSegment(GenericDSInterfaceABC *parentDescriptionInterface)
{

}

//----------------------------------------------------------------------------
void InkSegmentExtractionTool::SegmentRelationshipsExtraction(int iNumSegments,
					int **ppiSegmentFramesLists, char ***pppcSegRelsList, int *piNumRels) 
{

	return;
}

//----------------------------------------------------------------------------
void InkSegmentExtractionTool::InkSegmentEncoding(
		GenericDSInterfaceABC *parentDescriptionInterface, int iNumSegments, 
		char **ppcSegmentRelationshipsList, int iNumRelationships) 
{

	return;
}

//----------------------------------------------------------------------------
static void ExtractMetaStrokeData(char *stroke, int *endComp, int *endUnit)
{
	FILE *fp_read = NULL;
	int strokesleft = 0, pointsleftthisstroke = 0;
	char tempstring[256];
	int i, x, y;

	*endComp = *endUnit = 0;

	if ((fp_read = fopen(stroke, "r")) == NULL) {
		printf("failed to open stroke file : %s\n", stroke);
		exit(1);
	}

	if (NULL == fgets(tempstring, 256, fp_read)) { //get stroke-count line
		printf("failed to read stroke file : %s\n", stroke);
		fclose(fp_read);
		exit(1);
	}

	strtok(tempstring, " ");
	*endComp = strokesleft = (int)atoi(strtok(NULL, " ")); //stroke count = 2nd field

	if (NULL == fgets(tempstring, 256, fp_read)) { //ignore text line
		printf("failed to read stroke file : %s\n", stroke);
		fclose(fp_read);
		exit(1);
	}

	int nStkIndex = 0, nDotIndex = 0;

	//get lines containing stroke headers and points
	while (NULL != (fgets(tempstring, sizeof(tempstring), fp_read))) {

		if (strokesleft == 0 && pointsleftthisstroke == 0) { //goto error;
			printf("failed to read stroke file : %s\n", stroke);
			fclose(fp_read);
			exit(1);
		}

		if (pointsleftthisstroke == 0) { //read a stroke header line
			strtok(tempstring, " "); //point count in stroke = 2nd field */
			*endUnit = pointsleftthisstroke = (int)atoi(strtok(NULL, " "));

			nDotIndex = 0;
			
			strokesleft--;
		} else { //read a line with co-ordinates (max 8 coordinates)
//			char szBuf[256];

			for(i = pointsleftthisstroke; i > pointsleftthisstroke - 8 && i > 0; i--) {
				if (i == pointsleftthisstroke) {
					x = atoi(strtok(tempstring, " "));
				} else {
					x = atoi(strtok(NULL, " "));
				}
				y = atoi(strtok(NULL, " "));

				nDotIndex++;
			}

			pointsleftthisstroke = i;

			if (pointsleftthisstroke == 0) {
				nStkIndex++;
			}

		}
	}

	*endComp -= 1;
	*endUnit -= 1;
}
