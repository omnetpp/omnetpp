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
   protected:
      bool hascontentspage;
      bool focuscontentspage;
   public:
      GenericObjectInspector();
      ~GenericObjectInspector();
      virtual void setObject(cObject *obj);
      void setContentsPage(bool show, bool focus) {hascontentspage = show; focuscontentspage = focus;}
      virtual void createWindow(const char *window, const char *geometry);
      virtual void update();
      virtual void writeBack();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);
};

NAMESPACE_END

#endif



