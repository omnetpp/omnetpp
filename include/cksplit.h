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
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CKSPLIT_H
#define __CKSPLIT_H

#include "cdensity.h"

// classes declared
class cKSplit;
struct sGrid;
class cKSplitIterator;

// K: the grid size of the algorithm
#define K 2


/**
 * Prototype for cell split criterion functions used by cKSplit objects.
 * @ingroup EnumsTypes
 */
typedef int (*KSplitCritFunc)(const cKSplit&, sGrid&, int, double *);

/**
 * Prototype for cell division criterion functions used by cKSplit objects.
 * @ingroup EnumsTypes
 */
typedef double (*KSplitDivFunc)(const cKSplit&, sGrid&, double, double *);

// cell split criteria
int critfunc_const(const cKSplit&, sGrid&, int, double *);
int critfunc_depth(const cKSplit&, sGrid&, int, double *);

// cell division criteria
double divfunc_const(const cKSplit&, sGrid&, double, double *);
double divfunc_babak(const cKSplit&, sGrid&, double, double *);

//==========================================================================

/**
 * Supporting struct for cKSplit. Represents one grid in the k-split
 * data structure.
 */
struct sGrid   // FIXME: to be renamed to sKSplitGrid! make internal class to cKSplit?
{
  int parent;      ///< index of parent grid
  int reldepth;    ///< depth = (reldepth - rootgrid's reldepth)
  long total;      ///< sum of cells & all subgrids (also includes 'mother')
  int mother;      ///< observations 'inherited' from mother cell
  int cells[K];    ///< cell values
};

//==========================================================================

/**
 * Implements k-split, an adaptive histogram-like density estimation
 * algorithm.
 *
 * @ingroup Statistics
 * @see cKSplitIterator, sGrid
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

    mutable cKSplitIterator *iter; // iterator used by basepoint(), cell() etc.
    mutable long iter_num_samples; // num_samples when iterator was created

  protected:
    // internal:
    void resetGrids(int grid);

    // internal:
    void createRootGrid();

    // internal:
    void newRootGrids(double x);

    // internal:
    void insertIntoGrids(double x, int enable_splits);

    // internal:
    void splitCell(int grid, int cell);

    // internal:
    void distributeMotherObservations(int grid);

    // internal:
    void expandGridVector();

    // internal: helper for basepoint(), cell()
    void iteratorToCell(int cell_nr) const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cKSplit(const cKSplit& r);

    /**
     * Constructor.
     */
    explicit cKSplit(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cKSplit();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cKSplit& operator=(const cKSplit& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const   {return new cKSplit (*this);}

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

  protected:
    /**
     * Called internally by collect(), this method updates the k-split
     * data structure with the new value.
     */
    virtual void collectTransformed(double val);

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Transforms the table of pre-collected values into the k-split data structure.
     */
    virtual void transform();

    /**
     * Returns the number of histogram cells used.
     */
    virtual int cells() const;

    /**
     * Returns the kth cell boundary.
     */
    virtual double basepoint(int k) const;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double cell(int k) const;

    /**
     * Returns the value of the Probability Density Function at a given x.
     */
    virtual double pdf(double x) const;

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double cdf(double x) const;

    /**
     * Generates a random number based on the collected data. Uses the random number generator set by setGenK().
     */
    virtual double random() const;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);
    //@}

    /** @name Configuring the k-split algorithm. */
    //@{

    /**
     * Configures the k-split algorithm by supplying a custom split
     * criterion function.
     */
    void setCritFunc(KSplitCritFunc _critfunc, double *_critdata);

    /**
     * Configures the k-split algorithm by supplying a custom cell division
     * function.
     */
    void setDivFunc(KSplitDivFunc _divfunc, double *_divdata);

    /**
     * Enables/disables range extension. If range extension is enabled,
     * a new observation that falls outside the k-split range (ie. outside
     * the root grid) will cause the range to be expanded (i.e. new
     * root grid(s) to be placed above the current root grid).
     * If range extension is disabled, such observations will simply be
     * counted as underflows or overflows.
     */
    void rangeExtension( bool enabled );
    //@}

    /** @name Querying the k-split data structure. */
    //@{

    /**
     * Returns the depth of the k-split tree.
     */
    int treeDepth() const;

    /**
     * Returns the depth of the k-split tree measured from the specified grid.
     */
    int treeDepth(sGrid& grid) const;

    /**
     * Returns the actual amount of observations in cell 'cell' of 'grid'.
     * This is not necessarily an integer value because of previous cell splits.
     */
    double realCellValue(sGrid& grid, int cell) const;

    /**
     * Dumps the contents of the k-split data structure to ev.
     */
    void printGrids() const;

    /**
     * Returns the kth grid in the k-split data structure.
     */
    sGrid& grid(int k) const {return gridv[k];}

    /**
     * Returns the root grid of the k-split data structure.
     */
    sGrid& rootGrid() const {return gridv[rootgrid];}
    //@}
};


/**
 * Walks along cells of the distribution stored in a cKSplit object.
 */
class SIM_API cKSplitIterator
{
  private:
    cKSplit *ks;             // host object
    int cellnum;             // global index of current cell
    int grid, cell;          // root index in gridv[], cell index in grid.cell[]
    double gridmin;          // left edge of current grid
    double cellsize;         // cell width on current grid

    // internal
    void dive(int where);

  public:
    /**
     * Constructor.
     */
    cKSplitIterator(const cKSplit& _ks, int _beg=1);

    /**
     * Reinitializes the iterator.
     */
    void init(const cKSplit& _ks, int _beg=1);

    /**
     * Moves the iterator to the next cell.
     */
    void operator++(int);

    /**
     * Moves the iterator to the previous cell.
     */
    void operator--(int);

    /**
     * Returns true if the iterator has reached either end of the cell sequence.
     */
    bool end() const           {return grid==0;}

    /**
     * Returns the index of the current cell.
     */
    int cellNumber() const     {return cellnum;}

    /**
     * Returns the upper lower of the current cell.
     */
    double cellMin() const     {return gridmin+cell*cellsize;}

    /**
     * Returns the upper bound of the current cell.
     */
    double cellMax() const     {return gridmin+(cell+1)*cellsize;}

    /**
     * Returns the size of the current cell.
     */
    double cellSize() const    {return cellsize;}

    /**
     * Returns the actual amount of observations in current cell.
     * This is not necessarily an integer value because of previous cell splits.
     */
    double cellValue() const;
};

#endif

