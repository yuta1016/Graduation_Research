/* mpeg2enc.c, main() and parameter file reading                            */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */
/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Peter Kuhn (PK) Sony Corp. Japan
//  
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  mpeg2enc.c



#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define GLOBAL /* used by global.h */
#include "config.h"
#include "global.h"

/* private prototypes */
static void init _ANSI_ARGS_((void));
static void readparmfile _ANSI_ARGS_((char *fname));
static void readquantmat _ANSI_ARGS_((void));


// ---------- PK start --------------
static void read_XML_file(char *fname);
static void readframeparmfile _ANSI_ARGS_((char *fname));
static void difficulty_to_bits _ANSI_ARGS_ (());
void apply_motion_uncomp _ANSI_ARGS_ (());
static void set_defaults _ANSI_ARGS_ (());
static void readframeparmfile _ANSI_ARGS_((char *fname));
int get_digit(char *b);
char getstring(FILE *fp, char *buf, int buflen, char *delim, char *ignlead, 
               char *igntrail);
void err_loc(FILE *fp);
void err_exit(FILE *fp, char *err_message);
void e_exit(char *err_message);
char e_getstring(FILE *fp, char *buf, int buflen, char *delim, 
		 char *ignlead, char *igntrail);		 
void f_getstring(FILE *fp, char *string, char *delim, char *ignlead, 
		 char *igntrail);		 
int  ctohex(char c);		 
long atoix(char *str);
double atoid(char *str);		 
void shiftl(char *b);		 
void cat_char(char *string, char c);
void init_stats();
// ----------- PK end ------------

int fixq; 

int main(argc,argv)
int argc;
char *argv[];
{
  if (argc<=2) //PK
  {
    printf("\n%s, %s\n",version,author);
    printf("Usage: mpeg2encode in.par out.m2v [TransX.ddl | frame.tc]\n");
    exit(0);
  }
  
  // PK defaults
  FIXGOP=1; // may be changed later by par file (when N==0) && (M==0)
  DIAG=TRUE;
  
  MUC_GOP=1; // use motion uncompensability 
  
  //DIAG=FALSE;
  XML_input=0;
  use_difficulty=0;
  FULLSEARCH=1;  // 0: symmetric spiral search (original), 
  		 // 1: assymetric spiral search using DDL

  /* read parameter file */
  readparmfile(argv[1]);
  
  /* read quantization matrices */
  readquantmat();

  /* open output file */
  if (!(outfile=fopen(argv[2],"wb")))
  {
    sprintf(errortext,"Couldn't create output file %s",argv[2]);
    error(errortext);
  }
  
  set_defaults(); // PK 
  if(argc==4) read_XML_file(argv[3]); // PK
  init_stats(); // PK
  
  init();
  putseq();

  fclose(outfile);
  fclose(statfile);
  
#ifdef SONYHEADER  
  fclose(pk_fd); // PK 
#endif
    
  return 0;
}

static void init()
{
  int i, size;
  static int block_count_tab[3] = {6,8,12};

  initbits();
  init_fdct();
  init_idct();

  /* round picture dimensions to nearest multiple of 16 or 32 */
  mb_width = (horizontal_size+15)/16;
  mb_height = prog_seq ? (vertical_size+15)/16 : 2*((vertical_size+31)/32);
  mb_height2 = fieldpic ? mb_height>>1 : mb_height; /* for field pictures */
  width = 16*mb_width;
  height = 16*mb_height;
   
  sony_header_read =0; // PK
    
  chrom_width = (chroma_format==CHROMA444) ? width : width>>1;
  chrom_height = (chroma_format!=CHROMA420) ? height : height>>1;

  height2 = fieldpic ? height>>1 : height;
  width2 = fieldpic ? width<<1 : width;
  chrom_width2 = fieldpic ? chrom_width<<1 : chrom_width;
  
  block_count = block_count_tab[chroma_format-1];

  /* clip table */
  if (!(clp = (unsigned char *)malloc(1024)))
    error("malloc failed\n");
  clp+= 384;
  for (i=-384; i<640; i++)
    clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  for (i=0; i<3; i++)
  {
    size = (i==0) ? width*height : chrom_width*chrom_height;

    if (!(newrefframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
    if (!(oldrefframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
    if (!(auxframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
    if (!(neworgframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
    if (!(oldorgframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
    if (!(auxorgframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
    if (!(predframe[i] = (unsigned char *)malloc(size)))
      error("malloc failed\n");
  }

  mbinfo = (struct mbinfo *)malloc(mb_width*mb_height2*sizeof(struct mbinfo));

  if (!mbinfo)
    error("malloc failed\n");

  blocks =
    (short (*)[64])malloc(mb_width*mb_height2*block_count*sizeof(short [64]));

  if (!blocks)
    error("malloc failed\n");

  /* open statistics output file */
  if (statname[0]=='-')
    statfile = stdout;
  else if (!(statfile = fopen(statname,"w")))
  {
    sprintf(errortext,"Couldn't create statistics output file %s",statname);
    error(errortext);
  }
}

void error(text)
char *text;
{
  fprintf(stderr,text);
  putc('\n',stderr);
  exit(1);
}

static void readparmfile(fname)
char *fname;
{
  int i;
  int h,m,s,f;
  FILE *fd;
  char line[256];
  static double ratetab[8]=
    {24000.0/1001.0,24.0,25.0,30000.0/1001.0,30.0,50.0,60000.0/1001.0,60.0};
  extern int r,Xi,Xb,Xp,d0i,d0p,d0b; /* rate control */
  extern double avg_act; /* rate control */

  if (!(fd = fopen(fname,"r")))
  {
    sprintf(errortext,"Couldn't open parameter file %s",fname);
    error(errortext);
  }

  fgets(id_string,254,fd);
  fgets(line,254,fd); sscanf(line,"%s",tplorg);
  fgets(line,254,fd); sscanf(line,"%s",tplref);
  fgets(line,254,fd); sscanf(line,"%s",iqname);
  fgets(line,254,fd); sscanf(line,"%s",niqname);
  fgets(line,254,fd); sscanf(line,"%s",statname);
  fgets(line,254,fd); sscanf(line,"%d",&inputtype);
  fgets(line,254,fd); sscanf(line,"%d",&nframes);
  fgets(line,254,fd); sscanf(line,"%d",&frame0);
  fgets(line,254,fd); sscanf(line,"%d:%d:%d:%d",&h,&m,&s,&f);
  fgets(line,254,fd); sscanf(line,"%d",&N);
  fgets(line,254,fd); sscanf(line,"%d",&M);
  fgets(line,254,fd); sscanf(line,"%d",&mpeg1);
  fgets(line,254,fd); sscanf(line,"%d",&fixq);  // SUZ //sth put to end!!
  fgets(line,254,fd); sscanf(line,"%d",&fieldpic);
  fgets(line,254,fd); sscanf(line,"%d",&horizontal_size);
  fgets(line,254,fd); sscanf(line,"%d",&vertical_size);
  fgets(line,254,fd); sscanf(line,"%d",&aspectratio);
  fgets(line,254,fd); sscanf(line,"%d",&frame_rate_code);
  fgets(line,254,fd); sscanf(line,"%lf",&bit_rate);
  fgets(line,254,fd); sscanf(line,"%d",&vbv_buffer_size);   
  fgets(line,254,fd); sscanf(line,"%d",&low_delay);     
  fgets(line,254,fd); sscanf(line,"%d",&constrparms);
  fgets(line,254,fd); sscanf(line,"%d",&profile);
  fgets(line,254,fd); sscanf(line,"%d",&level);
  fgets(line,254,fd); sscanf(line,"%d",&prog_seq);
  fgets(line,254,fd); sscanf(line,"%d",&chroma_format);
  fgets(line,254,fd); sscanf(line,"%d",&video_format);
  fgets(line,254,fd); sscanf(line,"%d",&color_primaries);
  fgets(line,254,fd); sscanf(line,"%d",&transfer_characteristics);
  fgets(line,254,fd); sscanf(line,"%d",&matrix_coefficients);
  fgets(line,254,fd); sscanf(line,"%d",&display_horizontal_size);
  fgets(line,254,fd); sscanf(line,"%d",&display_vertical_size);
  fgets(line,254,fd); sscanf(line,"%d",&dc_prec);
  fgets(line,254,fd); sscanf(line,"%d",&topfirst);
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    frame_pred_dct_tab,frame_pred_dct_tab+1,frame_pred_dct_tab+2);
  
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    conceal_tab,conceal_tab+1,conceal_tab+2);
  
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    qscale_tab,qscale_tab+1,qscale_tab+2);

  fgets(line,254,fd); sscanf(line,"%d %d %d",
    intravlc_tab,intravlc_tab+1,intravlc_tab+2);
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    altscan_tab,altscan_tab+1,altscan_tab+2);
  fgets(line,254,fd); sscanf(line,"%d",&repeatfirst);
  fgets(line,254,fd); sscanf(line,"%d",&prog_frame);
/* intra slice interval refresh period */  
  fgets(line,254,fd); sscanf(line,"%d",&P);
  fgets(line,254,fd); sscanf(line,"%d",&r);
  fgets(line,254,fd); sscanf(line,"%lf",&avg_act);
  fgets(line,254,fd); sscanf(line,"%d",&Xi);
  fgets(line,254,fd); sscanf(line,"%d",&Xp);
  fgets(line,254,fd); sscanf(line,"%d",&Xb);
  fgets(line,254,fd); sscanf(line,"%d",&d0i);
  fgets(line,254,fd); sscanf(line,"%d",&d0p);
  fgets(line,254,fd); sscanf(line,"%d",&d0b);
  
  // PK
  if (N==0){
  	FIXGOP=0;
        fprintf(stderr,"using variable GOP size based on DDL\n");
  } else {
    	FIXGOP=1;
  	if (N<1)
    		error("N must be positive");
  	if (M<1)
    		error("M must be positive");
  	if (N%M != 0)
    		error("N must be an integer multiple of M");
  }
  
  motion_data = (struct motion_data *)malloc(M*sizeof(struct motion_data));
  if (!motion_data)
    error("malloc failed\n");

  memset(motion_data, 0, M*sizeof(struct motion_data)); // KY

  for (i=0; i<M; i++)
  {
    fgets(line,254,fd);
    sscanf(line,"%d %d %d %d",
      &motion_data[i].forw_hor_f_code, &motion_data[i].forw_vert_f_code,
      &motion_data[i].sxf, &motion_data[i].syf);

    if (i!=0)
    {
      fgets(line,254,fd);
      sscanf(line,"%d %d %d %d",
        &motion_data[i].back_hor_f_code, &motion_data[i].back_vert_f_code,
        &motion_data[i].sxb, &motion_data[i].syb);
    }
  }

  fclose(fd);

  /* make flags boolean (x!=0 -> x=1) */
  mpeg1 = !!mpeg1;
  fieldpic = !!fieldpic;
  low_delay = !!low_delay;
  constrparms = !!constrparms;
  prog_seq = !!prog_seq;
  topfirst = !!topfirst;

  for (i=0; i<3; i++)
  {
    frame_pred_dct_tab[i] = !!frame_pred_dct_tab[i];
    conceal_tab[i] = !!conceal_tab[i];
    qscale_tab[i] = !!qscale_tab[i];
    intravlc_tab[i] = !!intravlc_tab[i];
    altscan_tab[i] = !!altscan_tab[i];
  }
  repeatfirst = !!repeatfirst;
  prog_frame = !!prog_frame;

  /* make sure MPEG specific parameters are valid */
  range_checks();

  frame_rate = ratetab[frame_rate_code-1];

  /* timecode -> frame number */
  tc0 = h;
  tc0 = 60*tc0 + m;
  tc0 = 60*tc0 + s;
  tc0 = (int)(frame_rate+0.5)*tc0 + f;

  if (!mpeg1)
  {
    profile_and_level_checks();
  }
  else
  {
    /* MPEG-1 */
    if (constrparms)
    {
      if (horizontal_size>768
          || vertical_size>576
          || ((horizontal_size+15)/16)*((vertical_size+15)/16)>396
          || ((horizontal_size+15)/16)*((vertical_size+15)/16)*frame_rate>396*25.0
          || frame_rate>30.0)
      {
        if (!quiet)
          fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
        constrparms = 0;
      }
    }

    if (constrparms)
    {
      for (i=0; i<M; i++)
      {
        if (motion_data[i].forw_hor_f_code>4)
        {
          if (!quiet)
            fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
          constrparms = 0;
          break;
        }

        if (motion_data[i].forw_vert_f_code>4)
        {
          if (!quiet)
            fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
          constrparms = 0;
          break;
        }

        if (i!=0)
        {
          if (motion_data[i].back_hor_f_code>4)
          {
            if (!quiet)
              fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
            constrparms = 0;
            break;
          }

          if (motion_data[i].back_vert_f_code>4)
          {
            if (!quiet)
              fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
            constrparms = 0;
            break;
          }
        }
      }
    }
  }

  /* relational checks */

  if (mpeg1)
  {
    if (!prog_seq)
    {
      if (!quiet)
        fprintf(stderr,"Warning: setting progressive_sequence = 1\n");
      prog_seq = 1;
    }

    if (chroma_format!=CHROMA420)
    {
      if (!quiet)
        fprintf(stderr,"Warning: setting chroma_format = 1 (4:2:0)\n");
      chroma_format = CHROMA420;
    }

    if (dc_prec!=0)
    {
      if (!quiet)
        fprintf(stderr,"Warning: setting intra_dc_precision = 0\n");
      dc_prec = 0;
    }

    for (i=0; i<3; i++)
      if (qscale_tab[i])
      {
        if (!quiet)
          fprintf(stderr,"Warning: setting qscale_tab[%d] = 0\n",i);
        qscale_tab[i] = 0;
      }

    for (i=0; i<3; i++)
      if (intravlc_tab[i])
      {
        if (!quiet)
          fprintf(stderr,"Warning: setting intravlc_tab[%d] = 0\n",i);
        intravlc_tab[i] = 0;
      }

    for (i=0; i<3; i++)
      if (altscan_tab[i])
      {
        if (!quiet)
          fprintf(stderr,"Warning: setting altscan_tab[%d] = 0\n",i);
        altscan_tab[i] = 0;
      }
  }

  if (!mpeg1 && constrparms)
  {
    if (!quiet)
      fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
    constrparms = 0;
  }

  if (prog_seq && !prog_frame)
  {
    if (!quiet)
      fprintf(stderr,"Warning: setting progressive_frame = 1\n");
    prog_frame = 1;
  }

  if (prog_frame && fieldpic)
  {
    if (!quiet)
      fprintf(stderr,"Warning: setting field_pictures = 0\n");
    fieldpic = 0;
  }

  if (!prog_frame && repeatfirst)
  {
    if (!quiet)
      fprintf(stderr,"Warning: setting repeat_first_field = 0\n");
    repeatfirst = 0;
  }

  if (prog_frame)
  {
    for (i=0; i<3; i++)
      if (!frame_pred_dct_tab[i])
      {
        if (!quiet)
          fprintf(stderr,"Warning: setting frame_pred_frame_dct[%d] = 1\n",i);
        frame_pred_dct_tab[i] = 1;
      }
  }

  if (prog_seq && !repeatfirst && topfirst)
  {
    if (!quiet)
      fprintf(stderr,"Warning: setting top_field_first = 0\n");
    topfirst = 0;
  }

  /* search windows */
  for (i=0; i<M; i++)
  {
    if (motion_data[i].sxf > (4<<motion_data[i].forw_hor_f_code)-1)
    {
      if (!quiet)
        fprintf(stderr,
          "Warning: reducing forward horizontal search width to %d\n",
          (4<<motion_data[i].forw_hor_f_code)-1);
      motion_data[i].sxf = (4<<motion_data[i].forw_hor_f_code)-1;
    }

    if (motion_data[i].syf > (4<<motion_data[i].forw_vert_f_code)-1)
    {
      if (!quiet)
        fprintf(stderr,
          "Warning: reducing forward vertical search width to %d\n",
          (4<<motion_data[i].forw_vert_f_code)-1);
      motion_data[i].syf = (4<<motion_data[i].forw_vert_f_code)-1;
    }

    if (i!=0)
    {
      if (motion_data[i].sxb > (4<<motion_data[i].back_hor_f_code)-1)
      {
        if (!quiet)
          fprintf(stderr,
            "Warning: reducing backward horizontal search width to %d\n",
            (4<<motion_data[i].back_hor_f_code)-1);
        motion_data[i].sxb = (4<<motion_data[i].back_hor_f_code)-1;
      }

      if (motion_data[i].syb > (4<<motion_data[i].back_vert_f_code)-1)
      {
        if (!quiet)
          fprintf(stderr,
            "Warning: reducing backward vertical search width to %d\n",
            (4<<motion_data[i].back_vert_f_code)-1);
        motion_data[i].syb = (4<<motion_data[i].back_vert_f_code)-1;
      }
    }
  }

}

static void readquantmat()
{
  int i,v;
  FILE *fd;

  if (iqname[0]=='-')
  {
    /* use default intra matrix */
    load_iquant = 0;
    for (i=0; i<64; i++)
      intra_q[i] = default_intra_quantizer_matrix[i];
  }
  else
  {
    /* read customized intra matrix */
    load_iquant = 1;
    if (!(fd = fopen(iqname,"r")))
    {
      sprintf(errortext,"Couldn't open quant matrix file %s",iqname);
      error(errortext);
    }

    for (i=0; i<64; i++)
    {
      fscanf(fd,"%d",&v);
      if (v<1 || v>255)
        error("invalid value in quant matrix");
      intra_q[i] = v;
    }

    fclose(fd);
  }

  if (niqname[0]=='-')
  {
    /* use default non-intra matrix */
    load_niquant = 0;
    for (i=0; i<64; i++)
      inter_q[i] = 16;
  }
  else
  {
    /* read customized non-intra matrix */
    load_niquant = 1;
    if (!(fd = fopen(niqname,"r")))
    {
      sprintf(errortext,"Couldn't open quant matrix file %s",niqname);
      error(errortext);
    }

    for (i=0; i<64; i++)
    {
      fscanf(fd,"%d",&v);
      if (v<1 || v>255)
        error("invalid value in quant matrix");
      inter_q[i] = v;
    }

    fclose(fd);
  }
}


// ###################################################################
// 
//  subsequent code is added by SONY Corp.Japan, Peter Kuhn (PK), in 2000
//
// ########################################################################


void init_stats(){

  FILE *mystatfile;
  char c;
  char fname[STRLEN], string[STRLEN], string1[STRLEN], tmp_str[STRLEN];
  int frame, sum;
 
  sprintf(fname,statname);
  strcat(fname,".psnr");
  if (!(mystatfile=fopen(fname,"w")))
  {
    sprintf(errortext,"Couldn't open statistics file %s",fname);
    error(errortext);
  }
  fclose(mystatfile);
  
  sprintf(fname,statname);
  strcat(fname,".bits");
  if (!(mystatfile=fopen(fname,"w")))
  {
    sprintf(errortext,"Couldn't open statistics file %s",fname);
    error(errortext);
  }
  fclose(mystatfile); 
  
  sprintf(fname,statname);
  strcat(fname,".time");
  if (!(mystatfile=fopen(fname,"w")))
  {
    sprintf(errortext,"Couldn't open statistics file %s",fname);
    error(errortext);
  }
  fclose(mystatfile); 
}


void print_time_stats(int frame_num, double abstime){

  FILE *time_fd;
  char c;
  char fname[STRLEN];
  int frame, sum;
 
  sprintf(fname,statname);
  strcat(fname,".time");
  
  if (!(time_fd=fopen(fname,"a")))
  {
    sprintf(errortext,"Couldn't open statistics file %s",fname);
    error(errortext);
  }
  fprintf(time_fd,"%d %f\n", frame_num+frame0, abstime);
  fclose(time_fd);
}


void print_psnr_stats(int frame_num, double psnr_y, double psnr_u, double psnr_v){

  FILE *mystatfile;
  char c;
  char fname[STRLEN], string[STRLEN], string1[STRLEN], tmp_str[STRLEN];
  int frame, sum;
 
  sprintf(fname,statname);
  strcat(fname,".psnr");
  
  if (!(mystatfile=fopen(fname,"a")))
  {
    sprintf(errortext,"Couldn't open statistics file %s",fname);
    error(errortext);
  }
  fprintf(mystatfile,"%d %f %f %f \n", frame_num+frame0, psnr_y, psnr_u, psnr_v);
  fclose(mystatfile);
}

void print_rate_stats(int frame_num, int rate){

  FILE *mystatfile;
  char c;
  char fname[STRLEN], string[STRLEN], string1[STRLEN], tmp_str[STRLEN];
  int frame, sum;
 
  sprintf(fname,statname);
  strcat(fname,".bits");

  if (!(mystatfile=fopen(fname,"a")))
  {
    sprintf(errortext,"Couldn't open statistics file %s",fname);
    error(errortext);
  }
  fprintf(mystatfile,"%d %d \n", frame_num+frame0, rate);
  fclose(mystatfile);
}

/* Peter Kuhn 2000-06-27, modified 2001-06-19 */
/* This is a simple XML parser only for the transcoding hints ...*/
static void read_XML_file(char *fname){

 FILE *fd;
  char c;
  char string[STRLEN], string1[STRLEN], tmp_str[STRLEN];
  int frame, sum, xml, m_x_left, m_x_right, m_y_left, m_y_right, motion_hint,
      relinctime, relinctime_last, i, first, relinctime_new, seg;
  int use_difficulty, relinc_timepoint;
  int timedata, durationdata, no_motionhint;
  double difficulty, m_uncompensability;
  
  xml=motion_hint=relinctime=relinctime_last=relinctime_new=frame=0;
  relinctime=use_difficulty=relinc_timepoint=0;
  first=1;
  difficulty=m_uncompensability=0.0;
  m_x_left=m_y_left=m_x_right=m_y_right=seg=0;
  timedata = durationdata = 0;
  no_motionhint=1;

  if (!(fd=fopen(fname,"r")))
  {
    sprintf(errortext,"Couldn't read Metadata file %s",fname);
    error(errortext);
  }
  
  for(;;){
    c = e_getstring(fd,string,STRLEN," >\n\t"," <\t"," "); 
    if (DIAG) printf("string a: %s\n",string);
    if(c==EOF) 
    	break; 
	
    if(!strncmp(string,"?xml",4)){   // chicken out when not XML
       if (DIAG) printf("input file is XML \n");
       xml=1;  
       XML_input=1;
    } else if ((!strncmp(string,"?xml",4))&(xml=0)){
       if (DIAG) printf("input file is not a XML file, trying tc ..\n");
       fclose(fd);
       XML_input=0;
       if (DIAG) printf("*.tc format not supported \n");
       //exit(1);
       readframeparmfile(fname);
       return;
    }
     
    if(!strncmp(string,"#",1)){
       c = e_getstring(fd,string,STRLEN,"\n"," \t"," ");
       if (DIAG) printf("string b: %s\n",string);
       
    } else if((!strncmp(string,"MotionRange",11)) || (motion_hint==1)){
       if (DIAG) printf("MotionRange\n");
       
       no_motionhint=0;
       
       if(motion_hint==0){
       	 m_x_left=m_y_left=m_x_right=m_y_right=0;
	 c = e_getstring(fd,string,STRLEN," \n\t"," \t"," ");
	 motion_hint = 1;
       } 	    
       if (DIAG) printf("string: %s\n",string);

       if(!strncmp(string,"xLeft",5)){
          if (DIAG) printf(" m_xLeft\n");
	  	  
          c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
	  if (DIAG) printf(" string a: %s\n",string1); 
	  c = e_getstring(fd,string1,STRLEN,"\"\n\t","\"\t","\"");
	  if (DIAG) printf(" string b: %s\n",string1);   
	  
          m_x_left= - atoix(string1);
	  
       } else if(!strncmp(string,"yDown",5)){
          if (DIAG) printf(" m_yDown\n");
       
          c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
	  if (DIAG) printf(" string a: %s\n",string1); 
	  c = e_getstring(fd,string1,STRLEN,"\"\n\t","\"\t","\"");
	  if (DIAG) printf(" string b: %s\n",string1);   
         
          m_y_left= - atoix(string1);
	  
       } else if(!strncmp(string,"xRight",6)){
          if (DIAG) printf(" m_xRight\n");

          c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
	  if (DIAG) printf(" string a: %s\n",string1); 
	  c = e_getstring(fd,string1,STRLEN,"\"\n\t","\"\t","\"");
	  if (DIAG) printf(" string b: %s\n",string1);   
     
          m_x_right=atoix(string1);
	  
       } else if(!strncmp(string,"yUp",3)){
          if (DIAG) printf(" m_yUp\n");

          c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
	  if (DIAG) printf(" string a: %s\n",string1); 
	  c = e_getstring(fd,string1,STRLEN,"\"\n\t","\"\t","\"");
	  if (DIAG) printf(" string b: %s\n",string1);   

	  m_y_right=atoix(string1);

       }  else if(!strncmp(string,"/",1)){
      	  motion_hint=0;
	  	  
          if (DIAG) fprintf(stderr,"MotionRange: xLeft:%d yDown:%d "
			    "xRight:%d yUp:%d \n",
			    m_x_left, m_y_left, m_x_right, m_y_right);
       }  
             
   } else if(!strncmp(string,"uncompensability",16)){
          if (DIAG) fprintf(stderr," m_y_uncompensability\n");

          c = e_getstring(fd,string1,STRLEN,"\"\n\t"," \t"," ");
	  if (DIAG) fprintf(stderr," string a: %s\n",string1); 
	  
	  c = e_getstring(fd,string1,STRLEN,"\"\n\t","\"\t","\"");
	  if (DIAG) fprintf(stderr," string b: %s\n",string1);   

	  m_uncompensability=atoid(string1); 
	  if (DIAG) fprintf(stderr,"MotionHint uncompensability:%1.6f\n",
	  	m_uncompensability);
    
    } else if(!strncmp(string,"difficulty",10)){
       if (DIAG) fprintf(stderr,"difficulty\n");
          use_difficulty=1;
       
       	  c = e_getstring(fd,string1,STRLEN,"\"\n\t"," \t"," ");
	  if (DIAG) fprintf(stderr," string a: %s\n",string1); 

	  c = e_getstring(fd,string1,STRLEN,"\"\n\t","\"\t","\"");
	  if (DIAG) fprintf(stderr," string b: %s\n",string1);   
        	          
          difficulty=atoid(string1);
        
          //c = e_getstring(fd,string,STRLEN,"\n","","");
       if (DIAG) fprintf(stderr,"MediaTranscodingHints difficulty: %f\n",difficulty);  
    
    
     } else if(!strncmp(string,"MediaRelIncrTimePoint",21)){
       if (DIAG) fprintf(stderr,"MediaRelIncrTimePoint\n");	

       /* now get new media start time */
       c = e_getstring(fd,string,STRLEN,"<>\n\t"," \t"," ");
       if (DIAG) fprintf(stderr," string a: %s\n",string);
       
       relinc_timepoint=atoix(string);
       if (DIAG) fprintf(stderr,"MediaRelIncrTimePoint: %d\n",relinc_timepoint);
       timedata = 1;
      	 
       c = e_getstring(fd,string,STRLEN,"<>\n\t"," \t"," ");
       if (DIAG) fprintf(stderr," string b: %s\n",string);   
     
               
     } else if(!strncmp(string,"MediaIncrDuration",17)){
       if (DIAG) fprintf(stderr,"MediaIncrDuration\n");	
 																
       /* now get new media duration */																				 											  	 		  		 
       c = e_getstring(fd,string,STRLEN,"<>\n\t"," \t"," ");
       if (DIAG) fprintf(stderr," string a: %s\n",string);
       
       relinctime=atoix(string);
       if (DIAG) fprintf(stderr,"MediaIncrDuration: %d\n",relinctime);
       durationdata = 1;
	 
       c = e_getstring(fd,string,STRLEN,"<>\n\t"," \t"," ");
       if (DIAG) fprintf(stderr," string b: %s\n",string);   
           
     }   
       
     if ((timedata==1) && (durationdata==1)){  
       timedata = durationdata = 0;
          
       if (first==1){
       	  first = 0;
	  frame = 0; 
       }    
       if (DIAG) fprintf(stderr,"save parameters: frame:%d frame+relinctime:%d\n",
       	    frame, frame+relinctime); 
			 				 
       for(i=frame; i<frame+relinctime; i++){
             if (i>frame) framedata[i].display_new_seg = 0;
	     framedata[i].segn = relinctime;	
             framedata[i].sxp = m_x_right; 
             framedata[i].sxn = m_x_left;
             framedata[i].syp = m_y_right; 
             framedata[i].syn = m_y_left;
	     framedata[i].m_uncomp = m_uncompensability;
	     //framedata[i].difficulty = difficulty;
	     framedata[i].difficulty = difficulty/(double)relinctime;
       }
       framedata[frame].display_new_seg=1;
       frame += relinctime;        
     }      				    	       
  }
  SEGMAX=seg;	
  FMAX=frame; 

  framedata[FMAX].display_new_seg=1;
  for(i=FMAX; i<nframes; i++){
          if (i>frame) framedata[i].display_new_seg = 0;
	  framedata[i].segn = nframes-FMAX;	
          framedata[i].sxp = m_x_right; 
          framedata[i].sxn = m_x_left;
          framedata[i].syp = m_y_right; 
          framedata[i].syn = m_y_left;
	  framedata[i].difficulty = difficulty/((double)framedata[i].segn);
	  framedata[i].m_uncomp = m_uncompensability;     	  
  }
  if(no_motionhint==1){
    // set defaults
     for (i=0; i<nframes; i++){
	framedata[i].sxn = - motion_data[0].sxf;
	framedata[i].sxp = motion_data[0].sxf;
	framedata[i].syn = - motion_data[0].syf;
	framedata[i].syp = motion_data[0].syf;
     }  
  }
   
  for(i=0; i<nframes; i++){
         fprintf(stderr,"%4d segn:%3d sxp:%3d sxn:%3d syp:%3d syn:%3d "
		 "diff:%1.6f muc:%1.6f\n",
	  	i,
	   	framedata[i].segn,	
          	framedata[i].sxp, 
          	framedata[i].sxn,
          	framedata[i].syp, 
          	framedata[i].syn,
	  	framedata[i].difficulty,
	  	framedata[i].m_uncomp); 	
  }
  fclose(fd); 
}

static void set_defaults(){
    int i;
    for (i=0; i<nframes; i++){
	framedata[i].gop_bitrate=1.0;
	framedata[i].frame_bits=1.0; 
	framedata[i].M = M; 
	framedata[i].N = N;
	framedata[i].segn=0;
	framedata[i].gop_n=N;
	framedata[i].f0 = 0;
	framedata[i].new_gop=0;
	framedata[i].display_new_seg=0;
	framedata[i].sxn = - motion_data[0].sxf;
	framedata[i].sxp = motion_data[0].sxf;
	framedata[i].syn = - motion_data[0].syf;
	framedata[i].syp = motion_data[0].syf;
	framedata[i].X_I=0;
	framedata[i].X_P=0;
	framedata[i].X_B=0;
	framedata[i].bf_I=0.0;
	framedata[i].bf_P=0.0;	
	framedata[i].difficulty=1.0; 
	//framedata[i].m_uncomp=0.5;
    }
}

/* PK: "frame_bits" is definied here as a factor modulating 
       the original bitrate of the frame ! */
static void difficulty_to_bits(){

  int i;
  double sum, bit_rate_per_frame, difficulty_sum=0.0;  
  double mean_difficulty;
  
  bit_rate_per_frame = bit_rate/frame_rate;
  
  sum=0.0;	
  for (i=0; i<nframes; i++){
	     sum += framedata[i].difficulty;
  }
  if (sum==0) use_difficulty=0;
  
  if (use_difficulty==1){
  	if(DIAG) fprintf(stderr,"sum: %f \n", sum);
	
	mean_difficulty = sum/nframes;
 	for (i=0; i<nframes; i++){
  		framedata[i].frame_bits = framedata[i].difficulty/mean_difficulty; 
  	}
  } else {
    	for (i=0; i<nframes; i++){
  		framedata[i].frame_bits = 1.0; 
  	}
  }
}

void apply_motion_uncomp(){

  int f0, n, m, i, ii, k, nmax, gop_n, f_offset;
  int last_start, last_stop, p, gop, gopcount;
  double all_bits, bit_rate_per_frame, val, sum;
  
  double th1, th2, th3, th4;
  int n1, n2, n3, n4;
  int m1, m2, m3, m4;
  
  th1 = 0.10;  th2 = 0.25;  th3 = 0.35; th4 = 0;
  n1  = 36;    n2  = 18;     n3 = 9;    n4 = 6;
  m1  = M;     m2  = M;      m3 = M;     m4 = M;
   
  gopcount = ii = f_offset = 0;
  n = 12;	
		      									     												      														      	              
  for(i=0; i<nframes; i++){
        nmax = (i==0) ? framedata[i].segn : framedata[i].segn;	
  	if((i==0) ||(framedata[i].display_new_seg==1)){	
            fprintf(stderr,"m_un:%f ", framedata[i].m_uncomp); 
	    ii=0;
	    f_offset=i;	
	    if ((framedata[i].m_uncomp < th1) && (nmax>=n1)){ 		
	        n=n1;
		m=m1;
		fprintf(stderr," <th3: n:%d nmax:%d ",n, nmax);
	    } else if ((framedata[i].m_uncomp < th2) && (nmax>=n2)){ 		
	        n=n2;
		m=m2;
		fprintf(stderr," <th2: n:%d nmax:%d ",n, nmax);
	    } else if ((framedata[i].m_uncomp < th3) && (nmax>=n3)){
	        n=n3;
		m=m3;
		fprintf(stderr," <th1: n:%d nmax:%d ",n, nmax);
	    } else if ((framedata[i].m_uncomp >= th1) && (nmax>=n4)){
       		n=n4;
		m=m4;
		fprintf(stderr," >th1: n:%d nmax:%d ",n, nmax);
	    } else {
	        n=n4;
		m=m4;
	    } 		
	    fprintf(stderr," --> n:%d \n",n);
 	    gopcount = nmax / n;
        }
  	
	if(n%m!=0) error("apply_motion_uncomp(): N must be an integer multiple of M");
	
	if (gopcount > 0){
           f0 = (n*((ii+(m-1))/n) - (m-1));
	   gop_n = nmax - f0;
           if (f0<0) f0=0;	
	   if (ii >= ((gopcount*n)-(m-1))){ 
		framedata[i].gop_n=gop_n;
	   } else {
		framedata[i].gop_n=n;
           }    
	   
	} else if (gopcount==0){	         
	   framedata[i].gop_n=nmax;
	   f0 = 0;
	}
		
	framedata[i].f0=f0;
	framedata[i].N=n;
													      	         											
        if(ii==f0){
           	framedata[i].new_gop=1;
	} else {
           	framedata[i].new_gop=0;	
	}  
	
	fprintf(stderr,"i:%d ii:%d N:%d M:%d nmax:%d gopcount:%d gop_n:%d f_offset:%d f0:%d new_gop:%d\n",
		i, ii, framedata[i].N, m, nmax, gopcount, framedata[i].gop_n, f_offset, f0, framedata[i].new_gop);			
	ii++;																															  						
  }   
}

void determine_gop(){
  int f0,n,m,i, k;
  int last_start, last_stop, p, gop;
  double all_bits, bit_rate_per_frame, val, sum;
  all_bits=0;
   /* converts gop start from display order to coding order */
  bit_rate_per_frame = bit_rate/frame_rate;
 
  // f0: lowest frame number in current GOP
  if (FIXGOP==1){
   	for(i=0; i<nframes; i++){
        	f0 = N*((i+(M-1))/N) - (M-1);		    	     
        	if (f0<0) f0=0;
		framedata[i].f0=f0;

		if(i==f0){
           		framedata[i].new_gop=1;
		} else {
           		framedata[i].new_gop=0;	
		}
	}
  } else if (MUC_GOP==1){
        apply_motion_uncomp();
  } else {
  	for(i=0; i<nframes; i++){
  		m = M;
 		if(framedata[i+(m-1)].display_new_seg==1){
			n = framedata[i].N;	
		}
        	f0 = n*((i+(m-1))/n) - (m-1);
        	if (f0<0) f0=0;
		framedata[i].f0=f0;
		
		if(i==f0){
           		framedata[i].new_gop=1;
		} else {
           		framedata[i].new_gop=0;	
		}
  	}
  }
  
  if(use_difficulty==1){ 
  	difficulty_to_bits();
 
  	last_start =last_stop=0;
  	val=0.0;
  	gop=1;
	k=0;
  	for(i=0; i<nframes; i++){
	    if(framedata[i].new_gop==1){
		last_stop=i-1;
	        val=0.0;
		p=0;
		if (gop==0){
		  	val += framedata[i].frame_bits;
		        if (DIAG) fprintf(stderr,"i: %d k: %d last_start: %d "
					 "last_stop: %d bitfactor:%f\n",
		            i, k, last_start, last_stop, framedata[i].frame_bits); 
			p++;      
		} else {	
		      	val += framedata[i-(M-1)].frame_bits;
		        if (DIAG) fprintf(stderr,"i:%d k:%d p: %d last_start: "
					  "%d last_stop: %d bitfactor:%f\n",
		            i, k, p, last_start, last_stop, framedata[i].frame_bits); 
			p++;      
		}      
		for(k=last_start; k<=last_stop;k++){		
		    framedata[k].gop_bitrate=val/p;
		}
	        if (DIAG) fprintf(stderr,"last_start: %d gop_bits:%f\n",
		           last_start, framedata[last_start].gop_bitrate); 
		last_start=i;	
	   } 
  	}
  	val=0.0;
  	for (p=0, i=last_start; i<nframes; i++, p++){ // last GOP
	  val += framedata[i].frame_bits;
	  //if(DIAG) fprintf(stderr,"i: %d last_start: %d last_stop: %d bitfactor:%f val :%f\n",
	  //	             i, last_start, nframes, framedata[i].frame_bits, val); 
  	} 
  	for(i=last_start; i<nframes;i++){		
	    framedata[i].gop_bitrate=val/p;
  	}
  	if (DIAG) fprintf(stderr,"last_start: %d last_stop: %d gop_bits:%f\n",
		last_start, i, framedata[last_start+1].gop_bitrate);
 		
  	if(DIAG) fprintf(stderr,"nframes:%d \n", nframes);
  }
  for(i=0; i<nframes; i++){
          if(framedata[i].display_new_seg==1){
	  	if(DIAG) fprintf(stderr,"D ");
	  } else {
	  	if(DIAG) fprintf(stderr,"  ");	  
	  }
	  if(framedata[i].new_gop==1){
	  	if(DIAG) fprintf(stderr,"* ");
	  } else {
	  	if(DIAG) fprintf(stderr,"  ");	  
	  }
	  all_bits += framedata[i].frame_bits;	
	  if(DIAG) fprintf(stderr,"%d N:%2d sxn:%2d sxp:%2d syn:%2d syp:%2d "
			  "m_un:%1.3f diff:%1.8f diff/fr:%1.5f segn:%d",
	  	i, 
		framedata[i].N,
		framedata[i].sxn, 
		framedata[i].sxp, 
		framedata[i].syn, 
           	framedata[i].syp,  
	  	framedata[i].m_uncomp,
		framedata[i].difficulty * (double) framedata[i].segn,
		framedata[i].difficulty,
		framedata[i].segn
		);

          if(DIAG) fprintf(stderr," bits:%1.3f gop_bits:%f f0:%d gop_n:%d new_gop :%d \n", 	
          	framedata[i].frame_bits,
		framedata[i].gop_bitrate,
		framedata[i].f0,
		framedata[i].gop_n,
		framedata[i].new_gop);
   }
   if(DIAG) fprintf(stderr,"target bits:%f (= %f bits/s = %f bytes)\n", 
       		bit_rate_per_frame*all_bits, 
		(bit_rate_per_frame*frame_rate*all_bits)/nframes, 
     		bit_rate_per_frame*all_bits/8.0);
		
   if(use_difficulty==1){		
       sum = val = 0.0;	
       k = 0;
       fprintf(stderr,"GOP bitrate: \n");
       for(i=0; i<nframes; i++){
            framedata[i].frame_bits=1.0;
       	    if(framedata[i].new_gop==1){      
	    	val= ((double) framedata[i].gop_n * bit_rate * framedata[i].gop_bitrate / frame_rate) + 0.5;
		sum +=val;
		fprintf(stderr,"  i: %d  bitrate:%f  R:%f \n",i,framedata[i].gop_bitrate, val);
		k++;
		last_start=i;
	    }	
       }
       if(DIAG) fprintf(stderr,"all_gop_bits1:%f \n", sum);
       for(i=0; i<nframes; i++){
       		fprintf(stderr,"i: %d frame_bits %f gop_bitrate:%f \n",
			i, framedata[i].frame_bits, framedata[i].gop_bitrate);		
       }
       for(i=0; i<nframes; i++){
                framedata[i].gop_bitrate= (framedata[i].gop_bitrate*bit_rate_per_frame*all_bits)/sum;
       }     
       sum=0.0;
       for(i=0; i<nframes; i++){
        	if(framedata[i].new_gop==1){     
		sum += ((double) framedata[i].gop_n * bit_rate * framedata[i].gop_bitrate / frame_rate) + 0.5;
		fprintf(stderr,"  i: %d  bitrate:%f \n",i, framedata[i].gop_bitrate);
		k++;
		last_start=i;
	    }	
       }	     		            
       if(DIAG) fprintf(stderr,"all_gop_bits2:%f \n", sum);   
       
       for(i=0; i<nframes; i++){
       		fprintf(stderr,"i: %d frame_bits %f gop_bitrate:%f \n",
			i, framedata[i].frame_bits, framedata[i].gop_bitrate);		
       }
    }   
 }


static void readframeparmfile(fname)
char *fname;
{  
  FILE *fd;
  char c;
  char string[STRLEN], string1[STRLEN], tmp_str[STRLEN];
  int frame, sum;

  if (!(fd=fopen(fname,"r")))
  {
    sprintf(errortext,"Couldn't read frame parameter file %s",fname);
    error(errortext);
  }

  for(;;){
    c = e_getstring(fd,string,STRLEN," \n\t"," \t"," "); 
    if (DIAG) fprintf(stderr,"string a: %s\n",string);
    if(c==EOF) 
    	break; 
    if(!strncmp(string,"#",1)){
       c = e_getstring(fd,string,STRLEN,"\n"," \t"," ");
       if (DIAG) fprintf(stderr,"string b: %s\n",string);
    }else{		  		 
       frame = (int) atoix(string);
       if (DIAG) fprintf(stderr,"frame: %5d",frame);
    
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
       framedata[frame].gop_bitrate = atoid(string1);  
       if (DIAG) fprintf(stderr," gop_bitrate: %f", framedata[frame].gop_bitrate);
       if (framedata[frame].gop_bitrate==0) 
       		framedata[frame].gop_bitrate = 1.0;
              
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
       framedata[frame].M = atoix(string1);  
       if (DIAG) fprintf(stderr," M: %5d", framedata[frame].M);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," ");
       framedata[frame].N = atoix(string1);
       if (DIAG) fprintf(stderr," N: %d", framedata[frame].N);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].new_gop = atoix(string1);
       if (DIAG) fprintf(stderr," new_gop: %d", framedata[frame].new_gop);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].d_size_x = atoix(string1);
       if (DIAG) fprintf(stderr," d_size_x: %d", framedata[frame].d_size_x);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].d_size_y = atoix(string1);
       if (DIAG) fprintf(stderr," d_size_y: %d", framedata[frame].d_size_y);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].fps = atoix(string1);
       if (DIAG) fprintf(stderr," fps: %d", framedata[frame].fps);

       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].P_fcode = atoix(string1);
       if (DIAG) fprintf(stderr," P_fcode: %d", framedata[frame].P_fcode);

       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].B_fcode = atoix(string1);
       if (DIAG) fprintf(stderr," B_fcode: %d", framedata[frame].B_fcode);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].q_i = atoix(string1);
       if (DIAG) fprintf(stderr," q_i: %d", framedata[frame].q_i);

       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].q_p = atoix(string1);
       if (DIAG) fprintf(stderr," q_p: %d", framedata[frame].q_p);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].q_b = atoix(string1);
       if (DIAG) fprintf(stderr," q_b: %d", framedata[frame].q_b);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].q_ba = atoix(string1);
       if (DIAG) fprintf(stderr," q_ba: %d", framedata[frame].q_ba);
              
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].X_I = atoix(string1);
       if (DIAG) fprintf(stderr," X_I: %d", framedata[frame].X_I);

       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].X_P = atoix(string1);
       if (DIAG) fprintf(stderr," X_P: %d", framedata[frame].X_P);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].X_B = atoix(string1);
       if (DIAG) fprintf(stderr," X_B: %d", framedata[frame].X_B);

       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].bf_I = atoid(string1);
       if (DIAG) fprintf(stderr," bf_I: %f", framedata[frame].bf_I);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].bf_P = atoid(string1);
       if (DIAG) fprintf(stderr," bf_P: %f", framedata[frame].bf_P);

       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].sxp = atoix(string1);
       if (DIAG) fprintf(stderr," sxp: %d", framedata[frame].sxp);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].sxn = atoix(string1);
       if (DIAG) fprintf(stderr," sxn: %d", framedata[frame].sxn);     
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].syp = atoix(string1);
       if (DIAG) fprintf(stderr," syp: %d", framedata[frame].syp);
       
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].syn = atoix(string1);
       if (DIAG) fprintf(stderr," syn: %d", framedata[frame].syn);
                   
       c = e_getstring(fd,string1,STRLEN," \n\t"," \t"," "); 
       framedata[frame].frame_bits = atoid(string1);
       if (DIAG) fprintf(stderr," frame_bits: %f", framedata[frame].frame_bits);
           	                   	             
       c = e_getstring(fd,string1,STRLEN,"\n\t"," \t"," "); 
       framedata[frame].vbr = atoix(string1);
       if (DIAG) fprintf(stderr," vbr: %d", framedata[frame].vbr); 
       
       if (DIAG) fprintf(stderr,"\n");	     
    }
  }	
  FMAX=frame;	   
  
  fclose(fd);
}


/*-----------------------------------------------------------------
 g e t _ d i g i t

 extracts a number (also with several letters) out of a string 
 e.g.:. @VPCI15 -> returns 15
       >T7L    -> returns 7
*/
int get_digit(char *b)
{
 int kk, ii, i, flag =0;
 
 for (i = strlen(b), kk=1, ii=0; i >= 0; i--){
	if ( (flag == 0) && (isdigit(b[i-2]) != 0) && (isdigit(b[i]) != 0)) {   /* that is TmK0 !! */
		flag =1;
		if (DIAG == TRUE) fprintf(stderr,"get_digit(): Tmk0 recognized: %s\n",b);

 	} else if ( isdigit(b[i]) != 0 ) { 
		ii += (b[i] - '0') * kk;
		kk *=10;
	}
 }
 return ii;
}

/*-----------------------------------------------------------------
   getstring(fp,buf,buflen,delim,ignlead,igntrail):

   Read string from FILE 'fp' into string 'buf', until the string
   delimiter specified in 'delim'-string is encountered,
   EOF reached or a file_read_error occurred.
   Ignore leading chars specified in 'ignlead'-string and
   trailing chars specified in 'igntrail'-string.
   Limit length of 'buf' to 'buflen'.

   Return code :    NULL : error during file_read,
                    EOF  : EOF reached
                    detected delimiter char else.  

   Note: file_err gives no sense, because EOF: 
		EOF reached oder error during file_read,
*/
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
     	ffprintf(stderrlog_fd,"buf = %s\n",buf);
#endif
  return(c);
}

/*-----------------------------------------------------------------
 error_location:   - print location in file 'fp' where the error
                     occurred 
*/
void err_loc(FILE *fp)
#define FLINLEN 1024
{   
  long fpos, lpos;
  int line = 1;
  int i;
  char c, string[FLINLEN];

  fpos = ftell(fp) - 1L;
  fseek(fp,0L,0);
  do{
    c = fgetc(fp);
    if (c == '\n'){
      line++;
      lpos = ftell(fp);
    }
  }
  while (ftell(fp) < fpos);
  fseek(fp,lpos,0);
  for (i = 0; ftell(fp) <= fpos;){
     if (i < FLINLEN) string[i++] = fgetc(fp);
  }
  string[i] = '\0';
  fprintf(stderr,"   in line %d: %s\n",line,string);
}

/*-----------------------------------------------------------------
 err_exit:  err_message - string to be printed as error_message,
              fp          - file to extract error line,
 --> exit(1) after message printout 
*/
void err_exit(FILE *fp, char *err_message)
{
  fprintf(stderr,"\n** ERROR: %s\n",err_message);
  err_loc(fp);
  exit(1);
}

/*-----------------------------------------------------------------
  e_exit:  err_message - string to be printed as error_message,
  --> exit(1) after message printout 
*/
void e_exit(char *err_message)
{
  fprintf(stderr,"\n** ERROR: %s\n",err_message);
  exit(1);
}

/*-----------------------------------------------------------------
 error_handled getstring:  - transparent getstring_call,
 --> err_exit with appropriate message if EOF or NULL encountered
*/
char e_getstring(FILE *fp, char *buf, int buflen, char *delim, 
		 char *ignlead, char *igntrail)
{
  char c;
  c = getstring(fp,buf,buflen,delim,ignlead,igntrail);
  /* if ((c == EOF) || (c == NULL)) */
  if (c == EOF){
    // strcpy(err_message,(c == EOF) ? "unexpected end_of_file" : "during file_read");
    // err_exit(fp,err_message); // removed, KP
  }
  return(c);
}

/*-----------------------------------------------------------------
 fixed_string_expected e_getstring:  fp     - file to e_getstring,
                                     string - expected string,
                                     del    - expected delimiter,
  --> err_exit with appropriate message on unexpected string or delimiter 
*/

void f_getstring(FILE *fp, char *string, char *delim, char *ignlead, 
		 char *igntrail)
{
  int c;
  char buf[STRLEN];
  c = e_getstring(fp,buf,STRLEN,delim,ignlead,igntrail);
  if (strcmp(string,buf) != 0) err_exit(fp,"unexpected keyword");
}

/*-----------------------------------------------------------------
 ctohex(char):  char - character containing hex digit,

 --> returns value of hex digit (-1 = ERROR, 16 = 'x') 
*/
int ctohex(char c)
{
  if ((c == 'x') || (c == 'X')) return(16);
  if (!isxdigit(c)) return(-1);
  if (isdigit(c)) return(c - '0');
  if (islower(c)) return(c + 10 - 'a');
  else return(c + 10 - 'A');
}

/*-----------------------------------------------------------------
 atoix(str):  str - pointer to string to be interpreted
                    as an integer number in hex, octal or decimal,
		      to recognize as hex: put a 0x before the number
  --> returns long value  
*/
long atoix(char *str)
{
  long a;
  a = strtol(str,(char **)NULL,0);
  return a;
}

/*-----------------------------------------------------------------
 atoid(str):  str - pointer to string to be interpreted
                    as an integer number in hex, octal or decimal,
		      to recognize as hex: put a 0x before the number
  --> returns double  
*/
double atoid(char *str)
{
  double a;
  a = strtod(str,(char **)NULL);
  return a;
}

/*-----------------------------------------------------------------
 s h i f t l

   eliminates the first letter of the string b
*/
void shiftl(char *b)
{
  int i, max;

  max=strlen(b);
  for(i = 1; i <max; i++){  /* to remove the selector */
      b[i-1] = b[i];
  }
  b[max-1] = '\0'; /* set new stringdelimiter */
} 

/*-----------------------------------------------------------------
 c a t _ c h a r
 appends the letter c to the string b
*/
void cat_char(char *string, char c){ 
  int max;
  max=strlen(string);
  string[max] = c; 
  string[max+1] = '\0'; /* set new stringdelimiter */
} 



