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
// cVarHistogram
//
// layout of the variable bin width histogram:
//
//        underflow-cell   ordinary cells . . .             overflow-cell
//        ...---------|-----------|- ... -|-----------------|---------...
//                    | ^cellv[0] |      cellv[num_cells-1]^|
//                  n |           |                         |
//                rangemin        |                      rangemax
//                    |    bin_bounds[1]                    |
//              bin_bounds[0]                       bin_bounds[numcells]
//
// Rangemin and rangemax is chosen after collecting the num_firstvals initial
// observations
// Cannot add cell boundaries when histogram is already transformed!
// Now we do support the following 2 uses of cVarHistogram:
//   1: add all the boundaries (manually) before collecting samples
//   2: collect samples and transform() makes the boundaries
//

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
    void createEquiProbableCells();

  public:
    cVarHistogram(cVarHistogram& r) : cHistogramBase(r)
       {setName(r.name());bin_bounds=NULL;operator=(r);}
    explicit cVarHistogram(const char *name=NULL,
                           int numcells=11,
                           int transformtype=HIST_TR_AUTO_EPC_DBL);
    virtual ~cVarHistogram();

    // redefined functions
    virtual const char *className() const {return "cVarHistogram";}
    virtual cObject *dup()    {return new cVarHistogram(*this);}
    cVarHistogram& operator=(cVarHistogram& res);
    virtual int netPack();
    virtual int netUnpack();

    // new functions:
    virtual void addBinBound(double x); // adds a new bin boundary (keeping the right order)

    // redefined cHistogramBase functions
    virtual void clearResult();

    virtual void transform();
    virtual void collectTransformed(double val);

    virtual double random();
    virtual double pdf(double x); // --LG
    virtual double cdf(double x); // --LG

    virtual double basepoint(int k); // --LG
    virtual double cell(int k);

    virtual void saveToFile(FILE *); //--LG
    virtual void loadFromFile(FILE *);  //--LG

};

#endif

