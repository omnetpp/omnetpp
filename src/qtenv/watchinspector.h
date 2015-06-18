//==========================================================================
//  WATCHINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_WATCHINSPECTOR_H
#define __OMNETPP_QTENV_WATCHINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace qtenv {

class QTENV_API WatchInspector: public Inspector
{
   public:
      WatchInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(QWidget *parent) override;
      virtual void refresh() override;
      virtual void commit() override;
};

} // namespace qtenv qtenv
} // namespace omnetpp

#endif



