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
// Applicable File Name:  VideoSignatureCS.cpp
//
///////////////////////////////////////////////////////////////////////////////


#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>

#endif

//=============================================================================
//=============================================================================

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
//=============================================================================
//=============================================================================
// 2010.02 Compresion

typedef const unsigned int CUInt;

//=============================================================================
class CTernaryOpsCompress
{
private:

	//5 ternary numbers (10bits) are packed to 8 bits
	unsigned char _ter_to_bin_table[1024];//unpacked 5 ternary number takes up 10 bits thus 2^10 = 1024
	//static unsigned int bin_to_ter_table[243];//total packed ternary numbers = 3^5
	unsigned int _bin_to_ter_table[256];//for memory alignment

	//static unsigned int hamming_dist_table[243][243];//stores the hamming distance between two binary encoded packed 5 ternary values
	unsigned int _hamming_dist_table[256][256];//for proper alignment

	unsigned char NaN;

	const int _threshold_u;
	const int _threshold_l;

private:
	void init_ter_to_bin_table()
	{
		for(int i=0;i<1024;++i)
		{
			_ter_to_bin_table[i] = NaN;
		}
	}

	void init_bin_to_ter_table()
	{
		for(int i=0;i<243;++i)
		{
			_bin_to_ter_table[i] = 0;
		}
	}

	void generate_5_packed_ternary_numbers()
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

	// added 2010.02
	unsigned char get_5ternary_values_by_index(std::vector<int>::iterator bpi, const unsigned int index_to_the_byte,unsigned char * p_BET_data)const
	{
		unsigned char BET = p_BET_data[index_to_the_byte];
		
		unsigned int packed_ternary_value = DecodeToPackedTernaryValue(BET);
		
		int v0,v1,v2,v3,v4;
		v0=BET/81;
		BET-=(81*v0);
		v1=BET/27;
		BET-=(27*v1);
		v2=BET/9;
		BET-=(9*v2);
		v3=BET/3;
		BET-=(3*v3);
		v4=BET;

		*bpi=v0;
		*(bpi+1)=v1;
		*(bpi+2)=v2;
		*(bpi+3)=v3;
		*(bpi+4)=v4;
		return 0;
	}
	

public:
	CTernaryOpsCompress(int upperlimit,int lowerlimit) : _threshold_u(upperlimit),_threshold_l(lowerlimit)
	{
		NaN = 0xFF;
		init_ter_to_bin_table();
		init_bin_to_ter_table();

		generate_5_packed_ternary_numbers();
	}

	void GetTernary(std::vector<int> &bp, unsigned char *_signatures)
	{
		std::vector<int>::iterator bpi;
		
		bpi = bp.begin();    
		for (unsigned int byte_index = 0; byte_index < FRAME_SIGNATURE_SIZE; byte_index++)
		{
			get_5ternary_values_by_index(bpi, byte_index, _signatures);
			bpi += 5;
		}
	}
	
	void GetBinary(std::vector<int> &bp, unsigned char *_p_signatures)
	{
		std::vector<int>::iterator bpi;
		unsigned char ternary_values[5];
		unsigned int packed_ternary_value, ii = 0;
		
		bpi = bp.begin();    
		for (bpi = bp.begin(); bpi != bp.end();)
		{
			ternary_values[0] = *bpi++;
			ternary_values[1] = *bpi++;
			ternary_values[2] = *bpi++;
			ternary_values[3] = *bpi++;
			ternary_values[4] = *bpi++;
			//create packed ternary value
			packed_ternary_value = Pack5TernaryValuesToA32bitInt(ternary_values);
			_p_signatures[ii] = EncodeTo8BitBinary(packed_ternary_value);
			//get_ternary_value_by_index(index, _p_signatures);
			ii++;
		}
	}

	unsigned char ConvertToTernary(int value) const
	{
		if(value>_threshold_u)
			return 1;		//represents +1
		else
			if(value<_threshold_l)
				return 2;	//represents -1

		return 0;			//represents 0;
	}

	static unsigned int Pack5TernaryValuesToA32bitInt(unsigned char * p_ternary_array)
	{
		unsigned int retval = 0;

		retval = 81*p_ternary_array[0] + 27*p_ternary_array[1] + 9*p_ternary_array[2] + 3*p_ternary_array[3] + p_ternary_array[4];
		
		return retval;
	}

	unsigned char EncodeTo8BitBinary(unsigned int packed_ternary_value) const
	{
		if(packed_ternary_value >=243)
			return 0xFF;

		return (unsigned char)packed_ternary_value;
	}

	unsigned int DecodeToPackedTernaryValue(unsigned char value) const
	{
		if(value>242)
			return (unsigned int)(-1);

		return _bin_to_ter_table[value];
	}

};


//=============================================================================
inline unsigned int FloorLog2(unsigned int val2)
{
	unsigned int M = 0;
	while (val2 > 1)
	{//get the log base 2 of val.
		val2 >>= 1;
		M++;        
	}
	return M;
}

inline unsigned int CeilLog2(unsigned int val2)
{
	unsigned int M = 0;
	val2--;
	while (val2 > 0)
	{//get the log base 2 of val.
		val2 >>= 1;
		M++;        
	}
	return M;
}

//=============================================================================
struct GOP
{
	unsigned int gopstartpos, goplen, numel;
	std::vector<unsigned int> RLv; //run-length coded data
	unsigned char *key_picture;
};

enum SignatureRepresentation
{
	One_Dimension_Per_Byte = 0,
	Binary_1 = 1,	//Binary 00,01,10
	Binary_2 = 2,	//Binary 1 bit sign, 1 bit significance
	Packed_Ternary =3
};


struct VideoHeader 
{

	//char			_filename[1024];
	unsigned int	_frame_count;
	unsigned int	_frame_rate;
	unsigned short	_frame_width;
	unsigned short  _frame_height;
	unsigned int    _colour_channel_count; //only byte would be enough, but for better alignment int is used

};

//static const int	SH_SIZE_OF_RESERVED_BLOCK = 1022;

struct SignatureHeader
{
	unsigned short		_bytes_per_frame;
	unsigned short		_number_of_dimensions;

	unsigned int		_segment_count;

	unsigned char		_word_count;
	unsigned char		_signature_representation;

	unsigned short      _segment_size;
	unsigned short      _segment_offset;
};

//=============================================================================
class SegmentSignaturesCompressed
{
private:
	//unsigned int  _gopnum;
	unsigned int  picbytelen, picbitlen, _goplenbits, _numdim, _seglen;
	unsigned char *_p_signatures_compressed;
	unsigned int _p_signatures_compressed_size;
	unsigned char _byte_val;
	std::vector<GOP> _GOPs; 
	XM::BitBuffer *_p_coder;
	
public:
	
	  SegmentSignaturesCompressed(XM::BitBuffer *fp, CUInt seglen, CUInt numdim, CUInt bytes_per_frame)
	  {
		  _p_coder = fp;
		  CUInt numel = seglen * numdim; //a conservative estimation of the compressed byte length
		  
		  _numdim = numdim;
		  _seglen = seglen;
		  picbytelen = bytes_per_frame;
		  picbitlen = bytes_per_frame * 8;
		  _goplenbits = CeilLog2(seglen); //bits for encoding the length of a GOP, assumed that seglen >= 2
		  _p_signatures_compressed = 0; //new unsigned char [picbytelen];
		  _p_signatures_compressed_size = 0;
	  }
	  
	  ~SegmentSignaturesCompressed()
	  {
		  if(_p_signatures_compressed) free(_p_signatures_compressed);
	  }
	  
	  
private:
	
	void WriteDataPrec(unsigned int data, int bits)
	{
		_p_coder->PutBitsLong(data, bits);
	}
	
	void WriteBit(int bit)
		// Purpose: Writes one bit into the the bit-stream
	{
		if(bit)
		{
			_p_coder->PutBit(1);
		}
		else
		{
			_p_coder->PutBit(0);
		}
	}
	
	//------------------------------------------------------------------------
	int ReadBits(unsigned int number_of_bits)
		// Purpose: Read number_of_bits bits from bitstream
	{
		long lvalue=0;
		int value=0;
		_p_coder->GetBitsLong(&lvalue, number_of_bits);
		_p_coder->TakeBits(number_of_bits);
		value = (int)lvalue;
		
		return value;
	}
	
public:
	  unsigned int GopsNum()
	  {
		  return static_cast<unsigned int > (_GOPs.size());
	  }

	  int AllocNewKeySignature()
	  {
		  int new_pos = 0;
		  if(!_p_signatures_compressed)
		  {
			  _p_signatures_compressed = (unsigned char *)malloc(picbytelen*sizeof(unsigned char));
			  _p_signatures_compressed_size = picbytelen;
		  }
		  else
		  {
			  new_pos = _p_signatures_compressed_size;
			  _p_signatures_compressed = (unsigned char *)realloc(_p_signatures_compressed, (_p_signatures_compressed_size + picbytelen)*sizeof(unsigned char));
			  _p_signatures_compressed_size += picbytelen;
		  }
		  if(!_p_signatures_compressed) return -1;
		  return new_pos;
	  }

	void PushGOP(unsigned char *key_picture, CUInt gopstartpos)
	{
		GOP newGOP;
		newGOP.key_picture = key_picture;
		newGOP.gopstartpos = gopstartpos;
		_GOPs.push_back(newGOP);
	}
	
	void RLvGOPs(std::vector< std::vector<int> > &Bpdiff)
	{
		unsigned int g, n, r, c, zero_cntr, bplen;
		//Process GOPs
		for(g=0; g < (_GOPs.size() - 1); g+=1)
		{
			//vectorise the remaining pictures and create a run-length vector
			_GOPs[g].goplen = _GOPs[g+1].gopstartpos - _GOPs[g].gopstartpos; //GopStartPos[g+1] - GopStartPos[g] - 1;
			bplen = _GOPs[g].goplen - 1;
			_GOPs[g].numel = bplen * _numdim;
			
			if (bplen > 0) //non KP_GOP 
			{
				zero_cntr = 0;
				for  (n = 0; n < _GOPs[g].numel; n++)
				{
					
					c = n / bplen;
					r = _GOPs[g].gopstartpos + (n % bplen);
					if (Bpdiff[r][c] != 0)
					{
						_GOPs[g].RLv.push_back(zero_cntr); //length of the zero-run
						_GOPs[g].RLv.push_back(Bpdiff[r][c]); //the non-zero element
						zero_cntr = 0;
					}
					else
						zero_cntr += 1;
				}
				if (Bpdiff[r][c] == 0) //if the data ends with a zero run
					_GOPs[g].RLv.push_back(zero_cntr);
			}
			//else is a KP_GOP (no predicted pictures)
		}
	}
	
	void Compress()
	{
		unsigned int egr, egr2, tre;
		
		for(unsigned int g=0; g < (_GOPs.size() - 1); g+=1)
		{	
			_p_coder->PutBits((char*)(_GOPs[g].key_picture), picbitlen);
			
			//GOP length 
			WriteDataPrec(_GOPs[g].goplen-1, _goplenbits);
			//entropy encode the run-length coded data
			for (unsigned int n = 0; n < _GOPs[g].RLv.size(); n++)
			{
				if (n % 2 == 0) //zero run-length
				{//Exp-Golomb encode, s = 2 
					egr = FloorLog2(_GOPs[g].RLv[n] + 4) - 1;
					//prefix part
					egr2 = 1 << egr;
					WriteDataPrec(egr2 - 2, egr);
					//
					egr2 >>= 1;
					tre = 4 * (egr2 - 1);
					WriteDataPrec(_GOPs[g].RLv[n] - tre, egr + 1);
				}
				else
				{//just write one bit
					WriteBit(_GOPs[g].RLv[n] == 2);
				}
			}
		}
	}
	
	int Decompress()
	{
		unsigned int g = 0, egr, egr2, tre, value, bplen, numelcnt = 0;
		unsigned int picread = 0;
		
		while (picread < _seglen)
		{
			//get the key picture
			int pos = AllocNewKeySignature();
			if(pos < 0) return pos;
			_p_coder->GetBits((char*)(_p_signatures_compressed + pos), picbitlen);
			_p_coder->TakeBits(picbitlen);
			PushGOP(_p_signatures_compressed, pos);

			//get the GOP length
			_GOPs[g].goplen = ReadBits(_goplenbits)+1;
			bplen = _GOPs[g].goplen - 1;
			_GOPs[g].numel = bplen * _numdim;
			//read the RLV structure
			while (numelcnt < _GOPs[g].numel)
			{
				//first read the zero run length
				egr = 1;
				while (ReadBits(1))
					egr++;
				
				egr2 = 1 << (egr - 1);
				tre = 4 * (egr2 - 1);
				value = ReadBits(egr + 1) + tre; 
				_GOPs[g].RLv.push_back(value);
				numelcnt += value;
				if (numelcnt >= _GOPs[g].numel)
					break;
				//now read the non-zero symbol
				value = ReadBits(1) + 1;
				_GOPs[g].RLv.push_back(value);
				numelcnt += 1;
			}
			picread += _GOPs[g].goplen;
			g++;
			numelcnt = 0;
		}
		return 0;
	}
	
	unsigned int ReconstructGOP(CTernaryOpsCompress &_ternary, std::vector< std::vector<int> > &Bp, unsigned char * p_key_picture_sig, unsigned int gopnum)
	{
		//unsigned int bpos = 0;
		std::vector< std::vector<int> > Bpdiff (_GOPs[gopnum].goplen - 1, std::vector<int> (_numdim,0));
		unsigned int trg_bitpos = 0, bplen, n, m, r, c, data;
		
		//copy the key picture
		memcpy(p_key_picture_sig, _p_signatures_compressed + _GOPs[gopnum].gopstartpos, picbytelen);

		bplen = _GOPs[gopnum].goplen - 1;
		
		//create the matrix of picture differences
		n = 0;
		for (unsigned int rl = 0; rl < _GOPs[gopnum].RLv.size(); rl++)
		{
			data = _GOPs[gopnum].RLv[rl];
			if (rl % 2 == 0) //zero run-length
			{
				while (data > 0)
				{
					c = n / bplen;
					r = n % bplen;
					Bpdiff[r][c] = 0;
					n++;
					data--;
				}
			}
			else
			{//just write one non-zero element
				c = n / bplen;
				r = n % bplen;
				Bpdiff[r][c] = data;
				n++;
			}
		}
		_ternary.GetTernary(Bp[0], p_key_picture_sig);
		//compute modulo 3 addition to on the rest
		for (n = 0; n < bplen; n++)
		{
			for(m=0; m < _numdim; m++)
			{
				Bp[n+1][m] = (3 + Bp[n][m] - Bpdiff[n][m]) % 3;
			}
		}
		
		return bplen;
	}
	
	
};

//=============================================================================
class CBinFileFormat
{
private:
	VideoHeader			_video_header;
	
	SignatureHeader		_signature_header;
	
	std::vector<unsigned char *>		_frame_signatures;

	std::vector<SegmentSignaturesCompressed*> 	_segment_compressed;

	unsigned int  _segment_size;
	CUInt         _segment_offset;

	CTernaryOpsCompress & _ternary_ops;
public:

	CBinFileFormat(CUInt segment_size, 
				   CUInt segment_offset,
				   CUInt size_of_signature,
				   CUInt word_count,
				   CUInt frame_count,
				   CTernaryOpsCompress & ternary_ops
				   ):
						_segment_size(segment_size),
						_segment_offset(segment_offset),
						_ternary_ops(ternary_ops)
	{

		_signature_header._bytes_per_frame		= size_of_signature;
		_signature_header._number_of_dimensions = size_of_signature * 5;
		_signature_header._word_count		= word_count;
		_signature_header._signature_representation = Packed_Ternary;
		_signature_header._segment_size = segment_size;
		_signature_header._segment_offset = segment_offset;
		_video_header._frame_count = frame_count; 
	}


	~CBinFileFormat()
	{
		std::vector<unsigned char *>::const_iterator it1;
		for(it1 = _frame_signatures.begin();it1!=_frame_signatures.end();++it1)
		{
			unsigned char *p_frame_sig = *it1;
			delete p_frame_sig;
		}

		std::vector<SegmentSignaturesCompressed *>::const_iterator it2;
		for(it2 = _segment_compressed.begin();it2!=_segment_compressed.end();++it2)
		{
			SegmentSignaturesCompressed *p_segment = *it2;
			delete p_segment;
		}

	}

	const SignatureHeader &GetSignatureHeader() const
	{
		return _signature_header;
	}

	SegmentSignaturesCompressed* AddSegmentSignaturesCompressed(XM::BitBuffer *_encoder_buffer, CUInt segment_length, CUInt num_dimensions, CUInt bytes_per_frame)
	{
		SegmentSignaturesCompressed * p_seg_compr = new SegmentSignaturesCompressed(_encoder_buffer, segment_length, num_dimensions, bytes_per_frame);
		_segment_compressed.push_back(p_seg_compr);
		return p_seg_compr;
	}

	std::vector<SegmentSignaturesCompressed *> GetCompressedSignatures()
	{
		return _segment_compressed;
	}

	int LoadCompressed(XM::BitBuffer *_decoder_buffer)
	{
		unsigned int seg_step = _signature_header._segment_offset;
		unsigned int i=0;
		for( i=0; i < _video_header._frame_count; i += seg_step)
		{
			if ((i + seg_step) > _video_header._frame_count) //adapt to the last segment
				seg_step = static_cast<unsigned int> (_video_header._frame_count - i);
			
			SegmentSignaturesCompressed *p_seg_compr = new SegmentSignaturesCompressed(_decoder_buffer, seg_step, _signature_header._number_of_dimensions, _signature_header._bytes_per_frame);
			p_seg_compr->Decompress();

			
			_segment_compressed.push_back(p_seg_compr);
		}

		//arrays of ternary 0,1,2
		std::vector< std::vector<int> > Bp (_signature_header._segment_offset, std::vector<int> (_signature_header._number_of_dimensions,0));
		unsigned char *p_picture_sig;
		unsigned int bplen, n, g;
		//go through segments and recreate the frame signatures
		//unsigned int segstartpos;
		for(i=0; i < _segment_compressed.size(); ++i)
		{
			for(g=0; g < _segment_compressed[i]->GopsNum(); g+=1)
			{
				//new key picture 
				p_picture_sig = new unsigned char[_signature_header._bytes_per_frame];
				SegmentSignaturesCompressed *p_seg_compr = _segment_compressed[i];
				bplen = p_seg_compr->ReconstructGOP(_ternary_ops, Bp, p_picture_sig, g);
								
				_frame_signatures.push_back(p_picture_sig);

				//push back the rest of pictures
				for (n = 0; n < bplen; n++)
				{
					p_picture_sig = new unsigned char[_signature_header._bytes_per_frame];
					_ternary_ops.GetBinary(Bp[n+1], p_picture_sig);
					_frame_signatures.push_back(p_picture_sig);
				}
			}
		}
		
		return 0;	
	}

	unsigned char *GetFrameSignature(unsigned int pos)
	{
		if(pos < _frame_signatures.size())
		{
			return _frame_signatures[pos];
		}
		return 0;
	}
};

//=============================================================================
//=============================================================================


using namespace XM;

const XM::UUID VideoSignatureCSInterface::myID = XM::UUID("");
const char *VideoSignatureCSInterface::myName = "Video Signature Coding Engine Interface";

const XM::UUID VideoSignatureCS::myID = XM::UUID("");
const char *VideoSignatureCS::myName = "Video Signature Coding Scheme";

const XM::UUID VideoSignatureCS::valID = XM::UUID("");
const char *VideoSignatureCS::valName = "Video Signature Coding Scheme";

const XM::UUID VideoSignatureCSInterfaceABC::myID = XM::UUID("");

//=============================================================================
VideoSignatureCSInterfaceABC::VideoSignatureCSInterfaceABC()
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
VideoSignatureCSInterfaceABC *VideoSignatureCS::GetInterface(void)
{
  return &m_Interface;
}

//=============================================================================
VideoSignatureCSInterface::~VideoSignatureCSInterface()
{
}

//-----------------------------------------------------------------------------
VideoSignatureCSInterface::VideoSignatureCSInterface(VideoSignatureCS *pVideoSignature) :
m_pVideoSignatureCS(pVideoSignature)
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
const XM::UUID& VideoSignatureCSInterface::GetInterfaceID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *VideoSignatureCSInterface::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
void VideoSignatureCSInterface::destroy(void)
{
  delete m_pVideoSignatureCS;
}

//-----------------------------------------------------------------------------
BitBuffer *VideoSignatureCSInterface::GetEncoderStreamBuffer()
{
  return m_pVideoSignatureCS->GetEncoderStreamBuffer();
}

//-----------------------------------------------------------------------------
int VideoSignatureCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pVideoSignatureCS->SetEncoderStreamBuffer(aBuffer);
}

//-----------------------------------------------------------------------------
BitBuffer *VideoSignatureCSInterface::GetDecoderStreamBuffer()
{
  return m_pVideoSignatureCS->GetDecoderStreamBuffer();
}

//-----------------------------------------------------------------------------
int VideoSignatureCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pVideoSignatureCS->SetDecoderStreamBuffer(aBuffer);
}

//-----------------------------------------------------------------------------
VideoSignatureDescriptorInterfaceABC *VideoSignatureCSInterface::GetDescriptorInterface(void)
{
  return m_pVideoSignatureCS->GetDescriptorInterface();
}

//-----------------------------------------------------------------------------
int VideoSignatureCSInterface::SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC *pVideoSignatureDescriptorInterface)
{
  return m_pVideoSignatureCS->SetDescriptorInterface(pVideoSignatureDescriptorInterface);
}

//-----------------------------------------------------------------------------
int VideoSignatureCSInterface::StartEncode(void)
{
  return m_pVideoSignatureCS->StartEncode();
}

//-----------------------------------------------------------------------------
int VideoSignatureCSInterface::StartDecode(void)
{
  return m_pVideoSignatureCS->StartDecode();
}
//0825

void VideoSignatureCSInterface::SetKeyFrameInfoFlag(int flag)
{
	m_pVideoSignatureCS->SetKeyFrameInfoFlag(flag); 
}

int VideoSignatureCSInterface::GetKeyFrameInfoFlag()
{
	return m_pVideoSignatureCS->GetKeyFrameInfoFlag(); 
}

//=============================================================================
VideoSignatureCS::VideoSignatureCS()
: m_Interface(this)
, m_isProprietary(false)
, m_EncoderBuffer(0)
, m_DecoderBuffer(0)
, m_pVideoSignatureDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
  m_KeyFrameFlag = 0;
}

//-----------------------------------------------------------------------------
VideoSignatureCS::VideoSignatureCS(VideoSignatureDescriptorInterfaceABC
                               *VideoSignature)
: m_Interface(this)
, m_isProprietary(false)
, m_EncoderBuffer(0)
, m_DecoderBuffer(0)
, m_pVideoSignatureDescriptorInterface(0)
{
  if (!VideoSignature)
  {
    VideoSignatureDescriptor *descriptor = new VideoSignatureDescriptor();
    VideoSignature = descriptor->GetInterface();
  }

  SetDescriptorInterface(VideoSignature);

  SetExposedInterface(&m_Interface);
}

//-----------------------------------------------------------------------------
VideoSignatureCS::~VideoSignatureCS()
{
  if (m_pVideoSignatureDescriptorInterface)
    m_pVideoSignatureDescriptorInterface->release();
}

//-----------------------------------------------------------------------------
const XM::UUID& VideoSignatureCS::GetObjectID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *VideoSignatureCS::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
const XM::UUID& VideoSignatureCS::GetValueID(void)
{
  return valID;
}

//-----------------------------------------------------------------------------
const char* VideoSignatureCS::GetValueName(void)
{
  return valName;
}

//-----------------------------------------------------------------------------
bool VideoSignatureCS::IsProprietary(void)
{
  return m_isProprietary;
}

//-----------------------------------------------------------------------------
BitBuffer *VideoSignatureCS::GetEncoderStreamBuffer()
{
  return m_EncoderBuffer;
}

//-----------------------------------------------------------------------------
int VideoSignatureCS::SetEncoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;

  m_EncoderBuffer = pBuffer;
  return 0;
}

//-----------------------------------------------------------------------------
BitBuffer *VideoSignatureCS::GetDecoderStreamBuffer()
{
  return m_DecoderBuffer;
}

//-----------------------------------------------------------------------------
int VideoSignatureCS::SetDecoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;

  m_DecoderBuffer = pBuffer;
  return 0;
}
//0825
void VideoSignatureCS::SetKeyFrameInfoFlag(int flag)
{
	m_KeyFrameFlag = flag;
}

int VideoSignatureCS::GetKeyFrameInfoFlag()
{
	return m_KeyFrameFlag;
}

//-----------------------------------------------------------------------------
VideoSignatureDescriptorInterfaceABC *VideoSignatureCS::GetDescriptorInterface(void)
{
  return m_pVideoSignatureDescriptorInterface;
}

//-----------------------------------------------------------------------------
int VideoSignatureCS::SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC *pVideoSignatureDescriptorInterface)
{
  if (m_pVideoSignatureDescriptorInterface ==
      pVideoSignatureDescriptorInterface) return 0;

  if (m_pVideoSignatureDescriptorInterface)
    m_pVideoSignatureDescriptorInterface->release();

  m_pVideoSignatureDescriptorInterface = pVideoSignatureDescriptorInterface;
  if (m_pVideoSignatureDescriptorInterface)
  {
    m_pVideoSignatureDescriptorInterface->addref();
  }
  else
  {
    return -1;
  }

  return 0;
}

//-----------------------------------------------------------------------------
int VideoSignatureCS::StartEncode(void)
{
	unsigned int i;
	unsigned short *spatial_region;
	unsigned int *region_frame_info;
	unsigned short media_time_unit;
	unsigned int *media_time_info;

	unsigned int num_of_frames;
	unsigned int num_of_segments;
	unsigned int *seg_start_frames = 0;
	unsigned int *seg_end_frames = 0;
	unsigned char *bags_of_words = 0;
	unsigned char *confidences = 0;
	unsigned char *words = 0;
	unsigned char *signatures = 0;
	int compression_flag = 0;
	// check if Descriptor and stream buffers are linked to coding scheme
	if (!m_EncoderBuffer || !m_pVideoSignatureDescriptorInterface)
	{
		return -1;
	}

	spatial_region = m_pVideoSignatureDescriptorInterface->GetSpatialRegion();
	region_frame_info = m_pVideoSignatureDescriptorInterface->GetRegionFrames();
	media_time_unit = m_pVideoSignatureDescriptorInterface->GetMediaTimeUnit();
	media_time_info = m_pVideoSignatureDescriptorInterface->GetMediaTime();

	num_of_segments = m_pVideoSignatureDescriptorInterface->GetNumberOfSegments();
	seg_start_frames = m_pVideoSignatureDescriptorInterface->GetSegmentStartFrames();
	seg_end_frames = m_pVideoSignatureDescriptorInterface->GetSegmentEndFrames();
	bags_of_words = m_pVideoSignatureDescriptorInterface->GetSegmentBagOfWords();

	confidences = m_pVideoSignatureDescriptorInterface->GetFrameConfidences();
	words = m_pVideoSignatureDescriptorInterface->GetFrameWords();
	signatures = m_pVideoSignatureDescriptorInterface->GetFrameSignatures();
	compression_flag = m_pVideoSignatureDescriptorInterface->GetCompressionFlag();

	m_EncoderBuffer->PutBitsLong((long)1, 32); // NumOfSpatialRegions = 1
	m_EncoderBuffer->PutBit(1); // SpatialLocationFlag = 1
	m_EncoderBuffer->PutBitsLong((long)spatial_region[0], 16);
	m_EncoderBuffer->PutBitsLong((long)spatial_region[1], 16);
	m_EncoderBuffer->PutBitsLong((long)spatial_region[2], 16);
	m_EncoderBuffer->PutBitsLong((long)spatial_region[3], 16);
	m_EncoderBuffer->PutBitsLong((long)region_frame_info[0], 32);
	m_EncoderBuffer->PutBitsLong((long)region_frame_info[1], 32);
	m_EncoderBuffer->PutBitsLong((long)media_time_unit, 16);
	if(media_time_info)
	{
		m_EncoderBuffer->PutBit(1);
		m_EncoderBuffer->PutBitsLong((long)media_time_info[0], 32);
		m_EncoderBuffer->PutBitsLong((long)media_time_info[1], 32);
	}
	else
	{
		m_EncoderBuffer->PutBit(0);
	}

	m_EncoderBuffer->PutBitsLong((long)num_of_segments, 32);

	for(i=0; i<num_of_segments; i++)
	{
		m_EncoderBuffer->PutBitsLong((long)seg_start_frames[i], 32);
		m_EncoderBuffer->PutBitsLong((long)seg_end_frames[i], 32);

		m_EncoderBuffer->PutBit(1); // // MediaTimeFlagOfSegment = 1
		m_EncoderBuffer->PutBitsLong((long)m_pVideoSignatureDescriptorInterface->GetMediaTimeOfFrame(seg_start_frames[i]), 32);
		m_EncoderBuffer->PutBitsLong((long)m_pVideoSignatureDescriptorInterface->GetMediaTimeOfFrame(seg_end_frames[i]), 32);
		
		m_EncoderBuffer->PutBits((char*)(bags_of_words + (i*SEGMENT_BAGS_SIZE)), 243);
		m_EncoderBuffer->PutBits((char*)(bags_of_words + (i*SEGMENT_BAGS_SIZE)) + 32, 243);
		m_EncoderBuffer->PutBits((char*)(bags_of_words + (i*SEGMENT_BAGS_SIZE)) + 64, 243);
		m_EncoderBuffer->PutBits((char*)(bags_of_words + (i*SEGMENT_BAGS_SIZE)) + 96, 243);
		m_EncoderBuffer->PutBits((char*)(bags_of_words + (i*SEGMENT_BAGS_SIZE)) + 128, 243);
	}

	m_EncoderBuffer->PutBit(compression_flag);
	num_of_frames = region_frame_info[1];
	for(i=0; i<num_of_frames; i++)
	{
		m_EncoderBuffer->PutBit(1); // // MediaTimeFlagOfFrame = 1
		m_EncoderBuffer->PutBitsLong((long)m_pVideoSignatureDescriptorInterface->GetMediaTimeOfFrame(region_frame_info[0] + i), 32);

		m_EncoderBuffer->PutBitsLong((long)confidences[i], 8);
		m_EncoderBuffer->PutBits((char*)(words + (i*FRAME_WORD_COUNT)), FRAME_WORD_COUNT*8);
		if(!compression_flag)
		{
			m_EncoderBuffer->PutBits((char*)(signatures + (i*FRAME_SIGNATURE_SIZE)), FRAME_SIGNATURE_SIZE*8);
		}
	}

	if(compression_flag)
	{
		unsigned char *_frame_signatures = m_pVideoSignatureDescriptorInterface->GetFrameSignatures();
		CTernaryOpsCompress ternary_ops(0,0);
		unsigned int j;
		const double zero_thresh = 0.6;
		double zero_perc;
		
		CBinFileFormat mpeg7_signature(S_S, S_0, 76, SEGMENT_WORD_COUNT, num_of_frames, ternary_ops);
		
		SignatureHeader sig_header = mpeg7_signature.GetSignatureHeader();
		SegmentSignaturesCompressed *seg_compr;
		unsigned int seg_step = sig_header._segment_offset;
		
		//arrays of ternary 0,1,2
		vector< vector<int> > Bp (seg_step, vector<int> (sig_header._number_of_dimensions,0)), Bpdiff (seg_step-1, vector<int> (sig_header._number_of_dimensions,0));
		int zero_cntr;
		for(i=0; i < num_of_frames; i+=seg_step)
		{			
			if ((i + seg_step) > num_of_frames) //adapt to the last segment
				seg_step = static_cast<unsigned int> (num_of_frames - i);
			
			seg_compr = mpeg7_signature.AddSegmentSignaturesCompressed(m_EncoderBuffer, seg_step, sig_header._number_of_dimensions, sig_header._bytes_per_frame);
			
			//convert from the packed into ternary 
			seg_compr->PushGOP(_frame_signatures + i*FRAME_SIGNATURE_SIZE, 0);
			
			for(int f=0; f < seg_step; f+=1)
			{
				ternary_ops.GetTernary(Bp[f], _frame_signatures + (i+f) * FRAME_SIGNATURE_SIZE);
				
				if (f > 0)
				{//compute the modulo 3 difference
					zero_cntr = 0;
					for(j=0; j < sig_header._number_of_dimensions; j+=1)
					{
						Bpdiff[f-1][j] = (3 + Bp[f-1][j] - Bp[f][j]) % 3;
						if (Bpdiff[f-1][j] == 0)
							zero_cntr += 1;
					}
					zero_perc = (double) zero_cntr / (double) sig_header._number_of_dimensions;
					
					if (zero_perc < zero_thresh)
						seg_compr->PushGOP(_frame_signatures + (i+f) * FRAME_SIGNATURE_SIZE, f);
				}
			}
			//insert the "virtual" key picture from the following segment
			seg_compr->PushGOP(NULL, seg_step);
			seg_compr->RLvGOPs(Bpdiff);
			seg_compr->Compress();
		}	
	}

	return 0;

} // StartEncode

void VideoSignatureCS::setLUT_L1_DIST()
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

void VideoSignatureCS::freeLUT_L1_DIST()
{
	if( LUT_L1_DIST != NULL ){
		delete [] LUT_L1_DIST;
		LUT_L1_DIST = NULL;
	}
}

int VideoSignatureCS::L1Distance( unsigned char *signature1, unsigned char *signature2 )
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

//-----------------------------------------------------------------------------
int VideoSignatureCS::StartDecode(void)
{
	long ltmp;
	unsigned int i, j;
	int flag;

	unsigned short spatial_region[4];
	unsigned int region_frame_info[2];
	unsigned short media_time_unit;
	unsigned int media_time_info[2];

	unsigned int num_of_regions;
	unsigned int num_of_frames;
	unsigned int num_of_segments;
	unsigned int seg_start_frame = 0;
	unsigned int seg_end_frame = 0;
	unsigned char bags_of_words[SEGMENT_BAGS_SIZE];
	unsigned char confidence;
	unsigned char words[FRAME_WORD_COUNT];
	unsigned char signatures[FRAME_SIGNATURE_SIZE];
	unsigned char *sig_ptr = 0;
	int compression_flag = 0;
	int key_info_flag = 0;
	
	key_info_flag = GetKeyFrameInfoFlag();
	vector<RawFrameSignature*>	_frame_signatures;

	if (!m_DecoderBuffer || !m_pVideoSignatureDescriptorInterface)
		return -1;

	m_DecoderBuffer->GetBitsLong(&ltmp, 32);
	m_DecoderBuffer->TakeBits(32);
	num_of_regions = (unsigned int)ltmp;
	if(num_of_regions == 0) return -1;

	// 1 region is decoded
	flag = m_DecoderBuffer->GetBit(0);
	m_DecoderBuffer->TakeBits(1);
	if(flag)
	{
		for(i=0; i < 4; i++)
		{
			m_DecoderBuffer->GetBitsLong(&ltmp, 16);
			m_DecoderBuffer->TakeBits(16);
			spatial_region[i] = (unsigned short)ltmp;
		}
		m_pVideoSignatureDescriptorInterface->SetSpatialRegion(spatial_region[0], spatial_region[1],
												spatial_region[2], spatial_region[3]);

	}

	for(i=0; i < 2; i++)
	{
		m_DecoderBuffer->GetBitsLong(&ltmp, 32);
		m_DecoderBuffer->TakeBits(32);
		region_frame_info[i] = (unsigned int)ltmp;
	}
	m_pVideoSignatureDescriptorInterface->SetRegionFrames(region_frame_info[0], region_frame_info[1]);


	m_DecoderBuffer->GetBitsLong(&ltmp, 16);
	m_DecoderBuffer->TakeBits(16);
	media_time_unit = (unsigned short)ltmp;
	flag = m_DecoderBuffer->GetBit(0);	// MediaTimeFlagOfSpatialRegion
	m_DecoderBuffer->TakeBits(1);
	if(flag)
	{
		for(i=0; i < 2; i++)
		{
			m_DecoderBuffer->GetBitsLong(&ltmp, 32);
			m_DecoderBuffer->TakeBits(32);
			media_time_info[i] = (unsigned int)ltmp;
		}
		m_pVideoSignatureDescriptorInterface->SetMediaTime(media_time_info[0], media_time_info[1]);
	}

	m_DecoderBuffer->GetBitsLong(&ltmp, 32);
	m_DecoderBuffer->TakeBits(32);
	num_of_segments = (unsigned int)ltmp;

	if(m_pVideoSignatureDescriptorInterface->SetNumberOfSegments(num_of_segments)!=0) return -1;

	for(i=0; i<num_of_segments; i++)
	{
		m_DecoderBuffer->GetBitsLong(&ltmp, 32);
		m_DecoderBuffer->TakeBits(32);
		seg_start_frame = (unsigned int)ltmp;

		m_DecoderBuffer->GetBitsLong(&ltmp, 32);
		m_DecoderBuffer->TakeBits(32);
		seg_end_frame = (unsigned int)ltmp;

		flag = m_DecoderBuffer->GetBit(0);
		m_DecoderBuffer->TakeBits(1);
		if(flag)
		{
			for(j=0; j < 2; j++)
			{
				// skip media time
				m_DecoderBuffer->GetBitsLong(&ltmp, 32);
				m_DecoderBuffer->TakeBits(32);
			}
		}

		memset(bags_of_words, 0, SEGMENT_BAGS_SIZE);

		m_DecoderBuffer->GetBits((char*)(&bags_of_words[0]), 243);
		m_DecoderBuffer->TakeBits(243);
		m_DecoderBuffer->GetBits((char*)(&bags_of_words[32]), 243);
		m_DecoderBuffer->TakeBits(243);
		m_DecoderBuffer->GetBits((char*)(&bags_of_words[64]), 243);
		m_DecoderBuffer->TakeBits(243);
		m_DecoderBuffer->GetBits((char*)(&bags_of_words[96]), 243);
		m_DecoderBuffer->TakeBits(243);
		m_DecoderBuffer->GetBits((char*)(&bags_of_words[128]), 243);
		m_DecoderBuffer->TakeBits(243);

		m_pVideoSignatureDescriptorInterface->SetSegment(i, seg_start_frame, seg_end_frame, bags_of_words);
	}

	flag = m_DecoderBuffer->GetBit(0);	// CompressionFlag
	m_DecoderBuffer->TakeBits(1);
	compression_flag = flag;

	num_of_frames = region_frame_info[1];

	for(i=0; i<num_of_frames; i++)
	{

		flag = m_DecoderBuffer->GetBit(0);
		m_DecoderBuffer->TakeBits(1);
		if(flag)
		{
			// skip media time
			m_DecoderBuffer->GetBitsLong(&ltmp, 32);
			m_DecoderBuffer->TakeBits(32);
		}
		m_DecoderBuffer->GetBitsLong(&ltmp, 8);
		m_DecoderBuffer->TakeBits(8);
		confidence = (unsigned int)ltmp;

		m_DecoderBuffer->GetBits((char*)words, FRAME_WORD_COUNT*8);
		m_DecoderBuffer->TakeBits(FRAME_WORD_COUNT*8);

		if(compression_flag == 0)
		{
			m_DecoderBuffer->GetBits((char*)signatures, FRAME_SIGNATURE_SIZE*8);
			m_DecoderBuffer->TakeBits(FRAME_SIGNATURE_SIZE*8);
			sig_ptr = signatures;
		}
		else
		{
			sig_ptr = 0;
		}

		m_pVideoSignatureDescriptorInterface->SetFrame(i, confidence, words, sig_ptr);

		if(key_info_flag){
			RawFrameSignature* p_frame_sig = new RawFrameSignature;
			p_frame_sig->confidence = confidence;
			memcpy(p_frame_sig->framedescriptor,signatures,FRAME_SIGNATURE_SIZE);
			memcpy(p_frame_sig->frameword,words,FRAME_WORD_COUNT);
			p_frame_sig->FrameNum = i;
			p_frame_sig->reserve = 0.0;
			p_frame_sig->reserve1=0;

			_frame_signatures.push_back(p_frame_sig);
		}
	}
	if(compression_flag)
	{
		CTernaryOpsCompress ternary_ops(0,0);
		
		CBinFileFormat mpeg7_signature(S_S, S_0, 76, SEGMENT_WORD_COUNT, num_of_frames, ternary_ops);
		mpeg7_signature.LoadCompressed(m_DecoderBuffer);

		for(i=0; i<num_of_frames; i++)
		{
			unsigned char *p_sig=mpeg7_signature.GetFrameSignature(i);
			m_pVideoSignatureDescriptorInterface->SetFrame(i, 0, 0, p_sig);
		}
	}

	if(key_info_flag){
		vector<INDEX*> KeyFrameIndex;
		vector<INDEX*> AllFrameIndex;

		RawFrameSignature * p_Pre_frame_sig;
		setLUT_L1_DIST();
		std::vector<RawFrameSignature *>::const_iterator it2;

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

		for(it2 = _frame_signatures.begin(),i=0;it2!=_frame_signatures.end();++it2,i++)
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

		for(it2 = _frame_signatures.begin(),i=0;it2!=_frame_signatures.end();++it2,i++)
		{
			RawFrameSignature * p_frame_sig = *it2;
			if(distbuffer.size()==buffersize){
				distbuffer.push_back(p_frame_sig->reserve1);
				distbuffer.erase(distbuffer.begin());

				maxdist = 0;
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

		for(itdis = keypos.begin(),j=0;itdis!=keypos.end();++itdis,j++){
			int pos = *itdis;
			_frame_signatures[pos]->reserve = 1.0;
		}

		int keyconfidence = 0;

		for(it2 = _frame_signatures.begin(),i=0;it2!=_frame_signatures.end();++it2,i++)
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


		for(it2 = _frame_signatures.begin(),i=0;it2!=_frame_signatures.end();++it2,i++)
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

		m_pVideoSignatureDescriptorInterface->SetKeyLookUpTable(keyframesize,KeyMatchingPosition);

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

		m_pVideoSignatureDescriptorInterface->SetKeyIndexTable(FRAME_WORD_COUNT,nIndexContent,KeyIndex);


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

		m_pVideoSignatureDescriptorInterface->SetAllIndexTable(FRAME_WORD_COUNT,nIndexContent,AllIndex);


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

	return 0;
}
