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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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

NAMESPACE_BEGIN

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))

Register_Class(cLongHistogram);
Register_Class(cDoubleHistogram);

cHistogramBase::cHistogramBase(const char *name, int numcells) :
    cDensityEstBase(name)
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
        cellv = new unsigned int[numCells];
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
        cellv = new unsigned[numCells];
        memcpy(cellv, res.cellv, numCells * sizeof(unsigned));
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
        cellv[i] += (unsigned int)other->getCellValue(i);  //TODO overflow check
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
        throw cRuntimeError(this, "transform(): histogram already transformed");

    setupRange();  // this will set num_cells if it was unspecified (-1)

    int i;
    cellv = new unsigned[numCells];
    for (i = 0; i < numCells; i++)
        cellv[i] = 0;

    for (i = 0; i < numValues; i++)
        collectTransformed(precollectedValues[i]);

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
            fprintf(f, " %u\n", cellv[i]);
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
        cellv = new unsigned[numCells];
        for (int i = 0; i < numCells; i++)
            freadvarsf(f, " %u", cellv + i);
    }
}

void cHistogramBase::setNumCells(int numcells)
{
    if (cellv)
        throw cRuntimeError(this, "setNumCells(): too late, cells already set up");
    numCells = numcells;
}

//----
// cHistogram - member functions

cHistogram::cHistogram(const char *name, int numcells, HistogramMode mode) :
    cHistogramBase(name, numcells)
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
    // set up the missing ones of: rangemin, rangemax, num_cells, cellsize;
    // throw error if not everything can be set up consistently

    // cellsize is double but we want to calculate with integers here
    long cellsize = (long)this->cellSize;

    // convert range limits to integers
    rangeMin = floor(rangeMin);
    rangeMax = ceil(rangeMax);

    if (rangeMode == RANGE_FIXED) {
#define COMPLAINT    "Cannot set up cells to satisfy constraints"
        long range = (long)(rangeMax - rangeMin);

        if (numCells > 0 && cellsize > 0) {
            if (numCells * cellsize != range)
                throw cRuntimeError(this, COMPLAINT ": numCells*cellSize != rangeMax-rangeMin");
        }
        else if (cellsize > 0) {
            if (range % cellsize != 0)
                throw cRuntimeError(this, COMPLAINT ": specified range is not a multiple of cellSize");
            numCells = range / cellsize;
        }
        else if (numCells > 0) {
            if (range % numCells != 0)
                throw cRuntimeError(this, COMPLAINT ": specified range is not a multiple of numCells");
            cellsize = range / numCells;
        }
        else {
            int mincellsize = (int)ceil(range / 200.0);
            int maxcellsize = (int)ceil(range / 10.0);
            for (cellsize = mincellsize; cellsize <= maxcellsize; cellsize++)
                if (range % cellsize == 0)
                    break;

            if (cellsize > maxcellsize)
                throw cRuntimeError(this, COMPLAINT ": specified range is too large, and cannot divide it to 10..200 equal-sized cells");
            numCells = range / cellsize;
        }
#undef COMPLAINT
    }
    else {
        // non-fixed range
        if (numCells > 0 && cellsize > 0) {
            // both given; num_cells*cellsize will determine the range
        }
        else if (numCells > 0) {
            // num_cells given ==> choose cellsize
            cellsize = (long)ceil((rangeMax - rangeMin) / numCells);
        }
        else if (cellsize > 0) {
            // cellsize given ==> choose num_cells
            numCells = (int)ceil((rangeMax - rangeMin) / cellsize);
        }
        else {
            // neither given, choose both
            double range = rangeMax - rangeMin;
            cellsize = (long)ceil(range / 200.0);  // for range<=200, cellsize==1
            numCells = (int)ceil(range / cellsize);
        }

        // adjust range to be cellsize*num_cells
        double newrange = cellsize * numCells;
        double rangediff = newrange - (rangeMax - rangeMin);

        switch (rangeMode) {
            case RANGE_AUTO:
                rangeMin -= floor(rangediff / 2);
                rangeMax = rangeMin + newrange;
                break;

            case RANGE_AUTOLOWER:
                rangeMin = rangeMax - newrange;
                break;

            case RANGE_AUTOUPPER:
                rangeMax = rangeMin + newrange;
                break;

            case RANGE_FIXED:
                // no-op: rangemin, rangemax already set
                break;

            case RANGE_NOTSET:
                break;  //TODO: throw cRuntimeError(this, "Histogram range mode is unset");
        }
    }

    // write back the integer cellsize into double
    this->cellSize = cellsize;
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
        long m = intrand(rng, numValues - cellUnder - cellOver);

        // select a random cell (k-1) and return a random number from it
        int k;
        for (k = 0; m >= 0; k++)
            m -= cellv[k];

        if (mode == MODE_INTEGERS) {
            // min_vals, max_vals: integer-valued doubles (e.g.: -3.0, 5.0)
            // rangemin, rangemax: doubles like -1.5, 4.5 (integer+0.5)
            // cellsize: integer-valued double, >0
            return ceil(rangeMin) + (k-1) * (long)cellSize + intrand(rng, (long)cellSize);
        }
        else {
            // return an uniform double from the given cell
            return rangeMin + (k-1) * cellSize + dblrand(rng) * cellSize;
        }
    }
}

void cHistogram::collectTransformed(double val)
{
    int k = (int)floor((val - rangeMin) / cellSize);
    if (k < 0 || val < rangeMin)
        cellUnder++;
    else if (k >= numCells || val >= rangeMax)
        cellOver++;
    else
        cellv[k]++;
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

// return kth basepoint
double cHistogram::getBasepoint(int k) const
{
    //   k=0           : rangemin
    //   k=1,2,...     : rangemin + k*cellsize
    //   k=num_cells   : rangemax

    if (k < 0 || k > numCells)
        throw cRuntimeError(this, "invalid basepoint index %u", k);

    if (k == numCells)
        return rangeMax;
    else
        return rangeMin + k * cellSize;
}

double cHistogram::getCellValue(int k) const
{
    if (k < 0 || k > numCells)
        throw cRuntimeError(this, "invalid cell index %u", k);
    return cellv[k];
}

void cHistogram::saveToFile(FILE *f) const
{
    cHistogramBase::saveToFile(f);
    fprintf(f, "%g\t #= cellsize\n", cellSize);
}

void cHistogram::loadFromFile(FILE *f)
{
    cHistogramBase::loadFromFile(f);
    freadvarsf(f, "%g\t #= cellsize", &cellSize);
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

NAMESPACE_END

