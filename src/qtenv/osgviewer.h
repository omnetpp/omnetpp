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

class QTENV_API OsgViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
    Q_OBJECT   // this *must* be in a header file to be noticed by the moc!

  protected:
    osgViewer::View *view;
    QTimer _timer;

  protected:
    QWidget *addViewWidget(osgQt::GraphicsWindowQt *gw);
    osgQt::GraphicsWindowQt *createGraphicsWindow(int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false);
    virtual void paintEvent(QPaintEvent* event);

  public:
    OsgViewerWidget(QWidget *parent=nullptr);
    virtual ~OsgViewerWidget() {}
    void setScene(osg::Node* scene);
    osg::Node *getScene() const;
    void setClearColor(float r, float g, float b, float alpha);
    void setCameraManipulator(osgGA::CameraManipulator *manipulator);
    void setPerspective(double fieldOfViewAngle, double aspect, double zNear, double zFar);
};

} // qtenv
} // omnetpp

#endif


