///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation StateTransitionModel.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation StateTransitionModel tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  StateTransitionModelSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fstream> //LINUX_2011

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "SearchUtilities/SearchUtilities.h"

static const int VERB = 1;

#ifdef WIN32
//#include <strstrea.h>
#include <strstream>
#else
#include <strstream>
#include <cstring> //LINUX_2011
#endif

//=============================================================================

using namespace XM;
using namespace std;


const UUID StateTransitionModelSearchInterface::myID = UUID("");
const char *StateTransitionModelSearchInterface::myName = "StateTransitionModel Transcoding Interface";

const UUID StateTransitionModelSearchTool::myID = UUID("");
const char *StateTransitionModelSearchTool::myName = "StateTransitionModel Transcoding Tool";

const UUID StateTransitionModelSearchInterfaceABC::myID = UUID();


//=============================================================================
StateTransitionModelSearchInterfaceABC::StateTransitionModelSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
StateTransitionModelSearchInterface::StateTransitionModelSearchInterface(StateTransitionModelSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
StateTransitionModelSearchInterface::~StateTransitionModelSearchInterface()
{
}

//----------------------------------------------------------------------------
void StateTransitionModelSearchInterface::destroy()
{  delete m_SearchTool; }


//----------------------------------------------------------------------------
int StateTransitionModelSearchInterface::SetDescriptionInterface(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget)
{
  return m_SearchTool->SetDescriptionInterface(theQuery, theTarget);
}

//----------------------------------------------------------------------------
int StateTransitionModelSearchInterface::SetStateTransitionModelParms(const char* queryfile, const char* targetfile) 
{
	return m_SearchTool->SetStateTransitionModelParms(queryfile, targetfile);
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelSearchInterface::StartMatching(void)
{
	return m_SearchTool->StartMatching();
}

//----------------------------------------------------------------------------
const UUID& StateTransitionModelSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateTransitionModelSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
StateTransitionModelSearchTool::StateTransitionModelSearchTool():
m_Interface(this),
m_QueryInterface(0),
m_TargetInterface(0),
m_QueryFile(0),
m_TargetFile(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StateTransitionModelSearchTool::StateTransitionModelSearchTool(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget):
m_Interface(this),
m_QueryInterface(0),
m_TargetInterface(0),
m_QueryFile(0),
m_TargetFile(0)
{
	SetDescriptionInterface(theQuery, theTarget);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StateTransitionModelSearchTool::~StateTransitionModelSearchTool()
{
	delete [] m_QueryFile;
	delete [] m_TargetFile;
}

//----------------------------------------------------------------------------
const UUID& StateTransitionModelSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateTransitionModelSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool StateTransitionModelSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool StateTransitionModelSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int StateTransitionModelSearchTool::SetDescriptionInterface(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget)
{
  m_QueryInterface = theQuery;
  m_TargetInterface = theTarget;
  return 0;
}

//----------------------------------------------------------------------------
int StateTransitionModelSearchTool::SetStateTransitionModelParms(const char* queryfile, const char* targetfile) 
{
	if (m_QueryFile) delete [] m_QueryFile;
	if (queryfile) {
		m_QueryFile = new char[strlen(queryfile)+1];
		strcpy(m_QueryFile, queryfile);
	}
	else m_QueryFile = 0;

	if (m_TargetFile) delete [] m_TargetFile;
	if (targetfile) {
		m_TargetFile = new char[strlen(targetfile)+1];
		strcpy(m_TargetFile, targetfile);
	}
	else m_TargetFile = 0;

return 0;

}

struct StateTXModel {
	double confidence, reliability, numberOfStates;
	VectorR intial;
	VectorR transmatrix;
};

/*
<StateTransitionModel confidence="0.952381" reliability="0.5" numberOfStates="20">
  <Intial dim="20">0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0</Intial> 
  <Transitions dim="20 20">0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0 0 0 0.011236 0 0 0 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0 0.011236 0 0 0 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0.0224719 0 0.011236 0 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.0224719 0.0561798 0.011236 0.011236 0 0 0 0 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0.0224719 0.0224719 0.011236 0.011236 0.011236 0.011236 0 0 0 0 0 0 0 0 0 0 0.011236 0 0 0.011236 0 0.0224719 0.011236 0.011236 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0.011236 0.0224719 0 0.0224719 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0 0.0224719 0 0.011236 0.0224719 0 0.011236 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0 0 0.011236 0.0561798 0.0224719 0.0224719 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.011236 0 0 0.0337079 0.011236 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.0337079 0.011236 0 0.0561798 0.011236 0 0 0 0 0 0 0 0 0 0 0 0.011236 0.011236 0 0 0 0 0 0 0.101124 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0</Transitions> 
  <State Label="State 0" confidence="1" /> 
  <State Label="State 1" confidence="0.853553" /> 
  <State Label="State 2" confidence="0.788675" /> 
  <State Label="State 3" confidence="0.75" /> 
  <State Label="State 4" confidence="0.723607" /> 
  <State Label="State 5" confidence="0.704124" /> 
  <State Label="State 6" confidence="0.688982" /> 
  <State Label="State 7" confidence="0.676777" /> 
  <State Label="State 8" confidence="0.666667" /> 
  <State Label="State 9" confidence="0.658114" /> 
  <State Label="State 10" confidence="0.650756" /> 
  <State Label="State 11" confidence="0.644338" /> 
  <State Label="State 12" confidence="0.638675" /> 
  <State Label="State 13" confidence="0.633631" /> 
  <State Label="State 14" confidence="0.629099" /> 
  <State Label="State 15" confidence="0.625" /> 
  <State Label="State 16" confidence="0.621268" /> 
  <State Label="State 17" confidence="0.617851" /> 
  <State Label="State 18" confidence="0.614708" /> 
  <State Label="State 19" confidence="0.611803" /> 
  </StateTransitionModel>
*/

//----------------------------------------------------------------------------
unsigned long StateTransitionModelSearchTool::StartMatching(void)
{
	double confidence, reliability, numberOfStates, d;
	int initial_dim = 0, transitions_x_dim = 0, transitions_y_dim = 0, matrix_dim = 0;
	char buf[1024];

	if (VERB) cerr << "Matching " << m_QueryFile << " against " << m_TargetFile << endl;

	// Read Target Description

	GenericDS StateTransitionModelDS = m_QueryInterface->GetFirstChild();
	GenericDSInterfaceABC *pStateTransitionModelDSInt = StateTransitionModelDS.GetInterface();

	if (pStateTransitionModelDSInt->isNull()) {
		cerr << "Error - NULL Child" << endl;
		return 0;
	}

	pStateTransitionModelDSInt->GetRealAttribute("confidence", confidence);
	pStateTransitionModelDSInt->GetRealAttribute("reliability", reliability);
	pStateTransitionModelDSInt->GetRealAttribute("numberOfStates", numberOfStates);

	if (VERB) cerr << "confidence=" << confidence << " reliability=" << reliability << " numberOfStates=" << numberOfStates << endl;

	GenericDS InitialDS = pStateTransitionModelDSInt->GetFirstChild();
	GenericDSInterfaceABC *pInitialDSInt = InitialDS.GetInterface();
	pInitialDSInt->GetIntAttribute("dim", initial_dim);

	string sbuf;
	if (initial_dim>1000) return 0;
	pInitialDSInt->GetTextValue(sbuf);
	strcpy(buf, sbuf.c_str());
	std::strstream sval;
	sval << buf;
	VectorR initvec(initial_dim);
	int n = 0;
	while (sval >> d && n < initial_dim) *(initvec.data_nc(n++)) = d;
	if(VERB) initvec.write(cerr) << endl;

	GenericDS TransitionsDS = pInitialDSInt->GetNextSibling();
	GenericDSInterfaceABC *pTransitionsDSInt = TransitionsDS.GetInterface();
	string dims;
	pTransitionsDSInt->GetTextAttribute("dim",dims);
	strcpy(buf, dims.c_str());

	std::strstream s;
	s << buf;
	s >> transitions_x_dim >> transitions_y_dim;
	matrix_dim = transitions_x_dim*transitions_y_dim;

	string tbuf;
	if (matrix_dim>1000) return 0;

	pTransitionsDSInt->GetTextValue(tbuf);
	strcpy(buf, tbuf.c_str());
	std::strstream tval;
	tval << buf;
	VectorR transmatrix(matrix_dim);
	n = 0;
	while (tval >> d && n < matrix_dim) *(transmatrix.data_nc(n++)) = d;

	if (VERB) transmatrix.write(cerr) << endl;

	return 0;
}

//----------------------------------------------------------------------------
StateTransitionModelSearchInterfaceABC *StateTransitionModelSearchTool::GetInterface(void)
{ return &m_Interface; }

