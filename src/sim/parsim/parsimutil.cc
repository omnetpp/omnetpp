//=========================================================================
//  PARSIMUTIL.CC - part of
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

#include <string.h>
#include "cenvir.h"
#include "parsimutil.h"
#include "cexception.h"


void getProcIdFromCommandLineArgs(int& myProcId, int& numPartitions, const char *caller)
{
    int argc = ev.argCount();
    char **argv = ev.argVector();
    int i;
    for (i=1; i<argc; i++)
        if (argv[i][0]=='-' && argv[i][1]=='p')
            break;
    if (i==argc)
        throw new cRuntimeError("%s: missing -p<procId>,<numPartitions> switch on the command line", caller);

    char *parg = argv[i];
    myProcId = atoi(parg+2);
    char *s = parg;
    while (*s!=',' && *s) s++;
    numPartitions = (*s) ? atoi(s+1) : 0;
    if (myProcId<0 || numPartitions<=0 || myProcId>=numPartitions)
        throw new cRuntimeError("%s: invalid switch '%s' -- should have the format -p<procId>,<numPartitions>",
                                caller, parg);
}
