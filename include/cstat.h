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

/**
 * Base class of different statistic collecting classes.
 * cStatistic is the base class for all statistical data
 * collection classes. cStatistic itself adds no data members
 * or algorithms to cObject, it only defines virtual functions
 * that will be redefined in descendants. No instance of cStatistic
 * can be created.
 */
class SIM_API cStatistic : public cObject
{
  public:
    cTransientDetection *td;    // ptr to associated object
    cAccuracyDetection *ra;     // ptr to associated object
    int genk;                   // index of random number generator to use

  protected:

    /**
     * MISSINGDOC: cStatistic:void freadvarsf(FILE*,char*,...)
     */
    void freadvarsf (FILE *f,  const char *fmt, ...); // for loadFromFile()

  public:

    /**
     * Copy constructor.
     */
    cStatistic(cStatistic& r);

    /**
     * Constructor, creates an object with the given name
     */
    explicit cStatistic(const char *name=NULL);

    /**
     * The destructor does nothing.
     */
    virtual ~cStatistic()  {}

    // redefined functions

    /**
     * Returns a pointer to the class name string, "cStatistic".
     */
    virtual const char *className() const {return "cStatistic";}

    /**
     * The assignment operator is present since descendants may refer
     * to it.
     */
    cStatistic& operator=(cStatistic& res);

    /**
     * MISSINGDOC: cStatistic:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cStatisticIFC";}

    /**
     * MISSINGDOC: cStatistic:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cStatistic:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    // collect a value

    /**
     * Collect one value.
     */
    virtual void collect(double val) = 0;

    /**
     * MISSINGDOC: cStatistic:void collect2(double,double)
     */
    virtual void collect2(double val, double weight);

    /**
     * MISSINGDOC: cStatistic:void operator+)
     */
    void operator+= (double val) {collect(val);}

    // number of samples, minimum, maximum, mean, standard deviation:

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual long samples() = 0;

    /**
     * MISSINGDOC: cStatistic:double weights()
     */
    virtual double weights() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double sum() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double sqrSum() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double min() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double max() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double mean() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double stddev() = 0;

    /**
     * In derived classes, these functions return the number of values
     * collected, the smallest/largest value, the mean, the standard
     * deviation, the sum and the squared sum of the collected data,
     * respectively.
     */
    virtual double variance() = 0;

    // end of transient and result accuracy detection on inserted samples

    /**
     * Assigns transient and accuracy detection objects to the statistic
     * object.
     */
    void addTransientDetection(cTransientDetection *object);

    /**
     * Assigns transient and accuracy detection objects to the statistic
     * object.
     */
    void addAccuracyDetection(cAccuracyDetection *object);

    /**
     * Returns the assigned transient and accuracy detection objects.
     */
    cTransientDetection *transientDetectionObject()  {return td;}

    /**
     * Returns the assigned transient and accuracy detection objects.
     */
    cAccuracyDetection  *accuracyDetectionObject()   {return ra;}

    // generate random numbers based on the collected data:

    /**
     * Sets the index of the random number generator to use when the
     * object has to generate a random number based on the statistics
     * stored.
     */
    void setGenK(int gen_nr)   {genk=gen_nr;}  // random num. generator to use

    /**
     * The function generates a random number based on the collected
     * data.
     */
    virtual double random() = 0;

    // clear result

    /**
     * This function should be redefined in derived classes to clear
     * the results collected so far.
     */
    virtual void clearResult() = 0;

    // write to text file, read from text file

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) = 0;

    /**
     * Reads the object data from a file written out by saveToFile()(or
     * written "by hand")
     */
    virtual void loadFromFile(FILE *) = 0;

};

//==========================================================================

/**
 * Statistics class to collect min, max, mean, standard deviation.
 */
class SIM_API cStdDev : public cStatistic
{
  protected:
    long num_samples;
    double min_samples,max_samples;
    double sum_samples,sqrsum_samples;

  public:

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cStdDev(cStdDev& r) : cStatistic(r) {setName(r.name());operator=(r);}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    explicit cStdDev(const char *name=NULL);

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual ~cStdDev() {}

    // redefined functions

    /**
     * Returns a pointer to the class name string, "cStdDev".
     */
    virtual const char *className() const {return "cStdDev";}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual cObject *dup() {return new cStdDev(*this);}

    /**
     * Redefined cObject functions.
     */
    virtual void info(char *buf);

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cStdDev& operator=(cStdDev& res);

    /**
     * Redefined cObject functions.
     */
    virtual void writeContents(ostream& os);

    /**
     * MISSINGDOC: cStdDev:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cStdDev:int netUnpack()
     */
    virtual int netUnpack();

    // redefined cStatistic functions

    /**
     * Redefined cStatistic functions.
     */
    virtual void collect(double val);

    /**
     * Redefined cStatistic functions.
     */
    virtual long samples()   {return num_samples;}

    /**
     * MISSINGDOC: cStdDev:double weights()
     */
    virtual double weights() {return num_samples;}

    /**
     * MISSINGDOC: cStdDev:double sum()
     */
    virtual double sum()     {return sum_samples;}

    /**
     * MISSINGDOC: cStdDev:double sqrSum()
     */
    virtual double sqrSum()  {return sqrsum_samples;}

    /**
     * Redefined cStatistic functions.
     */
    virtual double min()     {return min_samples;}

    /**
     * Redefined cStatistic functions.
     */
    virtual double max()     {return max_samples;}

    /**
     * Redefined cStatistic functions.
     */
    virtual double mean()    {return num_samples ? sum_samples/num_samples : 0.0;}

    /**
     * Redefined cStatistic functions.
     */
    virtual double stddev();

    /**
     * Redefined cStatistic functions.
     */
    virtual double variance();

    /**
     * Redefined cStatistic function. cStdDev's random
     * number generator returns numbers of normal distribution with the
     * current mean and standard deviation.
     */
    virtual double random();

    /**
     * Redefined cStatistic functions.
     */
    virtual void clearResult();

    /**
     * Redefined cStatistic functions.
     */
    virtual void saveToFile(FILE *);

    /**
     * Redefined cStatistic functions.
     */
    virtual void loadFromFile(FILE *);

};

//==========================================================================

/**
 * Statistics class to collect doubles and calculate weighted statistics
 * of them.
 */
class SIM_API cWeightedStdDev : public cStdDev
{
  protected:
    double sum_weights;

  public:

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cWeightedStdDev(cWeightedStdDev& r) : cStdDev(r) {setName(r.name());operator=(r);}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    explicit cWeightedStdDev(const char *name=NULL) : cStdDev(name)  {sum_weights=0;}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual ~cWeightedStdDev() {}

    // redefined functions

    /**
     * Returns a pointer to the class name string, "cWeightedStdDev".
     */
    virtual const char *className() const {return "cWeightedStdDev";}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual cObject *dup() {return new cWeightedStdDev(*this);}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cWeightedStdDev& operator=(cWeightedStdDev& res);

    /**
     * MISSINGDOC: cWeightedStdDev:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cWeightedStdDev:int netUnpack()
     */
    virtual int netUnpack();

    // redefined cStatistic functions

    /**
     * FIXME: redefined cStatistic functions
     */
    virtual void collect(double val)  {collect2(val,1.0);}

    /**
     * New member function.
     */
    virtual void collect2(double val, double weight);

    /**
     * Redefined cStdDev functions.
     */
    virtual void clearResult();

    /**
     * MISSINGDOC: cWeightedStdDev:double weights()
     */
    virtual double weights() {return sum_weights;}

    /**
     * Redefined cStdDev functions.
     */
    virtual double mean()    {return sum_weights!=0 ? sum_samples/sum_weights : 0.0;}

    /**
     * Redefined cStdDev functions.
     */
    virtual double variance();

    /**
     * Redefined cStdDev functions.
     */
    virtual void saveToFile(FILE *);

    /**
     * Redefined cStdDev functions.
     */
    virtual void loadFromFile(FILE *);

};

#endif

