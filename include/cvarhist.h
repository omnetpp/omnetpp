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

#ifndef __CVARHIST_H
#define __CVARHIST_H

#include <iostream.h>
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
     * FIXME: the boundaries of the ordinary cells/bins are:
     * rangemin=bin_bounds[0], bin_bounds[1], ... bin_bounds[num_cells]=rangemax
     * consequence: sizeof(binbounds)=sizeof(cellv)+1
     */
    void createEquiProbableCells();

  public:

    /**
     * Copy constructor.
     */
    cVarHistogram(cVarHistogram& r) : cHistogramBase(r)
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

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cVarHistogram";}

    /**
     * MISSINGDOC: cVarHistogram:cObject*dup()
     */
    virtual cObject *dup()    {return new cVarHistogram(*this);}

    /**
     * MISSINGDOC: cVarHistogram:cVarHistogram&operator=(cVarHistogram&)
     */
    cVarHistogram& operator=(cVarHistogram& res);

    /**
     * MISSINGDOC: cVarHistogram:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cVarHistogram:int netUnpack()
     */
    virtual int netUnpack();

    // new functions:

    /**
     * If HIST_TR_NO_TRANSFORM was passed in the constructor
     * call, you may specify cell (bin) bounds manually before collection
     * starts.
     */
    virtual void addBinBound(double x); // adds a new bin boundary (keeping the right order)

    // redefined cHistogramBase functions

    /**
     * FIXME: redefined cHistogramBase functions
     */
    virtual void clearResult();

    /**
     * MISSINGDOC: cVarHistogram:void transform()
     */
    virtual void transform();

    /**
     * MISSINGDOC: cVarHistogram:void collectTransformed(double)
     */
    virtual void collectTransformed(double val);

    /**
     * MISSINGDOC: cVarHistogram:double random()
     */
    virtual double random();

    /**
     * MISSINGDOC: cVarHistogram:double pdf(double)
     */
    virtual double pdf(double x); // --LG

    /**
     * MISSINGDOC: cVarHistogram:double cdf(double)
     */
    virtual double cdf(double x); // --LG

    /**
     * MISSINGDOC: cVarHistogram:double basepoint(int)
     */
    virtual double basepoint(int k); // --LG

    /**
     * MISSINGDOC: cVarHistogram:double cell(int)
     */
    virtual double cell(int k);

    /**
     * MISSINGDOC: cVarHistogram:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *); //--LG

    /**
     * MISSINGDOC: cVarHistogram:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);  //--LG

};

#endif


