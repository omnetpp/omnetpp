//==========================================================================
//  OSGCANVASINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_OSGCANVASINSPECTOR_H
#define __OMNETPP_QTENV_OSGCANVASINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace qtenv {

class OsgViewerWidget;

class QTENV_API OsgCanvasInspector : public Inspector
{
    Q_OBJECT

   private:
      OsgViewerWidget *viewWidget;

   public:
      OsgCanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~OsgCanvasInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;
};

} // namespace qtenv
} // namespace omnetpp

#endif

