///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation Package.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Package tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  PackageSearch.h
//
#ifndef __PACKAGESEARCH_H__
#define __PACKAGESEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"


//=============================================================================

namespace XM
{

// Forward Declarations:
class PackageSearchTool;

//=============================================================================
class PackageSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	PackageSearchInterfaceABC();
	virtual ~PackageSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetPackageInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface) = 0;
	//sth not the std interface for search tools
	virtual int SetDescriptionInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface) = 0;
	//sth not the std interface for search tools
	virtual int SetPackageParms(const char* packagefile, const char* descriptionfile) = 0;
	//sth not the std interface for search tools
	virtual unsigned long StartPackaging(void) = 0;

	static const UUID myID;
};

//=============================================================================
class PackageSearchInterface: 
  public PackageSearchInterfaceABC
{
public:
	PackageSearchInterface(PackageSearchTool* aTool);
	virtual ~PackageSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetPackageInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface);
	virtual int SetPackageParms(const char* packagefile, const char* descriptionfile);
	virtual unsigned long StartPackaging(void);

	static const UUID myID;
	static const char *myName;

private:
	PackageSearchTool *m_SearchTool;
};

//=============================================================================
class PackageSearchTool: public Search
{
friend class PackageSearchInterface;
public:
        
	PackageSearchTool();
	PackageSearchTool(GenericDSInterfaceABC *thePackageInterface, 
		GenericDSInterfaceABC *theDescriptionInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual PackageSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetPackageInterface(GenericDSInterfaceABC *aRefGenericDSInterface);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aRefGenericDSInterface);

	virtual int SetPackageParms(const char* packagefile, const char* descriptionfile);
	virtual unsigned long StartPackaging(void);

	static const UUID myID;
	static const char * myName;


private:
	virtual ~PackageSearchTool();

	char* m_Packagefile;
	char* m_Descriptionfile;
	PackageSearchInterface m_Interface;
	GenericDSInterfaceABC *m_PackageInterface;
	GenericDSInterfaceABC *m_DescriptionInterface;

};

};


#endif //__PACKAGESEARCH_H__
