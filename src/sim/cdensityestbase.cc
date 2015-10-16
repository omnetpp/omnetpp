//=========================================================================
//  CDENSITY.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cDensityEstBase : common base class for density estimation classes
//
//   Authors: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cmath>
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cdensityestbase.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

cDensityEstBase::cDensityEstBase(const char *name) : cStdDev(name)
{
    rangeMode = RANGE_AUTO;
    numPrecollected = 100;
    rangeExtFactor = 2.0;
    rangeMin = rangeMax = 0;
    cellUnder = cellOver = 0;
    transformed = false;
    precollectedValues = new double[numPrecollected];  // to match RANGE_AUTO
}

cDensityEstBase::~cDensityEstBase()
{
    delete[] precollectedValues;
}

void cDensityEstBase::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStdDev::parsimPack(buffer);

    buffer->pack(rangeMin);
    buffer->pack(rangeMax);
    buffer->pack(numPrecollected);
    buffer->pack(cellUnder);
    buffer->pack(cellOver);
    buffer->pack(rangeExtFactor);
    buffer->pack(rangeMode);
    buffer->pack(transformed);

    if (buffer->packFlag(precollectedValues != nullptr))
        buffer->pack(precollectedValues, numValues);  // pack the used positions only
#endif
}

void cDensityEstBase::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStdDev::parsimUnpack(buffer);

    buffer->unpack(rangeMin);
    buffer->unpack(rangeMax);
    buffer->unpack(numPrecollected);
    buffer->unpack(cellUnder);
    buffer->unpack(cellOver);
    buffer->unpack(rangeExtFactor);
    int tmp;
    buffer->unpack(tmp);
    rangeMode = (RangeMode)tmp;
    buffer->unpack(transformed);

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (buffer->checkFlag()) {
        precollectedValues = new double[numPrecollected];
        buffer->unpack(precollectedValues, numValues);
    }
#endif
}

void cDensityEstBase::copy(const cDensityEstBase& res)
{
    rangeMin = res.rangeMin;
    rangeMax = res.rangeMax;
    numPrecollected = res.numPrecollected;
    cellUnder = res.cellUnder;
    cellOver = res.cellOver;

    rangeExtFactor = res.rangeExtFactor;
    rangeMode = res.rangeMode;

    transformed = res.transformed;

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (res.precollectedValues) {
        precollectedValues = new double[numPrecollected];
        memcpy(precollectedValues, res.precollectedValues, numPrecollected * sizeof(double));
    }
}

cDensityEstBase& cDensityEstBase::operator=(const cDensityEstBase& res)
{
    cStdDev::operator=(res);
    copy(res);
    return *this;
}

void cDensityEstBase::merge(const cStatistic *other)
{
    if (dynamic_cast<const cDensityEstBase *>(other) == nullptr)
        throw cRuntimeError(this, "Cannot merge non-histogram (non-cDensityEstBase) statistics (%s)%s into a histogram type",
                other->getClassName(), other->getFullPath().c_str());

    const cDensityEstBase *otherd = (const cDensityEstBase *)other;

    if (!otherd->isTransformed()) {
        // easiest and exact solution: simply recollect the observations
        // the other object has collected
        for (int i = 0; i < otherd->numValues; i++)
            collect(precollectedValues[i]);
    }
    else {
        // merge the base class
        cStdDev::merge(otherd);

        // force this object to be transformed as well
        if (!isTransformed())
            transform();

        // make sure that cells are aligned
        if (getNumCells() != otherd->getNumCells())
            throw cRuntimeError(this, "Cannot merge (%s)%s: different number of histogram cells (%d vs %d)",
                    otherd->getClassName(), otherd->getFullPath().c_str(), getNumCells(), otherd->getNumCells());
        int n = getNumCells();
        for (int i = 0; i <= n; i++)
            if (getBasepoint(i) != otherd->getBasepoint(i))
                throw cRuntimeError(this, "Cannot merge (%s)%s: histogram cells are not aligned",
                        otherd->getClassName(), otherd->getFullPath().c_str());


        // merge underflow/overflow cells
        cellUnder += otherd->getUnderflowCell();  //FIXME check overflow!! but this is unsigned long....
        cellOver += otherd->getOverflowCell();

        // then merge cell counters
        doMergeCellValues(otherd);
    }
}

void cDensityEstBase::clearResult()
{
    cStdDev::clearResult();

    transformed = false;
    rangeMode = RANGE_AUTO;
    numPrecollected = 100;
    rangeExtFactor = 2.0;
    rangeMin = rangeMax = 0;
    cellUnder = cellOver = 0;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];  // to match RANGE_AUTO
}

void cDensityEstBase::setRange(double lower, double upper)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange() can only be called before collecting any values");

    rangeMode = RANGE_FIXED;
    rangeMin = lower;
    rangeMax = upper;

    delete[] precollectedValues;
    precollectedValues = nullptr;  // not needed with RANGE_FIXED
}

void cDensityEstBase::setRangeAuto(int num_fstvals, double range_ext_fct)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    rangeMode = RANGE_AUTO;
    numPrecollected = num_fstvals;
    rangeExtFactor = range_ext_fct;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];
}

void cDensityEstBase::setRangeAutoLower(double upper, int numPrecoll, double rangeExtFact)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    rangeMode = RANGE_AUTOLOWER;
    numPrecollected = numPrecoll;
    rangeMax = upper;
    rangeExtFactor = rangeExtFact;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];
}

void cDensityEstBase::setRangeAutoUpper(double lower, int numPrecoll, double rangeExtFact)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    rangeMode = RANGE_AUTOUPPER;
    numPrecollected = numPrecoll;
    rangeMin = lower;
    rangeExtFactor = rangeExtFact;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];
}

void cDensityEstBase::setNumPrecollectedValues(int numPrecoll)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setNumPrecollectedValues() can only be called before collecting any values");

    numPrecollected = numPrecoll;

    // if already allocated, reallocate with the correct size
    if (precollectedValues) {
        delete[] precollectedValues;
        precollectedValues = new double[numPrecollected];
    }
}

void cDensityEstBase::setupRange()
{
    //
    // set rangemin and rangemax.
    //   Attempts not to make zero width range (makes it 1.0 wide).
    //
    double c, r;
    switch (rangeMode) {
        case RANGE_AUTO:
            c = (minValue + maxValue) / 2;
            r = (maxValue - minValue) * rangeExtFactor;
            if (r == 0)
                r = 1.0;  // warning?
            rangeMin = c - r / 2;
            rangeMax = c + r / 2;
            break;

        case RANGE_AUTOLOWER:
            if (rangeMax <= minValue)
                rangeMin = rangeMax - 1.0;  // warning?
            else
                rangeMin = rangeMax - (rangeMax - minValue) * rangeExtFactor;
            break;

        case RANGE_AUTOUPPER:
            if (rangeMin >= maxValue)
                rangeMax = rangeMin + 1.0;  // warning?
            else
                rangeMax = rangeMin + (maxValue - rangeMin) * rangeExtFactor;
            break;

        case RANGE_FIXED:
            // no-op: rangemin, rangemax already set
            break;

        case RANGE_NOTSET:
            break;  //TODO: throw cRuntimeError(this, "Histogram range mode is unset");
    }
}

void cDensityEstBase::collect(double val)
{
    if (!isTransformed() && rangeMode == RANGE_FIXED)
        transform();

    cStdDev::collect(val);  // this also increments num_vals

    if (!isTransformed()) {
        ASSERT(precollectedValues);
        precollectedValues[numValues-1] = val;

        if (numValues == numPrecollected)
            transform();
    }
    else {
        collectTransformed(val);  // must maintain underflow/overflow cells
    }
}

double cDensityEstBase::getCellPDF(int k) const
{
    if (numValues == 0)
        return 0.0;
    double cellSize = getBasepoint(k+1) - getBasepoint(k);
    return cellSize == 0 ? 0.0 : getCellValue(k) / cellSize / getCount();
}

void cDensityEstBase::plotline(ostream& os, const char *pref, double xval, double count, double a)
{
    const int pictureWidth = 54;
    char buf[101];
    sprintf(buf, "   %s%12f %5g :", pref, xval, count);
    char *s = buf + strlen(buf);
    int x = (int)floor(a * count + .5);
    int k = x <= pictureWidth ? x : pictureWidth;
    for (int m = 1; m <= k; m++)
        *s++ = '-';
    strcpy(s, x <= pictureWidth ? "*\n" : ">\n");
    os << buf;
}

std::string cDensityEstBase::detailedInfo() const
{
    std::stringstream os;
    os << cStdDev::detailedInfo();

    if (!isTransformed()) {
        ASSERT(precollectedValues || numValues == 0);
        os << "Values collected so far: ";
        for (int i = 0; i < numValues; i++)
            os << (i == 0 ? "" : ", ") << precollectedValues[i];
        os << "\n";
    }
    else {
        // transformed
        const int pictureWidth = 55;
        double max = 0;  // biggest cell value
        int nc = getNumCells();
        int k;
        double d;
        for (k = 0; k < nc; k++)
            if ((d = getCellValue(k)) > max)
                max = d;

        double a = (double)pictureWidth / max;

        os << "Distribution density function:\n";
        for (k = 0; k < nc; k++)
            plotline(os, "< ", getBasepoint(k), (k == 0 ? cellUnder : getCellValue(k-1)), a);
        plotline(os, ">=", getBasepoint(nc), cellOver, a);
        os << "\n";
    }
    return os.str();
}

void cDensityEstBase::saveToFile(FILE *f) const
{
    cStdDev::saveToFile(f);

    fprintf(f, "%d\t #= transformed\n", transformed);
    fprintf(f, "%d\t #= range_mode\n", rangeMode);
    fprintf(f, "%g\t #= range_ext_factor\n", rangeExtFactor);
    fprintf(f, "%g %g\t #= range\n", rangeMin, rangeMax);
    fprintf(f, "%lu %lu\t #= cell_under, cell_over\n", cellUnder, cellOver);
    fprintf(f, "%d\t #= num_firstvals\n", numPrecollected);

    fprintf(f, "%d\t #= firstvals[] exists\n", precollectedValues != nullptr);
    if (precollectedValues)
        for (int i = 0; i < numValues; i++)
            fprintf(f, " %g\n", precollectedValues[i]);

}

void cDensityEstBase::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);

    freadvarsf(f, "%d\t #= transformed", &transformed);
    freadvarsf(f, "%d\t #= range_mode", &rangeMode);
    freadvarsf(f, "%g\t #= range_ext_factor", &rangeExtFactor);
    freadvarsf(f, "%g %g\t #= range", &rangeMin, &rangeMax);
    freadvarsf(f, "%lu %lu\t #= cell_under, cell_over", &cellUnder, &cellOver);
    freadvarsf(f, "%ld\t #= num_firstvals", &numPrecollected);

    int firstvals_exists;
    freadvarsf(f, "%d\t #= firstvals[] exists", &firstvals_exists);

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (firstvals_exists) {
        precollectedValues = new double[numPrecollected];
        for (int i = 0; i < numValues; i++)
            freadvarsf(f, " %g", precollectedValues + i);
    }
}

cDensityEstBase::Cell cDensityEstBase::getCellInfo(int k) const
{
    if (k < 0 || k >= getNumCells())
        return Cell();
    Cell c;
    c.lower = getBasepoint(k);
    c.upper = getBasepoint(k+1);
    c.value = getCellValue(k);
    c.relativeFreq = getCellPDF(k);
    return c;
}

const cDensityEstBase::Cell& cDensityEstBase::internalGetCellInfo(int k) const
{
    // only for use in sim_std.msg (each call overwrites the static buffer!)
    static Cell buf;
    buf = getCellInfo(k);
    return buf;
}

}  // namespace omnetpp

