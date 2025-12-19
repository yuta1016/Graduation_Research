///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Sanghoon Sull, Korea University, Seoul, Korea
// Keansub Lee, Korea University, Seoul, Korea
//
// in the course of development of the MPEG-7 Experimentation SpatialResolutionHint.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation SpatialResolutionHint tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatialResolutionHintCAppl.h
//
#ifndef __SPATIALRESOLUTIONHINT_CLIENT__
#define __SPATIALRESOLUTIONHINT_CLIENT__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"
#include "momusys.h"


//=============================================================================

namespace XM
{

//=============================================================================
class SpatialResolutionHintClient: public ClientApplication
{
public:
	SpatialResolutionHintClient();
	virtual ~SpatialResolutionHintClient();

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

	MediaDB m_RamDB;
	int NoOfElements;
	GenericDS *m_DBDescription;

};
};

#endif //__SPATIALRESOLUTIONHINTT_CLIENT__
