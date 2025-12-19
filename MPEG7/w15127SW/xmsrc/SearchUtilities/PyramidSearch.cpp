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
// Applicable File Name:  PyramidSearch.cpp
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


const UUID PyramidSearchInterface::myID = UUID("");
const char *PyramidSearchInterface::myName = "PyramidMatchingInterface";

const UUID PyramidSearchTool::myID = UUID("");
const char *PyramidSearchTool::myName = "PyramidMatchingTool";

const UUID PyramidSearchInterfaceABC::myID = UUID();

//=============================================================================
PyramidSearchInterfaceABC::PyramidSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
PyramidSearchInterface::PyramidSearchInterface(PyramidSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PyramidSearchInterface::~PyramidSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& PyramidSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PyramidSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void PyramidSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int PyramidSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefPyramidDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefPyramidDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* PyramidSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int PyramidSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QueryPyramidDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryPyramidDescriptionInterface);
}

//----------------------------------------------------------------------------
double PyramidSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
PyramidSearchTool::PyramidSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
PyramidSearchTool::
PyramidSearchTool(GenericDSInterfaceABC
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
PyramidSearchTool::~PyramidSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& PyramidSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PyramidSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool PyramidSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool PyramidSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int PyramidSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefPyramidDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefPyramidDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* PyramidSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  PyramidSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryPyramidDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQueryPyramidDescriptionInterface;
  
  /* could also look totally different if
     aQueryPyramidDescriptionInterface is not of Pyramid node*/

  return 0;
}

//----------------------------------------------------------------------------
double PyramidSearchTool::GetDistance(void)
{
  double dist;
  string pstrTextAnnotationsRef[10], pstrTextAnnotationsQuery[10];

  // Check it's all set up ok
  if(m_RefDescriptionInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptionInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);

  if (GetAnnotationsPerLevel(m_RefDescriptionInterface, pstrTextAnnotationsRef) != 0 || 
	  GetAnnotationsPerLevel(m_QueryDescriptionInterface, pstrTextAnnotationsQuery) != 0) {
    return(DBL_MAX);
  }

  dist = AnnotationsPerLevelMatching(pstrTextAnnotationsRef, pstrTextAnnotationsQuery);

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
int PyramidSearchTool::
	GetAnnotationsPerLevel(GenericDSInterfaceABC *pdsiDSInt, string *pstrTextAnnotations)
{
  // Constants
  const string pstrPyramidLevels[10] = { "Type/Technique", "Global Distribution", "Local Structure",
	  "Global Composition", "Generic Object", "Generic Scene", "Specific Object", "Specific Scene", 
	  "Abstract Object", "Abstract Scene" };

  // Variable declaration
  GenericDS dsPyramid, dsFreeText;
  string strLevel, strText;
  int i;


  /* ------------------------------------------------------------------------
	   Read and validate the TextAnnotation
     ------------------------------------------------------------------------ */
  dsPyramid = pdsiDSInt->GetFirstChild();
  if (dsPyramid.isNull()) {
    fprintf(stderr, "Null Text Annotation\n");
	return(1);
  }
  if (dsPyramid.GetDSName() != "TextAnnotation") {
	fprintf(stderr, "Wrong DS name: TextAnnotation expected\n");
	return(1);
  }
  

  /* ------------------------------------------------------------------------
	   Read and validate the FreeText elements and the annotations per level
     ------------------------------------------------------------------------ */

  dsFreeText = dsPyramid.GetFirstChild();
  while (!dsFreeText.isNull()) {
    if (dsFreeText.GetDSName() != "FreeText") {
	  fprintf(stderr, "Wrong DS name: expected FreeText expected but found %s\n", 
		  dsFreeText.GetDSName().c_str());
	  return(1);
	}
	if (dsFreeText.GetTextAttribute("pyramidLevel", strLevel) != 0) {
	  fprintf(stderr, "Wrong text attribute pyramidLevel of FreeText: %s\n", strLevel);
	  return(1);
	}
	//fprintf(stderr, "Level = %s\n", strLevel.c_str());
	if (dsFreeText.GetTextValue(strText) != 0) {
	  fprintf(stderr, "Wrong text value of FreeText: %s\n", strText);
	  return(1);
	}
	//fprintf(stderr, "Text = %s\n", strText.c_str());

	// Collect all the annotations for a specific level of the pyramid
	// in the array pstrTextAnnotations
	for (i=0; i<10; i++) {
		if (strLevel == pstrPyramidLevels[i]) {
			pstrTextAnnotations[i].append(" ");
			pstrTextAnnotations[i].append(strText);
			pstrTextAnnotations[i].append(" ");
			break;
		}
	}
	dsFreeText = dsFreeText.GetNextSibling();
  }

  // TESTING
  //for (i=0; i<10; i++) {
    //fprintf(stderr, "Level %s: %s\n", pstrPyramidLevels[i].c_str(), pstrTextAnnotations[i].c_str());
  //}

  return 0;
}

//----------------------------------------------------------------------------
double PyramidSearchTool::
	AnnotationsPerLevelMatching(string *pstrTextAnnotsRef, string *pstrTextAnnotsQuery)
{
	// Variable declaration
	const string pstrPyramidLevels[10] = { "Type/Technique", "Global Distribution", "Local Structure",
	  "Global Composition", "Generic Object", "Generic Scene", "Specific Object", "Specific Scene", 
	  "Abstract Object", "Abstract Scene" };
	int iNumWordsRef = 0, iNumWordsQuery = 0, iNumMatches = 0;
	vector<string> vstrWordsRef, vstrWordsQuery;
	char aux[1024];
	int i, j, l, pos;
	double dist = 0.0;

	// For each level, 
	for (i=0; i<10; i++) {

		fprintf(stderr, "LEVEL: %s\n", pstrPyramidLevels[i].c_str());

		// Change orthographic symbols and dummy words to spaces
		ReplaceStrings(pstrTextAnnotsRef[i], "ssssssssssssssssssssss",
			       ".", ":", ";", ",", "(", ")", "/", " the ", " and ", " of ", 
			       " on ", " in ", " with ", " without ", " from ", " to ", " only ", "'s ", " at ", " as ", 
			       "\\", " ");

		ReplaceStrings(pstrTextAnnotsQuery[i], "ssssssssssssssssssssss",
			       ".", ":", ";", ",", "(", ")", "/", " the ", " and ", " of ", 
			       " on ", " in ", " with ", " without ", " from ", " to ", " only ", "'s ", " at ", " as ", 
			       "\\", " ");

		// Put words into vectors
		pos = 0;
		fprintf(stderr, "Ref words = ");
		while((sscanf(pstrTextAnnotsRef[i].c_str() + pos, "%s", aux)) > 0) {
			vstrWordsRef.push_back(string(aux));
			fprintf(stderr, "%s:", aux);
			pos = pstrTextAnnotsRef[i].find(string(aux), pos) + strlen(aux);
		}
		fprintf(stderr, "\n");
		iNumWordsRef = vstrWordsRef.size();

		pos = 0;
		fprintf(stderr, "Query words = ");
		while((sscanf(pstrTextAnnotsQuery[i].c_str() + pos, "%s", aux)) > 0) {
			vstrWordsQuery.push_back(string(aux));
			fprintf(stderr, "%s:", aux);
			pos = pstrTextAnnotsQuery[i].find(string(aux), pos) + strlen(aux);
		}
		fprintf(stderr, "\n");
		iNumWordsQuery = vstrWordsQuery.size();

		// Count word matches between query and ref
		for (j=0; j<iNumWordsQuery; j++) {
			for (l=0; l<iNumWordsRef; l++) {
				if ( _stricmp(vstrWordsQuery[j].c_str(), vstrWordsRef[l].c_str()) == 0) {
					iNumMatches++;
					break;
				}
			}
		}

		// The distance is calculated as the (1 - num matches / num maximum words)
		// The distance is accumulated for each level
		if (iNumWordsQuery > 0 || iNumWordsRef > 0) {
		  dist += (iNumWordsQuery > iNumWordsRef) ? 
		    (1.0 - (double) iNumMatches/iNumWordsQuery) : 
		    (1.0 - (double) iNumMatches/iNumWordsRef);
		}
	}

	// The distances for all 10 levels have been summed up, 
	// divide by 10 to have normalize from 0 to 1
	dist /= 10.0;

	return dist;
}

//----------------------------------------------------------------------------
void PyramidSearchTool::ReplaceStrings(string& string, char *strListTypes, ...)
{
	va_list vl;
	char *rpc_str;
	int i, last_str;

	va_start(vl, strListTypes);
	for (i=0; strListTypes[i]!='\0'; i++) {
		rpc_str = va_arg(vl, char *);
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
void PyramidSearchTool::ReplaceInString(string& str, char *str1, char *str2)
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
PyramidSearchInterfaceABC *PyramidSearchTool::GetInterface(void)
{ return &m_Interface; }

