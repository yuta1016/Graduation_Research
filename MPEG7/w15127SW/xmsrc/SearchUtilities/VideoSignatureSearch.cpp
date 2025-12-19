///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K. Wnukowicz, Warsaw Univesity of Technology
// P. Brasnett, Mitsubishi Electric R&D Centre Europe
// K. Iwamoto, R. Oami, NEC Corp.
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
// Applicable File Name:  VideoSignatureSearch.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <cstring>
#include <float.h>
#include <vector>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"					 // Naming Convention
#include "SearchUtilities/SearchUtilities.h"

const int NUM_DIMENSIONS = 380;
const int SIGNATURE_BYTES = NUM_DIMENSIONS/5;
const int NUM_WORDS = 5;

#define HOUGH_MAX_ABS_OFFSET 90
// HOUGH_MAX_OFFSET = 2*HOUGH_MAX_ABS_OFFSET + 1
#define HOUGH_MAX_OFFSET 181
#define TEST_MATCH_STEP 5

void initVIDEO(VIDEO &video, unsigned int num_of_frames, unsigned int num_of_segments, double frame_rate,
			   unsigned int *start_frames, unsigned int *end_frames, unsigned char *bag_of_words,
			   unsigned char *confidences, unsigned char *words, unsigned char *signatures)
{


	unsigned int i, j, k;
	video.setVideoInfo(num_of_frames, num_of_segments, frame_rate);
	SEGMENT segment;
	FRAME frame;

	for( i = 0; i < num_of_segments; i++ ){
		segment.startFrame = start_frames[i];
		segment.endFrame = end_frames[i];
		for(j=0; j< SEGMENT_WORD_COUNT; j++)
		{
			for(k=0; k< SIZE_OF_A_BAG; k++)
			{
				segment.BoW[j][k] = bag_of_words[i*SEGMENT_BAGS_SIZE + j*SIZE_OF_A_BAG + k];
			}
		}
		video.segments.push_back(segment);
	}
	for( i = 0; i < num_of_frames; i++ ){
		frame.confidence = confidences[i];
		frame.word = words + i*FRAME_WORD_COUNT;
		frame.signature = signatures + i*FRAME_SIGNATURE_SIZE;
		video.frames.push_back(frame);
	}
}

void initVIDEOwithIndex(VIDEO &video, unsigned int num_of_frames, unsigned int num_of_segments, double frame_rate,
			   unsigned int *start_frames, unsigned int *end_frames, unsigned char *bag_of_words,
			   unsigned char *confidences, unsigned char *words, unsigned char *signatures,
			   int** numkeyindex, unsigned int** keyindex, int** numallindex, unsigned int** allindex, unsigned int* keylookuptable)
{
	unsigned int i, j, k;
	video.setVideoInfo(num_of_frames, num_of_segments, frame_rate);
	SEGMENT segment;
	FRAME frame;

	for( i = 0; i < num_of_segments; i++ ){
		segment.startFrame = start_frames[i];
		segment.endFrame = end_frames[i];
		for(j=0; j< SEGMENT_WORD_COUNT; j++)
		{
			for(k=0; k< SIZE_OF_A_BAG; k++)
			{
				segment.BoW[j][k] = bag_of_words[i*SEGMENT_BAGS_SIZE + j*SIZE_OF_A_BAG + k];
			}
		}
		video.segments.push_back(segment);
	}
	for( i = 0; i < num_of_frames; i++ ){
		frame.confidence = confidences[i];
		frame.word = words + i*FRAME_WORD_COUNT;
		frame.signature = signatures + i*FRAME_SIGNATURE_SIZE;
		video.frames.push_back(frame);
	}

	if(video.number_of_key_index != NULL){
		for(i=0;i<NUM_WORDS;i++){
			delete[] video.number_of_key_index[i];
			video.number_of_key_index[i] = NULL;
		}
		delete[] video.number_of_key_index;
		video.number_of_key_index = NULL;
	}
	if(video.key_index != NULL){
		for(i=0;i<NUM_WORDS;i++){
			delete[] video.key_index[i];
			video.key_index[i] = NULL;
		}
		delete[] video.key_index;
		video.key_index = NULL;
	}
	if(video.number_of_all_index != NULL){
		for(i=0;i<NUM_WORDS;i++){
			delete[] video.number_of_all_index[i];
			video.number_of_all_index[i] = NULL;
		}
		delete[] video.number_of_all_index;
		video.number_of_all_index = NULL;
	}
	if(video.all_index != NULL){
		for(i=0;i<NUM_WORDS;i++){
			delete[] video.all_index[i];
			video.all_index[i] = NULL;
		}
		delete[] video.all_index;
		video.all_index = NULL;
	}
	if(video.key_lookup_table != NULL){
		delete[] video.key_lookup_table;
		video.key_lookup_table = NULL;
	}

	video.number_of_key_index = new int* [NUM_WORDS];
	video.number_of_all_index = new int* [NUM_WORDS];
	video.key_index = new unsigned int* [NUM_WORDS];
	video.all_index = new unsigned int* [NUM_WORDS];

	for(i=0;i<NUM_WORDS;i++){
		video.number_of_key_index[i] = new int [244];
		memcpy(video.number_of_key_index[i],numkeyindex[i],244*sizeof(int));
		video.number_of_all_index[i] = new int [244];
		memcpy(video.number_of_all_index[i],numallindex[i],244*sizeof(int));

		video.key_index[i] = new unsigned int [video.number_of_key_index[i][243]];
		memcpy(video.key_index[i],keyindex[i],video.number_of_key_index[i][243]*sizeof(unsigned int));
		video.all_index[i] = new unsigned int [video.number_of_all_index[i][243]];
		memcpy(video.all_index[i],allindex[i],video.number_of_all_index[i][243]*sizeof(unsigned int));
	}
	video.key_lookup_table = new unsigned int [video.number_of_key_index[0][243]];
	memcpy(video.key_lookup_table,keylookuptable,video.number_of_key_index[0][243]*sizeof(unsigned int));

}

//=============================================================================

using namespace XM;


//default parameter settings
enum MATCHING_MODE VideoMatching::mode		= DIRECT;
double VideoMatching::duration				= 10.0;

int VideoMatching::TH_JACCARD_SCORE			= 60000;
int VideoMatching::TH_JACCARD_SCORE_WORD	= 9000;
int VideoMatching::TH_L1_DIST				= 336;
int VideoMatching::TH_L1_DIST_HOUGH			= 116;
int VideoMatching::tFrm						= 0;
double VideoMatching::tHit					= 0.5;

int SEGMENT_SIZE = 90;
int SEGMENT_OFFSET = 45;

int	VideoMatching::NUMBER_OF_USED_WORDS=3;
int	VideoMatching::NUMBER_OF_MATCHED_FRAME=1;
int VideoMatching::TH_FRAME_DISTANCE = 150;

unsigned char *VideoMatching::LUT_L1_DIST	= NULL;

struct PAIR
{
	int a;
	int b;
	int d;
};

static int vidCostTable[256] =
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};


int unionBoW( unsigned char *bow1, unsigned char *bow2 )
{
	int val = 0;
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1++ | *bow2++ ];
	val += vidCostTable[ *bow1 | *bow2 ];

	return val;
}

int intersectionBoW( unsigned char *bow1, unsigned char *bow2 )
{
	int val = 0;
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1++ & *bow2++ ];
	val += vidCostTable[ *bow1 & *bow2 ];

	return val;
}

bool VideoMatching::matchVideo( VIDEO &query, VIDEO &original, VIDEO_MATCH_INFO &videoMatchInfo )
{
	if( mode == DIRECT ){
		tFrm = (int)query.frames.size();
	}
	else{ 
		tFrm = (int)(duration * original.videoInfo.frameRate);
	}


	vector<SEGMENT_MATCH_INFO> segmentMatchList;

	int nsm1 = 0;
	for( int m = 0; m < (int)query.segments.size(); m++ ){
		for( int n = 0; n < (int)original.segments.size(); n++ ){
			
			int score;
			int scoreWord[NUM_WORDS];

			if( segmentMatching1( query.segments[m], original.segments[n], &score, scoreWord ) == true ){
				nsm1++;
				VideoMatching::segmentMatching( query.segments[m], query, original.segments[n], original, segmentMatchList );
			}
		}
	}

	if( nsm1 == 0 ){
		return false;
	}

	if( segmentMatchList.size() == 0 ){
		return false;
	}

	vector<SEQUENCE_MATCH_INFO> sequenceMatchList;

	for( int i = 0; i < (int)segmentMatchList.size(); i++ ){
		if( frameByFrameMatching( query, original, segmentMatchList[i], sequenceMatchList ) == true ){
		}
	}

	if( sequenceMatchList.size() == 0 ){
		return false;
	}


	return selectOneInterval( query, original, sequenceMatchList, videoMatchInfo );
}

bool VideoMatching::matchVideoWithIndex( VIDEO &query, VIDEO &original, VIDEO_MATCH_INFO &videoMatchInfo )
{
	if( mode == DIRECT ){
		tFrm = (int)query.frames.size();
	}
	else{ 
		tFrm = (int)(duration * original.videoInfo.frameRate);
	}

	int KEYFRAMENUMTH;

	if(duration>=100){
		KEYFRAMENUMTH = 5;
	}
	else{
		KEYFRAMENUMTH=1;
	}

	int i,j,i1,k,l;
	int distance;
	int matchingpair=0;
	unsigned char KeyPositionBuffer[1000]={0,};
	for(i=0;i<243;i++){
		///////////////////////////////////////////////////////////////////////////////////////////
		//0
		i1 = i+1;
		for(l=0;l<VideoMatching::NUMBER_OF_USED_WORDS;l++){
			for(j=query.number_of_key_index[l][i];j<query.number_of_key_index[l][i1];j++){
				if(original.number_of_all_index[l][i] != original.number_of_all_index[l][i1]){
					if(KeyPositionBuffer[query.key_index[l][j]]==0){
						KeyPositionBuffer[query.key_index[l][j]]=1;
					}
					else if(KeyPositionBuffer[query.key_index[l][j]] == 1){
						for(k=original.number_of_all_index[l][i];k<original.number_of_all_index[l][i1];k++){
							distance = KEYL1Distance(query.frames[query.key_lookup_table[query.key_index[l][j]]].word,original.frames[original.all_index[l][k]].word
								,query.frames[query.key_lookup_table[query.key_index[l][j]]].signature,original.frames[original.all_index[l][k]].signature);

							if(distance<VideoMatching::TH_FRAME_DISTANCE){
								KeyPositionBuffer[query.key_index[l][j]]=100;
								matchingpair++;
								break;
							}
						}
					}
				}
			}
		}

	}
	if(matchingpair<VideoMatching::NUMBER_OF_MATCHED_FRAME){
		return false;
	}


	vector<SEGMENT_MATCH_INFO> segmentMatchList;

	int nsm1 = 0;
	for( int m = 0; m < (int)query.segments.size(); m++ ){
		for( int n = 0; n < (int)original.segments.size(); n++ ){
			
			int score;
			int scoreWord[NUM_WORDS];

			if( segmentMatching1( query.segments[m], original.segments[n], &score, scoreWord ) == true ){
				nsm1++;
				VideoMatching::segmentMatching( query.segments[m], query, original.segments[n], original, segmentMatchList );
			}
		}
	}

	if( nsm1 == 0 ){
		return false;
	}

	if( segmentMatchList.size() == 0 ){
		return false;
	}

	vector<SEQUENCE_MATCH_INFO> sequenceMatchList;

	for( i = 0; i < (int)segmentMatchList.size(); i++ ){
		if( frameByFrameMatching( query, original, segmentMatchList[i], sequenceMatchList ) == true ){
		}
	}

	if( sequenceMatchList.size() == 0 ){
		return false;
	}


	return selectOneInterval( query, original, sequenceMatchList, videoMatchInfo );
}


bool VideoMatching::segmentMatching1( SEGMENT &segment1, SEGMENT &segment2, int *score, int *scoreWord )
{
	int intersection;
	int uni;

	*score = 0;
	int support = 0;

	for( int n = 0; n < NUM_WORDS; n++ ){
		intersection = intersectionBoW( segment1.BoW[n], segment2.BoW[n] );
		if( intersection > 0 ){
			uni = unionBoW( segment1.BoW[n], segment2.BoW[n] );
			scoreWord[n] = (intersection<<16)/uni;
			*score += scoreWord[n];

			/////
			if( scoreWord[n] > TH_JACCARD_SCORE_WORD ){
				support++;
			}
			/////
		}
		else{
			scoreWord[n] = 0;
		}

		/////word5
		if( n >= 2 && support < n-1 ){
			return false;
		}

	}

	if( *score <= TH_JACCARD_SCORE ){
		return false;
	}

	return true;
}

int VideoMatching::L1Distance( unsigned char *signature1, unsigned char *signature2 )
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

int VideoMatching::L1DistanceHough( unsigned char *signature1, unsigned char *signature2, int th )
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
	if( dist > th )		return dist;

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
	if( dist > th )		return dist;

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
	if( dist > th )		return dist;

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
	if( dist > th )		return dist;

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
	if( dist > th )		return dist;

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
	if( dist > th )		return dist;

	// 340 trits
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
	dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];	

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

int VideoMatching::KEYL1Distance( unsigned char* word1, unsigned char* word2, unsigned char *signature1, unsigned char *signature2 )
{
	int dist = 0;
	int i;
	for(i=0;i<5;i++){
		dist += LUT_L1_DIST[((*word1++)<<8)+(*word2++)];
	}
	if(dist>15){
		dist = 1000;
	}
	else{
		dist = 0;
		for(i = 0; i < 68; i++ ){
			dist += LUT_L1_DIST[((*signature1++)<<8)+(*signature2++)];
		}
	}

	return dist;
}


bool VideoMatching::frameByFrameMatching( VIDEO &query, VIDEO &original, SEGMENT_MATCH_INFO &segmentMatchInfo, vector<SEQUENCE_MATCH_INFO> &sequenceMatchList )
{
	SEQUENCE_MATCH_INFO sequenceMatchInfo;

	int qsf = segmentMatchInfo.queryMatchFrame;
	int osf = segmentMatchInfo.originalMatchFrame;

	if( segmentMatchInfo.frameRateRatio == 0.0 ) return false;
	if( qsf >= (int)query.frames.size() || osf >= (int)original.frames.size() ) return false;

	double qi, oi;
	if( segmentMatchInfo.frameRateRatio >= 1.0 ){ // qFrameRate >= oFrameRate   ( frameRateRatio = qFrameRate / oFrameRate )
		qi = segmentMatchInfo.frameRateRatio;
		oi = 1.0;
	}
	else{ // qFrameRate < oFrameRate
		qi = 1.0;
		oi = 1.0/segmentMatchInfo.frameRateRatio;
	}


	double qod = 0.5;
	double ood = 0.5;
	int nfm = 0;

	int nc = 0;
	int nm = 0;

	int sd = 0;
	int nlcf1 = 0;
	int nlcf2 = 0;


	while(1){ //(int)qOffsetDbl < query.numFrame

		if( qsf+(int)qod >= (int)query.frames.size() || osf+(int)ood >= (int)original.frames.size() ){ //
			qod -= qi * (double)(nfm+1);
			ood -= oi * (double)(nfm+1);
			break;
		}

		int qf = qsf+(int)qod;
		int of = osf+(int)ood;
		int dist = L1Distance( query.frames[qf].signature, original.frames[of].signature );

		nc++;

		if( dist > TH_L1_DIST ){

			if( query.frames[qf].confidence >= 1 || original.frames[of].confidence >= 1 ){
				nfm++;
			}

			if( nfm > 2 ){
				qod -= qi * (double)nfm;		//added 2009.05.21
				ood -= oi * (double)nfm;		//added 2009.05.21
				break;
			}
		}
		else{
			sd += dist;
			if( query.frames[qf].confidence < 1 ) nlcf1++;
			if( original.frames[of].confidence < 1 ) nlcf2++;

			nm++;

			nfm = 0;
		}

		qod += qi;
		ood += oi;
	}
	int qop = (int)qod;
	int oop = (int)ood;


	qod = -0.5 - qi;
	ood = -0.5 - oi;
	nfm = 0;
	

	while(1){

		if( qsf+(int)qod < 0 || osf+(int)ood < 0 ){ //
			qod += qi * (double)(nfm+1);
			ood += oi * (double)(nfm+1);								
			break;
		}

		int qf = qsf+(int)qod;
		int of = osf+(int)ood;
		int dist = L1Distance( query.frames[qf].signature, original.frames[of].signature );

		nc++;

		if( dist > TH_L1_DIST ){

			if( query.frames[qf].confidence >= 1 || original.frames[of].confidence >= 1 ){
				nfm++;
			}

			if( nfm > 2 ){
				qod += qi * (double)nfm;
				ood += oi * (double)nfm;
				break;
			}
		}
		else{
			sd += dist;
			if( query.frames[qf].confidence < 1 ) nlcf1++;
			if( original.frames[of].confidence < 1 ) nlcf2++;

			nm++;

			nfm = 0;
		}

		qod -= qi;
		ood -= oi;
	}
	int qon = (int)qod;
	int oon = (int)ood;



	if( mode == DIRECT ){
		if( (qop-qon)+1 < tFrm ){ //
			return false;
		}
	}
	else if( mode == PARTIAL ){

		if( segmentMatchInfo.frameRateRatio < 1.0 ){ // qFrameRate < oFrameRate
			int thMatchNumFrameQuery = (int)(duration * original.videoInfo.frameRate * segmentMatchInfo.frameRateRatio );
			int fm = (int)(0.08333 * original.videoInfo.frameRate * segmentMatchInfo.frameRateRatio );

			if( (qop-qon)+1 < thMatchNumFrameQuery - fm ){ //
				return false;
			}
		}
		else{
			int fm = (int)(0.08333 * original.videoInfo.frameRate );

			if( (oop-oon)+1 < tFrm - fm ){ //
				return false;
			}
		}
	}


	if( ((double)nm/(double)nc) < tHit ){
		return false;
	}


	if( (double)nm*0.5 <= MAX( nlcf1, nlcf2 ) ){	//
		return false;
	}

	
	sequenceMatchInfo.qStartFrame = qsf+qon;
	sequenceMatchInfo.qEndFrame = qsf+qop;
	sequenceMatchInfo.oStartFrame = osf+oon;
	sequenceMatchInfo.oEndFrame = osf+oop;



	sequenceMatchInfo.meanDist = (double)sd/(double)nm;

	sequenceMatchList.push_back( sequenceMatchInfo );

	return true;
}

bool VideoMatching::selectOneInterval( VIDEO &query, VIDEO &original, vector<SEQUENCE_MATCH_INFO> &sequenceMatchList, VIDEO_MATCH_INFO &videoMatchInfo )
{
	int index = -1;

	double mmd = 9999.0;
	for( int i = 0; i < (int)sequenceMatchList.size(); i++ ){
		double md = sequenceMatchList[i].meanDist;

		if( md < mmd ){
			mmd = md;
			index = i;
		}
	}

	if( mode == PARTIAL ){	
		int mql = -1;
		int rql = sequenceMatchList[index].qEndFrame - sequenceMatchList[index].qStartFrame;

		for( int i = 0; i < (int)sequenceMatchList.size(); i++ ){
			double md = sequenceMatchList[i].meanDist;
			int ql = sequenceMatchList[i].qEndFrame - sequenceMatchList[i].qStartFrame;
			
			if( ql > rql+20 && ql > mql && md < (mmd*1.4) ){
				mql = ql;
				index = i;
			}
		}
	}

	if( index == -1 ){	//	
		return false;
	}

	videoMatchInfo.matchLevel = MATCH;
	videoMatchInfo.qStartTime = (double)sequenceMatchList[index].qStartFrame/query.videoInfo.frameRate;
	videoMatchInfo.qEndTime = (double)sequenceMatchList[index].qEndFrame/query.videoInfo.frameRate;
	videoMatchInfo.oStartTime = (double)sequenceMatchList[index].oStartFrame/original.videoInfo.frameRate;
	videoMatchInfo.oEndTime = (double)sequenceMatchList[index].oEndFrame/original.videoInfo.frameRate;

	return true;
}


void VideoMatching::setLUT_L1_DIST()
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

void VideoMatching::freeLUT_L1_DIST()
{
	if( LUT_L1_DIST != NULL ){
		delete [] LUT_L1_DIST;
		LUT_L1_DIST = NULL;
	}
}


void linearModel(PAIR p1, PAIR p2, double &m, double &c)
{
	m=(double)(p2.a-p1.a)/(double)(p2.b-p1.b);
	c=p2.a- (m*p2.b);
}

bool VideoMatching::segmentMatching( SEGMENT &segment1, VIDEO &video1, SEGMENT &segment2, VIDEO &video2, vector<SEGMENT_MATCH_INFO> &segmentMatchList )
{
	int i, j;
	int dd[ 90 ][ 90 ];
	int max[90][92];
	int hh[60][HOUGH_MAX_OFFSET];
	int hh_fr[60][HOUGH_MAX_OFFSET];
	int hh_fr1[60][HOUGH_MAX_OFFSET];
	int hh_fr2[60][HOUGH_MAX_OFFSET];
	int _segment_size = segment1.endFrame - segment1.startFrame;

	if((segment2.endFrame - segment2.startFrame) > _segment_size) _segment_size = segment2.endFrame - segment2.startFrame;
	_segment_size++;

	for(i=0;i<_segment_size;i++)
		for(j=0;j<_segment_size+2;j++)
			max[i][j]=99999;

	double ofr = video2.videoInfo.frameRate;


	int min_dist=TH_L1_DIST_HOUGH;

	for( i = segment1.startFrame; i <= segment1.endFrame; i+=TEST_MATCH_STEP ){
		unsigned char *sig1=video1.frames[i].signature;
		int ii = i-segment1.startFrame;
		for( j = segment2.startFrame; j <= segment2.endFrame; j++ ){
			int dist = L1DistanceHough( sig1, video2.frames[j].signature, TH_L1_DIST_HOUGH );
			dd[ j - segment2.startFrame ][ ii ] = dist;
			if(dist> TH_L1_DIST_HOUGH) continue;
			if(dist<min_dist)
				min_dist=dist;
			if( dist < max[ii][0] )
			{
				max[ii][0]=dist;
				max[ii][1]=j-segment2.startFrame;				
			}
		}
	}


	if(min_dist==TH_L1_DIST_HOUGH)
		return false;

	for(i=0;i<60;i++)
		memset(hh[i], 0, HOUGH_MAX_OFFSET*sizeof(int));

	for(i=0;i<60;i++)
	{
		memset(hh_fr1[i], 0, HOUGH_MAX_OFFSET*sizeof(int));
		memset(hh_fr2[i], 0, HOUGH_MAX_OFFSET*sizeof(int));
		for(int j=0;j<HOUGH_MAX_OFFSET;j++)
		{
			hh_fr[i][j]=99999;
		}
	}

	for( i = segment1.startFrame+1; i <= segment1.endFrame; i++ ){
		unsigned char *sig1=video1.frames[i].signature;
		int ii = i-segment1.startFrame;
		for( j = segment2.startFrame; j <= segment2.endFrame; j++ ){
			int dist;
			if((ii%TEST_MATCH_STEP) == 0)
			{
				dist = dd[ j - segment2.startFrame ][ ii ];
			}
			else
			{
				dist = L1DistanceHough( sig1, video2.frames[j].signature, TH_L1_DIST_HOUGH );
				dd[ j - segment2.startFrame ][ i - segment1.startFrame ] = dist;
			}
			if(dist> TH_L1_DIST_HOUGH) continue;

			if( dist < max[ii][0] )
			{
				max[ii][0]=dist;
				max[ii][1]=j-segment2.startFrame;				
			}
		}
	}
	
	vector<int> v_dist;

	for(i=0;i<_segment_size;i++)
	{
		int idx=2;
		v_dist.push_back(max[i][0]);

		if(max[i][0] == 99999) continue;
		for(j=(max[i][1]+1);j<_segment_size;j++)
		{
			if(dd[j][i]==max[i][0])
			{
				max[i][idx]=j;
				idx++;
			}
		}
	}

	//sort(v_dist.begin(),v_dist.end());

	int upper_threshold=v_dist[_segment_size-1];

	vector<PAIR> v_pair;

	for(i=0;i<_segment_size;i++)
	{
		if( (max[i][0]<=upper_threshold) && (max[i][0]<TH_L1_DIST_HOUGH))
		{
			PAIR p_buf;
			p_buf.a=i;
			p_buf.b=max[i][1];
			p_buf.d=max[i][0];

			int cnt=1; // KW: cnt = 2;?
			while( (max[i][cnt]!=99999))
			{
				cnt++;
			}

			if(cnt<12)
			{
				v_pair.push_back(p_buf);

				int idx=2;
				while( (max[i][idx]!=99999) && (idx<_segment_size))
				{
					PAIR p_buf;
					p_buf.a=i;
					p_buf.b=max[i][idx];
					p_buf.d=max[i][0];

					v_pair.push_back(p_buf);
					idx++;
				}
			}
		}
	}

	// 2D Hough Transform
	for(i=0;i<v_pair.size();i++)
	{
		for(j=(i+1);j<v_pair.size();j++)
		{
			if( (v_pair[j].b!=v_pair[i].b) && (v_pair[j].a!=v_pair[i].a) )
			{
				double m;
				double c;

				linearModel(v_pair[j],v_pair[i],m,c);

				int frameRate = (int)(m*ofr+0.5);
				double d_offset;
				d_offset = -c;
				//d_offset=-(c/m);
				int offset;
				if(d_offset<0)
					offset=(int)(d_offset-0.5);
				else
					offset=(int)(d_offset+0.5);

				if( frameRate>0 && frameRate <=60)
				{
					if(abs(offset)<=HOUGH_MAX_ABS_OFFSET)
					{
						if(v_pair[i].d < v_pair[j].d)
						{
							if(v_pair[i].d < hh_fr[frameRate-1][offset+90])
							{
								hh_fr[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET] = v_pair[i].d;
								hh_fr1[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET] = v_pair[i].a;
								hh_fr2[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET] = v_pair[i].b;
							}
						}
						else
						{
							if(v_pair[j].d < hh_fr[frameRate-1][offset+90])
							{
								hh_fr[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET] = v_pair[j].d;
								hh_fr1[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET] = v_pair[j].a;
								hh_fr2[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET] = v_pair[j].b;
							}
						}
						hh[frameRate-1][offset+HOUGH_MAX_ABS_OFFSET]++;
					}
				}
			}
		}
	}

	PAIR hh_max;
	hh_max.d=0;
	for(i=0;i<60;i++)
	{
		for(j=0;j<HOUGH_MAX_OFFSET;j++)
		{
			if(hh_max.d<hh[i][j])
			{
				hh_max.a=i;
				hh_max.b=j;
				hh_max.d = hh[i][j];
			}
		}
	}

	if(hh_max.d>0)
	{
		int hh_max_limit = (int)(0.7*hh_max.d);
		for(i=0;i<60;i++)
		{
			for(j=0;j<HOUGH_MAX_OFFSET;j++)
			{
				if(hh_max_limit<hh[i][j])
				{					
					SEGMENT_MATCH_INFO segMatchInfo;

					segMatchInfo.frameRateRatio = ((i+1.0)/ofr);
					segMatchInfo.intercept = (j-90);

					segMatchInfo.originalMatchFrame = segment2.startFrame+hh_fr2[i][j];
					segMatchInfo.queryMatchFrame = segment1.startFrame+hh_fr1[i][j];

					segMatchInfo.score = hh[i][j];

					if( segMatchInfo.originalMatchFrame >=0)
						segmentMatchList.push_back( segMatchInfo );
				}
			}
		}
	}

	return false;
}


//=============================================================================


const UUID VideoSignatureSearchInterface::myID = UUID("");
const char *VideoSignatureSearchInterface::myName = "Video Signature Matching Interface";

const UUID VideoSignatureSearchTool::myID = UUID("");
const char *VideoSignatureSearchTool::myName = "Video Signature Matching Tool";

const UUID VideoSignatureSearchInterfaceABC::myID = UUID();

//=============================================================================
VideoSignatureSearchInterfaceABC::VideoSignatureSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
VideoSignatureSearchInterface::VideoSignatureSearchInterface(VideoSignatureSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
VideoSignatureSearchInterface::~VideoSignatureSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& VideoSignatureSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoSignatureSearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void VideoSignatureSearchInterface::destroy(void)
{
	delete m_SearchTool;
}

//----------------------------------------------------------------------------
int VideoSignatureSearchInterface::
SetRefDescriptorInterface(VideoSignatureDescriptorInterfaceABC
						  *RefVideoSignatureDescriptorInterface)
						  
{
	return m_SearchTool->
		SetRefDescriptorInterface(RefVideoSignatureDescriptorInterface);
}

//----------------------------------------------------------------------------
int VideoSignatureSearchInterface::
SetQueryDescriptorInterface(VideoSignatureDescriptorInterfaceABC
							*QueryVideoSignatureDescriptorInterface)
							
{
	return m_SearchTool->
		SetQueryDescriptorInterface(QueryVideoSignatureDescriptorInterface);
}

//----------------------------------------------------------------------------
double VideoSignatureSearchInterface::GetDistance(VIDEO_MATCH_INFO &videoMatchInfo)

{
	return m_SearchTool->GetDistance(videoMatchInfo);
}



//=============================================================================
VideoSignatureSearchTool::VideoSignatureSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
VideoSignatureSearchTool::
VideoSignatureSearchTool(VideoSignatureDescriptorInterfaceABC
						 *aQueryDescriptorInterface)
						 : m_Interface(this)
						 , m_RefDescriptorInterface(0)
						 , m_QueryDescriptorInterface(0)
{
#ifdef VERBOSE
	fprintf(stderr, "Connect descriptor\n");
#endif
	
	if (aQueryDescriptorInterface)
	{
		VideoSignatureDescriptor *descriptor = new VideoSignatureDescriptor();
		aQueryDescriptorInterface = descriptor->GetInterface();
	}
	
	SetQueryDescriptorInterface(aQueryDescriptorInterface);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
VideoSignatureSearchTool::~VideoSignatureSearchTool()
{
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& VideoSignatureSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoSignatureSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool VideoSignatureSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool VideoSignatureSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int VideoSignatureSearchTool::
SetRefDescriptorInterface(VideoSignatureDescriptorInterfaceABC
						  *aRefVideoSignatureDescriptorInterface)
						  
{
	if (m_RefDescriptorInterface ==
		aRefVideoSignatureDescriptorInterface) return 0;
	
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	
	m_RefDescriptorInterface = aRefVideoSignatureDescriptorInterface;
	if (m_RefDescriptorInterface)
	{
		m_RefDescriptorInterface->addref();
	}
	else
	{
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int  VideoSignatureSearchTool::
SetQueryDescriptorInterface(VideoSignatureDescriptorInterfaceABC
							*aQueryVideoSignatureDescriptorInterface)
							
{
	if (m_QueryDescriptorInterface ==
		aQueryVideoSignatureDescriptorInterface) return 0;
	
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
	
	m_QueryDescriptorInterface = aQueryVideoSignatureDescriptorInterface;
	if (m_QueryDescriptorInterface)
	{
		m_QueryDescriptorInterface->addref();
	}
	else
	{
		return -1;
	}
	
	return 0;
}


//----------------------------------------------------------------------------
double VideoSignatureSearchTool::GetDistance(VIDEO_MATCH_INFO &videoMatchInfo)

{
	if (!m_RefDescriptorInterface) return DBL_MAX;
	if (!m_QueryDescriptorInterface) return DBL_MAX;
	
	if (strcmp(m_RefDescriptorInterface->GetName(),
		"Video Signature Description Interface") != 0)
	{
		return DBL_MAX;
	}
	
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		"Video Signature Description Interface") != 0)
	{
		return DBL_MAX;
	}
	unsigned int *region_frame_info = m_RefDescriptorInterface->GetRegionFrames();
	unsigned int ref_num_of_frames = region_frame_info[1];
	unsigned int ref_num_of_segments = m_RefDescriptorInterface->GetNumberOfSegments();
	double ref_frame_rate = m_RefDescriptorInterface->GetRegionFrameRate();

	unsigned int* ref_start_frames = m_RefDescriptorInterface->GetSegmentStartFrames();
	unsigned int* ref_end_frames = m_RefDescriptorInterface->GetSegmentEndFrames();
	unsigned char* ref_bag_of_words = m_RefDescriptorInterface->GetSegmentBagOfWords();

	unsigned char* ref_confidences = m_RefDescriptorInterface->GetFrameConfidences();
	unsigned char* ref_words = m_RefDescriptorInterface->GetFrameWords();
	unsigned char* ref_signatures = m_RefDescriptorInterface->GetFrameSignatures();

	int** ref_numberof_keyindex = m_RefDescriptorInterface->GetNumOfKeyIndex();
	unsigned int** ref_key_index = m_RefDescriptorInterface->GetKeyIndexTable();
	int** ref_numberof_allindex = m_RefDescriptorInterface->GetNumOfAllIndex();
	unsigned int** ref_all_index = m_RefDescriptorInterface->GetAllIndexTable();
	unsigned int* ref_key_lookup_table = m_RefDescriptorInterface->GetKeyLookUpTable();
	int ref_key_flag = m_RefDescriptorInterface->GetKeyFrameInfoFlag();

	region_frame_info = m_QueryDescriptorInterface->GetRegionFrames();
	unsigned int query_num_of_frames = region_frame_info[1];
	unsigned int query_num_of_segments = m_QueryDescriptorInterface->GetNumberOfSegments();
	double query_frame_rate = m_QueryDescriptorInterface->GetRegionFrameRate();

	unsigned int* query_start_frames = m_QueryDescriptorInterface->GetSegmentStartFrames();
	unsigned int* query_end_frames = m_QueryDescriptorInterface->GetSegmentEndFrames();
	unsigned char* query_bag_of_words = m_QueryDescriptorInterface->GetSegmentBagOfWords();

	unsigned char* query_confidences = m_QueryDescriptorInterface->GetFrameConfidences();
	unsigned char* query_words = m_QueryDescriptorInterface->GetFrameWords();
	unsigned char* query_signatures = m_QueryDescriptorInterface->GetFrameSignatures();

	int** query_numberof_keyindex = m_QueryDescriptorInterface->GetNumOfKeyIndex();
	unsigned int** query_key_index = m_QueryDescriptorInterface->GetKeyIndexTable();
	int** query_numberof_allindex = m_QueryDescriptorInterface->GetNumOfAllIndex();
	unsigned int** query_all_index = m_QueryDescriptorInterface->GetAllIndexTable();
	unsigned int* query_key_lookup_table = m_QueryDescriptorInterface->GetKeyLookUpTable();

	int index_flag = m_QueryDescriptorInterface->GetKeyFrameInfoFlag();
	
	if( VideoMatching::mode == PARTIAL && VideoMatching::duration > 3.0 ){
		VideoMatching::tHit = 0.7;
	}
	else{
		VideoMatching::tHit = 0.5;
	}

	
	VIDEO original;
	VIDEO query;

	if(index_flag){
		initVIDEOwithIndex(original, ref_num_of_frames, ref_num_of_segments, ref_frame_rate,
					ref_start_frames, ref_end_frames, ref_bag_of_words,
					ref_confidences, ref_words, ref_signatures,
					ref_numberof_keyindex, ref_key_index, ref_numberof_allindex, ref_all_index, ref_key_lookup_table);

		initVIDEOwithIndex(query, query_num_of_frames, query_num_of_segments, query_frame_rate,
					query_start_frames, query_end_frames, query_bag_of_words,
					query_confidences, query_words, query_signatures,
					query_numberof_keyindex, query_key_index, query_numberof_allindex, query_all_index, query_key_lookup_table);
	}
	else{
		initVIDEO(original, ref_num_of_frames, ref_num_of_segments, ref_frame_rate,
					ref_start_frames, ref_end_frames, ref_bag_of_words,
					ref_confidences, ref_words, ref_signatures);

		initVIDEO(query, query_num_of_frames, query_num_of_segments, query_frame_rate,
					query_start_frames, query_end_frames, query_bag_of_words,
					query_confidences, query_words, query_signatures);
	}


	if( VideoMatching::duration < 3.0 )
	{
		if( VideoMatching::mode == PARTIAL )
		{
			SEGMENT_SIZE = 50;
			SEGMENT_OFFSET = 25;
		}
		else
		{
			SEGMENT_SIZE = 40;
			SEGMENT_OFFSET = 20;
		}
		if( VideoMatching::overwriteWords( original ) == -1 ) return -1;
		if( VideoMatching::overwriteWords( query ) == -1 ) return -1;
	}


	double distance = DBL_MAX;

	//VideoMatching::setLUT_L1_DIST();
	bool match=false;

	if(index_flag){
		match = VideoMatching::matchVideoWithIndex( query, original, videoMatchInfo );
	}
	else{
		match = VideoMatching::matchVideo( query, original, videoMatchInfo );
	}

	if(match)
	{
		distance = 0.0;
	}
	//VideoMatching::freeLUT_L1_DIST();
	original.clearSignature();
	query.clearSignature();


	return distance;

}

int VideoMatching::overwriteWords( VIDEO &video )
{
	int w[25]={191,198,200,255,315,25,156,214,251,254,38,51,84,218,250,81,266,276,318,335,82,83,92,256,277};
		
	int d1, d2, d3, d4, d5;
	
	int n = 0;
	for( int w_idx=0; w_idx<25; w_idx+=5 ){
		d1=w[w_idx]; d2 = w[w_idx+1]; d3 = w[w_idx+2]; d4 = w[w_idx+3]; d5 = w[w_idx+4];

#ifndef _FOR_SUBMISSION
		printf( "overwriting word[%d] = {%d, %d, %d, %d, %d}\n", n, d1, d2, d3, d4, d5 );
#endif
		
		video.extractWords( n, d1, d2, d3, d4, d5 );
		n++;
		if( n >= NUM_WORDS ) break;
	}


	video.extractSegments( SEGMENT_SIZE, SEGMENT_OFFSET );

	return 0;
}

//----------------------------------------------------------------------------
VideoSignatureSearchInterfaceABC *VideoSignatureSearchTool::GetInterface(void)
{ return &m_Interface; }

