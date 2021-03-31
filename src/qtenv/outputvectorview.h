//==========================================================================
//  OUTPUTVECTORVIEW.H - part of
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

#ifndef __OMNETPP_QTENV_OUTPUTVECTORVIEW_H
#define __OMNETPP_QTENV_OUTPUTVECTORVIEW_H

#include <QtWidgets/QGraphicsView>
#include "omnetpp/simtime_t.h"
#include "outputvectorinspector.h"
#include "vectorplotitem.h"

namespace omnetpp {
namespace qtenv {

class ChartGridItem;

class QTENV_API OutputVectorView : public QGraphicsView
{
  private:
    ChartGridItem *gridItem;
    VectorPlotItem *plotItem;

  protected:
    // Disable mouse scrolling
    void scrollContentsBy(int, int) override {}

  public:
    OutputVectorView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void draw(const CircBuffer& circbuf);
    void setMinY(const double minY);
    void setMaxY(const double maxY);
    void setMinT(const simtime_t& minT);
    void setMaxT(const simtime_t& maxT);

    VectorPlotItem::PlottingMode getPlottingMode() const;
    void setPlottingMode(const VectorPlotItem::PlottingMode drawingMode);
    void setVisibleAllItem(bool isVisible);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

