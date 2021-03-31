//==========================================================================
//  IOSGVIEWER.H - part of
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

#ifndef __OMNETPP_QTENV_IOSGVIEWER_H
#define __OMNETPP_QTENV_IOSGVIEWER_H

#include "qtenv.h"
#include "common/ver.h"

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

class QMenu;

namespace osg { class Node; }

namespace omnetpp {

class cOsgCanvas;

namespace qtenv {

class IOsgViewer;

class QTENV_API IOsgViewer : public QOpenGLWidget
{
    Q_OBJECT

    static void ensureViewerFactory();

  public slots:
    virtual void applyViewerHints() = 0;

  public:

    IOsgViewer(QWidget *parent=nullptr): QOpenGLWidget(parent) { /* empty */ }

    // static interface for easy access, delegates to the factory, loads the library on-demand
    static IOsgViewer *createOne();
    static void uninit();

    static void refNode(osg::Node *node);
    static void unrefNode(osg::Node *node);

    // when opening up a new moduleinspector, and there is no saved preference
    // for which mode (canvas or OSG) we should show first, then we should only
    // default for OSG if there is actually a working viewer implementation loaded.
    static bool isOsgPreferred();

    virtual void setOsgCanvas(cOsgCanvas *canvas) = 0;
    virtual cOsgCanvas *getOsgCanvas() const = 0;

    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual void refresh() = 0;
    virtual void resetViewer() = 0;

    // coordinates in local widget frame
    virtual std::vector<cObject *> objectsAt(const QPoint &pos) = 0;

  signals:
    void objectsPicked(const std::vector<cObject*>&);
};

class QTENV_API IOsgViewerFactory : noncopyable
{
public:
    virtual IOsgViewer *createViewer() = 0;
    virtual void shutdown() = 0;

    virtual void refNode(osg::Node *node) = 0;
    virtual void unrefNode(osg::Node *node) = 0;

    virtual ~IOsgViewerFactory() {}
};


class QTENV_API DummyOsgViewer: public IOsgViewer
{
    Q_OBJECT

  public slots:
    void applyViewerHints() override {}

  public:

    DummyOsgViewer(QWidget *parent=nullptr): IOsgViewer(parent) {}

    void setOsgCanvas(cOsgCanvas *) override {}
    cOsgCanvas *getOsgCanvas() const override { return nullptr; }

    void enable() override {}
    void disable() override {}

    void refresh() override {}
    void resetViewer() override {}

    void paintEvent(QPaintEvent *) override;

    std::vector<cObject *> objectsAt(const QPoint &) override { return {}; }

  signals:
    void objectsPicked(const std::vector<cObject*>&);
};


} // qtenv
} // omnetpp

#endif // __OMNETPP_QTENV_IOSGVIEWER_H
