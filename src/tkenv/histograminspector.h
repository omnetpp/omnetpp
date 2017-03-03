//==========================================================================
//  HISTOGRAMINSPECTOR.H - part of
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

#ifndef __OMNETPP_TKENV_HISTOGRAMINSPECTOR_H
#define __OMNETPP_TKENV_HISTOGRAMINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace tkenv {


class TKENV_API HistogramInspector : public Inspector
{
   protected:
      char canvas[64];
   public:
      HistogramInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void refresh() override;
      virtual void commit() override {}
      virtual int inspectorCommand(int argc, const char **argv) override;

      // return textual information in general or about a value/value pair
      void generalInfo( char *buf );
      void getCellInfo( char *buf, int cellindex );
};


} // namespace tkenv
}  // namespace omnetpp

#endif


