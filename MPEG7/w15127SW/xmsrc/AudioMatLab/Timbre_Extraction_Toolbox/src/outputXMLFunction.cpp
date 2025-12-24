#include "DOMPrint.hpp"
#include "treeFunction.hpp"

extern "C" {
#include <stdio.h>
}


extern DOM_Document outputDoc;
extern DOM_Document typeDefinitionDoc;
extern DOM_Document originalDoc;
extern DOM_Document mappingDoc;

static XMLCh * gEncodingName = 0;
extern XMLFormatter::UnRepFlags gUnRepFlags;

void callPrint(void)
{
    cout << endl << " ORGINAL DOC " << endl;
    DOMPrintInit(originalDoc, gEncodingName, gUnRepFlags); 

    cout << endl << " MAPPING DOC " << endl;
    DOMPrintInit(mappingDoc, gEncodingName, gUnRepFlags); 

    cout << endl << " TYPEDEFINITION DOC " << endl;
    DOMPrintInit(typeDefinitionDoc, gEncodingName, gUnRepFlags); 

    cout << endl << " OUTPUT DOC " << endl;    
    DOMPrintInit(outputDoc, gEncodingName, gUnRepFlags); 

    return;
}


void callPrintFile(char *outputFile)
{
    DOMPrintToFile(outputDoc, gEncodingName, gUnRepFlags, outputFile);
    return;
}

// Set value attribute of Node (descriptorType -> descriptorName)
int setParamValue(char *descriptorType, char *descriptorName, double descriptorValue)
{
    // Call Method for setting Descriptor

    const DOMString parentType(descriptorType);
    const DOMString childType(descriptorName);
    DOMString myValue;

    myValue = createFromDouble(descriptorValue, 100);

    addThisValueToOutput(typeDefinitionDoc, outputDoc, parentType, childType, myValue);

    return 0;
}
