//=========================================================================
//  CCOMMBUFFER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/ccommbuffer.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cobjectfactory.h"  // createOne()

namespace omnetpp {

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
    delete[] classname;

    obj->parsimUnpack(this);
    return obj;
}

}  // namespace omnetpp

