//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  motion_activity_extraction.h
//

#ifndef _MOTION_ACTIVITY_EXTRACTION_
#define _MOTION_ACTIVITY_EXTRACTION_

#ifndef NUM_Nsr_Nmr_Nlr
#define	NUM_Nsr_Nmr_Nlr			3	// Number of Spatial Parameter Number
#define NUM_N0_N1_N2_N3_N4		5	// Number of Temporal Parameter Number
#endif

#ifndef MAX_DIVISION_BLOCKS // for SLMA
#define MAX_DIVISION_BLOCKS	(16*16)
#endif
#ifndef MAX_MACRO_BLOCKS // for SLMA
#define MAX_MACRO_BLOCKS	400
#endif

typedef struct MediaForMotinActivity {
	int		NumOfFrameUsed;
	int		NumOfMB;
	int		*MVx;
	int		*MVy;
} MediaForMotionActivity;

typedef struct MotionActivityDescriptorInfo{
	int		intensity;
/*
	bool	direction_flag;
	bool    spatial_distribution_flag;
	bool	temporal_distribution_flag; 
*/
	char	direction_flag;
	char    spatial_distribution_flag;
	char	spatial_localized_distribution_flag; // for SLMA
	char	temporal_distribution_flag; 
	int 	dominant_direction;
	int	spatial_parameters[NUM_Nsr_Nmr_Nlr];
	int	spa_loc_number; // for SLMA
	int	spatial_localized_parameter[MAX_DIVISION_BLOCKS]; // for SLMA
	int	temporal_parameters[NUM_N0_N1_N2_N3_N4];	
} MotionActivityDescriptorInfo;

/*
if first shot -> calculate motion vector for current sequece
int	decode_main(int argc, char *argv[]);
output moiton vector file name : "MV.mv" 
*/
#ifdef __cplusplus
	extern "C" int	decode_main(int argc, char *argv[]);
#else
	extern int	decode_main(int argc, char *argv[]);
#endif

/* 
Input  : Intermediate Calculations from Frame level Computations
Output : MotionActivityDescritorInfo
*/
#ifdef __cplusplus
	extern "C" int ShotDescriptor( MotionActivityDescriptorInfo *info);
#else
	int ShotDescriptor(MotionActivityDescriptorInfo *info);
#endif
/*
Input: the vop
Output: Frame level descriptor computations to be used to 
		compute shot descriptor
*/
#ifdef __cplusplus
	extern "C" int FrameDescriptor(MomVop *momvop, MotionActivityDescriptorInfo *info);
#else
	int FrameDescriptor(MomVop *momvop, MotionActivityDescriptorInfo *info);
#endif

/* 
Input  : motion-vector filename("MV.mv"), startframeindex, endframeindex
Output : MediaForMotionActivity
*/
#ifdef __cplusplus
	extern "C" MediaForMotionActivity *SetupMotionVectorsForaShot(char *filename, int startframe, int endframe);
#else
	MediaForMotionActivity *SetupMotionVectorsForaShot(char *filename, int startframe, int endframe);
#endif

/*
Initialize descriotor
*/
#ifdef __cplusplus
	extern "C" int Initialize_Descriptor(MotionActivityDescriptorInfo *pMAD,int direction_flag,int spatial_flag,int spatial_localized_flag,int temporal_flag);
#else
	int Initialize_Descriptor(MotionActivityDescriptorInfo *pMAD,int direction_flag,int spatial_flag,int spatial_localized_flag,int temporal_flag);
#endif

/*
free allocated memoty
*/
void freeMotionActivityMedia( MediaForMotionActivity *media );


#ifdef __cplusplus
	extern "C" int ShotDescriptorSLMA(MomVop *momvop, MotionActivityDescriptorInfo *info, int Bin_Of_Division);
#else
	int ShotDescriptorSLMA(MomVop *momvop, MotionActivityDescriptorInfo *info, int Bin_Of_Division);
#endif

#ifdef __cplusplus
	extern "C" int InitializeSLMAdata();
#else
	int InitializeSLMAdata();
#endif

#ifdef __cplusplus
	extern "C" void Make_Mask_forDivBlock(int mb_width, int mb_height, int Bin_Of_Division);
#else
	void Make_Mask_forDivBlock(int mb_width, int mb_height, int Bin_Of_Division);
#endif


#endif
