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
// Applicable File Name:  StateMemberSearch.cpp
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
const UUID StateMemberSearchInterface::myID = UUID("");
const char *StateMemberSearchInterface::myName = "SemanticStateMatchingInterface";

const UUID StateMemberSearchTool::myID = UUID("");
const char *StateMemberSearchTool::myName = "SemanticStateMatchingTool";

const UUID StateMemberSearchInterfaceABC::myID = UUID();

// Give value to constants
const int	StateMemberSearchTool::STRING_MAXSIZE= 4096;		// Maximum size of the strings
const string StateMemberSearchTool::SEMANTIC_ENTITY_TYPES[10] = {// Names of types of semantic entities
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
StateMemberSearchInterfaceABC::StateMemberSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
StateMemberSearchInterface::StateMemberSearchInterface(StateMemberSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
StateMemberSearchInterface::~StateMemberSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& StateMemberSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateMemberSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void StateMemberSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int StateMemberSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefSemanticDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefSemanticDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* StateMemberSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int StateMemberSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QuerySemanticDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QuerySemanticDescriptionInterface);
}

//----------------------------------------------------------------------------
double StateMemberSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
StateMemberSearchTool::StateMemberSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
StateMemberSearchTool::
StateMemberSearchTool(GenericDSInterfaceABC
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
StateMemberSearchTool::~StateMemberSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& StateMemberSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateMemberSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool StateMemberSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool StateMemberSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int StateMemberSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefSemanticDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefSemanticDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* StateMemberSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  StateMemberSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQuerySemanticDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQuerySemanticDescriptionInterface;
  
  /* could also look totally different if
     aQuerySemanticDescriptionInterface is not of Semantic node*/

  return 0;
}

//----------------------------------------------------------------------------
double StateMemberSearchTool::GetDistance(void)
{
  double dist = 0.0;
  STM_SEMapType mseSemEntitiesRef, mseSemEntitiesQuery;

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
  STM_SEMapType::iterator iter1;
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
  STM_SEMapType::iterator iter1;
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
//	dist = SemanticMatching(mseSemEntitiesRef, mseSemEntitiesQuery);
	  dist = GetMembershipDistance(mseSemEntitiesRef, mseSemEntitiesQuery);
  }


  // All done
  return dist;
}

//----------------------------------------------------------------------------
int StateMemberSearchTool::
	SemanticDecoding(
		GenericDSInterfaceABC *pdsiDSInt, 
		STM_SEMapType& mseSemEntities)
{
  // Variable declaration
  STM_SemanticEntity seSemEntity;
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
  if (dsSemGraph.GetDSName() == "SemanticGraph") {
	  if ( ReadSemanticGraph(dsSemGraph, mseSemEntities) != 0 ) {
		  fprintf(stderr, "Error reading SemanticGraph DS\n");
		  return 1;
	  }
  }

  return 0;
}

// Function to read Relations that are sourced in a Semantic entity rather than in a graph
int StateMemberSearchTool::ReadSemanticRelation(GenericDS dsEdge,STM_SemanticEntity& seSemEntity)
{
	// relations in semantic entities do not have sources (implicit)
	string sSource,sTarget, sName, sType, sId;
	RelationDS rRelation;

	sSource = seSemEntity.sId;

	dsEdge.GetTextAttribute("name", sName);
	dsEdge.GetTextAttribute("target", sTarget);

	// relations do not necessarily have id's set, if not give it an id
	if(dsEdge.GetTextAttribute("id", sId) != 0)
	{
		int size = seSemEntity.msEntRelations[sTarget].size();
//sth		char* sz;
//sth		sz = itoa(size,sz,3);//sth radix =3 ???
		char sz[33];
		sprintf(sz,"%d",size);
		string relno(sz);
		sId = sSource + sTarget + relno;
	}
	dsEdge.GetTextAttribute("xsi:type", sType);
	rRelation.sId = sId;
	rRelation.sName = sName;
	rRelation.sType = sType;

	seSemEntity.msEntRelations[sTarget].insert(RelMapType::value_type(sId,rRelation));
    return 0;
}

/*-------------------------------------------------------------------------
   Read the semantic base portion of a semantic description scheme
   record the labels, definitions, properties, relations, skip the MediaOccurrences
-------------------------------------------------------------------------*/

int StateMemberSearchTool::ReadSemanticBase(GenericDS& dsSemEntity,STM_SemanticEntity& seSemEntity, GenericDS& dsSubSemEntity )
{
	// Variable declaration
	GenericDS dsLabel, ds, dsDefinition, dsProperty, dsMediaOccurrence, dsRelation;
	string sText;

    /* ------------------------------------------------------------------------
  	     Check for semantic entity and attribute of semantic entity
       ------------------------------------------------------------------------ */

	// Check wether it is a semantic entity
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
	  if(ReadSemanticRelation(dsRelation,seSemEntity)!=0)
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
int StateMemberSearchTool::ReadSemanticObject(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities,string& sSubId)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    
    // Read the SemanticBase part first:
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);
	sSubId = seSemEntity.sId;
    // seSemEntity now holds the information for this object node.
    // now the rest of Object consists of recursing into subobjects,
    // these are in dsSubSemEntity as it parses through the children of dsSemEntity
    // The subObjects can be references, we only implement immediate mode here.
    while(!dsSubSemEntity.isNull())
    {
        // Read subobjects, adding each to the map, and adding the relation
        // decompositionOf to the map as well
		string sSubSemId;
        ReadSemanticObject(dsSubSemEntity,mseSemEntities,sSubSemId);

        int size = seSemEntity.msEntRelations[sSubSemId].size();
        char sz[33];
	sprintf(sz,"%d",size);
	//char* sz = itoa(size,sz,10);//sth itoa not defined on UNIX
        string relno(sz);
        string rid = seSemEntity.sId + sSubSemId + relno;
        RelationDS rel;
        rel.sId = rid;
        rel.sName = "decompositionOf";
        rel.sType = "ObjectObjectRelationType"; break;
        seSemEntity.msEntRelations[sSubSemId].insert(RelMapType::value_type(rid,rel));

        dsSubSemEntity = dsSubSemEntity.GetNextSibling();
    }
    // add Object to map
    mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int StateMemberSearchTool::ReadSemanticEvent(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities,string& sSubId)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    
    // Read the SemanticBase part first:
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);
	sSubId = seSemEntity.sId;
    // seSemEntity now holds the information for this Event node.
    // now the rest of Event consists of recursing into subEvents,
    // these are in dsSubSemEntity as it parses through the children of dsSemEntity
    // SubEvents can be referenced, we only implement immediate ones here.
    while(!dsSubSemEntity.isNull())
    {
        // Read subEvents, adding each to the map, and adding the relation
        // decompositionOf to the map as well
		string sSubSemId;
        ReadSemanticObject(dsSubSemEntity,mseSemEntities,sSubSemId);

        int size = seSemEntity.msEntRelations[sSubSemId].size();
	char sz[33];
        sprintf(sz,"%d",size);
	//char* sz = itoa(size,sz,10);//sth itoa not defined on UNIX
        string relno(sz);
        string rid = seSemEntity.sId + sSubSemId + relno;
        RelationDS rel;
        rel.sId = rid;
        rel.sName = "decompositionOf";
        rel.sType = "EventEventRelationType"; break;
        seSemEntity.msEntRelations[sSubSemId].insert(RelMapType::value_type(rid,rel));

		dsSubSemEntity = dsSubSemEntity.GetNextSibling();
    }
    // After all the subevents, come SemanticPlace and SemanticTime
    while(!dsSubSemEntity.isNull())
    {
        // Type statement here:
        ReadSemanticPlace(dsSubSemEntity,mseSemEntities);
    }
    while(!dsSubSemEntity.isNull())
    {
        // Type statement here:
        ReadSemanticTime(dsSubSemEntity,mseSemEntities);
    }
    // add Event to map
    mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int StateMemberSearchTool::ReadSemanticTime(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    // SemanticBase
	// Reads the Label and Freeterm Values.
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);

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

    mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int StateMemberSearchTool::ReadSemanticPlace(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    // SemanticBase
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);
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
    mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int StateMemberSearchTool::ReadSemanticState(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    // SemanticState consists of:
    // SemanticBase
    // 0-->unbounded AttributeValuePairs.
   	ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);

      while(!dsSubSemEntity.isNull())
      {
          if(dsSubSemEntity.GetDSName()=="AttributeValuePair")
          {
              string sMem;
              GenericDS dsAV = dsSubSemEntity.GetFirstChild();
              if(dsAV.GetDSName() == "Attribute") dsAV.GetTextValue(sMem);
              if(sMem!="MembershipFunction" && sMem!="MembershipFunctionValue")
              {
                  // skip the field, remember to advance to next sibling!
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
      //			mseSemEntities.insert(STM_SEMapType::value_type(seSubSemEntity.sId, seSemEntity));/
      //			dsSubSemEntity = dsSubSemEntity.GetNextSibling();
      
      // add SemanticState to map
      mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
      
    return 0;
}
int StateMemberSearchTool::ReadSemanticConcept(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities)
{
    // Concept consists of 
    // SemanticBase only.
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);
    // Concept should have been entirely read by now, it derives without addition
    // from SemanticBase.
    mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int StateMemberSearchTool::ReadSemanticSemantic(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities,string& sSubId)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    // Semantic consists of:
    // SemanticBase
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);
    // 0-->unbounded SemanticBase subobjects
    // 0-->unbounded Graphs
    
    // add Semantic to map
    mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));
    return 0;
}
int StateMemberSearchTool::ReadSemanticAgentObject(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities)
{
    GenericDS dsSubSemEntity;
    STM_SemanticEntity seSemEntity;
    ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity);
    // This is an object with a person in it.
    // Not clear whether the subobjects are "AgentObject" or Object
    
    // one or none (updated in Singapore) Agents go here, we implement only
    // immediate ones, and only limited fields (we are not verifying Agent DS here).
    
    return 0;
}

//----------------------------------------------------------------------------
int StateMemberSearchTool:: 
       ReadSemanticEntityTree(GenericDS dsSemEntity,
							  STM_SEMapType& mseSemEntities,
							  STM_SemanticEntity& seSemEntity)
{
	// Variable declaration
	GenericDS dsLabel, ds, dsDefinition, dsSubSemEntity;
	string sText;
	// the reading of the semantic base portion has been separated out
	if(ReadSemanticBase(dsSemEntity,seSemEntity,dsSubSemEntity)!=0)
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
		STM_SemanticEntityType type;
		if ( TypeOfSemanticEntity(dsSubSemEntity, type) == 0) {
			STM_SemanticEntity seSubSemEntity;
			GenericDS dsSemanticBase;
			string empty=string("");
			switch(type)
			{
			case OBJECT:
				ReadSemanticObject(dsSubSemEntity,mseSemEntities,empty);
				break;
			case EVENT:
				ReadSemanticEvent(dsSubSemEntity,mseSemEntities,empty);
				break;
			case SEMANTICSTATE:
				ReadSemanticState(dsSubSemEntity,mseSemEntities);
				break;
			case CONCEPT:
				ReadSemanticConcept(dsSubSemEntity,mseSemEntities);
				break;
    		case SEMANTICTIME:
				ReadSemanticTime(dsSubSemEntity,mseSemEntities);
				break;
			case SEMANTICPLACE:
				ReadSemanticPlace(dsSubSemEntity,mseSemEntities);
				break;
			case AGENTOBJECT:
				ReadSemanticAgentObject(dsSubSemEntity,mseSemEntities);
				break;
			case SEMANTIC:
				ReadSemanticSemantic(dsSubSemEntity,mseSemEntities,empty);
				break;
			default:
				// code needs to be written for these fields.  For now, let them pass.
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
  	     Add semantic entity to map
       ------------------------------------------------------------------------ */

	mseSemEntities.insert(STM_SEMapType::value_type(seSemEntity.sId, seSemEntity));


	return 0;
}

/*----------------------------------------------------------------------------
	Updated to read nodes as well as edges.  There is no such thing as
	SemanticGraph, it is just Graph.
-------------------------------------------------------------------------------*/
// Separate out and add the following functionality:
int StateMemberSearchTool::ReadGraphEdge(GenericDS dsEdge,STM_SEMapType& mseSemEntities)
{
	string sSource, sTarget, sName, sType, sId;
	RelationDS rRelation;

	dsEdge.GetTextAttribute("name", sName);
	dsEdge.GetTextAttribute("source", sSource);
	dsEdge.GetTextAttribute("target", sTarget);

	// relations do not necessarily have id's set, if not give it an id
	if(dsEdge.GetTextAttribute("id", sId) != 0)
	{
		int size = mseSemEntities[sSource].msEntRelations[sTarget].size();
//sth		char* sz;
//sth		sz = itoa(size,sz,3); who allocates the memory???; radix = 3???
		char sz[33];
		sprintf(sz,"%d",size);
		string relno(sz);
		sId = sSource + sTarget + relno;
	}
	dsEdge.GetTextAttribute("xsi:type", sType);
	rRelation.sId = sId;
	rRelation.sName = sName;
	rRelation.sType = sType;

	mseSemEntities[sSource].msEntRelations[sTarget].insert(RelMapType::value_type(sId,rRelation));

/*
	sEntRelation.append(" ");
	sEntRelation.append(sName);
	sEntRelation.append(" ");

	mseSemEntities[sSource].msEntRelations[sTarget] = sEntRelation;
*/
    return 0;
}
int StateMemberSearchTool::ReadGraphNode(GenericDS dsNode,STM_SEMapType& mseSemEntities)
{
	string sId,sIdref;

	dsNode.GetTextAttribute("id", sId);
	dsNode.GetTextAttribute("idref", sIdref);
	return 0;
}
int StateMemberSearchTool::ReadGraphProperty(GenericDS dsProperty,STM_SEMapType& mseSemEntities)
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
int StateMemberSearchTool:: 
       ReadSemanticGraph(GenericDS dsSemGraph, STM_SEMapType& mseSemEntities)
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
			ReadGraphEdge(dsElement,mseSemEntities);
		else if(dsElement.GetDSName() == "Property")
			ReadGraphProperty(dsElement,mseSemEntities);
		else if(dsElement.GetDSName() == "Node")
			ReadGraphNode(dsElement,mseSemEntities);
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
int StateMemberSearchTool:: 
       TypeOfSemanticEntity(GenericDS dsSemEntity, STM_SemanticEntityType& setType)
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
	for (i=0; i<9; i++) {
		if (sDSName == SEMANTIC_ENTITY_TYPES[i]) {
			iType = i;
			break;
		}
	}
	if ( iType > 8 || iType < 0 ) {
		return 1;
	} 

	setType = (STM_SemanticEntityType) iType;

	return 0;
}

//----------------------------------------------------------------------------
double StateMemberSearchTool:: 
       SemanticMatching(
			STM_SEMapType mseSemEntitiesRef,
			STM_SEMapType mseSemEntitiesQuery)
{
	// Variable declaration
	double dist = 0.0, avgavgDist= 0.0, tmpDist, minDist, avgminDist = 0.0;
	STM_SEMapType::iterator seIterRef, seIterQuery;

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
void StateMemberSearchTool::ReplaceStrings(string& string, char *strListTypes, ...)
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
void StateMemberSearchTool::ReplaceInString(string& str, char *str1, char *str2)
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
double StateMemberSearchTool::MatchStrings(string str1, string str2)
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

double StateMemberSearchTool::GetMembershipDistance(STM_SEMapType mseSemEntitiesRef,
												 STM_SEMapType mseSemEntitiesQuery)
{
	double dist = 0.0, mfQ, mfR;
	STM_SEMapType::iterator seIterRef, seIterQuery;

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
StateMemberSearchInterfaceABC *StateMemberSearchTool::GetInterface(void)
{ return &m_Interface; }

