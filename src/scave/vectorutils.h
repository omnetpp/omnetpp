//=========================================================================
//  VECTORUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_VECTORUTILS_H
#define __OMNETPP_SCAVE_VECTORUTILS_H

#include <limits>
#include "scavedefs.h"
#include "resultfilemanager.h"
#include "xyarray.h"
#include "common/stlutil.h"
#include "common/stringutil.h"

namespace omnetpp {
namespace scave {

/**
 * Read the VectorResult items in the IDList into the XYArrays.
 */
SCAVE_API std::vector<XYArray *> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, bool includePreciseX, bool includeEventNumbers, size_t memoryLimitBytes = std::numeric_limits<size_t>::max(), double simTimeStart = -INFINITY, double simTimeEnd = INFINITY, InterruptedFlag *interrupted=nullptr);

/**
  * This class simply wraps the std::vector<XYArray *> to make it usable from Java.
 */
class SCAVE_API XYArrayVector {

    std::vector<XYArray *> data;

  public:
    explicit XYArrayVector(std::vector<XYArray *> &&data) : data(data) { }

    XYArrayVector(const XYArrayVector &other) = delete;

    size_t size() const { return data.size(); }
    XYArray &get(int i) { return *data.at(i); }

    ~XYArrayVector() {
        for (XYArray * a: data)
            delete a;
    }
};

/*
 * The same as readVectorsIntoArrays, except the result is wrapped into an XYArrayVector.
 * This is just to make the data usable from Java.
 */
SCAVE_API XYArrayVector *readVectorsIntoArrays2(ResultFileManager *manager, const IDList& idlist, bool includePreciseX, bool includeEventNumbers, size_t memoryLimitBytes = std::numeric_limits<size_t>::max(), double simTimeStart = -INFINITY, double simTimeEnd = INFINITY, InterruptedFlag *interrupted=nullptr);

}  // namespace scave
}  // namespace omnetpp

#endif


