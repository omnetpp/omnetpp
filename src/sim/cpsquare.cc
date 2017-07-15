//=========================================================================
//  CPSQUARE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//     cPSquare: calculates quantile values without storing the observations
//
//   Written by Babak Fakhamzadeh, TU Delft, Dec 1996
//   Modifications by Andras Varga
//
//=========================================================================
/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cpsquare.h"
#include "omnetpp/cexception.h"
#include "omnetpp/distrib.h"
#include "omnetpp/csimulation.h" // for __contextComponentRNG()

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;
using std::endl;

Register_Class(cPSquare);

cPSquare::cPSquare(const cPSquare& r) : cDensityEstBase(r)
{
    n = nullptr;
    q = nullptr;
    copy(r);
}

cPSquare::cPSquare(const char *name, int cells) : cDensityEstBase(name)
{
    transformed = true;
    delete [] precollectedValues;
    precollectedValues = nullptr;

    numCells = cells;
    numObs = 0;
    n = new int[numCells+2];
    q = new double[numCells+2];

    for (int i = 0; i <= numCells+1; i++) {
        n[i] = i;
        q[i] = -DBL_MAX;
    }
}

cPSquare::~cPSquare()
{
    delete[] q;
    delete[] n;
}

void cPSquare::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimPack(buffer);

    buffer->pack(numCells);
    buffer->pack(numObs);

    if (buffer->packFlag(n != nullptr))
        buffer->pack(n, numCells + 2);
    if (buffer->packFlag(q != nullptr))
        buffer->pack(q, numCells + 2);
#endif
}

void cPSquare::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimUnpack(buffer);

    buffer->unpack(numCells);
    buffer->unpack(numObs);

    if (buffer->checkFlag()) {
        n = new int[numCells + 2];
        buffer->unpack(n, numCells + 2);
    }

    if (buffer->checkFlag()) {
        q = new double[numCells + 2];
        buffer->unpack(q, numCells + 2);
    }
#endif
}

void cPSquare::copy(const cPSquare& res)
{
    numObs = res.numObs;
    numCells = res.numCells;
    delete[] n;
    delete[] q;
    n = new int[numCells+2];
    q = new double[numCells+2];
    for (int i = 0; i <= numCells+1; i++) {
        n[i] = res.n[i];
        q[i] = res.q[i];
    }
}

cPSquare& cPSquare::operator=(const cPSquare& res)
{
    if (this == &res)
        return *this;
    cDensityEstBase::operator=(res);
    copy(res);
    return *this;
}

void cPSquare::raiseError()
{
    throw cRuntimeError(this, "setRange..() and setNumFirstVals() make no sense with cPSquare");
}

void cPSquare::collectTransformed(double val)
{
    numObs++;  // an extra observation is added

    if (numObs <= numCells+1) {
        // old code:
        // q[numcells+2-numobs] = val;
        // places val in front, because qsort puts everything at the end,
        // because initialized with q[i]=-max
        // qsort(q, numcells+2, sizeof(*q), CompDouble);

        // insert val into q[]; q[0] is not used!
        int i;
        for (i = numObs; i >= 2 && val <= q[i-1]; i--)
            q[i] = q[i-1];
        q[i] = val;
    }
    else {
        // now numObs==b+1, number of observations is enough to produce 'b' cells,
        // estimation has to be done

        int k = 0;  // the cell number in which 'val' falls

        for (int i = 1; i <= numCells+1; i++) {
            if (val <= q[i]) {
                if (i == 1) {
                    q[1] = val;
                    k = 1;
                }
                else {
                    k = i-1;
                }
                break;
            }
        }
        if (k == 0) {  // the new value falls outside of the current cells
            q[numCells+1] = val;
            k = numCells;
        }
        for (int i = k+1; i <= numCells+1; i++) {
            n[i] = n[i]+1;
        }

        double d, qd;
        for (int i = 2; i <= numCells; i++) {
            d = 1 + (i-1) * (numObs-1) / (double)numCells - n[i];

            if ((d >= 1 && n[i+1]-n[i] > 1) || (d <= -1 && n[i-1]-n[i] < -1)) {
                // if it is possible to adjust the marker position
                int e = (d < 0) ? -1 : 1;

                // try the parabolic formula
                qd = q[i] + e / ((double)(n[i+1] - n[i-1])) * ((n[i] - n[i-1] + e)
                        * (q[i+1] - q[i]) / ((double)(n[i+1] - n[i])) + (n[i+1]
                                - n[i] - e) * (q[i] - q[i-1]) / ((double)(n[i] - n[i-1])));

                if ((qd > q[i-1]) && (qd < q[i+1]))  // if it is possible to fit the new height
                    q[i] = qd;   // then do so (in between the neighboring heights)
                else   // else use the linear formula
                    q[i] += e * (q[i + e] - q[i]) / ((double)(n[i + e] - n[i]));
                n[i] += e;
            }
        }
    }
}

void cPSquare::collectTransformed2(double value, double weight)
{
    ASSERT(false); // weighted case is unsupported
}

void cPSquare::merge(const cStatistic *other)
{
    throw cRuntimeError(this, "The cPSquare class does not support merge()");
}

void cPSquare::doMergeCellValues(const cDensityEstBase *other)
{
    ASSERT(false);  // never comes here, as merge() already throws an error
}

double cPSquare::draw() const
{
    double s;
    int k = 0, l;
    cRNG *rng = getRNG();

    if (numObs < numCells+1)
        throw cRuntimeError(this, "Must collect at least num_cells values before random() can be used");

    s = numObs * dblrand(rng);

    for (int i = 1; i <= numCells+1; i++) {
        if (s < n[i]) {
            k = i;
            l = k-1;
            break;
        }
    }

    if (k == 1)
        l = k;

    if (numObs < numCells+1) {
        k += numCells-numObs+1;
        l += numCells-numObs+1;
    }

    return dblrand(rng)*(q[k]-q[l])+q[l];
}

int cPSquare::getNumCells() const
{
    if (numObs < 2)
        return 0;
    else if (numObs < numCells)
        return numObs-1;
    else
        return numCells;
}

double cPSquare::getBasepoint(int k) const
{
    return q[k+1];
}

double cPSquare::getCellValue(int k) const
{
    return n[k+2] - n[k+1] + (k == 0);
}

double cPSquare::getCDF(double x) const
{
    // returns 0..1; uses linear approximation between two markers
    for (int i = 1; i < numCells+2; i++)
        if (q[i] > x)
            return ((x-q[i-1]) / (q[i]-q[i-1]) * (n[i]-n[i-1] + (i == 1)) + n[i-1] + (i == 1)) / numObs;

    return 1.0;
}

double cPSquare::getPDF(double x) const
{
    // returns 0..1; assumes constant PDF within a cell
    for (int i = 1; i < numCells+2; i++)
        if (q[i] > x)
            return (n[i]-n[i-1] + (i == 1))/(q[i]-q[i-1])/numObs;

    return 0;
}

void cPSquare::saveToFile(FILE *f) const
{
    cDensityEstBase::saveToFile(f);

    fprintf(f, "%u\t #= numcells\n", numCells);
    fprintf(f, "%ld\t #= numobs\n", numObs);

    int i;
    fprintf(f, "#= n[]\n");
    for (i = 0; i < numCells+2; i++)
        fprintf(f, " %d\n", n[i]);

    fprintf(f, "#= q[]\n");
    for (i = 0; i < numCells+2; i++)
        fprintf(f, " %lg\n", q[i]);
}

void cPSquare::loadFromFile(FILE *f)
{
    cDensityEstBase::loadFromFile(f);

    freadvarsf(f, "%u\t #= numcells", &numCells);
    freadvarsf(f, "%ld\t #= numobs", &numObs);

    int i;
    freadvarsf(f, "#= n[]");
    for (i = 0; i < numCells+2; i++)
        freadvarsf(f, " %d", &n[i]);

    freadvarsf(f, "#= q[]");
    for (i = 0; i < numCells+2; i++)
        freadvarsf(f, " %lg", &q[i]);
}

}  // namespace omnetpp

