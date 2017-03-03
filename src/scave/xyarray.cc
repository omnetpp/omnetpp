//==========================================================================
//  DATASORTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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

template<typename T> static void permute(T *& array, int size, const vector<int>& permutation)
{
    // XXX could it be performed in place?
    T *oldArray = array;
    array = new T[size];
    for (int i = 0; i < size; ++i)
        array[i] = oldArray[permutation[i]];
    delete[] oldArray;
}

void XYArray::sortByX()
{
    if (x) {
        vector<pair<double, int> > xCoords;
        for (int i = 0; i < len; ++i)
            xCoords.push_back(make_pair(x[i], i));

        sort(xCoords.begin(), xCoords.end());
        vector<int> permutation;
        for (int i = 0; i < len; ++i) {
            x[i] = xCoords[i].first;
            permutation.push_back(xCoords[i].second);
        }

        if (y)
            permute(y, len, permutation);

        if (xp)
            permute(xp, len, permutation);

        if (evec)
            permute(evec, len, permutation);
    }
}

}  // namespace scave
}  // namespace omnetpp

