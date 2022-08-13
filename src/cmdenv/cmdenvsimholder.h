//==========================================================================
//  CMDENVSIMHOLDER.H - part of
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

#ifndef __OMNETPP_CMDENV_CMDENVSIMHOLDER_H
#define __OMNETPP_CMDENV_CMDENVSIMHOLDER_H

#include "cmddefs.h"
#include "envir/simulationholder.h"

namespace omnetpp {

class cSimulation;
class cConfiguration;
class cINedLoader;

namespace cmdenv {

class Cmdenv;

using namespace omnetpp::envir;

class CmdenvSimulationHolder : public SimulationHolder
{
  protected:
    cINedLoader *nedLoader;
    bool& sigintReceived;

  public:
    CmdenvSimulationHolder(Cmdenv *app);
    CmdenvSimulationHolder(cINedLoader *nedLoader);
    CmdenvSimulationHolder(std::ostream& out, cINedLoader *nedLoader, bool& sigintReceived);
    virtual void runCmdenvSimulation(cConfiguration *cfg, int numRuns=1, int runsTried=1);
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

