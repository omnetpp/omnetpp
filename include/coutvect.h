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


/**
 * FIXME: 
 * cOutFileMgr and cOutVector
 *    work together to allow the user save several "series" or
 *    "output vectors" (a series of numbers or number pairs
 *    which are produced during simulation as results) to a common file
 *    called "output vector file" or simply "output file".
 *    The output file is ASCII and can be read by a spreadsheet etc.
 *    There is only one instance of cOutFileMgr (in cSimulation), it
 *    stores the common file ptr and gives unique identifiers to cOutVector
 *    objects. There can be several cOutVectors, each one handles one
 *    output vector.
 * 
 *    cOutFileMgr is also used to handle snapshot and parameter change files
 * 
 * 
 * 
 * cOutFileMgr:
 *   This class is responsible for handling the output file for cOutVectors.
 *   cOutFileMgr is also used in module parameter change logging
 */
class SIM_API cOutFileMgr : public cObject
{
    long nextID;       // holds next free ID for output vectors
    opp_string fname;  // output file name
    FILE *handle;      // file ptr of output file

  public:

    /**
     * Constructor.
     */
    explicit cOutFileMgr(const char *name=NULL);

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~cOutFileMgr();

    // redefined functions

    /**
     * Returns a pointer to the class name "cOutFileMgr".
     */
    virtual const char *className() const {return "cOutFileMgr";}

    // new functions

    /**
     * Sets the name of the statistical output file. This name will be
     * set at the time of the next call of openFile().
     */
    void setFileName(const char *s);

    /**
     * MISSINGDOC: cOutFileMgr:char*fileName()
     */
    const char *fileName();

    /**
     * Opens the statistical output file with the currently set fileName.
     */
    void openFile();

    /**
     * Closes the statistical output file
     */
    void closeFile();

    /**
     * MISSINGDOC: cOutFileMgr:void deleteFile()
     */
    void deleteFile();

    /**
     * MISSINGDOC: cOutFileMgr:FILE*filePointer()
     */
    FILE *filePointer(); // calls openFile() if necessary


    /**
     * Returns a unique ID for constructing a new cOutVector
     * object. The user should not explicitly call this function.
     */
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


/**
 * FIXME: 
 * cOutVector
 * This class is responsible for writing one output vector to the file.
 * 
 * A sample file:
 *   vector 5  "net.site[2].terminal[12]"  "response time"  1
 *   5 12.056  2355.66666666
 *   5 15.850  5656.67644455
 * 
 * The first line is a "label" line, assigns a label ("response time") to an
 * id (5). The "net.site[2].terminal[12]" is the name of the module which
 * produces the data lines beginning with 5. The number 1 indicates that the
 * data lines (beginning with 5) will contain 1 data value after the time mark.
 * The second line is a data line, the 1st number is the id, the
 * second is the actual simulation time and the third is a simulation data
 * value.
 * If there was a number 2 in the "label" line instead of 1, the data lines
 * would contain 2 simulation data values. 3 or more are not allowed.
 * 
 */
class SIM_API cOutVector : public cObject
{
  protected:
    bool enabled;        // write to the output file can be enabled/disabled
    bool initialised;    // true if the "label" line is already written out
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

    /**
     * Copy constructor.
     */
    cOutVector(cOutVector& r) : cObject(r) {setName(r.name());operator=(r);}

    /**
     * Constructor. Accepts the object name and the multiplicity of the
     * data it should write to the file at a time. Possible values of
     * tuple are 1 or 2.
     */
    explicit cOutVector(const char *name=NULL, int tupl=1);

    /**
     * Destructor.
     */
    virtual ~cOutVector();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual void setName(const char *name);

    /**
     * Returns a pointer to the class name "cOutVector".
     */
    virtual const char *className() const {return "cOutVector";}

    /**
     * Dupping does not make much sense, not implemented.
     */
    virtual cObject *dup()    {return new cOutVector(*this);}

    /**
     * Redefined.
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cOutVector:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cOutVectorIFC";}
    // no operator=()

    // new functions

    /**
     * Records one data to the file. It can be used only in the case
     * if the instance of cOutVector was created with tupl=1,
     * otherwise it gives an error message.
     */
    void record (double value);

    /**
     * Records two values to the file. It can be used only in the case
     * if the instance of cOutVector was created with tupl=2,
     * otherwise it gives an error message.
     */
    void record (double value1, double value2);

    /**
     * Makes writing to the file enabled. It is enabled by default.
     */
    void enable();

    /**
     * Makes writing to the file disabled. It is enabled by default.
     */
    void disable();

    /**
     * Sets StartTime to t. Data is recorded to the file only if simulation
     * time > StartTime. The default value of StartTime is 0.
     */
    void setStartTime(simtime_t t);

    /**
     * Sets StopTime to t. Data is recorded to the file only if the simulation
     * time < StopTime. The default value of StopTime is 0 which means
     * no Stoptime.
     */
    void setStopTime(simtime_t t);

};

#endif
