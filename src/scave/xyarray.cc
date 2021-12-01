//==========================================================================
//  DATASORTER.CC - part of
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

#include <algorithm>
#include <vector>
#include <utility>
#include "xyarray.h"

namespace omnetpp {
namespace scave {

using namespace std;

XYArray::XYArray(std::vector<double> &&xs, std::vector<double> &&ys, std::vector<BigDecimal> &&xps, std::vector<eventnumber_t> &&ens) {

    Assert(xs.size() == ys.size());
    Assert(xps.empty() || xps.size() == xs.size());
    Assert(ens.empty() || ens.size() == xs.size());

    this->xs = xs;
    this->ys = ys;
    this->xps = xps;
    this->ens = ens;
}

}  // namespace scave
}  // namespace omnetpp

