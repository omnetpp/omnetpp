//==========================================================================
//  MODULEINSPECTOR.CC - part of
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
#include <QMessageBox>
#include <QBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QGraphicsItem>
#include <QTimer>
#include <QEventLoop>
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cosgcanvas.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cgate.h"
#include "moduleinspector.h"
#include "genericobjectinspector.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "arrow.h"
#include "canvasrenderer.h"
#include "osgviewer.h"
#include "mainwindow.h"
#include "modulecanvasviewer.h"
#include "inspectorutil.h"

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

/**
 * Layout that manages two children: a client that completely fills the
 * parent; and a toolbar that flots over the client in the top-right corner.
 */
class FloatingToolbarLayout : public QLayout
{
public:
    FloatingToolbarLayout(): QLayout(), child(nullptr), toolbar(nullptr) {}
    FloatingToolbarLayout(QWidget *parent): QLayout(parent), child(nullptr), toolbar(nullptr) {}
    ~FloatingToolbarLayout() {
        delete child;
        delete toolbar;
    }

    void addItem(QLayoutItem *item) {
        if (!child) child = item;
        else if (!toolbar) toolbar = item;
        else throw std::runtime_error("only two items are accepted");
    }

    int count() const {
        if (!child) return 0;
        else if (!toolbar) return 1;
        else return 2;
    }

    QLayoutItem *itemAt(int i) const {
        if (i==0) return child;
        else if (i==1) return toolbar;
        else return nullptr;
    }

    QLayoutItem *takeAt(int i) {
        QLayoutItem *result = nullptr;
        if (i==1) {result = toolbar; toolbar = nullptr;}
        else if (i==0) {result = child; child = toolbar; toolbar = nullptr;}
        else throw std::runtime_error("illegal index, must be 0 or 1");
        return result;
    }

    QSize sizeHint() const {
        if (child) return child->sizeHint();
        else return QSize(100,100);
    }

    QSize minimumSize() const {
        if (child) return child->minimumSize();
        else return QSize(0,0);
    }

    void setGeometry(const QRect &rect) {
        if (child)
            child->setGeometry(rect); // margin?
        if (toolbar) {
            QSize size = toolbar->sizeHint();
            int x = std::max(dx, rect.width() - size.width() - dx);
            int width = std::min(rect.width() - 2*dx, size.width());
            toolbar->setGeometry(QRect(x, dy, width, size.height()));
        }
    }

private:
    QLayoutItem *child;
    QLayoutItem *toolbar;
    const int dx = 5, dy = 5; // toolbar distance from edges
};


void _dummy_for_moduleinspector() {}

class ModuleInspectorFactory : public InspectorFactory
{
  public:
    ModuleInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cModule *>(obj) != nullptr; }
    int getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override
    {
        cModule *mod = dynamic_cast<cModule *>(object);
        return mod && mod->hasSubmodules() ? 3.0 : 0.9;
    }

    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new ModuleInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(ModuleInspectorFactory);

ModuleInspector::ModuleInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    switchToOsgViewAction = nullptr;
    switchToCanvasViewAction = nullptr;

    createViews(this);
    parent->setMinimumSize(20, 20);
}

ModuleInspector::~ModuleInspector()
{
    // so the window can be closed safely, without
    // double deleting the message items (by the
    // animations and the scene itself)
    getQtenv()->getAnimator()->clearInspector(this);
}

void ModuleInspector::createViews(QWidget *parent)
{
    canvasViewer = new ModuleCanvasViewer();
    canvasViewer->setRenderHints(QPainter::Antialiasing);

    connect(canvasViewer, SIGNAL(back()), this, SLOT(goBack()));
    connect(canvasViewer, SIGNAL(forward()), this, SLOT(goForward()));
    connect(canvasViewer, SIGNAL(click(QMouseEvent*)), this, SLOT(click(QMouseEvent*)));
    connect(canvasViewer, SIGNAL(doubleClick(QMouseEvent*)), this, SLOT(doubleClick(QMouseEvent*)));
    connect(canvasViewer, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this, SLOT(createContextMenu(QContextMenuEvent*)));

    osgViewer = new OsgViewer();

    connect(osgViewer, SIGNAL(objectsPicked(const std::vector<cObject*>&)), this, SLOT(objectsPicked(const std::vector<cObject*>&)));

    QToolBar *toolbar = createToolbar();
    QWidget *contentArea = new QWidget();
    auto layout = new FloatingToolbarLayout(parent);
    layout->addWidget(contentArea);
    layout->addWidget(toolbar);

    stackedLayout = new QStackedLayout(contentArea);
    stackedLayout->addWidget(osgViewer);
    stackedLayout->addWidget(canvasViewer);
}

QToolBar *ModuleInspector::createToolbar()
{
    QToolBar *toolbar = new QToolBar();

    // general
    toolbar->addAction(QIcon(":/tools/icons/tools/back.png"), "Back", this, SLOT(goBack()));
    toolbar->addAction(QIcon(":/tools/icons/tools/forward.png"), "Forward", this, SLOT(goForward()));
    toolbar->addAction(QIcon(":/tools/icons/tools/parent.png"), "Go to parent module", this, SLOT(inspectParent()));
    toolbar->addSeparator();

    toolbar->addAction(QIcon(":/tools/icons/tools/mrun.png"), "Run until next event in this module", this, SLOT(runUntil()));
    QAction *action = toolbar->addAction(QIcon(":/tools/icons/tools/mfast.png"), "Fast run until next event in this module (Ctrl+F4)", this, SLOT(fastRunUntil()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
    action = toolbar->addAction(QIcon(":/tools/icons/tools/stop.png"), "Stop the simulation (F8)", this, SLOT(stopSimulation()));
    toolbar->addSeparator();

    // canvas-specfic
    action = toolbar->addAction(QIcon(":/tools/icons/tools/redraw.png"), "Re-layout (Ctrl+R)", this, SLOT(relayout()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    canvasRelayoutAction = action;
    action = toolbar->addAction(QIcon(":/tools/icons/tools/zoomin.png"), "Zoom in (Ctrl+M)", this, SLOT(zoomIn()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    canvasZoomInAction = action;
    action = toolbar->addAction(QIcon(":/tools/icons/tools/zoomout.png"), "Zoom out (Ctrl+N)", this, SLOT(zoomOut()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    canvasZoomOutAction = action;

    // osg-specific
    action = toolbar->addAction(QIcon(":/tools/icons/tools/reset.png"), "Reset view", this, SLOT(resetOsgView()));
    resetOsgViewAction = action;
    toolbar->addSeparator();

    // mode switching
    action = toolbar->addAction(QIcon(":/tools/icons/tools/3dscene.png"), "3D Scene", this, SLOT(switchToOsgView()));
    action->setCheckable(true);
    switchToOsgViewAction = action;

    action = toolbar->addAction(QIcon(":/tools/icons/tools/modulegraphics.png"), "Module", this, SLOT(switchToCanvasView()));
    action->setCheckable(true);
    switchToCanvasViewAction = action;

    toolbar->setAutoFillBackground(true);

    return toolbar;
}


void ModuleInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    getQtenv()->getAnimator()->clearInspector(this);

    Inspector::doSetObject(obj);

    cModule *module = dynamic_cast<cModule*>(obj);

    canvasViewer->setObject(module);
    canvasViewer->clear();

    if (object) {
        canvasViewer->setLayoutSeed(1);  // we'll read the "bgl" display string tag from Tcl
        // TODO
        //    ModuleInspector:recallPreferences $insp

        try {
            canvasViewer->relayoutAndRedrawAll();

            // XXX TODO this is the recallPreferences part from above, but there might be a prettier solution
            QString objName = object->getFullName();
            QVariant zoomFactorVariant = getQtenv()->getPref(objName + ":" + INSP_DEFAULT + ":zoomfactor");
            double zoomFactor = zoomFactorVariant.isValid() ? zoomFactorVariant.value<double>() : 1;
            canvasViewer->setZoomFactor(zoomFactor);

            QVariant imageSizeVariant = getQtenv()->getPref(objName + ":" + INSP_DEFAULT + ":imagesizefactor");
            double imageSizeFactor = imageSizeVariant.isValid() ? imageSizeVariant.value<double>() : 1;
            canvasViewer->setImageSizeFactor(imageSizeFactor);
        }
        catch (std::exception& e) {
            QMessageBox::warning(this, QString("Error"), QString("Error displaying network graphics: ") + e.what());
        }

        // so they will appear on the correct places with the updated zoom levels.
        getQtenv()->getAnimator()->redrawMessages();
    }

    cOsgCanvas *osgCanvas = getOsgCanvas();
    setOsgCanvas(osgCanvas);
}

cCanvas *ModuleInspector::getCanvas()
{
    cModule *mod = static_cast<cModule *>(object);
    cCanvas *canvas = mod ? mod->getCanvasIfExists() : nullptr;
    return canvas;
}

void ModuleInspector::setOsgCanvas(cOsgCanvas *osgCanvas)
{
    osgViewer->setOsgCanvas(osgCanvas);

    switchToOsgViewAction->setEnabled(osgCanvas != nullptr);

    if (osgCanvas != nullptr)
        switchToOsgView();
    else
        switchToCanvasView();
}

cOsgCanvas *ModuleInspector::getOsgCanvas()
{
    cModule *module = dynamic_cast<cModule*>(getObject());
    cOsgCanvas *osgCanvas = module ? module->getOsgCanvasIfExists() : nullptr;
    return osgCanvas;
}

void ModuleInspector::refresh()
{
    Inspector::refresh();

    canvasViewer->refresh();
    refreshOsgViewer();
}

void ModuleInspector::refreshOsgViewer()
{
    cOsgCanvas *osgCanvas = getOsgCanvas();
    if (osgViewer->getOsgCanvas() != osgCanvas)
        setOsgCanvas(osgCanvas);
    else
        osgViewer->refresh();
}

void ModuleInspector::redraw()
{
    canvasViewer->redraw();
}

void ModuleInspector::clearObjectChangeFlags()
{
    cCanvas *canvas = getCanvas();
    if (canvas)
        canvas->getRootFigure()->clearChangeFlags();
}


bool ModuleInspector::needsRedraw()
{
    return canvasViewer->getNeedsRedraw();
}

void ModuleInspector::runUntil()
{
    runSimulationLocal(qtenv::Qtenv::eRunMode::RUNMODE_NORMAL);
}

void ModuleInspector::fastRunUntil()
{
    runSimulationLocal(qtenv::Qtenv::eRunMode::RUNMODE_FAST);
}

void ModuleInspector::stopSimulation()
{
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    mainWindow->on_actionStop_triggered();
}

//Relayout the compound module, and resize the window accordingly.

void ModuleInspector::relayout()
{
    //TODO
//    global config inspectordata

//    set c $insp.c
    canvasViewer->incLayoutSeed();
    canvasViewer->relayoutAndRedrawAll();

//    if {[opp_inspector_istoplevel $insp] && $config(layout-may-resize-window)} {
//        wm geometry $insp ""
//    }

//    ModuleInspector:adjustWindowSizeAndZoom $insp
}

void ModuleInspector::zoomIn(int x, int y)
{
    QVariant variant = getQtenv()->getPref("zoomby-factor");
    double zoomByFactor = variant.isValid() ? variant.value<double>() : 1.3;
    zoomBy(zoomByFactor, 1, x, y);
}

void ModuleInspector::zoomOut(int x, int y)
{
    QVariant variant = getQtenv()->getPref("zoomby-factor");
    double zoomByFactor = variant.isValid() ? variant.value<double>() : 1.3;
    zoomBy(1.0 / zoomByFactor, 1, x, y);

}

void ModuleInspector::increaseIconSize() {
    zoomIconsBy(1.25);
}

void ModuleInspector::decreaseIconSize() {
    zoomIconsBy(0.8);
}

void ModuleInspector::zoomBy(double mult, bool snaptoone, int x, int y)
{
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    double zoomFactor = variant.isValid() ? variant.value<double>() : 1;

    if((mult < 1 && zoomFactor > 0.001) || (mult > 1 && zoomFactor < 1000))
    {
        //remember canvas scroll position, we'll need it to zoom in/out around ($x,$y)
        if(x == 0)
            x = canvasViewer->width() / 2;
        if(y == 0)
            y = canvasViewer->height() / 2;

        //update zoom factor and redraw
        double newZoomFactor = zoomFactor * mult;

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
        canvasViewer->setZoomFactor(newZoomFactor);
    }
}

void ModuleInspector::resetOsgView()
{
    osgViewer->applyViewerHints();
}

GraphicsLayer *ModuleInspector::getAnimationLayer()
{
    return canvasViewer->getAnimationLayer();
}

void ModuleInspector::submoduleCreated(cModule *newmodule)
{
    canvasViewer->setNeedsRedraw();
}

void ModuleInspector::submoduleDeleted(cModule *module)
{
    canvasViewer->setNeedsRedraw();
}

void ModuleInspector::connectionCreated(cGate *srcgate)
{
    canvasViewer->setNeedsRedraw();
}

void ModuleInspector::connectionDeleted(cGate *srcgate)
{
    canvasViewer->setNeedsRedraw();
}

void ModuleInspector::displayStringChanged(cModule *)
{
    canvasViewer->setNeedsRedraw();
}

void ModuleInspector::displayStringChanged()
{
    canvasViewer->setNeedsRedraw();  // TODO check, probably only non-background tags have changed...
}

void ModuleInspector::displayStringChanged(cGate *)
{
    canvasViewer->setNeedsRedraw();
}

void ModuleInspector::bubble(cComponent *subcomponent, const char *text)
{
    canvasViewer->bubble(subcomponent, text);
}

QPointF ModuleInspector::getSubmodCoords(cModule *mod)
{
    return canvasViewer->getSubmodCoords(mod);
}

QPointF ModuleInspector::getMessageEndPos(cModule *src, cModule *dest)
{
    return canvasViewer->getMessageEndPos(src, dest);
}


int ModuleInspector::inspectorCommand(int argc, const char **argv)
{
    return Inspector::inspectorCommand(argc, argv);
}

int ModuleInspector::getDefaultLayoutSeed()
{
    const cDisplayString blank;
    cModule *parentModule = static_cast<cModule *>(object);
    const cDisplayString& ds = parentModule && parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;
    long seed = resolveLongDispStrArg(ds.getTagArg("bgl",4), parentModule, 1);
    return seed;
}

int ModuleInspector::getLayoutSeed(int argc, const char **argv)
{
    if (argc != 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    //Tcl_SetObjResult(interp, Tcl_NewIntObj((int)layoutSeed));
    return TCL_OK;
}

int ModuleInspector::setLayoutSeed(int argc, const char **argv)
{
    if (argc != 2) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    canvasViewer->setLayoutSeed(atol(argv[1]));
    return TCL_OK;
}

int ModuleInspector::getSubmoduleCount(int argc, const char **argv)
{
    if (argc != 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    int count = 0;
    for (cModule::SubmoduleIterator it(static_cast<cModule *>(object)); !it.end(); ++it)
        count++;
    Tcl_SetObjResult(interp, Tcl_NewIntObj(count));
    return TCL_OK;
}

int ModuleInspector::getSubmodQ(int argc, const char **argv)
{
    // args: <module ptr> <qname>
    if (argc != 3) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
    const char *qname = argv[2];
    cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));
    char buf[21];
    ptrToStr(q, buf);
    Tcl_SetResult(interp, buf, TCL_VOLATILE);
    return TCL_OK;
}

int ModuleInspector::getSubmodQLen(int argc, const char **argv)
{
    // args: <module ptr> <qname>
    if (argc != 3) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
    const char *qname = argv[2];
    cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));  // FIXME THIS MUST BE REFINED! SEARCHES WAY TOO DEEEEEP!!!!
    if (!q) {
        Tcl_SetResult(interp, TCLCONST(""), TCL_STATIC);
        return TCL_OK;
    }
    Tcl_SetObjResult(interp, Tcl_NewIntObj(q->getLength()));
    return TCL_OK;
}

void ModuleInspector::click(QMouseEvent *event) {
    auto objects = canvasViewer->getObjectsAt(event->pos().x(), event->pos().y());
    if (!objects.empty()) {
        getQtenv()->getMainObjectInspector()->setObject(objects.front());
    }
}

void ModuleInspector::objectsPicked(const std::vector<cObject*>& objects)
{
    getQtenv()->getMainObjectInspector()->setObject(objects.front());
}

void ModuleInspector::doubleClick(QMouseEvent *event)
{
    cObject *object = nullptr;
    QList<cObject *> objects = canvasViewer->getObjectsAt(event->pos().x(), event->pos().y());
    for (auto &i : objects) {
        if (i) {
            object = i;
            break;
        }
    }

    //TODO click to connection
    if(object == nullptr)
    {
        qDebug() << "mouseDoubleClickEvent: object is null";
        return;
    }

    if(supportsObject(object))    //TODO && $config(reuse-inspectors)
        setObject(object);
    else
    {}  //TODO opp_inspect $ptr
}

void ModuleInspector::createContextMenu(QContextMenuEvent *event)
{
    //TODO
    //global inspectordata tmp

    //ModuleInspector:zoomMarqueeCancel $insp ;# just in case

    QList<cObject*> objects = canvasViewer->getObjectsAt(event->x(), event->y());

   if(objects.size())
   {
        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects.toVector(), this);

        //TODO
        //set tmp($c:showlabels) $inspectordata($c:showlabels)
        //set tmp($c:showarrowheads) $inspectordata($c:showarrowheads)

        menu->addSeparator();
        menu->addAction("Show/Hide Canvas Layers...", this, SLOT(layers()));

        menu->addSeparator();
        menu->addAction("Show Module Names", this, SLOT(toggleLabels()), QKeySequence(Qt::CTRL + Qt::Key_L));
        menu->addAction("Show Arrowheads", this, SLOT(toggleArrowheads()), QKeySequence(Qt::CTRL + Qt::Key_A));

        menu->addSeparator();
        addAction(menu->addAction("Increase Icon Size", this, SLOT(increaseIconSize()), QKeySequence(Qt::CTRL + Qt::Key_I)));
        addAction(menu->addAction("Decrease Icon Size", this, SLOT(decreaseIconSize()), QKeySequence(Qt::CTRL + Qt::Key_O)));

        menu->addSeparator();
        menu->addAction("Zoom In", this, SLOT(zoomIn()), QKeySequence(Qt::CTRL + Qt::Key_M));
        menu->addAction("Zoom Out", this, SLOT(zoomOut()), QKeySequence(Qt::CTRL + Qt::Key_N));
        menu->addAction("Re-Layout", canvasViewer, SLOT(relayoutAndRedrawAll()), QKeySequence(Qt::CTRL + Qt::Key_R));

        menu->addSeparator();
        //TODO Create a preferences dialog in Qt designer and here set its exec() slot
//      $popup add command -label "Layouting Settings..." -command "preferencesDialog $insp l"
        menu->addAction("Layouting Settings...");
//      $popup add command -label "Animation Settings..." -command "preferencesDialog $insp a"
        menu->addAction("Animation Settings...");
//      $popup add command -label "Animation Filter..." -command "preferencesDialog $insp t"
        menu->addAction("Animation Filter...");

        menu->exec(event->globalPos());
        delete menu;
    }
}

void ModuleInspector::layers()
{
    CanvasRenderer *canvasRenderer = canvasViewer->getCanvasRenderer();
    if(!canvasRenderer->hasCanvas())
    {
        QMessageBox::warning(this, tr("Warning"), tr("No default canvas has been created for this module yet."),
                QMessageBox::Ok);
        return;
    }

    const char *allTags = canvasRenderer->getAllTags().c_str();
    const char *enabledTags = canvasRenderer->getEnabledTags().c_str();
    const char *exceptTags = canvasRenderer->getExceptTags().c_str();

    //TODO create dialog
//    set result [CanvasInspectors:layersDialog $allTags $enabledTags $exceptTags]

//    if {$result != {}} {
//        set enabledTags [lindex $result 0]
//        set exceptTags [lindex $result 1]
//        opp_inspectorcommand $insp setenabledtags $enabledTags
//        opp_inspectorcommand $insp setexcepttags $exceptTags
//        opp_inspectorcommand $insp redraw
//    }
//}
}

void ModuleInspector::toggleLabels()
{
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":showlabels";
    QVariant variant = getQtenv()->getPref(prefName);
    bool showLabels = variant.isValid() ? variant.value<bool>() : false;

    getQtenv()->setPref(prefName, !showLabels);
    redraw();
}

void ModuleInspector::toggleArrowheads()
{
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":showarrowheads";
    QVariant variant = getQtenv()->getPref(prefName);
    bool showArrowheads = variant.isValid() ? variant.value<bool>() : false;

    getQtenv()->setPref(prefName, !showArrowheads);
    redraw();
}

void ModuleInspector::zoomIconsBy(double mult) {
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":imagesizefactor";
    QVariant variant = getQtenv()->getPref(prefName);
    double imageSizeFactor = variant.isValid() ? variant.value<double>() : 1;
    if((mult < 1 && imageSizeFactor>0.1) || (mult > 1 && imageSizeFactor < 5))
    {
        double newImageSizeFactor = imageSizeFactor * mult;
        getQtenv()->setPref(prefName, newImageSizeFactor);
        canvasViewer->setImageSizeFactor(newImageSizeFactor);
    }
}

void ModuleInspector::switchToOsgView()
{
    stackedLayout->setCurrentWidget(osgViewer);

    switchToCanvasViewAction->setChecked(false);
    switchToOsgViewAction->setChecked(true);

    // show/hide view-specific actions
    canvasRelayoutAction->setVisible(false);
    canvasZoomInAction->setVisible(false);
    canvasZoomOutAction->setVisible(false);
    resetOsgViewAction->setVisible(true);
}

void ModuleInspector::switchToCanvasView()
{
    stackedLayout->setCurrentWidget(canvasViewer);

    switchToCanvasViewAction->setChecked(true);
    switchToOsgViewAction->setChecked(false);

    // show/hide view-specific actions
    canvasRelayoutAction->setVisible(true);
    canvasZoomInAction->setVisible(true);
    canvasZoomOutAction->setVisible(true);
    resetOsgViewAction->setVisible(false);
}

} // namespace qtenv
} // namespace omnetpp
