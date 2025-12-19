/********************************************************************************
//	Program: To Decode a given AccessUnit file into the respective description file.
//	Author : Ali. J. Tabatabai
//  Company: (c) Sony Electronics, Inc.,2001.


This software module was originally developed by Sony Electronics Inc.
("Sony") in the course of development of the MPEG-7 Systems (ISO/IEC
15938-1) standard.

This software module is an implementation of a part of one or more
MPEG-7 Systems (ISO/IEC 15938-1) tools as specified by the MPEG-7 Systems
ISO/IEC 15938-1) standard.

ISO/IEC gives users of the MPEG-7 Systems (ISO/IEC 15938-1) free license to
this software module or modifications thereof solely for testing and
evaluation purposes for use in hardware or software products claiming
conformance to the MPEG-7 Systems (ISO/IEC 15938-1).

Those intending to use this software module in hardware or software products
are advised that its use may infringe existing patents.

The original developer of this software module and his/her company, the
Subsequent editors and their companies, and ISO/IEC have no liability for
use of this software module or modifications thereof in an implementation.
This software module is provided AS IS.

Copyright is not released for non MPEG-7 Systems (ISO/IEC 15938-1)
conforming products.

Sony retains full right to use the code for his/her own purpose, assign or
donate the code to a third party and to inhibit third parties from using the
code for non MPEG-7 Systems (ISO/IEC 15938-1) conforming products.

This copyright notice must be included in all copies or derivative works.


Copyright Sony © 2001.
****************************************************************************/


#ifndef AUXPATHTYPE_HPP
#define AUXPATHTYPE_HPP

#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>

#include <vector>

using namespace std;

struct StepLocation {

	size_t startLocation;
	size_t length;
};

class NavigationPath {

public:

	NavigationPath() : name(0), position(0), attrFlg(false), xsitype(0) {
	}

	NavigationPath(char* c, unsigned int p = 0, bool f = false, char* xs=0) : name(c), position(p), attrFlg(f), xsitype(xs) {
	}

	~NavigationPath() {
		delete name;
	}

	char*   name;
	int     position;
	bool	attrFlg;
	char*	xsitype;
};

typedef vector<StepLocation> VSTEP;


class XPathType {

public:

	XPathType(char* path) : c(path), stepNum(0), absFlg(true), relFlg(false), navArray(0)  {
	}

	~XPathType() {
		for(int i = 0; i < stepNum; i++) delete navArray[i];
	}

	void		getNavigationNodes();
	char*		getAttributeName() const {
				if(navArray && navArray[0]) return navArray[0]->attrFlg ? navArray[0]->name : 0;
				return 0;
				}
	DOM_Node	getTargetNode(DOM_Node&);
	bool		isAbsAddress() const {return absFlg;}
	bool		isRoot() const { return stepNum == 0 ? true : false;}
	bool		isAttributeNode() const {
		        if(navArray && navArray[0]) return navArray[0]->attrFlg ? true : false;
				return false;
				}
	char * getNCName(char *QName);
		
private:

	unsigned int parsePositions();
	unsigned int moveOver(char*, unsigned int);
	unsigned int moveTo(char*, unsigned int);
	void		 findTargetNode(DOM_Node&, unsigned int);

private:

	char*					c;
	DOM_Node				targetNode;
	unsigned int			stepNum;
	bool					absFlg;
	bool					relFlg;
	VSTEP					breakPoint;
	NavigationPath**		navArray;

};

#endif
