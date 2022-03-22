//==========================================================================
//  RUNNABLEENVIR.H - part of
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

#ifndef __OMNETPP_ENVIR_RUNNABLEENVIR_H
#define __OMNETPP_ENVIR_RUNNABLEENVIR_H

#include "envirdefs.h"
#include "envirbase.h"
#include "debuggersupport.h"

namespace omnetpp {
namespace envir {

/**
 * @brief An Envir that can be instantiated as a user interface, like Cmdenv
 * and Qtenv.
 *
 * @ingroup SimSupport
 */
class ENVIR_API RunnableEnvir : public EnvirBase
{
  protected:
    DebuggerSupport *debuggerSupport = new DebuggerSupport();

  public:
    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program.
     */
    virtual int run(int argc, char *argv[], cConfiguration *config);

    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program. Delegates to the other run() overload.
     */
    virtual int run(const std::vector<std::string>& args, cConfiguration *cfg) final;

  protected:
    virtual std::ostream& err();
    virtual std::ostream& errWithoutPrefix();
    virtual std::ostream& warn();
    static void crashHandler(int signum);
    virtual std::vector<int> resolveRunFilter(const char *configName, const char *runFilter);
    virtual void printRunInfo(const char *configName, const char *runFilter, const char *query);
    virtual void printConfigValue(const char *configName, const char *runFilter, const char *optionName);

    virtual bool ensureDebugger(cRuntimeError *error = nullptr) override;

    // functions added locally
    virtual bool simulationRequired();
    virtual bool setup();  // does not throw; returns true if OK to go on
    virtual void run();  // does not throw; delegates to doRun()
    virtual void shutdown(); // does not throw
    virtual void doRun() = 0;
    virtual void loadNEDFiles();

    virtual void setupNetwork(cModuleType *network);
    virtual void prepareForRun();

    ArgList *argList()  {return args;}
    void printHelp();
    void setupEventLog();
    virtual void printUISpecificHelp() = 0;

    virtual void startOutputRedirection(const char *fileName);
    virtual void stopOutputRedirection();
    virtual bool isOutputRedirected();

//TODO
//    virtual EnvirOptions *createOptions() {return new EnvirOptions();}
//    virtual void readOptions();
//    virtual void readPerRunOptions();
    int parseSimtimeResolution(const char *resolution);

    // Utility function; never returns nullptr
    cModuleType *resolveNetwork(const char *networkname);

    virtual void displayException(std::exception& e);
    virtual std::string getFormattedMessage(std::exception& ex);

    // Utility function: checks simulation fingerprint and displays a message accordingly
    void checkFingerprint();

    // Utility function for getXMLDocument() and getParsedXMLString()
    cXMLElement *resolveXMLPath(cXMLElement *documentnode, const char *path);

    // Measuring elapsed time
    void checkTimeLimits();
    void resetClock();
    void startClock();
    void stopClock();
    double getElapsedSecs(); //FIXME into cEnvir, so it can be put into exception texts

    // Hook called when the simulation terminates normally.
    // Its current use is to notify parallel simulation part.
    void stoppedWithTerminationException(cTerminationException& e);

    // Hook called when the simulation is stopped with an error.
    // Its current use is to notify parallel simulation part.
    void stoppedWithException(std::exception& e);

};

}  // namespace envir
}  // namespace omnetpp

#endif

