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

#include "qtenv.h"

#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>

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
    cOsgCanvas *osgCanvas;
    osgViewer::View *view;
    QWidget *glWidget;
    QTimer timer;

  protected:
    QWidget *addViewWidget();
    virtual void paintEvent(QPaintEvent* event);

    void setClearColor(float r, float g, float b, float alpha);
    void setCameraManipulator(osgGA::CameraManipulator *manipulator);
    void setPerspective(double fieldOfViewAngle, double zNear, double zFar);
    void setEarthViewpoint(const osgEarth::Viewpoint& viewpoint);
    bool event(QEvent *);

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

#endif


