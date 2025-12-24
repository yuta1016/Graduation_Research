///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Perceptual3DShapeSAppl.h
//
#ifndef __PERCEPTUAL3DSHAPESERVER__
#define __PERCEPTUAL3DSHAPESERVER__


#include "BasicArchitecture/XMInterfaces.h" /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"    /* Naming Convention */

namespace XM
{

//=============================================================================
// This class creates the binary representation of the metadata for the 
// dummy type. 
// 
// For example purposes, each version of the dummy type consists of a
// signed long integer. Thus a library of such dtypes will consists of
// a set of files each containing one (textual representation of an) 
// integer. These are read in and stored in binary form. 
class Perceptual3DShapeServer: public ServerApplication
{
public:
  // Null constructor
  Perceptual3DShapeServer();

  // Destructor
  virtual ~Perceptual3DShapeServer();

  // not used to create descriptor data base
  virtual unsigned long Open();

  // This appears to be the main processing task. It takes in
  // a list of files which should be processed and written out to the
  // file supplied by 'bitstream'.
  virtual unsigned long Start(char *ListFile, char *Bitstream);

  // not used to create descriptor data base
  virtual unsigned long Stop();

  // not used to create descriptor data base
  virtual unsigned long Close();

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);
	
  // Access is allowed only by class factories for this object.  This 
  // avoids having to duplicate the ID definition in multiple locations.
  // In the future, we may have to do this.  PLEASE DO NOT USE THESE
  // UNLESS YOU ARE IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;
private:

}; // End class definition

}; // End namespace definition

#endif //__PERCEPTUAL3DSHAPESERVER__
