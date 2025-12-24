//////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshihiko Munetsugu 
// Katsunao Takahashi
//        (Matsushita Electric Industrial Co., Ltd./
//         Matsushita Research Institute Tokyo, Inc.)
//
// Ver4.0 for FDIS, 20010928
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard (ISO/IEC 15938) free licence
// to this software module or modifications thereof for use in hardware or
// software products claiming confermance to the MPEG-7 standard (ISO/IEC 15938).
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents.
// THE ORIGINAL DEVELOPER OF THIS SOFTWARE MODULE AND HIS/HER COMPANY,
// THE SUBSEQUENT EDITORS AND THEIR COMPANIES, AND ISO/IEC HAVE NO 
// LIABILITY FOR USE OF THIS SOFTWARE MODULE OR MODIFICATIONS THEROF.
// No license to this software module is granted for non MPEG-7 standard
// (ISO/IEC 15938) comforming products.
// Matsushita Electric Industrial Co., Ltd. and Matsushita Research Institute
// Tokyo, Inc. retain full right to use the software module for their own
// purpose, assign or donate the software module to a third party and 
// to inhibit third parties from using the code for non MPEG-7 standard
// (ISO/IEC 15938) conforming products.
//
// Copyright (c) 2000-2001.
//
// This copyright notice must be included in all copies or derivative works
// of the software module.
//
//
//////////////////////////////////////////////////////////////////////////////////
/********************************
 * PointOfViewCAppl.h           *
 ********************************/


#ifndef __POINTOFVIEW_CLIENT__
#define __POINTOFVIEW_CLIENT__
#include "BasicArchitecture/XMInterfaces.h"           /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"              /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */


namespace XM
{

//=============================================================================
class PointOfViewClient: public ClientApplication
{
public:
	PointOfViewClient();
	virtual ~PointOfViewClient();

	virtual unsigned long Open(char *ListFile, char *DescriptionFile);
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
	MediaDB m_RamDB ;
	int m_NoOfElements ;

	GenericDS *m_DBSegmentDS ;

};

};

#endif //__POINTOFVIEW_CLIENT__
