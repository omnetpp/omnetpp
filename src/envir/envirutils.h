//==========================================================================
//  ENVIRUTILS.H - part of
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
        static void dumpResultRecorderChain(std::ostream& out, cResultListener *listener, int depth);
    public:
        static void dumpComponentList(std::ostream& out, const char *category, bool verbose);
        static void dumpResultRecorders(std::ostream& out, cComponent *component);
        static void dumpComponentResultRecorders(std::ostream& out, cComponent *component);
};

}  // namespace envir
}  // namespace omnetpp

#endif

