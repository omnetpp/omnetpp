//=========================================================================
//  CCOMMBUFFER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "ccommbuffer.h"
#include "util.h"  // opp_string
#include "cobject.h"
#include "cclassfactory.h"    // createOne()

USING_NAMESPACE


void cCommBuffer::packObject(cObject *obj)
{
    pack(obj->className());
    obj->netPack(this);
}

cObject *cCommBuffer::unpackObject()
{
    char *classname;
    unpack(classname);
    cObject *obj = createOne(classname);
    delete [] classname;

    obj->netUnpack(this);
    return obj;
}

