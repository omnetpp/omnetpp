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

#include <iostream>
#include <vector>
#include <string>
#include "envirdefs.h"

#define ARGSPEC "h?f:u:l:c:r:n:x:i:q:e:avwsm"

namespace omnetpp {

class cConfigOption;
class cRuntimeError;

namespace envir {

class ArgList;
class InifileContents;
class DebuggerSupport;

extern cConfigOption *CFGID_NETWORK;
extern cConfigOption *CFGID_SIM_TIME_LIMIT;
extern cConfigOption *CFGID_REAL_TIME_LIMIT;
extern cConfigOption *CFGID_CPU_TIME_LIMIT;
extern cConfigOption *CFGID_DEBUG_STATISTICS_RECORDING;
extern cConfigOption *CFGID_WARNINGS;

/**
 * @brief An Envir that can be instantiated as a user interface, like Cmdenv
 * and Qtenv.
 *
 * @ingroup SimSupport
 */
class ENVIR_API AppBase
{
  protected:
    ArgList *args;  //TODO init!!!
    std::ostream& out;
    InifileContents *ini;

    bool verbose;
    bool useStderr; // only used in subclasses

    DebuggerSupport *debuggerSupport;

    static AppBase *activeApp;

  public:
    /**
     * Constructor
     */
    AppBase(std::ostream& out = std::cout);

    virtual ~AppBase();

    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program.
     */
    virtual int runApp(int argc, char *argv[], InifileContents *ini);

    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program. Delegates to the other run() overload.
     */
    virtual int runApp(const std::vector<std::string>& args, InifileContents *ini) final;

    static AppBase *getActiveApp() {return activeApp;}

    InifileContents *getInifileContents() const {return ini;}

    std::ostream& getOutputStream() const {return out;}

    ArgList *getArgList() const {return args;}

    bool isVerbose() const {return verbose;}

    DebuggerSupport *getDebuggerSupport() const {return debuggerSupport;}

    void displayException(std::exception& ex);

//  protected:
    // functions added locally
    virtual bool simulationRequired();
    virtual int doRunApp() = 0;
    void printHelp();
    virtual void printUISpecificHelp() = 0;

    virtual bool ensureDebugger(cRuntimeError *error = nullptr);
    virtual void alert(const char *msg) = 0;
    void alertf(const char *fmt, ...);

    static void crashHandler(int signum);
    virtual void installCrashHandler();
    virtual std::vector<int> resolveRunFilter(const char *configName, const char *runFilter);
    virtual void printRunInfo(const char *configName, const char *runFilter, const char *query);
    virtual void printConfigValue(const char *configName, const char *runFilter, const char *optionName);
};

}  // namespace envir
}  // namespace omnetpp

#endif

