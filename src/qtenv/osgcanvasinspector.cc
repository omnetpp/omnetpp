//==========================================================================
//  OSGCANVASINSPECTOR.CC - part of
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

#include "qtenvdefs.h"

#include "inspectorfactory.h"
#include "osgcanvasinspector.h"
#include "iosgviewer.h"
#include "omnetpp/cosgcanvas.h"

namespace omnetpp {
namespace qtenv {

class OsgCanvasInspectorFactory : public InspectorFactory
{
  public:
    OsgCanvasInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cOsgCanvas *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new OsgCanvasInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(OsgCanvasInspectorFactory);

OsgCanvasInspector::OsgCanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    osgViewer = IOsgViewer::createOne();
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(osgViewer, 0, 0);
    grid->setMargin(0);
    setLayout(grid);
}

void OsgCanvasInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    cOsgCanvas *osgCanvas = dynamic_cast<cOsgCanvas *>(obj);
    osgViewer->setOsgCanvas(osgCanvas);  // note: increases scene's reference count
}

void OsgCanvasInspector::refresh()
{
    Inspector::refresh();
    osgViewer->refresh();
}

}  // namespace qtenv
}  // namespace omnetpp
