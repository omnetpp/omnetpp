//=========================================================================
//  CCOMMBUFFER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "ccommbuffer.h"
#include "simutil.h"
#include "cobject.h"
#include "cobjectfactory.h"    // createOne()

NAMESPACE_BEGIN


void cCommBuffer::packObject(cObject *obj)
{
    pack(obj->getClassName());
    obj->parsimPack(this);
}

cObject *cCommBuffer::unpackObject()
{
    char *classname;
    unpack(classname);
    cObject *obj = createOne(classname);
    delete [] classname;

    obj->parsimUnpack(this);
    return obj;
}

NAMESPACE_END

