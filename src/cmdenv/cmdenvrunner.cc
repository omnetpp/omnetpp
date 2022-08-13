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

extern cConfigOption *CFGID_CMDENV_EXPRESS_MODE;
extern cConfigOption *CFGID_CMDENV_AUTOFLUSH;
extern cConfigOption *CFGID_CMDENV_STATUS_FREQUENCY;
extern cConfigOption *CFGID_CMDENV_PERFORMANCE_DISPLAY;
extern cConfigOption *CFGID_CMDENV_EVENT_BANNERS;
extern cConfigOption *CFGID_CMDENV_EVENT_BANNER_DETAILS;


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

