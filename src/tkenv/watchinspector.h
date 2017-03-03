//==========================================================================
//  WATCHINSPECTOR.H - part of
//
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

#ifndef __OMNETPP_TKENV_WATCHINSPECTOR_H
#define __OMNETPP_TKENV_WATCHINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace tkenv {

class TKENV_API WatchInspector: public Inspector
{
   public:
      WatchInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void refresh() override;
      virtual void commit() override;
};

} // namespace tkenv
}  // namespace omnetpp

#endif



