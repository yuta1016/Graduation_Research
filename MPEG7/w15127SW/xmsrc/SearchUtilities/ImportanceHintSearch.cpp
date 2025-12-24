///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation ImportanceHint.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation ImportanceHint tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  ImportanceHintSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

static const int VERB = 0;//sth define
static const char TRANSCODENAME[] = "transcode.pgm";//sth the name for the transcoded Mosaic?? Summary?? ...??

inline static int INTMIN(int a, int b) { return (b>a) ? a : b; }
inline static int INTMAX(int a, int b) { return (b<a) ? a : b; }

using namespace std;

/*** FOLLOWING CODE IS SPECIFIC TO TRANSCODING OF GRAYSCALE PGM IMAGES  ***/
/*** THE PURPOSE IS TO DEMONSTRATE TRANSCODING USING MP7 IMPORTANCE HINTS  ***/

// THIS IS A TRANSCODING APPLICATION.  MUST READ IMAGE!!!
// Currently handles grayscale input images only.

//sth images must not be read in this file. This is a task of the application
//sth Furthermore. use MediIO for reading and writing of
//sth pgm,ppm,pbm,jpg,tif,gif,ps,xbm,yuv, .. and many many more
class GrayImage {

 public:
  int d_width;
  int d_height;
  unsigned char *d_data_p;

 public:
	 GrayImage(const char* filename) {
		 readpgm(filename);
	 }

	 GrayImage(int w, int h) : d_width(w), d_height(h), d_data_p(0) {
		 d_data_p = new unsigned char[d_width*d_height];
	 }
	 
	 GrayImage(const GrayImage& A) : 
		d_width(A.d_width), d_height(A.d_height), d_data_p(0) {
		 d_data_p = new unsigned char[d_width*d_height];
		 memcpy(d_data_p, A.d_data_p, d_width*d_height);
	 }

	 void readpgm(const char* filename) {
		 char buf[256];
		#ifdef WIN32
		  ifstream inf(filename, ios::binary);
		#else
		  ifstream inf(filename);
		#endif
		 inf.getline(buf, 255);
		 if (buf[0]=='P' && (buf[1]=='5')) {
			 while (inf.peek()=='#') inf.getline(buf, 255);
			 inf >> d_width >> d_height;
			 inf.getline(buf, 255);
			 d_data_p = new unsigned char[d_width*d_height];
			 inf.read((char*)(void*)d_data_p, d_width*d_height);
		 }
		 inf.close();
	 }

	 void writepgm(const char* filename) {
		#ifdef WIN32
		  ofstream outf(filename, ios::binary);
		#else
		  ofstream outf(filename);
		#endif
		outf << "P5" << endl << d_width << " " << d_height << endl << 255 << endl;
		outf.write((char*)d_data_p, d_width*d_height);
		outf.close();
	 }
};

GrayImage region(const GrayImage& A, int x, int y, int w, int h) {
  int neww = INTMIN(w, (A.d_width-x));
  int newh = INTMIN(h, (A.d_height-y));
  GrayImage temp(neww, newh);
  register const unsigned char *g = A.d_data_p + y*A.d_width + x;
  register unsigned char *f = temp.d_data_p;
  {
    register int j;
    for (j = 0; j < newh; j++, g+=(A.d_width-neww)) {
      register int k;
      for (k = 0; k < neww; k++) *f++ = *g++;
    }
  }
  return temp;
}

/*** END OF SPECIFIC CODE FOR TRANSCODING OF GRAYSCALE PGM IMAGES  ***/

//=============================================================================

using namespace XM;

const UUID ImportanceHintSearchInterface::myID = UUID("");
const char *ImportanceHintSearchInterface::myName = "ImportanceHint Transcoding Interface";

const UUID ImportanceHintSearchTool::myID = UUID("");
const char *ImportanceHintSearchTool::myName = "ImportanceHint Transcoding Tool";

const UUID ImportanceHintSearchInterfaceABC::myID = UUID();


//=============================================================================
ImportanceHintSearchInterfaceABC::ImportanceHintSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
ImportanceHintSearchInterface::ImportanceHintSearchInterface(ImportanceHintSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ImportanceHintSearchInterface::~ImportanceHintSearchInterface()
{
}

//----------------------------------------------------------------------------
void ImportanceHintSearchInterface::destroy()
{  delete m_SearchTool; }


//----------------------------------------------------------------------------
int ImportanceHintSearchInterface::SetDescriptionInterface(GenericDSInterfaceABC
														   *aGenericDSInterface)
{
  return m_SearchTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int ImportanceHintSearchInterface::SetImportanceHintParms(const char* mediafile, const char* hintfile) 
{
	return m_SearchTool->SetImportanceHintParms(mediafile, hintfile);
}

//----------------------------------------------------------------------------
unsigned long ImportanceHintSearchInterface::StartTranscoding(void)
{
	return m_SearchTool->StartTranscoding();
}

//----------------------------------------------------------------------------
const UUID& ImportanceHintSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ImportanceHintSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
ImportanceHintSearchTool::ImportanceHintSearchTool():
m_MediaFile(0),
m_HintFile(0),
m_Interface(this),
m_DescriptionInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ImportanceHintSearchTool::ImportanceHintSearchTool(GenericDSInterfaceABC
						   *theParent):
m_MediaFile(0),
m_HintFile(0),
m_Interface(this),
m_DescriptionInterface(0)

{
	SetDescriptionInterface(theParent);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ImportanceHintSearchTool::~ImportanceHintSearchTool()
{
	delete [] m_MediaFile;
	delete [] m_HintFile;
}

//----------------------------------------------------------------------------
const UUID& ImportanceHintSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ImportanceHintSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ImportanceHintSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ImportanceHintSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ImportanceHintSearchTool::SetDescriptionInterface(GenericDSInterfaceABC
													  *aGenericDSInterface)
{
  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int ImportanceHintSearchTool::SetImportanceHintParms(const char* mediafile, const char* hintfile) 
{
	if (m_MediaFile) delete [] m_MediaFile;
	if (mediafile) {
		m_MediaFile = new char[strlen(mediafile)+1];
		strcpy(m_MediaFile, mediafile);
	}
	else m_MediaFile = 0;

	if (m_HintFile) delete [] m_HintFile;
	if (hintfile) {
		m_HintFile = new char[strlen(hintfile)+1];
		strcpy(m_HintFile, hintfile);
	}
	else m_HintFile = 0;

return 0;

}

/*
<StillRegion>
	<BoundingBox boxWidth="100" boxHeight="100" lengthUnits="Pixels">
		<CompositionInfo boxCenterH="130" boxCenterV="70" /> 
	</BoundingBox>
	<MediaInformation>
		<MediaProfile>
			<MediaTranscodingHint importance="1.0" spatialResolutionHint="0.5" difficulty="0.5" />
		</MediaProfile>
	</MediaInformation>
</StillRegion>
*/

//----------------------------------------------------------------------------
unsigned long ImportanceHintSearchTool::StartTranscoding(void)
{
	int x, y, w, h, cx, cy;
	double importance, spatialResolutionHint, difficulty, priority;
	if (VERB) cerr << "Transcoding of " << m_MediaFile << " using " << m_HintFile << endl;

	// Read Importance Hint Description

	GenericDS StillRegionDS = m_DescriptionInterface->GetFirstChild();
	GenericDSInterfaceABC *pStillRegionDSInt = StillRegionDS.GetInterface();

	if (pStillRegionDSInt->isNull()) {
		cerr << "Error - NULL Child" << endl;
		return 0;
	}

	GenericDS BoundingBoxDS = pStillRegionDSInt->GetFirstChild();
	GenericDSInterfaceABC *pBoundingBoxDSInt = BoundingBoxDS.GetInterface();

    pBoundingBoxDSInt->GetIntAttribute("boxWidth", w);
	pBoundingBoxDSInt->GetIntAttribute("boxHeight", h);

	GenericDS CompositionInfoDS = pBoundingBoxDSInt->GetFirstChild();
	GenericDSInterfaceABC *pCompositionInfoDSInt = CompositionInfoDS.GetInterface();

    pCompositionInfoDSInt->GetIntAttribute("boxCenterH", cx);
	pCompositionInfoDSInt->GetIntAttribute("boxCenterV", cy);
	x = cx - w/2;
	y = cy - h/2;

	GenericDS MediaInformationDS = pBoundingBoxDSInt->GetNextSibling();
	GenericDSInterfaceABC *pMediaInformationDSInt = MediaInformationDS.GetInterface();

	GenericDS MediaProfileDS = pMediaInformationDSInt->GetFirstChild();
	GenericDSInterfaceABC *pMediaProfileDSInt = MediaProfileDS.GetInterface();

	GenericDS MediaTranscodingHintDS = pMediaProfileDSInt->GetFirstChild();
//	GenericDSInterfaceABC *pMediaTranscodingHintDSInt = MediaTranscodingHintDS.GetInterface();

	pMediaProfileDSInt->GetRealAttribute("importance", importance);
	pMediaProfileDSInt->GetRealAttribute("spatialResolutionHint", spatialResolutionHint);
	pMediaProfileDSInt->GetRealAttribute("difficulty", difficulty);
	pMediaProfileDSInt->GetRealAttribute("priority", priority);

	if (VERB) cerr << x << " x " << y << " " << w << " x " << h << " " << importance << endl;

	char outname[256];
	strcpy(outname, m_MediaFile);
	int n = strlen(outname)-1;
	while (n>0 && outname[n]!='/') n--;
	if (outname[n]=='/') outname[n+1] = '\0';
	strcat(outname, TRANSCODENAME);

	// to replace local read with following:
	/*MediaDB RamDB;
	MomVop *M = RamDB.LoadNamedImage(m_MediaFile);
	*/

	cerr << "Writing transcoded media: " << outname << endl;
	GrayImage G(m_MediaFile);
	region(G, x, y, w, h).writepgm(outname);

	return 0;
}

//----------------------------------------------------------------------------
ImportanceHintSearchInterfaceABC *ImportanceHintSearchTool::GetInterface(void)
{ return &m_Interface; }

