//==========================================================================
//   CPSQUARE.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cPSquare : calculates quantile values without storing the observations
//
//   Written by Babak Fakhamzadeh, TU Delft, Dec 1996
//
//==========================================================================
/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPSQUARE_H
#define __CPSQUARE_H

#include "cdensity.h"

//==========================================================================

/**
 * Implements the P<SUP>2</SUP> algorithm, which calculates quantile values
 * without storing the observations.
 *
 * @ingroup Statistics
 */
class SIM_API cPSquare : public cDensityEstBase
{
  protected:
    int numcells;      // number of observations
    long numobs;       // number of cells,
    int *n;            // array of positions
    double *q;         // array of heights

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
    explicit cPSquare(const char *name=NULL, int cells=10);

    /**
     * Destructor.
     */
    virtual ~cPSquare();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cPSquare& operator=(const cPSquare& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const   {return new cPSquare(*this);}

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

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);
    //@}

  private:
    // internal: issues error message
    void giveError();

  protected:
    /**
     * Called internally by collect(), this method updates the P2 data structure
     * with the new value.
     */
    virtual void collectTransformed(double val);

  public:
    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * This method is not used with cPSquare, but it could not remain pure virtual.
     */
    virtual void transform() {}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm doesn't require them), but they could not remain pure virtual.
     */
    virtual void setRange(double,double) {giveError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm doesn't require them), but they could not remain pure virtual.
     */
    virtual void setRangeAuto(int,double) {giveError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm doesn't require them), but they could not remain pure virtual.
     */
    virtual void setRangeAutoLower(double,int,double) {giveError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm doesn't require them), but they could not remain pure virtual.
     */
    virtual void setRangeAutoUpper(double,int,double) {giveError();}

    /**
     * setRange() and setNumFirstVals() methods are not used with cPSquare
     * (the algorithm doesn't require them), but they could not remain pure virtual.
     */
    virtual void setNumFirstVals(int) {giveError();}

    /**
     * Returns the number of cells used.
     */
    virtual int cells() const;

    /**
     * Returns the kth cell boundary. Note that because of the P2 algorithm,
     * cell boundaries are shifting during data collection, thus cell() and
     * other methods based on cell() and basepoint() return approximate values.
     */
    virtual double basepoint(int k) const;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double cell(int k) const;

    /**
     * Returns the value of the Cumulated Density Function at a given x.
     */
    virtual double cdf(double x) const;

    /**
     * Returns the value of the Probability Density Function at a given x.
     */
    virtual double pdf(double x) const;

    /**
     * Generates a random number based on the collected data. Uses the random number generator set by setGenK().
     */
    virtual double random() const;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);
    //@}
};

#endif

