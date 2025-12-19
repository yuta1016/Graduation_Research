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
// Applicable File Name:  StateTransitionModelExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

//=============================================================================
	
#ifdef WIN32
//#include <strstrea>
#include <strstream>
#else
#include <strstream>
#endif

#ifdef _VISUALC_
#define STREAMNS(stream) stream
#else
#define STREAMNS(stream) std::stream
#endif

static const int VERB = 0;

using namespace XM;
using namespace std;

//=============================================================================
// Local functions for manipulating StateTransitionModel DS in memory
//=============================================================================

// Constructs state-transition matrix from vector of states (v)

VectorR ConstructSTMatrix(const VectorR& v, int dictlen) {
  VectorR temp(dictlen*dictlen);
  temp.clear();
  temp.setId(v.Id());
  int I = 0, J = 0;
  float *g = 0;
  const float *h = v.data();;
  for (int i = 0; i < v.length()-1; i++, h++) {
	  I = *h;
	  J = *(h+1);
	  g = temp.data_nc(I*dictlen + J);
	  *g = *g + 1;
	  }
  return temp;
}

//=============================================================================

const UUID StateTransitionModelExtractionInterface::myID = UUID("");
const char *StateTransitionModelExtractionInterface::myName = "StateTransitionModelDSExtractionInterface";

const UUID StateTransitionModelExtractionTool::myID = UUID("");
const char *StateTransitionModelExtractionTool::myName = "StateTransitionModelExtraction";

const UUID StateTransitionModelExtractionInterfaceABC::myID = UUID();

//=============================================================================
StateTransitionModelExtractionInterfaceABC::StateTransitionModelExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
StateTransitionModelExtractionInterface::StateTransitionModelExtractionInterface(StateTransitionModelExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
StateTransitionModelExtractionInterface::~StateTransitionModelExtractionInterface()
{
}

//----------------------------------------------------------------------------
void StateTransitionModelExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//----------------------------------------------------------------------------
int StateTransitionModelExtractionInterface::SetParams(ifstream& inf, int offset, int numsamples) 
{
  return m_ExtractionTool->SetParams(inf, offset, numsamples);

}


//----------------------------------------------------------------------------
int StateTransitionModelExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)

{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelExtractionInterface::PostExtraction(void)

{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& StateTransitionModelExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateTransitionModelExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
StateTransitionModelExtractionTool::StateTransitionModelExtractionTool():
m_Interface(this),
m_DescriptionInterface(0),
m_Inf(0),
m_Offset(0),
m_NumSamples(0)
{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
StateTransitionModelExtractionTool::StateTransitionModelExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(theParent),
m_Inf(0),
m_Offset(0),
m_NumSamples(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StateTransitionModelExtractionTool::~StateTransitionModelExtractionTool()
{

}

//----------------------------------------------------------------------------
bool StateTransitionModelExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool StateTransitionModelExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------

int StateTransitionModelExtractionTool::SetParams(ifstream& inf, 
												  int offset, 
												  int numsamples) {
	m_Inf		= &inf;
	m_Offset	= offset;
	m_NumSamples = numsamples;
	return(0);
}

//----------------------------------------------------------------------------
int StateTransitionModelExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelExtractionTool::InitExtracting(void)
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelExtractionTool::StartExtracting(void)
{

	ExtractStateTransitionModel( 1, m_DescriptionInterface);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelExtractionTool::PostExtraction(void)
{
	return 0;
}

//----------------------------------------------------------------------------
const UUID& StateTransitionModelExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateTransitionModelExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
StateTransitionModelExtractionInterfaceABC *StateTransitionModelExtractionTool::GetInterface(void)
{ return &m_Interface; }

	
static int const MAXNUMSTATES = 20;
static int const MAXSAMPLES = 2000;

GenericDSInterfaceABC*
StateTransitionModelExtractionTool::ExtractStateTransitionModel(int iStateTransitionModelID, 
									GenericDSInterfaceABC *parentInterface) {

	istream& in = *m_Inf;
	int ID, numentries = 1;
	float SCORE, SYMBSTR[MAXSAMPLES];
	int scored, nSCORE;
	STREAMNS(strstream) s;
	float INIT[MAXNUMSTATES];

	// First line in scores file gives label for data
	char label[256], buf[256];
	in.getline(buf, 255);
	sprintf(label, "%s - computed from samples %d through %d", buf, m_Offset, m_Offset+m_NumSamples-1);
	
	// Skip the first m_Offset samples
	for (int k = 0; k < m_Offset; k++) in.getline(buf, 255);

	// Get first sample (sample ID and VALUE) and start symbol string
	in >> ID >> SCORE;
	scored = SCORE;
	SYMBSTR[0] = SCORE;
	s << SCORE;
		
	// Get remaining m_NumSamples samples (sample IDs and VALUEs) and cat to symbol string
	while (in >> ID >> nSCORE && numentries < m_NumSamples) {
		s << " " << nSCORE;
		SYMBSTR[numentries] = nSCORE;
		numentries++;
		}
	s << '\0';

	// Got all samples
	if (VERB) cerr << "Got " << numentries << " entries" << endl;

	// Construct vector (sequence) of samples from symbol string
	VectorR VSTR(numentries, SYMBSTR);

	// Construct state-transition matrix from sequence of states
	VectorR PT(ConstructSTMatrix(VSTR, MAXNUMSTATES));

	// Print for testing
	//VSTR.write(cout) << endl;
	//cout << PT.sum() << ", ";
	//PT.write(cout) << endl << endl;

	// Normalize (L1) the thing to 1 to transform transition counts to frequencies
	VectorR NPT(PT);
	NPT.normalize(1, 1);

	// Set up array of initial state probabilities (all zero except first symbol)
	for (int n = 0; n < MAXNUMSTATES; n++) INIT[n]=0;
	INIT[(int)*(VSTR.data())] = 1;

	/*
	<StateTransitionModel numberOfStates="3">
		<Initial dim="3"> 0.25 0.5 0.25 </Initial>
		<Transitions dim="3 3"> 0.2 0.2 0.6 0.1 0.8 0.1 0.3 0.3 0.4 </Transitions>
		<State><Label> Pass </Label></State>
		<State><Label> Shot on goal </Label></State>
		<State><Label> Goal score </Label></State>
	</StateTransitionModel>
	*/

	// ------------------------------------------------------------------------
	// Create root of the StateTransitionModel
	// ------------------------------------------------------------------------
	
	GenericDS StateTransitionModel = parentInterface->CreateChild("StateTransitionModel");
	GenericDSInterfaceABC *StateTransitionModelInterface = StateTransitionModel.GetInterface();

	StateTransitionModelInterface->SetAttribute("numberOfStates", MAXNUMSTATES);
	StateTransitionModelInterface->SetAttribute("confidence", (float)(1.0 - 1.0/(1.0+MAXNUMSTATES)));
	StateTransitionModelInterface->SetAttribute("reliability", (float)0.5);

	// ------------------------------------------------------------------------
	// Extract Initial State Probability Vector
	// ------------------------------------------------------------------------

	GenericDS InitialState = StateTransitionModelInterface->CreateChild("Intial");
	GenericDSInterfaceABC *InitialStateInterface = InitialState.GetInterface();
	InitialStateInterface->SetAttribute("dim", MAXNUMSTATES);

	STREAMNS(strstream) statestr;
	{
	  for (int i = 0; i < MAXNUMSTATES; i++) statestr << INIT[i] << " ";
	}
	statestr << '\0';
	InitialStateInterface->SetValue(statestr.str());
	delete [] statestr.str();

	// ------------------------------------------------------------------------
	// Extract Transitions Matrix 
	// ------------------------------------------------------------------------

	char dimbuf[256];
	sprintf(dimbuf, "%d %d", MAXNUMSTATES, MAXNUMSTATES);
	GenericDS Transitions = StateTransitionModelInterface->CreateChild("Transitions");
	GenericDSInterfaceABC *TransitionsInterface = Transitions.GetInterface();
	TransitionsInterface->SetAttribute("dim", dimbuf);

	STREAMNS(strstream) matstr;
	{
	  for (int i = 0; i < NPT.length(); i++) matstr << *NPT.data(i) << " ";
	}
	matstr << '\0';
	TransitionsInterface->SetValue(matstr.str());
	delete [] matstr.str();

	// ------------------------------------------------------------------------
	// Extract States
	// ------------------------------------------------------------------------

	for (int j = 0; j < MAXNUMSTATES; j++) {
		GenericDS State = StateTransitionModelInterface->CreateChild("State");
		GenericDSInterfaceABC *StateInterface = State.GetInterface();
		char buf[256];
		sprintf(buf, "State %d", j);
		StateInterface->SetAttribute("Label", buf);
		float confidence = ((float)m_NumSamples/(5.0*MAXNUMSTATES));
		confidence = (confidence > 0.5) ? 0.5 : confidence;
		confidence += 0.5*sqrt(1.0/(j+1));
		StateInterface->SetAttribute("confidence", confidence);
	}

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}

