//==========================================================================
//  CANVASINSPECTOR.CC - part of
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

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "common/stringutil.h"
#include "canvasinspector.h"
#include "figurerenderers.h"
#include "qtutil.h"
#include "inspectorfactory.h"
#include "canvasrenderer.h"
#include "canvasviewer.h"
#include "qtenv.h"
#include "inspectorutil.h"
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtCore/QDebug>
#include <QtWidgets/QScrollBar>

#define emit

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

class CanvasInspectorFactory : public InspectorFactory
{
  public:
    CanvasInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cCanvas *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new CanvasInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(CanvasInspectorFactory);

CanvasInspector::CanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    canvasViewer = new CanvasViewer();
    canvasViewer->setRenderHint(QPainter::Antialiasing);

    auto contentLayout = new QVBoxLayout(this);
    contentLayout->addWidget(createToolbar());
    contentLayout->addWidget(canvasViewer);
    contentLayout->setMargin(0);

    connect(canvasViewer, SIGNAL(click(QMouseEvent *)), this, SLOT(onClick(QMouseEvent *)));
    connect(canvasViewer, SIGNAL(contextMenuRequested(QContextMenuEvent *)), this, SLOT(onContextMenuRequested(QContextMenuEvent *)));
}

QToolBar *CanvasInspector::createToolbar()
{
    QToolBar *toolbar = new QToolBar(this);
    addTopLevelToolBarActions(toolbar);

    toolbar->addSeparator();

    // canvas-specfic
    QAction *action = toolbar->addAction(QIcon(":/tools/redraw"), "Redraw (Ctrl+R)", this, SLOT(redraw()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    action = toolbar->addAction(QIcon(":/tools/zoomin"), "Zoom in (Ctrl+Plus)", this, SLOT(zoomIn()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
    action = toolbar->addAction(QIcon(":/tools/zoomout"), "Zoom out (Ctrl+Minus)", this, SLOT(zoomOut()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));

    toolbar->setAutoFillBackground(true);

    return toolbar;
}

void CanvasInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    if (object) {
        canvasViewer->setObject(getCanvas());
        QVariant zoomFactorVariant = getQtenv()->getPref(object->getFullName() + QString(":") + INSP_DEFAULT + ":zoomfactor");
        double zoomFactor = zoomFactorVariant.isValid() ? zoomFactorVariant.value<double>() : 1;
        canvasViewer->setZoomFactor(zoomFactor);
    }

    redraw();
}

void CanvasInspector::refresh()
{
    Inspector::refresh();

    canvasViewer->refresh();
}

void CanvasInspector::redraw()
{
    canvasViewer->redraw();
}

void CanvasInspector::postRefresh()
{
    if (object)
        getCanvas()->getRootFigure()->clearChangeFlags();
}

cCanvas *CanvasInspector::getCanvas()
{
    return static_cast<cCanvas *>(object);
}

void CanvasInspector::zoomIn()
{
    QVariant variant = getQtenv()->getPref("zoomby-factor");
    double zoomByFactor = variant.isValid() ? variant.value<double>() : 1.3;
    zoomBy(zoomByFactor);
}

void CanvasInspector::zoomOut()
{
    QVariant variant = getQtenv()->getPref("zoomby-factor");
    double zoomByFactor = variant.isValid() ? variant.value<double>() : 1.3;
    zoomBy(1.0 / zoomByFactor);
}

void CanvasInspector::zoomBy(double mult)
{
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    double zoomFactor = variant.isValid() ? variant.value<double>() : 1;

    if ((mult < 1 && zoomFactor > 0.001) || (mult > 1 && zoomFactor < 1000)) {
        // update zoom factor and redraw
        double newZoomFactor = zoomFactor * mult;

        // Snaptoone always 1
        //snap to true (note: this is not desirable when zoom is set programmatically to fit network into window)
        /*if(snaptoone)
        { // this code constantly kept the factor at 1...
            double m = mult < 1 ? 1.0/mult : mult;
            double a = 1 - 0.9*(1 - 1.0/m);
            double b = 1 + 0.9*(m - 1);
            if (zoomFactor > a && zoomFactor < b)
                newZoomFactor = 1;
        }*/

        getQtenv()->setPref(prefName, newZoomFactor);
        canvasViewer->setZoomFactor(newZoomFactor);
        redraw();
    }
}

void CanvasInspector::onClick(QMouseEvent *event)
{
    auto objects = canvasViewer->getObjectsAt(event->pos());
    if (!objects.empty())
        getQtenv()->onSelectionChanged(objects.front());
}

void CanvasInspector::onContextMenuRequested(QContextMenuEvent *event)
{
    std::vector<cObject *> objects = canvasViewer->getObjectsAt(event->pos());

    if (objects.size()) {
        QMenu *menu = InspectorUtil::createInspectorContextMenu(QVector<cObject *>::fromStdVector(objects), this);

        menu->addSeparator();
        // TODO Create Select Layers dialog
        menu->addAction("Show/Hide Canvas Layers...", this, SLOT(layers()));

        menu->addSeparator();

        menu->addAction("Zoom In", this, SLOT(zoomIn()), QKeySequence(Qt::CTRL + Qt::Key_Plus));
        menu->addAction("Zoom Out", this, SLOT(zoomOut()), QKeySequence(Qt::CTRL + Qt::Key_Minus));

        menu->exec(event->globalPos());
        delete menu;
    }
}

}  // namespace omnetpp
}  // namespace omnetpp

