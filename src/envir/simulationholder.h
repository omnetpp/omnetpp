//==========================================================================
//  SIMULATIONHOLDER.H - part of
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

#ifndef __OMNETPP_ENVIR_SIMULATIONHOLDER_H
#define __OMNETPP_ENVIR_SIMULATIONHOLDER_H

#include <iostream>
#include "envirdefs.h"
#include "omnetpp/cexception.h"
#include "omnetpp/clifecyclelistener.h"

namespace omnetpp {

class cConfiguration;
class cSimulation;
class cINedLoader;
class cTerminationException;
class cModuleType;
class IRunner;

namespace envir {


class SimulationHolder
{
  protected:
    std::ostream& out;
    cINedLoader *nedLoader;
    bool verbose;
    bool useStderr = false;
    std::string redirectionFilename;
    cTerminationException *terminationReason = nullptr;

  protected:
    virtual cSimulation *createSimulation();
    virtual void startOutputRedirection(const char *fileName);
    virtual void stopOutputRedirection();
    virtual bool isOutputRedirected();
    virtual void printException(std::exception& e, const char *when=nullptr);
    virtual void deleteNetwork(cSimulation *simulation);

  public:
    static cINedLoader *createConfiguredNedLoader(cConfiguration *cfg, ArgList *args=nullptr);
    SimulationHolder(std::ostream& out, cINedLoader *nedLoader=nullptr) : out(out), nedLoader(nedLoader) {}
    virtual void setVerbose(bool verbose) {this->verbose = verbose;}
    virtual void setUseStderr(bool useStderr) {this->useStderr = useStderr;}
    virtual void setupAndRunSimulation(cConfiguration *cfg, IRunner *runner=nullptr, const char *redirectFileName=nullptr);
    virtual void setupAndRunSimulation(cSimulation *simulation, cConfiguration *cfg, IRunner *runner=nullptr, const char *redirectFileName=nullptr);
    const cTerminationException *getTerminationReason() const {return terminationReason;}  // stopping reason in case of normal (error-free) simulation execution
};

}  // namespace envir
}  // namespace omnetpp

#endif

