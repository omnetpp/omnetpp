//==========================================================================
//
//   CHIST.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cHistogramBase        : common base class for histogram classes
//      cEqdHistogramBase   : Equi-distant histograms
//        cLongHistogram    : long int distribution
//        cDoubleHistogram  : double distribution
//
//==========================================================================

#ifndef __CHIST_H
#define __CHIST_H

#include <iostream.h>
#include "cdensity.h"

//==========================================================================

/**
 * Base class for histogram classes. It adds a vector of counters to
 * cDensityEstBase.
 */
class SIM_API cHistogramBase : public cDensityEstBase
{
  protected:
    int num_cells;        // nr. of categories
    unsigned *cellv;      // array of counters

  public:
    /**
     * Copy constructor.
     */
    cHistogramBase(cHistogramBase& r) : cDensityEstBase(r)
        {setName(r.name());cellv=NULL;operator=(r);}

    /**
     * Constructor.
     */
    cHistogramBase(const char *name, int numcells);

    /**
     * Destructor.
     */
    virtual ~cHistogramBase();

    // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cHistogramBase".
     */
    virtual const char *className() const {return "cHistogramBase";}

    /**
     * MISSINGDOC: cHistogramBase:cHistogramBase&operator=(cHistogramBase&)
     */
    cHistogramBase& operator=(cHistogramBase& res);

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
    // dup() and inspector(..) not needed

    // redefined cDensityEstBase functions

    /**
     * FIXME: dup() and inspector(..) not needed
     * redefined cDensityEstBase functions
     */
    virtual void clearResult();

    /**
     * MISSINGDOC: cHistogramBase:void transform()
     */
    virtual void transform();

    /**
     * MISSINGDOC: cHistogramBase:int cells()
     */
    virtual int cells();

    /**
     * MISSINGDOC: cHistogramBase:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *); //--LG

    /**
     * MISSINGDOC: cHistogramBase:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);  //--LG
};

//==========================================================================

/**
 * Base class for equal cell size histograms.
 */
class SIM_API cEqdHistogramBase : public cHistogramBase //--LG
{
  protected:
    double cellsize;            // cell/category  sizes

  public:

    /**
     * Copy constructor.
     */
    cEqdHistogramBase(cEqdHistogramBase& r) : cHistogramBase(r)
        {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cEqdHistogramBase(const char *name=NULL, int numcells=10);

    // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cEqdHistogramBase".
     */
    virtual const char *className() const {return "cEqdHistogramBase";}

    /**
     * MISSINGDOC: cEqdHistogramBase:cEqdHistogramBase&operator=(cEqdHistogramBase&)
     */
    cEqdHistogramBase& operator=(cEqdHistogramBase& res);

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
    // dup() not needed

    // redefined cDensityEstBase functions
  protected:

    /**
     * FIXME: dup() not needed
     * redefined cDensityEstBase functions
     */
    virtual void collectTransformed(double val);

    /**
     * MISSINGDOC: cEqdHistogramBase:void setupRange()
     */
    virtual void setupRange(); // also calculates cellsize
  public:

    /**
     * MISSINGDOC: cEqdHistogramBase:double basepoint(int)
     */
    virtual double basepoint(int k);

    /**
     * MISSINGDOC: cEqdHistogramBase:double cell(int)
     */
    virtual double cell(int k);


    /**
     * MISSINGDOC: cEqdHistogramBase:double pdf(double)
     */
    virtual double pdf(double x); // --LG

    /**
     * MISSINGDOC: cEqdHistogramBase:double cdf(double)
     */
    virtual double cdf(double x); // --LG


    /**
     * MISSINGDOC: cEqdHistogramBase:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *); //--LG

    /**
     * MISSINGDOC: cEqdHistogramBase:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);  //--LG
};

//==========================================================================

/**
 * Equidistant histogram for integers. cLongHistogram is derived from
 * cEqdHistogramBase which contains most of the functionality.
 *
 * The histogram is set up in the following way:
 * <UL>
 *   <LI> the cell size is always integer: 1, 2, 3 etc.
 *   <LI> <I>rangemin</I>, <I>rangemax</I> and the cell boundaries are
 *        at halves. For example 1.5, 10.5. This is done so to prevent
 *        misunderstandings.
 *   <LI> the number of cells is exactly <I>num_cells</I>
 *   <LI> the <I>range_ext_factor</I> is also kept. The actual histogram
 *        range will be: (<I>min</I>, <I>max</I>) extended
 *        <I>range_ext_factor</I> times and rounded up to the nearest integer
 *        multiple of <I>num_cells</I>.
 * </UL>
 */
class SIM_API cLongHistogram : public cEqdHistogramBase
{
  public:

    /**
     * Copy constructor.
     */
    cLongHistogram(cLongHistogram& r) : cEqdHistogramBase(r)
        {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cLongHistogram(const char *name=NULL, int numcells=10);

    /**
     * Destructor.
     */
    virtual ~cLongHistogram();

    // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cLongHistogram".
     */
    virtual const char *className() const {return "cLongHistogram";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup()    {return new cLongHistogram(*this);}

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
    // base class' op=() is OK

    // redefined cEqdHistogramBase functions
  protected:

    /**
     * FIXME: base class' op=() is OK
     * redefined cEqdHistogramBase functions
     */
    virtual void setupRange();
  public:

    /**
     * MISSINGDOC: cLongHistogram:void collect(double)
     */
    virtual void collect(double val);  // truncates + collects

    /**
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer is returned.
     */
    virtual double random();
};

//==========================================================================

/**
 * Equidistant histogram for doubles. cDoubleHistogram is derived from
 * cEqdHistogramBase which contains most of the functionality.
 */
class SIM_API cDoubleHistogram : public cEqdHistogramBase
{
  public:

    /**
     * Copy constructor
     */
    cDoubleHistogram(cDoubleHistogram& r) : cEqdHistogramBase(r)
          {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cDoubleHistogram(const char *name=NULL, int numcells=10);

    /**
     * Destructor.
     */
    virtual ~cDoubleHistogram();

    // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cDoubleHistogram".
     */
    virtual const char *className() const  {return "cDoubleHistogram";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup()  {return new cDoubleHistogram(*this);}

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
    // base class' op=() is OK

    // redefined cHistogramBase functions

    /**
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer is returned.
     */
    virtual double random();
};

#endif

