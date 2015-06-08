//=========================================================================
//  CVARHIST.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cVarHistogram     : Variable bin size histogram
//
//   Authors: Gabor Lencse
//   Adapted by: Andras Varga
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
#include "omnetpp/globals.h"
#include "omnetpp/distrib.h"
#include "omnetpp/cvarhist.h"
#include "omnetpp/csimulation.h"  // __contextComponentRNG()
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

NAMESPACE_BEGIN

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))

Register_Class(cVarHistogram);

//----
// cVarHistogram - member functions

cVarHistogram::cVarHistogram(const char *name, int maxnumcells, TransformType transformtype) :
    cHistogramBase(name, -1)  //--LG
{
    // num_cells==-1 means that no bin boundaries are defined (num_cells+1 is 0)
    rangeMode = RANGE_NOTSET;
    transformType = transformtype;
    maxNumCells = maxnumcells;
    cellLowerBounds = nullptr;
    ASSERT(precollectedValues);  // base class must have allocated it for RANGE_AUTO

    if ((transformType == HIST_TR_AUTO_EPC_DBL ||
         transformType == HIST_TR_AUTO_EPC_INT) && maxNumCells < 2)
        throw cRuntimeError(this, "constructor: the maximal number of cells should be >=2");
}

cVarHistogram::~cVarHistogram()
{
    delete[] cellLowerBounds;
}

void cVarHistogram::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cHistogramBase::parsimPack(buffer);

    buffer->pack(maxNumCells);
    if (buffer->packFlag(cellLowerBounds != nullptr))
        buffer->pack(cellLowerBounds, maxNumCells+1);
#endif
}

void cVarHistogram::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cHistogramBase::parsimUnpack(buffer);

    buffer->unpack(maxNumCells);
    if (buffer->checkFlag()) {
        cellLowerBounds = new double[maxNumCells+1];
        buffer->unpack(cellLowerBounds, maxNumCells+1);
    }
#endif
}

void cVarHistogram::addBinBound(double x)  //--LG
{
    if (isTransformed())
        throw cRuntimeError(this, "cannot add bin bound after transform()");

    // create bin_bounds if not exists
    if (cellLowerBounds == nullptr)
        cellLowerBounds = new double[maxNumCells+1];

    // expand if full
    if (numCells == maxNumCells) {
        double *temp = new double[maxNumCells * 2+1];
        memcpy(temp, cellLowerBounds, (maxNumCells+1) * sizeof(double));
        delete[] cellLowerBounds;
        cellLowerBounds = temp;
        maxNumCells *= 2;
    }

    // insert bound
    int i;
    for (i = numCells+1; cellLowerBounds[i] > x; i--)
        cellLowerBounds[i] = cellLowerBounds[i-1];  // shift up bin boundaries
    cellLowerBounds[i] = x;

    numCells++;

    if (rangeMode == RANGE_NOTSET || rangeMode == RANGE_AUTO || rangeMode == RANGE_AUTOLOWER)
        rangeMin = cellLowerBounds[0];
    if (rangeMode == RANGE_NOTSET || rangeMode == RANGE_AUTO || rangeMode == RANGE_AUTOUPPER)
        rangeMax = cellLowerBounds[numCells];
}

void cVarHistogram::copy(const cVarHistogram& res)
{
    // hack: as this ^ uses num_cells instead of max_num_cells, we must correct it:
    if (res.cellv) {
        delete[] cellv;
        cellv = new unsigned[maxNumCells];
        memcpy(cellv, res.cellv, numCells * sizeof(unsigned));
    }

    maxNumCells = res.maxNumCells;
    transformType = res.transformType;

    delete[] cellLowerBounds;
    cellLowerBounds = nullptr;
    if (res.cellLowerBounds) {
        cellLowerBounds = new double[maxNumCells+1];
        memcpy(cellLowerBounds, res.cellLowerBounds, (numCells+1) * sizeof(double));
    }
}

cVarHistogram& cVarHistogram::operator=(const cVarHistogram& res)  //--LG
{
    if (this == &res)
        return *this;
    cHistogramBase::operator=(res);
    copy(res);
    return *this;
}

static int double_compare_function(const void *p1, const void *p2)  //--LG
{
    double x1 = *(double *)p1;
    double x2 = *(double *)p2;

    if (x1 == x2)
        return 0;
    else if (x1 < x2)
        return -1;
    else
        return 1;
}

void cVarHistogram::createEquiprobableCells()
{
    // this method is called from transform() if equi-probable cells (automatic setup) was requested
    if (numCells > 0)
        throw cRuntimeError(this, "some bin bounds already present when making equi-probable cells");

    // setRange() methods must not be used with cVarHistogram's equi-probable cell auto-setup mode,
    // so range_mode should still be the RANGE_NOTSET that we set in the ctor
    if (rangeMode != RANGE_NOTSET)
        throw cRuntimeError(this, "setRange..() only supported with HIST_TR_NO_TRANSFORM mode");

    // this version automatically sets the cell boundaries...
    ASSERT(maxNumCells >= 2);  // maybe 1 is enough...

    // allocate cellv and bin_bounds
    cellv = new unsigned[maxNumCells];
    cellLowerBounds = new double[maxNumCells+1];

    qsort(precollectedValues, numValues, sizeof(double), double_compare_function);

    // expected sample number per cell
    double esnpc = numValues / (double)maxNumCells;

    int cell;  // index of cell being constructed
    int prev_index;  // index of first observation in firstvals[] that will go into cellv[cell]
    int index;  // index of first observation in firstvals[] that will be left for the next cell (cellv[cell+1])

    double prev_boundary;  // previous value of boundary
    double boundary;  // firstvals[index]

    // construct cells; last cell will be handled separately
    for (cell = 0, prev_index = 0, prev_boundary = precollectedValues[prev_index],
         rangeMin = cellLowerBounds[0] = precollectedValues[0], index = prev_index + (int)esnpc;

         cell < maxNumCells-1 && index < numValues;

         cell++, prev_index = index, prev_boundary = boundary,
         index = (int)MAX(prev_index + esnpc, (cell+1) * esnpc))
    {
        boundary = precollectedValues[index];
        if (boundary == prev_boundary) {
            // try to find a greater one
            int j;
            for (j = index; j < numValues && precollectedValues[j] == prev_boundary; j++)
                ;
            // remark: either j == num_vals or
            //  prev_boundary == firstvals[j-1] < firstvals[j] holds
            if (j == numValues)
                break;  // the cell-th cell/bin will be the last cell/bin
            else {
                index = j;  // a greater one was found
                boundary = precollectedValues[index];
            }
        }
        else {
            // go backwards in firstvals[] to find first observation that
            // equals `boundary' (that is, check if there's a j:
            // j<index, firstvals[j] == firstvals[index])
            int j;
            // sure: prev_boundary==firstvals[prev_index] < boundary
            //       AND index > prev_index (otherwise   ^^^ here '=' would be)
            //        ==> j >= prev_index when firstvals[j] is evaluated
            // for this reason we do not need to check j>=0
            for (j = index-1; precollectedValues[j] == boundary; j--)
                ;  // may run 0 or more times
            index = j+1;  // unnecessary if cycle ran 0 times
        }
        cellLowerBounds[cell+1] = boundary;
        cellv[cell] = index - prev_index;
    }

    // the last cell/bin:
    cellv[cell] = numValues - prev_index;

    // the last boundary:
    rangeMax = cellLowerBounds[cell+1] = precollectedValues[numValues-1];

    // correction of the last boundary (depends on DBL/INT)
    if (transformType == HIST_TR_AUTO_EPC_DBL) {
        double range = precollectedValues[numValues-1] - precollectedValues[0];
        double epsilon = range * 1e-6;  // hack: value < boundary; not '<='
        rangeMax = cellLowerBounds[cell+1] += epsilon;
    }
    else if (transformType == HIST_TR_AUTO_EPC_INT) {
        rangeMax = cellLowerBounds[cell+1] += 1;  // hack: take the next integer
    }

    // remark: cellv[0]...cellv[cell] are the valid cells
    numCells = cell+1;  // maybe num_cells < max_num_cells
}

void cVarHistogram::transform()  //--LG
{
    if (isTransformed())
        throw cRuntimeError(this, "transform(): histogram already transformed");

    setupRange();

    if (transformType == HIST_TR_AUTO_EPC_DBL || transformType == HIST_TR_AUTO_EPC_INT) {
        // create bin bounds based on firstvals[]
        createEquiprobableCells();
    }
    else {
        ASSERT(transformType == HIST_TR_NO_TRANSFORM);

        // all manually added bin bounds must be in the range
        if (rangeMode != RANGE_NOTSET) {
            if (rangeMin > cellLowerBounds[0] || rangeMax < cellLowerBounds[numCells])
                throw cRuntimeError(this, "some bin bounds out of preset range");

            if (rangeMin < cellLowerBounds[0])
                addBinBound(rangeMin);
            if (rangeMax > cellLowerBounds[numCells])
                addBinBound(rangeMax);
        }

        // create cell vector and insert observations
        cellv = new unsigned[numCells];
        for (int i = 0; i < numCells; i++)
            cellv[i] = 0;

        for (int i = 0; i < numValues; i++)
            collectTransformed(precollectedValues[i]);
    }

    delete[] precollectedValues;
    precollectedValues = nullptr;

    transformed = true;
}

void cVarHistogram::collectTransformed(double val)
{
    if (val < rangeMin) {  // rangemin == bin_bounds[0]
        cellUnder++;
    }
    else if (val >= rangeMax) {  // rangemax == bin_bounds[num_cells]
        cellOver++;
    }
    else {  // sample falls in the range of ordinary cells/bins
            // rangemin <= val < rangemax
            // binary search
        int lower_index, upper_index, index;
        for (lower_index = 0, upper_index = numCells,
             index = (lower_index + upper_index) / 2;

             lower_index < index;

             index = (lower_index + upper_index) / 2)
        {
            // cycle invariant: bin_bound[lower_index]<=val<bin_bounds[upper_index]
            if (val < cellLowerBounds[index])
                upper_index = index;
            else
                lower_index = index;
        }
        // here, bin_bound[lower_index]<=val<bin_bounds[lower_index+1]

        // increment the appropriate counter
        cellv[lower_index]++;
    }
}

// clear results
void cVarHistogram::clearResult()  //--LG
{
    cHistogramBase::clearResult();

    delete[] cellLowerBounds;
    cellLowerBounds = nullptr;
}

// return kth basepoint
double cVarHistogram::getBasepoint(int k) const
{
    if (k < numCells+1)
        return cellLowerBounds[k];
    else
        throw cRuntimeError(this, "invalid basepoint index %u", k);
}

double cVarHistogram::getCellValue(int k) const
{
    if (k < numCells)
        return cellv[k];
    else
        throw cRuntimeError(this, "invalid cell index %u", k);
}

double cVarHistogram::draw() const
{
    if (numValues == 0)
        return 0;

    cRNG *rng = getRNG();
    if (numValues < numPrecollected) {
        // randomly select a sample from the stored ones
        return precollectedValues[intrand(rng, numValues)];
    }
    else {
        double lower, upper;

        // generate in [lower, upper)
        double m = intrand(rng, numValues - cellUnder - cellOver);

        // select a random interval (k-1) and return a random number from
        // that interval generated according to uniform distribution.
        m -= cellUnder;
        int k;
        for (k = 0; m >= 0; k++)
            m -= cellv[k];
        lower = cellLowerBounds[k-1];
        upper = cellLowerBounds[k];

        return lower + dblrand(rng) * (upper - lower);
    }
}

double cVarHistogram::getPDF(double x) const  // --LG
{
    if (!numValues)
        return 0.0;

    if (!isTransformed())
        throw cRuntimeError(this, "getPDF(x) cannot be called before histogram is transformed");

    if (x < rangeMin || x >= rangeMax)
        return 0.0;

    // rangemin <= x < rangemax
    // binary search
    int lower_index, upper_index, index;
    for (lower_index = 0, upper_index = numCells,
         index = (lower_index + upper_index) / 2;

         lower_index < index;

         index = (lower_index + upper_index) / 2)
    {
        // cycle invariant: bin_bound[lower_index]<=x<bin_bounds[upper_index]
        if (x < cellLowerBounds[index])
            upper_index = index;
        else
            lower_index = index;
    }

    // here, bin_bound[lower_index]<=x<bin_bounds[lower_index+1]
    return cellv[lower_index] / (cellLowerBounds[lower_index+1] - cellLowerBounds[lower_index]) / numValues;
}

double cVarHistogram::getCDF(double) const
{
    throw cRuntimeError(this, "getCDF(x) not implemented");
}

void cVarHistogram::saveToFile(FILE *f) const  //--LG
{
    cHistogramBase::saveToFile(f);

    fprintf(f, "%d\t #= transform_type\n", transformType);
    fprintf(f, "%u\t #= max_num_cells\n", maxNumCells);

    fprintf(f, "%d\t #= bin_bounds[] exists\n", cellLowerBounds != nullptr);
    if (cellLowerBounds)
        for (int i = 0; i < maxNumCells+1; i++)
            fprintf(f, " %g\n", cellLowerBounds[i]);

}

void cVarHistogram::loadFromFile(FILE *f)
{
    cHistogramBase::loadFromFile(f);

    freadvarsf(f, "%d\t #= transform_type", &transformType);
    freadvarsf(f, "%u\t #= max_num_cells", &maxNumCells);

    // increase allocated size of cellv[] to max_num_cells
    if (cellv && maxNumCells > numCells) {
        unsigned int *new_cellv = new unsigned[maxNumCells];
        memcpy(new_cellv, cellv, numCells * sizeof(unsigned));
        delete[] cellv;
        cellv = new_cellv;
    }

    int binbounds_exists;
    freadvarsf(f, "%d\t #= bin_bounds[] exists", &binbounds_exists);
    delete[] cellLowerBounds;
    cellLowerBounds = nullptr;
    if (binbounds_exists) {
        cellLowerBounds = new double[maxNumCells+1];
        for (int i = 0; i < maxNumCells+1; i++)
            freadvarsf(f, " %g", cellLowerBounds + i);
    }
}

NAMESPACE_END

