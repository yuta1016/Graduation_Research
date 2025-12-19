//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  motion_activity_extraction.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <momusys.h>
#include "motion_activity_extraction.h"

//sth delimiters are missing
static int		DivisionBlockSize[4] = {2*2, 4*4, 8*8, 16*16};		// N_D
static int		numOfQuantLevel = 8;								// L

static int		numOfMBs_onDBxFrames[MAX_DIVISION_BLOCKS];			// N_B_mn(3)
static float	sumOfMVs_onDBxFrames[MAX_DIVISION_BLOCKS];			// sum(r_fij)
static int		Mask_forDivBlock[MAX_MACRO_BLOCKS];
static int		mb_width_prev;
static int		mb_height_prev;


/* Static Variables for intensity, directional, spatial and temporal
	attributes
*/

/* Variables for shot level computation */
static float shot_f_var=0;
static float shot_avg_angle=0;
static int temporal_activity[5] = {0,0,0,0,0};
static int shot_Nsr=0,shot_Nmr=0,shot_Nlr=0;

static int angles_of_activity[8]={0,0,0,0,0,0,0,0};
static int direction;
static float direction_ratio;
static int direction_max_freq;/* This variable indicates the frequency of the most
						   common direction. So the most common direction is
						   deemed the dominant direction only if it accounts for 
						   more than 50% of the frames.
						*/
static int direction_ok=0; /* This flag indicates if the dominant direction is
						valid or not */
static int frame_count=0;

static	int	mb_width;
static	int	mb_height;

/*
int	decode_main(int argc, char *argv[])
{
	int result;

	return result;
}

int ShotDescriptor( MediaForMotionActivity *motionvectors,
				     MotionActivityDescriptorInfo *motionactivityInfo)
{
	int result;

	return result;
}

MediaForMotionActivity *SetupMotionVectorsForaShot(char *filename, int startframe, int endframe)
{
	MediaForMotionActivity *media=0;

	return media;
}
*/
void freeMotionActivityMedia( MediaForMotionActivity *media )
{
    if( media != NULL )
	{
		if( media->MVx != NULL ) free( media->MVx );
		if( media->MVx != NULL ) free( media->MVy );
		free( media );
	}
}

int Initialize_Descriptor(MotionActivityDescriptorInfo *pMAD,
						  int direction_flag,
						  int spatial_flag,
						  int spatial_localized_flag,
						  int temporal_flag)
{
	int result;
	int i;

	result = 0;

	pMAD->intensity	= 0;
	pMAD->direction_flag						= direction_flag;
	pMAD->spatial_distribution_flag				= spatial_flag;
	pMAD->spatial_localized_distribution_flag	= spatial_localized_flag;
	pMAD->temporal_distribution_flag			= temporal_flag;
	pMAD->dominant_direction = 0;
	for(i=0;i<NUM_Nsr_Nmr_Nlr;i++){
		pMAD->spatial_parameters[i]	= 0;
	}
	pMAD->spa_loc_number = 0;
	for(i=0;i<MAX_DIVISION_BLOCKS;i++){
		pMAD->spatial_localized_parameter[i] = 0;
	}
	for(i=0;i<NUM_N0_N1_N2_N3_N4;i++){
		pMAD->temporal_parameters[i] = 0;
	}
	
	return result;
}

int ShotDescriptorSLMA(MomVop *momvop, MotionActivityDescriptorInfo *info, int Bin_Of_Division) // for SLMA
{
	unsigned char	*pMvx;
	unsigned char	*pMvy;
	unsigned char	*pMbStat;
	int				mb_width;
	int				mb_height;
	char			Mvx;								// x_fij
	char			Mvy;								// y_fij
	char			MbStat;								// 
	float			MotionVector;						// r_fij
	int				numOfDivBlock;						// N_D
	float			SLMA[MAX_DIVISION_BLOCKS];			// a_mn
	float			NormalizedSLMA[MAX_DIVISION_BLOCKS];// a_mn(p)
	unsigned char	QuantizedSLMA[MAX_DIVISION_BLOCKS];	// a_mn(q)
	float			intensityOfActivity;				// S_A
	float			max_percentage_activity;			// (L-1)Q

	int				i;

	pMvx	= momvop->v_chan->data->u;
	pMvy	= momvop->y_chan->data->u;
	pMbStat	= momvop->u_chan->data->u;
	mb_width  = momvop->width;
	mb_height = momvop->height;
	numOfDivBlock = DivisionBlockSize[Bin_Of_Division];

	// 1. Division block arrangements
	if (mb_width!=mb_width_prev||mb_height!=mb_height_prev){
		Make_Mask_forDivBlock(mb_width, mb_height, Bin_Of_Division);
		mb_width_prev  = mb_width;
        mb_height_prev = mb_height;
	}

	// 2. Spatial localized motion activity calculations
	for(i=0;i<mb_width*mb_height;i++){
		Mvx=*pMvx++;
		Mvy=*pMvy++;
		MbStat=*pMbStat++;
		if(MbStat==1||MbStat==2){
			MotionVector=(float) sqrt((float)Mvx*Mvx + Mvy*Mvy);
			numOfMBs_onDBxFrames[Mask_forDivBlock[i]]++;
			sumOfMVs_onDBxFrames[Mask_forDivBlock[i]]+=MotionVector;
		}
	}

	//	step 1: activity calculation
	for(i=0;i<numOfDivBlock;i++){
		SLMA[i]=sumOfMVs_onDBxFrames[i]/(float)numOfMBs_onDBxFrames[i];
	}

	//	step 2: normalization procedure
	intensityOfActivity=0.0;
	for(i=0;i<numOfDivBlock;i++){
		intensityOfActivity+=SLMA[i];
	}

	for(i=0;i<numOfDivBlock;i++){
		if(intensityOfActivity!=0){
			NormalizedSLMA[i]=SLMA[i]/intensityOfActivity;
		}else{
	//	step 3: exception procedure
			NormalizedSLMA[i]=(1/(float)numOfDivBlock);
		}
	}

	// 3. Quantization
	max_percentage_activity
		=(2*(float)(numOfQuantLevel-1))/((float)(numOfDivBlock*numOfQuantLevel));
	for(i=0;i<numOfDivBlock;i++){
		if(NormalizedSLMA[i]>max_percentage_activity){
			QuantizedSLMA[i]=numOfQuantLevel-1;
		}else{
			QuantizedSLMA[i]=(unsigned char)(NormalizedSLMA[i]*(float)(numOfQuantLevel-1)/max_percentage_activity);
		}
	}

	// storing SLMA descriptor info
	info->spa_loc_number=Bin_Of_Division;
	for(i=0;i<numOfDivBlock;i++){
		info->spatial_localized_parameter[i]=QuantizedSLMA[i];
	}

	return 0;
}

int InitializeSLMAdata() // for SLMA
{
	int i;

	for(i=0;i<MAX_DIVISION_BLOCKS;i++){
		numOfMBs_onDBxFrames[i]=0;
		sumOfMVs_onDBxFrames[i]=0;
	}

	return 0;
}

void Make_Mask_forDivBlock(int mb_width, int mb_height, int Bin_Of_Division) // for SLMA
{
	int i_width, i_height, i_Nmbs;
	int i_mask, i_seq, i_bin, bin, bin_prev, i_bin_2;
    int div_width [4][16], div_height [4][16];
	int line_mask_width [22], line_mask_height [18];

	int i;
	int binOfDivBlock;
	binOfDivBlock=1;
	for(i=0;i<=Bin_Of_Division;i++) binOfDivBlock*=2;

    div_width [0][0] = mb_width / 2;
    div_width [0][1] = mb_width - div_width [0][0];
	bin_prev = 1;
    for (i_mask = 0; i_mask < Bin_Of_Division; i_mask++) {
		bin = bin_prev * 2;
		for (i_bin = 0; i_bin < bin_prev; i_bin++) {
			i_bin_2 = i_bin * 2;
            div_width [i_mask+1][bin-i_bin_2-1] = div_width [i_mask][bin_prev-i_bin-1] / 2;
            div_width [i_mask+1][bin-i_bin_2-2] = div_width [i_mask][bin_prev-i_bin-1] - div_width [i_mask+1][bin-i_bin_2-1];
            div_width [i_mask+1][bin+i_bin_2  ] = div_width [i_mask][bin_prev+i_bin  ] / 2;
			div_width [i_mask+1][bin+i_bin_2+1] = div_width [i_mask][bin_prev+i_bin  ] - div_width [i_mask+1][bin+i_bin_2  ];
		}
        bin_prev *= 2;
	}
    div_height [0][0] = mb_height / 2;
    div_height [0][1] = mb_height - div_height [0][0];
	bin_prev = 1;
    for (i_mask = 0; i_mask < Bin_Of_Division; i_mask++) {
		bin = bin_prev * 2;
		for (i_bin = 0; i_bin < bin_prev; i_bin++) {
			i_bin_2 = i_bin * 2;
            div_height [i_mask+1][bin-i_bin_2-1] = div_height [i_mask][bin_prev-i_bin-1] / 2;
            div_height [i_mask+1][bin-i_bin_2-2] = div_height [i_mask][bin_prev-i_bin-1] - div_height [i_mask+1][bin-i_bin_2-1];
            div_height [i_mask+1][bin+i_bin_2  ] = div_height [i_mask][bin_prev+i_bin  ] / 2;
			div_height [i_mask+1][bin+i_bin_2+1] = div_height [i_mask][bin_prev+i_bin  ] - div_height [i_mask+1][bin+i_bin_2  ];
		}
        bin_prev *= 2;
	}

	i_seq = 0;
	for (i_bin = 0; i_bin < binOfDivBlock; i_bin++)
		for (i_width = 0; i_width < div_width [Bin_Of_Division][i_bin]; i_width++)
			line_mask_width [i_seq++] = i_bin;
	i_seq = 0;
	for (i_bin = 0; i_bin < binOfDivBlock; i_bin++)
		for (i_height = 0; i_height < div_height [Bin_Of_Division][i_bin]; i_height++)
			line_mask_height [i_seq++] = i_bin;
	i_Nmbs = 0;
	for (i_height = 0; i_height < mb_height; i_height++) {
		for (i_width = 0; i_width < mb_width; i_width++)
			Mask_forDivBlock [i_Nmbs + i_width]
				  = line_mask_height [i_height] * binOfDivBlock + line_mask_width [i_width];
			i_Nmbs += mb_width;
	}
}



/* Extraction code for the rest of the Motion Activity Descriptor */
/* Ajay Divakaran, June 22, 2001 */
int activity_dominant_direction(int angles_of_activity[])
{
int i, direction,direction_max_freq;

direction=0;
direction_max_freq=angles_of_activity[0];
for (i=0;i<8;i++)
{
 if (angles_of_activity[i]>direction) 
 {
	direction=i;
	direction_max_freq=angles_of_activity[i];
 }
}
return direction;
}

int activity_quantize_angle(float f_angle)
{
	int direction;
		/* Quantize angle using uniform 3 bit quantization over
		0-360 degrees i.e. 0,45,90,135,180,225,270,315
	   
	 */
if((f_angle >=0.) && (f_angle <22.5)) direction=0;
	
	else if ((f_angle >=22.5) &&(f_angle < 67.5)) direction=1;
	else if ((f_angle >=67.5) &&(f_angle < 112.5)) direction=2;
	else if ((f_angle >=112.5) &&(f_angle < 157.5)) direction=3;
	else if ((f_angle >=157.5) &&(f_angle < 202.5)) direction=4;
	else if ((f_angle >=202.5) &&(f_angle < 247.5)) direction=5;
	else if ((f_angle >=247.5) &&(f_angle < 292.5)) direction=6;
	else if ((f_angle >=292.5) &&(f_angle < 337.5)) direction=7;
	else if ((f_angle >=337.5) &&(f_angle < 360.)) direction=0;
	return direction;
}

int activity_quantize(float f_var, int levels)
{
	int intensity;
		/* Quantize std deviation using thresholds optimized for
	   test set consisting of MPEG-1 footage.
	   The quantization is over 5 levels as follows:
	   1 - Very Low Activity
	   2 - Low Activity
	   3 - Medium Activity
	   4 - High Activity
	   5 - Very High activity
	   For other formats, the quantization thresholds will have to be 
	   changed. These quantization thresholds are also only illustrative.
	 */
if (f_var <3.9) intensity=1;
	else if (f_var < 10.7) intensity=2;
	else if (f_var < 17.1) intensity=3;
	else if (f_var < 32)   intensity=4;
	else if (f_var >=32)   intensity=5;

	return intensity;
}


int ShotDescriptor(MotionActivityDescriptorInfo *pMAD)

{
	int out_flag=0;
	int intensity;
	int levels=5;
	int i;

	/* Local copies of shot-wide computation to prevent loss of 
		cumulative data
	*/
	/* Variables for shot level computation */
	float local_shot_f_var=0;
	float local_shot_avg_angle=0;
	int local_temporal_activity[5] = {0,0,0,0,0};
	int local_shot_Nsr=0,local_shot_Nmr=0,local_shot_Nlr=0;

/* End Local variables */


	/* Calculate Intensity parameter */
	local_shot_f_var=(shot_f_var/frame_count);
if (out_flag==1) printf("Shot deviation at end is %f\n",local_shot_f_var);

	intensity=activity_quantize(local_shot_f_var,levels);
if (out_flag==1) 	printf("Overall Shot Intensity is %d\n",intensity);

	pMAD->intensity=intensity;


	
	/* Calculate Temporal Parameters */

	if ((int)pMAD->temporal_distribution_flag==1)
	{
	for (i=0;i<5;i++)local_temporal_activity[i]=(64.0*temporal_activity[i])/frame_count;
	
if (out_flag==1) 	printf("Temporal parms");
if (out_flag==1) for (i=0;i<5;i++) printf(" %d ",local_temporal_activity[i]);
if (out_flag==1) printf("\n");
	/* Fill up descriptor structure */
for (i=0;i<5;i++)pMAD->temporal_parameters[i]=local_temporal_activity[i];
	}

/* Calculate Spatial Parameters */

if ((int) pMAD->spatial_distribution_flag == 1)

{

local_shot_Nsr=shot_Nsr/(frame_count);
local_shot_Nmr=shot_Nmr/(frame_count);
local_shot_Nlr=shot_Nlr/(frame_count);

if (out_flag==1) printf("Shot Nsr %d Nmr %d Nlr %d\n",local_shot_Nsr,local_shot_Nmr,local_shot_Nlr);

/* fill up Descriptor Structure */

pMAD->spatial_parameters[0]=local_shot_Nsr;
pMAD->spatial_parameters[1]=local_shot_Nmr;
pMAD->spatial_parameters[2]=local_shot_Nlr;

}


/* Calculate Directional Parameters */

	
if (out_flag==1) 	printf("Angles of activity \n");
if (out_flag==1) for (i=0;i<8;i++) printf(" %d ",angles_of_activity[i]);

/* Find "dominant" direction by finding max of angles_of_activity */



if ((int) pMAD->direction_flag==1)
{
direction=0;
direction_max_freq=angles_of_activity[0];
for (i=0;i<8;i++)
{
 if (angles_of_activity[i]>direction) 
 {
	direction=i;
	direction_max_freq=angles_of_activity[i];
 }
}

direction_ratio=(float)direction_max_freq/(frame_count);
if ( ((float) direction/ (float)(frame_count)) > 0.5) direction_ok=1;

if (out_flag==1) printf("\n");
if (out_flag==1) printf("Direction ratio is %f\n",direction_ratio);
if (out_flag==1) printf("Direction_ok is %d\n",direction_ok);

if (direction_ratio>0.5)
{

	pMAD->dominant_direction=direction;
}
else
{
	pMAD->direction_flag=0;
}

}


/* Reset frame counter to 0 for next shot */
//frame_counter=0;
	return 0;

}




int FrameDescriptor(MomVop *momvop,MotionActivityDescriptorInfo *info) 

/*
	This routine computes frame-level features, and stores
	them in static variables, that are in turn fed to the 
	routine ShotDescriptor() to get the shot descriptor

*/

{
	unsigned char	*pMvx;
	unsigned char	*pMvy;
	unsigned char	*pMbStat;

	char			MbStat;								// 
	int				i;


	/* Ajay Variables */

	int frame_number;
	/* January 11, 2000 */
	int offset=0;

	
	
	int out_flag=0;
	
	

	int *mv_x,*mv_y;

	float *mv_mag;
	float f_avg;
	

	float f_var;
	int levels=5;

	int intensity=0;


	int *mv_mag_thresh;
	int run_length=0,run_flag=0;
	int Nsr=0,Nmr=0,Nlr=0;

	float *angle;
	float f_avg_angle;

	
	





	/* End of Ajay Variables */

	pMvx	= momvop->v_chan->data->u;
	pMvy	= momvop->y_chan->data->u;
	pMbStat	= momvop->u_chan->data->u;
	mb_width  = momvop->width;
	mb_height = momvop->height;

	/* Allocate space for frame-wide arrays */
mv_x=(int *) calloc(mb_width*mb_height,sizeof(int));
mv_y=(int *) calloc(mb_width*mb_height,sizeof(int));
mv_mag=(float *)calloc(mb_width*mb_height,sizeof(float));
mv_mag_thresh=(int *)calloc(mb_height*mb_width,sizeof(int));	
angle=(float *)calloc(mb_height*mb_width,sizeof(float));

/* Fill up motion vector magnitude array */
/* Compute average motion vector magnitude */
f_avg=0;
for(i=0;i<mb_width*mb_height;i++)
	{
			mv_x[i]=*pMvx++;
			mv_y[i]=*pMvy++;
			MbStat=*pMbStat++;
			if(MbStat==1||MbStat==2)
			{
			mv_mag[i]=(float)sqrt((float)(mv_x[i]*mv_x[i] + mv_y[i]*mv_y[i]));
			}
			else
			{
				mv_mag[i]=0;
			}
			f_avg+=mv_mag[i];
	}
f_avg=f_avg/(mb_width*mb_height);

/* Compute standard deviation of motion vector magnitude */
f_var=0;

for(i=0;i<mb_width*mb_height;i++)

	{
			f_var+=(f_avg-mv_mag[i])*(f_avg-mv_mag[i]);
	}
	f_var=f_var/(mb_width*mb_height);
	f_var=(float)sqrt(f_var);

		/* Quantize std deviation 
	 */

intensity=activity_quantize(f_var,levels);

/* Calculate shot-wide intensity and temporal parameters */
shot_f_var+=f_var;
if (out_flag==1) printf("Shot deviation so far is %f\n",shot_f_var);

/* This increments the activity histogram appropriately */

temporal_activity[intensity-1]++;

if (out_flag==1) printf("Temporal parms");
if (out_flag==1) for (i=0;i<5;i++) printf(" %d ",temporal_activity[i]);
if (out_flag==1) printf("\n");


/* Calculate Spatial attributes */
	Nsr=0;
	Nmr=0;
	Nlr=0;
	run_length=0;
	run_flag=0;
	/* Calculate Run-Length Features */
	for (i=0;i<mb_width*mb_height;i++)
			{

	// Debugging
	if (frame_number==627)
	{
		frame_number=627;
	}
/* Compute thresholded matrix using Average Motion Bits as a threshold */
			if ((float) mv_mag[i] >= f_avg)
				{
				mv_mag_thresh[i]=(int) (mv_mag[i]);
				}
			else
				{
				mv_mag_thresh[i]=0;
				}

			}
/*
printf("Thresholded Matrix \n");
*/
for (i=0;i<mb_width*mb_height;i++)
{
/*
	printf("%d \n",Ajay_thresh[Ajay_i]);
*/
if (mv_mag_thresh[i]==0)
		{
			if (run_flag==1)
			{
			run_length++;
			}
			else
			{
			run_flag=1;
			run_length=1;
			}
		}
else
		{
			if (run_flag==1)
			{
/*
			printf("Run-length is %d\n",Ajay_run_length);
*/
		/* Place in short/medium/long category */
			if (run_length <= (mb_width/3)) Nsr++;
			if ((run_length > (mb_width/3)) && (run_length <=(2*(mb_width/3))))
			{
				Nmr++;
			}
		
			
/*

			printf("Nsr upto here is %d\n",Ajay_Nsr);
*/
			
			 if (run_length >(2*(mb_width/3))) Nlr=Nlr+ (run_length/mb_width);

			run_flag=0;
			}
			else
			{
			}
		}
}
/* Check if the last element is zero, and if so print out run_length */
if (mv_mag_thresh[mb_width*mb_height-1]==0)
		{
/*
		printf("Run-length is %d\n",Ajay_run_length);
*/

		/* Place in short/medium/long category */
	if (run_length <= (mb_width/3)) Nsr++;
			if ((run_length > (mb_width/3)) && 
				(run_length <=(2*(mb_width/3))))
			{
				Nmr++;
			}
		
			
			
	if (run_length >(2*(mb_width/3))) Nlr=Nlr+ (run_length/mb_width);

			

		

		
		}
if (out_flag==1) printf ("Nsr %d Nmr %d Nlr %d\n",Nsr,Nmr,Nlr);

shot_Nsr+=Nsr;
shot_Nmr+=Nmr;
shot_Nlr+=Nlr;

/* End of Spatial Parameter computation */

/* Calculate Directional Attribute */

for (i=0;i<mb_width*mb_height;i++)
{
if ((mv_x[i] ==0))
	{
	angle[i]=90;
	if (mv_y[i]==0) angle[i]=0;

	}
else
	{

	angle[i]= (float) (atan2((double)mv_y[i],(double)mv_x[i])/3.141592*180);

	}
	


}


f_avg_angle=0.;
for (i=0;i<mb_width*mb_height;i++)
{
f_avg_angle+=angle[i];
}
f_avg_angle=f_avg_angle/(mb_width*mb_height);


/* Quantize Angle to 3 bit value */
if (f_avg_angle<0)f_avg_angle+=360.;
direction=activity_quantize_angle(f_avg_angle);

angles_of_activity[direction]++;

frame_count++;



/*End of Directional Attribute Computation */


	/* De-allocate space for frame-wide arrays */
free(mv_x);
free(mv_y);
free(mv_mag);
free(mv_mag_thresh);	
free(angle);
	
ShotDescriptor(info);
		
	return 0;
}


