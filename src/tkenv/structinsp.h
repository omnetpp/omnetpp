//==========================================================================
//  STRUCTINSP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __STRUCTINSP_H
#define __STRUCTINSP_H

#include <tk.h>
#include "inspector.h"
#include "omnetapp.h"

class cStructDescriptor;

#define BUFSIZE     2048           /* buffer size */
#define FLUSHLIMIT  (BUFSIZE-256)  /* one sprintf() should be less than 256 chars */

class TStructPanel : public TInspectorPanel
{
   protected:
      char buf[BUFSIZE];
      char *writeptr;

      void flushIfNeeded(int limit);
      void displayStruct(cStructDescriptor *sd, int level);

   public:
      TStructPanel(const char *widgetname, cObject *obj);
      virtual void update();
      virtual void writeBack();
      virtual int inspectorCommand(Tcl_Interp *, int, const char **);
};

#endif
