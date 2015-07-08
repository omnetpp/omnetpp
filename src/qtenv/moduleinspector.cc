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
#include <QGridLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>

#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cgate.h"
#include "moduleinspector.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "arrow.h"
#include "canvasrenderer.h"
#include "mainwindow.h"
#include "modulegraphicsview.h"
#include "inspectorutil.h"

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

#define UNKNOWNICON_WIDTH     32
#define UNKNOWNICON_HEIGHT    32

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
    canvasRenderer = new CanvasRenderer();
    createView(this);
    canvasRenderer->setQtCanvas(view->scene(), getCanvas());
}

ModuleInspector::~ModuleInspector()
{
    delete canvasRenderer;
}

void ModuleInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    view->setObject(static_cast<cModule*>(obj));
    Inspector::doSetObject(obj);

    canvasRenderer->setCanvas(getCanvas());

    view->clear();

    if (object) {
        view->setLayoutSeed(1);  // we'll read the "bgl" display string tag from Tcl
        // TODO
        //    ModuleInspector:recallPreferences $insp

        try {
            view->relayoutAndRedrawAll();
        }
        catch (std::exception& e) {
            QMessageBox::warning(this, QString("Error"), QString("Error displaying network graphics: ") + e.what());
        }
        //    ModuleInspector:updateZoomLabel $insp
        //    ModuleInspector:adjustWindowSizeAndZoom $insp
        // }
    }
}

bool ModuleInspector::needsRedraw()
{
    return view->getNeedsRedraw();
}

void ModuleInspector::createView(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    parent->setLayout(layout);
    view = new ModuleGraphicsView(canvasRenderer);

    connect(view, SIGNAL(doubleClick(QMouseEvent*)), this, SLOT(doubleClick(QMouseEvent*)));
    connect(view, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this, SLOT(createContextMenu(QContextMenuEvent*)));

    layout->addWidget(view);
    layout->setMargin(0);
    view->setRenderHints(QPainter::Antialiasing);
    view->setScene(new QGraphicsScene());

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->insertStretch(0);
    view->setLayout(horizontalLayout);

    addToolBar(verticalLayout);
}

void ModuleInspector::addToolBar(QBoxLayout *layout)
{
    QToolBar *toolBar = new QToolBar();
    layout->addWidget(toolBar);
    layout->insertStretch(1);

    toolBar->addAction(QIcon(":/tools/icons/tools/back.png"), "Back",
                       this, SLOT(goBack()));
    toolBar->addAction(QIcon(":/tools/icons/tools/forward.png"), "Forward",
                       this, SLOT(goForward()));
    toolBar->addAction(QIcon(":/tools/icons/tools/parent.png"), "Go to parent module",
                       this, SLOT(inspectParent()));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/tools/icons/tools/mrun.png"), "Run until next event in this module",
                       this, SLOT(runUntil()));
    QAction *action = toolBar->addAction(QIcon(":/tools/icons/tools/mfast.png"), "Fast run until next event in this module (Ctrl + F4)",
                       this, SLOT(fastRunUntil()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));

    action = toolBar->addAction(QIcon(":/tools/icons/tools/stop.png"), "Stop the simulation (F8)",
                       this, SLOT(stopSimulation()));
    action->setShortcut(QKeySequence(Qt::Key_F8));

    toolBar->addSeparator();
    action = toolBar->addAction(QIcon(":/tools/icons/tools/redraw.png"), "Re-layout (Ctrl + R)",
                                this, SLOT(relayout()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

    action = toolBar->addAction(QIcon(":/tools/icons/tools/zoomin.png"), "Zoom in (Ctrl + M)",
                                this, SLOT(zoomIn()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));

    action = toolBar->addAction(QIcon(":/tools/icons/tools/zoomout.png"), "Zoom out (Ctrl + N)",
                                this, SLOT(zoomOut()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

    toolBar->setAutoFillBackground(true);
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
    view->incLayoutSeed();

    view->relayoutAndRedrawAll();

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
            x = view->width() / 2;
        if(y == 0)
            y = view->height() / 2;

        //update zoom factor and redraw
        double newZoomFactor = zoomFactor * mult;

        //snap to true (note: this is not desirable when zoom is set programmatically to fit network into window)
        if(snaptoone)
        {
            double m = mult < 1 ? 1.0/mult : mult;
            double a = 1 - 0.9*(1 - 1.0/m);
            double b = 1 + 0.9*(m - 1);
            if(zoomFactor > a && zoomFactor < b)
                newZoomFactor = 1;
        }

        getQtenv()->setPref(prefName, newZoomFactor);
        redraw();
        //TODO
        //ModuleInspector:updateZoomLabel $insp
    }
}

cCanvas *ModuleInspector::getCanvas()
{
    cModule *mod = static_cast<cModule *>(object);
    cCanvas *canvas = mod ? mod->getCanvasIfExists() : nullptr;
    return canvas;
}

void ModuleInspector::refresh()
{
    Inspector::refresh();

    view->refresh();
}

void ModuleInspector::redraw()
{
    view->redraw();
}

void ModuleInspector::clearObjectChangeFlags()
{
    cCanvas *canvas = getCanvas();
    if (canvas)
        canvas->getRootFigure()->clearChangeFlags();
}

void ModuleInspector::submoduleCreated(cModule *newmodule)
{
    view->setNeedsRedraw();
}

void ModuleInspector::submoduleDeleted(cModule *module)
{
    view->setNeedsRedraw();
}

void ModuleInspector::connectionCreated(cGate *srcgate)
{
    view->setNeedsRedraw();
}

void ModuleInspector::connectionDeleted(cGate *srcgate)
{
    view->setNeedsRedraw();
}

void ModuleInspector::displayStringChanged(cModule *)
{
    view->setNeedsRedraw();
}

void ModuleInspector::displayStringChanged()
{
    view->setNeedsRedraw();  // TODO check, probably only non-background tags have changed...
}

void ModuleInspector::displayStringChanged(cGate *)
{
    view->setNeedsRedraw();
}

void ModuleInspector::bubble(cComponent *subcomponent, const char *text)
{
    view->bubble(subcomponent, text);
}

const char *ModuleInspector::animModeToStr(SendAnimMode mode)
{
    return mode == ANIM_BEGIN ? "beg" : mode == ANIM_THROUGH ? "thru" : mode == ANIM_END ? "end" : "???";
}

void ModuleInspector::animateMethodcallAscent(cModule *srcSubmod, const char *methodText)
{
    char parentPtr[30], modPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(srcSubmod, modPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallAscent ", getWindowName(), " ", parentPtr, " ", modPtr, " ", TclQuotedString(methodText).get(), TCL_NULL));
}

void ModuleInspector::animateMethodcallDescent(cModule *destSubmod, const char *methodText)
{
    char parentPtr[30], modPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(destSubmod, modPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallDescent ", getWindowName(), " ", parentPtr, " ", modPtr, " ", TclQuotedString(methodText).get(), TCL_NULL));
}

void ModuleInspector::animateMethodcallHoriz(cModule *srcSubmod, cModule *destSubmod, const char *methodText)
{
    char srcPtr[30], destPtr[30];
    ptrToStr(srcSubmod, srcPtr);
    ptrToStr(destSubmod, destPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallHoriz ", getWindowName(), " ", srcPtr, " ", destPtr, " ", TclQuotedString(methodText).get(), TCL_NULL));
}

void ModuleInspector::animateMethodcallDelay(Tcl_Interp *interp)
{
    CHK(Tcl_Eval(interp, "ModuleInspector:animateMethodcallWait"));
}

void ModuleInspector::animateMethodcallCleanup()
{
    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallCleanup ", getWindowName(), TCL_NULL));
}

void ModuleInspector::animateSendOnConn(cGate *srcGate, cMessage *msg, SendAnimMode mode)
{
    char gatePtr[30], msgPtr[30];
    ptrToStr(srcGate, gatePtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateOnConn ", getWindowName(), " ", msgPtr, " ", gatePtr, " ", modeStr, TCL_NULL));
}

void ModuleInspector::animateSenddirectAscent(cModule *srcSubmod, cMessage *msg)
{
    char parentPtr[30], modPtr[30], msgPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(srcSubmod, modPtr);
    ptrToStr(msg, msgPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectAscent ", getWindowName(), " ", msgPtr, " ", parentPtr, " ", modPtr, " ", "thru", TCL_NULL));
}

void ModuleInspector::animateSenddirectDescent(cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    char parentPtr[30], modPtr[30], msgPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(destSubmod, modPtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectDescent ", getWindowName(), " ", msgPtr, " ", parentPtr, " ", modPtr, " ", modeStr, TCL_NULL));
}

void ModuleInspector::animateSenddirectHoriz(cModule *srcSubmod, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    char srcPtr[30], destPtr[30], msgPtr[30];
    ptrToStr(srcSubmod, srcPtr);
    ptrToStr(destSubmod, destPtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectHoriz ", getWindowName(), " ", msgPtr, " ", srcPtr, " ", destPtr, " ", modeStr, TCL_NULL));
}

void ModuleInspector::animateSenddirectCleanup()
{
    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectCleanup ", getWindowName(), TCL_NULL));
}

void ModuleInspector::animateSenddirectDelivery(cModule *destSubmod, cMessage *msg)
{
    char destPtr[30], msgPtr[30];
    ptrToStr(destSubmod, destPtr);
    ptrToStr(msg, msgPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectDelivery ", getWindowName(), " ", msgPtr, " ", destPtr, TCL_NULL));
}

void ModuleInspector::performAnimations(Tcl_Interp *interp)
{
    CHK(Tcl_VarEval(interp, "performAnimations", TCL_NULL));
}

int ModuleInspector::inspectorCommand(int argc, const char **argv)
{
    if (argc < 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    E_TRY
    if (strcmp(argv[0], "arrowcoords") == 0) {
        return arrowcoords(interp, argc, argv);
    }
    else if (strcmp(argv[0], "relayout") == 0) {
        //TRY(relayoutAndRedrawAll());
        return TCL_OK;
    }
    else if (strcmp(argv[0], "redraw") == 0) {
        TRY(redraw());
        return TCL_OK;
    }
//   else if (strcmp(argv[0], "getdefaultlayoutseed") == 0) {
//       return getDefaultLayoutSeed(argc, argv);
//   }
    else if (strcmp(argv[0], "getlayoutseed") == 0) {
        return getLayoutSeed(argc, argv);
    }
    else if (strcmp(argv[0], "setlayoutseed") == 0) {
        return setLayoutSeed(argc, argv);
    }
    else if (strcmp(argv[0], "submodulecount") == 0) {
        return getSubmoduleCount(argc, argv);
    }
    else if (strcmp(argv[0], "getsubmodq") == 0) {
        return getSubmodQ(argc, argv);
    }
    else if (strcmp(argv[0], "getsubmodqlen") == 0) {
        return getSubmodQLen(argc, argv);
    }
    else if (strcmp(argv[0], "hascanvas") == 0) {  // needed because getalltags/getenabledtags/etc throw error if there's no canvas
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->hasCanvas() ? "1" : "0"), TCL_STATIC);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getalltags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getAllTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getenabledtags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getEnabledTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getexcepttags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getExceptTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "setenabledtags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setEnabledTags(argv[1]);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "setexcepttags") == 0) {
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
    view->setLayoutSeed(atol(argv[1]));
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

void ModuleInspector::doubleClick(QMouseEvent *event)
{
    cObject *object = view->getObjectAt(event->pos().x(), event->pos().y());

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

    QList<cObject*> objects = view->getObjectsAt(event->x(), event->y());

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
        QAction *action = menu->addAction("Increase Icon Size", this, SLOT(zoomIconsBy()), QKeySequence(Qt::CTRL + Qt::Key_I));
        action->setData(QVariant::fromValue(1.25));
        action = menu->addAction("Decrease Icon Size", this, SLOT(zoomIconsBy()), QKeySequence(Qt::CTRL + Qt::Key_O));
        action->setData(QVariant::fromValue(0.8));

        menu->addSeparator();
        menu->addAction("Zoom In", this, SLOT(zoomIn()), QKeySequence(Qt::CTRL + Qt::Key_M));
        menu->addAction("Zoom Out", this, SLOT(zoomOut()), QKeySequence(Qt::CTRL + Qt::Key_N));
        menu->addAction("Re-Layout", view, SLOT(relayoutAndRedrawAll()), QKeySequence(Qt::CTRL + Qt::Key_R));

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

void ModuleInspector::zoomIconsBy()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid())
    {
        double mult = variant.value<double>();
        QString objName = object->getFullName();
        QString prefName = objName + ":" + INSP_DEFAULT + ":imagesizefactor";
        variant = getQtenv()->getPref(prefName);
        double imageSizeFactor = variant.isValid() ? variant.value<double>() : 1;
        if((mult < 1 && imageSizeFactor>0.1) || (mult > 1 && imageSizeFactor < 5))
        {
            double newImageSizeFactor = imageSizeFactor * mult;
            if(imageSizeFactor - 1 < 0.1)
                newImageSizeFactor = 1;
            getQtenv()->setPref(prefName, newImageSizeFactor);
            redraw();
        }
    }
}

} // namespace qtenv
} // namespace omnetpp
