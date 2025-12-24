///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// P. Brasnett
// Mitsubishi Electric R&D Centre Europe
// K. Iwamoto, R. Oami
// NEC Corp.
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
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignatureExtraction.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <cv.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
#include <algorithm>

#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"

//#include <assert.h>
enum E_M { EL_D, EL_A };
enum TH_M { TH_A, TH_F };
unsigned int g_aoi[25] = {210,217,219,274,334,44,175,233,270,273,57,70,103,237,269,100,285,295,337,354,101,102,111,275,296 };
#define N_D	380
#define N_T	10


#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define GetGray(i,x,y)	((uchar*)((i)->imageData + (i)->widthStep*(y)))[(x)]		// 8bit

static const int g_verbose = 1;


typedef struct DIMENSION {
	enum E_M elM;
	enum TH_M thM;

	double	thV;			
	short	thIndex;			
	short	type;			

	std::vector<int> aaaaIndex;
	std::vector<int> bbbbIndex;

} DIMENSION;



static const int Thd[3][3] = {{0,1,1},{1,0,2},{1,2,0}};
static const unsigned char NaN = 0xFF;

class INDEX 
{
public:
	unsigned char* word;
	int framenum;

	INDEX(const unsigned char *p_word, unsigned int word_count, const int position)
	{
		word = new unsigned char [word_count];
		memcpy(word,p_word,word_count);
		framenum = position;
	}

	~INDEX()
	{
		delete [] word;
	}
};

typedef struct tagRawFrameSignature
{
	unsigned char framedescriptor[FRAME_SIGNATURE_SIZE];
	unsigned char frameword[FRAME_WORD_COUNT];
	unsigned char confidence;
	int bScn;
	unsigned int FrameNum;
	float reserve;
	int reserve1;
}RawFrameSignature;

class FrameSignature
{
public:
	unsigned char	_confidence;
	unsigned char *	_p_words;
	unsigned char *	_p_signatures;

	bool _valid;

	unsigned int _word_count;
	unsigned int _size_of_signature;

	const CTernaryOps & _ternary_ops;
	
	FrameSignature(const unsigned char *p_signature_array, unsigned int size_of_signature, unsigned int word_count,unsigned int * p_array_of_indices,const CTernaryOps & ternary_ops):
		_word_count(word_count),
		_size_of_signature(size_of_signature),
		_ternary_ops(ternary_ops)

	{
		_p_words = new unsigned char [word_count];

		_p_signatures = new unsigned char [size_of_signature];

		for(unsigned int i=0;i<size_of_signature;++i)
		{
			_p_signatures[i] = p_signature_array[i];
		}

		calculate_words(word_count, p_array_of_indices);

		_valid = true;
	}

	FrameSignature(FILE *fp,unsigned int size_of_signature,unsigned int word_count,const CTernaryOps & ternary_ops):
	_size_of_signature(size_of_signature),
		_word_count(word_count),
		_ternary_ops(ternary_ops)
	{
		_p_words = new unsigned char [word_count];

		_p_signatures = new unsigned char [size_of_signature];

		size_t retval = fread(&_confidence, sizeof(unsigned char), 1, fp);

		retval &= fread(_p_words, sizeof(unsigned char) * _word_count, 1, fp);

		retval &= fread(_p_signatures, sizeof(unsigned char) * _size_of_signature, 1, fp);

		if(retval!= 1)
			_valid = false;
		else
			_valid = true;
	}

	~FrameSignature()
	{
		delete [] _p_words;
		delete [] _p_signatures;
	}


	void PopulateHistogramOfWords(unsigned int * p_histogram_of_words) const
	{
		unsigned int h;
		for(h=0;h<_word_count;++h)
		{
			unsigned char word = _p_words[h];

			p_histogram_of_words[ h*243 + word ]++;
		}
	}

	const unsigned char * GetSignatureData() const
	{
		return _p_signatures;
	}

private:

	unsigned char get_ternary_value_from_packed_binary( unsigned int idx, unsigned int packed_ternary_value ) const
	{
		int v;
		if(idx>=0)
		{
			v=packed_ternary_value/81;
			packed_ternary_value-=(81*v);
		}
		if(idx>=1)
		{
			v=packed_ternary_value/27;
			packed_ternary_value-=(27*v);
		}
		if(idx>=2)
		{
			v=packed_ternary_value/9;
			packed_ternary_value-=(9*v);
		}
		if(idx>=3)
		{
			v=packed_ternary_value/3;
			packed_ternary_value-=(3*v);
		}
		if(idx>=4)
		{
			v=packed_ternary_value;
			packed_ternary_value-=v;
		}
		return (unsigned char)v;
	}

	unsigned char get_ternary_value_by_index(unsigned int index,unsigned char * p_BET_data)const
	{
		unsigned int index_to_the_byte = index / 5;

		unsigned char BET = p_BET_data[index_to_the_byte];

		unsigned int packed_ternary_value = _ternary_ops.DecodeToPackedTernaryValue(BET);

		unsigned int index_to_ternary_value = index - (index_to_the_byte * 5);

		return get_ternary_value_from_packed_binary( index_to_ternary_value, packed_ternary_value );
	}

	

	void calculate_words(unsigned int word_count,unsigned int *	p_array_of_indices)
	{
		for(unsigned int i=0;i<word_count;++i)
		{
			unsigned char ternary_values[5];
			for(int j=0;j<5;++j)
			{
				ternary_values[j] = get_ternary_value_by_index(p_array_of_indices[j + i * 5], _p_signatures);
			}

			unsigned int packed_ternary_value = _ternary_ops.Pack5TernaryValuesToA32bitInt(ternary_values);

			_p_words[i] = _ternary_ops.EncodeTo8BitBinary(packed_ternary_value);

		}
	}
};

class CBitBucket
{
private:
	unsigned int	_bit_count;
	unsigned int	_byte_array_size;
	std::vector<unsigned int>	_bit_vector;

public:
	CBitBucket(const unsigned int size):_bit_count(size),_bit_vector(std::vector<unsigned int>(size,0))
	{
		int n = size/8;

		if(size%8 !=0)
			n++;

		_byte_array_size = n;
	}

	const unsigned int GetSetBitCount()
	{
		unsigned int i;
		int count = 0;

		for(i=0;i<_bit_vector.size();++i)
		{
			if(_bit_vector[i]==1)
				count++;
		}

		return count;
	}

	const unsigned int GetBitCount()const 	{return _bit_count;}

	unsigned int GetBit(const unsigned int index)const
	{
		return _bit_vector[index];
	}

	void SetBit(const unsigned int index)
	{
		_bit_vector[index] = 1;
	}

	void ClearBit(const unsigned int index)
	{
		_bit_vector[index] = 0;
	}

	CBitBucket * Intersection(const CBitBucket & p_right)const
	{
		unsigned int i;
		CBitBucket * p_retval = new CBitBucket(_bit_count);

		for(i=0;i<_bit_count;++i)
		{
			unsigned int val = _bit_vector[i] & p_right.GetBit(i);

			if(val==0)
				p_retval->ClearBit(i);
			else
				p_retval->SetBit(i);
		}

		return p_retval;
	}

	CBitBucket * Union(const CBitBucket & p_right)const
	{
		CBitBucket * p_retval = new CBitBucket(_bit_count);

		for(unsigned int i=0;i<_bit_count;++i)
		{
			unsigned int val = _bit_vector[i] | p_right.GetBit(i);

			if(val==0)
				p_retval->ClearBit(i);
			else
				p_retval->SetBit(i);
		}

		return p_retval;
	}


	void Copy(const CBitBucket & rhs)
	{
		unsigned int i;
		unsigned max_count = _bit_count;
		if(max_count > rhs.GetBitCount())
			max_count = rhs.GetBitCount();

		for(i=0;i<max_count;++i)
			_bit_vector[i] = rhs.GetBit(i);

		_bit_count = max_count;

		int n = max_count/8;

		if(max_count%8 !=0)
			n++;

		_byte_array_size = n;
	}

	void get_byte_array(unsigned char * p_byte_array)
	{
		unsigned int k;
		unsigned int seg_sig = 0;
		unsigned int index_to_the_byte = 0;

		for(k= 0;k<_bit_count;++k)
		{
			if(k%8 ==0)
			{
				p_byte_array[index_to_the_byte] =seg_sig; 
				seg_sig = 0;
			}

			index_to_the_byte = k / 8;

			const unsigned int index_to_the_bit = k - (index_to_the_byte * 8);

			if(_bit_vector[k]==1)
			{
				seg_sig |= (0x1 << (7-index_to_the_bit));
			}
		}

		p_byte_array[index_to_the_byte] =seg_sig;
	}

private:

	void set_byte_value(const unsigned int byte_index,const unsigned char value)
	{
		int k;
		for(k=0;k<8;++k)
		{
			if( ((0x1 << k) & value) >0 )
			{
				_bit_vector[byte_index * 8  + k] = 1;
			}
		}
	}
};

struct SegmentSignature
{
	unsigned int	_start_frame_index;	
	unsigned int	_end_frame_index;

	std::vector<CBitBucket *> _bit_buckets;

	const unsigned int _word_count;

	SegmentSignature(const unsigned int word_count):_word_count(word_count)
	{
		unsigned int i;
		for(i=0;i<word_count;++i)
		{
			CBitBucket * p_bit_bucket = new CBitBucket(SIZE_OF_A_BAG *  8);

			_bit_buckets.push_back(p_bit_bucket);
		}
	}

	~SegmentSignature()
	{
		unsigned int i;
		for(i=0;i<_word_count;++i)
		{
			CBitBucket * p_bit_bucket =_bit_buckets[i];

			delete p_bit_bucket;
		}
	}


	void Create(const unsigned int * p_histogram_of_words)
	{
		unsigned int seg_sig = 0;
		unsigned int index_to_the_byte = 0;

		for(unsigned int word_counter=0;word_counter<_word_count;++word_counter)
		{
			for(int k=0;k<243;++k)
			{
				if(p_histogram_of_words[ word_counter*243 +k]>0)
					_bit_buckets[word_counter]->SetBit(k);
			}
		}
	}

	void CopyWordOfBags(unsigned char *data)
	{
		unsigned int i;
		for(i=0;i<_word_count;++i)
		{
			_bit_buckets[i]->get_byte_array(data+i*SIZE_OF_A_BAG);
		}
	}
};


void clear_buffer(int * p_buffer,const int val=0)
{
	memset(p_buffer,val,sizeof(int) * 243 * SEGMENT_WORD_COUNT);
}

void populate_histogram_of_words( std::vector<FrameSignature *> _frame_signatures,  const unsigned int frame_counter_offset, unsigned int _S_S, unsigned int * p_histogram_of_words)
{
	for(unsigned int j=0;j<_S_S;++j)
	{
		FrameSignature * p_frame_sig = _frame_signatures[frame_counter_offset+j]; 

		p_frame_sig->PopulateHistogramOfWords(p_histogram_of_words);
	}
}

SegmentSignature * create_segment_signature( std::vector<FrameSignature *> _frame_signatures, const unsigned int frame_counter, unsigned int _S_S, unsigned int * p_histogram_of_words)
{
	SegmentSignature * p_segment_sig = new SegmentSignature(SEGMENT_WORD_COUNT);
	p_segment_sig->_start_frame_index = frame_counter;
	p_segment_sig->_end_frame_index   = frame_counter + _S_S - 1;
	p_segment_sig->Create(p_histogram_of_words);
	return p_segment_sig; 
}



using namespace XM;


//=============================================================================

const UUID VideoSignatureExtractionInterface::myID = UUID();
const char *VideoSignatureExtractionInterface::myName = "Streaming Component Control Interface";

const UUID VideoSignatureExtractionTool::myID = UUID();
const char *VideoSignatureExtractionTool::myName = "Video Signature Descriptor Extractor";

const UUID VideoSignatureExtractionInterfaceABC::myID = UUID();

//=============================================================================
VideoSignatureExtractionInterfaceABC::VideoSignatureExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
VideoSignatureExtractionInterface::VideoSignatureExtractionInterface(VideoSignatureExtractionTool* aTool)
: m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
VideoSignatureExtractionInterface::~VideoSignatureExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& VideoSignatureExtractionInterface::GetInterfaceID(void)
{
	return myID;
}

//----------------------------------------------------------------------------
const char *VideoSignatureExtractionInterface::GetName(void)
{
	return myName;
}

//----------------------------------------------------------------------------
void VideoSignatureExtractionInterface::destroy(void)
{
	delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int VideoSignatureExtractionInterface::
SetSourceMedia(MultiMediaInterfaceABC *media)
{
	return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
VideoSignatureDescriptorInterfaceABC *VideoSignatureExtractionInterface::
GetDescriptorInterface(void)

{
	return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int VideoSignatureExtractionInterface::
SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
					   *aVideoSignatureDescriptorInterface)
					   
{
	return m_ExtractionTool->SetDescriptorInterface(aVideoSignatureDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureExtractionInterface::InitExtracting(void)
{
	return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureExtractionInterface::StartExtracting(void)
{
	return m_ExtractionTool->StartExtracting();
}


//=============================================================================
VideoSignatureExtractionTool::VideoSignatureExtractionTool()
: m_Interface(this)
, m_DescriptorInterface(0)
, m_Media(0)
, m_ImageMedia(0)  
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
VideoSignatureExtractionTool::
VideoSignatureExtractionTool(VideoSignatureDescriptorInterfaceABC
							 *videosignature)
							 : m_Interface(this)
							 , m_DescriptorInterface(0)
							 , m_Media(0)
							 , m_ImageMedia(0)  
{
#ifdef VERBOSE
	fprintf(stderr, "Connect descriptor\n");
#endif
	
	if (!videosignature)
	{
		VideoSignatureDescriptor *descriptor = new VideoSignatureDescriptor();
		videosignature = descriptor->GetInterface();
	}
	
	SetDescriptorInterface(videosignature);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
VideoSignatureExtractionTool::~VideoSignatureExtractionTool()
{
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool VideoSignatureExtractionTool::SupportsPush(void)
{
	return true;
}

//----------------------------------------------------------------------------
bool VideoSignatureExtractionTool::SupportsPull(void)
{
	return false;
}

//----------------------------------------------------------------------------
int VideoSignatureExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
	if (!media) return -1;
	
	m_Media=media;
	return 0;
}


//----------------------------------------------------------------------------
VideoSignatureDescriptorInterfaceABC *VideoSignatureExtractionTool::
GetDescriptorInterface(void)
{
	return m_DescriptorInterface;
}


//----------------------------------------------------------------------------
int VideoSignatureExtractionTool::
SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
					   *aVideoSignatureDescriptorInterface)
					   
{
	if (m_DescriptorInterface ==
		aVideoSignatureDescriptorInterface) return 0;
	
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();
	
	m_DescriptorInterface = aVideoSignatureDescriptorInterface;
	if (m_DescriptorInterface)
	{
		m_DescriptorInterface->addref();
	}
	else
	{
		return -1;
	}
	
	return 0;
}


//----------------------------------------------------------------------------
unsigned long VideoSignatureExtractionTool::InitExtracting(void)
{
	return 0UL;
}

static int count=-1;

//----------------------------------------------------------------------------
unsigned long VideoSignatureExtractionTool::StartExtracting(void)
{	
	if (!m_DescriptorInterface)
		return (unsigned long)-1;
	
	if (strcmp(m_DescriptorInterface->GetName(),
		"Video Signature Description Interface") != 0)
		return (unsigned long)-1;
	
	if (m_Media==NULL) return (unsigned long)-1;

	int i;
	int num_of_signatures = 0;
	int num_of_segments = 0;
	int process_frame_counter = 0;
	int frame_width, frame_height;
	//IplImage *p_gray_image = NULL;
	unsigned char signature[FRAME_SIGNATURE_SIZE];
	unsigned char confidence;

	vector<SegmentSignature *>	_segment_signatures;
	vector<FrameSignature*>	_frame_signatures;

	int key_info_flag = 0;
	key_info_flag = m_DescriptorInterface->GetKeyFrameInfoFlag();
	vector<RawFrameSignature*>	_raw_frame_signatures;


	const CTernaryOps ternary_ops(UPPER_LIMIT, LOWER_LIMIT);
	vector<DIMENSION> dimensionList;
	if(initDimensionList(dimensionList)!=0) return (unsigned long)-1;

	if(g_verbose)
	{
		printf("processing %s\n", m_Media->GetVideoFileName());
	}
	count=-1;
	while(1)
	{
		m_ImageMedia=m_Media->LoadNextVideoFrame();

		
		if (m_ImageMedia==NULL) break;

		frame_width = m_ImageMedia->width;
		frame_height = m_ImageMedia->height;
		unsigned char *imgData = m_ImageMedia->y_chan->data->u;
		/*p_gray_image = processFrame(m_ImageMedia, process_frame_counter);

		

		if(p_gray_image != NULL)				
		{
			unsigned char *imgData = new unsigned char [p_gray_image->width*p_gray_image->height];

			for( int j = 0; j < p_gray_image->height; j++ )
			{
				for( int i = 0; i < p_gray_image->width; i++ )
				{
					imgData[j*p_gray_image->width+i] = GetGray( p_gray_image, i, j );
				}
			}
*/


			ExtractSignature( imgData, frame_width, frame_height, dimensionList, signature, confidence, ternary_ops );

			
			FrameSignature * p_frame_sig = new FrameSignature(signature, FRAME_SIGNATURE_SIZE, FRAME_WORD_COUNT,
											g_aoi, ternary_ops);
		
			p_frame_sig->_confidence = confidence;
				
			_frame_signatures.push_back(p_frame_sig);

			num_of_signatures++;
					
			//delete [] imgData;

			//cvReleaseImage(&p_gray_image);
		//}
		//else
		//{
		//	break;
		//}

	}


	unsigned int _S_S = S_S;

	unsigned int * p_histogram_of_words = new unsigned int [243 * SEGMENT_WORD_COUNT];

	unsigned int frame_counter = 0;
	unsigned int end_media_time;

	// 2010.02: modified for conformity with "compressed segment"
	//for(frame_counter=0;(frame_counter+_S_S)<_frame_signatures.size();frame_counter+=S_0)
	for(frame_counter=0;frame_counter<_frame_signatures.size();frame_counter+=S_0)
	{
		_S_S = S_S;
		if((frame_counter + _S_S) > _frame_signatures.size())
			_S_S = _frame_signatures.size() - frame_counter;

		clear_buffer((int*)p_histogram_of_words);
		populate_histogram_of_words(_frame_signatures, frame_counter, _S_S, p_histogram_of_words);
		SegmentSignature * p_segment_sig  = create_segment_signature(_frame_signatures, frame_counter, _S_S, p_histogram_of_words);
		_segment_signatures.push_back(p_segment_sig);
		num_of_segments++;
	}

	do
	{
		if(m_DescriptorInterface->SetRegionFrames(0, num_of_signatures)!=0) break;
		if(m_DescriptorInterface->SetNumberOfSegments(num_of_segments)!=0) break;
		m_DescriptorInterface->SetMediaTimeUnit(1000);
		end_media_time = (unsigned int)(1000*num_of_signatures/m_Media->GetVideoFrameRate());
		m_DescriptorInterface->SetMediaTime(0, end_media_time);
		//m_DescriptorInterface->SetFrameRate(m_Media->GetVideoFrameRate());
		m_DescriptorInterface->SetSpatialRegion(0, 0, (frame_width-1), (frame_height-1));

		i=0;
		unsigned char bags_of_words[SEGMENT_BAGS_SIZE];
		std::vector<SegmentSignature *>::const_iterator it1;
		for(it1 = _segment_signatures.begin();it1!=_segment_signatures.end();++it1)
		{
			SegmentSignature * p_segment_sig = *it1;
			p_segment_sig->CopyWordOfBags(bags_of_words);
			m_DescriptorInterface->SetSegment(i, p_segment_sig->_start_frame_index, p_segment_sig->_end_frame_index, bags_of_words);
			i++;
			delete p_segment_sig;
		}

		i=0;
		std::vector<FrameSignature *>::const_iterator it2;
		for(it2 = _frame_signatures.begin();it2!=_frame_signatures.end();++it2)
		{
			FrameSignature * p_frame_sig = *it2;
			m_DescriptorInterface->SetFrame(i, p_frame_sig->_confidence, p_frame_sig->_p_words, p_frame_sig->GetSignatureData());

			if(key_info_flag){
				RawFrameSignature* p_raw_frame_sig = new RawFrameSignature;
				p_raw_frame_sig->confidence = p_frame_sig->_confidence;
				memcpy(p_raw_frame_sig->framedescriptor,p_frame_sig->GetSignatureData(),FRAME_SIGNATURE_SIZE);
				memcpy(p_raw_frame_sig->frameword,p_frame_sig->_p_words,FRAME_WORD_COUNT);
				p_raw_frame_sig->FrameNum = i;
				p_raw_frame_sig->reserve = 0.0;
				p_raw_frame_sig->reserve1=0;

				_raw_frame_signatures.push_back(p_raw_frame_sig);
			}

			i++;
			delete p_frame_sig;
		}
	}
	while(false);

	if(key_info_flag){
		vector<INDEX*> KeyFrameIndex;
		vector<INDEX*> AllFrameIndex;

		RawFrameSignature * p_Pre_frame_sig;
		setLUT_L1_DIST();
		std::vector<RawFrameSignature *>::const_iterator it2;

		unsigned int region_frame_info[2];
		unsigned short media_time_unit = 1000;
		unsigned int media_time_info[2];

		region_frame_info[0] = 0;
		region_frame_info[1] = num_of_signatures;

		media_time_info[0] = 0;
		media_time_info[1] = end_media_time;

		///////////////////////////////////////////////
		//Find Key-Frame
		int maxdist=0;
		int maxdistposition=1;
		int maxcount=0;
		int buffersize = (int)((region_frame_info[1]-region_frame_info[0])*1000.0/(media_time_info[1]-media_time_info[0]))*4;
		int quaterframerate = (int)((region_frame_info[1]-region_frame_info[0])*250.0/(media_time_info[1]-media_time_info[0]));
		int prevmaxpos=-4*quaterframerate;

		int seqtime = (int)((media_time_info[1]-media_time_info[0])/1000.0);
		if(seqtime<=4){
			seqtime = (int)((media_time_info[1]-media_time_info[0])/2000.0);
			buffersize = (int)((region_frame_info[1]-region_frame_info[0])*1000.0/(media_time_info[1]-media_time_info[0]))*seqtime;
		}

		vector<int> distbuffer;
		std::vector<int>::const_iterator itdis;
		vector<int> keypos;

		for(it2 = _raw_frame_signatures.begin(),i=0;it2!=_raw_frame_signatures.end();++it2,i++)
		{
			RawFrameSignature * p_frame_sig = *it2;
			if(i!=0){
				int dist = L1Distance( p_frame_sig->framedescriptor, p_Pre_frame_sig->framedescriptor );
				p_frame_sig->reserve1 = dist;
			}
			p_Pre_frame_sig = *it2;

			INDEX * p_index = new INDEX(p_frame_sig->frameword,FRAME_WORD_COUNT,i);

			AllFrameIndex.push_back(p_index);
		}

		for(it2 = _raw_frame_signatures.begin(),i=0;it2!=_raw_frame_signatures.end();++it2,i++)
		{
			RawFrameSignature * p_frame_sig = *it2;
			if(distbuffer.size()==buffersize){
				distbuffer.push_back(p_frame_sig->reserve1);
				distbuffer.erase(distbuffer.begin());

				maxdist = 0;
				int j;
				for(itdis = distbuffer.begin(),j=0;itdis!=distbuffer.end();++itdis,j++){
					int dis = *itdis;
					if(dis>=maxdist){
						maxdist=dis;
						maxdistposition=i+j-buffersize;
					}
				}
				if(maxdistposition!=prevmaxpos){
					if(maxdistposition-prevmaxpos<=quaterframerate){
	//					int ttt = *keypos.end();
	//					ttt = keypos.size();
						keypos.erase(keypos.end()-1);
					}
	//				else{
					keypos.push_back(maxdistposition);
	//				}
				}

				prevmaxpos=maxdistposition;
			}
			else{
				distbuffer.push_back(p_frame_sig->reserve1);
			}
		}

		for(itdis = keypos.begin();itdis!=keypos.end();++itdis){
			int pos = *itdis;
			_raw_frame_signatures[pos]->reserve = 1.0;
		}

		int keyconfidence = 0;

		for(it2 = _raw_frame_signatures.begin(),i=0;it2!=_raw_frame_signatures.end();++it2,i++)
		{
			RawFrameSignature * p_frame_sig = *it2;
//			if(p_frame_sig->reserve == 1.0){
//				INDEX * p_keyindex = new INDEX(p_frame_sig->frameword,FRAME_WORD_COUNT,i);
//				KeyFrameIndex.push_back(p_keyindex);
//			}
			if(p_frame_sig->reserve == 1.0 || keyconfidence ==1){
				if(p_frame_sig->confidence<1){
					keyconfidence=1;
				}
				else{
					INDEX * p_keyindex = new INDEX(p_frame_sig->frameword,FRAME_WORD_COUNT,i);
					KeyFrameIndex.push_back(p_keyindex);
					keyconfidence = 0;
				}
			}
		}

		for(it2 = _raw_frame_signatures.begin(),i=0;it2!=_raw_frame_signatures.end();++it2,i++)
		{
			RawFrameSignature * p_frame_sig = *it2;

			delete p_frame_sig;
		}

		distbuffer.clear();
		keypos.clear();

		vector <unsigned int> indexTable0[243];
		vector <unsigned int> indexTable1[243];
		vector <unsigned int> indexTable2[243];
		vector <unsigned int> indexTable3[243];
		vector <unsigned int> indexTable4[243];

		int **nIndexContent = new int* [FRAME_WORD_COUNT];
		for(i=0;i<FRAME_WORD_COUNT;i++){
			nIndexContent[i] = new int[244];
		}

		for(i=0;i<243;i++){
			indexTable0[i].clear();
			indexTable1[i].clear();
			indexTable2[i].clear();
			indexTable3[i].clear();
			indexTable4[i].clear();
		}


		//For Key Frame Index Table
		for(i=0;i<FRAME_WORD_COUNT;i++){
			memset(nIndexContent[i],0,sizeof(int)*244);
		}

		std::vector<INDEX *>::const_iterator itIndex;
		int l;
		int totalcount;

		int keyframesize = KeyFrameIndex.size();

		unsigned int* KeyMatchingPosition = new unsigned int [keyframesize];
		l=0;

		for(itIndex = KeyFrameIndex.begin();itIndex!=KeyFrameIndex.end();++itIndex)
		{
			INDEX * p_Index = *itIndex;
			unsigned int tempword;
			unsigned int temppos;
			temppos=(unsigned int) p_Index->framenum;

			KeyMatchingPosition[l] = temppos;
			
			tempword = p_Index->word[0];
			indexTable0[tempword].push_back(l);

			tempword = p_Index->word[1];
			indexTable1[tempword].push_back(l);

			tempword = p_Index->word[2];
			indexTable2[tempword].push_back(l);

			tempword = p_Index->word[3];
			indexTable3[tempword].push_back(l);

			tempword = p_Index->word[4];
			indexTable4[tempword].push_back(l);

			l++;

			delete p_Index;
		}

		KeyFrameIndex.clear();

		m_DescriptorInterface->SetKeyLookUpTable(keyframesize,KeyMatchingPosition);

		for(i=0; i<243; i++)
		{
			nIndexContent[0][i+1] =nIndexContent[0][i]+(int)(indexTable0[i].size());
			nIndexContent[1][i+1] =nIndexContent[1][i]+(int)(indexTable1[i].size());
			nIndexContent[2][i+1] =nIndexContent[2][i]+(int)(indexTable2[i].size());
			nIndexContent[3][i+1] =nIndexContent[3][i]+(int)(indexTable3[i].size());
			nIndexContent[4][i+1] =nIndexContent[4][i]+(int)(indexTable4[i].size());
		}

		unsigned int** KeyIndex  = new unsigned int* [FRAME_WORD_COUNT];
		for(i=0;i<FRAME_WORD_COUNT;i++){
			KeyIndex[i] = new unsigned int[nIndexContent[i][243]];
		}

		totalcount=0;

		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable0[i].begin();itushort!=indexTable0[i].end();++itushort)
			{
				unsigned int position = *itushort;
				KeyIndex[0][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable1[i].begin();itushort!=indexTable1[i].end();++itushort)
			{
				unsigned int position = *itushort;
				KeyIndex[1][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable2[i].begin();itushort!=indexTable2[i].end();++itushort)
			{
				unsigned int position = *itushort;
				KeyIndex[2][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable3[i].begin();itushort!=indexTable3[i].end();++itushort)
			{
				unsigned int position = *itushort;
				KeyIndex[3][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable4[i].begin();itushort!=indexTable4[i].end();++itushort)
			{
				unsigned int position = *itushort;
				KeyIndex[4][totalcount] = position;
				totalcount++;
			}
		}

		m_DescriptorInterface->SetKeyIndexTable(FRAME_WORD_COUNT,nIndexContent,KeyIndex);

		for(i=0;i<243;i++){
			indexTable0[i].clear();
			indexTable1[i].clear();
			indexTable2[i].clear();
			indexTable3[i].clear();
			indexTable4[i].clear();
		}


		//For All Frame Index Table
		for(i=0;i<FRAME_WORD_COUNT;i++){
			memset(nIndexContent[i],0,sizeof(int)*244);
		}

		int allframesize = AllFrameIndex.size();
		l=0;

		for(itIndex = AllFrameIndex.begin();itIndex!=AllFrameIndex.end();++itIndex)
		{
			l++;
			INDEX * p_Index = *itIndex;
			unsigned int tempword;
			unsigned int temppos;
			temppos=(unsigned int) p_Index->framenum;
			
			tempword = p_Index->word[0];
			indexTable0[tempword].push_back(temppos);

			tempword = p_Index->word[1];
			indexTable1[tempword].push_back(temppos);

			tempword = p_Index->word[2];
			indexTable2[tempword].push_back(temppos);

			tempword = p_Index->word[3];
			indexTable3[tempword].push_back(temppos);

			tempword = p_Index->word[4];
			indexTable4[tempword].push_back(temppos);

			delete p_Index;
		}
		AllFrameIndex.clear();

		for(i=0; i<243; i++)
		{
			nIndexContent[0][i+1] =nIndexContent[0][i]+(int)(indexTable0[i].size());
			nIndexContent[1][i+1] =nIndexContent[1][i]+(int)(indexTable1[i].size());
			nIndexContent[2][i+1] =nIndexContent[2][i]+(int)(indexTable2[i].size());
			nIndexContent[3][i+1] =nIndexContent[3][i]+(int)(indexTable3[i].size());
			nIndexContent[4][i+1] =nIndexContent[4][i]+(int)(indexTable4[i].size());
		}

		unsigned int** AllIndex = new unsigned int* [FRAME_WORD_COUNT];
		for(i=0;i<FRAME_WORD_COUNT;i++){
			AllIndex[i] = new unsigned int[nIndexContent[i][243]];
		}

		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable0[i].begin();itushort!=indexTable0[i].end();++itushort)
			{
				unsigned int position = *itushort;
				AllIndex[0][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable1[i].begin();itushort!=indexTable1[i].end();++itushort)
			{
				unsigned int position = *itushort;
				AllIndex[1][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable2[i].begin();itushort!=indexTable2[i].end();++itushort)
			{
				unsigned int position = *itushort;
				AllIndex[2][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable3[i].begin();itushort!=indexTable3[i].end();++itushort)
			{
				unsigned int position = *itushort;
				AllIndex[3][totalcount] = position;
				totalcount++;
			}
		}
		totalcount=0;
		for(i=0; i<243; i++)
		{
			std::vector<unsigned int>::const_iterator itushort;
			for(itushort = indexTable4[i].begin();itushort!=indexTable4[i].end();++itushort)
			{
				unsigned int position = *itushort;
				AllIndex[4][totalcount] = position;
				totalcount++;
			}
		}

		m_DescriptorInterface->SetAllIndexTable(FRAME_WORD_COUNT,nIndexContent,AllIndex);


		for(i=0;i<243;i++){
			indexTable0[i].clear();
			indexTable1[i].clear();
			indexTable2[i].clear();
			indexTable3[i].clear();
			indexTable4[i].clear();
		}
		
		for(i=0;i<FRAME_WORD_COUNT;i++){
			delete[] nIndexContent[i];
			nIndexContent[i] = NULL;

			delete[] AllIndex[i];
			AllIndex[i] = NULL;

			delete[] KeyIndex[i];
			KeyIndex[i] = NULL;
		}
		delete[] nIndexContent;
		nIndexContent = NULL;

		delete[] AllIndex;
		AllIndex = NULL;

		delete[] KeyIndex;
		KeyIndex = NULL;

		delete[] KeyMatchingPosition;
		KeyMatchingPosition = NULL;
	}

	
	delete[] p_histogram_of_words;

	m_Media->CloseVideoFile();

	return 0;
}

void VideoSignatureExtractionTool::setLUT_L1_DIST()
{
	LUT_L1_DIST = new unsigned char [256*256];
	int val1, val2, s1, s2, dist;
	
	for( int i = 0; i < 256; i++ ){
		for( int j = 0; j < 256; j++ ){
			dist = 0;
			val1 = i;		val2 = j;

			s1 = val1/81;	s2 = val2/81;
			dist += ABS( s1 - s2 );
			val1 -= 81*s1;	val2 -= 81*s2;

			s1 = val1/27;	s2 = val2/27;
			dist += ABS( s1 - s2 );
			val1 -= 27*s1;	val2 -= 27*s2;

			s1 = val1/9;	s2 = val2/9;
			dist += ABS( s1 - s2 );
			val1 -= 9*s1;	val2 -= 9*s2;

			s1 = val1/3;	s2 = val2/3;
			dist += ABS( s1 - s2 );
			val1 -= 3*s1;	val2 -= 3*s2;

			s1 = val1;		s2 = val2;
			dist += ABS( s1 - s2 );

			LUT_L1_DIST[(i<<8)+j] = dist;
		}
	}
}

void VideoSignatureExtractionTool::freeLUT_L1_DIST()
{
	if( LUT_L1_DIST != NULL ){
		delete [] LUT_L1_DIST;
		LUT_L1_DIST = NULL;
	}
}

int VideoSignatureExtractionTool::L1Distance( unsigned char *signature1, unsigned char *signature2 )
{
	int dist = 0;

	// 50 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	// 100 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	// 150 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	// 200 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	// 250 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	// 300 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	// 340 trits 
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

//	if(VideoMatching::mode == PARTIAL)
//		return dist;	

	// 380 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];

	return dist;
}




//----------------------------------------------------------------------------
const UUID& VideoSignatureExtractionTool::GetObjectID(void)
{
	return myID;
}

//----------------------------------------------------------------------------
const char *VideoSignatureExtractionTool::GetName(void)
{
	return myName;
}

//----------------------------------------------------------------------------
VideoSignatureExtractionInterfaceABC *VideoSignatureExtractionTool::GetInterface(void)
{
	return &m_Interface;
}


//----------------------------------------------------------------------------
int VideoSignatureExtractionTool::initDimensionList(vector<DIMENSION> &dimensionList )
{
	int i;
	FILE *fp = NULL;
	char *file = "A32+D348_v3.txt";

	if( (fp = fopen( file, "rb" )) == NULL ){
		fprintf( stderr, "Cannot open %s\n", file );
		return -1;
	}

	int		elM, thM, thT;
	double	thV;
	char	aaaa[10000];
	char	bbbb[10000];
	char	*regions[1000];
	char	*pstr;

	int num = 0;
	while( fscanf( fp, "%d %s %s %d %d %lf", &elM, aaaa, bbbb, &thM, &thT, &thV ) != EOF ){
		DIMENSION dimension;

		if( elM == 0 ){ 
			dimension.elM = EL_D; 
		}
		else if( elM == 1 ){ 
			dimension.elM = EL_A; 
		}
		else{
			fprintf( stderr, "Error in readDimensionListFile(): Invalid element mode\n" );
			return -1;
		}

		if( thM == 0 ){
			dimension.thM = TH_A;
			dimension.type = thT;
		}
		else if( thM == 1 ){
			dimension.thM = TH_F;
		}
		else{
			fprintf( stderr, "Error in readDimensionListFile(): Invalid threshold mode\n" );
			return -1;
		}

		dimension.thV = thV;

		pstr = aaaa;
		int numRegionElement = 0;
		for( i = 0; ; i++ ){
			if( (regions[i] = strtok(pstr, ",")) == NULL ){
				break;
			}
			numRegionElement++;
			pstr = NULL;
		}
		for( i = 0; i < numRegionElement; i++ ){
			pstr = strtok( regions[i], "-" );
			int tLI = atoi(pstr);
			
			pstr = strtok( NULL, "-" );
			int bRI = atoi(pstr);

			int tl_x = tLI%32;
			int tl_y = tLI/32;
			int br_x = bRI%32;
			int br_y = bRI/32;

			if( tLI > bRI ){
				fprintf( stderr, "Error in readDimensionListFile(): Invalid region index\n" );
				return -1;
			}
			if( bRI >= 32*32 ){
				fprintf( stderr, "Error in readDimensionListFile(): Invalid region index\n" );
				return -1;
			}

			for( int y = tl_y; y <= br_y; y++ ){
				for( int x = tl_x; x <= br_x; x++ ){
					dimension.aaaaIndex.push_back( y*32+x );
				}
			}
		}

		if( dimension.elM == EL_D ){
			pstr = bbbb;
			numRegionElement = 0;
			for( int i = 0; i < 1000; i++ ){
				if( (regions[i] = strtok(pstr, ",")) == NULL ){
					break;
				}
				numRegionElement++;
				pstr = NULL;
			}
			for( i = 0; i < numRegionElement; i++ ){
				pstr = strtok( regions[i], "-" );
				int tLI = atoi(pstr);
				
				pstr = strtok( NULL, "-" );
				int bRI = atoi(pstr);
							
				int tl_x = tLI%32;
				int tl_y = tLI/32;
				int br_x = bRI%32;
				int br_y = bRI/32;
				
				if( tLI > bRI ){
					fprintf( stderr, "Error in readDimensionListFile(): Invalid region index\n" );
					return -1;
				}
				if( bRI >= 32*32 ){
					fprintf( stderr, "Error in readDimensionListFile(): Invalid region index\n" );
					return -1;
				}
				
				for( int y = tl_y; y <= br_y; y++ ){
					for( int x = tl_x; x <= br_x; x++ ){
						dimension.bbbbIndex.push_back( y*32+x );
					}
				}
			}

		}

		dimensionList.push_back( dimension );
	}
	fclose(fp);

	if( dimensionList.size() != N_D ){
		fprintf( stderr, "Error in readDimensionListFile(): Number of dimensions does not match\n" );
		return -1;
	}

	int count[N_T];
	for( i = 0; i < N_T; i++ ){
		count[i] = 0;
	}
	for( i = 0; i < (int)dimensionList.size(); i++ ){
		if( dimensionList[i].thM == TH_A ){
			int type = dimensionList[i].type;
			if( type < 0 || type >= N_T ){
				fprintf( stderr, "Error in readDimensionListFile(): Type number is not consistent with the number of types\n" );
				return -1;
			}
			count[type]++;
		}
	}
	for( i = 0; i < (int)dimensionList.size(); i++ ){
		if( dimensionList[i].thM == TH_A ){
			int type = dimensionList[i].type;
			dimensionList[i].thIndex = (int)(count[type]*dimensionList[i].thV);
		}
	}

	return 0;
}

IplImage *VideoSignatureExtractionTool::processFrame(momvop *vop, int &frame_counter)
{
	CvSize size;
	size.width = vop->width;
	size.height= vop->height;
	
	IplImage *p_gray_image = cvCreateImage(size, IPL_DEPTH_8U, 1);
	
	unsigned char * p_dst = (unsigned char * )p_gray_image->imageData;
	unsigned char * p_src = vop->y_chan->data->u;
	
	for(int i=0;i<size.height;++i)
	{
		memcpy(p_dst,p_src,size.width);
		
		p_dst += p_gray_image->widthStep;
		p_src += vop->width;
	}
	
	frame_counter++;
	
	return p_gray_image;
}

int VideoSignatureExtractionTool::ExtractSignature( unsigned char *img, int imgWidth, int imgHeight, std::vector<DIMENSION> &dimensionList, unsigned char *signature, unsigned char &confidence, const CTernaryOps &ternary_ops )
{
	int i, j, x, y;
	int posy, posj;
	int type;
	int		sum[32*32];
	short	numPixel[32*32];
	
	for( i = 0; i < 32*32; i++ ){
		sum[i] = 0;
		numPixel[i] = 0;
	}
	
	/*for( y = 0; y < imgHeight; y++ ){
		j = y*32/imgHeight;
		for( x = 0; x < imgWidth; x++ ){
			i = x*32/imgWidth;
			
			sum[j*32+i] += img[y*imgWidth+x];
			numPixel[j*32+i]++;
		}
		*/
	int *i_tab=(int*)malloc(sizeof(int)*imgWidth);
	for( x = 0; x < imgWidth; x++ )
	{
		i_tab[x] = (x<<5)/imgWidth;
	}
	for( y = 0; y < imgHeight; y++ )
	{
		j = (y<<5)/imgHeight;
		posy = y*imgWidth;
		posj = j<<5;
		for( x = 0; x < imgWidth; x++ )
		{
			// speed optimisation: avoid dividing for each pixel
			i = i_tab[x]; //(x<<5)/imgWidth;
			
			sum[posj+i] += img[posy+x];
			numPixel[posj+i]++;
		}
	}
	free(i_tab);
	//}
	
	double value[N_D];
	vector<double> sortList[N_T];
	for( type = 0; type < N_T; type++ ){
		sortList[type].clear();
	}
	
	for( i = 0; i < (int)dimensionList.size(); i++ ){
		int sum1 = 0;
		int sum2 = 0;
		int numPixel1 = 0;
		int numPixel2 = 0;
		double value1, value2;
		
		for( j = 0; j < (int)dimensionList[i].aaaaIndex.size(); j++ ){
			int index = dimensionList[i].aaaaIndex[j];
			sum1 += sum[index];
			numPixel1 += numPixel[index];
		}
		value1 = (double)sum1/(double)numPixel1;
		
		if( dimensionList[i].elM == EL_D ){
			for( j = 0; j < (int)dimensionList[i].bbbbIndex.size(); j++ ){
				int index = dimensionList[i].bbbbIndex[j];
				sum2 += sum[index];
				numPixel2 += numPixel[index];
			}
			value2 = (double)sum2/(double)numPixel2;
		}
		else{ 
			value2 = 128.0; 
		}
		
		value[i] = value1 - value2;
		
		if( dimensionList[i].thM == TH_A ){
			int type = dimensionList[i].type;
			sortList[type].push_back( ABS(value[i]) );
		}
	}
	
	for( type = 0; type < N_T; type++ ){
		std::sort( sortList[type].begin(), sortList[type].end() );
	}
	
	unsigned char *ternary=(unsigned char *)calloc(dimensionList.size(),sizeof(unsigned char));
	for( i = 0; i < (int)dimensionList.size(); i++ ){
		double th;
		
		if( dimensionList[i].thM == TH_F ){ //TH_F
			th = dimensionList[i].thV;
		}
		else{ 
			type = dimensionList[i].type;
			int thIndex = dimensionList[i].thIndex;
			th = sortList[type][thIndex];
		}
		
		if( ABS( value[i] ) > th ){
			if( value[i] < 0.0 ){
				ternary[i] = 0;
			}
			else{			
				ternary[i] = 2;
			}
		}
		else{			
			ternary[i] = 1;
		}

	}
	
	for( i = 0; i < (int)dimensionList.size()/5; i++ ){
		unsigned int b = ternary_ops.Pack5TernaryValuesToA32bitInt(&ternary[5*i]);
		signature[i] = ternary_ops.EncodeTo8BitBinary(b);
	}
	
	free(ternary);
	
	vector<double> absDiffList;
	for( i = 0; i < (int)dimensionList.size(); i++ ){
		if( dimensionList[i].elM == EL_D ){
			absDiffList.push_back( ABS( value[i] ) );
		}
	}
	std::sort( absDiffList.begin(), absDiffList.end() );
	
	int medIndex = (int)absDiffList.size()/2;
	double medAbsDiff = absDiffList[medIndex];
	
	confidence = MIN( (int)(medAbsDiff*8.0), 255 );
	
	return 0;
}

