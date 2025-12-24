///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// S.Herrmann, Lehrstuhl fuer Integrierte Systeme, TU Muenchen
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
// readparam.cpp

/******* CVSInfo *************************************************************/
/* Filename :    $RCSfile: readparam.cpp,v $
   Version  :    $Revision: 1.1.1.1 $
   Last Edit:    $Date: 2004/10/08 10:43:18 $ 
   Released :    %D% %T% */
/******* CVSInfo *************************************************************/

/**********************************************************************/
/*  INCLUDES                                                          */
/**********************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#ifndef _VISUALC_
#include<unistd.h>
#else
#include<io.h>
#endif
#include"readparam.h"

/**********************************************************************/
/*  VAROIABLES                                                        */
/**********************************************************************/
static LIS::Param *LISParam_default=0;
static int ParamSetInternal=0;

/**********************************************************************/
/*  PROTOTYPES                                                        */
/**********************************************************************/
//typedef struct param_struct param;

//static TParamElement *m_FirstPar=0;

using namespace LIS;

/**********************************************************************/
/*  FUNCTIONS                                                         */
/**********************************************************************/

//======================================================================
Param::Param(void)
{
  m_FirstPar=0;
}

//----------------------------------------------------------------------
Param::Param(char *filename)
{
  m_FirstPar=0;

  ReadParameters(filename);
}

//----------------------------------------------------------------------
Param::~Param()
{
  EmptyParameters();

  m_FirstPar=0;
}

//----------------------------------------------------------------------
/**********************************************************************/
/*  readparameters                                                    
    this procedure opens the parfilename and copies the
    contens to the buffer parabuffer.
    than the funktions to read specific parameters are called*/
/**********************************************************************/
int Param::ReadParameters(char *parfilename)

/*char *parfilename;*/

{
  FILE *parafile;
  char string[512];
  int comment, name;
  TParamElement **newparam;
  TParamElement *nextparam;
  int i;
  char *p;
  
  parafile=fopen(parfilename,"rb");
  if (!parafile) {
    fprintf(stderr,"ERROR : Could not open parameter file %s!",parfilename);
    perror("");
    return 2;
  }

  i=0;
  name=0;
  comment=0;
  nextparam=m_FirstPar;
  newparam=&m_FirstPar;
  while (fread(&string[i],1,1,parafile)) {
    if (i) {                            /* start of comment*/
      if ((string[i-1]== '/') && (string[i]=='*')) {
	i--;
	comment=1;
	continue;
      }
    }

    if (comment) {                     /* end of comment*/
      while (string[i]== '*') {
	fread(&string[i],1,1,parafile);
	if (string[i]== '/') {
	  comment=0;
	}
      }
      if (!comment) continue;
    }
    else {
      if (strchr(" \t\n\x0",string[i])) {    /* if delimiter*/
	if (!i) {
	  if (name && (string[0]=='\n')) {
	    name=0;
	    newparam=&((*newparam)->next);
	  }
	  continue;                          /* starting space or end of line*/
	}
	if (!name) {                         /* end of name*/  
	  *newparam=(TParamElement *)malloc(sizeof(TParamElement));
	  if (!*newparam) MemError();
	  (*newparam)->name=(char *)malloc(i+1);
	  if (!(*newparam)->name) MemError();
	  (*newparam)->value=0;
	  (*newparam)->next=nextparam;
	  if (string[i] != '\n') name=1;
	  string[i]=0;
	  while ((p=strrchr(string,'\n'))||(p=strrchr(string,'\r'))) *p=0;
	  strcpy((*newparam)->name,string);
	  i=0;
	  if (!name) newparam=&((*newparam)->next);
	  continue;
	}
	if (string[i] == '\n') {
	  (*newparam)->value=(char *)malloc(i+1);
	  if (!(*newparam)->value) MemError();
	  string[i]=0;
	  while ((p=strrchr(string,'\n'))||(p=strrchr(string,'\r'))) *p=0;
	  strcpy((*newparam)->value,string);
	  i=0;
	  name=0;
	  newparam=&((*newparam)->next);
	  continue;
	}
	
	if (string[i-1] == ' ') {     /* more the one white space*/
	  continue;
	}
	string[i]= ' ';
	i++;
	continue;
      }
      i++;
    }
  }

  if (i) {                          /* file not terminatetd with \n*/
    if (!name) {
      *newparam=(TParamElement *)malloc(sizeof(TParamElement));
      if (!*newparam) MemError();
      i++;
      (*newparam)->name=(char *)malloc(i+1);
      if (!(*newparam)->name) MemError();
      (*newparam)->value=0;
      (*newparam)->next=0;
      string[i]=0;
      strcpy((*newparam)->name,string);
    }
    else {
      i++;
      (*newparam)->value=(char *)malloc(i+1);
      if (!(*newparam)->value) MemError();
      string[i]=0;
      strcpy((*newparam)->value,string);
    }
  }
  fclose(parafile);
  return 0;
}

//----------------------------------------------------------------------
/**********************************************************************/
/*  EmptyParameters
    searches for a parameter in the parameterbuffer and
    returns the value
    if not found, return default-value*/
/**********************************************************************/
void Param::EmptyParameters()

/*char *param;
char *deflt;*/

{
  TParamElement *para;

  if (!m_FirstPar) return;

  para=m_FirstPar->next;

  while (para){
    free(m_FirstPar);
    m_FirstPar=para;
    para=para->next;
  }
  free(m_FirstPar);
  m_FirstPar=0;
  return;
}

//----------------------------------------------------------------------
/**********************************************************************/
/*  GetParameter
    searches for a parameter in the parameterbuffer and
    returns the value
    if not found, return default-value*/
/**********************************************************************/
char *Param::GetParameter(char *paraname,char * deflt)

/*char *param;
char *deflt;*/

{
  TParamElement *para;

  para=m_FirstPar;

  while (para){
    if (!strcmp(para->name,paraname)) {
      if (!para->value) return "";
      return para->value;
    }
    para=para->next;
  }
  return(deflt);
}

//----------------------------------------------------------------------
/**********************************************************************/
/*  TestParameter
    if parameter param is found in parfilename the
    returnvalue is 1 els it is 0*/
/**********************************************************************/
int Param::TestParameter(char *paraname)

/*char *param;
char *deflt;*/

{
  TParamElement *para;

  para=m_FirstPar;

  while (para){
    if (!strcmp(para->name,paraname)) return 1;
    para=para->next;
  }
  return(0);
}

//----------------------------------------------------------------------
/**********************************************************************/
/* MemError   prints error message                                    */
/**********************************************************************/

void Param::MemError()

{
  fprintf(stderr,"ERROR : Out of memory!\n");
  exit (1);
}



//----------------------------------------------------------------------
/**********************************************************************/
/*  strtrunc
    deletes trailing " \t\n" from string*/
/**********************************************************************/
char *Param::strtrunc(char *string)

/*char *string;*/

{
  char *a;
  
  a=strchr(string,'\n');
  if (a) *a=0;
  a=string;
  a+=strlen(a);
  a--;
  
  while (strchr(" \t\n",*a) && (a>=string)) {
    *a=0;
    a--;
  }
  return(a);
}

//----------------------------------------------------------------------
int readparameters(char *parameterfile)
{
  if (!LISParam_default) {
    setdefaultparam(0);
  }
  return LISParam_default->ReadParameters(parameterfile);
}

//----------------------------------------------------------------------
char  *getpara(char *param, char *defaultval)

{
  if (!LISParam_default) {
    return defaultval;
  }
  return LISParam_default->GetParameter(param,defaultval);
}

//----------------------------------------------------------------------
int testpara(char *param)

{
  if (!LISParam_default) {
    return 0;
  }
  return LISParam_default->TestParameter(param);
}

//----------------------------------------------------------------------
void setdefaultparam(LIS::Param *aParam)

{
  if (LISParam_default && (aParam == LISParam_default)) return;

  if (LISParam_default && ParamSetInternal) {
    delete LISParam_default;
  }
  
  //2014/07 KW - fixed pointer to int conversion for 64-bit system (?)
  LIS::Param *ptest = (LIS::Param *)-1;
  LISParam_default=0;
  ParamSetInternal=0;
  if (aParam == ptest) return;
  ptest = 0;
  if (aParam==ptest) {
    LISParam_default= new LIS::Param;
    ParamSetInternal=1;
    return;
  }
  LISParam_default=aParam;
}
