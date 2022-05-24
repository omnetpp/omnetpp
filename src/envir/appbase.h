//==========================================================================
//  APPBASE.H - part of
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

#ifndef __OMNETPP_ENVIR_APPBASE_H
#define __OMNETPP_ENVIR_APPBASE_H

#include "envirdefs.h"
#include "envirbase.h"
#include "iallinone.h"
#include "debuggersupport.h"

#define ARGSPEC "h?f:u:l:c:r:n:x:i:q:e:avwsm"

namespace omnetpp {
namespace envir {

struct AppBaseOptions
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    std::string networkName;
    std::string inifileNetworkDir;

    simtime_t simtimeLimit;
    simtime_t warmupPeriod;
    double realTimeLimit = 0;
    double cpuTimeLimit = 0;

    bool useStderr = true;
    bool verbose = true;
    bool warnings = true;
    bool debugStatisticsRecording = false;
};

/**
 * @brief An Envir that can be instantiated as a user interface, like Cmdenv
 * and Qtenv.
 *
 * @ingroup SimSupport
 */
class ENVIR_API AppBase : public IAllInOne
{
  protected:
    AppBaseOptions *opt;   // alias to EnvirBase::opt

    ArgList *args;  //TODO init!!!
    std::ostream& out; //TODO FIXME
    cConfigurationEx *cfg; //TODO FIXME user EnvirBase's

    std::string redirectionFilename;
    int exitCode = 0;

    DebuggerSupport *debuggerSupport = new DebuggerSupport();

    // CPU and real time limit checking
    Stopwatch stopwatch;

    simtime_t simulatedTime;  // sim. time after finishing simulation

  public:
    /**
     * Constructor
     */
    AppBase();

    virtual ~AppBase();

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

    EnvirBase *getEnvir() const {return dynamic_cast<EnvirBase*>(cSimulation::getActiveEnvir());}
    cSimulation *getSimulation() const {return cSimulation::getActiveSimulation();}

    cConfiguration *getConfig() {return getEnvir()->getConfig();}
    cConfigurationEx *getConfigEx() {return getEnvir()->getConfigEx();}
    cIOutputVectorManager *getOutVectorManager() const {return getEnvir()->getOutVectorManager();}
    cIOutputScalarManager *getOutScalarManager() const {return getEnvir()->getOutScalarManager();}
    cISnapshotManager *getSnapshotManager() const {return getEnvir()->getSnapshotManager();}
    DebuggerSupport *getDebuggerSupport() const {return debuggerSupport;}

    bool isLoggingEnabled() const {return getEnvir()->isLoggingEnabled();}
    void setLoggingEnabled(bool enabled) {getEnvir()->setLoggingEnabled(enabled);}
    bool getDebugOnErrors() const {return getEnvir()->getDebugOnErrors();}
    void setDebugOnErrors(bool enable) {getEnvir()->setDebugOnErrors(enable);}
    bool getAttachDebuggerOnErrors() const {return getEnvir()->getAttachDebuggerOnErrors();}
    void setAttachDebuggerOnErrors(bool enable) {getEnvir()->setAttachDebuggerOnErrors(enable);}

    void setLogLevel(LogLevel logLevel) {getEnvir()->setLogLevel(logLevel);};
    void setLogFormat(const char *logFormat) {getEnvir()->setLogFormat(logFormat);}
    LogFormatter& getLogFormatter() {return getEnvir()->getLogFormatter();}

    bool getEventlogRecording() const {return getEnvir()->getEventlogRecording();}
    void setEventlogRecording(bool enabled) {getEnvir()->setEventlogRecording(enabled);}

  protected:
    void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details=nullptr);

    virtual std::ostream& err();
    virtual std::ostream& errWithoutPrefix();
    virtual std::ostream& warn();
    void printfmsg(const char *fmt, ...); // internal, TODO rename
    static void crashHandler(int signum);
    virtual void installCrashHandler();
    virtual std::vector<int> resolveRunFilter(const char *configName, const char *runFilter);
    virtual void printRunInfo(const char *configName, const char *runFilter, const char *query);
    virtual void printConfigValue(const char *configName, const char *runFilter, const char *optionName);

    virtual bool ensureDebugger(cRuntimeError *error = nullptr) override;

    virtual void log(cLogEntry *entry) override {}   //TODO why needed?

    // functions added locally
    virtual bool simulationRequired();
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

    virtual AppBaseOptions *createOptions() {return new AppBaseOptions();}
    virtual void readOptions();
    virtual void readPerRunOptions();

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

