//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips
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
// Applicable File Name:  camera_motion_extraction.c
//        

#include  "camera_motion_extraction.h"

MediaForCameraMotion * ComputeFromText( char* media_parameters) 
{
	char* intermediate  ;
	int index = 0 ;
	int start, stop, h_size, v_size ;
	double frame_rate;
	MediaForCameraMotion * media;
	media = (MediaForCameraMotion *) malloc(sizeof(MediaForCameraMotion));

//	while (media_parameters[index] != "[")
//		strcat(intermediate,&media_parameters[index]) ;
	intermediate = strrchr(media_parameters,',') ;
	*intermediate=0 ;
	intermediate++ ;
	v_size = atoi(intermediate) ;

	intermediate = strrchr(media_parameters,',') ;
	*intermediate=0 ;
	*(intermediate-1) = 0 ;
	intermediate++ ;
	h_size = atoi(intermediate) ;

	// 2014/03 - frame rate
	intermediate = strrchr(media_parameters,',') ;
	*intermediate=0 ;
	intermediate++ ;
	frame_rate = atof(intermediate) ;

	intermediate = strrchr(media_parameters,',') ;
	*intermediate=0 ;
	intermediate++ ;
	stop = atoi(intermediate) ;

	intermediate = strrchr(media_parameters,'[') ;
	*intermediate=0 ;
	intermediate++ ;
	start = atoi(intermediate) ;


	strcpy(media->mpeg_file_name,media_parameters) ;
	media->start_frame = start ;
	media->stop_frame = stop ;
	media->h_im_size = h_size ;
	media->v_im_size = v_size ;
	media->frame_rate = frame_rate ;

	return media ;
}


CameraMotionInfo_t ExtractCameraParameters( MediaForCameraMotion aMedia  ){

   int h_size      = aMedia.h_im_size; 
   int v_size      = aMedia.v_im_size; 
   int Start_frame = aMedia.start_frame; 
   int Stop_frame  = aMedia.stop_frame;
   double Frame_rate = aMedia.frame_rate;

   mat_size_t           image_dimensions, image_block_dimensions;
   int                  *bma_buffer ;
   motion_histogram_t   motion_types_histo  ;
   motion_histogram_t   motion_speeds_histo ;
   med_low_level_desc_t motion_chrono ;
   CameraMotionInfo_t   cmInfo ;
   double a ;

   char		raw_file_name[STRSIZE] ;
   char     command[512] ;

   strcpy(raw_file_name,"decoded_2") ;
   printf("\n \n MPEG VIDEO DECODING : %s, %d -> %d \n",aMedia.mpeg_file_name,aMedia.start_frame,aMedia.stop_frame) ;
   printf(" _________________________________________________________________ \n\n");


#ifdef WIN32
   // Windows del
   sprintf( command, "del %s.Y",raw_file_name) ;
   system(command) ;
   sprintf( command, "del %s.U",raw_file_name) ;
   system(command) ;
   sprintf( command, "del %s.V",raw_file_name) ;
   system(command) ;
#else
   // Linux remove
   sprintf( command, "rm %s.Y",raw_file_name) ;
   system(command) ;
   sprintf( command, "rm %s.U",raw_file_name) ;
   system(command) ;
   sprintf( command, "rm %s.V",raw_file_name) ;
   system(command) ;
#endif // WIN32


   sprintf(command,"mpeg2decode -s %d -p %d -b %s -o0 %s",Start_frame,Stop_frame-Start_frame+1,aMedia.mpeg_file_name,raw_file_name) ;
   system(command) ;
                  
   // sth printf will create confusing output. You are only allowed
   // sth to write to stderr
   fprintf(stdout,"\n\n");
   fprintf(stdout," MPEG-7 XM : CAMERA MOTION EXTRACTION TOOL    \n");  
   fprintf(stdout," Report bugs to <Benoit.Mory@philips.com> \n\n") ;

   /*Frame boundaries checkings */
 //  if (Start_frame == 0)  Start_frame = 1 ;
 //  if (Start_frame >= Stop_frame) Stop_frame=Start_frame+1 ;

   image_dimensions.lines = v_size ;
   image_dimensions.cols  = h_size ;

   /* PERFORMING BLOCK MATCHING */
   fprintf(stderr,"\n \n MOTION VECTORS COMPUTATION BY FIXED SIZE BLOCK MATCHING ALGORITHM \n") ;
   fprintf(stderr," _________________________________________________________________ \n\n");
   fprintf(stderr," IMAGE FORMAT : %d by %d,  BLOCK_SIZE : %d\n", image_dimensions.cols, image_dimensions.lines, MOT_BSIZE ); 
   
   strcat(raw_file_name,".Y") ;

   bma_buffer = (int*) compute_block_matching_raw(raw_file_name, 0 , Stop_frame-Start_frame, image_dimensions ) ; 
 
   /* GLOBAL CAMERA MOTION ANALYSIS */
   image_block_dimensions.lines  = image_dimensions.lines / MOT_BSIZE ;
   image_block_dimensions.cols   = image_dimensions.cols   / MOT_BSIZE ;
   
   // sth not allowd to be used in a per sequence base
   // sth please use predefined way to extract your descriptor
   // sth otherwise nobody can reuse your technique!!!
   shot_camera_motion_descriptor_extraction( bma_buffer,Stop_frame-Start_frame+1, image_block_dimensions, &motion_types_histo, &motion_speeds_histo, &motion_chrono) ; 
   // sth did you ever tried to print your listing?

   // KW 2014/03 - set startTime/duration in milliseconds
   if(Frame_rate <= 0) Frame_rate = 1;
   cmInfo.startTime  = (long)(1000.0*Start_frame/Frame_rate);
   cmInfo.duration   = (long)(1000.0*(Stop_frame - Start_frame + 1)/Frame_rate) ;
   //cmInfo.startTime  = Start_frame ;
   //cmInfo.duration   = Stop_frame - Start_frame + 1 ;
   cmInfo.x_FoeFoc   = 0 ;
   cmInfo.y_FoeFoc   = 0 ;

   cmInfo.presence[0]   = motion_types_histo[TRACK_LEFT] ;
   cmInfo.presence[1]   = motion_types_histo[TRACK_RIGHT] ;
   cmInfo.presence[2]   = motion_types_histo[BOOM_DOWN] ;
   cmInfo.presence[3]   = motion_types_histo[BOOM_UP] ;
   cmInfo.presence[4]   = 0 ; // unable to detect dolly movements 
   cmInfo.presence[5]   = 0 ; // unable to detect dolly movements
   cmInfo.presence[6]   = motion_types_histo[PAN_LEFT] ;
   cmInfo.presence[7]   = motion_types_histo[PAN_RIGHT] ;
   cmInfo.presence[8]   = motion_types_histo[TILT_UP] ;
   cmInfo.presence[9]   = motion_types_histo[TILT_DOWN] ;
   cmInfo.presence[10]  = motion_types_histo[ROLL_RIGHT] ;
   cmInfo.presence[11]  = motion_types_histo[ROLL_LEFT] ;
   cmInfo.presence[12]  = motion_types_histo[ZOOM_IN] ;
   cmInfo.presence[13]  = motion_types_histo[ZOOM_OUT] ;
   cmInfo.presence[14]    = motion_types_histo[FIXED] ;


   cmInfo.speeds[0]   = motion_speeds_histo[TRACK_LEFT] / ( 2 * image_dimensions.lines ) ;
   cmInfo.speeds[1]   = motion_speeds_histo[TRACK_RIGHT] / ( 2 * image_dimensions.lines ) ;
   cmInfo.speeds[2]   = motion_speeds_histo[BOOM_DOWN] / ( 2 * image_dimensions.cols );
   cmInfo.speeds[3]   = motion_speeds_histo[BOOM_UP] / ( 2 * image_dimensions.cols ) ;
   cmInfo.speeds[4]   = 0 ; // unable to detect dolly movements 
   cmInfo.speeds[5]   = 0 ; // unable to detect dolly movements
   cmInfo.speeds[6]   = motion_speeds_histo[PAN_LEFT] / ( 2 * image_dimensions.lines ) ;
   cmInfo.speeds[7]   = motion_speeds_histo[PAN_RIGHT] / ( 2 * image_dimensions.lines ) ;
   cmInfo.speeds[8]   = motion_speeds_histo[TILT_UP] / ( 2 * image_dimensions.cols ) ;
   cmInfo.speeds[9]   = motion_speeds_histo[TILT_DOWN] / ( 2 * image_dimensions.cols ) ;
   cmInfo.speeds[10]  = motion_speeds_histo[ROLL_RIGHT] ;
   cmInfo.speeds[11]  = motion_speeds_histo[ROLL_LEFT] ;

   a = (motion_speeds_histo[ZOOM_IN]/2) * cos( atan( ((float)image_dimensions.cols) / ((float)image_dimensions.lines) ) ) / ((float)image_dimensions.cols) ;
   cmInfo.speeds[12]  =  4*(a-a*a)  ;
   a = (motion_speeds_histo[ZOOM_OUT]/2) * cos( atan( ((float)image_dimensions.cols) / ((float)image_dimensions.lines) ) ) / ((float)image_dimensions.cols) ;
   cmInfo.speeds[13]  =  4*(a-a*a) ;

   return cmInfo ;
}

void  find_vectors(short *y_buffer, vector_t *vectors, mat_size_t image_size )    
{
    int                 frame_size;
    int                 num_blocks;
    int                 num_pixels, num_lines ;

    num_pixels = image_size.cols ;
    num_lines  = image_size.lines ;
    frame_size = image_size.lines * image_size.cols;
    num_blocks = frame_size / (MOT_BSIZE * MOT_BSIZE);

   /* Computation of forward vectors */
    search_fullpel(  y_buffer, y_buffer + frame_size, num_pixels, num_lines, MOT_BSIZE, MOT_BSIZE,  SEARCH_RANGE, SEARCH_RANGE,  &vectors[0] );
}

void  search_fullpel(
         short *frame1, 
         short *frame2,
         int h_size, 
         int v_size, 
         int mb_h_size, 
         int mb_v_size,
         int range_x, 
         int range_y,
         vector_t * vector)
{
    int           num_blocks;  
    register int  block; 
    int           x, y;  
    register int  pixel, line; 
    int           fr1_addr, fr2_addr;   
    register int  dx, dy;
    int           min_dx, min_dy;
    int           cost, min_cost; 
    int           value;

    num_blocks = (h_size / mb_h_size) * (v_size / mb_v_size);

    for (block = 0; block < num_blocks; block++)
    {
      x = mb_h_size * (block % (h_size/mb_h_size));
      y = mb_v_size * (block / (h_size/mb_h_size));
      
      min_cost = mb_h_size * mb_v_size * MAXY ;
      min_dx = 0;
      min_dy = 0;

      for (dy = -range_y; dy <= +range_y; dy++) 
      {
        if (y + dy < 0 || y + dy + mb_v_size > v_size)      
          goto next_y;
        for (dx = -range_x; dx <= +range_x; dx ++) 
        {
          if (x + dx < 0 || x + dx + mb_h_size > h_size )   
            goto next_x;

          cost = 0; 
          for (line = y; line < y + mb_v_size; line ++) 
          {
            fr1_addr = line * h_size;
            fr2_addr = (line + dy) * h_size;

            for (pixel = x; pixel < x + mb_h_size; pixel++) 
            {
              value = (*(frame1 + fr1_addr + pixel) - *(frame2 + fr2_addr + pixel + dx));
              cost += abs(value);
              if (cost > min_cost)
                goto next_x;
            }
          }
          
          if (cost < min_cost) 
          {
            min_cost = cost;
            min_dx = dx;
            min_dy = dy;
          }
          next_x:;
        }
        next_y:;
      }
      
      vector[block].x = min_dx;
      vector[block].y = min_dy;
   }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                        COMPUTE_BLOCK_MATCHING_RAW                                   *
 *                                                                                     *
 *    Purpose          : outputs a buffer of motion vectors from a yuv sequence        *
 *                                                                                     *
 *    Arguments in     : char * raw_filename   -  name of the sequence                 *
 *                       int frame_start         -  first frame                        *
 *                       int frame_stop          -  last frame                         *
 *                       mat_size_t   image_size -  size of the images (colsxlines)    *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : buffer of int values (x,y,x,y,x...)                           *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
int * compute_block_matching_raw( char * raw_filename , int frame_start, int frame_stop, mat_size_t image_size  )
{
 
  
   static short    *buffer ;
   static vector_t *vectors ;
   int             *bma_buffer ;
   int             frame_number, num_blocks, frame_size, block ;
   
   frame_size = image_size.lines * image_size.cols ;
   num_blocks = frame_size / (MOT_BSIZE * MOT_BSIZE) ;
   
   if ( ( buffer = (short *) calloc( 2*frame_size, sizeof(short) ) ) == NULL)
   {
     fprintf(stderr," *** ERROR : Memory allocation problem !! *** (buffer) \n");
     exit(1);
   }
     
   if ( ( vectors = (vector_t *) calloc(2*num_blocks, sizeof(vector_t))) == NULL)
   {
     fprintf(stderr," *** ERROR : Memory allocation problem  !! *** (vectors) \n");
     exit(1);
   }
     
   if ( ( bma_buffer = (int *) calloc ( 2 * (frame_stop-frame_start+1) * num_blocks, sizeof(int) ) ) == NULL)
   {
     fprintf(stderr," *** ERROR : Memory allocation problem  !! *** (bma_buffer) \n");
     exit(1);
   }
     
   /********** read images, call procedure to find motion vectors ***********/
   
   printf(" Finding forward motion vectors... \n") ;
   
   for ( frame_number = frame_start; frame_number < frame_stop; frame_number++)
   {
     ReadY_RAW( raw_filename, frame_number+1,buffer+frame_size,image_size.cols,image_size.lines) ;
     ReadY_RAW( raw_filename, frame_number,buffer,image_size.cols,image_size.lines) ;

     fprintf(stderr," Frame #%d    \r",frame_number+1) ;
     
     find_vectors(buffer,vectors,image_size);
              
     for(block=0; block<num_blocks; block++)
     {
       bma_buffer[ 2*num_blocks*( frame_number-frame_start) + 2*block]   =    vectors[block].x ;  
       bma_buffer[ 2*num_blocks*( frame_number-frame_start) + 2*block + 1] =  vectors[block].y ;  
    
       vectors[block].x = 0 ;
       vectors[block].y = 0 ;
     }
   }

   fprintf(stderr,"                                   \n") ;
   
   free(vectors) ; 
   free(buffer) ;
   
   return( bma_buffer ) ;
}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                             CONSTRUCT_START_SIMPLEX                                 *
 *                                                                                     *
 *    Purpose          : Constructs a starting NxN+1 simplex according to a single     *
 *                       guess                                                         *
 *                                                                                     *
 *    Arguments in     : inst_rot_param_t init_vect - guess vector                     *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    : simplex_t * start_simplex  - starting simplex computed        *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void construct_start_simplex ( simplex_t * start_simplex, inst_rot_param_t init_vect ) 
{   
  (*start_simplex)[0][0] = init_vect[0]    ;
  (*start_simplex)[0][1] = init_vect[1]    ;
  (*start_simplex)[0][2] = init_vect[2]    ;
  (*start_simplex)[0][3] = init_vect[3]    ;
  (*start_simplex)[0][4] = init_vect[4]    ;
  
  (*start_simplex)[1][0] = init_vect[0] + (float) ELEM_INC ;
  (*start_simplex)[1][1] = init_vect[1]            ;
  (*start_simplex)[1][2] = init_vect[2]            ;
  (*start_simplex)[1][3] = init_vect[3]            ;
  (*start_simplex)[1][4] = init_vect[4]            ;
   
  (*start_simplex)[2][0] = init_vect[0]            ;
  (*start_simplex)[2][1] = init_vect[1] + (float) ELEM_INC ;
  (*start_simplex)[2][2] = init_vect[2]            ;
  (*start_simplex)[2][3] = init_vect[3]            ;
  (*start_simplex)[2][4] = init_vect[4]            ;
   
  (*start_simplex)[3][0] = init_vect[0]            ;
  (*start_simplex)[3][1] = init_vect[1]            ;
  (*start_simplex)[3][2] = init_vect[2] + (float) ELEM_INC ;
  (*start_simplex)[3][3] = init_vect[3]            ;
  (*start_simplex)[3][4] = init_vect[4]            ;
 
  (*start_simplex)[4][0] = init_vect[0]            ;
  (*start_simplex)[4][1] = init_vect[1]            ;
  (*start_simplex)[4][2] = init_vect[2]            ;
  (*start_simplex)[4][3] = init_vect[3] + (float) ELEM_INC ;
  (*start_simplex)[4][4] = init_vect[4]            ;
   
  (*start_simplex)[5][0] = init_vect[0]              ;
  (*start_simplex)[5][1] = init_vect[1]              ;
  (*start_simplex)[5][2] = init_vect[2]              ; 
  (*start_simplex)[5][3] = init_vect[3]              ;
  (*start_simplex)[5][4] = init_vect[4] +  (float) FOCAL_INC ;   
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                COST_FUNCTION                                        *
 *                                                                                     *
 *    Purpose          : Computes parallelism cost function                            *
 *                                                                                     *
 *    Arguments in     : flow_t flow                - flow field                       *
 *                       block_t * mask_1D          - Confidence mask for ref. points  *
 *                       inst_rot_param_t rot_vect  - considered vector                *
 *                       mat_size_t im_bsize        - image size (in blocks)           *
 *                       int bsize                  - block size                       *
 *                       int nb_ref_points          - number of reference points       *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : Cost function value (float)                                   *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float cost_function( flow_t flow, block_t * mask_1D, inst_rot_param_t rot_vect, mat_size_t im_bsize,   int bsize,   int nb_ref_points )
{
  float            Rx, Ry, Rz, Rzoom, f, centre_x, centre_y ;
  float            x, y, Ux, Uy, norm_V, norm_vi, cost ;
  float_vector_t   resultant_vect ;
  float_vector_t * res_flow_1D ;
  int              cpt ;

  res_flow_1D = (float_vector_t *) calloc( nb_ref_points, sizeof( float_vector_t ) ) ;
  if ( res_flow_1D == NULL )
  {
    fprintf(stderr,"\n *** ERROR, Memory allocation problem (res_flow_1D) ***\n\n") ;
    exit(1) ;
  }
  
  Rx =    rot_vect[0] ;
  Ry =    rot_vect[1] ;
  Rz =    rot_vect[2] ;
  Rzoom = rot_vect[3] ;
  f     = rot_vect[4] ;
  
  centre_x = ((float) (im_bsize.cols-1))  / 2 ;
  centre_y = ((float) (im_bsize.lines-1)) / 2 ;
  
  resultant_vect.x = 0 ;
  resultant_vect.y = 0 ;
  
  for (cpt=0; cpt<nb_ref_points; cpt++)
    {
      x = bsize*(mask_1D[cpt].n-centre_x)  -flow[mask_1D[cpt].m][mask_1D[cpt].n].x ; 
      y = bsize*(mask_1D[cpt].m-centre_y)  -flow[mask_1D[cpt].m][mask_1D[cpt].n].y ; 
      
      /* Induced velocity according to model 1 */
      Ux =    x*y*Rx/f - f*(1+ SQR(x)/SQR(f) )*Ry + f*((float)atan(x/f))*(1+SQR(x)/SQR(f) )*Rzoom + y*Rz ;
      Uy =   -x*y*Ry/f + f*(1+ SQR(y)/SQR(f) )*Rx +  f*((float)atan(y/f))*(1+SQR(y)/SQR(f) )*Rzoom - x*Rz ;
      
      /* Residual flow computation */
      res_flow_1D[cpt].x = flow[mask_1D[cpt].m][mask_1D[cpt].n].x - Ux ;
      res_flow_1D[cpt].y = flow[mask_1D[cpt].m][mask_1D[cpt].n].y - Uy ;
      
      resultant_vect.x += mask_1D[cpt].value * res_flow_1D[cpt].x ;
      resultant_vect.y += mask_1D[cpt].value * res_flow_1D[cpt].y ;  
    }
  
  cost = 0 ;
  norm_V = (float) NORM(resultant_vect)  ;
  
  for (cpt=0; cpt<nb_ref_points; cpt++)
    {
      norm_vi = (float) NORM(res_flow_1D[cpt]) ; 
      
      if ( (norm_vi != 0) & (norm_V != 0) )
         cost += SQR(norm_vi) * (float) asin( ABS( (res_flow_1D[cpt].x * resultant_vect.y - resultant_vect.x * res_flow_1D[cpt].y)/(norm_V*norm_vi) ) ) * mask_1D[cpt].value ;
    }

  free(res_flow_1D) ;
  
  return cost ;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                GET_REAL_PARAM                                       *
 *                                                                                     *
 *    Purpose          : Computes real parameters from rotational parameters           *
 *                                                                                     *
 *    Arguments in     : flow_t flow                - flow field                       *
 *                       block_t * mask_1D          - Confidence mask for ref. points  *
 *                       inst_rot_param_t rot_vect  - considered rotational parameters *
 *                       mat_size_t im_bsize        - image size (in blocks)           *
 *                       int bsize                  - block size                       *
 *                       int nb_ref_points          - number of reference points       *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    : inst_cam_param_t * real_param  - all  parameters computed     *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void get_real_param(inst_cam_param_t * real_param, flow_t flow, inst_rot_param_t rot_vect, block_t * mask_1D, mat_size_t im_bsize,   int bsize,   int nb_ref_points ) 
{
  
  int               cpt ;
  float             Rx, Ry, Rz, Rzoom, f, Alpha, Beta, Zoom_dx, Zoom_dy ;
  float             sum_mask, sum_mod_ui, sum_mod_vi, mod_sum_vi, x, y, Ux, Uy ;
  float             centre_x, centre_y ;
  float_vector_t    resultant_vect ;
  float_vector_t *  res_flow_1D ;
    
  res_flow_1D = (float_vector_t *) calloc( nb_ref_points, sizeof( float_vector_t ) ) ;
  if ( res_flow_1D == NULL )
  {
    fprintf(stderr,"\n *** ERROR, Memory allocation problem (res_flow_1D) ***\n\n") ;
    exit(1) ;
  }
  
  Rx     = rot_vect[0] ;
  Ry     = rot_vect[1] ;
  Rz     = rot_vect[2] ;
  Rzoom  = rot_vect[3] ;
  f      = rot_vect[4] ;
  
  centre_x = ((float)(im_bsize.cols-1))  / 2 ;
  centre_y = ((float)(im_bsize.lines-1)) / 2 ;
  
  resultant_vect.x = 0 ;
  resultant_vect.y = 0 ;
  
  sum_mask = 0 ;
  sum_mod_ui = 0 ;
  sum_mod_vi = 0 ;
 
  for (cpt=0; cpt<nb_ref_points; cpt++)
    {
      x = bsize*(mask_1D[cpt].n-centre_x)  -flow[mask_1D[cpt].m][mask_1D[cpt].n].x ; 
      y = bsize*(mask_1D[cpt].m-centre_y)  -flow[mask_1D[cpt].m][mask_1D[cpt].n].y ; 
      
      /* Induced velocity according to model 1 */
      Ux =    x*y*Rx/f - f*(1+ SQR(x)/SQR(f) )*Ry + f*((float)atan(x/f))*(1+SQR(x)/SQR(f) )*Rzoom + y*Rz ;
      Uy =   -x*y*Ry/f + f*(1+ SQR(y)/SQR(f) )*Rx + f*((float)atan(y/f))*(1+SQR(y)/SQR(f) )*Rzoom - x*Rz ;
        
      /* Residual flow computation */
      res_flow_1D[cpt].x = flow[mask_1D[cpt].m][mask_1D[cpt].n].x - Ux ;
      res_flow_1D[cpt].y = flow[mask_1D[cpt].m][mask_1D[cpt].n].y - Uy ;
      
      resultant_vect.x += mask_1D[cpt].value * res_flow_1D[cpt].x ;
      resultant_vect.y += mask_1D[cpt].value * res_flow_1D[cpt].y ;  
      
      sum_mod_vi += (float) NORM(res_flow_1D[cpt]) * mask_1D[cpt].value ;
      sum_mod_ui += (float) NORM(flow[mask_1D[cpt].m][mask_1D[cpt].n]) * mask_1D[cpt].value ;
      sum_mask   += mask_1D[cpt].value ;
    }
    
  mod_sum_vi = (float) sqrt( SQR(resultant_vect.x) + SQR(resultant_vect.y) ) ;
  
  if ( sum_mask != 0 )
  {
    resultant_vect.x = resultant_vect.x / sum_mask ;
    resultant_vect.y = resultant_vect.y / sum_mask ;
  }
  
  if ( sum_mod_ui != 0 )
    Alpha = sum_mod_vi / sum_mod_ui ;
  else
    Alpha = 0 ;
    
  if ( sum_mod_vi != 0 ) 
    Beta = mod_sum_vi / sum_mod_vi ;
  else
    Beta = 0 ; 
  
  /* Aplha and Beta ratios features correction */
  if ( Alpha > ALPHA_THRESHOLD )
  {
    Rx = Ry = Rz = Rzoom = 0 ;
    if (Beta < BETA_THRESHOLD)
    {
      resultant_vect.x = resultant_vect.y = 0 ;
    }
  }
    
  (*real_param)[0] = - resultant_vect.x ;
  (*real_param)[1] = - resultant_vect.y ;
  
  (*real_param)[2] = Rx * f ;   /* angle -> Average pixel displacement unit */
  (*real_param)[3] = Ry * f ;   /* angle -> Average pixel displacement unit */
  (*real_param)[4] = Rz     ;   /* No unit change */
  
  /* Zoom unit : Angle magnification -> Displacement of a corner pixel */
  x         = ((float) im_bsize.lines * bsize ) / 2 ;
  y         = ((float) im_bsize.cols  * bsize ) / 2 ;
  Zoom_dx   = f * ((float) atan ( x/f ))  * ( 1 + SQR(x) / SQR(f) ) * Rzoom ;
  Zoom_dy   = f * ((float) atan ( y/f ))  * ( 1 + SQR(y) / SQR(f) ) * Rzoom ;
  
  if ( Rzoom > 0 )
    (*real_param)[5] =   (float) sqrt( SQR(Zoom_dx) + SQR(Zoom_dy) ) ; 
  else
    (*real_param)[5] = - (float)  sqrt( SQR(Zoom_dx) + SQR(Zoom_dy) ) ; 
  
  (*real_param)[6] = Alpha ;
  (*real_param)[7] = Beta  ;
  (*real_param)[8] = f     ;
  
  if (! ((ABS((*real_param)[0])<MOTION_SENSITIVITY)
        &(ABS((*real_param)[1])<MOTION_SENSITIVITY)
        &(ABS((*real_param)[2])<MOTION_SENSITIVITY)
        &(ABS((*real_param)[3])<MOTION_SENSITIVITY)
        &(ABS((*real_param)[4])<MOTION_SENSITIVITY)
        &(ABS((*real_param)[5])<MOTION_SENSITIVITY)) )
    (*real_param)[9] = 0 ; 
  else
    (*real_param)[9] = 1 ;
     
  free(res_flow_1D) ;
}
     
     
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                SIMPLEX_MINI                                         *
 *                                                                                     *
 *    Purpose          : Performs the simplex minimization algorithm                   *
 *                                                                                     *
 *    Arguments in     : simplex_t simplex          - Starting simplex (NxN+1)         *
 *                       int nb_it_min              - minimum nb of iterations         *
 *                       int nb_it_max              - maximum number of iterations     *
 *                       float epsilon              - cost function precision required *
 *                       flow_t flow                - motion flow field                *
 *                       block_t * mask_1D          - confidence mask                  *
 *                       mat_size_t im_bsize        - image size (in blocks)           *
 *                       int bsize                  - block size                       *
 *                       int nb_ref_points          - number of reference points       *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    : inst_rot_param_t * vect_mini - Minimum vector                 *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
 
void  simplex_mini ( simplex_t simplex, inst_rot_param_t * vect_mini, int nb_it_min, int nb_it_max, float epsilon, flow_t flow, block_t * mask_1D, mat_size_t im_bsize, int bsize ,   int nb_ref_points)
/*
 * simplex   :          Starting simplex (NxN+1)
 * N         :          Space dimension
 * nb_it_min :          Minimum number of iterations to be performed
 * nb_it_max :          Maximum number of iterations
 * epsilon   :          Algorithm precision 
 */
{
  float  Y[NB_PARA+1]  ;      /* Function value vector */
  float  Z[NB_PARA]  ;        /* Function argument     */
  float  M[NB_PARA]  ;        /* Vertex M              */
  float  R[NB_PARA]  ;        /* Vertex R              */
  float  E[NB_PARA]  ;        /* Vertex E              */
  float  C[NB_PARA]  ;        /* Vertex C              */
  float  C2[NB_PARA] ;        /* Vertex C2              */
  float yR, yE, yC, yC2 ;     /* Function Values for each computed vertex */
  
  int i, j ;
  int lo, li, ho, hi ;
  int nb_it ;
  
  const int N = NB_PARA ;
  
  /* * * * * * * * * * * * * * * * * * * * */
  /* Initialisations                       */
  /* * * * * * * * * * * * * * * * * * * * */
 
  for (j=0 ; j<N+1; j++)
  {
     for (i=0; i<N; i++)
       Z[i] = simplex[j][i] ;
     
     Y[j] = cost_function( flow, mask_1D, Z, im_bsize, bsize, nb_ref_points ) ;
  }
  
  /* * * * * * * * * * * * */
  /* Ordering the vertices */
  /* * * * * * * * * * * * */
  
  lo = arg_min(Y,N+1) ;
  hi = arg_max(Y,N+1) ;
  li = hi ;
  ho = lo ;
  
  for (j=0; j<N+1; j++)
  {
    if ((j!=lo) & (j!=hi) & (Y[j]<=Y[li]))
      li = j ;
    if ((j!=hi) & (j!=lo) & (Y[j]>=Y[ho]))
      ho = j ;
  }
  
  /* * * * * * * * * * * * * * * * */
  /*            MAIN LOOP          */
  /* * * * * * * * * * * * * * * * */   
  
  nb_it = 0 ;
  while ( (( Y[hi] > (Y[lo] + epsilon) ) & (nb_it < nb_it_max)) | (nb_it < nb_it_min) )
  {
    /* Construction of vertex M */
    for (i=0; i<N; i++)
      M[i] = 0 ; 
  
    for (j=0; j<N+1; j++)
      for (i=0; i<N; i++)
        M[i] += simplex[j][i] ;
        
    for (i=0; i<N; i++)
      M[i] = (M[i] - simplex[hi][i]) / N ;
  
    /* Construction of vertex R */
    for (i=0; i<N; i++)
      R[i] = 2*M[i] - simplex[hi][i] ;
    yR = cost_function( flow, mask_1D, R, im_bsize, bsize, nb_ref_points ) ;
    
    if ( yR  < Y[li] )
    {
      /* Construction of vertex E */     
      for (i=0; i<N; i++)
        E[i] = 2*R[i] - M[i] ;      
      yE = cost_function( flow, mask_1D, E, im_bsize, bsize, nb_ref_points ) ;
      
      if ( yE < Y[li] )
      {
        /* Replace the worst vertex by vertex E */
        for (i=0; i<N; i++)
          simplex[hi][i] = E[i] ;
        Y[hi] = yE ; 
      }
      else  /* yE > Y[li] */
      {
        /* Replace the worst vertex by vertex R */
        for (i=0; i<N; i++)
          simplex[hi][i] = R[i] ;
        Y[hi] = yR ;
      }
    }
    else    /* yR > Y[li] */
    {
      if ( yR < Y[hi] )
      {
        /* Replace the worst vertex by vertex R */
        for (i=0; i<N; i++)
          simplex[hi][i] = R[i] ;
        Y[hi] = yR ;
      }
      /* Construction of vertex C */     
      for (i=0; i<N; i++)
        C[i] = ( simplex[hi][i] + M[i] ) / 2;        
      yC = cost_function( flow, mask_1D, C , im_bsize, bsize, nb_ref_points ) ;
      
      /* Construction of vertex C2 */     
      for (i=0; i<N; i++)
        C2[i] = ( R[i] + M[i] ) / 2;        
      yC2 = cost_function( flow, mask_1D, C2 , im_bsize, bsize, nb_ref_points ) ; 
      
      if ( yC2 < yC )
      {
        /* Replace vertex C by vertex C2 */
        for (i=0; i<N; i++)
          C[i] = C2[i];
        yC = yC2 ; 
      }
      
      if ( yC < Y[hi] )
      {
        /* Replace the worst vertex by vertex C */
        for (i=0; i<N; i++)
          simplex[hi][i] = C[i] ;
        Y[hi] = yC ; 
      }
      else   /* yC > Y[hi] */
      {
        /* Shrink the simplex */
        for (j=0; j<N+1; j++)
          if (j!=lo)
            {
              for (i=0; i<N; i++)
              {
                simplex[j][i] = ( simplex[j][i] + simplex[lo][i] ) /2 ;
                Z[i] = simplex[j][i] ;
              }
              Y[j] = cost_function( flow, mask_1D, Z, im_bsize, bsize, nb_ref_points ) ;   
            }
      }
    }  
     
    /* * * * * * * * * * * * */
    /* Ordering the vertices */
    /* * * * * * * * * * * * */
  
    lo = arg_min(Y,N+1) ;
    hi = arg_max(Y,N+1) ;
    li = hi ;
    ho = lo ;
  
    for (j=0; j<N+1; j++)
    {
      if ((j!=lo) & (j!=hi) & (Y[j]<=Y[li]))
        li = j ;
      if ((j!=hi) & (j!=lo) & (Y[j]>=Y[ho]))
        ho = j ;
    }          
    
    nb_it += 1 ;  
  } /* End of while */
    
  for (i=0; i<N; i++)
     (*vect_mini)[i] = simplex[lo][i] ;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                     MEAN                                            *
 *                                                                                     *
 *    Purpose          : Computes the mean of a float vector                           *
 *                                                                                     *
 *    Arguments in     : float * vector          -  float vector                       *
 *                       long nb_el              - number of elements                  *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : average value (float)                                         *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float mean( float * vector ,   long nb_el ) 
{
   long  cpt ;
   float mean_value = 0 ;
   
   if ( (vector == NULL) | (nb_el<1) )
      fprintf(stderr," WARNING : Try to compute mean value of 0 elements.\n") ;
   else 
   {
     for (cpt = 0 ; cpt<nb_el ; cpt++)
       mean_value += vector[cpt] ;
     mean_value /=  nb_el ;
   } 
    
   return mean_value ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                     MEAN_FLOW                                       *
 *                                                                                     *
 *    Purpose          : Computes the mean of a matrix of 2D vectors                   *
 *                                                                                     *
 *    Arguments in     : flow_t  flow            -  flow                               *
 *                       mat_size_t flow_size   -  flow size (colsxlines)              *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : average value (2D float vector)                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float_vector_t mean_flow( flow_t flow , mat_size_t flow_size )
{
   register int  m,n ;
   float_vector_t mean_value  ;
   
   mean_value.x = 0 ;
   mean_value.y = 0 ;
   
   if ( (flow == NULL) | (flow_size.cols<1) | (flow_size.lines<1) )
      fprintf(stderr," WARNING : Try to compute mean value of 0 elements.\n") ;
   else 
        {
          for (m = 0 ; m<flow_size.lines ; m++)
            for (n = 0 ; n<flow_size.cols ; n++)
            {
               mean_value.x += flow[m][n].x ;
               mean_value.y += flow[m][n].y ;
            }
          mean_value.x /=  (flow_size.cols * flow_size.lines) ;
          mean_value.y /=  (flow_size.cols * flow_size.lines) ;
       } 
    
   return mean_value ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                     MEAN_MAT                                        *
 *                                                                                     *
 *    Purpose          : Computes the mean of a matrix of float values                 *
 *                                                                                     *
 *    Arguments in     : float ** matrix         -  float matrix                       *
 *                       mat_size_t mat_size     -  matrix size (colsxlines)           *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : average value (float)                                         *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float mean_mat( float ** matrix , mat_size_t mat_size )
{
   register int   m,n ;
   float          mean_value  ;
   
   mean_value = 0 ;
   
   if ( (matrix == NULL) | (mat_size.cols<1) | (mat_size.lines<1) )
      fprintf(stderr," WARNING : Try to compute mean value of 0 elements.\n") ;
   else 
   {
     for (m = 0 ; m<mat_size.lines ; m++)
       for (n = 0 ; n<mat_size.cols ; n++)
       {
          mean_value += matrix[m][n] ;
       }
     mean_value /=  (mat_size.cols * mat_size.lines) ;
   } 
        
   return mean_value ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                     VARIANCE                                        *
 *                                                                                     *
 *    Purpose          : Computes the variance of a float vector                       *
 *                                                                                     *
 *    Arguments in     : float * vector          -  vector                             *
 *                       long nb_el              -  vector  size                       *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : variance value (float)                                        *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float variance ( float * vector ,   long nb_el )
/* Computes the variance of the vector */
{
   long  cpt ;
   float       var_value  = 0.0 ;
   float       mean_value = 0.0;
   
   if (nb_el > 1)
   {  
      for (cpt=0; cpt<nb_el; cpt++)  mean_value += vector[cpt] ;
      mean_value = mean_value / nb_el ;
      for (cpt=0; cpt<nb_el; cpt++)
         var_value += ( vector[cpt] - mean_value ) * ( vector[cpt] - mean_value ) ;
      var_value = var_value / (nb_el-1) ;
   }
   return var_value ;
}
            
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                         MIN, MAX, ARGMIN, ARGMAX                                    *
 *                                                                                     *
 *    Purpose(s)       : Compute what is undesrtandable in the name...                 *
 *                                                                                     *
 *    Arguments in     : float *  vector         -  vector                             *
 *                       long nb_el              -  vector size                        *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : max, min, argmax, argmin value                                *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float min_vector( float * vector ,   long nb_el )
/* Computes the value of the element minimizing the vector */
{
   long     cpt ;
   float    min_value = 0 ;
   
   if (vector == NULL)
      fprintf(stderr," WARNING : Try computing min value on null vector.\n") ;
   else 
   {
     min_value = vector[0] ;
     for (cpt = 1 ; cpt < nb_el ; cpt++)
        if ( vector[cpt]<min_value )
           min_value = vector[cpt] ;
   }
   return min_value ;
}

float max_vector( float * vector ,   long nb_el )
/* Computes the value of element maximizing the vector */
{
   long     cpt ;
   float    max_value = 0 ;
   
   if (vector == NULL)
      fprintf(stderr," WARNING : Try computing max value on null vector.\n") ;
   else 
   {
     max_value = vector[0] ;
     for (cpt = 1 ; cpt < nb_el ; cpt++)
        if ( vector[cpt]>max_value )
           max_value = vector[cpt] ;
   }     
   return max_value ;
}

long arg_max( float * vector ,   long nb_el )
/* Computes the index of the element maximizing the vector */
{
   long     cpt ;
   long  max_ind = 0 ;
   float       max_value  ;
   
   if (vector == NULL)
      fprintf(stderr," WARNING : Try computing arg_max value on null vector.\n") ;
   else 
   {
     max_value = vector[0] ;
     for (cpt = 1 ; cpt < nb_el ; cpt++)
        if ( vector[cpt]>max_value )
        {  max_value = vector[cpt] ;  max_ind = cpt ; }
   }     
   return max_ind ;
}

long arg_min( float * vector ,   long nb_el )
/* Computes the index of the element maximizing the vector */
{
     long   cpt ;
     long   min_ind = 0 ;
   float       min_value ;
   
   if (vector == NULL)
      fprintf(stderr," WARNING : Try computing arg_min value on null vector.\n") ;
   else 
   {
     min_value = vector[0] ;
     for (cpt = 1 ; cpt < nb_el ; cpt++)
        if ( vector[cpt]<min_value )
           { min_value = vector[cpt] ; min_ind = cpt ; }
   }     
   return min_ind ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                ORDER_K_FIRST                                        *
 *                                                                                     *
 *    Purpose          : Computes a sorted vector, according to a rank n               *
 *                                                                                     *
 *    Arguments in     : long n, long nb_points  -  sorting rank, number of blocks     *
 *                                                                                     *
 *                                                                                     *
 *    Arguments in/out : block_t * vector        -  vector of blocks (mask_1D)         *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void order_k_first( block_t * vector,   long  n,   long  nb_points  )
{
   long     i, ir, j, l, mid, k ;
   block_t     temp, a ;
   
   k = n - nb_points ;
        
   l  = 0 ;
   ir = n-1 ;
   
   for (;;)
   {     
      if (ir <= l+1)
      {
         if (ir == l+1  && vector[ir].value < vector[l].value)  { SWAP(vector[l],vector[ir]) }
         temp = vector[k] ;
         return ;
      }
      else
      {
         mid = (l+ir) >> 1 ;
         SWAP( vector[mid],vector[l+1] )
         if ( vector[l].value > vector[ir].value )   { SWAP(vector[l],vector[ir])   }
         if ( vector[l+1].value > vector[ir].value ) { SWAP(vector[l+1],vector[ir]) }
         if ( vector[l].value > vector[l+1].value )  { SWAP(vector[l],vector[l+1])  }

         i = l+1 ;
         j = ir  ;
         a = vector[l+1] ;
                  
         for(;;)
         {           
           do i++ ; while (vector[i].value < a.value ) ; 
           do j-- ; while (vector[j].value > a.value ) ;
           
           if (j < i) break ;
           SWAP(vector[i],vector[j]) ;
         }
         
         vector[l+1] = vector[j] ;
         vector[j] = a ;
         
         if (j >= k) ir = j-1 ;
         if (j <= k) l  = i   ;
       }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                SELECT                                               *
 *                                                                                     *
 *    Purpose          : outputs the kth element of a vector                           *
 *                                                                                     *
 *    Arguments in     : float * vector          -  vector (not sorted at the end)     *
 *                       long n, long k          -  nb elements, chosen rank           *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : kth element (float)                                           *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float select( float * vector,   long  n,   long  k  )
{
   long  i, ir, j, l, mid ;
   float       a, temp ;
   float *     vect_copy ;
      
   vect_copy = (float *) calloc( n , sizeof(float) ) ;
   
   if (vect_copy == NULL)  
   {
      fprintf(stderr," *** ERROR while allocating memory *** \n") ;
      return 0 ;
   }
   
   for (i=0; i<n; i++)
      vect_copy[i] = vector[i] ;
        
   l  = 0 ;
   ir = n-1 ;
   
   for (;;)
   {     
      if (ir <= l+1)
      {
         if (ir == l+1  && vect_copy[ir] < vect_copy[l])  { SWAP(vect_copy[l],vect_copy[ir]) }
         temp = vect_copy[k] ;
         
         free(vect_copy) ;
         return temp ;
      }
      else
      {
         mid = (l+ir) >> 1 ;
         SWAP( vect_copy[mid],vect_copy[l+1] )
         if ( vect_copy[l] > vect_copy[ir] )   { SWAP(vect_copy[l],vect_copy[ir])   }
         if ( vect_copy[l+1] > vect_copy[ir] ) { SWAP(vect_copy[l+1],vect_copy[ir]) }
         if ( vect_copy[l] > vect_copy[l+1] )  { SWAP(vect_copy[l],vect_copy[l+1])  }

         i = l+1 ;
         j = ir  ;
         a = vect_copy[l+1] ;
                  
         for(;;)
         {           
           do i++ ; while (vect_copy[i] < a ) ; 
           do j-- ; while (vect_copy[j] > a ) ;
           
           if (j < i) break ;
           SWAP(vect_copy[i],vect_copy[j]) ;
         }
         
         vect_copy[l+1] = vect_copy[j] ;
         vect_copy[j] = a ;
         
         if (j >= k) ir = j-1 ;
         if (j <= k) l  = i   ;
       }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                   MEDIAN                                            *
 *                                                                                     *
 *    Purpose          : outputs the median of a vector                                *
 *                                                                                     *
 *    Arguments in     : float * vector          -  vector                             *
 *                       long n                  -  nb elements                        *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : median value (float)                                          *
 *                                                                                     *
 *    Description      : uses 'select'                                                 *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float median( float * vector,   long n ) 
/* Computes the median of the input vector (size n) */
{
  if ((n==0) | (vector==NULL))
     return(0) ;
  else if ( n%2 != 0 )  
            return select(vector,n,(long) (n-1)/2) ;
       else             
            return (  ( select(vector,n,(long) n/2-1) + select(vector,n,(long) n/2) ) / 2 ) ;  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                   FLOAT2INT                                      *
 *                                                                                     *
 *    Purpose          : outputs the round value of a float                            *
 *                                                                                     *
 *    Arguments in     : float a                 -  input float                        *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : round  value (int)                                            *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
int float2int( float a ) 
{
  if ( a > (floor(a) + 0.5) )
    return( (int) floor(a)+1 ) ;
  else
    return( (int) floor(a) ) ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                        MASK_ALLOC                                                   *
 *                                                                                     *
 *    Purpose          : Allocation routine fot type mask_t                            *
 *                                                                                     *
 *    Arguments in     : mat_size_t              -  mask size (colsxlines)             *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : Alocated mask                                                 *
 *                                                                                     *
 *    Description      : If a problem occurs, the program stops                        *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
mask_t mask_alloc(mat_size_t mask_size) 
{
   long cpt ;
   mask_t mask ;

   mask = (mask_t) calloc ( mask_size.lines , sizeof(float*) ) ;
   
   if (mask != NULL)
     for (cpt=0; cpt<mask_size.lines; cpt++)
     {
         mask[cpt] = (float *) calloc( (long) mask_size.cols ,  sizeof(float) ) ;
         if (mask[cpt]==NULL)
         {
           fprintf(stdout," \n *** ERROR while trying to allocate mask memory *** \n ") ;
           exit(1) ;
         }
     }
   else 
   {
     fprintf(stdout," \n *** ERROR while trying to allocate mask memory *** \n ") ;
     exit(1) ;
   }
     
   return mask ;
} 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                        FLOW_ALLOC                                                   *
 *                                                                                     *
 *    Purpose          : Allocation routine fot type flow_t                            *
 *                                                                                     *
 *    Arguments in     : flow_size_t             -  flow size (colsxlines)             *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : Allocated flow ( matrix of 2D vectors )                       *
 *                                                                                     *
 *    Description      : If a problem occurs, the program stops                        *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
flow_t flow_alloc(mat_size_t flow_size) 
{
   long cpt ;
   flow_t flow ;

   flow = (flow_t) calloc ( flow_size.lines , sizeof(float_vector_t*) ) ;
   
   if (flow != NULL)
     for (cpt=0; cpt<flow_size.lines; cpt++)
     {
         flow[cpt] = (float_vector_t *) calloc( (long) flow_size.cols ,  sizeof(float_vector_t) ) ;
         if (flow[cpt] == NULL)
         {
           fprintf(stdout," \n *** ERROR while trying to allocate motion vectors flow memory *** \n ") ;
           exit(1) ;
         }
     }
   else 
   {
     fprintf(stdout," \n *** ERROR while trying to allocate motion vectors flow memory *** \n ") ;
     exit(1) ;
   }
   
   return flow ;
} 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                        FLOW_FREE                                                    *
 *                                                                                     *
 *    Purpose          : Free routine fot type flow_t                                  *
 *                                                                                     *
 *    Arguments in     : flow_t flow             -  flow to free                       *
 *                       flow_size_t             -  flow size (colsxlines)             *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void flow_free (flow_t flow, mat_size_t flow_size)
{
     long cpt ;
   
   for (cpt=0; cpt<flow_size.lines; cpt++)
       free(flow[cpt])   ;
     
   free(flow);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                        MASK_FREE                                                    *
 *                                                                                     *
 *    Purpose          : Free routine fot type mask_t                                  *
 *                                                                                     *
 *    Arguments in     : mask_t mask             -  mask to free                       *
 *                       mask_size_t             -  mask size (colsxlines)             *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void mask_free (mask_t mask, mat_size_t mask_size)
{
   long cpt ;
   
   for (cpt=0; cpt<mask_size.lines; cpt++)
       free(mask[cpt])   ;
     
   free(mask );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                          MOTION_CHRONO_ALLOC                                        *
 *                                                                                     *
 *    Purpose          : Allocation routine for type motion_chrono_t (structure)       *
 *                                                                                     *
 *    Arguments in     : int clusters            -  number of motion clusters          *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : motion_chrono_t         - allocated chronogram                *
 *                                                                                     *
 *    Description      : nb_clusters field is also set to the input value              *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 

motion_chrono_t  motion_chrono_alloc ( int  clusters )
{
   motion_chrono_t  new_chrono ;   
   new_chrono.nb_clusters = clusters ;
   
   new_chrono.start_frame = (int*) calloc( clusters, sizeof(int) ) ;
   if ( new_chrono.start_frame == NULL )
   {
     fprintf(stderr," \n\n *** ERROR *** Memory allocation problem \n\n ") ;
     exit(1) ;
   }
   
   new_chrono.stop_frame = (int*) calloc( clusters, sizeof(int) ) ;
   if ( new_chrono.stop_frame == NULL )
   {
     fprintf(stderr," \n\n *** ERROR *** Memory allocation problem \n\n ") ;
     exit(1) ;
   }
   
   new_chrono.avg_value = (float*) calloc( clusters, sizeof(float) ) ;
   if ( new_chrono.avg_value == NULL )
   {
     fprintf(stderr," \n\n *** ERROR *** Memory allocation problem \n\n ") ;
     exit(1) ;
   }
   
   new_chrono.std_dev = (float*) calloc( clusters, sizeof(float) ) ;
   if ( new_chrono.std_dev == NULL )
   {
     fprintf(stderr," \n\n *** ERROR *** Memory allocation problem \n\n ") ;
     exit(1) ;
   }
   
   return new_chrono ;
}
      
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                          LOW_LEVEL_DESC_ALLOC                                       *
 *                                                                                     *
 *    Purpose          : Allocation routine for type low_level_desc_t                  *
 *                                                                                     *
 *    Arguments in     : int frames              -  number of frames                   *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : low_level_desc_t         - allocated low-level descriptor     *
 *                                                                                     *
 *    Description      : The program stops if an error occurs                          *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */  
   
low_level_desc_t  low_level_desc_alloc( int frames ) 
{
  low_level_desc_t   low_desc ;
  int cpt ;
  
  low_desc.nb_frames = frames ;
  for (cpt=0; cpt<NB_MOTION_TYPES; cpt++)
  {
    low_desc.motion[cpt]    = (float *) calloc ( frames, sizeof( float ) ) ;
    if ( low_desc.motion[cpt] == NULL )
    {
      fprintf(stderr," \n\n *** ERROR *** Memory allocation problem \n\n ") ;
      exit(1) ;
    }
  }
   
  return low_desc ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                          MOTION_CHRONO_FREE                                         *
 *                                                                                     *
 *    Purpose          : Free routine for type motion_chrono_t                         *
 *                                                                                     *
 *    Arguments in     : motion_chrono_t         -  data to free                       *
 *                                                                                     *
 *    Arguments in/out :                                                               *                       
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      : nb_clusters field is set to 0                                 *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void motion_chrono_free ( motion_chrono_t chrono ) 
{  
  free( chrono.start_frame ) ;
  free( chrono.stop_frame  ) ;
  free( chrono.avg_value   ) ;
  free( chrono.std_dev     ) ;
  chrono.nb_clusters = 0 ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                    CLEAN                                            *
 *                                                                                     *
 *    Purpose          : Outputs a time-median filtered buffer                         *
 *                                                                                     *
 *    Arguments in     : float * vector          - buffer to filter                    *
 *                       long n                  - number of elements in buffer        *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : float *                 - filtered vbuffer                    *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
float * clean( float * vector,   long n ) 
{
   long                  frame, mini, maxi ;
   float *        filt_vect ;
   
   filt_vect = (float *) calloc( n , sizeof(float) ) ;
   if (filt_vect == NULL)
     {
       fprintf(stderr," \n*** ERROR, Memory allocation problem : Not enough memory for results_buffer *** \n\n") ;
       exit(1) ;
     }
   
   for (frame=0; frame<n; frame++)
     {
        mini = (long) MAX((float)frame-(float)MEDIAN_WIN_INC,0.0)   ;
        maxi = (long) MIN((float)frame+(float)MEDIAN_WIN_INC,(float)n-1) ;
        
        filt_vect[frame] = (float) median( &vector[mini],maxi-mini+1 ) ;
     }
   
   return filt_vect ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                   LONG_TERM_ANALYSIS                                *
 *                                                                                     *
 *    Purpose          : extracts higher level descriptor from instantaneous results   *
 *                                                                                     *
 *    Arguments in     : float * inst_features_buffer    - instantaneous results       *
 *                       int  nb_frames                  - number of frames            *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    : med_low_level_desc_t * med_low_desc - Descriptor extracted    *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      : see 'camera.h' to get descriptor definition                   *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void long_term_analysis ( med_low_level_desc_t * med_low_desc, float * inst_features_buffer, int nb_frames ) 
{
  float *      pan_left,   *pan_right ;
  float *      tilt_down,  *tilt_up  ;
  float *      track_left, *track_right ;
  float *      boom_down,  *boom_up ;
  float *      zoom_out,   *zoom_in ;
  float *      roll_left,  *roll_right ;
  float *      fixed, *unknown, *almost_fixed ;
  float *      aux_buffer ;
  int          is_unknown_segment ;  
  int          k,l,m, motion_type;
  int          a,b,c,d ;
     
  pan_left     = (float*) calloc ( nb_frames , sizeof(float) )  ;
  if ( pan_left == NULL  )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  pan_right    = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( pan_right == NULL ) 
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  tilt_down    = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( tilt_down == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  } 
  
  tilt_up      = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( tilt_up == NULL  )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  } 
  
  track_left   = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( track_left == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  track_right  = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( track_right == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  zoom_out     = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( zoom_out == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  zoom_in      = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( zoom_in == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  } 
  
  boom_down    = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( boom_down == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  boom_up      = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( boom_up == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  roll_left    = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( roll_left == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
   
  roll_right   = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( roll_right == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  fixed   = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( fixed == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  unknown   = (float*) calloc ( nb_frames , sizeof(float) ) ;
  if ( unknown == NULL )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  almost_fixed     = (float*) calloc ( nb_frames , sizeof(float) )  ;
  if ( almost_fixed == NULL  )
  {
    fprintf(stderr," \n *** ERROR ***   Memory allocation problem \n \n ") ;
    exit(1) ;
  }
  
  for (k=0; k<nb_frames; k++)
  {
    pan_left[k]     = 0 ;
    pan_right[k]    = 0 ; 
    tilt_down[k]    = 0 ;
    tilt_up[k]      = 0 ;
    track_left[k]   = 0 ; 
    track_right[k]  = 0 ;
    zoom_out[k]     = 0 ;
    zoom_in[k]      = 0 ; 
    boom_down[k]    = 0 ;
    boom_up[k]      = 0 ;
    roll_left[k]    = 0 ; 
    roll_right[k]   = 0 ;
    fixed[k]        = 0 ;
    unknown[k]      = 1 ;
    almost_fixed[k]  = 1 ;
  }
  
  for (k=0; k<nb_frames; k++)
  {
    aux_buffer  = &(inst_features_buffer[0]) ;
    if ( aux_buffer[k] < 0 )      
      track_left[k] = - aux_buffer[k] ;
    else
      track_right[k] = aux_buffer[k] ;
      
    if ( ABS(aux_buffer[k]) > 5*MOTION_SENSITIVITY )
      almost_fixed[k] = 0 ;
    
    aux_buffer  = &(inst_features_buffer[nb_frames]) ;
    if ( aux_buffer[k] < 0 )
      boom_down[k] = - aux_buffer[k] ;
    else
      boom_up[k] = aux_buffer[k] ;
      
    if ( ABS(aux_buffer[k]) > 5*MOTION_SENSITIVITY )
      almost_fixed[k] = 0 ;
      
    aux_buffer  = &(inst_features_buffer[2*nb_frames]) ;
    if ( aux_buffer[k] < 0 )
      tilt_down[k] = - aux_buffer[k] ;
    else
      tilt_up[k] = aux_buffer[k] ;
      
    if ( ABS(aux_buffer[k]) > 5*MOTION_SENSITIVITY )
      almost_fixed[k] = 0 ;
      
    aux_buffer  = &(inst_features_buffer[3*nb_frames]) ;
    if ( aux_buffer[k] < 0 )
      pan_left[k] = - aux_buffer[k] ;
    else
      pan_right[k] = aux_buffer[k] ;
      
    if ( ABS(aux_buffer[k]) > 5*MOTION_SENSITIVITY )
      almost_fixed[k] = 0 ;
 
    aux_buffer  = &(inst_features_buffer[4*nb_frames]) ;
    if ( aux_buffer[k] < 0 )
      roll_left[k] = - aux_buffer[k] ;
    else
      roll_right[k] = aux_buffer[k] ;
    
    if ( ABS(aux_buffer[k]) > ROLL_THRESHOLD / 10)
      almost_fixed[k] = 0 ;
      
    aux_buffer  = &(inst_features_buffer[5*nb_frames]) ;
    if ( aux_buffer[k] < 0 )
      zoom_out[k] = - aux_buffer[k] ;
    else
      zoom_in[k] = aux_buffer[k] ;
    
    if ( ABS(aux_buffer[k]) > 5*MOTION_SENSITIVITY )
      almost_fixed[k] = 0 ;
    
    aux_buffer  = &(inst_features_buffer[9*nb_frames]) ;
    if ( aux_buffer[k] == 1 )
      fixed[k] = 1 ;            
  }  
    
  (*med_low_desc)[TRACK_LEFT]  = find_motion_clusters ( track_left   , nb_frames, DURATION_THRESHOLD, TRACK_THRESHOLD , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[TRACK_RIGHT] = find_motion_clusters ( track_right  , nb_frames, DURATION_THRESHOLD, TRACK_THRESHOLD , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[BOOM_DOWN]   = find_motion_clusters ( boom_down    , nb_frames, DURATION_THRESHOLD, BOOM_THRESHOLD  , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[BOOM_UP]     = find_motion_clusters ( boom_up      , nb_frames, DURATION_THRESHOLD, BOOM_THRESHOLD  , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[TILT_DOWN]   = find_motion_clusters ( tilt_down    , nb_frames, DURATION_THRESHOLD, TILT_THRESHOLD  , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[TILT_UP]     = find_motion_clusters ( tilt_up      , nb_frames, DURATION_THRESHOLD, TILT_THRESHOLD  , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[PAN_LEFT]    = find_motion_clusters ( pan_left     , nb_frames, DURATION_THRESHOLD, PAN_THRESHOLD   , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[PAN_RIGHT]   = find_motion_clusters ( pan_right    , nb_frames, DURATION_THRESHOLD, PAN_THRESHOLD   , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[ROLL_LEFT]   = find_motion_clusters ( roll_left    , nb_frames, DURATION_THRESHOLD, ROLL_THRESHOLD  , ROLL_THRESHOLD / 10  ) ; 
  (*med_low_desc)[ROLL_RIGHT]  = find_motion_clusters ( roll_right   , nb_frames, DURATION_THRESHOLD, ROLL_THRESHOLD  , ROLL_THRESHOLD / 10  ) ; 
  (*med_low_desc)[ZOOM_OUT]    = find_motion_clusters ( zoom_out     , nb_frames, DURATION_THRESHOLD, ZOOM_THRESHOLD  , MOTION_SENSITIVITY  ) ; 
  (*med_low_desc)[ZOOM_IN]     = find_motion_clusters ( zoom_in      , nb_frames, DURATION_THRESHOLD, ZOOM_THRESHOLD  , MOTION_SENSITIVITY  ) ; 
  
  /* If a frame is not indexed yet, it is considered as unknown */  
  for (motion_type = 0 ; motion_type < NB_MOTION_TYPES-2 ; motion_type++)
    for (k=0; k<(*med_low_desc)[motion_type].nb_clusters; k++)
      for (l=(*med_low_desc)[motion_type].start_frame[k]; l<=(*med_low_desc)[motion_type].stop_frame[k]; l++)
        unknown[l] = 0 ;

  for (k=0; k<nb_frames; k++)
    if (( unknown[k] == 1 ) && ( almost_fixed[k] == 1 ) )
      fixed[k] = 1 ;
  
  (*med_low_desc)[FIXED]       = find_motion_clusters ( fixed        , nb_frames, FIX_DURATION_THRESHOLD , 0 , 0                   ) ;

  /* If a frame is not indexed yet, it is considered as unknown */
  for  (k=0; k<nb_frames; k++)
    unknown[k] = 1 ;
  
  for (motion_type = 0 ; motion_type < NB_MOTION_TYPES-1 ; motion_type++)
    for (k=0; k<(*med_low_desc)[motion_type].nb_clusters; k++)
      for (l=(*med_low_desc)[motion_type].start_frame[k]; l<=(*med_low_desc)[motion_type].stop_frame[k]; l++)
        unknown[l] = 0 ;
        
  (*med_low_desc)[UNKNOWN]     = find_motion_clusters ( unknown      , nb_frames, 0, 0               , 0                   ) ;

  /* Unknown frames between consistent motion segments are removed and replaced by the said motion */
  for (motion_type = 0 ; motion_type < NB_MOTION_TYPES-1 ; motion_type++)
    {
      l=0 ;
      
      for (k=0; k<(*med_low_desc)[motion_type].nb_clusters-1; k++)
        {
          is_unknown_segment = -1 ;
          for (m=0;m<(*med_low_desc)[UNKNOWN].nb_clusters; m++)
            {
              
              if ( ((*med_low_desc)[UNKNOWN].start_frame[m] == (*med_low_desc)[motion_type].stop_frame[l]+1) &&
                 ((*med_low_desc)[UNKNOWN].stop_frame[m] == (*med_low_desc)[motion_type].start_frame[l+1]-1) &&
                 ((*med_low_desc)[UNKNOWN].stop_frame[m] -(*med_low_desc)[UNKNOWN].start_frame[m] < EXTRAPOL_LIMIT ))
                is_unknown_segment = m ;
            }
          
          
          if ( is_unknown_segment != -1 )
            {            
              a = (*med_low_desc)[motion_type].start_frame[l];
              b = (*med_low_desc)[motion_type].stop_frame[l];
              c = (*med_low_desc)[motion_type].start_frame[l+1];
              d = (*med_low_desc)[motion_type].stop_frame[l+1]  ;
              
              (*med_low_desc)[motion_type].stop_frame[l] = (*med_low_desc)[motion_type].stop_frame[l+1] ;
              
              (*med_low_desc)[motion_type].avg_value[l] = ( (b-a) * (*med_low_desc)[motion_type].avg_value[l] + (d-c) * (*med_low_desc)[motion_type].avg_value[l+1] ) / ((b-a)+(d-c)) ;
              (*med_low_desc)[motion_type].std_dev[l] = ( (b-a) * (*med_low_desc)[motion_type].std_dev[l] + (d-c) * (*med_low_desc)[motion_type].std_dev[l+1] ) / ((b-a)+(d-c)) ;
              
              for (m=l+1; m<(*med_low_desc)[motion_type].nb_clusters; m++)
                {
                  (*med_low_desc)[motion_type].stop_frame[m] = (*med_low_desc)[motion_type].stop_frame[m+1] ;
                  (*med_low_desc)[motion_type].start_frame[m] = (*med_low_desc)[motion_type].start_frame[m+1] ;
                  (*med_low_desc)[motion_type].avg_value[m] = (*med_low_desc)[motion_type].avg_value[m+1] ;
                  (*med_low_desc)[motion_type].std_dev[m] = (*med_low_desc)[motion_type].std_dev[m+1] ;
                  
                }
              
              (*med_low_desc)[motion_type].nb_clusters -- ;            
            }
          l++ ;          
        }
    }
  
      /* If a frame is not indexed yet, it is considered as unknown */

  for  (k=0; k<nb_frames; k++)
    unknown[k] = 1 ;
  
  for (motion_type = 0 ; motion_type < NB_MOTION_TYPES ; motion_type++)
    for (k=0; k<(*med_low_desc)[motion_type].nb_clusters; k++)
      for (l=(*med_low_desc)[motion_type].start_frame[k]; l<=(*med_low_desc)[motion_type].stop_frame[k]; l++)
        unknown[l] = 0 ;
  
  (*med_low_desc)[UNKNOWN]     = find_motion_clusters ( unknown      , nb_frames, 0, 0               , 0                   ) ;
  
  free(pan_left) ;
  free(pan_right) ;
  free(tilt_down) ;
  free(tilt_up) ;
  free(track_left) ;
  free(track_right) ;
  free(boom_down) ;
  free(boom_up) ;
  free(zoom_out) ;
  free(zoom_in) ;
  free(roll_left) ;
  free(roll_right) ;
  free(fixed) ;
  free(unknown) ;
  free(almost_fixed) ;  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                   FIND_MOTION_CLUSTERS                              *
 *                                                                                     *
 *    Purpose          : Finds consistent motion clusters in a instantaneous buffer    *
 *                                                                                     *
 *    Arguments in     : float * features_buffer         - 1D features buffer          *
 *                       int  min_consistent_frames      - minimum duration            *
 *                       float min_mean_value            - minimum mean value for      *
 *                                                         a consistent cluster        *
 *                       float min_motion_value          - minimum value that can be   *
 *                                                         considered as motion        *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : motion_chrono_t    - representation of the motion clusters    *
 *                                                                                     *
 *    Description      : see 'camera.h' to get descriptor definition                   *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
motion_chrono_t find_motion_clusters ( float *  features_buffer, int nb_frames,  int min_consistent_frames, float min_mean_value, float min_motion_value) 
{
  float *      chrono ;
  motion_chrono_t    res_chrono ;
  int          start_motion = 0, end_motion, motion_duration ;
  float     last_chrono ;
  int       clusters = 0 ;
  int          k, l ;
  float                 avg ;
      
  chrono = (float *) calloc ( nb_frames, sizeof(float ) ) ;
  
  /* FIRST LOOP */
  for (k=0; k<nb_frames; k++ ) 
    if ( features_buffer[k] > min_motion_value )
       chrono[k] = 1 ;
    else
       chrono[k] = 0 ;
       
  /* SECOND LOOP */     
  last_chrono = 0 ;
  for (k=0; k<nb_frames; k++)
  {
    if ( ( chrono[k] == 1 ) & ( last_chrono == 0 ) )
      start_motion = k ;
      
    if ( (( chrono[k] == 0 ) | ( k == nb_frames-1 )) & ( last_chrono == 1 ) )
    {
      if ( k == nb_frames-1 )
        end_motion = k ;
      else
        end_motion = k-1 ;
            
      motion_duration = end_motion - start_motion + 1 ;    
      avg = mean     ( &(features_buffer[start_motion]) , motion_duration ) ;
      
      if (( avg > min_mean_value ) & ( motion_duration > min_consistent_frames ))
      {
       /* A new consistent motion cluster has been found */       
        clusters ++ ;
        for (l=start_motion; l<end_motion+1; l++)
          chrono[l] = avg ;
      }
      else
        for (l=start_motion; l<end_motion+1; l++)
          chrono[l] = 0 ;
    }
    
    last_chrono = chrono[k] ;
  }    
  
  /* THIRD LOOP */
  res_chrono = motion_chrono_alloc ( clusters ) ;
  
  last_chrono = 0 ;
  clusters    = 0 ;

  for (k=0; k<nb_frames; k++)
  {
    if ( ( chrono[k] != 0 ) & ( last_chrono == 0 ) )
      start_motion = k ;
      
    if ( (( chrono[k] == 0 ) | ( k == nb_frames-1 )) & ( last_chrono != 0 ) )
    {
      /* A new consistent motion cluster has been found */ 
      res_chrono.start_frame[clusters] = start_motion ;

      if ( k == nb_frames-1 )
        res_chrono.stop_frame[clusters] = k ;
      else
        res_chrono.stop_frame[clusters] = k-1 ;
      
      res_chrono.avg_value[clusters]  = (float) 0.5 * float2int ( 2 * mean     ( &(features_buffer[res_chrono.start_frame[clusters]]) , res_chrono.stop_frame[clusters] - res_chrono.start_frame[clusters] +1) ) ; /* 0.5 pix unit */
      res_chrono.std_dev[clusters]    =(float) sqrt( variance ( &(features_buffer[res_chrono.start_frame[clusters]]) , res_chrono.stop_frame[clusters] - res_chrono.start_frame[clusters] +1 ) )  ;
      
      clusters ++ ;
    }
    
    last_chrono = chrono[k] ;
  }    
  
  free(chrono) ;
  
  return(res_chrono) ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                              MED_LOW_TO_MED_HIGH                                    *
 *                                                                                     *
 *    Purpose          : Descriptor extraction medium-low level -> medium-high level   *
 *                                                                                     *
 *    Arguments in     : med_low_level_desc_t med_low_desc   - MLL descriptor          *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    : med_high_level_desc_t * med_high_desc - MHL descriptor        *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void med_low_to_med_high_desc ( med_low_level_desc_t med_low_desc , motion_histogram_t * motion_histogram, motion_histogram_t * motion_speeds, int nb_frames ) 
{
  short     motion_type, cpt ;
  int       sum_moving_frames = 0 ;
  float     sum_speeds = 0 ;
  
  for ( motion_type = 0 ; motion_type < NB_MOTION_TYPES-1; motion_type ++ )
  {
    sum_moving_frames = 0 ;
    sum_speeds = 0 ;
    
    for (cpt = 0 ; cpt < med_low_desc[motion_type].nb_clusters; cpt++)
    {
      sum_moving_frames += med_low_desc[motion_type].stop_frame[cpt] - med_low_desc[motion_type].start_frame[cpt] + 1 ;
      sum_speeds +=  med_low_desc[motion_type].avg_value[cpt] * (med_low_desc[motion_type].stop_frame[cpt] - med_low_desc[motion_type].start_frame[cpt] + 1 ) ;
    }

    (*motion_histogram)[motion_type] = (float) ( 100 * sum_moving_frames / nb_frames ) ;

    if ( sum_moving_frames != 0 )
      (*motion_speeds)[motion_type]    = (float) ( 2 * sum_speeds / sum_moving_frames ) ;
    else
      (*motion_speeds)[motion_type]    = (float) 0 ;
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                              MERGE_CLUSTERS                                         *
 *                                                                                     *
 *    Purpose          : Merging  the clusters in the input chrono according to some   *
 *                       merging rules (for the moment very simple)                    *
 *                                                                                     *
 *    Arguments in     :                                                               *
 *                                                                                     *
 *    Arguments in/out : motion_chrono_t * old_chrono        - chronogram              *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
 
void merge_clusters ( motion_chrono_t * old_chrono ) 
{
   const int   MIN_SEP_FRAMES = FIX_DURATION_THRESHOLD ;
   int         cpt, k ;
   int         clusters = 0 ;
   motion_chrono_t  new_chrono ;
   
   clusters = (*old_chrono).nb_clusters ;
   
   for (cpt=0; cpt<clusters-1; cpt++)
   {
     if (  ( (*old_chrono).start_frame[cpt+1] - (*old_chrono).stop_frame[cpt] ) < MIN_SEP_FRAMES )
     {
       (*old_chrono).stop_frame[cpt] = (*old_chrono).stop_frame[cpt+1] ;
       (*old_chrono).avg_value[cpt]  = ( (*old_chrono).avg_value[cpt] + (*old_chrono).avg_value[cpt] ) /2 ;
       (*old_chrono).std_dev[cpt]    = ( (*old_chrono).std_dev[cpt] + (*old_chrono).std_dev[cpt] ) /2 ;
       
       for (k=cpt+1; k<clusters-1; k++)
       {
         (*old_chrono).stop_frame[k]  = (*old_chrono).stop_frame[k+1] ;
         (*old_chrono).start_frame[k] = (*old_chrono).start_frame[k+1] ;
         (*old_chrono).avg_value[k]   = (*old_chrono).avg_value[k+1] ;
         (*old_chrono).std_dev[k]     = (*old_chrono).std_dev[k+1] ;
       }
       
       clusters -- ;
       cpt-- ;
     }
   }
   
   new_chrono = motion_chrono_alloc ( clusters ) ;
   
   for (cpt=0; cpt<clusters; cpt ++) 
   {
     new_chrono.start_frame[cpt] = (*old_chrono).start_frame[cpt] ;
     new_chrono.stop_frame[cpt]  = (*old_chrono).stop_frame[cpt] ;
     new_chrono.avg_value[cpt]   = (*old_chrono).avg_value[cpt] ;
     new_chrono.std_dev[cpt]     = (*old_chrono).std_dev[cpt] ;
   }
   
   motion_chrono_free( (*old_chrono) ) ;
   
   (*old_chrono).start_frame = new_chrono.start_frame ;
   (*old_chrono).stop_frame  = new_chrono.stop_frame  ;
   (*old_chrono).avg_value   = new_chrono.avg_value   ;
   (*old_chrono).std_dev     = new_chrono.std_dev     ;
   (*old_chrono).nb_clusters = new_chrono.nb_clusters ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                READ_FLOW_FROM_BUFFER                                *
 *                                                                                     *
 *    Purpose          : Reads a motion vector flow from a buffer                      *
 *                                                                                     *
 *    Arguments in     : int  * bma_buffer       - block matching results buffer       *
 *                       mat_size_t im_bsize     - image size ( in blocks )            *
 *                       long im_number          - frame number                        *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : flow_t                  - expected flow                       *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
flow_t read_flow_from_buffer( int * bma_buffer, mat_size_t im_bsize,int im_number )
{
   int      m, n, i, im_start; 
   int *    im_buffer  ;
   flow_t   flow ;    
     
     im_start = (int) 2 * (im_bsize.cols) * (im_bsize.lines) * im_number ;
     im_buffer = &(bma_buffer[im_start]) ;
     
     /* Memory allocation for 2-dimensional array flow */
     flow = flow_alloc(im_bsize) ;
     
     /* Filling the motion vectors flow */     
     for (m=0; m<im_bsize.lines; m++)
       for (n=0; n<im_bsize.cols; n++)
       {
         i = 2*m*im_bsize.cols + 2*n ;
         flow[m][n].x = (float) im_buffer[i]   ;
         flow[m][n].y = (float) im_buffer[i+1] ;
       }
              
     return(flow) ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                    NULL_MOTION                                      *
 *                                                                                     *
 *    Purpose          : Outputs a boolean : TRUE for null motion ( fixed camera )     *
 *                                           FALSE otherwise                           *
 *                                                                                     *
 *    Arguments in     : flow_t flow             - motion flow field                   *
 *                       mat_size_t im_bsize     - image size ( in blocks )            *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : int                     - boolean                             *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
int    null_motion ( flow_t flow , mat_size_t flow_size )
{
   int    m,n ;
   float    sum_zeros  = 0 ;
      
   for (m=0; m<flow_size.lines; m++)
     for (n=0; n<flow_size.cols; n++)
       if ( (ABS(flow[m][n].x) < MOTION_SENSITIVITY) & (ABS(flow[m][n].y)< MOTION_SENSITIVITY) )
          sum_zeros += 1 ;

   if ( (float) sum_zeros / (float) (flow_size.lines * flow_size.cols ) > NULL_VECT_PERCENTAGE )      
      return 1 ;
   else
      return 0 ;
}

int   fine_null_motion ( flow_t flow ,  block_t * mask_1D,  int nb_ref_points )
{
  int   k ;
  float         sum_zeros  = 0 ;
      
  for (k=0; k<nb_ref_points; k++)
      if ( (ABS(flow[mask_1D[k].m][mask_1D[k].n].x) < MOTION_SENSITIVITY) & (ABS(flow[mask_1D[k].m][mask_1D[k].n].y)< MOTION_SENSITIVITY) )
        sum_zeros += 1 ;

  if ( (float) sum_zeros / (float) (nb_ref_points) > NULL_VECT_PERCENTAGE )      
    return 1 ;
  else
    return 0 ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                 CHOOSE_REF_POINTS                                   *
 *                                                                                     *
 *    Purpose          : Outputs a list of blocks considered as appropriate            *
 *                       to compute camera motion features                             *
 *                                                                                     *
 *    Arguments in     : flow_t flow             - motion flow field                   *
 *                       mat_size_t flow_size    - image size ( in blocks )            *
 *                       long nb_ref_points      - number of reference points          *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : block_t *               - list of nb_ref_points blocks        *
 *                                                                                     *
 *    Description      : For the moment, the best blocks are chosen according to       *
 *                       a linear combination of the confidence mask, the distance     *
 *                       to the center (gaussian) and the neibourghood condidence      *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
           
block_t *   choose_ref_points( flow_t flow, mat_size_t flow_size,int  nb_ref_points )
{
  int          cpt, wm, wn ;
  register int m, n ;
  mask_t       mask ;
  block_t *    mask_1D, * short_mask_1D ;
  float        centre_x, centre_y ;

  mask = construct_mask( flow, flow_size ) ;
  
  mask_1D       = (block_t *) calloc(  flow_size.lines * flow_size.cols , sizeof(block_t) ) ;
  if (mask_1D == NULL)
  {
    fprintf(stderr," *** ERROR, Memory allocation problem *** \n") ;
    exit(1) ;
  }
  
  short_mask_1D = (block_t *) calloc ( nb_ref_points,                     sizeof(block_t) ) ;
  if (short_mask_1D == NULL)
  {
    fprintf(stderr," *** ERROR, Memory allocation problem *** \n") ;
    exit(1) ;
  }
  
  centre_x = (float) (flow_size.cols-1)  / 2 ;
  centre_y = (float) (flow_size.lines-1) / 2 ;
  
  for (m=0; m<flow_size.lines; m++)
    for (n=0; n<flow_size.cols; n++)
    {
      mask_1D[m*flow_size.cols + n].value = 10 *  mask[m][n] ;
      
      for (wm = m-1 ; wm <= m+1 ; wm++)
        for (wn = n-1 ; wn <= n+1 ; wn++)
          if ( !((wm == m)&(wn == n)) & (wm >= 0) & (wn >= 0) & (wm < flow_size.lines) & (wn < flow_size.cols)  )
             mask_1D[m*flow_size.cols + n].value +=  mask[wm][wn];
          else 
             mask_1D[m*flow_size.cols + n].value +=  (float) 0.2 ;
                  
      mask_1D[m*flow_size.cols + n].m = m ;
      mask_1D[m*flow_size.cols + n].n = n ;
      mask_1D[m*flow_size.cols + n].value +=  10*(1- (float) exp( - (SQR(m-centre_y)+SQR(n-centre_x))/(SQR(centre_x) + SQR(centre_y))) ); 
    }
 
 order_k_first( mask_1D , flow_size.lines * flow_size.cols , nb_ref_points ) ;
    
 for (cpt = 0 ; cpt< nb_ref_points ; cpt++)
 {
    short_mask_1D[cpt]       = mask_1D[ flow_size.lines * flow_size.cols - (cpt+1) ] ;
    short_mask_1D[cpt].value = mask[short_mask_1D[cpt].m][short_mask_1D[cpt].n] ;
 } 
 
 mask_free( mask,       flow_size) ;
 free     ( mask_1D ) ;
 
 return short_mask_1D ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                 CONSTRUCT_MASK                                      *
 *                                                                                     *
 *    Purpose          : Outputs a confidence mask according to neibourghood           *
 *                       analysis                                                      *
 *                                                                                     *
 *    Arguments in     : flow_t flow             - motion flow field                   *
 *                       mat_size_t flow_size    - image size ( in blocks )            *
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    : mask_t                  - confidence mask                     *
 *                                                                                     *
 *    Description      : Enhancement iterations are performed                          *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
mask_t construct_mask( flow_t flow, mat_size_t flow_size ) 
{
   int          nb_it ;   /* block counters and enhancement counter */
   int          wm, wn ;  /* Neibourghood window counters */
   register int m, n;
   const int    WIN_INC  = 2 ;                               /* window size (neibourhood) */  
   float        global_variance, global_mean ,  win_norm ;   /* Statistical data */
   mask_t       variance_mask, prev_mask, aux_mask ;
   
   variance_mask = mask_alloc(flow_size) ;
   prev_mask     = mask_alloc( flow_size )  ;
  
   for (m=0; m<flow_size.lines; m++)
     for (n=0; n<flow_size.cols; n++)
         prev_mask[m][n] = 1 ;
            
   for (nb_it=0; nb_it < NB_MASK_ITERATIONS; nb_it++)
   { 
     for (m=0; m<flow_size.lines; m++)
       for (n=0; n<flow_size.cols; n++)
       {
         variance_mask[m][n] = 0 ;
         win_norm =   0 ;
        
         for (wm = m-WIN_INC ; wm <= m+WIN_INC ; wm++)
           for (wn = n-WIN_INC ; wn <= n+WIN_INC ; wn++)
             if ( !((wm == m)&(wn == n)) & (wm >= 0) & (wn >= 0) & (wm < flow_size.lines) & (wn < flow_size.cols)  )
             {
               variance_mask[m][n] += prev_mask[wm][wn]*((float)sqrt(( SQR( flow[wm][wn].x - flow[m][n].x )) + SQR( flow[wm][wn].y - flow[m][n].y ) )) / (1+  (float)sqrt(SQR( flow[m][n].x) +SQR(flow[m][n].y )) + ((float)sqrt(SQR(flow[wm][wn].x)) + SQR(flow[wm][wn].y))  ) ;
               win_norm += prev_mask[wm][wn] ;
             }
         
         variance_mask[m][n] =  variance_mask[m][n] / (1+win_norm) ;     
       } 
            
     global_mean = mean_mat( variance_mask , flow_size ) ;
     
     global_variance = 0 ;
     for (m=0; m<flow_size.lines; m++)
       for (n=0; n<flow_size.cols; n++)
         global_variance +=   SQR(variance_mask[m][n] - global_mean) ;
   
     global_variance /= (flow_size.lines*flow_size.cols-1) ;
   
     if (global_variance == 0)
       for (m=0; m<flow_size.lines; m++)
         for (n=0; n<flow_size.cols; n++)
            variance_mask[m][n] = 1 ;
     else
       for (m=0; m<flow_size.lines; m++)
         for (n=0; n<flow_size.cols; n++)
            variance_mask[m][n] =  (float) exp ( -SQR(variance_mask[m][n]) / (8*global_variance) ) ; 
              
     /* Prepare next iteration */
     aux_mask      = prev_mask ;
     prev_mask     = variance_mask ;
     variance_mask = aux_mask ;     
  } 
  
  mask_free(variance_mask,flow_size) ;
  variance_mask = prev_mask ;
            
  return variance_mask ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                 DOWNSAMPLE FLOW                                     *
 *                                                                                     *
 *    Purpose          : downsampling (and filtering) the flow field according to      *
 *                       confidence mask                                               *
 *                                                                                     *
 *    Arguments in     : mat_size_t flow_size    - image size ( in blocks )            *
 *                                                                                     *
 *    Arguments in/out : flow_t * flow           - motion flow field  (in -> out )     *
 *        int * block_size        - block size ( 1 -> 2 )                              *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void downsample_flow( flow_t * flow, mat_size_t * flow_size,   int * block_size ) 
{
   register int   m,n ;
   mask_t         conf_mask ;
   float          weight_sum ;
   flow_t         new_flow ;
   mat_size_t     new_flow_size ;
   
   conf_mask = construct_mask( *flow , *flow_size ) ;
     
   new_flow_size.lines = (  long) floor( flow_size->lines/2 ) ;
   new_flow_size.cols  = (  long) floor( flow_size->cols /2 )  ;
   
   new_flow = flow_alloc( new_flow_size ) ;
   
   /* fprintf(stdout," Filtering and downsampling the motion field : [%dx%d] -> [%dx%d],  BLOCK_SIZE %d -> %d  ... ",flow_size->cols,flow_size->lines,new_flow_size.cols,new_flow_size.lines,(*block_size),(*block_size) * 2) ;*/
   
   for ( m = 0; m < new_flow_size.lines; m++ )
     for ( n = 0; n < new_flow_size.cols; n++ )
     {
        weight_sum = conf_mask[2*m][2*n] + conf_mask[2*m+1][2*n] + conf_mask[2*m+1][2*n+1] + conf_mask[2*m][2*n+1] ;
        if (weight_sum != 0 )
        {
          new_flow[m][n].x = ( (*flow)[2*m][2*n].x * conf_mask[2*m][2*n] + (*flow)[2*m+1][2*n].x * conf_mask[2*m+1][2*n] + (*flow)[2*m+1][2*n+1].x*conf_mask[2*m+1][2*n+1] + (*flow)[2*m][2*n+1].x*conf_mask[2*m][2*n+1] ) / weight_sum ;
          new_flow[m][n].y = ( (*flow)[2*m][2*n].y * conf_mask[2*m][2*n] + (*flow)[2*m+1][2*n].y * conf_mask[2*m+1][2*n] + (*flow)[2*m+1][2*n+1].y*conf_mask[2*m+1][2*n+1] + (*flow)[2*m][2*n+1].y*conf_mask[2*m][2*n+1] ) / weight_sum ;
        }
        else 
        {
          new_flow[m][n].x = ( (*flow)[2*m][2*n].x + (*flow)[2*m+1][2*n].x + (*flow)[2*m+1][2*n+1].x + (*flow)[2*m][2*n+1].x ) / 4 ;
          new_flow[m][n].y = ( (*flow)[2*m][2*n].y + (*flow)[2*m+1][2*n].y + (*flow)[2*m+1][2*n+1].y + (*flow)[2*m][2*n+1].y ) / 4 ;       
        }
     
     }

   mask_free(conf_mask,*flow_size) ;
   flow_free(*flow,*flow_size) ;
   
   (*flow) = new_flow ;
   (*flow_size).lines = new_flow_size.lines ;
   (*flow_size).cols  = new_flow_size.cols ;
   (*block_size) *= 2 ;
   
   /* fprintf(stdout," done \n") ; */
}   

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                 SHOT_CAMERA_MOTION_DESCRIPTOR_EXTRACTION                            *
 *                                                                                     *
 *    Purpose          : Extract camera parameters at the shot level                   *
 *                                                                                     *
 *    Arguments in     : bma_buffer  -  motion vectors buffer for the whole shot       *
 *                       shot_size   - size of the shot (number of frames)             *
 *                       image_bsize - size of the image (in macro-blocks)             * 
 *                                                                                     *
 *    Arguments in/out :                                                               *
 *                                                                                     *
 *                                                                                     *
 *    Arguments out    :                                                               *
 *                                                                                     *
 *    Return values    :                                                               *
 *                                                                                     *
 *    Description      :                                                               *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
 
void shot_camera_motion_descriptor_extraction(  int *    bma_buffer,
                                                int      shot_size, 
                                                mat_size_t  image_bsize,
                                                motion_histogram_t *    motion_histogram,
                                                motion_histogram_t * motion_speeds,
                                                med_low_level_desc_t * motion_chrono )
{
   int         frame_number, motion_type ;
   int         block_size, init_block_size  ;
   int         nb_ref_points ;
   
   mat_size_t  image_size, init_image_bsize ; 
   flow_t      flow ;
   block_t *   mask_1D ;
   
   register int  m, n ;
      
   inst_rot_param_t       vect_mini  = { 0, 0, 0, 0, 500 } ;
   inst_rot_param_t       init_vect  = { 0, 0, 0, 0, 500 } ;
   inst_cam_param_t       real_param = { 0, 0, 0, 0, 0, 0, 0, 0, 500, 0 };
   
   float       start_simplex[NB_PARA+1][NB_PARA] ;
   float *     results_buffer ;
   float *     filt_buffer ;
   
   char     motion_text[NB_MOTION_TYPES][STRSIZE] ;

   init_block_size         = MOT_BSIZE ;   
   image_size.lines        = image_bsize.lines * init_block_size ;
   image_size.cols         = image_bsize.cols  * init_block_size ;
   block_size              = init_block_size ; 
   init_image_bsize.lines  = image_bsize.lines ;
   init_image_bsize.cols   = image_bsize.cols ;
     
   /* RESULTS BUFFER MEMORY ALLOCATION  */
   results_buffer = (float*) calloc( (shot_size) * 10 , sizeof(float) ) ;
   if ( results_buffer == NULL )
   {
     fprintf(stderr," \n*** ERROR, Memory allocation problem : Not enough memory for results_buffer *** \n\n") ;
     exit(1) ;
   }

   /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * Camera motion parameters estimation, from the start to the end of the shot  * 
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

   nb_ref_points = (int) floor( REF_POINTS_FRAC * image_bsize.lines * image_bsize.cols ) ;

   fprintf(stdout,"\n INSTANTANEOUS CAMERA MOTION PARAMETERS ESTIMATION \n") ;
   fprintf(stdout," _________________________________________________\n\n");
   fprintf(stdout," Motion field size (Blocks) :\t\t [%dx%d] \n",image_bsize.cols,image_bsize.lines);
   fprintf(stdout," Video segment duration :\t\t %d frames \n",shot_size);
   
   for (frame_number = 0 ; frame_number < shot_size ; frame_number++)    /* SHOT LEVEL LOOP */
   {
	   fprintf(stderr," Frame #%d/%d      \r",frame_number+1,shot_size) ;
       /* CORE INSTANTANEOUS CAMERA MOTION ESTIMATION ALGORITHM */
       {
         block_size  = init_block_size ;
         image_bsize.lines = init_image_bsize.lines ;
         image_bsize.cols  = init_image_bsize.cols ;

         flow = read_flow_from_buffer(bma_buffer,image_bsize,frame_number) ;

         downsample_flow(&flow, &image_bsize, &block_size ) ;
         downsample_flow(&flow, &image_bsize, &block_size ) ;

         nb_ref_points = (int) floor( REF_POINTS_FRAC * image_bsize.lines * image_bsize.cols ) ;
         
         if ( null_motion(flow,image_bsize) == TRUE ) 
         {
           /* NO MOTION BETWEEN THE 2 FRAMES */
           for (m=0; m<NB_PARA-1; m++) vect_mini[m]  = 0 ;
             for (m=0; m<8 ; m++)        real_param[m] = 0 ; 
               real_param[9] = 1 ; /* No motion */   
         }
         else   
         {
           /* INSTANTANEOUS MOTION ANALYSIS */
           mask_1D = choose_ref_points( flow, image_bsize, nb_ref_points ) ;    
         
           if ( fine_null_motion( flow, mask_1D, nb_ref_points ) == TRUE )
           {
             /* NO MOTION BETWEEN THE 2 FRAMES */
             for (m=0; m<NB_PARA-1; m++)  vect_mini[m]  = 0 ;
               for (m=0; m<8 ; m++)       real_param[m] = 0 ; 
                 real_param[9] = 1 ; /* No motion */
           }
           else
           {
             /* PERFORMS THE MINIMIZATION */      
             construct_start_simplex ( &start_simplex, init_vect ) ;
                 
             simplex_mini ( start_simplex,&vect_mini,MIN_SIMP_IT,MAX_SIMP_IT,SIMP_PREC,flow,mask_1D,image_bsize,block_size,nb_ref_points) ;
             get_real_param (&real_param,flow,vect_mini,mask_1D,image_bsize,block_size,nb_ref_points ) ;
                 
             init_vect[4] = ( (frame_number+1) * init_vect[4] + vect_mini[4] ) / ( frame_number+2) ;
                 
           }
           free ( mask_1D ) ;
         }
         flow_free(flow,image_bsize) ;
       }
     
     /* Simplex initialisation from last frame features estimation */     
     for (m=0; m<NB_PARA-1; m++)  init_vect[m] =  vect_mini[m]   ;   
     
     /* Write instantaneous parameters in buffer */
     for (m=0; m<10; m++) results_buffer[m*shot_size+frame_number] =  real_param[m] ;
  
   }  /* Frames LOOP ending */

   fprintf(stderr,"                                     \n") ;

   /* * * * * * * * * * * * * * * * * * * * * 
    * MEDIAN & TEMPORAL FEATURES FILTERING  * 
    * * * * * * * * * * * * * * * * * * * * */
   
   for (m=0; m<8; m++)
   {
     filt_buffer  = clean ( &(results_buffer[m*shot_size]) , (long) shot_size ) ;
       
     for (n=0; n  < shot_size; n++)
       results_buffer[m*shot_size + n] = filt_buffer[n]  ; 
         
     free(filt_buffer) ; 
   }
 
   /* * * * * * * * * * * * * * * * * * * * * * 
    * LONG-TERM ANALYSIS                      *  
    * * * * * * * * * * * * * * * * * * * * * */
    
   long_term_analysis       ( motion_chrono, results_buffer, shot_size ) ;  
   med_low_to_med_high_desc ( (*motion_chrono), motion_histogram , motion_speeds, shot_size ) ;
    
   /* Motion text descriptions initialisations */
   strcpy(motion_text[TRACK_LEFT],  "TRACK LEFT ") ;
   strcpy(motion_text[TRACK_RIGHT], "TRACK RIGHT") ;
   strcpy(motion_text[BOOM_DOWN],   "BOOM DOWN  ") ;
   strcpy(motion_text[BOOM_UP],     "BOOM UP    ") ;
   strcpy(motion_text[TILT_DOWN],   "TILT DOWN  ") ;
   strcpy(motion_text[TILT_UP],     "TILT UP    ") ;
   strcpy(motion_text[PAN_LEFT],    "PAN LEFT   ") ;
   strcpy(motion_text[PAN_RIGHT],   "PAN RIGHT  ") ;
   strcpy(motion_text[ROLL_LEFT],   "ROLL LEFT  ") ;
   strcpy(motion_text[ROLL_RIGHT],  "ROLL RIGHT ") ;
   strcpy(motion_text[ZOOM_OUT],    "ZOOM OUT   ") ;
   strcpy(motion_text[ZOOM_IN],     "ZOOM IN    ") ;
   strcpy(motion_text[FIXED],       "FIXED      ") ;
   strcpy(motion_text[UNKNOWN],     "UNKNOWN    ") ;

   fprintf(stdout," \n LONG-TERM ANALYSIS (Description computation) \n") ;
   fprintf(stdout," ____________________________________________\n\n") ;
    
   for (motion_type=0; motion_type<NB_MOTION_TYPES; motion_type ++ )
   {
     if ( (*motion_chrono)[motion_type].nb_clusters != 0 )
     {
       fprintf(stdout,"\t %s \n",motion_text[motion_type]) ;
       for (m=0; m<(*motion_chrono)[motion_type].nb_clusters; m++)
         if (( motion_type != FIXED ) & ( motion_type != UNKNOWN ))
           fprintf(stdout,"\t   %4d -> %4d   :  %3.1f pix/frame   std dev = %3.2f\n",
               (*motion_chrono)[motion_type].start_frame[m]+1,
               (*motion_chrono)[motion_type].stop_frame[m]+1,
               (*motion_chrono)[motion_type].avg_value[m],
               (*motion_chrono)[motion_type].std_dev[m]) ;
         else 
           fprintf(stdout,"\t   %4d -> %4d   \n",(*motion_chrono)[motion_type].start_frame[m]+1, (*motion_chrono)[motion_type].stop_frame[m]+1 ) ;
          
         fprintf(stdout,"\n") ;
      }
    }

    /* ALLOCATED MEMORY FREEING */
    free((char*)results_buffer) ;
    free((char*)bma_buffer) ;
}

void ReadY_RAW( char *y_file, int frame_nb, short *YFrame, int width, int height)
{
  FILE            *file;
  unsigned char   *data;
  int             i, j, error;
  int             size_y, size_uv, skip, framesize;

  size_y    = width * height;
  size_uv   = size_y / 4;
  framesize = size_y ; //+ 2 * size_uv;

  data = (unsigned char *) malloc(framesize * sizeof(unsigned char));

  /************************** Y_Data ***************************/
  if((file = fopen (y_file,"rb"))==NULL)
  {
    fprintf(stderr," - ERROR: can not open Y file");
    fprintf(stderr,"(%s)\n",y_file);
    return;
  }

  /* No. of bits we have to skip to reach the frame */
   skip = frame_nb * framesize;

   error = fseek (file, skip, SEEK_SET);

  /* The data */
  error = fread ( (char*) data, size_y, 1, file);
  if (error == 0)
    fprintf(stderr," - ERROR: input Y-Data incomplete\n");

  for(j = 0; j < height; j++)
    for(i = 0; i < width; i++)
      YFrame[j*width + i] = (short) data[j*width + i];

  /*fprintf(stdout,"\t\t - Read Y-Data\n");*/
  fclose (file);

 free((char*)data);
}

