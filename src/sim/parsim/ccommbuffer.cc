//=========================================================================
//
// CCOMMBUFFER.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga & David Wu, 2003
//
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003 Andras Varga & David Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "ccommbuffer.h"
#include "util.h"  // opp_string
#include "cobject.h"
#include "ctypes.h"    // createOne()


bool notNull(void *ptr, cCommBuffer *buffer)
{
    bool flag = ptr != NULL;
    buffer->pack(flag);
    return flag;
}

bool checkFlag(cCommBuffer *buffer)
{
    bool flag = 0;
    buffer->unpack(flag);
    return flag;
}


void packObject(cObject *obj, cCommBuffer *buffer)
{
    buffer->pack(obj->className());
    obj->netPack(buffer);
}

cObject *unpackObject(cCommBuffer *buffer)
{
    char *classname;
    buffer->unpack(classname);
    cObject *obj = (cObject *) createOne(classname);
    delete [] classname;

    obj->netUnpack(buffer);
    return obj;
}

