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

/**
 * Responsible for handling the output file for cOutVectors.
 * This is an internal class: users don't need to use cOutFileMgr
 * directly, only through cOutVector.
 *
 * cOutFileMgr is also used to handle snapshot and parameter change files.
 *
 * @ingroup Internals
 */
class SIM_API cOutFileMgr : public cObject
{
    long nextID;       // holds next free ID for output vectors
    opp_string fname;  // output file name
    FILE *handle;      // file ptr of output file

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    explicit cOutFileMgr(const char *name=NULL);

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~cOutFileMgr();

    // FIXME: op= missing!

    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cOutFileMgr".
     */
    virtual const char *className() const {return "cOutFileMgr";}
    //@}

    /** @name File handling. */
    //@{

    /**
     * Sets the name of the output file. This name will be set at the time
     * of the next call of openFile().
     */
    void setFileName(const char *s);

    /**
     * Returns the file name.
     */
    const char *fileName();

    /**
     * Opens the output file with the currently set fileName.
     */
    void openFile();

    /**
     * Closes the output file.
     */
    void closeFile();

    /**
     * Deletes the output file.
     */
    void deleteFile();

    /**
     * Returns the file pointer to the file. This method calls openFile() if necessary.
     */
    FILE *filePointer();
    //@}

    /** @name Handling output vectors. */
    //@{

    /**
     * Returns a unique ID for constructing a new cOutVector
     * object. The user should not explicitly call this function.
     */
    long getNewID();
    //@}

};

//==========================================================================

typedef void (*RecordFunc)(void *, double, double);

class TOutVectorInspector;
class TOutVectorWindow;

/**
 * Responsible for writing simulation data (an output vector) to a file.
 * A cOutVector object can write doubles or pair of doubles to the "statistical
 * output file". The file consists of label lines and data lines.
 *
 * One can use UNIX tools like sed, awk or perl to extract a particular
 * vector etc. from the file, or/and read it in spreadsheets like
 * Excel.
 *
 * The cOutVector::record() member is used to output a value
 * (or a value pair). This will generate a data line in the file,
 * unless the output vector is disabled or the current simulation
 * time is outside a specified interval (see member functions).
 *
 * @ingroup SimSupport
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
    /** @name Constructors, destructor, assignment */
    //@{

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

    // FIXME: op= missing!

    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * FIXME: redefined functions
     */
    virtual void setName(const char *name);

    /**
     * Returns pointer to a string containing the class name, "cOutVector".
     */
    virtual const char *className() const {return "cOutVector";}

    /**
     * Dupping is not implemented for cOutVector. This function
     * gives an error when called.
     */
    virtual cObject *dup()    {return new cOutVector(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cOutVectorIFC";}
    //@}

    /** @name Configuring and writing to output vectors. */
    //@{

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
    //@}
};

#endif
