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


static DOMString parentNode;
static ostream& operator<< (ostream& target, const DOMString& s);


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


// Remove namespace
char * removeNameSpace(const char *inputString, const char *nameSpaceToRemove)
{

    char *tmp = (char *)malloc(sizeof(char) * (strlen(inputString) + 1));
    tmp = strstr(inputString, nameSpaceToRemove);
    if (tmp != NULL)
    {
	tmp = tmp + strlen(nameSpaceToRemove);
	return tmp;
    }
    else
    {
	tmp = strcpy(tmp, inputString);
	return tmp;
    }
}


// Create a DOMString from a float
DOMString createFromFloat(float value, int numberOfDigit)
{
    char *myStringValue = (char *)malloc(numberOfDigit * sizeof(char));
    snprintf(myStringValue, numberOfDigit, "%f", value);
    DOMString returnValue(myStringValue);

    return returnValue;
}


// Create a DOMString from a double
DOMString createFromDouble(double value, int numberOfDigit)
{
    char *myStringValue = (char *)malloc(numberOfDigit * sizeof(char));
    snprintf(myStringValue, numberOfDigit, "%lf", value);
    DOMString returnValue(myStringValue);

    return returnValue;
}


// 
DOM_Element expandElementNode(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc )
{
    int indexAttr;
    DOM_Element currentChildElem;
    DOM_Element currentElement;
    DOMString rootNodeName = rootNode.getNodeName();
    if (strcmp(rootNodeName.transcode(), "extension") == 0)
    {
	// EXTENSION NODE
	DOM_NamedNodeMap rootAttributes = rootNode.getAttributes();
	DOM_Node attributBase = rootAttributes.getNamedItem("base");
	DOMString nodeValue = attributBase.getNodeValue();
	// Remove namespace
	DOMString typeToFind(removeNameSpace(nodeValue.transcode(), "mpeg7:"));
	// Get element in original Doc
	DOM_NodeList parentTypeDefinitionList = doc.getElementsByTagName(typeToFind);

	if (parentTypeDefinitionList.getLength() == 1)
	{
	    DOM_Node tmpNode = parentTypeDefinitionList.item(0);
	    DOM_Node parentRootNode = rootNode.getParentNode();
	    DOM_NodeList tmpChildList = tmpNode.getChildNodes();
	    int numberTmpChild = tmpChildList.getLength();
	    for (int childIndex = 0 ; childIndex < numberTmpChild ; childIndex++)
	    {
		//cout << " Append child Note " << childIndex << endl;
		DOM_Node currTmpChild = tmpChildList.item(childIndex);
		DOM_Node currTmpChildClone = currTmpChild.cloneNode(true);
		if (!currTmpChildClone.isNull())
		    rootElem.appendChild(currTmpChildClone);
	    }
	    
	}

    }
    // Now time to get all attributes of this node (name and type for element) (only name for complexType)
    DOM_NamedNodeMap rootNodeMap = rootNode.getAttributes();
    int numberOfNode = rootNodeMap.getLength();
    for (indexAttr = 0; indexAttr < numberOfNode; indexAttr++)
    {
	DOM_Node currNode = rootNodeMap.item(indexAttr);
	//const DOMString tagName(currNode.getNodeName());
	DOMString tagName(currNode.getNodeName());
	//const DOMString tagValue(currNode.getNodeValue());
	DOMString tagValue(currNode.getNodeValue());
	if (strcmp(tagName.transcode(), "name") == 0)
	    {
		if (strcmp(tagValue.transcode(), "0") != 0)
		{
		    currentChildElem = doc.createElement(tagValue);
		    // get next attributes to know if the type is specified
		    if ((indexAttr + 1) < numberOfNode) // There is another attribute
		    {
			indexAttr++; // To set on following attributes
			currNode = rootNodeMap.item(indexAttr);
			tagName = currNode.getNodeName();
			tagValue = currNode.getNodeValue();
			// Check if it is a type attribute
			    //if (strcmp(tagName.transcode(), "type") == 0)
			if (strcmp(tagName.transcode(), "type") == 0)
			{
			    // Add attributes for type definition
			    currentChildElem.setAttribute(tagName, tagValue);
			}
			if (strcmp(tagName.transcode(), "base") == 0)
			{
			    // Add attributes for type definition
			    DOMString newTagName("type");
			    currentChildElem.setAttribute(newTagName, tagValue);
			}
		    }
		}
		if (!currentChildElem.isNull())
		{
		    rootElem.appendChild(currentChildElem);
		    currentElement = currentChildElem;
		}
	    }
	
    }

    return currentElement;
}

 	
// Function which read the DOM tree (recursive function)
// DOM_Node rootNode (current Node)
// DOM_Element rootElem
// DOM_Document doc : mapped DOM tree
void mappingTree(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc)
{
    DOM_NodeList childNodeList = rootNode.getChildNodes(); 
    int numberOfChild = childNodeList.getLength();
    DOM_Element currentChildElem(rootElem);
    DOM_Element currentElement(rootElem);
    int indexChild;
    int indexAttr;

#ifdef DEBUG1
    //cout << " READTREE CALLED !!! \n";
#endif
    if (rootNode.getNodeType() == DOM_Node::ELEMENT_NODE)
    {
	// Now time to get all attributes of this node (name and type for element) (only name for complexType)
	DOM_NamedNodeMap rootNodeMap = rootNode.getAttributes();
	int numberOfNode = rootNodeMap.getLength();
	for (indexAttr = 0; indexAttr < numberOfNode; indexAttr++)
	{
	    DOM_Node currNode = rootNodeMap.item(indexAttr);
	    //const DOMString tagName(currNode.getNodeName());
	    //const DOMString tagValue(currNode.getNodeValue());
	    DOMString tagName(currNode.getNodeName());
	    DOMString tagValue(currNode.getNodeValue());
	    if (strcmp(tagName.transcode(), "name") == 0)
	    {
		if (strcmp(tagValue.transcode(), "0") != 0)
		{
		    currentChildElem = doc.createElement(tagValue);
		    // get next attributes to know if the type is specified
		    if ((indexAttr + 1) < numberOfNode) // There is another attribute
		    {
			indexAttr++; // To set on following attributes
			currNode = rootNodeMap.item(indexAttr);
			tagName = currNode.getNodeName();
			tagValue = currNode.getNodeValue();
			// Check if it is a type attribute
			if (strcmp(tagName.transcode(), "type") == 0)
			{
			    // Add attributes for type definition
			    currentChildElem.setAttribute(tagName, tagValue);
			}
			
		    }
		}
		rootElem.appendChild(currentChildElem);
		//parentNode += "/";
		//parentNode += currNode.getNodeValue();
		currentElement = currentChildElem;
	    }
	    
	}
    }

    for (indexChild =0 ; indexChild < numberOfChild; indexChild++)
    {
	// get currentChild
	DOM_Node currentChild = childNodeList.item(indexChild);
	// Append child to rootNodeDoc
	
	if (currentChild.isNull())
	    return;
	if (currentChild.hasChildNodes())
	{
	    // read tree from child
	    mappingTree(currentChild, currentChildElem, doc);
	}
	else
	{
	    if (currentChild.getNodeType() == DOM_Node::ELEMENT_NODE)
	    {
		DOM_NamedNodeMap rootNodeMap = currentChild.getAttributes();
		int numberOfNode = rootNodeMap.getLength();
		for (indexAttr = 0; indexAttr < numberOfNode; indexAttr++)
		{
		    DOM_Node currNode = rootNodeMap.item(indexAttr);
		    const DOMString tagName(currNode.getNodeName());
		    const DOMString tagValue(currNode.getNodeValue());
		    if (strcmp(tagName.transcode(), "name") == 0)
		    {
			if (strcmp(tagValue.transcode(), "0") != 0)
			{
			    currentChildElem = doc.createElement(tagValue);
			}
			currentElement.appendChild(currentChildElem);
		    }
		}
	    }
	}
    }
}

//
// TYPE DEFINITION TREE
//

// Function which read the DOM tree (recursive function)
// DOM_Node rootNode (current Node)
// DOM_Element rootElem (root Node of the Document)
// DOM_Document doc : mapped DOM tree
void typeDefinitionTree(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc, DOM_Document originalDoc)
{
    DOM_NodeList childNodeList;
    DOM_Element currentChildElem; //(rootElem);
    DOM_Element currentElement(rootElem);
    int indexChild;
    int indexAttr;
    //DOMString parentNode;

    if (rootNode.getNodeType() == DOM_Node::ELEMENT_NODE)
    {
	//currentElement = expandElementNode(rootNode, rootElem, doc);
	DOMString rootNodeName = rootNode.getNodeName();
	if (strcmp(rootNodeName.transcode(), "extension") == 0)
	{
	    // EXTENSION NODE
	    DOM_NamedNodeMap rootAttributes = rootNode.getAttributes();
	    DOM_Node attributBase = rootAttributes.getNamedItem("base");
	    DOMString nodeValue = attributBase.getNodeValue();
	    // Remove namespace
	    DOMString typeToFind(removeNameSpace(nodeValue.transcode(), "mpeg7:"));
	    // Get element in original Doc
	    DOM_NodeList parentTypeDefinitionList = doc.getElementsByTagName(typeToFind);

	    if (parentTypeDefinitionList.getLength() == 1)
	    {
		DOM_Node tmpNode = parentTypeDefinitionList.item(0);
		DOM_Node parentRootNode = rootNode.getParentNode();

		DOM_NodeList tmpChildList = tmpNode.getChildNodes();
		int numberTmpChild = tmpChildList.getLength();
		for (int childIndex = 0 ; childIndex < numberTmpChild ; childIndex++)
		{
		    //cout << " Append child Note " << childIndex << endl;
		    DOM_Node currTmpChild = tmpChildList.item(childIndex);
		    DOM_Node currTmpChildClone = currTmpChild.cloneNode(true);
		    if (!currTmpChildClone.isNull())
			rootElem.appendChild(currTmpChildClone);
		}
		
	    }
	}
	// Now time to get all attributes of this node (name and type for element) (only name for complexType)
	DOM_NamedNodeMap rootNodeMap = rootNode.getAttributes();
	int numberOfNode = rootNodeMap.getLength();
	for (indexAttr = 0; indexAttr < numberOfNode; indexAttr++)
	{
	    DOM_Node currNode = rootNodeMap.item(indexAttr);
	    //const DOMString tagName(currNode.getNodeName());
	    //const DOMString tagValue(currNode.getNodeValue());
	    DOMString tagName(currNode.getNodeName());
	    DOMString tagValue(currNode.getNodeValue());
	    if (strcmp(tagName.transcode(), "name") == 0)
	    {
		if (strcmp(tagValue.transcode(), "0") != 0)
		{
		    currentChildElem = doc.createElement(tagValue);
		    // get next attributes to know if the type is specified
		    if ((indexAttr + 1) < numberOfNode) // There is another attribute
		    {
			indexAttr++; // To set on following attributes
			currNode = rootNodeMap.item(indexAttr);
			tagName = currNode.getNodeName();
			tagValue = currNode.getNodeValue();			
			// Check if it is a type attribute
			// TODO : Passe the tagName in function attribute
			if (strcmp(tagName.transcode(), "type") == 0)
			{
			    // Add attributes for type definition
			    currentChildElem.setAttribute(tagName, tagValue);
			}
			if (strcmp(tagName.transcode(), "base") == 0)
			{
			    // Add attributes for type definition
			    DOMString newTagName("type");
			    currentChildElem.setAttribute(newTagName, tagValue);
			}
		    }
		}
		if (!currentChildElem.isNull())
		{
		    rootElem.appendChild(currentChildElem);
		    currentElement = currentChildElem;
		}
	    }	    
	}	
    }
    
    int numberOfChild;
    if (rootNode.hasChildNodes())
    {
	childNodeList = rootNode.getChildNodes(); 
	numberOfChild = childNodeList.getLength();
    }
    else
	numberOfChild = 0;
    
    for (indexChild =0 ; indexChild < numberOfChild; indexChild++)
    {
	// get currentChild
	DOM_Node currentChild = childNodeList.item(indexChild);
	
	if (currentChild.hasChildNodes())
	{
	    // read tree from child
	    typeDefinitionTree(currentChild, currentChildElem, doc, originalDoc);
	}
	else
	{
	    if (currentChild.getNodeType() == DOM_Node::ELEMENT_NODE)
	    {
		DOMString childNodeName = currentChild.getNodeName();
		if (strcmp(childNodeName.transcode(), "extension") == 0)
		{
		    // EXTENSION NODE
		    DOM_NamedNodeMap childAttributes = currentChild.getAttributes();
		    DOM_Node attributBase = childAttributes.getNamedItem("base");
		    DOMString nodeValue = attributBase.getNodeValue();
		    // Remove namespace
		    DOMString typeToFind(removeNameSpace(nodeValue.transcode(), "mpeg7:"));
		    // Get element in original Doc
		    DOM_NodeList parentTypeDefinitionList = doc.getElementsByTagName(typeToFind);

		    if (parentTypeDefinitionList.getLength() == 1)
		    {
			DOM_Node tmpNode = parentTypeDefinitionList.item(0);
			DOM_Node parentRootNode = currentChild.getParentNode();

			DOM_NodeList tmpChildList = tmpNode.getChildNodes();
			int numberTmpChild = tmpChildList.getLength();
			for (int childIndex = 0 ; childIndex < numberTmpChild ; childIndex++)
			{
			    // Append child Node
			    DOM_Node currTmpChild = tmpChildList.item(childIndex);
			    DOM_Node currTmpChildClone = currTmpChild.cloneNode(true);
			    if (!currTmpChildClone.isNull())
				//parentRootNode.appendChild(currTmpChildClone);
				currentElement.appendChild(currTmpChildClone);
			}
		    }
		    continue;
		}
		DOM_NamedNodeMap rootNodeMap = currentChild.getAttributes();
		int numberOfNode = rootNodeMap.getLength();
		for (indexAttr = 0; indexAttr < numberOfNode; indexAttr++)
		{
		    DOM_Node currNode = rootNodeMap.item(indexAttr);
		    //const DOMString tagName(currNode.getNodeName());
		    //const DOMString tagValue(currNode.getNodeValue());
		    DOMString tagName(currNode.getNodeName());
		    DOMString tagValue(currNode.getNodeValue());
		    if (strcmp(tagName.transcode(), "name") == 0)
		    {
			if (strcmp(tagValue.transcode(), "0") != 0)
			{
			    currentChildElem = doc.createElement(tagValue);
			    // get next attributes to know if the type is specified
			    if ((indexAttr + 1) < numberOfNode) // There is another attribute
			    {
				indexAttr++; // To set on following attributes
				currNode = rootNodeMap.item(indexAttr);
				tagName = currNode.getNodeName();
				tagValue = currNode.getNodeValue();
				// Check if it is a type attribute
				if ((strcmp(tagName.transcode(), "type") == 0) || (strcmp(tagName.transcode(), "base") == 0))
				{
				    // Add attributes for type definition
				    currentChildElem.setAttribute(tagName, tagValue);
				}
			    }
			}
			if (!currentElement.isNull())
			{
			    currentElement.appendChild(currentChildElem);

			    parentNode += "/";
			    parentNode += currNode.getNodeValue();
			}
		    }
		    
		}
	    }
	}
    }
}


void checkTypedElement(DOM_Node rootNode, DOM_Document domDocument, DOM_Document originalDoc)
{
    DOM_NodeList childNodeList = rootNode.getChildNodes();
    int numberOfChild = childNodeList.getLength();
    int indexChild;

    if (rootNode.getNodeType() == DOM_Node::ELEMENT_NODE)
    {
	// Expand this part of the tree
	//cout << " ELEMENT NODE !! " << rootNode.getNodeName() << endl;
	expandTypedElement(rootNode, domDocument, originalDoc);
    }

    for (indexChild = 0; indexChild < numberOfChild ; indexChild++)
    {
	DOM_Node currentChild = childNodeList.item(indexChild);
	
	if (currentChild.hasChildNodes())
	{
	    checkTypedElement(currentChild, domDocument, originalDoc);
	}
	else
	{	    
	    if (currentChild.getNodeType() == DOM_Node::ELEMENT_NODE)
	    {
		// Get attributes of this element
		expandTypedElement(currentChild, domDocument, originalDoc);		
	    }
	}
    }
}

void expandExtensionElement(DOM_Node rootNode, DOM_Document domDocument, DOM_Document typedDocument)
{
    DOM_NodeList extensionElementList = domDocument.getElementsByTagName("extension");
    int numberOfExtensionElement  = extensionElementList.getLength();
    
    for (int index = 0; index < numberOfExtensionElement; index++)
    {
	DOM_Node currentNode = extensionElementList.item(index);
	DOM_NamedNodeMap attributesMap = currentNode.getAttributes();
	int numberOfAttributes = attributesMap.getLength();
	for (int indexAttr = 0 ; indexAttr < numberOfAttributes; indexAttr++)
	{
	    DOM_Node currentAttributes = attributesMap.item(indexAttr);
	    DOMString tagName = currentAttributes.getNodeName();
	    DOMString tagValue = currentAttributes.getNodeValue();
	    if (strcmp(tagName.transcode(), "base") == 0)
	    {
		DOMString tagToSearch(removeNameSpace(tagValue.transcode(), "mpeg7:"));
		//cout << " tagToSearch " << tagToSearch << endl;
		DOM_NodeList foundElementList = typedDocument.getElementsByTagName(tagToSearch);
		int numberOfElementFound = foundElementList.getLength();
		//cout << " numberOfElementFound " << numberOfElementFound << endl << endl;
	    }
	}			   
    }    
    return;
}


void expandTypedElement(DOM_Node rootNode, DOM_Document domDocument, DOM_Document originalDoc)
{
    DOM_Node currentChild(rootNode);
    int indexAttr;

    DOMString nodeName = rootNode.getNodeName();
    if (strcmp(nodeName.transcode(), "extension") == 0)
    {
	//expandExtensionElement(rootNode, domDocument);
	//cout << " EXTENSION ELEMENT SEE IN EXPAND TYPED ELEMENT !!!! " << endl;
    }

    // Get attributes of this element
    DOM_NamedNodeMap currentChildMap = currentChild.getAttributes();
    int numberOfAttributes = currentChildMap.getLength();

    for (indexAttr = 0; indexAttr < numberOfAttributes ; indexAttr++)
    {
	DOM_Node currentNode = currentChildMap.item(indexAttr);
	//const DOMString tagName(currentNode.getNodeName());
	//const DOMString tagValue(currentNode.getNodeValue());
	DOMString tagName(currentNode.getNodeName());
	DOMString tagValue(currentNode.getNodeValue());
	// BEWARE CHECK FOR SPECIAL ATTRIBUTES
	if (strcmp(tagName.transcode(), "type") == 0)
	{
	    // Parent Node is rootNode			
	    // Get Node where type definition occurs
	    // Need to check for xsd:type use !!!!
	    if (strstr(tagValue.transcode(), "xsd") == NULL)
	    {
		// Deal with mpeg7: type
		//cout << " BEFORE : " << tagValue << endl;
		DOMString tmpTag(removeNameSpace(tagValue.transcode(), "mpeg7:"));
		tagValue = tmpTag;
		//cout << " AFTER : " << tagValue << endl ;
		    
		//cout << " TAG CURRENTLY SEARCHED : " << tagValue << endl;
		DOM_NodeList thisTypeDefinition = domDocument.getElementsByTagName(tagValue);
		int numberOfTypeDefinition = thisTypeDefinition.getLength();			
		if (numberOfTypeDefinition > 1)
		{
		    //cout << " There is " << numberOfTypeDefinition << " !!! " << endl;
		}
		else
		{
		    if (numberOfTypeDefinition == 0)
		    {
			//cout << " This type is not defined !! " << endl;
			return;
		    }
		    DOM_Node tmpNode = thisTypeDefinition.item(0);
		    DOM_NodeList tmpChildList = tmpNode.getChildNodes();
		    int tmpNumberOfChild = tmpChildList.getLength();
		    //Check for all child of this node
		    //while (tmpChildList.getLength() != 0)			
		    for (int i = 0; i < tmpNumberOfChild ; i++)
		    {
			// Get first child (all previous have been removed)
			// With while loop
			//int i = 0;
			DOM_Node oldTmpNode = tmpChildList.item(i);
			DOM_NamedNodeMap oldTmpNodeAttr = oldTmpNode.getAttributes();
			if (oldTmpNodeAttr != NULL)
			{
			    if (!oldTmpNode.hasChildNodes())
			    {
				expandTypedElement(oldTmpNode, domDocument, originalDoc);
				// Here the current node has been expanded
				// So we can try to remove the type attributes
				const DOMString attributesNameToRemove("type");
				DOM_Node typeAttr = oldTmpNodeAttr.getNamedItem(attributesNameToRemove);
				const DOMString newValue("xsd:foo");
				typeAttr.setNodeValue(newValue);
			    }
			}
			// The type attribute have been remove, so we can append it to the tree
			// Clone the node before append it to the tree
			DOM_Node currTmpNode = oldTmpNode.cloneNode(true);

			currentChild.appendChild(currTmpNode);
		    }		    
		}
	    }
	}
    }
}


void appendAllChildren(DOM_Document outputDoc, DOM_Node parentDefNode , DOM_Element fatherElement, DOMString myValue)
{
    // Function which get all the children and append it to outputDoc DOM tree

    DOM_NodeList  parentDefNodeChildList = parentDefNode.getChildNodes();
    int numberOfChild = parentDefNodeChildList.getLength();
    for (int indexChild = 0; indexChild < numberOfChild; indexChild++)
    {
	DOM_Node currentParentDefNodeChild = parentDefNodeChildList.item(indexChild);
	DOM_Element myChildElement = outputDoc.createElement(currentParentDefNodeChild.getNodeName());
	if (!currentParentDefNodeChild.hasChildNodes())
	{
	    if (!myChildElement.isNull())
	    {
		DOM_Text textValue = outputDoc.createTextNode(myValue);				
		myChildElement.appendChild(textValue);
	    }
	fatherElement.appendChild(myChildElement);
	}
	else
	{
	    DOM_Text textValue = outputDoc.createTextNode(myValue);
	    DOM_Node nodeToRemove = fatherElement.getFirstChild();
	    fatherElement.removeChild(nodeToRemove);
	    fatherElement.appendChild(myChildElement);
	}

    }
}

void searchAndSetValue(DOM_Document outputDoc, DOM_Node currentTypeDefinition, DOM_Node myNodeToAppend,
		       DOMString childType, DOMString myValue )
{    
    // Function search and set node (child name, new value, parent node, typedNode)
    DOM_NodeList subTypeList = currentTypeDefinition.getChildNodes();
    int numberSubType = subTypeList.getLength();
    bool needToAppendAChild = true;
    for (int index = 0; index < numberSubType; index++)
    {
	// Check if the name is the one we want
	DOM_Node tmpNode = subTypeList.item(index);
	if (strcmp(tmpNode.getNodeName().transcode(), childType.transcode()) == 0)
	{	    
	    if (myNodeToAppend.hasChildNodes())
	    {		
		// Check if there is no child with the same name
		DOM_NodeList localChild = myNodeToAppend.getChildNodes();
		for (int i = 0; i < localChild.getLength(); i++)
		{
		    DOM_Node tmpChildNode = localChild.item(i);
		    if (strcmp(tmpChildNode.getNodeName().transcode(), childType.transcode()) == 0)
		    {
			// There is already a child with the same name
			needToAppendAChild = false;
		    }
		}
	    }
	    if (needToAppendAChild == true)
	    {
		DOM_Element myClone = outputDoc.createElement(childType);
		if (!myClone.isNull())
		    myNodeToAppend.appendChild(myClone);
		
		appendAllChildren(outputDoc, tmpNode, myClone, myValue);
	    }
	}
    }
}


void addThisValueToOutput(DOM_Document typeDefinitionDoc, DOM_Document outputDoc, 
			  DOMString parentType, DOMString childType, DOMString myValue)
{
    // Function to create OUTPUT
    DOMString tagToSearch;
    tagToSearch.appendData(parentType);
    tagToSearch.appendData("Type");
    
    DOM_NodeList nodeListDoc = typeDefinitionDoc.getElementsByTagName(tagToSearch);
    int lengthNodeListDoc = nodeListDoc.getLength();
    
    if (lengthNodeListDoc != 1)
    {
	cerr << " Error in type definition !!! " << endl;
	return;
    }
    
    DOM_Node currentTypeDefinition = nodeListDoc.item(0);
    
    // Check if the node already exist !!
    DOM_Node myNodeToAppend;
    DOM_NodeList resultSearchNode = outputDoc.getElementsByTagName(parentType);
    if (resultSearchNode.getLength() == 0)
    {
	myNodeToAppend = outputDoc.createElement(parentType);
	DOM_Element rootOutputElement = outputDoc.getDocumentElement();
	
	rootOutputElement.appendChild(myNodeToAppend);
    }
    else
    {
	myNodeToAppend = resultSearchNode.item(0);
    }
    searchAndSetValue(outputDoc, currentTypeDefinition, myNodeToAppend, childType, myValue);
	    
}

void generateFirstMappingTree(DOM_Document originalDoc, DOM_Document mappingDoc, DOM_XMLDecl rootDecl, DOMString tagToSearch)
{
    if (originalDoc.getNodeType() == DOM_Node::DOCUMENT_NODE)
    {
	// Get reference of the root Element of mappingDoc DOM Tree
	DOM_Element mappingDocRootElem = mappingDoc.getDocumentElement();
	if (rootDecl.isNull())
	{
	    cout << " Create XML Decl Failed !!! \n";
	}
	else
	{
	    // Insert XML_DECL_NODE in first position
	    mappingDoc.insertBefore(rootDecl,mappingDocRootElem);
	}
	
	// Get a list of node
	DOM_NodeList nodeListDoc = originalDoc.getElementsByTagName(tagToSearch);
	int lengthNodeListDoc = nodeListDoc.getLength();
	for (int i = 0; i < lengthNodeListDoc ; i++)
	{
	    DOM_Node currentSelectedNode = nodeListDoc.item(i);
	    DOM_Element currentRootElem(mappingDocRootElem);
	    
	    mappingTree(currentSelectedNode, currentRootElem, mappingDoc);
	}
    }
}



void generateSecondMappingTree(DOM_Document originalDoc, DOM_Document typeDefinitionDoc, 
			       DOM_XMLDecl rootDecl, DOMString tagToSearch)
{
    if (originalDoc.getNodeType() == DOM_Node::DOCUMENT_NODE)
    {
	// Get reference of the root Element of typeDefinitionDoc DOM Tree
	DOM_Element typeDefinitionDocRootElem = typeDefinitionDoc.getDocumentElement();
	if (rootDecl.isNull())
	    cout << " Create XML Decl Failed !!! \n";
	else
	{
	    // Insert XML_DECL_NODE in first position
	    typeDefinitionDoc.insertBefore(rootDecl,typeDefinitionDocRootElem);
	}
	
	//cout << " Tag To Search : " << tagToSearch << endl;
	DOM_NodeList nodeListDoc = originalDoc.getElementsByTagName(tagToSearch);
	int lengthNodeListDoc = nodeListDoc.getLength();
	for (int i = 0; i < lengthNodeListDoc ; i++)
	{
	    DOM_Node currentSelectedNode = nodeListDoc.item(i);		
	    DOM_Element currentRootElem(typeDefinitionDocRootElem);
	    
	    typeDefinitionTree(currentSelectedNode, currentRootElem, typeDefinitionDoc, originalDoc);
	}
    }
}
