//=========================================================================
//
//  CDETECT.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//         File designed and written by the Hollandiaba Team
//
//   Class declarations:
//     cTransientDetection :  virtual base class for transient detection
//     cAccuracyDetection  :  virtual base class for result accuracy detection
//
//     cTDExpandingWindows :  an algorithm for transient detection
//     cADByStddev         :  an algorithm for result accuracy detection
//
//   Bugfixes: Andras Varga, Oct.1996
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDETECT_H
#define __CDETECT_H

#include "cobject.h"
#include "cstat.h"

//=== classes declared here:
class cTransientDetection;
class cAccuracyDetection;
class cTDExpandingWindows;
class cADByStddev;

//=== class mentioned here:
class cStatistic;

/**
 * Prototype for callback functions used by a cTransientDetection object.
 * @ingroup EnumsTypes
 */
typedef void (*PostTDFunc)(cTransientDetection *, void *);

/**
 * Prototype for callback functions used by a cAccuracyDetection object.
 * @ingroup EnumsTypes
 */
typedef void (*PostADFunc)(cAccuracyDetection *, void *);

//==========================================================================

/**
 * Virtual base class for transient detection classes.
 *
 * @ingroup Statistics
 */
class SIM_API cTransientDetection : public cObject
{
  private:
    cStatistic *back;    // ptr to cStatistic that uses this object

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cTransientDetection(const cTransientDetection& r) : cObject() {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cTransientDetection(const char *name=NULL) : cObject(name) {}

    /**
     * Destructor.
     */
    virtual ~cTransientDetection()  {}

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cTransientDetection& operator=(const cTransientDetection&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

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
    //@}

    /** @name Pure virtual functions to define the interface. */
    //@{

    /**
     * Should be redefined to update the detection algorithm with a value.
     */
    virtual void collect(double val) = 0;

    /**
     * Should be redefined to return true if end of transient has been detected.
     */
    virtual bool detected() const = 0;

    /**
     * Should be redefined to reset detection algorithm.
     */
    virtual void reset() = 0;

    /**
     * Should be redefined to stop detection (further calls to collect()
     * should be ignored.)
     */
    virtual void stop() = 0;

    /**
     * Should be redefined to start detection (further calls to collect()
     * should update the detection algorithm.)
     */
    virtual void start() = 0;
    //@}

    /** @name Host object. */
    //@{

    /**
     * Sets the host object. This is internally called by cStatistic's
     * addTransientDetection() method.
     */
    virtual void setHostObject(cStatistic *ptr)  {back = ptr;}

    /**
     * Returns a pointer to the host object.
     */
    virtual cStatistic *hostObject() const  {return back;}
    //@}
};

//==========================================================================

/**
 * Virtual base class for result accuracy detection classes.
 *
 * @ingroup Statistics
 */
class SIM_API cAccuracyDetection : public cObject
{
  private:
    cStatistic *back;              // ptr to cStatistic that uses this object

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cAccuracyDetection(const cAccuracyDetection& r) : cObject() {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cAccuracyDetection(const char *name=NULL) : cObject(name)  {}

    /**
     * Destructor.
     */
    virtual ~cAccuracyDetection()  {}

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cAccuracyDetection& operator=(const cAccuracyDetection&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

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
    //@}

    /** @name Pure virtual functions to define the interface. */
    //@{

    /**
     * Should be redefined to update the detection algorithm with a value.
     */
    virtual void collect(double val) = 0;

    /**
     * Should be redefined to return true if required accuracy has been reached.
     */
    virtual bool detected() const = 0;

    /**
     * Should be redefined to reset detection algorithm.
     */
    virtual void reset() = 0;

    /**
     * Should be redefined to stop detection (further calls to collect()
     * should be ignored).
     */
    virtual void stop() = 0;

    /**
     * Should be redefined to start detection (further calls to collect()
     * will update the detection algorithm).
     */
    virtual void start() = 0;
    //@}

    /** @name Host object. */
    //@{

    /**
     * Sets the host object. This is internally called by cStatistic's
     * addTransientDetection() method.
     */
    virtual void setHostObject(cStatistic *ptr)  {back = ptr;}

    /**
     * Returns a pointer to the host object.
     */
    virtual cStatistic *hostObject() const  {return back;}
    //@}
};

//===========================================================================

/**
 * A transient detection algorithm. Uses sliding window approach
 * with two windows, and checks the difference of the two averages
 * to see if the transient period is over.
 *
 * @ingroup Statistics
 */
class SIM_API cTDExpandingWindows : public cTransientDetection
{
  private:
    bool go;                  // collect & detect
    bool transval;            // value of the last detection
    double accuracy;          // accuracy for detection
    int minwinds;             // minimum windows size
    double windexp;           // window expansion factor
    int repeats;              // repetitions necessary for detection
    int detreps;              // number of detections in a row
    int size;                 // number of collected values
    struct xy {double x; double y; xy *next;};
    xy *func;                 // structure of collected values
    PostTDFunc pdf;           // function to call after detection
    void *pdfdata;            // data for PostDetectFunct

  private:
    // internal: computes new value of transval
    void detectTransient();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cTDExpandingWindows(const cTDExpandingWindows& r);

    /**
     * Constructor.
     */
    explicit cTDExpandingWindows(const char *name=NULL,
                        int reps=3, int minw=4, double wind=1.3, double acc=0.3,
                        PostTDFunc f=NULL,void *p=NULL);

    /**
     * Destructor.
     */
    virtual ~cTDExpandingWindows();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's
     * operator=() for more details.
     */
    cTDExpandingWindows& operator=(const cTDExpandingWindows& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Dupping is not implemented for this class. This function
     * gives an error (throws cException) when called.
     */
    virtual cObject *dup() const  {return new cTDExpandingWindows(*this);}
    //@}

    /** @name Redefined cTransientDetection member functions. */
    //@{

    /**
     * Updates the detection algorithm with a value.
     */
    virtual void collect(double val);

    /**
     * Returns true if end of transient has been detected.
     */
    virtual bool detected() const {return transval;}

    /**
     * Resets detection algorithm.
     */
    virtual void reset();

    /**
     * Stop detection; further calls to collect() will be ignored.
     */
    virtual void stop()      {go = false;}

    /**
     * Start detection; further calls to collect() will update the
     * detection algorithm.
     */
    virtual void start()     {go = true;}
    //@}

    /** @name Setting up the detection object. */
    //@{

    /**
     * Adds a function that will be called when accuracy has reached the
     * configured limit.
     */
    // FIXME: why not in base class?
    void setPostDetectFunction(PostTDFunc f, void *p) {pdf = f; pdfdata = p;}

    /**
     * Sets the parameters of the detection algorithm.
     */
    void setParameters(int reps=3, int minw=4,
                       double wind=1.3, double acc=0.3);
    //@}
};


//===========================================================================

/**
 * An algorithm for result accuracy detection. The actual algorithm:
 * divide the standard deviation by the square of the number of values
 * and check if this is small enough.
 *
 * @ingroup Statistics
 */
class SIM_API cADByStddev : public cAccuracyDetection
{
  private:
    bool go;                    // start collecting if true
    bool resaccval;             // value of the last detection
    double accuracy;            // minimum needed for detection
    long int sctr;              // counter
    double ssum,sqrsum;         // sum, square sum;
    int repeats, detreps;       // repetitions necessary for detection
    PostADFunc pdf;             // function to call after detection
    void *pdfdata;              // data for PostDetectFunc

  private:
    // internal: compute new value of transval
    void detectAccuracy();

    // internal: compute the standard deviation
    double stddev();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cADByStddev(const cADByStddev& r);

    /**
     * Constructor.
     */
    explicit cADByStddev(const char *name=NULL,
                         double acc=0.01, int reps=3,
                         PostADFunc f=NULL, void *p=NULL);

    /**
     * Destructor.
     */
    virtual ~cADByStddev()  {}

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cADByStddev& operator=(const cADByStddev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Dupping is not implemented for this class. This function
     * gives an error (throws cException) when called.
     */
    virtual cObject *dup() const  {return new cADByStddev(*this);}
    //@}

    /** @name Redefined cAccuracyDetection functions. */
    //@{

    /**
     * Updates the detection algorithm with a value.
     */
    virtual void collect(double val);

    /**
     * Returns true if required accuracy has been reached.
     */
    virtual bool detected() const {return resaccval;}

    /**
     * Resets detection algorithm.
     */
    virtual void reset();

    /**
     * Stop detection; further calls to collect() will be ignored.
     */
    virtual void stop()   {go=false;}

    /**
     * Start detection; further calls to collect() will update the
     * detection algorithm.
     */
    virtual void start()  {go=true;}
    //@}

    /** @name Setting up the detection object. */
    //@{

    /**
     * Adds a function that will be called when accuracy has reached the
     * configured limit.
     */
    // FIXME: why not in base class?
    void setPostDetectFunction(PostADFunc f, void *p) {pdf=f; pdfdata=p;}

    /**
     * Sets the parameters of the detection algorithm.
     */
    void setParameters(double acc=0.1, int reps=3)
        {accuracy=acc; repeats=detreps=reps;}
    //@}
};

#endif

