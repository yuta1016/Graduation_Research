/* 
 * (C) Lehrstuhl fuer Integrierte Systeme, TU Muenchen
 */

///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// S. Herrmann TU-Munich, Institute for Integrated Circuits
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
// Copyright (c) 1998-2004.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  readparam.h
//

#ifndef _LIS_PARA
#define _LIS_PARA

/******* CVSInfo *************************************************************/
/* Filename :    $RCSfile: readparam.h,v $
   Version  :    Revision: 1.5
   Last Edit:    Date: 2000/03/14 16:44:03
   Released :    %D% %T%   */
/******* CVSInfo *************************************************************/

/******* INCLUDES ************************************************************/

/******* DEFINES *************************************************************/

/******* MAKROS  *************************************************************/

/******* TYPES ***************************************************************/
typedef struct t_param_lis_element {
  char *name;
  char *value;
  t_param_lis_element *next;
} TParamElement;

/******* VARIABLES ***********************************************************/

/******* FUNCTIONS ***********************************************************/
namespace LIS {

class Param {
 public:
  Param(void);
  Param(char *filename);
  ~Param();

  int ReadParameters(char *filename);
  /* Reads in a paramter file. The parameters are stored in a linked list.
     New parameters are prependened to the List, thus, the new values
     overrule old values*/

  void EmptyParameters();
  /* Empties a paramter list*/

  char  *GetParameter(char *param, char *defaultval);
  /* Give pack the value of the paramter "param". If param is not found
     the default value is returned*/

  int  TestParameter(char *param);
  /* Tests if a parameter exists*/

  // set this object to be accessible via the C interface
  //int SetCParameters();

 private:
  TParamElement *m_FirstPar;

  void MemError();
  /* drop an error message when out of memory and exit program*/

  char *strtrunc(char *string);
  /* Entfernen von Leerzeichen am Ende eines Strings*/

};
}

extern "C" {
  int readparameters(char *parameterfile);
  char  *getpara(char *param, char *defaultval);
  int  testpara(char *param);
  void setdefaultparam(LIS::Param *aParam);
}

#endif //_LIS_PARA




