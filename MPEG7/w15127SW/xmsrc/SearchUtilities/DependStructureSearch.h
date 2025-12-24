////////////////////////////////////////////////////////////////////////
//
// DependStructureSearch.h
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

#ifndef __DEPEND_STRUCTURE_SEARCH_H__
#define __DEPEND_STRUCTURE_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include <string>
#include <vector>
#include <set>

namespace XM
{

class DependStructureSearchTool;
class DependStructureSearchResult;

//=============================================================================
class DependStructureSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	DependStructureSearchInterfaceABC();
	virtual ~DependStructureSearchInterfaceABC(){ ;}

	virtual void destroy(void) = 0;

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC *aGenericDSInterface) = 0;
	//sth Get QueryDescritionInterface missing
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC *aGenericDSInterface) = 0;
	virtual int GetDistance(std::vector< DependStructureSearchResult >&,
				string &) = 0; //sth not the normal interface

	static const UUID myID;
};

//=============================================================================
class DependStructureSearchInterface:
  public DependStructureSearchInterfaceABC
{
public:
	DependStructureSearchInterface(DependStructureSearchTool * aTool);
	virtual ~DependStructureSearchInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC *aGenericDSInterface);
	virtual int GetDistance(std::vector< DependStructureSearchResult >&,
				string &);

	static const UUID myID;
	static const char *myName;

private:
	DependStructureSearchTool *m_SearchTool;

};

//=============================================================================
class DependStructureSearchTool: public Search
{
friend class DependStructureSearchInterface;
public:
	DependStructureSearchTool();
	DependStructureSearchTool(GenericDSInterfaceABC *aDescription);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual DependStructureSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC *aGenericDSInterface);
	virtual int GetDistance(std::vector< DependStructureSearchResult >&,
				string &);

	static const UUID myID;
	static const char * myName;

private:
	static int PenaltySubject;
	static int PenaltyObject;
	static int PenaltyParticle;
	static int PenaltyOther;

	virtual ~DependStructureSearchTool();

	DependStructureSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;
};

class DependStructureSearchResult
{
public:
	DependStructureSearchResult();
	DependStructureSearchResult(const string &, double);
	DependStructureSearchResult(const string &, double, const char *);
	~DependStructureSearchResult(){ ;}
	DependStructureSearchResult &operator=(const DependStructureSearchResult&);
	bool operator<(const DependStructureSearchResult &);
	const string& GetIndex() const {return m_Index;}
	double GetPenalty() const {return m_Penalty;}
	const string&	GetLack() const {return m_Lack;}

private:
	string m_Index;
	double m_Penalty;
	string m_Lack;
};

using std::set;
struct DssCHead;
struct DssSHead;

struct DssHead{
	enum HeadType{
		tSHead,
		tCHead
	};
	enum OpType{
		undef,
		subject,
		object,
		indirectObject,
		predicate,
		expletive,
		nonRestrictive
	};
	DssHead(): opType(DssHead::undef), compound(0){ ;}
	virtual ~DssHead(){ ;}
	virtual HeadType get_type() = 0;
	virtual int compare(DssHead *, set<DssHead*> &, bool) = 0;
	virtual void get_lack(set<DssHead *> &) = 0;
	virtual std::ostream &print_form(std::ostream &) = 0;
	virtual std::ostream &print_node(std::ostream &, int) = 0;
	int compare_cmb(vector<DssHead *> &, vector<DssHead *> &, set<DssHead*> &, bool);
	int sum_penalty(set<DssHead *> &);
	std::ostream &out_penalty(set<DssHead *> &, std::ostream &);
	virtual void get_match(set<DssHead *> &, set<DssHead *> &) = 0;
	void add_lack(set<DssHead *> &, set<DssHead *> &);
	void del_lack(set<DssHead *> &, set<DssHead *> &);

	string				functionWord;
	OpType				opType;
	DssCHead			*compound;

	static int PenaltySubject;
	static int PenaltyObject;
	static int PenaltyParticle;
	static int PenaltyOther;
};

struct DssSHead: public DssHead{
	DssSHead(): DssHead(), conj(false){ ;}
	~DssSHead(){ ;}
	DssHead::HeadType	get_type(){return(DssHead::tSHead);}
	int					compare(DssHead *, set<DssHead*> &, bool);
	void				get_lack(set<DssHead *> &);
	void				get_match(set<DssHead *> &, set<DssHead *> &);
	std::ostream		&print_form(std::ostream &);
	std::ostream		&print_node(std::ostream &, int);

	string				form;
	bool				conj;
	vector<DssHead*>	relation;
};

struct DssCHead: public DssHead{
	DssCHead(): DssHead(){ ;}
	DssHead::HeadType	get_type(){return(DssHead::tCHead);}
	int					compare(DssHead *, set<DssHead*> &, bool);
	void				get_lack(set<DssHead *> &);
	void				get_match(set<DssHead *> &, set<DssHead *> &);
	std::ostream		&print_form(std::ostream &);
	std::ostream		&print_node(std::ostream &, int);

	vector<DssHead *>	head;
};

struct DssPhraseC{
	DssPhraseC(GenericDS &);
	~DssPhraseC();

	void make_phrase(GenericDS &, vector<DssHead *> &);
	void make_nonDepPhrase(GenericDS &, vector<DssHead *> &);
	void make_synthesis(GenericDS &, vector<DssHead *> &);

	int compare(DssPhraseC &);
	int compare(DssPhraseC &, std::ostream &);
	std::ostream &print_node(std::ostream &aOs){
						iHead->print_node(aOs, 0);
						return(aOs);
						}

	vector<DssHead *> array;
	DssSHead * iHead;			// independent word
};

}

//=============================================================================

#endif // __DEPEND_STRUCTURE_SEARCH_H__
