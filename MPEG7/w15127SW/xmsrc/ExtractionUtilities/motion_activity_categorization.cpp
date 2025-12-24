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
// Applicable File Name:  motion_activity_categorization.cpp
//


//sth ***** where does this belong to (normative?)*****
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "motion_activity_categorization.h"
//sth function and class delimiters are missing

static float Amask [NUMOFAMASK][16][16];

void SLMACategorization (MotionActivityDescriptorInformation *pInfo) {
	int		bin;
	int		i_mask;
	int		i_height, i_width;
	float	potential [NUMOFAMASK];
	float	maximum_value;
	int		maximum_point;
	float	minimum_value;
	float	next_value;
	int		next_point;
	float	minprmax_threshold = (float) ML_Threshold_MpM;
	float	cornerDM_threshold = (float) DM_Threshold_Crnr;


	//assert (pInfo->spatial_localized_distribution_flag);

	switch (pInfo->spa_loc_number) {
		case 0:	bin =  2; break;
		case 1:	bin =  4; break;
		case 2:	bin =  8; break;
		case 3:	bin = 16; break;
		default: printf ("ERROR: Irregular bin size\n"); exit(1);
	}

	//Make Weighted Masks
	Make_AMask (bin);

	//Calculate Potentials
	for (i_mask = 0; i_mask < NUMOFAMASK; i_mask++) {
		potential [i_mask] = 0;
		for (i_height = 0; i_height < bin; i_height++)
			for (i_width = 0; i_width < bin; i_width++)
				potential [i_mask] += Amask [i_mask][i_height][i_width] * pInfo-> spatial_localized_parameter [i_height * bin + i_width];
	}

	//Exception Process 1: Detect Non-Categorized Shots
	maximum_value = potential [0];
	maximum_point = 0;
	minimum_value = potential [0];
	for (i_mask = 1; i_mask < NUMOFAMASK; i_mask++) {
		if (potential [i_mask] > maximum_value) {
			maximum_value = potential [i_mask];
			maximum_point = i_mask;
		}
		if (potential [i_mask] < minimum_value)
			minimum_value = potential [i_mask];
	}
	minimum_value = (maximum_value > (float) 0.0) ? (minimum_value / maximum_value) : (float) 1.0;
	if (minimum_value > minprmax_threshold)
		maximum_point = NUMOFAMASK;

	//Exception Process 2: Detect Quad-Directional Tendency Shots
	next_point = NUMOFAMASK;
	if (maximum_point > 3 && maximum_point < 8) {
		next_value = potential [0];
		next_point = 0;	
		for (i_mask = 1; i_mask < NUMOFAMASK; i_mask++) {
			if ((i_mask != maximum_point) && (potential [i_mask] > next_value)) {
				next_value = potential [i_mask];
				next_point = i_mask;
			}
		}
		if (next_value / maximum_value < cornerDM_threshold)
			next_point = NUMOFAMASK;
		else {
			switch (maximum_point) {
				case TL:
					if (next_point == L || next_point == BL)   maximum_point = L;
					if (next_point == T || next_point == TR)   maximum_point = T;
					break;
				case BL:
					if (next_point == L || next_point == TL)   maximum_point = L;
					if (next_point == B || next_point == BR)   maximum_point = B;
					break;
				case TR:
					if (next_point == R || next_point == BR)   maximum_point = R;
					if (next_point == T || next_point == TL)   maximum_point = T;
					break;
				case BR:
					if (next_point == R || next_point == TR)   maximum_point = R;
					if (next_point == B || next_point == BL)   maximum_point = B;
					break;
			}
		}
	}

	//Output
	switch (maximum_point) {
		case  B:	printf("< B  >"); break;
		case  T:	printf("< T  >"); break;
		case  R:	printf("< R  >"); break;
		case  L:	printf("< L  >"); break;
		case TL:	printf("< TL >"); break;
		case BL:	printf("< BL >"); break;
		case TR:	printf("< TR >"); break;
		case BR:	printf("< BR >"); break;
		case  C:	printf("< C  >"); break;
		case  N:	printf("< N  >"); break;
	}
	printf ("\n");

	return;
}

void Make_AMask (int bin) {
	int		i_mask;
	int		i_width, i_height;

	float	center_coord_w[NUMOFAMASK], center_coord_h[NUMOFAMASK];
	float	length_height, length_width, length;
	float	mask_normalize_factor_for_1;
	float	mask_normalize_factor_for_0 = 1 / (float) (bin * bin);
	float	shift_m = (float) 0.5;
	float	shift_s = (float) 0.0;
	float	shift_l = (float) 1.0 - shift_s;

	for (i_mask = 0; i_mask < NUMOFAMASK; i_mask++) {
		for (i_height = 0; i_height < bin; i_height++)
			for (i_width = 0; i_width < bin; i_width++)
				Amask[i_mask][i_height][i_width] = 0;
	}
	center_coord_w [0] = shift_m * (float) bin - (float) 0.5;	center_coord_h [0] = shift_l * (float) bin - (float) 0.5;
	center_coord_w [1] = shift_m * (float) bin - (float) 0.5;	center_coord_h [1] = shift_s * (float) bin - (float) 0.5;  
	center_coord_w [2] = shift_l * (float) bin - (float) 0.5;	center_coord_h [2] = shift_m * (float) bin - (float) 0.5;
	center_coord_w [3] = shift_s * (float) bin - (float) 0.5;	center_coord_h [3] = shift_m * (float) bin - (float) 0.5;
	for (i_mask = 4; i_mask < 8; i_mask++) {
		center_coord_w [i_mask] = (float) ((i_mask / 2 - 2) * bin) * (shift_l - shift_s) + shift_s * (float) bin - (float) 0.5;
		center_coord_h [i_mask] = (float) ((i_mask % 2)     * bin) * (shift_l - shift_s) + shift_s * (float) bin - (float) 0.5;
	}
	center_coord_w [8] = shift_m * (float) bin - (float) 0.5;	center_coord_h [8] = shift_m * (float) bin - (float) 0.5;

	for (i_mask = 0; i_mask < NUMOFAMASK; i_mask++) {
		mask_normalize_factor_for_1 = 0;
		for (i_height = 0; i_height < bin; i_height++) {
			length_height = (float) i_height - center_coord_h [i_mask];
			length_height *= length_height;
			for (i_width = 0; i_width < bin; i_width++) {
				length_width = (float) i_width - center_coord_w [i_mask];
				length_width *= length_width;
				length = length_height + length_width;
				length *= mask_normalize_factor_for_0;		//i.e. length /= (float) (bin * bin);
				Amask [i_mask][i_height][i_width] = (float) exp ((-GAMMA) * length);
				mask_normalize_factor_for_1 += Amask [i_mask][i_height][i_width];
			}
		}
		for (i_height = 0; i_height < bin; i_height++)
			for (i_width = 0; i_width < bin; i_width++)
				Amask [i_mask][i_height][i_width] /= mask_normalize_factor_for_1;
	}

    return;
}
