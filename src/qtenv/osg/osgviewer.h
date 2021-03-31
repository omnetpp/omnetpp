//==========================================================================
//  OSGVIEWER.H - part of
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

#ifndef __OMNETPP_QTENV_OSG_OSGVIEWER_H
#define __OMNETPP_QTENV_OSG_OSGVIEWER_H

#include "qtenv/iosgviewer.h"

#include "qtenv/qtenv.h"
#include "omnetpp/cosgcanvas.h"
#include "omnetpp/osgutil.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLContext>
#include <QtGui/QInputEvent>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osg/Camera>
#include <osgGA/TrackballManipulator>
#ifdef WITH_OSGEARTH
#include <osgEarthUtil/EarthManipulator>
#endif

class QMenu;

namespace omnetpp {
namespace qtenv {

// Responsible for rendering frames when necessary.
class HeartBeat : public QObject {
    Q_OBJECT
    osg::ref_ptr<osgViewer::CompositeViewer> viewer;

    // Only using a single global timer, so
    // the number of (superfluously) rendered frames
    // does not scale with the number of open views.
    // This is because imply calling frame() on the
    // viewer will redraw all the views.
    static HeartBeat *instance;

    HeartBeat(osg::ref_ptr<osgViewer::CompositeViewer> viewer, QObject *parent = nullptr);

    QBasicTimer timer;
    void timerEvent(QTimerEvent *event) override;

public:
    static void init(osg::ref_ptr<osgViewer::CompositeViewer> viewer);
    static void start();
    static void stop();
};

class OsgViewer : public IOsgViewer
{
    Q_OBJECT

    static osg::ref_ptr<osgViewer::CompositeViewer> viewer;

    osg::ref_ptr<osgViewer::GraphicsWindow> graphicsWindow;
    osg::ref_ptr<osgViewer::View> view;
    osg::ref_ptr<osg::Camera> camera;

    osg::ref_ptr<osg::Node> scene = nullptr;
    cOsgCanvas *osgCanvas = nullptr;

    // In local widget coords.
    // Needed to show the context menu only on release,
    // and only if there was no mouse drag.
    QPoint lastRightClick;
    // we have to remember this as a "starting point"
    // and subtract from all timestamp values before
    // passing it to OSG, because the values were sometimes
    // too freaking large (I mean 3e9 large), which
    // caused problems with camera manipulation even
    // after converting it to seconds.
    unsigned long firstInputEventTimestamp = 0;

    // gets the timestamp from the event, subtracts the
    // value above, and converts it to "normally" seconds
    double getTimestamp(QInputEvent *event);
    osg::CullSettings::ComputeNearFarMode defaultComputeNearFarMode;

    QAction *toTerrainManipulatorAction;
    QAction *toOverviewManipulatorAction;
    QAction *toTrackballManipulatorAction;
    QAction *toEarthManipulatorAction;

    static unsigned int mouseButtonQtToOsg(Qt::MouseButton button);

    void setClearColor(float r, float g, float b, float alpha);
    void setCameraManipulator(cOsgCanvas::CameraManipulatorType type, bool keepView = false);
    void setFieldOfViewAngle(double fovy);
    void setAspectRatio(double aspectRatio);
    void setZNearFar(double zNear, double zFar);
    QMenu *createCameraManipulatorMenu();

    void resizeGL(int width, int height) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // For some mysterious reason we have to scale the x coordinate
    // of mouse events with the widget aspect ratio. Tried to play
    // around with the event queue's setUseFixedMouseInputRange
    // and setMouseInputRange, but with no results.
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    bool event(QEvent *event) override;


    osgGA::EventQueue *getEventQueue() const;

  protected slots:
    void setCameraManipulator(QAction *sender); // will get the type from the QAction data

  public slots:
    void applyViewerHints() override;

  public:
    OsgViewer(QWidget *parent=nullptr);

    void setOsgCanvas(cOsgCanvas *canvas) override;
    cOsgCanvas *getOsgCanvas() const override {return osgCanvas;}

    void enable() override;
    void disable() override;

    float scaleFactor() const;
    float widgetAspectRatio() const;

    void refresh() override;
    void resetViewer() override;

    static void uninit();

    // coordinates in local widget frame
    std::vector<cObject *> objectsAt(const QPoint &pos) override;

    ~OsgViewer();

  signals:
    void objectsPicked(const std::vector<cObject*>&);
};

} // qtenv
} // omnetpp

#endif // __OMNETPP_QTENV_OSG_OSGVIEWER_H
