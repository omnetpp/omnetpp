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


/**
 * FIXME: 
 * cHistogramBase
 * 
 *  adds a vector of counters to cDensityEstBase
 * 
 */
class SIM_API cHistogramBase : public cDensityEstBase
{
  protected:
    int num_cells;        // nr. of categories
    unsigned *cellv;      // array of counters

  public:

    /**
     * MISSINGDOC: cHistogramBase:cHistogramBase(cHistogramBase&)
     */
    cHistogramBase(cHistogramBase& r) : cDensityEstBase(r)
        {setName(r.name());cellv=NULL;operator=(r);}

    /**
     * MISSINGDOC: cHistogramBase:cHistogramBase(char*,int)
     */
    cHistogramBase(const char *name, int numcells);

    /**
     * MISSINGDOC: cHistogramBase:~cHistogramBase()
     */
    virtual ~cHistogramBase();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cHistogramBase";}

    /**
     * MISSINGDOC: cHistogramBase:cHistogramBase&operator=(cHistogramBase&)
     */
    cHistogramBase& operator=(cHistogramBase& res);

    /**
     * MISSINGDOC: cHistogramBase:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cHistogramBase:int netUnpack()
     */
    virtual int netUnpack();
    // dup() and inspector(..) not needed

    // redefined cDensityEstBase functions

    /**
     * FIXME: dup() and inspector(..) not needed
     * redefined cDensityEstBase functions
     */
    virtual void clearResult();

    /**
     * MISSINGDOC: cHistogramBase:void transform()
     */
    virtual void transform();

    /**
     * MISSINGDOC: cHistogramBase:int cells()
     */
    virtual int cells();


    /**
     * MISSINGDOC: cHistogramBase:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *); //--LG

    /**
     * MISSINGDOC: cHistogramBase:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);  //--LG
};

//==========================================================================
// cEqdHistogramBase


/**
 * FIXME: 
 * cEqdHistogramBase
 */
class SIM_API cEqdHistogramBase : public cHistogramBase //--LG
{
  protected:
    double cellsize;            // cell/category  sizes

  public:

    /**
     * MISSINGDOC: cEqdHistogramBase:cEqdHistogramBase(cEqdHistogramBase&)
     */
    cEqdHistogramBase(cEqdHistogramBase& r) : cHistogramBase(r)
        {setName(r.name());operator=(r);}

    /**
     * MISSINGDOC: cEqdHistogramBase:cEqdHistogramBase(char*,int)
     */
    explicit cEqdHistogramBase(const char *name=NULL, int numcells=10);

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cEqdHistogramBase";}

    /**
     * MISSINGDOC: cEqdHistogramBase:cEqdHistogramBase&operator=(cEqdHistogramBase&)
     */
    cEqdHistogramBase& operator=(cEqdHistogramBase& res);

    /**
     * MISSINGDOC: cEqdHistogramBase:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cEqdHistogramBase:int netUnpack()
     */
    virtual int netUnpack();
    // dup() not needed

    // redefined cDensityEstBase functions
  protected:

    /**
     * FIXME: dup() not needed
     * redefined cDensityEstBase functions
     */
    virtual void collectTransformed(double val);

    /**
     * MISSINGDOC: cEqdHistogramBase:void setupRange()
     */
    virtual void setupRange(); // also calculates cellsize
  public:

    /**
     * MISSINGDOC: cEqdHistogramBase:double basepoint(int)
     */
    virtual double basepoint(int k);

    /**
     * MISSINGDOC: cEqdHistogramBase:double cell(int)
     */
    virtual double cell(int k);


    /**
     * MISSINGDOC: cEqdHistogramBase:double pdf(double)
     */
    virtual double pdf(double x); // --LG

    /**
     * MISSINGDOC: cEqdHistogramBase:double cdf(double)
     */
    virtual double cdf(double x); // --LG


    /**
     * MISSINGDOC: cEqdHistogramBase:void saveToFile(FILE*)
     */
    virtual void saveToFile(FILE *); //--LG

    /**
     * MISSINGDOC: cEqdHistogramBase:void loadFromFile(FILE*)
     */
    virtual void loadFromFile(FILE *);  //--LG
};

//==========================================================================
// cLongHistogram - long int distribution result


/**
 * FIXME: 
 * cLongHistogram - long int distribution result
 */
class SIM_API cLongHistogram : public cEqdHistogramBase
{
  public:

    /**
     * Copy constructor.
     */
    cLongHistogram(cLongHistogram& r) : cEqdHistogramBase(r)
        {setName(r.name());operator=(r);}

    /**
     * MISSINGDOC: cLongHistogram:cLongHistogram(char*,int)
     */
    explicit cLongHistogram(const char *name=NULL, int numcells=10);

    /**
     * Destructor.
     */
    virtual ~cLongHistogram();

    // redefined functions

    /**
     * Returns a pointer to the class name string, "cLongHistogram".
     */
    virtual const char *className() const {return "cLongHistogram";}

    /**
     * Duplication and assignment work all right with cLongHistogram.
     */
    virtual cObject *dup()    {return new cLongHistogram(*this);}

    /**
     * MISSINGDOC: cLongHistogram:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cLongHistogram:int netUnpack()
     */
    virtual int netUnpack();
    // base class' op=() is OK

    // redefined cEqdHistogramBase functions
  protected:

    /**
     * FIXME: base class' op=() is OK
     * redefined cEqdHistogramBase functions
     */
    virtual void setupRange();
  public:

    /**
     * MISSINGDOC: cLongHistogram:void collect(double)
     */
    virtual void collect(double val);  // truncates + collects

    /**
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer is returned.
     */
    virtual double random();
};

//==========================================================================
// cDoubleHistogram - double distribution result


/**
 * FIXME: 
 * cDoubleHistogram - double distribution result
 */
class SIM_API cDoubleHistogram : public cEqdHistogramBase
{
  public:

    /**
     * Copy constructor
     */
    cDoubleHistogram(cDoubleHistogram& r) : cEqdHistogramBase(r)
          {setName(r.name());operator=(r);}

    /**
     * MISSINGDOC: cDoubleHistogram:cDoubleHistogram(char*,int)
     */
    explicit cDoubleHistogram(const char *name=NULL, int numcells=10);

    /**
     * Destructor.
     */
    virtual ~cDoubleHistogram();

    // redefined functions

    /**
     * Returns a pointer to the class name string, "cDoubleHistogram".
     */
    virtual const char *className() const  {return "cDoubleHistogram";}

    /**
     * Duplication and assignment work all right with cDoubleHistogram.
     */
    virtual cObject *dup()  {return new cDoubleHistogram(*this);}

    /**
     * MISSINGDOC: cDoubleHistogram:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cDoubleHistogram:int netUnpack()
     */
    virtual int netUnpack();
    // base class' op=() is OK

    // redefined cHistogramBase functions

    /**
     * Returns a random number based on the distribution collected. If
     * no values have been collected, it returns 0; when in initial collection
     * phase, it returns one of the stored observations; after the histogram
     * has been set up, a random integer is returned.
     */
    virtual double random();
};

#endif
