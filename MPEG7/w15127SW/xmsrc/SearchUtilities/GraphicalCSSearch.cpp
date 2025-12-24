//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Hawley K. Rising III, Mohammed Zubair Visharam
// Company: Sony Electronics, Inc.
// From a basis written by Ana Belen Benitez Jimenez, Columbia University
// (in SemanticSearch).
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GraphicalCSSearch.cpp
//

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"



//=============================================================================
	
using namespace XM;
using namespace std; //LINUX_2011

// Update the name of the matching tool:  This file matches by SemanticState, and Membership fcns.
const UUID GraphicalCSSearchInterface::myID = UUID("");
const char *GraphicalCSSearchInterface::myName = "SemanticStateMatchingInterface";

const UUID GraphicalCSSearchTool::myID = UUID("");
const char *GraphicalCSSearchTool::myName = "SemanticStateMatchingTool";

const UUID GraphicalCSSearchInterfaceABC::myID = UUID();

// Give value to constants
const int	GraphicalCSSearchTool::STRING_MAXSIZE= 4096;		// Maximum size of the strings
const string GraphicalCSSearchTool::SEMANTIC_ENTITY_TYPES[10] = {// Names of types of semantic entities
	  "Semantic",
	  "Object",
	  "PersonObject",
	  "SemanticPlace", // updated
	  "Event",
	  "SemanticBase",   // Properties removed, SemanticBase added. Never occurs, need the label.
	  "Concept",
	  "SemanticState",
	  "SemanticTime",
     "AgentObject"   // added
};

//=============================================================================
GraphicalCSSearchInterfaceABC::GraphicalCSSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
GraphicalCSSearchInterface::GraphicalCSSearchInterface(GraphicalCSSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GraphicalCSSearchInterface::~GraphicalCSSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& GraphicalCSSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GraphicalCSSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void GraphicalCSSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int GraphicalCSSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefSemanticDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefSemanticDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* GraphicalCSSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int GraphicalCSSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QuerySemanticDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QuerySemanticDescriptionInterface);
}

//----------------------------------------------------------------------------
double GraphicalCSSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
GraphicalCSSearchTool::GraphicalCSSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
GraphicalCSSearchTool::
GraphicalCSSearchTool(GenericDSInterfaceABC
		      *aQueryDescriptionInterface):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  /* connect Descriptors with Searchs*/
  SetQueryDescriptionInterface(aQueryDescriptionInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GraphicalCSSearchTool::~GraphicalCSSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& GraphicalCSSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GraphicalCSSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool GraphicalCSSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool GraphicalCSSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int GraphicalCSSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefSemanticDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefSemanticDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* GraphicalCSSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  GraphicalCSSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQuerySemanticDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQuerySemanticDescriptionInterface;
  
  /* could also look totally different if
     aQuerySemanticDescriptionInterface is not of Semantic node*/

  return 0;
}

//----------------------------------------------------------------------------
double GraphicalCSSearchTool::GetDistance(void)
{
  double dist = 0.0;
  GCS_SEMapType mseSemEntitiesRef, mseSemEntitiesQuery;
//	For Testing GraphicalClassificationSchemes
  GCS graphCS;

  // Check it's all set up ok
  if(m_RefDescriptionInterface==NULL) {
	  return DBL_MAX;
  }
  if(m_QueryDescriptionInterface==NULL) {
	  return DBL_MAX;
  }

  if(strcmp(m_RefDescriptionInterface->GetName(),
	  "GenericDSInterface") != 0) {
		return DBL_MAX;
  }
  if(strcmp(m_QueryDescriptionInterface->GetName(),
	  "GenericDSInterface") != 0) { 
	  return DBL_MAX;
  }

  // Read the GraphicalClassificationScheme if it is in the Query document
  GenericDS dsRootCS = m_QueryDescriptionInterface->GetFirstChild();
  // this should be dummyroot
  GenericDS dsGraphCS = dsRootCS.GetFirstChild();
  int hasGraphInfo = 0;
  if(dsGraphCS.GetDSName()=="GraphicalClassificationScheme")
  {
	  fprintf(stderr,"GraphicalClassificationScheme found, analyzing...\n");
	  if(readGraphicalCS(dsGraphCS,graphCS)==0) hasGraphInfo=1;
  }
  // Read labels and relations among semantic entities for query and reference
  // modified to be more debuggable.
  int retval = SemanticDecoding(m_RefDescriptionInterface,mseSemEntitiesRef);
  retval    += SemanticDecoding(m_QueryDescriptionInterface,mseSemEntitiesQuery);
//  retval    += SemanticDecoding(m_QueryDescriptionInterface,mseSemEntitiesQuery);
  if(retval!=0) dist = DBL_MAX;

  // find gGraph in the Semantic decoding for the referent.
  GCS_SEMapType::iterator iter;
  for(iter=mseSemEntitiesRef.begin();iter!=mseSemEntitiesRef.end();iter++)
  {
	  if((*iter).second.gGraph.numEdges()!=0) break;
  }
  // Use the graph to determine whether a membership function is present.
  // We compare all template graphs for a subgraph match with the graph
  // in the instance.  If any template graph matches, and it has a 
  // membership function, then use the membership function distance,
  // otherwise use label matching.
  // In the real world, we would use the term id to determine this,
  // we are just exercising the software and validating the scheme.
  int hasMembershipFunction = 0;
  if(iter!=mseSemEntitiesRef.end())
  {
	  Graph gGraph=(*iter).second.gGraph;
	  // do a compare to all template graphs in the GCS
	  GCSMap::iterator msgiter;
	  for(msgiter=graphCS.msGraphTerms.begin();msgiter!=graphCS.msGraphTerms.end();msgiter++)
	  {
		  GTermMap::iterator gtiter;
		  for(gtiter=(*msgiter).second.msGEGraphDefs.begin();
			  gtiter!=(*msgiter).second.msGEGraphDefs.end(); gtiter++)
			  {
				GraphType type = (*gtiter).second.gtType;
				Graph sGraph = (*gtiter).second.gGraph;
				if(type==TEMPLATE)
				{
					int* f;
					if(sGraph.isSubGraphInTypeOf(gGraph,f))
					{
						// we have found a match
						string s = "isMembershipFunctionFor";
						if(sGraph.findEdgeByName(s)!=NULL)
							hasMembershipFunction = 1;
					}
				}
			  }
				
	  }
  }

  if ( dist < DBL_MAX) {
	// Match labels and relations among semantic entities for query and reference
	  if(hasMembershipFunction) dist = GetMembershipDistance(mseSemEntitiesRef, mseSemEntitiesQuery);
	  else dist = SemanticMatching(mseSemEntitiesRef, mseSemEntitiesQuery);
  }


  // All done
  return dist;
}

//----------------------------------------------------------------------------
//---------------Graph class--------------------------------------------------
// These functions are for adjusting the type of comparisons
// done to match the graphs.

// Functions which decide how nodes are matched
inline int sameType(GraphicalCSSearchTool::GCSNodeList::iterator a,GraphicalCSSearchTool::GCSNodeList::iterator b)
{ return ((*a).seType == (*b).seType); }

// Functions which decide how edges are matched
inline int onlyCheckNodes(GraphicalCSSearchTool::EdgeList::iterator iter)
{ return 1; }



GraphicalCSSearchTool::SemanticRelationType& GraphicalCSSearchTool::SemanticRelationType::operator=(const SemanticRelationType& srt)
{
    source = srt.source;
    target = srt.target;
    return *this;
}
GraphicalCSSearchTool::SemanticRelationType& GraphicalCSSearchTool::SemanticRelationType::getRelationType(string s)
{
    SemanticRelationType* retval = new SemanticRelationType;
    int i;
    for(i=0;i<11;i++)
        if(s==sRelationTypes[i]) break;
        if(i==11) return *retval;
        switch(i)
        {
        case 0:
            retval->source =  OBJECT; retval->target = OBJECT; break;
        case 1:
            retval->source =  OBJECT; retval->target = EVENT; break;
        case 2:
            retval->source =  EVENT; retval->target = EVENT; break;
        case 3:
            break; // default is SEMANTICBASE, SEMANTICBASE
        case 4:
            retval->source =  SEMANTIC; retval->target = SEMANTIC; break;
        case 5:
            retval->source =  SEMANTIC; retval->target = SEMANTICBASE; break;
        case 6:
            retval->source =  EVENT; retval->target = SEMANTICTIME; break;
        case 7:
            retval->source =  EVENT; retval->target = SEMANTICPLACE; break;
        case 8:
            retval->source =  SEMANTICBASE; retval->target = ANALYTICMODEL; break;
        case 9:
            retval->source =  SEGMENT; retval->target = SEMANTICBASE; break;
        case 10:
            retval->source =  RELATION; retval->target = RELATION; break;
        };
        return *retval;
}

void GraphicalCSSearchTool::Graph::clearAdjacency()
{
    if(Adjacency==NULL) return;
    int vsize = nodeList.size();
    for(int i=0;i<vsize;i++)
        delete[] Adjacency[i];
    delete[] Adjacency; Adjacency = NULL;
}
void GraphicalCSSearchTool::Graph::newAdjacency()
{
    int i,vsize = nodeList.size();
    Adjacency = new int*[vsize];
    for( i=0;i<vsize;i++)
    {
        Adjacency[i] = new int[vsize];
        for(int j=0;j<vsize;j++)
            Adjacency[i][j] = 0;
    }
}
void GraphicalCSSearchTool::Graph::clearP(int**& P)
{
    if(P==NULL) return;
    int vsize = nodeList.size();
    for(int i=0;i<vsize;i++)
        delete[] P[i];
    delete[] P; P = NULL;
}
void GraphicalCSSearchTool::Graph::newP(int sz,int**& P)
{
    int i,vsize = nodeList.size();
    P = new int*[vsize];
    for( i=0;i<vsize;i++)
    {
        P[i] = new int[sz];
        for(int j=0;j<sz;j++)
            P[i][j] = 0;
    }
}
void GraphicalCSSearchTool::Graph::copyP(int sz,int**& P,int**& PP)
{
    newP(sz,PP);
    int vsize = nodeList.size();
    for(int i=0;i<vsize;i++)
        for(int j=0;j<sz;j++)
            PP[i][j] = P[i][j];
}
void GraphicalCSSearchTool::Graph::clearCompatibilityList()
{
    for(int i=0;i<30;i++) clearP(CompatibilityList[i]);
}

GraphicalCSSearchTool::Graph& GraphicalCSSearchTool::Graph::addNode(Node& n)
{
    nodeList.insert(nodeList.end(),1,n);
    return *this;
}
GraphicalCSSearchTool::Graph& GraphicalCSSearchTool::Graph::addEdge(Edge& e)
{
    edgeList.insert(edgeList.end(),1,e);
    return *this;
}
GraphicalCSSearchTool::Graph& GraphicalCSSearchTool::Graph::addNode(string nid, int stype)
{
    Node node;
    node.id = nid;
    node.seType = stype;
    node.number = nodeList.size();
    addNode(node);
    return *this;
}
GraphicalCSSearchTool::Graph& GraphicalCSSearchTool::Graph::addRefNode(string nid,string ref, int stype)
{
    Node node;
    node.id = nid;
    node.idref = ref;
    node.seType = stype;
    node.number = nodeList.size();
    addNode(node);
    return *this;
}

GraphicalCSSearchTool::Node* GraphicalCSSearchTool::Graph::getNodeByReference(string ref)
{
	GCSNodeList::iterator iter;
	for(iter=nodeList.begin();iter!=nodeList.end();iter++)
		if((*iter).id==ref) break;
	if(iter==nodeList.end()) return NULL;
	return &(*iter);
}
GraphicalCSSearchTool::Edge* GraphicalCSSearchTool::Graph::getEdgeByReference(string ref)
{
	EdgeList::iterator iter;
	for(iter=edgeList.begin();iter!=edgeList.end();iter++)
		if((*iter).id==ref) break;
	if(iter==edgeList.end()) return NULL;
	return &(*iter);
}
GraphicalCSSearchTool::Graph& GraphicalCSSearchTool::Graph::relate(string eid, string id1, string id2, GraphicalCSSearchTool::SemanticRelationType type1, string name1)
{
    GCSNodeList::iterator iter1,iter2;
    int src,tgt;
    for(iter1=nodeList.begin(),src=0;iter1!=nodeList.end();iter1++,src++)
        if((*iter1).id==id1) break;
        for(iter2=nodeList.begin(),tgt=0;iter2!=nodeList.end();iter2++,tgt++)
            if((*iter2).id==id2) break;
            Edge edge;
            edge.id = eid;
            edge.source = id1;
            edge.snumber = src;
            edge.target = id2;
            edge.tnumber = tgt;
            edge.type = type1;
            edge.name = name1;
            addEdge(edge);
            return *this;
}

GraphicalCSSearchTool::Graph& GraphicalCSSearchTool::Graph::relate(string eid, string id1, string id2, string type1, string name1)
{
    GCSNodeList::iterator iter1,iter2;
    int src,tgt;
    for(iter1=nodeList.begin(),src=0;iter1!=nodeList.end();iter1++,src++)
        if((*iter1).id==id1) break;
        for(iter2=nodeList.begin(),tgt=0;iter2!=nodeList.end();iter2++,tgt++)
            if((*iter2).id==id2) break;
            Edge edge;
            edge.id = eid;
            edge.source = id1;
            edge.snumber = src;
            edge.target = id2;
            edge.tnumber = tgt;
            edge.type = edge.type.getRelationType(type1);
            edge.name = name1;
            addEdge(edge);
            return *this;
}


int GraphicalCSSearchTool::Graph::isEqualInTypeTo(GraphicalCSSearchTool::Graph& gr,int*& f)
{
    // Equality requires that all nodes and edges match.
    if(gr.edgeList.size()!=edgeList.size() || 
       gr.nodeList.size()!=nodeList.size()) return 0;
    return graphMatching(gr,f,sameType,onlyCheckNodes);
}
int GraphicalCSSearchTool::Graph::isSubGraphInTypeOf(GraphicalCSSearchTool::Graph& gr,int*& f)
{
    return graphMatching(gr,f,sameType,onlyCheckNodes);
}



void GraphicalCSSearchTool::Graph::setAdjacencyMatrix(ADJEDGE adjacencyLabel)
{
    // Assume that V is the vertex list, E is the edgelist,
    // and that V.size() is the number of vertices.
    clearAdjacency();
    newAdjacency();
    EdgeList::iterator iter;
    
    for(iter=edgeList.begin();iter!=edgeList.end();iter++)
    {
        int i=(*iter).snumber;
        int j=(*iter).tnumber;
        Adjacency[i][j] = adjacencyLabel(iter);
    }
}


int GraphicalCSSearchTool::Graph::graphMatching(GraphicalCSSearchTool::Graph& GIP,int*& f,COMPARATOR compare,ADJEDGE adjacencyLabel)
{
    GI = &GIP;
    vsize = nodeList.size();
    givsize = GIP.nodeList.size();
    // Set up the adjacency matrices.
    setAdjacencyMatrix(adjacencyLabel);
    GI->setAdjacencyMatrix(adjacencyLabel);
    
    f = new int[vsize];
    for(int i=0;i<vsize;i++) f[i]=0;
    
    clearCompatibilityList();
    newP(givsize,CompatibilityList[0]);
    int** P = CompatibilityList[0];
    
    GCSNodeList::iterator iteri,iterj;
    for(iteri=nodeList.begin();iteri!=nodeList.end();iteri++)
    {
        for(iterj=GI->nodeList.begin();iterj!=GI->nodeList.end();iterj++)
        {
//            if((*iteri).seType == (*iterj).seType) 
            if((*compare)(iteri,iterj)) 
                P[(*iteri).number][(*iterj).number] = 1;
        }
    }
	int retval = backTracking(0,f); //recursive call
	clearAdjacency();
	GI->clearAdjacency();
    return retval;
}

int GraphicalCSSearchTool::Graph::backTracking(int i,int*& f)
{
    int** P = CompatibilityList[i];
    
    if(i >= vsize) return 1;
    for(int j=0;j<givsize;j++)
    {
        if(P[i][j]==1)
        {
            f[i] = j;
            clearP(CompatibilityList[i+1]);
            copyP(givsize,P,CompatibilityList[i+1]);
            int** PP = CompatibilityList[i+1];
            for(int k=i+1;k<vsize;k++)
                PP[k][j] = 0;
            if(forwardChecking(i,f)) 
            { 
                if(backTracking(i+1,f)) return 1; 
            }
            else f[i] = 0;
            
        }
    }
    return 0;
}

int GraphicalCSSearchTool::Graph::forwardChecking(int i,int* f)
{
    int** P = CompatibilityList[i+1]; // We want "PP" from the calling routine, here.
    int k;

    // we made sure there were adjacency matrices at the beginning.
    for(k=i+1;k<vsize;k++)
    {
        int j;
	for(j=0;j<givsize;j++)
        {
            if(P[k][j]==1)
            {
                int l;
		for(l=0;l<=i;l++)
                {
                    // Assume AM is the GM adjacency matrix
                    // and AI is the GI adjacency matrix
                    if(((Adjacency[k][l]==1) && (GI->Adjacency[j][f[l]]==0)) ||
                        ((Adjacency[k][l]==0) && (GI->Adjacency[j][f[l]]==1)))
                    {
                        P[k][j] = 0; break;
                    }
                }
            }
        }
    }
    
    for(k=0;k<vsize;k++)
    {
        int acc = 0;
	int j;
        for(j=0;j<givsize;j++)
            acc += P[k][j];
        if(acc==0) return 0;
    }
    return 1;
}


string GraphicalCSSearchTool::SemanticRelationType::sRelationTypes[11] = 
        { "ObjectObject","ObjectEvent","EventEvent","SemanticBaseSemanticBase",
			 "SemanticSemantic","SemanticSemanticBase","EventSemanticTime","EventSemanticPlace",
			 "SemanticBaseAnalyticModel","SegmentSemanticBase","RelationRelation" };

//----------------------------------------------------------------------------

/*----------------------------------------------------------------------

    Functions for validating graphical classification schemes

----------------------------------------------------------------------*/

int GraphicalCSSearchTool::GetGraphType(GraphType& type,string str)
{
	string graphTypes[4];
	int i;

	graphTypes[0] = "AlphabetGraph";
	graphTypes[1] = "TemplateGraph";
	graphTypes[2] = "RuleGraph";
	graphTypes[3] = "MorphismGraph";
	for(i=0;i<4;i++)
	{
		if(str==graphTypes[i]) break;
	}
	if(i==4) return 1;
	type = (GraphType)i;
	return 0;
}

// read the GraphicalClassificationScheme header
int GraphicalCSSearchTool::readCSHeader(GenericDS& dsGCS,GCS& graphCS)
{
	if(dsGCS.GetDSName()!="GraphicalClassificationScheme")
	{
		fprintf(stderr,"not a graphical classification scheme");
		return 1;
	}
	dsGCS.GetTextAttribute("uri",graphCS.uri);
	dsGCS.GetTextAttribute("domain",graphCS.domain);
	dsGCS.GetBoolAttribute("strict",graphCS.strict);
	return 0;
}

int GraphicalCSSearchTool::readAlphabetGraph(GenericDS& dsGraph,GEl& graphElement)
{
	if(dsGraph.GetDSName()!="AlphabetGraph") return 1;
	string gid;
	dsGraph.GetTextAttribute("id",gid);
	GenericDS dsGElement = dsGraph.GetFirstChild();
	while(!dsGElement.isNull())
	{
		if(dsGElement.GetDSName()=="Node")
		{
			string sId;
			int en;

			dsGElement.GetTextAttribute("id",sId);
			char* entities[13] = { "semantic","object","person","place","event","base",
							   "concept","state","time","agent","model","segment","relation" };
			char* str = new char[sId.length() + 1];
			strcpy(str,sId.c_str());

			for(en=0;en<13;en++)
				if(strspn(str,entities[en])==strlen(entities[en])) break;
			if(en==13) graphElement.gGraph.addNode(sId);
			else graphElement.gGraph.addNode(sId,en);
		}
		else if(dsGElement.GetDSName()=="Relation")
		{
			string eid;
			string src;
			string tgt;
			string typ;
			string nam;
			dsGElement.GetTextAttribute("id",eid);
			dsGElement.GetTextAttribute("source",src);
			dsGElement.GetTextAttribute("target",tgt);
			dsGElement.GetTextAttribute("type",typ);
			dsGElement.GetTextAttribute("name",nam);
			graphElement.gGraph.relate(eid,src,tgt,typ,nam);
		}
		// alphabet graphs (currently) only nodes and relations
		dsGElement = dsGElement.GetNextSibling();
	}
	return 0;
}
int GraphicalCSSearchTool::readTemplateGraph(GenericDS& dsGraph,GEl& graphElement)
{
	if(dsGraph.GetDSName()!="TemplateGraph") return 1;
	dsGraph.GetTextAttribute("id",graphElement.sId);
	GenericDS dsGElement = dsGraph.GetFirstChild();
	while(!dsGElement.isNull())
	{
		if(dsGElement.GetDSName()=="Node")
		{
			string sId;
			int en;

			dsGElement.GetTextAttribute("id",sId);
			char* entities[13] = { "semantic","object","person","place","event","base",
							   "concept","state","time","agent","model","segment","relation" };
			char* str = new char[sId.length() + 1];
			strcpy(str,sId.c_str());
			for(en=0;en<13;en++)
				if(strspn(str,entities[en])==strlen(entities[en])) break;
			if(en==13) graphElement.gGraph.addNode(sId);
			else graphElement.gGraph.addNode(sId,en);
		}
		else if(dsGElement.GetDSName()=="Relation")
		{
			string eid;
			string src;
			string tgt;
			string typ;
			string nam;
			dsGElement.GetTextAttribute("id",eid);
			dsGElement.GetTextAttribute("source",src);
			dsGElement.GetTextAttribute("target",tgt);
			dsGElement.GetTextAttribute("type",typ);
			dsGElement.GetTextAttribute("name",nam);
			graphElement.gGraph.relate(eid,src,tgt,typ,nam);
		}
		// template graphs (currently) only nodes and relations
		dsGElement = dsGElement.GetNextSibling();
	}
	return 0;
}
int GraphicalCSSearchTool::readRuleGraph(GenericDS& dsGraph,GEl& graphElement)
{
	if(dsGraph.GetDSName()!="RuleGraph") return 1;
	string gid;
	dsGraph.GetTextAttribute("id",gid);
	GenericDS dsGElement = dsGraph.GetFirstChild();
	while(!dsGElement.isNull())
	{
		if(dsGElement.GetDSName()=="Node")
		{
			string sId;
			int en;

			dsGElement.GetTextAttribute("id",sId);
			char* entities[13] = { "semantic","object","person","place","event","base",
							   "concept","state","time","agent","model","segment","relation" };
			char* str = new char[sId.length() + 1];
			strcpy(str,sId.c_str());
			for(en=0;en<13;en++)
				if(strspn(str,entities[en])==strlen(entities[en])) break;
			if(en==13) graphElement.gGraph.addNode(sId);
			else graphElement.gGraph.addNode(sId,en);
		}
		else if(dsGElement.GetDSName()=="Relation")
		{
			string eid;
			string src;
			string tgt;
			string typ;
			string nam;
			dsGElement.GetTextAttribute("id",eid);
			dsGElement.GetTextAttribute("source",src);
			dsGElement.GetTextAttribute("target",tgt);
			dsGElement.GetTextAttribute("type",typ);
			dsGElement.GetTextAttribute("name",nam);
			graphElement.gGraph.relate(eid,src,tgt,typ,nam);
		}
		// alphabet graphs (currently) only nodes and relations
		dsGElement = dsGElement.GetNextSibling();
	}
	return 0;
}
int GraphicalCSSearchTool::readMorphismGraph(GenericDS& dsGraph,GEl& graphElement)
{
	if(dsGraph.GetDSName()!="MorphismGraph") return 1;
	string gid;
	dsGraph.GetTextAttribute("id",gid);
	GenericDS dsGElement = dsGraph.GetFirstChild();
	while(!dsGElement.isNull())
	{
		if(dsGElement.GetDSName()=="Node")
		{
			string sId;
			int en;

			dsGElement.GetTextAttribute("id",sId);
			char* entities[13] = { "semantic","object","person","place","event","base",
							   "concept","state","time","agent","model","segment","relation" };
			char* str = new char[sId.length() + 1];
			strcpy(str,sId.c_str());
			for(en=0;en<13;en++)
				if(strspn(str,entities[en])==strlen(entities[en])) break;
			if(en==13) graphElement.gGraph.addNode(sId);
			else graphElement.gGraph.addNode(sId,en);
		}
		else if(dsGElement.GetDSName()=="Relation")
		{
			string eid;
			string src;
			string tgt;
			string typ;
			string nam;
			dsGElement.GetTextAttribute("id",eid);
			dsGElement.GetTextAttribute("source",src);
			dsGElement.GetTextAttribute("target",tgt);
			dsGElement.GetTextAttribute("type",typ);
			dsGElement.GetTextAttribute("name",nam);
			graphElement.gGraph.relate(eid,src,tgt,typ,nam);
		}
		// alphabet graphs (currently) only nodes and relations
		dsGElement = dsGElement.GetNextSibling();
	}
	return 0;
}

// Graphical Terms consist of a sequence of graphs.
// before any graph, a reference to an alphabet graph may occur.
// This reads as many of such references are there are, then
// exactly one graph.
int GraphicalCSSearchTool::readGraphElement(GenericDS& dsGEl,GTrm& graphTerm)
{
	GEl graphElement;
//	dsGEl = dsGEl.GetNextSibling();
	GraphType type;
	while(!dsGEl.isNull())
	{
		if(GetGraphType(type,dsGEl.GetDSName())!=0)
		{
			// test to see if it is alphabet ref
			if(dsGEl.GetDSName()=="AlphabetGraphRef")
			{
				dsGEl.GetTextValue(graphElement.sAlphabetRef);
				dsGEl = dsGEl.GetNextSibling();
				continue;
			}
			else return 1;
		}
		graphElement.gtType = type;
		switch(type)
		{
		case ALPHABET:
			if(readAlphabetGraph(dsGEl,graphElement)!=0) return 1;
			graphTerm.msGEGraphDefs.insert(GTermMap::value_type(graphElement.sId,graphElement));
			return 0;
		case TEMPLATE:
			if(readTemplateGraph(dsGEl,graphElement)!=0) return 1;
			graphTerm.msGEGraphDefs.insert(GTermMap::value_type(graphElement.sId,graphElement));
			return 0;
		case RULE:
			if(readRuleGraph(dsGEl,graphElement)!=0) return 1;
			graphTerm.msGEGraphDefs.insert(GTermMap::value_type(graphElement.sId,graphElement));
			return 0;
		case MORPHISM:
			if(readMorphismGraph(dsGEl,graphElement)!=0) return 1;
			graphTerm.msGEGraphDefs.insert(GTermMap::value_type(graphElement.sId,graphElement));
			return 0;
		default:
			return 1;
		};
		dsGEl = dsGEl.GetNextSibling();
	}
	return 0;  // only exits here if whole rest of term is references. 
}

// Read Graphical Term.  We do not use recursion or relations in this implementation
int GraphicalCSSearchTool::readGTermHeader(GenericDS& dsSubTerm,GTrm& graphTerm)
{
	while(!dsSubTerm.isNull())
	{
		string str;
		if(dsSubTerm.GetDSName()!="Name") break;
		dsSubTerm.GetTextValue(str);
		graphTerm.sName.append(" ");
		graphTerm.sName.append(str);
		dsSubTerm = dsSubTerm.GetNextSibling();
	}
	while(!dsSubTerm.isNull())
	{
		string str;
		if(dsSubTerm.GetDSName()!="Definition") break;
		dsSubTerm.GetTextValue(str);
		graphTerm.sDefinition.append(" ");
		graphTerm.sDefinition.append(str);
		dsSubTerm = dsSubTerm.GetNextSibling();
	}
	return 0;
}

int GraphicalCSSearchTool::readGraphicalTerm(GenericDS& dsGTerm,GCS& graphCS)
{
	if(dsGTerm.GetDSName()!="GraphicalTerm") return 1;
	GTrm graphTerm;
	dsGTerm.GetTextAttribute("termId",graphTerm.sId);
	GenericDS dsSubTerm = dsGTerm.GetFirstChild();
	// read the header
	readGTermHeader(dsSubTerm,graphTerm);
	while(!dsSubTerm.isNull())
	{
		if(readGraphElement(dsSubTerm,graphTerm)!=0) return 1;
		dsSubTerm = dsSubTerm.GetNextSibling();
	}
	graphCS.msGraphTerms.insert(GCSMap::value_type(graphTerm.sId,graphTerm));
	return 0;
}

int GraphicalCSSearchTool::readGraphicalCS(GenericDS& dsGCS,GCS& graphCS)
{
	if(dsGCS.GetDSName()!="GraphicalClassificationScheme") return 1;
	// read the header
	readCSHeader(dsGCS,graphCS);
	GenericDS dsGTerm = dsGCS.GetFirstChild();
	while(!dsGTerm.isNull())
	{
		if(readGraphicalTerm(dsGTerm,graphCS)!=0) return 1;
		dsGTerm = dsGTerm.GetNextSibling();
	}
	return 0;
}

//------------End of GCS validating functions---------------------------------



//----------------------------------------------------------------------------
int GraphicalCSSearchTool::
	SemanticDecoding(
		GenericDSInterfaceABC *pdsiDSInt, 
		GCS_SEMapType& mseSemEntities)
{
  // Variable declaration
  GCS_SemanticEntity seSemEntity;
  GenericDS dsSemantic, dsSemGraph;
  


  /* ------------------------------------------------------------------------
	   Read and validate the Semantic DS description
     ------------------------------------------------------------------------ */

  dsSemantic = pdsiDSInt->GetFirstChild(); // kluge to get two roots read
  while(!dsSemantic.isNull()) // skip the graphical cs header
  {
	  if(dsSemantic.GetDSName()=="Semantic") break;
		  if(dsSemantic.GetDSName()=="DummyRoot")
		  {
				dsSemantic = dsSemantic.GetFirstChild();
				if(dsSemantic.GetDSName()!="Semantic")
					dsSemantic = dsSemantic.GetNextSibling();
		  }

  }
  if (dsSemantic.isNull()) {
    fprintf(stderr, "Null Semantic\n");
	return 1;
  }
  if (dsSemantic.GetDSName() != "Semantic") {
	fprintf(stderr, "Wrong DS name: Semantic expected\n");
	return 1;
  }


  /* ------------------------------------------------------------------------
	   Read and validate all the semantic entities
	   Check at least one semantic entity was found
     ------------------------------------------------------------------------ */

  if ( ReadSemanticEntityTree(dsSemantic, mseSemEntities) != 0 ) {
	fprintf(stderr, "Error reading Semantic DS\n");
	return 1;
  }


  return 0;
}

// Function to read Relations that are sourced in a Semantic entity rather than in a graph
int GraphicalCSSearchTool::ReadSemanticRelation(GenericDS dsEdge,GCS_SemanticEntity& seSemEntity,Graph& gGraph)
{
	// relations in semantic entities do not have sources (implicit)
	string sSource,sTarget, sName, sType, sId;
	GCS_RelationDS rRelation;

	sSource = seSemEntity.sId;

	dsEdge.GetTextAttribute("name", sName);
	dsEdge.GetTextAttribute("target", sTarget);

	// relations do not necessarily have id's set, if not give it an id
	if(dsEdge.GetTextAttribute("id", sId) != 0)
	{
//		int size = 0; //seSemEntity.msEntRelations[sTarget].size();
/*		char *sz; //sth why not char sz[33];
		sz = itoa(size,sz,3);//sth sz has no memory allocated,
		//sth itoa not defined in ANSI C (Unix)
		//sth itoa(value, string, radix) => radix is 3??*/
		//sth size=0 => string = "0"*/
		char sz[33]="0";
		string relno(sz);
		sId = sSource + sTarget + relno;
	}
	dsEdge.GetTextAttribute("xsi:type", sType);
	rRelation.sId = sId;
	rRelation.sName = sName;
	rRelation.sType = sType;

//	seSemEntity.msEntRelations[sTarget].insert(RelMapType::value_type(sId,rRelation));
    return 0;
}

/*-------------------------------------------------------------------------
   Read the semantic base portion of a semantic description scheme
   record the labels, definitions, properties, relations, skip the MediaOccurrences
-------------------------------------------------------------------------*/

int GraphicalCSSearchTool::ReadSemanticBase(GenericDS& dsSemEntity,GCS_SemanticEntity& seSemEntity, GenericDS& dsSubSemEntity, Graph& gGraph )
{
	// Variable declaration
	GenericDS dsLabel, ds, dsDefinition, dsProperty, dsMediaOccurrence, dsRelation;
	string sText;

    /* ------------------------------------------------------------------------
  	     Check for semantic entity and attribute of semantic entity
       ------------------------------------------------------------------------ */

	// Check whether it is a semantic entity
	if ( TypeOfSemanticEntity(dsSemEntity, seSemEntity.setType) != 0) {
	  return 1;
	}

	// Check wether the semantic entity has an id attribute
	if ( dsSemEntity.GetTextAttribute("id", seSemEntity.sId) != 0) {
	  return 1;
	}


   /* ------------------------------------------------------------------------
  	     Read all the Labels (there must be at least one label)
		 Labels are only expected to contain FreeTerms
       ------------------------------------------------------------------------ */

    dsLabel = dsSemEntity.GetFirstChild();
	// this could be an abstraction level, we check:
	if(dsLabel.GetDSName() == "AbstractionLevel")
	{
		dsLabel.GetIntAttribute("dimension",seSemEntity.abstractionLevel);
		dsLabel = dsLabel.GetNextSibling();
	}
	else seSemEntity.abstractionLevel = -1; // use -1 to indicate  "concrete"

    if (dsLabel.GetDSName() != "Label") {
      fprintf(stderr, "Wrong DS name: expected Label expected but found %s\n", 
		  dsLabel.GetDSName().c_str());
	    return 1;
	}
	seSemEntity.sLabel.append(" ");
	while ( !dsLabel.isNull() ) 
	{
	  if (dsLabel.GetDSName() != "Label") {
		break;
	  }
	  if (dsLabel.GetNumberOfChildren() != 1) {
		  fprintf(stderr, "Wrong number of elements in Label: %d\n", 
			  dsLabel.GetNumberOfChildren());
		  return 1;		
	  }
	  ds = dsLabel.GetFirstChild();
	  if (ds.GetDSName() != "FreeTerm") {
		  fprintf(stderr, "Wrong DS name: expected FreeTerm but found %s\n", 
			  ds.GetDSName().c_str());
		  return 1;
	  }
	  if (ds.GetTextValue(sText) != 0) {
	    cerr << "Wrong text value for FreeTerm:" << sText << endl;
//	    fprintf(stderr, "Wrong text value for FreeTerm: %s\n", sText);
	    return 1;
	  }

	  seSemEntity.sLabel.append(sText);
	  seSemEntity.sLabel.append(" ");

	  dsLabel = dsLabel.GetNextSibling();
	}


    /* ------------------------------------------------------------------------
  	     Read all the Definitions (there may be none)
		 Definition are only expected to contain one FreeText
       ------------------------------------------------------------------------ */

    dsDefinition = dsLabel;
	while ( !dsDefinition.isNull() ) {
	  if ( dsDefinition.GetDSName() != "Definition" ) {
		  break;
	  }
	  seSemEntity.sDefinition.append(" ");

	  ds = dsDefinition.GetFirstChild();
	  if (ds.GetDSName() != "FreeText") {
		  fprintf(stderr, "Wrong DS name: expected FreeText but found %s\n", 
			  ds.GetDSName().c_str());
		  return 1;
	  }
	  if (ds.GetTextValue(sText) != 0) {
	    cerr << "Wrong text value for FreeTerm:" << sText << endl;
//	    fprintf(stderr, "Wrong text value for FreeText: %s\n", sText);
	    return 1;
	  }

	  seSemEntity.sDefinition.append(sText);
	  seSemEntity.sDefinition.append(" ");

	  dsDefinition = dsDefinition.GetNextSibling();
	}

    /* ------------------------------------------------------------------------
  	     Read all the Properties (there may be none)
		 Properties are only expected here to contain a FreeTerm
       ------------------------------------------------------------------------ */

    dsProperty = dsDefinition;
	while ( !dsProperty.isNull() ) {
	  if ( dsProperty.GetDSName() != "Property" ) {
		  break;
	  }
	  seSemEntity.sProperty.append(" ");

	  ds = dsProperty.GetFirstChild();
	  if (ds.GetDSName() != "FreeTerm") {
		  fprintf(stderr, "Wrong DS name: expected FreeTerm but found %s\n", 
			  ds.GetDSName().c_str());
		  return 1;
	  }
	  if (ds.GetTextValue(sText) != 0) {
	    cerr << "Wrong text value for FreeTerm:" << sText << endl;
//	    fprintf(stderr, "Wrong text value for FreeTerm: %s\n", sText);
	    return 1;
	  }

	  seSemEntity.sProperty.append(sText);
	  seSemEntity.sProperty.append(" ");

	  dsProperty = dsProperty.GetNextSibling();
	}

	dsMediaOccurrence = dsProperty;

    /* ------------------------------------------------------------------------
  	     Read all the MediaOccurrences (there may be none), we skip them here.
       ------------------------------------------------------------------------ */


	while ( !dsMediaOccurrence.isNull() ) {
	  if ( dsMediaOccurrence.GetDSName() != "MediaOccurrence" ) {
		  break;
	  }

	  dsMediaOccurrence = dsMediaOccurrence.GetNextSibling();
	}


    /* ------------------------------------------------------------------------
  	     Read all the Relations (there may be none)
		 Relations have only a target, and we need to generate id's for some.
       ------------------------------------------------------------------------ */

    dsRelation = dsMediaOccurrence;

	while ( !dsRelation.isNull() ) {
	  if ( dsRelation.GetDSName() != "Relation" ) {
		  break;
	  }

	  if(ReadSemanticRelation(dsRelation,seSemEntity,gGraph)!=0)
	  {
		  fprintf(stderr,"Problem reading semantic relations");
		  return 1;
	  }

	  dsRelation = dsRelation.GetNextSibling();
	}

	// This ends reading the "SemanticBase" portion of the DS.  Note that we
	// did skip MediaOccurrence (not used here for search task).
	dsSubSemEntity = dsRelation;
	return 0;
}
//----------------------------------------------------------------------------
// Break down the task by semantic entity.
int GraphicalCSSearchTool::ReadSemanticObject(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    GCS_SemanticEntityType type;
    
    // Read the SemanticBase part first:
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);
	string sSubId = seSemEntity.sId;
    // seSemEntity now holds the information for this object node.
    // now the rest of Object consists of recursing into subobjects,
    // these are in dsSubSemEntity as it parses through the children of dsSemEntity
    // The subObjects can be references, we only implement immediate mode here.
    while(!dsSubSemEntity.isNull())
    {
        // Read subobjects, adding each to the map, and adding the relation
        // decompositionOf to the map as well
		TypeOfSemanticEntity(dsSubSemEntity, type) ;
		if(type!=OBJECT) break;

		string sSubSemId;

		dsSubSemEntity.GetTextAttribute("id",sSubSemId);
		SemanticRelationType srType(OBJECT,OBJECT);
        ReadSemanticObject(dsSubSemEntity,mseSemEntities,gGraph);

        int size = gGraph.numEdges(); //seSemEntity.msEntRelations[sSubSemId].size();

        char* sz = new char[33];//sth why not defined as char sz[33];??
	sprintf(sz,"%d",size);
	//sz = itoa(size,sz,10);//sth itao not ANSI C (UNIX)
        string relno(sz);
	delete[] sz;//sth very suboptimal!!
        string rid = seSemEntity.sId + sSubSemId + relno;
        GCS_RelationDS rel;
        rel.sId = rid;
        rel.sName = "decompositionOf";
        rel.sType = "ObjectObjectRelationType"; break;
//        seSemEntity.msEntRelations[sSubSemId].insert(RelMapType::value_type(rid,rel));
		gGraph.relate(rid,sSubId,sSubSemId,srType,rel.sName);

        dsSubSemEntity = dsSubSemEntity.GetNextSibling();
    }
    // add Object to map
    mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int GraphicalCSSearchTool::ReadSemanticEvent(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    GCS_SemanticEntityType type;
    
    // Read the SemanticBase part first:
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);
	string sSubId = seSemEntity.sId;
    // seSemEntity now holds the information for this Event node.
    // now the rest of Event consists of recursing into subEvents,
    // these are in dsSubSemEntity as it parses through the children of dsSemEntity
    // SubEvents can be referenced, we only implement immediate ones here.
    while(!dsSubSemEntity.isNull())
    {
        // Read subEvents, adding each to the map, and adding the relation
        // decompositionOf to the map as well
		TypeOfSemanticEntity(dsSubSemEntity, type) ;
		if(type!=EVENT) break;

		string sSubSemId;

		dsSubSemEntity.GetTextAttribute("id",sSubSemId);
		SemanticRelationType srType(EVENT,EVENT);
        ReadSemanticEvent(dsSubSemEntity,mseSemEntities,gGraph);

        int size = gGraph.numEdges(); //seSemEntity.msEntRelations[sSubSemId].size();
        char* sz = new char[33];//sth why not defined as char sz[33];??
	sprintf(sz,"%d",size);
	//sz = itoa(size,sz,10);//sth itao not ANSI C (UNIX)
        string relno(sz);
	delete[] sz;//sth very suboptimal!!
        string rid = seSemEntity.sId + sSubSemId + relno;
        GCS_RelationDS rel;
        rel.sId = rid;
        rel.sName = "decompositionOf";
        rel.sType = "EventEventRelationType"; break;
//        seSemEntity.msEntRelations[sSubSemId].insert(RelMapType::value_type(rid,rel));
		gGraph.relate(rid,sSubId,sSubSemId,srType,rel.sName);

		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
    }
    // After all the subevents, come SemanticPlace and SemanticTime
    while(!dsSubSemEntity.isNull())
    {
		TypeOfSemanticEntity(dsSubSemEntity, type) ;
		if(type!=SEMANTICPLACE) break;
        ReadSemanticPlace(dsSubSemEntity,mseSemEntities,gGraph);
		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
    }
    while(!dsSubSemEntity.isNull())
    {
		TypeOfSemanticEntity(dsSubSemEntity, type) ;
		if(type!=SEMANTICTIME) break;
        ReadSemanticTime(dsSubSemEntity,mseSemEntities,gGraph);
		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
    }
    // add Event to map
    mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int GraphicalCSSearchTool::ReadSemanticTime(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    // SemanticBase
	// Reads the Label and Freeterm Values.
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);

    // Time DS.  We don't test this here, make TimePoint and Duration semantic (extent,position).
    // TimePoint and Duration
    // add SemanticTime to map
	while(!dsSubSemEntity.isNull())
	{
		if(dsSubSemEntity.GetDSName() == "Time")
		{
			string sTPoint, sDur;
			GenericDS dsAV = dsSubSemEntity.GetFirstChild();
			if(dsAV.GetDSName() == "TimePoint") dsAV.GetTextValue(sTPoint);
			dsAV = dsAV.GetNextSibling();
			if(dsAV.GetDSName() == "Duration") dsAV.GetTextValue(sDur);
		}
		if(dsSubSemEntity.GetDSName() == "SemanticTimeInterval")
		{
			GenericDS dsAV = dsSubSemEntity.GetFirstChild();

			for(int loop=0;loop<dsSubSemEntity.GetNumberOfChildren();++loop)
			{
				if(dsAV.GetDSName() == "TimePoint")
					{
						string sOrigin,sDispMeasType, sDispUnit, sDispValue;
						string sDirMeasType, sDirUnit, sDirValue;
						dsAV.GetTextAttribute("origin",sOrigin);
	
						GenericDS dssubAV = dsAV.GetFirstChild();
						if(dssubAV.GetDSName() == "Displacement") 
							{
							dssubAV.GetTextAttribute("measurementType",sDispMeasType);
							dssubAV.GetTextAttribute("unit",sDispUnit);
							dssubAV.GetTextAttribute("value",sDispValue);
							}
						dssubAV = dssubAV.GetNextSibling();
						if(dssubAV.GetDSName() == "Direction") 
							{
							dssubAV.GetTextAttribute("measurementType",sDirMeasType);
							dssubAV.GetTextAttribute("unit",sDirUnit);
							dssubAV.GetTextAttribute("value",sDirValue);
							}
					}
				if(dsAV.GetDSName() == "Duration")
					{
					string sAttributeNames, sMeasType, sUnit, sValue;	
					dsAV.GetTextAttribute("measurementType",sMeasType);
					dsAV.GetTextAttribute("unit",sUnit);
					dsAV.GetTextAttribute("value",sValue);
					}
				dsAV = dsAV.GetNextSibling();
			}
 		}
		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
	}

    mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int GraphicalCSSearchTool::ReadSemanticPlace(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    // SemanticBase
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);
    // Place DS.  We will not do a full implementation, we are not testing Place DS.
    // instead, test location and extent.
    // Location and Extent (both 0-->1)
    while(!dsSubSemEntity.isNull())
	{
		if(dsSubSemEntity.GetDSName() == "Place")
		{
			string sName, sCountry, sAddLine, sPostingIden;
			GenericDS dsAV = dsSubSemEntity.GetFirstChild();
			
			if(dsAV.GetDSName() == "Name") dsAV.GetTextValue(sName);
			dsAV = dsAV.GetNextSibling();
			if(dsAV.GetDSName() == "Country") dsAV.GetTextValue(sCountry);
			dsAV = dsAV.GetNextSibling();
			if(dsAV.GetDSName() == "PostalAddress") 
			{
				GenericDS dssubAV = dsAV.GetFirstChild();
				if(dssubAV.GetDSName() == "AddressLine") 
					dssubAV.GetTextValue(sAddLine);
				dssubAV = dssubAV.GetNextSibling();
				if(dssubAV.GetDSName() == "PostingIdentifier") 
					dssubAV.GetTextValue(sPostingIden);
			}
		}
		if(dsSubSemEntity.GetDSName() == "SemanticPlaceInterval")
		{
			string sMeasType, sUnit, sValue;
			string sOrigin,sDispMeasType, sDispUnit, sDispValue;
			string sDirMeasType, sDirUnit, sDirValue;

			GenericDS dsAV = dsSubSemEntity.GetFirstChild();
			for(int loop=0;loop<dsSubSemEntity.GetNumberOfChildren();++loop)
			{
				if(dsAV.GetDSName() == "Extent") 
				{
					dsAV.GetTextAttribute("measurementType",sMeasType);
					dsAV.GetTextAttribute("unit",sUnit);
					dsAV.GetTextAttribute("value",sValue);
				}
				if(dsAV.GetDSName() == "Location") 
				{
					dsAV.GetTextAttribute("origin",sOrigin);
					if(dsAV.GetNumberOfChildren()>0)
					{
						GenericDS dssubAV = dsAV.GetFirstChild();
						if(dssubAV.GetDSName() == "Displacement") 
						{
							dssubAV.GetTextAttribute("measurementType",sDispMeasType);
							dssubAV.GetTextAttribute("unit",sDispUnit);
							dssubAV.GetTextAttribute("value",sDispValue);
						}
						dssubAV = dssubAV.GetNextSibling();
						if(dssubAV.GetDSName() == "Direction") 
						{
							dssubAV.GetTextAttribute("measurementType",sDirMeasType);
							dssubAV.GetTextAttribute("unit",sDirUnit);
							dssubAV.GetTextAttribute("value",sDirValue);
						}
					}

				}

				dsAV = dsAV.GetNextSibling();
			}
		

		}
	dsSubSemEntity = dsSubSemEntity.GetNextSibling();
	}
    // add SemanticPlace to map
    mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int GraphicalCSSearchTool::ReadSemanticState(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    // SemanticState consists of:
    // SemanticBase
    // 0-->unbounded AttributeValuePairs.
   	ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);

      while(!dsSubSemEntity.isNull())
      {
          if(dsSubSemEntity.GetDSName()=="AttributeValuePair")
          {
              string sMem;
              GenericDS dsAV = dsSubSemEntity.GetFirstChild();
              if(dsAV.GetDSName() == "Attribute") dsAV.GetTextValue(sMem);
              if(sMem!="MembershipFunction" && sMem!="MembershipFunctionValue")
			  {
				  dsSubSemEntity = dsSubSemEntity.GetNextSibling();
                  continue;
			  }
              if(sMem=="MembershipFunction")
              {
                  string sMf;
                  dsAV = dsAV.GetNextSibling();
                  if(dsAV.GetDSName()== "TextualTypeValue") dsAV.GetTextValue(sMf);
                  seSemEntity.sMemberFunction = sMf;
              }
              else if(sMem=="MembershipFunctionValue")
              {
                  double fMfv;
                  dsAV = dsAV.GetNextSibling();
                  if(dsAV.GetDSName()== "FloatValue") dsAV.GetRealValue(fMfv);
                  seSemEntity.dMemberFunctionValue = fMfv;
              }
          }
          dsSubSemEntity = dsSubSemEntity.GetNextSibling();
      }
      // for SemanticState, we only look for the attributes "MembershipFunction"
      // and "MembershipFunctionValue" which we use for the search.
      //			mseSemEntities.insert(GCS_SEMapType::value_type(seSubSemEntity.sId, seSemEntity));/
      //			dsSubSemEntity = dsSubSemEntity.GetNextSibling();
      
      // add SemanticState to map
      mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));
      
    return 0;
}
int GraphicalCSSearchTool::ReadSemanticConcept(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    // Concept consists of 
    // SemanticBase only.
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);
    // Concept should have been entirely read by now, it derives without addition
    // from SemanticBase.
    mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int GraphicalCSSearchTool::ReadSemanticAgentObject(GenericDS dsSemEntity,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
    GenericDS dsSubSemEntity;
    GCS_SemanticEntity seSemEntity;
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,gGraph);
    // This is an object with a person in it.
    // Not clear whether the subobjects are "AgentObject" or Object
    
    // one or none (updated in Singapore) Agents go here, we implement only
    // immediate ones, and only limited fields (we are not verifying Agent DS here).
    
    return 0;
}

//----------------------------------------------------------------------------
int GraphicalCSSearchTool:: 
       ReadSemanticEntityTree(GenericDS dsSemEntity,
							  GCS_SEMapType& mseSemEntities)
//							  GCS_SemanticEntity& seSemEntity)
{
	// Variable declaration
	GCS_SemanticEntity seSemEntity;
	GenericDS dsLabel, ds, dsDefinition, dsSubSemEntity;
	string sText;
	// the reading of the semantic base portion has been separated out
	if(ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity,seSemEntity.gGraph)!=0)
	{
		fprintf(stderr,"Problem reading SemanticBase\n");
		return 1;
	}
    /* ------------------------------------------------------------------------
  	     Read all the sub-semantic entities
		 Ignore all other elements within the semantic entity
		 Add decompositionOf relations from current semantic entity to 
		 any sub-semantic entities found
       ------------------------------------------------------------------------ */

	// Sub-semantic entities.  This is where we read the objects, events, times, places,
	// states, concepts.  For objects,person objects, and events, they can recurse here,
	// as subobjects, and subevents.  We are in a Semantic, if a Semantic occurs, it starts
	// a recursive chain of Semantics.  For Concept, SemanticState, SemanticPlace, and 
	// SemanticTime, this is the end of the road.
	// for the others, subjecting them to this recursion loses information.

	while ( !dsSubSemEntity.isNull() ) {
		GCS_SemanticEntityType type;
		if ( TypeOfSemanticEntity(dsSubSemEntity, type) == 0) 
		{
/* ----------------Use only explicit graph declarations for the GraphicalCS validation
-------------------uncomment these lines to have full decomposition relations in a
-------------------Semantic DS break down.

			// Add relation between both semantic entities create relation ID as necessary
			string sId;
			SemanticRelationType srType(SEMANTIC,type);
			dsSubSemEntity.GetTextAttribute("id",sId);
			int size = seSemEntity.gGraph.numEdges(); //seSemEntity.msEntRelations[sId].size();
			char* sz = new char[20]; 
				sz = itoa(size,sz,10);
			string relno(sz);
			delete[] sz;
			string rid = seSemEntity.sId + sId + relno;
			RelationDS rel;
			rel.sId = rid;
			rel.sName = "contains";
			rel.sType = "SemanticBaseSemanticBaseType";
//			seSemEntity.msEntRelations[sId].insert(RelMapType::value_type(rid,rel));
			seSemEntity.gGraph.relate(rid,seSemEntity.sId,sId,srType,rel.sName);
*/			
			GCS_SemanticEntity seSubSemEntity;
			GenericDS dsSemanticBase;
			switch(type)
			{
			case OBJECT:
				ReadSemanticObject(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
			case EVENT:
				ReadSemanticEvent(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
			case SEMANTICSTATE:
				ReadSemanticState(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
			case CONCEPT:
				ReadSemanticConcept(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
    		case SEMANTICTIME:
				ReadSemanticTime(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
			case SEMANTICPLACE:
				ReadSemanticPlace(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
			case AGENTOBJECT:
				ReadSemanticAgentObject(dsSubSemEntity,mseSemEntities,seSemEntity.gGraph);
				break;
			case SEMANTIC:
				ReadSemanticEntityTree(dsSubSemEntity,mseSemEntities);  // True recursion.
				break;
			default:
				break;
			};
/*
			if ( ReadSemanticEntityTree(dsSubSemEntity, mseSemEntities, seSubSemEntity) != 0) {
				return 1;
			} 
			if (dsSubSemEntity.GetDSName() == "Object" || 
				dsSubSemEntity.GetDSName() == "PersonObject" || 
				dsSubSemEntity.GetDSName() == "AgentObject"  ||
				dsSubSemEntity.GetDSName() == "Semantic" ||  
				dsSubSemEntity.GetDSName() == "Event") {
			// Add relation between both semantic entities create relation ID as necessary
				int size = seSemEntity.msEntRelations[seSubSemEntity.sId].size();
				char* sz = itoa(size,sz,10);
				string relno(sz);
				string rid = seSemEntity.sId + seSubSemEntity.sId + relno;
				RelationDS rel;
				rel.sId = rid;
				rel.sName = "decompositionOf";
				switch(type)
				{
				case OBJECT:
				case PERSONOBJECT:
					rel.sType = "ObjectObjectRelationType"; break;
				case EVENT: 
					rel.sType = "EventEventRelationType"; break;
					
				}
				seSemEntity.msEntRelations[seSubSemEntity.sId].insert(RelMapType::value_type(rid,rel));
			} // end if
*/
		} // end if

		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
	}  // end while



  /* ------------------------------------------------------------------------
	   Read and validate the semantic graph, 
	   Only one semantic graph is expected as the last element in a Semantic DS 
	   description
	   This was moved from the calling routine: Graph is part of Semantic DS
     ------------------------------------------------------------------------ */

	GenericDS dsSemGraph = dsSemEntity.GetChild(dsSemEntity.GetNumberOfChildren() - 1);
	if (dsSemGraph.GetDSName() == "Graph") {
		if ( ReadSemanticGraph(dsSemGraph, mseSemEntities, seSemEntity.gGraph) != 0 ) {
			fprintf(stderr, "Error reading Graph DS\n");
			return 1;
		}
	}
    /* ------------------------------------------------------------------------
  	     Add semantic entity to map
       ------------------------------------------------------------------------ */

	mseSemEntities.insert(GCS_SEMapType::value_type(seSemEntity.sId, seSemEntity));


	return 0;
}

/*----------------------------------------------------------------------------
	Updated to read nodes as well as edges.  There is no such thing as
	SemanticGraph, it is just Graph.
-------------------------------------------------------------------------------*/
// Separate out and add the following functionality:
int GraphicalCSSearchTool::ReadGraphEdge(GenericDS dsEdge,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
	string sSource, sTarget, sName, sType, sId;
	GCS_RelationDS rRelation;

	dsEdge.GetTextAttribute("name", sName);
	dsEdge.GetTextAttribute("source", sSource);
	dsEdge.GetTextAttribute("target", sTarget);

	// relations do not necessarily have id's set, if not give it an id
	if(dsEdge.GetTextAttribute("id", sId) != 0)
	{
//		int size = 0; //mseSemEntities[sSource].msEntRelations[sTarget].size();
/*		char *sz; //sth why not char sz[33];
		sz = itoa(size,sz,3);//sth sz has no memory allocated,
		//sth itoa not defined in ANSI C (Unix)
		//sth itoa(value, string, radix) => radix is 3??*/
		//sth size=0 => string = "0"*/
		char sz[33]="0";
		string relno(sz);
		sId = sSource + sTarget + relno;
	}
	dsEdge.GetTextAttribute("xsi:type", sType);
	rRelation.sId = sId;
	rRelation.sName = sName;
	rRelation.sType = sType;

//	mseSemEntities[sSource].msEntRelations[sTarget].insert(RelMapType::value_type(sId,rRelation));
	gGraph.relate(sId,sSource,sTarget,sType,sName);
/*
	sEntRelation.append(" ");
	sEntRelation.append(sName);
	sEntRelation.append(" ");

	mseSemEntities[sSource].msEntRelations[sTarget] = sEntRelation;
*/
    return 0;
}
int GraphicalCSSearchTool::ReadGraphNode(GenericDS dsNode,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
	char* entities[13] = { "semantic","object","person","place","event","base",
						   "concept","state","time","agent","model","segment","relation" };
	string sId,sIdref;
	int iType=-1;

	dsNode.GetTextAttribute("id", sId);
	if(dsNode.GetTextAttribute("idref", sIdref) == 0)
	{
		// find the reference in mseSemEntities.  It should be there because these are read before graph.
		// first check that it is not a relation id
		// there is an ordering problem unless relations are read first.
		// there is an ordering problem for relations if they are.
		// Here we assume we can check the id of the node
		// On GCS, we have used the convention of starting names with types for nodes.
		Edge* edgeptr = gGraph.getEdgeByReference(sIdref);
		if(edgeptr!=NULL) iType = RELATION;
		else
		{
			GCS_SEMapType::iterator iter;
			for(iter=mseSemEntities.begin();iter!=mseSemEntities.end();iter++)
				if((*iter).second.sId==sIdref)
				{
					iType = mseSemEntities[sIdref].setType;
					break;
				}
		}
		if(iType==-1)
		{
			int en;
			char* str = new char[sId.length() + 1];
			strcpy(str,sId.c_str());
			for(en=0;en<13;en++)
				if(strspn(str,entities[en])==strlen(entities[en])) break;
			if(en<13) iType=en;
		}
		gGraph.addRefNode(sId,sIdref,iType);
	}
	else
	{
		int en;
		char* str = new char[sId.length() + 1];
		strcpy(str,sId.c_str());
		for(en=0;en<13;en++)
			if(strspn(str,entities[en])==strlen(entities[en])) break;
		if(en==13) gGraph.addNode(sId);
		else gGraph.addNode(sId,en);
	}

	return 0;
}
int GraphicalCSSearchTool::ReadGraphProperty(GenericDS dsProperty,GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
	string sId;
	bool symmetric,transitive,antisymmetric,reflexive,strict,total;
	dsProperty.GetTextAttribute("id", sId);
	dsProperty.GetBoolAttribute("symmetric",symmetric);
	dsProperty.GetBoolAttribute("transitive",transitive);
	dsProperty.GetBoolAttribute("antisymmetric",antisymmetric);
	dsProperty.GetBoolAttribute("reflexive",reflexive);
	dsProperty.GetBoolAttribute("strict",strict);
	dsProperty.GetBoolAttribute("total",total);
	return 0;
}
int GraphicalCSSearchTool:: 
       ReadSemanticGraph(GenericDS dsSemGraph, GCS_SEMapType& mseSemEntities,Graph& gGraph)
{
	// Variable declaration
	GenericDS dsElement;
	int i, iNumElements;

    /* ------------------------------------------------------------------------
       Read edges in the semantic graph.
	   They are expected to reference directly semantic entities in the same
	   description (or xml document) using their ids.
	   Only Edges elements are expected in the semantic graph.
       ------------------------------------------------------------------------ */

	if (dsSemGraph.GetDSName() != "Graph") {
		fprintf(stderr, "Wrong DS: expected Graph but found %s\n", 
			dsSemGraph.GetDSName().c_str());
		return 1;
	}
	// We may have any of three things next:  Properties, Relations, and Nodes.
	// The three have been made into functions.

	iNumElements = dsSemGraph.GetNumberOfChildren();
	for (i=0; i<iNumElements; i++) {
		dsElement = dsSemGraph.GetChild(i);
/*
		if (dsEdge.GetDSName() != "Edge") {
			fprintf(stderr, "Wrong DS: expected Edge but found %s\n", 
				dsEdge.GetDSName().c_str());
			return 1;
		}
*/
		if(dsElement.GetDSName() == "Relation")
			ReadGraphEdge(dsElement,mseSemEntities,gGraph);
		else if(dsElement.GetDSName() == "Property")
			ReadGraphProperty(dsElement,mseSemEntities,gGraph);
		else if(dsElement.GetDSName() == "Node")
			ReadGraphNode(dsElement,mseSemEntities,gGraph);
		else
		{
			fprintf(stderr,"Wrong DS: Expected Relation, Node, Property, but found %s\n",
				dsElement.GetDSName().c_str());
			return 1;
		}
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int GraphicalCSSearchTool:: 
       TypeOfSemanticEntity(GenericDS dsSemEntity, GCS_SemanticEntityType& setType)
{
	int iType = -1, i;
	string sDSName = dsSemEntity.GetDSName();
	if(sDSName == "SemanticBase")
	{
		string subs;
		if(dsSemEntity.GetTextAttribute("xsi:type",subs)!=0)
			return 1;
		char str[50];
		
		strcpy(str,subs.c_str());
		str[strlen(str)-4] = 0;
		subs = str;
		sDSName = subs;
	}
	// Compare to each type of semantic entities
	for (i=0; i<10; i++) {
		if (sDSName == SEMANTIC_ENTITY_TYPES[i]) {
			iType = i;
			break;
		}
	}
	if ( iType > 9 || iType < 0 ) {
		return 1;
	} 

	setType = (GCS_SemanticEntityType) iType;

	return 0;
}

//----------------------------------------------------------------------------
double GraphicalCSSearchTool:: 
       SemanticMatching(
			GCS_SEMapType mseSemEntitiesRef,
			GCS_SEMapType mseSemEntitiesQuery)
{
	// Variable declaration
	double dist = 0.0, avgavgDist= 0.0, tmpDist, minDist, avgminDist = 0.0;
	GCS_SEMapType::iterator seIterRef, seIterQuery;

    /* ------------------------------------------------------------------------
  	     For each semantic entity in the query, find the mininum distance
		 to the semantic entities in the reference.
		 The distance between two semantic entities is calculated comparing
		 the keywords in the Label field
       ------------------------------------------------------------------------ */

	if ( mseSemEntitiesRef.size() == 0 || mseSemEntitiesRef.size() == 0) {
		return DBL_MAX;
	}

	for (seIterQuery = mseSemEntitiesQuery.begin(); 
		 seIterQuery != mseSemEntitiesQuery.end(); 
		 seIterQuery++) {

		minDist = DBL_MAX;

		for (seIterRef = mseSemEntitiesRef.begin(); 
			 seIterRef != mseSemEntitiesRef.end(); 
			 seIterRef++) {

			tmpDist = MatchStrings((*seIterQuery).second.sLabel,
									(*seIterRef).second.sLabel);

			minDist = (tmpDist < minDist) ? tmpDist : minDist;
			avgavgDist += tmpDist;
		}

		avgminDist += minDist;
	}

    avgminDist /= (double)(double)mseSemEntitiesQuery.size();

	avgavgDist /= (double)mseSemEntitiesRef.size();
	avgavgDist /= (double)mseSemEntitiesQuery.size();

	dist = (avgavgDist + avgminDist)/2.0;

	fprintf(stderr, "Distance = %f\n", dist);

	return dist;
}

//----------------------------------------------------------------------------
void GraphicalCSSearchTool::ReplaceStrings(string& string, char *strListTypes, ...)
{
	va_list vl;
	char *rpc_str;
	int i, last_str;

	va_start(vl, strListTypes);
	for (i=0; strListTypes[i]!='\0'; i++) {
		rpc_str = va_arg(vl, char *);
		//fprintf(stderr, "%s\n", rpc_str);
	}
	va_end(vl);
	last_str = i - 1;

	va_start(vl, strListTypes);
	for (i=0; i<last_str; i++) {
		ReplaceInString(string, va_arg(vl, char *), rpc_str);
	}
	va_end(vl);
}

//----------------------------------------------------------------------------
void GraphicalCSSearchTool::ReplaceInString(string& str, char *str1, char *str2)
{
	int pos, size, rep_size;

	pos = str.find(string(str1));
	size = str.size();
	rep_size = strlen(str1);

	while ( pos >= 0 && pos < size) {
		str.replace(pos, rep_size, string(str2));
		pos = str.find(string(str1), pos + rep_size);
	}

	return;
}

//----------------------------------------------------------------------------
double GraphicalCSSearchTool::MatchStrings(string str1, string str2)
{
	int iNumWords1 = 0, iNumWords2 = 0, iNumMatches = 0;
	vector<string> vstrWords1, vstrWords2;
	char aux[1024];
	int j, l, pos;
	double dist = 0.0;

	// Remove irrevelant words from the strings
	ReplaceStrings(str1, "ssssssssssssssssssssss",
		".", ":", ";", ",", "(", ")", "/", " the ", " and ", " of ", 
		" on ", " in ", " with ", " without ", " from ", " to ", " only ", "'s ", " at ", " as ", 
		"\\", " ");

	ReplaceStrings(str2, "ssssssssssssssssssssss",
		".", ":", ";", ",", "(", ")", "/", " the ", " and ", " of ", 
		" on ", " in ", " with ", " without ", " from ", " to ", " only ", "'s ", " at ", " as ", 
		"\\", " ");


	// Put words into vectors
	pos = 0;
	//fprintf(stderr, "Ref words = ");
	while((sscanf(str1.c_str() + pos, "%s", aux)) > 0) {
		vstrWords1.push_back(string(aux));
		//fprintf(stderr, "%s:", aux);
		pos = str1.find(string(aux), pos) + strlen(aux);
	}
	//fprintf(stderr, " (%d)\n", vstrWords1.size());
	iNumWords1 = vstrWords1.size();

	pos = 0;
	//fprintf(stderr, "Query words = ");
	while((sscanf(str2.c_str() + pos, "%s", aux)) > 0) {
		vstrWords2.push_back(string(aux));
		//fprintf(stderr, "%s:", aux);
		pos = str2.find(string(aux), pos) + strlen(aux);
	}
	//fprintf(stderr, " (%d)\n", vstrWords2.size());
	iNumWords2 = vstrWords2.size();

	// Count word matches between query and ref
	iNumMatches = 0;
	for (j=0; j<iNumWords2; j++) {
		for (l=0; l<iNumWords1; l++) {
		  //sth why not strcmp?? _stricmp is only defined on Win
		  //if ( _stricmp(vstrWords2[j].c_str(), vstrWords1[l].c_str()) == 0) {
		  if (strcmp(vstrWords2[j].c_str(), vstrWords1[l].c_str()) == 0) {
				iNumMatches++;
			}
		}
	}

	// The distance is calculated as the (1 - num matches / num maximum words)
	// The distance is accumulated for each level
	if (iNumWords2 > 0 || iNumWords1 > 0) {
		dist = (iNumWords2 > iNumWords1) ? 
			   (1.0 - (double) iNumMatches/iNumWords2) : 
			   (1.0 - (double) iNumMatches/iNumWords1);
	}

	//fprintf(stderr, "Distance = %f (%d)\n", dist, iNumMatches);

	return dist;
}

double GraphicalCSSearchTool::GetMembershipDistance(GCS_SEMapType mseSemEntitiesRef,
												 GCS_SEMapType mseSemEntitiesQuery)
{
	double dist = 0.0, mfQ, mfR;
	GCS_SEMapType::iterator seIterRef, seIterQuery;

	/*------------------------------------------------------------------
		Get the membership function of each of the elements of the query,
		and subtract them to get the distance between them.

		This version does no checking of anything, just the membership function.
		It is possible to check that the membership function is the correct one.
	--------------------------------------------------------------------*/
	if ( mseSemEntitiesRef.size() == 0 || mseSemEntitiesRef.size() == 0) 
	{
		return DBL_MAX;
	}
	for (seIterQuery = mseSemEntitiesQuery.begin(); 
		 seIterQuery != mseSemEntitiesQuery.end(); 
		 seIterQuery++) 
	{
		if((*seIterQuery).second.setType==SEMANTICSTATE) 
			mfQ = (*seIterQuery).second.dMemberFunctionValue;
	}
	for (seIterRef = mseSemEntitiesRef.begin(); 
		 seIterRef != mseSemEntitiesRef.end(); 
		 seIterRef++) 
	{
		if((*seIterRef).second.setType==SEMANTICSTATE) 
			mfR = (*seIterRef).second.dMemberFunctionValue;
	}
	dist = fabs(mfQ - mfR);
	return dist;

}
//----------------------------------------------------------------------------
GraphicalCSSearchInterfaceABC *GraphicalCSSearchTool::GetInterface(void)
{ return &m_Interface; }

