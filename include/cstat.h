//==========================================================================
//   CSTAT.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStatistic : base for statistics
//    cStdDev    : collects min, max, mean, standard deviation
//
//==========================================================================

#ifndef __CSTAT_H
#define __CSTAT_H

#include <stdio.h>
#include <iostream.h>
#include "cobject.h"

class cTransientDetection;
class cAccuracyDetection;

//==========================================================================
// cStatistic - base class of different statistic collecting classes
//   NOTE: Provides no functionality, only defines virtual functions

class SIM_API cStatistic : public cObject
{
  public:
    cTransientDetection *td;    // ptr to associated object
    cAccuracyDetection *ra;     // ptr to associated object
    int genk;                   // index of random number generator to use

  protected:
    void freadvarsf (FILE *f,  const char *fmt, ...); // for loadFromFile()

  public:
    cStatistic(cStatistic& r);
    explicit cStatistic(const char *name=NULL);
    virtual ~cStatistic()  {}

    // redefined functions
    virtual const char *className() const {return "cStatistic";}
    cStatistic& operator=(cStatistic& res);
    virtual const char *inspectorFactoryName() const {return "cStatisticIFC";}
    virtual int netPack();
    virtual int netUnpack();

    // new functions

    // collect a value
    virtual void collect(double val) = 0;
    virtual void collect2(double val, double weight);
    void operator+= (double val) {collect(val);}

    // number of samples, minimum, maximum, mean, standard deviation:
    virtual long samples() = 0;
    virtual double weights() = 0;
    virtual double sum() = 0;
    virtual double sqrSum() = 0;
    virtual double min() = 0;
    virtual double max() = 0;
    virtual double mean() = 0;
    virtual double stddev() = 0;
    virtual double variance() = 0;

    // end of transient and result accuracy detection on inserted samples
    void addTransientDetection(cTransientDetection *object);
    void addAccuracyDetection(cAccuracyDetection *object);
    cTransientDetection *transientDetectionObject()  {return td;}
    cAccuracyDetection  *accuracyDetectionObject()   {return ra;}

    // generate random numbers based on the collected data:
    void setGenK(int gen_nr)   {genk=gen_nr;}  // random num. generator to use
    virtual double random() = 0;

    // clear result
    virtual void clearResult() = 0;

    // write to text file, read from text file
    virtual void saveToFile(FILE *) = 0;
    virtual void loadFromFile(FILE *) = 0;

};

//==========================================================================
// cStdDev - collects min, max, mean, standard deviation

class SIM_API cStdDev : public cStatistic
{
  protected:
    long num_samples;
    double min_samples,max_samples;
    double sum_samples,sqrsum_samples;

  public:
    cStdDev(cStdDev& r) : cStatistic(r) {setName(r.name());operator=(r);}
    explicit cStdDev(const char *name=NULL);
    virtual ~cStdDev() {}

    // redefined functions
    virtual const char *className() const {return "cStdDev";}
    virtual cObject *dup() {return new cStdDev(*this);}
    virtual void info(char *buf);
    cStdDev& operator=(cStdDev& res);
    virtual void writeContents(ostream& os);
    virtual int netPack();
    virtual int netUnpack();

    // redefined cStatistic functions
    virtual void collect(double val);

    virtual long samples()   {return num_samples;}
    virtual double weights() {return num_samples;}
    virtual double sum()     {return sum_samples;}
    virtual double sqrSum()  {return sqrsum_samples;}
    virtual double min()     {return min_samples;}
    virtual double max()     {return max_samples;}
    virtual double mean()    {return num_samples ? sum_samples/num_samples : 0.0;}
    virtual double stddev();
    virtual double variance();

    virtual double random();

    virtual void clearResult();

    virtual void saveToFile(FILE *);
    virtual void loadFromFile(FILE *);

};

//==========================================================================
// cWeightedStdDev - collects min, max, mean, std. dev. of weighted stats

class SIM_API cWeightedStdDev : public cStdDev
{
  protected:
    double sum_weights;

  public:
    cWeightedStdDev(cWeightedStdDev& r) : cStdDev(r) {setName(r.name());operator=(r);}
    explicit cWeightedStdDev(const char *name=NULL) : cStdDev(name)  {sum_weights=0;}
    virtual ~cWeightedStdDev() {}

    // redefined functions
    virtual const char *className() const {return "cWeightedStdDev";}
    virtual cObject *dup() {return new cWeightedStdDev(*this);}
    cWeightedStdDev& operator=(cWeightedStdDev& res);
    virtual int netPack();
    virtual int netUnpack();

    // redefined cStatistic functions
    virtual void collect(double val)  {collect2(val,1.0);}
    virtual void collect2(double val, double weight);

    virtual void clearResult();

    virtual double weights() {return sum_weights;}
    virtual double mean()    {return sum_weights!=0 ? sum_samples/sum_weights : 0.0;}
    virtual double variance();

    virtual void saveToFile(FILE *);
    virtual void loadFromFile(FILE *);

};

#endif
