//==========================================================================
//  OUTPUTVECTORINSPECTOR.H - part of
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

#ifndef __OMNETPP_TKENV_OUTPUTVECTORINSPECTOR_H
#define __OMNETPP_TKENV_OUTPUTVECTORINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace tkenv {


class CircBuffer
{
   public:
      struct CBEntry {
        simtime_t t;
        double value;
      };
   protected:
      CBEntry *buf;
      int siz, n, head;
   public:
      CircBuffer( int size );
      ~CircBuffer();
      void add(simtime_t t, double value);
      int headPos() {return head;}
      int tailPos() {return (head-n+1+siz)%siz;}
      int items() {return n;}
      int size() {return siz;}
      CBEntry& entry(int k) {return buf[k];}
};

class TKENV_API OutputVectorInspector : public Inspector
{
   public:
      CircBuffer circbuf;

   protected:
      char canvas[64];

      // configuration
      enum {DRAW_DOTS, DRAW_BARS, DRAW_PINS, DRAW_SAMPLEHOLD, DRAW_LINES, NUM_DRAWINGMODES};
      bool autoscale;
      int drawingMode;
      double miny, maxy;
      double timeScale; // x scaling: secs/10 pixel
      simtime_t hairlineTime;   // t position of moving axis

   public:
      OutputVectorInspector(InspectorFactory *f);
      ~OutputVectorInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void refresh() override;
      virtual int inspectorCommand(int argc, const char **argv) override;

      // return textual information in general or about a value/value pair
      void generalInfo( char *buf );
      void valueInfo( char *buf, int valueindex );

      // configuration get (text form) / set
      void getConfig( char *buf );
      void setConfig( bool autoscale, double timef, double miny, double maxy, const char *mode);
};

} // namespace tkenv
}  // namespace omnetpp


#endif


