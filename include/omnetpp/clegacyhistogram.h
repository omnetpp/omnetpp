//==========================================================================
//  CLEGACYHISTOGRAM.H - part of
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

#ifndef __OMNETPP_CLEGACYHISTOGRAM_H
#define __OMNETPP_CLEGACYHISTOGRAM_H

#include <cmath>
#include "cprecolldensityest.h"

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace omnetpp {

/**
 * @brief Base class for histogram classes. It adds a vector of counters to
 * cPrecollectionBasedDensityEst.
 *
 * DEPRECATED CLASS, do not use for new classes.
 *
 * @ingroup Statistics
 */
class _OPPDEPRECATED SIM_API cLegacyHistogramBase : public cPrecollectionBasedDensityEst
{
  protected:
    int numCells;     // number of histogram cells or bins
    double *cellv;    // counts, type double because of weighted statistics

  private:
    void copy(const cLegacyHistogramBase& other);

  protected:
    // abstract method in cPrecollectionBasedDensityEst
    virtual void doMergeBinValues(const cPrecollectionBasedDensityEst *other) override;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cLegacyHistogramBase(const cLegacyHistogramBase& r) : cPrecollectionBasedDensityEst(r) {cellv=nullptr;copy(r);}

    /**
     * Constructor.
     */
    cLegacyHistogramBase(const char *name, int numcells, bool weighted=false);

    /**
     * Destructor.
     */
    virtual ~cLegacyHistogramBase();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cLegacyHistogramBase& operator=(const cLegacyHistogramBase& res);
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
    virtual void clear() override;

    /**
     * Transforms the table of pre-collected values into an internal
     * histogram structure.
     */
    virtual void setUpBins() override;

    /**
     * Returns the number of histogram bins used.
     */
    virtual int getNumBins() const override;

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
     * Sets the number of bins. Cannot be called when the bins have been
     * set up already.
     */
    virtual void setNumCells(int numcells);
    //@}
};


/**
 * @brief Implements an equidistant histogram.
 *
 * DEPRECATED CLASS, use cHistogram instead.
 *
 * The histogram can operate in two modes. In INTEGERS mode, bin boundaries
 * are whole numbers; in DOUBLES mode, they can be real numbers. The operating
 * mode can be chosen with a constructor argument or with the setMode() method;
 * the default behavior is to choose the mode automatically, by inspecting
 * precollected observations.
 *
 * By default, the number of bins is chosen automatically, and the histogram
 * range is determined by precollecting a number of observations and extending
 * their range by a range extension factor.
 *
 * It is possible to explicitly set any of the following values (and the rest
 * will be chosen or computed automatically): number of bins; bin size;
 * number of observations to precollect; range extension factor; range lower
 * bound; range upper bound. See the setNumCells(), setCellSize(),
 * setNumFirstVals(), setRangeAuto(), setRangeAutoUpper(), and
 * setRangeAutoLower() methods (many of them are inherited from
 * cPrecollectionBasedDensityEst and cLegacyHistogramBase).
 *
 * Especially in INTEGERS mode, if the bins cannot be set up to satisfy all
 * explicitly given constraints (for example, if the explicitly specified
 * range is not an integer multiple of the explicitly specified bin size),
 * an error will be thrown.
 *
 * Informational defaults for the various configuration values (subject to change
 * without notice in any release): number of values to precollect: 100;
 * range extension factor: 2.0; number of bins: 200 in INTEGERS mode and 30 in
 * DOUBLES mode.
 *
 * Examples:
 *
 * Automatic mode:
 *
 * \code
 * cLegacyHistogram h("histogram");
 * \endcode
 *
 * Setting up a 50-bin histogram on the range [0.0, 5.0) (bin size = 0.1):
 *
 * \code
 * cLegacyHistogram h("histogram", 50, cLegacyHistogram::MODE_DOUBLES);
 * h.setRange(0,5);
 * \endcode
 *
 * If you only know that the numbers will be nonnegative but the exact range
 * is unknown:
 *
 * \code
 * cLegacyHistogram h("histogram");
 * h.setRangeAutoUpper(0);  // sets zero as lower bound
 * \endcode
 *
 * @ingroup Statistics
 */
class _OPPDEPRECATED SIM_API cLegacyHistogram : public cLegacyHistogramBase
{
  public:
    enum Mode {MODE_AUTO, MODE_INTEGERS, MODE_DOUBLES};
    typedef Mode HistogramMode;

  protected:
    Mode mode;
    double cellSize;  // cell (bin) size; <=0 if unset

  private:
    void copy(const cLegacyHistogram& other);

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
    cLegacyHistogram(const cLegacyHistogram& r) : cLegacyHistogramBase(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cLegacyHistogram(const char *name=nullptr, int numcells=-1, Mode mode=MODE_AUTO, bool weighted=false);

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cLegacyHistogram& operator=(const cLegacyHistogram& res);
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
     * after histogram bins have been set up.
     */
    virtual void collectIntoHistogram(double value) override;

    /**
     * Called internally by collect2(), this method collects a value
     * after histogram bins have been set up.
     */
    virtual void collectWeightedIntoHistogram(double value, double weight) override;

    /**
     * Called internally by setUpBins(), this method should determine and set up
     * the histogram range. It also calculates the bin size.
     */
    virtual void setupRange() override;

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Returns the kth bin boundary.
     */
    virtual double getBinEdge(int k) const override;

    /**
     * Returns the number of observations that fell into the kth histogram bin.
     */
    virtual double getBinValue(int k) const override;

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
     * Cannot be called when the bins have been set up already.
     */
    virtual void setMode(Mode mode);

    /**
     * Returns the histogram mode, which is MODE_AUTO, MODE_INTEGERS or
     * MODE_DOUBLES.
     */
    virtual Mode getMode() const {return mode;}

    /**
     * Sets the bin size. Cannot be called when the bins have been
     * set up already.
     */
    virtual void setCellSize(double d);

    /**
     * Returns the bin size, or 0 when unknow (i.e. the bins have not
     * been set up yet, and setCellSize() has not been called).
     */
    virtual double getCellSize() const {return cellSize;}
    //@}

};


/**
 * @brief Equidistant histogram for integers.
 *
 * DEPRECATED CLASS, use cHistogram instead.
 *
 * This class is just a cLegacyHistogram preconfigured for collecting integers,
 * that is, it operates in INTEGERS mode.
 *
 * @ingroup Statistics
 */
class _OPPDEPRECATED SIM_API cLongHistogram : public cLegacyHistogram
{
  private:
    void copy(const cLongHistogram& other) {}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Copy constructor.
     */
    cLongHistogram(const cLongHistogram& r) : cLegacyHistogram(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cLongHistogram(const char *name=nullptr, int numcells=-1, bool weighted=false) :
        cLegacyHistogram(name, numcells, MODE_INTEGERS, weighted) {}

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
    virtual void collect(double value) override {cLegacyHistogram::collect(std::floor(value));}

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) override {collect(value.dbl());}
};


/**
 * @brief Equidistant histogram for doubles.
 *
 * DEPRECATED CLASS, use cHistogram instead.
 *
 * This class is just a cLegacyHistogram preconfigured for collecting doubles,
 * that is, it operates in DOUBLES mode.
 *
 * @ingroup Statistics
 */
class _OPPDEPRECATED SIM_API cDoubleHistogram : public cLegacyHistogram
{
  private:
    void copy(const cDoubleHistogram& other) {}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor
     */
    cDoubleHistogram(const cDoubleHistogram& r) : cLegacyHistogram(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cDoubleHistogram(const char *name=nullptr, int numcells=-1, bool weighted=false) :
        cLegacyHistogram(name, numcells, MODE_DOUBLES, weighted) {}

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

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif

