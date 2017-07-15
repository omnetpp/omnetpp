//==========================================================================
//  CHISTOGRAM.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CHISTOGRAM_H
#define __OMNETPP_CHISTOGRAM_H

#include "cdensityestbase.h"

namespace omnetpp {


/**
 * @brief Base class for histogram classes. It adds a vector of counters to
 * cDensityEstBase.
 *
 * @ingroup Statistics
 */
class SIM_API cHistogramBase : public cDensityEstBase
{
  protected:
    int numCells;     // number of histogram cells or bins
    double *cellv;    // counts, type double because of weighted statistics

  private:
    void copy(const cHistogramBase& other);

  protected:
    // abstract method in cDensityEstBase
    virtual void doMergeCellValues(const cDensityEstBase *other) override;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cHistogramBase(const cHistogramBase& r) : cDensityEstBase(r) {cellv=nullptr;copy(r);}

    /**
     * Constructor.
     */
    cHistogramBase(const char *name, int numcells, bool weighted=false);

    /**
     * Destructor.
     */
    virtual ~cHistogramBase();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cHistogramBase& operator=(const cHistogramBase& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult() override;

    /**
     * Transforms the table of pre-collected values into an internal
     * histogram structure.
     */
    virtual void transform() override;

    /**
     * Returns the number of histogram cells used.
     */
    virtual int getNumCells() const override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *) override;
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


/**
 * @brief Implements an equidistant histogram.
 *
 * The histogram can operate in two modes. In INTEGERS mode, cell boundaries
 * are whole numbers; in DOUBLES mode, they can be real numbers. The operating
 * mode can be chosen with a constructor argument or with the setMode() method;
 * the default behavior is to choose the mode automatically, by inspecting
 * precollected observations.
 *
 * By default, the number of cells is chosen automatically, and the histogram
 * range is determined by precollecting a number of observations and extending
 * their range by a range extension factor.
 *
 * It is possible to explicitly set any of the following values (and the rest
 * will be chosen or computed automatically): number of cells; cell size;
 * number of observations to precollect; range extension factor; range lower
 * bound; range upper bound. See the setNumCells(), setCellSize(),
 * setNumFirstVals(), setRangeAuto(), setRangeAutoUpper(), and
 * setRangeAutoLower() methods (many of them are inherited from
 * cDensityEstBase and cHistogramBase).
 *
 * Especially in INTEGERS mode, if the cells cannot be set up to satisfy all
 * explicitly given constraints (for example, if the explicitly specified
 * range is not an integer multiple of the explicitly specified cell size),
 * an error will be thrown.
 *
 * Informational defaults for the various configuration values (subject to change
 * without notice in any release): number of values to precollect: 100;
 * range extension factor: 2.0; number of cells: 200 in INTEGERS mode and 30 in
 * DOUBLES mode.
 *
 * Examples:
 *
 * Automatic mode:
 *
 * \code
 * cHistogram h("histogram");
 * \endcode
 *
 * Setting up a 50-cell histogram on the range [0.0, 5.0) (cell size = 0.1):
 *
 * \code
 * cHistogram h("histogram", 50, cHistogram::MODE_DOUBLES);
 * h.setRange(0,5);
 * \endcode
 *
 * If you only know that the numbers will be nonnegative but the exact range
 * is unknown:
 *
 * \code
 * cHistogram h("histogram");
 * h.setRangeAutoUpper(0);  // sets zero as lower bound
 * \endcode
 *
 * @ingroup Statistics
 */
class SIM_API cHistogram : public cHistogramBase
{
  public:
    enum HistogramMode {MODE_AUTO, MODE_INTEGERS, MODE_DOUBLES};

  protected:
    HistogramMode mode;
    double cellSize;  // cell (bin) size; <=0 if unset

  private:
    void copy(const cHistogram& other);

  protected:
    virtual void setupRangeInteger();
    virtual void setupRangeDouble();
    virtual void getAttributesToRecord(opp_string_map& attributes) override;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cHistogram(const cHistogram& r) : cHistogramBase(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cHistogram(const char *name=nullptr, int numcells=-1, HistogramMode mode=MODE_AUTO, bool weighted=false);

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cHistogram& operator=(const cHistogram& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

  protected:
    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     */
    virtual void collectTransformed(double value) override;

    /**
     * Called internally by collect2(), this method collects a value
     * after the histogram has been transformed.
     */
    virtual void collectTransformed2(double value, double weight) override;

    /**
     * Called internally by transform(), this method should determine and set up
     * the histogram range. It also calculates the cell size.
     */
    virtual void setupRange() override;

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Returns the kth cell boundary.
     */
    virtual double getBasepoint(int k) const override;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double getCellValue(int k) const override;

    /**
     * Returns the value of the Probability Density Function at a given x.
     */
    virtual double getPDF(double x) const override;

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double getCDF(double x) const override;

    /**
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer or double is returned, depending on
     * mode.
     */
    virtual double draw() const override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *) override;
    //@}

    /** @name Misc. */
    //@{
    /**
     * Sets the histogram mode: MODE_AUTO, MODE_INTEGERS or MODE_DOUBLES.
     * Cannot be called when the cells have been set up already.
     */
    virtual void setMode(HistogramMode mode);

    /**
     * Returns the histogram mode, which is MODE_AUTO, MODE_INTEGERS or
     * MODE_DOUBLES.
     */
    virtual HistogramMode getMode() const {return mode;}

    /**
     * Sets the cell size. Cannot be called when the cells have been
     * set up already.
     */
    virtual void setCellSize(double d);

    /**
     * Returns the cell size, or 0 when unknow (i.e. the cells have not
     * been set up yet, and setCellSize() has not been called).
     */
    virtual double getCellSize() const {return cellSize;}
    //@}

};


/**
 * @brief Equidistant histogram for integers.
 *
 * This class is just a cHistogram preconfigured for collecting integers,
 * that is, it operates in INTEGERS mode.
 *
 * @ingroup Statistics
 */
class SIM_API cLongHistogram : public cHistogram
{
  private:
    void copy(const cLongHistogram& other) {}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Copy constructor.
     */
    cLongHistogram(const cLongHistogram& r) : cHistogram(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cLongHistogram(const char *name=nullptr, int numcells=-1, bool weighted=false) :
        cHistogram(name, numcells, MODE_INTEGERS, weighted) {}

    /**
     * Destructor.
     */
    virtual ~cLongHistogram() {}

    /**
     * Assignment operator.
     */
    cLongHistogram& operator=(const cLongHistogram& other);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cLongHistogram *dup() const override  {return new cLongHistogram(*this);}
    //@}

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Collects one value. Internally, the double value is converted to an
     * integer using floor() before any processing.
     */
    virtual void collect(double value) override {cHistogram::collect(floor(value));}

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) override {collect(value.dbl());}
};


/**
 * @brief Equidistant histogram for doubles.
 *
 * This class is just a cHistogram preconfigured for collecting doubles,
 * that is, it operates in DOUBLES mode.
 *
 * @ingroup Statistics
 */
class SIM_API cDoubleHistogram : public cHistogram
{
  private:
    void copy(const cDoubleHistogram& other) {}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor
     */
    cDoubleHistogram(const cDoubleHistogram& r) : cHistogram(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cDoubleHistogram(const char *name=nullptr, int numcells=-1, bool weighted=false) :
        cHistogram(name, numcells, MODE_DOUBLES, weighted) {}

    /**
     * Destructor.
     */
    virtual ~cDoubleHistogram() {}

    /**
     * Assignment operator.
     */
    cDoubleHistogram& operator=(const cDoubleHistogram& other);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cDoubleHistogram *dup() const override  {return new cDoubleHistogram(*this);}
    //@}
};

}  // namespace omnetpp


#endif

