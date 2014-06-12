//==========================================================================
//  HISTOGRAMINSPECTOR.H - part of
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

#ifndef __HISTOGRAMINSPECTOR_H
#define __HISTOGRAMINSPECTOR_H

#include <tk.h>
#include "inspector.h"

NAMESPACE_BEGIN


class TKENV_API HistogramInspector : public Inspector
{
   protected:
      char canvas[64];
   public:
      HistogramInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual void commit() {}
      virtual int inspectorCommand(int argc, const char **argv);

      // return textual information in general or about a value/value pair
      void generalInfo( char *buf );
      void getCellInfo( char *buf, int cellindex );
};


NAMESPACE_END

#endif


