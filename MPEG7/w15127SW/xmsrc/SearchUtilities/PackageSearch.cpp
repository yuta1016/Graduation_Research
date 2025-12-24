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
// Applicable File Name:  PackageSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

static const int VERB = 0;//sth define

//=============================================================================

using namespace XM;
using namespace std;

const UUID PackageSearchInterface::myID = UUID("");
const char *PackageSearchInterface::myName = "Package Interface";

const UUID PackageSearchTool::myID = UUID("");
const char *PackageSearchTool::myName = "Package Tool";

const UUID PackageSearchInterfaceABC::myID = UUID();


//=============================================================================
PackageSearchInterfaceABC::PackageSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
PackageSearchInterface::PackageSearchInterface(PackageSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PackageSearchInterface::~PackageSearchInterface()
{
}

//----------------------------------------------------------------------------
void PackageSearchInterface::destroy()
{  delete m_SearchTool; }


//----------------------------------------------------------------------------
int PackageSearchInterface::SetPackageInterface(GenericDSInterfaceABC
														   *aGenericDSInterface)
{
  return m_SearchTool->SetPackageInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int PackageSearchInterface::SetDescriptionInterface(GenericDSInterfaceABC
														   *aGenericDSInterface)
{
  return m_SearchTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int PackageSearchInterface::SetPackageParms(const char* packagefile, const char* descriptionfile)
{
	return m_SearchTool->SetPackageParms(packagefile, descriptionfile);
}

//----------------------------------------------------------------------------
unsigned long PackageSearchInterface::StartPackaging(void)
{
	return m_SearchTool->StartPackaging();
}

//----------------------------------------------------------------------------
const UUID& PackageSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PackageSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
PackageSearchTool::PackageSearchTool():
m_Packagefile(0),
m_Descriptionfile(0),
m_Interface(this),
m_DescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PackageSearchTool::PackageSearchTool(GenericDSInterfaceABC 
				     *thePackageInterface,
				     GenericDSInterfaceABC
				     *theDescriptionInterface) :
m_Packagefile(0),
m_Descriptionfile(0),
m_Interface(this),
m_PackageInterface(0),
m_DescriptionInterface(0)
{
	SetPackageInterface(thePackageInterface);
	SetDescriptionInterface(theDescriptionInterface);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PackageSearchTool::~PackageSearchTool()
{
	delete [] m_Packagefile;
	delete [] m_Descriptionfile;
}

//----------------------------------------------------------------------------
const UUID& PackageSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PackageSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool PackageSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool PackageSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int PackageSearchTool::SetPackageInterface(GenericDSInterfaceABC
													  *aGenericDSInterface)
{
  m_PackageInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int PackageSearchTool::SetDescriptionInterface(GenericDSInterfaceABC
													  *aGenericDSInterface)
{
  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int PackageSearchTool::SetPackageParms(const char* packagefile, const char* descriptionfile)
{
	delete [] m_Packagefile;
	if (packagefile) {
		m_Packagefile = new char[strlen(packagefile)+1];
		strcpy(m_Packagefile, packagefile);
	}
	else m_Packagefile = 0;

	delete [] m_Descriptionfile;
	if (descriptionfile) {
		m_Descriptionfile = new char[strlen(descriptionfile)+1];
		strcpy(m_Descriptionfile, descriptionfile);
	}
	else m_Descriptionfile = 0;

return 0;

}

struct SCHEMES {
	char name[80][80];
	int recurse[80];
	int count;
};


static void getSchemes(GenericDS& DS, SCHEMES& schemes) {
		string str;
		vector<GenericDS> SDS(DS.GetAllDescriptions("scheme"));
		for (int i = 0; i < SDS.size() && !SDS[i].isNull(); i++) {
			SDS[i].GetTextAttribute("name", str);
			if (VERB) cerr << SDS[i].GetDSName().data() << " name = \"" << str.data() << "\"" << endl;
			strcpy(schemes.name[schemes.count], str.data());
			schemes.recurse[schemes.count] = 0;
			if (SDS[i].GetTextAttribute("recurse", str)==GenericDS::SUCCESS) 
				if (strcmp(str.data(), "true")==0) schemes.recurse[schemes.count] = 1;
			schemes.count++;
		}
}

static void getPackages(GenericDS& DS, SCHEMES& schemes) {
		string str;
		vector<GenericDS> PDS(DS.GetAllDescriptions("Package"));
		for (int i = 0; i < PDS.size() && !PDS[i].isNull(); i++) {
			PDS[i].GetTextAttribute("name", str);
			if (VERB) cerr << PDS[i].GetDSName().data() << " name = \"" << str.data() << "\"" << endl;
			getPackages(PDS[i], schemes);
			getSchemes(PDS[i], schemes);
		}
}

static int checkSchemes(const SCHEMES& schemes, const char* path, int& recurse) {
	for (int i = 0; i < schemes.count; i++) {
		if (strcmp(schemes.name[i], path)==0) {
			recurse = schemes.recurse[i];
			return 1;
		}
	}
	recurse = 0;
	return 0;
}


static void getChildren(GenericDS& DS, const SCHEMES& schemes, const char* path, int all = 0) {
	char buf[256];
	int i = 0, recurse = 0;
	string str;
	GenericDS ChildDS = DS.GetChild(i++);
	while (!ChildDS.isNull()) {
		sprintf(buf, "%s/%s", path, ChildDS.GetDSName().data());
		if (all || checkSchemes(schemes, buf, recurse)) {
			cout << "IN  " << buf << endl;
			if (all || recurse) all = 1;
			getChildren(ChildDS, schemes, buf, all);
			}
		else cout << "OUT " << buf << endl;
		ChildDS = DS.GetChild(i++);
	}
}


//----------------------------------------------------------------------------
unsigned long PackageSearchTool::StartPackaging(void)
{
	if (VERB) cerr << "Packaging " << m_Descriptionfile << " using Package " << m_Packagefile << endl;
	// Read Package Description

	string str;
	GenericDS PackageDS = m_PackageInterface->GetFirstChild();
	PackageDS.GetTextAttribute("name", str);
	if (VERB) cerr << PackageDS.GetDSName().data() << " name =\"" << str.data() << "\"" << endl;

	if (PackageDS.isNull()) {
		cerr << "NULL Package" << endl;
		return 0;
	}

	SCHEMES schemes;
	schemes.count = 0;

	if (!PackageDS.isNull() && PackageDS.GetNumberOfChildren()>0) getPackages(PackageDS, schemes);

	if (VERB) {
		cerr << "The package includes the following schemes: " << endl;
		for (int i = 0; i < schemes.count; i++) cerr << schemes.name[i] << " " << schemes.recurse[i] << endl;
		cerr << endl;
	}

	GenericDS DescriptionDS = m_DescriptionInterface->GetFirstChild();

	if (DescriptionDS.isNull()) {
		cerr << "NULL Description" << endl;
		return 0;
	}

	cerr << endl << "Results of filtering description using package: " << endl;
	getChildren(DescriptionDS, schemes, DescriptionDS.GetDSName().data());

	return 0;
}


//----------------------------------------------------------------------------
PackageSearchInterfaceABC *PackageSearchTool::GetInterface(void)
{ return &m_Interface; }

