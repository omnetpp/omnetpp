//==========================================================================
//  CMDENVRUNNER.H - part of
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

#ifndef __OMNETPP_CMDENV_CMDENVRUNNER_H
#define __OMNETPP_CMDENV_CMDENVRUNNER_H

#include "cmddefs.h"
#include "envir/runner.h"

namespace omnetpp {
namespace cmdenv {

using namespace envir;

class CMDENV_API CmdenvRunner : public Runner
{
  public:
    CmdenvRunner(cSimulation *simulation, std::ostream& out, bool& sigintReceived) : Runner(simulation, out, sigintReceived) {}
    virtual void configure(cConfiguration *cfg) override;
    virtual void run() override;
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

