//=========================================================================
//  PARSIMUTIL.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include "cenvir.h"
#include "parsimutil.h"
#include "cexception.h"

USING_NAMESPACE


void getProcIdFromCommandLineArgs(int& myProcId, int& numPartitions, const char *caller)
{
    int argc = ev.argCount();
    char **argv = ev.argVector();
    int i;
    for (i=1; i<argc; i++)
        if (argv[i][0]=='-' && argv[i][1]=='p')
            break;
    if (i==argc)
        throw cRuntimeError("%s: missing -p<procId>,<numPartitions> switch on the command line", caller);

    char *parg = argv[i];
    myProcId = atoi(parg+2);
    char *s = parg;
    while (*s!=',' && *s) s++;
    numPartitions = (*s) ? atoi(s+1) : 0;
    if (myProcId<0 || numPartitions<=0 || myProcId>=numPartitions)
        throw cRuntimeError("%s: invalid switch '%s' -- should have the format -p<procId>,<numPartitions>",
                                caller, parg);
}
