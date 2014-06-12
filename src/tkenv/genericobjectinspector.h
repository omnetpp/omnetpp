//==========================================================================
//  GENERICOBJECTINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GENERICOBJECTINSPECTOR_H
#define __GENERICOBJECTINSPECTOR_H

#include <tk.h>
#include "inspector.h"

NAMESPACE_BEGIN


class TKENV_API GenericObjectInspector : public Inspector
{
   public:
      GenericObjectInspector(InspectorFactory *f);
      ~GenericObjectInspector();
      virtual void doSetObject(cObject *obj);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual void commit();
      virtual int inspectorCommand(int argc, const char **argv);
};

NAMESPACE_END

#endif



