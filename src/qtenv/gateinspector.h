//==========================================================================
//  GATEINSPECTOR.H - part of
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

#ifndef __OMNETPP_GATEINSPECTOR_H
#define __OMNETPP_GATEINSPECTOR_H

#include "inspector.h"

NAMESPACE_BEGIN
namespace qtenv {


class TKENV_API GateInspector : public Inspector
{
   protected:
      char canvas[128];

   protected:
      virtual void doSetObject(cObject *obj);

   public:
      GateInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(QWidget *parent);
      virtual void refresh();
      virtual void redraw();
      virtual int inspectorCommand(int argc, const char **argv);

      // notifications from envir:
      virtual void displayStringChanged(cGate *gate);
};

} //namespace
NAMESPACE_END

#endif
