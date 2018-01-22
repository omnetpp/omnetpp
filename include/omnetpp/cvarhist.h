//==========================================================================
//  CVARHIST.H - part of
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

#ifndef __OMNETPP_CVARHIST_H
#define __OMNETPP_CVARHIST_H

#include "clegacyhistogram.h"

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace omnetpp {

/**
 * @brief Variable bin size histogram.
 *
 * One may add bin (bin) boundaries manually, or let the object create
 * bins with approximately equal number of observations in them.
 *
 * The histogram range (rangemin, rangemax) is chosen after precollection
 * of the initial observations has finished. It is not possible to add
 * bins after the histogram has already been set up.
 *
 * @ingroup Statistics
 */
class _OPPDEPRECATED SIM_API cVarHistogram : public cLegacyHistogramBase
{
  public:
    /**
     * Controls the way histogram bins are set up at the end of the precollection period.
     */
    enum TransformType {
       HIST_TR_NO_TRANSFORM, ///< no transformation; uses bin boundaries previously defined by addBinBound()/appendBinBound()
       HIST_TR_AUTO_EPC_DBL, ///< automatically creates equi-probable bins
       HIST_TR_AUTO_EPC_INT  ///< a variation of HIST_TR_AUTO_EPC_DBL
    };

  protected:
    TransformType transformType;

    // bin lower boundaries, plus an extra one for the upper bound of the last bin
    // Note: cellLowerBounds[0] = rangeMin, and cellLowerBounds[numCells] = rangeMax
    double *cellLowerBounds;
    int maxNumCells;      // allocated size of cellLowerBounds[] during setup

  private:
    void copy(const cVarHistogram& other);

  protected:
    /**
     * Used internally to create equiprobable bins from the precollected
     * observations. This cannot be mixed with manually adding bin boundaries
     * -- if there are already some, an error is raised.
     */
    void createEquiprobableCells();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cVarHistogram(const cVarHistogram& r) : cLegacyHistogramBase(r) {cellLowerBounds=nullptr;copy(r);}

    /**
     * Constructor. The third argument can be one of HIST_TR_NO_TRANSFORM,
     * HIST_TR_AUTO_EPC_DBL, HIST_TR_AUTO_EPC_INT. With HIST_TR_NO_TRANSFORM,
     * you can set up bins manually (see addBinBound()), in the other two
     * cases it tries to create equiprobably bins.
     */
    explicit cVarHistogram(const char *name=nullptr,
                           int numcells=11,
                           TransformType transformtype=HIST_TR_AUTO_EPC_DBL);

    /**
     * Destructor.
     */
    virtual ~cVarHistogram();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cVarHistogram& operator=(const cVarHistogram& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cVarHistogram *dup() const override  {return new cVarHistogram(*this);}

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
     * Sets up the bins using the precollected observations.
     */
    virtual void setUpBins() override;

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram bins have been set up.
     */
    virtual void collectIntoHistogram(double value) override;

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram bins have been set up.
     */
    virtual void collectWeightedIntoHistogram(double value, double weight) override;

    /**
     * Generates a random number based on the collected data.
     */
    virtual double draw() const override;

    /**
     * Returns the value of the Probability Density Function at a given x.
     */
    virtual double getPDF(double x) const override;

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double getCDF(double x) const override;

    /**
     * Returns the kth bin boundary.
     */
    virtual double getBinEdge(int k) const override;

    /**
     * Returns the number of observations that fell into the kth histogram bin.
     */
    virtual double getBinValue(int k) const override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *) override;
    //@}

    /** @name Setting up the histogram. */
    //@{

    /**
     * Adds a new bin (bin) boundary. This method can only be called
     * if HIST_TR_NO_TRANSFORM was specified in the constructor call,
     * and only when the object is still in the initial data collection phase
     * (that is, setUpBins() has been invoked yet).
     */
    virtual void addBinBound(double x);
    //@}
};

}  // namespace omnetpp

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif


