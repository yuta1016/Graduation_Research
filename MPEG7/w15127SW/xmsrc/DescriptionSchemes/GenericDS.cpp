///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S. Herrmann
// (contributing organizations names)
//
// Javier Ruiz Hidalgo - University Politechnic of Catalonis
// Ana B. Benitez - Columbia University
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
// Applicable File Name:  GenericDS.cpp
//
#include "DescriptionSchemes/DescriptionSchemes.h"          /* Naming Convention */

#ifdef WIN32
#pragma warning(disable: 4786)
//#include <strstream>
//#include <sstream>
#endif
#include <stdio.h>
#include <string.h>

#if defined(USEXML) || defined(USEXML3)
#include "DDL/XML4Cutils.h"
#endif

using namespace XM;

const UUID GenericDSInterface::myID = UUID("");
const char *GenericDSInterface::myName = "GenericDSInterface";

const UUID GenericDS::myID = UUID("");
const char *GenericDS::myName = "GenericDS";

const UUID GenericDS::valID = UUID("");
const char *GenericDS::valName = "Generic";

const UUID GenericDSInterfaceABC::myID = UUID();


GenericDSInterfaceABC::GenericDSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
GenericDSInterface::
GenericDSInterface(GenericDS *aGenericDS):
m_GenericDS(aGenericDS)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GenericDSInterface::~GenericDSInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& GenericDSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GenericDSInterface::GetName(void)
{ return myName; }
//----------------------------------------------------------------------------
#ifdef OLDGENERICDS
DOM_Document GenericDSInterface::GetDOMDocument(void)
{
	return m_GenericDS->GetDOMDocument();
}

//----------------------------------------------------------------------------
unsigned long GenericDSInterface::SetDOMDocument(DOM_Document DOMDoc)
{
		return m_GenericDS->SetDOMDocument(DOMDoc);
}
#endif //OLDGENERICDS

//----------------------------------------------------------------------------
#if defined(USEXML3)
DOMDocument *GenericDSInterface::GetDocument(void)
{
	return m_GenericDS->GetDocument();
}

//----------------------------------------------------------------------------
void GenericDSInterface::SetDOMNode(DOMNode *node)
{
	m_GenericDS->SetDOMNode(node);
}
#elif defined(USEXML)
DOM_Document GenericDSInterface::GetDocument(void)
{
	return m_GenericDS->GetDocument();
}

//----------------------------------------------------------------------------
void GenericDSInterface::SetDOMNode(DOM_Node node)
{
	m_GenericDS->SetDOMNode(node);
}
#endif // USEXML3

//----------------------------------------------------------------------------
bool GenericDSInterface::isNull()
{
	return m_GenericDS->isNull();
}

//----------------------------------------------------------------------------
string GenericDSInterface::GetDSName()
{
	return m_GenericDS->GetDSName();
}

//----------------------------------------------------------------------------
vector<string> GenericDSInterface::GetAttributeNames()
{
	return m_GenericDS->GetAttributeNames();
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetIntAttribute(const string& aAttrName, int& value)
{
	return m_GenericDS->GetIntAttribute(aAttrName, value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetRealAttribute(const string& attrName, double& value)
{
	return m_GenericDS->GetRealAttribute(attrName, value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetTextAttribute(const string& name, string& value)
{
	return m_GenericDS->GetTextAttribute(name, value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetBoolAttribute(const string& name, bool& value)
{
	return m_GenericDS->GetBoolAttribute(name, value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetIntValue(int& value)
{
	return m_GenericDS->GetIntValue(value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetRealValue(double& value)
{
	return m_GenericDS->GetRealValue(value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetTextValue(string& value)
{
	return m_GenericDS->GetTextValue(value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetRealVector(vector<double>& value)
{
	return m_GenericDS->GetRealVector(value);
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetIntVector(vector<int>& value)
{
	return m_GenericDS->GetIntVector(value);
}

//----------------------------------------------------------------------------
GenericDS& GenericDSInterface::SetValue(const int& value)
{
	return m_GenericDS->SetValue(value);
}

//----------------------------------------------------------------------------
GenericDS& GenericDSInterface::SetValue(const double& value)
{
	return m_GenericDS->SetValue(value);
}

//----------------------------------------------------------------------------
GenericDS& GenericDSInterface::SetValue(const string& value)
{
	return m_GenericDS->SetValue(value);
}

//----------------------------------------------------------------------------
GenericDS& GenericDSInterface::SetValue(const vector<double>& value)
{
	return m_GenericDS->SetValue(value);
}

//----------------------------------------------------------------------------
GenericDS& GenericDSInterface::SetValue(const vector<int>& value)
{
	return m_GenericDS->SetValue(value);
}

//----------------------------------------------------------------------------
GenericDS& GenericDSInterface::SetValue(const vector<string>& value)
{
	return m_GenericDS->SetValue(value);
}

//----------------------------------------------------------------------------
GenericDS & GenericDSInterface::SetAttribute(const string& aName, int value)
{
	return m_GenericDS->SetAttribute(aName, value);
}

//----------------------------------------------------------------------------
GenericDS & GenericDSInterface::SetAttribute(const string& aName, double value)
{
	return m_GenericDS->SetAttribute(aName, value);
}

//----------------------------------------------------------------------------
GenericDS & GenericDSInterface::SetAttribute(const string& aName, string value)
{
	return m_GenericDS->SetAttribute(aName, value);
}

//----------------------------------------------------------------------------
void GenericDSInterface::RemoveAttribute(const string& aName)
{
	m_GenericDS->RemoveAttribute(aName);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetDescription(const string& name, int index)
{
	return m_GenericDS->GetDescription(name, index);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetNextSibling(const string& name)
{
	return m_GenericDS->GetNextSibling(name);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::LocateNode(const string& name)
{
	return m_GenericDS->LocateNode(name);
}

//----------------------------------------------------------------------------
vector<GenericDS> GenericDSInterface::GetDescription(const vector<string>& path)
{
	return m_GenericDS->GetDescription(path);
}

//----------------------------------------------------------------------------
vector<GenericDS> GenericDSInterface::GetAllDescriptions(const string& name)
{
	return m_GenericDS->GetAllDescriptions(name);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetRoot()
{
	return m_GenericDS->GetRoot();
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetFirstChild()
{
	return m_GenericDS->GetFirstChild();
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetNextSibling()
{
	return m_GenericDS->GetNextSibling();
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetChild(int index)
{
	return m_GenericDS->GetChild(index);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetChild(const string& name)
{
	return m_GenericDS->GetChild(name);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::GetParent()
{
	return m_GenericDS->GetParent();
}

//----------------------------------------------------------------------------
int GenericDSInterface::GetNumberOfChildren()
{
	return m_GenericDS->GetNumberOfChildren();
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::InsertBefore (const GenericDS &newChild,
	     const GenericDS &refChild)
{
	return m_GenericDS->InsertBefore(newChild, refChild);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::ReplaceChild (const GenericDS &newChild,
	     const GenericDS &oldChild)
{
	return m_GenericDS->ReplaceChild (newChild,oldChild);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::RemoveChild (const GenericDS &oldChild)
{
	return m_GenericDS->RemoveChild (oldChild);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::AppendChild (const GenericDS &newChild)
{
	return m_GenericDS->AppendChild(newChild);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::CreateChild(const string& name)
{
	return m_GenericDS->CreateChild(name);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::CreateDescription(GenericDS &root,
						const string& name)
{
        return m_GenericDS->CreateDescription(root,name);
}


//----------------------------------------------------------------------------
GenericDS GenericDSInterface::CreateDescriptionRoot()
{
        return m_GenericDS->CreateDescriptionRoot();
}

#ifdef USEXML3
void GenericDSInterface::SetDocument(GenericDS &other)
{
	m_GenericDS->SetDocument(other);
}

//----------------------------------------------------------------------------
GenericDS GenericDSInterface::CreateDescriptionRoot(const DOMDocument* doc)
{
        return m_GenericDS->CreateDescriptionRoot(doc);
}

//=============================================================================
GenericDS::GenericDS():
m_Interface(this),
m_isProprietary(false)
{
	m_DOMNode = 0;
	m_DocRef = 0;
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GenericDS::~GenericDS()
{
	releaseDocument();
}

//----------------------------------------------------------------------------
const UUID& GenericDS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GenericDS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GenericDS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GenericDS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GenericDS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
GenericDSInterfaceABC *GenericDS::GetInterface(void)
{ return &m_Interface; }


//----------------------------------------------------------------------------
bool
GenericDS::isNull() {
	return (m_DOMNode == 0); 
};

//----------------------------------------------------------------------------
    //----------- Get functions--------------
    //---------------------------------------

    // Returns the name of the descriptor or description scheme.
    // This is the same as the name of the XML tag.
string
GenericDS::GetDSName() {
	return DOMString_to_string(m_DOMNode->getNodeName());
};

//----------------------------------------------------------------------------
    // Attribute access.
    // Gets all the attribute names
vector<string>
GenericDS::GetAttributeNames() {
    vector<string> attrNames(0);
    DOMNamedNodeMap *attrMap;
    DOMNode  *attrNode;

	attrMap = m_DOMNode->getAttributes();
	if (attrMap != 0) {
	    for (unsigned int i=0; i<attrMap->getLength(); i++)
			attrNames.push_back(DOMString_to_string(attrMap->item(i)->getNodeName()));
	}
    return attrNames;
};

//----------------------------------------------------------------------------
    // For all these methods, it is an error to reference a non-integer or non-existent attribute.
    // If an error occurs, the a non-zero return code is return.
    // Return the value of the named attribute as an integer.
int
GenericDS::GetIntAttribute(const string& attrName,
				    int&  value) {
    DOMNamedNodeMap *attrMap;
    DOMNode  *attrNode;
	XMLCh *xmlStr=0;
    string str;
    double testValue;
	attrMap = m_DOMNode->getAttributes();
	if(attrMap == NULL)  return GenericDS::NO_SUCH_ATTR;
	xmlStr = string_to_DOMString(attrName);
	attrNode = attrMap->getNamedItem(xmlStr);
	release_DOMString(&xmlStr);
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    str = DOMString_to_string(attrNode->getNodeValue());
    if (sscanf(str.c_str(),"%d", &value) !=1 ) return UNABLE_TO_CONVERT;
    sscanf(str.c_str(),"%le", &testValue);
    if (testValue-value != 0.0) return UNABLE_TO_CONVERT_IS_REAL;
    return GenericDS::SUCCESS; // Successfull convertion
};
  
//----------------------------------------------------------------------------
    // Return the value of the named attribute as areal value.
int
GenericDS::GetRealAttribute(const string& attrName,
					double& value) {
    DOMNamedNodeMap *attrMap;
    DOMNode  *attrNode;
    string str;
	XMLCh *xmlStr;

	attrMap = m_DOMNode->getAttributes();
	if(attrMap == NULL)  return GenericDS::NO_SUCH_ATTR;
	xmlStr = string_to_DOMString(attrName);
	attrNode = attrMap->getNamedItem(xmlStr);
	release_DOMString(&xmlStr);
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    str = DOMString_to_string(attrNode->getNodeValue());
    if (sscanf(str.c_str(),"%le",&value) !=1)
		return GenericDS::UNABLE_TO_CONVERT;

	return GenericDS::SUCCESS; // Successfull convertion
};  

//----------------------------------------------------------------------------
    // Return the value of the named attribute as a string.
int
GenericDS::GetTextAttribute(const string& attrName, string& value) {
    DOMNamedNodeMap *attrMap;
    DOMNode  *attrNode;
	XMLCh *xmlStr;

	attrMap = m_DOMNode->getAttributes();
	if(attrMap == NULL)  return GenericDS::NO_SUCH_ATTR;
	xmlStr = string_to_DOMString(attrName);
	attrNode = attrMap->getNamedItem(xmlStr);
	release_DOMString(&xmlStr);
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    value = DOMString_to_string(attrNode->getNodeValue());
	return GenericDS::SUCCESS; // Successfull convertion
};   

//----------------------------------------------------------------------------
    // Return the value of the named attribute as a boolean.
int
GenericDS::GetBoolAttribute(const string& attrName, bool& value) {
    DOMNamedNodeMap *attrMap;
    DOMNode  *attrNode;
    string str;
	XMLCh *xmlStr;

	attrMap = m_DOMNode->getAttributes();
	if(attrMap == NULL)  return GenericDS::NO_SUCH_ATTR;
	xmlStr = string_to_DOMString(attrName);
	attrNode = attrMap->getNamedItem(xmlStr);
	release_DOMString(&xmlStr);
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    str = DOMString_to_string(attrNode->getNodeValue());
    if ((str == "true") || (str == "TRUE") || (str == "True") || (str == "1")) {
	value = true;
	return GenericDS::SUCCESS;
    }
    if ((str == "false") || (str == "FALSE") || (str == "False")|| (str == "0")) {
	value = false;
	return GenericDS::SUCCESS;
    }
    // Unsuccessfull convertion`
    return GenericDS::UNABLE_TO_CONVERT;
};

//----------------------------------------------------------------------------
    // Get the value of this descriptor or description scheme.
    // The value is the text enclosed between the tags 
    // representing the descriptor
    // For example <ControlledTerm>term</ControlledTerm> the value
    // refers to "term".
    // If the value can not be returned, an non-zero error code if returned.
    
    // Get the D/DS value as an integer.
int
GenericDS::GetIntValue(int& value) {
    DOMNode *contentNode;
    string str;

    if (m_DOMNode == NULL) return 0;
    //    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode->getFirstChild();
    while (contentNode != 0) {
	if (contentNode->getNodeType() == DOMNode::TEXT_NODE) {
	    str = DOMString_to_string(contentNode->getNodeValue());
	    if (sscanf(str.c_str(),"%d",&value) !=1)
			return GenericDS::UNABLE_TO_CONVERT;
	    return GenericDS::SUCCESS; // Successfull convertion
	}
	contentNode = contentNode->getNextSibling();
    }
    return GenericDS::NO_CONTENT;
};

//----------------------------------------------------------------------------
    // Get the D/DS value as a real number.
int
GenericDS::GetRealValue(double& value) {
    DOMNode *contentNode;
    string str;
    
    if (m_DOMNode == NULL) return 0;
    //    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode->getFirstChild();
    while (contentNode != 0) {
	if (contentNode->getNodeType() == DOMNode::TEXT_NODE) {
	    str = DOMString_to_string(contentNode->getNodeValue());
	    if (sscanf(str.c_str(),"%le", &value) !=1)
			return GenericDS::UNABLE_TO_CONVERT;
	    return GenericDS::SUCCESS; // Successfull convertion
	}
	contentNode = contentNode->getNextSibling();
    }
    return GenericDS::NO_CONTENT;
};

//----------------------------------------------------------------------------
// Get the D/DS value as text.
int
GenericDS::GetTextValue(string& value) {
    DOMNode *contentNode;
    
    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode->getFirstChild();
    while (contentNode != 0) {
	if (contentNode->getNodeType() == DOMNode::TEXT_NODE) {
	    value = DOMString_to_string(contentNode->getNodeValue());
	    return GenericDS::SUCCESS;
		}
	contentNode = contentNode->getNextSibling();
    }
    return GenericDS::NO_CONTENT;
};

//----------------------------------------------------------------------------
// Get the D/DS value as vector of real numbers. This is
// used to access value that represent real vectors. 
int
GenericDS::GetRealVector(vector<double>& value) {
    DOMNode *contentNode;
    string str;
    double realValue;
    
    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode->getFirstChild();
    while (contentNode != 0) {
	if (contentNode->getNodeType() == DOMNode::TEXT_NODE) {
	    str = DOMString_to_string(contentNode->getNodeValue());
	    break;
	}
	contentNode = contentNode->getNextSibling();
    }
    if (contentNode==0)
	return GenericDS::UNABLE_TO_CONVERT;
    
    char* cStr = new char[str.size() + 1];
    str.copy(cStr, str.size());
    cStr[str.size()] = 0;
    
    int  error = 0;
    char *seps = " \t\n";
    char *token = strtok(cStr, seps);
    while(token != NULL)
    {
	if (sscanf(token,"%le",&realValue) !=1)
	{
	    error = GenericDS::UNABLE_TO_CONVERT;
	    break;
	}
	value.push_back(realValue);    
	token = strtok( NULL, seps );
    }
    delete cStr;
    return error;
}

//----------------------------------------------------------------------------
// Get the D/DS value as vector of integer numbers. This is
// used to access value that represent integer vectors. 

int
GenericDS::GetIntVector(vector<int>& value) {
    DOMNode *contentNode;
    string str;
    int intValue;
    
    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode->getFirstChild();
    while (contentNode != 0) {
	if (contentNode->getNodeType() == DOMNode::TEXT_NODE) {
	    str = DOMString_to_string(contentNode->getNodeValue());
	    break;
	}
	contentNode = contentNode->getNextSibling();
    }
    if (contentNode == 0)
	return GenericDS::UNABLE_TO_CONVERT;
    
    char* cStr = new char[str.size() + 1];
    str.copy(cStr, str.size());
    cStr[str.size()] = 0;
    
    int  error = 0;
    char *seps = " \t\n";
    char *token = strtok(cStr, seps);
    while(token != NULL)
    {
	if (sscanf(token,"%d",&intValue) !=1)
	{
	    error = GenericDS::UNABLE_TO_CONVERT;
	    break;
	}
	value.push_back(intValue);    
	token = strtok( NULL, seps );
    }
    delete cStr;
    return error;
}

//----------------------------------------------------------------------------
    //----------- Set functions--------------
    //---------------------------------------

// Set GenericDS value
// Set the D/DS value as an integer.
GenericDS&
GenericDS::SetValue(const int& value) {
    char valueCStr[30];
	XMLCh *xmlStr;
    sprintf(valueCStr,"%d", value);
    string valueStr(valueCStr);
	xmlStr = string_to_DOMString(valueStr);
    m_DOMNode->appendChild(GetDocument()->createTextNode(xmlStr));
	release_DOMString(&xmlStr);
    return *this;
};
  
//----------------------------------------------------------------------------
// Set the D/DS value as a real number.
GenericDS&
GenericDS::SetValue(const double& value) {
    char valueCStr[30];
	XMLCh *xmlStr;
    sprintf(valueCStr,"%g", value);
    string valueStr(valueCStr);
	xmlStr = string_to_DOMString(valueStr);
    m_DOMNode->appendChild(GetDocument()->createTextNode(xmlStr));
	release_DOMString(&xmlStr);
    return *this;
};
    
//----------------------------------------------------------------------------
// Set the D/DS value as text.
GenericDS&
GenericDS::SetValue(const string& value) {
	XMLCh *xmlStr = string_to_DOMString(value);
    m_DOMNode->appendChild(GetDocument()->createTextNode(xmlStr));
	release_DOMString(&xmlStr);
    return *this;
};


//----------------------------------------------------------------------------
// Set the D/DS value as vector of real numbers. This is
// used to set value represented real vectors. 
GenericDS&
GenericDS::SetValue(const vector<double>& value) {
    string valueStr;
    char valueCStr[30];
	XMLCh *xmlStr;
    for (unsigned int i=0; i<value.size(); i++) {
	sprintf(valueCStr,"%g", value[i]);
	valueStr += valueCStr;
	if (i < value.size()-1)
	    valueStr += " ";
    }
	xmlStr = string_to_DOMString(valueStr);
    m_DOMNode->appendChild(GetDocument()->createTextNode(xmlStr));
	release_DOMString(&xmlStr);
    return *this;
};

//----------------------------------------------------------------------------
// Set the D/DS value as vector of integer numbers. This is
// used to set value represented as integer vectors. 
GenericDS&
GenericDS::SetValue(const vector<int>& value) {
    string valueStr;
    char valueCStr[30];
	XMLCh *xmlStr;
    for (unsigned int i=0; i<value.size(); i++) {
	sprintf(valueCStr,"%d ", value[i]);
	valueStr += valueCStr;
	if (i < value.size()-1)
	    valueStr += " ";
    }
	xmlStr = string_to_DOMString(valueStr);
    m_DOMNode->appendChild(GetDocument()->createTextNode(xmlStr));
	release_DOMString(&xmlStr);
    return *this;
};

//----------------------------------------------------------------------------
// Set the D/DS value as vector of integer numbers. This is
// used to set value represented as integer vectors. 
GenericDS&
GenericDS::SetValue(const vector<string>& value) {
	XMLCh *xmlStr;
    for (unsigned int i=0; i<value.size(); i++) {
		xmlStr = string_to_DOMString(value[i]);
		m_DOMNode->appendChild(GetDocument()->createTextNode(xmlStr));
		release_DOMString(&xmlStr);
    }
    return *this;
};

//----------------------------------------------------------------------------
// Set attributes
// Set (create if necessary) the specified attribute to the specified
// value.
GenericDS&
GenericDS::SetAttribute(const string& aName, int value) {
    char valueStr[100];
    DOMElement *thisElement;
	XMLCh *xmlStr1, *xmlStr2;
    
    thisElement = (DOMElement*)(m_DOMNode);
    sprintf(valueStr,"%d", value);
	xmlStr1 = string_to_DOMString(aName);
	xmlStr2 = string_to_DOMString(string(valueStr));
    thisElement->setAttribute(xmlStr1, xmlStr2);
	release_DOMString(&xmlStr1);
	release_DOMString(&xmlStr2);
    return *this;
};

//----------------------------------------------------------------------------
GenericDS&
GenericDS::SetAttribute(const string& aName, double value) {
    char valueStr[100];
    DOMElement *thisElement;
	XMLCh *xmlStr1, *xmlStr2;
    
    thisElement = (DOMElement*)(m_DOMNode);
    sprintf(valueStr,"%g", value);
	xmlStr1 = string_to_DOMString(aName);
	xmlStr2 = string_to_DOMString(string(valueStr));
    thisElement->setAttribute(xmlStr1, xmlStr2);
	release_DOMString(&xmlStr1);
	release_DOMString(&xmlStr2);
    return *this;
};

//----------------------------------------------------------------------------
GenericDS&
GenericDS::SetAttribute(const string& aName, string value) {
    DOMElement *thisElement;
	XMLCh *xmlStr1, *xmlStr2;
    
    thisElement = (DOMElement*)(m_DOMNode);
	xmlStr1 = string_to_DOMString(aName);
	xmlStr2 = string_to_DOMString(string(value));
    thisElement->setAttribute(xmlStr1, xmlStr2);
	release_DOMString(&xmlStr1);
	release_DOMString(&xmlStr2);
    return *this;
};

//----------------------------------------------------------------------------
    // Remove the specified attribute from this node, if it exists.
void
GenericDS::RemoveAttribute(const string& aName) {
    DOMElement *thisElement;
	XMLCh *xmlStr;
    
    thisElement = (DOMElement*)(m_DOMNode);
	xmlStr = string_to_DOMString(aName);
    thisElement->removeAttribute(xmlStr);
	release_DOMString(&xmlStr);
};    
	
//----------------------------------------------------------------------------
//----------- Tree traversal-------------
//---------------------------------------

// Note: In this API cases, comments 
// and other non-essential nodes are ignored and not consider
// to exist as siblings or children. This is different
// from the DOM approach, which exposes more information.

//----------------------------------------------------------------------------
    // Return the index-th occurrence of the named value of
    // the named D or DS among the direct
    // direct children of this node.
GenericDS
GenericDS::GetDescription(const string& name, int index) {
    int DionCounter;
    DOMNode *currentNode = m_DOMNode->getFirstChild();
    
    DionCounter = 0;
    while (currentNode != 0) {
	if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode->getNodeName()) == name) {
		if (DionCounter == index) {
			return GenericDS(currentNode);
		    break;
		}
		DionCounter++;
	    }
	}
	currentNode = currentNode->getNextSibling();
    }
    return GenericDS(0);
};

//----------------------------------------------------------------------------
    // Get the next sibling of this node of the named D or DS.
GenericDS
GenericDS::GetNextSibling(const string& name) {
	DOMNode *currentNode = m_DOMNode->getNextSibling();
	while (currentNode != 0) {
	  if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode->getNodeName()) == name) {
			return GenericDS(currentNode);
	    }
	  }
	  currentNode = currentNode->getNextSibling();
	}
	return GenericDS(0);
};

//----------------------------------------------------------------------------
    // Get the next sibling of this node of the named D or DS.
GenericDS GenericDS::LocateNode(const string& name) {
	DOMNode *currentNode = m_DOMNode;
	DOMNode *nextNode;
	while (currentNode != 0) {
		nextNode=currentNode->getFirstChild();
		if (nextNode != 0) {
			currentNode=nextNode;
		}
		else {
			nextNode=currentNode->getNextSibling();
			if (nextNode != 0) {
				currentNode=nextNode;
			}
			else {
				nextNode=currentNode->getParentNode();
				while (nextNode != 0) {
					currentNode=nextNode->getNextSibling();
					if (currentNode == 0) {
						nextNode=nextNode->getParentNode();
					}
					else {
						break;
					}
				}
				if (nextNode == 0) {
					return nextNode;
				}
				if (currentNode == 0) {
					return currentNode;
				}

			}
		} 
		if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			if (DOMString_to_LocalName(currentNode->getNodeName()) == name) {
				return GenericDS(currentNode);
			}
		}
	}
	return GenericDS();
};

//----------------------------------------------------------------------------
    // Return the occurrence of the named value of the named descriptor
    // designated by the path. The path is a sequence of string labels
    // that designates a path to the node. 
    // For example, 
    // <a>
    //    <b>
    //       <c>
    //			<d>
    //			</d>			
    //			<d>
    //       </c>
    //    </b>
    // </a>
    // From the node "a", the path <b, c, d> refers to the 
    // <d> node above. All possible nodes referable by the 
    // path are returned.
vector<GenericDS>
GenericDS::GetDescription(const vector<string>& path) {  
    vector<GenericDS> pathDSVector(0);
    unsigned int i;
    DOMNode *currentNode;
    
    currentNode = m_DOMNode->getFirstChild();
    for (i=0; i<(path.size())-1; i++) {
	while (currentNode != 0) {
	    if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
		if (path[i] == DOMString_to_LocalName(currentNode->getNodeName())) 
		    break;
	    }
	    currentNode = currentNode->getNextSibling();
	}
	if (currentNode == 0) return pathDSVector; // No such path
	currentNode = currentNode->getFirstChild();
    };
    // Arrived at the last path element
    while (currentNode != 0) {
	if (path[i] == DOMString_to_LocalName(currentNode->getNodeName())) {
	    pathDSVector.push_back(GenericDS(currentNode));
	}
	currentNode = currentNode->getNextSibling();
    }
    return pathDSVector;
};

//----------------------------------------------------------------------------
    // Return a list of all descriptors with the given name that are
    // direct children of this node.
vector<GenericDS>
GenericDS::GetAllDescriptions(const string& name) {  
    vector<GenericDS> newGenericDSVector(0);
    int DionCounter;
    DOMNode *currentNode;
    
    DionCounter = 0;
    currentNode = m_DOMNode->getFirstChild();
    while (currentNode != 0) {
	if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode->getNodeName()) == name) {
		newGenericDSVector.push_back(GenericDS(currentNode));
	    }
	}
	currentNode = currentNode->getNextSibling();
    }
    return newGenericDSVector;
};  

//----------------------------------------------------------------------------
// Return the "root" of the description for this node.
// In XML terms, this correspond to the document.
GenericDS
GenericDS::GetRoot() {
    return GenericDS(GetDocument());
};

//----------------------------------------------------------------------------
// Get the first child of this node. 
GenericDS
GenericDS::GetFirstChild() {
    DOMNode *currentNode = m_DOMNode->getFirstChild();
    while (currentNode != 0) {
	if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) 
	    break;
	currentNode = currentNode->getNextSibling();
    }
    return GenericDS(currentNode);
}


//----------------------------------------------------------------------------
    // Get the next sibling of this node.
GenericDS
GenericDS::GetNextSibling() {
	DOMNode *currentNode = m_DOMNode->getNextSibling();
	while (currentNode != 0) {
	    if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) 
			return GenericDS(currentNode);
	    currentNode = currentNode->getNextSibling();
	}
	return GenericDS();
};

//----------------------------------------------------------------------------
    // Get the nth child of this node. Children are numbered from zero.
GenericDS
GenericDS::GetChild(int index) {
    int nodeCounter;
    
    DOMNode *currentNode;
    nodeCounter = 0;
    currentNode = m_DOMNode->getFirstChild();
    while (currentNode != 0) {
	if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
	    if (nodeCounter == index) {
		return GenericDS(currentNode);
	    }
	    nodeCounter++;
	}
	currentNode = currentNode->getNextSibling();
    }
    // Current node is null
    return GenericDS();
};

//----------------------------------------------------------------------------
    // Get the nth child of this node. Children are numbered from zero.
GenericDS
GenericDS::GetChild(const string& name) {
    int nodeCounter;
    
    DOMNode *currentNode;
    nodeCounter = 0;
    currentNode = m_DOMNode->getFirstChild();
    while (currentNode != 0) {
      if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
	if (DOMString_to_LocalName(currentNode->getNodeName()) == name) {
	  return GenericDS(currentNode);
	}
      }
      currentNode = currentNode->getNextSibling();
    }
    // Current node is null
    return GenericDS();
};

//----------------------------------------------------------------------------
// Get the parent of this node.
GenericDS
GenericDS::GetParent() {
	DOMNode *currentNode = m_DOMNode->getParentNode();
	while (currentNode != 0) {
		if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			return GenericDS(currentNode);
		}
		currentNode = currentNode->getParentNode();
	}
	return GenericDS();
};    

//----------------------------------------------------------------------------
// Return the number of children for this node.
int
GenericDS::GetNumberOfChildren() {
	//return m_DOMNode.getChildNodes().getLength();

	int i = 0;	// Ana's
	while (!this->GetChild(i).isNull()) { i++; }
	return i;
};


//----------------------------------------------------------------------------
//----------- Tree modification----------
//---------------------------------------
// Tree modification operations, the same as the corresponding
// DOM operations. 

// Inserts the node newChild before the existing child node refChild 
GenericDS 
GenericDS::InsertBefore (const GenericDS &newChild,
				  const GenericDS &refChild) {
	return GenericDS(m_DOMNode->insertBefore(newChild.m_DOMNode, refChild.m_DOMNode));
};

//----------------------------------------------------------------------------
// Replaces the child node oldChild with newChild in the list of children,
// and returns the oldChild node 
GenericDS 
GenericDS::ReplaceChild (const GenericDS &newChild,
				  const GenericDS &oldChild) {
	return GenericDS(m_DOMNode->replaceChild(newChild.m_DOMNode, oldChild.m_DOMNode));
};

//----------------------------------------------------------------------------
// Removes the child node indicated by oldChild from the list of children,
// and returns it 
GenericDS
GenericDS::RemoveChild (const GenericDS &oldChild) {
	return GenericDS(m_DOMNode->removeChild(oldChild.m_DOMNode));
};

//----------------------------------------------------------------------------
// Adds the node newChild to the end of the list of children of this node 
GenericDS
GenericDS::AppendChild (const GenericDS &newChild) {
	return GenericDS(m_DOMNode->appendChild(newChild.m_DOMNode));
};

//----------------------------------------------------------------------------
GenericDS
GenericDS::CreateDescriptionRoot() {
	XMLCh *istr;
	istr = XMLString::transcode("Core");
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(istr);
	XMLString::release(&istr);
	
	if (impl != NULL)
	{
		DOMNode *doc= impl->createDocument();

		if(doc)
		{
			return GenericDS(doc);
		}
	}
	return GenericDS();
};

//----------------------------------------------------------------------------
GenericDS
GenericDS::CreateDescriptionRoot(const DOMDocument* doc) {
	return GenericDS((DOMNode*)doc);
};

//----------------------------------------------------------------------------
// Creation of new elements
// Create a new descriptor as the last child of the specified parent.
GenericDS
GenericDS::CreateDescription(GenericDS &root,const string& name) {
	XMLCh *xmlStr = string_to_DOMString(name);
	DOMElement *element = root.GetDocument()->createElement(xmlStr);
	release_DOMString(&xmlStr);
	return GenericDS(element);
}

//----------------------------------------------------------------------------
// Create a new description scheme as the last child of the specified parent.
// Create a new description scheme as the last child of the specified parent.
GenericDS
GenericDS::CreateChild(const string& name) {
	XMLCh *xmlStr = string_to_DOMString(name);
	DOMElement *element = GetDocument()->createElement(xmlStr);
	release_DOMString(&xmlStr);
	m_DOMNode->appendChild(element);
	return GenericDS(element);
};

//----------------------------------------------------------------------------
GenericDS
GenericDS::CreateChild(GenericDS& parent,
				const string& name) {
	XMLCh *xmlStr = string_to_DOMString(name);
    DOMElement *element=parent.GetDocument()->createElement(xmlStr);
	release_DOMString(&xmlStr);
	parent.m_DOMNode->appendChild(element);
    return GenericDS(element);
};

//----------------------------------------------------------------------------
DOMDocument *GenericDS::GetDocument()
{
    // Returning the owner document of a document itself 
    // return null. So check to handle the special case 
    // when the document is null.
    if (m_DOMNode->getNodeType() == DOMNode::DOCUMENT_NODE)
		return (DOMDocument*)m_DOMNode;
    else
		return m_DOMNode->getOwnerDocument();
}

//----------------------------------------------------------------------------
DOMNode *GenericDS::GetDOMNode() {
    return m_DOMNode;
}

//----------------------------------------------------------------------------
void GenericDS::SetDOMNode(DOMNode *node)
{
	// Note: this should not be used to make a copy of a DOM document from another GenericDS - for that purpose use
	// function SetDocument(...) instead.
	releaseDocument();
	m_DOMNode = node;
	if(m_DOMNode && (m_DOMNode->getNodeType() == DOMNode::DOCUMENT_NODE))
	{
		m_DocRef = new DOMDocumentRef();
		m_DocRef->addRef();
	}
}

void GenericDS::releaseDocument()
{
	if(m_DocRef && m_DOMNode)
	{
		if(m_DOMNode->getNodeType() == DOMNode::DOCUMENT_NODE)
		{
			bool r = m_DocRef->releaseRef();
			if(r)
			{
				m_DOMNode->release();
				m_DOMNode = 0;
				delete m_DocRef;
				m_DocRef = 0;
			}
		}
	}
}

void GenericDS::SetDocument(GenericDS &other)
{
	if(other.m_DocRef && other.m_DOMNode)
	{
		releaseDocument();
		m_DocRef = other.m_DocRef;
		m_DOMNode = other.GetDocument();
		m_DocRef->addRef();
	}	
}

//----------------------------------------------------------------------------
GenericDS::GenericDS(DOMNode * Node) 
	:
m_Interface(this),
m_isProprietary(false)
{
	m_DocRef = 0;
	m_DOMNode = Node;
	if(m_DOMNode && m_DOMNode->getNodeType() == DOMNode::DOCUMENT_NODE)
	{
		m_DocRef = new DOMDocumentRef();
		m_DocRef->addRef();
	}
}

//----------------------------------------------------------------------------
GenericDS& GenericDS::operator=(const GenericDS& rhs)
{
	if (this == &rhs)
		return *this;

	// Call the superclasses operator
	DescriptionScheme::operator=(rhs);
	releaseDocument();
	m_DOMNode =	rhs.m_DOMNode;
	m_DocRef = rhs.m_DocRef;
	if(m_DOMNode && m_DocRef)
	{
		m_DocRef->addRef();
	}

	return *this;
}

//----------------------------------------------------------------------------
bool GenericDS::operator==(const GenericDS& rhs)
{
	// This needs to be checked.
	if(this->m_DOMNode->isEqualNode(rhs.m_DOMNode))
	//if(this->m_DOMNode->isSameNode(rhs.m_DOMNode))
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------
bool GenericDS::operator!=(const GenericDS& rhs)
{
	if(this->m_DOMNode->isEqualNode(rhs.m_DOMNode))
	//if(this->m_DOMNode->isSameNode(rhs.m_DOMNode))
		return false;
	else
		return true;
}
#else
//----------------------------------------------------------------------------
#ifdef USEXML
GenericDS GenericDSInterface::CreateDescriptionRoot(const DOM_Document& doc)
{
        return m_GenericDS->CreateDescriptionRoot(doc);
}
#endif /* USEXML */	

//=============================================================================
GenericDS::GenericDS():
m_Interface(this),
m_isProprietary(false)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GenericDS::~GenericDS()
{
}

//----------------------------------------------------------------------------
const UUID& GenericDS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GenericDS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GenericDS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GenericDS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GenericDS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
GenericDSInterfaceABC *GenericDS::GetInterface(void)
{ return &m_Interface; }


//----------------------------------------------------------------------------
#ifdef USEXML
#	ifdef OLDGENERICDS

DOM_Document GenericDS::GetDOMDocument(void)
{
	return GetDocument();
	//	m_DOMDoc;
}

//----------------------------------------------------------------------------
unsigned long GenericDS::SetDOMDocument(DOM_Document DOMDoc)
{
	m_DOMDoc = DOMDoc;
	return 0;
}
#	endif //OLDGENERICDS
#endif /* USEXML */

//----------------------------------------------------------------------------
bool
GenericDS::isNull() {
#ifdef USEXML
    return m_DOMNode.isNull();
#else
    return 0;
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    //----------- Get functions--------------
    //---------------------------------------

    // Returns the name of the descriptor or description scheme.
    // This is the same as the name of the XML tag.
string
GenericDS::GetDSName() {
#ifdef USEXML
	return DOMString_to_string(m_DOMNode->getNodeName());
#else
	return string("");
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Attribute access.
    // Gets all the attribute names
vector<string>
GenericDS::GetAttributeNames() {
    vector<string> attrNames(0);
#ifdef USEXML
    DOM_NamedNodeMap attrMap;
    DOM_Node  attrNode;

	attrMap = m_DOMNode.getAttributes();
	if (attrMap != (DOM_NullPtr*)0) {
	    for (unsigned int i=0; i<attrMap.getLength(); i++)
			attrNames.push_back(DOMString_to_string(attrMap.item(i).getNodeName()));
	}
#endif // USEXML
    return attrNames;
};

//----------------------------------------------------------------------------
    // For all these methods, it is an error to reference a non-integer or non-existent attribute.
    // If an error occurs, the a non-zero return code is return.
    // Return the value of the named attribute as an integer.
int
GenericDS::GetIntAttribute(const string& attrName,
				    int&  value) {
#ifdef USEXML
    DOM_Node  attrNode;
    string str;
    double testValue;

    attrNode = m_DOMNode.getAttributes().getNamedItem(string_to_DOMString(attrName));
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    str = DOMString_to_string(attrNode.getNodeValue());
    if (sscanf(str.c_str(),"%d", &value) !=1 ) return UNABLE_TO_CONVERT;
    sscanf(str.c_str(),"%le", &testValue);
    if (testValue-value != 0.0) return UNABLE_TO_CONVERT_IS_REAL;
    return GenericDS::SUCCESS; // Successfull convertion
#else
    return GenericDS::NO_SUCH_ATTR;
#endif /* USEXML */
};
  
//----------------------------------------------------------------------------
    // Return the value of the named attribute as areal value.
int
GenericDS::GetRealAttribute(const string& attrName,
					double& value) {
#ifdef USEXML
    DOM_Node  attrNode;
    string str;

    attrNode = m_DOMNode.getAttributes().getNamedItem(string_to_DOMString(attrName));
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    str = DOMString_to_string(attrNode.getNodeValue());
    if (sscanf(str.c_str(),"%le",&value) !=1)
	return GenericDS::UNABLE_TO_CONVERT;
    return 0; // Successfull convertion
#else
    return GenericDS::NO_SUCH_ATTR;
#endif /* USEXML */
};  

//----------------------------------------------------------------------------
    // Return the value of the named attribute as a string.
int
GenericDS::GetTextAttribute(const string& attrName, string& value) {
#ifdef USEXML
    DOM_Node   attrNode;
    
    attrNode = m_DOMNode.getAttributes().getNamedItem(string_to_DOMString(attrName));
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    value = DOMString_to_string(attrNode.getNodeValue());
    return 0; // Successfull convertion
#else
    return GenericDS::NO_SUCH_ATTR;
#endif /* USEXML */
};   

//----------------------------------------------------------------------------
    // Return the value of the named attribute as a boolean.
int
GenericDS::GetBoolAttribute(const string& attrName, bool& value) {
#ifdef USEXML
    DOM_Node  attrNode;
    string str;

    attrNode = m_DOMNode.getAttributes().getNamedItem(string_to_DOMString(attrName));
    if (attrNode == NULL) return GenericDS::NO_SUCH_ATTR;
    str = DOMString_to_string(attrNode.getNodeValue());
    if ((str == "true") || (str == "TRUE") || (str == "True") || (str == "1")) {
	value = true;
	return 0;
    }
    if ((str == "false") || (str == "FALSE") || (str == "False")|| (str == "0")) {
	value = false;
	return 0;
    }
    // Unsuccessfull convertion`
    return GenericDS::UNABLE_TO_CONVERT;
#else
    return GenericDS::NO_SUCH_ATTR;
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Get the value of this descriptor or description scheme.
    // The value is the text enclosed between the tags 
    // representing the descriptor
    // For example <ControlledTerm>term</ControlledTerm> the value
    // refers to "term".
    // If the value can not be returned, an non-zero error code if returned.
    
    // Get the D/DS value as an integer.
int
GenericDS::GetIntValue(int& value) {
#ifdef USEXML
    DOM_Node contentNode;
    string str;

    if (m_DOMNode == NULL) return 0;
    //    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode.getFirstChild();
    while (contentNode != (DOM_NullPtr*)0) {
	if (contentNode.getNodeType() == DOM_Node::TEXT_NODE) {
	    str = DOMString_to_string(contentNode.getNodeValue());
	    if (sscanf(str.c_str(),"%d",&value) !=1)
		return GenericDS::UNABLE_TO_CONVERT;
	    return 0; // Successfull convertion
	}
	contentNode = contentNode.getNextSibling();		// Ana's
    }
    return GenericDS::NO_CONTENT;
#else
    return GenericDS::NO_CONTENT;
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Get the D/DS value as a real number.
int
GenericDS::GetRealValue(double& value) {
#ifdef USEXML
    DOM_Node contentNode;
    string str;
    
    if (m_DOMNode == NULL) return 0;
    //    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode.getFirstChild();
    while (contentNode != (DOM_NullPtr*)0) {
	if (contentNode.getNodeType() == DOM_Node::TEXT_NODE) {
	    str = DOMString_to_string(contentNode.getNodeValue());
	    if (sscanf(str.c_str(),"%le", &value) !=1)
		return GenericDS::UNABLE_TO_CONVERT;
	    return 0; // Successfull convertion
	}
	contentNode = contentNode.getNextSibling();		// Ana's
    }
    return GenericDS::NO_CONTENT;
#else
    return GenericDS::NO_CONTENT;
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Get the D/DS value as text.
int
GenericDS::GetTextValue(string& value) {
#ifdef USEXML
    DOM_Node contentNode;
    
    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode.getFirstChild();
    while (contentNode != (DOM_NullPtr*)0) {
	if (contentNode.getNodeType() == DOM_Node::TEXT_NODE) {
	    value = DOMString_to_string(contentNode.getNodeValue());
	    return 0;
		}
	contentNode = contentNode.getNextSibling();		// Ana's
    }
    return GenericDS::NO_CONTENT;
#else
    return GenericDS::NO_CONTENT;
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Get the D/DS value as vector of real numbers. This is
// used to access value that represent real vectors. 
int
GenericDS::GetRealVector(vector<double>& value) {
#ifdef USEXML
    DOM_Node contentNode;
    string str;
    double realValue;
    
    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode.getFirstChild();
    while (contentNode != (DOM_NullPtr*)0) {
	if (contentNode.getNodeType() == DOM_Node::TEXT_NODE) {
	    str = DOMString_to_string(contentNode.getNodeValue());
	    break;
	}
	contentNode = contentNode.getNextSibling();		// Ana's
    }
    if (contentNode.isNull())
	return GenericDS::UNABLE_TO_CONVERT;
    
    char* cStr = new char[str.size() + 1];
    str.copy(cStr, str.size());
    cStr[str.size()] = 0;
    
    int  error = 0;
    char *seps = " \t\n";
    char *token = strtok(cStr, seps);
    while(token != NULL)
    {
	if (sscanf(token,"%le",&realValue) !=1)
	{
	    error = GenericDS::UNABLE_TO_CONVERT;
	    break;
	}
	value.push_back(realValue);    
	token = strtok( NULL, seps );
    }
    delete cStr;
    return error;
#else
    return GenericDS::NO_CONTENT;
#endif /* USEXML */
}

//----------------------------------------------------------------------------
// Get the D/DS value as vector of integer numbers. This is
// used to access value that represent integer vectors. 

int
GenericDS::GetIntVector(vector<int>& value) {
#ifdef USEXML
    DOM_Node contentNode;
    string str;
    int intValue;
    
    if (m_DOMNode == NULL) return GenericDS::NODE_IS_NULL;
    contentNode = m_DOMNode.getFirstChild();
    while (contentNode != (DOM_NullPtr*)0) {
	if (contentNode.getNodeType() == DOM_Node::TEXT_NODE) {
	    str = DOMString_to_string(contentNode.getNodeValue());
	    break;
	}
	contentNode = contentNode.getNextSibling();		// Ana's
    }
    if (contentNode.isNull())
	return GenericDS::UNABLE_TO_CONVERT;
    
    char* cStr = new char[str.size() + 1];
    str.copy(cStr, str.size());
    cStr[str.size()] = 0;
    
    int  error = 0;
    char *seps = " \t\n";
    char *token = strtok(cStr, seps);
    while(token != NULL)
    {
	if (sscanf(token,"%d",&intValue) !=1)
	{
	    error = GenericDS::UNABLE_TO_CONVERT;
	    break;
	}
	value.push_back(intValue);    
	token = strtok( NULL, seps );
    }
    delete cStr;
    return error;
#else
    return GenericDS::NO_CONTENT;
#endif /* USEXML */
}

//----------------------------------------------------------------------------
    //----------- Set functions--------------
    //---------------------------------------

// Set GenericDS value
// Set the D/DS value as an integer.
GenericDS&
GenericDS::SetValue(const int& value) {
#ifdef USEXML
    char valueCStr[30];
    sprintf(valueCStr,"%d", value);
    string valueStr(valueCStr);
    m_DOMNode.appendChild(GetDocument().createTextNode(string_to_DOMString(valueStr)));
#endif /* USEXML */
    return *this;
};
  
//----------------------------------------------------------------------------
// Set the D/DS value as a real number.
GenericDS&
GenericDS::SetValue(const double& value) {
#ifdef USEXML
    char valueCStr[30];
    sprintf(valueCStr,"%g", value);
    string valueStr(valueCStr);
    m_DOMNode.appendChild(GetDocument().createTextNode(string_to_DOMString(valueStr)));
#endif /* USEXML */
    return *this;
};
    
//----------------------------------------------------------------------------
// Set the D/DS value as text.
GenericDS&
GenericDS::SetValue(const string& value) {
#ifdef USEXML
    m_DOMNode.appendChild(GetDocument().createTextNode(string_to_DOMString(value)));
#endif /* USEXML */
    return *this;
};


//----------------------------------------------------------------------------
// Set the D/DS value as vector of real numbers. This is
// used to set value represented real vectors. 
GenericDS&
GenericDS::SetValue(const vector<double>& value) {
#ifdef USEXML
    string valueStr;
    char valueCStr[30];
    for (unsigned int i=0; i<value.size(); i++) {
	sprintf(valueCStr,"%g", value[i]);
	valueStr += valueCStr;
	if (i < value.size()-1)
	    valueStr += " ";
    }
    m_DOMNode.appendChild(GetDocument().createTextNode(string_to_DOMString(valueStr)));
#endif /* USEXML */
    return *this;
};

//----------------------------------------------------------------------------
// Set the D/DS value as vector of integer numbers. This is
// used to set value represented as integer vectors. 
GenericDS&
GenericDS::SetValue(const vector<int>& value) {
#ifdef USEXML
    string valueStr;
    char valueCStr[30];
    for (unsigned int i=0; i<value.size(); i++) {
	sprintf(valueCStr,"%d ", value[i]);
	valueStr += valueCStr;
	if (i < value.size()-1)
	    valueStr += " ";
    }
    m_DOMNode.appendChild(GetDocument().createTextNode(string_to_DOMString(valueStr)));
#endif /* USEXML */
    return *this;
};

//----------------------------------------------------------------------------
// Set the D/DS value as vector of integer numbers. This is
// used to set value represented as integer vectors. 
GenericDS&
GenericDS::SetValue(const vector<string>& value) {
#ifdef USEXML
    for (unsigned int i=0; i<value.size(); i++) {
	m_DOMNode.appendChild(GetDocument().createTextNode(string_to_DOMString(value[i])));
    }
#endif /* USEXML */
    return *this;
};

//----------------------------------------------------------------------------
// Set attributes
// Set (create if necessary) the specified attribute to the specified
// value.
GenericDS&
GenericDS::SetAttribute(const string& aName, int value) {
#ifdef USEXML
    char valueStr[100];
    DOM_Element *thisElement;
    
    thisElement = (DOM_Element*)(&m_DOMNode);
    sprintf(valueStr,"%d", value);
    thisElement->setAttribute(string_to_DOMString(aName),
			      string_to_DOMString(string(valueStr)));
#endif /* USEXML */
    return *this;
};

//----------------------------------------------------------------------------
GenericDS&
GenericDS::SetAttribute(const string& aName, double value) {
#ifdef USEXML
    char valueStr[100];
    DOM_Element *thisElement;
    
    thisElement = (DOM_Element*)(&m_DOMNode);
    sprintf(valueStr,"%g", value);
    thisElement->setAttribute(string_to_DOMString(aName),
			      string_to_DOMString(string(valueStr)));
#endif /* USEXML */
    return *this;
};

//----------------------------------------------------------------------------
GenericDS&
GenericDS::SetAttribute(const string& aName, string value) {
#ifdef USEXML
    DOM_Element *thisElement;
    
    thisElement = (DOM_Element*)(&m_DOMNode);
    thisElement->setAttribute(string_to_DOMString(aName),
			      string_to_DOMString(string(value)));
#endif /* USEXML */
    return *this;
};

//----------------------------------------------------------------------------
    // Remove the specified attribute from this node, if it exists.
void
GenericDS::RemoveAttribute(const string& aName) {
#ifdef USEXML
    DOM_Element *thisElement;
    
    thisElement = (DOM_Element*)(&m_DOMNode);
    thisElement->removeAttribute(string_to_DOMString(aName));
#endif /* USEXML */
};    
	
//----------------------------------------------------------------------------
//----------- Tree traversal-------------
//---------------------------------------

// Note: In this API cases, comments 
// and other non-essential nodes are ignored and not consider
// to exist as siblings or children. This is different
// from the DOM approach, which exposes more information.

//----------------------------------------------------------------------------
    // Return the index-th occurrence of the named value of
    // the named D or DS among the direct
    // direct children of this node.
GenericDS
GenericDS::GetDescription(const string& name, int index) {
#ifdef USEXML
    int DionCounter;
    DOM_Node currentNode = m_DOMNode.getFirstChild();
    
    DionCounter = 0;
    while (currentNode != (DOM_NullPtr*)0) {
	if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode.getNodeName()) == name) {
		if (DionCounter == index) {
		    break;
		}
		DionCounter++;
	    }
	}
	currentNode = currentNode.getNextSibling();
    }
    return GenericDS(currentNode);
#else
    return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Get the next sibling of this node of the named D or DS.
GenericDS
GenericDS::GetNextSibling(const string& name) {
#ifdef USEXML
	DOM_Node currentNode = m_DOMNode.getNextSibling();
	while (currentNode != (DOM_NullPtr*)0) {
	  if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode.getNodeName()) == name) {
			return GenericDS(currentNode);
	    }
	  }
	  currentNode = currentNode.getNextSibling();
	}
	return GenericDS(currentNode);
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Get the next sibling of this node of the named D or DS.
GenericDS
GenericDS::LocateNode(const string& name) {
#ifdef USEXML
	DOM_Node currentNode = m_DOMNode;
	DOM_Node nextNode;
	while (currentNode != (DOM_NullPtr*)0) {
	  nextNode=currentNode.getFirstChild();
	  if (nextNode != (DOM_NullPtr*)0) {
	    currentNode=nextNode;
	  }
	  else {
	    nextNode=currentNode.getNextSibling();
	    if (nextNode != (DOM_NullPtr*)0) {
	      currentNode=nextNode;
	    }
	    else {
	      nextNode=currentNode.getParentNode();
	      while (nextNode != (DOM_NullPtr*)0) {
		currentNode=nextNode.getNextSibling();
		if (currentNode == (DOM_NullPtr*)0) {
		  nextNode=nextNode.getParentNode();
		}
		else {
		  break;
		}
	      }
	      if (nextNode == (DOM_NullPtr*)0) {
		return nextNode;
	      }
	      if (currentNode == (DOM_NullPtr*)0) {
		return currentNode;
	      }
	  	     
	    }
	  } 
	  if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode.getNodeName()) == name) {
	      return GenericDS(currentNode);
	    }
	  }
	}
	return GenericDS(currentNode);
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Return the occurrence of the named value of the named descriptor
    // designated by the path. The path is a sequence of string labels
    // that designates a path to the node. 
    // For example, 
    // <a>
    //    <b>
    //       <c>
    //			<d>
    //			</d>			
    //			<d>
    //       </c>
    //    </b>
    // </a>
    // From the node "a", the path <b, c, d> refers to the 
    // <d> node above. All possible nodes referable by the 
    // path are returned.
vector<GenericDS>
GenericDS::GetDescription(const vector<string>& path) {  
    vector<GenericDS> pathDSVector(0);
#ifdef USEXML
    unsigned int i;
    DOM_Node currentNode;
    
    currentNode = m_DOMNode.getFirstChild();
    for (i=0; i<(path.size())-1; i++) {
	while (currentNode != (DOM_NullPtr*)0) {
	    if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
		if (path[i] == DOMString_to_LocalName(currentNode.getNodeName())) 
		    break;
	    }
	    currentNode = currentNode.getNextSibling();
	}
	if (currentNode == (DOM_NullPtr*)0) return pathDSVector; // No such path
	currentNode = currentNode.getFirstChild();
    };
    // Arrived at the last path element
    while (currentNode != (DOM_NullPtr*)0) {
	if (path[i] == DOMString_to_LocalName(currentNode.getNodeName())) {
	    pathDSVector.push_back(GenericDS(currentNode));
	}
	currentNode = currentNode.getNextSibling();
    }
#endif /* USEXML */
    return pathDSVector;
};

//----------------------------------------------------------------------------
    // Return a list of all descriptors with the given name that are
    // direct children of this node.
vector<GenericDS>
GenericDS::GetAllDescriptions(const string& name) {  
    vector<GenericDS> newGenericDSVector(0);
#ifdef USEXML
    int DionCounter;
    DOM_Node currentNode;
    
    DionCounter = 0;
    currentNode = m_DOMNode.getFirstChild();
    while (currentNode != (DOM_NullPtr*)0) {
	if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	    if (DOMString_to_LocalName(currentNode.getNodeName()) == name) {
		newGenericDSVector.push_back(GenericDS(currentNode));
	    }
	}
	currentNode = currentNode.getNextSibling();
    }
#endif /* USEXML */
    return newGenericDSVector;
};  

//----------------------------------------------------------------------------
// Return the "root" of the description for this node.
// In XML terms, this correspond to the document.
GenericDS
GenericDS::GetRoot() {
#ifdef USEXML
    return GenericDS(GetDocument());
#else
    return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Get the first child of this node. 
GenericDS
GenericDS::GetFirstChild() {
#ifdef USEXML
    DOM_Node currentNode = m_DOMNode.getFirstChild();
    while (currentNode != (DOM_NullPtr*)0) {
	if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) 
	    break;
	currentNode = currentNode.getNextSibling();
    }
    return GenericDS(currentNode);
#else
    return GenericDS();
#endif /* USEXML */
}


//----------------------------------------------------------------------------
    // Get the next sibling of this node.
GenericDS
GenericDS::GetNextSibling() {
#ifdef USEXML
	DOM_Node currentNode = m_DOMNode.getNextSibling();
	while (currentNode != (DOM_NullPtr*)0) {
	    if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) 
			return GenericDS(currentNode);
	    currentNode = currentNode.getNextSibling();
	}
	return GenericDS(currentNode);
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Get the nth child of this node. Children are numbered from zero.
GenericDS
GenericDS::GetChild(int index) {
#ifdef USEXML
    int nodeCounter;
    
    DOM_Node currentNode;
    nodeCounter = 0;
    currentNode = m_DOMNode.getFirstChild();
    while (currentNode != (DOM_NullPtr*)0) {
	if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	    if (nodeCounter == index) {
		return GenericDS(currentNode);
	    }
	    nodeCounter++;
	}
	currentNode = currentNode.getNextSibling();
    }
    // Current node is null
    return GenericDS(currentNode);
#else
    return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
    // Get the nth child of this node. Children are numbered from zero.
GenericDS
GenericDS::GetChild(const string& name) {
#ifdef USEXML
    int nodeCounter;
    
    DOM_Node currentNode;
    nodeCounter = 0;
    currentNode = m_DOMNode.getFirstChild();
    while (currentNode != (DOM_NullPtr*)0) {
      if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	if (DOMString_to_LocalName(currentNode.getNodeName()) == name) {
	  return GenericDS(currentNode);
	}
      }
      currentNode = currentNode.getNextSibling();
    }
    // Current node is null
    return GenericDS(currentNode);
#else
    return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Get the parent of this node.
GenericDS
GenericDS::GetParent() {
#ifdef USEXML
    DOM_Node currentNode = m_DOMNode.getParentNode();
    while (currentNode != (DOM_NullPtr*)0) {
      if (currentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
	return GenericDS(currentNode);
      }
      currentNode = currentNode.getParentNode();
    }
    return GenericDS(currentNode);
#else
    return GenericDS();
#endif /* USEXML */
};    

//----------------------------------------------------------------------------
// Return the number of children for this node.
int
GenericDS::GetNumberOfChildren() {
#ifdef USEXML
	//return m_DOMNode.getChildNodes().getLength();

	int i = 0;	// Ana's
	while (!this->GetChild(i).isNull()) { i++; }
	return i;
#else
	return 0;
#endif /* USEXML */
};


//----------------------------------------------------------------------------
//----------- Tree modification----------
//---------------------------------------
// Tree modification operations, the same as the corresponding
// DOM operations. 

// Inserts the node newChild before the existing child node refChild 
GenericDS 
GenericDS::InsertBefore (const GenericDS &newChild,
				  const GenericDS &refChild) {
#ifdef USEXML
	return GenericDS(m_DOMNode.insertBefore(newChild.m_DOMNode, refChild.m_DOMNode));
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Replaces the child node oldChild with newChild in the list of children,
// and returns the oldChild node 
GenericDS 
GenericDS::ReplaceChild (const GenericDS &newChild,
				  const GenericDS &oldChild) {
#ifdef USEXML
	return GenericDS(m_DOMNode.replaceChild(newChild.m_DOMNode, oldChild.m_DOMNode));
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Removes the child node indicated by oldChild from the list of children,
// and returns it 
GenericDS
GenericDS::RemoveChild (const GenericDS &oldChild) {
#ifdef USEXML
	return GenericDS(m_DOMNode.removeChild(oldChild.m_DOMNode));
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
// Adds the node newChild to the end of the list of children of this node 
GenericDS
GenericDS::AppendChild (const GenericDS &newChild) {
#ifdef USEXML
	return GenericDS(m_DOMNode.appendChild(newChild.m_DOMNode));
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
GenericDS
GenericDS::CreateDescriptionRoot() {
#ifdef USEXML
	return GenericDS(DOM_Document::createDocument());
#else
	return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
#ifdef USEXML
GenericDS
GenericDS::CreateDescriptionRoot(const DOM_Document& doc) {
	return GenericDS(doc);
};
#endif /* USEXML */

//----------------------------------------------------------------------------
// Creation of new elements
// Create a new descriptor as the last child of the specified parent.
GenericDS
GenericDS::CreateDescription(GenericDS &root,
				      const string& name) {
#ifdef USEXML
   DOM_Element element(root.GetDocument().createElement(string_to_DOMString(name)));
   return GenericDS(element);
#else
   return GenericDS();
#endif /* USEXML */
}

//----------------------------------------------------------------------------
// Create a new description scheme as the last child of the specified parent.
// Create a new description scheme as the last child of the specified parent.
GenericDS
GenericDS::CreateChild(const string& name) {
#ifdef USEXML
  DOM_Element element(GetDocument().createElement(string_to_DOMString(name)));
  m_DOMNode.appendChild(element);
  return GenericDS(element);
#else
  return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
GenericDS
GenericDS::CreateChild(GenericDS& parent,
				const string& name) {
#ifdef USEXML
    DOM_Element element(parent.GetDocument().createElement(string_to_DOMString(name)));
	parent.m_DOMNode.appendChild(element);
    return GenericDS(element);
#else
    return GenericDS();
#endif /* USEXML */
};

//----------------------------------------------------------------------------
#ifdef USEXML
DOM_Document GenericDS::GetDocument()
{
    // Returning the owner document of a document itself 
    // return null. So check to handle the special case 
    // when the document is null.
    if (m_DOMNode.getNodeType() == DOM_Node::DOCUMENT_NODE)
	return DOM_Document(*(DOM_Document*)&m_DOMNode);
    else
	return m_DOMNode.getOwnerDocument();
}
#endif /* USEXML */

//----------------------------------------------------------------------------
#ifdef USEXML
DOM_Node GenericDS::GetDOMNode() {
    return m_DOMNode;
}
#endif /* USEXML */

//----------------------------------------------------------------------------
#ifdef USEXML	// Ana's
void GenericDS::SetDOMNode(DOM_Node node)
{
	m_DOMNode = node;
}
#endif /* USEXML */

//----------------------------------------------------------------------------
#ifdef USEXML
GenericDS::GenericDS(const DOM_Node& Node) 
    : m_DOMNode(Node),
	m_Interface(this),
	m_isProprietary(false)
{}

#endif /* USEXML */

//----------------------------------------------------------------------------
GenericDS& GenericDS::operator=(const GenericDS& rhs)
{
	if (this == &rhs)
		return *this;

	// Call the superclasses operator
	DescriptionScheme::operator=(rhs);

#ifdef USEXML
	m_DOMNode =	rhs.m_DOMNode;

    #ifdef OLDGENERICDS
	m_DOMDoc = rhs.m_DOMDoc;
    #endif
#endif /* USEXML */	
	return *this;
}

//----------------------------------------------------------------------------
bool GenericDS::operator==(const GenericDS& rhs)
{
#ifdef USEXML
	if(this->m_DOMNode == rhs.m_DOMNode)
		return true;
	else
#endif /* USEXML */
		return false;
}

//----------------------------------------------------------------------------
bool GenericDS::operator!=(const GenericDS& rhs)
{
#ifdef USEXML
	if(this->m_DOMNode == rhs.m_DOMNode)
		return false;
	else
#endif /* USEXML */
		return true;
}

#endif // USEXML3
