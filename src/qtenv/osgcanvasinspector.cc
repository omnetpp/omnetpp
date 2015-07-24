//==========================================================================
//  OSGCANVASINSPECTOR.CC - part of
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

#include "qtenvdefs.h"

#include "inspectorfactory.h"
#include "osgcanvasinspector.h"
#include "osgviewer.h"
#include "omnetpp/cosgcanvas.h"
#include <osgDB/ReadFile>  //TMP

namespace omnetpp {
namespace qtenv {

class OsgCanvasInspectorFactory : public InspectorFactory
{
  public:
    OsgCanvasInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cOsgCanvas*>(obj) != nullptr; }
    int getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new OsgCanvasInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(OsgCanvasInspectorFactory);

OsgCanvasInspector::OsgCanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    viewWidget = new OsgViewerWidget();
    QGridLayout *grid = new QGridLayout;
    grid->addWidget( viewWidget, 0, 0 );
    grid->setMargin(0);
    setLayout( grid );
}

OsgCanvasInspector::~OsgCanvasInspector()
{
}

void OsgCanvasInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    //XXX test:
    //scene = osgDB::readNodeFile("cow.osgt");
    //osg::Node* scene = osgDB::readNodeFile("/usr/share/osgearth/maps/boston.earth");
    //viewWidget->setScene(scene);

    cOsgCanvas *osgCanvas = dynamic_cast<cOsgCanvas*>(obj);
    osg::Node *scene = osgCanvas ? osgCanvas->getScene() : nullptr;
    viewWidget->setScene(scene); // note: increases scene's reference count

    cOsgCanvas::Color color = osgCanvas->getClearColor();
    viewWidget->setClearColor(color.red/255.0, color.green/255.0, color.blue/255.0, 1.0);

    osgGA::CameraManipulator *manipulator = nullptr;
    switch (osgCanvas->getCameraManipulatorType()) {
    case cOsgCanvas::CAM_TRACKBALL: manipulator = new osgGA::TrackballManipulator; break;
    case cOsgCanvas::CAM_EARTH: manipulator = new osgEarth::Util::EarthManipulator; break;
    }
    viewWidget->setCameraManipulator(manipulator);

    viewWidget->setPerspective(osgCanvas->getFieldOfViewAngle(), osgCanvas->getAspect(), osgCanvas->getZNear(), osgCanvas->getZFar());

}

void OsgCanvasInspector::refresh()
{
    cOsgCanvas *osgCanvas = dynamic_cast<cOsgCanvas*>(getObject());
    osg::Node *scene = osgCanvas ? osgCanvas->getScene() : nullptr;
    if (scene != viewWidget->getScene())
        viewWidget->setScene(scene);
}

} // namespace qtenv
} // namespace omnetpp
