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
  Copyright (C) 1992,99 Andras Varga
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
// cPSquare - calculates quantile values without storing the observations
//

class cPSquare : public cDensityEstBase
{
  protected:
    int numcells;      // number of observations
    long numobs;       // number of cells,
    int *n;            // array of positions
    double *q;         // array of heights

  public:
    cPSquare(cPSquare& r);
    explicit cPSquare(const char *name=NULL, int cells=10);
    virtual ~cPSquare();

     // redefined functions
    virtual const char *className()  {return "cPSquare";}
    virtual cObject *dup()   {return new cPSquare(*this);}
    cPSquare& operator=(cPSquare& res);
    virtual int netPack();
    virtual int netUnpack();
    virtual void writeContents(ostream& os);

    // redefined cStdDev functions
    virtual void transform() {}  // not used, but cannot remain pure virtual

    virtual void setRange(double,double) {giveError();}
    virtual void setRangeAuto(int,double) {giveError();}
    virtual void setRangeAutoLower(double,int,double) {giveError();}
    virtual void setRangeAutoUpper(double,int,double) {giveError();}
    virtual void setNumFirstVals(int) {giveError();}
 private:
    void giveError(); // issues error message

  protected:
    virtual void collectTransformed(double val);

  public:
    virtual int cells();
    virtual double basepoint(int k);
    virtual double cell(int k);

    virtual double cdf(double x);
    virtual double pdf(double x);

    virtual double random();

    virtual void saveToFile(FILE *);
    virtual void loadFromFile(FILE *);
};

#endif


