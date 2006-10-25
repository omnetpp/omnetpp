//==========================================================================
//  STRUCTINSP.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __STRUCTINSP_H
#define __STRUCTINSP_H

#include <tk.h>
#include "inspector.h"
#include "omnetapp.h"

class cClassDescriptor;

#define BUFSIZE     4096           /* buffer size */
#define MAXWRITE    3072           /* one sprintf() shouldn't be more than MAXWRITE chars */
#define FLUSHLIMIT  (BUFSIZE-MAXWRITE-2)

class TStructPanel : public TInspectorPanel
{
   protected:
      char buf[BUFSIZE];
      char tmpbuf[MAXWRITE+1];
      char *writeptr;

      void flushIfNeeded(int limit);
      void displayStruct(void *object, cClassDescriptor *sd, int level);

   public:
      TStructPanel(const char *widgetname, cPolymorphic *obj);
      virtual void update();
      virtual void writeBack();
      virtual int inspectorCommand(Tcl_Interp *, int, const char **);
};

#endif
