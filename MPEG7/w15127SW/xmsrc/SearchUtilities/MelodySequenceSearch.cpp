//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Thorsten Heinz
// Fraunhofer IIS
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
// Applicable File Name:  MelodySequenceSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID MelodySequenceSearchInterface::myID = UUID("");
const char *MelodySequenceSearchInterface::myName = 
                                      "MelodySequenceMatchingInterface";

const UUID MelodySequenceSearchTool::myID = UUID("");
const char *MelodySequenceSearchTool::myName = 
                                      "MelodySequenceMatchingTool";

const UUID MelodySequenceSearchInterfaceABC::myID = UUID();


//=============================================================================
MelodySequenceSearchInterfaceABC::MelodySequenceSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MelodySequenceSearchInterface::MelodySequenceSearchInterface(MelodySequenceSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MelodySequenceSearchInterface::~MelodySequenceSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MelodySequenceSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MelodySequenceSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void MelodySequenceSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int MelodySequenceSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefMelodySequenceDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefMelodySequenceDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* MelodySequenceSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int MelodySequenceSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QueryMelodySequenceDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryMelodySequenceDescriptionInterface);
}

//----------------------------------------------------------------------------
double MelodySequenceSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
MelodySequenceSearchTool::MelodySequenceSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
MelodySequenceSearchTool::
MelodySequenceSearchTool(GenericDSInterfaceABC
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
MelodySequenceSearchTool::~MelodySequenceSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& MelodySequenceSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MelodySequenceSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MelodySequenceSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MelodySequenceSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MelodySequenceSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefMelodySequenceDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefMelodySequenceDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* MelodySequenceSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  MelodySequenceSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryMelodySequenceDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQueryMelodySequenceDescriptionInterface;
  
  /* could also look totally different if
     aQueryMelodySequenceDescriptionInterface is not of MelodySequence node*/

  return 0;
}

//----------------------------------------------------------------------------
double MelodySequenceSearchTool::GetDistance(void)
{
  double dist;
  GenericDS MelodySequenceRef;
  int          NoteValueTmp;
  unsigned int i;

  
  // Check it's all set up ok
  if(m_RefDescriptionInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptionInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);


 
  //--- Extract Query data
  //--- MelodyType
  QueryMelodyType = m_QueryDescriptionInterface->GetFirstChild();

  if(strcmp(QueryMelodyType.GetDSName().c_str(),"MelodyType") != 0) {
	  std::cerr << "Error in XML-File:\n"
	 << "Instead of expected DSName \"MelodyType\" found"
	 << QueryMelodyType.GetDSName().c_str() << "\n";;
    exit(1);
  }

  //--- MelodySequence
  QueryMelodySequence = QueryMelodyType.GetDescription("MelodySequence");

  //--- NoteArray
  QueryNoteArray = QueryMelodySequence.GetDescription("NoteArray");
  //--- Notes
  QueryNoNotes = QueryNoteArray.GetNumberOfChildren();

  for( i = 0; i < QueryNoNotes; i++) {
    QueryNote = QueryNoteArray.GetChild(i);
    QueryInterval = QueryNote.GetDescription("Interval");
    QueryInterval.GetIntValue(NoteValueTmp);
    QueryNoteValues.push_back(NoteValueTmp);
  }


  
  //--- Extract Database data
  //--- MelodyType
  DBMelodyType = m_RefDescriptionInterface->GetFirstChild();  
  if(strcmp(DBMelodyType.GetDSName().c_str(),"MelodyType") != 0) {
    std::cerr << "Error in XML-File:\n"
	 << "Instead of expected DSName \"MelodyType\" found"
	 << DBMelodyType.GetDSName().c_str() << "\n";;
    exit(1);
  }

  //--- MelodySequence
  DBMelodySequence = DBMelodyType.GetDescription("MelodySequence");

  //--- NoteArray
  DBNoteArray = DBMelodySequence.GetDescription("NoteArray");

  //--- Notes
  DBNoNotes = DBNoteArray.GetNumberOfChildren();

  for( i = 0; i < DBNoNotes; i++) {
    DBNote = DBNoteArray.GetChild(i);
    DBInterval = DBNote.GetDescription("Interval");
    DBInterval.GetIntValue(NoteValueTmp);
    DBNoteValues.push_back(NoteValueTmp);
  }
  
  
  //--- Ausgabe der Noten
  //--- Query
  std::cout << "QueryData:\n";
  for( i = 0; i < QueryNoteValues.size(); i++) {
    std::cout << "Note(" << i << ") = " << QueryNoteValues[i] << "\n";
  }
  //--- DataBase
  std::cout << "DBData:\n";
  for( i = 0; i < DBNoteValues.size(); i++) {
    std::cout << "Note(" << i << ") = " << DBNoteValues[i] << "\n";
  }



  //--- Calculate similarity (very simple, just to have an algorithm)
  dist = 0;
  unsigned int minNoNotes;

  if(QueryNoteValues.size() <= DBNoteValues.size())
    minNoNotes = QueryNoteValues.size();
  else
    minNoNotes = DBNoteValues.size();


  for( i = 0; i < minNoNotes; i++) {
    dist += abs(QueryNoteValues[i] - DBNoteValues[i]);
  }

  dist = dist/double(minNoNotes);



  //--- tidy up
  QueryNoteValues.clear();
  DBNoteValues.clear();

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
MelodySequenceSearchInterfaceABC *MelodySequenceSearchTool::GetInterface(void)
{ return &m_Interface; }

