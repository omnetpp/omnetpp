//==========================================================================
//  STRUCTINSP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __STRUCTINSP_H
#define __STRUCTINSP_H

#include <tk.h>
#include "inspector.h"
#include "omnetapp.h"


class TStructPanel : public TInspectorPanel
{
   public:
      TStructPanel(const char *widgetname, cObject *obj);
      virtual void update();
      virtual void writeBack();
      virtual int inspectorCommand(Tcl_Interp *, int, const char **);
};

#endif
