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
    bool verbose; // for subclasses
    bool useStderr = false;
    std::string redirectionFilename;
    cTerminationException *terminationReason = nullptr;

  public:
    SimulationHolder(std::ostream& out) : out(out) {}
    virtual void setVerbose(bool verbose) {this->verbose = verbose;}
    virtual void setUseStderr(bool useStderr) {this->useStderr = useStderr;}

    static cINedLoader *loadNEDFiles(cConfiguration *cfg, ArgList *args=nullptr);
    static std::string getFormattedMessage(std::exception& ex);

    virtual void configureAndRunSimulation(cSimulation *simulation, cConfiguration *cfg, IRunner *runner, const char *redirectFileName);
    virtual void runConfiguredSimulation(cSimulation *simulation, IRunner *runner, const char *redirectFileName);

    const cTerminationException *getTerminationReason() const {return terminationReason;}  // stopping reason in case of normal (error-free) simulation execution

  protected:
    virtual std::ostream& err();
    virtual std::ostream& errWithoutPrefix();
    virtual std::ostream& warn();

    virtual void setupNetwork(cModuleType *networkType);

    virtual void startOutputRedirection(const char *fileName);
    virtual void stopOutputRedirection();
    virtual bool isOutputRedirected();

    // Utility function; never returns nullptr
    virtual cModuleType *resolveNetwork(const char *networkName, const char *baseDirectory);

    virtual void printException(std::exception& e, const char *when=nullptr);

    // Utility function: checks simulation fingerprint and displays a message accordingly
    virtual void checkFingerprint();

    virtual void afterRunFinally(cSimulation *simulation, bool endRunRequired); // wish C++ had 'finally'

    // Hook called when the simulation terminates normally.
    // Its current use is to notify parallel simulation part.
    virtual void stoppedWithTerminationException(cTerminationException& e);

    // Hook called when the simulation is stopped with an error.
    // Its current use is to notify parallel simulation part.
    virtual void stoppedWithException(std::exception& e);
};

}  // namespace envir
}  // namespace omnetpp

#endif

