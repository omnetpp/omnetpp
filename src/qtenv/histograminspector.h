//==========================================================================
//  HISTOGRAMINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_HISTOGRAMINSPECTOR_H
#define __OMNETPP_QTENV_HISTOGRAMINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace qtenv {


class QTENV_API HistogramInspector : public Inspector
{
   protected:
      char canvas[64];
   public:
      HistogramInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(QWidget *parent) override;
      virtual void refresh() override;
      virtual void commit() override {}
      virtual int inspectorCommand(int argc, const char **argv) override;

      // return textual information in general or about a value/value pair
      void generalInfo( char *buf );
      void getCellInfo( char *buf, int cellindex );
};


} // namespace qtenv
} // namespace omnetpp

#endif


