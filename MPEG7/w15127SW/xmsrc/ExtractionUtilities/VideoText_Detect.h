///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Gang Wei,  Nevenka	Dimitrova,	and Lalitha Agnihotri - Philips Research.
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
// Applicable File Name:  videotext_detect.h
//
//////////////////////////////////////////////////////////////////////

#ifndef _VIDEOTEXT_DETECT_H
#define _VIDEOTEXT_DETECT_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_TEXT_MODEL_NUM 1000
#define MAX_TEXT_MODEL_PER_FRAME 20
#define MIN_NR_CHAR_THRESHOLD 2
#define MAX_TEXT_TRAJ_NUM 500

#define PIXEL short
//#define BYTE unsigned char


//===============================================================
struct STextModel{
	int cenRow;
	int cenCol;
	int height;
	int width;
	int frmNum;
	bool valid;
};

//===============================================================
class CTextFrame{
protected:
	STextModel textModels[MAX_TEXT_MODEL_PER_FRAME];
	int modelNum;
public:
	CTextFrame();
	int get_model_num(){return modelNum;}
	int get_valid_model_num();
	void add_model(STextModel model);
	void model_validate(CTextFrame * otherTextFrame);
	STextModel * get_models(){return textModels;}
	void clear(){modelNum=0;}
	void out_put(FILE * fpTextReport);
};

//===============================================================
class CTextModelMan{
protected:
	STextModel textModels[MAX_TEXT_MODEL_NUM];
	int modelNum;
	int imgHeight;
	int imgWidth;
public:
	CTextModelMan(){modelNum=0;}
	void set_height(int height){imgHeight=height;}
	void set_width(int width){imgWidth=width;}
	int get_height(){return imgHeight;}
	int get_width(){return imgWidth;}
	int get_model_num(){return modelNum;}
	STextModel * get_models(){return textModels;}
	void new_model(STextModel model){
		modelNum++; 
		if (modelNum>=MAX_TEXT_MODEL_NUM)
		{
			fprintf(stderr, " CTextModelMan overflow!");
//			MessageBox(NULL, " CTextModelMan overflow!", "Exit", MB_OK);
			exit(1);
		}
		textModels[modelNum]=model;
	}
};


//===============================================================
//===============================================================
class CTextTraj{
protected:
	int ID;
	int cenRow;
	int cenCol;
	int height;
	int width;
	int totalFrmNum;
	int startFrmNum;
	int endFrmNum;
	int I_frm_num;
public:
	CTextTraj(){I_frm_num=0;}
	void set_ID(int num){ID=num;}
	void set_cen_row(int row){cenRow=row;}
	void set_cen_col(int col){cenCol=col;}
	void set_height(int heightVal){height=heightVal;}
	void set_width(int widthVal){width=widthVal;}
	void set_total_frm_num(){totalFrmNum=endFrmNum-startFrmNum+1;}
	void set_start_frm_num(int num){startFrmNum=num;}
	void set_end_frm_num(int num){endFrmNum=num;}
	void add_model(STextModel textModel);
	bool is_similar(STextModel textModel);
	
	int get_ID(){return ID;}
	int get_cen_row(){return cenRow;}
	int get_cen_col(){return cenCol;}
	int get_height(){return height;}
	int get_width(){return width;}
	int get_total_frm_num(){return totalFrmNum;}
	int get_end_frm_num(){return endFrmNum;}
	int get_start_frm_num(){return startFrmNum;}

};

//===============================================================
class CTextTrajMan{
protected:
	CTextTraj textTrajs[MAX_TEXT_TRAJ_NUM];
	int trajNum;
	int imgHeight;
	int imgWidth;
	int meanFrmNum;
public:
	CTextTrajMan(){trajNum=0;}
	void filter_by_length(int lenThr);
	void add_model(STextModel textModel, int preFrmNum);
	CTextTraj * get_trajs(){return textTrajs;}
	void new_traj(STextModel textModel);
	void output_trajs(int style, char * sFileName, char * sVideoName);
	void set_mean_frm_num();
	int get_mean_frm_num(){return meanFrmNum;}
	int get_traj_num(){return trajNum;}
	int get_text_num_on_frame(int frmNum);
};

//===============================================================
//===============================================================
//pnBuf points to the Y component of a frame
//sTextReportText is a string containing location and size of each frame
void text_track(PIXEL *** colorData, int Height, int Width, 
				 CTextModelMan &textModelMan, CTextTrajMan &textTrajMan, 
				 int preFrmNum, int iFrameIndex);

//===============================================================
void text_detect(PIXEL *** colorData, int Height, int Width, 
				 CTextFrame * curTextFrame, int lResult);

void text_detect_2D(PIXEL ** imgData, int Height, int Width, 
					CTextFrame * curTextFrame, int iFrameIndex);


FILE * OpenFileNameMode(char *FileName, const char *mode);
PIXEL ** New2D(int XSize, int YSize);
void one_D_to_two_D(PIXEL ** Data, int Height, int Width, 
					unsigned char * pnOutBuf, int picWidth, int picHeight);
void short_one_D_to_two_D(PIXEL ** Data, int Height, int Width, 
						  short * pnOutBuf, int picWidth, int picHeight);
void Delete2D(PIXEL **Data, int RawSize); 
void write_raw_image(PIXEL **Data, int Height, int Width, char *FileName);

void write_image_data(PIXEL **Data, int RSize, int CSize, FILE *fp);

#endif
