/*--------------------------------------------------------------------------------------
 This software module was originally developed by

 EPFL (Yousri Abdeljaoued,abdel@ltssg3.epfl.ch)
 Ericsson Radio Systems AB,(Charilaos Christopoulos, ch.christopoulos@era.ericsson.se)
 (contributing organizations names)
 
 in the course of development of the MPEG-7 Experimentation Model.

 This software module is an implementation of a part of one or more MPEG-7
 Experimentation Model tools as specified by the MPEG-7 Requirements.

 ISO/IEC gives users of MPEG-7 free license to this software module or
 modifications thereof for use in hardware or software products claiming
 conformance to MPEG-7.

 Those intending to use this software module in hardware or software products
 are advised that its use may infringe existing patents. The original
 developers of this software module and their companies, the subsequent
 editors and their companies, and ISO/IEC have no liability for use of this
 software module or modifications thereof in an implementation.
 
 Copyright is not released for non MPEG-7 conforming products. The
 organizations named above retain full right to use the code for their own
 purpose, assign or donate the code to a third party and inhibit third parties
 from using the code for non MPEG-7 conforming products.
 
 Copyright (c) 1998-1999-2000.
 
 This notice must be included in all copies or derivative works.
 
 Applicable File Name:  LinkedList.c

-------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "Utilities/LinkedList.h"

#ifndef FAILURE
#define FAILURE -1
#endif

#ifndef SUCCESS
#define SUCCESS  0
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
/* extern void *malloc (size_t); */
/* extern void free (void *); */

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
Element *elementCreate(void *object)
{
  Element *element;
  element = (Element *) malloc(sizeof(Element));
  if (element == (Element *) NULL) 
  { 
    perror("List.c: elementCreate(): malloc() fails!"); 
    exit(FAILURE); 
  } 
  element->previousElement = (Element *) NULL;
  element->nextElement = (Element *) NULL;
  element->object = object;
  return(element);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void elementDestroy(Element *element)
{
  element->previousElement = (Element *) NULL;
  element->nextElement = (Element *) NULL;
  element->object = (void *)NULL;
  free(element);
  element = (Element *) NULL;  
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void elementInit(Element *element, void *object)
{
  element->object = object;
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int elementTraverseDown(
		      Element *start, 
		      void action(void *object, void *par),
		      void *par)
{
  Element *e;
  e = start;

  while( e != (Element*) NULL)
  {
    action(e->object, par);
    e = e->previousElement;
  }

  return(SUCCESS); 
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int elementTraverseUp(
			Element *start, 
			void action(void *object, void *par),
			void *par)
{
  Element *e;
  e = start;

  while( e != (Element*) NULL)
  {
    action(e->object, par);
    e = e->nextElement;
  }

  return(SUCCESS); 
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int linkedListTraverse(
		 LinkedList *linkedList,
		 void action(void *object, void *par),
		 void *par)

{
  return(elementTraverseUp(linkedList->firstElement, action, par));
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
LinkedList *linkedListCreate(void)
{
  LinkedList *linkedList;
  linkedList = (LinkedList *) malloc(sizeof(LinkedList));
  if (linkedList == (LinkedList *) NULL) 
  { 
    perror("LinkedList.c: linkedListCreate(): malloc() fails!"); 
    exit(FAILURE); 
  } 
  linkedList->firstElement = (Element *)NULL;
  linkedList->lastElement = (Element *)NULL;
  
  return(linkedList);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int linkedListInit(LinkedList *linkedList)
{
  return(SUCCESS);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void linkedListAddFirst(LinkedList *linkedList, void *object)
{
  Element *element = elementCreate(object);

  if(linkedList->firstElement == (Element *)NULL)
  {
    element->previousElement = (Element *)NULL;    
    element->nextElement = (Element *)NULL;    
    linkedList->firstElement = element;
    
    /* linkedList->firstElement = element; (Yousri 25-03-99 should be: */
    linkedList->lastElement = element; 
  }
  else
  {
    Element *e = linkedList->firstElement;
    e->previousElement = element;
    element->nextElement = e;
    element->previousElement = (Element *)NULL;
    linkedList->firstElement = element;
  }
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void linkedListAddLast(LinkedList *linkedList, void *object)
{
  Element *element = elementCreate(object);

  if(linkedList->lastElement == (Element *)NULL)
  {
    element->previousElement = (Element *)NULL;    
    element->nextElement = (Element *)NULL;    
    linkedList->lastElement = element;
    linkedList->firstElement = element;
  }
  else
  {
    Element *e = linkedList->lastElement;
    e->nextElement = element;
    element->previousElement = e;
    element->nextElement = (Element *)NULL;
    linkedList->lastElement = element;
  }
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void *linkedListRemoveFirst(LinkedList *linkedList)
{
  void *object = linkedList->firstElement->object;
  Element *element = linkedList->firstElement;
  
  if (element->nextElement == (Element *)NULL)
    {
      elementDestroy(element);
      linkedList->firstElement = (Element *)NULL;
      linkedList->lastElement = (Element *)NULL;
    }
  else
    {
      linkedList->firstElement = linkedList->firstElement->nextElement;
      element->nextElement->previousElement = (Element *)NULL;
      elementDestroy(element);
    }
  return(object);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void *linkedListRemoveLast(LinkedList* linkedList)
{
  void *object = linkedList->lastElement->object;
  Element *element = linkedList->lastElement;
  
  linkedList->lastElement = linkedList->lastElement->previousElement;
  element->previousElement->nextElement = (Element *)NULL;

  elementDestroy(element);
  return(object);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

int linkedListDestroy(LinkedList *linkedList, void action(void *object, void *par))
{
  
  /*void *object;*/

  while( linkedList->firstElement != (Element*) NULL)
  {
    void *object = linkedListRemoveFirst(linkedList);
    action(object, NULL);
  }


  linkedList->firstElement = (Element *)NULL;
  linkedList->lastElement = (Element *)NULL;
  free(linkedList);
  linkedList = (LinkedList *)NULL;
  return(SUCCESS);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void *linkedListGetFirst(LinkedList *linkedList)
{
  if (!linkedList->firstElement) return 0;
  return(linkedList->firstElement->object);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void *linkedListGetLast(LinkedList *linkedList)
{
  if (!linkedList->firstElement) return 0;
  return(linkedList->lastElement->object);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void *linkedListGetObject(LinkedList *linkedList, int position)
{
  Element *element;

  element=linkedList->firstElement;
  while (position && element) {
    element=element->nextElement;
    position--;
  }
  if (!element) return 0;
  return element->object;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int linkedListCountElement(LinkedList *linkedList)
{
  int c = 0;
  Element *e;
  
  e = linkedList->firstElement;
  while (e != (Element *)NULL)
    {
      c++;
      e = e->nextElement;
    }
  return(c);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
#ifdef LINKEDLIST
void action(void *object, void *par)
{
  printf("%s\n",(char *)object);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int compare(void *object1, void *object2)
{
  return(
    strcmp((char *)object1, (char *)object2)
    );
}
/*-----------------------------------------------------------------------------
gcc -Wall -pedantic -ansi  -D_LONGLONG=1 -DLINKEDLIST LinkedList.c
cc -fullwarn -ansi -DLINKEDLIST LinkedList.c
-----------------------------------------------------------------------------*/
int main(void)
{
  LinkedList *linkedList;
  linkedList = linkedListCreate();
  linkedListAddLast(linkedList, "1");
  linkedListAddLast(linkedList, "77");
  linkedListAddLast(linkedList, "2");
  linkedListAddLast(linkedList, "3");  
  linkedListAddFirst(linkedList, "3");  
  linkedListTraverse(linkedList, action, NULL);

  printf("--------------------\n");
  linkedListRemoveLast(linkedList);
  linkedListRemoveFirst(linkedList);
  linkedListTraverse(linkedList, action, NULL);
  
  linkedListDestroy(linkedList);
  
  return(SUCCESS);
}
#endif
/*-----------------------------------------------------------------------------
####### #     # ######
#       ##    # #     #
#       # #   # #     #
#####   #  #  # #     #
#       #   # # #     #
#       #    ## #     #
####### #     # ######
-----------------------------------------------------------------------------*/





