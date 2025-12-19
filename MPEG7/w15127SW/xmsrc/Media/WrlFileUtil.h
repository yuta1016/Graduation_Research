//sth must be integrated into MediaIO and/or MultiMedia
//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, Marius Preda, Titus Zaharia
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
// Applicable File Name:  WrlFileUtil.h
//
#include "Media/Media.h"
#include "stdio.h"

#ifndef __WRLFILEUTIL_H__
#define __WRLFILEUTIL_H__

void TestforTrash(FILE *fp);
int PassToNumber(FILE *fp);
int PassToNumberOrPar(FILE *fp);
int GetInt(FILE *fp); 
float GetFloat(FILE *fp);

int GetObjectNo(FILE *wrlFile);
int SearchObject(FILE *wrlFile,int *nr_points, int *nr_triangles,long *place_points, long *place_triangles,long *place_points_end, long *place_triangles_end);


int Load3dObject(FILE *wrlFile, MediaThreeD *media3d);
int WriteVrml2(FILE *temp, MediaThreeD *media3d);

MediaThreeD * Load3DMesh(char *name); 

int ObjAlloc(MediaThreeD *mesh, int np, int nt);
int ObjFree(MediaThreeD *mesh, int np, int nt); 
int free_media3D(MediaThreeD *mesh); 

#endif //__WRLFILEUTIL_H__
