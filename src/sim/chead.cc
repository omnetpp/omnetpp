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
  Copyright (C) 1992-2002 Andras Varga
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

cHead networks("networks");
cHead modinterfaces("module-interfaces");
cHead modtypes("module-types");
cHead linktypes("link-types");
cHead functions("functions");
cHead classes("classes");
cHead enums("enums");

//=== Registration
Register_Class(cHead);

//==========================================================================
//=== cHead - member functions

cHead::cHead(const char *name) : cObject(name)
{
}

void cHead::forEach(ForeachFunc do_fn)
{
      if( do_fn(this,true) )
         for( cObject *p=firstchildp; p!=NULL; p=p->nextp)
            p->forEach( do_fn );
      do_fn(this,false);
}

cObject *cHead::find(const char *objname) const    // faster than findObject(s,false)
{
      cObject *p = firstchildp;
      while (p && !p->isName(objname)) p=p->nextp;
      return p;
}

int cHead::count() const
{
      int i=0;
      for (cObject *p = firstchildp; p!=NULL; p=p->nextp, i++);
      return i;
}

