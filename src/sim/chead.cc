//=========================================================================
//
//  CHEAD.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Definition of
//    cHead locals : a 'dummy' object
//
//   Member functions of
//    cHead  : head of a list containing cObject descendants
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // memcmp, memcpy, memset
#include "macros.h"
#include "chead.h"

//==========================================================================
//=== Global objects:

cHead superhead         ( "root",               NULL       );
cHead networks          ( "networks",           &superhead );
cHead modinterfaces     ( "module-interfaces",  &superhead );
cHead modtypes          ( "module-types",       &superhead );
cHead linktypes         ( "link-types",         &superhead );
cHead functions         ( "functions",          &superhead );
cHead classes           ( "classes",            &superhead );
cHead inspectorfactories( "inspector-factories",&superhead );
cHead enums             ( "enums",              &superhead );

//=== Registration
Register_Class( cHead )

//==========================================================================
//=== cHead - member functions

cHead::cHead(const char *name) : cObject(name)
{
}

cHead::cHead(const char *name, cHead *h) : cObject(name, h)
{
}

void cHead::forEach(ForeachFunc do_fn)
{
      if( do_fn(this,true) )
         for( cObject *p=firstchildp; p!=NULL; p=p->nextp)
            p->forEach( do_fn );
      do_fn(this,false);
}

cObject *cHead::find(const char *objname)    // faster than findObject(s,false)
{
      cObject *p = firstchildp;
      while (p && !p->isName(objname)) p=p->nextp;
      return p;
}

int cHead::count()
{
      int i=0;
      for (cObject *p = firstchildp; p!=NULL; p=p->nextp, i++);
      return i;
}

