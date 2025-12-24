///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann, Munich University of Technology
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
// XM_Main.cpp

#ifndef _VERSION1_DEFINED
#define _VERSION1_DEFINED


typedef struct {
  char *ListFile;
  char *DCParameterFileName;
  char *CSParameterFileName;
  char *CLParameterFileName;
  char *SCParameterFileName;
  char *HTParameterFileName;
  char *EHParameterFileName;
  char *Bitstream;
  char *Application;
  double  Thresh;
  char *LogRedirection;
  char *ClusterFile;
} TXMApplicationParameterSet;

extern "C" 
{

  TXMApplicationParameterSet *parse_cmdline (int argc, char **argv);
  int select_application (TXMApplicationParameterSet * Parameterset);
  int printhelp (TXMApplicationParameterSet * Parameterset,
		 int type_of_messages);
}

#endif //_VERSION1_DEFINED
