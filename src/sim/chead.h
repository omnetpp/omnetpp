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
  Copyright (C) 1992,99 Andras Varga
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

class cHead : public cObject
{
       friend class cIterator;
       friend class cObject;
   public:
       cHead(const char *name=NULL);
       cHead(const char *name, cHead *h);
       virtual ~cHead()  {deleteChildren();}

       // redefined functions
       virtual const char *className()  {return "cHead";}
       virtual const char *inspectorFactoryName()  {return "cHeadIFC";}
       virtual void forEach(ForeachFunc f);

       // new functions
       cObject *find(const char *objname);
       int count();
};

//==========================================================================
//  cIterator : walks along a cHead-cObject list
//   NOTE: not a cObject descendant!

class cIterator
{
   private:
      cObject *p;
   public:
      cIterator(cObject& h)    {p = &h ? h.firstchildp : NO(cObject);}
      void init(cObject& h)    {p = &h ? h.firstchildp : NO(cObject);}
      cObject *operator()()    {return p;}
      bool end()               {return (bool)(p==NULL);}
      cObject *operator++(int) {cObject *t=p; if(p) p=p->nextp; return t;}
};

//==========================================================================
//=== useful functions for finding objects by name:

inline cNetworkType *findNetwork(const char *s)    {return (cNetworkType *)networks.find(s);}
inline cModuleType *findModuleType(const char *s)  {return (cModuleType *)modtypes.find(s);}
inline cModuleInterface *findModuleInterface(const char *s) {return (cModuleInterface *)modinterfaces.find(s);}
inline cLinkType *findLink(const char *s)          {return (cLinkType *)linktypes.find(s);}
inline cFunctionType *findFunction(const char *s)  {return (cFunctionType *)functions.find(s);}
inline cInspectorFactory *findInspectorFactory(const char *s) {return (cInspectorFactory *)inspectorfactories.find(s);}

#endif
