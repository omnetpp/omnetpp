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
// cHistogramBase
//
//  adds a vector of counters to cDensityEstBase
//

class cHistogramBase : public cDensityEstBase
{
  protected:
    int num_cells;        // nr. of categories
    unsigned *cellv;      // array of counters

  public:
    cHistogramBase(cHistogramBase& r) : cDensityEstBase(r)
        {setName(r.name());cellv=NULL;operator=(r);}
    cHistogramBase(const char *name, int numcells);
    virtual ~cHistogramBase();

    // redefined functions
    virtual const char *className()  {return "cHistogramBase";}
    cHistogramBase& operator=(cHistogramBase& res);
    virtual void writeContents(ostream& os);
    virtual int netPack();
    virtual int netUnpack();
    // dup() and inspector(..) not needed

    // redefined cDensityEstBase functions
    virtual void clearResult();
    virtual void transform();
    virtual int cells();

    virtual void saveToFile(FILE *); //--LG
    virtual void loadFromFile(FILE *);  //--LG
};

//==========================================================================
// cEqdHistogramBase

class cEqdHistogramBase : public cHistogramBase //--LG
{
  protected:
    double cellsize;            // cell/category  sizes

  public:
    cEqdHistogramBase(cEqdHistogramBase& r) : cHistogramBase(r)
        {setName(r.name());operator=(r);}
    explicit cEqdHistogramBase(const char *name=NULL, int numcells=10);

    // redefined functions
    virtual const char *className()  {return "cEqdHistogramBase";}
    cEqdHistogramBase& operator=(cEqdHistogramBase& res);
    virtual int netPack();
    virtual int netUnpack();
    // dup() not needed

    // redefined cDensityEstBase functions
  protected:
    virtual void collectTransformed(double val);
    virtual void setupRange(); // also calculates cellsize
  public:
    virtual double basepoint(int k);
    virtual double cell(int k);

    virtual double pdf(double x); // --LG
    virtual double cdf(double x); // --LG

    virtual void saveToFile(FILE *); //--LG
    virtual void loadFromFile(FILE *);  //--LG
};

//==========================================================================
// cLongHistogram - long int distribution result

class cLongHistogram : public cEqdHistogramBase
{
  public:
    cLongHistogram(cLongHistogram& r) : cEqdHistogramBase(r)
        {setName(r.name());operator=(r);}
    explicit cLongHistogram(const char *name=NULL, int numcells=10);
    virtual ~cLongHistogram();

    // redefined functions
    virtual const char *className()  {return "cLongHistogram";}
    virtual cObject *dup()    {return new cLongHistogram(*this);}
    virtual int netPack();
    virtual int netUnpack();
    // base class' op=() is OK

    // redefined cEqdHistogramBase functions
  protected:
    virtual void setupRange();
  public:
    virtual void collect(double val);  // truncates + collects
    virtual double random();
};

//==========================================================================
// cDoubleHistogram - double distribution result

class cDoubleHistogram : public cEqdHistogramBase
{
  public:
    cDoubleHistogram(cDoubleHistogram& r) : cEqdHistogramBase(r)
          {setName(r.name());operator=(r);}
    explicit cDoubleHistogram(const char *name=NULL, int numcells=10);
    virtual ~cDoubleHistogram();

    // redefined functions
    virtual const char *className()  {return "cDoubleHistogram";}
    virtual cObject *dup()  {return new cDoubleHistogram(*this);}
    virtual int netPack();
    virtual int netUnpack();
    // base class' op=() is OK

    // redefined cHistogramBase functions
    virtual double random();
};

#endif

