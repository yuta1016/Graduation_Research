///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez		- Columbia University.
// Javier Ruiz Hidalgo	- Universidad Politecnica de Catalunya.
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GenericDSCS.cpp
//
#include "CodingSchemes/CodingSchemes.h"

#include <iostream>
#include <fstream>

using namespace XM;
using namespace std;

const UUID GenericDSCSInterface::myID = UUID("");
const char *GenericDSCSInterface::myName =
                         "Color Space Coding Engine Interface";

const UUID GenericDSCS::myID = UUID("");
const char *GenericDSCS::myName = "Color Space Coding Scheme";

const UUID GenericDSCS::valID = UUID("");
const char *GenericDSCS::valName = "";

const UUID GenericDSCSInterfaceABC::myID = UUID("");

GenericDSCSInterfaceABC::GenericDSCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
GenericDSCSInterface::GenericDSCSInterface(GenericDSCS *aColorSpace):
m_GenericDSCS(aColorSpace)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GenericDSCSInterface::~GenericDSCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& GenericDSCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GenericDSCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void GenericDSCSInterface::destroy(void)
{ delete m_GenericDSCS; }

//----------------------------------------------------------------------------
int GenericDSCSInterface::SetEncoderFileName(char *filename)
{
        return m_GenericDSCS->SetEncoderFileName(filename);
}

//----------------------------------------------------------------------------
int GenericDSCSInterface::SetDecoderFileName(char *filename)
{
        return m_GenericDSCS->SetDecoderFileName(filename);
}

//----------------------------------------------------------------------------
// ostream& GenericDSCSInterface::GetXMLStreamO(void)
// {
//         return m_GenericDSCS->GetXMLStreamO();
// }

//----------------------------------------------------------------------------
// int GenericDSCSInterface::SetXMLStreamO(ostream& stream)
// {
//         return m_GenericDSCS->SetXMLStreamO(stream);
// }

//----------------------------------------------------------------------------
// istream& GenericDSCSInterface::GetXMLStreamI(void)
// {
//         return m_GenericDSCS->GetXMLStreamI();
// }

//----------------------------------------------------------------------------
// int GenericDSCSInterface::SetXMLStreamI(istream& stream)
// {
//         return m_GenericDSCS->SetXMLStreamI(stream);
// }

//----------------------------------------------------------------------------
// char* GenericDSCSInterface::GetXMLFileI(void)
// {
//         return m_GenericDSCS->GetXMLFileI();
// }

//----------------------------------------------------------------------------
// int GenericDSCSInterface::SetXMLFileI(char *file)
// {
//         return m_GenericDSCS->SetXMLFileI(file);
// }



//----------------------------------------------------------------------------
GenericDSInterfaceABC *GenericDSCSInterface::GetDescriptionInterface(void)
{
        return m_GenericDSCS->GetDescriptionInterface();
}

//----------------------------------------------------------------------------
int GenericDSCSInterface::
SetDescriptionInterface(GenericDSInterfaceABC
		       *aGenericDSInterface)
{
        return m_GenericDSCS->
	  SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int GenericDSCSInterface::StartEncode(void)
{
        return m_GenericDSCS->StartEncode();
}

//----------------------------------------------------------------------------
int GenericDSCSInterface::StartDecode(void)
{
        return m_GenericDSCS->StartDecode();
}

//=============================================================================

GenericDSCS::GenericDSCS():
m_Interface(this),
m_isProprietary(false),
m_DescriptionInterface(0),
m_XMLFileI(0),
m_XMLStreamO(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GenericDSCS::GenericDSCS(GenericDSInterfaceABC *theDescription):
m_Interface(this),
m_isProprietary(false),
m_DescriptionInterface(0),
m_XMLFileI(0),
m_XMLStreamO(0)

{
        SetDescriptionInterface(theDescription);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GenericDSCS::~GenericDSCS()
{
  if (m_XMLStreamO) {
/*    m_XMLStreamO->close();*/
    delete m_XMLStreamO;
  }
}

//----------------------------------------------------------------------------
const UUID& GenericDSCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GenericDSCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GenericDSCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GenericDSCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GenericDSCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
int GenericDSCS::SetEncoderFileName(char *filename)
{
	if (!filename) return -1;

	if (m_XMLStreamO) delete m_XMLStreamO;
	m_XMLStreamO = new ofstream(filename);
/*	m_XMLStreamO->open(filename);*/
	return 0;
}

//----------------------------------------------------------------------------
int GenericDSCS::SetDecoderFileName(char *filename)
{
	if (!filename) return -1;

	m_XMLFileI = filename;
	return 0;
}

//----------------------------------------------------------------------------
// ostream& GenericDSCS::GetXMLStreamO(void)
// {
// 	return *m_XMLStreamO;
// }

//---------------------------------------------------------------------------
// int GenericDSCS::SetXMLStreamO(ostream& stream)
// {
// 	if (!stream) return -1;

// 	m_XMLStreamO = &stream;
// 	return 0;
// }

//----------------------------------------------------------------------------
// istream& GenericDSCS::GetXMLStreamI(void)
// {
// 	return *m_XMLStreamI;
// }

//----------------------------------------------------------------------------
// int GenericDSCS::SetXMLStreamI(istream& stream)
// {
// 	if (!stream) return -1;

// 	m_XMLStreamI = &stream;
// 	return 0;
// }

//----------------------------------------------------------------------------
// char* GenericDSCS::GetXMLFileI(void)
// {
// 	return m_XMLFileI;
// }

//----------------------------------------------------------------------------
// int GenericDSCS::SetXMLFileI(char *file)
// {
// 	if (!file) return -1;

// 	m_XMLFileI = file;
// 	return 0;
// }


//----------------------------------------------------------------------------
GenericDSInterfaceABC *GenericDSCS::GetDescriptionInterface(void)
{
        return m_DescriptionInterface;
}

//----------------------------------------------------------------------------
int GenericDSCS::
SetDescriptionInterface(GenericDSInterfaceABC
	      *aGenericDSInterface)
{
        if (!aGenericDSInterface) return -1;

        m_DescriptionInterface = aGenericDSInterface;
	return 0;
}


//----------------------------------------------------------------------------
int GenericDSCS::StartEncode(void)
{

#if defined(USEXML3)
	DOMNode *n = m_DescriptionInterface->GetDocument();
	PrintDOMNode(*m_XMLStreamO, *n);
#elif defined(USEXML)
	DOM_Node n = m_DescriptionInterface->GetDocument();
	PrintDOMNode(*m_XMLStreamO, n);
#endif // USEXML3
	return 0;
}

//----------------------------------------------------------------------------
#if defined(USEXML3)
int GenericDSCS::PrintDOMNode(ostream& out, const DOMNode& toWrite) 
{
    // Get the name and value out for convenience
	const XMLCh   *nodeName = toWrite.getNodeName();
    const XMLCh   *nodeValue = toWrite.getNodeValue();

	switch (toWrite.getNodeType())
    {
		case DOMNode::TEXT_NODE:
        {
            outputContent(out, nodeValue, true);
            break;
        }

        case DOMNode::PROCESSING_INSTRUCTION_NODE :
        {
            out  << "<?";
			PrintDOMNode(out, nodeName);
			out  << ' ';
			PrintDOMNode(out, nodeValue);
            out  << "?>";
            break;
        }

        case DOMNode::DOCUMENT_NODE :
        {
            // Bug here:  we need to find a way to get the encoding name
            //   for the default code page on the system where the
            //   program is running, and plug that in for the encoding
            //   name.  
            out << "<?xml version='1.0' encoding='ISO-8859-1' ?>\n";
            const DOMNode *child = toWrite.getFirstChild();
            while( child != 0)
            {
				PrintDOMNode(out, *child);
                child = child->getNextSibling();
            }

            break;
        }

        case DOMNode::ELEMENT_NODE :
        {
            // Output the element start tag.
            out << '<';
			PrintDOMNode(out, nodeName);

            // Output any attributes on this element
			DOMNamedNodeMap *attributes = toWrite.getAttributes();
            int attrCount = attributes->getLength();
            for (int i = 0; i < attrCount; i++)
            {
                DOMNode  *attribute = attributes->item(i);

                out << ' ';
				PrintDOMNode(out, attribute->getNodeName());
				out  << " = \"";
                //  Note that "<" must be escaped in attribute values.
				outputContent(out, attribute->getNodeValue(), true);
                out << '"';
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            DOMNode *child = toWrite.getFirstChild();
            if (child != 0)
            {
                // There are children. Close start-tag, and output children.
                out << ">";
                while( child != 0)
                {
					PrintDOMNode(out, *child);
                    child = child->getNextSibling();
                }

                // Done with children.  Output the end tag.
                out << "</";
				PrintDOMNode(out, nodeName);
				out << ">" << endl;
            }
            else
            {
                //
                //  There were no children.  Output the short form close of the
                //  element start tag, making it an empty-element tag.
                //
                out << "/>" << endl;
            }
            break;
        }

        case DOMNode::ENTITY_REFERENCE_NODE:
        {
            DOMNode *child;
            for (child = toWrite.getFirstChild(); child != 0; child = child->getNextSibling())
				PrintDOMNode(out, *child);
            break;
        }

        case DOMNode::CDATA_SECTION_NODE:
        {
            out << "<![CDATA[";
			PrintDOMNode(out, nodeValue);
			out << "]]>";
            break;
        }

        case DOMNode::COMMENT_NODE:
        {
            out << "<!--";
			PrintDOMNode(out, nodeValue);
			out << "-->";
            break;
        }

        default:
            cerr << "Unrecognized node type = "
                 << (long)toWrite.getNodeType() << endl;
    }

	return 0;
}

// ---------------------------------------------------------------------------
//
//  outputContent  - Write document content from a DOMString to a C++ ostream.
//                   Escape the XML special characters (<, &, etc.) unless this
//                   is suppressed by the command line option.
//
// ---------------------------------------------------------------------------
void GenericDSCS::outputContent(ostream& out, const XMLCh* toWrite, bool doEscapes)
{
	int            length = XMLString::stringLen(toWrite);
	const XMLCh*   chars  = toWrite;
        
        int index;
        for (index = 0; index < length; index++)
        {
            switch (chars[index])
            {
            case chAmpersand :
                out << "&amp;";
                break;
                
            case chOpenAngle :
                out << "&lt;";
                break;
                
            case chCloseAngle:
                out << "&gt;";
                break;
                
            case chDoubleQuote :
                out << "&quot;";
                break;
                
            default:
                // If it is none of the special characters, print it as such
				XMLCh xmlch[2];
				xmlch[0] = chars[index];
				xmlch[1] = 0;
				PrintDOMNode(out, xmlch);
                break;
            }
        }

    return;
}


// ---------------------------------------------------------------------------
//
//  Print DOM Node          Stream out a DOM string.
//                          Doing this requires that we first transcode
//                          to char * form in the default code page
//                          for the system
//
// ---------------------------------------------------------------------------
void GenericDSCS::PrintDOMNode(ostream& out, const XMLCh* s)
{
	if(!s) return;
	char *p = XMLString::transcode(s);
    out << p;
	XMLString::release(&p);

	return;
}
#elif defined (USEXML)
int GenericDSCS::PrintDOMNode(ostream& out, DOM_Node& toWrite) 
{
    // Get the name and value out for convenience
    DOMString   nodeName = toWrite.getNodeName();
    DOMString   nodeValue = toWrite.getNodeValue();

	switch (toWrite.getNodeType())
    {
		case DOM_Node::TEXT_NODE:
        {
            outputContent(out, nodeValue, true);
            break;
        }

        case DOM_Node::PROCESSING_INSTRUCTION_NODE :
        {
            out  << "<?";
			PrintDOMNode(out, nodeName);
			out  << ' ';
			PrintDOMNode(out, nodeValue);
            out  << "?>";
            break;
        }

        case DOM_Node::DOCUMENT_NODE :
        {
            // Bug here:  we need to find a way to get the encoding name
            //   for the default code page on the system where the
            //   program is running, and plug that in for the encoding
            //   name.  
            out << "<?xml version='1.0' encoding='ISO-8859-1' ?>\n";
            DOM_Node child = toWrite.getFirstChild();
            while( child != 0)
            {
				PrintDOMNode(out, child);
                child = child.getNextSibling();
            }

            break;
        }

        case DOM_Node::ELEMENT_NODE :
        {
            // Output the element start tag.
            out << '<';
			PrintDOMNode(out, nodeName);

            // Output any attributes on this element
            DOM_NamedNodeMap attributes = toWrite.getAttributes();
            int attrCount = attributes.getLength();
            for (int i = 0; i < attrCount; i++)
            {
                DOM_Node  attribute = attributes.item(i);

                out << ' ';
				PrintDOMNode(out, attribute.getNodeName());
				out  << " = \"";
                //  Note that "<" must be escaped in attribute values.
				outputContent(out, attribute.getNodeValue(), true);
                out << '"';
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            DOM_Node child = toWrite.getFirstChild();
            if (child != 0)
            {
                // There are children. Close start-tag, and output children.
                out << ">";
                while( child != 0)
                {
					PrintDOMNode(out, child);
                    child = child.getNextSibling();
                }

                // Done with children.  Output the end tag.
                out << "</";
				PrintDOMNode(out, nodeName);
				out << ">" << endl;
            }
            else
            {
                //
                //  There were no children.  Output the short form close of the
                //  element start tag, making it an empty-element tag.
                //
                out << "/>" << endl;
            }
            break;
        }

        case DOM_Node::ENTITY_REFERENCE_NODE:
        {
            DOM_Node child;
            for (child = toWrite.getFirstChild(); child != 0; child = child.getNextSibling())
				PrintDOMNode(out, child);
            break;
        }

        case DOM_Node::CDATA_SECTION_NODE:
        {
            out << "<![CDATA[";
			PrintDOMNode(out, nodeValue);
			out << "]]>";
            break;
        }

        case DOM_Node::COMMENT_NODE:
        {
            out << "<!--";
			PrintDOMNode(out, nodeValue);
			out << "-->";
            break;
        }

        default:
            cerr << "Unrecognized node type = "
                 << (long)toWrite.getNodeType() << endl;
    }

	return 0;
}

// ---------------------------------------------------------------------------
//
//  outputContent  - Write document content from a DOMString to a C++ ostream.
//                   Escape the XML special characters (<, &, etc.) unless this
//                   is suppressed by the command line option.
//
// ---------------------------------------------------------------------------
void GenericDSCS::outputContent(ostream& out, const DOMString &toWrite, bool doEscapes)
{
    
    if (doEscapes == false)
    {
		PrintDOMNode(out, toWrite);
    }
     else
    {
        int            length = toWrite.length();
        const XMLCh*   chars  = toWrite.rawBuffer();
        
        int index;
        for (index = 0; index < length; index++)
        {
            switch (chars[index])
            {
            case chAmpersand :
                out << "&amp;";
                break;
                
            case chOpenAngle :
                out << "&lt;";
                break;
                
            case chCloseAngle:
                out << "&gt;";
                break;
                
            case chDoubleQuote :
                out << "&quot;";
                break;
                
            default:
                // If it is none of the special characters, print it as such
                // out << StrX(&chars[index], 1);
				PrintDOMNode(out, toWrite.substringData(index, 1));
                break;
            }
        }
    }

    return;
}


// ---------------------------------------------------------------------------
//
//  Print DOM Node          Stream out a DOM string.
//                          Doing this requires that we first transcode
//                          to char * form in the default code page
//                          for the system
//
// ---------------------------------------------------------------------------
void GenericDSCS::PrintDOMNode(ostream& out, const DOMString& s)
{
    //if (s.length() > 0)
    //    out << StrX(s.rawBuffer(), s.length());
    char *p = s.transcode();
    out << p;
//    delete p;

	return;
}
#endif // USEXML3

//----------------------------------------------------------------------------
int GenericDSCS::StartDecode(void)
{
#if defined(USEXML) || defined(USEXML3)
  // We don't really know if this can be done more than one time but
  // we're possitive it HAS to be done at least one.
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    fprintf(stderr,"ERROR(GenericDS): Error during Xerces-c Initialization: ");
    fprintf(stderr,(char *)toCatch.getMessage());
    exit(-1);
  }

#if defined(USEXML3)
  // Some decoding or reading stuff
  XercesDOMParser parser;

  //  Parse the XML file
  try {
    parser.parse(m_XMLFileI);
  }
  catch (const XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
	 << (char*)(e.getMessage()) << endl;
    exit(-1);
  }

  DOMDocument *doc = parser.adoptDocument();

//  m_DescriptionInterface->CreateDescriptionRoot(doc);
  m_DescriptionInterface->SetDOMNode(doc);
#elif defined(USEXML)
  // Some decoding or reading stuff
  DOMParser parser;

  //  Parse the XML file
  try {
    parser.parse(m_XMLFileI);
  }
  catch (const XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
	 << (char*)(e.getMessage()) << endl;
    exit(-1);
  }

  DOM_Document doc = parser.getDocument();

//  m_DescriptionInterface->CreateDescriptionRoot(doc);
  m_DescriptionInterface->SetDOMNode(doc);
#endif // USEXML3
#endif // USEXML || USEXML3
  return 0;
}

//----------------------------------------------------------------------------
GenericDSCSInterfaceABC *GenericDSCS::GetInterface(void)
{ return &m_Interface; }



