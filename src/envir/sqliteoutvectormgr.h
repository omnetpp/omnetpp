//==========================================================================
//  SQLITEOUTPUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_SQLITEOUTPUTVECTORMGR_H
#define __OMNETPP_ENVIR_SQLITEOUTPUTVECTORMGR_H

#include <cstdio>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envir/envirdefs.h"
#include "envir/intervals.h"
#include "common/sqlitevectorfilewriter.h"
#include "resultfileutils.h"

namespace omnetpp {
namespace envir {

using omnetpp::common::SqliteVectorFileWriter;

/**
 * A cIOutputVectorManager that uses an SQlite database as output.
 *
 * @ingroup Envir
 */
class SqliteOutputVectorManager : public cIOutputVectorManager
{
  protected:
    struct VectorData {
        void *handleInWriter;      // nullptr until vector is registered in the writer
        opp_string moduleName;     // full path of component the vector belongs to
        opp_string vectorName;     // vector name
        opp_string_map attributes; // vector attributes
        bool enabled;              // write to the output file can be enabled/disabled
        Intervals intervals;       // recording intervals
    };

    typedef std::vector<VectorData*> Vectors;

    enum State {NEW, STARTED, OPENED, ENDED} state = NEW;
    std::string fname;
    SqliteVectorFileWriter writer;
    Vectors vectors;  // registered output vectors

    enum IndexingMode { INDEX_AHEAD, INDEX_AFTER, INDEX_NONE } indexingMode = INDEX_AFTER;

  protected:
    virtual void openFileForRun();
    virtual void closeFile();
    bool isBad() {return state==OPENED && !writer.isOpen();}

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    SqliteOutputVectorManager() {}

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~SqliteOutputVectorManager() {closeFile();}
    //@}

    /** @name Redefined cIOutputVectorManager member functions. */
    //@{

    /**
     * Deletes output vector file if exists (left over from previous runs).
     * The file is not yet opened, it is done inside registerVector() on demand.
     */
    virtual void startRun() override;

    /**
     * Closes the output file.
     */
    virtual void endRun() override;

    /**
     * Registers a vector and returns a handle.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname) override;

    /**
     * Deregisters the output vector.
     */
    virtual void deregisterVector(void *vechandle) override;

    /**
     * Sets an attribute of an output vector.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) override;

    /**
     * Writes the (time, value) pair into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value) override;

    /**
     * Returns the file name.
     */
    const char *getFileName() const override {return fname.c_str();}

    /**
     * Calls fflush().
     */
    virtual void flush() override;
    //@}
};

}  // namespace envir
}  // namespace omnetpp

#endif
