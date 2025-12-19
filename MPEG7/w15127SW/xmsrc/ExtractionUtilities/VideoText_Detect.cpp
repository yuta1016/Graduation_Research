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
// Applicable File Name:  videotext_detect.cpp
//
//////////////////////////////////////////////////////////////////////

#include "VideoText_Detect.h"
#include "VideoText_Line.h"
#include "VideoText_FrameFile.h"

#include <math.h>
#include <stdlib.h>

CTextFrame::CTextFrame()
{
	modelNum=0;
	int i;
	for (i=1; i<MAX_TEXT_MODEL_PER_FRAME; i++)
		textModels[i].valid=false;
}
	
void CTextFrame::add_model(STextModel model)
{
	modelNum++; 
	if (modelNum>=MAX_TEXT_MODEL_PER_FRAME)
	{
		fprintf(stderr, " CTextFrame overflow!");
		//MessageBox(NULL, " CTextFrame overflow!", "Exit", MB_OK);
		exit(1);
	}
	textModels[modelNum]=model;
}

void CTextFrame::out_put(FILE *fpTextReport)
{
	int i;
	for (i=1; i<=modelNum; i++)
	{
		if (textModels[i].valid==false)
			continue;

		fprintf(fpTextReport, "%d	%d	%d	%d	%d\n", 
			textModels[i].frmNum, textModels[i].cenRow, textModels[i].cenCol,
			textModels[i].height, textModels[i].width);
	}
}

void CTextFrame::model_validate(CTextFrame * otherTextFrame)
{
	int i, j;
	bool match_found;
	int posDist, sizeDist, minSizeSum;
	int otherModelNum=otherTextFrame->get_model_num();
	STextModel *otherModels=otherTextFrame->get_models();
	for (i=1; i<=modelNum; i++)
	{
		match_found=false;
		for (j=1; j<=otherModelNum; j++)
		{
			posDist=abs(textModels[i].cenCol-otherModels[j].cenCol)
				+abs(textModels[i].cenRow-otherModels[j].cenRow);
			sizeDist=abs(textModels[i].height-otherModels[i].height)
				+abs(textModels[i].width-otherModels[i].width);
			if ((textModels[i].height+textModels[i].width) < (otherModels[i].height+otherModels[i].width))
				minSizeSum=textModels[i].height+textModels[i].width;
			else
				minSizeSum=otherModels[i].height+otherModels[i].width;

			if ((posDist<=0.05*minSizeSum)&&(sizeDist<=0.1*minSizeSum))
			{
				textModels[i].valid=true;
				otherModels[i].valid=true;
			}
		}
	}
}

int CTextFrame::get_valid_model_num()
{
	int i;
	int validModelNum=0;
	for (i=1; i<=modelNum; i++)
		if (textModels[i].valid==true)
			validModelNum++;

	return validModelNum;
}

void CTextTraj::add_model(STextModel textModel)
{
	int cenColSum=cenCol*I_frm_num+textModel.cenCol;
	int cenRowSum=cenRow*I_frm_num+textModel.cenRow;
	int heightSum=height*I_frm_num+textModel.height;
	int widthSum=width*I_frm_num+textModel.width;

	I_frm_num++;
	cenCol=cenColSum/I_frm_num;
	cenRow=cenRowSum/I_frm_num;
	height=heightSum/I_frm_num;
	width=widthSum/I_frm_num;
	endFrmNum=textModel.frmNum;
	totalFrmNum=endFrmNum-startFrmNum+1;
}

bool CTextTraj::is_similar(STextModel textModel)
{
	int posDist=abs(cenCol-textModel.cenCol)+abs(cenRow-textModel.cenRow);
	int sizeDist=abs(height-textModel.height)+abs(width-textModel.width);
	int minSizeSum;
	if ((height+width)<(textModel.height+textModel.width))
		minSizeSum=height+width;
	else
		minSizeSum=textModel.height+textModel.width;

	if (posDist>=0.05*minSizeSum)	return false;
	if (sizeDist>=0.1*minSizeSum)	return false;
	return true;
}

void CTextTrajMan::add_model(STextModel textModel, int preFrmNum)
{
	int i;
	bool match_found=false;
	if (trajNum==0)
	{
		new_traj(textModel);
		return;
	}
	for (i=1; i<=trajNum; i++)
		if (textTrajs[i].get_end_frm_num()==preFrmNum)
			if(textTrajs[i].is_similar(textModel))
			{
				textTrajs[i].add_model(textModel);
				match_found=true;
				break;
			}
	if(match_found==false)
		new_traj(textModel);
}

void CTextTrajMan::new_traj(STextModel textModel)
{
	trajNum++;
	textTrajs[trajNum].set_ID(trajNum);
	textTrajs[trajNum].set_start_frm_num(textModel.frmNum);
	textTrajs[trajNum].add_model(textModel);
}

void CTextTrajMan::output_trajs(int style, char * sFileName, char * sVideoName)
{
/*	FILE *fpOut;
	int i;

	switch(style)
	{
	case BRIEF:
		fpOut=OpenFileNameMode(sFileName, "w");
		fprintf(fpOut, "%s:\nThere are %d text trajectories in the video, averaging %d frames\n",
			sVideoName, trajNum, meanFrmNum);
		fclose(fpOut);
		break;
	case HEAD_ONLY:
		fpOut=OpenFileNameMode(sFileName, "a");
		fprintf(fpOut, "%s:\nThere are %d text trajectories in the video, averaging %d frames\n",
			sVideoName, trajNum, meanFrmNum);
		fclose(fpOut);
		break;
	case DETAIL:
		fpOut=OpenFileNameMode(sFileName, "w");
		fprintf(fpOut, "%s:\nThere are %d text trajectories in the video, averaging %d frames\n",
			sVideoName, trajNum, meanFrmNum);
		for(i=1; i<=trajNum; i++)
		{
			fprintf(fpOut, "	Traj %d: extends %d frames, row %d, col %d\n", 
				textTrajs[i].get_ID(), textTrajs[i].get_total_frm_num(),
				textTrajs[i].get_cen_row(), textTrajs[i].get_cen_col());
			fprintf(fpOut, "		starts from frame %d, ends at frame %d\n",
				textTrajs[i].get_start_frm_num(), textTrajs[i].get_end_frm_num());
		}
		fclose(fpOut);
		break;
	}
*/
}

void CTextTrajMan::set_mean_frm_num()
{
	int i;
	int frmNumSum=0;
	for (i=1; i<=trajNum; i++)
		frmNumSum+=textTrajs[i].get_total_frm_num();
	meanFrmNum=frmNumSum/trajNum;
}

void CTextTrajMan::filter_by_length(int lenThr)
{
	int i;
	int trajNumOld=trajNum;
	trajNum=0;
	for (i=1; i<=trajNumOld; i++)
	{
		if (textTrajs[i].get_total_frm_num()>=lenThr)
		{
			trajNum++;
			textTrajs[trajNum]=textTrajs[i];
			textTrajs[trajNum].set_ID(trajNum);
		}
	}
	set_mean_frm_num();
}

int CTextTrajMan::get_text_num_on_frame(int frmNum)
{
	int textNum=0;
	int i;
	int startFrm, endFrm;
	for (i=1; i<=trajNum; i++)
	{
		startFrm=textTrajs[i].get_start_frm_num();
		endFrm=textTrajs[i].get_end_frm_num();
		if ((startFrm<=frmNum)&&(endFrm>=frmNum))
			textNum++;
	}
	return textNum;
}


void text_detect(PIXEL *** colorData, int Height, int Width, 
				 CTextFrame * curTextFrame, int iFrameIndex)
{
	curTextFrame->clear();
	int nrLines = 0;
	int i;
	STextModel textModel;

	Line *line;
	FILE * fLogPtr;
	FrameFile *frame;
	int lineAccepted = 0;//find out how many (if at all any) lines were actually accepted laa 1/20/99

	unsigned char *OrigImage; //laa 1/20/99 uint->uchar
	
	frame = new FrameFile(colorData[0], Height, Width);
		
    frame->sharpen();

	int edgeCount[7] = {0}; //laa 1/19/98
    frame->edgeDetect(edgeCount); //edge filtering...
    // object detection
//	fLogPtr=OpenFileNameMode("c:\\temp\\textExt.txt", "w+");
//    nrLines = frame->findAreas(fLogPtr);
    nrLines = frame->findAreas();
//    fprintf(fLogPtr, "Found %d areas\n\n", nrLines);
    int threshSubArea = (Height * Width)/3 * .10; 
	//10% of the sub area is the edge tolerance thresh laa 1/11/99
    for (int lineIndex = 0; lineIndex < nrLines; lineIndex++)
	{

		line = frame->getLineAt(lineIndex);
		if (line->ACCEPT)
		{
			int chars = line->getWidth()/line->getHeight(); //laa additional heuristic to find the nr of chars
			if
					 ( 
						(
						 (line->getNrChar() > MIN_NR_CHAR_THRESHOLD) || 
						 (
						  (chars > MIN_NR_CHAR_THRESHOLD+1) && 
							(line->getHeight() > 15)
						 )
					  )
						&&
					  (
					   (line->getWidth()  > 30)   &&
					   (line->getHeight() > 10)  &&
					   (line->getHeight() < 70) 

					  )

					 )
			{
				textModel.frmNum=iFrameIndex;
				textModel.cenRow=line->getCentreY();
				textModel.cenCol=line->getCentreX();
				textModel.height=line->getHeight();
				textModel.width=line->getWidth();
				textModel.valid=false;
				curTextFrame->add_model(textModel);
				lineAccepted++;
			}
		}
	}
		frame->cleanFrame(nrLines);

}

void text_detect_2D(PIXEL ** imgData, int Height, int Width, 
					CTextFrame * curTextFrame, int iFrameIndex)
{
	curTextFrame->clear();
	int nrLines = 0;
	int i;
	STextModel textModel;

	Line *line;
	FILE * fLogPtr;
	FrameFile *frame;
	int lineAccepted = 0;//find out how many (if at all any) lines were actually accepted laa 1/20/99

	unsigned char *OrigImage; //laa 1/20/99 uint->uchar
	
	frame = new FrameFile(imgData, Height, Width);
		
    frame->sharpen();

	int edgeCount[7] = {0}; //laa 1/19/98
    frame->edgeDetect(edgeCount); //edge filtering...
    // object detection
//	fLogPtr=OpenFileNameMode("c:\\temp\\textExt.txt", "w+");
//    nrLines = frame->findAreas(fLogPtr);
    nrLines = frame->findAreas();
//    fprintf(fLogPtr, "Found %d areas\n\n", nrLines);
    int threshSubArea = (Height * Width)/3 * .10; 
	//10% of the sub area is the edge tolerance thresh laa 1/11/99
    for (int lineIndex = 0; lineIndex < nrLines; lineIndex++)
	{

		line = frame->getLineAt(lineIndex);
		if (line->ACCEPT)
		{
			int chars = line->getWidth()/line->getHeight(); //laa additional heuristic to find the nr of chars
			if
					 ( 
						(
						 (line->getNrChar() > MIN_NR_CHAR_THRESHOLD) || 
						 (
						  (chars > MIN_NR_CHAR_THRESHOLD+1) && 
							(line->getHeight() > 15)
						 )
					  )
						&&
					  (
					   (line->getWidth()  > 30)   &&
					   (line->getHeight() > 10)  &&
					   (line->getHeight() < 70) 

					  )

					 )
			{
				textModel.frmNum=iFrameIndex;
				textModel.cenRow=line->getCentreY();
				textModel.cenCol=line->getCentreX();
				textModel.height=line->getHeight();
				textModel.width=line->getWidth();
				textModel.valid=false;
				curTextFrame->add_model(textModel);
				lineAccepted++;
			}
		}
	}
		frame->cleanFrame(nrLines);

}

void text_track(PIXEL *** colorData, int Height, int Width, 
				 CTextModelMan &textModelMan, CTextTrajMan &textTrajMan, 
				 int preFrmNum, int iFrameIndex)
{

	int nrLines = 0;
	int i;
	STextModel textModel;

	Line *line;
	FILE * fLogPtr;
	FrameFile *frame;
	char sTextReportFrame[200];
	int lineAccepted = 0;//find out how many (if at all any) lines were actually accepted laa 1/20/99

	unsigned char *OrigImage; //laa 1/20/99 uint->uchar
	
	frame = new FrameFile(colorData[0], Height, Width);
		
    frame->sharpen();

	int edgeCount[7] = {0}; //laa 1/19/98
    frame->edgeDetect(edgeCount); //edge filtering...
    // object detection
//	fLogPtr=OpenFileNameMode("c:\\temp\\textExt.txt", "w+");
//    nrLines = frame->findAreas(fLogPtr);
    nrLines = frame->findAreas();
//    fprintf(fLogPtr, "Found %d areas\n\n", nrLines);
    int threshSubArea = (Height * Width)/3 * .10; 
	//10% of the sub area is the edge tolerance thresh laa 1/11/99
    for (int lineIndex = 0; lineIndex < nrLines; lineIndex++)
	{

		line = frame->getLineAt(lineIndex);
		if (line->ACCEPT)
		{
			int chars = line->getWidth()/line->getHeight(); //laa additional heuristic to find the nr of chars
			if
					 ( 
						(
						 (line->getNrChar() > MIN_NR_CHAR_THRESHOLD) || 
						 (
						  (chars > MIN_NR_CHAR_THRESHOLD+1) && 
							(line->getHeight() > 15)
						 )
					  )
						&&
					  (
					   (line->getWidth()  > 30)   &&
					   (line->getHeight() > 10)  &&
					   (line->getHeight() < 70) 

					  )

					 )
			{
				textModel.frmNum=iFrameIndex;
				textModel.cenRow=line->getCentreY();
				textModel.cenCol=line->getCentreX();
				textModel.height=line->getHeight();
				textModel.width=line->getWidth();
				textModelMan.new_model(textModel);
				textTrajMan.add_model(textModel, preFrmNum);
				lineAccepted++;
			}
		}
	}
		frame->cleanFrame(nrLines);

}


FILE * OpenFileNameMode(char *FileName, const char *mode)
{
   FILE * fp;
     
   fp=fopen(FileName,mode);
   if (fp==NULL)
   {
	  fprintf(stderr, " FILE OPEN failed!");
      exit(1);
   }
   return fp;
}

PIXEL ** New2D(int RSize, int CSize)
{
   PIXEL ** Data;   
   int i, j;
   Data=new PIXEL *[RSize];
   if (Data==NULL)
   {
       cout<<"Heap Allocation Error !! \n";
       exit(1);
   }
   for (i=0;i<RSize;i++)
   {
      Data[i]=new PIXEL [CSize];
      if (Data[i]==NULL)
      {
          cout<<"Heap Allocation Error !! \n";
          exit(1);
      }
   }
   for (i=0; i<RSize; i++)
	   for (j=0; j<CSize; j++)
		   Data[i][j]=0;
   
   return Data;
} 
 
void one_D_to_two_D(PIXEL ** Data, int Height, int Width, 
					unsigned char * pnOutBuf, int picHeight, int picWidth)
{
	int heightRatio=Height/picHeight;
	int widthRatio=Width/picWidth;

	int i, j;

	if ((heightRatio==1)&&(widthRatio==1))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
				Data[i][j]=pnOutBuf[i*picWidth+j];
	if ((heightRatio==1)&&(widthRatio==2))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
			{
				Data[i][2*j]=pnOutBuf[i*picWidth+j];
				Data[i][2*j+1]=pnOutBuf[i*picWidth+j];
			}
	if ((heightRatio==2)&&(widthRatio==1))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
			{
				Data[2*i][j]=pnOutBuf[i*picWidth+j];
				Data[2*i+1][j]=pnOutBuf[i*picWidth+j];
			}
	if ((heightRatio==2)&&(widthRatio==2))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
			{
				Data[2*i][2*j]=pnOutBuf[i*picWidth+j];
				Data[2*i][2*j+1]=pnOutBuf[i*picWidth+j];
				Data[2*i+1][2*j]=pnOutBuf[i*picWidth+j];
				Data[2*i+1][2*j+1]=pnOutBuf[i*picWidth+j];
			}

}

void short_one_D_to_two_D(PIXEL ** Data, int Height, int Width, 
						  short * pnOutBuf, int picWidth, int picHeight)
{
	int heightRatio=Height/picHeight;
	int widthRatio=Width/picWidth;

	int i, j;

	if ((heightRatio==1)&&(widthRatio==1))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
				Data[i][j]=pnOutBuf[i*picWidth+j];
	if ((heightRatio==1)&&(widthRatio==2))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
			{
				Data[i][2*j]=pnOutBuf[i*picWidth+j];
				Data[i][2*j+1]=pnOutBuf[i*picWidth+j];
			}
	if ((heightRatio==2)&&(widthRatio==1))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
			{
				Data[2*i][j]=pnOutBuf[i*picWidth+j];
				Data[2*i+1][j]=pnOutBuf[i*picWidth+j];
			}
	if ((heightRatio==2)&&(widthRatio==2))
		for (i=0; i<picHeight; i++)
			for (j=0; j<picWidth; j++)
			{
				Data[2*i][2*j]=pnOutBuf[i*picWidth+j];
				Data[2*i][2*j+1]=pnOutBuf[i*picWidth+j];
				Data[2*i+1][2*j]=pnOutBuf[i*picWidth+j];
				Data[2*i+1][2*j+1]=pnOutBuf[i*picWidth+j];
			}

}

void Delete2D(PIXEL **Data, int RowSize) 
{
   for (int i=0;i<RowSize;i++)
   {
      delete [] Data[i];
   }
   delete [] Data;
}

BYTE ** byteNew2D(int RSize, int CSize)
{
   BYTE ** Data;   
   int i, j;
   Data=new BYTE *[RSize];
   if (Data==NULL)
   {
       cout<<"Heap Allocation Error !! \n";
       exit(1);
   }
   for (i=0;i<RSize;i++)
   {
      Data[i]=new BYTE[CSize];
      if (Data[i]==NULL)
      {
          cout<<"Heap Allocation Error !! \n";
          exit(1);
      }
   }
   for (i=0; i<RSize; i++)
	   for (j=0; j<CSize; j++)
		   Data[i][j]=0;
   
   return Data;
}  

void byteDelete2D(BYTE **Data, int RowSize) 
{
   for (int i=0;i<RowSize;i++)
   {
      delete [] Data[i];
   }
   delete [] Data;
}

void write_image_data(PIXEL **Data, int RSize, int CSize, FILE *fp)
{
  int Row;
  int i, j;
  BYTE ** byteData=byteNew2D(RSize, CSize);
  for (i=0; i<RSize; i++)
	  for (j=0; j<CSize; j++)
		  byteData[i][j]=(BYTE)Data[i][j];
 
  for ( Row=0;Row<RSize;Row++)
  {
     if (fwrite(byteData[Row],sizeof(BYTE),CSize,fp)!=CSize )
      {
	 cout<<"File Write Error !! \n";
	 exit(1);
       }
   }
  byteDelete2D(byteData, RSize);
}

void write_raw_image(PIXEL **Data, int Height, int Width, char *FileName)
{
	FILE *fp=OpenFileNameMode(FileName, "wb");
	write_image_data(Data, Height, Width, fp);
	fclose(fp);
}
