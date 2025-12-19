#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLUniDefs.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>

#include <dom/DOM_DOMException.hpp>

#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "DOMPrint.hpp"
#include "treeFunction.hpp"

extern "C" {
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include "mex.h"
}

#define DESC_LENGTH_MAX 50 // 50 Char for descriptor Name 

int setParamValue(char *descriptorType, char *descriptorName, double descriptorValue);
void callPrint(void);
void callPrintFile(char* outputFileName);
void removeOutputDoc(void);

static ostream& operator<< (ostream& target, const DOMString& s);


DOMParser::ValSchemes    gValScheme             = DOMParser::Val_Auto;
char*                    gXmlFile               = 0;
bool                     gDoNamespaces          = false;
bool                     gDoCreate              = false;
//XMLCh*                   gEncodingName          = 0;
XMLFormatter::UnRepFlags gUnRepFlags            = XMLFormatter::UnRep_CharRef;

// DOM_Document generated
DOM_Document outputDoc;
DOM_Document typeDefinitionDoc;
DOM_Document originalDoc;
DOM_Document mappingDoc;
DOM_DOMImplementation impl;
static bool status = 0;



void removeOutputDoc(void)
{
    DOM_Node rootNode = outputDoc.getDocumentElement();
    
    if (rootNode.hasChildNodes())
    {
	DOM_NodeList listOfChild = rootNode.getChildNodes();
	int numberOfChild = listOfChild.getLength();
	while((numberOfChild > 0) && (rootNode.hasChildNodes()))
	{
	    DOM_Node tmpNode = listOfChild.item(0);
	    rootNode.removeChild(tmpNode);
	    listOfChild = rootNode.getChildNodes();
	    numberOfChild = listOfChild.getLength();
	}
    }

}

// Main function: should be cut in several functions
int createAllTree(char *fileName)
{
   // Initialize the XML4C2 system
    try
    {
	XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
	cerr << "Error during Xerces-c Initialization. \n"
	     << "  Exception Message:"
	     << DOMString(toCatch.getMessage()) << endl;
	return 1;
    }

    gXmlFile = (char *)malloc(PATH_MAX*sizeof(char));
    gXmlFile = strncpy(gXmlFile, fileName, PATH_MAX);
    DOMParser *parser = new DOMParser;

    parser->setValidationScheme(gValScheme);
    parser->setDoNamespaces(gDoNamespaces);
    ErrorHandler *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);
    parser->setCreateEntityReferenceNodes(gDoCreate);
    parser->setToCreateXMLDeclTypeNode(true);


    // Parse the XML file, catching any XML exceptions that might propogate
    // out of it
    bool errorsOccured = false;

    try
    {       
	parser->parse(gXmlFile);
    }
    catch (const XMLException& e)
    {
	cerr << "An error occured during parsing\n"
	     << "   Message: "
	     << DOMString(e.getMessage()) << endl;
	errorsOccured = true;
    }

    catch (const DOM_DOMException& e)
    {
       cerr << "An error occured during parsing\n   Message: "
             << DOMString(e.msg) << endl;
        errorsOccured = true;
    }

    catch (...)
    {
        cerr << "An error occured during parsing\n " << endl;
        errorsOccured = true;
    }

    if (!errorsOccured)
    {
	//DOM_Document 
	if (status == 0)
	{
	    originalDoc = parser->getDocument();

	    mappingDoc = impl.createDocument(
		0,                      // root element namespace URI.
		"DummyContainer",              // root element name
		DOM_DocumentType());    // document type object (DTD)
	    
	    DOM_Node aNode = originalDoc.getFirstChild();
	    DOM_XMLDecl tmpDecl;
	    DOM_XMLDecl rootDecl;
	    if (aNode.getNodeType() == DOM_Node::XML_DECL_NODE)
	    {
		tmpDecl = (DOM_XMLDecl &)aNode;
		if (!tmpDecl.isNull())
		{
		    const DOMString version = tmpDecl.getVersion();
		    const DOMString encoding = tmpDecl.getEncoding();
		    const DOMString standalone = tmpDecl.getStandalone();
		    rootDecl = mappingDoc.createXMLDecl(version, encoding, standalone);		
		}
	    }
	    
#ifdef TIMBRE
	    DOMString tagToSearch("element");
#else
	    DOMString tagToSearch("xsd:element");
#endif
	    
	    generateFirstMappingTree(originalDoc, mappingDoc, rootDecl, tagToSearch);
	    
	    // This DOM tree is the type definition DOM tree (before set value in Final DOM tree)
	    //DOM_Document 
	    typeDefinitionDoc = impl.createDocument(
		0,                      // root element namespace URI.
		"DummyContainer",              // root element name
		DOM_DocumentType());    // document type object (DTD)
	    
	    aNode = originalDoc.getFirstChild();
	    if (aNode.getNodeType() == DOM_Node::XML_DECL_NODE)
	    {
		tmpDecl = (DOM_XMLDecl &)aNode;
	    }
	    if (!tmpDecl.isNull())
	    {
		const DOMString version = tmpDecl.getVersion();
		const DOMString encoding = tmpDecl.getEncoding();
		const DOMString standalone = tmpDecl.getStandalone();
		rootDecl = typeDefinitionDoc.createXMLDecl(version, encoding, standalone);
	    }
	    
	    // Name of element to be expanded
#ifdef TIMBRE
	    tagToSearch = DOMString::transcode("complexType");
#else
	    tagToSearch = DOMString::transcode("xsd:complexType");
#endif
	    generateSecondMappingTree(originalDoc, typeDefinitionDoc, rootDecl, tagToSearch);
	    
	    
	    // NOW LET'S ROCK WITH typeDefinitionDoc
	    DOM_Element typeDefinitionRootElement = typeDefinitionDoc.getDocumentElement();	
	    // Looking for element which have a type attributes
	    // Each of these elements will be add in typedElementList.	
	    checkTypedElement(typeDefinitionRootElement, typeDefinitionDoc, originalDoc);
	    
	    
	    DOM_Node fooNode;	
	    expandExtensionElement(fooNode, originalDoc, typeDefinitionDoc);

	    // Create the outputDocument
	    // DOM_Document 
	    outputDoc = impl.createDocument(
		0,                      // root element namespace URI.
		"DummyContainer",              // root element name
		DOM_DocumentType());    // document type object (DTD)
	    
	    const DOMString version = tmpDecl.getVersion();
	    const DOMString encoding = tmpDecl.getEncoding();
	    const DOMString standalone = tmpDecl.getStandalone();
	    rootDecl = outputDoc.createXMLDecl(version, encoding, standalone);
	    
	    
	}
	//delete gEncodingName;
	delete errReporter;
	delete parser;

	status = 1;
    }
	//XMLPlatformUtils::Terminate();
    return 0;
}

// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------
ostream& operator<< (ostream& target, const DOMString& s)
{
    char *p = s.transcode();
    target << p;
    delete [] p;
    return target;
}




extern "C" {
    void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
    {
	char* fileName;
	char* descriptorType;
	char* descriptorName;	
	char* option;
	double descriptorValue;
	/* double descriptorConfidence; */
	/* int i; */
	int result;

	if (nrhs < 1)
	{
	    mexErrMsgTxt("progBodyMatlab requires at least one argument.");
	    return;
	}

	switch (nrhs)
	{
	case 1:
	    /* Input argument is fileName */
	    fileName = (char *)malloc(PATH_MAX * sizeof(char));
	    mxGetString (prhs[0], fileName, PATH_MAX);
	    
	    if (strcmp (fileName, "print") != 0)
	    {
		createAllTree(fileName);
	    }
	    else
	    {
		callPrint();
	    }
	    free(fileName);
	    removeOutputDoc();
	    break;
	    
	case 2:	    
	    /* To save xml output in a file */
	    option = (char *)malloc(20 * sizeof(char));
	    mxGetString(prhs[0], option, 20);
	    if (strcmp (option, "save") != 0)
	    {
		mexErrMsgTxt("Need a file name to save output");
		return;
	    }
	    /* Get Output File Name */
	    fileName = (char *)malloc(PATH_MAX * sizeof(char));
	    mxGetString(prhs[1], fileName, PATH_MAX);

	    callPrintFile(fileName);
	    free(fileName);
	    free(option);
	    break;

	case 3:
	    /* Descriptor Type (string), Name (string) , value (double) */
	    /* Get description Type */
	    descriptorType = (char *)malloc(DESC_LENGTH_MAX * sizeof(char));
	    mxGetString(prhs[0], descriptorType, DESC_LENGTH_MAX);

	    /* Get descriptor Name */
	    descriptorName = (char *)malloc(DESC_LENGTH_MAX * sizeof(char));	    
	    mxGetString(prhs[1], descriptorName, DESC_LENGTH_MAX);

	    /* Get Value */
	    descriptorValue = mxGetScalar(prhs[2]);

	    /* Call fonction to set descriptor and value */
	    result = setParamValue(descriptorType, descriptorName, descriptorValue);
	    /*  printf("Set descriptor : %s with value : %g \n",descriptorName, descriptorValue); */
	    free(descriptorType);
	    free(descriptorName);
	    break;


	default:
	    break;
	}
	return;
    }
}
