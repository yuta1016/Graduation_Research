///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// P. Brasnett
// Mitsubishi Electric R&D Centre Europe
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignature.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <cstring>
#include "Descriptors/Descriptors.h"


void CTernaryOps::init_ter_to_bin_table()
{
	for(int i=0;i<1024;++i)
	{
		_ter_to_bin_table[i] = 0;
	}
}

void CTernaryOps::init_bin_to_ter_table()
{
	for(int i=0;i<243;++i)
	{
		_bin_to_ter_table[i] = 0;
	}
}

void CTernaryOps::generate_5_packed_ternary_numbers()
{
	unsigned int binary_number=0;
	for(unsigned int a=0;a<3;++a)
	{
		for(unsigned int b=0;b<3;++b)
		{
			for(unsigned int c=0;c<3;++c)
			{
				for(unsigned int d=0;d<3;++d)
				{
					for(unsigned int e=0;e<3;++e)
					{
						unsigned int t = 81*a + 27*b + 9*c + 3*d + e;
						
						_ter_to_bin_table[t] = binary_number;
						_bin_to_ter_table[binary_number] = t;
						
						binary_number++;
					}
				}
			}
		}
	}
}


CTernaryOps::CTernaryOps(int upperlimit,int lowerlimit) : _threshold_u(upperlimit),_threshold_l(lowerlimit)
{
	init_ter_to_bin_table();
	init_bin_to_ter_table();
	
	generate_5_packed_ternary_numbers();
}

unsigned int CTernaryOps::Pack5TernaryValuesToA32bitInt(unsigned char * p_ternary_array)
{
	unsigned int retval = 0;
	
	retval = 81*p_ternary_array[0] + 27*p_ternary_array[1] + 9*p_ternary_array[2] + 3*p_ternary_array[3] + p_ternary_array[4];
	
	return retval;
}

unsigned char CTernaryOps::EncodeTo8BitBinary(unsigned int packed_ternary_value) const
{		
	return (unsigned char)packed_ternary_value;
}

unsigned int CTernaryOps::DecodeToPackedTernaryValue(unsigned char value) const
{
	if(value>242)
		return (unsigned int)(-1);
	
	return _bin_to_ter_table[value];
}
	

using namespace XM;

const UUID VideoSignatureDescriptorInterface::myID = UUID("");
const char *VideoSignatureDescriptorInterface::myName = "Video Signature Description Interface";

const UUID VideoSignatureDescriptor::myID = UUID("");
const char *VideoSignatureDescriptor::myName = "Video Signature Descriptor";

const UUID VideoSignatureDescriptor::valID = UUID("");
const char *VideoSignatureDescriptor::valName = "Video Signature Descriptor";

const UUID VideoSignatureDescriptorInterfaceABC::myID = UUID();

VideoSignatureDescriptorInterfaceABC::VideoSignatureDescriptorInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================

VideoSignatureDescriptorInterface::VideoSignatureDescriptorInterface(VideoSignatureDescriptor *pVideoSignature)
: m_pVideoSignatureDescriptor(pVideoSignature)
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
VideoSignatureDescriptorInterface::~VideoSignatureDescriptorInterface()
{
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::addref(void)
{
  m_pVideoSignatureDescriptor->addref();
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::release(void)
{
  m_pVideoSignatureDescriptor->release();
}

//-----------------------------------------------------------------------------
const UUID& VideoSignatureDescriptorInterface::GetInterfaceID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *VideoSignatureDescriptorInterface::GetName(void)
{
  return myName;
}


//-----------------------------------------------------------------------------
unsigned long VideoSignatureDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  return m_pVideoSignatureDescriptor->ExportDDL(aParentDescription);
}

//-----------------------------------------------------------------------------
unsigned long VideoSignatureDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_pVideoSignatureDescriptor->ImportDDL(aDescription);
}

//-----------------------------------------------------------------------------
int VideoSignatureDescriptorInterface::SetRegionFrames(unsigned int start_frame, unsigned int nof)
{
  return m_pVideoSignatureDescriptor->SetRegionFrames(start_frame, nof);
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetSpatialRegion(unsigned short top_left_x, unsigned short top_left_y,
					unsigned short bottom_right_x, unsigned short bottom_right_y)
{
	m_pVideoSignatureDescriptor->SetSpatialRegion(top_left_x, top_left_y, bottom_right_x, bottom_right_y);
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetMediaTimeUnit(unsigned int time_unit)
{
	m_pVideoSignatureDescriptor->SetMediaTimeUnit(time_unit);

}

void VideoSignatureDescriptorInterface::SetMediaTime(unsigned int start_time, unsigned int end_time)
{
	m_pVideoSignatureDescriptor->SetMediaTime(start_time, end_time);
}


//-----------------------------------------------------------------------------
int VideoSignatureDescriptorInterface::SetNumberOfSegments(unsigned int nos)
{
  return m_pVideoSignatureDescriptor->SetNumberOfSegments(nos);
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetSegment(unsigned int pos, unsigned int start_frame, unsigned int end_frame,
					const unsigned char *bag_of_words)
{
	m_pVideoSignatureDescriptor->SetSegment(pos, start_frame, end_frame, bag_of_words);
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetFrame(unsigned int pos, unsigned char confidence, const unsigned char *words,
					const unsigned char *frame_signature)
{
	m_pVideoSignatureDescriptor->SetFrame(pos, confidence, words, frame_signature);
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetCompressionFlag(int flag)
{
	m_pVideoSignatureDescriptor->SetCompressionFlag(flag);
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptorInterface::GetRegionFrames()
{
	return m_pVideoSignatureDescriptor->GetRegionFrames();
}

//-----------------------------------------------------------------------------
unsigned short* VideoSignatureDescriptorInterface::GetSpatialRegion()
{
	return m_pVideoSignatureDescriptor->GetSpatialRegion();
}

//-----------------------------------------------------------------------------
unsigned short VideoSignatureDescriptorInterface::GetMediaTimeUnit()
{
	return m_pVideoSignatureDescriptor->GetMediaTimeUnit();
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptorInterface::GetMediaTime()
{
	return m_pVideoSignatureDescriptor->GetMediaTime();
}

//-----------------------------------------------------------------------------
double VideoSignatureDescriptorInterface::GetRegionFrameRate()
{
	return m_pVideoSignatureDescriptor->GetRegionFrameRate();
}

//-----------------------------------------------------------------------------
unsigned int VideoSignatureDescriptorInterface::GetMediaTimeOfFrame(unsigned int frame)
{
	return m_pVideoSignatureDescriptor->GetMediaTimeOfFrame(frame);
}

//-----------------------------------------------------------------------------
unsigned int VideoSignatureDescriptorInterface::GetNumberOfSegments()
{
	return m_pVideoSignatureDescriptor->GetNumberOfSegments();
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptorInterface::GetSegmentStartFrames()
{
	return m_pVideoSignatureDescriptor->GetSegmentStartFrames();
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptorInterface::GetSegmentEndFrames()
{
	return m_pVideoSignatureDescriptor->GetSegmentEndFrames();
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptorInterface::GetSegmentBagOfWords()
{
	return m_pVideoSignatureDescriptor->GetSegmentBagOfWords();
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptorInterface::GetFrameConfidences()
{
	return m_pVideoSignatureDescriptor->GetFrameConfidences();
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptorInterface::GetFrameWords()
{
	return m_pVideoSignatureDescriptor->GetFrameWords();
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptorInterface::GetFrameSignatures()
{
	return m_pVideoSignatureDescriptor->GetFrameSignatures();
}

//-----------------------------------------------------------------------------
int VideoSignatureDescriptorInterface::GetCompressionFlag()
{
	return m_pVideoSignatureDescriptor->GetCompressionFlag();
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetKeyFrameInfo(unsigned char* keyframeinfo, unsigned int numofframe)
{
	m_pVideoSignatureDescriptor->SetKeyFrameInfo(keyframeinfo, numofframe);
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptorInterface::SetKeyFrameInfoFlag(int flag)
{
	m_pVideoSignatureDescriptor->SetKeyFrameInfoFlag(flag); 
}

void VideoSignatureDescriptorInterface::SetKeyIndexTable(unsigned char numofwords, int** nKeyIndexContent, unsigned int** KeyIndex)
{
	m_pVideoSignatureDescriptor->SetKeyIndexTable(numofwords, nKeyIndexContent, KeyIndex);
}

void VideoSignatureDescriptorInterface::SetKeyLookUpTable(unsigned int numberofkey, unsigned int* KeyLookUpTable)
{
	m_pVideoSignatureDescriptor->SetKeyLookUpTable(numberofkey, KeyLookUpTable);
}

void VideoSignatureDescriptorInterface::SetAllIndexTable(unsigned char numofwords, int** nAllIndexContent, unsigned int** AllIndex)
{
	m_pVideoSignatureDescriptor->SetAllIndexTable(numofwords, nAllIndexContent, AllIndex);
}
 

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptorInterface::GetKeyFrameInfo()
{
	return m_pVideoSignatureDescriptor->GetKeyFrameInfo();
}

int VideoSignatureDescriptorInterface::GetNumOfWords()
{
	return m_pVideoSignatureDescriptor->GetNumOfWords();
}

int** VideoSignatureDescriptorInterface::GetNumOfKeyIndex()
{
	return m_pVideoSignatureDescriptor->GetNumOfKeyIndex();
}

int** VideoSignatureDescriptorInterface::GetNumOfAllIndex()
{
	return m_pVideoSignatureDescriptor->GetNumOfAllIndex();
}

unsigned int* VideoSignatureDescriptorInterface::GetKeyLookUpTable()
{
	return m_pVideoSignatureDescriptor->GetKeyLookUpTable();
}

unsigned int** VideoSignatureDescriptorInterface::GetKeyIndexTable()
{
	return m_pVideoSignatureDescriptor->GetKeyIndexTable();
}

unsigned int** VideoSignatureDescriptorInterface::GetAllIndexTable()
{
	return m_pVideoSignatureDescriptor->GetAllIndexTable();
}

//-----------------------------------------------------------------------------
int VideoSignatureDescriptorInterface::GetKeyFrameInfoFlag()
{
	return m_pVideoSignatureDescriptor->GetKeyFrameInfoFlag();
}


//=============================================================================

VideoSignatureDescriptor::VideoSignatureDescriptor()
: m_refcount(0)
, m_Interface(this)
, m_isProprietary(false)
{
	m_SpatialRegion = 0;
	m_RegionFrames[0] = 0;
	m_RegionFrames[1] = 0;
	m_MediaTimeUnit = 1000;
	m_MediaTime = 0;

	m_NumberOfSegments = 0;
	//m_FrameRate = 25;

	m_SegmentStartFrames = 0;
	m_SegmentEndFrames = 0;
	m_SegmentBagOfWords = 0;

	m_FrameConfidences = 0;
	m_FrameWords = 0;
	m_FrameSignatures = 0;

	m_CompressionFlag = 0;
	
	m_KeyFrameInfo =0;
	SetExposedInterface(&m_Interface);

	m_nNumOfWords = 0;

	m_nKeyIndexContents = NULL;
	m_nAllIndexContents = NULL;
	m_unAllIndex = NULL;
	m_unKeyIndex = NULL;

	m_nNumOfKey = 0;
	m_pKeyLookUpTable = NULL;
}

//-----------------------------------------------------------------------------
VideoSignatureDescriptor::~VideoSignatureDescriptor()
{
	free(m_SegmentStartFrames);
	free(m_SegmentEndFrames);
	free(m_SegmentBagOfWords);

	free(m_FrameConfidences);
	free(m_FrameWords);
	free(m_FrameSignatures);

	if(m_SpatialRegion) free(m_SpatialRegion);
	if(m_MediaTime) free(m_MediaTime);


	if(m_KeyFrameInfo) free(m_KeyFrameInfo);

	int i=0;

	if(m_nKeyIndexContents != NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_nKeyIndexContents[i];
			m_nKeyIndexContents[i] = NULL;
		}
		m_nKeyIndexContents = NULL;
	}

	if(m_nAllIndexContents != NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_nAllIndexContents[i];
			m_nAllIndexContents[i] = NULL;
		}
		m_nAllIndexContents = NULL;
	}

	if(m_unAllIndex != NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_unAllIndex[i];
			m_unAllIndex[i] = NULL;
		}
		m_unAllIndex = NULL;
	}

	if(m_unKeyIndex != NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_unKeyIndex[i];
			m_unKeyIndex[i] = NULL;
		}
		m_unKeyIndex = NULL;
	}

	if(m_pKeyLookUpTable != NULL){
		delete[] m_pKeyLookUpTable;
		m_pKeyLookUpTable = NULL;
	}
}

//-----------------------------------------------------------------------------
const UUID& VideoSignatureDescriptor::GetObjectID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *VideoSignatureDescriptor::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
const UUID& VideoSignatureDescriptor::GetValueID(void)
{
  return valID;
}

//-----------------------------------------------------------------------------
const char* VideoSignatureDescriptor::GetValueName(void)
{
  return valName;
}

#define SEGMENT_BAG_SIZE_BITS 243
#define FRAME_SIGNATURE_SIZE_BITS (FRAME_SIGNATURE_SIZE*8)
#define N_D	380

//-----------------------------------------------------------------------------
unsigned long VideoSignatureDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{

	if (!aParentDescription)
		return (unsigned long)-1;

	int i, j, k;
	unsigned short *spatial_region;
	unsigned int *region_frame_info;
	unsigned short media_time_unit;
	unsigned int *media_time_info;
	vector <int> pix1(2);
	vector <int> pix2(2);

	int segment_media_time_flag = 1;	// MediaTimeFlagOfSegment
	int frame_media_time_flag = 1;		// MediaTimeFlagOfFrame

	//GenericDS NoF, NoS;
	GenericDS l_DDLDescription;
	GenericDS VSRegion;
	GenericDS VSSpatialRegion, Pixel1, Pixel2;
	GenericDS StartFrameOfSpatialRegion;
	GenericDS NumOfFrames;
	GenericDS MediaTimeUnit, MediaTime, StartMediaTime, EndMediaTime;

	l_DDLDescription = aParentDescription->CreateChild("Descriptor");
	l_DDLDescription.SetAttribute("xsi:type", "VideoSignatureType");

	VSRegion = l_DDLDescription.CreateChild("VideoSignatureRegion");

	spatial_region = GetSpatialRegion();
	if(spatial_region)
	{
		VSSpatialRegion = VSRegion.CreateChild("VideoSignatureSpatialRegion");	
		Pixel1 = VSSpatialRegion.CreateChild("Pixel");
		
		pix1[0] = spatial_region[0];
		pix1[1] = spatial_region[1];

		Pixel1.SetValue(pix1);

		Pixel2 = VSSpatialRegion.CreateChild("Pixel");
		
		pix2[0] = spatial_region[2];
		pix2[1] = spatial_region[3];

		Pixel2.SetValue(pix2);
	}
	
	region_frame_info = GetRegionFrames();
	StartFrameOfSpatialRegion = VSRegion.CreateChild("StartFrameOfSpatialRegion");
	StartFrameOfSpatialRegion.SetValue((int)region_frame_info[0]);
	//NumOfFrames = VSRegion.CreateChild("NumOfFrames");
	//NumOfFrames.SetValue((int)region_frame_info[1]);

	media_time_unit = GetMediaTimeUnit();
	MediaTimeUnit = VSRegion.CreateChild("MediaTimeUnit");
	MediaTimeUnit.SetValue(media_time_unit);

	media_time_info = GetMediaTime();
	if(media_time_info)
	{
		MediaTime = VSRegion.CreateChild("MediaTimeOfSpatialRegion");
		StartMediaTime = MediaTime.CreateChild("StartMediaTimeOfSpatialRegion");
		StartMediaTime.SetValue((int)media_time_info[0]);
		EndMediaTime = MediaTime.CreateChild("EndMediaTimeOfSpatialRegion");
		EndMediaTime.SetValue((int)media_time_info[1]);
	}
	
	int num_of_segments = GetNumberOfSegments();
	unsigned int *start_frames=GetSegmentStartFrames();
	unsigned int *end_frames=GetSegmentEndFrames();
	unsigned char *bag_of_words = GetSegmentBagOfWords();
	for(i=0; i< num_of_segments; i++)
	{
		GenericDS VideoSegment = VSRegion.CreateChild("VSVideoSegment");
		GenericDS StartFrame = VideoSegment.CreateChild("StartFrameOfSegment");
		StartFrame.SetValue((int)start_frames[i]);
		GenericDS EndFrame = VideoSegment.CreateChild("EndFrameOfSegment");
		EndFrame.SetValue((int)end_frames[i]);
		if(segment_media_time_flag)
		{
			GenericDS SegMediaTime = VideoSegment.CreateChild("MediaTimeOfSegment");
			GenericDS SegStartMediaTime = SegMediaTime.CreateChild("StartMediaTimeOfSegment");
			SegStartMediaTime.SetValue((int)GetMediaTimeOfFrame(start_frames[i]));
			GenericDS SegEndMediaTime = SegMediaTime.CreateChild("EndMediaTimeOfSegment");
			SegEndMediaTime.SetValue((int)GetMediaTimeOfFrame(end_frames[i]));
		}
		for(j=0; j < SEGMENT_WORD_COUNT; j++)
		{
			GenericDS BagOfWords = VideoSegment.CreateChild("BagOfWords");
			vector<int> v_bow(SEGMENT_BAG_SIZE_BITS);
			unsigned char* bagptr = bag_of_words + (SEGMENT_BAGS_SIZE*i+j*SIZE_OF_A_BAG);
			for ( k = 0; k < SEGMENT_BAG_SIZE_BITS; k++)
				v_bow[k] = (bagptr[(k>>3)] >> ( 7-(k & 0x7) ) ) & 0x1;
			BagOfWords.SetValue(v_bow);
		}

	}

	unsigned char *confidences=GetFrameConfidences();
	unsigned char *words=GetFrameWords();
	unsigned char *signatures=GetFrameSignatures();

	const CTernaryOps ternary_ops(UPPER_LIMIT, LOWER_LIMIT);

	for(i=0; i< region_frame_info[1]; i++)
	{
		GenericDS VideoFrame = VSRegion.CreateChild("VideoFrame");
		if(frame_media_time_flag)
		{
			GenericDS FrameMediaTime = VideoFrame.CreateChild("MediaTimeOfFrame");
			FrameMediaTime.SetValue((int)GetMediaTimeOfFrame(region_frame_info[0] + i));
		}
		GenericDS Confidence = VideoFrame.CreateChild("FrameConfidence");
		Confidence.SetValue(confidences[i]);

		GenericDS Words = VideoFrame.CreateChild("Word");
		vector<int> v_words(FRAME_WORD_COUNT);
		unsigned char* wordptr = words + (FRAME_WORD_COUNT*i);
		for ( k = 0; k < FRAME_WORD_COUNT; k++)
			v_words[k] = wordptr[k];
		Words.SetValue(v_words);

		GenericDS FrameSignature = VideoFrame.CreateChild("FrameSignature");
		vector<int> v_signature(N_D);
		unsigned char* sigptr = GetFrameSignatures() + (FRAME_SIGNATURE_SIZE*i);
		for ( k = 0; k < FRAME_SIGNATURE_SIZE; k++)
		{
			unsigned char PackedValue = sigptr[k];
			int v0,v1,v2,v3,v4;
			v0=PackedValue/81;
			PackedValue-=(81*v0);
			v1=PackedValue/27;
			PackedValue-=(27*v1);
			v2=PackedValue/9;
			PackedValue-=(9*v2);
			v3=PackedValue/3;
			PackedValue-=(3*v3);
			v4=PackedValue;

			v_signature[5*k]=v0;
			v_signature[5*k+1]=v1;
			v_signature[5*k+2]=v2;
			v_signature[5*k+3]=v3;
			v_signature[5*k+4]=v4;
		}

		FrameSignature.SetValue(v_signature);

	}
	
	return 0;

}

//-----------------------------------------------------------------------------
unsigned long VideoSignatureDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
	
	if (!aDescription)
		return (unsigned long)-1;
	
	GenericDS l_DDLDescription;
	GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
	
	string xsitype;
	if (aDescription->GetDSName() == "Descriptor")
	{
		aDescription->GetTextAttribute("xsi:type", xsitype);
		if (xsitype == "VideoSignatureType")
		{
			l_DDLDescriptionInterface = aDescription;
		}
	}
	
	if (!l_DDLDescriptionInterface)
	{
		l_DDLDescription = aDescription->GetDescription("Descriptor");
		
		while (!l_DDLDescription.isNull())
		{
			l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
			if (xsitype == "VideoSignatureType")
				break;
			l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
		}
		
		if (l_DDLDescription.isNull())
			return (unsigned long)-1;
		
		l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
	}


	unsigned int i, j, k;
	int spatial_region[4];
	int region_frame_info[2];
	int media_time_unit;
	int media_time_info[2];
	int num_of_frames = 0;
	int num_of_segments = 0;
	int start_frame, end_frame;
	unsigned char bag_of_words[SEGMENT_BAGS_SIZE];
	int confidence;
	unsigned char word[FRAME_WORD_COUNT];
	unsigned char signature[FRAME_SIGNATURE_SIZE];
	const CTernaryOps ternary_ops(UPPER_LIMIT, LOWER_LIMIT);


	GenericDS VSRegion = l_DDLDescriptionInterface->GetDescription("VideoSignatureRegion");
	if(VSRegion.isNull()) return (unsigned long)-1;

	
	GenericDS VSSpatialRegion = VSRegion.GetDescription("VideoSignatureSpatialRegion");
	if(!VSSpatialRegion.isNull()) 
	{
		vector<int> pix1;
		vector<int> pix2;
		GenericDS Pixel1 = VSSpatialRegion.GetDescription("Pixel");
		if(Pixel1.isNull()) return (unsigned long)-1;
		Pixel1.GetIntVector(pix1);

		GenericDS Pixel2 = Pixel1.GetNextSibling("Pixel"); 
		if(Pixel2.isNull()) return (unsigned long)-1;
		Pixel2.GetIntVector(pix2);

		spatial_region[0] = pix1[0];
		spatial_region[1] = pix1[1];
		spatial_region[2] = pix2[0];
		spatial_region[3] = pix2[1];

		SetSpatialRegion(spatial_region[0], spatial_region[1], spatial_region[2], spatial_region[3]);
	}
	
	
	GenericDS StartFrameOfSpatialRegion = VSRegion.GetDescription("StartFrameOfSpatialRegion");
	if(StartFrameOfSpatialRegion.isNull()) return (unsigned long)-1;
	StartFrameOfSpatialRegion.GetIntValue(region_frame_info[0]);
	//GenericDS NumOfFrames = VSRegion.GetDescription("NumOfFrames");
	//if(NumOfFrames.isNull()) return (unsigned long)-1;
	//NumOfFrames.GetIntValue(region_frame_info[1]);

	GenericDS MediaTimeUnit = VSRegion.GetDescription("MediaTimeUnit");
	if(MediaTimeUnit.isNull()) return (unsigned long)-1;
	MediaTimeUnit.GetIntValue(media_time_unit);
	SetMediaTimeUnit(media_time_unit);
	GenericDS MediaTime = VSRegion.GetDescription("MediaTimeOfSpatialRegion");
	if(!MediaTime.isNull())
	{
		GenericDS StartMediaTime = MediaTime.GetDescription("StartMediaTimeOfSpatialRegion");
		if(StartMediaTime.isNull()) return (unsigned long)-1;
		StartMediaTime.GetIntValue(media_time_info[0]);
		GenericDS EndMediaTime = MediaTime.GetDescription("EndMediaTimeOfSpatialRegion");
		if(EndMediaTime.isNull()) return (unsigned long)-1;
		EndMediaTime.GetIntValue(media_time_info[1]);
		SetMediaTime(media_time_info[0], media_time_info[1]);
	}


	
	GenericDS VideoSegment = VSRegion.GetDescription("VSVideoSegment");
	while(!VideoSegment.isNull())
	{
		num_of_segments++;
		VideoSegment = VideoSegment.GetNextSibling("VSVideoSegment");
	}
	SetNumberOfSegments(num_of_segments);
	VideoSegment = VSRegion.GetDescription("VSVideoSegment");
	for(i=0; i < num_of_segments; i++)
	{

		GenericDS StartFrame = VideoSegment.GetDescription("StartFrameOfSegment");
		if(StartFrame.isNull()) return (unsigned long)-1;
		StartFrame.GetIntValue(start_frame);

		GenericDS EndFrame = VideoSegment.GetDescription("EndFrameOfSegment");
		if(EndFrame.isNull()) return (unsigned long)-1;
		EndFrame.GetIntValue(end_frame);

		memset(bag_of_words, 0, sizeof(bag_of_words));
		GenericDS BagOfWords = VideoSegment.GetDescription("BagOfWords");
		for(j=0; j < SEGMENT_WORD_COUNT; j++)
		{
			if(BagOfWords.isNull()) return (unsigned long)-1;
			vector<int> v_bow;
			BagOfWords.GetIntVector(v_bow);
			for (k = 0; k < SEGMENT_BAG_SIZE_BITS; k++)
				bag_of_words[j*SIZE_OF_A_BAG + (k>>3)] |= (v_bow[k] << (7-(k&7)));
			BagOfWords=BagOfWords.GetNextSibling("BagOfWords");
		}
		SetSegment(i, start_frame, end_frame, bag_of_words);
		VideoSegment = VideoSegment.GetNextSibling("VSVideoSegment");
	}
	
	num_of_frames = 0;
	GenericDS VideoFrame = VSRegion.GetDescription("VideoFrame");
	while(!VideoFrame.isNull())
	{
		num_of_frames++;
		VideoFrame = VideoFrame.GetNextSibling("VideoFrame");
	}
	region_frame_info[1] = num_of_frames;
	SetRegionFrames(region_frame_info[0], region_frame_info[1]);

	VideoFrame = VSRegion.GetDescription("VideoFrame");
	for(i=0; i < num_of_frames; i++)
	{
		if(VideoFrame.isNull()) return (unsigned long)-1;
		GenericDS Confidence = VideoFrame.GetDescription("FrameConfidence");
		if(Confidence.isNull()) return (unsigned long)-1;
		Confidence.GetIntValue(confidence);

		memset(word, 0, sizeof(word));
		GenericDS Word = VideoFrame.GetDescription("Word");
		if(Word.isNull())  return (unsigned long)-1;
		vector<int> v_word;
		Word.GetIntVector(v_word);
		for(j=0; j < FRAME_WORD_COUNT; j++)
		{
			word[j] = v_word[j];
		}

		memset(signature, 0, sizeof(signature));
		GenericDS FrameSignature = VideoFrame.GetDescription("FrameSignature");

		if(FrameSignature.isNull()) return (unsigned long)-1;
		vector<int> v_signature;
		FrameSignature.GetIntVector(v_signature);
		unsigned char *TernarySignature=new unsigned char[5];
		for (k = 0; k < FRAME_SIGNATURE_SIZE; k++)
		{
			TernarySignature[0] =  (unsigned char)v_signature[5*k];
			TernarySignature[1] =  (unsigned char)v_signature[5*k+1];
			TernarySignature[2] =  (unsigned char)v_signature[5*k+2];
			TernarySignature[3] =  (unsigned char)v_signature[5*k+3];
			TernarySignature[4] =  (unsigned char)v_signature[5*k+4];

			unsigned char PackedSignature = (unsigned char)ternary_ops.Pack5TernaryValuesToA32bitInt(TernarySignature);

			signature[k]=PackedSignature;
		}
		delete [] TernarySignature;


		SetFrame(i, (unsigned char)confidence, word, signature);
		VideoFrame = VideoFrame.GetNextSibling("VideoFrame");
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
bool VideoSignatureDescriptor::IsProprietary(void)
{
  return m_isProprietary;
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::addref(void)
{
#ifdef VERBOSE
  fprintf(stderr, "VideoSignature connect\n");
#endif // VERBOSE
  m_refcount++;
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::release(void)
{
#ifdef VERBOSE
  fprintf(stderr, "VideoSignature release\n");
#endif // VERBOSE
  if (!(--m_refcount)) delete this;
}

//-----------------------------------------------------------------------------
VideoSignatureDescriptorInterfaceABC *VideoSignatureDescriptor::GetInterface(void)
{
  return &m_Interface;
}



//-----------------------------------------------------------------------------
int VideoSignatureDescriptor::SetRegionFrames(unsigned int start_frame, unsigned int nof)

{
	m_RegionFrames[0] = start_frame;
	m_RegionFrames[1] = nof;

	if(nof > 0)
	{
		m_FrameConfidences = (unsigned char*)malloc(nof*sizeof(unsigned char));
		m_FrameWords = (unsigned char*)malloc(nof*FRAME_WORD_COUNT*sizeof(unsigned char));
		m_FrameSignatures = (unsigned char*)malloc(nof*FRAME_SIGNATURE_SIZE*sizeof(unsigned char));
		if(!m_FrameConfidences || !m_FrameWords || !m_FrameSignatures) return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetSpatialRegion(unsigned short top_left_x, unsigned short top_left_y,
					unsigned short bottom_right_x, unsigned short bottom_right_y)
{
	if(!m_SpatialRegion)
	{
		m_SpatialRegion = (unsigned short*)malloc(4*sizeof(unsigned short));
		if(!m_SpatialRegion) return;
	}
	m_SpatialRegion[0] = top_left_x;
	m_SpatialRegion[1] = top_left_y;
	m_SpatialRegion[2] = bottom_right_x;
	m_SpatialRegion[3] = bottom_right_y;
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetMediaTimeUnit(unsigned int time_unit)
{
	m_MediaTimeUnit = time_unit;
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetMediaTime(unsigned int start_time, unsigned int end_time)
{
	if(!m_MediaTime)
	{
		m_MediaTime = (unsigned int*)malloc(2*sizeof(unsigned int));
		if(!m_MediaTime) return;
	}
	m_MediaTime[0] = start_time;
	m_MediaTime[1] = end_time;
}

//-----------------------------------------------------------------------------
int VideoSignatureDescriptor::SetNumberOfSegments(unsigned int nos)
{
	m_NumberOfSegments = nos;

	if(nos > 0)
	{
		m_SegmentStartFrames = (unsigned int*)malloc(nos*sizeof(unsigned int));
		if(!m_SegmentStartFrames) return -1;
		m_SegmentEndFrames = (unsigned int*)malloc(nos*sizeof(unsigned int));
		if(!m_SegmentEndFrames) return -1;
		m_SegmentBagOfWords = (unsigned char*)malloc(nos*SEGMENT_BAGS_SIZE*sizeof(unsigned char));
	}
	return 0;

}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetSegment(unsigned int pos, unsigned int start_frame, unsigned int end_frame,
					const unsigned char *bag_of_words)
{
	if(m_NumberOfSegments >0 && m_NumberOfSegments > pos)
	{
		m_SegmentStartFrames[pos] = start_frame;
		m_SegmentEndFrames[pos] = end_frame;
		memcpy(m_SegmentBagOfWords + (SEGMENT_BAGS_SIZE*pos), bag_of_words, SEGMENT_BAGS_SIZE);
	}
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetFrame(unsigned int pos, unsigned char confidence, const unsigned char *words,
					const unsigned char *frame_signature)
{
	if(pos >= 0 &&  pos < m_RegionFrames[1])
	{
		if(words)
		{
			m_FrameConfidences[pos] = confidence;
			memcpy(m_FrameWords + (FRAME_WORD_COUNT*pos), words, FRAME_WORD_COUNT);
		}

		if(frame_signature)
		{
			memcpy(m_FrameSignatures + (FRAME_SIGNATURE_SIZE*pos), frame_signature, FRAME_SIGNATURE_SIZE);
		}
	}
}

//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetCompressionFlag(int flag)
{
	m_CompressionFlag = flag;
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptor::GetRegionFrames()
{
	return m_RegionFrames;
}

//-----------------------------------------------------------------------------
unsigned short* VideoSignatureDescriptor::GetSpatialRegion()
{
	return m_SpatialRegion;
}

//-----------------------------------------------------------------------------
unsigned short VideoSignatureDescriptor::GetMediaTimeUnit()
{
	return m_MediaTimeUnit;
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptor::GetMediaTime()
{
	return m_MediaTime;
}

//-----------------------------------------------------------------------------
double VideoSignatureDescriptor::GetRegionFrameRate()
{
	double time=0;
	double nof_frames = m_RegionFrames[1];

	// Media time is optional and is used to obtain the frame rate which is needed
	// for the current (reference) detection function of video duplicates.
	// Set any frame rate value if media time is absent or invalid.
	if(m_MediaTime)
	{
		time = m_MediaTime[1] - m_MediaTime[0];
	}
	if(time <=0) return 25.0;

	return (nof_frames*m_MediaTimeUnit/time);
}

//-----------------------------------------------------------------------------
unsigned int VideoSignatureDescriptor::GetMediaTimeOfFrame(unsigned int frame)
{
	double frame_rate = GetRegionFrameRate();
	return (unsigned int)(m_MediaTimeUnit * frame / frame_rate);

}

//-----------------------------------------------------------------------------
unsigned int VideoSignatureDescriptor::GetNumberOfSegments()
{
	return m_NumberOfSegments;
}


//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptor::GetSegmentStartFrames()
{
	return m_SegmentStartFrames;
}

//-----------------------------------------------------------------------------
unsigned int* VideoSignatureDescriptor::GetSegmentEndFrames()
{
	return m_SegmentEndFrames;
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptor::GetSegmentBagOfWords()
{
	return m_SegmentBagOfWords;
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptor::GetFrameConfidences()
{
	return m_FrameConfidences;
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptor::GetFrameWords()
{
	return m_FrameWords;
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptor::GetFrameSignatures()
{
	return m_FrameSignatures;
}

//-----------------------------------------------------------------------------
int VideoSignatureDescriptor::GetCompressionFlag()
{
	return m_CompressionFlag;
}

//-----------------------------------------------------------------------------
unsigned char* VideoSignatureDescriptor::GetKeyFrameInfo()
{
	return m_KeyFrameInfo;
}

int VideoSignatureDescriptor::GetNumOfWords()
{
	return m_nNumOfWords;
}

unsigned int* VideoSignatureDescriptor::GetKeyLookUpTable()
{
	return m_pKeyLookUpTable;;
}

int** VideoSignatureDescriptor::GetNumOfKeyIndex()
{
	return m_nKeyIndexContents;
}

int** VideoSignatureDescriptor::GetNumOfAllIndex()
{
	return m_nAllIndexContents;
}

unsigned int** VideoSignatureDescriptor::GetKeyIndexTable()
{
	return m_unKeyIndex;
}

unsigned int** VideoSignatureDescriptor::GetAllIndexTable()
{
	return m_unAllIndex;
}

int VideoSignatureDescriptor::GetKeyFrameInfoFlag()
{
	return this->m_KeyFrameFlag;
}


//-----------------------------------------------------------------------------
void VideoSignatureDescriptor::SetKeyFrameInfo(unsigned char* keyframeinfo, int numofframe)
{
	if(m_KeyFrameInfo) free(m_KeyFrameInfo);
	m_KeyFrameInfo = new unsigned char[numofframe];
	memcpy(m_KeyFrameInfo,keyframeinfo,numofframe);
	
}

void VideoSignatureDescriptor::SetKeyLookUpTable(unsigned int numberofkey, unsigned int* KeyLookUpTable)
{
	m_nNumOfKey = numberofkey;
	if(m_pKeyLookUpTable != NULL){
		delete[] m_pKeyLookUpTable;
		m_pKeyLookUpTable = NULL;
	}
	m_pKeyLookUpTable = new unsigned int[m_nNumOfKey];
	memcpy(m_pKeyLookUpTable,KeyLookUpTable,m_nNumOfKey*sizeof(unsigned int));
}

void VideoSignatureDescriptor::SetKeyIndexTable(unsigned char numofwords, int** nKeyIndexContent, unsigned int** KeyIndex)
{
	m_nNumOfWords = numofwords;
	int i;
	if(m_nKeyIndexContents!=NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_nKeyIndexContents[i];
			m_nKeyIndexContents[i] = NULL;
		}
		delete[] m_nKeyIndexContents;
		m_nKeyIndexContents = NULL;
	}

	m_nKeyIndexContents = new int* [m_nNumOfWords];
	for(i=0;i<m_nNumOfWords;i++){
		m_nKeyIndexContents[i] = new int [244];
		memcpy(m_nKeyIndexContents[i],nKeyIndexContent[i],244*sizeof(int));
	}

	if(m_unKeyIndex!=NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_unKeyIndex[i];
			m_unKeyIndex[i] = NULL;
		}
		m_unKeyIndex = NULL;		
	}
	m_unKeyIndex = new unsigned int* [m_nNumOfWords];
	for(i=0;i<m_nNumOfWords;i++){
		m_unKeyIndex[i] = new unsigned int [m_nKeyIndexContents[i][243]];
		memcpy(m_unKeyIndex[i],KeyIndex[i],m_nKeyIndexContents[i][243]*sizeof(unsigned int));
	}

//	for(i=0;i<m_nNumOfWords;i++){
//		memcpy(m_nKeyIndexContents[i],nKeyIndexContent[i],243*sizeof(int));
//		memcpy(m_unKeyIndex[i],KeyIndex[i],m_nKeyIndexContents[i][243]*sizeof(unsigned int));
//	}
}

void VideoSignatureDescriptor::SetAllIndexTable(unsigned char numofwords, int** nAllIndexContent, unsigned int** AllIndex)
{
	m_nNumOfWords = numofwords;
	int i;
	if(m_nAllIndexContents!=NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_nAllIndexContents[i];
			m_nAllIndexContents[i] = NULL;
		}
		delete[] m_nAllIndexContents;
		m_nAllIndexContents = NULL;
		
	}
	m_nAllIndexContents = new int* [m_nNumOfWords];
	for(i=0;i<m_nNumOfWords;i++){
		m_nAllIndexContents[i] = new int [244];
		memcpy(m_nAllIndexContents[i],nAllIndexContent[i],244*sizeof(int));
	}

	if(m_unAllIndex!=NULL){
		for(i=0;i<m_nNumOfWords;i++){
			delete[] m_unAllIndex[i];
			m_unAllIndex[i]= NULL;
		}
		delete[] m_unAllIndex;
		m_unAllIndex = NULL;
	}
	m_unAllIndex = new unsigned int* [m_nNumOfWords];
	for(i=0;i<m_nNumOfWords;i++){
		m_unAllIndex[i] = new unsigned int [m_nAllIndexContents[i][243]];
		memcpy(m_unAllIndex[i],AllIndex[i],m_nAllIndexContents[i][243]*sizeof(unsigned int));
	}

//	for(i=0;i<m_nNumOfWords;i++){
//		memcpy(m_nAllIndexContents[i],nAllIndexContent[i],243*sizeof(int));
//		memcpy(m_unAllIndex[i],AllIndex[i],m_nAllIndexContents[i][243]*sizeof(unsigned int));
//	}
}

//this function set the keyframe infoflag. if the key frame information is exist, set to 'true' otherwise set to 'false'
void VideoSignatureDescriptor::SetKeyFrameInfoFlag(int flag)
{
	m_KeyFrameFlag = flag;
}