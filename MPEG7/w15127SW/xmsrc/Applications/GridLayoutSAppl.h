////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by 
// Karol Wnukowicz (Visual Atoms) 
// Stavros Paschalakis (Visual Atoms) 
// in the course of development of the MPEG-7 Standard (ISO/IEC 15938). 
// This software module is an implementation of a part 
// of one or more MPEG-7 Standard tools as specified by the MPEG-7 Standard. 
// ISO/IEC gives users of the MPEG-7 Standard free license to this software
// module or modifications thereof for use in hardware or software products
// claiming conformance to the MPEG-7 Standard. 
// Those intending to use this software module in hardware or software products 
// are advised that its use may infringe existing patents. The original 
// developer of this software module and his/her company, the subsequent 
// editors and their companies, and ISO/IEC have no liability for use of this 
// software module or modifications thereof in an implementation.
// Copyright is not released for non MPEG-7 Standard conforming products. 
// <CN1> retains full right to use the code for his/her own purpose, 
// assign or donate the code to a third party and to inhibit third parties 
// from using the code for non MPEG-7 Standard conforming products. 
// This copyright notice must be included in all copies or derivative works. 
// Copyright (c)2003-2015
//
////////////////////////////////////////////////////////////////////////////////
//
// Applicable File Name:  GridLayoutSAppl.h

#ifndef __GRIDLAYOUTSERVER__
#define __GRIDLAYOUTSERVER__


#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"

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
class GridLayoutServer: public ServerApplication
{
public:
  // Null constructor
  GridLayoutServer();

  // Destructor
  virtual ~GridLayoutServer();

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

#endif // __GRIDLAYOUTSERVER__
