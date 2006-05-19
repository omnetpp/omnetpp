//=========================================================================
//  TRACETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "eventlog.h"

int main(int argc, char **argv)
{
    if (argc<3)
    {
        fprintf(stderr, "Usage: tracetool <logfile> <eventnumber>\n");
        exit(0);
    }

    EventLog eventLog(argv[1]);
    eventLog.writeTrace(stdout);
    
    EventLog *traceLog = eventLog.traceEvent(atol(argv[2]));
    traceLog->writeTrace(stdout);
    delete traceLog;
}
