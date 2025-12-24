//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
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
// Applicable File Name:  motion_activity_categorization.h
//

#ifndef _MOTION_ACTIVITY_CATEGORIZATION_
#define _MOTION_ACTIVITY_CATEGORIZATION_

#ifndef NUM_Nsr_Nmr_Nlr
#define	NUM_Nsr_Nmr_Nlr			3
#define NUM_N0_N1_N2_N3_N4		5
#endif

#ifndef MAX_DIVISION_BLOCKS
#define MAX_DIVISION_BLOCKS	(16*16)
#endif
#ifndef MAX_MACRO_BLOCKS
#define MAX_MACRO_BLOCKS	400
#endif
#ifndef NUMOFAMASK
#define	NUMOFAMASK	9
#endif
#ifndef GAMMA
#define GAMMA		4.0
#endif
#ifndef ML_Threshold_MpM
#define ML_Threshold_MpM	0.85
#endif
#ifndef DM_Threshold_Crnr
#define DM_Threshold_Crnr	0.75
#endif

typedef struct MotionActivityDescriptorInformation{
	int		intensity;
	char	direction_flag;
	char    spatial_distribution_flag;
	char	spatial_localized_distribution_flag;
	char	temporal_distribution_flag; 
	int 	dominant_direction;
	int		spatial_parameters[NUM_Nsr_Nmr_Nlr];
	int		spa_loc_number;
	int		spatial_localized_parameter[MAX_DIVISION_BLOCKS];
	int		temporal_parameters[NUM_N0_N1_N2_N3_N4];	
} MotionActivityDescriptorInformation;

enum	CATEGORY	{ B, T, R, L, TL, BL, TR, BR, C, N };

#ifdef __cplusplus
	extern "C" void	SLMACategorization(MotionActivityDescriptorInformation *pInfo);
#else
	extern void	SLMACategorization(MotionActivityDescriptorInformation *pInfo);
#endif

#ifdef __cplusplus
	extern "C" void	Make_AMask(int bin);
#else
	extern void	Make_AMask(int bin);
#endif


#endif
