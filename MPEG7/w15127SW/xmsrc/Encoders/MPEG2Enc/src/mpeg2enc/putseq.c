/* putseq.c, sequence level routines                                        */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */
/* Copyright (C) 2000, SONY Corp. Japan, Peter Kuhn (PK) for MPEG-7 extensions */

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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "global.h"

void putseq()
{
  /* this routine assumes (N % M) == 0 */
  int i, j, k, f, f0, n, np, nb, sxf, syf, sxb, syb, pk, pp, p0, last_p;
  int sxfn, syfn, sxbn, sybn;
  int ipflag;
  long zeit0, zeit1; // PK
  int nsegstart, gop_n; // PK

  FILE *fd;
  char name[256];
  unsigned char *neworg[3], *newref[3];
  static char ipb[5] = {' ','I','P','B','D'};
  last_p=0;

  rc_init_seq(); /* initialize rate control */

  /* sequence header, sequence extension and sequence display extension */
  putseqhdr();
  if (!mpeg1)
  {
    putseqext();
    putseqdispext();
  }

  /* optionally output some text data (description, copyright or whatever) */
  if (strlen(id_string) > 1)
    putuserdata(id_string);
    
  // PK determine gop structure 
  determine_gop();
      
  /* loop through all frames in encoding/decoding order */
  // for (i=0; i<nframes; i++)
  for (i=0, pp=0; pp<nframes; i++, pp++)
  {
    if (!quiet)
    {
      fprintf(stderr,"Encoding frame %d ",pp);
      fflush(stderr);
    }

    /* f0: lowest frame number in current GOP
     *
     * first GOP contains N-(M-1) frames,
     * all other GOPs contain N frames
     */
 // ------------- PK start -------------- 
        
   if(XML_input==1){
   
        // PK: use calculated N (or N) only when one of these is 0 in the par file	
        if(FIXGOP==0) 
		N = framedata[pp].N;							
        if(M==0) 
		M = framedata[pp].M;
				      		  		  		  	
	gop_n = framedata[pp].gop_n;   
	f0 = framedata[pp].f0; 
	
	//put M-1 P-frames before a new segment starts  	  
	if((framedata[pp+(M-1)].display_new_seg==1) || (gop_n <= M-1)){ 
		last_p=1;
	}
	//start a new segment	  
	if(framedata[pp].display_new_seg==1){ 
		i=0;
	}
	if(i==f0){	
		last_p = 0; // PK
	}	
	
	motion_data[0].sxf = framedata[pp].sxp; // P-frame
	motion_data[0].syf = framedata[pp].syp; // P-frame
	motion_data[0].sxfn = framedata[pp].sxn; // P-frame
	motion_data[0].syfn = framedata[pp].syn; // P-frame
	 		    						    	   	    
        for(pk=1; pk < M; pk++){	// B-frames forward and backward
          	motion_data[pk].sxf  = (int) (((double) framedata[pp].sxp)/4); 
	  	motion_data[pk].sxfn = (int) (((double) framedata[pp].sxn)/4);
	  
 	  	motion_data[pk].syf  = (int) (((double) framedata[pp].syp)/4);
	  	motion_data[pk].syfn = (int) (((double) framedata[pp].sxn)/4);
	   
	  	motion_data[pk].sxb  = (int) (((double) framedata[pp].sxp)/2);
	  	motion_data[pk].sxbn = (int) (((double) framedata[pp].sxn)/2);
	  	  
 	  	motion_data[pk].syb  = (int) (((double) framedata[pp].syp)/2);
	  	motion_data[pk].sybn = (int) (((double) framedata[pp].sxn)/2);	  				     		   	  
        }         			
    } else {
        f0 = N*((pp+(M-1))/N) - (M-1);     
        if (f0<0) f0=0;
    }
  
    if(DIAG){  
      printf("\n");
      
      if(framedata[pp].display_new_seg == 1) printf("D");
      else printf(" ");
	 
      if(i == 0)  printf("*");
      else printf(" ");
	 	 
      printf("pp:%d i:%d M:%d N:%d gop_n:%d f0:%d m_un:%f ", 
    	        pp, i, M, N, framedata[pp].gop_n, f0, framedata[pp].m_uncomp);
		 
      printf("sxf:%d syf:%d sxfn:%d syfn:%d sxb:%d sxbn:%d syb:%d sybn:%d bits:%f XML:%d last_p:%d ",
		motion_data[0].sxf,
		motion_data[0].syf,  
		motion_data[1].sxfn, 
		motion_data[1].syfn,
		motion_data[1].sxb, 
		motion_data[1].sxbn, 
		motion_data[1].syb, 
		motion_data[1].sybn,
		framedata[pp].frame_bits,
		XML_input,
		last_p);
    }  
    
    framenum = pp;

#ifdef IPROF  
    if (framenum%25==0){  // write statistics for iprof every 100 frames, PK 
       __bb_write_file();
       __bb_set_zero();
    } 
#else  
    if (framenum%25==0){ 
       if(framenum==0){
         zeit0 = time(zerotime);
       } else {
         zeit0 = zeit1;
       }
       zeit1 = time(zeit);	           
       print_time_stats(framenum, (double) zeit1-zeit0);
    }
 #endif             
				   
   
 
    if (i==0 || ((i-1)%M==0) || (last_p==1))
    {   
      /* I or P frame */
      for (j=0; j<3; j++)
      {
        /* shuffle reference frames */
        neworg[j] = oldorgframe[j];
        newref[j] = oldrefframe[j];
        oldorgframe[j] = neworgframe[j];
        oldrefframe[j] = newrefframe[j];
        neworgframe[j] = neworg[j];
        newrefframe[j] = newref[j];
      }

      /* f: frame number in display order */
      if(last_p==0){
	f = (i==0) ? pp : pp+M-1;	
	if(framedata[pp+M].display_new_seg==1) f = pp;	  	
      } else {
        f = pp;
      }
      
      if (f>=nframes)
        f = nframes - 1;

      if (i==f0) /* first displayed frame in GOP is I */ 
      {
        p0 = pp; // PK
	
        /* I frame */
        pict_type = I_TYPE;
        forw_hor_f_code = forw_vert_f_code = 15;
        back_hor_f_code = back_vert_f_code = 15;

        /* n: number of frames in current GOP
         *
         * first GOP contains (M-1) less (B) frames
         */
        n = (i==0) ? N-(M-1) : N;

        /* last GOP may contain less frames */
  	if (n > nframes-p0)
            n = nframes-p0;
		          
	if(XML_input==1){ // PK: last GOP in segment may contain less frames
	   n = gop_n;
	   printf(" * gop_n:%d p0:%d *",n, p0); 
	}
 
        /* number of P frames */
        if (i==0)
          np = (n + 2*(M-1))/M - 1; /* first GOP */
        else
          np = (n + (M-1))/M - 1;

        /* number of B frames */
        nb = n - np - 1;

        if (last_p==1){
		nb -= M-1;
		np += M-1;
	}

        rc_init_GOP(np,nb);

        putgophdr(p0,i==0); /* set closed_GOP in first GOP only */
      }
      else
      {
        /* P frame */
        pict_type = P_TYPE;
        forw_hor_f_code = motion_data[0].forw_hor_f_code;
        forw_vert_f_code = motion_data[0].forw_vert_f_code;
        back_hor_f_code = back_vert_f_code = 15;
        sxf = motion_data[0].sxf;
        syf = motion_data[0].syf;
	sxfn = motion_data[0].sxfn;
        syfn = motion_data[0].syfn;

      }
    }
    else
    {
      /* B frame */
      for (j=0; j<3; j++)
      {
        neworg[j] = auxorgframe[j];
        newref[j] = auxframe[j];
      }

      /* f: frame number in display order */
      f = pp - 1;
      pict_type = B_TYPE;
      n = (i-2)%M + 1; /* first B: n=1, second B: n=2, ... */
      forw_hor_f_code = motion_data[n].forw_hor_f_code;
      forw_vert_f_code = motion_data[n].forw_vert_f_code;
      back_hor_f_code = motion_data[n].back_hor_f_code;
      back_vert_f_code = motion_data[n].back_vert_f_code;
      sxf = motion_data[n].sxf;
      syf = motion_data[n].syf;
      sxb = motion_data[n].sxb;
      syb = motion_data[n].syb;
      sxfn = motion_data[n].sxfn;
      syfn = motion_data[n].syfn;
      sxbn = motion_data[n].sxbn;
      sybn = motion_data[n].sybn;

    }
  
    temp_ref = f - p0;
    frame_pred_dct = frame_pred_dct_tab[pict_type-1];
    q_scale_type = qscale_tab[pict_type-1];
    intravlc = intravlc_tab[pict_type-1];
    altscan = altscan_tab[pict_type-1];

    fprintf(statfile,"\nFrame %d (#%d in display order):\n",pp,f);
    fprintf(statfile," picture_type=%c\n",ipb[pict_type]);
    fprintf(statfile," temporal_reference=%d\n",temp_ref);
    fprintf(statfile," frame_pred_frame_dct=%d\n",frame_pred_dct);
    fprintf(statfile," q_scale_type=%d\n",q_scale_type);
    fprintf(statfile," intra_vlc_format=%d\n",intravlc);
    fprintf(statfile," alternate_scan=%d\n",altscan);
    
    // ------------- PK end ----------------

    if (pict_type!=I_TYPE)
    {
      fprintf(statfile," forward search window: %d...%d / %d...%d\n",
        -sxf,sxf,-syf,syf);
      fprintf(statfile," forward vector range: %d...%d.5 / %d...%d.5\n",
        -(4<<forw_hor_f_code),(4<<forw_hor_f_code)-1,
        -(4<<forw_vert_f_code),(4<<forw_vert_f_code)-1);
    }

    if (pict_type==B_TYPE)
    {
      fprintf(statfile," backward search window: %d...%d / %d...%d\n",
        -sxb,sxb,-syb,syb);
      fprintf(statfile," backward vector range: %d...%d.5 / %d...%d.5\n",
        -(4<<back_hor_f_code),(4<<back_hor_f_code)-1,
        -(4<<back_vert_f_code),(4<<back_vert_f_code)-1);
    }

    sprintf(name,tplorg,f+frame0);
    readframe(name,neworg);

    if (fieldpic)
    {
      if (!quiet)
      {
        fprintf(stderr,"\nfirst field  (%s) ",topfirst ? "top" : "bot");
        fflush(stderr);
      }

      pict_struct = topfirst ? TOP_FIELD : BOTTOM_FIELD;

      motion_estimation(oldorgframe[0],neworgframe[0],
                        oldrefframe[0],newrefframe[0],
                        neworg[0],newref[0],
                        sxf,syf,sxb,syb,mbinfo,0,0,sxfn, syfn, sxbn, sybn);

      predict(oldrefframe,newrefframe,predframe,0,mbinfo);
      dct_type_estimation(predframe[0],neworg[0],mbinfo);
      transform(predframe,neworg,mbinfo,blocks);

      putpict(neworg[0]);

      for (k=0; k<mb_height2*mb_width; k++)
      {
        if (mbinfo[k].mb_type & MB_INTRA)
          for (j=0; j<block_count; j++)
            iquant_intra(blocks[k*block_count+j],blocks[k*block_count+j],
                         dc_prec,intra_q,mbinfo[k].mquant);
        else
          for (j=0;j<block_count;j++)
            iquant_non_intra(blocks[k*block_count+j],blocks[k*block_count+j],
                             inter_q,mbinfo[k].mquant);
      }

      itransform(predframe,newref,mbinfo,blocks);
      calcSNR(neworg,newref);
      stats();

      if (!quiet)
      {
        fprintf(stderr,"second field (%s) ",topfirst ? "bot" : "top");
        fflush(stderr);
      }

      pict_struct = topfirst ? BOTTOM_FIELD : TOP_FIELD;

      ipflag = (pict_type==I_TYPE);
      if (ipflag)
      {
        /* first field = I, second field = P */
        pict_type = P_TYPE;
        forw_hor_f_code = motion_data[0].forw_hor_f_code;
        forw_vert_f_code = motion_data[0].forw_vert_f_code;
        back_hor_f_code = back_vert_f_code = 15;
        sxf = motion_data[0].sxf;
        syf = motion_data[0].syf;
	sxfn = motion_data[0].sxfn;
        syfn = motion_data[0].syfn;
      }

      motion_estimation(oldorgframe[0],neworgframe[0],
                        oldrefframe[0],newrefframe[0],
                        neworg[0],newref[0],
                        sxf,syf,sxb,syb,mbinfo,1,ipflag,
			sxfn, syfn, sxbn, sybn);

      predict(oldrefframe,newrefframe,predframe,1,mbinfo);
      dct_type_estimation(predframe[0],neworg[0],mbinfo);
      transform(predframe,neworg,mbinfo,blocks);

      putpict(neworg[0]);

      for (k=0; k<mb_height2*mb_width; k++)
      {
        if (mbinfo[k].mb_type & MB_INTRA)
          for (j=0; j<block_count; j++)
            iquant_intra(blocks[k*block_count+j],blocks[k*block_count+j],
                         dc_prec,intra_q,mbinfo[k].mquant);
        else
          for (j=0;j<block_count;j++)
            iquant_non_intra(blocks[k*block_count+j],blocks[k*block_count+j],
                             inter_q,mbinfo[k].mquant);
      }

      itransform(predframe,newref,mbinfo,blocks);
      calcSNR(neworg,newref);
      stats();
    }
    else
    {
      pict_struct = FRAME_PICTURE;

      /* do motion_estimation
       *
       * uses source frames (...orgframe) for full pel search
       * and reconstructed frames (...refframe) for half pel search
       */

      motion_estimation(oldorgframe[0],neworgframe[0],
                        oldrefframe[0],newrefframe[0],
                        neworg[0],newref[0],
                        sxf,syf,sxb,syb,mbinfo,0,0,
			sxfn, syfn, sxbn, sybn);

      predict(oldrefframe,newrefframe,predframe,0,mbinfo);
      dct_type_estimation(predframe[0],neworg[0],mbinfo);
      transform(predframe,neworg,mbinfo,blocks);

      putpict(neworg[0]);

      for (k=0; k<mb_height*mb_width; k++)
      {
        if (mbinfo[k].mb_type & MB_INTRA)
          for (j=0; j<block_count; j++)
            iquant_intra(blocks[k*block_count+j],blocks[k*block_count+j],
                         dc_prec,intra_q,mbinfo[k].mquant);
        else
          for (j=0;j<block_count;j++)
            iquant_non_intra(blocks[k*block_count+j],blocks[k*block_count+j],
                             inter_q,mbinfo[k].mquant);
      }

      itransform(predframe,newref,mbinfo,blocks);
      calcSNR(neworg,newref);
      stats();
    }

    sprintf(name,tplref,f+frame0);
    writeframe(name,newref);

  }

  putseqend();
}
