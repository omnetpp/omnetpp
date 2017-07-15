//=========================================================================
//  CHISTOGRAM.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cHistogramBase    : common base class for histogram classes
//    cHistogram        : equi-distant histogram
//    cLongHistogram    : long integer histogram
//    cDoubleHistogram  : double histogram
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
#include "omnetpp/cdetect.h"
#include "omnetpp/chistogram.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"  // __contextComponentRNG()

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))

Register_Class(cLongHistogram);
Register_Class(cDoubleHistogram);

cHistogramBase::cHistogramBase(const char *name, int numcells, bool weighted) :
    cDensityEstBase(name, weighted)
{
    cellv = nullptr;
    numCells = numcells;
}

cHistogramBase::~cHistogramBase()
{
    delete[] cellv;
}

void cHistogramBase::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimPack(buffer);
    buffer->pack(numCells);

    if (buffer->packFlag(cellv != nullptr))
        buffer->pack(cellv, numCells);
#endif
}

void cHistogramBase::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimUnpack(buffer);
    buffer->pack(numCells);

    if (buffer->checkFlag()) {
        cellv = new double[numCells];
        buffer->unpack(cellv, numCells);
    }
#endif
}

void cHistogramBase::copy(const cHistogramBase& res)
{
    numCells = res.numCells;
    delete[] cellv;
    cellv = nullptr;
    if (res.cellv) {
        cellv = new double[numCells];
        memcpy(cellv, res.cellv, numCells * sizeof(double));
    }
}

cHistogramBase& cHistogramBase::operator=(const cHistogramBase& res)
{
    cDensityEstBase::operator=(res);
    copy(res);
    return *this;
}

void cHistogramBase::doMergeCellValues(const cDensityEstBase *other)
{
    for (int i = 0; i < numCells; i++)
        cellv[i] += other->getCellValue(i);
}

void cHistogramBase::clearResult()
{
    cDensityEstBase::clearResult();

    delete[] cellv;
    cellv = nullptr;
}

void cHistogramBase::transform()
{
    if (isTransformed())
        throw cRuntimeError(this, "transform(): Histogram already transformed");

    setupRange();  // this will set num_cells if it was unspecified (-1)

    cellv = new double[numCells];
    for (int i = 0; i < numCells; i++)
        cellv[i] = 0;

    for (int i = 0; i < numValues; i++)
        if (!weighted)
            collectTransformed(precollectedValues[i]);
        else
            collectTransformed2(precollectedValues[i], precollectedWeights[i]);

    delete[] precollectedValues;
    precollectedValues = nullptr;

    transformed = true;
}

int cHistogramBase::getNumCells() const
{
    if (!isTransformed())
        return 0;
    return numCells;
}

void cHistogramBase::saveToFile(FILE *f) const
{
    cDensityEstBase::saveToFile(f);
    fprintf(f, "%d\t #= num_cells\n", numCells);
    fprintf(f, "%d\t #= cellv[] exists\n", cellv != nullptr);
    if (cellv)
        for (int i = 0; i < numCells; i++)
            fprintf(f, " %g\n", cellv[i]);
}

void cHistogramBase::loadFromFile(FILE *f)
{
    cDensityEstBase::loadFromFile(f);
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

void cHistogramBase::setNumCells(int numcells)
{
    if (cellv)
        throw cRuntimeError(this, "setNumCells(): Too late, cells already set up");
    numCells = numcells;
}

//----

cHistogram::cHistogram(const char *name, int numcells, HistogramMode mode, bool weighted) :
    cHistogramBase(name, numcells, weighted)
{
    cellSize = 0;
    this->mode = mode;
}

void cHistogram::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cHistogramBase::parsimPack(buffer);
    buffer->pack(cellSize);
#endif
}

void cHistogram::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cHistogramBase::parsimUnpack(buffer);
    buffer->unpack(cellSize);
#endif
}

void cHistogram::copy(const cHistogram& res)
{
    cellSize = res.cellSize;
    mode = res.mode;
}

cHistogram& cHistogram::operator=(const cHistogram& res)
{
    if (this == &res)
        return *this;
    cHistogramBase::operator=(res);
    copy(res);
    return *this;
}

void cHistogram::setMode(HistogramMode mode)
{
    if (isTransformed())
        throw cRuntimeError(this, "setMode() cannot be called when cells have been set up already");
    this->mode = mode;
}

void cHistogram::setCellSize(double d)
{
    if (isTransformed())
        throw cRuntimeError(this, "setCellSize() cannot be called when cells have been set up already");
    cellSize = d;
}

void cHistogram::getAttributesToRecord(opp_string_map& attributes)
{
    cHistogramBase::getAttributesToRecord(attributes);

    if (mode == MODE_INTEGERS)
        attributes["type"] = "int";
}

void cHistogram::setupRange()
{
    cHistogramBase::setupRange();

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

void cHistogram::setupRangeInteger()
{
    // set up the missing ones of: rangeMin, rangeMax, numCells, cellSize;
    // throw error if not everything can be set up consistently

    // cellsize is double but we want to calculate with integers here
    long cellSize = (long)this->cellSize;

    // convert range limits to integers
    rangeMin = floor(rangeMin);
    rangeMax = ceil(rangeMax);

    if (rangeMode == RANGE_FIXED) {
#define COMPLAINT    "Cannot set up cells to satisfy constraints"
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
                throw cRuntimeError(this, COMPLAINT ": Specified range is too large, and cannot divide it to 10..200 equal-sized cells");
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

void cHistogram::setupRangeDouble()
{
    if (numCells == -1)
        numCells = 30;  // to allow merging every 2, 3, 5, 6 adjacent cells in post-processing
    cellSize = (rangeMax - rangeMin) / numCells;
}

double cHistogram::draw() const
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

        // select a random cell (k-1) and return a random number from it
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
            // return an uniform double from the given cell
            return rangeMin + (k-1) * cellSize + dblrand(rng) * cellSize;
        }
    }
}

void cHistogram::collectTransformed(double value)
{
    collectTransformed2(value, 1.0);
}

void cHistogram::collectTransformed2(double value, double weight)
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

double cHistogram::getPDF(double x) const
{
    if (!isTransformed())
        throw cRuntimeError(this, "getPDF(x) cannot be called before histogram is transformed");

    int k = (int)floor((x - rangeMin) / cellSize);
    if (k < 0 || x < rangeMin || k >= numCells || x >= rangeMax)
        return 0.0;

    return cellv[k] / cellSize / numValues;
}

double cHistogram::getCDF(double) const
{
    throw cRuntimeError(this, "getCDF() not implemented");
}

double cHistogram::getBasepoint(int k) const
{
    //   k=0           : rangemin
    //   k=1,2,...     : rangemin + k*cellsize
    //   k=num_cells   : rangemax

    if (k < 0 || k > numCells)
        throw cRuntimeError(this, "Invalid basepoint index %u", k);

    if (k == numCells)
        return rangeMax;
    else
        return rangeMin + k * cellSize;
}

double cHistogram::getCellValue(int k) const
{
    if (k < 0 || k > numCells)
        throw cRuntimeError(this, "Invalid cell index %u", k);
    return cellv[k];
}

void cHistogram::saveToFile(FILE *f) const
{
    cHistogramBase::saveToFile(f);
    fprintf(f, "%lg\t #= cellsize\n", cellSize);
}

void cHistogram::loadFromFile(FILE *f)
{
    cHistogramBase::loadFromFile(f);
    freadvarsf(f, "%lg\t #= cellsize", &cellSize);
}

cLongHistogram& cLongHistogram::operator=(const cLongHistogram& res)
{
    if (this == &res)
        return *this;
    cHistogram::operator=(res);
    copy(res);
    return *this;
}

cDoubleHistogram& cDoubleHistogram::operator=(const cDoubleHistogram& res)
{
    if (this == &res)
        return *this;
    cHistogram::operator=(res);
    copy(res);
    return *this;
}

}  // namespace omnetpp

