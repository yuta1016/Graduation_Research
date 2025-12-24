
/* mpeg2dec.c, main(), initialization, option processing                    */

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

/** MPEG2 decoding library based on mpeg2dec.c */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#ifdef _VISUALC_
#include <io.h>
#else
#include <unistd.h>
#endif


#define GLOBAL
#include "config.h"
#include "global.h"

/*
#define VERIFY
*/
/* private prototypes */
static int  Headers _ANSI_ARGS_((void));
static void Initialize_Sequence _ANSI_ARGS_((void));
static void Initialize_Decoder _ANSI_ARGS_((void));
static void Deinitialize_Sequence _ANSI_ARGS_((void));

static int MPEG_Frame_Number;
static int first_frame = 0;
static int last_frame = 0;
static int mpeg_opened = 0;

/* #define DEBUG */



// KW 2009.05.15
int Open_MPEG_Video(unsigned char *videofile)
{
	int ret, code;
	ld = &base; /* select base layer context */
	
	if(mpeg_opened) Close_MPEG_Video();

	first_frame = 0;
	last_frame = 0;
	MPEG_Frame_Number=0;
	
	// clear flags
	Verbose_Flag = 0;
	Output_Type = 0;
	Output_Picture_Filename = 0;
	hiQdither  = 0;
	Output_Type = 0;
	Frame_Store_Flag = 0;
	System_Stream_Flag = 0;
	Spatial_Flag = 0;
	Lower_Layer_Picture_Filename = 0;
	Reference_IDCT_Flag = 0;
	Trace_Flag = 0;
	Quiet_Flag = 0;
	Ersatz_Flag = 0;
	Substitute_Picture_Filename  = 0;
	Two_Streams = 0;
	Enhancement_Layer_Bitstream_Filename = 0;
	Big_Picture_Flag = 0;
	Main_Bitstream_Flag = 0;
	Main_Bitstream_Filename = 0;
	Verify_Flag = 0;
	Stats_Flag  = 0;
	User_Data_Flag = 0; 
	// S.Herrmann for MPEG-7 Motion Vector extraction
	Motion_Vector_Filename=0;

	smolic_sync = 1;
	pics = -1;
	skip = 0;
	vectorfile=0;

	
	/* open MPEG base layer bitstream file(s) */
	/* NOTE: this is either a base layer stream or a spatial enhancement stream */
	if ((base.Infile=open(videofile,O_RDONLY|O_BINARY))<0)
	{
		fprintf(stderr,"Base layer input file %s not found\n", Main_Bitstream_Filename);
		return -1;
	}

	
	Initialize_Buffer(); 
		
	if(Show_Bits(8)==0x47)
	{
		printf("Decoder currently does not parse transport streams\n");
		return -1;;
	}
		
	next_start_code();
	code = Show_Bits(32);
		
	switch(code)
	{
	case SEQUENCE_HEADER_CODE:
		break;
	case PACK_START_CODE:
		System_Stream_Flag = 1;
	case VIDEO_ELEMENTARY_STREAM:
		System_Stream_Flag = 1;
		break;
	default:
		printf("Unable to recognize stream type\n");
		return -1;
	}
	
	lseek(base.Infile, 0l, 0);
	Initialize_Buffer();
	
	MPEG_Frame_Number = 0;

	Initialize_Decoder();

    ret = Get_Sequence();
	if(ret != 1)
	{
		return -1;
	}

	Initialize_Sequence();

	lseek(base.Infile, 0l, 0);
	Initialize_Buffer();

	mpeg_opened=1;

	return 0;
}

unsigned char *Get_Next_MPEG_frame()
{
	int ret;
	unsigned char *frameptr = 0;
	//static int reference_count = 0;

	if(last_frame)
	{
		return 0;
	}

	if(!first_frame)
	{
		ret=Headers();
		if(ret != 1)
		{
			last_frame=1;
			return 0;
		}
		Decode_Picture(MPEG_Frame_Number, MPEG_Frame_Number);

		if(Second_Field)
		{
			ret=Headers();
			if(ret != 1)
			{
				last_frame=1;
				return 0;
			}
			Decode_Picture(MPEG_Frame_Number, MPEG_Frame_Number);
		}
		first_frame = 1;
	}
	ret=Headers();
	if(ret != 1)
	{
		last_frame=1;
	}
	else
	{
		Decode_Picture(MPEG_Frame_Number, MPEG_Frame_Number);
		if(Second_Field)
		{
			ret=Headers();
			if(ret != 1)
			{
				last_frame=1;
				return 0;
			}
			Decode_Picture(MPEG_Frame_Number, MPEG_Frame_Number);
		}
	}
	if(last_frame)
	{
		frameptr=backward_reference_frame[0];
	}
	else if (picture_coding_type==B_TYPE)
		frameptr = auxframe[0];
	else
	{
		frameptr = forward_reference_frame[0];
	}


	MPEG_Frame_Number++;
	return frameptr;

}

int Get_MPEG_width()
{
	return horizontal_size;
}

int Get_MPEG_stride()
{
	return Coded_Picture_Width;
}

int Get_MPEG_height()
{
	return vertical_size;
}

int Get_MPEG_pixel_format()
{
	return chroma_format;
}

int Get_MPEG_is_interlaced()
{
	return (int)(progressive_frame==0);
}

double Get_MPEG_framerate()
{
	return frame_rate;
}


void Close_MPEG_Video()
{
	if(mpeg_opened == 0) return;
	Deinitialize_Sequence();
	close(base.Infile);
	mpeg_opened=0;
}



/* IMPLEMENTAION specific rouintes */
static void Initialize_Decoder()
{
	int i;
	
	/* Clip table */
	if (!(Clip=(unsigned char *)malloc(1024)))
		Error("Clip[] malloc failed\n");
	
	Clip += 384;
	
	for (i=-384; i<640; i++)
		Clip[i] = (i<0) ? 0 : ((i>255) ? 255 : i);
	
	/* IDCT */
	if (Reference_IDCT_Flag)
		Initialize_Reference_IDCT();
	else
		Initialize_Fast_IDCT();
	
}

/* mostly IMPLEMENTAION specific rouintes */
static void Initialize_Sequence()
{
  int cc, size;
  static int Table_6_20[3] = {6,8,12};

  /* check scalability mode of enhancement layer */
  if (Two_Streams && (enhan.scalable_mode!=SC_SNR) && (base.scalable_mode!=SC_DP))
    Error("unsupported scalability mode\n");

  /* force MPEG-1 parameters for proper decoder behavior */
  /* see ISO/IEC 13818-2 section D.9.14 */
  if (!base.MPEG2_Flag)
  {
    progressive_sequence = 1;
    progressive_frame = 1;
    picture_structure = FRAME_PICTURE;
    frame_pred_frame_dct = 1;
    chroma_format = CHROMA420;
    matrix_coefficients = 5;
  }

  /* round to nearest multiple of coded macroblocks */
  /* ISO/IEC 13818-2 section 6.3.3 sequence_header() */
  mb_width = (horizontal_size+15)/16;
  mb_height = (base.MPEG2_Flag && !progressive_sequence) ? 2*((vertical_size+31)/32)
                                        : (vertical_size+15)/16;

  Coded_Picture_Width = 16*mb_width;
  Coded_Picture_Height = 16*mb_height;

  /* ISO/IEC 13818-2 sections 6.1.1.8, 6.1.1.9, and 6.1.1.10 */
  Chroma_Width = (chroma_format==CHROMA444) ? Coded_Picture_Width
                                           : Coded_Picture_Width>>1;
  Chroma_Height = (chroma_format!=CHROMA420) ? Coded_Picture_Height
                                            : Coded_Picture_Height>>1;
  
  /* derived based on Table 6-20 in ISO/IEC 13818-2 section 6.3.17 */
  block_count = Table_6_20[chroma_format-1];

  for (cc=0; cc<3; cc++)
  {
    if (cc==0)
      size = Coded_Picture_Width*Coded_Picture_Height;
    else
      size = Chroma_Width*Chroma_Height;

    if (!(backward_reference_frame[cc] = (unsigned char *)malloc(size)))
      Error("backward_reference_frame[] malloc failed\n");

    if (!(forward_reference_frame[cc] = (unsigned char *)malloc(size)))
      Error("forward_reference_frame[] malloc failed\n");

    if (!(auxframe[cc] = (unsigned char *)malloc(size)))
      Error("auxframe[] malloc failed\n");

    if(Ersatz_Flag)
      if (!(substitute_frame[cc] = (unsigned char *)malloc(size)))
        Error("substitute_frame[] malloc failed\n");


    if (base.scalable_mode==SC_SPAT)
    {
      /* this assumes lower layer is 4:2:0 */
      if (!(llframe0[cc] = (unsigned char *)malloc((lower_layer_prediction_horizontal_size*lower_layer_prediction_vertical_size)/(cc?4:1))))
        Error("llframe0 malloc failed\n");
      if (!(llframe1[cc] = (unsigned char *)malloc((lower_layer_prediction_horizontal_size*lower_layer_prediction_vertical_size)/(cc?4:1))))
        Error("llframe1 malloc failed\n");
    }
  }

  /* SCALABILITY: Spatial */
  if (base.scalable_mode==SC_SPAT)
  {
    if (!(lltmp = (short *)malloc(lower_layer_prediction_horizontal_size*((lower_layer_prediction_vertical_size*vertical_subsampling_factor_n)/vertical_subsampling_factor_m)*sizeof(short))))
      Error("lltmp malloc failed\n");
  }

#ifdef DISPLAY
  if (Output_Type==T_X11)
  {
    Initialize_Display_Process("");
    Initialize_Dither_Matrix();
  }
#endif /* DISPLAY */

}

void Error(text)
char *text;
{
  fprintf(stderr,text);
  exit(1);
}





static int Headers()
{
  int ret;

  ld = &base;
  

  /* return when end of sequence (0) or picture
     header has been parsed (1) */

  ret = Get_Hdr();

  if (Two_Streams)
  {
    ld = &enhan;
    if (Get_Hdr()!=ret && !Quiet_Flag)
      fprintf(stderr,"streams out of sync\n");
    ld = &base;
  }

  return ret;
}


static void Deinitialize_Sequence()
{
  int i;

  /* clear flags */
  base.MPEG2_Flag=0;

  for(i=0;i<3;i++)
  {
    free(backward_reference_frame[i]);
    free(forward_reference_frame[i]);
    free(auxframe[i]);

    if (base.scalable_mode==SC_SPAT)
    {
     free(llframe0[i]);
     free(llframe1[i]);
    }
  }

  if (base.scalable_mode==SC_SPAT)
    free(lltmp);

}

