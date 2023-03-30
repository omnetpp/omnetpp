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

cPSquare::cPSquare(const cPSquare& r) : cAbstractHistogram(r)
{
    copy(r);
}

cPSquare::cPSquare(const char *name, int bins) : cAbstractHistogram(name), numBins(bins)
{
    n = new int[numBins+2];
    q = new double[numBins+2];
    for (int i = 0; i <= numBins+1; i++) {
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
    cAbstractHistogram::parsimPack(buffer);

    buffer->pack(numBins);
    buffer->pack(numObs);
    buffer->pack(numNegInfs);
    buffer->pack(numPosInfs);

    if (buffer->packFlag(n != nullptr))
        buffer->pack(n, numBins + 2);
    if (buffer->packFlag(q != nullptr))
        buffer->pack(q, numBins + 2);
#endif
}

void cPSquare::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cAbstractHistogram::parsimUnpack(buffer);

    buffer->unpack(numBins);
    buffer->unpack(numObs);
    buffer->unpack(numNegInfs);
    buffer->unpack(numPosInfs);

    if (buffer->checkFlag()) {
        n = new int[numBins + 2];
        buffer->unpack(n, numBins + 2);
    }

    if (buffer->checkFlag()) {
        q = new double[numBins + 2];
        buffer->unpack(q, numBins + 2);
    }
#endif
}

void cPSquare::copy(const cPSquare& res)
{
    numObs = res.numObs;
    numBins = res.numBins;
    numNegInfs = res.numNegInfs;
    numPosInfs = res.numPosInfs;
    delete[] n;
    delete[] q;
    n = new int[numBins+2];
    q = new double[numBins+2];
    for (int i = 0; i <= numBins+1; i++) {
        n[i] = res.n[i];
        q[i] = res.q[i];
    }
}

cPSquare& cPSquare::operator=(const cPSquare& res)
{
    if (this == &res)
        return *this;
    cAbstractHistogram::operator=(res);
    copy(res);
    return *this;
}

void cPSquare::collect(double val)
{
    cAbstractHistogram::collect(val);

    if (std::isinf(val)) {
        if (val < 0)
            numNegInfs++;
        else
            numPosInfs++;

        return;
    }

    numObs++;  // an extra observation is added

    if (numObs <= numBins+1) {
        // old code:
        // q[numBins+2-numObs] = val;
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
        // now numObs==b+1, number of observations is enough to produce 'b' bins,
        // estimation has to be done

        int k = 0;  // the bin number in which 'val' falls

        for (int i = 1; i <= numBins+1; i++) {
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
        if (k == 0) {  // the new value falls outside of the current bins
            q[numBins+1] = val;
            k = numBins;
        }
        for (int i = k+1; i <= numBins+1; i++) {
            n[i] = n[i]+1;
        }

        double d, qd;
        for (int i = 2; i <= numBins; i++) {
            d = 1 + (i-1) * (numObs-1) / (double)numBins - n[i];

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


void cPSquare::collectWeighted(double value, double weight)
{
    throw cRuntimeError(this, "cPSquare does not support weighted statistics");
}

void cPSquare::merge(const cStatistic *other)
{
    throw cRuntimeError(this, "cPSquare does not support merge()");
}

void cPSquare::clear()
{
    numObs = 0;
    numNegInfs = numPosInfs = 0;

    for (int i = 0; i <= numBins+1; i++) {
        n[i] = i;
        q[i] = -DBL_MAX;
    }
}

double cPSquare::draw() const
{
    double s;
    int k = 0, l = 0;
    cRNG *rng = getRNG();

    if (numObs < numBins+1)
        throw cRuntimeError(this, "Must collect at least as many values as bins before calling random()");

    s = numObs * dblrand(rng);

    for (int i = 1; i <= numBins+1; i++) {
        if (s < n[i]) {
            k = i;
            l = k-1;
            break;
        }
    }

    if (k == 1)
        l = k;

    if (numObs < numBins+1) {
        k += numBins-numObs+1;
        l += numBins-numObs+1;
    }

    return dblrand(rng)*(q[k]-q[l])+q[l];
}

int cPSquare::getNumBins() const
{
    if (numObs < 2)
        return 0;
    else if (numObs < numBins)
        return numObs-1;
    else
        return numBins;
}

double cPSquare::getBinEdge(int k) const
{
    if (k == 0)
        const_cast<cPSquare*>(this)->ensureStrictlyIncreasingEdges();

    return q[k+1];
}

double cPSquare::getBinValue(int k) const
{
    return n[k+2] - n[k+1] + (k == 0);
}

inline double nextAfter(double x)
{
    return std::nextafter(x, x+1 /*up*/);
}

void cPSquare::ensureStrictlyIncreasingEdges()
{
    // P^2 sometimes creates equal bin edges (e.g. when the distribution is a single constant value)
    // but the getBinEdge() contract promises strictly increasing edges. Tweak bin edges a little to make
    // them strictly increasing.
    for (int i = 1; i <= getNumBins(); i++)
        if (q[i+1] <= q[i])
            q[i+1] = nextAfter(q[i]);
}

void cPSquare::saveToFile(FILE *f) const
{
    cAbstractHistogram::saveToFile(f);

    fprintf(f, "%u\t #= numbins\n", numBins);
    fprintf(f, "%ld\t #= numneginfs\n", numNegInfs);
    fprintf(f, "%ld\t #= numposinfs\n", numPosInfs);
    fprintf(f, "%ld\t #= numobs\n", numObs);

    int i;
    fprintf(f, "#= n[]\n");
    for (i = 0; i < numBins+2; i++)
        fprintf(f, " %d\n", n[i]);

    fprintf(f, "#= q[]\n");
    for (i = 0; i < numBins+2; i++)
        fprintf(f, " %lg\n", q[i]);
}

void cPSquare::loadFromFile(FILE *f)
{
    cAbstractHistogram::loadFromFile(f);

    freadvarsf(f, "%u\t #= numbins", &numBins);
    freadvarsf(f, "%ld\t #= numneginfs", &numNegInfs);
    freadvarsf(f, "%ld\t #= numposinfs", &numPosInfs);
    freadvarsf(f, "%ld\t #= numobs", &numObs);

    int i;
    freadvarsf(f, "#= n[]");
    for (i = 0; i < numBins+2; i++)
        freadvarsf(f, " %d", &n[i]);

    freadvarsf(f, "#= q[]");
    for (i = 0; i < numBins+2; i++)
        freadvarsf(f, " %lg", &q[i]);
}

}  // namespace omnetpp

