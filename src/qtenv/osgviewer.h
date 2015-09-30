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

    QAction *toTerrainManipulatorAction;
    QAction *toTrackballManipulatorAction;
    QAction *toEarthManipulatorAction;

  protected:
    QWidget *addViewWidget();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

    void setClearColor(float r, float g, float b, float alpha);
    void setCameraManipulator(cOsgCanvas::CameraManipulatorType type);
    void setPerspective(double fieldOfViewAngle, double zNear, double zFar);
    void setEarthViewpoint(const osgEarth::Viewpoint& viewpoint);
    QMenu *createCameraManipulatorMenu();

  protected slots:
    void setCameraManipulator(QAction *sender);

  public:
    OsgViewer(QWidget *parent=nullptr);
    virtual ~OsgViewer() {}
    void setOsgCanvas(cOsgCanvas *canvas);
    cOsgCanvas *getOsgCanvas() const {return osgCanvas;}
    void refresh();
    void applyViewerHints();
    void resetViewer();

  signals:
    void objectsPicked(const std::vector<cObject*>&);
};

} // qtenv
} // omnetpp

#endif // WITH_OSG

#endif
