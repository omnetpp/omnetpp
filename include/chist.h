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

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CHIST_H
#define __CHIST_H

#include "cdensity.h"

//==========================================================================

/**
 * Base class for histogram classes. It adds a vector of counters to
 * cDensityEstBase.
 *
 * @ingroup Statistics
 */
class SIM_API cHistogramBase : public cDensityEstBase
{
  protected:
    int num_cells;        // nr. of categories
    unsigned *cellv;      // array of counters

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cHistogramBase(const cHistogramBase& r) : cDensityEstBase(r)
        {setName(r.name());cellv=NULL;operator=(r);}

    /**
     * Constructor.
     */
    cHistogramBase(const char *name, int numcells);

    /**
     * Destructor.
     */
    virtual ~cHistogramBase();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cHistogramBase& operator=(const cHistogramBase& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

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
     * Transforms the table of pre-collected values into an internal
     * histogram structure.
     */
    virtual void transform();

    /**
     * Returns the number of histogram cells used.
     */
    virtual int cells() const;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const; //--LG

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);  //--LG
    //@}

    /** @name New member functions. */
    //@{

    /**
     * Sets the number of cells. Cannot be called when the cells have been
     * set up already.
     */
    virtual void setNumCells(int numcells);
    //@}
};

//==========================================================================

/**
 * Base class for equal cell size histograms.
 *
 * @ingroup Statistics
 */
class SIM_API cEqdHistogramBase : public cHistogramBase //--LG
{
  protected:
    double cellsize;            // cell/category  sizes

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cEqdHistogramBase(const cEqdHistogramBase& r) : cHistogramBase(r)
        {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cEqdHistogramBase(const char *name=NULL, int numcells=10);

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cEqdHistogramBase& operator=(const cEqdHistogramBase& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

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

  protected:
    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     */
    virtual void collectTransformed(double val);

    /**
     * Called internally by transform(), this method should determine and set up
     * the histogram range. It also calculates the cell size.
     */
    virtual void setupRange();

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

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
    virtual double pdf(double x) const; // --LG

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double cdf(double x) const; // --LG

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const; //--LG

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);  //--LG
    //@}
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
 *
 * @ingroup Statistics
 */
class SIM_API cLongHistogram : public cEqdHistogramBase
{
  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cLongHistogram(const cLongHistogram& r) : cEqdHistogramBase(r)
        {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cLongHistogram(const char *name=NULL, int numcells=10);

    /**
     * Destructor.
     */
    virtual ~cLongHistogram();

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cLongHistogram& operator=(const cLongHistogram&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const    {return new cLongHistogram(*this);}

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

  protected:
    /**
     * Called internally by transform(), this method should determine and set up
     * the histogram range
     */
    virtual void setupRange();

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Collects one value. FIXME: truncates + collects
     */
    virtual void collect(double val);

    /**
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer is returned.
     */
    virtual double random() const;
    //@}
};

//==========================================================================

/**
 * Equidistant histogram for doubles. cDoubleHistogram is derived from
 * cEqdHistogramBase which contains most of the functionality.
 *
 * @ingroup Statistics
 */
class SIM_API cDoubleHistogram : public cEqdHistogramBase
{
  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor
     */
    cDoubleHistogram(const cDoubleHistogram& r) : cEqdHistogramBase(r)
          {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cDoubleHistogram(const char *name=NULL, int numcells=10);

    /**
     * Destructor.
     */
    virtual ~cDoubleHistogram();

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cDoubleHistogram& operator=(const cDoubleHistogram&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cDoubleHistogram(*this);}

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
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer is returned.
     */
    virtual double random() const;
    //@}
};

#endif

