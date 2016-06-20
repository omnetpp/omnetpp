//==========================================================================
//  OSGVIEWER.H - part of
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

#ifndef __OMNETPP_QTENV_OSGVIEWER_H
#define __OMNETPP_QTENV_OSGVIEWER_H

#ifdef WITH_OSG

#include "qtenv.h"
#include "omnetpp/cosgcanvas.h"

#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QMenu>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osgEarthUtil/EarthManipulator>

namespace omnetpp {
namespace qtenv {

class PickHandler;

class QTENV_API OsgViewer : public QWidget, public osgViewer::CompositeViewer
{
    friend class PickHandler;

    Q_OBJECT   // this *must* be in a header file to be noticed by the moc!

  protected:
    cOsgCanvas *osgCanvas = nullptr;
    osgViewer::View *view = nullptr;
    QWidget *glWidget = nullptr;
    QTimer timer;
    osg::CullSettings::ComputeNearFarMode defaultComputeNearFarMode;

    QAction *toTerrainManipulatorAction;
    QAction *toOverviewManipulatorAction;
    QAction *toTrackballManipulatorAction;
    QAction *toEarthManipulatorAction;

    QWidget *addViewWidget();
    void paintEvent(QPaintEvent* event) override;

    bool event(QEvent *event) override;

    void setClearColor(float r, float g, float b, float alpha);
    void setCameraManipulator(cOsgCanvas::CameraManipulatorType type, bool keepView = false);
    void setFieldOfViewAngle(double fovy);
    void setAspectRatio(double aspectRatio);
    void setZNearFar(double zNear, double zFar);
    QMenu *createCameraManipulatorMenu();

  protected slots:
    void setCameraManipulator(QAction *sender); // will get the type from the QAction data

  public slots:
    void applyViewerHints();

  public:
    OsgViewer(QWidget *parent=nullptr);
    virtual ~OsgViewer();
    void setOsgCanvas(cOsgCanvas *canvas);
    cOsgCanvas *getOsgCanvas() const {return osgCanvas;}
    QWidget *getGLWidget() const {return glWidget;}
    void refresh();
    void resetViewer();

    // coordinates in local widget frame
    std::vector<cObject *> objectsAt(const QPoint &pos);

  signals:
    void objectsPicked(const std::vector<cObject*>&);
};

} // qtenv
} // omnetpp

#endif // WITH_OSG

#endif
