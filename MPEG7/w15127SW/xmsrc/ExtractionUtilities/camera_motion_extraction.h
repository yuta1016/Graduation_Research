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
// Applicable File Name:  camera_motion_extraction.h
//        

#ifndef _CAMERA_H
#define _CAMERA_H

#include   <stdio.h>
#include   <stdlib.h>
#include   <math.h>
#include   <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               MACROS DEFINITION                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SWAP
#define SWAP(a,b)     temp=(a) ; (a) = (b) ; (b) = temp ;
#endif
#ifndef SQR
#define SQR(x)        ((x)*(x))
#endif
#ifndef ABS
#define ABS(x)        ((x > 0) ? (x) : (-(x)))
#endif
#ifndef NORM
#define NORM(v)       sqrt( SQR((v).x) + SQR((v).y) )
#endif

#ifndef MIN
  #define MIN(a,b)      (((a)<(b)) ? (a) : (b))   /* minumum of two values   */
#endif

#ifndef MAX
  #define MAX(a,b)      (((a)>(b)) ? (a) : (b))   /* maximum of two values   */
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               CONSTANTS DEFINITION                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TRUE
  #define TRUE  1
#endif

#ifndef FALSE
  #define FALSE 0
#endif

/* BLOCK SIZE for pre-processing block matching */
#define MOT_BSIZE    8
#define SEARCH_RANGE 8

#ifndef STRSIZE
  #define STRSIZE    100
#endif /* STRSIZE */

/* Number of reference points */
#define REF_POINTS_FRAC    (float)  0.25

/* Minimum displacement that generates a subjectively moving pixel */
#define MOTION_SENSITIVITY (float) 0.15

/* Number of rotational parameters to minimize */
#define NB_PARA      5
/* Number of real parameters, defined in the following */
#define NB_REAL_PARA 10 

/* Minimum percentage of not moving blocks that generates subjectively fixed frames */
#define NULL_VECT_PERCENTAGE  (float) 0.35    

/* Number of mask enhancement iterations */
#define NB_MASK_ITERATIONS    4    

/* Motion Types ID Definition  */
#define TRACK_LEFT         0
#define TRACK_RIGHT        1
#define BOOM_DOWN          2
#define BOOM_UP            3
#define TILT_DOWN          4
#define TILT_UP            5
#define PAN_LEFT           6
#define PAN_RIGHT          7
#define ROLL_LEFT          8
#define ROLL_RIGHT         9
#define ZOOM_OUT           10
#define ZOOM_IN            11
#define FIXED              12
#define UNKNOWN            13

/* Number of motion types taken into account */
#define NB_MOTION_TYPES    14

/* LONG-TERM ANALYSIS thresholds, minimum values for a consistent motion cluster */
#define DURATION_THRESHOLD      30
#define FIX_DURATION_THRESHOLD  5
#define PAN_THRESHOLD           (float) 0.5
#define TILT_THRESHOLD          (float)  0.5
#define ROLL_THRESHOLD          (float)  0.05
#define ZOOM_THRESHOLD          (float)  0.5
#define TRACK_THRESHOLD         (float)  0.5
#define BOOM_THRESHOLD          (float) 0.5
#define EXTRAPOL_LIMIT          50 

/* half window size for temporal median filtering (in frames) */
#define MEDIAN_WIN_INC 10 

/* Translation ratios thresholds */
#define ALPHA_THRESHOLD (float) 0.80
#define BETA_THRESHOLD  (float) 0.5

/* Simplex constants definition */
#define MIN_SIMP_IT     10
#define MAX_SIMP_IT     1000
#define SIMP_PREC       (float) 0.01
#define ELEM_INC        0.1
#define FOCAL_INC       50 

#define MINY            0      /* minimal Y value */
#define MAXY            255    /* maximal Y value */
#define MEDY            128    /* median  Y value */

#define MINUV           0      /* minimal U/V value */
#define MAXUV           255    /* maximal U/V value */
#define MEDUV           128    /* median  U/V value */

#define CLIP(a,min,max) MAX(min,MIN(max,a))       /* clipping of value       */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               DATATYPES DEFINITION                                */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef float    motion_histogram_t[NB_MOTION_TYPES-1] ; 
/* 1-> 100 % for 13 motion types */

typedef  struct
{
  int       nb_clusters ;
  int *     start_frame ;
  int *     stop_frame  ;
  float *   avg_value   ;
  float *   std_dev     ;
} motion_chrono_t ;

typedef  motion_chrono_t   med_low_level_desc_t[NB_MOTION_TYPES] ;

typedef struct
{
  int       nb_frames               ;
  float*    motion[NB_MOTION_TYPES] ;
} low_level_desc_t ;

/* Motion vectors flow and matrices */

typedef struct 
{
   float x ;
   float y ;
}  float_vector_t ;

typedef struct
{                         
   int      x;
   int      y;
}  vector_t;

typedef struct 
{
   int cols ;
   int lines ;
}  mat_size_t ;

typedef struct
{
   int m ;
   int n ;
   float value ;
}  block_t ; 

typedef   float_vector_t ** flow_t ;

typedef   float ** mask_t    ; 

/* Camera parameters */

typedef float inst_cam_param_t[NB_REAL_PARA] ;

/* * * * * * * * * * * * * * * * * * * * * * * * 
 *                                             * 
 *            Parameters definition            * 
 *                                             * 
 *  0 : Tracking ( Translation X )             * 
 *  1 : Booming  ( Translation Y )             * 
 *  2 : Tilting  ( Rotation X )                * 
 *  3 : Panning  ( Rotation Y )                * 
 *  4 : Rolling  ( Rotation Z )                * 
 *  5 : Zooming  ( Change of focal length )    *  
 *  6 : Alpha    ( Translation amount ratio )  * 
 *  7 : Beta     ( Parallelism ratio )         * 
 *  8 : f        ( Focal length )              * 
 *  9 : fixed    ( No motion )                 * 
 *                                             * 
 * * * * * * * * * * * * * * * * * * * * * * * */

typedef float inst_rot_param_t[NB_PARA] ;

/* * * * * * * * * * * * * * * * * * * * * * * * 
 *    0 : Rx    ( Tilting )                    * 
 *    1 : Ry    ( Panning )                    * 
 *    2 : Rz    ( Rolling )                    * 
 *    3 : Rzoom ( Zooming )                    * 
 *    4 : f     ( Focal length )               * 
 * * * * * * * * * * * * * * * * * * * * * * * */

/* Simplex definition (used in the minimization process */
typedef float simplex_t[NB_PARA+1][NB_PARA] ;

typedef char   string_t[STRSIZE];  /* Strings are arrays of characters.  */

/* Interfaces with the XM architecture */
typedef struct  {
   long     start_frame ;
   long     stop_frame ;
   double   frame_rate ;
   char     mpeg_file_name[STRSIZE] ;
   int      h_im_size ;
   int      v_im_size ;
} MediaForCameraMotion ;

typedef struct CameraMotionInfo_t {
   long     startTime ;
   long     duration ;
   double   presence[15] ;
   double   speeds[14] ;
   double   x_FoeFoc ;
   double y_FoeFoc ;
} CameraMotionInfo_t ;
                                                
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                           FUNCTIONS PROTOTYPES                                    */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
                                                
/* Pre-processing of motion vectors field for camera parameters extraction*/
flow_t      read_flow_from_buffer   ( int  *  bma_buffer, mat_size_t im_bsize,   int im_number ) ;
mask_t      construct_mask          ( flow_t flow, mat_size_t  flow_size ) ;
int         null_motion             ( flow_t  flow , mat_size_t   flow_size ) ;
void        downsample_flow         ( flow_t * flow, mat_size_t * flow_size, int * block_size ) ;
block_t *   choose_ref_points       ( flow_t flow, mat_size_t   flow_size, int nb_ref_points ) ;
int         compute_nb_downsampling ( mat_size_t image_size ) ;

/* Post-processing routines , temporal filtering and long-term analysis : extraction of descriptors */
float *           clean                     ( float * vector, long    n ) ;
motion_chrono_t   find_motion_clusters      ( float *  features_buffer, int nb_frames, int min_consistent_frames, float min_mean_value, float min_motion_value ) ;
void              long_term_analysis        ( med_low_level_desc_t * med_low_desc, float * inst_features_buffer, int nb_frames ) ;
void              merge_clusters            ( motion_chrono_t * old_chrono ) ;
void              med_low_to_med_high_desc  ( med_low_level_desc_t med_low_desc , motion_histogram_t * motion_histogram, motion_histogram_t * motion_speeds, int nb_frames ) ;
                            
/* Mathematical routines for camera parameters extraction software */
 
float           min_vector    ( float * vector ,   long nb_el )  ;
float           max_vector    ( float * vector ,   long nb_el )  ;
long            arg_max       ( float * vector ,   long nb_el ) ;
long            arg_min       ( float * vector ,   long nb_el ) ;
float           max2          ( float a, float b ) ;
float           min2          ( float a, float b ) ;
float           variance      ( float * vector ,   long nb_el) ;
float           mean          ( float * vector ,   long nb_el ) ;
float_vector_t  mean_flow     ( flow_t flow , mat_size_t flow_size ) ;
float           mean_mat      ( float ** matrix , mat_size_t mat_size ) ;
float           select        ( float * vector,   long  n,   long  k  ) ;
float           median        ( float * vector,   long n ) ;
void           order_k_first  ( block_t * vector,   long  n,   long  nb_points  );
int             float2int  ( float a ) ;

/* Frame-based camera motion estimation routines, based on simplex minimization of parallelism deviation */

void      construct_start_simplex ( simplex_t * start_simplex, inst_rot_param_t init_vect ) ;
void      simplex_mini            ( simplex_t simplex, inst_rot_param_t * vect_mini, int nb_it_min, int nb_it_max, float epsilon, flow_t flow, block_t * mask_1D, mat_size_t im_bsize, int bsize, int nb_ref_points ) ;
float     cost_function           ( flow_t flow,  block_t * mask_1D, inst_rot_param_t rot_vect, mat_size_t im_bsize, int bsize, int nb_ref_points) ;
void      get_real_param          ( inst_cam_param_t * real_param, flow_t flow, inst_rot_param_t rot_vect, block_t * mask_1D, mat_size_t im_bsize, int bsize, int nb_ref_points ) ;
                                  
/* Motion estimation */
void   find_vectors              ( short *y_buffer, vector_t *vectors,  mat_size_t image_size) ;
void   search_fullpel            ( short * frame1, short * frame2, int h_size, int v_size, int mb_h_size, int mb_v_size, int range_x, int range_y, vector_t * vector );
int *  compute_block_matching_raw( char * raw_filename , int frame_start, int frame_stop, mat_size_t image_size  ) ;

/* Specific Memory allocation functions */
mask_t           mask_alloc            ( mat_size_t mask_size ) ;
flow_t           flow_alloc            ( mat_size_t flow_size ) ;
motion_chrono_t  motion_chrono_alloc   ( int  clusters ) ;
low_level_desc_t low_level_desc_alloc  ( int frames ) ;

void             flow_free             ( flow_t flow, mat_size_t flow_size );
void             mask_free             ( mask_t mask, mat_size_t mask_size );
void             motion_chrono_free    ( motion_chrono_t chrono ) ;
void             low_level_desc_free   ( low_level_desc_t  low_desc ) ;

void ReadY_RAW( char *y_file, int frame_nb, short *YFrame, int width, int height) ;   

void shot_camera_motion_descriptor_extraction( int * bma_buffer, int shot_size, mat_size_t image_bsize, motion_histogram_t * motion_histogram, motion_histogram_t * motion_speeds, med_low_level_desc_t *  motion_chrono);



#ifdef __cplusplus
	extern "C" CameraMotionInfo_t ExtractCameraParameters( MediaForCameraMotion aMedia  ) ;
	extern "C" MediaForCameraMotion * ComputeFromText( char* media_parameters)  ;
#else
	CameraMotionInfo_t ExtractCameraParameters( MediaForCameraMotion aMedia  ) ;    
	MediaForCameraMotion * ComputeFromText( char* media_parameters)  ;
#endif

#endif /* _CAMERA_H */

