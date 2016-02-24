//==========================================================================
//  ENVIRUTILS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_ENVIRUTILS_H
#define __OMNETPP_ENVIR_ENVIRUTILS_H

#include "envirdefs.h"

namespace omnetpp {

class cComponent;
class cConfigOption;
class cResultListener;

namespace envir {

/**
 * Utility functions for EnvirBase.
 */
class ENVIR_API EnvirUtils
{
    private:
        static std::string getConfigOptionType(cConfigOption *option);
        static void dumpResultRecorderChain(cResultListener *listener, int depth);
    public:
        static void dumpComponentList(const char *category);
        static void dumpResultRecorders(cComponent *component);
};

}  // namespace envir
}  // namespace omnetpp

#endif

