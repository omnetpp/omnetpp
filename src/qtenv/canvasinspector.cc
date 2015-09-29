//==========================================================================
//  CANVASINSPECTOR.CC - part of
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

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "common/stringutil.h"
#include "canvasinspector.h"
#include "figurerenderers.h"
#include "tklib.h"
#include "qtutil.h"
#include "inspectorfactory.h"
#include "canvasrenderer.h"
#include "canvasviewer.h"
#include "qtenv.h"
#include "inspectorutil.h"
#include <QGridLayout>
#include <QToolBar>
#include <QAction>
#include <QMouseEvent>
#include <QMenu>
#include <QDebug>
#include <QLabel>
#include <QScrollBar>

#define emit

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

class ZoomLabel : public QLabel
{
private:
    double zoomFactor;
    int h = -1;

public:
    ZoomLabel(QFont font)
    {
        font.setBold(true);
        setFont(font);
        setAutoFillBackground(true);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, QColor("lightgrey"));
        setPalette(pal);
    }

    void setZoomFactor(double zoomFactor)
    {
        if(this->zoomFactor == zoomFactor)
            return;

        this->zoomFactor = zoomFactor;
        setText(" Zoom:" + QString::number(zoomFactor, 'f', 2) + "x");
        update();
    }

    int height()
    {
        if(h == -1)
            h = QLabel::height();

        return h;
    }
};

class CanvasInspectorFactory : public InspectorFactory
{
  public:
    CanvasInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cCanvas *>(obj) != nullptr; }
    int getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new CanvasInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(CanvasInspectorFactory);

CanvasInspector::CanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    canvasViewer = new CanvasViewer();
    canvasViewer->setRenderHint(QPainter::Antialiasing);

    zoomLabel = new ZoomLabel(canvasViewer->scene()->font());

    QWidget *contentArea = new QWidget();
    floatingLayout = new FloatingToolbarLayout(this);
    floatingLayout->addWidget(contentArea);
    floatingLayout->addWidget(zoomLabel);

    auto contentLayout = new QVBoxLayout(contentArea);
    contentLayout->addWidget(createToolbar());
    contentLayout->addWidget(canvasViewer);
    contentLayout->setMargin(0);

    connect(canvasViewer, SIGNAL(click(QMouseEvent*)), this, SLOT(onClick(QMouseEvent*)));
    connect(canvasViewer, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this, SLOT(onContextMenuRequested(QContextMenuEvent*)));
    QTimer::singleShot(0, this, SLOT(setZoomLabelMargins()));
}

CanvasInspector::~CanvasInspector()
{
}

QToolBar *CanvasInspector::createToolbar()
{
    QToolBar *toolbar = new QToolBar(this);
    addTopLevelToolBarActions(toolbar);

    toolbar->addSeparator();

    // canvas-specfic
    //TODO QAction::eventFilter: Ambiguous shortcut overload: Ctrl+M
    QAction *action = toolbar->addAction(QIcon(":/tools/icons/tools/redraw.png"), "Redraw (Ctrl+R)", this, SLOT(redraw()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    action = toolbar->addAction(QIcon(":/tools/icons/tools/zoomin.png"), "Zoom in (Ctrl+M)", this, SLOT(zoomIn()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    action = toolbar->addAction(QIcon(":/tools/icons/tools/zoomout.png"), "Zoom out (Ctrl+N)", this, SLOT(zoomOut()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

    toolbar->setAutoFillBackground(true);

    return toolbar;
}

void CanvasInspector::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    setZoomLabelMargins();
}

//TODO label position when scrollbar is visible
void CanvasInspector::setZoomLabelMargins()
{
    QScrollBar *scrollBar = canvasViewer->horizontalScrollBar();
    int top = scrollBar->isVisible() ? scrollBar->height() + 2 : 0;

    scrollBar = canvasViewer->verticalScrollBar();
    int right = scrollBar->isVisible() ? scrollBar->width() + 2: 0;
    floatingLayout->setToolbarMargins(QMargins(0, height() - zoomLabel->height() - top - 3, right + 3, 0));
}

void CanvasInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    canvasViewer->setObject(getCanvas());
    QVariant zoomFactorVariant = getQtenv()->getPref(object->getFullName() + QString(":") + INSP_DEFAULT + ":zoomfactor");
    double zoomFactor = zoomFactorVariant.isValid() ? zoomFactorVariant.value<double>() : 1;
    zoomLabel->setZoomFactor(zoomFactor);

    if (object)
        redraw();
}

void CanvasInspector::refresh()
{
    Inspector::refresh();

    if (!object) {
        //TCLKILL CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
        return;
    }

    canvasViewer->refresh();
}

void CanvasInspector::redraw()
{
    canvasViewer->redraw();
}

void CanvasInspector::clearObjectChangeFlags()
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

    if((mult < 1 && zoomFactor > 0.001) || (mult > 1 && zoomFactor < 1000))
    {
        //update zoom factor and redraw
        double newZoomFactor = zoomFactor * mult;

        // Snaptoone always 1
        //snap to true (note: this is not desirable when zoom is set programmatically to fit network into window)
        /*if(snaptoone)
        { // this code constantly kept the factor at 1...
            double m = mult < 1 ? 1.0/mult : mult;
            double a = 1 - 0.9*(1 - 1.0/m);
            double b = 1 + 0.9*(m - 1);
            if(zoomFactor > a && zoomFactor < b)
                newZoomFactor = 1;
        }*/

        getQtenv()->setPref(prefName, newZoomFactor);
        zoomLabel->setZoomFactor(newZoomFactor);
        redraw();
        setZoomLabelMargins();
    }
}

void CanvasInspector::onClick(QMouseEvent *event) {
    auto objects = canvasViewer->getObjectsAt(event->pos());
    if (!objects.empty())
        getQtenv()->onSelectionChanged(objects.front());
}

void CanvasInspector::onContextMenuRequested(QContextMenuEvent *event)
{
    std::vector<cObject*> objects = canvasViewer->getObjectsAt(event->pos());

    if(objects.size())
    {
        QMenu *menu = InspectorUtil::createInspectorContextMenu(QVector<cObject*>::fromStdVector(objects), this);

        menu->addSeparator();
        // TODO Create Select Layers dialog
        menu->addAction("Show/Hide Canvas Layers...", this, SLOT(layers()));

        menu->addSeparator();

        menu->addAction("Zoom In", this, SLOT(zoomIn()), QKeySequence(Qt::CTRL + Qt::Key_M));
        menu->addAction("Zoom Out", this, SLOT(zoomOut()), QKeySequence(Qt::CTRL + Qt::Key_N));

        menu->exec(event->globalPos());
        delete menu;
    }

}

/*TCLKILL
int CanvasInspector::inspectorCommand(int argc, const char **argv)
{
    if (argc < 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    E_TRY
    if (strcmp(argv[0], "redraw") == 0) {
        redraw();
        return TCL_OK;
    }
    if (strcmp(argv[0], "getalltags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getAllTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    if (strcmp(argv[0], "getenabledtags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getEnabledTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    if (strcmp(argv[0], "getexcepttags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getExceptTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    if (strcmp(argv[0], "setenabledtags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setEnabledTags(argv[1]);
        return TCL_OK;
    }
    if (strcmp(argv[0], "setexcepttags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setExceptTags(argv[1]);
        return TCL_OK;
    }
    E_CATCH

    return Inspector::inspectorCommand(argc, argv);
}
*/
} // namespace omnetpp
} // namespace omnetpp

