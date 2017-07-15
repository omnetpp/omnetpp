//==========================================================================
//  CPSQUARE.H - part of
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

//  Author: Babak Fakhamzadeh, TU Delft, Dec 1996

#ifndef __OMNETPP_CPSQUARE_H
#define __OMNETPP_CPSQUARE_H

#include "cdensityestbase.h"

namespace omnetpp {


/**
 * @brief Implements the P<sup>2</sup> algorithm, which calculates quantile
 * values without storing the observations.
 *
 * @ingroup Statistics
 */
class SIM_API cPSquare : public cDensityEstBase
{
  protected:
    int numCells;      // number of cells
    long numObs;       // number of observations
    int *n;            // array of positions
    double *q;         // array of heights

  protected:
    void copy(const cPSquare& other);

  protected:
    // abstract method in cDensityEstBase
    virtual void doMergeCellValues(const cDensityEstBase *other) override;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cPSquare(const cPSquare& r);

    /**
     * Constructor.
     */
    explicit cPSquare(const char *name=nullptr, int cells=10);

    /**
     * Destructor.
     */
    virtual ~cPSquare();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cPSquare& operator=(const cPSquare& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPSquare *dup() const override  {return new cPSquare(*this);}

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

  private:
    // internal: issues error message
    void raiseError();

  protected:
    /**
     * Called internally by collect(), this method updates the P2 data structure
     * with the new value.
     */
    virtual void collectTransformed(double value) override;

    /**
     * Called internally by collect().
     */
    virtual void collectTransformed2(double value, double weight) override;

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * This method is not used with cPSquare, but it could not remain pure virtual.
     */
    virtual void transform() override {}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm does not require them), but they could not remain pure virtual.
     */
    virtual void setRange(double,double) override {raiseError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm does not require them), but they could not remain pure virtual.
     */
    virtual void setRangeAuto(int,double) override {raiseError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm does not require them), but they could not remain pure virtual.
     */
    virtual void setRangeAutoLower(double,int,double) override {raiseError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm does not require them), but they could not remain pure virtual.
     */
    virtual void setRangeAutoUpper(double,int,double) override {raiseError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm does not require them), but they could not remain pure virtual.
     */
    virtual void setNumPrecollectedValues(int) override {raiseError();}

    /**
     * Returns the number of cells used.
     */
    virtual int getNumCells() const override;

    /**
     * Returns the kth cell boundary. Note that because of the P2 algorithm,
     * cell boundaries are shifting during data collection, thus getCellValue() and
     * other methods based on getCellValue() and getBasepoint() return approximate values.
     */
    virtual double getBasepoint(int k) const override;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double getCellValue(int k) const override;

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double getCDF(double x) const override;

    /**
     * Returns the value of the Probability Density Function at a given x.
     */
    virtual double getPDF(double x) const override;

    /**
     * Generates a random number based on the collected data.
     */
    virtual double draw() const override;

    /**
     * Merging is not supported by this class. This method throws an error.
     */
    virtual void merge(const cStatistic *other) override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *) override;
    //@}
};

}  // namespace omnetpp


#endif

