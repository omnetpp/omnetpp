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

/**
 * FIXME: classes declared
 * K: the grid size of the algorithm
 * cell split criteria
 * cell division criteria
 * 
 * sGrid: struct used internally by cKSplit
 */
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


/**
 * FIXME: 
 * cKSplit
 */
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

    /**
     * FIXME: operations during inserting an observation
     */
    void resetGrids(int grid);

    /**
     * MISSINGDOC: cKSplit:void createRootGrid()
     */
    void createRootGrid();

    /**
     * MISSINGDOC: cKSplit:void newRootGrids(double)
     */
    void newRootGrids(double x);

    /**
     * MISSINGDOC: cKSplit:void insertIntoGrids(double,int)
     */
    void insertIntoGrids(double x, int enable_splits);

    /**
     * MISSINGDOC: cKSplit:void splitCell(int,int)
     */
    void splitCell(int grid, int cell);

    /**
     * MISSINGDOC: cKSplit:void distributeMotherObservations(int)
     */
    void distributeMotherObservations(int grid);

    /**
     * MISSINGDOC: cKSplit:void expandGridVector()
     */
    void expandGridVector();

    // helper for basepoint(), cell():

    /**
     * FIXME: helper for basepoint(), cell():
     */
    void iteratorToCell(int cell_nr);

  public:


    /**
     * MISSINGDOC: cKSplit:cKSplit(cKSplit&)
     */
    cKSplit(cKSplit& r);

    /**
     * MISSINGDOC: cKSplit:cKSplit(char*)
     */
    explicit cKSplit(const char *name=NULL);

    /**
     * MISSINGDOC: cKSplit:~cKSplit()
     */
    virtual ~cKSplit();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cKSplit";}

    /**
     * MISSINGDOC: cKSplit:cObject*dup()
     */
    virtual cObject *dup()   {return new cKSplit (*this);}

    /**
     * MISSINGDOC: cKSplit:cKSplit&operator=(cKSplit&)
     */
    cKSplit& operator=(cKSplit& res);

    /**
     * MISSINGDOC: cKSplit:void writeContents(ostream&)
     */
    virtual void writeContents(ostream& os);

    /**
     * MISSINGDOC: cKSplit:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cKSplit:int netUnpack()
     */
    virtual int netUnpack();

    // redefined cDensityEstBase functions

    /**
     * FIXME: redefined cDensityEstBase functions
     */
    virtual void transform();
  protected:

    /**
     * MISSINGDOC: cKSplit:void collectTransformed(double)
     */
    virtual void collectTransformed(double val);
  public:

    /**
     * MISSINGDOC: cKSplit:int cells()
     */
    virtual int cells();

    /**
     * MISSINGDOC: cKSplit:double basepoint(int)
     */
    virtual double basepoint(int k);

    /**
     * MISSINGDOC: cKSplit:double cell(int)
     */
    virtual double cell(int k);


    /**
     * MISSINGDOC: cKSplit:double pdf(double)
     */
    virtual double pdf(double x);

    /**
     * MISSINGDOC: cKSplit:double cdf(double)
     */
    virtual double cdf(double x);


    /**
     * MISSINGDOC: cKSplit:double random()
     */
    virtual double random();


    /**
     * MISSINGDOC: cKSplit:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *);

    /**
     * MISSINGDOC: cKSplit:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);

    // new functions

    /**
     * FIXME: new functions
     */
    void setCritFunc(KSplitCritFunc _critfunc, double *_critdata);

    /**
     * MISSINGDOC: cKSplit:void setDivFunc(KSplitDivFunc,double*)
     */
    void setDivFunc(KSplitDivFunc _divfunc, double *_divdata);

    /**
     * MISSINGDOC: cKSplit:void rangeExtension(bool)
     */
    void rangeExtension( bool enabled );


    /**
     * MISSINGDOC: cKSplit:int treeDepth()
     */
    int treeDepth();

    /**
     * MISSINGDOC: cKSplit:int treeDepth(sGrid&)
     */
    int treeDepth(sGrid& grid);


    /**
     * MISSINGDOC: cKSplit:double realCellValue(sGrid&,int)
     */
    double realCellValue(sGrid& grid, int cell);

    /**
     * MISSINGDOC: cKSplit:void printGrids()
     */
    void printGrids();


    /**
     * MISSINGDOC: cKSplit:sGrid&grid(int)
     */
    sGrid& grid(int k) {return gridv[k];}

    /**
     * MISSINGDOC: cKSplit:sGrid&rootGrid()
     */
    sGrid& rootGrid()  {return gridv[rootgrid];}
};


/**
 * MISSINGDOC: cKSplitIterator
 */
class SIM_API cKSplitIterator
{
  private:
    cKSplit *ks;             // host object
    int cellnum;             // global index of current cell
    int grid, cell;          // root index in gridv[], cell index in grid.cell[]
    double gridmin;          // left edge of current grid
    double cellsize;         // cell width on current grid

    /**
     * MISSINGDOC: cKSplitIterator:void dive(int)
     */
    void dive(int where);
  public:

    /**
     * MISSINGDOC: cKSplitIterator:cKSplitIterator(cKSplit&,int)
     */
    cKSplitIterator(cKSplit& _ks, int _beg=1);

    /**
     * MISSINGDOC: cKSplitIterator:void init(cKSplit&,int)
     */
    void init(cKSplit& _ks, int _beg=1);

    /**
     * MISSINGDOC: cKSplitIterator:void operator++()
     */
    void operator++(int);

    /**
     * MISSINGDOC: cKSplitIterator:void operator--()
     */
    void operator--(int);

    /**
     * MISSINGDOC: cKSplitIterator:bool end()
     */
    bool end()           {return grid==0;}

    /**
     * MISSINGDOC: cKSplitIterator:int cellNumber()
     */
    int cellNumber()     {return cellnum;}

    /**
     * MISSINGDOC: cKSplitIterator:double cellMin()
     */
    double cellMin()     {return gridmin+cell*cellsize;}

    /**
     * MISSINGDOC: cKSplitIterator:double cellMax()
     */
    double cellMax()     {return gridmin+(cell+1)*cellsize;}

    /**
     * MISSINGDOC: cKSplitIterator:double cellSize()
     */
    double cellSize()    {return cellsize;}

    /**
     * MISSINGDOC: cKSplitIterator:double cellValue()
     */
    double cellValue();
};

#endif
