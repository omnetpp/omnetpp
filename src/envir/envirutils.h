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
    public:
        static void dumpComponentList(std::ostream& out, const char *category, bool verbose);
};

}  // namespace envir
}  // namespace omnetpp

#endif

