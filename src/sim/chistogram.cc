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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"
#include "distrib.h"
#include "globals.h"
#include "cdetect.h"
#include "chistogram.h"
#include "cexception.h"
#include "cenvir.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

NAMESPACE_BEGIN

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))


Register_Class(cLongHistogram);
Register_Class(cDoubleHistogram);


cHistogramBase::cHistogramBase(const char *name, int numcells) :
cDensityEstBase(name)
{
    cellv = NULL;
    num_cells = numcells;
}

cHistogramBase::~cHistogramBase()
{
    delete [] cellv;
}

void cHistogramBase::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, eNOPARSIM);
#else
    cDensityEstBase::parsimPack(buffer);
    buffer->pack(num_cells);

    if (buffer->packFlag(cellv!=NULL))
        buffer->pack(cellv, num_cells);
#endif
}

void cHistogramBase::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, eNOPARSIM);
#else
    cDensityEstBase::parsimUnpack(buffer);
    buffer->pack(num_cells);

    if (buffer->checkFlag())
    {
        cellv = new unsigned int[num_cells];
        buffer->unpack(cellv, num_cells);
    }
#endif
}

void cHistogramBase::copy(const cHistogramBase& res)
{
    num_cells = res.num_cells;
    delete [] cellv;
    cellv = NULL;
    if (res.cellv)
    {
        cellv = new unsigned[num_cells];
        memcpy(cellv, res.cellv, num_cells*sizeof(unsigned));
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
    for (int i=0; i<num_cells; i++)
        cellv[i] += (unsigned int) other->getCellValue(i);  //TODO overflow check
}

void cHistogramBase::clearResult()
{
    cDensityEstBase::clearResult();

    delete [] cellv;
    cellv = NULL;
}

void cHistogramBase::transform()
{
    if (isTransformed())
        throw cRuntimeError(this, "transform(): histogram already transformed");

    setupRange(); // this will set num_cells if it was unspecified (-1)

    int i;
    cellv = new unsigned [num_cells];
    for (i=0; i<num_cells; i++)
        cellv[i] = 0;

    for (i=0; i<num_vals; i++)
        collectTransformed(firstvals[i]);

    delete [] firstvals;
    firstvals = NULL;

    transfd = true;
}

int cHistogramBase::getNumCells() const
{
    if (!isTransformed())
        return 0;
    return num_cells;
}

void cHistogramBase::saveToFile(FILE *f) const
{
    cDensityEstBase::saveToFile(f);
    fprintf(f, "%d\t #= num_cells\n", num_cells);
    fprintf(f, "%d\t #= cellv[] exists\n", cellv!=NULL);
    if (cellv) for (int i=0; i<num_cells; i++) fprintf(f, " %u\n", cellv[i]);
}

void cHistogramBase::loadFromFile(FILE *f)
{
    cDensityEstBase::loadFromFile(f);
    freadvarsf(f, "%d\t #= num_cells", &num_cells);

    int cellv_exists;
    freadvarsf(f, "%d\t #= cellv[] exists", &cellv_exists);
    delete [] cellv; cellv = NULL;
    if (cellv_exists)
    {
        cellv = new unsigned[num_cells];
        for (int i=0; i<num_cells; i++) freadvarsf(f, " %u", cellv+i);
    }
}

void cHistogramBase::setNumCells(int numcells)
{
    if (cellv)
        throw cRuntimeError(this, "setNumCells(): too late, cells already set up");
    num_cells = numcells;
}


//----
// cHistogram - member functions

cHistogram::cHistogram(const char *name, int numcells, Mode mode) :
cHistogramBase(name, numcells)
{
    cellsize = 0;
    this->mode = mode;
}

void cHistogram::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, eNOPARSIM);
#else
    cHistogramBase::parsimPack(buffer);
    buffer->pack(cellsize);
#endif
}

void cHistogram::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, eNOPARSIM);
#else
    cHistogramBase::parsimUnpack(buffer);
    buffer->unpack(cellsize);
#endif
}

void cHistogram::copy(const cHistogram& res)
{
    cellsize = res.cellsize;
    mode = res.mode;
}

cHistogram& cHistogram::operator=(const cHistogram& res)
{
    if (this==&res) return *this;
    cHistogramBase::operator=(res);
    copy(res);
    return *this;
}

void cHistogram::setMode(Mode mode)
{
    if (isTransformed())
        throw cRuntimeError(this, "setMode() cannot be called when cells have been set up already");
    this->mode = mode;
}

void cHistogram::setCellSize(double d)
{
    if (isTransformed())
        throw cRuntimeError(this, "setCellSize() cannot be called when cells have been set up already");
    cellsize = d;
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

    if (mode == MODE_AUTO)
    {
        // if all precollected numbers are integers (and they are not all zeroes), go for integer mode
        bool allzeroes = true;
        bool allints = true;
        for (int i=0; i < num_vals; i++)
        {
            if (firstvals[i] != 0)
                allzeroes = false;
            if (firstvals[i] != floor(firstvals[i]))
                allints = false;
        }

        mode = (num_vals > 0 && allints && !allzeroes) ? MODE_INTEGERS : MODE_DOUBLES;
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
    long cellsize = (long) this->cellsize;

    // convert range limits to integers
    rangemin = floor(rangemin);
    rangemax = ceil(rangemax);

    if (range_mode == RANGE_FIXED)
    {
#define COMPLAINT "Cannot set up cells to satisfy constraints"
        long range = (long)(rangemax-rangemin);

        if (num_cells>0 && cellsize>0) {
            if (num_cells*cellsize != range)
                throw cRuntimeError(this, COMPLAINT": numCells*cellSize != rangeMax-rangeMin");
        }
        else if (cellsize>0) {
            if (range % cellsize != 0)
                throw cRuntimeError(this, COMPLAINT": specified range is not a multiple of cellSize");
            num_cells = range / cellsize;
        }
        else if (num_cells>0) {
            if (range % num_cells != 0)
                throw cRuntimeError(this, COMPLAINT": specified range is not a multiple of numCells");
            cellsize = range / num_cells;
        }
        else {
            int mincellsize = (int) ceil(range/200.0);
            int maxcellsize = (int) ceil(range/10.0);
            for (cellsize = mincellsize; cellsize <= maxcellsize; cellsize++)
                if (range % cellsize == 0)
                    break;
            if (cellsize > maxcellsize)
                throw cRuntimeError(this, COMPLAINT": specified range is too large, and cannot divide it to 10..200 equal-sized cells");
            num_cells = range / cellsize;
        }
#undef COMPLAINT
    }
    else
    {
        // non-fixed range
        if (num_cells>0 && cellsize>0) {
            // both given; num_cells*cellsize will determine the range
        }
        else if (num_cells>0) {
            // num_cells given ==> choose cellsize
            cellsize = (long) ceil((rangemax-rangemin)/num_cells);
        }
        else if (cellsize>0) {
            // cellsize given ==> choose num_cells
            num_cells = (int) ceil((rangemax-rangemin)/cellsize);
        }
        else {
            // neither given, choose both
            double range = rangemax - rangemin;
            cellsize = (long) ceil(range / 200.0);  // for range<=200, cellsize==1
            num_cells = (int) ceil(range/cellsize);
        }

        // adjust range to be cellsize*num_cells
        double newrange = cellsize*num_cells;
        double rangediff = newrange - (rangemax-rangemin);

        switch (range_mode)
        {
           case RANGE_AUTO:
             rangemin -= floor(rangediff/2);
             rangemax = rangemin + newrange;
             break;
           case RANGE_AUTOLOWER:
             rangemin = rangemax - newrange;
             break;
           case RANGE_AUTOUPPER:
             rangemax = rangemin + newrange;
             break;
        }
    }

    // write back the integer cellsize into double
    this->cellsize = cellsize;
}

void cHistogram::setupRangeDouble()
{
    if (num_cells == -1)
        num_cells = 30; // to allow merging every 2, 3, 5, 6 adjacent cells in post-processing
    cellsize = (rangemax - rangemin) / num_cells;
}

double cHistogram::random() const
{
    if (num_vals == 0)
    {
        return 0L;
    }
    else if (num_vals < num_firstvals)
    {
        // randomly select a sample from the stored ones
        return firstvals[genk_intrand(genk, num_vals)];
    }
    else
    {
        long m = genk_intrand(genk, num_vals - cell_under - cell_over);

        // select a random cell (k-1) and return a random number from it
        int k;
        for (k = 0; m >= 0; k++)
           m -= cellv[k];

        if (mode == MODE_INTEGERS)
        {
            // min_vals, max_vals: integer-valued doubles (e.g.: -3.0, 5.0)
            // rangemin, rangemax: doubles like -1.5, 4.5 (integer+0.5)
            // cellsize: integer-valued double, >0
            return ceil(rangemin) + (k-1)*(long)cellsize + genk_intrand(genk, (long)cellsize);
        }
        else
        {
            // return an uniform double from the given cell
            return rangemin + (k-1)*cellsize + genk_dblrand(genk)*cellsize;
        }
    }
}

void cHistogram::collectTransformed (double val)
{
    int k = (int)floor((val-rangemin)/cellsize);
    if (k < 0 || val < rangemin)
        cell_under++;
    else if (k >= num_cells || val >= rangemax)
        cell_over++;
    else
        cellv[k]++;
}

double cHistogram::getPDF(double x) const
{
    if (!isTransformed())
        throw cRuntimeError(this, "getPDF(x) cannot be called before histogram is transformed");

    int k = (int)floor((x-rangemin)/cellsize);
    if (k < 0 || x < rangemin || k >= num_cells || x >= rangemax)
        return 0.0;

    return cellv[k] / cellsize / num_vals;
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

    if (k<0 || k>num_cells)
        throw cRuntimeError(this, "invalid basepoint index %u", k);

    if (k == num_cells)
        return rangemax;
    else
        return rangemin + k*cellsize;
}

double cHistogram::getCellValue(int k) const
{
    if (k<0 || k>num_cells)
        throw cRuntimeError(this, "invalid cell index %u", k);
    return cellv[k];
}

void cHistogram::saveToFile(FILE *f) const
{
    cHistogramBase::saveToFile(f);
    fprintf(f, "%g\t #= cellsize\n", cellsize);
}

void cHistogram::loadFromFile(FILE *f)
{
    cHistogramBase::loadFromFile(f);
    freadvarsf(f, "%g\t #= cellsize", &cellsize);
}

cLongHistogram& cLongHistogram::operator=(const cLongHistogram& res)
{
    if (this==&res) return *this;
    cHistogram::operator=(res);
    copy(res);
    return *this;
}

cDoubleHistogram& cDoubleHistogram::operator=(const cDoubleHistogram& res)
{
    if (this==&res) return *this;
    cHistogram::operator=(res);
    copy(res);
    return *this;
}

NAMESPACE_END

