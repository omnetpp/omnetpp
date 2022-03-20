//==========================================================================
//  RUNNABLEENVIR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "runnableenvir.h"

namespace omnetpp {
namespace envir {

int RunnableEnvir::run(const std::vector<std::string>& args, cConfiguration *cfg)
{
    char **argv = new char *[args.size()];
    for (int i = 0; i < args.size(); i++)
        argv[i] = const_cast<char*>(args[i].c_str());
    return run(args.size(), argv, cfg);
}

}  // namespace envir
}  // namespace omnetpp

