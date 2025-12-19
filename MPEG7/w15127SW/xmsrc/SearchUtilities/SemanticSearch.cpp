//////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  SemanticSearch.cpp
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


const UUID SemanticSearchInterface::myID = UUID("");
const char *SemanticSearchInterface::myName = "SemanticMatchingInterface";

const UUID SemanticSearchTool::myID = UUID("");
const char *SemanticSearchTool::myName = "SemanticMatchingTool";

const UUID SemanticSearchInterfaceABC::myID = UUID();

// Give value to constants
const int	SemanticSearchTool::STRING_MAXSIZE= 4096;		// Maximum size of the strings
const string SemanticSearchTool::SEMANTIC_ENTITY_TYPES[8] = {// Names of types of segment entities
	  "SemanticType",
	  "ObjectType",
	  "AgentObjectType",
	  "SemanticPlaceType",
	  "EventType",
	  "ConceptType",
	  "SemanticStateType",
	  "SemanticTimeType"
};

//=============================================================================
SemanticSearchInterfaceABC::SemanticSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
SemanticSearchInterface::SemanticSearchInterface(SemanticSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SemanticSearchInterface::~SemanticSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SemanticSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SemanticSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SemanticSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int SemanticSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefSemanticDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefSemanticDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* SemanticSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int SemanticSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QuerySemanticDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QuerySemanticDescriptionInterface);
}

//----------------------------------------------------------------------------
double SemanticSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
SemanticSearchTool::SemanticSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
SemanticSearchTool::
SemanticSearchTool(GenericDSInterfaceABC
		      *aQueryDescriptionInterface):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  /* connect Descritors with Searchs*/
  SetQueryDescriptionInterface(aQueryDescriptionInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SemanticSearchTool::~SemanticSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& SemanticSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SemanticSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SemanticSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SemanticSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SemanticSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefSemanticDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefSemanticDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* SemanticSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  SemanticSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQuerySemanticDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQuerySemanticDescriptionInterface;
  
  /* could also look totally different if
     aQuerySemanticDescriptionInterface is not of Semantic node*/

  return 0;
}

//----------------------------------------------------------------------------
double SemanticSearchTool::GetDistance(void)
{
  double dist = 0.0;
  SEM_SEMapType mseSemEntitiesRef, mseSemEntitiesQuery;

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

  // Read labels and relations among semantic entities for query and reference
  if ( SemanticDecoding(m_RefDescriptionInterface, mseSemEntitiesRef) != 0 || 
	   SemanticDecoding(m_QueryDescriptionInterface, mseSemEntitiesQuery) != 0) {
	  dist = DBL_MAX;
  }

  // TESTING
/*  {
  SEM_SEMapType::iterator iter1;
  FILE *fd = fopen("out", "a");
  fprintf(fd, "\n\nREFERENCE\n"); 
  for (iter1=mseSemEntitiesRef.begin(); iter1!=mseSemEntitiesRef.end(); iter1++) {
	fprintf(fd, "SEMANTIC ENTITY: %d\n", (*iter1).second.setType); 
	fprintf(fd, "id =  %s\n", (*iter1).second.sId.c_str());
	fprintf(fd, "Label = %s\n", (*iter1).second.sLabel.c_str());
	fprintf(fd, "Definition = %s\n", (*iter1).second.sDefinition.c_str());
	fprintf(fd, "Relations\n");
	StrMapType::iterator iter2;
	for (iter2=(*iter1).second.msEntRelations.begin(); 
		 iter2!=(*iter1).second.msEntRelations.end(); 
		 iter2++) {
		fprintf(fd, "\tTarget = %s\tName = %s\n", 
			(*iter2).first.c_str(), (*iter2).second.c_str());
	}
  }
  fclose(fd);
  }
  {
  SEM_SEMapType::iterator iter1;
  FILE *fd = fopen("out", "a");
  fprintf(fd, "\n\nQUERY\n"); 
  for (iter1=mseSemEntitiesQuery.begin(); iter1!=mseSemEntitiesQuery.end(); iter1++) {
	fprintf(fd, "SEMANTIC ENTITY: %d\n", (*iter1).second.setType); 
	fprintf(fd, "id =  %s\n", (*iter1).second.sId.c_str());
	fprintf(fd, "Label = %s\n", (*iter1).second.sLabel.c_str());
	fprintf(fd, "Definition = %s\n", (*iter1).second.sDefinition.c_str());
	fprintf(fd, "Relations\n");
	StrMapType::iterator iter2;
	for (iter2=(*iter1).second.msEntRelations.begin(); 
		 iter2!=(*iter1).second.msEntRelations.end(); 
		 iter2++) {
		fprintf(fd, "\tTarget = %s\tName = %s\n", 
			(*iter2).first.c_str(), (*iter2).second.c_str());
	}
  }
  fclose(fd);
  }
*/

  if ( dist < DBL_MAX) {
	// Match labels and relations among semantic entities for query and reference
	dist = SemanticMatching(mseSemEntitiesRef, mseSemEntitiesQuery);
  }


  // All done
  return dist;
}

//----------------------------------------------------------------------------
int SemanticSearchTool::
	SemanticDecoding(
		GenericDSInterfaceABC *pdsiDSInt, 
		SEM_SEMapType& mseSemEntities)
{
  // Variable declaration
  SEM_SemanticEntity seSemEntity;
  GenericDS dsSemantic, dsSemGraph;


  /* ------------------------------------------------------------------------
	   Read and validate the Semantic DS description
     ------------------------------------------------------------------------ */

  dsSemantic = pdsiDSInt->GetFirstChild();
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

  if ( ReadSemanticEntityTree(dsSemantic, mseSemEntities, seSemEntity) != 0 ) {
	fprintf(stderr, "Error reading Semantic DS\n");
	return 1;
  }


  /* ------------------------------------------------------------------------
	   Read and validate the semantic graph, 
	   Only one semantic graph is expected as the last element in a Semantic DS 
	   description
     ------------------------------------------------------------------------ */

  dsSemGraph = dsSemantic.GetChild(dsSemantic.GetNumberOfChildren() - 1);
  if (dsSemGraph.GetDSName() == "Graph") {
	  if ( ReadSemanticGraph(dsSemGraph, mseSemEntities) != 0 ) {
		  fprintf(stderr, "Error reading Graph DS\n");
		  return 1;
	  }
  }

  return 0;
}

//----------------------------------------------------------------------------
int SemanticSearchTool:: 
       ReadSemanticEntityTree(GenericDS dsSemEntity,
							  SEM_SEMapType& mseSemEntities,
							  SEM_SemanticEntity& seSemEntity)
{
	// Variable declaration
	GenericDS dsLabel, ds, dsDefinition, dsSubSemEntity;
	string sText;


    /* ------------------------------------------------------------------------
  	     Check for semantic entity and attribute of semantic entity
       ------------------------------------------------------------------------ */

	// Check wether it is a semantic entity
	if ( TypeOfSemanticEntity(dsSemEntity, seSemEntity.setType) != 0) {
	  fprintf(stderr, "DS is not a SemanticBase DS\n"); 
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
    if (dsLabel.GetDSName() != "Label") {
      fprintf(stderr, "Wrong DS name: expected Label expected but found %s\n", 
		  dsLabel.GetDSName().c_str());
	    return 1;
	}
	seSemEntity.sLabel.append(" ");
	while ( !dsLabel.isNull() ) {
	  if (dsLabel.GetDSName() != "Label") {
		break;
	  }
	  if (dsLabel.GetNumberOfChildren() != 1) {
		  fprintf(stderr, "Wrong number of elements in Label: %d\n", 
			  dsLabel.GetNumberOfChildren());
		  return 1;		
	  }
	  ds = dsLabel.GetFirstChild();
	  if (ds.GetDSName() != "Name") {
		  fprintf(stderr, "Wrong DS name: expected Name but found %s\n", 
			  ds.GetDSName().c_str());
		  return 1;
	  }
	  if (ds.GetTextValue(sText) != 0) {
	    cerr << "Wrong text value for Name: " << sText << endl;
	    //	    fprintf(stderr, "Wrong text value for Name: %s\n", sText);
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
	if ( !dsDefinition.isNull() ) {
	  if ( dsDefinition.GetDSName() == "Definition" ) {
		seSemEntity.sDefinition.append(" ");
		ds = dsDefinition.GetFirstChild();
		if (ds.GetDSName() != "FreeTextAnnotation") {
		  fprintf(stderr, "Wrong DS name: expected FreeTextAnnotation but found %s\n", 
			  ds.GetDSName().c_str());
		  return 1;
		}
		if (ds.GetTextValue(sText) != 0) {
		        cerr << "Wrong text value for FreeTextAnnotation: "<< sText << endl;
//			fprintf(stderr, "Wrong text value for FreeTextAnnotation: %s\n", sText);
			return 1;
		}

		seSemEntity.sDefinition.append(sText);
		seSemEntity.sDefinition.append(" ");

		dsDefinition = dsDefinition.GetNextSibling();
	  }
	}


    /* ------------------------------------------------------------------------
  	     Read all the sub-semantic entities
		 Ignore all other elements within the semantic entity
		 Add decompositionOf relations from current semantic entity to 
		 any sub-semantic entities found
       ------------------------------------------------------------------------ */

	// Sub-semantic entities
	dsSubSemEntity = dsDefinition;
	while ( !dsSubSemEntity.isNull() ) {
		SEM_SemanticEntityType type;
		if ( TypeOfSemanticEntity(dsSubSemEntity, type) == 0) {
			SEM_SemanticEntity seSubSemEntity;
			if ( ReadSemanticEntityTree(dsSubSemEntity, mseSemEntities, seSubSemEntity) != 0) {
				return 1;
			} 
			// If sub-semantic entity is an object, agentobject, semantic place, semantic time 
			// or event
			if (type == OBJECT || type == AGENTOBJECT || type == SEMANTICPLACE || 
				type == EVENT || type == SEMANTICTIME) {
			// Add relation between both semantic entities
				seSemEntity.msEntRelations[seSubSemEntity.sId] = " decompositionOf ";
			}
		}

		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
	}


    /* ------------------------------------------------------------------------
  	     Add semantic entity to map
       ------------------------------------------------------------------------ */

	mseSemEntities.insert(SEM_SEMapType::value_type(seSemEntity.sId, seSemEntity));


	return 0;
}

//----------------------------------------------------------------------------
int SemanticSearchTool:: 
       ReadSemanticGraph(GenericDS dsSemGraph, SEM_SEMapType& mseSemEntities)
{
	// Variable declaration
	GenericDS dsEdge;
	string sSource, sTarget, sTSource, sTTarget, sName, sEntRelation;
	int i, iNumEdges;

    /* ------------------------------------------------------------------------
       Read edges in the semantic graph.
	   They are expected to reference directly semantic entities in the same
	   description (or xml document) using their ids.
	   Only Edges elements are expected in the semantic graph.
       ------------------------------------------------------------------------ */

	if (dsSemGraph.GetDSName() != "Graph") {
		fprintf(stderr, "Wrond DS: expected Graph but found %s\n", 
			dsSemGraph.GetDSName().c_str());
		return 1;
	}

	iNumEdges = dsSemGraph.GetNumberOfChildren();
	for (i=0; i<iNumEdges; i++) {
		dsEdge = dsSemGraph.GetChild(i);

		if (dsEdge.GetDSName() != "Relation") {
			fprintf(stderr, "Wrond DS: expected Relation but found %s\n", 
				dsEdge.GetDSName().c_str());
			return 1;
		}

		dsEdge.GetTextAttribute("name", sName);
		dsEdge.GetTextAttribute("source", sTSource);
		dsEdge.GetTextAttribute("target", sTTarget);

		sSource = sTSource.erase(0,1);
		sTarget = sTTarget.erase(0,1);

		sEntRelation = mseSemEntities[sSource].msEntRelations[sTarget];
		sEntRelation.append(" ");
		sEntRelation.append(sName);
		sEntRelation.append(" ");

		mseSemEntities[sSource].msEntRelations[sTarget] = sEntRelation;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int SemanticSearchTool:: 
       TypeOfSemanticEntity(GenericDS dsSemEntity, SEM_SemanticEntityType& setType)
{
	int iType = -1, i;
	string str;

	// Check name of element
	str = dsSemEntity.GetDSName();
	if ( !str.compare("SemanticBase") ) {
		dsSemEntity.GetTextAttribute("xsi:type", str);
	} else {
		str.append("Type");
	}

	// Compare to each type of semantic entity
	for (i=0; i<8; i++) {
		if ( !str.compare(0, str.length(), SEMANTIC_ENTITY_TYPES[i]) ) {
			iType = i;
			break;
		}
	}
	if ( iType > 7 || iType < 0 ) {
		return 1;
	} 

	setType = (SEM_SemanticEntityType) iType;

	return 0;
}

//----------------------------------------------------------------------------
double SemanticSearchTool:: 
       SemanticMatching(
			SEM_SEMapType mseSemEntitiesRef,
			SEM_SEMapType mseSemEntitiesQuery)
{
	// Variable declaration
	double dist = 0.0, avgavgDist= 0.0, tmpDist, minDist, avgminDist = 0.0;
	SEM_SEMapType::iterator seIterRef, seIterQuery;

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
void SemanticSearchTool::ReplaceStrings(string& string, char *strListTypes, ...)
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
void SemanticSearchTool::ReplaceInString(string& str, char *str1, char *str2)
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
double SemanticSearchTool::MatchStrings(string str1, string str2)
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
#ifdef _VISUALC_
			if ( _stricmp(vstrWords2[j].c_str(), vstrWords1[l].c_str()) == 0) {
				iNumMatches++;
			}
#else
			if ( strcasecmp(vstrWords2[j].c_str(), vstrWords1[l].c_str()) == 0) {
				iNumMatches++;
			}
#endif
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

//----------------------------------------------------------------------------
SemanticSearchInterfaceABC *SemanticSearchTool::GetInterface(void)
{ return &m_Interface; }

