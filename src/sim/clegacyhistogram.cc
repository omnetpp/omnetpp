//=========================================================================
//  CLEGACYHISTOGRAM.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Authors: Andras Varga, Gabor Lencse
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "omnetpp/distrib.h"
#include "omnetpp/globals.h"
#include "omnetpp/clegacyhistogram.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"  // __contextComponentRNG()

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace omnetpp {

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))

Register_Class(cLongHistogram);
Register_Class(cDoubleHistogram);

cLegacyHistogramBase::cLegacyHistogramBase(const char *name, int numcells, bool weighted) :
    cPrecollectionBasedDensityEst(name, weighted)
{
    cellv = nullptr;
    numCells = numcells;
}

cLegacyHistogramBase::~cLegacyHistogramBase()
{
    delete[] cellv;
}

void cLegacyHistogramBase::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cPrecollectionBasedDensityEst::parsimPack(buffer);
    buffer->pack(numCells);

    if (buffer->packFlag(cellv != nullptr))
        buffer->pack(cellv, numCells);
#endif
}

void cLegacyHistogramBase::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cPrecollectionBasedDensityEst::parsimUnpack(buffer);
    buffer->pack(numCells);

    if (buffer->checkFlag()) {
        cellv = new double[numCells];
        buffer->unpack(cellv, numCells);
    }
#endif
}

void cLegacyHistogramBase::copy(const cLegacyHistogramBase& res)
{
    numCells = res.numCells;
    delete[] cellv;
    cellv = nullptr;
    if (res.cellv) {
        cellv = new double[numCells];
        memcpy(cellv, res.cellv, numCells * sizeof(double));
    }
}

cLegacyHistogramBase& cLegacyHistogramBase::operator=(const cLegacyHistogramBase& res)
{
    cPrecollectionBasedDensityEst::operator=(res);
    copy(res);
    return *this;
}

void cLegacyHistogramBase::doMergeBinValues(const cPrecollectionBasedDensityEst *other)
{
    for (int i = 0; i < numCells; i++)
        cellv[i] += other->getBinValue(i);
}

void cLegacyHistogramBase::clear()
{
    cPrecollectionBasedDensityEst::clear();

    delete[] cellv;
    cellv = nullptr;
}

void cLegacyHistogramBase::setUpBins()
{
    if (binsAlreadySetUp())
        throw cRuntimeError(this, "setUpBins(): Histogram bins already set up");

    setupRange();  // this will set num_cells if it was unspecified (-1)

    cellv = new double[numCells];
    for (int i = 0; i < numCells; i++)
        cellv[i] = 0;

    for (int i = 0; i < numValues; i++)
        if (!weighted)
            collectIntoHistogram(precollectedValues[i]);
        else
            collectWeightedIntoHistogram(precollectedValues[i], precollectedWeights[i]);

    delete[] precollectedValues;
    precollectedValues = nullptr;

    transformed = true;
}

int cLegacyHistogramBase::getNumBins() const
{
    if (!binsAlreadySetUp())
        return 0;
    return numCells;
}

void cLegacyHistogramBase::saveToFile(FILE *f) const
{
    cPrecollectionBasedDensityEst::saveToFile(f);
    fprintf(f, "%d\t #= num_cells\n", numCells);
    fprintf(f, "%d\t #= cellv[] exists\n", cellv != nullptr);
    if (cellv)
        for (int i = 0; i < numCells; i++)
            fprintf(f, " %g\n", cellv[i]);
}

void cLegacyHistogramBase::loadFromFile(FILE *f)
{
    cPrecollectionBasedDensityEst::loadFromFile(f);
    freadvarsf(f, "%d\t #= num_cells", &numCells);

    int cellv_exists;
    freadvarsf(f, "%d\t #= cellv[] exists", &cellv_exists);
    delete[] cellv;
    cellv = nullptr;
    if (cellv_exists) {
        cellv = new double[numCells];
        for (int i = 0; i < numCells; i++)
            freadvarsf(f, " %lg", cellv + i);
    }
}

void cLegacyHistogramBase::setNumCells(int numcells)
{
    if (cellv)
        throw cRuntimeError(this, "setNumCells(): Too late, bins already set up");
    numCells = numcells;
}

//----

cLegacyHistogram::cLegacyHistogram(const char *name, int numcells, Mode mode, bool weighted) :
    cLegacyHistogramBase(name, numcells, weighted)
{
    cellSize = 0;
    this->mode = mode;
}

void cLegacyHistogram::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cLegacyHistogramBase::parsimPack(buffer);
    buffer->pack(cellSize);
#endif
}

void cLegacyHistogram::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cLegacyHistogramBase::parsimUnpack(buffer);
    buffer->unpack(cellSize);
#endif
}

void cLegacyHistogram::copy(const cLegacyHistogram& res)
{
    cellSize = res.cellSize;
    mode = res.mode;
}

cLegacyHistogram& cLegacyHistogram::operator=(const cLegacyHistogram& res)
{
    if (this == &res)
        return *this;
    cLegacyHistogramBase::operator=(res);
    copy(res);
    return *this;
}

void cLegacyHistogram::setMode(Mode mode)
{
    if (binsAlreadySetUp())
        throw cRuntimeError(this, "setMode() cannot be called when bins have been set up already");
    this->mode = mode;
}

void cLegacyHistogram::setCellSize(double d)
{
    if (binsAlreadySetUp())
        throw cRuntimeError(this, "setCellSize() cannot be called when bins have been set up already");
    cellSize = d;
}

void cLegacyHistogram::getAttributesToRecord(opp_string_map& attributes)
{
    cLegacyHistogramBase::getAttributesToRecord(attributes);

    if (mode == MODE_INTEGERS)
        attributes["type"] = "int";
}

void cLegacyHistogram::setupRange()
{
    cLegacyHistogramBase::setupRange();

    // the following code sets num_cells, and cellsize as (rangemax - rangemin) / num_cells

    if (mode == MODE_AUTO) {
        // if all precollected numbers are integers (and they are not all zeroes), go for integer mode
        bool allzeroes = true;
        bool allints = true;
        for (int i = 0; i < numValues; i++) {
            if (precollectedValues[i] != 0)
                allzeroes = false;
            if (precollectedValues[i] != floor(precollectedValues[i]))
                allints = false;
        }

        mode = (numValues > 0 && allints && !allzeroes) ? MODE_INTEGERS : MODE_DOUBLES;
    }

    if (mode == MODE_INTEGERS)
        setupRangeInteger();
    else
        setupRangeDouble();
}

void cLegacyHistogram::setupRangeInteger()
{
    // set up the missing ones of: rangeMin, rangeMax, numCells, cellSize;
    // throw error if not everything can be set up consistently

    // cellsize is double but we want to calculate with integers here
    long cellSize = (long)this->cellSize;

    // convert range limits to integers
    rangeMin = floor(rangeMin);
    rangeMax = ceil(rangeMax);

    if (rangeMode == RANGE_FIXED) {
#define COMPLAINT    "Cannot set up bins to satisfy constraints"
        long range = (long)(rangeMax - rangeMin);

        if (numCells > 0 && cellSize > 0) {
            if (numCells * cellSize != range)
                throw cRuntimeError(this, COMPLAINT ": numCells*cellSize != rangeMax-rangeMin");
        }
        else if (cellSize > 0) {
            if (range % cellSize != 0)
                throw cRuntimeError(this, COMPLAINT ": specified range is not a multiple of cellSize");
            numCells = range / cellSize;
        }
        else if (numCells > 0) {
            if (range % numCells != 0)
                throw cRuntimeError(this, COMPLAINT ": specified range is not a multiple of numCells");
            cellSize = range / numCells;
        }
        else {
            int minCellsize = (int)ceil(range / 200.0);
            int maxCellsize = (int)ceil(range / 10.0);
            for (cellSize = minCellsize; cellSize <= maxCellsize; cellSize++)
                if (range % cellSize == 0)
                    break;

            if (cellSize > maxCellsize)
                throw cRuntimeError(this, COMPLAINT ": Specified range is too large, and cannot divide it to 10..200 equal-sized bins");
            numCells = range / cellSize;
        }
#undef COMPLAINT
    }
    else {
        // non-fixed range
        if (numCells > 0 && cellSize > 0) {
            // both given; numCells*cellSize will determine the range
        }
        else if (numCells > 0) {
            // numCells given ==> choose cellSize
            cellSize = (long)ceil((rangeMax - rangeMin) / numCells);
        }
        else if (cellSize > 0) {
            // cellSize given ==> choose numCells
            numCells = (int)ceil((rangeMax - rangeMin) / cellSize);
        }
        else {
            // neither given, choose both
            double range = rangeMax - rangeMin;
            cellSize = (long)ceil(range / 200.0);  // for range<=200, cellSize==1
            numCells = (int)ceil(range / cellSize);
        }

        // adjust range to be cellSize*numCells
        double newRange = cellSize * numCells;
        double rangeDiff = newRange - (rangeMax - rangeMin);

        switch (rangeMode) {
            case RANGE_AUTO:
                rangeMin -= floor(rangeDiff / 2);
                rangeMax = rangeMin + newRange;
                break;

            case RANGE_AUTOLOWER:
                rangeMin = rangeMax - newRange;
                break;

            case RANGE_AUTOUPPER:
                rangeMax = rangeMin + newRange;
                break;

            case RANGE_FIXED:
                // no-op: rangemin, rangemax already set
                break;

            case RANGE_NOTSET:
                break;  //TODO: throw cRuntimeError(this, "Histogram range mode is unset");
        }
    }

    // write back the integer cellSize into double
    this->cellSize = cellSize;
}

void cLegacyHistogram::setupRangeDouble()
{
    if (numCells == -1)
        numCells = 30;  // to allow merging every 2, 3, 5, 6 adjacent bins in post-processing
    cellSize = (rangeMax - rangeMin) / numCells;
}

double cLegacyHistogram::draw() const
{
    cRNG *rng = getRNG();
    if (numValues == 0) {
        return 0L;
    }
    else if (numValues < numPrecollected) {
        // randomly select a sample from the stored ones
        return precollectedValues[intrand(rng, numValues)];
    }
    else {
        long m = intrand(rng, numValues - numUnderflows - numOverflows);

        // select a random bin (k-1) and return a random number from it
        int k;
        for (k = 0; m >= 0; k++)
            m -= cellv[k];

        if (mode == MODE_INTEGERS) {
            // minValues, maxValues: integer-valued doubles (e.g.: -3.0, 5.0)
            // rangeMin, rangeMax: doubles like -1.5, 4.5 (integer+0.5)
            // cellSize: integer-valued double, >0
            return ceil(rangeMin) + (k-1) * (long)cellSize + intrand(rng, (long)cellSize);
        }
        else {
            // return an uniform double from the given bin
            return rangeMin + (k-1) * cellSize + dblrand(rng) * cellSize;
        }
    }
}

void cLegacyHistogram::collectIntoHistogram(double value)
{
    collectWeightedIntoHistogram(value, 1.0);
}

void cLegacyHistogram::collectWeightedIntoHistogram(double value, double weight)
{
    int k = (int)floor((value - rangeMin) / cellSize);
    if (k < 0 || value < rangeMin) {
        numUnderflows++;
        underflowSumWeights += weight;
    }
    else if (k >= numCells || value >= rangeMax) {
        numOverflows++;
        overflowSumWeights += weight;
    }
    else
        cellv[k] += weight;
}

double cLegacyHistogram::getPDF(double x) const
{
    if (!binsAlreadySetUp())
        throw cRuntimeError(this, "getPDF(x) cannot be called before histogram bins have been set up");

    int k = (int)floor((x - rangeMin) / cellSize);
    if (k < 0 || x < rangeMin || k >= numCells || x >= rangeMax)
        return 0.0;

    return cellv[k] / cellSize / numValues;
}

double cLegacyHistogram::getCDF(double) const
{
    throw cRuntimeError(this, "getCDF() not implemented");
}

double cLegacyHistogram::getBinEdge(int k) const
{
    //   k=0           : rangemin
    //   k=1,2,...     : rangemin + k*cellsize
    //   k=num_cells   : rangemax

    if (k < 0 || k > numCells)
        throw cRuntimeError(this, "Invalid bin edge index %u", k);

    if (k == numCells)
        return rangeMax;
    else
        return rangeMin + k * cellSize;
}

double cLegacyHistogram::getBinValue(int k) const
{
    if (k < 0 || k > numCells)
        throw cRuntimeError(this, "Invalid bin index %u", k);
    return cellv[k];
}

void cLegacyHistogram::saveToFile(FILE *f) const
{
    cLegacyHistogramBase::saveToFile(f);
    fprintf(f, "%lg\t #= cellsize\n", cellSize);
}

void cLegacyHistogram::loadFromFile(FILE *f)
{
    cLegacyHistogramBase::loadFromFile(f);
    freadvarsf(f, "%lg\t #= cellsize", &cellSize);
}

cLongHistogram& cLongHistogram::operator=(const cLongHistogram& res)
{
    if (this == &res)
        return *this;
    cLegacyHistogram::operator=(res);
    copy(res);
    return *this;
}

cDoubleHistogram& cDoubleHistogram::operator=(const cDoubleHistogram& res)
{
    if (this == &res)
        return *this;
    cLegacyHistogram::operator=(res);
    copy(res);
    return *this;
}

}  // namespace omnetpp

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
