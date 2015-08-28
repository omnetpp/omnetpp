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
#include "qtenv.h"
#include "inspectorfactory.h"
#include "arrow.h"
#include "canvasrenderer.h"
#include "osgviewer.h"
#include "mainwindow.h"
#include "modulecanvasviewer.h"
#include "inspectorutil.h"

using namespace OPP::common;

#define emit

namespace omnetpp {
namespace qtenv {

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

    createViews(this, isTopLevel);
    parent->setMinimumSize(20, 20);
}

ModuleInspector::~ModuleInspector()
{
    // so the window can be closed safely, without
    // double deleting the message items (by the
    // animations and the scene itself)
    getQtenv()->getAnimator()->clearInspector(this);
}

void ModuleInspector::createViews(QWidget *parent, bool isTopLevel)
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

    QToolBar *toolbar = createToolbar(isTopLevel);
    if(isTopLevel)
    {
        auto layout = new QVBoxLayout(parent);
        layout->setMargin(1);
        stackedLayout = new QStackedLayout();
        layout->addWidget(toolbar);
        layout->addLayout(stackedLayout);
    }
    else
    {
        QWidget *contentArea = new QWidget();
        auto layout = new FloatingToolbarLayout(parent);
        layout->addWidget(contentArea);
        layout->addWidget(toolbar);
        stackedLayout = new QStackedLayout(contentArea);
    }

    stackedLayout->addWidget(osgViewer);
    stackedLayout->addWidget(canvasViewer);
}

QToolBar *ModuleInspector::createToolbar(bool isTopLevel)
{
    QToolBar *toolbar;

    if(isTopLevel)
        toolbar = createToolBarToplevel();
    else
    {
        toolbar = new QToolBar();
        // general
        goBackAction = toolbar->addAction(QIcon(":/tools/icons/tools/back.png"), "Back", this, SLOT(goBack()));
        goForwardAction = toolbar->addAction(QIcon(":/tools/icons/tools/forward.png"), "Forward", this, SLOT(goForward()));
        goUpAction = toolbar->addAction(QIcon(":/tools/icons/tools/parent.png"), "Go to parent module", this, SLOT(inspectParent()));
    }
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
    getQtenv()->runSimulationLocal(qtenv::Qtenv::eRunMode::RUNMODE_NORMAL);
}

void ModuleInspector::fastRunUntil()
{
    getQtenv()->runSimulationLocal(qtenv::Qtenv::eRunMode::RUNMODE_FAST);
}

void ModuleInspector::stopSimulation()
{
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    mainWindow->on_actionStop_triggered();
}

//Relayout the compound module, and resize the window accordingly.

void ModuleInspector::relayout()
{
    canvasViewer->incLayoutSeed();
    canvasViewer->relayoutAndRedrawAll();
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
        // so animations will not wander around at the old module positions
        getQtenv()->getAnimator()->clearInspector(this);
        canvasViewer->setZoomFactor(newZoomFactor);
        getQtenv()->getAnimator()->redrawMessages();
    }
}

void ModuleInspector::resetOsgView()
{
    osgViewer->applyViewerHints();
}

double ModuleInspector::getZoomFactor() {
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    return variant.isValid() ? variant.value<double>() : 1;
}

double ModuleInspector::getImageSizeFactor() {
    QString objName = object->getFullName();
    QString prefName = objName + ":" + INSP_DEFAULT + ":imagesizefactor";
    QVariant variant = getQtenv()->getPref(prefName);
    return variant.isValid() ? variant.value<double>() : 1;
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

QLineF ModuleInspector::getConnectionLine(cGate *gate)
{
    return canvasViewer->getConnectionLine(gate);
}

/*TCLKILL
int ModuleInspector::inspectorCommand(int argc, const char **argv)
{
    return Inspector::inspectorCommand(argc, argv);
}
*/
int ModuleInspector::getDefaultLayoutSeed()
{
    const cDisplayString blank;
    cModule *parentModule = static_cast<cModule *>(object);
    const cDisplayString& ds = parentModule && parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;
    long seed = resolveLongDispStrArg(ds.getTagArg("bgl",4), parentModule, 1);
    return seed;
}

int ModuleInspector::getLayoutSeed(int argc, const char **argv)
{/*
    if (argc != 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    //Tcl_SetObjResult(interp, Tcl_NewIntObj((int)layoutSeed));
    return TCL_OK;*/
    return 0;
}

int ModuleInspector::setLayoutSeed(int argc, const char **argv)
{/*
    if (argc != 2) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    canvasViewer->setLayoutSeed(atol(argv[1]));
    return TCL_OK;*/
    return 0;
}

int ModuleInspector::getSubmoduleCount(int argc, const char **argv)
{
    if (argc != 1) {
        //TCLKILL Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        //TCLKILL return TCL_ERROR;
    }
    int count = 0;
    for (cModule::SubmoduleIterator it(static_cast<cModule *>(object)); !it.end(); ++it)
        count++;
    //TCLKILL Tcl_SetObjResult(interp, Tcl_NewIntObj(count));
    //TCLKILL return TCL_OK;
    return 0;
}

int ModuleInspector::getSubmodQ(int argc, const char **argv)
{
    // args: <module ptr> <qname>
    if (argc != 3) {
        //TCLKILL Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        //TCLKILL return TCL_ERROR;
    }

    cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
    const char *qname = argv[2];
    cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));
    char buf[21];
    ptrToStr(q, buf);
    //TCLKILL Tcl_SetResult(interp, buf, TCL_VOLATILE);
    //TCLKILL return TCL_OK;
    return 0;
}

int ModuleInspector::getSubmodQLen(int argc, const char **argv)
{
    // args: <module ptr> <qname>
    if (argc != 3) {
        //TCLKILL Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        //TCLKILL return TCL_ERROR;
    }

    cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
    const char *qname = argv[2];
    cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));  // FIXME THIS MUST BE REFINED! SEARCHES WAY TOO DEEEEEP!!!!
    if (!q) {
        //TCLKILL Tcl_SetResult(interp, TCLCONST(""), TCL_STATIC);
        //TCLKILL return TCL_OK;
    }
    //TCLKILL Tcl_SetObjResult(interp, Tcl_NewIntObj(q->getLength()));
    //TCLKILL return TCL_OK;
    return 0;
}

void ModuleInspector::click(QMouseEvent *event) {
    auto objects = canvasViewer->getObjectsAt(event->pos());
    if (!objects.empty())
        emit selectionChanged(objects.front());
}

void ModuleInspector::doubleClick(QMouseEvent *event) {
    objectsPicked(canvasViewer->getObjectsAt(event->pos()));
}

void ModuleInspector::objectsPicked(const std::vector<cObject*>& objects)
{
    cObject *object = nullptr;
    for (auto &o : objects) {
        if (o) {
            object = o;
            break;
        }
    }

    if (object)
        emit selectionChanged(object);
}

void ModuleInspector::createContextMenu(QContextMenuEvent *event)
{
    std::vector<cObject*> objects = canvasViewer->getObjectsAt(event->pos());

    if(objects.size())
    {
        QMenu *menu = InspectorUtil::createInspectorContextMenu(QVector<cObject*>::fromStdVector(objects), this);

        QString objName = object->getFullName();
        QString prefName = objName + ":" + INSP_DEFAULT + ":showlabels";
        bool showLabels = getQtenv()->getPref(prefName, QVariant::fromValue(true)).value<bool>();
        prefName = objName + ":" + INSP_DEFAULT + ":showarrowheads";
        bool showArrowHeads = getQtenv()->getPref(prefName, QVariant::fromValue(true)).value<bool>();

        menu->addSeparator();
        menu->addAction("Show/Hide Canvas Layers...", this, SLOT(layers()));

        menu->addSeparator();
        //TODO not working these "Show" menu points
        QAction *action = menu->addAction("Show Module Names", this, SLOT(toggleLabels()), QKeySequence(Qt::CTRL + Qt::Key_L));
        action->setCheckable(true);
        action->setChecked(showLabels);
        action = menu->addAction("Show Arrowheads", this, SLOT(toggleArrowheads()), QKeySequence(Qt::CTRL + Qt::Key_A));
        action->setCheckable(true);
        action->setChecked(showArrowHeads);

        menu->addSeparator();
        addAction(menu->addAction("Increase Icon Size", this, SLOT(increaseIconSize()), QKeySequence(Qt::CTRL + Qt::Key_I)));
        addAction(menu->addAction("Decrease Icon Size", this, SLOT(decreaseIconSize()), QKeySequence(Qt::CTRL + Qt::Key_O)));

        menu->addSeparator();
        menu->addAction("Zoom In", this, SLOT(zoomIn()), QKeySequence(Qt::CTRL + Qt::Key_M));
        menu->addAction("Zoom Out", this, SLOT(zoomOut()), QKeySequence(Qt::CTRL + Qt::Key_N));
        menu->addAction("Re-Layout", canvasViewer, SLOT(relayoutAndRedrawAll()), QKeySequence(Qt::CTRL + Qt::Key_R));

        menu->addSeparator();
        action = menu->addAction("Layouting Settings...", this, SLOT(runPreferencesDialog()));
        action->setData(InspectorUtil::TAB_LAYOUTING);
        action = menu->addAction("Animation Settings...", this, SLOT(runPreferencesDialog()));
        action->setData(InspectorUtil::TAB_ANIMATION);
        action = menu->addAction("Animation Filter...", this, SLOT(runPreferencesDialog()));
        action->setData(InspectorUtil::TAB_FILTERING);

        menu->exec(event->globalPos());
        delete menu;
    }
}

void ModuleInspector::runPreferencesDialog()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid())
        InspectorUtil::preferencesDialog((InspectorUtil::eTab)variant.value<int>());
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

    //TCLKILL const char *allTags = canvasRenderer->getAllTags().c_str();
    //TCLKILL const char *enabledTags = canvasRenderer->getEnabledTags().c_str();
    //TCLKILL const char *exceptTags = canvasRenderer->getExceptTags().c_str();

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
        getQtenv()->getAnimator()->redrawMessages();
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
