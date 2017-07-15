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

#include "chistogram.h"

namespace omnetpp {


/**
 * @brief Variable bin size histogram.
 *
 * One may add cell (bin) boundaries manually, or let the object create
 * cells with approximately equal number of observations in them.
 *
 * The histogram range (rangemin, rangemax) is chosen after precollection
 * of the initial observations has finished. It is not possible to add
 * cell boundaries when histogram is already transformed.
 *
 * @ingroup Statistics
 */
class SIM_API cVarHistogram : public cHistogramBase
{
  public:
    /**
     * Controls the way histogram bins are set up at the end of the precollection period.
     */
    enum TransformType {
       HIST_TR_NO_TRANSFORM, ///< no transformation; uses bin boundaries previously defined by addBinBound()/appendBinBound()
       HIST_TR_AUTO_EPC_DBL, ///< automatically creates equi-probable cells
       HIST_TR_AUTO_EPC_INT  ///< a variation of HIST_TR_AUTO_EPC_DBL
    };

  protected:
    TransformType transformType;

    // cell lower boundaries, plus an extra one for the upper bound of the last cell
    // Note: cellLowerBounds[0] = rangeMin, and cellLowerBounds[numCells] = rangeMax
    double *cellLowerBounds;
    int maxNumCells;      // allocated size of cellLowerBounds[] during setup

  private:
    void copy(const cVarHistogram& other);

  protected:
    /**
     * Used internally to create equiprobable cells from the precollected
     * observations. This cannot be mixed with manually adding cell boundaries
     * -- if there are already some, an error is raised.
     */
    void createEquiprobableCells();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cVarHistogram(const cVarHistogram& r) : cHistogramBase(r) {cellLowerBounds=nullptr;copy(r);}

    /**
     * Constructor. The third argument can be one of HIST_TR_NO_TRANSFORM,
     * HIST_TR_AUTO_EPC_DBL, HIST_TR_AUTO_EPC_INT. With HIST_TR_NO_TRANSFORM,
     * you can set up cells manually (see addBinBound()), in the other two
     * cases it tries to create equiprobably cells.
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
    virtual void clearResult() override;

    /**
     * Transforms the table of precollected values into an internal
     * histogram structure.
     */
    virtual void transform() override;

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     */
    virtual void collectTransformed(double value) override;

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     */
    virtual void collectTransformed2(double value, double weight) override;

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
     * Returns the kth cell boundary.
     */
    virtual double getBasepoint(int k) const override;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double getCellValue(int k) const override;

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
     * Adds a new bin (cell) boundary. This method can only be called
     * if HIST_TR_NO_TRANSFORM was specified in the constructor call,
     * and only when the object is still in the initial data collection phase
     * (that is, transform() has been invoked yet).
     */
    virtual void addBinBound(double x);
    //@}
};

}  // namespace omnetpp


#endif


