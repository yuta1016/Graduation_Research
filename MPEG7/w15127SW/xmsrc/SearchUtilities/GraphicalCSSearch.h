///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Hawley K. Rising III, Mohammed Zubair Visharam
// Company: Sony Corp.
// From a basis (SemanticSearch) by Ana Belen Benitez Jimenez, Columbia University.
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
// Copyright (c) 2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GraphicalCSSearch.h
//


// Redefine the GraphicalCSSearch files to do SemanticState based MemberFunction
// searches against a property or set of properties in a concept.

#ifndef __GRAPHICALCS_SEARCH_H__
#define __GRAPHICALCS_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

// fix for truncations in Microsoft Visual Studio (comes with Microsoft recommendations!)
#ifdef WIN32
#ifdef _DEBUG
#define const_iterator CIT
#define SemanticEntity SME
#define RelationDS  RDS
#define RelMapType  RMT
#define GraphicalCSSearchTool GCSST
#endif
#endif

#include <string>
#include <list>
#include <map>

//=============================================================================

namespace XM
{

//#ifdef WIN32
// Make the map, list and functional datatype available from the STL std namespace.
	using std::map;
	using std::list;
	using std::string;
//#endif

// Forward Declarations:
class GraphicalCSSearchTool;

//=============================================================================
class GraphicalCSSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  GraphicalCSSearchInterfaceABC();
  virtual ~GraphicalCSSearchInterfaceABC() {};

  virtual void destroy() = 0;

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface) = 0;
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void) = 0;
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  static const UUID myID;
};

//=============================================================================
class GraphicalCSSearchInterface: 
public GraphicalCSSearchInterfaceABC
{
public:
  GraphicalCSSearchInterface(GraphicalCSSearchTool* aTool);
  virtual ~GraphicalCSSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;

private:
  GraphicalCSSearchTool *m_SearchTool;
};

//=============================================================================
class GraphicalCSSearchTool: public Search
{
friend class GraphicalCSSearchInterface;
public:
  GraphicalCSSearchTool();
  GraphicalCSSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual GraphicalCSSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;

private:
  virtual ~GraphicalCSSearchTool();

  GraphicalCSSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;

public:
  // Enumerations and structures for the semantic entities and relations
  // This has been updated to include AgentObject, and change to new names
  // Added SEMANTICBASE for indeterminate types.
  // Also removed PROPERTIES since this cannot occur as a semantic entity
  enum GCS_SemanticEntityType {
	  SEMANTIC=0,
	OBJECT=1,
	PERSONOBJECT=2,
	SEMANTICPLACE=3,
	EVENT=4,
	SEMANTICBASE=5,
	CONCEPT=6,
	SEMANTICSTATE=7,
	SEMANTICTIME=8,
	AGENTOBJECT=9
  };



//----------------------------------------------------------------------
//----------Graph class definitions-------------------------------------

  enum RelatedType
  {
	  ANALYTICMODEL=10,
	  SEGMENT=11,
	  RELATION=12
  };

/*
   SemanticGraphEntityType {
	SEMANTIC,
	OBJECT,
	PERSONOBJECT,
	SEMANTICPLACE,
	EVENT,
	SEMANTICBASE,
	CONCEPT,
	SEMANTICSTATE,
	SEMANTICTIME,
	AGENTOBJECT,
	ANALYTICMODEL,
	SEGMENT,
	RELATION
  };
*/

struct SemanticRelationType
{
	int source;
	int target;
	SemanticRelationType(int src=SEMANTICBASE,
        int tgt=SEMANTICBASE) : source(src),target(tgt) { }
	SemanticRelationType& operator=(const SemanticRelationType& srt);
	static string sRelationTypes[11];
	SemanticRelationType& getRelationType(string s);
};

struct Node
{
	string id;
	string idref;  // current idrefs are strings, within file
   int number;
	int seType;
	Node() { }
	Node& operator=(const Node& n)
	{
		id = n.id;
		idref = n.idref;
		number = n.number;
		seType = n.seType;
		return *this;
	}
};


typedef list<Node> GCSNodeList; 

typedef int (*COMPARATOR)(GCSNodeList::iterator,GCSNodeList::iterator);



#ifdef _DEBUG
#define Edge E
#endif


struct Edge
{
	string id;
	string source;  // these are currently idref, we use strings
	string target;
   int snumber;
   int tnumber;
	SemanticRelationType type;
	string name;
	Edge& operator=(const Edge& e)
	{
		id = e.id;
		source = e.source;
		target = e.target;
		snumber = e.snumber;
		tnumber = e.tnumber;
		type = e.type;
		name = e.name;
		return *this;
	}
};



#ifdef _DEBUG
#define EdgeList EL
#endif

typedef list<Edge> EdgeList;

typedef int (*ADJEDGE)(EdgeList::iterator);

class Graph
{

    string id;
    GCSNodeList nodeList;
    EdgeList edgeList;
    int** Adjacency;
    int*** CompatibilityList;
    int vsize;
    int givsize;
    Graph* GI;
    void clearAdjacency();
    void newAdjacency();
    void clearP(int**& P);
    void newP(int sz,int**& P);
    void copyP(int sz,int**& P,int**& PP);
    void clearCompatibilityList();
    void setAdjacencyMatrix(ADJEDGE adjacencyLabel);
    int graphMatching(Graph& GIP,int*& f,COMPARATOR compare,ADJEDGE adjacencyLabel);
    int backTracking(int i,int*& f);
    int forwardChecking(int i,int* f);

public:
    Graph() : Adjacency(NULL) 
    {
        CompatibilityList = new int**[30]; // should be big enough
        for(int i=0;i<30;i++) CompatibilityList[i]=NULL;
    }
    ~Graph()
    {
        clearAdjacency();
        clearCompatibilityList();
    }
	Graph& operator=(const Graph& g)
	{
		id = g.id;
		GCSNodeList::iterator niter;
		for(niter = ((GCSNodeList)(g.nodeList)).begin();niter!=((GCSNodeList)(g.nodeList)).end();niter++)
		{
			addNode((*niter));
		}
		EdgeList::iterator eiter;
		for(eiter = ((EdgeList)(g.edgeList)).begin();eiter!=((EdgeList)(g.edgeList)).end();eiter++)
		{
			addEdge((*eiter));
		}
		return *this;
	}
    int numNodes() { return nodeList.size(); }
    int numEdges() { return edgeList.size(); }
	Edge* findEdgeByName(string name)
	{
		EdgeList::iterator iter;
		for(iter=edgeList.begin();iter!=edgeList.end();iter++)
		{
			if((*iter).name==name)
				return &(*iter);
		}
		return NULL;
	}
    Graph& setId(char* Id) { id = Id; return *this; }
    Graph& addNode(Node& n);
    Graph& addEdge(Edge& e);
    Graph& addNode(string nid, int stype=SEMANTICBASE);
    Graph& addRefNode(string nid,string ref, int stype=SEMANTICBASE);
	Node*  getNodeByReference(string ref);
	Edge*  getEdgeByReference(string ref);
    Graph& relate(string eid, string id1, string id2, SemanticRelationType type1, string name1);
    Graph& relate(string eid, string id1, string id2, string type1, string name1);
    int isEqualInTypeTo(Graph& gr,int*& f);
    int isSubGraphInTypeOf(Graph& gr,int*& f);
    

	friend std::ostream& operator<<(std::ostream&,Graph&); //LINUX_2011
};




typedef list<Graph> GraphList;

//----------------------------------------------------------------------



private:

  // Constant variables
  // A more elegant solution involving the 
  // use of dinamic memory might be found later.
  static const int	STRING_MAXSIZE;		// maximum size of the strings

  // Names of types of segment entities
  static const string SEMANTIC_ENTITY_TYPES[10];

//  typedef map< string, string> StrMapType;

  
  // Map of strings for semantic entity relations
  // We adopt a more complex version of cataloging
  // relations:
  // Still stored under the "source" field,
  // still keyed there by target, but this is then
  // another map, which is keyed on id.  Since not
  // all relations are assigned id's, we assign them
  // when they do not exist by id="sourcetargetnumber"
  // where number is equal to size() on the target
  // when the relation is stored.

  // Structure for a relation

  typedef struct
  {
	  string sId;
	  string sName;
	  string sType;

  } GCS_RelationDS;

  // Map of relations, keyed by id, used as target fields.

//  typedef map<string,GCS_RelationDS> GCS_RelMapType;

  // Map used for targets in the Semantic Entities

//  typedef map<string,GCS_RelMapType> GCS_TargetMapType;


  // Structure for a semantic entity
  typedef struct {
	string sId;
	int abstractionLevel;
	GCS_SemanticEntityType setType;
	string sLabel;
	string sDefinition;
	string sProperty;
	// The following two fields are solely for using membership functions.
	// In a general tool, this would be in a derived class.
	string sMemberFunction;
	double dMemberFunctionValue;
//	GCS_TargetMapType msEntRelations;
	Graph gGraph;
  } GCS_SemanticEntity;

  // Map of semantic entities
  // The semantic entity ids are the keys of the map
  typedef map< string, GCS_SemanticEntity> GCS_SEMapType;

  // Reads the semantic descriptions and extracts the labels and relations
  // for/between the semantic entities 
  int SemanticDecoding(
			GenericDSInterfaceABC *pdsiDSInt, 
	  		GCS_SEMapType& mseSemanticEntities);

  // Reads the semantic base portion of a description scheme
  // interprets everything but the MediaOccurrence.
  int ReadSemanticBase(GenericDS& dsSemEntity,GCS_SemanticEntity& seSemEntity, GenericDS& dsSubSemEntity, Graph& gGraph );

  
  // Reads any relations coming directly from semantic objects.
  // Essentially puts these relations into the composite graph.
  int ReadSemanticRelation(GenericDS dsEdge,GCS_SemanticEntity& seSemEntity,Graph& gGraph);

// New functions to cover the various semantic entities
int ReadSemanticObject(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadSemanticEvent(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadSemanticTime(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadSemanticPlace(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadSemanticState(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadSemanticConcept(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadSemanticAgentObject(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph);

  // Read one semantic entity and sub-semantic entities
  // putting them into a vector of semantic entities
  int ReadSemanticEntityTree(GenericDS dsSemEntity,
						 GCS_SEMapType& vseSemEntities); 
//						 GCS_SemanticEntity& seSemEntity);

  // added to separate out graph reading tasks (hkr)
int ReadGraphEdge(GenericDS dsEdge,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadGraphNode(GenericDS dsNode,GCS_SEMapType& mseSemEntities,Graph& gGraph);
int ReadGraphProperty(GenericDS dsProperty,GCS_SEMapType& mseSemEntities,Graph& gGraph);



  // Read graph of relations among semantic entities
  int ReadSemanticGraph(GenericDS dsSemGraph, GCS_SEMapType& mseSemEntities,Graph& gGraph);

  // Type of semantic entity
  int TypeOfSemanticEntity(GenericDS dsSemEntity, GCS_SemanticEntityType& setType);

  // Matches the semantic descriptions based on the labels and relations
  // for/between the semantic entities 
  double SemanticMatching(
			GCS_SEMapType mseSemanticEntitiesRef,
			GCS_SEMapType mseSemanticEntitiesQuery);

  // Remove dummy words and signs from two strings and returns
  // and score on how similar the two string are
  double MatchStrings(string str1, string str2);
  // Replaces multiple sub-string for another sub-string in a string
  void ReplaceStrings(string& string, char *strListTypes, ...);
  // Replaces a sub-string for another sub-string in a string
  void ReplaceInString(string& string, char *str1, char *str2);
double GetMembershipDistance(GCS_SEMapType mseSemEntitiesRef,GCS_SEMapType mseSemEntitiesQuery);

/*-------------------------------------------------------------------------
   Structures and functions for validating GraphicalClassificationSchemes
--------------------------------------------------------------------------*/

enum GraphType
{
	ALPHABET=0,
	TEMPLATE=1,
	RULE    =2,
	MORPHISM=3
};


typedef struct
{
	string sAlphabetRef;
	string sId;
	GraphType gtType;
	Graph  gGraph;
} GEl;

typedef map<string,GEl> GTermMap;

typedef struct
{
	string sId;
	string sName;
	string sDefinition;
	GTermMap msGEGraphDefs;
} GTrm;

typedef map<string,GTrm> GCSMap;

typedef struct
{
	string uri;
	string domain;
	bool strict;
	GCSMap msGraphTerms;
}GCS;

//------------------------------------------------------------------------


int GetGraphType(GraphType& type,string str);
int readCSHeader(GenericDS& dsGCS,GCS& graphCS);
int readAlphabetGraph(GenericDS& dsGraph,GEl& graphElement);
int readTemplateGraph(GenericDS& dsGraph,GEl& graphElement);
int readRuleGraph(GenericDS& dsGraph,GEl& graphElement);
int readMorphismGraph(GenericDS& dsGraph,GEl& graphElement);
int readGraphElement(GenericDS& dsGEl,GTrm& graphTerm);
int readGTermHeader(GenericDS& dsSubTerm,GTrm& graphTerm);
int readGraphicalTerm(GenericDS& dsGTerm,GCS& graphCS);
int readGraphicalCS(GenericDS& dsGCS,GCS& graphCS);

//------------End of Graphical Classification Scheme fcns-------------



};

};

#endif //__GRAPHICALCS_SEARCH_H__
