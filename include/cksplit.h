//==========================================================================
//   CKSPLIT.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cKSplit : implements the k-split algorithm in 1 dimension
//
//  Written by Babak Fakhamzadeh, TU Delft, Mar-Jun 1996
//  Rewritten by Andras Varga
//
//==========================================================================
/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CKSPLIT_H
#define __CKSPLIT_H

#include <iostream.h>
#include "cdensity.h"

// classes declared
class cKSplit;
struct sGrid;
class cKSplitIterator;

// K: the grid size of the algorithm
#define K 2

typedef int (*KSplitCritFunc)(cKSplit&, sGrid&, int, double *);
typedef double (*KSplitDivFunc)(cKSplit&, sGrid&, double, double *);

// cell split criteria
int critfunc_const(cKSplit&, sGrid&, int, double *);
int critfunc_depth(cKSplit&, sGrid&, int, double *);

// cell division criteria
double divfunc_const(cKSplit&, sGrid&, double, double *);
double divfunc_babak(cKSplit&, sGrid&, double, double *);

//==========================================================================
// sGrid: struct used internally by cKSplit
struct sGrid
{
  int parent;      // index of parent grid
  int reldepth;    // depth = (reldepth - rootgrid's reldepth)
  long total;      // sum of cells & all subgrids (also includes 'mother')
  int mother;      // observations 'inherited' from mother cell
  int cells[K];    // cell values
};

//==========================================================================
// cKSplit

class SIM_API cKSplit : public cDensityEstBase
{
    friend class cKSplitIterator;

  protected:
    int num_cells;            // number of cells

    sGrid *gridv;             // grid vector
    int gridv_size;           // size of gridv[]+1
    int rootgrid, lastgrid;   // indices into gridv[]
    bool rangeext_enabled;    // enable/disable range extension

    KSplitCritFunc critfunc;  // function that determines when to split a cell
    double *critdata;         // data array to pass to crit. function

    KSplitDivFunc divfunc;    // function to calc. lambda for cell division
    double *divdata;          // data array to pass to div. function

    cKSplitIterator *iter;    // iterator used by basepoint(), cell() etc.
    long iter_num_samples;    // num_samples when iterator was created

  protected:
    // operations during inserting an observation
    void resetGrids(int grid);
    void createRootGrid();
    void newRootGrids(double x);
    void insertIntoGrids(double x, int enable_splits);
    void splitCell(int grid, int cell);
    void distributeMotherObservations(int grid);
    void expandGridVector();

    // helper for basepoint(), cell():
    void iteratorToCell(int cell_nr);

  public:

    cKSplit(cKSplit& r);
    explicit cKSplit(const char *name=NULL);
    virtual ~cKSplit();

    // redefined functions
    virtual const char *className() const {return "cKSplit";}
    virtual cObject *dup()   {return new cKSplit (*this);}
    cKSplit& operator=(cKSplit& res);
    virtual void writeContents(ostream& os);
    virtual int netPack();
    virtual int netUnpack();

    // redefined cDensityEstBase functions
    virtual void transform();
  protected:
    virtual void collectTransformed(double val);
  public:
    virtual int cells();
    virtual double basepoint(int k);
    virtual double cell(int k);

    virtual double pdf(double x);
    virtual double cdf(double x);

    virtual double random();

    virtual void saveToFile(FILE *);
    virtual void loadFromFile(FILE *);

    // new functions
    void setCritFunc(KSplitCritFunc _critfunc, double *_critdata);
    void setDivFunc(KSplitDivFunc _divfunc, double *_divdata);
    void rangeExtension( bool enabled );

    int treeDepth();
    int treeDepth(sGrid& grid);

    double realCellValue(sGrid& grid, int cell);
    void printGrids();

    sGrid& grid(int k) {return gridv[k];}
    sGrid& rootGrid()  {return gridv[rootgrid];}
};

class SIM_API cKSplitIterator
{
  private:
    cKSplit *ks;             // host object
    int cellnum;             // global index of current cell
    int grid, cell;          // root index in gridv[], cell index in grid.cell[]
    double gridmin;          // left edge of current grid
    double cellsize;         // cell width on current grid
    void dive(int where);
  public:
    cKSplitIterator(cKSplit& _ks, int _beg=1);
    void init(cKSplit& _ks, int _beg=1);
    void operator++(int);
    void operator--(int);
    bool end()           {return grid==0;}
    int cellNumber()     {return cellnum;}
    double cellMin()     {return gridmin+cell*cellsize;}
    double cellMax()     {return gridmin+(cell+1)*cellsize;}
    double cellSize()    {return cellsize;}
    double cellValue();
};

#endif

