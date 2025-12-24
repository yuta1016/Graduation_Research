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
// Applicable File Name:  WrlFileUtil.cpp
//


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "Media/Media.h"

#define WRL_BUFFER_SIZE 256
#define DEBUG_WRL 0

//sth delimiters
void TestforTrash(FILE *fp)
{
  char  trash[WRL_BUFFER_SIZE];
  
  while(ungetc(getc(fp),fp) == '#')
    fgets(trash,WRL_BUFFER_SIZE,fp);
  
}

int PassToNumber(FILE *fp) 
{
  int c, tt;
  while(c = getc(fp),(!isdigit(c)) && (c!=EOF) && (c!='-') );
  tt = ungetc(c,fp);
  return tt;  
}

int PassToNumberOrPar(FILE *fp) 
{

  int c; char cc; 

  while(1)
    {  
      fscanf(fp, "%c", &cc);
      c = int(cc);    
      if ( (!isdigit(c)) && (c!=EOF) && (c!='-') && (c!=']') )
	continue;
      else
	break;
    }
    
  ungetc(c,fp);

  if (c==']') return 1;
  else return 0;
}

int GetInt(FILE *fp)
{
  int get=0;
  PassToNumber(fp);  
  fscanf(fp,"%d", &get);
  return get;
}

float GetFloat(FILE *fp)
{
  float get;
  PassToNumber(fp);
  fscanf(fp,"%f", &get);
  return get;
}





int GetObjectNo(FILE *wrlFile)
{
  int noObj=0;
  char geom[200];
  
  rewind(wrlFile);
  while (!feof(wrlFile)){
    fscanf(wrlFile,"%s",geom);
    if (strcmp(geom,"geometry") == 0) noObj++;
  }
  return noObj;
}

int SearchObject(FILE *wrlFile,int *nr_points, int *nr_triangles,long *place_points, long *place_triangles,long *place_points_end, long *place_triangles_end)
{
  char s_DEF[30], s_Segment[30], s_Test[30], s_Geometry[100],s_Form[30],s_Point[30],s_Par[30],s_CoordIndex[30];
  int flag=0, i=0;
  long a;
  int v1=0,ii;
  float p1;
  
  
  p1=0.0;
  sprintf(s_Geometry,"%c",'a');
  fscanf(wrlFile,"%s",s_Geometry);
  while(strcmp(s_Geometry,"geometry")!=0){
  
    fscanf(wrlFile,"%s",s_Geometry);
  }
 
  fscanf(wrlFile,"%s",s_Form);
  if (strncmp(s_Form,"IndexedFaceSet",14)==0){
    strcpy(s_Point," ");
    while(strncmp(s_Point,"point",5)!=0){
      fscanf(wrlFile,"%s",s_Point);
    }
   
    PassToNumber(wrlFile);
    i=0;
    *place_points=ftell(wrlFile);     
    
    while(!PassToNumberOrPar(wrlFile)){      
      i++;
      p1=GetFloat(wrlFile);    
      p1=GetFloat(wrlFile);    
      p1=GetFloat(wrlFile);     
  
    }
   
    *place_points_end=ftell(wrlFile);   
   
    *nr_points=i;
   

    strcpy(s_CoordIndex," ");
    while(strncmp(s_CoordIndex,"coordIndex",10)!=0){
      fscanf(wrlFile,"%s",s_CoordIndex);
    }
   
     
    *place_triangles=ftell(wrlFile);
    i=0;v1=1;
    v1=0;
    while(!PassToNumberOrPar(wrlFile)){
      ii=-1;
      while (v1!=-1){
	ii++;
	v1=GetInt(wrlFile);
      }
      v1=1;
      i+=ii-2;
    }
  
    *nr_triangles=i;
    *place_triangles_end=ftell(wrlFile);
  }
  else{
    printf("\n type of geometry not supported");
    return 0;
  }
  return (1);
}


int Load3dObject(FILE *wrlFile, MediaThreeD *media3d, long PlacePoints, long PlaceTriangl)
{  
  int i,v1,j,k;
  int varfuri[200];
  long a;
  char s_Par[100];
  int iPlacePoints = (int)PlacePoints; 
  int iPlaceTriangl = (int)PlaceTriangl; 
 
  fseek(wrlFile,iPlacePoints,SEEK_SET);
 
  for (i=0;i<media3d->NoPoints;i++){
    media3d->PObj[i][0] = GetFloat(wrlFile);
    media3d->PObj[i][1] = GetFloat(wrlFile);
    media3d->PObj[i][2] = GetFloat(wrlFile);
  }

  rewind(wrlFile);
  fseek(wrlFile,iPlaceTriangl,SEEK_SET);
  i=0;
   
  while(i<media3d->NoTriangl){    
    v1=0;j=0;strcpy(s_Par," ");
    while ((v1!=-1)&&(strcmp(s_Par,"]")!=0)){      
      v1=GetInt(wrlFile);      
      varfuri[j]=v1;     
      j++;
      a=ftell(wrlFile);
      fscanf(wrlFile,"%s",s_Par);
      fseek(wrlFile,a,SEEK_SET);
    }
    
    if ((v1!=-1)&&(strcmp(s_Par,"]")==0)) j++;
    for (k=0;k<j-3;k++){
      media3d->TObj[i][0] = varfuri[0];    
      media3d->TObj[i][1] = varfuri[k+1];
      media3d->TObj[i][2] = varfuri[k+2]; 
      if ((media3d->TObj[i][0]<0)||(media3d->TObj[i][1]<0)||(media3d->TObj[i][2]<0)) return 0; /* returns 0 if failure */ 
      i++;
    } 
  }

  return 1;
}


int WriteVrml2(FILE *temp, MediaThreeD *media3d)
{
  int i;
  int pos; 
  pos = ftell(temp); 
  if(pos == 0)
    fprintf(temp,"#VRML V2.0 utf8 (Converted to ASCII)\n");
  fprintf(temp,"Shape {\n");
  fprintf(temp,"	geometry IndexedFaceSet {\n");
  fprintf(temp,"		coord Coordinate {\n");
  fprintf(temp,"			point [\n");
  for (i=0;i<media3d->NoPoints;i++)
    fprintf(temp,"%f %f %f,\n",media3d->PObj[i][0],media3d->PObj[i][1],media3d->PObj[i][2]);
  fprintf(temp," ]\n");
  fprintf(temp," }\n");
  fprintf(temp,"                         coordIndex [\n");
  for (i=0;i<media3d->NoTriangl;i++)
    fprintf(temp,"%d, %d, %d, -1,\n",media3d->TObj[i][0],media3d->TObj[i][1],media3d->TObj[i][2]);
  fprintf(temp," ]\n");
  fprintf(temp," }\n");
  fprintf(temp," }\n"); 
  return 1;
}


MediaThreeD * Load3DMesh(char *name)
{
  MediaThreeD *mesh=0;
  MediaThreeD *mesh_t=0;

  int ob, np, nt, np_tot, nt_tot, contp, contt, k, po, to;
  long PlacePoints, PlaceTriangl, PlacePointsEnd, PlaceTrianglEnd;
  long pp,pt,ppe,pte;

  FILE *fwrl = fopen(name, "rb");
  if(fwrl == NULL) return 0; /*failure*/ 

  int noObj=GetObjectNo(fwrl); rewind(fwrl);

  mesh = new MediaThreeD[noObj];
  if(!mesh) return 0; 
  
  mesh_t = new MediaThreeD;
  if(!mesh_t) return 0; 
  
  np_tot = 0; nt_tot = 0;
 
  for (ob=0;ob<noObj;ob++){   
    SearchObject(fwrl,&np,&nt,&pp,&pt,&ppe,&pte);
    mesh[ob].NoPoints = np; mesh[ob].NoTriangl = nt; 
    
    if(!ObjAlloc(&mesh[ob], np, nt)) return 0;
   
    PlacePoints=pp;
    PlaceTriangl=pt;
    PlacePointsEnd=ppe;
    PlaceTrianglEnd=pte;    
    if(!Load3dObject(fwrl, &mesh[ob], PlacePoints, PlaceTriangl)) return 0; /*failure*/    
    
    np_tot += np; nt_tot += nt; 
  } 
  
  mesh_t->NoPoints = np_tot; mesh_t->NoTriangl = nt_tot;
  if(!ObjAlloc(mesh_t, np_tot, nt_tot)) return 0;

  contp = 0; contt = 0; 
  
  for (ob=0;ob<noObj;ob++)
    {      
      for(po=0;po<mesh[ob].NoPoints;po++)	
	for(k=0;k<3;k++)
	  mesh_t->PObj[contp+po][k] = mesh[ob].PObj[po][k];
            
      for(to=0;to<mesh[ob].NoTriangl;to++)
	for(k=0;k<3;k++)
	  mesh_t->TObj[contt+to][k] = mesh[ob].TObj[to][k] + contp;
	
      contp += mesh[ob].NoPoints; contt += mesh[ob].NoTriangl; 
      
      if(!ObjFree(&mesh[ob], mesh[ob].NoPoints, mesh[ob].NoTriangl)) return 0;
  }

  if(DEBUG_WRL)  
    {
      char namewrltest[300]; char *p;
      p = strstr(name, ".wrl"); unsigned int length = p-name; 
      strncpy(namewrltest, name, length); namewrltest[length] = '\0';
      strcat(namewrltest, "_T.wrl"); 
      fprintf(stderr, "namewrltest = %s\n", namewrltest);
      FILE *fwrlout = fopen(namewrltest, "wb");
      WriteVrml2(fwrlout, mesh_t);
      fclose(fwrlout); 
    }
  
  delete [] mesh;  
  fclose(fwrl);
  return mesh_t;
}


int ObjAlloc(MediaThreeD *mesh, int np, int nt)
{
  if(np != mesh->NoPoints || nt != mesh->NoTriangl) return 0;
  int i; 
  mesh->PObj = 0; 
  mesh->PObj = new float * [np];
  if(!mesh->PObj) return 0; 
  for(i=0;i<np;i++)
    {
      mesh->PObj[i] = 0; 
      mesh->PObj[i] = new float[3];
      if(!mesh->PObj[i]) return 0; 
    }
  
  mesh->TObj = 0; 
  mesh->TObj = new int * [nt];
  if(!mesh->TObj) return 0; 
  for(i=0;i<nt;i++)
    {
      mesh->TObj[i] = 0;
      mesh->TObj[i] = new int[3];
      if(!mesh->TObj) return 0; 
    }
  
  return 1; 
}


int ObjFree(MediaThreeD *mesh, int np, int nt)
{
  int i;
  if(np != mesh->NoPoints || nt != mesh->NoTriangl) return 0;

  for(i=0;i<np;i++)
    if(mesh->PObj[i])
      delete [] (mesh->PObj[i]);
  if(mesh->PObj)
    delete [] (mesh->PObj); 

  for(i=0;i<nt;i++)
    if(mesh->TObj[i])
      delete [] (mesh->TObj[i]);
  if(mesh->TObj)
    delete [] (mesh->TObj);
  return 1; 
}

int free_media3D(MediaThreeD *mesh)
{
  int res;
  if(mesh)
    {
      res = ObjFree(mesh, mesh->NoPoints, mesh->NoTriangl);
      fprintf(stderr, "res = %d\n", res);
      if (!res) return 0;
      free(mesh);
      return 1; 
    }
  else
    return 0; 
}
