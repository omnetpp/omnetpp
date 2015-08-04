//==========================================================================
//  OSGVIEWER.CC - part of
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

#include <osgEarthUtil/EarthManipulator>
#include "omnetpp/cosgcanvas.h"
#include "osgviewer.h"

namespace omnetpp {
namespace qtenv {

OsgViewer::OsgViewer(QWidget *parent) : QWidget(parent)
{
    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);  //XXX crashes with Multithreaded

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    QWidget *glWidget = addViewWidget();

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(glWidget, 0, 0);
    grid->setMargin(1);  // WARNING: don't set 0 or the 3D scene won't appear!
    setLayout(grid);

    // set up periodic update of 3D view
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(10);
}

QWidget *OsgViewer::addViewWidget()
{
    // create an OSG viewer (NOT part of the Qt widget tree!)
    view = new osgViewer::View();
    addView(view);

    // configure viewer and its camera with some default settings
    // (they will be overwritten from inspector hints later)
    osg::Camera *camera = view->getCamera();
    camera->setClearColor(osg::Vec4(0.9, 0.9, 0.9, 1.0));
    camera->setViewport(new osg::Viewport(0, 0, 100, 100));
    camera->setProjectionMatrixAsPerspective(120.0f, 1.0, 1.0f, 10000.0f);
    view->addEventHandler(new osgViewer::StatsHandler);
    view->setCameraManipulator(new osgGA::TrackballManipulator);

    // create an OSG "graphics window" where camera will render the scene;
    // the graphics window wraps a QGLWidget, and we'll add THAT to the Qt widget tree
    osg::DisplaySettings *ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = "";
    traits->windowDecoration = false;
    traits->x = 0;
    traits->y = 0;
    traits->width = 100;
    traits->height = 100;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    osgQt::GraphicsWindowQt *graphicsWindow = new osgQt::GraphicsWindowQt(traits.get());
    camera->setGraphicsContext(graphicsWindow);
    return graphicsWindow->getGLWidget();
}

void OsgViewer::paintEvent(QPaintEvent *event)
{
    //printf("OsgViewerWidget::paintEvent()\r");
    frame();
}

void OsgViewer::setOsgCanvas(cOsgCanvas *canvas)
{
    if (osgCanvas != canvas) {
        osgCanvas = canvas;
        refresh();
        if (osgCanvas != nullptr)
            applyViewerHints();
        else
            resetViewer();
    }
}

void OsgViewer::refresh()
{
    osg::Node *scene = osgCanvas ? osgCanvas->getScene() : nullptr;
    if (scene != view->getSceneData())
        view->setSceneData(scene);
}

void OsgViewer::applyViewerHints()
{
    printf("applyViewerHints()\n");
    ASSERT(osgCanvas != nullptr);

    cOsgCanvas::Color color = osgCanvas->getClearColor();
    setClearColor(color.red/255.0, color.green/255.0, color.blue/255.0, 1.0);

    osgGA::CameraManipulator *manipulator = nullptr;
    auto manipulatorType = osgCanvas->getCameraManipulatorType();
    if (manipulatorType == cOsgCanvas::CAM_AUTO)
        manipulatorType = osgCanvas->getViewerStyle()==cOsgCanvas::STYLE_GENERIC ? cOsgCanvas::CAM_TRACKBALL : cOsgCanvas::CAM_EARTH;
    switch (manipulatorType) {
        case cOsgCanvas::CAM_TRACKBALL: manipulator = new osgGA::TrackballManipulator; break;
        case cOsgCanvas::CAM_EARTH: manipulator = new osgEarth::Util::EarthManipulator; break;
    }
    setCameraManipulator(manipulator);

    setPerspective(osgCanvas->getFieldOfViewAngle(), osgCanvas->getAspect(), osgCanvas->getZNear(), osgCanvas->getZFar());

    if (osgCanvas->getViewerStyle() == cOsgCanvas::STYLE_EARTH)
        setEarthViewpoint(osgCanvas->getEarthViewpoint());
}

void OsgViewer::resetViewer()
{
    printf("resetViewer()\n");
    setClearColor(0.9, 0.9, 0.9, 1.0);
    setCameraManipulator(nullptr);
    setPerspective(30, 1.0, 1, 1000);
}

void OsgViewer::setClearColor(float r, float g, float b, float alpha)
{
    osg::Camera *camera = view->getCamera();
    camera->setClearColor(osg::Vec4(r, g, b, alpha));
}

void OsgViewer::setCameraManipulator(osgGA::CameraManipulator *manipulator)
{
    view->setCameraManipulator(manipulator);
}

void OsgViewer::setPerspective(double fieldOfViewAngle, double aspect, double zNear, double zFar)
{
    osg::Camera *camera = view->getCamera();
    camera->setProjectionMatrixAsPerspective(fieldOfViewAngle, aspect, zNear, zFar);
}

void OsgViewer::setEarthViewpoint(const osgEarth::Viewpoint& viewpoint)
{
    if (viewpoint.isValid()) {
        osgGA::CameraManipulator *manip = view->getCameraManipulator();
        if (osgEarth::Util::EarthManipulator *earthManip = dynamic_cast<osgEarth::Util::EarthManipulator*>(manip)) {
            double duration = 0.5; //XXX or something
            earthManip->setViewpoint(viewpoint, duration);
        }
    }
}

} // qtenv
} // omnetpp

