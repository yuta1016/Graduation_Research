//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
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
// Applicable File Name:  MultiMedia.cpp
//

#include <stdio.h>
#include <string.h>
#include "momusys.h"
#include "vopio.h"
#include "Utilities/LinkedList.h"
#include "Media/Media.h"

extern "C"
{
#include "Decoders/MPEG2Dec/src/mpeg2dec/mpeg2dec.h"
}

//=============================================================================
	

using namespace XM;


const UUID MultiMediaInterface::myID = UUID("");
const char *MultiMediaInterface::myName = "MultiMedia Interface";

const UUID MultiMedia::myID = UUID("");
const char *MultiMedia::myName = "MultiMedia Data";

const UUID MultiMedia::valID = UUID("");
const char *MultiMedia::valName = "MultiMedia Data";

const UUID MultiMediaInterfaceABC::myID = UUID();


static TnDKeyPoint _ZERO_KEY_POINT={0,0,0};

//=============================================================================
MultiMediaInterfaceABC::MultiMediaInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MultiMediaInterface::MultiMediaInterface(MultiMedia* aMultiMedia):
m_MultiMedia(aMultiMedia)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiMediaInterface::~MultiMediaInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MultiMediaInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiMediaInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MultiMediaInterface::destroy(void)
{ delete m_MultiMedia; }

//----------------------------------------------------------------------------
CAudioFrame *MultiMediaInterface::GetAudioFrame(void)
{
  return m_MultiMedia->GetAudioFrame();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SetAudioFrame(CAudioFrame af)
{
  return m_MultiMedia->SetAudioFrame(af);
}

// video media
//----------------------------------------------------------------------------
char *MultiMediaInterface::GetVideoFileName(void)
{
  return m_MultiMedia->GetVideoFileName();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SetVideoFileName(char *aFileName)
{
  return m_MultiMedia->SetVideoFileName(aFileName);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::OpenVideoFile()
{
	return m_MultiMedia->OpenVideoFile();
}

//----------------------------------------------------------------------------
MomVop *MultiMediaInterface::LoadNextVideoFrame()
{
	return m_MultiMedia->LoadNextVideoFrame();
}

//----------------------------------------------------------------------------
void MultiMediaInterface::CloseVideoFile()
{
	m_MultiMedia->CloseVideoFile();
}

//----------------------------------------------------------------------------
double MultiMediaInterface::GetVideoFrameRate()
{
	return m_MultiMedia->GetVideoFrameRate();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::IsVideoInterlaced()
{
	return m_MultiMedia->IsVideoInterlaced();
}

// image media
//----------------------------------------------------------------------------
char *MultiMediaInterface::GetImageFileName(void)
{
  return m_MultiMedia->GetImageFileName();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SetImageFileName(char *aFileName)
{
  return m_MultiMedia->SetImageFileName(aFileName);
}

//----------------------------------------------------------------------------
char *MultiMediaInterface::GetImageComment(void)
{
  return m_MultiMedia->GetImageComment();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SetImageComment(char *aComment)
{
  return m_MultiMedia->SetImageComment(aComment);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::GetImageTime(void)
{
  return m_MultiMedia->GetImageTime();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SetImageTime(int aTime)
{
  return m_MultiMedia->SetImageTime(aTime);
}

//----------------------------------------------------------------------------
MomVop *MultiMediaInterface::GetImage(void)
{
  return m_MultiMedia->GetImage();
}

MomVop *MultiMediaInterface::GetSubImage(int x, int y, int w, int h)
{
  return m_MultiMedia->GetSubImage(x, y, w, h);
}


//----------------------------------------------------------------------------
int MultiMediaInterface::SetImage(MomVop *aVop)
{
  return m_MultiMedia->SetImage(aVop);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::FreeImage(void)
{
  return m_MultiMedia->FreeImage();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::GetMinAlpha(void)
{
  return m_MultiMedia->GetMinAlpha();
}

//----------------------------------------------------------------------------
void MultiMediaInterface::SetMinAlpha(int value)
{
  m_MultiMedia->SetMinAlpha(value);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::GetMaxAlpha(void)
{
  return m_MultiMedia->GetMaxAlpha();
}

//----------------------------------------------------------------------------
void MultiMediaInterface::SetMaxAlpha(int value)
{
  m_MultiMedia->SetMaxAlpha(value);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SelectVopAlphaLayer(int selectedlayer)
{
  return m_MultiMedia->SelectVopAlphaLayer(selectedlayer);
}

//----------------------------------------------------------------------------
char *MultiMediaInterface::GetKeyPointFileName(void)
{
  return m_MultiMedia->GetKeyPointFileName();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::SetKeyPointFileName(char *aFileName)
{
  return m_MultiMedia->SetKeyPointFileName(aFileName);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::GetKeyPointTime(void)
{
  return m_MultiMedia->GetKeyPointTime();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::GotoKeyPointList(int index)
{
  return m_MultiMedia->GotoKeyPointList(index);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::NextKeyPointList(void)
{
  return m_MultiMedia->NextKeyPointList();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::AddKeyPointsList(int aTime)
{
  return m_MultiMedia->AddKeyPointsList(aTime);
}

//----------------------------------------------------------------------------
TnDKeyPoint *MultiMediaInterface::GetKeyPoint(void)
{
  return m_MultiMedia->GetKeyPoint();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::GotoKeyPoint(int Index)
{
  return m_MultiMedia->GotoKeyPoint(Index);
}

//sth to be removed
//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPointNo in multimedia class
int MultiMediaInterface::SetKeyPointNumber(int Number)
{
  return m_MultiMedia->SetKeyPointNumber(Number);
}

//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 23, to get m_KeyPointNo in multimedia class
int MultiMediaInterface::GetKeyPointNumber(void)
{
  return m_MultiMedia->GetKeyPointNumber();
}

//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPoint in multimedia class
int MultiMediaInterface::SetKeyPointPointer(T4DKeyPoint *KeyPointPointer)
{
  return m_MultiMedia->SetKeyPointPointer(KeyPointPointer);
}

//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 22, to get m_KeyPoint in multimedia class
T4DKeyPoint *MultiMediaInterface::GetKeyPointPointer(void)
{
  return m_MultiMedia->GetKeyPointPointer();
}

//----------------------------------------------------------------------------
int MultiMediaInterface::NextKeyPoint(void)
{
  return m_MultiMedia->NextKeyPoint();
}
;
//----------------------------------------------------------------------------
int MultiMediaInterface::AddKeyPoint(TnDKeyPoint *aKeyPoint)
{
  return m_MultiMedia->AddKeyPoint(aKeyPoint);
}

//----------------------------------------------------------------------------
int MultiMediaInterface::FreeKeyPointLists()

{
  return m_MultiMedia->FreeKeyPointLists();
}

//=============================================================================
MultiMedia::MultiMedia():
m_Interface(this),
m_isProprietary(0), //added by Sylvie: error compilation if not
m_Time(0),
m_VideoFileName(0),
m_ImageFileName(0),
m_ImageComment(0),
m_Image(0),
m_OrigAlpha(0),
m_MinAlpha(255),
m_MaxAlpha(0),
m_KeyPointFileName(0),
m_KeyPointLists(0),
m_KeyPointListNo(0),
m_KeyPoints(0),
m_KeyPoint(0),
m_KeyPointNo(0),
m_oldKeyPoint(0),
m_oldKeyPointNo(0)

{
  m_KeyPointLists=linkedListCreate();
  linkedListInit(m_KeyPointLists);
  m_KeyPoints=0;
  m_KeyPointListNo=-1;

  m_pAudioFrame = &m_AudioFrame;

  //  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MultiMedia::~MultiMedia()
{
  linkedListDestroy(m_KeyPointLists,&DeleteKeyPointList);
  if (m_oldKeyPoint) delete [] m_oldKeyPoint;

  if (m_Image) freevop(m_Image);
  if (m_OrigAlpha) freeimg(m_OrigAlpha);

  if (m_VideoFileName) delete [] m_VideoFileName;
  if (m_ImageFileName) delete [] m_ImageFileName;
  if (m_ImageComment) delete [] m_ImageComment;
  if (m_KeyPointFileName) delete [] m_KeyPointFileName;
}

//----------------------------------------------------------------------------
const UUID& MultiMedia::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiMedia::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MultiMedia::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MultiMedia::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MultiMedia::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
CAudioFrame * MultiMedia::GetAudioFrame(void)
{
  return m_pAudioFrame;
}

//----------------------------------------------------------------------------
int MultiMedia::SetAudioFrame(CAudioFrame af)
{
  m_AudioFrame = af;
  return 1;
}

//----------------------------------------------------------------------------
char *MultiMedia::GetVideoFileName(void)
{
  return m_VideoFileName;
}

//----------------------------------------------------------------------------
int MultiMedia::SetVideoFileName(char *aFileName)
{
  if (m_VideoFileName) delete [] m_VideoFileName;
  m_VideoFileName= new char [strlen(aFileName)+1];

  if (!m_VideoFileName) return 1;

  strcpy(m_VideoFileName,aFileName);

  return 0;
}

int MultiMedia::OpenVideoFile()
{
	if(Open_MPEG_Video((unsigned char*)m_VideoFileName)==0) return 1;
	return 0;
}

MomVop *MultiMedia::LoadNextVideoFrame()
{
	unsigned char *	pixels;
	
	unsigned int xsize,ysize, xstride, totalpix;
	int i, j;
	MomVop *vop;
	
	/* free old media before replacing with new meda*/
	MomVop *oldvop=GetImage();
	if (oldvop) {
		FreeImage();
	}


	MomImageType buf_type;
	MomImageData r,g,b;

	pixels = Get_Next_MPEG_frame();
	if(!pixels) return 0;

		
	/* allocate MomVop buffers*/
	xsize=Get_MPEG_width();
	ysize=Get_MPEG_height();
	xstride = Get_MPEG_stride();
	totalpix=xsize*ysize;
	
	buf_type=UCHAR_TYPE;

	// greylevel image
	vop=initvop(xsize,ysize,_400_,buf_type,buf_type,buf_type,buf_type);
	//vop=initvop(xsize,ysize,_444_,buf_type,buf_type,buf_type,buf_type);

	if (!vop) {
		fprintf(stderr,"\n");
		return 0;
	}
	
	/* copy data from video buffer to Vop*/
	if (vop->y_chan) g.u=vop->y_chan->data->u;
	else g.u=0;
	if (vop->u_chan) b.u=vop->u_chan->data->u;
	else b.u=0;
	if (vop->v_chan) r.u=vop->v_chan->data->u;
	else r.u=0;
	
	unsigned char *pixptr;
	for (i=0; i < ysize; i++) {
		pixptr = pixels+(i*xstride);
		for(j=0; j< xsize; j++)
		{
			if (g.u) {
				*g.u = *pixptr;
				g.u++;
				pixptr++;
			}
			/*if (b.u) {
				*b.u = *pixptr;
				b.u++;
			}*/
			/*if (r.u) {
				*r.u = *pixptr;
				r.u++;
			}*/		
		}
	}		
					
	/* put filename*/
	//SetImageFileName(imagefilename);

	SetImage(vop);			
	return vop;
}

void MultiMedia::CloseVideoFile()
{
	Close_MPEG_Video();
}

double MultiMedia::GetVideoFrameRate()
{
	return Get_MPEG_framerate();
}

int MultiMedia::IsVideoInterlaced()
{
	return Get_MPEG_is_interlaced();
}

// image media
//----------------------------------------------------------------------------
char *MultiMedia::GetImageFileName(void)
{
  return m_ImageFileName;
}


//----------------------------------------------------------------------------
int MultiMedia::SetImageFileName(char *aFileName)
{
  if (!aFileName) return 1;

  if (m_ImageFileName) delete [] m_ImageFileName;
  m_ImageFileName= new char [strlen(aFileName)+1];

  if (!m_ImageFileName) return 1;

  strcpy(m_ImageFileName,aFileName);

  return 0;
}

//----------------------------------------------------------------------------
char *MultiMedia::GetImageComment(void)
{
  return m_ImageComment;
}

//----------------------------------------------------------------------------
int MultiMedia::SetImageComment(char *aFileName)
{
  if (!aFileName) return 1;
 

  if (m_ImageComment) delete [] m_ImageComment;
  m_ImageComment= new char [strlen(aFileName)+1];

  if (!m_ImageComment) return 1;

  strcpy(m_ImageComment,aFileName);

  return 0;
}

//----------------------------------------------------------------------------
int MultiMedia::GetImageTime(void)
{
  return m_Time;
}

//----------------------------------------------------------------------------
int MultiMedia::SetImageTime(int aTime)
{
  m_Time=aTime;
  return 0;
}

//----------------------------------------------------------------------------
MomVop *MultiMedia::GetImage(void)
{
  return m_Image;
}

MomVop *MultiMedia::GetSubImage(int x, int y, int w, int h)
{
	if(!m_Image)
		return 0;
	MomImageType buf_type = m_Image->y_chan->type;
	MomImageData r,g,b,a;
	int sr,sg,sb,sa;        // size of pixel for the buffers

	int img_width = m_Image->width;
	int img_height = m_Image->height;
	if((w+x) > img_width) w = img_width-x;
	if((h+y) > img_height) h = img_height-y;

	MomVop *vop = initvop(w, h, _444_, buf_type, buf_type, buf_type, buf_type);

	int i, j;

	sr=datasize(vop->v_chan);
	sg=datasize(vop->y_chan);
	sb=datasize(vop->u_chan);
	sa=datasize(vop->a_chan);

	if(!m_Image->a_chan)
	{
		freeimg(vop->a_chan);
		vop->a_chan = 0;
	}

	for(i = y; i < (y+h); i++)
	{
		int src_offset = i*sr*img_width;
		int dst_offset = (i-y)*sr*w;

		if (vop->v_chan && m_Image->v_chan)
		{
			memcpy(vop->v_chan->data->u + dst_offset, m_Image->v_chan->data->u + src_offset, w*sr);
		}

		src_offset = i*sg*img_width;
		dst_offset = (i-y)*sg*w;
		if (vop->y_chan && m_Image->y_chan)
		{
			memcpy(vop->y_chan->data->u + dst_offset, m_Image->y_chan->data->u + src_offset, w*sg);
		}

		src_offset = i*sb*img_width;
		dst_offset = (i-y)*sb*w;
		if (vop->u_chan && m_Image->u_chan)
		{
			memcpy(vop->u_chan->data->u + dst_offset, m_Image->u_chan->data->u + src_offset, w*sb);
		}

		src_offset = i*sa*img_width;
		dst_offset = (i-y)*sa*w;
		if(vop->a_chan && m_Image->a_chan)
		{
			memcpy(vop->a_chan->data->u + dst_offset, m_Image->a_chan->data->u + src_offset, w*sa);
		}
	}
	return vop;
}

//----------------------------------------------------------------------------
int MultiMedia::SetImage(MomVop *aVop)
{
//   if (m_Image) freevop(m_Image);
//   m_Image=0;
  if (m_OrigAlpha) freeimg(m_OrigAlpha);
  m_OrigAlpha=0;
  
//  if (!aVop) return 1;

  m_Image=aVop;
  return 0;
}

//----------------------------------------------------------------------------
int MultiMedia::FreeImage(void)
{
  if (!m_Image) return 0;
  freevop(m_Image);
  m_Image=0;
  if (m_OrigAlpha) freeimg(m_OrigAlpha);
  m_OrigAlpha=0;
  return 0;
}

//----------------------------------------------------------------------------
int MultiMedia::GetMinAlpha(void)
{
  return m_MinAlpha;
}

//----------------------------------------------------------------------------
void MultiMedia::SetMinAlpha(int value)
{
  m_MinAlpha=value;
}

//----------------------------------------------------------------------------
int MultiMedia::GetMaxAlpha(void)
{
  return m_MaxAlpha;
}

//----------------------------------------------------------------------------
void MultiMedia::SetMaxAlpha(int value)
{
  m_MaxAlpha=value;
}

//----------------------------------------------------------------------------
int MultiMedia::SelectVopAlphaLayer(int selectedlayer)

{
  if (!m_OrigAlpha) {
    m_OrigAlpha=m_Image->a_chan;
    if (!m_OrigAlpha) {
      m_OrigAlpha=initimg(m_Image->width,m_Image->height,UCHAR_TYPE);
      memset(m_OrigAlpha->data->u,0,m_Image->width*m_Image->height);
    }
    m_Image->a_chan=initimg(m_Image->width,m_Image->height,UCHAR_TYPE);
  }

  if (!m_Image->a_chan || !m_Image->a_chan->data ||
      !m_Image->a_chan->data->u) {
    return 1;
  }

  if (!m_Image->width || !m_Image->height) {
    return 1;
  }

  switch (selectedlayer) {
  case BackgroundFromBinary:
    _intra_regin1 = m_MinAlpha;
    _intra_regin2 = 0;
    _intra_regin3 = 255;
    _intra_regin4 = 0;
    break;
  case BackgroundFromAlpha:
    if (m_MinAlpha==255) { /* no valid mask or all background*/
      _intra_regin1 = 256;
    }
    else {
    _intra_regin1 = 128;
    }
    _intra_regin2 = 0;
    _intra_regin3 = 255;
    _intra_regin4 = -1;
    break;
  case AllForegroundFromBinary:
    _intra_regin1 = m_MinAlpha;
    _intra_regin2 = 255;
    _intra_regin3 = 0;
    _intra_regin4 = 0;
    break;
  case ForegroundFromAlpha:
    if (m_MinAlpha==255) { /* no valid mask or all background*/
      _intra_regin1 = 256;
    }
    else {
    _intra_regin1 = 128;
    }
    _intra_regin2 = 0;
    _intra_regin3 = 255;
    _intra_regin4 = 1;
    break;
  default:
    _intra_regin1 = selectedlayer;
    _intra_regin2 = 0;
    _intra_regin3 = 255;
    _intra_regin4 = 0;
    break;
  }

  intra_proc(m_Image,0,m_OrigAlpha,
	     A_CH,AX_CH,CON_0,SCAN_HO,
	     -1,-1,-1,-1,-1,
	     -1,-1,0,0,0,0,
	     size1to1,&extractalpha_pix);
 
  return 0;
}

//---------------------------------------------------------------------------
void extractalpha_pix(TPixel *res,TPixel *in, TCoor x, TCoor y)

{

  res->ax=in[12].ax;
  res->y.i=in[12].y.i;
  res->u.i=in[12].u.i;
  res->v.i=in[12].v.i;

  if (((_intra_regin4 == 0) && (in[12].ax.i==_intra_regin1)) || 
      ((_intra_regin4 == -1) && (in[12].ax.i<_intra_regin1)) ||
      ((_intra_regin4 == 1) && (in[12].ax.i>=_intra_regin1))) {
    res->a.i=_intra_regin3;
  }
  else {
    res->a.i=_intra_regin2;
  }
}

//key points
//----------------------------------------------------------------------------
char *MultiMedia::GetKeyPointFileName(void)
{
  return m_KeyPointFileName;
}

//----------------------------------------------------------------------------
int MultiMedia::SetKeyPointFileName(char *aFileName)
{
  if (!aFileName) return 1;

  if (m_KeyPointFileName) delete [] m_KeyPointFileName;
  m_KeyPointFileName= new char [strlen(aFileName)+1];

  if (!m_KeyPointFileName) return 1;

  strcpy(m_KeyPointFileName,aFileName);

  return 0;
}

//----------------------------------------------------------------------------
int MultiMedia::GetKeyPointTime(void)
{
  if (!m_KeyPoints) return 0;
  return m_KeyPoints->Time;
}

//----------------------------------------------------------------------------
int MultiMedia::GotoKeyPointList(int Index)
{
  m_KeyPoint=0;
  m_KeyPointNo=-1;

  if (!m_KeyPointLists) return 1;
  if (Index > linkedListCountElement(m_KeyPointLists)) {
    m_KeyPoints=0;
    m_KeyPointListNo=-1;
    return 2;
  }

  m_KeyPoints=(KeyPointList *)linkedListGetObject(m_KeyPointLists,Index);
  m_KeyPointListNo=Index;
  if (!m_KeyPoints) return 3;


  return 0;
}

//----------------------------------------------------------------------------
int MultiMedia::NextKeyPointList(void)
{
  return GotoKeyPointList(m_KeyPointListNo+1);
}

//----------------------------------------------------------------------------
int MultiMedia::AddKeyPointsList(int aTime)
{
  int i,nooflists;

  m_KeyPoint=0;
  m_KeyPointNo=-1;

  /* search if key point list already exists*/
  nooflists=linkedListCountElement(m_KeyPointLists);
  i=0;
  while (GotoKeyPointList(i)==0) {
    if (m_KeyPoints->Time == aTime) {
      m_KeyPointListNo=i;
      return 0;
    }
    i++;
  }

  /* not found => create new list*/
  m_KeyPoints= new KeyPointList;
  if (!m_KeyPoints) {
    m_KeyPointListNo=-1;
    return 1;
  }

  m_KeyPointListNo=i;
  m_KeyPoints->Time=aTime;
  m_KeyPoints->KeyPoints=linkedListCreate();
  if (!m_KeyPoints->KeyPoints) return 1;
  linkedListInit(m_KeyPoints->KeyPoints);

  /* add it to list of lists*/
  linkedListAddLast(m_KeyPointLists,(void *)m_KeyPoints);
  
  return 0;
}

//----------------------------------------------------------------------------
TnDKeyPoint *MultiMedia::GetKeyPoint(void)
{
  if (!m_KeyPoints) return &_ZERO_KEY_POINT;
  if (!m_KeyPoints->KeyPoints) return &_ZERO_KEY_POINT;

  if (!m_KeyPoint) return &_ZERO_KEY_POINT;

  return m_KeyPoint;
}

//----------------------------------------------------------------------------
int MultiMedia::GotoKeyPoint(int Index)
{
  if (!m_KeyPoints) return 1;
  if (!m_KeyPoints->KeyPoints) return 1;

  if (Index > linkedListCountElement(m_KeyPoints->KeyPoints)) {
    m_KeyPoint=0;
    m_KeyPointNo=-1;
    return 2;
  }

  m_KeyPoint=(TnDKeyPoint *)linkedListGetObject(m_KeyPoints->KeyPoints,Index);
  if (!m_KeyPoint) {
    return 3;
  }

  return 0;
}


//sth to be removed
//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPointNo in multimedia class
int MultiMedia::SetKeyPointNumber(int Number)
{
  m_oldKeyPointNo = Number;
  return 0;
}

//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 23, to fill m_KeyPointNo in multimedia class
int MultiMedia::GetKeyPointNumber(void)
{
  return m_oldKeyPointNo;
}

//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPoint in multimedia class
int MultiMedia::SetKeyPointPointer(T4DKeyPoint *KeyPointPointer)
{
  m_oldKeyPoint = KeyPointPointer;
  return 0;
}

//----------------------------------------------------------------------------
//--- added by Sylvie Jeannin, August 22, to get m_KeyPoint in multimedia class
//sth is identical to GetKeyPoint
T4DKeyPoint *MultiMedia::GetKeyPointPointer(void)
{
  return m_oldKeyPoint;
}

//----------------------------------------------------------------------------
int MultiMedia::NextKeyPoint(void)
{
  return GotoKeyPoint(m_KeyPointNo+1);
}

//----------------------------------------------------------------------------
int MultiMedia::AddKeyPoint(TnDKeyPoint *aKeyPoint)
{
  if (!m_KeyPoints) return 1;
  if (!m_KeyPoints->KeyPoints) return 1;

  m_KeyPoint = new TnDKeyPoint;
  if (!m_KeyPoint) {
    m_KeyPointNo=-1;
    return 3;
  }

  /* copy values*/
  m_KeyPoint->time=aKeyPoint->time;
  m_KeyPoint->dim=aKeyPoint->dim;

  if (m_KeyPoint->dim) {
    m_KeyPoint->p=new float [m_KeyPoint->dim];
    if (m_KeyPoint->p) {
      int i;
      for (i=0; i<m_KeyPoint->dim; i++) {
	m_KeyPoint->p[i]=aKeyPoint->p[i];
      }
    }
    else {
      m_KeyPoint->dim=0;
    }
  }
  else {
    m_KeyPoint->p=0;
  }

  /* add key point to list */
  linkedListAddLast(m_KeyPoints->KeyPoints,(void *)m_KeyPoint);
  m_KeyPoint=aKeyPoint;

  return 0;
}

//----------------------------------------------------------------------------
int MultiMedia::FreeKeyPointLists()

{
  /* remove old list of lists*/
  linkedListDestroy(m_KeyPointLists,&DeleteKeyPointList);

  /* create new list of lists*/
  m_KeyPointLists=linkedListCreate();
  linkedListInit(m_KeyPointLists);
  m_KeyPoints=0;
  m_KeyPointListNo=-1;

  return 0;
}

//----------------------------------------------------------------------------
MultiMediaInterfaceABC *MultiMedia::GetInterface(void)
{ return &m_Interface; }

//============================================================================
void DeleteKeyPointList(void *KeyPoints,void *unused)
{
  linkedListDestroy(((KeyPointList *)KeyPoints)->KeyPoints,&DeleteKeyPoint);
  delete (KeyPointList *)KeyPoints;
}

//----------------------------------------------------------------------------
void DeleteKeyPoint(void *KeyPoint,void *unused)
{
  if (((TnDKeyPoint*)(KeyPoint))->dim) {
    delete [] ((TnDKeyPoint*)(KeyPoint))->p;
  }
  delete ((TnDKeyPoint*)KeyPoint);
}
