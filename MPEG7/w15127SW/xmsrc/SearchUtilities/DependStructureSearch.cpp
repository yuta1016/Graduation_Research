////////////////////////////////////////////////////////////////////////
//
// DependStructureSearch.cpp
//
// This software module was originally developed by
//
// Masahiro Shibata, NHK(Japan Broadcasting Corporation)
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// NHK(Japan Broadcasting Corp.) retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#include <strstream>
#include "DescriptionSchemes/DescriptionSchemes.h"		/* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "DependStructureSearch.h"	//TODO remove after development is stable
#include <stack>
#include <iostream>


// 2001/3/21
//    EN_FREE_TEXT	"FreeText" => "FreeTextAnnotation"
#define EN_TEXT_ANNOTATION	"TextAnnotation"
#define EN_FREE_TEXT		"FreeTextAnnotation"
#define EN_DEPEND_STRUCTURE	"DependencyStructure"
#define EN_ROOTNODE			"Sentence"
#define EN_HEAD				"Head"
#define EN_CHEAD			"CompoundHead"
#define EN_WORD				"Word"
#define EN_PHRASE			"Phrase"
#define EN_NONDEP			"NonDependencyPhrase"
#define EN_QUERY			"Query"
#define EN_AV_SEGMENT		"AudioVisualSegment"
#define EN_IMAGE			"Image"

#define	EA_BASE_FORM		"baseForm"
#define EA_OPERATOR			"operator"
#define EA_PARTICLE			"functionWord"
#define EA_ANNOTATION_ID	"id"
#define EA_SYNTHE			"synthesis"
#define EA_TYPE				"type"

#define EAV_COORD			"coordination"
#define EAV_CONJ			"conjunction"

using namespace XM;
using namespace std;

const UUID DependStructureSearchInterface::myID = UUID("");
const char *DependStructureSearchInterface::myName = "DependStructureSearchInterface";

const UUID DependStructureSearchTool::myID = UUID("");
const char *DependStructureSearchTool::myName = "DependStructureSearchTool";

const UUID DependStructureSearchInterfaceABC::myID = UUID();

// default penalty
int DependStructureSearchTool::PenaltySubject = 10;	// operator="subject"
int DependStructureSearchTool::PenaltyObject = 5;	// operator="object"
int DependStructureSearchTool::PenaltyParticle = 3;	// particle=
int DependStructureSearchTool::PenaltyOther = 2;

int DssHead::PenaltySubject = 10; // operator="subject"
int DssHead::PenaltyObject = 5;	  // operator="object"
int DssHead::PenaltyParticle = 3; // particle=
int DssHead::PenaltyOther = 2;


//=============================================================================
DependStructureSearchInterfaceABC::DependStructureSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
DependStructureSearchInterface::DependStructureSearchInterface(DependStructureSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DependStructureSearchInterface::~DependStructureSearchInterface()
{
}

//----------------------------------------------------------------------------
void DependStructureSearchInterface::destroy()
{
  delete m_SearchTool;	// Declare friend
}

//----------------------------------------------------------------------------
int DependStructureSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC *aRefGenericDSInterface)
{
  return m_SearchTool->
    SetRefDescriptionInterface(aRefGenericDSInterface);
}

//----------------------------------------------------------------------------
int DependStructureSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryGenericDSInterface)
{
  return m_SearchTool->
    SetQueryDescriptionInterface(aQueryGenericDSInterface);
}

//----------------------------------------------------------------------------
int DependStructureSearchInterface::
GetDistance(vector< DependStructureSearchResult > &aResult,
	    string &aQueryText)
{
  return m_SearchTool->GetDistance(aResult, aQueryText);
}

//----------------------------------------------------------------------------
const UUID& DependStructureSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DependStructureSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
DependStructureSearchTool::DependStructureSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DependStructureSearchTool::DependStructureSearchTool(GenericDSInterfaceABC
						   *aDescription):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
	SetQueryDescriptionInterface(aDescription);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DependStructureSearchTool::~DependStructureSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& DependStructureSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DependStructureSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool DependStructureSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool DependStructureSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
DependStructureSearchInterfaceABC *DependStructureSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
// Set the reference Program interface into the Search Tool
//----------------------------------------------------------------------------
int DependStructureSearchTool::SetRefDescriptionInterface(GenericDSInterfaceABC
												*aRefGenericDSInterface)
{
	m_RefDescriptionInterface = aRefGenericDSInterface;

	return 0;
}

//----------------------------------------------------------------------------
// Set the UserPreference interface into the Search Tool
//----------------------------------------------------------------------------
int DependStructureSearchTool::SetQueryDescriptionInterface(GenericDSInterfaceABC
												   *aQueryGenericDSInterface)
{
	m_QueryDescriptionInterface = aQueryGenericDSInterface;

	return 0;
}

//----------------------------------------------------------------------------
static string &
erase_space(
string &a_str
){
	int pos;
	if((pos = a_str.find_first_not_of(" \t\n")) != string::npos){
		a_str.erase(0, pos);
	}
	if((pos = a_str.find_last_not_of(" \t\n")) != string::npos){
		a_str.erase(pos+1);
	}
    return(a_str);
}				

//----------------------------------------------------------------------------
// Evaluate the query
//----------------------------------------------------------------------------
int
DependStructureSearchTool::GetDistance(
vector< DependStructureSearchResult >&aResult,
string	&aQueryText
){

	GenericDS dsQuery = m_QueryDescriptionInterface->GetChild((int)0);

	if(dsQuery.isNull()){
		return -1;
	}
	if(dsQuery.GetDSName().compare(EN_QUERY)){
		dsQuery = dsQuery.GetFirstChild();
		for(; !dsQuery.isNull(); dsQuery = dsQuery.GetNextSibling()){
			if(!dsQuery.GetDSName().compare(EN_QUERY)){
				break;
			}
		}
	}
	// 2001/8/29  print FreeText
	aQueryText.erase();
	dsQuery.GetDescription(EN_FREE_TEXT).GetTextValue(aQueryText);
    erase_space(aQueryText);

	GenericDS dsRef = m_RefDescriptionInterface->GetChild((int)0);
	if(!dsQuery.isNull() && !dsRef.isNull()){
		string	avSegId;
		int		avSegSubId;
		stack< GenericDS > stkR;

		GenericDS dsDs = dsQuery.GetDescription(EN_DEPEND_STRUCTURE);
		if(dsDs.isNull()) return(0);
	    dsDs = dsDs.GetDescription(EN_ROOTNODE);
		if(dsDs.isNull()) return(0);
		DssPhraseC		query(dsDs);
#ifdef DEBUG
#define DEBUG_SN
#endif
#ifdef DEBUG_SN
		{
		strstream tmp;
		query.print_node(tmp);
		tmp << '\0';
		std::cerr << tmp.str();
		tmp.freeze(0);
		}
#endif // DEBUG_SN

		stkR.push(dsRef.GetFirstChild());
		for(;;){
			if(stkR.top().isNull()){
				if(stkR.size() > 1){
					stkR.pop();
					stkR.top() = stkR.top().GetNextSibling();
					continue;
				}else{
					break;
				}
			}

			string tmpStr = stkR.top().GetDSName();
			if(!tmpStr.compare(EN_AV_SEGMENT) ||
			   !tmpStr.compare(EN_IMAGE)){
				stkR.top().GetTextAttribute(EA_ANNOTATION_ID, avSegId);
				avSegSubId = 0;
				stkR.push(stkR.top().GetFirstChild());

			}else if(!tmpStr.compare(EN_TEXT_ANNOTATION)){
				dsDs = stkR.top().GetDescription(EN_DEPEND_STRUCTURE);
				if(dsDs.isNull()) continue;
			    dsDs = dsDs.GetDescription(EN_ROOTNODE);
				if(dsDs.isNull()) continue;
				DssPhraseC		ref(dsDs);
				strstream	detail;
//#undef DEBUG
#ifdef DEBUG
				int penalty = query.compare(ref, detail);
#ifdef DEBUG_SN
				detail << endl;
				ref.print_node(detail);
#endif // DEBUG_SN
#else
				int penalty = query.compare(ref);
#endif
				detail << '\0';
				++avSegSubId;
				if(penalty >= 0){
					tmpStr = avSegId;
					strstream	strid;
					strid << '.' << avSegSubId << '\0';
					tmpStr += strid.str();
					strid.freeze(0);
// 2001/8/29  print FreeText
//#ifdef DEBUG
					GenericDS	tmpDs =
									stkR.top().GetDescription(EN_FREE_TEXT);
					string tmpStr2;
					tmpDs.GetTextValue(tmpStr2);
					erase_space(tmpStr2);
					tmpStr += "(";
					tmpStr += tmpStr2;
					tmpStr += ")";
//#endif // DEBUG
					aResult.push_back(
						DependStructureSearchResult(tmpStr, penalty, detail.str()));
					detail.freeze(0);
				}
				stkR.top() = stkR.top().GetNextSibling();
			}else{
				stkR.push(stkR.top().GetFirstChild());
			}
		}
	}

	return(0);
}

//=============================================================================
DependStructureSearchResult::DependStructureSearchResult()
			: m_Penalty(0)
{
}

//----------------------------------------------------------------------------
DependStructureSearchResult::DependStructureSearchResult(const string &aId, double aPenalty)
			: m_Index(aId), m_Penalty(aPenalty)
{
}

//----------------------------------------------------------------------------
DependStructureSearchResult::DependStructureSearchResult
		(const string &aId, double aPenalty, const char *aLack)
		: m_Index(aId), m_Penalty(aPenalty), m_Lack(aLack)
{
}

//----------------------------------------------------------------------------
DependStructureSearchResult &DependStructureSearchResult::operator=
		(const DependStructureSearchResult &aDsR)
{
	m_Index = aDsR.GetIndex();
	m_Penalty = aDsR.GetPenalty();
	m_Lack = aDsR.GetLack();
	return(*this);
}

//----------------------------------------------------------------------------
bool DependStructureSearchResult::
operator<(const DependStructureSearchResult &aDsR)
{
	if(m_Penalty - aDsR.GetPenalty() < 0){
		return(true);
	}else{
		return(false);
	}
}

//=============================================================================
DssPhraseC::DssPhraseC(
GenericDS	&aDs
){
	vector<DssHead *> childPhrase;
	make_phrase(aDs, childPhrase);
	iHead = new DssSHead();
	vector<DssHead *>::iterator itr = childPhrase.begin();
	for(; itr != childPhrase.end(); ++itr){
		iHead->relation.push_back(*itr);
	}
}

//----------------------------------------------------------------------------
DssPhraseC::~DssPhraseC(
){
	vector<DssHead *>::iterator	  itr = array.begin();
	for(; itr != array.end(); ++itr){ delete *itr;}
}

//----------------------------------------------------------------------------
int
DssPhraseC::compare(
DssPhraseC &aDp
){
	set<DssHead*> lack;
	int penalty;
	penalty = iHead->compare(aDp.iHead, lack, true);
	return(penalty);
}

//----------------------------------------------------------------------------
int
DssPhraseC::compare(
DssPhraseC		&aDp,
std::ostream	&aOut
){
	set<DssHead *> lack;

	int maxPenalty = 0;
	vector<DssHead *>::iterator itr = iHead->relation.begin();
	for(; itr != iHead->relation.end(); ++itr){
		(*itr)->get_lack(lack);
	}
	maxPenalty = iHead->sum_penalty(lack);

	int penalty;
	penalty = iHead->compare(aDp.iHead, lack, true);
	if(maxPenalty > penalty){
		iHead->out_penalty(lack, aOut);
		return(penalty);
	}else{
		return(-1);
	}
}

//----------------------------------------------------------------------------
void
DssPhraseC::make_phrase(
GenericDS			&aDs,
vector<DssHead *> &aHead
){
	vector<DssHead *>	rPhrase;
	vector<DssHead *>	rHead;

	string		tmpStr;
	GenericDS	child = aDs.GetFirstChild();
	for(; !child.isNull(); child = child.GetNextSibling()){
		tmpStr = child.GetDSName();
		if(!tmpStr.compare(EN_CHEAD)){
			if(!child.GetTextAttribute(EA_SYNTHE, tmpStr) &&
				!tmpStr.compare(EAV_COORD)
			){
				make_synthesis(child, rHead);

			}else{
				make_nonDepPhrase(child, rHead);

				DssCHead *cHead = new DssCHead();
				array.push_back(cHead);
				vector<DssHead *>::iterator		itr = rHead.begin();
				for(; itr != rHead.end(); ++itr){
					(*itr)->compound = cHead;
					cHead->head.push_back(*itr);
				}
				rHead.clear();
				rHead.push_back(cHead);
			}

		}else if(!tmpStr.compare(EN_HEAD)){
			DssSHead	*sHead = new DssSHead();
			array.push_back(sHead);
			rHead.push_back(sHead);
			if(child.GetTextAttribute(EA_BASE_FORM, tmpStr)){
				child.GetTextValue(tmpStr);
			}
			sHead->form = tmpStr;

		}else if(!tmpStr.compare(EN_PHRASE)){
			make_phrase(child, rPhrase);

		}else{	// Quotation
			;
		}
	}

	string particle;
	aDs.GetTextAttribute(EA_PARTICLE, particle);

	string oper;
	DssHead::OpType operType = DssHead::undef;
	if(!aDs.GetTextAttribute(EA_OPERATOR, oper)){
		if(!oper.compare("subject")){
			operType = DssHead::subject;
		}else if(!oper.compare("object")){
			operType = DssHead::object;
		}else if(!oper.compare("indirectObject")){
			operType = DssHead::indirectObject;
		}else if(!oper.compare("predicate")){
			operType = DssHead::predicate;
		}else if(!oper.compare("expletive")){
			operType = DssHead::expletive;
		}else if(!oper.compare("nonRestrictive")){
			operType = DssHead::nonRestrictive;
		}
	}

	vector<DssHead *>::iterator itrH = rHead.begin();
	for(; itrH != rHead.end(); ++itrH){
		if(!((*itrH)->get_type() == DssHead::tSHead &&
		     ((DssSHead*)*itrH)->conj)
		){
			vector<DssHead *>::iterator itrP = rPhrase.begin();
			for(; itrP != rPhrase.end(); ++itrP){
				if((*itrH)->get_type() == DssHead::tCHead){
					vector<DssHead *>::iterator itrCh =
											((DssCHead*)*itrH)->head.begin();
					for(; itrCh != ((DssCHead*)*itrH)->head.end(); ++itrCh){
						if((*itrCh)->get_type() == DssHead::tCHead){
							continue;
						}
						((DssSHead*)*itrCh)->relation.push_back(*itrP);
					}
				}else{
					((DssSHead*)*itrH)->relation.push_back(*itrP);
				}
			}
			((DssSHead*)*itrH)->opType = operType;
			if(!particle.empty()){
				((DssSHead*)*itrH)->functionWord = particle;
			}
		}
		aHead.push_back(*itrH);
	}
}

//----------------------------------------------------------------------------
void
DssPhraseC::make_synthesis(
GenericDS			&aDs,
vector<DssHead *> &aHead
){
	GenericDS	child = aDs.GetFirstChild();
	for(; !child.isNull(); child = child.GetNextSibling()){
		string tmpStr = child.GetDSName();
		if(!tmpStr.compare(EN_NONDEP)){
			vector<DssHead *> rHead;
			make_nonDepPhrase(child, rHead);
			if(rHead.size() > 1){
				DssCHead *cHead = new DssCHead();
				array.push_back(cHead);
				vector<DssHead *>::iterator		itr = rHead.begin();
				for(; itr != rHead.end(); ++itr){
					(*itr)->compound = cHead;
					cHead->head.push_back(*itr);
				}
				aHead.push_back(cHead);
				
			}else if(rHead.size() == 1){
				aHead.push_back(rHead.front());
			}

		}else if(!tmpStr.compare(EN_PHRASE)){
			make_phrase(child, aHead);

		}else if(!tmpStr.compare(EN_WORD)){
			DssSHead	*sHead = new DssSHead();
			array.push_back(sHead);
			aHead.push_back(sHead);
			if(child.GetTextAttribute(EA_BASE_FORM, tmpStr)){
				child.GetTextValue(tmpStr);
			}
			sHead->form = tmpStr;
			if(!child.GetTextAttribute(EA_TYPE, tmpStr)){
				if(!tmpStr.compare(EAV_CONJ)){
					sHead->conj = true;
				}
			}

		}else{	// Quotation
			;
		}
   }
}

//----------------------------------------------------------------------------
void
DssPhraseC::make_nonDepPhrase(
GenericDS			&aDs,
vector<DssHead *> &aHead
){
	GenericDS	child = aDs.GetFirstChild();
	for(; !child.isNull(); child = child.GetNextSibling()){
		string tmpStr = child.GetDSName();
		if(!tmpStr.compare(EN_NONDEP)){
			make_nonDepPhrase(child, aHead);

		}else if(!tmpStr.compare(EN_PHRASE)){
			make_phrase(child, aHead);

		}else if(!tmpStr.compare(EN_WORD)){
			DssSHead	*sHead = new DssSHead();
			array.push_back(sHead);
			aHead.push_back(sHead);
			if(child.GetTextAttribute(EA_BASE_FORM, tmpStr)){
				child.GetTextValue(tmpStr);
			}
			sHead->form = tmpStr;
			if(!child.GetTextAttribute(EA_TYPE, tmpStr)){
				if(!tmpStr.compare(EAV_CONJ)){
					sHead->conj = true;
				}
			}
			
		}else{	// Quotation
			;
		}
	}
}

//=============================================================================
int
DssHead::compare_cmb(
vector<DssHead *> &aSrc,
vector<DssHead *> &aDst,
set<DssHead *>	&aLack,
bool			aCmpFw
){
	int pltySum;
	int lca, lcb;
	set<DssHead *> *maxPltyTbl = new set<DssHead *>[aSrc.size()];
	set<DssHead *> *pltyTbl =
						new set<DssHead *>[aDst.size() * aSrc.size()];
	set<DssHead *> *matchTbl =
						new set<DssHead *>[aDst.size() * aSrc.size()];
	for(lca = 0; lca < aSrc.size(); lca++){
		for(lcb = 0; lcb < aDst.size(); lcb++){
			aSrc[lca]->compare(aDst[lcb], pltyTbl[lca * aDst.size() + lcb], aCmpFw);
			aSrc[lca]->get_match(
							matchTbl[lca * aDst.size() + lcb],
							pltyTbl[lca * aDst.size() + lcb]);
		}
		aSrc[lca]->get_lack(maxPltyTbl[lca]);
	}

	int *cmbTbl = new int[aSrc.size()];
	for(lca = 0; lca < aSrc.size(); lca++){
		if(lca < aDst.size()){
			cmbTbl[lca] = lca;
		}else{
			cmbTbl[lca] = -1;
		}
	}

	set<DssHead *>		tmpLack;
	int		minPlty = -1;
	for(;;){
		tmpLack.clear();
		pltySum = 0;
		for(lca = 0; lca < aSrc.size(); lca++){
			if(cmbTbl[lca] == -1){
				add_lack(tmpLack, maxPltyTbl[lca]);
			}else{
				add_lack(tmpLack, pltyTbl[lca * aDst.size() + cmbTbl[lca]]);
			}
		}
		for(lca = 0; lca < aSrc.size(); lca++){
			if(cmbTbl[lca] != -1){
				del_lack(tmpLack, matchTbl[lca * aDst.size() + cmbTbl[lca]]);
			}
		}
		pltySum = sum_penalty(tmpLack);
		if(minPlty < 0 || minPlty > pltySum){
			minPlty = pltySum;
			aLack = tmpLack;
		}

		lca = aSrc.size()-1;
		if(cmbTbl[lca] == -1){
			for(--lca; lca >= 0 && cmbTbl[lca] == -1; lca--) ;
		}
		if(lca < 0){
			break;
		}else{
			for(;;){
				++cmbTbl[lca];
				if(cmbTbl[lca] == aDst.size()){
					cmbTbl[lca] = -1;
					break;
				}
				for(lcb = 0; lcb < lca; lcb++){
					if(cmbTbl[lca] == cmbTbl[lcb]) break;
				}
				if(lcb == lca){
					break;
				}
			}

			lcb = 0;
			for(++lca; lca < aSrc.size(); lca++){
				for(; lcb < aDst.size(); lcb++){
					int lcc;
					for(lcc = 0; lcc < lca; lcc++){
						if(cmbTbl[lcc] == lcb) break;
					}
					if(lcc == lca) break;
				}
				if(lcb == aDst.size()){
					for(; lca < aSrc.size(); lca++){
						cmbTbl[lca] = -1;
					}
					break;
				}else{
					cmbTbl[lca] = lcb;
					lcb++;
				}
			}
		}
	}

	delete [] pltyTbl;
	delete [] cmbTbl;
	delete [] maxPltyTbl;

	return(minPlty);
}

//----------------------------------------------------------------------------
int
DssHead::sum_penalty(
set<DssHead *>	&aDh
){
	int penalty = 0;
	set<DssHead *>::iterator	itr;
	for(itr = aDh.begin(); itr != aDh.end(); ++itr){
		if((*itr)->get_type() == DssHead::tSHead &&
			((DssSHead*)*itr)->conj == true
		){
			penalty++;
		}else if((*itr)->opType == subject){
			penalty += PenaltySubject;
		}else if((*itr)->opType == object){
			penalty += PenaltyObject;
		}else if(!(*itr)->functionWord.empty()){
			penalty += PenaltyParticle;
		}else if((*itr)->compound != 0){
			penalty++;
		}else{
			penalty += PenaltyOther;
		}
	}
	return(penalty);
}

//sth several function delimiters are missing
std::ostream &
DssHead::out_penalty(
set<DssHead *>	&aDh,
std::ostream	&aOut
){
	set<DssHead *>::iterator	itr = aDh.begin();
	if(itr != aDh.end()){
		for(;;){
			(*itr)->print_form(aOut);
			if((*itr)->get_type() == DssHead::tSHead &&
				((DssSHead*)*itr)->conj == true
			){
				aOut << "(conj:1)";
			}else if((*itr)->opType == subject){
				aOut << "(sub:" << PenaltySubject << ")";
			}else if((*itr)->opType == object){
				aOut << "(obj:" << PenaltyObject << ")";
			}else if(!(*itr)->functionWord.empty()){
				aOut << "(fw:" << PenaltyParticle << ")";
			}else if((*itr)->compound != 0){
				aOut << "(ch:1)";
			}else{
				aOut << "(:" << PenaltyOther << ")";
			}
			++itr;
			if(itr == aDh.end()) break;
			aOut << ' ';
		}
	}
	return(aOut);
}

//----------------------------------------------------------------------------
void
DssHead::add_lack(
set<DssHead *> &aDst,
set<DssHead *> &aSrc
){
	set<DssHead *>::iterator	itrSrc = aSrc.begin();
	for(; itrSrc != aSrc.end(); ++itrSrc){
		aDst.insert(*itrSrc);
	}
}

//----------------------------------------------------------------------------
void
DssHead::del_lack(
set<DssHead *> &aDst,
set<DssHead *> &aSrc
){
	set<DssHead *>::iterator	itrSrc = aSrc.begin();
	for(; itrSrc != aSrc.end(); ++itrSrc){
		set<DssHead *>::iterator	itrF;
		if((itrF = aDst.find(*itrSrc)) != aDst.end()){
			aDst.erase(itrF);
		}
	}
}

//=============================================================================
int
DssSHead::compare(
DssHead			*aDh,
set<DssHead*>	&aLack,
bool			aCmpFw
){
	aLack.clear();
	if(aCmpFw && functionWord != aDh->functionWord){
		get_lack(aLack);
		return(sum_penalty(aLack));
	}
	if(aDh->get_type() == DssHead::tCHead){
		set<DssHead *>	lack;
		get_lack(lack);
		int		penalty = sum_penalty(lack);
		aLack = lack;
		vector<DssHead *>::iterator		itr = ((DssCHead*)aDh)->head.begin();
		for(; itr != ((DssCHead*)aDh)->head.end(); ++itr){
			int tmp = compare(*itr, lack, false);
			if(tmp < penalty){
				penalty = tmp;
				aLack = lack;
			}
		}
		return(penalty);

	}else{
		if(functionWord != ((DssSHead*)aDh)->functionWord ||
		   form != ((DssSHead*)aDh)->form
		){
			get_lack(aLack);
			return(sum_penalty(aLack));
		}

		if(relation.empty()){
			return(0);
		}else if(((DssSHead*)aDh)->relation.empty()){
			vector<DssHead *>::iterator itr = relation.begin();
			for(; itr != relation.end(); ++itr){
				(*itr)->get_lack(aLack);
			}
			return(sum_penalty(aLack));
		}

		return(compare_cmb(relation, ((DssSHead*)aDh)->relation, aLack, true));
	}
}

//----------------------------------------------------------------------------
void
DssSHead::get_lack(
set<DssHead *> &aS
){
	aS.insert(this);
	vector<DssHead*>::iterator	itr = relation.begin();
	for(; itr != relation.end(); ++itr){
		(*itr)->get_lack(aS);
	}
}

//----------------------------------------------------------------------------
void
DssSHead::get_match(
set<DssHead *> &aMatch,
set<DssHead *> &aLack
){
	if(aLack.find(this) == aLack.end()){
		aMatch.insert(this);
		vector<DssHead*>::iterator itr = relation.begin();
		for(; itr != relation.end(); ++itr){
			(*itr)->get_match(aMatch, aLack);
		}
	}
}

//----------------------------------------------------------------------------
std::ostream &
DssSHead::print_form(
std::ostream	&aOut
){
	aOut << form.c_str();
	return(aOut);
}

//----------------------------------------------------------------------------
std::ostream &
DssSHead::print_node(
std::ostream	&aOut,
int				aDepth
){
	for(int lca = 0; lca < aDepth; lca++) aOut << "  ";
	aOut << "-" << form.c_str() << "[" << functionWord.c_str() << "]";
	if(conj == true) aOut << "conj";
	aOut << endl;
	vector<DssHead *>::iterator itr = relation.begin();
	for(; itr != relation.end(); ++itr){
		(*itr)->print_node(aOut, aDepth+1);
	}
	return(aOut);
}

//=============================================================================
int
DssCHead::compare(
DssHead			*aDh,
set<DssHead *>	&aLack,
bool			aCmpFw
){
	aLack.clear();
	if(aCmpFw && functionWord != aDh->functionWord){
		get_lack(aLack);
		return(sum_penalty(aLack));
	}
	int penalty;
	if(aDh->get_type() == DssHead::tCHead){
		penalty = compare_cmb(head, ((DssCHead*)aDh)->head, aLack, false);
	}else{
		vector<DssHead *>		dst;
		dst.push_back(aDh);
		penalty = compare_cmb(head, dst, aLack, false);
	}

	int mCnt = 0;
	vector<DssHead *>	tmp;
	vector<DssHead *>::iterator	itr = head.begin();
	for(; itr != head.end(); ++itr){
		set<DssHead *>::iterator itrF;
		if((itrF = aLack.find(*itr)) != aLack.end()){
			mCnt++;
			tmp.push_back(*itrF);
			aLack.erase(itrF);
		}
	}
	penalty -= (mCnt * PenaltyOther);
	if(mCnt == head.size()){
		aLack.insert(this);
		penalty = sum_penalty(aLack);
	}else{
		penalty += mCnt;
		for(itr = tmp.begin(); itr != tmp.end(); ++itr){
			aLack.insert(*itr);
		}
	}

	return(penalty);
}

//----------------------------------------------------------------------------
void
DssCHead::get_lack(
set<DssHead *> &aS
){
	vector<DssHead*>::iterator	itr = head.begin();
	aS.insert(this);
	for(; itr != head.end(); ++itr){
		(*itr)->get_lack(aS);
		aS.erase(*itr);
	}
}

//----------------------------------------------------------------------------
void
DssCHead::get_match(
set<DssHead *> &aMatch,
set<DssHead *> &aLack
){
	if(aLack.find(this) == aLack.end()){
		aMatch.insert(this);
		vector<DssHead*>::iterator itr = head.begin();
		for(; itr != head.end(); ++itr){
			(*itr)->get_match(aMatch, aLack);
		}
	}
}

//----------------------------------------------------------------------------
std::ostream &
DssCHead::print_form(
std::ostream	&aOut
){
	vector<DssHead *>::iterator itr = head.begin();
	if(itr != head.end()){
		for(;;){
			(*itr)->print_form(aOut);
			++itr;
			if(itr != head.end()){
				aOut << ",";
			}else{
				break;
			}
		}
	}
	return(aOut);
}

//----------------------------------------------------------------------------
std::ostream &
DssCHead::print_node(
std::ostream	&aOut,
int				aDepth
){
	for(int lca = 0; lca < aDepth; lca++) aOut << "  ";
	aOut << "-(ch)" << "[" << functionWord.c_str() << "]" << endl;
	vector<DssHead *>::iterator itr = head.begin();
	for(; itr != head.end(); ++itr){
		(*itr)->print_node(aOut, aDepth+1);
	}
	return(aOut);
}

//=============================================================================
