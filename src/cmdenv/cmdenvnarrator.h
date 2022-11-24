//==========================================================================
//  CMDENVNARRATOR.H - part of
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

#ifndef __OMNETPP_CMDENV_CMDENVNARRATOR_H
#define __OMNETPP_CMDENV_CMDENVNARRATOR_H

#include <fstream>
#include "cmddefs.h"
#include "omnetpp/clifecyclelistener.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/csimulation.h"

namespace omnetpp {
namespace cmdenv {

class ICmdenvNarrator
{
  protected:
    class VerboseListener : public cISimulationLifecycleListener {
      private:
        ICmdenvNarrator *narrator;
        std::ofstream& fout;  // closed if not used
      public:
        VerboseListener(ICmdenvNarrator *narrator, std::ofstream& fout) : narrator(narrator), fout(fout) {}
        virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override {
            narrator->simulationLifecycleEvent(getSimulation(), fout, eventType, details);
        }
        virtual void listenerRemoved() override { cISimulationLifecycleListener::listenerRemoved(); delete this; }
    };

    std::ostream& out;
    bool verbose = true;
    bool useStderr = true;

  public:
    ICmdenvNarrator(std::ostream& out) : out(out) {}
    virtual ~ICmdenvNarrator() {}

    virtual void setVerbose(bool verbose) {this->verbose = verbose;}
    virtual void setUseStderr(bool useStderr) {this->useStderr = useStderr;}

    virtual void usingThreads(int numThreads) = 0;
    virtual void preparing(const char *configName, int runNumber) = 0;
    virtual void summary(int numRuns, int runsTried, int numErrors) = 0;
    virtual void beforeRedirecting(cConfiguration *cfg) = 0;
    virtual void redirectingTo(cConfiguration *cfg, const char *redirectFileName) = 0;
    virtual void onRedirectionFileOpen(std::ostream& fout, cConfiguration *cfg, const char *redirectFileName) = 0; // non-narrating
    virtual void afterRedirecting(cConfiguration *cfg, std::ofstream& fout) = 0;
    virtual void simulationCreated(cSimulation *simulation, std::ofstream& fout);
    virtual void simulationLifecycleEvent(cSimulation *simulation, std::ofstream& fout, SimulationLifecycleEventType eventType, cObject *details) = 0;
    virtual void displayException(std::exception& ex) = 0;
    virtual void logException(std::ofstream& fout, std::exception& ex) = 0;
};

class CmdenvNarrator : public ICmdenvNarrator
{
  public:
    CmdenvNarrator(std::ostream& out) : ICmdenvNarrator(out) {}
    virtual void usingThreads(int numThreads) override;
    virtual void preparing(const char *configName, int runNumber) override;
    virtual void summary(int numRuns, int runsTried, int numErrors) override;
    virtual void beforeRedirecting(cConfiguration *cfg) override;
    virtual void redirectingTo(cConfiguration *cfg, const char *redirectFileName) override;
    virtual void onRedirectionFileOpen(std::ostream& fout, cConfiguration *cfg, const char *redirectFileName) override;
    virtual void afterRedirecting(cConfiguration *cfg, std::ofstream& fout) override;
    virtual void simulationLifecycleEvent(cSimulation *simulation, std::ofstream& fout, SimulationLifecycleEventType eventType, cObject *details) override;
    virtual void displayException(std::exception& ex) override;
    virtual void logException(std::ofstream& fout, std::exception& ex) override;
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

