//==========================================================================
//  CMDENVRUNNER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "cmdenvir.h"
#include "cmdenvrunner.h"

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOption(CFGID_CMDENV_EXPRESS_MODE, "cmdenv-express-mode", CFG_BOOL, "true", "Selects normal (debug/trace) or express mode.")
Register_GlobalConfigOption(CFGID_CMDENV_AUTOFLUSH, "cmdenv-autoflush", CFG_BOOL, "false", "Call `fflush(stdout)` after each event banner or status update; affects both express and normal mode. Turning on autoflush may have a performance penalty, but it can be useful with printf-style debugging for tracking down program crashes.")
Register_GlobalConfigOption(CFGID_CMDENV_EVENT_BANNERS, "cmdenv-event-banners", CFG_BOOL, "true", "When `cmdenv-express-mode=false`: turns printing event banners on/off.")
Register_GlobalConfigOption(CFGID_CMDENV_EVENT_BANNER_DETAILS, "cmdenv-event-banner-details", CFG_BOOL, "false", "When `cmdenv-express-mode=false`: print extra information after event banners.")
Register_GlobalConfigOptionU(CFGID_CMDENV_STATUS_FREQUENCY, "cmdenv-status-frequency", "s", "2s", "When `cmdenv-express-mode=true`: print status update every n seconds.")
Register_GlobalConfigOption(CFGID_CMDENV_PERFORMANCE_DISPLAY, "cmdenv-performance-display", CFG_BOOL, "true", "When `cmdenv-express-mode=true`: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")

void CmdenvRunner::configure(cConfiguration *cfg)
{
    Runner::configure(cfg);
    setExpressMode(cfg->getAsBool(CFGID_CMDENV_EXPRESS_MODE));
    setAutoFlush(cfg->getAsBool(CFGID_CMDENV_AUTOFLUSH));
    setStatusFrequencyMs(1000*cfg->getAsDouble(CFGID_CMDENV_STATUS_FREQUENCY));
    setPrintPerformanceData(cfg->getAsBool(CFGID_CMDENV_PERFORMANCE_DISPLAY));
    setPrintThreadId(false); //TODO
    setPrintEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNERS));
    setDetailedEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNER_DETAILS));
}

void CmdenvRunner::run()
{
    CmdEnvir *envir = dynamic_cast<CmdEnvir*>(simulation->getEnvir());
    setFakeGUI(envir ? envir->getFakeGui() : nullptr); // use same fakeGUI as envir

    Runner::run();
}

}  // namespace cmdenv
}  // namespace omnetpp

