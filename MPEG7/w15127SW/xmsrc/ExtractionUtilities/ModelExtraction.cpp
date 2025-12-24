///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ModelExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================
	
#ifdef WIN32
//#include <strstrea.h>
#include <strstream>
#else
#include <strstream>
#endif

#ifdef _VISUALC_
#define STREAMNS(stream) stream
#else
#define STREAMNS(stream) std::stream
#endif

static const int VERB = 0;//sth define??

using namespace XM;
using namespace std;

//=============================================================================
// Local classes for handling Model DS in memory
//=============================================================================

class DescriptorCollectionDS {

public:

	char d_label[256];
	int d_length;
	VectorR *d_vlist;

	DescriptorCollectionDS() : d_vlist(0) {}

	~DescriptorCollectionDS() { 
		delete [] d_vlist; 
		}

	istream& read(istream& in) {
		in.getline(d_label, 255);
		in >> d_length;
		if (VERB) cerr << "Length = " << d_length << endl;
		delete [] d_vlist;
		d_vlist = new VectorR[d_length];
		for (int i = 0; i < d_length; i++) {
			d_vlist[i].read(in);
			d_vlist[i].normalize(1, 1);
			}
		return in;
		}

	ostream& write(ostream& out) {
		out << d_label << endl;
		out << d_length << endl;
		for (int i = 0; i < d_length; i++) {
			d_vlist[i].write(out) << endl;
			}

		return out;
		}

VectorR mean() {
	if (d_length==0) return 0;
	//return d_vlist[0];   // <-- JRS: FIX?
	VectorR M(d_vlist[0].length());
	M.clear();
	for (int i = 0; i < d_length; i++) {
		M+=d_vlist[0];
		}
	if (d_length>0) M/=d_length;
	return M;
}


VectorR variance() {
	if (d_length==0) return 0;
	VectorR M(mean());
	M*=M;
	VectorR T(d_vlist[0].length());
	T.clear();
	for (int i = 0; i < d_length; i++) {
		VectorR H(d_vlist[i]);
		H*=H;
		H-=M;
		T+=H;
		}
	if (d_length>0) T/=d_length;
	return T;
}

	};

static DescriptorCollectionDS E;

//=============================================================================


const UUID ModelExtractionInterface::myID = UUID("");
const char *ModelExtractionInterface::myName = "ModelDSExtractionInterface";

const UUID ModelExtractionTool::myID = UUID("");
const char *ModelExtractionTool::myName = "ModelExtraction";

const UUID ModelExtractionInterfaceABC::myID = UUID();


//=============================================================================
ModelExtractionInterfaceABC::ModelExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ModelExtractionInterface::ModelExtractionInterface(ModelExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ModelExtractionInterface::~ModelExtractionInterface()
{
}


//----------------------------------------------------------------------------
void ModelExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//----------------------------------------------------------------------------
int ModelExtractionInterface::SetSourceMedia(ifstream& in)
{
  return m_ExtractionTool->SetSourceMedia(in);
}


//----------------------------------------------------------------------------
int ModelExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)

{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long ModelExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long ModelExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long ModelExtractionInterface::PostExtraction(void)

{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& ModelExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ModelExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
ModelExtractionTool::ModelExtractionTool():
m_Interface(this),
m_DescriptionInterface(0),
m_Inf(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ModelExtractionTool::
ModelExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(theParent),
m_Inf(0)
{
  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
ModelExtractionTool::~ModelExtractionTool()
{

}

//----------------------------------------------------------------------------
bool ModelExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ModelExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------

int ModelExtractionTool::SetSourceMedia(ifstream& in) {
	m_Inf			= &in;
	return(0);
}

//----------------------------------------------------------------------------
int ModelExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long ModelExtractionTool::InitExtracting(void)
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ModelExtractionTool::StartExtracting(void)
{
	// Creates one description that combines three different collection/model types

	GenericDS Models = m_DescriptionInterface->CreateChild("CollectionModels");
	GenericDSInterfaceABC *ModelsInterface = Models.GetInterface();

	istream& in = *m_Inf;  // parameter file for ProbabilityModelClass
	E.read(in);
	if (VERB) E.write(cerr);
	
	ExtractContentCollection( 1, ModelsInterface);
	ExtractDescriptorCollection( 1, ModelsInterface);
	ExtractProbabilityModelClass( 1, ModelsInterface);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ModelExtractionTool::PostExtraction(void)
{
	return 0;
}

//----------------------------------------------------------------------------
const UUID& ModelExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ModelExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ModelExtractionInterfaceABC *ModelExtractionTool::GetInterface(void)
{ return &m_Interface; }

	
/*
<ProbabilityModelClass confidence="0.75" reliability="0.5">
	<Label> Nature scenes </Label>
	<ProbabilityModel xsi:type="ProbabilityDistributionType" confidence="1.0" 
 			dim="16"> 
		<Mean dim="16"> 0.5 0.5 ... 0.25 </Mean>
		<Variance dim="16"> 0.25 0.75 ... 0.5 </Variance>
	</ProbabilityModel>
</ProbabilityModelClass>

*/

//sth function delimiter
GenericDSInterfaceABC*
ModelExtractionTool::
ExtractProbabilityModelClass(int iModelID,
			     GenericDSInterfaceABC *parentInterface)
{


	// ------------------------------------------------------------------------
	// Create root of the Model (Probability Model Class)
	// ------------------------------------------------------------------------
	
	GenericDS ProbabilityModelClass = parentInterface->CreateChild("ProbabilityModelClass");
	GenericDSInterfaceABC *ProbabilityModelClassInterface = ProbabilityModelClass.GetInterface();

	GenericDS Label = ProbabilityModelClassInterface->CreateChild("Label");
	GenericDSInterfaceABC *LabelInterface = Label.GetInterface();
	LabelInterface->SetValue(E.d_label);

	ProbabilityModelClassInterface->SetAttribute("confidence", (float)1.0 - 1.0/(1.0+E.d_length));
	ProbabilityModelClassInterface->SetAttribute("reliability", (float)0.5);

	// ------------------------------------------------------------------------
	// Create Gaussian element
	// ------------------------------------------------------------------------

	GenericDS ProbabilityModel = ProbabilityModelClassInterface->CreateChild("ProbabilityModel");
	GenericDSInterfaceABC *ProbabilityModelInterface = ProbabilityModel.GetInterface();

	// ------------------------------------------------------------------------
	// Extract Mean
	// ------------------------------------------------------------------------

	VectorR M(E.mean());

	GenericDS Mean = ProbabilityModelInterface->CreateChild("Mean");
	GenericDSInterfaceABC *MeanInterface = Mean.GetInterface();
	MeanInterface->SetAttribute("dim", M.length());

	STREAMNS(strstream) vstr;
	{
	  for (int i = 0; i < M.length(); i++) vstr << *(M.data(i)) << " ";
	}
	vstr << '\0';
	MeanInterface->SetValue(vstr.str());
	delete [] vstr.str();

	// ------------------------------------------------------------------------
	// Extract Variance
	// ------------------------------------------------------------------------

	VectorR V(E.variance());

	GenericDS Variance = ProbabilityModelInterface->CreateChild("Variance");
	GenericDSInterfaceABC *VarianceInterface = Variance.GetInterface();
	VarianceInterface->SetAttribute("dim", V.length());

	STREAMNS(strstream) varstr;
	{
	  for (int i = 0; i < V.length(); i++) varstr << *(V.data(i)) << " ";
	}
	varstr << '\0';
	VarianceInterface->SetValue(varstr.str());
	delete [] varstr.str();

	ProbabilityModelInterface->SetAttribute("confidence", (float)1.0 - 1.0/(1.0+E.d_length));
	ProbabilityModelInterface->SetAttribute("dim", V.length());

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}


/*
<DescriptorCollection>
	<Descriptor xsi:type="ScalableColorType" 
		numberOfCoefficients="16" numberOfBitplancesDiscarded="0">
		<Coefficients dim="16"> 1 2 . . . 16 </Coefficients>
	</Descriptor>
	<Descriptor xsi:type="ScalableColorType" 
		numberOfCoefficients="16" numberOfBitplancesDiscarded="0">
		<Coefficients dim="16"> 1 2 . . . 16 </Coefficients>
	</Descriptor>
		:
</DescriptorCollection>
*/


//sth function delimiter
GenericDSInterfaceABC*
ModelExtractionTool::ExtractDescriptorCollection(int iModelID, 
										GenericDSInterfaceABC *parentInterface) {

	// ------------------------------------------------------------------------
	// Create root of the Model (DescriptorCollection)
	// ------------------------------------------------------------------------
	
	GenericDS DescriptorCollection = parentInterface->CreateChild("DescriptorCollection");
	GenericDSInterfaceABC *DescriptorCollectionInterface = DescriptorCollection.GetInterface();

	GenericDS Label = DescriptorCollectionInterface->CreateChild("Label");
	GenericDSInterfaceABC *LabelInterface = Label.GetInterface();
	LabelInterface->SetValue(E.d_label);

	DescriptorCollectionInterface->SetAttribute("confidence", (float)1.0 - 1.0/(1.0+E.d_length));
	DescriptorCollectionInterface->SetAttribute("reliability", (float)0.5);

	// ------------------------------------------------------------------------
	// Extract DescriptorCollection Descriptors
	// ------------------------------------------------------------------------

	for (int j = 0; j < E.d_length; j++) {
	GenericDS DescriptorExample = DescriptorCollectionInterface->CreateChild("Descriptor");
	GenericDSInterfaceABC *DescriptorExampleInterface = DescriptorExample.GetInterface();

	STREAMNS(strstream) vstr;
	for (int i = 0; i < E.d_vlist[j].length(); i++) vstr << *(E.d_vlist[j].data(i)) << " ";
	vstr << '\0';
	DescriptorExampleInterface->SetValue(vstr.str());
	delete [] vstr.str();

	}

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}


/*
<ContentCollection>
	<Content xsi:type="ImageType">
		<Image>
			<MediaLocator xsi:type="ImageLocatorType">
 				<MediaUri>soccer1.jpg</MediaUri>
			</MediaLocator>
		</Image>
	</Content>
	<Content xsi:type="ImageType">
		<Image>
			<MediaLocator xsi:type="ImageLocatorType">
				<MediaUri>soccer2.jpg</MediaUri>
			</MediaLocator>
		</Image>
	</Content>
</ContentCollection>
*/

//sth
GenericDSInterfaceABC*
ModelExtractionTool::ExtractContentCollection(int iModelID, 
									GenericDSInterfaceABC *parentInterface) {

	// ------------------------------------------------------------------------
	// Create root of the Model (ContentCollection)
	// ------------------------------------------------------------------------
	
	GenericDS ContentCollection = parentInterface->CreateChild("ContentCollection");
	GenericDSInterfaceABC *ContentCollectionInterface = ContentCollection.GetInterface();

	// ------------------------------------------------------------------------
	// Extract ContentCollection Descriptors
	// ------------------------------------------------------------------------

	for (int j = 0; j < E.d_length; j++) {
	
		GenericDS Content = ContentCollectionInterface->CreateChild("Content");
		GenericDSInterfaceABC *ContentInterface = Content.GetInterface();
		ContentInterface->SetAttribute("xsi:type", "ImageType");

		GenericDS Media = ContentInterface->CreateChild("Image");
		GenericDSInterfaceABC *MediaInterface = Media.GetInterface();

		GenericDS MediaLocator = MediaInterface->CreateChild("MediaLocator");
		GenericDSInterfaceABC *MediaLocatorInterface = MediaLocator.GetInterface();

		GenericDS SourceMediaUri = MediaLocatorInterface->CreateChild("MediaUri");
		GenericDSInterfaceABC *SourceMediaUriInterface = SourceMediaUri.GetInterface();
		char buf[256];
		sprintf(buf, "image%d.jpg", j);
		SourceMediaUriInterface->SetValue(buf);
	}

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

}


