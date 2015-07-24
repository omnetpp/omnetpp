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

#include "osgviewer.h"

namespace omnetpp {
namespace qtenv {

OsgViewerWidget::OsgViewerWidget(QWidget *parent) : QWidget(parent)
{
    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);  //XXX crashes with Multithreaded

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    QWidget* widget1 = addViewWidget(createGraphicsWindow(0,0,100,100));

    QGridLayout* grid = new QGridLayout;
    grid->addWidget( widget1, 0, 0 );
    grid->setMargin(1);  // WARNING: don't set 0 or the 3D scene won't appear!

    setLayout( grid );

    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}

QWidget *OsgViewerWidget::addViewWidget(osgQt::GraphicsWindowQt *gw)
{
    view = new osgViewer::View();
    addView( view );

    osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );

    const osg::GraphicsContext::Traits* traits = gw->getTraits();

    // default settings (will be overridden from inspector's setObject())
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(120.0f, 1.0, 1.0f, 10000.0f );

    view->addEventHandler( new osgViewer::StatsHandler );

    //view->setCameraManipulator( new osgGA::TrackballManipulator );
    view->setCameraManipulator( new osgEarth::Util::EarthManipulator() );

    return gw->getGLWidget();
}

osgQt::GraphicsWindowQt *OsgViewerWidget::createGraphicsWindow(int x, int y, int w, int h, const std::string &name, bool windowDecoration)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    return new osgQt::GraphicsWindowQt(traits.get());
}

void OsgViewerWidget::paintEvent(QPaintEvent *event)
{
    printf("OsgViewerWidget::paintEvent()\n");
    frame();
}

void OsgViewerWidget::setScene(osg::Node* scene)
{
    view->setSceneData(scene);
}

osg::Node *OsgViewerWidget::getScene() const
{
    return view->getSceneData();
}

void OsgViewerWidget::setClearColor(float r, float g, float b, float alpha)
{
    osg::Camera* camera = view->getCamera();
    camera->setClearColor(osg::Vec4(r, g, b, alpha));
    camera->setProjectionMatrixAsPerspective(120.0f, 1.0, 1.0f, 10000.0f );
}

void OsgViewerWidget::setCameraManipulator(osgGA::CameraManipulator *manipulator)
{
    view->setCameraManipulator(manipulator);
}

void OsgViewerWidget::setPerspective(double fieldOfViewAngle, double aspect, double zNear, double zFar)
{
    osg::Camera* camera = view->getCamera();
    camera->setProjectionMatrixAsPerspective(fieldOfViewAngle, aspect, zNear, zFar);
}

} // qtenv
} // omnetpp

