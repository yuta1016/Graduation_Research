///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Ana B. Benitez
// Company: Columbia University
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  PyramidCAppl.h
//

#ifndef __PYRAMID_CLIENT__
#define __PYRAMID_CLIENT__

#include "BasicArchitecture/XMInterfaces.h"           /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"              /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

namespace XM
{

//=============================================================================
class PyramidClient: public ClientApplication
{
public:
  PyramidClient();
  virtual ~PyramidClient();

  virtual unsigned long Open(char *ListFile, char *Bitstream);
  virtual unsigned long Start(char *Query,int NoOfMatches);
  virtual unsigned long Stop();
  virtual unsigned long Close();

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);
	
  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;
private:
  // maybe this class could contain the quantizer descriptor directly?

  MediaDB RamDB; // This appears to be a list of filenames
  int NoOfElements;

  GenericDS *DBDescription;
};
};

#endif //__PYRAMID_CLIENT__
