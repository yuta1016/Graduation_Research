///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VariationExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

//=============================================================================

static const int VERB = 0;//sth define??

using namespace XM;
using namespace std;

//=============================================================================
// Local classes for handling Variation DS and Media Coding DS in memory
//=============================================================================

enum VARIATIONTYPE { SCALING, EXTRACTION, SUBSTITUTION };
const char* VARIATIONTYPESTRING[] = {"Scaling\0", "Extraction\0", "Substitution\0" };

class MediaCodingDS {

public:

	char filename[512];
	float has_video, has_audio, datasize, fps, audiorate, color; 
	float spatial_size;

	MediaCodingDS() {}

	istream& read(istream& in) {
		in >> filename >> has_video >>  has_audio >>  datasize 
			>> fps >> audiorate >>  spatial_size >> color;
		return in;
		}

	ostream& write(ostream& out) {
		out << filename << " " << has_video << " " 
			<< has_audio << " " << datasize  << " " 
			<< fps << " " << audiorate << " " 
			<< spatial_size << " " << color;
		return out;
		}

	};


class VariationDS {

public:

	VARIATIONTYPE d_type;
	float d_fidelity;
	char d_baseurl[256];
	char d_varurl[256];
	const MediaCodingDS& d_MA, d_MB;

	VariationDS(const MediaCodingDS& MA, const MediaCodingDS& MB) : d_MA(MA), d_MB(MB) {
		d_type = SUBSTITUTION;
		strcpy(d_baseurl, MA.filename);
		strcpy(d_varurl, MB.filename);

		d_fidelity = 0;
		if (MA.has_audio) d_fidelity+=(MB.has_audio/MA.has_audio);
		if (MA.has_video) d_fidelity+=(MB.has_video/MA.has_video);
		if (MA.fps) d_fidelity+=(MB.fps/MA.fps);
		if (MA.audiorate) d_fidelity+=(MB.audiorate/MA.audiorate);
		if (MA.spatial_size) d_fidelity+=(MB.spatial_size/MA.spatial_size);
		if (MA.color) d_fidelity+=(MB.color/MA.color);
		if (MA.datasize) d_fidelity+=(MB.datasize/MA.datasize);

		d_fidelity/=7.0;

		}

		ostream& write(ostream& out) {
		out	<< "source" << "=" << d_baseurl << ", "
			<< "variation" << "=" << d_varurl << ", " 
			<< "fidelity" << "=" << d_fidelity << ", " 
			<< "type" << "=" <<  VARIATIONTYPESTRING[(int) d_type];
		return out;
		}

	};


//=============================================================================


const UUID VariationExtractionInterface::myID = UUID("");
const char *VariationExtractionInterface::myName = "VariationDSExtractionInterface";

const UUID VariationExtractionTool::myID = UUID("");
const char *VariationExtractionTool::myName = "VariationExtraction";

const UUID VariationExtractionInterfaceABC::myID = UUID();


//=============================================================================
VariationExtractionInterfaceABC::VariationExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
VariationExtractionInterface::VariationExtractionInterface(VariationExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
VariationExtractionInterface::~VariationExtractionInterface()
{
}

//----------------------------------------------------------------------------
void VariationExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//----------------------------------------------------------------------------
int VariationExtractionInterface::SetSourceMedia(ifstream& in)
{
  return m_ExtractionTool->SetSourceMedia(in);
}

//----------------------------------------------------------------------------
int VariationExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)

{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long VariationExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long VariationExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long VariationExtractionInterface::PostExtraction(void)

{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& VariationExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VariationExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
VariationExtractionTool::VariationExtractionTool():
m_Interface(this),
m_DescriptionInterface(0),
m_Inf(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
VariationExtractionTool::
VariationExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(theParent),//sth
m_Inf(0)
{
  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
VariationExtractionTool::~VariationExtractionTool()
{

}

//----------------------------------------------------------------------------
bool VariationExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool VariationExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------

int VariationExtractionTool::SetSourceMedia(ifstream& in) {
	m_Inf			= &in;
	if (VERB) if (VERB) cerr << "SetSourceMedia to &in" << endl;
	return(0);
}

//----------------------------------------------------------------------------
int VariationExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long VariationExtractionTool::InitExtracting(void)
{

	/* test if parent exists*/
	if (!m_DescriptionInterface)  return (unsigned long)-1;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long VariationExtractionTool::StartExtracting(void)
{

	ExtractVariations( 1, m_DescriptionInterface);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long VariationExtractionTool::PostExtraction(void)
{
	return 0;
}

//----------------------------------------------------------------------------
const UUID& VariationExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VariationExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
VariationExtractionInterfaceABC *VariationExtractionTool::GetInterface(void)
{ return &m_Interface; }

	
/*
<VariationSet>
   <Source>
		<Video>
			<MediaLocator><MediaUri>file://soccer-A.mpg</MediaUri></MediaLocator>
		</Video>
   </Source>
   <Variation fidelity="0.85" priority="1">
	 	<Image>
			<MediaLocator><MediaUri>file://soccer-B.mpg</MediaUri></MediaLocator>
	 	</Image>
		<VariationRelationship>extraction</VariationRelationship>
   </Variation>
   <Variation fidelity="0.75" priority="3">
		<Audio>
			<MediaLocator><MediaUri>file://soccer-C.mpg</MediaUri></MediaLocator>
		</Audio>
      		<VariationRelationship>extraction</VariationRelationship>
		<VariationRelationship>languageTranslation</VariationRelationship>
   </Variation>
   <Variation fidelity="0.5" priority="2" timeOffset="PT10S" timeScale="0.5">
 		<Video>
			<MediaInformation>
				<MediaProfile master="false">
					<MediaFormat>
						<FileFormat>MPEG</FileFormat>
						<Color>color</Color>
						<FrameRate>30.0</FrameRate>
						<AudioSamplingRate>22000</AudioSamplingRate>
						<CompressionFormat>MPEG-1</CompressionFormat>
					</MediaFormat>
				</MediaProfile>
			</MediaInformation>
			<MediaLocator><MediaUri>file://soccer-E.mpg</MediaUri></MediaLocator>
		</Video>
      	<VariationRelationship>spatialReduction</VariationRelationship>
		<VariationRelationship>compression</VariationRelationship>
		<VariationRelationship>alternativeMediaProfile</VariationRelationship>
   </Variation>
</VariationSet>
*/


GenericDSInterfaceABC*
VariationExtractionTool::ExtractVariations(int iVariationID, 
										   GenericDSInterfaceABC *parentInterface) {

	if (VERB) cerr << "ExtractVariations" << endl;
	assert(parentInterface != NULL);

	int i = 0;
	istream& in = *m_Inf;
	MediaCodingDS MA, MB;

	// ------------------------------------------------------------------------
	// Create root of the Variations
	// ------------------------------------------------------------------------

	if (VERB) cerr << "Creating Child" << endl;
	GenericDS Variations = parentInterface->CreateChild("VariationSet");
	if (VERB) cerr << "CreateChild" << endl;
	GenericDSInterfaceABC *VariationsInterface = Variations.GetInterface();
	if (VERB) cerr << "Create root of the Variations" << endl;

	// ------------------------------------------------------------------------
	// Add Source Media information
	// ------------------------------------------------------------------------

	GenericDS Source = VariationsInterface->CreateChild("Source");
	GenericDSInterfaceABC *SourceInterface = Source.GetInterface();

	GenericDS SourceMedia = SourceInterface->CreateChild("Video");
	GenericDSInterfaceABC *SourceMediaInterface = SourceMedia.GetInterface();

	// ------------------------------------------------------------------------
	// Read media coding information and extract variations
	// ------------------------------------------------------------------------

	if (MA.read(in)) {
		if (VERB) MA.write(cerr) << endl;

			// ------------------------------------------------------------------------
			// Add Source MediaInformation
			// ------------------------------------------------------------------------

			GenericDS SourceMediaInformation = SourceMediaInterface->CreateChild("MediaInformation");
			GenericDSInterfaceABC *SourceMediaInformationInterface = SourceMediaInformation.GetInterface();

			GenericDS SourceMediaProfile = SourceMediaInformationInterface->CreateChild("MediaProfile");
			GenericDSInterfaceABC *SourceMediaProfileInterface = SourceMediaProfile.GetInterface();

			GenericDS SourceMediaFormat = SourceMediaProfileInterface->CreateChild("MediaFormat");
			GenericDSInterfaceABC *SourceMediaFormatInterface = SourceMediaFormat.GetInterface();

			GenericDS SourceFileFormat = SourceMediaFormatInterface->CreateChild("FileFormat");
			GenericDSInterfaceABC *SourceFileFormatInterface = SourceFileFormat.GetInterface();
			SourceFileFormatInterface->SetValue("MPEG");

			GenericDS SourceColor = SourceMediaFormatInterface->CreateChild("Color");
			GenericDSInterfaceABC *SourceColorInterface = SourceColor.GetInterface();
			SourceColorInterface->SetValue("color");

			GenericDS SourceFileSize = SourceMediaFormatInterface->CreateChild("FileSize");
			GenericDSInterfaceABC *SourceFileSizeInterface = SourceFileSize.GetInterface();
			SourceFileSizeInterface->SetValue(MA.datasize);

			GenericDS SourceFrameRate = SourceMediaFormatInterface->CreateChild("FrameRate");
			GenericDSInterfaceABC *SourceFrameRateInterface = SourceFrameRate.GetInterface();
			SourceFrameRateInterface->SetValue(MA.fps);

			GenericDS SourceAudioSampling = SourceMediaFormatInterface->CreateChild("AudioSampling");
			GenericDSInterfaceABC *SourceAudioSamplingInterface = SourceAudioSampling.GetInterface();
			SourceAudioSamplingInterface->SetValue(MA.audiorate);

			GenericDS SourceCompressionFormat = SourceMediaFormatInterface->CreateChild("CompressionFormat");
			GenericDSInterfaceABC *SourceCompressionFormatInterface = SourceCompressionFormat.GetInterface();
			SourceCompressionFormatInterface->SetValue("MPEG-1");

			// ------------------------------------------------------------------------
			// Add Source MediaLocator
			// ------------------------------------------------------------------------

			
			GenericDS SourceMediaLocator = SourceMediaInterface->CreateChild("MediaLocator");
			GenericDSInterfaceABC *SourceMediaLocatorInterface = SourceMediaLocator.GetInterface();

			GenericDS SourceMediaUri = SourceMediaLocatorInterface->CreateChild("MediaUri");
			GenericDSInterfaceABC *SourceMediaUriInterface = SourceMediaUri.GetInterface();
			SourceMediaUriInterface->SetValue(MA.filename);

		while (MB.read(in)) {
			i++;
			if (VERB) MB.write(cerr) << endl;
			in >> ws;
			VariationDS V(MA, MB);
			if (VERB) V.write(cerr) << endl;

			// ------------------------------------------------------------------------
			// Create a new Variation as child of the Variations
			// ------------------------------------------------------------------------

			GenericDS Variation = VariationsInterface->CreateChild("Variation");
			GenericDSInterfaceABC *VariationInterface = Variation.GetInterface();

			// ------------------------------------------------------------------------
			// Add Variation Media information
			// ------------------------------------------------------------------------

			GenericDS VariationMedia = VariationInterface->CreateChild("Video");
			GenericDSInterfaceABC *VariationMediaInterface = VariationMedia.GetInterface();

			// ------------------------------------------------------------------------
			// Add Variation MediaInformation
			// ------------------------------------------------------------------------

			GenericDS VariationMediaInformation = VariationMediaInterface->CreateChild("MediaInformation");
			GenericDSInterfaceABC *VariationMediaInformationInterface = VariationMediaInformation.GetInterface();

			GenericDS VariationMediaProfile = VariationMediaInformationInterface->CreateChild("MediaProfile");
			GenericDSInterfaceABC *VariationMediaProfileInterface = VariationMediaProfile.GetInterface();

			GenericDS VariationMediaFormat = VariationMediaProfileInterface->CreateChild("MediaFormat");
			GenericDSInterfaceABC *VariationMediaFormatInterface = VariationMediaFormat.GetInterface();

			GenericDS VariationFileFormat = VariationMediaFormatInterface->CreateChild("FileFormat");
			GenericDSInterfaceABC *VariationFileFormatInterface = VariationFileFormat.GetInterface();
			VariationFileFormatInterface->SetValue("MPEG");

			if (MB.fps!=0) {
				GenericDS VariationColor = VariationMediaFormatInterface->CreateChild("Color");
				GenericDSInterfaceABC *VariationColorInterface = VariationColor.GetInterface();
				VariationColorInterface->SetValue("color");
			}

			GenericDS VariationFileSize = VariationMediaFormatInterface->CreateChild("FileSize");
			GenericDSInterfaceABC *VariationFileSizeInterface = VariationFileSize.GetInterface();
			VariationFileSizeInterface->SetValue(MB.datasize);

			if (MB.fps!=0) {
				GenericDS VariationFrameRate = VariationMediaFormatInterface->CreateChild("FrameRate");
				GenericDSInterfaceABC *VariationFrameRateInterface = VariationFrameRate.GetInterface();
				VariationFrameRateInterface->SetValue(MB.fps);
			}

			if (MB.audiorate!=0) {
				GenericDS VariationAudioSampling = VariationMediaFormatInterface->CreateChild("AudioSampling");
				GenericDSInterfaceABC *VariationAudioSamplingInterface = VariationAudioSampling.GetInterface();
				VariationAudioSamplingInterface->SetValue(MB.audiorate);
			}

			GenericDS VariationCompressionFormat = VariationMediaFormatInterface->CreateChild("CompressionFormat");
			GenericDSInterfaceABC *VariationCompressionFormatInterface = VariationCompressionFormat.GetInterface();
			VariationCompressionFormatInterface->SetValue("MPEG-1");

/*
			<MediaInformation>
				<MediaProfile master="false">
					<MediaFormat>
						<FileFormat>MPEG</FileFormat>
						<Color>color</Color>
						<FileSize>200000000</FileSize>
						<FrameRate>30.0</FrameRate>
						<AudioSamplingRate>22000</AudioSamplingRate>
						<CompressionFormat>MPEG-1</CompressionFormat>
					</MediaFormat>
				</MediaProfile>
			</MediaInformation>
*/


			// ------------------------------------------------------------------------
			// Add Variation MediaLocator
			// ------------------------------------------------------------------------

			GenericDS VariationMediaLocator = VariationMediaInterface->CreateChild("MediaLocator");
			GenericDSInterfaceABC *VariationMediaLocatorInterface = VariationMediaLocator.GetInterface();

			GenericDS VariationMediaUri = VariationMediaLocatorInterface->CreateChild("MediaUri");
			GenericDSInterfaceABC *VariationMediaUriInterface = VariationMediaUri.GetInterface();
			VariationMediaUriInterface->SetValue(V.d_varurl);

			// ------------------------------------------------------------------------
			// Add Variation relationship
			// ------------------------------------------------------------------------

			if ((int)V.d_type<3) {
				GenericDS VariationRelationship = VariationInterface->CreateChild("VariationRelationship");
				GenericDSInterfaceABC *VariationRelationshipInterface = VariationRelationship.GetInterface();
				VariationRelationshipInterface->SetValue(VARIATIONTYPESTRING[(int) V.d_type]);
			}

    		// ------------------------------------------------------------------------
			// Add Variation attribute information (type and fidelity)
			// ------------------------------------------------------------------------
	
			VariationInterface->SetAttribute("fidelity", V.d_fidelity);
			VariationInterface->SetAttribute("priority", i);

			}
		}
	return parentInterface;
	}

