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

class SIM_API cTransientDetection : public cObject
{
  private:
    cStatistic *back;          // ptr to cStatistic that uses this object

  public:
    cTransientDetection(cTransientDetection& r) : cObject() {setName(r.name());operator=(r);}
    explicit cTransientDetection(const char *name=NULL) : cObject(name) {}
    virtual ~cTransientDetection()  {}

    // redefined functions
    virtual const char *className() const {return "cTransientDetection";}
    virtual int netPack();
    virtual int netUnpack();

    // new functions
    virtual void collect(double val)=0;  // collect a value
    virtual bool detected()=0;           // transient period over
    virtual void reset()=0;              // reset detection
    virtual void stop()=0;               // stop detection
    virtual void start()=0;              // start detection

    virtual void setHostObject(cStatistic *ptr)  {back = ptr;}
    virtual cStatistic *hostObject()  {return back;}
};

//===NL=====================================================================
// cAccuracyDetection - virtual base class for result accuracy detection

class SIM_API cAccuracyDetection : public cObject
{
  private:
    cStatistic *back;              // ptr to cStatistic that uses this object

  public:
    cAccuracyDetection(cAccuracyDetection& r) : cObject() {setName(r.name());operator=(r);}
    explicit cAccuracyDetection(const char *name=NULL) : cObject(name)  {}
    virtual ~cAccuracyDetection()  {}

    // redefined functions
    virtual const char *className() const {return "cAccuracyDetection";}
    virtual int netPack();
    virtual int netUnpack();

    // new functions
    virtual void collect(double val)=0;  // collect a value
    virtual bool detected()=0;           // transient period over
    virtual void reset()=0;              // reset detection
    virtual void stop()=0;               // stop detection
    virtual void start()=0;              // start detection

    virtual void setHostObject(cStatistic *ptr)  {back = ptr;}
    virtual cStatistic *hostObject()  {return back;}
};

//===NL======================================================================
// cTDExpandingWindows - first algorithm for transient detection

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
    void detectTransient();   // computes new value of transval

  public:
    cTDExpandingWindows(cTDExpandingWindows& r);
    explicit cTDExpandingWindows(const char *name,
                        int reps=3, int minw=4, double wind=1.3, double acc=0.3,
                        PostTDFunc f=NULL,void *p=NULL);
    virtual ~cTDExpandingWindows();

    // redefined functions
    virtual const char *className() const {return "cTDExpandingWindows";}
    cTDExpandingWindows& operator=(cTDExpandingWindows& res);

    // redefined cTransientDetection functions
    void setPostDetectFunction(PostTDFunc f, void *p) {pdf = f; pdfdata = p;}
    virtual void collect(double val);
    virtual bool detected() {return transval;}
    virtual void reset();
    virtual void stop()      {go = false;}
    virtual void start()     {go = true;}
    void setParameters(int reps=3, int minw=4,   // set/change the detection parameters
                       double wind=1.3, double acc=0.3);
};


//===NL======================================================================
// cADByStddev - first algorithm for detecting result accuracy

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
    void detectAccuracy();      // compute new value of transval
    double stddev();            // compute the standard deviation

  public:
    cADByStddev(cADByStddev& r);
    explicit cADByStddev(const char *name=NULL,
                         double acc=0.01, int reps=3,
                         PostADFunc f=NULL, void *p=NULL);
    virtual ~cADByStddev()  {}

    // redefined functions
    virtual const char *className() const {return "cADByStddev";}
    cADByStddev& operator=(cADByStddev& res);

    // redefined cAccuracyDetection functions
    void setPostDetectFunction(PostADFunc f, void *p) {pdf=f; pdfdata=p;}
    virtual void collect(double val);
    virtual bool detected() {return resaccval;}
    virtual void reset();
    virtual void stop()   {go=false;}
    virtual void start()  {go=true;}
    void setParameters(double acc=0.1, int reps=3)  //set the detection parameters
        {accuracy=acc; repeats=detreps=reps;}
};

#endif
