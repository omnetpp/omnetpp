//==========================================================================
//
//   CVARHIST.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cVarHistogram : Variable bin size histogram
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CVARHIST_H
#define __CVARHIST_H

#include "chist.h"

//==========================================================================
//  transform types for cVarHistogram
//
// HIST_TR_NO_TRANSFORM: no transformation; uses bin boundaries
//                       previously defined by addBinBound()/appendBinBound()
// HIST_TR_AUTO_EPC_DBL: automatically creates equiprobable cells
// HIST_TR_AUTO_EPC_INT: like the above, but uses a different hack :-)

enum {
   HIST_TR_NO_TRANSFORM=1,
   HIST_TR_AUTO_EPC_DBL=0,
   HIST_TR_AUTO_EPC_INT=2
};

//==========================================================================

/**
 * Variable bin size histogram. You may add cell (bin) boundaries
 * manually, or .let the object create cells with equal number of
 * observations in them (or as close to that as possible).
 *
 * Transform types for cVarHistogram:
 * <UL>
 *   <LI> HIST_TR_NO_TRANSFORM: no transformation; uses bin boundaries
 *        previously defined by addBinBound()/appendBinBound()
 *   <LI> HIST_TR_AUTO_EPC_DBL: automatically creates equiprobable cells
 *   <LI> HIST_TR_AUTO_EPC_INT: like the above, but uses a different hack :-)
 * </Ul>
 *
 * Layout of the variable bin width histogram:
 * <PRE><TT>
 *
 *        underflow-cell   ordinary cells . . .             overflow-cell
 *        ...---------|-----------|- ... -|-----------------|---------...
 *                    | ^cellv[0] |      cellv[num_cells-1]^|
 *                  n |           |                         |
 *                rangemin        |                      rangemax
 *                    |    bin_bounds[1]                    |
 *              bin_bounds[0]                       bin_bounds[numcells]
 * </TT></PRE>
 *
 * Rangemin and rangemax is chosen after collecting the num_firstvals initial
 * observations. It is not possible to add cell boundaries when histogram is
 * already transformed.
 *
 * Now we do support the following 2 uses of cVarHistogram:
 * <OL>
 *   <LI> add all the boundaries (manually) before collecting samples
 *   <LI> collect samples and transform() makes the boundaries
 * </OL>
 *
 * @ingroup Statistics
 */
class SIM_API cVarHistogram : public cHistogramBase //--LG
{
  protected:
    int transform_type;     // one of the HIST_TR_xxx constants
    int max_num_cells;      // the length of the allocated cellv
    double *bin_bounds;     // bin/cell boundaries

    // the boundaries of the ordinary cells/bins are:
    // rangemin=bin_bounds[0], bin_bounds[1], ... bin_bounds[num_cells]=rangemax
    // consequence: sizeof(binbounds)=sizeof(cellv)+1

  protected:
    /**
     * FIXME:
     */
    void createEquiProbableCells();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cVarHistogram(const cVarHistogram& r) : cHistogramBase(r)
       {setName(r.name());bin_bounds=NULL;operator=(r);}

    /**
     * Constructor. The third argument can be one of HIST_TR_NO_TRANSFORM,
     * HIST_TR_AUTO_EPC_DBL, HIST_TR_AUTO_EPC_INT.
     */
    explicit cVarHistogram(const char *name=NULL,
                           int numcells=11,
                           int transformtype=HIST_TR_AUTO_EPC_DBL);

    /**
     * Destructor.
     */
    virtual ~cVarHistogram();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cVarHistogram& operator=(const cVarHistogram& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const    {return new cVarHistogram(*this);}

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Transforms the table of precollected values into an internal
     * histogram structure.
     */
    virtual void transform();

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     */
    virtual void collectTransformed(double val);

    /**
     * Generates a random number based on the collected data. Uses the random number generator set by setGenK().
     */
    virtual double random() const;

    /**
     * Returns the value of the Probability Density Function at a given x.
     */
    virtual double pdf(double x) const; // --LG

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double cdf(double x) const; // --LG

    /**
     * Returns the kth cell boundary.
     */
    virtual double basepoint(int k) const; // --LG

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double cell(int k) const;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const; //--LG

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);  //--LG
    //@}

    /** @name Setting up the histogram. */
    //@{

    /**
     * FIXME: Adds a new bin boundary (keeping the right order).
     * If HIST_TR_NO_TRANSFORM was passed in the constructor
     * call, you may specify cell (bin) bounds manually before collection
     * starts.
     */
    virtual void addBinBound(double x);
    //@}
};

#endif


