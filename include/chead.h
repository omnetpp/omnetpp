//==========================================================================
//   CHEAD.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cHead     : head of a list of cObjs
//    cIterator : walks along a list
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CHEAD_H
#define __CHEAD_H

#include "cobject.h"

//=== classes declared here
class  cIterator;
class  cHead;

//=== classes mentioned
class  cModuleInterface;
class  cModuleType;
class  cLinkType;
class  cFunctionType;
class  cNetworkType;
class  cInspectorFactory;

//==========================================================================
// cHead : head of a cObject chain
//    NOTE: CANNOT dup() itself! (dup() only creates a cObject)


/**
 * FIXME: //=== classes declared here
 * //=== classes mentioned
 * 
 * cHead : head of a cObject chain
 *    NOTE: CANNOT dup() itself! (dup() only creates a cObject)
 */
class SIM_API cHead : public cObject
{
       friend class const_cIterator;
       friend class cIterator;
       friend class cObject;
   public:

    /**
     * MISSINGDOC: cHead:cHead(char*)
     */
       cHead(const char *name=NULL);

    /**
     * MISSINGDOC: cHead:cHead(char*,cHead*)
     */
       cHead(const char *name, cHead *h);

    /**
     * The destructor deletes all objects in the list that were created
     * on the heap.
     */
       virtual ~cHead()  {deleteChildren();}

       // redefined functions

    /**
     * Returns the pointer to the class name string, "cHead".
     */
       virtual const char *className() const {return "cHead";}

    /**
     * MISSINGDOC: cHead:char*inspectorFactoryName()
     */
       virtual const char *inspectorFactoryName() const {return "cHeadIFC";}

    /**
     * Calls the function passed for each object in the list.
     */
       virtual void forEach(ForeachFunc f);

       // new functions

    /**
     * Searches the list for an object with the given name and returns
     * its pointer. If no such object was found, NULL is returned.
     */
       cObject *find(const char *objname);

    /**
     * Returns the number of objects in the list.
     */
       int count();
};

//==========================================================================
//  cIterator : walks along a cHead-cObject list
//   NOTE: not a cObject descendant!


/**
 * FIXME: 
 *  cIterator : walks along a cHead-cObject list
 *   NOTE: not a cObject descendant!
 */
class SIM_API cIterator
{
   private:
      cObject *p;
   public:

    /**
     * MISSINGDOC: cIterator:cIterator(cObject&)
     */
      cIterator(cObject& h)    {p = &h ? h.firstchildp : NO(cObject);}

    /**
     * MISSINGDOC: cIterator:void init(cObject&)
     */
      void init(cObject& h)    {p = &h ? h.firstchildp : NO(cObject);}

    /**
     * Returns a pointer to the current object.
     */
      cObject *operator()()    {return p;}

    /**
     * Returns true if we reach the end of the list.
     */
      bool end()               {return (bool)(p==NULL);}

    /**
     * MISSINGDOC: cIterator:cObject*operator++()
     */
      cObject *operator++(int) {cObject *t=p; if(p) p=p->nextp; return t;}
};



/**
 * MISSINGDOC: const_cIterator
 */
class SIM_API const_cIterator
{
 private:
  const cObject *p;
 public:

    /**
     * MISSINGDOC: const_cIterator:_cIterator(cObject&)
     */
  const_cIterator(const cObject& h)    {p = &h ? h.firstchildp : NO(cObject);}

    /**
     * MISSINGDOC: const_cIterator:void init(cObject&)
     */
  void init(const cObject& h)    {p = &h ? h.firstchildp : NO(cObject);}

    /**
     * MISSINGDOC: const_cIterator:cObject*operator()()
     */
  const cObject *operator()() const {return p;}

    /**
     * MISSINGDOC: const_cIterator:bool end()
     */
  bool end() const               {return (bool)(p==NULL);}

    /**
     * MISSINGDOC: const_cIterator:cObject*operator++()
     */
  const cObject *operator++(int) {const cObject *t=p; if(p) p=p->nextp; return t;}
};

//==========================================================================
//=== useful functions for finding objects by name:

inline cNetworkType *findNetwork(const char *s)    {return (cNetworkType *)networks.find(s);}
inline cModuleType *findModuleType(const char *s)  {return (cModuleType *)modtypes.find(s);}
inline cModuleInterface *findModuleInterface(const char *s) {return (cModuleInterface *)modinterfaces.find(s);}
inline cLinkType *findLink(const char *s)          {return (cLinkType *)linktypes.find(s);}
inline cFunctionType *findFunction(const char *s)  {return (cFunctionType *)functions.find(s);}
inline cInspectorFactory *findInspectorFactory(const char *s) {return (cInspectorFactory *)inspectorfactories.find(s);}

