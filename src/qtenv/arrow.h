//==========================================================================
//  ARROW.H - part of
//
//                     OMNeT++/OMNEST
//
//  Contents:
//   connection arrow positioning in module drawing
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_ARROW_H
#define __OMNETPP_QTENV_ARROW_H

#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QLineF>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

// just a bit more convenient wrapper for the other function
QTENV_API QLineF arrowcoords(const QRectF &srcRect, const QRectF &destRect,
                  int src_i = 0, int src_n = 1, // src vector gate index and size
                  int dest_i = 0, int dest_n = 1, // src vector gate index and size
                  char mode = 'a', // must be one of "amnews"
                  QPointF srcAnch = QPointF(50, 50),
                  QPointF destAnch = QPointF(50, 50));

QTENV_API QLineF arrowcoords(double src_x1, double src_y1, double src_x2, double src_y2, // src rect
                  double dest_x1, double dest_y1, double dest_x2, double dest_y2, // dest rect
                  int src_i = 0, int src_n = 1, // src vector gate index and size
                  int dest_i = 0, int dest_n = 1, // src vector gate index and size
                  char mode = 'a', // must be one of "amnews"
                  double src_anch_dx = 50, double src_anch_dy = 50, // src anchor percentages
                  double dest_anch_dx = 50, double dest_anch_dy = 50); // dest anchor percentages


}  // namespace qtenv
}  // namespace omnetpp


#endif

