#ifndef TREEFUNCTION_HPP_
#define TREEFUNCTION_HPP_ 1

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

char* removeNameSpace(const char *inputString, const char *nameSpaceToRemove);
DOMString createFromFloat(float value, int numberOfDigit);
DOMString createFromDouble(double value, int numberOfDigit);


void mappingTree(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc);
void typeDefinitionTree(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc, DOM_Document originalDoc);
void checkTypedElement(DOM_Node rootNode, DOM_Document domDocument, DOM_Document originalDoc);
void expandExtensionElement(DOM_Node rootNode, DOM_Document domDocument, DOM_Document typedDocument);
void expandTypedElement(DOM_Node rootNode, DOM_Document domDocument, DOM_Document originalDoc);
DOM_Element expandElementNode(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc);


void appendAllChildren(DOM_Document outputDoc, DOM_Node parentTypeDefintionNode , 
		       DOM_Element fatherElement, DOMString myValue);
void searchAndSetValue(DOM_Document outputDoc, DOM_Node currentTypeDefinition, DOM_Node myNodeToAppend,
		       DOMString childType, DOMString myValue );
void addThisValueToOutput(DOM_Document typedDefinitionDoc, DOM_Document outputDoc, 
			  DOMString parentType, DOMString childType, DOMString myValue);
void generateFirstMappingTree(DOM_Document originalDoc, DOM_Document mappingDoc, 
			      DOM_XMLDecl rootDecl, DOMString tagToSearch);
void generateSecondMappingTree(DOM_Document originalDoc, DOM_Document typeDefinitionDoc, 
			       DOM_XMLDecl rootDecl, DOMString tagToSearch);
#endif /* TREEFUNCTION_HPP_ */
