//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jiuhuai Lu
// Tektronix, Inc.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MediaQualityExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "DescriptionSchemes/DescriptionSchemes.h"         /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"
#include "MediaQualityExtraction.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID MediaQualityExtractionInterface::myID = UUID("");
const char *MediaQualityExtractionInterface::myName =
"MediaQualityDescriptionExtractorInterface";

const UUID MediaQualityExtractionTool::myID = UUID("");
const char *MediaQualityExtractionTool::myName =
"MediaQualityDescriptionExtractor";

const UUID MediaQualityExtractionInterfaceABC::myID = UUID();

//=============================================================================
MediaQualityExtractionInterfaceABC::MediaQualityExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MediaQualityExtractionInterface::
MediaQualityExtractionInterface(MediaQualityExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MediaQualityExtractionInterface::~MediaQualityExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MediaQualityExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaQualityExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void MediaQualityExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int MediaQualityExtractionInterface::SetSourceMedia
    (MomVop* media_comp, MomVop* media_ref)
{
  return m_ExtractionTool->SetSourceMedia(media_comp, media_ref);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* MediaQualityExtractionInterface::
GetDescriptionInterface(void)

{
  return m_ExtractionTool->GetDescriptionInterface();
}

//----------------------------------------------------------------------------
int MediaQualityExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC
		       *aMediaQualityDescriptionInterface)

{
  return m_ExtractionTool->
    SetDescriptionInterface(aMediaQualityDescriptionInterface);
}

//----------------------------------------------------------------------------
unsigned long MediaQualityExtractionInterface::InitExtracting( MediaDB *RamDB_comp, MediaDB *RamDB_ref )
{
  return(m_ExtractionTool->InitExtracting(RamDB_comp, RamDB_ref));
}

//----------------------------------------------------------------------------
unsigned long MediaQualityExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long MediaQualityExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
MediaQualityExtractionTool::MediaQualityExtractionTool():
m_Interface(this),
m_DescriptionInterface(NULL),
m_FrameCnt(0)

{

} 

//=============================================================================
MediaQualityExtractionTool::
MediaQualityExtractionTool(GenericDSInterfaceABC
			*MediaQuality):
m_Interface(this),
m_DescriptionInterface(NULL),
m_FrameCnt(0)

{
  // connect Descritors with Extractions*/
  SetDescriptionInterface(MediaQuality);
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MediaQualityExtractionTool::~MediaQualityExtractionTool()
{
}

//----------------------------------------------------------------------------
bool MediaQualityExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool MediaQualityExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.

int MediaQualityExtractionTool::SetSourceMedia
	(MomVop* media_comp, MomVop* media_ref)
{
  if(media_comp==NULL) return -1;
  m_Media_comp =media_comp;
  m_Media_ref  =media_ref ;

  return 0;
}


//----------------------------------------------------------------------------
GenericDSInterfaceABC* MediaQualityExtractionTool::
GetDescriptionInterface(void)

{
  return m_DescriptionInterface;
}

//----------------------------------------------------------------------------
int MediaQualityExtractionTool::
SetDescriptionInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptionInterface=ParentDSInterface;
  return 0;
}
//----------------------------------------------------------------------------
// This reads in the parameters to be used to fill the description scheme

int MediaQualityExtractionTool::ReadParameterfile ()
{
	char *paraval;

	paraval = getpara ("RatingSource", NULL);
	if (!paraval) m_RatingSource[0] = '\0';
	else { strcpy (m_RatingSource, paraval); }

	paraval = getpara ("RatingInfoLocator", NULL);
	if (!paraval) m_RatingInfoLocator[0] = '\0';
	else { strcpy (m_RatingInfoLocator, paraval); }

	
	// Reads video defect information 
	paraval = getpara ("PerceptibleDefectsVisualL", NULL);
	if (!paraval) { 
		m_PerceptibleDefectsVisualL[0][0] = '\0';
		m_NoOfVisDefects = 0;
	}
	else { 
		const int MAXLENGTH = 100;
		char form[MAXLENGTH];
		int ientry=0;

		strcpy (form, "%s");
		while 
		 ( sscanf (paraval, form, m_PerceptibleDefectsVisualL[ientry])!=EOF) {
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		m_NoOfVisDefects = ientry;

		/*
		paraval = getpara ("PerceptibleDefectsVisualCSN", NULL);
		if (!paraval) return -1;
		strcpy (form, "%s");
		ientry = 0;
		while 
		 (sscanf(paraval, form, m_PerceptibleDefectsVisualCSN[ientry])!=EOF) {
			if (strcmp(m_PerceptibleDefectsVisualCSN[ientry], "__empty")==0) 
				m_PerceptibleDefectsVisualCSN[ientry][0] = '\0';
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		if (ientry!= m_NoOfVisDefects) return -1;

		paraval = getpara ("PerceptibleDefectsVisualCSL", NULL);
		if (!paraval) return -1;
		strcpy (form, "%s");
		ientry = 0;
		while 
		 ( sscanf (paraval, form, m_PerceptibleDefectsVisualCSL[ientry])!=EOF)
		{
			if (strcmp(m_PerceptibleDefectsVisualCSL[ientry], "__empty")==0) 
				m_PerceptibleDefectsVisualCSL[ientry][0] = '\0';
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		if (ientry!= m_NoOfVisDefects) return -1;
		*/
		paraval = getpara ("PerceptibleDefectsVisualHref", NULL);
		if (!paraval) return -1;
		strcpy (form, "%s");
		ientry = 0;
		while 
		 ( sscanf (paraval, form, m_PerceptibleDefectsVisualCST[ientry])!=EOF)
		{
			if (strcmp(m_PerceptibleDefectsVisualCST[ientry], "__empty")==0) 
				m_PerceptibleDefectsVisualCST[ientry][0] = '\0';
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		if (ientry!= m_NoOfVisDefects) return -1;
	}
		
	// Reads audio defect information parameters
	paraval = getpara ("PerceptibleDefectsAudioL", NULL);
	if (!paraval) { 
		m_PerceptibleDefectsAudioL[0][0] = '\0';
		m_NoOfAudDefects = 0;
	}
	else { 
		const int MAXLENGTH = 100;
		char form[MAXLENGTH];
		int ientry=0;

		strcpy (form, "%s");
		while 
		 ( sscanf (paraval, form, m_PerceptibleDefectsAudioL[ientry])!=EOF) {
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		m_NoOfAudDefects = ientry;

		/*
		paraval = getpara ("PerceptibleDefectsAudioCSN", NULL);
		if (!paraval) return -1;
		strcpy (form, "%s");
		ientry = 0;
		while 
		 (sscanf(paraval, form, m_PerceptibleDefectsAudioCSN[ientry])!=EOF) {
			if (strcmp(m_PerceptibleDefectsAudioCSN[ientry], "__empty")==0) 
				m_PerceptibleDefectsAudioCSN[ientry][0] = '\0';
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		if (ientry!= m_NoOfAudDefects) return -1;

		paraval = getpara ("PerceptibleDefectsAudioCSL", NULL);
		if (!paraval) return -1;
		strcpy (form, "%s");
		ientry = 0;
		while 
		 ( sscanf (paraval, form, m_PerceptibleDefectsAudioCSL[ientry])!=EOF)
		{
			if (strcmp(m_PerceptibleDefectsAudioCSL[ientry], "__empty")==0) 
				m_PerceptibleDefectsAudioCSL[ientry][0] = '\0';
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		if (ientry!= m_NoOfAudDefects) return -1;
		*/

		paraval = getpara ("PerceptibleDefectsAudioHref", NULL);
		if (!paraval) return -1;
		strcpy (form, "%s");
		ientry = 0;
		while 
		 ( sscanf (paraval, form, m_PerceptibleDefectsAudioCST[ientry])!=EOF)
		{
			if (strcmp(m_PerceptibleDefectsAudioCST[ientry], "__empty")==0) 
				m_PerceptibleDefectsAudioCST[ientry][0] = '\0';
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		if (ientry!= m_NoOfAudDefects) return -1;
	}

	paraval = getpara ("RatingMax", NULL);
	if (!paraval) m_isRatingMaxDefined = 0;
	else { 
		m_isRatingMaxDefined = 1;
		sscanf (paraval, "%d", &m_RatingMax); 
	}

	paraval = getpara ("RatingMin", NULL);
	if (!paraval) m_isRatingMinDefined = 0;
	else { 
		m_isRatingMinDefined = 1;
		sscanf (paraval, "%d", &m_RatingMax); 
	}
	paraval = getpara ("RatingStyle", NULL);
	if (!paraval) m_RatingStyle[0] = '\0';
	else { strcpy (m_RatingStyle, paraval); }
  	
	paraval = getpara ("RatingType", NULL);
	if (!paraval) m_RatingType[0] = '\0';
	else { strcpy (m_RatingType, paraval); }

	/*
	paraval = getpara ("RatingValueY", NULL);
	if (!paraval) return -1;
	else { sscanf (paraval, "%f", &m_RatingValueY); }

	paraval = getpara ("RatingValueU", NULL);
	if (!paraval) return -1;
	else { sscanf (paraval, "%f", &m_RatingValueU); }

	paraval = getpara ("RatingValueV", NULL);
	if (!paraval) return -1;
	else { sscanf (paraval, "%f", &m_RatingValueV); }
	*/

	int buf_NoOfSchemes;
	paraval = getpara ("RatingSchemeL", NULL);
	if (!paraval) {
		fprintf (stderr, "In parameter file: At lease one value is needed for RatingSchemeL\n");
		exit (1);
	}
	else { 
		const int MAXLENGTH = 100;
		char form[MAXLENGTH];
		int ientry=0;

		strcpy (form, "%s");
		while (sscanf(paraval, form, m_RatingSchemeL[ientry])!=EOF) {
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		m_NoOfSchemes = ientry;
	}
	buf_NoOfSchemes = m_NoOfSchemes;

	paraval = getpara ("RatingSchemeHref", NULL);
	if (!paraval) {
		fprintf (stderr, "In parameter file: At lease one value is needed for RatingSchemeL\n");
		exit (1);
	}
	else { 
		const int MAXLENGTH = 100;
		char form[MAXLENGTH];
		int ientry=0;

		strcpy (form, "%s");
		while (sscanf(paraval, form, m_RatingSchemeCST[ientry])!=EOF) {
			strcpy (&form [strlen(form)-2], "%*s%s");
			ientry++;
		}
		m_NoOfSchemes = ientry;
	}
	m_NoOfSchemes = 
		buf_NoOfSchemes < m_NoOfSchemes? buf_NoOfSchemes:m_NoOfSchemes;

	/*
	paraval = getpara ("RatingSchemeCSN", NULL);
	if (!paraval) m_RatingSchemeCSN[0] = '\0';
	else { strcpy (m_RatingSchemeCSN, paraval); }
	paraval = getpara ("RatingSchemeCSL", NULL);
	if (!paraval) m_RatingSchemeCSL[0] = '\0';
	else { strcpy (m_RatingSchemeCSL, paraval); }
	*/
	return (0);
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long MediaQualityExtractionTool::InitExtracting
   (MediaDB *RamDB_comp, MediaDB *RamDB_ref)

{

  
  if (!RamDB_comp) return (unsigned long)-1;
  // Get the number of frames
  //m_noframes = RamDB_comp->GetNoOfElements();

  /* test if parent exists*/
  if (!m_DescriptionInterface)  return (unsigned long)-1;

  /* test if parent is document node. If yes, remove old MediaQuality*/

//sth   cerr << m_DescriptionInterface->GetDSName() << endl;

  if (m_DescriptionInterface->GetDSName() == "#document") {
    if (!m_MediaQuality.isNull()) {
      if (m_MediaQuality.GetDSName() == "MediaQuality") {
		m_DescriptionInterface->RemoveChild(m_MediaQuality);
      }
    }
  }

  /* create MediaQuality*/
  m_MediaQuality=m_DescriptionInterface->
    CreateChild("MediaQuality");

  // Reads information and parameters about the media to be loaded
  if (ReadParameterfile()==-1) {
  	fprintf (stderr, "Error reading the parameter file\n") ;
	exit (1);
  } 
  
  /* initialize description values*/
  m_FrameCnt=0;

  return(0UL);
}

//----------------------------------------------------------------------------
// This performs frame based extraction
// 
unsigned long MediaQualityExtractionTool::StartExtracting(void)

{
  // select media type you require and remove this comment
  MomVop *ImageMedia;

  // First check that it's all correctly initialised
  if (m_MediaQuality.isNull()) return (unsigned long)-1;
  if (m_MediaQuality.GetDSName() != "MediaQuality") return (unsigned long)-1;


  if (ImageMedia==NULL) return (unsigned long)-1;

  // Extract the PSNR for the current frame. 
	Psnr_ColorImage_YUV (m_Media_comp, m_Media_ref, 
		psnr_per_frame_Y[m_FrameCnt], 
		psnr_per_frame_U[m_FrameCnt], 
		psnr_per_frame_V[m_FrameCnt]);
	// S. Herrmann: sth don´t print to stdout - OK, commented off below
	//printf ("%f, %f, %f\n", psnr_per_frame_Y[m_FrameCnt], psnr_per_frame_U[m_FrameCnt], psnr_per_frame_V[m_FrameCnt]);
  /* Increment frame counter*/
  m_FrameCnt++;
  
  return 0;
}

//----------------------------------------------------------------------------
// This computes the average of the quality evaluation across frames. 
// and creates the description tree.
// 

unsigned long MediaQualityExtractionTool::PostExtracting(void)
{
 	int iframe; 

	// Calculate average PSNR 
	m_RatingValueY = m_RatingValueU = m_RatingValueV = 0.0;
	for (iframe=0; iframe<m_FrameCnt; iframe++) {
		m_RatingValueY+= psnr_per_frame_Y[iframe];
		m_RatingValueU+= psnr_per_frame_U[iframe];
		m_RatingValueV+= psnr_per_frame_V[iframe];
	}
	m_RatingValueY /= iframe;
	m_RatingValueU /= iframe;
	m_RatingValueV /= iframe;
	fprintf(stderr,"\n\nAverage PSNRs: %f, %f, %f\n", m_RatingValueY, m_RatingValueU, m_RatingValueV);

	// Start build the XML description 

	if (m_MediaQuality.isNull()) return (unsigned long)-1;
	if (m_MediaQuality.GetDSName() != "MediaQuality") return (unsigned long)-1;

	GenericDSInterfaceABC *MediaQualityDSint = m_MediaQuality.GetInterface(); 


	for (int ischeme=0; ischeme<m_NoOfSchemes; ischeme++) {
		float rating_value;
		if (strcmp (m_RatingSchemeL[ischeme], "PSNR_Y")==0)
			rating_value= m_RatingValueY;
		else if (strcmp (m_RatingSchemeL[ischeme], "PSNR_U")==0)
			rating_value= m_RatingValueU;
		else if (strcmp (m_RatingSchemeL[ischeme], "PSNR_V")==0)
			rating_value= m_RatingValueV;
		else {
			fprintf (stderr, "%s is not supported", m_RatingSchemeL[ischeme]);
			continue;
		}

		GenericDS RatingDS = MediaQualityDSint->CreateChild ("QualityRating");
		GenericDSInterfaceABC *RatingDSint = RatingDS.GetInterface();
		RatingDSint->SetAttribute ("type", m_RatingType);
		GenericDS RatingValueDS = RatingDSint->CreateChild ("RatingValue");
		RatingValueDS.SetValue (rating_value);

		GenericDS RatingSchemeDS = RatingDSint->CreateChild ("RatingScheme");

		GenericDSInterfaceABC 
			*RatingSchemeDSint = RatingSchemeDS.GetInterface();
		RatingSchemeDSint->SetAttribute ("href", m_RatingSchemeCST[ischeme]);
		RatingSchemeDSint->SetAttribute ("style",  m_RatingStyle);
		//RatingSchemeDSint->SetAttribute ("scheme", m_RatingSchemeCSN);
		//RatingSchemeDSint->SetAttribute ("term", m_RatingSchemeCST[ischeme]);
		//RatingSchemeDSint->SetAttribute ("schemeLocation", m_RatingSchemeCSL);
		GenericDS 
			LabelDS = RatingSchemeDSint->CreateChild ("Name" );
		GenericDSInterfaceABC 
			*LabelDSint = LabelDS.GetInterface();
		LabelDSint->SetAttribute ("xml:lang", "en");
		LabelDSint->SetValue (m_RatingSchemeL[ischeme]);

		if (m_isRatingMaxDefined) {
			RatingSchemeDSint->SetAttribute ("best", m_RatingMax);
		}

		if (m_isRatingMinDefined) {
			RatingSchemeDSint->SetAttribute ("worst", m_RatingMin);
		}

		//GenericDS 
		//	RatingStyleDS = RatingMetricDSint->CreateChild ("RatingStyle");

		//GenericDSInterfaceABC *RatingStyleDSint = RatingStyleDS.GetInterface();
		//RatingStyleDSint->SetValue (m_RatingStyle);

	}

	GenericDS RatingSourceDS = 
		MediaQualityDSint->CreateChild ("RatingSource");
	GenericDSInterfaceABC *RatingSourceDSint = RatingSourceDS.GetInterface();
	//if ( isDefined (Organization) {
		GenericDS OrganizationDS = 
			RatingSourceDSint->CreateChild ("Organization");
		GenericDSInterfaceABC *OrganizationDSint =
			OrganizationDS.GetInterface();
		GenericDS OrganizationNameDS = OrganizationDSint->CreateChild("Name");
		OrganizationNameDS.SetAttribute ("xml:lang", "en");
		OrganizationNameDS.SetAttribute ("type", "main");
		OrganizationNameDS.SetValue (m_RatingSource);
		// Addresses and Contacts not used 
	//}
	// else may include Person and GroupPerson, not included here 

	GenericDS RatingInformationLocatorDS = 
		MediaQualityDSint->CreateChild ("RatingInformationLocator");
	GenericDSInterfaceABC *RatingInformationLocatorDSint =
		RatingInformationLocatorDS.GetInterface();
	RatingInformationLocatorDSint->SetAttribute ("href", m_RatingInfoLocator);
	// The following is set arbitorarily
	// RatingInformationLocatorDSint->SetAttribute ("IDREF", 12);
	
	// Create a Perceptible Defect Type 
	if (m_NoOfVisDefects||m_NoOfAudDefects)
	{
		GenericDS PerceptibleDefectsDS = 
			MediaQualityDSint->CreateChild ("PerceptibleDefects");

		GenericDSInterfaceABC 
			*PerceptibleDefectsDSint = PerceptibleDefectsDS.GetInterface();

		// printf ("m_NoOfVisDefects= %d\n", m_NoOfVisDefects); //sth

		if (m_NoOfVisDefects) 
		  for (int idefect=0; idefect<m_NoOfVisDefects; idefect++) { 
			GenericDS VisualTypeDS = 
				PerceptibleDefectsDSint->CreateChild("VisualDefects");

			GenericDSInterfaceABC 
				*VisualTypeDSInt = VisualTypeDS.GetInterface();

			//VisualTypeDSInt->SetAttribute 
			//	("scheme",     m_PerceptibleDefectsVisualCSN[idefect]);
			//VisualTypeDSInt->SetAttribute 
			//	("term",   m_PerceptibleDefectsVisualCST[idefect]);
			//VisualTypeDSInt->SetAttribute 
			//	("schemeLocation", m_PerceptibleDefectsVisualCSL[idefect]);
			VisualTypeDSInt->SetAttribute 
				("href", m_PerceptibleDefectsVisualCST[idefect]);

			GenericDS 
				VisualTypeLabelDS = VisualTypeDSInt->CreateChild("Name");
			GenericDSInterfaceABC *VisualTypeLabelDSint = 
				VisualTypeLabelDS.GetInterface();
			VisualTypeLabelDSint->SetAttribute ("xml:lang", "en");
			VisualTypeLabelDSint->SetValue 
				(m_PerceptibleDefectsVisualL[idefect]);
		}
	}


	return(0UL);
}

//----------------------------------------------------------------------------
const UUID& MediaQualityExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaQualityExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
MediaQualityExtractionInterfaceABC *MediaQualityExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
void MediaQualityExtractionTool::Psnr_ColorImage_YUV 
(MomVop* imgdata, MomVop* refdata, float& psnry, float& psnru, float& psnrv )
{
	UChar *imgu, *imgv, *imgy, *refy, *refu, *refv;
	
	assert (imgdata); 
	assert (refdata);

	imgu = imgdata->u_chan->data->u;
	imgv = imgdata->v_chan->data->u;
	imgy = imgdata->y_chan->data->u;
	refu = refdata->u_chan->data->u;
	refv = refdata->v_chan->data->u;
	refy = refdata->y_chan->data->u;
	
	psnry = psnr_image ( imgy, refy, 
		imgdata->y_chan->x * imgdata->y_chan->y);
	psnru = psnr_image ( imgu, refu, 
		imgdata->u_chan->x * imgdata->u_chan->y);
	psnrv = psnr_image ( imgv, refv, 
		imgdata->v_chan->x * imgdata->v_chan->y);
}

//----------------------------------------------------------------------------
float MediaQualityExtractionTool::psnr_image 
   (UChar *i1, UChar *i2, const int size )
{
	int i;
	float sum=0.0;
	const float MAX_PSNR = 10.0*log(256.0*256.0*(float)size);
	for (i=0;i<size;i++) {
		sum+= ((float)*(i2+i)-(float)*(i1+i))*((float)*(i2+i)-(float)*(i1+i));
	}

	if (sum < 1.0 ) return (MAX_PSNR);
	
	sum /= size;
	return ( 10.0*log(256.0*256.0/sum) );
}
