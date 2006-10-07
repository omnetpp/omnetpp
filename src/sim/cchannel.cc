//========================================================================
//  CCHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cChannel : channel class
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cchannel.h"
#include "cpar.h"
#include "cgate.h"
#include "cmessage.h"
#include "cmodule.h"
#include "cenvir.h"
#include "csimulation.h"
#include "globals.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;

Register_Class(cChannel);


cChannel::cChannel(const char *name) : cComponent(name)
{
    fromgatep = NULL;
}

cChannel::~cChannel()
{
}

std::string cChannel::info() const
{
    // print all parameters
    std::stringstream out;
    for (int i=0; i<params(); i++)
    {
        cPar& p = const_cast<cChannel *>(this)->par(i);
        out << p.fullName() << "=" << p.info() << " ";
    }
    return out.str();
}

void cChannel::forEachChild(cVisitor *v)
{
    cDefaultList::forEachChild(v);
}

void cChannel::writeContents(ostream& os)
{
    //FIXME throw out this function from everywhere
}

void cChannel::netPack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,"netPack() not implemented");
}

void cChannel::netUnpack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,"netUnpack() not implemented");
}

bool cChannel::deliver(cMessage *msg, simtime_t t)
{
    // just hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}


