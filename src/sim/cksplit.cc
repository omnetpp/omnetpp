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
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cksplit.h"
#include "omnetpp/distrib.h"
#include "omnetpp/cexception.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimulation.h"  // __contextComponentRNG()

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using std::ostream;
using std::endl;

namespace omnetpp {

Register_Class(cKSplit);


//----
// Cell division criteria - they are used to decide whether a cell should be split.

double critdata_default[] = {20, 4, 2};

int critfunc_const(const cKSplit&, cKSplit::Grid& g, int i, double *c)
{
    return g.cells[i] >= c[0];
}

int critfunc_depth(const cKSplit& ks, cKSplit::Grid& g, int i, double *c)
{
    int depth = g.reldepth - ks.getRootGrid().reldepth;
    return g.cells[i] >= c[1] * pow(c[2], depth);
}

double divdata_default[] = {0.5};

double divfunc_const(const cKSplit&, cKSplit::Grid&, double, double *d)
{
    return d[0];  // lambda=constant
}

double divfunc_babak(const cKSplit&, cKSplit::Grid& g, double mother, double *d)
{
    int newobs = g.total - g.mother;
    double lambda = newobs / (d[1] * mother);
    return lambda < 1.0 ? lambda : 1.0;
}

//----

cKSplit::cKSplit(const cKSplit& r) : cDensityEstBase(r)
{
    gridv = nullptr;
    iter = nullptr;
    copy(r);
}

cKSplit::cKSplit(const char *name) : cDensityEstBase(name)
{
    if (K < 2 || (K > 2 && K != 2 * (int)(K / 2) + 1))
        throw cRuntimeError("cKSplit: K must be 2 or a >=3 odd number");

    numCells = 0;

    critFunc = critfunc_depth;
    critData = critdata_default;
    divFunc = divfunc_const;
    divData = divdata_default;
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
    cDensityEstBase::operator=(res);
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
    for (int & cell : g->cells) {
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

void cKSplit::doMergeCellValues(const cDensityEstBase *other)
{
    ASSERT(false);  // never comes here, as merge() already throws an error
}

void cKSplit::transform()
{
    ASSERT(!weighted);

    if (isTransformed())
        throw cRuntimeError(this, "transform(): Histogram already transformed");

    setupRange();
    createRootGrid();

    // Trick to avoid error from cell splits: first, insert observations
    // just to create grid structure; second, reset all cells to zero and
    // insert observations again, with cell splits disabled now.

    for (int i = 0; i < numValues; i++)
        collectTransformed(precollectedValues[i]);

    resetGrids(rootGridIndex);

    for (int i = 0; i < numValues; i++)
        insertIntoGrids(precollectedValues[i], false);

    delete[] precollectedValues;
    precollectedValues = nullptr;

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
    for (int & cell : gridv[1].cells)
        cell = 0;
}

void cKSplit::collectTransformed(double x)
{
    // see if x fits into current range and act accordingly
    if (x >= rangeMin && x < rangeMax)
        insertIntoGrids(x, true);
    else if (rangeExtEnabled)
        newRootGrids(x);
    else if (x < rangeMin) {
        numUnderflows++;
        underflowSumWeights += 1;
    }
    else if (x >= rangeMax) {
        numOverflows++;
        overflowSumWeights += 1;
    }
}

void cKSplit::collectTransformed2(double value, double weight)
{
    ASSERT(false); // weighted case is unsupported
}

void cKSplit::insertIntoGrids(double x, int enable_splits)
{
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
        i = (int)((x - gridmin) / cellsize);

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
    if (enable_splits && critFunc(*this, gridv[location], i, critData)) {
        splitCell(location, i);

        // go down to new subgrid and insert the observation there
        // (NOTE: code is copied here from the above loop)

        location = -gridv[location].cells[i];

        gridmin += i * cellsize;
        // gridmax = gridmin + cellsize;
        cellsize /= K;

        gridv[location].total++;

        i = (int)((x - gridmin) / cellsize);

        if (i < 0)
            i = 0;
        if (i >= K)
            i = K-1;
    }

    // increment cell value
    gridv[location].cells[i]++;
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
    for (int & cell : subg.cells)
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

void cKSplit::newRootGrids(double x)
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
        for (int & cell : gridv[rootGridIndex].cells)
            cell = 0;

        double gridsize = rangeMax - rangeMin;
        if (K == 2) {
            if (x < rangeMin) {
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

        if (x >= rangeMin && x < rangeMax)
            break;
    }

    // now, insert x into new root grid

    // calc. in which cell x falls
    int i = (int)(K * (x - rangeMin) / (rangeMax - rangeMin));

    // if it falls in the old root grid, we have to correct it
    if (i == (K-1) / 2) {
        if (x > (rangeMax - rangeMin) / 2)
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
    if (!isTransformed()) {
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
    // create iterator or reinit if it is stale
    iter = nullptr; //FIXME why?
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

int cKSplit::getNumCells() const
{
    if (!isTransformed())
        return 0;
    return numCells;
}

double cKSplit::getCellValue(int nr) const
{
    if (nr >= numCells)
        return 0.0;

    iteratorToCell(nr);
    return iter->getCellValue();
}

double cKSplit::getBasepoint(int nr) const
{
    if (nr >= numCells)
        return rangeMax;

    iteratorToCell(nr);
    return iter->getCellMin();
}

/////////////////////////////////////////////////////////////////
// random()
//  This random number generator works by generating a random number
//  between 0 and the total number of observations. After that, it walks
//  through the cells, until the total of observations encountered
//  is larger than the random number drawn.
//
//
//  CODE NOT CLEANED UP BY VA YET!
/////////////////////////////////////////////////////////////////

double cKSplit::draw() const
{
    int i;
    // int dp = getTreeDepth();
    int cd = 1;

    cRNG *rng = getRNG();
    double x = intrand(rng, numValues);

    int location = rootGridIndex;

    double xi = rangeMin, xa;

    short finish1 = 0;

    while (finish1 == 0) {
        i = 0;
        short finish2 = 0;

        double sum = 0;
        double hlp, hlp4;

        while (finish2 == 0) {
            Grid lp = gridv[location];

            if (lp.cells[i] >= 0)
                hlp = getRealCellValue(lp, i);
            else
                hlp = lp.cells[i];

            hlp4 = 0;

            if (hlp < 0) {
                int hlp3 = -(int)hlp;
                hlp = gridv[hlp3].total;
                hlp4 = gridv[hlp3].mother;
            }

            if ((sum + hlp + hlp4) > x)
                finish2 = 1;
            else {
                sum += hlp + hlp4;
                i++;
            }
        }  // while (finish2..
        x -= sum;

        xi += i * (rangeMax - rangeMin) / pow((double)K, cd);

        if (gridv[location].cells[i] < 0) {
            location = -(int)gridv[location].cells[i];
            cd++;
        }
        else
            finish1 = 1;
    }  // while (finish1..

    xa = xi + (rangeMax - rangeMin) / pow((double)K, cd);

    x = rand() / (double)RAND_MAX * (xa - xi) + xi;
    return x;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// getPDF()
//  The location of the observation in the tree of cells is found. After that, the pdf
//  value of that location is calculated and returned. When the observation falls outside of
//  root grid, zero is returned.
//  The vars dpth and cdpth are used to calculate the actual pdf value of a possibly divided
//  cell.
//
//  CODE NOT CLEANED UP BY VA YET!
/////////////////////////////////////////////////////////////////////////////////////////////

double cKSplit::getPDF(double x) const
{
    if (!isTransformed())
        return 0;

    int i;

    int dpth = getTreeDepth();
    int cdpth = 1;

    double xi = rangeMin;  // used for computing the boundary values of the current
    double xa = rangeMax;  // location

    int location = rootGridIndex;  // start at the beginning of the gridv
    short finish = 0;

    while (finish == 0) {  // keep searching until the right grid has been found
                           // returns the cellnr, in which the point falls
        i = (int)(K * (x - xi) / (xa - xi));

        if (gridv[location].cells[i] < 0) {
            cdpth++;

            location = -gridv[location].cells[i];
            double hlp = xi;

            xi += i * (xa - hlp) / K;
            xa = xi + (xa - hlp) / K;
        }
        else  // the right grid has been found
            finish = 1;
    }  // while...

    Grid lp = gridv[location];

    return getRealCellValue(lp, i) / pow((double)K, dpth - cdpth);
}

double cKSplit::getCDF(double) const
{
    throw cRuntimeError(this, "getCDF() not implemented");
}

void cKSplit::saveToFile(FILE *f) const
{
    cDensityEstBase::saveToFile(f);

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
            fprintf(f, "%ld\t #= total\n", gridv[i].total);
            fprintf(f, "%d\t #= mother\n", gridv[i].mother);
            if (K == 2)
                fprintf(f, "%d %d\t #= cells[0], cells[1]\n", gridv[i].cells[0], gridv[i].cells[1]);
            else {
                fprintf(f, "#= cells[]\n");
                for (int cell : gridv[i].cells)
                    fprintf(f, " %d\n", cell);
            }
        }
    }
}

void cKSplit::loadFromFile(FILE *f)
{
    cDensityEstBase::loadFromFile(f);

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
            freadvarsf(f, "%ld\t #= total", &gridv[i].total);
            freadvarsf(f, "%d\t #= mother", &gridv[i].mother);
            if (K == 2)
                freadvarsf(f, "%d %d\t #= cells[0], cells[1]", gridv[i].cells + 0, gridv[i].cells+1);
            else {
                freadvarsf(f, "#= cells[]");
                for (int j = 0; j < K; j++)
                    freadvarsf(f, " %d", gridv[i].cells + j);
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

