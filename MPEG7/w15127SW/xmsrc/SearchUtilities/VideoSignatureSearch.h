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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignatureSearch.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEOSIGNATURESEARCH_H__
#define __VIDEOSIGNATURESEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

enum MATCH_LEVEL { MATCH, NO_MATCH_L0, NO_MATCH_L1, NO_MATCH_L2, NO_MATCH_L3 };
enum MATCHING_MODE { DIRECT, PARTIAL };

typedef struct {
	enum MATCH_LEVEL matchLevel;

	double	qStartTime;
	double	qEndTime;
	double	oStartTime;
	double	oEndTime;
}VIDEO_MATCH_INFO;

class SEGMENT {
public:
	unsigned int startFrame;
	unsigned int endFrame;
	unsigned char BoW[SEGMENT_WORD_COUNT][SIZE_OF_A_BAG];

	void initBoW()
	{
		for( int i = 0; i < SEGMENT_WORD_COUNT; i++ ){
			for( int j = 0; j < SIZE_OF_A_BAG; j++ ){
				BoW[i][j] = 0;
			}
		}
	};

	void setBoW( int nWord, int word )
	{
		int nByte = word>>3;
		int nBit = word%8;
		unsigned char mask = 1;
		BoW[nWord][nByte] = BoW[nWord][nByte] | (mask<<nBit);
	}

};

class FRAME {
public:
	unsigned char confidence;
	unsigned char *word;		
	unsigned char *signature;
	
	void extractWord( int nWord, int d1, int d2, int d3, int d4, int d5 )
	{
		int nByte, nBit;
		unsigned char b;
		unsigned char ternary[5];

		word[nWord] = 0;

		//d1
		nByte = (d1-1)/5;
		nBit  = (d1-1)-5*nByte;
		b = signature[nByte];
		ternary[0] = b/81; b -= ternary[0]*81;
		ternary[1] = b/27; b -= ternary[1]*27;
		ternary[2] = b/9;  b -= ternary[2]*9;
		ternary[3] = b/3;  b -= ternary[3]*3;
		ternary[4] = b;
		word[nWord] += 81*ternary[nBit];

		//d2
		nByte = (d2-1)/5;
		nBit  = (d2-1)-5*nByte;
		b = signature[nByte];
		ternary[0] = b/81; b -= ternary[0]*81;
		ternary[1] = b/27; b -= ternary[1]*27;
		ternary[2] = b/9;  b -= ternary[2]*9;
		ternary[3] = b/3;  b -= ternary[3]*3;
		ternary[4] = b;
		word[nWord] += 27*ternary[nBit];

		//d3
		nByte = (d3-1)/5;
		nBit  = (d3-1)-5*nByte;
		b = signature[nByte];
		ternary[0] = b/81; b -= ternary[0]*81;
		ternary[1] = b/27; b -= ternary[1]*27;
		ternary[2] = b/9;  b -= ternary[2]*9;
		ternary[3] = b/3;  b -= ternary[3]*3;
		ternary[4] = b;
		word[nWord] += 9*ternary[nBit];

		//d4
		nByte = (d4-1)/5;
		nBit  = (d4-1)-5*nByte;
		b = signature[nByte];
		ternary[0] = b/81; b -= ternary[0]*81;
		ternary[1] = b/27; b -= ternary[1]*27;
		ternary[2] = b/9;  b -= ternary[2]*9;
		ternary[3] = b/3;  b -= ternary[3]*3;
		ternary[4] = b;
		word[nWord] += 3*ternary[nBit];

		//d5
		nByte = (d5-1)/5;
		nBit  = (d5-1)-5*nByte;
		b = signature[nByte];
		ternary[0] = b/81; b -= ternary[0]*81;
		ternary[1] = b/27; b -= ternary[1]*27;
		ternary[2] = b/9;  b -= ternary[2]*9;
		ternary[3] = b/3;  b -= ternary[3]*3;
		ternary[4] = b;
		word[nWord] += ternary[nBit];
	};
};

typedef struct {
	int		groupId;
	int		numSegments;
	int		numFrames;
	double	frameRate;
} VIDEO_INFO;


class VIDEO {
public:

	VIDEO_INFO videoInfo;

	std::vector<SEGMENT> segments;
	std::vector<FRAME> frames;

	int **number_of_key_index;
	unsigned int **key_index;
	int **number_of_all_index;
	unsigned int **all_index;
	unsigned int *key_lookup_table;
	int frame_words_count;

	VIDEO(){
		number_of_key_index = NULL;
		key_index = NULL;
		number_of_all_index = NULL;
		all_index = NULL;
		key_lookup_table = NULL;
		frame_words_count = 0;
	};
	~VIDEO(){ 
		int i;
		if(number_of_key_index != NULL){
			for(i=0;i<frame_words_count;i++){
				delete[] number_of_key_index[i];
				number_of_key_index[i] = NULL;
			}
			delete[] number_of_key_index;
			number_of_key_index = NULL;
		}
		if(key_index != NULL){
			for(i=0;i<frame_words_count;i++){
				delete[] key_index[i];
				key_index[i] = NULL;
			}
			delete[] key_index;
			key_index = NULL;
		}
		if(number_of_all_index != NULL){
			for(i=0;i<frame_words_count;i++){
				delete[] number_of_all_index[i];
				number_of_all_index[i] = NULL;
			}
			delete[] number_of_all_index;
			number_of_all_index = NULL;
		}
		if(all_index != NULL){
			for(i=0;i<frame_words_count;i++){
				delete[] all_index[i];
				all_index[i] = NULL;
			}
			delete[] all_index;
			all_index = NULL;
		}
		if(key_lookup_table != NULL){
			delete[] key_lookup_table;
			key_lookup_table = NULL;
		}
		clearSignature();
	};

	void clearSignature(){ segments.clear(); frames.clear(); };
	void setVideoInfo(int numFrames, int numSegments, double frameRate) { videoInfo.numFrames = numFrames; videoInfo.numSegments = numSegments; videoInfo.frameRate = frameRate; }
	int extractWords( int nWord, int d1, int d2, int d3, int d4, int d5 )
	{
		for( int i = 0; i < (int)frames.size(); i++ )
			frames[i].extractWord( nWord, d1, d2, d3, d4, d5 );

		return 0;
	};

	int extractBoW( int startFrame, int endFrame, SEGMENT &segment )
	{
		segment.startFrame = startFrame;
		segment.endFrame = endFrame;

		segment.initBoW();
		for( int i = startFrame; i <= endFrame; i++ ){
			for( int n = 0; n < SEGMENT_WORD_COUNT;  n++ ){
				segment.setBoW( n, frames[i].word[n] );
			}
		}

		return 0;	
	}

	int extractSegments( int segmentSize, int segmentOffset )
	{
		SEGMENT segment;

		segments.clear();

		for( int startFrame = 0; ; startFrame += segmentOffset ){
			int endFrame = startFrame+segmentSize-1;

			if( endFrame  > (int)frames.size()-1 ){
				extractBoW( startFrame, (int)frames.size()-1, segment );
				segments.push_back( segment );
				break;
			}
			else{
				extractBoW( startFrame, endFrame, segment );
				segments.push_back( segment );
			}
		}

		return 0;
	};

};


class SEGMENT_MATCH_INFO{
public:
	double frameRateRatio;   // frameRateRatio = queryFrameRate / originalFrameRate 

	int queryMatchFrame;
	int originalMatchFrame;

	int intercept;

	int score;	// The higher the better!

	bool operator<(const SEGMENT_MATCH_INFO &seg2 ) const {
		return score < seg2.score;
	}
	
	bool operator>(const SEGMENT_MATCH_INFO &seg2 ) const {
		return score > seg2.score;
	}
};


class FRAME_MATCH_INFO {
public:
	int dist;
	unsigned char confidence1;
	unsigned char confidence2;

	bool operator<(const FRAME_MATCH_INFO &frameMatchInfo ) const {
		return dist < frameMatchInfo.dist;
	}
};

typedef struct {
	int qStartFrame;
	int qEndFrame;
	int oStartFrame;
	int oEndFrame;

	double meanDist;
}SEQUENCE_MATCH_INFO;



//=============================================================================

namespace XM
{


// Forward Declarations:
class VideoSignatureSearchTool;

//=============================================================================
class VideoSignatureSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	VideoSignatureSearchInterfaceABC();
	virtual ~VideoSignatureSearchInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetRefDescriptorInterface
	  (VideoSignatureDescriptorInterfaceABC
	   *aVideoSignatureDescriptorInterface) = 0;
	virtual int SetQueryDescriptorInterface
	  (VideoSignatureDescriptorInterfaceABC
	   *aVideoSignatureDescriptorInterface) = 0;
	virtual double GetDistance(VIDEO_MATCH_INFO &videoMatchInfo) = 0;

	static const UUID myID;
};

//=============================================================================
class VideoSignatureSearchInterface: 
  public VideoSignatureSearchInterfaceABC
{
public:
	VideoSignatureSearchInterface(VideoSignatureSearchTool* aTool);
	virtual ~VideoSignatureSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (VideoSignatureDescriptorInterfaceABC
	   *aVideoSignatureDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (VideoSignatureDescriptorInterfaceABC
	   *aVideoSignatureDescriptorInterface);
	virtual double GetDistance(VIDEO_MATCH_INFO &videoMatchInfo);


	static const UUID myID;
	static const char *myName;

private:
	VideoSignatureSearchTool *m_SearchTool;
};

//=============================================================================
class VideoSignatureSearchTool: public Search
{
friend class VideoSignatureSearchInterface;
public:
	VideoSignatureSearchTool();
	VideoSignatureSearchTool(VideoSignatureDescriptorInterfaceABC
                               *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual VideoSignatureSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (VideoSignatureDescriptorInterfaceABC
	   *aVideoSignatureDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (VideoSignatureDescriptorInterfaceABC
	   *aVideoSignatureDescriptorInterface);
	virtual double GetDistance(VIDEO_MATCH_INFO &videoMatchInfo);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~VideoSignatureSearchTool();

	VideoSignatureSearchInterface m_Interface;
	VideoSignatureDescriptorInterfaceABC *m_RefDescriptorInterface;
	VideoSignatureDescriptorInterfaceABC *m_QueryDescriptorInterface;

};

//=============================================================================

class VideoMatching {
public:
	//parameters for matching
	static enum MATCHING_MODE mode;
	static double	duration;
	static int		TH_JACCARD_SCORE;
	static int		TH_JACCARD_SCORE_WORD;
	//KW: not used: 
	static int		TH_L1_DIST;
	static int		TH_L1_DIST_HOUGH;
	static int		tFrm;
	static double	tHit;

	static int		NUMBER_OF_USED_WORDS;
	static int		NUMBER_OF_MATCHED_FRAME;
	static int		TH_FRAME_DISTANCE;

	//look up table (L1 distance)
	static unsigned char *LUT_L1_DIST;

	

	static bool matchVideo( VIDEO &query, VIDEO &original, VIDEO_MATCH_INFO &videoMatchInfo );
	static bool matchVideoWithIndex( VIDEO &query, VIDEO &original, VIDEO_MATCH_INFO &videoMatchInfo );
	
	static bool segmentMatching1( SEGMENT &segment1, SEGMENT &segment2, int *score, int *scoreWord );
	static bool segmentMatching( SEGMENT &segment1, VIDEO &video1, SEGMENT &segment2, VIDEO &video2, vector<SEGMENT_MATCH_INFO> &segmentMatchList );

	static bool frameByFrameMatching( VIDEO &video1, VIDEO &video2, SEGMENT_MATCH_INFO &segmentMatchInfo, vector<SEQUENCE_MATCH_INFO> &sequenceMatchInfo );
	
	static bool selectOneInterval( VIDEO &query, VIDEO &original, vector<SEQUENCE_MATCH_INFO> &sequenceMatchList, VIDEO_MATCH_INFO &videoMatchInfo );


	static int L1Distance( unsigned char *signature1, unsigned char *signature2 );
	static int L1DistanceHough( unsigned char *signature1, unsigned char *signature2, int th );
	

	static void setLUT_L1_DIST();
	static void freeLUT_L1_DIST();

	static int setParameter( char *parameterFile );
	static void printParameter();

	static int overwriteWords( VIDEO &video );

	static int KEYL1Distance( unsigned char* word1, unsigned char* word2, unsigned char *signature1, unsigned char *signature2 );
};

};

#endif //__VIDEOSIGNATURESEARCH_H__
