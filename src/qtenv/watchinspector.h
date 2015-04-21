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

#ifndef __OMNETPP_WATCHINSPECTOR_H
#define __OMNETPP_WATCHINSPECTOR_H

#include "inspector.h"

NAMESPACE_BEGIN
namespace qtenv {

class TKENV_API WatchInspector: public Inspector
{
   public:
      WatchInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual void commit();
};

} //namespace
NAMESPACE_END

#endif



