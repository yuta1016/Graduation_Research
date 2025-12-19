//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Peter Kuhn, Teruhiko Suzuki (SONY Corporation Japan)
//
// and modified by
//
// Anthony Vetro (Mitsubishi)
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
// Applicable File Name:  TranscodingExtraction.cpp
//

#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif
#include "Utilities/readparam.h"
#include "momusys.h"
#include "intra.h"
#include "inter.h"
#include "coortrans.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"              /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

/* ----------------------------------- local data --------------------------*/ 

#include "../lib/kltlib/klt.h"  // PK mod

//#define FPTRACE2
#define DIAG
#define USEKLT
#define MBMAX 1584  // HACK
#define SEGMENTSMAX 100000
#define FRAMEMAX 5*SEGMENTSMAX // HACK
#define STRLEN 512
#define MAXVO 6

/* local definitions */
	
   struct {
	double x;
	double y;
	int val;
   } tch_fp_mv[MBMAX*4][2], tch_fp_mv_m[MBMAX*4][2]; // max of 4 feature points per MB

   struct {
	int sum; // new featurepoints per frame
	double mv_xmax_p;
	double mv_xmax_n;
	double mv_ymax_p;
	double mv_ymax_n;
	// int bits;
	int bits[MAXVO];
	float mv_var[MAXVO];
	float sh_dist[MAXVO];
	int qscale[MAXVO];
	int num_nontransp[MAXVO];
	int bits_in[MAXVO];
   } framedata[FRAMEMAX];

   struct {
 	int sxp;
	int sxn;
	int syp;
	int syn;
	double m_uncomp;
	// double difficulty;
	double difficulty[MAXVO];
	double shape[MAXVO];
	double m_intensity[MAXVO];
   } tc[FRAMEMAX];

   
   struct {
   	int startframe;
	int stopframe;
	// int sxp;
	// int sxn;
	// int syp;
	// int syn;
	// double m_uncomp;
	// double difficulty;	
	double difficulty[MAXVO];
	double shape[MAXVO];
	double m_intensity[MAXVO];
	double avg_qscale[MAXVO];
	double avg_nontransp[MAXVO];
	double bitrate[MAXVO];
   } tch_seg_data[SEGMENTSMAX];

   static int framewidth, frameheight;
   static float framerate;

   static int last_seg_start, last_seg_stop, new_seg_start, new_seg, new_seg_stop, sum;
   int ME_SEGMIN, FP_NEW_ME_SEG, ME_SEGMAX, ME_FIX, ME_MAX, ME_MIN, SEG_FRAMES;
   int SEG_NUM_MAX;
   unsigned long Frame_number_last;

   KLT_TrackingContext KLT_tc;
   KLT_FeatureList KLT_fl; 
   KLT_FeatureTable KLT_ft;
   int nFeatures, fp_first_frame;

   int FP_Flag;
   char FP_Filename[512];
   FILE *FP_FD;

   int TCH_ME_Flag;
   char TCH_ME_Filename[512];
   FILE *TCH_ME_FD;	  	
 
/* ------------------------ implementation ------------------------------- */
void klt_if(MomVop *m_Media, MomVop *m_Media_prev, int ncols, int nrows, int framenum){

#ifdef FPTRACE2 	
  char outname[512];
#endif
  
  int i, new_fp;
    
#ifdef FPTRACE2  

#else
  KLTSetVerbosity(0);
#endif
	                
  KLTSelectGoodFeatures(KLT_tc,(unsigned char *)m_Media_prev->y_chan->data->s, 
  	ncols, nrows, KLT_fl);

  KLTStoreFeatureList(KLT_fl, KLT_ft, 0); // new
  
  for (i = 0 ; i < KLT_fl->nFeatures ; i++)  {
  	tch_fp_mv[i][0].x = KLT_fl->feature[i]->x;
    	tch_fp_mv[i][0].y = KLT_fl->feature[i]->y;
    	tch_fp_mv[i][0].val = KLT_fl->feature[i]->val;	
  }
  
#ifdef FPTRACE2  
  fprintf(stderr,"KLT start \n");
  fprintf(stderr,"\nIn first image:\n");
  for (i = 0 ; i < KLT_fl->nFeatures ; i++)  {
    fprintf(stderr,"Feature #%d:  (%f,%f) with value of %d\n",
           i, KLT_fl->feature[i]->x, KLT_fl->feature[i]->y,
           KLT_fl->feature[i]->val);
  }
  sprintf(outname,"feat1_%04d.ppm",framenum);
  KLTWriteFeatureListToPPM(KLT_fl, (unsigned char *)m_Media_prev->y_chan->data->s, 
  		ncols, nrows, outname);
  
  sprintf(outname,"feat1_%04d.txt",framenum);
  KLTWriteFeatureList(KLT_fl, outname, "%3d");
#endif

  KLTTrackFeatures(KLT_tc, (unsigned char *)m_Media_prev->y_chan->data->s, 
  	(unsigned char *)m_Media->y_chan->data->s,ncols, nrows, KLT_fl);
  //KLTReplaceLostFeatures(KLT_tc, (unsigned char *)m_Media->y_chan->data->s, ncols, nrows, KLT_fl); // PK
  KLTStoreFeatureList(KLT_fl, KLT_ft, i); // new 
    
  for (i = 0 ; i < KLT_fl->nFeatures ; i++)  {
  	tch_fp_mv[i][1].x = KLT_fl->feature[i]->x;
    	tch_fp_mv[i][1].y = KLT_fl->feature[i]->y;
    	tch_fp_mv[i][1].val = KLT_fl->feature[i]->val;	
  }
  for (i = 0, new_fp=0 ; i < KLT_fl->nFeatures ; i++)  {
  	if(KLT_fl->feature[i]->val < 0) new_fp++; // count all new featurepoints
  }
  framedata[framenum].sum=new_fp;
  
  if (FP_Flag == 1){   
     FP_FD=fopen(FP_Filename,"a");
     fprintf(FP_FD,"%6d %4d\n",framenum, new_fp);
     //fprintf(FP_FD,"%6.2f %4d\n",TIME,new_fp);
     
     fclose(FP_FD);
  }
  
#ifdef FPTRACE2   
  printf("\nIn second image:\n");
  for (i = 0 ; i < KLT_fl->nFeatures ; i++)  {
    fprintf(stderr,"Feature #%d:  (%f,%f) with value of %d\n",
           i, KLT_fl->feature[i]->x, 
	   KLT_fl->feature[i]->y,
           KLT_fl->feature[i]->val);
  } 
  sprintf(outname,"feat2_%04d.ppm",framenum);  
  KLTWriteFeatureListToPPM(KLT_fl, (unsigned char *) m_Media->y_chan->data->s, 
  	ncols, nrows, outname); 
  
  sprintf(outname,"feat2_%04d.fl",framenum);
  KLTWriteFeatureList(KLT_fl, outname, NULL);      // binary file 
  
  sprintf(outname,"feat2_%04d.txt",framenum);
  KLTWriteFeatureList(KLT_fl, outname, "%5.1f");  // text file  
#endif    
}

//----------------------------------------------------------------------------
void fp_range(int framenum, int nfeatures)
{
  int i; 
  double mv_x, mv_y, mv_xmax_p, mv_xmax_n, mv_ymax_p, mv_ymax_n;
     
  mv_xmax_p=mv_xmax_n=mv_ymax_p=mv_ymax_n=0.0;
    
  for (i=0; i< nfeatures; i++){ 
      	
  if(tch_fp_mv[i][1].val==0){			  	
       mv_x = tch_fp_mv[i][1].x-tch_fp_mv[i][0].x;
       mv_y = tch_fp_mv[i][1].y-tch_fp_mv[i][0].y;
      
       if(mv_x > 0.0){ 	     
	     if(mv_x > mv_xmax_p) mv_xmax_p = mv_x; 
       } else {
     	     if(mv_x < mv_xmax_n) mv_xmax_n = mv_x;
       }
       if(mv_y > 0.0){ 	     
	     if(mv_y > mv_ymax_p) mv_ymax_p = mv_y; 
       } else {
     	     if(mv_y < mv_ymax_n) mv_ymax_n = mv_y;
       }    
      
   //    printf("%d from first %4.1f,%4.1f to %4.1f,%4.1f mv_x:%2.5f mv_y:%2.5f mv_xmax_p:%2.5f mv_xmax_n:%2.5f mv_ymax_p:%2.5f mv_ymax_n:%2.5f\n",
   //  		i+1, tch_fp_mv[i][0].x, tch_fp_mv[i][0].y, 
   // 		tch_fp_mv[i][1].x, tch_fp_mv[i][1].y, mv_x, mv_y,
   //		mv_xmax_p, mv_xmax_n, mv_ymax_p, mv_ymax_n);
  
    } else { // default
       mv_xmax_p = mv_xmax_n = mv_ymax_p = mv_ymax_n = 0.0;					     
    }										     			     	    	     
  }  
  framedata[framenum].mv_xmax_p = mv_xmax_p;
  framedata[framenum].mv_xmax_n = mv_xmax_n;
  framedata[framenum].mv_ymax_p = mv_ymax_p;
  framedata[framenum].mv_ymax_n = mv_ymax_n;
 

  if (TCH_ME_Flag == 1){   
     TCH_ME_FD=fopen(TCH_ME_Filename,"a");
     fprintf(TCH_ME_FD,"  %6d ",framenum);
     fprintf(TCH_ME_FD,"%4.1f %4.1f %4.1f %4.1f\n",
     	framedata[framenum].mv_xmax_p, 
	framedata[framenum].mv_xmax_n, 
	framedata[framenum].mv_ymax_p, 
	framedata[framenum].mv_ymax_n);
     fclose(TCH_ME_FD);
  }	      	              
}
  		           
//----------------------------------------------------------------------------
int fp_calc_me_segment(int frame, int is_last_frame, int compression_format){
 
 int i;
 int start_new_segment=0;
 double frames_in_segment;
 
    if (fp_first_frame==0){       
      sum = new_seg = 0;
      last_seg_start = frame; // 0; // avetro changed from frame to 0 
      last_seg_stop = frame;
      new_seg_start = frame; // 0; // avetro changed from frame to 0
      new_seg_stop = frame;
      SEG_FRAMES=0;
    } 
    sum += framedata[frame].sum;

#if 0 // avetro: change order of conditions 
    if (is_last_frame==1){
    	 new_seg = 1;	 
    } else if(fp_first_frame==0){                
	 new_seg=0;
    } else if( ((SEG_FRAMES>= ME_SEGMIN) & (framedata[frame].sum > FP_NEW_ME_SEG)) || (SEG_FRAMES >= ME_SEGMAX)){
	 new_seg = 1;
    } else if (SEG_FRAMES >= ME_SEGMAX){
    	 new_seg = 1;
    } else {
         new_seg = 0;
    }
#else
    if(fp_first_frame==0){                
	 new_seg=0;
    } else if( ((SEG_FRAMES>= ME_SEGMIN) & (framedata[frame].sum > FP_NEW_ME_SEG)) || (SEG_FRAMES >= ME_SEGMAX)){
	 new_seg = 1;
    } else if (SEG_FRAMES >= ME_SEGMAX){
    	 new_seg = 1;
    } else if (is_last_frame==1){
    	 new_seg = 1;	 
    } else {
         new_seg = 0;
    }  
#endif
               
    fprintf(stderr,"frame: %d framedata[frame].sum: %d\n",frame, framedata[frame].sum);  		
    					 	   
    if(new_seg==1){
         start_new_segment=1;   
    	 last_seg_start = new_seg_start;	 
	 if(fp_first_frame>1){
	    last_seg_stop=frame-1;
	 } else {
	    //last_seg_stop=0;
	    last_seg_stop=frame-1;;
	 }	 
#ifdef FPTRACE2	 
     	 printf("frame:%d last_seg_start:%d last_seg_stop:%d \n",frame, last_seg_start, last_seg_stop);
	 printf("SEG_FRAMES:%d \n", SEG_FRAMES);
#endif			     
	 if(compression_format == MPEG2) // calc search range only if MPEG2
	 calc_search_range(last_seg_start, last_seg_stop);
	 tch_seg_data[SEG_NUM_MAX].startframe = last_seg_start;
	 tch_seg_data[SEG_NUM_MAX].stopframe = last_seg_stop;
	 SEG_NUM_MAX++;
	 
	 if ((last_seg_stop-last_seg_start)>0){
		frames_in_segment = last_seg_stop-last_seg_start;
         } else {
	 	frames_in_segment = 1;
	 }
	 if(compression_format == MPEG2) // calc motion uncomp only if MPEG2
	 for(i=last_seg_start; i<= last_seg_stop; i++){
	 	tc[i].m_uncomp = (double) sum / (frames_in_segment*((double)nFeatures));
	 }
	 														         	 				   							     	           
         SEG_FRAMES = new_seg = sum = 0;
	 if(fp_first_frame>1){
	    new_seg_start=frame;
	 } else {
	    new_seg_start=1;
	 }
    } else {
     
    }
    SEG_FRAMES++; 
    fp_first_frame++;
    return start_new_segment;    	               
}
//----------------------------------------------------------------------------
void calc_search_range(int start_frame, int end_frame)
{
  int i, jmp, symmetric, round;
  int sx_max_p, sx_max_n, sy_max_p, sy_max_n;
  
  round = 1; //  round to next full pel search position
  symmetric = sx_max_p = sx_max_n = sy_max_p = sy_max_n = 0;
     
  if(ME_FIX==0){
          	    	    
      jmp=1; // there is no temporal relation ship between 
             // prev and current segment 
      
      for (i=start_frame+jmp; i<=end_frame; i++){ 	 
         if(framedata[i].mv_xmax_p > sx_max_p){
		sx_max_p = (int) (framedata[i].mv_xmax_p+round);
 	 }
         if(framedata[i].mv_ymax_p > sy_max_p){
		sy_max_p = (int) (framedata[i].mv_ymax_p+round);
	 }
	 if(framedata[i].mv_xmax_n < sx_max_n){
		sx_max_n = (int) (framedata[i].mv_xmax_n-round);
	 }
         if(framedata[i].mv_ymax_n < sy_max_n){
		sy_max_n = (int) (framedata[i].mv_ymax_n-round);
	 }	
      }  
      if (symmetric==1){  
         for(i=start_frame; i<=end_frame; i++){
             if(sx_max_p > abs(sx_max_n)){
	 	tc[i].sxp = sx_max_p;
	     } else {
	 	tc[i].sxp = abs(sx_max_n);
	     }
	
             if(sy_max_p > abs(sy_max_n)){
		tc[i].syp = sy_max_p;
	     } else {
		tc[i].syp = abs(sy_max_n);
	     }
	     tc[i].sxn=-tc[i].sxp;
	     tc[i].syn=-tc[i].syp;
         }
       } else {
         for(i=start_frame; i<=end_frame; i++){
   	     tc[i].sxn=sx_max_n;
	     tc[i].sxp=sx_max_p;
	     tc[i].syn=sy_max_n;
	     tc[i].syp=sy_max_p;
        }
      }
     
      // set boundaries   
      for(i=start_frame; i<=end_frame; i++){    
          if(tc[i].sxp > ME_MAX) 
             tc[i].sxp = ME_MAX;	     
	  if(tc[i].sxp < ME_MIN) 
             tc[i].sxp = ME_MIN;
	     
	  if(tc[i].syp > ME_MAX) 
       	     tc[i].syp = ME_MAX;	     
	  if(tc[i].syp < ME_MIN) 
       	     tc[i].syp = ME_MIN;
      	  
          if(tc[i].sxn < -ME_MAX) 
       	     tc[i].sxn = -ME_MAX;	     
	  if(tc[i].sxn > -ME_MIN) 
       	     tc[i].sxn = -ME_MIN;	  
	  
	  if(tc[i].syn < -ME_MAX) 
       	     tc[i].syn = -ME_MAX;	     
	  if(tc[i].syn > -ME_MIN) 
       	     tc[i].syn = -ME_MIN;	 	            
       }
   } else if(ME_FIX==1){ 
      for(i=start_frame; i<=end_frame; i++){
          tc[i].sxn = -ME_MAX;	
          tc[i].sxp = +ME_MAX;	  
          tc[i].syp = +ME_MAX; 
          tc[i].syn = -ME_MAX;
      }
   }       
   if (TCH_ME_Flag == 1){   
     TCH_ME_FD=fopen(TCH_ME_Filename,"a");   
     for(i=start_frame; i<=end_frame; i++){
         fprintf(TCH_ME_FD,"%6d ",i);
         fprintf(TCH_ME_FD,"%4d %4d %4d %4d\n",tc[i].sxp, tc[i].sxn, tc[i].syp, tc[i].syn);
     }  
     fclose(TCH_ME_FD); 
  }   
}
//----------------------------------------------------------------------------
unsigned long init_difficulty(int max){
    int i;
    
    unsigned long difficulty_bitsum=0;
    
    for(i=0; i<= max; i++)
	 difficulty_bitsum += (unsigned long) framedata[i].bits[0]; 
    
    return difficulty_bitsum;
}
//----------------------------------------------------------------------------
void calc_seg_difficulty(int seg, unsigned long difficulty_bitsum, int max){
    int i;
    double sum, difficulty;
      
    sum = 0.0;
    	   
    for(i=tch_seg_data[seg].startframe; i<= tch_seg_data[seg].stopframe; i++){
	 sum += (double) framedata[i].bits[0];
    }
            
    difficulty = sum / ((double) difficulty_bitsum); 
    
    tch_seg_data[seg].difficulty[0] = difficulty;	
    
    for(i=tch_seg_data[seg].startframe; i<= tch_seg_data[seg].stopframe; i++){
	 tc[i].difficulty[0] = difficulty;
    } 
    
}
//----------------------------------------------------------------------------
int read_bitratefile(char *file_name){

 FILE *fd; 
 char c;
 char string1[STRLEN], string[STRLEN];
 int frm, lastframe=0;
 
 if (!(fd=fopen(file_name,"r"))){
    printf("Couldn't read frame parameter file %s",file_name);
    exit(1);
 }

 for(;;){
    c = e_getstring(fd,string,STRLEN," \n\t"," \t"," "); 
    if(c==EOF) 
    	break; 
    if(!strncmp(string,"#",1)){
       c = e_getstring(fd,string,STRLEN,"\n"," \t"," ");
    }else{		  		 
       frm = (int) atoix(string);
       //printf("frame: %5d",frm);
    
       c = e_getstring(fd,string1,STRLEN,"\n\t"," \t"," ");
       framedata[frm].bits[0] = (int) atoix(string1);  
       //printf(" bits: %d\n", framedata[frm].bits[0]);              
    }
    if (frm>lastframe) 
    	lastframe=frm;
  } 
  fclose(fd);
  
  return lastframe;
}

//----------------------------------------------------------------------------
int read_mediacoding_mp4(int maxframe, int maxvo) 
{
  FILE *fp;
  int k, i;
 
  if (!(fp=fopen("coding_mp4.dat","r"))) {
    printf("Cannot open coding_mp4.dat for reading\n");
    return 0;
  }

  /* read common coding parameters */
  fscanf(fp,"%d %d %f\n",&framewidth, &frameheight, &framerate);

  /* read qscale, number of non-transparent blocks and bits for each frame */
  for(k=0; k<maxframe; k++) for(i=0; i<maxvo; i++)
    fscanf(fp,"%d %d %d\n",&framedata[k].qscale[i], 
	   &framedata[k].num_nontransp[i], &framedata[k].bits_in[i]);

  fclose(fp);

  return 1;
}

//----------------------------------------------------------------------------
void calc_seg_mediacoding(int seg, int numvos, float framerate)
{
  int k, i;
  int sum, cnt;
  double avg_qscale, avg_nontransp, bitrate;

  /* avg qscale */
  for(i=0; i<numvos; i++) {
    sum = 0; cnt=0;
    for(k=tch_seg_data[seg].startframe; k<=tch_seg_data[seg].stopframe; k++) {
      sum += framedata[k].qscale[i];
      cnt++;
    }
    avg_qscale = sum/(double)cnt;
    tch_seg_data[seg].avg_qscale[i] = avg_qscale;
  }

  /* avg non-transparent blocks */
  for(i=0; i<numvos; i++) {
    sum = 0; cnt=0;
    for(k=tch_seg_data[seg].startframe; k<=tch_seg_data[seg].stopframe; k++) {
      sum += framedata[k].num_nontransp[i];
      cnt++;
    }
    avg_nontransp = sum/(double)cnt;
    tch_seg_data[seg].avg_nontransp[i] = avg_nontransp;
  }

  /* bitrate */
  for(i=0; i<numvos; i++) {
    sum = 0; cnt=0;
    for(k=tch_seg_data[seg].startframe; k<=tch_seg_data[seg].stopframe; k++) {
      sum += framedata[k].bits_in[i];
      cnt++;
    }
    bitrate = sum*framerate/(double)cnt;
    tch_seg_data[seg].bitrate[i] = bitrate;
  }
}

//----------------------------------------------------------------------------
void read_objbits_file(char *file_name, int maxframe, int maxvo) 
{
  FILE *fp; 
  int k, i;
 
  if (!(fp=fopen(file_name,"r"))) {
    printf("Cannot open %s for reading",file_name);
    exit(1);
  }

  for(k=0; k<maxframe; k++) for(i=0; i<maxvo; i++)
    fscanf(fp,"%d ",&framedata[k].bits[i]);

  fclose(fp);
}

//----------------------------------------------------------------------------
void calc_seg_obj_difficulty(int seg, int numvos)
{
  int k, i;
  double sum[MAXVO], sum_total=0.0;
  double difficulty;
    	   
  for(i=0; i<numvos; i++) {
    sum[i] = 0.0;
    for(k=tch_seg_data[seg].startframe; k<=tch_seg_data[seg].stopframe; k++)
      sum[i] += (double) framedata[k].bits[i];
    sum_total += sum[i];
  }

  for(i=0; i<numvos; i++) {
    difficulty = sum[i] / ((double) sum_total);
    tch_seg_data[seg].difficulty[i] = difficulty;
    for(k=tch_seg_data[seg].startframe; k<= tch_seg_data[seg].stopframe; k++)
	 tc[k].difficulty[i] = difficulty;
  } 
}

//----------------------------------------------------------------------------
void read_intensity_file(char *file_name, int maxframe, int maxvo) 
{
  FILE *fp; 
  int k, i;
 
  if (!(fp=fopen(file_name,"r"))) {
    printf("Cannot open %s for reading",file_name);
    exit(1);
  }

  for(k=0; k<maxframe; k++) for(i=0; i<maxvo; i++)
    fscanf(fp,"%f ",&framedata[k].mv_var[i]);

  fclose(fp);
}

//----------------------------------------------------------------------------
void calc_seg_intensity(int seg, int numvos)
{
  int k, i;
  double sum[MAXVO], sum_total=0.0;
  double motion_intensity;
    	   
  for(i=0; i<numvos; i++) {
    sum[i] = 0.0;
    for(k=tch_seg_data[seg].startframe; k<=tch_seg_data[seg].stopframe; k++)
      sum[i] += (double) framedata[k].mv_var[i];
    sum_total += sum[i];
  }

  for(i=0; i<numvos; i++) {
    motion_intensity = sum[i] / ((double) sum_total);
    tch_seg_data[seg].m_intensity[i] = motion_intensity;
    for(k=tch_seg_data[seg].startframe; k<= tch_seg_data[seg].stopframe; k++)
	 tc[k].m_intensity[i] = motion_intensity;
  } 
}

//----------------------------------------------------------------------------
void read_shape_file(char *file_name, int maxframe, int maxvo) 
{
  FILE *fp; 
  int k, i;
 
  if (!(fp=fopen(file_name,"r"))) {
    printf("Cannot open %s for reading",file_name);
    exit(1);
  }

  for(k=0; k<maxframe; k++) for(i=0; i<maxvo; i++)
    fscanf(fp,"%f ",&framedata[k].sh_dist[i]);

  fclose(fp);
}

//----------------------------------------------------------------------------
void calc_seg_shape_dist(int seg, int numvos)
{
  int k, i;
  int frames_in_seg;
  double sum;
  double shape_dist;
    	   
  for(i=0; i<numvos; i++) {
    sum = 0.0;
    for(k=tch_seg_data[seg].startframe; k<=tch_seg_data[seg].stopframe; k++)
      sum += (double) framedata[k].sh_dist[i];

    frames_in_seg = tch_seg_data[seg].stopframe-tch_seg_data[seg].startframe+1;
    shape_dist = sum / ((double)frames_in_seg);
    tch_seg_data[seg].shape[i] = shape_dist;
    for(k=tch_seg_data[seg].startframe; k<= tch_seg_data[seg].stopframe; k++)
	 tc[k].shape[i] = shape_dist;
  } 
}

//----------------------------------------------------------------------------
long atoix(char *str)
{
  long a;
  a = strtol(str,(char **)NULL,0);
  return a;
}
//----------------------------------------------------------------------------
char e_getstring(FILE *fp, char *buf, int buflen, char *delim, 
		 char *ignlead, char *igntrail)
{
  char c;
  c = getstring(fp,buf,buflen,delim,ignlead,igntrail);
  return(c);
}
//----------------------------------------------------------------------------
char getstring(FILE *fp, char *buf, int buflen, char *delim, char *ignlead, 
char *igntrail)
{
  int i=0, j;
  int file_end, file_err =0, ignore, delimit;
  char c;
  long fpos;

  /* skip leading ignorechars */
  do{
    c = fgetc(fp);
    file_end = (c == EOF);
   /* -- file_err = (c == NULL); */
   
    j = 0;
    do  ignore = (c == ignlead[j]);
      while ((ignlead[j++] != '\0') && !ignore);
  }
  while (ignore && !file_end && !file_err);

  if (!file_end && !file_err){ 
    /* rewind file for one char */
    fpos = ftell(fp) - 1;
    fseek(fp,fpos,0);

    do{
      c = fgetc(fp);
      file_end = (c == EOF);
      /* --file_err = (c == NULL); */

      j = 0;
      do  ignore = (c == igntrail[j]);
        while ((igntrail[j++] != '\0') && !ignore);

      j = 0;
      do  delimit = (c == delim[j]);
        while ((delim[j++] != '\0') && !delimit);

      if (!file_end && !file_err && !ignore && !delimit) buf[i++] = c;
    }
    while (!file_end && !file_err && !delimit);
  }

  if (i > (buflen-1))  i = buflen - 1;
  buf[i] = '\0'; 	 /* append c-stringdelimiter */

#ifdef I_DIAG    
     	fprintf(log_fd,"buf = %s\n",buf);
#endif
  return(c);
}

// ##########################################################################

using namespace XM;

const UUID TranscodingExtractionInterface::myID = UUID("");
const char *TranscodingExtractionInterface::myName = "TranscodingExtractionInterface";

const UUID TranscodingExtractionTool::myID = UUID("");
const char *TranscodingExtractionTool::myName = "TranscodingExtraction";

const UUID TranscodingExtractionInterfaceABC::myID = UUID("");



//=============================================================================
TranscodingExtractionInterfaceABC::TranscodingExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
TranscodingExtractionInterface::TranscodingExtractionInterface(
                 TranscodingExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TranscodingExtractionInterface::~TranscodingExtractionInterface()
{
}

//----------------------------------------------------------------------------
void TranscodingExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::SetSourceMedia(MomVop *VOP, MomVop *VOPprev)
{
  return m_ExtractionTool->SetSourceMedia(VOP,VOPprev);
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  return m_ExtractionTool->SetSourceMediaFrameNumber(FrameNo);
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::SetSourceMediaFilename(char *aFilename)
{
  return m_ExtractionTool->SetSourceMediaFilename(aFilename);
  return 0;
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::SetDSInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  return m_ExtractionTool->SetDSInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}


//----------------------------------------------------------------------------
unsigned long TranscodingExtractionInterface::PostExtracting(int startframe, int k)

{
  return m_ExtractionTool->PostExtracting(startframe, k);
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionInterface::CloseSession(int lastframe)

{
  return m_ExtractionTool->CloseSession(lastframe);
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetUseDifficulty(void)
{
 return m_ExtractionTool->GetUseDifficulty();
}
//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetUseDDL(void)
{
 return m_ExtractionTool->GetUseDDL();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetStartFrame(void)
{
 return m_ExtractionTool->GetStartFrame();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetFrames(void)
{
 return m_ExtractionTool->GetFrames();
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionInterface::GetSimDir(void)
{
 return m_ExtractionTool->GetSimDir();
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionInterface::GetDifficultyParamfilename(void)
{
 return m_ExtractionTool->GetDifficultyParamfilename();
}

//----------------------------------------------------------------------------	
unsigned long TranscodingExtractionInterface::GetCalculateDifficulty(void)
{
 return m_ExtractionTool->GetCalculateDifficulty();
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionInterface::GetReencodingParamfilename(void)
{
 return m_ExtractionTool->GetReencodingParamfilename();
}

//----------------------------------------------------------------------------	
unsigned long TranscodingExtractionInterface::GetCalculateObjectDifficulty(void)
{
 return m_ExtractionTool->GetCalculateObjectDifficulty();
}

//----------------------------------------------------------------------------	
unsigned long TranscodingExtractionInterface::GetCalculateMotionIntensity(void)
{
 return m_ExtractionTool->GetCalculateMotionIntensity();
}

//----------------------------------------------------------------------------	
unsigned long TranscodingExtractionInterface::GetCalculateShape(void)
{
 return m_ExtractionTool->GetCalculateShape();
}

//----------------------------------------------------------------------------	
unsigned long TranscodingExtractionInterface::GetMediaCodingParams(void)
{
 return m_ExtractionTool->GetMediaCodingParams();
}


//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetUseMESearchRange(void)
{
 return m_ExtractionTool->GetUseMESearchRange();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetUseMotionUncompensability(void)
{
 return m_ExtractionTool->GetUseMotionUncompensability();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetUseMotionIntensity(void)
{
 return m_ExtractionTool->GetUseMotionIntensity();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetUseShape(void)
{
 return m_ExtractionTool->GetUseShape();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetBitRate(void)
{
 return m_ExtractionTool->GetBitRate();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetCompressionFormat(void)
{
 return m_ExtractionTool->GetCompressionFormat();
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionInterface::GetDifficultyCtlfilename(void)
{
 return m_ExtractionTool->GetDifficultyCtlfilename();
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionInterface::GetMotionIntensityfilename(void)
{
 return m_ExtractionTool->GetMotionIntensityfilename();
}
//----------------------------------------------------------------------------	
char *TranscodingExtractionInterface::GetShapeDistfilename(void)
{
 return m_ExtractionTool->GetShapeDistfilename();
}

//----------------------------------------------------------------------------
int TranscodingExtractionInterface::GetNumVO(void)
{
 return m_ExtractionTool->GetNumVO();
}

//----------------------------------------------------------------------------
const UUID& TranscodingExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TranscodingExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TranscodingExtractionTool::TranscodingExtractionTool():
m_Interface(this),
m_seg_frames(0),
m_fp_new_me_seg(100), 	
m_segmin(15),
m_me_segmax(15),
m_me_fix(0),
m_me_max(31),
m_me_min(0),
m_motion_range_x_left(0),
m_motion_range_x_right(0),
m_motion_range_y_left(0),
m_motion_range_y_right(0),
m_motion_uncompensability(0),	
m_difficulty(0),
m_Sim_Dir_filename(0),
m_Encoding_par_filename(0),
m_Difficulty_par_filename(0),
m_Difficulty_bits_filename(0),
m_startframe(0),
m_frames(30),
m_use_me_search_range(1),
m_use_motion_uncompensability(1),
m_use_difficulty(1),
m_use_ddl(1),	
m_DSInterface(0),

m_Mpeg7Interface(0),
m_ContentDescriptionInterface(0),
m_MultimediaContentInterface(0),
m_VideoInterface(0),
m_HeaderInterface(0),
m_MediaLocatorInterface(0),
m_MediaUriInterface(0),

m_TranscodingHintsInterface(0),
m_Media(0),
m_Media_prev(0),
m_MediaFilename(0),
m_FrameNumber(0),
m_nnfraction(30),
m_nn(1)
{	
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TranscodingExtractionTool::TranscodingExtractionTool(GenericDSInterface *theParent):
m_Interface(this),
m_seg_frames(0),
m_fp_new_me_seg(100), 	
m_segmin(15),
m_me_segmax(15),
m_me_fix(0),
m_me_max(31),
m_me_min(0),
m_motion_range_x_left(0),
m_motion_range_x_right(0),
m_motion_range_y_left(0),
m_motion_range_y_right(0),
m_motion_uncompensability(0),	
m_difficulty(0),
m_Sim_Dir_filename(0),
m_Encoding_par_filename(0),
m_Difficulty_par_filename(0),
m_Difficulty_bits_filename(0),
m_startframe(0),
m_frames(30),
m_use_me_search_range(1),
m_use_motion_uncompensability(1),
m_use_difficulty(1),
m_use_ddl(1),
m_DSInterface(0),

m_Mpeg7Interface(0),
m_ContentDescriptionInterface(0),
m_MultimediaContentInterface(0),
m_VideoInterface(0),
m_HeaderInterface(0),
m_MediaLocatorInterface(0),
m_MediaUriInterface(0),

m_TranscodingHintsInterface(0),
m_Media(0),
m_Media_prev(0),
m_MediaFilename(0),
m_FrameNumber(0),
m_nnfraction(30),
m_nn(1)
{
	
  SetDSInterface(theParent);
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TranscodingExtractionTool::~TranscodingExtractionTool()
{
  /* release descriptor*/
     
}

//----------------------------------------------------------------------------
bool TranscodingExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool TranscodingExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int TranscodingExtractionTool::SetSourceMedia(MomVop *media, MomVop *prevmedia)

{
  if (!media) return -1;
  m_Media=media;
  m_Media_prev=prevmedia; 
  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionInterface::NewSequence(void)

{
  return m_ExtractionTool->NewSequence();
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  m_FrameNumber=FrameNo;
  return 0;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::SetSourceMediaFilename(char *aFilename)
{
  m_MediaFilename=aFilename;
  return 0;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::SetDSInterface(GenericDSInterfaceABC *ParentDSInterface)
{
  if (!ParentDSInterface) return -1;

  m_DSInterface = ParentDSInterface;
  return 0;
}
	
//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::InitExtracting(void)
{
	int nFrames, i;
   
        // if (!m_Media) return (unsigned long)-1;
	
	if (!m_DSInterface){
	  printf("InitExtracting: !m_DSInterface fault\n");
	  return (unsigned long)-1;
	}
	  
	for(i=0; i<FRAMEMAX; i++){
		framedata[i].sum=framedata[i].bits[0]=0;
		framedata[i].mv_xmax_p=framedata[i].mv_xmax_n=0.0;
		framedata[i].mv_ymax_p=framedata[i].mv_ymax_n=0.0;		
   	}			
    				
	m_Encoding_par_filename = new char[STRLEN];
	m_Difficulty_par_filename = new char[STRLEN];
	m_Difficulty_bits_filename = new char[STRLEN];
	if(!m_Sim_Dir_filename) m_Sim_Dir_filename = new char[STRLEN];
	m_Difficulty_ctl_filename = new char[STRLEN];
	m_Difficulty_objbits_filename = new char[STRLEN];
	m_Motion_intensity_filename = new char[STRLEN];
	m_Shape_dist_filename = new char[STRLEN];
	
  	/* read parameters from parameter file*/
	strcpy(m_Encoding_par_filename, getpara("Encoding_par_filename","news1.par"));
	strcpy(m_Difficulty_par_filename, getpara("Difficulty_par_filename","news1_diff.par"));
	strcpy(m_Difficulty_bits_filename,getpara("Difficulty_bits_filename","stat.out.bits"));
	strcpy(m_Sim_Dir_filename,getpara("Sim_Dir_filename","sim1"));
	strcpy(m_Difficulty_ctl_filename, getpara("Difficulty_ctl_filename","encode_q1.ctl"));
	strcpy(m_Difficulty_objbits_filename,getpara("Difficulty_objbits_filename","bits.dat"));
	strcpy(m_Motion_intensity_filename,getpara("Motion_intensity_filename","intensity.dat"));
	strcpy(m_Shape_dist_filename,getpara("Shape_dist_filename","sh_dist.dat"));
	
	m_startframe=atoi(getpara("startframe","0"));
	m_frames=atoi(getpara("frames","30"));	
	m_seg_frames=atoi(getpara("seg_frames","0"));
	m_fp_new_me_seg=atoi(getpara("fp_new_me_seg","100")); 
	m_segmin=atoi(getpara("segmin","15"));
	m_me_segmax=atoi(getpara("me_segmax","15"));
	m_me_fix=atoi(getpara("me_fix","0"));
	m_me_max=atoi(getpara("me_max","31"));
	m_me_min=atoi(getpara("me_min","0"));	
	m_nnfraction=atoi(getpara("nnfraction","30"));
	m_nn=atoi(getpara("nn","1"));
	
	m_use_me_search_range=atoi(getpara("m_use_me_search_range","1")); 	
	m_use_motion_uncompensability=atoi(getpara("m_use_motion_uncompensability","1"));
	m_use_difficulty=atoi(getpara("m_use_difficulty","1"));
	
	m_compression_format=atoi(getpara("CompressionFormat","0"));
	m_numvos=atoi(getpara("numvos","1"));
	m_bitrate=atoi(getpara("bitrate","256000"));
	m_use_motion_intensity=atoi(getpara("m_use_motion_intensity","0"));
	m_use_shape=atoi(getpara("m_use_shape","0"));
	m_mediacoding = 0;

	
#ifdef DIAG	
	printf("m_seg_frames %d \n",m_seg_frames);
	printf("m_fp_new_me_seg %d \n",m_fp_new_me_seg);
	printf("m_segmin %d \n",m_segmin);
	printf("m_me_segmax %d \n",m_me_segmax);
	printf("m_me_fix %d \n",m_me_fix);
	printf("m_me_max %d \n",m_me_max);
	printf("m_me_min %d \n",m_me_min);
	
	printf("m_Sim_Dir_filename %s\n", m_Sim_Dir_filename);
        printf("m_Encoding_par_filename %s\n", m_Encoding_par_filename);
	printf("m_Difficulty_par_filename %s\n", m_Difficulty_par_filename);
	printf("m_Difficulty_bits_filename %s\n", m_Difficulty_bits_filename);
	printf("m_startframe %d\n", m_startframe);
	printf("m_frames %d\n", m_frames);
	printf("m_use_me_search_range %d\n", m_use_me_search_range); 	
	printf("m_use_motion_uncompensability %d \n",m_use_motion_uncompensability);
	printf("m_use_difficulty %d\n",m_use_difficulty);
	
	printf("nnfraction %d \n",m_nnfraction);
	printf("nn %d \n",m_nn);

	printf("m_Difficulty_ctl_filename %s\n", m_Difficulty_ctl_filename);
	printf("m_Difficulty_objbits_filename %s\n", m_Difficulty_objbits_filename);
	printf("m_Motion_intensity_filename %s\n", m_Motion_intensity_filename);
	printf("m_Shape_dist_filename %s\n", m_Shape_dist_filename);
	printf("m_compression_format %d\n",m_compression_format);
	printf("m_numvos %d\n",m_numvos);
	printf("m_bitrate %d\n",m_bitrate);
	printf("m_use_motion_intensity %d\n",m_use_motion_intensity);
	printf("m_use_shape %d\n",m_use_shape);

#endif //DIAG
        SEG_FRAMES = m_seg_frames;  
        ME_SEGMIN = m_segmin;
        FP_NEW_ME_SEG = m_fp_new_me_seg;
       	ME_SEGMAX = m_me_segmax;
  	ME_FIX = m_me_fix;
  	ME_MAX = m_me_max;
  	ME_MIN = m_me_min;
	SEG_NUM_MAX=0;		
         
  	nFrames = 1000; 
  	nFeatures = 200;
	fp_first_frame=0;
	// Frame_number_last=0;

	if(m_compression_format == MPEG2) {
	if (!m_Media) return (unsigned long)-1;

  	KLT_tc = KLTCreateTrackingContext();
  	// KLTPrintTrackingContext(tc);
  	KLT_fl = KLTCreateFeatureList(nFeatures);
  	KLT_ft = KLTCreateFeatureTable(nFrames, nFeatures);
  	KLT_tc->sequentialMode = TRUE;
	
	FP_Flag=1;
	sprintf(FP_Filename,"results.fp");

	TCH_ME_Flag=1;
	sprintf(TCH_ME_Filename,"results.me");
	}
	
        /* create basic structure of description*/	
	NewSequence();													 	   	
        return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::StartExtracting()

{
  int nfeatures, start_newseg=0;

  fprintf(stderr," FrameNo: %ld\n", m_FrameNumber);

  if(m_compression_format == MPEG2){ // klt only for MPEG2 sequence
  /* check for media*/
  if (!m_Media) return (unsigned long)-1;

  nfeatures=200;

  klt_if(m_Media, m_Media_prev, m_Media->width,m_Media->height,
	 m_FrameNumber);      
  fp_range(m_FrameNumber, nfeatures);  
  }

  start_newseg = fp_calc_me_segment(m_FrameNumber,0,m_compression_format); 

  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::NewSequence(void)
{
        
   	if (!m_DSInterface){
       		printf("!! NewSequence(): m_DSInterface fault !!\n");
      		return (unsigned long)-1;
   	}
		
	//m_Comment1=m_DSInterface->CreateChild("-- Sony MPEG-7 Transcoding Metadata --");
	//m_Comment1=m_DSInterface->CreateChild("-- Authors: Peter Kuhn, Teruhiko Suzuki --");
	     
        m_Mpeg7=m_DSInterface->CreateChild("Mpeg7");
        m_Mpeg7Interface=m_Mpeg7.GetInterface();	
	m_Mpeg7.SetAttribute("type","complete");
	m_Mpeg7.SetAttribute("xmlns","http://www.mpeg7.org/2001/MPEG-7_Schema");
	m_Mpeg7.SetAttribute("xml:lang","en");
	m_Mpeg7.SetAttribute("xmlns:xsi","http://www.w3.org/2000/10/XMLSchema-instance");
	m_Mpeg7.SetAttribute("xsi:schemaLocation","http://www.mpeg7.org/2001/MPEG-7_Schema D:/Mpeg7/schemaVer3/Mpeg7Ver3.xsd");
	
	m_ContentDescription=m_Mpeg7Interface->CreateChild("ContentDescription");
        m_ContentDescriptionInterface=m_ContentDescription.GetInterface();	
	m_ContentDescription.SetAttribute("xsi:type","ContentEntityType");
	
	m_MultimediaContent=m_ContentDescriptionInterface->CreateChild("MultimediaContent");
	m_MultimediaContentInterface=m_MultimediaContent.GetInterface();
	m_MultimediaContent.SetAttribute("xsi:type","VideoType");
	
	m_Video=m_MultimediaContentInterface->CreateChild("Video");
	m_VideoInterface=m_Video.GetInterface();
	
	m_Header=m_VideoInterface->CreateChild("Header");
	m_HeaderInterface=m_Header.GetInterface();
	m_Header.SetAttribute("timeBase","../../MediaLocator[1]");
	m_Header.SetAttribute("timeUnit","PT1N30F");
	m_Header.SetAttribute("xsi:type","MediaTimeHeaderType");
	
	if(m_compression_format!=MPEG4){ // no MediaLocator in Header for MPEG4
	m_MediaLocator=m_VideoInterface->CreateChild("MediaLocator");
	m_MediaLocatorInterface=m_MediaLocator.GetInterface();
	
	m_MediaUri=m_MediaLocatorInterface->CreateChild("MediaUri");
	m_MediaUriInterface=m_MediaUri.GetInterface();
	m_MediaUri.SetValue("http://www.mpeg.org/contents/news1.mpg");
        }
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::GetCalculateDifficulty(void)
{    

   int seg, k, maxframes;
   double sum_diff;
   unsigned long total_bitrate;
   
   maxframes= read_bitratefile(m_Difficulty_bits_filename);
   total_bitrate = init_difficulty(maxframes);
   sum_diff=0.0;
   
#ifdef DIAG   
   printf("### Difficulty ###\n");
   printf("maxframes %d total_bitrate:%ld\n", maxframes, total_bitrate);		
#endif				
																
   for(seg=0; seg<SEG_NUM_MAX; seg++){		
   	calc_seg_difficulty(seg, total_bitrate,maxframes);
	sum_diff +=  tch_seg_data[seg].difficulty[0];	    	     		     			     	     
#ifdef DIAG	     
	fprintf(stderr,"seg: %d start: %d stop: %d difficulty: %2.9f\n", 
	     	seg, tch_seg_data[seg].startframe, tch_seg_data[seg].stopframe, 
		tch_seg_data[seg].difficulty[0]);
	for(k=tch_seg_data[seg].startframe; k<= tch_seg_data[seg].stopframe; k++){
	   fprintf(stderr," %d sxp %d sxn %d syp %d syn %d m_uncomp %2.9f difficulty %2.9f \n",
		k,tc[k].sxp, tc[k].sxn,tc[k].syp,tc[k].syn, tc[k].m_uncomp,tc[k].difficulty[0]);
        } 		
#endif	     
   } 
   
#ifdef DIAG   
   printf("Sum of Difficulties: sum_diff: %f\n",sum_diff);
#endif   
 
return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::GetCalculateObjectDifficulty(void)
{    

  int seg, k;
   
  read_objbits_file(m_Difficulty_objbits_filename, m_frames, m_numvos);
  for(seg=0; seg<SEG_NUM_MAX; seg++) {
    calc_seg_obj_difficulty(seg, m_numvos);

#ifdef DIAG
    printf("seg: %d start: %d stop: %d\n", seg, tch_seg_data[seg].startframe, 
	    tch_seg_data[seg].stopframe);
    for(k=0; k<m_numvos; k++)
      printf("  difficulty[%d]=%5.3f\n",k,tch_seg_data[seg].difficulty[k]);
#endif
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::GetCalculateMotionIntensity(void)
{    

  int seg, k;
   
  read_intensity_file(m_Motion_intensity_filename, m_frames, m_numvos);
  for(seg=0; seg<SEG_NUM_MAX; seg++) {
    calc_seg_intensity(seg, m_numvos);

#ifdef DIAG
    printf("seg: %d start: %d stop: %d\n", seg, tch_seg_data[seg].startframe, 
	    tch_seg_data[seg].stopframe);
    for(k=0; k<m_numvos; k++)
      printf("- intensity[%d]=%5.3f\n",k,tch_seg_data[seg].m_intensity[k]);
#endif
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::GetCalculateShape(void)
{    

  int seg, k;
   
  read_shape_file(m_Shape_dist_filename, m_frames, m_numvos);
  for(seg=0; seg<SEG_NUM_MAX; seg++) {
    calc_seg_shape_dist(seg, m_numvos);

#ifdef DIAG
    printf("seg: %d start: %d stop: %d\n", seg, tch_seg_data[seg].startframe, 
	    tch_seg_data[seg].stopframe);
    for(k=0; k<m_numvos; k++)
      printf("- shape_dist[%d]=%5.3f\n",k,tch_seg_data[seg].shape[k]);
#endif
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::GetMediaCodingParams(void)
{    

  int seg, k;

  if(m_compression_format == MPEG4) {
    m_mediacoding = read_mediacoding_mp4(m_frames, m_numvos);
    if(m_mediacoding) {
	m_framerate = framerate;
    	m_framewidth = framewidth;
	m_frameheight = frameheight;
    }
  }

  if(m_mediacoding) {
    for(seg=0; seg<SEG_NUM_MAX; seg++) {
      calc_seg_mediacoding(seg, m_numvos, m_framerate);

#ifdef DIAG
      printf("seg: %d start: %d stop: %d\n", seg, tch_seg_data[seg].startframe, 
	    tch_seg_data[seg].stopframe);
      for(k=0; k<m_numvos; k++) {
        printf("  avg_qscale[%d]=%5.3f\n",k,tch_seg_data[seg].avg_qscale[k]);
        printf("  avg_nontransp[%d]=%5.3f\n",k,tch_seg_data[seg].avg_nontransp[k]);
        printf("  bitrate[%d]=%5.3f\n",k,tch_seg_data[seg].bitrate[k]);
      }
#endif
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::CloseSession(int lastframe)

{
  int start_newseg;
  printf("lastframe:%d \n",lastframe);
  start_newseg = fp_calc_me_segment(lastframe, 1, m_compression_format);
  return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingExtractionTool::PostExtracting(int startframe, int k)

{
  int seg, incframe=0;
  char instance_id[256], media_uri[256];
   
#ifdef USEXML

     if(m_compression_format==MPEG2 && m_numvos==1){ // use VideoSegment

     fprintf(stderr,"SEG_NUM_MAX: %d \n",SEG_NUM_MAX);
     for(seg=0; seg<SEG_NUM_MAX; seg++){
  
  	GenericDS 
		l_VideoSegmentDS=m_VideoInterface->CreateChild("VideoSegment");
	GenericDSInterfaceABC 
		*l_VideoSegmentDSInterface=l_VideoSegmentDS.GetInterface();
	
	GenericDS 
		l_MediaInformation=l_VideoSegmentDSInterface->CreateChild("MediaInformation");
	GenericDSInterfaceABC
		*l_MediaInformationInterface=l_MediaInformation.GetInterface();
	
	GenericDS 
		l_MediaProfile=l_MediaInformationInterface->CreateChild("MediaProfile");
	GenericDSInterfaceABC
		*l_MediaProfileInterface=l_MediaProfile.GetInterface();
	
	GenericDS 
		l_TranscodingHints=l_MediaProfileInterface->CreateChild("TranscodingHints");
	GenericDSInterfaceABC
		*l_TranscodingHintsInterface=l_TranscodingHints.GetInterface();

       
    	GenericDS 
		l_MediaTime=l_VideoSegmentDSInterface->CreateChild("MediaTime");
    	GenericDS 
		l_MediaRelIncrTimePoint=l_MediaTime.CreateChild("MediaRelIncrTimePoint");
	GenericDS 
		l_MediaIncrDuration=l_MediaTime.CreateChild("MediaIncrDuration");

    	incframe = tch_seg_data[seg].stopframe-tch_seg_data[seg].startframe+1;
	
	fprintf(stderr,"incframe:%d start:%d stop:%d \n", 
			incframe, tch_seg_data[seg].startframe, tch_seg_data[seg].stopframe);
	l_MediaRelIncrTimePoint.SetValue(tch_seg_data[seg].startframe);		
	l_MediaIncrDuration.SetValue(incframe);
	

		   			
	GenericDS 
		m_MotionHint=l_TranscodingHintsInterface->CreateChild("MotionHint");
			
	GenericDS 
		m_TranscodingMotionRange=m_MotionHint.CreateChild("MotionRange");
					
	if(m_use_difficulty==1){ 							   				    
  		l_TranscodingHintsInterface->SetAttribute("difficulty", 
				tc[(int)tch_seg_data[seg].startframe].difficulty[0]);
        }					  

	GenericDSInterfaceABC 
		*m_TranscodingRangeInterface=m_TranscodingMotionRange.GetInterface();
			       																							
     	m_TranscodingRangeInterface->SetAttribute("xLeft", 
		- tc[(int)tch_seg_data[seg].startframe].sxn);					     					   	   					   
     	m_TranscodingRangeInterface->SetAttribute("xRight",
		tc[(int)tch_seg_data[seg].startframe].sxp);		      					   
     	m_TranscodingRangeInterface->SetAttribute("yDown", 
		- tc[(int)tch_seg_data[seg].startframe].syn);					  					   
     	m_TranscodingRangeInterface->SetAttribute("yUp",
		tc[(int)tch_seg_data[seg].startframe].syp); 
			
	if(m_use_motion_uncompensability==1){			
		m_MotionHint.SetAttribute("uncompensability",
			tc[(int)tch_seg_data[seg].startframe].m_uncomp);
	}
  																	
     } // for
   
     } else { // use MovingRegion for MPEG4 or numvo!=1

     //only instantiate 1 segment
     //fprintf(stderr,"SEG_NUM_MAX: %d \n",SEG_NUM_MAX);
     //for(seg=0; seg<SEG_NUM_MAX; seg++)
     {
  	GenericDS m_SpatioTemporalDecomposition=
		m_VideoInterface->CreateChild("SpatioTemporalDecomposition");
	GenericDSInterfaceABC *m_SpatioTemporalDecompositionInterface=
		m_SpatioTemporalDecomposition.GetInterface();

  	GenericDS m_MovingRegion=
		m_SpatioTemporalDecompositionInterface->CreateChild("MovingRegion");
	GenericDSInterfaceABC *m_MovingRegionInterface=
		m_MovingRegion.GetInterface();
	
	GenericDS m_MediaInformation=
		m_MovingRegionInterface->CreateChild("MediaInformation");
	GenericDSInterfaceABC *m_MediaInformationInterface=
		m_MediaInformation.GetInterface();
	
	GenericDS m_MediaProfile=
		m_MediaInformationInterface->CreateChild("MediaProfile");
	GenericDSInterfaceABC *m_MediaProfileInterface=
		m_MediaProfile.GetInterface();

	GenericDS m_MediaFormat=
		m_MediaProfileInterface->CreateChild("MediaFormat");
	GenericDSInterfaceABC *m_MediaFormatInterface=
		m_MediaFormat.GetInterface();
	GenericDS m_Content=m_MediaFormatInterface->CreateChild("Content");
	m_Content.SetValue("video");
	GenericDS m_BitRate=m_MediaFormatInterface->CreateChild("BitRate");
	m_BitRate.SetValue(m_bitrate);
	GenericDS m_VisualCoding=
		m_MediaFormatInterface->CreateChild("VisualCoding");
	GenericDSInterfaceABC *m_VisualCodingInterface=
		m_VisualCoding.GetInterface();
	GenericDS m_Format=m_VisualCodingInterface->CreateChild("Format");
	GenericDSInterfaceABC *m_FormatInterface=m_Format.GetInterface();
	m_FormatInterface->SetAttribute("href","urn:mpeg:MPEG7FileFormatCD:5");
	GenericDS m_Name=m_FormatInterface->CreateChild("Name");
	m_Name.SetValue("MPEG-4");
	GenericDS m_Frame=m_VisualCodingInterface->CreateChild("Frame");
	GenericDSInterfaceABC *m_FrameInterface=m_Frame.GetInterface();
	m_FrameInterface->SetAttribute("height",m_frameheight);
	m_FrameInterface->SetAttribute("width",m_framewidth);
	m_FrameInterface->SetAttribute("rate",m_framerate);
	
	GenericDS m_MediaTranscodingHints=
		m_MediaProfileInterface->CreateChild("MediaTranscodingHints");
	GenericDSInterfaceABC *m_MediaTranscodingHintsInterface=
		m_MediaTranscodingHints.GetInterface();

	if(m_use_motion_intensity==1){ 
 	    GenericDS m_MotionHint=
		m_MediaTranscodingHintsInterface->CreateChild("MotionHint");
	    GenericDSInterfaceABC *m_MotionHintInterface=
		m_MotionHint.GetInterface();
	    m_MotionHintInterface->SetAttribute("intensity", 
		tc[(int)tch_seg_data[seg].startframe].m_intensity[k]);
	}	      
				
	if(m_use_shape){ 
	    GenericDS m_Shape=
		m_MediaTranscodingHintsInterface->CreateChild("ShapeHint"); 
  	    m_Shape.SetAttribute("shapeChange", 
		tc[(int)tch_seg_data[seg].startframe].shape[k]);
  	    m_Shape.SetAttribute("numOfNonTranspBlocks", 
		tch_seg_data[seg].avg_nontransp[k]);
	}

	if(m_mediacoding) {
      	    GenericDS m_CodingHints=
		m_MediaTranscodingHintsInterface->CreateChild("CodingHints");
            m_CodingHints.SetAttribute("avgQuantScale",
		tch_seg_data[seg].avg_qscale[k]);
	}

	if(m_use_difficulty==1){ 
  	    m_MediaTranscodingHintsInterface->SetAttribute("difficulty", 
		tc[(int)tch_seg_data[seg].startframe].difficulty[k]);
        }					  

	GenericDS m_MediaInstance=
		m_MediaProfileInterface->CreateChild("MediaInstance");
	GenericDSInterfaceABC *m_MediaInstanceInterface=
		m_MediaInstance.GetInterface();

	GenericDS m_InstanceIdentifier=
		m_MediaInstanceInterface->CreateChild("InstanceIdentifier");
	sprintf(instance_id,"seg%d_obj%d",seg,k);
	m_InstanceIdentifier.SetValue(instance_id);

	GenericDS m_mediaLocator=m_MediaInstanceInterface->CreateChild("MediaLocator");
	GenericDSInterfaceABC *m_mediaLocatorInterface=m_mediaLocator.GetInterface();
	GenericDS m_mediaUri=m_mediaLocatorInterface->CreateChild("MediaUri");
	sprintf(media_uri,"http://www.merl.com/content/test_%d.bits",k);
	m_mediaUri.SetValue(media_uri);
	
	GenericDS m_SpatioTemporalLocator=
		m_MovingRegionInterface->CreateChild("SpatioTemporalLocator");
	GenericDSInterfaceABC *m_SpatioTemporalLocatorInterface=
		m_SpatioTemporalLocator.GetInterface();
	      
    	GenericDS m_MediaTime=
		m_SpatioTemporalLocatorInterface->CreateChild("MediaTime");
    	GenericDS m_MediaRelIncrTimePoint=
		m_MediaTime.CreateChild("MediaRelIncrTimePoint");
	GenericDS m_MediaIncrDuration=
		m_MediaTime.CreateChild("MediaIncrDuration");

    	incframe = tch_seg_data[seg].stopframe-tch_seg_data[seg].startframe+1;
	fprintf(stderr,"incframe:%d start:%d stop:%d \n", 
			incframe, tch_seg_data[seg].startframe, tch_seg_data[seg].stopframe);
	m_MediaRelIncrTimePoint.SetValue(tch_seg_data[seg].startframe);	
	m_MediaIncrDuration.SetValue(incframe);

     }
     }
  #endif //USEXML
 
   return 0;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetUseDifficulty(void)
{
 return m_use_difficulty;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetUseDDL(void)
{
 return m_use_ddl;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetStartFrame(void)
{
 m_startframe=atoi(getpara("startframe","0"));
 printf("m_startframe %d\n", m_startframe);
 return m_startframe;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetFrames(void)
{
 m_frames=atoi(getpara("frames","30"));
 printf("m_frames %d\n", m_frames);
 return m_frames;
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionTool::GetSimDir(void)
{
 if(!m_Sim_Dir_filename) m_Sim_Dir_filename = new char[STRLEN];
 strcpy(m_Sim_Dir_filename,getpara("Sim_Dir_filename","sim1"));
 printf("m_Sim_Dir_filename %s\n", m_Sim_Dir_filename);
 return m_Sim_Dir_filename;
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionTool::GetReencodingParamfilename(void)
{
 return m_Encoding_par_filename;
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionTool::GetDifficultyParamfilename(void)
{
 return m_Difficulty_par_filename;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetUseMESearchRange(void)
{
 return m_use_me_search_range;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetUseMotionUncompensability(void)
{
 return m_use_motion_uncompensability;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetUseMotionIntensity(void)
{
 return m_use_motion_intensity;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetUseShape(void)
{
 return m_use_shape;
}

//----------------------------------------------------------------------------
int TranscodingExtractionTool::GetBitRate(void)
{
 m_frames=atoi(getpara("bitrate","256000"));
 printf("m_bitrate %d\n", m_bitrate);
 return m_bitrate;
}

//----------------------------------------------------------------------------	
int TranscodingExtractionTool::GetCompressionFormat(void)
{
 m_compression_format=atoi(getpara("CompressionFormat","0"));
 printf("m_compression_format: %s\n", m_compression_format?"MPEG-4":"MPEG-2");
 return m_compression_format;
}

//----------------------------------------------------------------------------	
int TranscodingExtractionTool::GetNumVO(void)
{
 m_numvos=atoi(getpara("numvos","1"));
 printf("m_numvos: %d\n", m_numvos);
 return m_numvos;
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionTool::GetDifficultyCtlfilename(void)
{
 return m_Difficulty_ctl_filename;
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionTool::GetMotionIntensityfilename(void)
{
 if(!m_Motion_intensity_filename) m_Motion_intensity_filename = new char[STRLEN];
 strcpy(m_Motion_intensity_filename,getpara("Motion_intensity_filename","intensity.dat"));
 printf("m_Motion_intensity_filename %s\n", m_Motion_intensity_filename);
 return m_Motion_intensity_filename;
}

//----------------------------------------------------------------------------	
char *TranscodingExtractionTool::GetShapeDistfilename(void)
{
 if(!m_Shape_dist_filename) m_Shape_dist_filename = new char[STRLEN];
 strcpy(m_Shape_dist_filename,getpara("Shape_dist_filename","sh_dist.dat"));
 printf("m_Shape_dist_filename %s\n", m_Shape_dist_filename);
 return m_Shape_dist_filename;
}

//----------------------------------------------------------------------------
const UUID& TranscodingExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TranscodingExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TranscodingExtractionInterfaceABC 
   *TranscodingExtractionTool::GetInterface(void)
{ return &m_Interface; }

#undef FPTRACE2 
#undef USEKLT
#undef MBMAX
#undef FRAMEMAX



