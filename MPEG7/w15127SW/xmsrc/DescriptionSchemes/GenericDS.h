///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Javier Ruiz Hidalgo - University Politechnic of Catalonis
// Ana B. Benitez - Columbia University
// Toby Walker - Sony Co. (Japan)
// Rui Jorge Lopes - Lancaster University, UK
// S. Herrmann - Technische Univeristaet Muenchen, DE
// (contributing organizations names)
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
// Applicable File Name:  Summarization.h
#ifndef __GENERICDS__
#define __GENERICDS__

/*#include "Descriptors/Descriptors.h"*/
/*#include "DescriptionSchemes/DescriptionSchemes.h"*/


// XML Stuff
#ifdef USEXML3
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE
#elif defined(USEXML)
#ifndef XM_XERCES_VERSION
#define XM_XERCES_VERSION 0x0110
#endif
#include <util/PlatformUtils.hpp>
#include <dom/DOM.hpp>
#endif // USEXML3

#include <vector>

#ifndef BOOLEAN
#define BOOLEAN int
#endif

namespace XM
{

//#ifdef WIN32
using std::string;
using std::vector;
//#endif

class GenericDS;

#ifdef USEXML3
// The DOMDocumentRef class adds/releases references to Xerces-C++ DOM documents.
// If last reference is released the DOM document may be deleted.
// It is added when updating GenericDC to new Xerces-C++ version (3.x.x).
class DOMDocumentRef
{
public:
	DOMDocumentRef() { refCount = 0; };

	void addRef() { refCount++; };

	// Returns true if last refeence was released. This means that the related document can be deleted.
	bool releaseRef() { refCount--; return (refCount == 0) ? true : false ; };

private:
	int refCount;
};
#endif // USEXML3

//=============================================================================
class GenericDSInterfaceABC: public I_InterfaceABC
{
public:
    GenericDSInterfaceABC();
    virtual ~GenericDSInterfaceABC() {};

#if defined(USEXML3)
    virtual DOMDocument *GetDocument() = 0;
#elif defined(USEXML)
#ifdef OLDGENERICDS
    virtual DOM_Document GetDOMDocument(void) = 0;
    virtual unsigned long SetDOMDocument(DOM_Document DOMDoc) = 0;
#endif // OLDGENERICDS
    virtual DOM_Document GetDocument() = 0;
#endif // USEXML

    virtual bool isNull() = 0;  
    virtual string GetDSName() = 0;
    virtual vector<string>GetAttributeNames() = 0;
    virtual int GetIntAttribute(const string& aAttrName, int& value) = 0;
    virtual int GetRealAttribute(const string& attrName, double& value) = 0;
    virtual int GetTextAttribute(const string& name, string& value) = 0;
    virtual int GetBoolAttribute(const string& name, bool& value) = 0;
    virtual int GetIntValue(int& value) = 0;
    virtual int GetRealValue(double& value) = 0;
    virtual int GetTextValue(string& value) = 0;
    virtual int GetRealVector(vector<double>& value) = 0;
    virtual int GetIntVector(vector<int>& value) = 0;

    virtual GenericDS& SetAttribute(const string& aName, int value) = 0;
    virtual GenericDS& SetAttribute(const string& aName, double value) = 0;
    virtual GenericDS& SetAttribute(const string& aName, string value) = 0;
    virtual GenericDS& SetValue(const int& value) = 0;
    virtual GenericDS& SetValue(const double& value) = 0;
    virtual GenericDS& SetValue(const string& value) = 0;
    virtual GenericDS& SetValue(const vector<double>& value) = 0;
    virtual GenericDS& SetValue(const vector<int>& value) = 0;
    virtual GenericDS& SetValue(const vector<string>& value) = 0;
    virtual void RemoveAttribute(const string& aName) = 0;

    virtual GenericDS GetDescription(const string& name, int index=0) = 0;
    virtual GenericDS GetNextSibling(const string& name) = 0;
    virtual GenericDS LocateNode(const string& name) = 0;
    virtual vector<GenericDS> GetDescription(const vector<string>& path) = 0;
    virtual vector<GenericDS> GetAllDescriptions(const string& name) = 0;
    virtual GenericDS GetRoot() = 0;
    virtual GenericDS GetFirstChild() = 0;
    virtual GenericDS GetNextSibling() = 0;
    virtual GenericDS GetChild(int index = 0) = 0;
    virtual GenericDS GetChild(const string& name = 0) = 0;
    virtual GenericDS GetParent() = 0;
    virtual int GetNumberOfChildren() = 0;

    virtual GenericDS InsertBefore (const GenericDS &newChild,
				     const GenericDS &refChild) = 0;
    virtual GenericDS ReplaceChild (const GenericDS &newChild,
				     const GenericDS &oldChild) = 0;
    virtual GenericDS RemoveChild (const GenericDS &oldChild) = 0;
    virtual GenericDS AppendChild (const GenericDS &newChild) = 0;
    virtual GenericDS CreateChild(const string& name) = 0;
    virtual GenericDS CreateDescription(GenericDS &root,const string& name)=0;

    virtual GenericDS CreateDescriptionRoot() = 0;
    // From existing DOM document
#ifdef USEXML3
	virtual void SetDocument(GenericDS &other) = 0;
    virtual GenericDS CreateDescriptionRoot(const DOMDocument* doc) = 0;
    virtual void SetDOMNode(DOMNode *node) = 0;
#elif defined(USEXML)
    virtual GenericDS CreateDescriptionRoot(const DOM_Document& doc) = 0;
    virtual void SetDOMNode(DOM_Node node) = 0;
#endif // USEXML3

	static const UUID myID;
};

class GenericDS;

//=============================================================================
class GenericDSInterface: public GenericDSInterfaceABC
{
public:
    GenericDSInterface(GenericDS *bbox);
    virtual ~GenericDSInterface();

    virtual const UUID& GetInterfaceID(void);
    virtual const char *GetName(void);

#ifdef USEXML3
    virtual DOMDocument *GetDocument();
#elif defined(USEXML)
#ifdef OLDGENERICDS
    virtual DOM_Document GetDOMDocument(void);
    virtual unsigned long SetDOMDocument(DOM_Document DOMDoc);
#endif // OLDGENERICDS
    virtual DOM_Document GetDocument();
#endif // USEXML

    virtual bool isNull();  
    virtual string GetDSName();
    virtual vector<string>GetAttributeNames();
    virtual int GetIntAttribute(const string& aAttrName, int& value);
    virtual int GetRealAttribute(const string& attrName, double& value);
    virtual int GetTextAttribute(const string& name, string& value);
    virtual int GetBoolAttribute(const string& name, bool& value);
    virtual int GetIntValue(int& value);
    virtual int GetRealValue(double& value);
    virtual int GetTextValue(string& value);
    virtual int GetRealVector(vector<double>& value);
    virtual int GetIntVector(vector<int>& value);

    virtual GenericDS& SetAttribute(const string& aName, int value);
    virtual GenericDS& SetAttribute(const string& aName, double value);
    virtual GenericDS& SetAttribute(const string& aName, string value);
    virtual GenericDS& SetValue(const int& value);
    virtual GenericDS& SetValue(const double& value);
    virtual GenericDS& SetValue(const string& value);
    virtual GenericDS& SetValue(const vector<double>& value);
    virtual GenericDS& SetValue(const vector<int>& value);
    virtual GenericDS& SetValue(const vector<string>& value);
    virtual void RemoveAttribute(const string& aName);

    virtual GenericDS GetDescription(const string& name, int index=0);
    virtual GenericDS GetNextSibling(const string& name);
    virtual GenericDS LocateNode(const string& name);
    virtual vector<GenericDS> GetDescription(const vector<string>& path);
    virtual vector<GenericDS> GetAllDescriptions(const string& name);
    virtual GenericDS GetRoot();
    virtual GenericDS GetFirstChild();
    virtual GenericDS GetNextSibling();
    virtual GenericDS GetChild(int index = 0);
    virtual GenericDS GetChild(const string& name);
    virtual GenericDS GetParent();
    virtual int GetNumberOfChildren();

    virtual GenericDS InsertBefore (const GenericDS &newChild,
			    const GenericDS &refChild);
    virtual GenericDS ReplaceChild (const GenericDS &newChild,
			    const GenericDS &oldChild);
    virtual GenericDS RemoveChild (const GenericDS &oldChild);
    virtual GenericDS AppendChild (const GenericDS &newChild);
    virtual GenericDS CreateChild(const string& name);
    virtual GenericDS CreateDescription(GenericDS &root,const string& name);

    virtual GenericDS CreateDescriptionRoot();
    // From existing DOM document
#if defined(USEXML3)
	virtual void SetDocument(GenericDS &other);
    virtual GenericDS CreateDescriptionRoot(const DOMDocument* doc);
    virtual void SetDOMNode(DOMNode *node);
#elif defined(USEXML)
    virtual GenericDS CreateDescriptionRoot(const DOM_Document& doc);
    virtual void SetDOMNode(DOM_Node node);
#endif // USEXML3

    static const UUID myID;
    static const char *myName;

private:
    GenericDS *m_GenericDS;
};

//=============================================================================
class GenericDS: public DescriptionScheme
{
public:
    GenericDS();
    virtual ~GenericDS();
  
    // Assignment operator
    GenericDS& operator=(const GenericDS&);
    bool operator==(const GenericDS&);
    bool operator!=(const GenericDS&);

    // actual descriptor methods
    virtual bool IsProprietary(void);
    virtual const UUID& GetValueID(void);
    virtual const char* GetValueName(void);
  
    virtual const UUID& GetObjectID(void);
    virtual const char *GetName(void);
    virtual GenericDSInterfaceABC *GetInterface(void);
  
    // access is allowed only by class factories for this
    // object.  This avoids having to duplicate the
    // ID definition in multiple locations.  In the future, we may
    // have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
    // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
    static const UUID myID;
    static const char * myName;
  
    // Accesssor methods
#if defined(USEXML3)
    virtual DOMDocument *GetDocument();
#elif defined(USEXML)
#ifdef OLDGENERICDS
    virtual DOM_Document GetDOMDocument(void);
    virtual unsigned long SetDOMDocument(DOM_Document DOMDoc);
#endif // OLDGENERICDS
    virtual DOM_Document GetDocument();
#endif // USEXML3

    virtual bool isNull();  
    //----------- Get functions--------------
    //---------------------------------------

    // Returns the name of the descriptor or description scheme.
    // This is the same as the name of the XML tag.
    virtual string GetDSName();

    // Attribute access.
    // Gets all the attribute names
    virtual vector<string>GetAttributeNames();
    // For all these methods, it is an error to reference a non-integer
    // or non-existent attribute.
    // If an error occurs, the a non-zero return code is return.
    // Return the value of the named attribute as an integer.
    virtual int GetIntAttribute(const string& aAttrName, int& value);
    // Return the value of the named attribute as areal value.
    virtual int GetRealAttribute(const string& attrName, double& value);
    // Return the value of the named attribute as a string.
    virtual int GetTextAttribute(const string& name, string& value);
    // Return the value of the named attribute as a boolean.
    virtual int GetBoolAttribute(const string& name, bool& value);

    // Get the value of this descriptor or description scheme.
    // The value is the text enclosed between the tags 
    // representing the descriptor
    // For example <ControlledTerm>term</ControlledTerm> the value
    // refers to "term".
    // If the value can not be returned, an non-zero error code if returned.
    
    // Get the D/DS value as an integer.
    virtual int GetIntValue(int& value);
    // Get the D/DS value as a real number.
    virtual int GetRealValue(double& value);
    // Get the D/DS value as text.
    virtual int GetTextValue(string& value);
    // Get the D/DS value as vector of real numbers. This is
    // used to access value that represent real vectors. 
    virtual int GetRealVector(vector<double>& value);
    // Get the D/DS value as vector of integer numbers. This is
    // used to access value that represent integer vectors. 
    virtual int GetIntVector(vector<int>& value);


    //----------- Set functions--------------
    //---------------------------------------
    

    // Set attributes
    // Set (create if necessary) the specified attribute to the 
    // specified value.
    virtual GenericDS& SetAttribute(const string& aName, int value);
    virtual GenericDS& SetAttribute(const string& aName, double value);
    virtual GenericDS& SetAttribute(const string& aName, string value);

    // Set GenericDS value
    // Set the D/DS value as an integer.
    virtual GenericDS& SetValue(const int& value);
    // Set the D/DS value as a real number.
    virtual GenericDS& SetValue(const double& value);
    // Set the D/DS value as text.
    virtual GenericDS& SetValue(const string& value);
    // Set the D/DS value as vector of real numbers. This is
    // used to set value represented real vectors. 
    virtual GenericDS& SetValue(const vector<double>& value);
    // Set the D/DS value as vector of integer numbers. This is
    // used to Set value represented as integer vectors. 
    virtual GenericDS& SetValue(const vector<int>& value);
    // Set the D/DS value as vector of strings. This is
    // used to set value represented as string vectors. 
    virtual GenericDS& SetValue(const vector<string>& value);

    // Remove the specified attribute from this node, if it exists.
    virtual void RemoveAttribute(const string& aName);
	
    //----------- Tree traversal-------------
    //---------------------------------------

    // Note: In this API cases, comments 
    // and other non-essential nodes are ignored and not consider
    // to exist as siblings or children. This is different
    // from the DOM approach, which exposes more information.

    // Return the index-th occurrence of the named value of the named
    // D or DS among the direct
    // direct children of this node.
    virtual GenericDS GetDescription(const string& name, int index=0);
    // Get the next sibling of this node with given name.
    virtual GenericDS GetNextSibling(const string& name);
    // Get the next node of this with given name ignoring the hierarchie 
    virtual GenericDS LocateNode(const string& name);
    // Return the occurrence of the named value of the named descriptor
    // designated by the path. The path is a sequence of string labels
    // that designates a path to the node. 
    // For example, 
    // <a>
    //    <b>
    //       <c>
    //          <d>
    //          </d>			
    //       </c>
    //    </b>
    // </a>
    // From the node "a", the path <b, c, d> refers to the 
    // <d> node above. All possible nodes referable by the 
    // path are returned.
    virtual vector<GenericDS> GetDescription(const vector<string>& path);

    // Return a list of all descriptors with the given name that are
    // direct children of this node.
    virtual vector<GenericDS> GetAllDescriptions(const string& name);

    // Return the "root" of the description for this node. In XML terms,
    // this correspond to the 
    // document.
    virtual GenericDS GetRoot();

    // Get the first child of this node. 
    virtual GenericDS GetFirstChild();
    // Get the next sibling of this node.
    virtual GenericDS GetNextSibling();
    // Get the nth child of this node. Children are numbered from zero.
    virtual GenericDS GetChild(int index = 0);
    // Get the child node with the given name
    virtual GenericDS GetChild(const string& name);
    // Search from this up to LocalRoot for node with given name
    //virtual GenericDS GetDescendants(GenerigDSInterfaceABC *LocalRoot,
    //const string &name);
    // Get the parent of this node.
    virtual GenericDS GetParent();
    // Return the number of children for this node.
    virtual int GetNumberOfChildren();

    //----------- Tree modification----------
    //---------------------------------------
    // Tree modification operations, the same as the corresponding
    // DOM operations. 
    
    // Inserts the node newChild before the existing child node refChild 
    // Returns the new child
    virtual GenericDS InsertBefore (const GenericDS &newChild,
			    const GenericDS &refChild);
    // Replaces the child node oldChild with newChild in the list of children,
    // Returns the old child
    virtual GenericDS ReplaceChild (const GenericDS &newChild,
			    const GenericDS &oldChild);
    // Removes the child node indicated by oldChild from the list of children
    // Returns the removed child
    virtual GenericDS RemoveChild (const GenericDS &oldChild);
    // Adds the node newChild to the end of the list of children of this node 
    // Returns the new child
    virtual GenericDS AppendChild (const GenericDS &newChild);

    // Creates Document Root
    // From scratch
    static GenericDS CreateDescriptionRoot();
    // From existing DOM document
#if defined(USEXML3)
	void releaseDocument();
	void SetDocument(GenericDS &other);
    static GenericDS CreateDescriptionRoot(const DOMDocument* doc);
    virtual void SetDOMNode(DOMNode *node);
#elif defined(USEXML)
    static GenericDS CreateDescriptionRoot(const DOM_Document& doc);
    virtual void SetDOMNode(DOM_Node node);
#endif // USEXML3

    // Creation of new elements
    // Create a new descriptor as the last child of the specified parent.
    static GenericDS CreateDescription(GenericDS &root,
				       const string& name);
    // Create a new description scheme as the last child of the
    // specified parent.
    virtual GenericDS CreateChild(const string& name);
    static GenericDS CreateChild(GenericDS & parent,
				 const string& name);
    
    // Error codes for GenericDS methods
    enum GenericDSErrorCodes {
	SUCCESS =0,
	NODE_IS_NULL =-1,
	NO_SUCH_ATTR =-2,
	UNABLE_TO_CONVERT =-3,
	UNABLE_TO_CONVERT_IS_REAL =-4,
	NO_CONTENT   =-5
    };

#if defined(USEXML3)
    // The DOM_Node that is wrapped by this object
    DOMNode     *m_DOMNode;
	// The reference counter for XML document.
	DOMDocumentRef *m_DocRef;

    // Obtain the DOM Node wrapped
    //DOM_Node& GetDOMnode();
    virtual DOMNode *GetDOMNode();
#elif defined(USEXML)
    // The DOM_Node that is wrapped by this object
    DOM_Node     m_DOMNode;   

    // Obtain the DOM Node wrapped
    //DOM_Node& GetDOMnode();
    virtual DOM_Node GetDOMNode();
#endif // USEXML3

private:

    GenericDSInterface m_Interface;
	
    const bool m_isProprietary;
    static const char * valName;
    static const UUID valID;

#if defined(USEXML3)
    GenericDS(DOMNode* NodeImpl);
#elif defined(USEXML)
    // actual values:
#	ifdef OLDGENERICDS
    DOM_Document m_DOMDoc;
#	endif //OLDGENERICDS
    GenericDS(const DOM_Node& NodeImpl);
#endif // USEXML3

};

};

#endif //__GENERICDS__
