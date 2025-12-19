/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jin-Soo Lee - LG Electronics Institute of Technology
//
// (contributing organizations names)

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
// Copyright (c) 2000 - .
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MatchingHintCAppl.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATCHINGHINT_SERVER__
#define __MATCHINGHINT_SERVER__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

//=============================================================================
class MatchingHintServer : public ServerApplication
{
public:
	MatchingHintServer();
	virtual ~MatchingHintServer();

	virtual unsigned long Open();
	virtual unsigned long Start(char *ListFile, char *Query);
	virtual unsigned long Stop();
	virtual unsigned long Close();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	
	static const UUID myID;
	static const char * myName;
private:

	//jslee: The extraction of MatchingHint is done by calculation of similarity among query and feedbak images. So here, they are required 
	MediaDB m_RamDB;
	int NoOfElements;

	GenericDS *m_DBDescription;
};
};

#endif //__MATCHINGHINT_SERVER__//
