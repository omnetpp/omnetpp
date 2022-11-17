//==========================================================================
//  CMDENVAPP.H - part of
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

#ifndef __OMNETPP_CMDENV_CMDENVAPP_H
#define __OMNETPP_CMDENV_CMDENVAPP_H

#include "cmddefs.h"
#include "envir/appbase.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

/**
 * Command line user interface.
 */
class CMDENV_API CmdenvApp : public AppBase
{
   protected:
     virtual void alert(const char *msg) override; //TODO route to CmdenvCore instance?
     virtual void displayException(std::exception& ex) override; //TODO route to CmdenvCore instance?

   public:
     CmdenvApp();
     virtual ~CmdenvApp();

   protected:
     virtual int doRunApp() override;
     virtual void printUISpecificHelp() override;

};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

