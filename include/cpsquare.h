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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPSQUARE_H
#define __CPSQUARE_H

#include "iostream.h"
#include "cdensity.h"

//==========================================================================

/**
 * Implements the P<SUP>2</SUP> algorithm, which calculates quantile values
 * without storing the observations.
 */
class SIM_API cPSquare : public cDensityEstBase
{
  protected:
    int numcells;      // number of observations
    long numobs;       // number of cells,
    int *n;            // array of positions
    double *q;         // array of heights

  public:

    /**
     * Copy constructor.
     */
    cPSquare(cPSquare& r);

    /**
     * Constructor.
     */
    explicit cPSquare(const char *name=NULL, int cells=10);

    /**
     * Destructor.
     */
    virtual ~cPSquare();

     // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cPSquare".
     */
    virtual const char *className() const {return "cPSquare";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup()   {return new cPSquare(*this);}

    /**
     * MISSINGDOC: cPSquare:cPSquare&operator=(cPSquare&)
     */
    cPSquare& operator=(cPSquare& res);

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

    // redefined cStdDev functions

    /**
     * FIXME: redefined cStdDev functions
     */
    virtual void transform() {}  // not used, but cannot remain pure virtual


    /**
     * MISSINGDOC: cPSquare:void setRange(,)
     */
    virtual void setRange(double,double) {giveError();}

    /**
     * MISSINGDOC: cPSquare:void setRangeAuto(,)
     */
    virtual void setRangeAuto(int,double) {giveError();}

    /**
     * MISSINGDOC: cPSquare:void setRangeAutoLower(,,)
     */
    virtual void setRangeAutoLower(double,int,double) {giveError();}

    /**
     * MISSINGDOC: cPSquare:void setRangeAutoUpper(,,)
     */
    virtual void setRangeAutoUpper(double,int,double) {giveError();}

    /**
     * MISSINGDOC: cPSquare:void setNumFirstVals()
     */
    virtual void setNumFirstVals(int) {giveError();}
  private:

    /**
     * MISSINGDOC: cPSquare:void giveError()
     */
    void giveError(); // issues error message

  protected:

    /**
     * MISSINGDOC: cPSquare:void collectTransformed(double)
     */
    virtual void collectTransformed(double val);

  public:

    /**
     * MISSINGDOC: cPSquare:int cells()
     */
    virtual int cells();

    /**
     * MISSINGDOC: cPSquare:double basepoint(int)
     */
    virtual double basepoint(int k);

    /**
     * MISSINGDOC: cPSquare:double cell(int)
     */
    virtual double cell(int k);


    /**
     * MISSINGDOC: cPSquare:double cdf(double)
     */
    virtual double cdf(double x);

    /**
     * MISSINGDOC: cPSquare:double pdf(double)
     */
    virtual double pdf(double x);


    /**
     * MISSINGDOC: cPSquare:double random()
     */
    virtual double random();


    /**
     * MISSINGDOC: cPSquare:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *);

    /**
     * MISSINGDOC: cPSquare:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);
};

#endif

