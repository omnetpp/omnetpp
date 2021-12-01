//=========================================================================
//  PARSIMUTIL.CC - part of
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

#include <cstring>
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cexception.h"
#include "common/stringutil.h"
#include "parsimutil.h"

namespace omnetpp {

int getProcIdFromCommandLineArgs(int numPartitions, const char *caller)
{
    int argc = getEnvir()->getArgCount();
    char **argv = getEnvir()->getArgVector();
    int i;
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-' && argv[i][1] == 'p')
            break;

    if (i == argc)
        throw cRuntimeError("%s: Missing -p<procId> switch on the command line", caller);

    const char *s = argv[i] + 2;
    char *endp;
    int result = common::opp_strtol(s, &endp);
    if (s==endp || *endp || result < 0)
        throw cRuntimeError("%s: Invalid -p<procId> switch on the command line: \"%s\"", caller, argv[i]);
    if (result >= numPartitions)
        throw cRuntimeError("%s: Invalid -p<procId> switch on the command line: procId=%d is out of range 0..%d", caller, result, numPartitions-1);
    return result;
}

}  // namespace omnetpp

