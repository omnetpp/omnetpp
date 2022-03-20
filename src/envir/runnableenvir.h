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
#include "omnetpp/cenvir.h"

namespace omnetpp {

namespace envir {

/**
 * @brief A cEnvir that can be instantiated as a user interface, like Cmdenv
 * and Qtenv.
 *
 * @ingroup SimSupport
 */
class ENVIR_API RunnableEnvir : public cEnvir
{
  public:
    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program.
     */
    virtual int run(int argc, char *argv[], cConfiguration *cfg) = 0;

    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program. Delegates to the other run() overload.
     */
    virtual int run(const std::vector<std::string>& args, cConfiguration *cfg) final;
};

}  // namespace envir
}  // namespace omnetpp

#endif

