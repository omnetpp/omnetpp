//==========================================================================
//   COUTVECT.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cOutVector         : represents a vector in the statistical output file
//    cOutFileMgr        : manages the file for statistical output
//
//==========================================================================

#ifndef __COUTVECT_H
#define __COUTVECT_H

#include <stdio.h>
#include "cobject.h"

//==========================================================================
// cOutFileMgr and cOutVector
//    work together to allow the user save several "series" or
//    "output vectors" (a series of numbers or number pairs
//    which are produced during simulation as results) to a common file
//    called "output vector file" or simply "output file".
//    The output file is ASCII and can be read by a spreadsheet etc.
//    There is only one instance of cOutFileMgr (in cSimulation), it
//    stores the common file ptr and gives unique identifiers to cOutVector
//    objects. There can be several cOutVectors, each one handles one
//    output vector.
//
//    cOutFileMgr is also used to handle snapshot and parameter change files
//
//==========================================================================

//==========================================================================
// cOutFileMgr:
//   This class is responsible for handling the output file for cOutVectors.
//   cOutFileMgr is also used in module parameter change logging

class SIM_API cOutFileMgr : public cObject
{
    long nextID;       // holds next free ID for output vectors
    opp_string fname;  // output file name
    FILE *handle;      // file ptr of output file

  public:
    explicit cOutFileMgr(const char *name=NULL);
    virtual ~cOutFileMgr();

    // redefined functions
    virtual const char *className()  {return "cOutFileMgr";}

    // new functions
    void setFileName(const char *s);
    const char *fileName();
    void openFile();
    void closeFile();
    void deleteFile();
    FILE *filePointer(); // calls openFile() if necessary

    long getNewID();
};

//==========================================================================
// cOutVector
// This class is responsible for writing one output vector to the file.
//
// A sample file:
//   vector 5  "net.site[2].terminal[12]"  "response time"  1
//   5 12.056  2355.66666666
//   5 15.850  5656.67644455
//
// The first line is a "label" line, assigns a label ("response time") to an
// id (5). The "net.site[2].terminal[12]" is the name of the module which
// produces the data lines beginning with 5. The number 1 indicates that the
// data lines (beginning with 5) will contain 1 data value after the time mark.
// The second line is a data line, the 1st number is the id, the
// second is the actual simulation time and the third is a simulation data
// value.
// If there was a number 2 in the "label" line instead of 1, the data lines
// would contain 2 simulation data values. 3 or more are not allowed.
//

typedef void (*RecordFunc)(void *, double, double);

class TOutVectorInspector;
class TOutVectorWindow;

class SIM_API cOutVector : public cObject
{
  protected:
    bool enabled;        // write to the output file can be enabled/disabled
    bool initialised;    // TRUE if the "label" line is already written out
    long ID;             // ID of the data lines
    int tuple;           // 1 or 2 : the number of data values
    simtime_t starttime; // write to file begins at starttime
    simtime_t stoptime;  // write to file stops at stoptime

    // the following members will be used directly by inspectors
    friend class TOutVectorInspector;
    friend class TOutVectorWindow;
    RecordFunc record_in_inspector; // to notify inspector about file writes
    void *data_for_inspector;

  public:
    cOutVector(cOutVector& r) : cObject(r) {setName(r.name());operator=(r);}
    explicit cOutVector(const char *name=NULL, int tupl=1);
    virtual ~cOutVector();

    // redefined functions
    virtual void setName(const char *name);
    virtual const char *className()  {return "cOutVector";}
    virtual cObject *dup()    {return new cOutVector(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName() {return "cOutVectorIFC";}
    // no operator=()

    // new functions
    void record (double value);
    void record (double value1, double value2);
    void enable();
    void disable();
    void setStartTime(simtime_t t);
    void setStopTime(simtime_t t);

};

#endif
