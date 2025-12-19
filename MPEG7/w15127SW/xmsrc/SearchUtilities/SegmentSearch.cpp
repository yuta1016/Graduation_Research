//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
// Javier Ruiz Hidalgo - University Politechnic of Catalonia
//
// (contributing organizations names)

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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SegmentSearch.cpp
//

#include <stdio.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID SegmentSearchInterface::myID = UUID("");
const char *SegmentSearchInterface::myName = "Segment Matching Interface";

const UUID SegmentSearchTool::myID = UUID("");
const char *SegmentSearchTool::myName = "Segment Matching Tool";

const UUID SegmentSearchInterfaceABC::myID = UUID();

//=============================================================================
SegmentSearchInterfaceABC::SegmentSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SegmentSearchInterface::SegmentSearchInterface(SegmentSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SegmentSearchInterface::~SegmentSearchInterface()
{
}

//----------------------------------------------------------------------------
int SegmentSearchInterface::
SetRefDSInterface(GenericDSInterfaceABC
		       *RefGenericDSInterface)

{
  return m_SearchTool->
    SetRefDSInterface(RefGenericDSInterface);
}

//----------------------------------------------------------------------------
int SegmentSearchInterface::
SetQueryDSInterface(GenericDSInterfaceABC
		       *QueryGenericDSInterface)

{
  return m_SearchTool->
    SetQueryDSInterface(QueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double SegmentSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& SegmentSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
SegmentSearchTool::SegmentSearchTool():
m_Interface(this),
m_RefDSInterface(0),
m_QueryDSInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SegmentSearchTool::~SegmentSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& SegmentSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SegmentSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SegmentSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SegmentSearchTool::
SetRefDSInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  m_RefDSInterface = aRefGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int  SegmentSearchTool::
SetQueryDSInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  m_QueryDSInterface=aQueryGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
double SegmentSearchTool::GetDistance(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	
	double distance;

	DOM_Document docRef, docQuery;
	DOM_Element segElemRef, segElemQuery;
	DOMString segTypeRef, segTypeQuery;


	// ------------------------------------------------------------------------
	// Check Descriptor schemes
	// ------------------------------------------------------------------------

    if (!m_RefDSInterface) return 10000000000.0;
    if (!m_QueryDSInterface) return 10000000000.0;

	if (strcmp(m_RefDSInterface->GetName(),
		   "GenericDSInterface") != 0) {
	  return 10000000000.0;
	}
	if (strcmp(m_QueryDSInterface->GetName(),
		   "GenericDSInterface") != 0) {
	  return 10000000000.0;
	}

	// ------------------------------------------------------------------------
	// Perform matching
	// ------------------------------------------------------------------------

	// Read DOM documents for query and database items
	docRef = m_RefDSInterface->GetDOMDocument();
	docQuery = m_QueryDSInterface->GetDOMDocument();

	// Read first element for each Descriptor
	segElemRef = GetFirstChildElement(docRef);
	segElemQuery = GetFirstChildElement(docQuery);

	// Check the elements are segments of the same type
	if (segElemRef == 0 || segElemQuery ==0) {
		fprintf(stderr, "Document lacks child element/n");
		distance = 1000000000000.0;
	} else {
		segTypeRef = segElemRef.getTagName();
		segTypeQuery = segElemQuery.getTagName();

		if (segTypeRef == 0 || segTypeQuery == 0) {
			distance = 1000000000000.0;
		} else if ( segTypeRef.equals(segTypeQuery) ) {
			distance = RecursiveMatching(segElemRef, segElemQuery);
		} else {
			distance = 1000000000000.0;
		}
	}

	return distance;
}

//----------------------------------------------------------------------------
double SegmentSearchTool::RecursiveMatching(DOM_Element segElemRef,
											DOM_Element segElemQuery)
{
	// ------------------------------------------------------------------------
	// Variable declaration
	// ------------------------------------------------------------------------

	double distance, descDistance;

	DOMString segTypeRef, segTypeQuery;
	

	// ------------------------------------------------------------------------
	// Read info from DOM tree for each segment
	// ------------------------------------------------------------------------

	segTypeRef = segElemRef.getTagName();
	segTypeQuery = segElemQuery.getTagName();

	// ------------------------------------------------------------------------
	// Validate elements are segments and of the same type
	// ------------------------------------------------------------------------

	if (segTypeRef == 0 || segTypeQuery == 0 ) {
		distance = 1000000000000.0;
	} else if ( !segTypeRef.equals(segTypeQuery) ) {
		distance = 1000000000000.0;

	// ------------------------------------------------------------------------
	// Decide what to do for each Segment Class
	// ------------------------------------------------------------------------

	} else if (segTypeQuery.equals("VideoSegment")) {

		descDistance = VideoSegmentMatching(segElemRef, segElemQuery);

	} else if (segTypeQuery.equals("MovingRegion")) {
		// Not implemented yet!
		// descDistance = VideoSegmentMatching(segElemRef, segElemQuery);

	} else if (segTypeQuery.equals("StillRegions")) {
		// Not implemented yet!
		// descDistance = VideoSegmentMatching(segElemRef, segElemQuery);

	}

	// ------------------------------------------------------------------------
	// Print the Result
	// ------------------------------------------------------------------------
	fprintf( stderr, "Distance between segments of type %s = %f\n", 
			 segTypeRef.transcode(), descDistance);
	
	// ------------------------------------------------------------------------
	// Call recursive matching for each child
	// ------------------------------------------------------------------------

	// Number of SegmentDecomposition for Ref Segment

	// Based on type of children segment get the distances among pairs and
	// combine them

	// Not implemented yet!!

/*
	if (SegmentNumChildren>0) {

		// Create a SegmentDS Type for Children
		domSegmentDecompositionElement = domDoc.createElement("SegmentDecomposition");

		// Set Decomposition Type
		domSegmentDecompositionElement.setAttribute("DecompositionType", pcDecompType);
		domSegmentDecompositionElement.setAttribute("Overlap", pcOverlap);
		domSegmentDecompositionElement.setAttribute("Gap", pcGap);
		
		// Attach it to Segment Node	
		domSegmentElement.appendChild(domSegmentDecompositionElement);		

		// Do the recursivity
		for (i=0; i<iSegmentNumChildren; i++) {
			RecursiveParse( piSegmentChildIDList[i], domSegmentDecompositionElement);
		}

	}
*/

	// At this moment
	distance = descDistance;

	return distance;
}

//----------------------------------------------------------------------------
double SegmentSearchTool::VideoSegmentMatching(DOM_Element elemRef,
											   DOM_Element elemQuery)
{
	double distance= 0.0;

	DOM_Element GoFColorHistogramDecriptorElemRef, GoFColorHistogramDecriptorElemQuery;

	// Find one GoFColorHistogramDecriptor element for each segment (Ref,Query)
	// Only one GoFColorHistogramDecriptor is processed 

	// (Reference)
	GoFColorHistogramDecriptorElemRef = 
		GetChildByTagName(elemRef, "GoFGoPHistogramD");

	// (Query)
	GoFColorHistogramDecriptorElemQuery = 
		GetChildByTagName(elemQuery, "GoFGoPHistogramD");

	// If found one GoFColorHistogramDecriptor for each segment,
	// time to do matching
	if ( GoFColorHistogramDecriptorElemRef.isNull() || 
		 GoFColorHistogramDecriptorElemQuery.isNull() ) {
		fprintf(stderr, "No GoFGoPHistogramD in Segment\n");
	} else {
		distance = GoFColorHistogramDescriptorMatching(
			GoFColorHistogramDecriptorElemRef,
			GoFColorHistogramDecriptorElemQuery);
	}

	return distance;
}

//----------------------------------------------------------------------------
double SegmentSearchTool::GoFColorHistogramDescriptorMatching(DOM_Element elemRef,
															  DOM_Element elemQuery)
{
	double distance = 0.0;

	// Descriptor and sub-descriptor (Reference)
	ColorSpaceDescriptor refColorSpaceDescriptor;
	ColorSpaceDescriptorInterfaceABC *refColorSpaceDescriptorInterface;

	ColorQuantizerDescriptor  refColorQuantizerDescriptor;
	ColorQuantizerDescriptorInterfaceABC
	  *refColorQuantizerDescriptorInterface;

	HistogramDescriptor refHistogramDescriptor;
	HistogramDescriptorInterfaceABC *refHistogramDescriptorInterface;

	ColorHistogramDescriptor refColorHistogramDescriptor;
	ColorHistogramDescriptorInterfaceABC
	  *refColorHistogramDescriptorInterface;

	GoFColorHistogramDescriptor refGoFColorHistogramDescriptor;
	GoFColorHistogramDescriptorInterfaceABC
	  *refGoFColorHistogramDescriptorInterface;


	// Descriptor and sub-descriptor (Query)
	ColorSpaceDescriptor queryColorSpaceDescriptor;
	ColorSpaceDescriptorInterfaceABC *queryColorSpaceDescriptorInterface;

	ColorQuantizerDescriptor  queryColorQuantizerDescriptor;
	ColorQuantizerDescriptorInterfaceABC
	  *queryColorQuantizerDescriptorInterface;

	HistogramDescriptor queryHistogramDescriptor;
	HistogramDescriptorInterfaceABC *queryHistogramDescriptorInterface;

	ColorHistogramDescriptor queryColorHistogramDescriptor;
	ColorHistogramDescriptorInterfaceABC
	  *queryColorHistogramDescriptorInterface;

	GoFColorHistogramDescriptor queryGoFColorHistogramDescriptor;
	GoFColorHistogramDescriptorInterfaceABC
	  *queryGoFColorHistogramDescriptorInterface;

	// Search tool
	GoFColorHistogramSearchTool theSearchTool;
	GoFColorHistogramSearchInterfaceABC *theSearchInterface;

	// Get descriptor and sub-descriptor interfaces (Reference)
	refColorSpaceDescriptorInterface=
	  refColorSpaceDescriptor.GetInterface();

	refColorQuantizerDescriptorInterface=
	  refColorQuantizerDescriptor.GetInterface();

	refHistogramDescriptorInterface=
	  refHistogramDescriptor.GetInterface();

	refColorHistogramDescriptorInterface=
	  refColorHistogramDescriptor.GetInterface();

	refGoFColorHistogramDescriptorInterface=
	  refGoFColorHistogramDescriptor.GetInterface();

	// Get descriptor and sub-descriptor interfaces (Query)
	queryColorSpaceDescriptorInterface=
	  queryColorSpaceDescriptor.GetInterface();

	queryColorQuantizerDescriptorInterface=
	  queryColorQuantizerDescriptor.GetInterface();

	queryHistogramDescriptorInterface=
	  queryHistogramDescriptor.GetInterface();

	queryColorHistogramDescriptorInterface=
	  queryColorHistogramDescriptor.GetInterface();

	queryGoFColorHistogramDescriptorInterface=
	  queryGoFColorHistogramDescriptor.GetInterface();

	// Get GoF color histogram search interface
	theSearchInterface=theSearchTool.GetInterface();

	// Connect colorspace and quantizer to histogram descriptor (Reference)
	refGoFColorHistogramDescriptorInterface=
	  refGoFColorHistogramDescriptor.GetInterface();

	refGoFColorHistogramDescriptorInterface->
	  SetColorHistogramDescriptorInterface(refColorHistogramDescriptorInterface);
	refColorHistogramDescriptorInterface->
	  SetColorSpaceDescriptorInterface(refColorSpaceDescriptorInterface);
	refColorHistogramDescriptorInterface->
	  SetColorQuantizerDescriptorInterface(refColorQuantizerDescriptorInterface);
	refColorHistogramDescriptorInterface->
	  SetHistogramDescriptorInterface(refHistogramDescriptorInterface);

	// Connect query descriptor to search tool (Reference)
    theSearchInterface->
	  SetRefDescriptorInterface(refGoFColorHistogramDescriptorInterface);

	// Connect colorspace and quantizer to histogram descriptor (Query)
	queryGoFColorHistogramDescriptorInterface=
	  queryGoFColorHistogramDescriptor.GetInterface();

	queryGoFColorHistogramDescriptorInterface->
	  SetColorHistogramDescriptorInterface(queryColorHistogramDescriptorInterface);
	queryColorHistogramDescriptorInterface->
	  SetColorSpaceDescriptorInterface(queryColorSpaceDescriptorInterface);
	queryColorHistogramDescriptorInterface->
	  SetColorQuantizerDescriptorInterface(queryColorQuantizerDescriptorInterface);
	queryColorHistogramDescriptorInterface->
	  SetHistogramDescriptorInterface(queryHistogramDescriptorInterface);

	// Connect query descriptor to search tool (Query)
    theSearchInterface->
	  SetQueryDescriptorInterface(queryGoFColorHistogramDescriptorInterface);

	// Decode DOM (Reference) 
	GoFColorHistogramDescriptorDOMDecoding(elemRef, 
		refGoFColorHistogramDescriptorInterface);

	// Decode DOM (Query)
	GoFColorHistogramDescriptorDOMDecoding(elemQuery, 
		queryGoFColorHistogramDescriptorInterface);

	// Get distance among query and reference descriptors
	distance = theSearchInterface->GetDistance();

	fprintf(stderr, "GoFColorHistogram distance = %f\n", distance);

	return distance;
}

//----------------------------------------------------------------------------
void SegmentSearchTool::GoFColorHistogramDescriptorDOMDecoding(DOM_Element elem, 
GoFColorHistogramDescriptorInterfaceABC *theGoFColorHistogramDescriptorInterface)
{
	// Decode DOM for GoFColorHistogramD

	DOMString HistogramType = elem.getAttribute("HistogramTypeInfo");

	if (HistogramType == 0) {
		fprintf(stderr, "No HistogramTypeInfo attribute in GoFGoPHistogramD\n");
	} else if (HistogramType.equals("Average")) {
		theGoFColorHistogramDescriptorInterface->SetGoFColorHistogramType(AVGH);
	} else if (HistogramType.equals("Median")) {
		theGoFColorHistogramDescriptorInterface->SetGoFColorHistogramType(MEDH);
	} else if (HistogramType.equals("Intersection")) {
		theGoFColorHistogramDescriptorInterface->SetGoFColorHistogramType(INTH);
	} else if (HistogramType.equals("Reserved")) {
		theGoFColorHistogramDescriptorInterface->SetGoFColorHistogramType(INTH);
	} else {
		fprintf(stderr, "Value of HistogramType attribute in GoFGoPHistogramD is not valid: %s\n", 
			HistogramType.transcode());
	}

	DOM_Element ColorHistogramDescriptorNode = GetChildByTagName(elem, "ColorHistogramD");
	if (ColorHistogramDescriptorNode.isNull()) {
		fprintf(stderr, "No ColorHistogramD in GoFGoPHistogramD\n");
	} else {
		ColorHistogramDescriptorDOMDecoding(ColorHistogramDescriptorNode,
			theGoFColorHistogramDescriptorInterface->GetColorHistogramDescriptorInterface());		
	}

	// Check no other elements are included can be done

	return;
}

//----------------------------------------------------------------------------
void SegmentSearchTool::ColorHistogramDescriptorDOMDecoding(DOM_Element elem, 
ColorHistogramDescriptorInterfaceABC *theColorHistogramDescriptorInterface)
{
	// DOM for ColorHistogramD

	DOM_Element ColorSpaceDescriptorNode = GetChildByTagName(elem, "ColorSpaceD");	
	DOM_Element ColorQuantizerDescriptorNode = GetChildByTagName(elem, "LinearColorQuantizationD");
	if ( ColorQuantizerDescriptorNode.isNull() ) {
		ColorQuantizerDescriptorNode = GetChildByTagName(elem, "NonLinearColorQuantizationD");
		if ( ColorQuantizerDescriptorNode.isNull() ) {
			ColorQuantizerDescriptorNode = GetChildByTagName(elem, "LUTColorQuantizationD");
		}
	}
	DOM_Element HistogramDescriptorNode = GetChildByTagName(elem, "HistogramD");

	if ( !ColorSpaceDescriptorNode.isNull() && !ColorQuantizerDescriptorNode.isNull() 
		&& !HistogramDescriptorNode.isNull() ) {
		ColorSpaceDescriptorDOMDecoding(ColorSpaceDescriptorNode,
			theColorHistogramDescriptorInterface->GetColorSpaceDescriptorInterface());
		ColorQuantizerDescriptorDOMDecoding(ColorQuantizerDescriptorNode,
			theColorHistogramDescriptorInterface->GetColorQuantizerDescriptorInterface());
		HistogramDescriptorDOMDecoding(HistogramDescriptorNode,
			theColorHistogramDescriptorInterface->GetHistogramDescriptorInterface());
	} else {
		fprintf(stderr, "No ColoSpaceD, ColorQuantizationD, or HistogramD in ColorHistogramD\n");
	}

	// Check no other elements are included can be done

	return;
}

//----------------------------------------------------------------------------
void SegmentSearchTool::ColorSpaceDescriptorDOMDecoding(DOM_Element elem, 
ColorSpaceDescriptorInterfaceABC *theColorSpaceDescriptorInterface)
{
	// DOM for ColorSpaceD

	DOMString Space = elem.getAttribute("Space");

	if (Space == 0) {
		fprintf(stderr, "No Space attribute in ColorSpaceD\n");
	} else if (Space.equals("RGB")) {
		theColorSpaceDescriptorInterface->SetDescriptionColorSpace(RGB);
	} else if (Space.equals("YCrCb")) {
		theColorSpaceDescriptorInterface->SetDescriptionColorSpace(YCRCB);
	} else if (Space.equals("HSV")) {
		theColorSpaceDescriptorInterface->SetDescriptionColorSpace(HSV);
	} else if (Space.equals("HMMD")) {
		theColorSpaceDescriptorInterface->SetDescriptionColorSpace(HMMD);
	} else if (Space.equals("LinearMatrix")) {
		theColorSpaceDescriptorInterface->
			SetDescriptionColorSpace(Linear_Matrix);

		DOM_Element LinearMatrixNode = GetChildByTagName(elem, "LinearMatrix");		
		if ( LinearMatrixNode.isNull() ) {
			fprintf(stderr, "No LinearMatrix in ColorSpaceD\n");
		} else {
			DOM_Element ColorTransMatValueNode = GetFirstChildElement(elem);
			int yindex, xindex;

			for (yindex=0; yindex<3; yindex++) {
				for (xindex=0; xindex<3; xindex++) {
					if ( ColorTransMatValueNode.isNull() ) {
						fprintf(stderr, "Null ColorTransMatValue in LinearMatrix\n");
					} else {
						theColorSpaceDescriptorInterface->SetMatrixElement(xindex,yindex, 
							ReadIntegerElement(ColorTransMatValueNode, "ColorTransMatValue"));
					}
					ColorTransMatValueNode = GetNextSiblingElement(ColorTransMatValueNode);
				}
			}			
		}
	} else {
		fprintf(stderr, "Value of Space attribute in ColorSpaceD is not valid: %s", Space);
	}

	return;
}

//----------------------------------------------------------------------------
void SegmentSearchTool::ColorQuantizerDescriptorDOMDecoding(DOM_Element elem, 
ColorQuantizerDescriptorInterfaceABC *theColorQuantizerDescriptorInterface)
{

	// DOM for ColorQuantizationD

	int index, k, ki, l, li, m, mi;

	DOMString Quantization = elem.getAttribute("Quantization");

	if (Quantization == 0) {
		fprintf(stderr, "No Quantization attribute in ColorQuantizationD\n");

	} else if (Quantization.equals("linear")) {
		// Possible checking that tag name of element is LinearColorQuantization
		theColorQuantizerDescriptorInterface->SetColorQuantizerType(Linear);

		DOM_Element bin_numberNode = GetFirstChildElement(elem);
		int bins[3] = {0, 0, 0};

		for (index=0; index<3; index++) {
			if (bin_numberNode.isNull()) {
				fprintf(stderr, "Null bin_number in ColorQuantizationD\n");
			} else {
				bins[index] = ReadIntegerElement(bin_numberNode, "bin_number");
			}
			bin_numberNode = GetNextSiblingElement(bin_numberNode);
		}
		theColorQuantizerDescriptorInterface->SetNoOfBins( bins[0], bins[1], bins[2]);

	} else if (Quantization.equals("non_linear")) {
		// Possible checking that tag name of element is NonLinearColorQuantization
		theColorQuantizerDescriptorInterface->SetColorQuantizerType(NonLinear);

		DOM_Element childNode = GetFirstChildElement(elem);
		if (childNode.isNull()) {
			fprintf(stderr, "Null bin_number_1 in ColorQuantizationD\n");
			return;
		} else {
			k = ReadIntegerElement(childNode, "bin_number_1");
			theColorQuantizerDescriptorInterface->SetNonLinBins1(k);
		}

		for (ki=0; ki<k; ki++) {
			childNode = GetNextSiblingElement(childNode);
			if (childNode.isNull()) {
				fprintf(stderr, "Null color_value_1 in ColorQuantizationD\n");
				return;
			} else {
				theColorQuantizerDescriptorInterface->SetNonLinColorValue1(ki, 
					ReadIntegerElement(childNode, "color_value_1"));
			}

			childNode = GetNextSiblingElement(childNode);
			if (childNode.isNull()) {
				fprintf(stderr, "Null bin_number_2 in ColorQuantizationD\n");
				return;
			} else {
				l = ReadIntegerElement(childNode, "bin_number_2");
				theColorQuantizerDescriptorInterface->SetNonLinBins2(ki, l);
			}

			for(li=0; li<l; li++) {
				childNode = GetNextSiblingElement(childNode);
				if (childNode.isNull()) {
					fprintf(stderr, "Null color_value_2 in ColorQuantizationD\n");
					return;
				} else {
					theColorQuantizerDescriptorInterface->SetNonLinColorValue2(ki, li,
						ReadIntegerElement(childNode, "color_value_2"));
				}

				childNode = GetNextSiblingElement(childNode);
				if (childNode.isNull()) {
					fprintf(stderr, "Null bin_number_3 in ColorQuantizationD\n");
					return;
				} else {
					m = ReadIntegerElement(childNode, "bin_number_3");
					theColorQuantizerDescriptorInterface->SetNonLinBins3(ki, li, m);
				}

				for(mi=0; mi<m; mi++) {
					childNode = GetNextSiblingElement(childNode);
					if (childNode.isNull()) {
						fprintf(stderr, "Null color_value_3 in ColorQuantizationD\n");
						return;
					} else {
						theColorQuantizerDescriptorInterface->SetNonLinColorValue3(ki, li, mi,
							ReadIntegerElement(childNode, "color_value_3"));
					}
				}
			}
		}

	} else if (Quantization.equals("lookup_table")) {
		// Possible checking that tag name of element is LUTColorQuantization
		theColorQuantizerDescriptorInterface->SetColorQuantizerType(LookupTable);

		DOMString bin_number = elem.getAttribute("bin_number");
		if (bin_number == 0) {
			fprintf(stderr, "No bin_number attribute in ColorQuantizationD\n");			
		} else {
			k = atoi(bin_number.transcode());
			theColorQuantizerDescriptorInterface->SetLookupBins(k);
		}

		DOM_Element color_valueNode = GetFirstChildElement(elem);
		for( ki=0; ki<k; ki++) {
			int val[3] = {0, 0, 0};
			for (index=0; index<3; index++) {
				if (color_valueNode.isNull()) {
					fprintf(stderr, "Null color_value in ColorQuantizationD\n");
				} else {
					val[index] = ReadIntegerElement(color_valueNode, "color_value");
				}
				color_valueNode = GetNextSiblingElement(color_valueNode);
			}
			theColorQuantizerDescriptorInterface->SetLookupValues(ki, val[0], val[1], val[3]);
		}

	} else {
		fprintf(stderr, "Value of Quantization attribute in ColorQuantizationD is not valid: %s", 
			Quantization);
	}

	return;
}

//----------------------------------------------------------------------------
void SegmentSearchTool::HistogramDescriptorDOMDecoding(DOM_Element elem, 
HistogramDescriptorInterfaceABC *theHistogramDescriptorInterface)
{
	// DOM for HistogramD

	DOMString NumberHistogramBins = elem.getAttribute("NumberHistogramBins");

	if (NumberHistogramBins == 0) {
		fprintf(stderr, "No NumberHistogramBins attribute in HistogramD\n");
	} else {
		int size = atoi(NumberHistogramBins.transcode());
		theHistogramDescriptorInterface->SetHistogramSize(size);

		DOM_Element HistogramValueNode = GetFirstChildElement(elem);
		for (int i=0; i<size; i++) {
			if ( HistogramValueNode.isNull() ) {
				fprintf(stderr, "Null HistogramValue in HistogramD\n");
			} else {
				theHistogramDescriptorInterface->SetElement(i, 
					ReadIntegerElement(HistogramValueNode, "HistogramValue"));
			}
			HistogramValueNode = GetNextSiblingElement(HistogramValueNode);
		}
	}
	
	return;
}

//----------------------------------------------------------------------------
DOM_Element SegmentSearchTool::GetChildByTagName(DOM_Element elem, DOMString name) {

	DOM_Element child = GetFirstChildElement(elem);

 	while ( child != 0 ) {
		DOMString childName = child.getNodeName();
		if (childName.equals(name)) {
			return child;
		}
		child = GetNextSiblingElement(child);
	}

	return child;
}

//----------------------------------------------------------------------------
DOM_Element SegmentSearchTool::GetFirstChildElement(DOM_Node node) 
{
	DOM_Node child = node.getFirstChild();

	while (child != 0) {
		if (child.getNodeType() == DOM_Node::ELEMENT_NODE) {
			return (DOM_Element &) child;
		}
		child = child.getNextSibling();
	}

	return (DOM_Element &) child;
}

//----------------------------------------------------------------------------
DOM_Element SegmentSearchTool::GetNextSiblingElement(DOM_Node node) 
{
	DOM_Node child = node.getNextSibling();

	while (child != 0) {
		if (child.getNodeType() == DOM_Node::ELEMENT_NODE) {
			return (DOM_Element &) child;
		}
		child = child.getNextSibling();
	}

	return (DOM_Element &) child;
}

//----------------------------------------------------------------------------
int SegmentSearchTool::ReadIntegerElement(DOM_Element elem, DOMString elemName) 
{
	int value = 0;
	DOMString name = elem.getTagName();

	if ( !name.equals(elemName) ) {
		fprintf(stderr, "ReadIntegerElement: Name does not match element name\n");
	} else {
		// This may need to be changed in the future to avoid white spaces
		DOM_NodeList childList = elem.getChildNodes();
		if (childList.getLength() != 1) {
			fprintf(stderr, "ReadIntegerElement: Element has more than one node\n");
		} else {
			char *str = ((DOM_Text &)childList.item(0)).getData().transcode();
			value = atoi(str);
		}
	}
	
	return value;
}

//----------------------------------------------------------------------------
SegmentSearchInterfaceABC *SegmentSearchTool::GetInterface(void)
{ return &m_Interface; }
