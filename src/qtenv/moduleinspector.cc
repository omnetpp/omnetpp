//==========================================================================
//  MODULEINSPECTOR.CC - part of
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
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStackedLayout>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMenu>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsItem>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtWidgets/QFileDialog>
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
#include "iosgviewer.h"
#include "mainwindow.h"
#include "modulecanvasviewer.h"
#include "inspectorutil.h"
#include "layersdialog.h"
#include "messageanimator.h"

using namespace omnetpp::common;

#define emit

namespace omnetpp {
namespace qtenv {

void _dummy_for_moduleinspector() {}

class ModuleInspectorFactory : public InspectorFactory
{
  public:
    ModuleInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cModule *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override {
        cModule *mod = dynamic_cast<cModule *>(object);
        return mod && mod->hasSubmodules() ? 3.0 : 0.9;
    }

    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new ModuleInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(ModuleInspectorFactory);

const QString ModuleInspector::PREF_MODE = "mode";
const QString ModuleInspector::PREF_CENTER = "center";
const QString ModuleInspector::PREF_ZOOMFACTOR = "zoomfactor";
const QString ModuleInspector::PREF_ZOOMBYFACTOR = "zoombyfactor";
const QString ModuleInspector::PREF_ICONSCALE = "iconscale";
const QString ModuleInspector::PREF_SHOWMETHODCALLS = "showmethodcalls";
const QString ModuleInspector::PREF_SHOWLABELS = "showlabels";
const QString ModuleInspector::PREF_SHOWARROWHEADS = "showarrowheads";
const QString ModuleInspector::PREF_SUBMODULENAMEFORMAT = "submodulenameformat";

ModuleInspector::ModuleInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    switchToOsgViewAction = nullptr;
    switchToCanvasViewAction = nullptr;

    createViews(isTopLevel);
    parent->setMinimumSize(20, 20);
    switchToCanvasView();

    showMethodCallsAction = new QAction("Show Method Calls", this);
    connect(showMethodCallsAction, SIGNAL(triggered(bool)), this, SLOT(showMethodCalls(bool)));
    showMethodCallsAction->setShortcut(Qt::CTRL | Qt::Key_E);
    showMethodCallsAction->setCheckable(true);
    addAction(showMethodCallsAction);

    showModuleNamesAction = new QAction("Show Module Names", this);
    connect(showModuleNamesAction, SIGNAL(triggered(bool)), this, SLOT(showLabels(bool)));
    showModuleNamesAction->setShortcut(Qt::CTRL | Qt::Key_L);
    showModuleNamesAction->setCheckable(true);
    addAction(showModuleNamesAction);

    showArrowheadsAction = new QAction("Show Arrowheads", this);
    connect(showArrowheadsAction, SIGNAL(triggered(bool)), this, SLOT(showArrowheads(bool)));
    showArrowheadsAction->setShortcut(Qt::CTRL | Qt::Key_A);
    showArrowheadsAction->setCheckable(true);
    addAction(showArrowheadsAction);

    increaseIconSizeAction = new QAction("Increase Icon Size", this);
    increaseIconSizeAction->setShortcut(Qt::CTRL | Qt::Key_I);
    connect(increaseIconSizeAction, SIGNAL(triggered(bool)), this, SLOT(increaseIconSize()));
    addAction(increaseIconSizeAction);

    decreaseIconSizeAction = new QAction("Decrease Icon Size", this);
    decreaseIconSizeAction->setShortcut(Qt::CTRL | Qt::Key_O);
    connect(decreaseIconSizeAction, SIGNAL(triggered(bool)), this, SLOT(decreaseIconSize()));
    addAction(decreaseIconSizeAction);

    auto *layouter = getQtenv()->getModuleLayouter();
    connect(layouter, &ModuleLayouter::layoutVisualisationStarts, this, &ModuleInspector::onLayoutVisualizationStarts);
    connect(layouter, &ModuleLayouter::layoutVisualisationEnds, this, &ModuleInspector::onLayoutVisualizationEnds);
    connect(layouter, &ModuleLayouter::moduleLayoutChanged, this, &ModuleInspector::onModuleLayoutChanged);
}

ModuleInspector::~ModuleInspector()
{
    // so the window can be closed safely, without
    // double deleting the message items (by the
    // animations and the scene itself)
    getQtenv()->getMessageAnimator()->clearInspector(this);
}

void ModuleInspector::createViews(bool isTopLevel)
{
    canvasViewer = new ModuleCanvasViewer();
    canvasViewer->setRenderHints(QPainter::Antialiasing);

    // otherwise if the content fits exactly, these will disappear only for a moment during resizing
    canvasViewer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    canvasViewer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(canvasViewer, SIGNAL(back()), this, SLOT(goBack()));
    connect(canvasViewer, SIGNAL(forward()), this, SLOT(goForward()));
    connect(canvasViewer, SIGNAL(click(QMouseEvent *)), this, SLOT(click(QMouseEvent *)));
    connect(canvasViewer, SIGNAL(doubleClick(QMouseEvent *)), this, SLOT(doubleClick(QMouseEvent *)));
    connect(canvasViewer, SIGNAL(dragged(QPointF)), this, SLOT(onViewerDragged(QPointF)));
    connect(canvasViewer, SIGNAL(contextMenuRequested(std::vector<cObject *>, QPoint)), this, SLOT(createContextMenu(std::vector<cObject *>, QPoint)));
    connect(canvasViewer, SIGNAL(marqueeZoom(QRectF)), this, SLOT(onMarqueeZoom(QRectF)));
    connect(getQtenv(), SIGNAL(fontChanged()), this, SLOT(onFontChanged()));

    stackedLayout = new QStackedLayout();
    stackedLayout->addWidget(canvasViewer);

    // the osgViewer will be created and added the first time the user switches to it (if ever)

    toolbar = createToolbar(isTopLevel);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    if (isTopLevel) {
        layout->addWidget(toolbar, 0, Qt::AlignLeft);
        layout->addLayout(stackedLayout);
    }
    else {
        toolbarLayout = new QGridLayout(canvasViewer);
        canvasViewer->setLayout(toolbarLayout);
        toolbarLayout->addWidget(toolbar, 0, 0, Qt::AlignRight | Qt::AlignTop);

        layout->addLayout(stackedLayout);
    }
}

QToolBar *ModuleInspector::createToolbar(bool isTopLevel)
{
    QToolBar *toolbar = new QToolBar();

    if (isTopLevel)
        addTopLevelToolBarActions(toolbar);
    else {
        // general
        goBackAction = toolbar->addAction(QIcon(":/tools/back"), "Back", this, SLOT(goBack()));
        goForwardAction = toolbar->addAction(QIcon(":/tools/forward"), "Forward", this, SLOT(goForward()));
        goUpAction = toolbar->addAction(QIcon(":/tools/parent"), "Go to parent module", this, SLOT(inspectParent()));
    }
    toolbar->addSeparator();

    toolbar->addAction(QIcon(":/tools/mrun"), "Run until next event in this module", this, SLOT(runUntil()));
    QAction *action = toolbar->addAction(QIcon(":/tools/mfast"), "Fast run until next event in this module (Ctrl+F4)", this, SLOT(fastRunUntil()));
    action->setShortcut(Qt::CTRL | Qt::Key_F4);
    toolbar->addAction(getQtenv()->getMainWindow()->getStopAction());
    toolbar->addSeparator();

    // canvas-specfic
    action = toolbar->addAction(QIcon(":/tools/redraw"), "Re-layout", this, SLOT(relayout()));
    action->setShortcut(Qt::CTRL | Qt::Key_R);
    canvasRelayoutAction = action;
    action = toolbar->addAction(QIcon(":/tools/zoomin"), "Zoom in", this, SLOT(zoomIn()));
    action->setShortcut(Qt::CTRL | Qt::Key_Plus);
    canvasZoomInAction = action;
    action = toolbar->addAction(QIcon(":/tools/zoomout"), "Zoom out", this, SLOT(zoomOut()));
    action->setShortcut(Qt::CTRL + Qt::Key_Minus);
    canvasZoomOutAction = action;
    action = toolbar->addAction(QIcon(":/tools/layers"), "Show/Hide Canvas Layers...", this, SLOT(showCanvasLayersDialog()));
    canvasLayersAction = action;

    // osg-specific
    action = toolbar->addAction(QIcon(":/tools/reset"), "Reset view");
    resetOsgViewAction = action;
    toolbar->addSeparator();

    // mode switching
    action = toolbar->addAction(QIcon(":/tools/3dscene"), "3D scene", this, SLOT(switchToOsgView()));
    action->setCheckable(true);
    switchToOsgViewAction = action;

    action = toolbar->addAction(QIcon(":/tools/modulegraphics"), "Module", this, SLOT(switchToCanvasView()));
    action->setCheckable(true);
    switchToCanvasViewAction = action;

    // this is to fill the remaining space on the toolbar, replacing the ugly default gradient on Mac
    toolbar->setAutoFillBackground(true);
    QWidget *stretch = new QWidget(toolbar);
    stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    stretch->setAutoFillBackground(true);
    toolbar->addWidget(stretch);

    return toolbar;
}

void ModuleInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    cModule *module = dynamic_cast<cModule *>(obj);

    // TODO: there are too many redraws (also multiple dialogs on errors), see notes below.

    canvasViewer->setObject(module); // this already calls redraw()
    canvasViewer->clear(); // then we clear it
    bubblesToShow.clear();

    setOsgCanvas(nullptr);

    getQtenv()->getMessageAnimator()->clearInspector(this);

    if (object) {
        canvasViewer->refreshLayout();
        try {
            canvasViewer->redraw(); // and then redraw again
            canvasViewer->setZoomFactor(getPref(PREF_ZOOMFACTOR, 1).toDouble()); // this also redraws a couple things
            canvasViewer->setImageSizeFactor(getPref(PREF_ICONSCALE, 1).toDouble());
            showMethodCalls(getPref(PREF_SHOWMETHODCALLS, true).toBool());
            showLabels(getPref(PREF_SHOWLABELS, true).toBool());
            showArrowheads(getPref(PREF_SHOWARROWHEADS, true).toBool());
            setSubmoduleNameFormat((SubmoduleNameFormat)getPref(PREF_SUBMODULENAMEFORMAT, (int)SubmoduleNameFormat::FMT_FULLNAME_AND_QDISPLAYNAME).toInt());
        }
        catch (std::exception& e) {
            QMessageBox::warning(this, QString("Error"), QString("Error displaying network graphics: ") + e.what());
        }
        getQtenv()->getMessageAnimator()->addInspector(this);

        // both are catching exceptions
        getQtenv()->callRefreshDisplaySafe();
        getQtenv()->callRefreshInspectors();
    } else {
        canvasViewer->setZoomFactor(1);
        if (isToplevelInspector())
            setEnabled(false);
    }

    cOsgCanvas *osgCanvas = getOsgCanvas();
    setOsgCanvas(osgCanvas);

    bool enabled = ((getPref(PREF_MODE, IOsgViewer::isOsgPreferred() ? 1 : 0).toInt() == 1));
    if (osgCanvas != nullptr && enabled)
        switchToOsgView();
    else
        switchToCanvasView();
}

cOsgCanvas *ModuleInspector::getOsgCanvas()
{
    cModule *module = dynamic_cast<cModule *>(getObject());
    cOsgCanvas *osgCanvas = module ? module->getOsgCanvasIfExists() : nullptr;
    return osgCanvas;
}

void ModuleInspector::setOsgCanvas(cOsgCanvas *osgCanvas)
{
    if (osgViewer)
        osgViewer->setOsgCanvas(osgCanvas);
    switchToOsgViewAction->setEnabled(osgCanvas != nullptr);
}

void ModuleInspector::onFontChanged()
{
    canvasViewer->setFont(getQtenv()->getCanvasFont());
}

void ModuleInspector::updateToolbarLayout()
{
    if (!toolbarLayout)
        return;

    if (stackedLayout->currentWidget() == canvasViewer) {
        canvasViewer->setLayout(toolbarLayout);

        toolbarLayout->setContentsMargins(
                toolbarSpacing, toolbarSpacing,
                canvasViewer->verticalScrollBar()->width() + toolbarSpacing,
                canvasViewer->horizontalScrollBar()->height() + toolbarSpacing);
    }
    else {
        if (osgViewer)
            osgViewer->setLayout(toolbarLayout);
        // the osg mode never displays scrollbars.
        toolbarLayout->setContentsMargins(toolbarSpacing, toolbarSpacing,
                                          toolbarSpacing, toolbarSpacing);
    }
}

void ModuleInspector::onLayoutVisualizationStarts(cModule *module, QGraphicsScene *layoutingScene)
{
    if ((cObject*)module == object)
        canvasViewer->setLayoutingScene(layoutingScene);
}

void ModuleInspector::onLayoutVisualizationEnds(cModule *module)
{
    if ((cObject*)module == object)
        canvasViewer->setLayoutingScene(nullptr);
}

void ModuleInspector::onModuleLayoutChanged(cModule *module)
{
    if ((cObject*)module == object)
        canvasViewer->setNeedsRedraw();
}

QImage ModuleInspector::getScreenshot()
{
    QImage image = grab().toImage();

    if (stackedLayout->currentWidget() == osgViewer) {
        // QOpenGLWidget can't be simply grab()-bed,
        // so we have to stitch the image back together
        // after capturing the off-screen framebuffer.
        QImage osgImage = osgViewer->grabFramebuffer();
        QPainter p;
        p.begin(&image);
        p.drawImage(osgViewer->mapTo(this, QPoint(0, 0)), osgImage);
        if (!isToplevelInspector())
            p.drawPixmap(toolbar->mapTo(this, QPoint(0, 0)), toolbar->grab());
        p.end();
    }

    return image;
}

cCanvas *ModuleInspector::getCanvas()
{
    cModule *mod = static_cast<cModule *>(object);
    cCanvas *canvas = mod ? mod->getCanvasIfExists() : nullptr;
    return canvas;
}

void ModuleInspector::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int d = event->delta() / 120;  // see the Qt docs for the division
        if (d < 0)
            zoomOut(event->x(), event->y(), -d);
        if (d > 0)
            zoomIn(event->x(), event->y(), d);

        event->accept();
    }
    else {
        Inspector::wheelEvent(event);
    }
}

void ModuleInspector::resizeEvent(QResizeEvent *event)
{
    Inspector::resizeEvent(event);
    updateToolbarLayout();
}

void ModuleInspector::refresh()
{
    Inspector::refresh();

    canvasViewer->refresh();

    if (object != nullptr)
        for (auto p : bubblesToShow)
            canvasViewer->bubble(p.first, p.second.c_str());
    bubblesToShow.clear();

    refreshOsgViewer();
}

void ModuleInspector::refreshOsgViewer()
{
    if (osgViewer) {
        cOsgCanvas *osgCanvas = getOsgCanvas();
        if (osgViewer->getOsgCanvas() != osgCanvas)
            setOsgCanvas(osgCanvas);
        else
            osgViewer->refresh();
    }
}

void ModuleInspector::postRefresh()
{
    cCanvas *canvas = getCanvas();
    if (canvas)
        canvas->getRootFigure()->clearChangeFlags();
}

void ModuleInspector::runUntil()
{
    if (object)
        getQtenv()->runSimulationLocal(RUNMODE_NORMAL, nullptr, this);
}

void ModuleInspector::fastRunUntil()
{
    if (object)
        getQtenv()->runSimulationLocal(RUNMODE_FAST, nullptr, this);
}

void ModuleInspector::relayout()
{
    if (getQtenv()->getSimulationState() != Qtenv::SIM_ERROR) {
        // TODO: use some simpler signals-slots mechanism
        getQtenv()->getMainWindow()->busy("Relayouting...");
        getQtenv()->getModuleLayouter()->fullRelayout(static_cast<cModule*>(object));
        canvasViewer->redraw();
        getQtenv()->callRefreshDisplaySafe();
        getQtenv()->callRefreshInspectors();
        getQtenv()->getMainWindow()->busy();
        getQtenv()->getMessageAnimator()->updateInspector(this);
    }
}

void ModuleInspector::zoomIn(int x, int y, int n)
{
    zoomBy(getPref(PREF_ZOOMBYFACTOR, 1.3).toDouble() * n, true, x, y);
}

void ModuleInspector::zoomOut(int x, int y, int n)
{
    zoomBy(1.0 / (getPref(PREF_ZOOMBYFACTOR, 1.3).toDouble() * n), true, x, y);
}

void ModuleInspector::zoomBy(double mult, bool snaptoone, int x, int y)
{
    if (!object)
        return;

    double zoomFactor = getZoomFactor();

    if ((mult < 1 && zoomFactor > 0.001) || (mult > 1 && zoomFactor < 1000)) {
        int cx = canvasViewer->viewport()->width() / 2;
        int cy = canvasViewer->viewport()->height() / 2;

        // remember canvas scroll position, we'll need it to zoom in/out around ($x,$y)
        if (x == 0)
            x = cx;
        if (y == 0)
            y = cy;

        QPointF oldModulePos = canvasViewer->mapToScene(x, y) / zoomFactor;

        // update zoom factor and redraw
        double newZoomFactor = zoomFactor * mult;

        // snap to true (note: this is not desirable when zoom is set programmatically to fit network into window)
        if (snaptoone) {
            double m = mult < 1 ? 1.0/mult : mult;
            double a = 1 - 0.9*(1 - 1.0/m);
            double b = 1 + 0.9*(m - 1);
            if (newZoomFactor > a && newZoomFactor < b)
                newZoomFactor = 1;
        }

        setPref(PREF_ZOOMFACTOR, newZoomFactor);

        // so animations will not wander around at the old module positions
        canvasViewer->setZoomFactor(newZoomFactor);
        getQtenv()->getMessageAnimator()->updateInspector(this);
        getQtenv()->callRefreshDisplaySafe();
        getQtenv()->callRefreshInspectors();

        auto center = oldModulePos * newZoomFactor - QPointF(x - cx, y - cy);
        canvasViewer->centerOn(center);

        setPref(PREF_CENTER, center.toPoint());
    }
}

double ModuleInspector::getZoomFactor()
{
    return getPref(PREF_ZOOMFACTOR, 1).toDouble();
}

double ModuleInspector::getImageSizeFactor()
{
    return getPref(PREF_ICONSCALE, 1).toDouble();
}

GraphicsLayer *ModuleInspector::getAnimationLayer()
{
    return canvasViewer->getAnimationLayer();
}

QPointF ModuleInspector::getSubmodCoords(cModule *mod)
{
    return canvasViewer->getSubmodCoords(mod);
}

QRectF ModuleInspector::getSubmodRect(cModule *mod)
{
    return canvasViewer->getSubmodRect(mod);
}

QLineF ModuleInspector::getConnectionLine(cGate *gate)
{
    return canvasViewer->getConnectionLine(gate);
}

QRect ModuleInspector::getModuleRect(bool includeBorder, int margin)
{
    if (stackedLayout->currentWidget() == canvasViewer) {
        QRectF rectInScene = canvasViewer->getModuleRect(includeBorder);
        QRect mappedRect = canvasViewer->mapFromScene(rectInScene).boundingRect().adjusted(1, 1, -1, -1); // adjust because of AA
        mappedRect.adjust(-margin, -margin, margin, margin);
        return mappedRect;
    }
    else {
        return rect();
    }
}

void ModuleInspector::submoduleCreated(cModule *newmodule)
{
    canvasViewer->setNeedsRedraw(); // could be optimized by remembering what needs to be drawn, but this is simpler
}

void ModuleInspector::submoduleDeleted(cModule *module)
{
    canvasViewer->setNeedsRedraw(); // could be optimized by remembering what needs to be erased, but this is simpler
}

void ModuleInspector::connectionCreated(cGate *srcgate)
{
    canvasViewer->setNeedsRedraw(); // could be optimized by remembering what needs to be drawn, but this is simpler
}

void ModuleInspector::connectionDeleted(cGate *srcgate)
{
    canvasViewer->setNeedsRedraw(); // could be optimized by remembering what needs to be erased, but this is simpler
}

void ModuleInspector::displayStringChanged(cModule *submodule)
{
    canvasViewer->displayStringChanged(submodule);
}

void ModuleInspector::displayStringChanged()
{
    canvasViewer->displayStringChanged();
}

void ModuleInspector::displayStringChanged(cGate *gate)
{
    canvasViewer->displayStringChanged(gate);
}

void ModuleInspector::redraw()
{
    canvasViewer->redraw();
}

void ModuleInspector::click(QMouseEvent *event)
{
    auto objects = canvasViewer->getObjectsAt(event->pos());
    if (!objects.empty()) {

        // When a click on a figure redirects the selection to the associated object,
        // this little hack will put both of them into the history of the embedded
        // generic object inspector. This is not the most elegant solution...
        // We are using that bit of information that for figures with an assoc.obj.,
        // the assoc.obj. is pushed into the vector right before the figure.
        if (objects.size() >= 2) {
            // just a guess:
            cFigure *figure = dynamic_cast<cFigure *>(objects[1]);
            cObject *assocObj = objects[0];
            // let's check if we were right:
            if (figure && figure->getAssociatedObject() == assocObj)
                emit selectionChanged(figure);
        }

        emit selectionChanged(objects.front());
    }
}

void ModuleInspector::doubleClick(QMouseEvent *event)
{
    auto objects = canvasViewer->getObjectsAt(event->pos());

    if (!objects.empty() && objects.front() != object) {
        if (supportsObject(objects.front()))
            setObject(objects.front());
        else  // If this inspector supports object then no need to inspect one more time.
            emit objectDoubleClicked(objects.front());
    } else {
        if (event->modifiers() & Qt::ShiftModifier)
            zoomOut(event->pos().x(), event->pos().y());
        else
            zoomIn(event->pos().x(), event->pos().y());
    }
}

void ModuleInspector::onViewerDragged(QPointF center)
{
    setPref(PREF_CENTER, center.toPoint());
}

void ModuleInspector::onMarqueeZoom(QRectF rect)
{
    qreal rectLongerSide;
    qreal viewportSide;

    // Zoom factor is based on selection rectangle's longer side
    if (rect.width() < rect.height()) {
        rectLongerSide = rect.height();
        viewportSide = canvasViewer->viewport()->height();
    }
    else {
        rectLongerSide = rect.width();
        viewportSide = canvasViewer->viewport()->width();
    }

    if (rectLongerSide != 0.0) {
        QPointF center = canvasViewer->mapToScene(rect.center().toPoint()) / getZoomFactor();
        zoomBy(viewportSide/rectLongerSide);

        center *= getZoomFactor();
        canvasViewer->centerOn(center);
        setPref(PREF_CENTER, center);
    }
}

void ModuleInspector::onObjectsPicked(const std::vector<cObject *>& objects)
{
    cObject *object = nullptr;
    for (auto& o : objects)
        if (o) {
            object = o;
            break;
        }

    if (object)
        emit selectionChanged(object);
}

void ModuleInspector::createContextMenu(const std::vector<cObject *>& objects, const QPoint& globalPos)
{
    if (!object)
        return;

    auto o = QVector<cObject *>::fromStdVector(objects);
    if (o.empty())
        o.push_back(object);

    QMenu *menu = InspectorUtil::createInspectorContextMenu(o, this);

    menu->addSeparator();
    menu->addAction(canvasLayersAction);

    menu->addSeparator();
    menu->addAction(showMethodCallsAction);
    showMethodCallsAction->setEnabled(getQtenv()->opt->animateMethodCalls);
    menu->addAction(showModuleNamesAction);
    menu->addAction(showArrowheadsAction);

    QMenu *subMenu = menu->addMenu(QString("Submodule Name Format"));
    QActionGroup *actionGroup = new QActionGroup(menu);
    addNameFormatItem(SubmoduleNameFormat::FMT_FULLNAME, "Name", actionGroup, subMenu);
    addNameFormatItem(SubmoduleNameFormat::FMT_QDISPLAYNAME, "\"DisplayName\"", actionGroup, subMenu);
    addNameFormatItem(SubmoduleNameFormat::FMT_FULLNAME_AND_QDISPLAYNAME, "Name \"DisplayName\"", actionGroup, subMenu);

    menu->addSeparator();
    menu->addAction(increaseIconSizeAction);
    menu->addAction(decreaseIconSizeAction);

    menu->addSeparator();
    menu->addAction(canvasZoomInAction);
    menu->addAction(canvasZoomOutAction);
    menu->addAction(canvasRelayoutAction);

    menu->addSeparator();
    menu->addAction("Layouting Settings...", this, SLOT(runPreferencesDialog()))
            ->setData(QVariant::fromValue(TAB_LAYOUTING));
    menu->addAction("Animation Settings...", this, SLOT(runPreferencesDialog()))
            ->setData(QVariant::fromValue(TAB_ANIMATION));
    menu->addAction("Animation Filter...", this, SLOT(runPreferencesDialog()))
            ->setData(QVariant::fromValue(TAB_FILTERING));

    menu->addSeparator();
    menu->addAction("Export to Image...", canvasViewer, SLOT(exportToImage()));
    menu->addAction("Export to PDF...", canvasViewer, SLOT(exportToPdf()));
    menu->addAction("Print...", canvasViewer, SLOT(print()));

    menu->exec(globalPos);
    delete menu;
}

void ModuleInspector::addNameFormatItem(SubmoduleNameFormat format, QString label, QActionGroup *actionGroup, QMenu *subMenu)
{
    QAction *action = subMenu->addAction(label, [=] {setSubmoduleNameFormat(format);});
    action->setCheckable(true);
    action->setChecked(canvasViewer->getSubmoduleNameFormat() == format);
    action->setActionGroup(actionGroup);
}

void ModuleInspector::runPreferencesDialog()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid())
        InspectorUtil::preferencesDialog(variant.value<eTab>());
}

void ModuleInspector::showCanvasLayersDialog()
{
    CanvasRenderer *canvasRenderer = canvasViewer->getCanvasRenderer();
    if (!canvasRenderer->hasCanvas()) {
        QMessageBox::warning(this, tr("Warning"), tr("No default canvas has been created for this module yet."),
                QMessageBox::Ok);
        return;
    }

    LayersDialog(canvasViewer, this).exec();
}

void ModuleInspector::showMethodCalls(bool show)
{
    if (!object)
        return;

    setPref(PREF_SHOWMETHODCALLS, show);
    showMethodCallsAction->setChecked(show);
}

void ModuleInspector::showLabels(bool show)
{
    if (!object)
        return;

    setPref(PREF_SHOWLABELS, show);
    showModuleNamesAction->setChecked(show);
    canvasViewer->setShowModuleNames(show);
    update();
}

void ModuleInspector::setSubmoduleNameFormat(SubmoduleNameFormat format)
{
    if (!object)
        return;

    setPref(PREF_SUBMODULENAMEFORMAT, format);
    canvasViewer->setSubmoduleNameFormat(format);
    update();
}

void ModuleInspector::showArrowheads(bool show)
{
    if (!object)
        return;

    setPref(PREF_SHOWARROWHEADS, show);
    showArrowheadsAction->setChecked(show);
    canvasViewer->setShowArrowheads(show);
    update();
}

void ModuleInspector::zoomIconsBy(double mult)
{
    if (!object)
        return;

    double imageSizeFactor = getPref(PREF_ICONSCALE, 1.0).toDouble();

    if ((mult < 1 && imageSizeFactor > 0.1) || (mult > 1 && imageSizeFactor < 5)) {
        double newImageSizeFactor = imageSizeFactor * mult;
        setPref(PREF_ICONSCALE, newImageSizeFactor);
        canvasViewer->setImageSizeFactor(newImageSizeFactor);
        getQtenv()->getMessageAnimator()->updateInspector(this);
        getQtenv()->getMessageAnimator()->redrawMessages();
        getQtenv()->callRefreshDisplaySafe();
        getQtenv()->callRefreshInspectors();
    }
}

void ModuleInspector::switchToOsgView()
{
    // creating and hooking up the viewer if does not yet exist.
    if (!osgViewer) {
        osgViewer = IOsgViewer::createOne();
        connect(osgViewer, SIGNAL(objectsPicked(const std::vector<cObject *>&)), this, SLOT(onObjectsPicked(const std::vector<cObject *>&)));
        stackedLayout->addWidget(osgViewer);

        connect(resetOsgViewAction, SIGNAL(triggered()), osgViewer, SLOT(applyViewerHints()));
        osgViewer->setOsgCanvas(getOsgCanvas());

        if (toolbarLayout)
            osgViewer->setLayout(toolbarLayout);
    }

    stackedLayout->setCurrentWidget(osgViewer);
    osgViewer->enable();
    updateToolbarLayout();

    switchToCanvasViewAction->setChecked(false);
    switchToOsgViewAction->setChecked(true);

    // show/hide view-specific actions
    canvasRelayoutAction->setVisible(false);
    canvasZoomInAction->setVisible(false);
    canvasZoomOutAction->setVisible(false);
    resetOsgViewAction->setVisible(true);

    setPref(PREF_MODE, 1);
    update();
}

void ModuleInspector::switchToCanvasView()
{
    stackedLayout->setCurrentWidget(canvasViewer);
    updateToolbarLayout();

    if (osgViewer)
        osgViewer->disable();
    switchToCanvasViewAction->setChecked(true);
    switchToOsgViewAction->setChecked(false);

    // show/hide view-specific actions
    canvasRelayoutAction->setVisible(true);
    canvasZoomInAction->setVisible(true);
    canvasZoomOutAction->setVisible(true);
    resetOsgViewAction->setVisible(false);

    if (object) {
        QPointF center = getPref(PREF_CENTER, QPointF()).toPointF();

        // if couldn't read a valid center pref, aligning the top left corners
        // (but if yes, it still has to be called, just with false)
        canvasViewer->recalcSceneRect(center.isNull());
        // otherwise restoring the viewport
        if (!center.isNull())
            canvasViewer->centerOn(center);

        setPref(PREF_MODE, 0);
    }
    else {
        canvasViewer->setSceneRect(QRect());
    }

    update();
}

}  // namespace qtenv
}  // namespace omnetpp

