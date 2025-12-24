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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignature.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEO_SIGNATURE__
#define __VIDEO_SIGNATURE__

//#===--- Typedefs 

//#===--- Defines


//no of words
#define FRAME_WORD_COUNT 5
#define SEGMENT_WORD_COUNT 5

#define SIZE_OF_A_BAG 32

#define SEGMENT_BAGS_SIZE 160

#define FRAME_SIGNATURE_SIZE 76

#define UPPER_LIMIT 130
#define LOWER_LIMIT 126

#define S_S 90
#define S_0 45


namespace XM
{

//#===-----------------------------------------
//#===--- VideoSignatureDescriptorInterfaceABC

class VideoSignatureDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	VideoSignatureDescriptorInterfaceABC();
	virtual ~VideoSignatureDescriptorInterfaceABC() {}

	virtual void addref()=0;
	virtual void release()=0;

	virtual int SetRegionFrames(unsigned int start_frame, unsigned int nof) = 0;
	//virtual void SetFrameRate(double frameRate) = 0;
	virtual void SetSpatialRegion(unsigned short top_left_x, unsigned short top_left_y,
					unsigned short bottom_right_x, unsigned short bottom_right_y) = 0;
	virtual void SetMediaTimeUnit(unsigned int time_unit) = 0;
	virtual void SetMediaTime(unsigned int start_time, unsigned int end_time) = 0;

	virtual int SetNumberOfSegments(unsigned int nos) = 0;
	virtual void SetSegment(unsigned int pos, unsigned int start_frame, unsigned int end_frame,
					const unsigned char *bag_of_words) = 0;

	virtual void SetFrame(unsigned int pos, unsigned char confidence, const unsigned char *words,
					const unsigned char *frame_signature) = 0;
	virtual void SetCompressionFlag(int flag) = 0;


	virtual unsigned int *GetRegionFrames() = 0;
	virtual unsigned short* GetSpatialRegion() = 0;
	virtual unsigned short GetMediaTimeUnit() = 0;
	virtual unsigned int* GetMediaTime() = 0;
	virtual double GetRegionFrameRate() = 0;
	virtual unsigned int GetMediaTimeOfFrame(unsigned int frame) = 0;

	virtual unsigned int GetNumberOfSegments() = 0;
	virtual unsigned int* GetSegmentStartFrames() = 0;
	virtual unsigned int* GetSegmentEndFrames() = 0;
	virtual unsigned char* GetSegmentBagOfWords() = 0;
	virtual unsigned char* GetFrameConfidences() = 0;
	virtual unsigned char* GetFrameWords() = 0;
	virtual unsigned char* GetFrameSignatures() = 0;
	virtual int GetCompressionFlag() = 0;

	virtual unsigned char* GetKeyFrameInfo() =0;
	virtual int GetKeyFrameInfoFlag()=0;

	virtual int GetNumOfWords() = 0;
	virtual int** GetNumOfKeyIndex() = 0;
	virtual int** GetNumOfAllIndex() = 0;
	virtual unsigned int** GetKeyIndexTable() = 0;
	virtual unsigned int** GetAllIndexTable() = 0;

	virtual unsigned int* GetKeyLookUpTable()=0;

	virtual void SetKeyFrameInfo(unsigned char* pkeyinfo, unsigned int numofframe)=0;
	virtual void SetKeyFrameInfoFlag(int flag)=0;

	virtual void SetKeyIndexTable(unsigned char numofwords, int** nKeyIndexContent, unsigned int** KeyIndex)=0;
	virtual void SetAllIndexTable(unsigned char numofwords, int** nAllIndexContent, unsigned int** AllIndex)=0;

	virtual void SetKeyLookUpTable(unsigned int numberofkey, unsigned int* KeyLookUpTable)=0;


	static const UUID myID;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class VideoSignatureDescriptor;

//#===--------------------------------------
//#===--- VideoSignatureDescriptorInterface

class VideoSignatureDescriptorInterface: public VideoSignatureDescriptorInterfaceABC
{
public:
	VideoSignatureDescriptorInterface(VideoSignatureDescriptor *pVideoSignature);
	virtual ~VideoSignatureDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	static const UUID myID;
	static const char *myName;


	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	virtual int SetRegionFrames(unsigned int start_frame, unsigned int nof);
	//virtual void SetFrameRate(double frameRate);
	virtual void SetSpatialRegion(unsigned short top_left_x, unsigned short top_left_y,
					unsigned short bottom_right_x, unsigned short bottom_right_y);
	virtual void SetMediaTimeUnit(unsigned int time_unit);
	virtual void SetMediaTime(unsigned int start_time, unsigned int end_time);

	virtual int SetNumberOfSegments(unsigned int nos);
	virtual void SetSegment(unsigned int pos, unsigned int start_frame, unsigned int end_frame,
					const unsigned char *bag_of_words);

	virtual void SetFrame(unsigned int pos, unsigned char confidence, const unsigned char *words,
					const unsigned char *frame_signature);
	virtual void SetCompressionFlag(int flag);

	virtual unsigned int *GetRegionFrames();
	virtual unsigned short* GetSpatialRegion();
	virtual unsigned short GetMediaTimeUnit();
	virtual unsigned int* GetMediaTime();
	virtual double GetRegionFrameRate();
	virtual unsigned int GetMediaTimeOfFrame(unsigned int frame);

	virtual unsigned int GetNumberOfSegments();
	virtual unsigned int* GetSegmentStartFrames();
	virtual unsigned int* GetSegmentEndFrames();
	virtual unsigned char* GetSegmentBagOfWords();
	virtual unsigned char* GetFrameConfidences();
	virtual unsigned char* GetFrameWords();
	virtual unsigned char* GetFrameSignatures();
	virtual int GetCompressionFlag();

	virtual unsigned char* GetKeyFrameInfo();
	virtual int GetKeyFrameInfoFlag();

	virtual int GetNumOfWords();
	virtual int** GetNumOfKeyIndex();
	virtual int** GetNumOfAllIndex();
	virtual unsigned int** GetKeyIndexTable();
	virtual unsigned int** GetAllIndexTable();

	virtual unsigned int* GetKeyLookUpTable();

	virtual void SetKeyFrameInfo(unsigned char* keyframeinfo, unsigned int numofframe);
	virtual void SetKeyFrameInfoFlag(int flag);

	virtual void SetKeyIndexTable(unsigned char numofwords, int** nKeyIndexContent, unsigned int** KeyIndex);
	virtual void SetAllIndexTable(unsigned char numofwords, int** nAllIndexContent, unsigned int** AllIndex);

	virtual void SetKeyLookUpTable(unsigned int numberofkey, unsigned int* KeyLookUpTable);

private:
	VideoSignatureDescriptor *m_pVideoSignatureDescriptor;	
};

//#===-----------------------------
//#===--- VideoSignatureDescriptor

class VideoSignatureDescriptor: public Descriptor
{
friend class VideoSignatureDescriptorInterface;
public:
	VideoSignatureDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual VideoSignatureDescriptorInterfaceABC *GetInterface(void);

	// accessor methods
	virtual int SetRegionFrames(unsigned int start_frame, unsigned int nof);
	//virtual void SetFrameRate(double frameRate);
	virtual void SetSpatialRegion(unsigned short top_left_x, unsigned short top_left_y,
					unsigned short bottom_right_x, unsigned short bottom_right_y);
	virtual void SetMediaTimeUnit(unsigned int time_unit);
	virtual void SetMediaTime(unsigned int start_time, unsigned int end_time);

	virtual int SetNumberOfSegments(unsigned int nos);
	virtual void SetSegment(unsigned int pos, unsigned int start_frame, unsigned int end_frame,
					const unsigned char *bag_of_words);

	virtual void SetFrame(unsigned int pos, unsigned char confidence, const unsigned char *words,
					const unsigned char *frame_signature);
	virtual void SetCompressionFlag(int flag);

	virtual unsigned int *GetRegionFrames();
	virtual unsigned short* GetSpatialRegion();
	virtual unsigned short GetMediaTimeUnit();
	virtual unsigned int* GetMediaTime();
	virtual double GetRegionFrameRate();
	virtual unsigned int GetMediaTimeOfFrame(unsigned int frame);

	virtual unsigned int GetNumberOfSegments();
	virtual unsigned int* GetSegmentStartFrames();
	virtual unsigned int* GetSegmentEndFrames();
	virtual unsigned char* GetSegmentBagOfWords();
	virtual unsigned char* GetFrameConfidences();
	virtual unsigned char* GetFrameWords();
	virtual unsigned char* GetFrameSignatures();
	virtual int GetCompressionFlag();

	virtual unsigned char* GetKeyFrameInfo();
	virtual int GetKeyFrameInfoFlag();

	virtual int GetNumOfWords();
	virtual int** GetNumOfKeyIndex();
	virtual int** GetNumOfAllIndex();
	virtual unsigned int** GetKeyIndexTable();
	virtual unsigned int** GetAllIndexTable();

	virtual unsigned int* GetKeyLookUpTable();

	virtual void SetKeyFrameInfo(unsigned char* keyframeinfo, int numofframe);
	virtual void SetKeyFrameInfoFlag(int flag);

	virtual void SetKeyIndexTable(unsigned char numofwords, int** nKeyIndexContent, unsigned int** KeyIndex);
	virtual void SetAllIndexTable(unsigned char numofwords, int** nAllIndexContent, unsigned int** AllIndex);

	virtual void SetKeyLookUpTable(unsigned int numberofkey, unsigned int* KeyLookUpTable);


private:
	virtual ~VideoSignatureDescriptor();

	unsigned long m_refcount;

	VideoSignatureDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data

	// VideoSignatureRegion
	unsigned short *m_SpatialRegion;
	unsigned int m_RegionFrames[2];
	unsigned short m_MediaTimeUnit;
	unsigned int *m_MediaTime;

	//double m_FrameRate;
	unsigned int m_NumberOfSegments;
	unsigned int *m_SegmentStartFrames;
	unsigned int *m_SegmentEndFrames;
	unsigned char * m_SegmentBagOfWords;

	unsigned char * m_FrameConfidences;
	unsigned char * m_FrameWords;
	unsigned char * m_FrameSignatures;

	int m_CompressionFlag;

	// keyframe information
	int m_KeyFrameFlag;
	unsigned char* m_KeyFrameInfo;

	int **m_nKeyIndexContents;
	int **m_nAllIndexContents;
	unsigned int **m_unAllIndex;
	unsigned int **m_unKeyIndex;

	int m_nNumOfWords;

	int m_nNumOfKey;
	unsigned int *m_pKeyLookUpTable;
};

};

class CTernaryOps
{
private:
	

	unsigned char _ter_to_bin_table[1024];
	unsigned int _bin_to_ter_table[256];
	unsigned int _hamming_dist_table[256][256];
	
	
	const int _threshold_u;
	const int _threshold_l;
	
private:
	void init_ter_to_bin_table();
	void init_bin_to_ter_table();
	
	void generate_5_packed_ternary_numbers();
	
	
public:
	CTernaryOps(int upperlimit,int lowerlimit);
	
	static unsigned int Pack5TernaryValuesToA32bitInt(unsigned char * p_ternary_array);
	
	unsigned char EncodeTo8BitBinary(unsigned int packed_ternary_value) const;
	
	unsigned int DecodeToPackedTernaryValue(unsigned char value) const;
	
};


#endif // __VIDEO_SIGNATURE__
