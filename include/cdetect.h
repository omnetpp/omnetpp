//==NL=====================================================================
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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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

typedef void (*PostTDFunc)(cTransientDetection *, void *);
typedef void (*PostADFunc)(cAccuracyDetection *, void *);

//===NL=====================================================================
// cTransientDetection - virtual base class for transient detection


/**
 * FIXME: //=== classes declared here:
 * //=== class mentioned here:
 * //===NL=====================================================================
 * cTransientDetection - virtual base class for transient detection
 */
class SIM_API cTransientDetection : public cObject
{
  private:
    cStatistic *back;          // ptr to cStatistic that uses this object

  public:

    /**
     * MISSINGDOC: cTransientDetection:cTransientDetection(cTransientDetection&)
     */
    cTransientDetection(cTransientDetection& r) : cObject() {setName(r.name());operator=(r);}

    /**
     * MISSINGDOC: cTransientDetection:cTransientDetection(char*)
     */
    explicit cTransientDetection(const char *name=NULL) : cObject(name) {}

    /**
     * MISSINGDOC: cTransientDetection:~cTransientDetection()
     */
    virtual ~cTransientDetection()  {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cTransientDetection";}

    /**
     * MISSINGDOC: cTransientDetection:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cTransientDetection:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    /**
     * FIXME: new functions
     */
    virtual void collect(double val)=0;  // collect a value

    /**
     * MISSINGDOC: cTransientDetection:bool detected()
     */
    virtual bool detected()=0;           // transient period over

    /**
     * MISSINGDOC: cTransientDetection:void reset()
     */
    virtual void reset()=0;              // reset detection

    /**
     * MISSINGDOC: cTransientDetection:void stop()
     */
    virtual void stop()=0;               // stop detection

    /**
     * MISSINGDOC: cTransientDetection:void start()
     */
    virtual void start()=0;              // start detection


    /**
     * MISSINGDOC: cTransientDetection:void setHostObject(cStatistic*)
     */
    virtual void setHostObject(cStatistic *ptr)  {back = ptr;}

    /**
     * MISSINGDOC: cTransientDetection:cStatistic*hostObject()
     */
    virtual cStatistic *hostObject()  {return back;}
};

//===NL=====================================================================
// cAccuracyDetection - virtual base class for result accuracy detection


/**
 * FIXME: //===NL=====================================================================
 * cAccuracyDetection - virtual base class for result accuracy detection
 */
class SIM_API cAccuracyDetection : public cObject
{
  private:
    cStatistic *back;              // ptr to cStatistic that uses this object

  public:

    /**
     * MISSINGDOC: cAccuracyDetection:cAccuracyDetection(cAccuracyDetection&)
     */
    cAccuracyDetection(cAccuracyDetection& r) : cObject() {setName(r.name());operator=(r);}

    /**
     * MISSINGDOC: cAccuracyDetection:cAccuracyDetection(char*)
     */
    explicit cAccuracyDetection(const char *name=NULL) : cObject(name)  {}

    /**
     * MISSINGDOC: cAccuracyDetection:~cAccuracyDetection()
     */
    virtual ~cAccuracyDetection()  {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cAccuracyDetection";}

    /**
     * MISSINGDOC: cAccuracyDetection:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cAccuracyDetection:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    /**
     * FIXME: new functions
     */
    virtual void collect(double val)=0;  // collect a value

    /**
     * MISSINGDOC: cAccuracyDetection:bool detected()
     */
    virtual bool detected()=0;           // transient period over

    /**
     * MISSINGDOC: cAccuracyDetection:void reset()
     */
    virtual void reset()=0;              // reset detection

    /**
     * MISSINGDOC: cAccuracyDetection:void stop()
     */
    virtual void stop()=0;               // stop detection

    /**
     * MISSINGDOC: cAccuracyDetection:void start()
     */
    virtual void start()=0;              // start detection


    /**
     * MISSINGDOC: cAccuracyDetection:void setHostObject(cStatistic*)
     */
    virtual void setHostObject(cStatistic *ptr)  {back = ptr;}

    /**
     * MISSINGDOC: cAccuracyDetection:cStatistic*hostObject()
     */
    virtual cStatistic *hostObject()  {return back;}
};

//===NL======================================================================
// cTDExpandingWindows - first algorithm for transient detection


/**
 * FIXME: //===NL======================================================================
 * cTDExpandingWindows - first algorithm for transient detection
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

    /**
     * MISSINGDOC: cTDExpandingWindows:void detectTransient()
     */
    void detectTransient();   // computes new value of transval

  public:

    /**
     * MISSINGDOC: cTDExpandingWindows:cTDExpandingWindows(cTDExpandingWindows&)
     */
    cTDExpandingWindows(cTDExpandingWindows& r);
    explicit cTDExpandingWindows(const char *name,
                        int reps=3, int minw=4, double wind=1.3, double acc=0.3,

    /**
     * MISSINGDOC: cTDExpandingWindows:cTDExpandingWindows(char*,int,int,double,double,PostTDFunc,void*)
     */
                        PostTDFunc f=NULL,void *p=NULL);

    /**
     * MISSINGDOC: cTDExpandingWindows:~cTDExpandingWindows()
     */
    virtual ~cTDExpandingWindows();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cTDExpandingWindows";}

    /**
     * MISSINGDOC: cTDExpandingWindows:cTDExpandingWindows&operator=(cTDExpandingWindows&)
     */
    cTDExpandingWindows& operator=(cTDExpandingWindows& res);

    // redefined cTransientDetection functions

    /**
     * FIXME: redefined cTransientDetection functions
     */
    void setPostDetectFunction(PostTDFunc f, void *p) {pdf = f; pdfdata = p;}

    /**
     * MISSINGDOC: cTDExpandingWindows:void collect(double)
     */
    virtual void collect(double val);

    /**
     * MISSINGDOC: cTDExpandingWindows:bool detected()
     */
    virtual bool detected() {return transval;}

    /**
     * MISSINGDOC: cTDExpandingWindows:void reset()
     */
    virtual void reset();

    /**
     * MISSINGDOC: cTDExpandingWindows:void stop()
     */
    virtual void stop()      {go = false;}

    /**
     * MISSINGDOC: cTDExpandingWindows:void start()
     */
    virtual void start()     {go = true;}
    void setParameters(int reps=3, int minw=4,   // set/change the detection parameters

    /**
     * MISSINGDOC: cTDExpandingWindows:void setParameters(int,int,double,double)
     */
                       double wind=1.3, double acc=0.3);
};


//===NL======================================================================
// cADByStddev - first algorithm for detecting result accuracy


/**
 * FIXME: //===NL======================================================================
 * cADByStddev - first algorithm for detecting result accuracy
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

    /**
     * MISSINGDOC: cADByStddev:void detectAccuracy()
     */
    void detectAccuracy();      // compute new value of transval

    /**
     * MISSINGDOC: cADByStddev:double stddev()
     */
    double stddev();            // compute the standard deviation

  public:

    /**
     * MISSINGDOC: cADByStddev:cADByStddev(cADByStddev&)
     */
    cADByStddev(cADByStddev& r);
    explicit cADByStddev(const char *name=NULL,
                         double acc=0.01, int reps=3,

    /**
     * MISSINGDOC: cADByStddev:cADByStddev(char*,double,int,PostADFunc,void*)
     */
                         PostADFunc f=NULL, void *p=NULL);

    /**
     * MISSINGDOC: cADByStddev:~cADByStddev()
     */
    virtual ~cADByStddev()  {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cADByStddev";}

    /**
     * MISSINGDOC: cADByStddev:cADByStddev&operator=(cADByStddev&)
     */
    cADByStddev& operator=(cADByStddev& res);

    // redefined cAccuracyDetection functions

    /**
     * FIXME: redefined cAccuracyDetection functions
     */
    void setPostDetectFunction(PostADFunc f, void *p) {pdf=f; pdfdata=p;}

    /**
     * MISSINGDOC: cADByStddev:void collect(double)
     */
    virtual void collect(double val);

    /**
     * MISSINGDOC: cADByStddev:bool detected()
     */
    virtual bool detected() {return resaccval;}

    /**
     * MISSINGDOC: cADByStddev:void reset()
     */
    virtual void reset();

    /**
     * MISSINGDOC: cADByStddev:void stop()
     */
    virtual void stop()   {go=false;}

    /**
     * MISSINGDOC: cADByStddev:void start()
     */
    virtual void start()  {go=true;}

    /**
     * MISSINGDOC: cADByStddev:void setParameters(double,int)
     */
    void setParameters(double acc=0.1, int reps=3)  //set the detection parameters
        {accuracy=acc; repeats=detreps=reps;}
};

#endif
