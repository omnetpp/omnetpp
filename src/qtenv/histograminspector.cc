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
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMenu>
#include "omnetpp/cabstracthistogram.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "histograminspector.h"
#include "histograminspectorconfigdialog.h"

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

    bool supportsObject(cObject *obj) override { return dynamic_cast<cAbstractHistogram *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new HistogramInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(HistogramInspectorFactory);

HistogramInspector::HistogramInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) :
    Inspector(parent, isTopLevel, f)
{
    QGraphicsScene *scene = new QGraphicsScene();
    // Set background color to LightBlue
    scene->setBackgroundBrush(QBrush(BACKGROUND_COLOR));

    view = new HistogramView(scene);
    statusBar = new QStatusBar();

    QToolBar *toolBar = new QToolBar();
    addTopLevelToolBarActions(toolBar);

    toolBar->addSeparator();
    setUpBinsAction = toolBar->addAction(QIcon(":/tools/setupbins"), "Set bins up now", this, SLOT(onSetUpBinsClicked()));

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

    cAbstractHistogram *distr = static_cast<cAbstractHistogram *>(object);
    statusBar->showMessage(generalInfo());
    setUpBinsAction->setEnabled(!distr->binsAlreadySetUp());

    // can we draw anything at all?
    if (!distr->binsAlreadySetUp() || distr->getNumBins() == 0)
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
        minXVal = isPDFMinXAutoscaled ? (distr->getUnderflowSumWeights() > 0 ? distr->getMin() : distr->getBinEdge(0)) : pdfMinX;
        maxXVal = isPDFMaxXAutoscaled ? (distr->getOverflowSumWeights() > 0 ? distr->getMax() : distr->getBinEdge(distr->getNumBins())) : pdfMaxX;
    }
    else {
        isMinYAutoscaled = isCountsMinYAutoscaled;
        isMaxYAutoscaled = isCountsMaxYAutoscaled;
        isMaxXAutoscaled = isCountsMaxXAutoscaled;
        isMinXAutoscaled = isCountsMinXAutoscaled;
        minYVal = countsMinY;
        maxYVal = isCountsMaxYAutoscaled ? maxY() : countsMaxY;
        minXVal = isCountsMinXAutoscaled ? (distr->getUnderflowSumWeights() > 0 ? distr->getMin() : distr->getBinEdge(0)) : countsMinX;
        maxXVal = isCountsMaxXAutoscaled ? (distr->getOverflowSumWeights() > 0 ? distr->getMax() : distr->getBinEdge(distr->getNumBins())) : countsMaxX;
    }

    if (minXVal >= maxXVal)
        xRangeCorrection(minXVal, maxXVal, isMinXAutoscaled, isMaxXAutoscaled);
    if (minYVal >= maxYVal)
        yRangeCorrection(minYVal, maxYVal, isMinYAutoscaled, isMaxYAutoscaled);

    if (isMaxYAutoscaled) {
        // computing the top of the y scale before adjustment (if this is the first time we do this)
        if (lastMaxY < 0)
            lastMaxY = minYVal + (maxYVal - minYVal) / AUTOSCALE_Y_RATIO;

        // We add some hysteresis to the rescaling of the Y axis.
        if ((maxYVal < minYVal + (AUTOSCALE_Y_HYST_LOW * (lastMaxY - minYVal)))
                || (maxYVal > minYVal + AUTOSCALE_Y_HYST_HIGH * (lastMaxY - minYVal)))
            maxYVal = minYVal + (maxYVal - minYVal) / AUTOSCALE_Y_RATIO;
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
    cAbstractHistogram *distr = static_cast<cAbstractHistogram *>(object);
    for (int bin = 0; bin < distr->getNumBins(); bin++) {
        // calculate height
        double y = chartType == HistogramView::SHOW_PDF ? distr->getBinPDF(bin) : distr->getBinValue(bin);
        if (y > maxY && std::isfinite(y))
            maxY = y;
    }

    double underflowY;
    double overflowY;

    if (chartType == HistogramView::SHOW_PDF) {
        underflowY = distr->getUnderflowSumWeights() / distr->getSumWeights() / (distr->getBinEdge(0) - distr->getMin());
        overflowY = distr->getOverflowSumWeights() / distr->getSumWeights() / (distr->getMax() - distr->getBinEdge(distr->getNumBins()));
    }
    else {
         underflowY = distr->getUnderflowSumWeights();
         overflowY = distr->getOverflowSumWeights();
    }

    if (std::isfinite(underflowY) && underflowY > maxY)
        maxY = underflowY;
    if (std::isfinite(overflowY) && overflowY > maxY)
        maxY = overflowY;

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
    cAbstractHistogram *d = static_cast<cAbstractHistogram *>(object);
    if (!d->binsAlreadySetUp())
        return QString("Collecting initial values, N=%1  [%2, %3]").arg(
                QString::number(d->getCount()),
                QString::number(d->getMin()), QString::number(d->getMax())
                );
    else if (!d->isWeighted())
        return QString("Histogram: [%1, %2)  N=%3  #bins=%4  Outliers: lower=%5 upper=%6").arg(
                QString::number(d->getBinEdge(0)), QString::number(d->getBinEdge(d->getNumBins())),
                QString::number(d->getCount()), QString::number(d->getNumBins()),
                QString::number(d->getNumUnderflows()), QString::number(d->getNumOverflows())
                );
    else
        return QString("Histogram: [%1, %2)  N=%3  W=%4  #bins=%5  Outliers: lower=%6 upper=%7").arg(
                QString::number(d->getBinEdge(0)), QString::number(d->getBinEdge(d->getNumBins())),
                QString::number(d->getCount()), QString::number(d->getSumWeights()), QString::number(d->getNumBins()),
                QString::number(d->getUnderflowSumWeights()), QString::number(d->getOverflowSumWeights())
                );
}

void HistogramInspector::onShowCellInfo(int bin)
{
    if (!object) {
        statusBar->showMessage("");
        return;
    }

    if (bin == INT_MIN) {
        // no bin selected
        statusBar->showMessage(generalInfo());
        return;
    }

    cAbstractHistogram *d = static_cast<cAbstractHistogram *>(object);

    QString binName;
    double binValue;
    double lowerEdge;
    double upperEdge;
    QString binValueText;

    if (bin == -1) {
        // underflows
        binName = "Underflows";
        binValue = d->getUnderflowSumWeights();
        lowerEdge = d->getMin();
        upperEdge = d->getBinEdge(0);
        binValueText = QString("w=%1").arg(QString::number(binValue));
    }
    else if (bin == d->getNumBins()) {
        // overflows
        binName = "Overflows";
        binValue = d->getOverflowSumWeights();
        lowerEdge = d->getBinEdge(d->getNumBins());
        upperEdge = d->getMax();
        binValueText = QString("w=%1").arg(QString::number(binValue));
    }
    else {
        // regular histogram bin
        binName = QString("Bin #%1").arg(QString::number(bin));
        binValue = d->getBinValue(bin);
        lowerEdge = d->getBinEdge(bin);
        upperEdge = d->getBinEdge(bin+1);
        binValueText = "w=" + QString::number(binValue);
    }

    QString text = QString("%1:  [%2, %3)  %4  PDF=%5").arg(binName, QString::number(lowerEdge),
                    QString::number(upperEdge), binValueText,
                    QString::number(binValue / d->getSumWeights() / (upperEdge-lowerEdge)));

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

void HistogramInspector::onSetUpBinsClicked()
{
    cAbstractHistogram *d = static_cast<cAbstractHistogram *>(object);
    if (d && !d->binsAlreadySetUp()) {
        d->setUpBins();
        refresh();
    }
}

}  // namespace qtenv
}  // namespace omnetpp

