//==========================================================================
//  HISTOGRAMINSPECTOR.CC - part of
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
#include <QGraphicsView>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include "omnetpp/cdensityestbase.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "histograminspector.h"
#include "histograminspectorconfigdialog.h"

//unused? static const double SNAP_Y_AXIS_TO_ZERO_FACTOR = 0.3;
static const double X_RANGE = 1;  // use when minX>=maxX
static const double Y_RANGE = 1;  // use when minY>=maxY
static const QColor BACKGROUND_COLOR("#E2E8FA");

namespace omnetpp {
namespace qtenv {

void _dummy_for_histograminspector() {}

class HistogramInspectorFactory : public InspectorFactory
{
  public:
    HistogramInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cDensityEstBase *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new HistogramInspector(parent, isTopLevel, this); }
};

// TODO: uncomment when the inspector implementation in the topic branch is completed and merged.
Register_InspectorFactory(HistogramInspectorFactory);

HistogramInspector::HistogramInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) :
    Inspector(parent, isTopLevel, f),
    chartType(HistogramView::SHOW_PDF),
    drawingStyle(HistogramView::DRAW_FILLED),
    isCountsMinYAutoscaled(true),
    isCountsMaxYAutoscaled(true),
    isCountsMinXAutoscaled(true),
    isCountsMaxXAutoscaled(true),
    isPDFMinYAutoscaled(true),
    isPDFMaxYAutoscaled(true),
    isPDFMinXAutoscaled(true),
    isPDFMaxXAutoscaled(true),
    countsMinY(0),
    countsMinX(0),
    pdfMinY(0),
    pdfMinX(0)
{
    QGraphicsScene *scene = new QGraphicsScene();
    // Set background color to LightBlue
    scene->setBackgroundBrush(QBrush(BACKGROUND_COLOR));

    view = new HistogramView(scene);
    statusBar = new QStatusBar();

    QToolBar *toolBar = new QToolBar();
    addTopLevelToolBarActions(toolBar);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(toolBar);
    layout->addWidget(view);
    layout->addWidget(statusBar);

    layout->setContentsMargins(0, 0, 1, 1);

    setLayout(layout);
    connect(view, SIGNAL(showCellInfo(int)), this, SLOT(onShowCellInfo(int)));
    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

void HistogramInspector::resizeEvent(QResizeEvent *)
{
    refresh();
}

void HistogramInspector::refresh()
{
    Inspector::refresh();

    if (!object) {
        view->scene()->clear();
        statusBar->showMessage("");
        return;
    }

    cDensityEstBase *distr = static_cast<cDensityEstBase *>(object);
    statusBar->showMessage(generalInfo());

    // can we draw anything at all?
    if (!distr->isTransformed() || distr->getNumCells() == 0)
        return;

    bool isMinYAutoscaled, isMaxYAutoscaled, isMaxXAutoscaled, isMinXAutoscaled;
    double minYVal, maxYVal, minXVal, maxXVal;
    if (chartType == HistogramView::SHOW_PDF) {
        isMinYAutoscaled = isPDFMinYAutoscaled;
        isMaxYAutoscaled = isPDFMaxYAutoscaled;
        isMaxXAutoscaled = isPDFMaxXAutoscaled;
        isMinXAutoscaled = isPDFMinXAutoscaled;
        minYVal = pdfMinY;
        maxYVal = isPDFMaxYAutoscaled ? maxY() : pdfMaxY;
        minXVal = isPDFMinXAutoscaled ? distr->getBasepoint(0) : pdfMinX;
        maxXVal = isPDFMaxXAutoscaled ? distr->getBasepoint(distr->getNumCells()) : pdfMaxX;
    }
    else {
        isMinYAutoscaled = isCountsMinYAutoscaled;
        isMaxYAutoscaled = isCountsMaxYAutoscaled;
        isMaxXAutoscaled = isCountsMaxXAutoscaled;
        isMinXAutoscaled = isCountsMinXAutoscaled;
        minYVal = countsMinY;
        maxYVal = isCountsMaxYAutoscaled ? maxY() : countsMaxY;
        minXVal = isCountsMinXAutoscaled ? distr->getBasepoint(0) : countsMinX;
        maxXVal = isCountsMaxXAutoscaled ? distr->getBasepoint(distr->getNumCells()) : countsMaxX;
    }

    if (minXVal >= maxXVal)
        xRangeCorrection(minXVal, maxXVal, isMinXAutoscaled, isMaxXAutoscaled);
    if (minYVal >= maxYVal)
        yRangeCorrection(minYVal, maxYVal, isMinYAutoscaled, isMaxYAutoscaled);

    if (isMaxYAutoscaled) {
        if (lastMaxY < 0)
            lastMaxY = minYVal + (maxYVal - minYVal) / 0.85;

        if ((maxYVal < minYVal + (0.75 * (lastMaxY - minYVal)))
                || (maxYVal > minYVal + 0.95 * (lastMaxY - minYVal)))
            maxYVal = minYVal + (maxYVal - minYVal) / 0.85;
        else
            maxYVal = lastMaxY;
    }

    lastMaxY = maxYVal;

    view->setMinX(minXVal);
    view->setMaxX(maxXVal);
    view->setMinY(minYVal);
    view->setMaxY(maxYVal);

    view->draw(chartType, drawingStyle, distr);
}

double HistogramInspector::maxY()
{
    // determine maximum height (will be used for y scaling)
    double maxY = -1.0;  // a good start because all y values are >=0
    cDensityEstBase *distr = static_cast<cDensityEstBase *>(object);
    for (int cell = 0; cell < distr->getNumCells(); cell++) {
        // calculate height
        double y = chartType == HistogramView::SHOW_PDF ? distr->getCellPDF(cell) : distr->getCellValue(cell);
        if (y > maxY)
            maxY = y;
    }

    return maxY;
}

void HistogramInspector::yRangeCorrection(double& minY, double& maxY, bool isMinYAutoscaled, bool isMaxYAutoscaled)
{
    if (isMinYAutoscaled && isMaxYAutoscaled) {
        if (maxY == 0)
            maxY = Y_RANGE;
        minY = 0;
    }
    else if (isMinYAutoscaled)
        minY = maxY - Y_RANGE;
    else if (isMaxYAutoscaled)
        maxY = minY + Y_RANGE;
}

void HistogramInspector::xRangeCorrection(double& minX, double& maxX, bool isMinXAutoscaled, bool isMaxXAutoscaled)
{
    if (isMinXAutoscaled && isMaxXAutoscaled) {
        minX -= X_RANGE / 2;
        maxX += X_RANGE / 2;
    }
    else if (isMinXAutoscaled)
        minX = maxX - X_RANGE;
    else if (isMaxXAutoscaled)
        maxX = minX + X_RANGE;
}

QString HistogramInspector::generalInfo()
{
    cDensityEstBase *d = static_cast<cDensityEstBase *>(object);
    if (!d->isTransformed())
        return QString("(collecting initial values, N=%1)").arg(QString::number(d->getCount()));
    else
        return QString("Histogram: (%1...%2)  N=%3  #cells=%4").arg(
                QString::number(d->getBasepoint(0)), QString::number(d->getBasepoint(d->getNumCells())),
                QString::number(d->getCount()), QString::number(d->getNumCells()));
}

void HistogramInspector::onShowCellInfo(int cell)
{
    if (!object) {
        statusBar->showMessage("");
        return;
    }

    if (cell == -1) {
        statusBar->showMessage(generalInfo());
        return;
    }

    cDensityEstBase *d = static_cast<cDensityEstBase *>(object);
    double count = d->getCellValue(cell);
    double cell_lower = d->getBasepoint(cell);
    double cell_upper = d->getBasepoint(cell+1);
    QString text = "Cell #%1:  [%2...%3)  n=%4  PDF=%5";
    text = text.arg(QString::number(cell), QString::number(cell_lower), QString::number(cell_upper),
                QString::number(count), QString::number(count / (double)(d->getCount()) / (cell_upper-cell_lower)));

    statusBar->showMessage(text);
}

void HistogramInspector::onCustomContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu();

    QActionGroup *actionGroup = new QActionGroup(menu);
    QAction *action = menu->addAction("Show Counts", this, SLOT(onShowCounts()));
    action->setCheckable(true);
    action->setChecked(chartType == HistogramView::SHOW_COUNTS);
    action->setActionGroup(actionGroup);

    action = menu->addAction("Show Density Function", this, SLOT(onShowPDF()));
    action->setCheckable(true);
    action->setChecked(chartType == HistogramView::SHOW_PDF);
    action->setActionGroup(actionGroup);

    menu->addSeparator();
    menu->addAction("Options...", this, SLOT(onOptionsTriggered()));

    // if mouse pos isn't contained by viewport, then context menu will appear in view (10, 10) pos
    QPoint eventPos = view->viewport()->rect().contains(pos) ? pos : QPoint(10, 10);
    menu->exec(view->mapToGlobal(eventPos));
}

void HistogramInspector::onShowCounts()
{
    if (chartType != HistogramView::SHOW_COUNTS) {
        chartType = HistogramView::SHOW_COUNTS;
        refresh();
    }
}

void HistogramInspector::onShowPDF()
{
    if (chartType != HistogramView::SHOW_PDF) {
        chartType = HistogramView::SHOW_PDF;
        refresh();
    }
}

void HistogramInspector::setConfig()
{
    bool isMinYAutoscaled = !configDialog->hasMinY();
    bool isMaxYAutoscaled = !configDialog->hasMaxY();
    bool isMinXAutoscaled = !configDialog->hasMinX();
    bool isMaxXAutoscaled = !configDialog->hasMaxX();

    // Based on data all y values are >=0
    double minXVal, maxXVal, minYVal, maxYVal;
    minYVal = isMinYAutoscaled ? 0 : configDialog->getMinY();
    if (!isMaxYAutoscaled)
        maxYVal = configDialog->getMaxY();
    if (!isMinXAutoscaled)
        minXVal = configDialog->getMinX();
    if (!isMaxXAutoscaled)
        maxXVal = configDialog->getMaxX();

    if (chartType == HistogramView::SHOW_PDF) {
        isPDFMinYAutoscaled = isMinYAutoscaled;
        isPDFMaxYAutoscaled = isMaxYAutoscaled;
        isPDFMinXAutoscaled = isMinXAutoscaled;
        isPDFMaxXAutoscaled = isMaxXAutoscaled;
        pdfMinY = minYVal;
        pdfMaxY = maxYVal;
        pdfMinX = minXVal;
        pdfMaxX = maxXVal;
    }
    else {
        isCountsMinYAutoscaled = isMinYAutoscaled;
        isCountsMaxYAutoscaled = isMaxYAutoscaled;
        isCountsMinXAutoscaled = isMinXAutoscaled;
        isCountsMaxXAutoscaled = isMaxXAutoscaled;
        countsMinY = minYVal;
        countsMaxY = maxYVal;
        countsMinX = minXVal;
        countsMaxX = maxXVal;
    }

    drawingStyle = configDialog->getDrawingStyle();

    refresh();
}

void HistogramInspector::onOptionsTriggered()
{
    configDialog = new HistogramInspectorConfigDialog(drawingStyle);

    bool isMinYAutoscaled, isMaxYAutoscaled, isMinXAutoscaled, isMaxXAutoscaled;
    double minYVal, maxYVal, minXVal, maxXVal;
    if (chartType == HistogramView::SHOW_PDF) {
        isMinYAutoscaled = isPDFMinYAutoscaled;
        isMaxYAutoscaled = isPDFMaxYAutoscaled;
        isMinXAutoscaled = isPDFMinXAutoscaled;
        isMaxXAutoscaled = isPDFMaxXAutoscaled;
        minYVal = pdfMinY;
        maxYVal = pdfMaxY;
        minXVal = pdfMinX;
        maxXVal = pdfMaxX;
    }
    else {
        isMinYAutoscaled = isCountsMinYAutoscaled;
        isMaxYAutoscaled = isCountsMaxYAutoscaled;
        isMinXAutoscaled = isCountsMinXAutoscaled;
        isMaxXAutoscaled = isCountsMaxXAutoscaled;
        minYVal = countsMinY;
        maxYVal = countsMaxY;
        minXVal = countsMinX;
        maxXVal = countsMaxX;
    }

    if (!isMinYAutoscaled)
        configDialog->setMinY(minYVal);
    if (!isMaxYAutoscaled)
        configDialog->setMaxY(maxYVal);
    if (!isMinXAutoscaled)
        configDialog->setMinX(minXVal);
    if (!isMaxXAutoscaled)
        configDialog->setMaxX(maxXVal);

    connect(configDialog, SIGNAL(applyButtonClicked()), this, SLOT(onApplyButtonClicked()));
    configDialog->exec();

    if (configDialog->result() == QDialog::Accepted)
        setConfig();

    delete configDialog;
}

void HistogramInspector::onApplyButtonClicked()
{
    setConfig();
}

}  // namespace qtenv
}  // namespace omnetpp

