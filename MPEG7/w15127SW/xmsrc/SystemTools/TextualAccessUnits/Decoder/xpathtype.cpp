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

#include "StdAfx.h"
#include <iostream>
#include	"xpathtype.hpp"

//===============
// Parses the Navigation Path and 
// stores each individual path in an array navArray
//===============
void XPathType::getNavigationNodes() {
	stepNum  = parsePositions();
	absFlg = true;
	if(stepNum) {	
		navArray = new NavigationPath* [stepNum];
		for(int n = 0; n < stepNum; n++) navArray[n] = 0;
		for(int i = 0, k = stepNum -1; i < stepNum; i++, k--) {
			//int lb = 0, rb = 0;
			int lb[5],rb[5],index=0;
			navArray[k]= new NavigationPath(new char[breakPoint[i].length + 1]);
			(navArray[k]->name)[breakPoint[i].length] = '\0';
			strncpy(navArray[k]->name, static_cast<char*>(c + breakPoint[i].startLocation), breakPoint[i].length);
			for(int n = 0; n < breakPoint[i].length; n++) {
				if(navArray[k]->name[n] == '[') {
					lb[index] = n + 1;
				}
				if(navArray[k]->name[n] == ']') {
					rb[index] = n;
					++index;
				}
				if(k==0 && navArray[k]->name[n] == '@') navArray[k]->attrFlg = true; //assumes attribute is at the end
				if(k==0 && navArray[k]->attrFlg) navArray[k]->name[n] = navArray[k]->name[n + 1];
			}
			/*if(lb > 0 && lb < rb) {
				navArray[k]->name[lb-1] = '\0';
				char* foo = new char [rb - lb + 1];
				foo[rb - lb] = '\0';
				strncpy(foo, static_cast<char*>(navArray[k]->name) + lb, rb - lb);
				navArray[k]->position = atoi(foo) - 1;
				delete [] foo;
			}*/
			if(index>0){
				while(index>0){
					int loop_par = index-1;
					char* foo = new char [rb[loop_par] - lb[loop_par] + 1];
					strncpy(foo, static_cast<char*>(navArray[k]->name) + lb[loop_par], rb[loop_par] - lb[loop_par]+1);
					foo[rb[loop_par] - lb[loop_par]] = '\0';
					char *pdest;
					int result;
					pdest = strstr(foo,"@xsi:type=");
					if(pdest != NULL){
						result = pdest - foo + strlen("@xsi:type=");//pdest - foo +
						navArray[k]->xsitype = (char *)calloc(strlen(foo)-result+1, sizeof(char));
						strncpy(navArray[k]->xsitype, foo+result+1, strlen(foo)-result-2);
						strcpy(navArray[k]->xsitype,getNCName(navArray[k]->xsitype));
					}
					else{
						navArray[k]->position = atoi(foo) - 1;
					}
					delete [] foo;
					index--;
				}
				navArray[k]->name[lb[0]-1] = '\0';
				strcpy(navArray[k]->name,getNCName(navArray[k]->name)); //Not checking for Namespaces in its entirety
			}
		}
		absFlg = true;
		if(!strcmp(navArray[stepNum - 1]->name,".") || !strcmp(navArray[stepNum - 1]->name,"..")) 
			absFlg = false;

	}
}

//============================================================
// Used to traverse through a 
// DOM Document given the navigation path
// This module calls findTargetNode which calls 
// itself recursively to find the target Node
//============================================================
DOM_Node XPathType::getTargetNode(DOM_Node& nd) {
	if(!stepNum) return nd;
	findTargetNode(nd, stepNum);
	return targetNode;
}

void XPathType::findTargetNode(DOM_Node& nd, unsigned int i) {
	while(nd != 0 && i > 0 && !navArray[i-1]->attrFlg) {
		if(!strcmp(".", static_cast<char*>(navArray[i-1]->name))) {
			i--;
			targetNode = nd;
			cout << "\nTarget Node in ./ function: " << targetNode.getNodeName().transcode() << endl << endl; 
			findTargetNode(nd, i);
			break;
		}
		if(!strcmp("..", static_cast<char*>(navArray[i-1]->name))) {
			i--;
			if(i>0)
			if(strcmp("..", static_cast<char*>(navArray[i-1]->name)))
				relFlg=true;
			targetNode = nd.getParentNode();
			cout << "\nTarget Node in function: " << targetNode.getNodeName().transcode() << endl << endl; 
			findTargetNode(targetNode, i);
			break;
		}
		if(relFlg)
		{
			targetNode = nd.getFirstChild();
			relFlg=false;
		}

		char* name = (nd.getNodeName()).transcode();
		if(!strcmp(name, static_cast<char*>(navArray[i-1]->name))) { // do the check for xsi:type information over here
			for(int j = 0; j < navArray[i-1]->position; j++) {
					nd = nd.getNextSibling();
					if(strcmp(nd.getNodeName().transcode(),name))
						j--;
					cout << "\nTarget Node Sibling Sequence: " << nd.getNodeName().transcode() << endl;
					if(nd == 0) {
						cerr << "Error in Syntax" << endl;
						exit(-1);
					}
			}
			if(navArray[i-1]->xsitype != NULL){
				if(navArray[i-1]->position>0){ 
					DOM_Element *test = static_cast<DOM_Element*>(&nd);
					char * xsiname = (test->getAttribute(navArray[i-1]->xsitype)).transcode();
					if( strcmp(navArray[i-1]->xsitype,xsiname) ){
						cout << "Node does not match xsi:type definition" << endl;
						return;
					}
				}
				else{
					while(nd != NULL){
						char *outName = (nd.getNodeName()).transcode();
						DOM_Element *test = static_cast<DOM_Element*>(&nd);
						char * xsiname = (test->getAttribute("xsi:type")).transcode();
						if( !strcmp(navArray[i-1]->xsitype,xsiname) ){
							cout << outName << endl;
							break;
						}
						else
							nd = nd.getNextSibling();
					}
				}
			}
			i--;
			targetNode = nd;
			cout << "\n Final Target Node in Sibling : " << targetNode.getNodeName().transcode() << endl;
			nd = nd.getFirstChild(); // actually ~
			findTargetNode(nd, i); //nd.getFirstChild(),i
			break;
		}
		else {
			nd = nd.getNextSibling();
		}
	}
	return;
}

unsigned int XPathType::parsePositions() {

	int end = strlen(c);
	unsigned int nodeNumber = 0;
	int step = 0;
	while(step < end) {
		step = moveOver(c, step);
		if(step < end) {
			if(nodeNumber >= breakPoint.size()) {
				breakPoint.resize(breakPoint.size() + 10);
			}
			breakPoint[nodeNumber].startLocation = step;
			step = moveTo(c, step);
			breakPoint[nodeNumber].length = step - breakPoint[nodeNumber].startLocation;
			nodeNumber++;
		}
	}
	return nodeNumber;
};


unsigned int XPathType::moveOver(char* path, unsigned int step) {
	
	while((path[step] != '\0') && (path[step] == '/')) {
		step++;
	}
	return step;
};


unsigned int XPathType::moveTo(char* path, unsigned int step) {
	
	while((path[step] != '\0') && (path[step] != '/')) {
		step++;
	}
	return step;
}

char * XPathType::getNCName(char *QName){ // Get a Non-Colonized name from a QName
	int ch = ':';
	char *pdest,*NCName;
	int result;

	pdest = strchr(QName,ch);
	if( pdest == NULL )
		return QName;
	result = pdest-QName+1;
	NCName = (char *)calloc(strlen(QName)-result+1,sizeof(char));
//	int i = strlen(QName);
	strncpy( NCName, QName + result, strlen(QName)-result);
   
	return NCName;
}
