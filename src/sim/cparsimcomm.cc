//=========================================================================
//  CPARCOMM.CC - part of
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

#include <cstdlib>
#include <cstdio>
#include "omnetpp/errmsg.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"

namespace omnetpp {

void cParsimCommunications::broadcast(cCommBuffer *buffer, int tag)
{
    // Default implementation: send to everyone. Try to do as much of the job
    // as possible -- if there're exceptions, throw on only one of them.
    bool hadException = false;
    std::string exceptionText;

    int n = getNumPartitions();
    int myProcId = getProcId();
    for (int i = 0; i < n; i++) {
        try {
            if (myProcId != i)
                send(buffer, tag, i);
        }
        catch (std::exception& e) {
            hadException = true;
            exceptionText = e.what();
        }
    }

    if (hadException)
        throw cRuntimeError("Error during broadcast: %s", exceptionText.c_str());
}

}  // namespace omnetpp

