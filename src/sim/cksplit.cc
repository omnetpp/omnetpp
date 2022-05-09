//=========================================================================
//  CKSPLIT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cKSplit : implements the k-split algorithm in 1 dimension
//
//   Original version by Babak Fakhamzadeh, TU Delft, Mar-Jun 1996
//   This version written by Andras Varga, 1997-98
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
#include <algorithm>
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cksplit.h"
#include "omnetpp/distrib.h"
#include "omnetpp/cexception.h"
#include "omnetpp/clog.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using std::ostream;
using std::endl;

namespace omnetpp {

Register_Class(cKSplit);

// Cell division criteria - they are used to decide whether a cell should be split.

//TODO potential improvement: after a split, if one bin never gathers new observations, assume it's outside the distribution! (pdf=1)

static const double CRITDATA_DEFAULT[] = {20, 10, 2};

int critfunc_const(const cKSplit&, cKSplit::Grid& g, int i, const double *c)
{
    return g.cells[i] >= c[0];
}

int critfunc_depth(const cKSplit& ks, cKSplit::Grid& g, int i, const double *c)
{
    int depth = g.reldepth - ks.getRootGrid().reldepth;
    double averageWeight = ks.isWeighted() ? ks.getSumWeights() /ks.getCount() : 1;
    return g.cells[i] >= averageWeight * c[1] * pow(c[2], depth);
}

static const double DIVDATA_DEFAULT[] = {0.0}; // force even distributions

double divfunc_const(const cKSplit&, cKSplit::Grid&, double, const double *d)
{
    return d[0];  // lambda=constant
}

double divfunc_babak(const cKSplit&, cKSplit::Grid& g, double mother, const double *d)
{
    int newobs = g.total - g.mother;
    double lambda = newobs / (d[1] * mother);
    return lambda < 1.0 ? lambda : 1.0;
}

//----

cKSplit::cKSplit(const cKSplit& r) : cPrecollectionBasedDensityEst(r)
{
    gridv = nullptr;
    iter = nullptr;
    copy(r);
}

cKSplit::cKSplit(const char *name, bool weighted) : cPrecollectionBasedDensityEst(name, weighted)
{
    if (K < 2 || (K > 2 && K != 2 * (int)(K / 2) + 1))
        throw cRuntimeError("cKSplit: K must be 2 or a >=3 odd number");

    numCells = 0;

    critFunc = critfunc_depth;
    critData = CRITDATA_DEFAULT;
    divFunc = divfunc_const;
    divData = DIVDATA_DEFAULT;
    rangeExtEnabled = true;

    gridv = nullptr;
    gridvSize = 0;
    rootGridIndex = lastGridIndex = -1;

    iter = nullptr;
    iterNumValues = -1;
}

cKSplit::~cKSplit()
{
    delete[] gridv;
    delete iter;
}

void cKSplit::parsimPack(cCommBuffer *buffer) const
{
    throw cRuntimeError(this, "parsimPack() not implemented");
}

void cKSplit::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "parsimUnpack() not implemented");
}

void cKSplit::copy(const cKSplit& res)
{
    numCells = res.numCells;

    rootGridIndex = res.rootGridIndex;
    lastGridIndex = res.lastGridIndex;
    gridvSize = res.gridvSize;

    delete[] gridv;
    if (!res.gridv)
        gridv = nullptr;
    else {
        gridv = new Grid[gridvSize+1];
        for (int i = 1; i <= lastGridIndex; i++)
            gridv[i] = res.gridv[i];
    }

    critFunc = res.critFunc;
    critData = res.critData;
    divFunc = res.divFunc;
    divData = res.divData;
    rangeExtEnabled = res.rangeExtEnabled;

    delete iter;
    iter = nullptr;
    iterNumValues = -1;
}

cKSplit& cKSplit::operator=(const cKSplit& res)
{
    if (this == &res)
        return *this;
    cPrecollectionBasedDensityEst::operator=(res);
    copy(res);
    return *this;
}

void cKSplit::setCritFunc(CritFunc _critfunc, double *_critdata)
{
    critFunc = _critfunc;
    critData = _critdata;
}

void cKSplit::setDivFunc(DivFunc _divfunc, double *_divdata)
{
    divFunc = _divfunc;
    divData = _divdata;
}

void cKSplit::setRangeExtension(bool enabled)
{
    rangeExtEnabled = enabled;
}

void cKSplit::resetGrids(int grid)
{
    Grid *g = &(gridv[grid]);
    g->total = g->mother = 0;
    for (double& cell : g->cells) {
        if (cell < 0)
            resetGrids(-cell);
        else
            cell = 0;
    }
}

void cKSplit::merge(const cStatistic *other)
{
    throw cRuntimeError(this, "The cKSplit class does not support merge()");
}

void cKSplit::doMergeBinValues(const cPrecollectionBasedDensityEst *other)
{
    ASSERT(false);  // never comes here, as merge() already throws an error
}

void cKSplit::clear()
{
    cPrecollectionBasedDensityEst::clear();

    numCells = 0;
    delete [] gridv;
    gridv = nullptr;
    gridvSize = 0;
    rootGridIndex = lastGridIndex = -1;

    iter = nullptr;
    iterNumValues = -1;
}

void cKSplit::setUpBins()
{
    if (binsAlreadySetUp())
        throw cRuntimeError(this, "setUpBins(): Histogram bins already set up");

    setupRange();
    createRootGrid();

    // Trick to avoid error from cell splits: first, insert observations
    // just to create grid structure; second, reset all cells to zero and
    // insert observations again, with cell splits disabled now.

    for (int i = 0; i < numValues; i++)
        if (!weighted)
            collectIntoHistogram(precollectedValues[i]);
        else
            collectWeightedIntoHistogram(precollectedValues[i], precollectedWeights[i]);

    resetGrids(rootGridIndex);

    for (int i = 0; i < numValues; i++)
        if (!weighted)
            insertIntoGrids(precollectedValues[i], 1.0, false);
        else
            insertIntoGrids(precollectedValues[i], precollectedWeights[i], false);

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (weighted) {
        delete[] precollectedWeights;
        precollectedWeights = nullptr;
    }

    transformed = true;
}

void cKSplit::createRootGrid()
{
    gridvSize = 8;
    gridv = new Grid[gridvSize+1];

    rootGridIndex = 1;
    lastGridIndex = 1;
    numCells = K;

    gridv[1].parent = 0;
    gridv[1].reldepth = 0;
    gridv[1].total = 0;
    gridv[1].mother = 0;
    for (double& cell : gridv[1].cells)
        cell = 0;
}

void cKSplit::collectIntoHistogram(double value)
{
    collectWeightedIntoHistogram(value, 1.0);
}

void cKSplit::collectWeightedIntoHistogram(double value, double weight)
{
    if (std::isinf(value)) {
        if (value < 0) {
            numNegInfs++;
            negInfSumWeights += value;
        }
        else {
            numPosInfs++;
            posInfSumWeights += value;
        }
    }
    else {
        // see if x fits into current range and act accordingly
        if (value >= rangeMin && value < rangeMax)
            insertIntoGrids(value, weight, true);
        else if (rangeExtEnabled)
            newRootGrids(value, weight);
        else if (value < rangeMin) {
            numFiniteUnderflows++;
            finiteUnderflowSumWeights += weight;
        }
        else if (value >= rangeMax) {
            numFiniteOverflows++;
            finiteOverflowSumWeights += weight;
        }
    }
}

void cKSplit::insertIntoGrids(double value, double weight, int enableSplits)
{
    if (!std::isfinite(value))
        return;

    int i;

    // new observation fits in the current grid
    double gridmin = rangeMin;
    double gridmax = rangeMax;
    double cellsize = (gridmax - gridmin) / (double)K;

    int location = rootGridIndex;

    // keep searching until the right grid has been found
    for (;;) {
        gridv[location].total++;

        // calc. the cell in which the new observation falls
        i = (int)((value - gridmin) / cellsize);

        // guard against rounding errors
        if (i < 0)
            i = 0;
        if (i >= K)
            i = K-1;

        // exit loop if no subgrid
        if (gridv[location].cells[i] >= 0)
            break;

        // go down to subgrid
        location = -gridv[location].cells[i];

        gridmin += i * cellsize;
        // gridmax = gridmin + cellsize;
        cellsize /= K;
    }

    // arrived at gridv[location].cells[i] -- possibly split this cell
    // But do not split it if it is already too small (compared to the
    // edge values themselves), otherwise we might get a 0 wide bin due
    // to the limited precision of the `double` type, which is forbidden.
    if (enableSplits && critFunc(*this, gridv[location], i, critData)
            && (cellsize > std::max(std::abs(gridmin), std::abs(gridmax)) * 1e-12)) {

        splitCell(location, i);

        // go down to new subgrid and insert the observation there
        // (NOTE: code is copied here from the above loop)

        location = -gridv[location].cells[i];

        gridmin += i * cellsize;
        // gridmax = gridmin + cellsize;
        cellsize /= K;

        gridv[location].total++;

        i = (int)((value - gridmin) / cellsize);

        if (i < 0)
            i = 0;
        if (i >= K)
            i = K-1;
    }

    // increment cell value
    gridv[location].cells[i] += weight;
}

void cKSplit::splitCell(int grid, int cell)
{
    // make room for new grid
    if (lastGridIndex == gridvSize)
        expandGridVector();

    // create shorthands for the two grids in question
    Grid& g = gridv[grid];
    Grid& subg = gridv[lastGridIndex+1];

    // if none of the cells in the current grid has divided yet, than
    // the observations from the mother cell have to be divided as well
#ifdef DISTRIBUTE_ON_CHILD_SPLIT
    if (g.mother > 0)
        distributeMotherObservations(grid);
#endif

    // insert subg into g.cell[cell]
    subg.parent = grid;
    subg.reldepth = g.reldepth+1;
    subg.mother = g.cells[cell];
    subg.total = subg.mother;
    for (double& cell : subg.cells)
        cell = 0;

    g.cells[cell] = -(lastGridIndex+1);

    lastGridIndex++;
    numCells += K-1;
}

 #ifdef DISTRIBUTE_ON_CHILD_SPLIT
void cKSplit::distributeMotherObservations(int grid)
{
    Grid& g = gridv[grid];
    Grid orig = g;

    for (int i = 0; i < K; i++)
        g.cells[i] = (unsigned)getRealCellValue(orig, i);  // WRONG!!!

    g.mother = 0;
}

#endif

void cKSplit::newRootGrids(double value, double weight)
{
    // new "supergrid" has to be inserted until it includes x
    for (;;) {
        // a new grid still has to be inserted around the current root grid
        if (lastGridIndex == gridvSize)
            expandGridVector();

        int old_rootgrid = rootGridIndex;
        rootGridIndex = lastGridIndex+1;

        lastGridIndex++;
        numCells += K-1;

        gridv[old_rootgrid].parent = rootGridIndex;
        gridv[rootGridIndex].parent = 0;
        gridv[rootGridIndex].reldepth = gridv[old_rootgrid].reldepth-1;
        gridv[rootGridIndex].total = gridv[old_rootgrid].total;
        gridv[rootGridIndex].mother = 0;
        for (double& cell : gridv[rootGridIndex].cells)
            cell = 0;

        double gridsize = rangeMax - rangeMin;
        if (K == 2) {
            if (value < rangeMin) {
                gridv[rootGridIndex].cells[1] = -old_rootgrid;
                rangeMin -= gridsize;
            }
            else {  // (x>=rangemax)
                gridv[rootGridIndex].cells[0] = -old_rootgrid;
                rangeMax += gridsize;
            }
        }
        else {
            gridv[rootGridIndex].cells[(K-1) / 2] = -old_rootgrid;

            rangeMin -= (K-1) / 2.0 * gridsize;
            rangeMax += (K-1) / 2.0 * gridsize;
        }

        if (value >= rangeMin && value < rangeMax)
            break;
    }

    // now, insert x into new root grid

    // calc. in which cell x falls
    int i = (int)(K * (value - rangeMin) / (rangeMax - rangeMin));

    // if it falls in the old root grid, we have to correct it
    if (i == (K-1) / 2) {
        if (value > (rangeMax - rangeMin) / 2)
            i++;
        else
            i--;
    }

    // increment cell value
    gridv[rootGridIndex].cells[i]++;
    gridv[rootGridIndex].total++;
}

void cKSplit::expandGridVector()
{
    gridvSize += 8;
    Grid *newgridv = new Grid[gridvSize+1];

    for (int i = 1; i <= lastGridIndex; i++)
        newgridv[i] = gridv[i];

    delete[] gridv;
    gridv = newgridv;
}

double cKSplit::getRealCellValue(Grid& grid, int i) const
{
    // returns the actual amount of observations in cell 'i' of 'grid'

#ifdef DISTRIBUTE_ON_CHILD_SPLIT
    double mother = grid.mother;
#else
    // might go up until the root grid to collect mother observations
    double mother;
    if (grid.parent == 0) {
        mother = 0;
    }
    else {
        // find (into k) which cell of the parent our 'grid' was
        int k;
        Grid& parentgrid = gridv[grid.parent];
        int gridnum = &grid - gridv;
        for (k = 0; k < K; k++)
            if (parentgrid.cells[k] == -gridnum)
                break;


        // parent grid's undistributed observations
        mother = getRealCellValue(parentgrid, k);
    }
#endif
    // interpolate between even and proportional division of the mother obs.
    double lambda = divFunc(*this, grid, mother, divData);

    int cell_tot = grid.cells[i];
    if (cell_tot < 0)
        cell_tot = gridv[-cell_tot].total;
    int cell_mot = grid.cells[i];
    if (cell_mot < 0)
        cell_mot = gridv[-cell_mot].mother;

    double even = mother / K;
    double prop = mother * cell_tot / (grid.total - grid.mother);

    return cell_mot + (1 - lambda) * even + lambda * prop;
}

int cKSplit::getTreeDepth() const
{
    return getTreeDepth(gridv[rootGridIndex]);
}

int cKSplit::getTreeDepth(Grid& grid) const
{
    int maxDepth = 0;
    for (int cell : grid.cells) {
        if (cell < 0) {
            int depth = getTreeDepth(gridv[-(int)cell]);
            if (depth > maxDepth)
                maxDepth = depth;
        }
    }
    return maxDepth+1;
}

void cKSplit::printGrids() const
{
    if (!binsAlreadySetUp()) {
        EV << "collecting initial observations; no grids yet.\n";
        return;
    }
    EV << "Range: " << rangeMin << "..." << rangeMax << "\n";
    EV << "Root grid: #" << rootGridIndex << "\n";
    for (int i = 1; i <= lastGridIndex; i++) {
        EV << "grid #" << i << ": parent=#" << gridv[i].parent;
        EV << "  total=" << gridv[i].total;
        EV << "  mother=" << gridv[i].mother << "  (";

        for (int cell : gridv[i].cells)
            if (cell < 0)
                EV << " " << cell;
            else
                EV << " " << cell;

        EV << ")\n";
    }
}

void cKSplit::iteratorToCell(int cell_nr) const
{
    // Note: Ideally, we should always just use the existing iterator instance,
    // and increment/decrement it to reach the desired cell. However, iteration
    // will cause numeric errors to accumulate, causing bin edges to be
    // slightly off, potentially causing the recorded histogram (or the fingerprint
    // if it contains statistic results as ingredient) to be off.
    // Saved results/fingerprint should not depend on the iteration history,
    // so we always create a new iterator for now, until the issue is resolved.
    delete iter;
    iter = nullptr;

    // create iterator or reinit if it is stale
    if (!iter) {
        iter = new Iterator(*this);
        iterNumValues = numValues;
    }
    else if (numValues != iterNumValues) {
        iter->init(*this, cell_nr < numCells / 2);
        iterNumValues = numValues;
    }

    // drive iterator up or down to reach cell_nr
    if (cell_nr > iter->getCellNumber())
        while (cell_nr != iter->getCellNumber())
            (*iter)++;

    else if (cell_nr < iter->getCellNumber())
        while (cell_nr != iter->getCellNumber())
            (*iter)--;
}

int cKSplit::getNumBins() const
{
    if (!binsAlreadySetUp())
        return 0;
    return numCells;
}

double cKSplit::getBinValue(int nr) const
{
    if (nr >= numCells)
        return 0.0;

    iteratorToCell(nr);
    return iter->getCellValue();
}

double cKSplit::getBinEdge(int nr) const
{
    if (nr >= numCells)
        return rangeMax;

    iteratorToCell(nr);
    return iter->getCellMin();
}

void cKSplit::saveToFile(FILE *f) const
{
    cPrecollectionBasedDensityEst::saveToFile(f);

    fprintf(f, "%d\t #= num_cells\n", numCells);
    fprintf(f, "%d\t #= gridv_size\n", gridvSize);
    fprintf(f, "%d\t #= rootgrid\n", rootGridIndex);
    fprintf(f, "%d\t #= lastgrid\n", lastGridIndex);

    fprintf(f, "%d\t #= gridv_exists\n", gridv != nullptr);
    if (gridv) {
        for (int i = 1; i <= lastGridIndex; i++) {
            fprintf(f, "# grid %d\n", i);
            fprintf(f, "%d\t #= parent\n", gridv[i].parent);
            fprintf(f, "%d\t #= reldepth\n", gridv[i].reldepth);
            fprintf(f, "%lg\t #= total\n", gridv[i].total);
            fprintf(f, "%lg\t #= mother\n", gridv[i].mother);
            if (K == 2)
                fprintf(f, "%lg %lg\t #= cells[0], cells[1]\n", gridv[i].cells[0], gridv[i].cells[1]);
            else {
                fprintf(f, "#= cells[]\n");
                for (double cell : gridv[i].cells)
                    fprintf(f, " %lg\n", cell);
            }
        }
    }
}

void cKSplit::loadFromFile(FILE *f)
{
    cPrecollectionBasedDensityEst::loadFromFile(f);

    freadvarsf(f, "%d\t #= num_cells", &numCells);
    freadvarsf(f, "%d\t #= gridv_size", &gridvSize);
    freadvarsf(f, "%d\t #= rootgrid", &rootGridIndex);
    freadvarsf(f, "%d\t #= lastgrid", &lastGridIndex);

    int gridv_exists;
    freadvarsf(f, "%d\t #= gridv_exists", &gridv_exists);

    delete[] gridv;
    gridv = nullptr;
    if (gridv_exists) {
        gridv = new Grid[gridvSize+1];
        for (int i = 1; i <= lastGridIndex; i++) {
            freadvarsf(f, "%d\t #= parent", &gridv[i].parent);
            freadvarsf(f, "%d\t #= reldepth", &gridv[i].reldepth);
            freadvarsf(f, "%lg\t #= total", &gridv[i].total);
            freadvarsf(f, "%lg\t #= mother", &gridv[i].mother);
            if (K == 2)
                freadvarsf(f, "%lg %lg\t #= cells[0], cells[1]", gridv[i].cells + 0, gridv[i].cells+1);
            else {
                freadvarsf(f, "#= cells[]");
                for (double& cell : gridv[i].cells)
                    freadvarsf(f, " %lg", &cell);

            }
        }
    }
}

//----

cKSplit::Iterator::Iterator(const cKSplit& _ks, bool _beg)
{
    init(_ks, _beg);
}

void cKSplit::Iterator::init(const cKSplit& _ks, bool _beg)
{
    ks = const_cast<cKSplit *>(&_ks);
    grid = ks->rootGridIndex;
    cellnum = _beg ? 0 : ks->numCells-1;
    cell = _beg ? 0 : K-1;
    cellsize = (ks->rangeMax - ks->rangeMin) / K;
    gridmin = ks->rangeMin;

    dive(_beg ? 0 : K-1);
}

void cKSplit::Iterator::dive(int where)
{
    // go into subgrids as deep as possible, along cells[where]
    while (ks->gridv[grid].cells[cell] < 0) {
        gridmin += cell * cellsize;
        grid = -ks->gridv[grid].cells[cell];

        cell = where;
        cellsize /= K;
    }
}

void cKSplit::Iterator::operator++(int)
{
    if (grid == 0) {
        init(*ks, 1);
        return;
    }

    cellnum++;

    cell++;
    while (cell == K) {
        // step back to parent
        int i, oldgrid = grid;
        grid = ks->gridv[grid].parent;
        if (grid == 0)
            return;

        // find (into i) which cell of the parent 'grid' was
        for (i = 0; i < K; i++)
            if (ks->gridv[grid].cells[i] == -oldgrid)
                break;


        // calc. cellsize and left edge of parent grid
        cellsize *= K;
        gridmin -= i * cellsize;

        cell = i+1;  // actual '++' operation
    }
    dive(0);
}

void cKSplit::Iterator::operator--(int)
{
    if (grid == 0) {
        init(*ks, 0);
        return;
    }

    cellnum--;

    cell--;
    while (cell == -1) {
        // step to parent
        int i, oldgrid = grid;
        grid = ks->gridv[grid].parent;
        if (grid == 0)
            return;

        // find (into i) which cell of the parent 'grid' was
        for (i = 0; i < K; i++)
            if (ks->gridv[grid].cells[i] == -oldgrid)
                break;


        // calc. cellsize and left edge of parent grid
        cellsize *= K;
        gridmin -= i * cellsize;

        cell = i-1;  // actual '--' operation
    }
    dive(K-1);
}

double cKSplit::Iterator::getCellValue() const
{
    cKSplit::Grid& g = ks->gridv[grid];
    return ks->getRealCellValue(g, cell);
}

}  // namespace omnetpp

