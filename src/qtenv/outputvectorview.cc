//==========================================================================
//  OUTPUTVECTORVIEW.CC - part of
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

#include "outputvectorview.h"
#include <QtGui/QResizeEvent>
#include "outputvectorinspector.h"
#include "vectorplotitem.h"
#include "chartgriditem.h"

// unused? static const int VERT_TEXT_DISTANCE = 5;
static const int MARGIN = 10;

namespace omnetpp {
namespace qtenv {

using namespace common;

OutputVectorView::OutputVectorView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setContextMenuPolicy(Qt::CustomContextMenu);

    gridItem = new ChartGridItem();
    plotItem = new VectorPlotItem(gridItem->getDiagramFrame());
    plotItem->setColor(Qt::red);
    gridItem->setPos(MARGIN, MARGIN);
    scene->addItem(gridItem);
}

void OutputVectorView::setMinY(const double minY)
{
    gridItem->setMinY(minY);
    plotItem->setMinY(minY);
}

void OutputVectorView::setMaxY(const double maxY)
{
    gridItem->setMaxY(maxY);
    plotItem->setMaxY(maxY);
}

void OutputVectorView::setMinT(const simtime_t& minT)
{
    gridItem->setMinX(minT.dbl());
    plotItem->setMinX(minT.dbl());
}

void OutputVectorView::setMaxT(const simtime_t& maxT)
{
    gridItem->setMaxX(maxT.dbl());
    plotItem->setMaxX(maxT.dbl());
}

VectorPlotItem::PlottingMode OutputVectorView::getPlottingMode() const
{
    return plotItem->getPlottingMode();
}

void OutputVectorView::setPlottingMode(const VectorPlotItem::PlottingMode drawingMode)
{
    plotItem->setPlottingMode(drawingMode);
}

void OutputVectorView::draw(const CircBuffer& circbuf)
{
    setSceneRect(rect());
    gridItem->setRect(QRectF(0, 0, width() - 2 * MARGIN, height() - 2 * MARGIN));
    plotItem->setData(&circbuf);
}

void OutputVectorView::setVisibleAllItem(bool isVisible)
{
    gridItem->setVisible(isVisible);
    plotItem->setVisible(isVisible);
}

}  // namespace qtenv
}  // namespace omnetpp

