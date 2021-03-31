//==========================================================================
//  OUTPUTVECTORINSPECTOR.CC - part of
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
#include <cmath>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMenu>
#include "omnetpp/coutvector.h"
#include "qtenv.h"
#include "qtutil.h"
#include "inspectorfactory.h"
#include "outputvectorinspector.h"
#include "outputvectorinspectorconfigdialog.h"
#include "outputvectorview.h"

namespace omnetpp {
namespace qtenv {

static const double SNAP_Y_AXIS_TO_ZERO_FACTOR = 0.3;
static const double Y_RANGE = 1;  // use when  minY>=maxY
static const QColor BACKGROUND_COLOR("#FAF3D2");

void _dummy_for_outputvectorinspector() {}

class OutputVectorInspectorFactory : public InspectorFactory
{
  public:
    OutputVectorInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cOutVector *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new OutputVectorInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(OutputVectorInspectorFactory);

CircBuffer::CircBuffer(int size)
{
    siz = size;
    buf = new CBEntry[siz];
    n = 0;
    head = 0;
}

CircBuffer::~CircBuffer()
{
    delete[] buf;
}

void CircBuffer::add(simtime_t t, double value)
{
    head = (head+1)%siz;
    CBEntry& p = buf[head];
    p.t = t;
    p.value = value;
    if (n < siz)
        n++;
}

static void record_in_insp(void *data, simtime_t t, double val)
{
    OutputVectorInspector *insp = (OutputVectorInspector *)data;
    insp->circbuf.add(t, val);
}

OutputVectorInspector::OutputVectorInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) :
    Inspector(parent, isTopLevel, f),
    circbuf(100000)
{
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(QBrush(BACKGROUND_COLOR));

    view = new OutputVectorView(scene);

    // Add statusbar
    statusBar = new QStatusBar();

    QToolBar *toolBar = new QToolBar();
    addTopLevelToolBarActions(toolBar);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(toolBar);
    layout->addWidget(view);
    layout->addWidget(statusBar);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

OutputVectorInspector::~OutputVectorInspector()
{
    if (object) {
        // cancel installed callback in inspected outvector
        cOutVector *ov = static_cast<cOutVector *>(object);
        ov->setCallback(nullptr, nullptr);
    }
}

void OutputVectorInspector::doSetObject(cObject *obj)
{
    if (object) {
        cOutVector *ov = static_cast<cOutVector *>(object);
        ov->setCallback(nullptr, nullptr);
    }

    Inspector::doSetObject(obj);

    if (object) {
        // make inspected output vector to call us back when it gets data to write out
        cOutVector *ov = static_cast<cOutVector *>(object);
        ov->setCallback(record_in_insp, (void *)this);
    }
}

void OutputVectorInspector::refresh()
{
    Inspector::refresh();

    if (!object) {
        view->setVisibleAllItem(false);
        statusBar->showMessage("");
        return;
    }
    else
        view->setVisibleAllItem(true);

    statusBar->showMessage(generalInfo());

    if (circbuf.items() == 0)
        return;

    simtime_t maxT = circbuf.entry(circbuf.headPos()).t;
    view->setMaxT(maxT);

    if (isTimeScaleAutoscaled)
        timeScale = (circbuf.entry(circbuf.headPos()).t - circbuf.entry(circbuf.tailPos()).t).dbl();

    // timeScale have to be > 0
    if (timeScale == 0) {
        timeScale = 1;
        isTimeScaleAutoscaled = true;
    }

    view->setMinT(maxT - timeScale);

    if (isMinYAutoscaled || isMaxYAutoscaled) {
        // determine minY/maxY
        if (isMinYAutoscaled)
            minY = circbuf.entry(circbuf.headPos()).value;
        if (isMaxYAutoscaled)
            maxY = circbuf.entry(circbuf.headPos()).value;

        for (int i = 0, pos = circbuf.tailPos(); i < circbuf.items(); ++i, pos = (pos + 1) % circbuf.size()) {
            CircBuffer::CBEntry& p = circbuf.entry(pos);
            if (isMinYAutoscaled && p.value < minY && p.t >= maxT - timeScale)
                minY = p.value;
            if (isMaxYAutoscaled && p.value > maxY && p.t >= maxT - timeScale)
                maxY = p.value;
        }
    }

    if (minY >= maxY)
        yRangeCorrection();

    // Snap to zero if it's close to the range
    if (isMinYAutoscaled && minY > 0 && minY < SNAP_Y_AXIS_TO_ZERO_FACTOR * (maxY - minY))
        minY = 0;
    else if (isMaxYAutoscaled && maxY < 0 && -maxY < SNAP_Y_AXIS_TO_ZERO_FACTOR * (maxY - minY))
        maxY = 0;

    view->setMinY(minY);
    view->setMaxY(maxY);
    view->draw(circbuf);
}

void OutputVectorInspector::resizeEvent(QResizeEvent *)
{
    refresh();
}

QString OutputVectorInspector::generalInfo()
{
    if (circbuf.items() == 0)
        return "(no write since opening window)";

    CircBuffer::CBEntry& p = circbuf.entry(circbuf.headPos());
    return QString("Last value: t=%1  value=%2").arg(SIMTIME_STR(p.t), QString::number(p.value));
}

void OutputVectorInspector::yRangeCorrection()
{
    if (isMinYAutoscaled && isMaxYAutoscaled) {
        minY -= Y_RANGE / 2;
        maxY += Y_RANGE / 2;
    }
    else if (isMinYAutoscaled)
        minY = maxY - Y_RANGE;
    else if (isMaxYAutoscaled)
        maxY = minY + Y_RANGE;
}

void OutputVectorInspector::onOptionsDialogTriggerd()
{
    configDialog = new OutputVectorInspectorConfigDialog(view->getPlottingMode());

    if (!isMinYAutoscaled)
        configDialog->setMinY(minY);
    if (!isMaxYAutoscaled)
        configDialog->setMaxY(maxY);
    if (!isTimeScaleAutoscaled)
        configDialog->setTimeScale(timeScale);

    connect(configDialog, SIGNAL(applyButtonClicked()), this, SLOT(onClickApply()));
    configDialog->exec();

    if (configDialog->result() == QDialog::Accepted)
        setConfig();

    delete configDialog;
}

void OutputVectorInspector::onClickApply()
{
    setConfig();
}

void OutputVectorInspector::setConfig()
{
    // store new parameters
    isMinYAutoscaled = !configDialog->hasMinY();
    if (!isMinYAutoscaled)
        minY = configDialog->getMinY();

    isMaxYAutoscaled = !configDialog->hasMaxY();
    if (!isMaxYAutoscaled)
        maxY = configDialog->getMaxY();

    isTimeScaleAutoscaled = !configDialog->hasTimeScale();
    if (!isTimeScaleAutoscaled)
        timeScale = configDialog->getTimeScale();

    // identify drawing style
    view->setPlottingMode(configDialog->getPlottingMode());

    refresh();
}

void OutputVectorInspector::onCustomContextMenuRequested(QPoint pos)
{
    QMenu menu;

    menu.addAction("Options...", this, SLOT(onOptionsDialogTriggerd()));
    // if mouse pos isn't contained by viewport, then context menu will appear in view (10, 10) pos
    QPoint eventPos = view->viewport()->rect().contains(pos) ? pos : QPoint(10, 10);
    menu.exec(view->mapToGlobal(eventPos));
}

}  // namespace qtenv
}  // namespace omnetpp

