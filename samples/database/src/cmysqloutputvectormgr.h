//==========================================================================
//  MYSQLOUTPUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2011 Zoltan Bojthe

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MYSQLOUTPUTVECTORMGR_H
#define __MYSQLOUTPUTVECTORMGR_H

#include <mysql.h>

#include <omnetpp.h>

using namespace omnetpp;

#include "intervals.h"


/**
 * An Output Vector Manager that writes into a MySQL database instead of
 * a .vec file.
 *
 * It expects to find two tables in the database: VECTOR and VECDATA.
 * VECTOR corresponds to the "vector" lines in the normal .vec files,
 * and VECDATA to the data lines.
 *
 * Note that the vecdata table is created with the MyISAM engine. This is important,
 * because the InnoDB engine doesn't support INSERT DELAYED, which completely
 * ruins performance.
 *
 * <pre>
 * CREATE TABLE vrun (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      runnumber INT NOT NULL,
 *      network VARCHAR(80) NOT NULL,
 *      date TIMESTAMP NOT NULL
 * );
 *
 * CREATE TABLE vector (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      runid INT NOT NULL,
 *      module VARCHAR(200) NOT NULL,
 *      name VARCHAR(80) NOT NULL,
 *      FOREIGN KEY (runid) REFERENCES vrun(id)
 * );
 *
 * CREATE TABLE vecattr (
 *      vectorid INT NOT NULL,
 *      name VARCHAR(200) NOT NULL,
 *      value VARCHAR(200) NOT NULL,
 *      FOREIGN KEY (vectorid) REFERENCES vector(id)
 * );
 *
 * CREATE TABLE vecdata (
 *      vectorid INT NOT NULL,
 *      time DOUBLE PRECISION NOT NULL,
 *      value DOUBLE PRECISION NOT NULL,
 *      FOREIGN KEY (vectorid) REFERENCES vector(id)
 * ) ENGINE = MYISAM;
 * </pre>
 *
 * For database configuration parameters, please refer to the documentation
 * of opp_mysql_connectToDB().
 *
 * Additional config parameters:
 *
 * <pre>
 * [General]
 * mysqloutputvectormanager-commit-freq = <integer> # COMMIT every n INSERTs, default=50
 * mysqloutputscalarmanager-connectionname = <string> # look for connect parameters in the given object
 * </pre>
 *
 * @ingroup Envir
 */
class cMySQLOutputVectorManager : public cIOutputVectorManager
{
  protected:
    struct sVectorData {
       long id;             // vector ID
       opp_string modulename; // module of cOutVector object
       opp_string vectorname; // cOutVector object name
       opp_string_map attributes; // vector attributes
       bool initialised;    // true if the "label" line is already written out
       bool enabled;        // write to the output file can be enabled/disabled
       bool recordEventNumbers;  // write to the output file can be enabled/disabled
       Intervals intervals; // write begins at starttime
    };

    // the database connection
    MYSQL *mysql;

    // database id (vrun.id) of current run
    long runId;

    bool initialized;

    // we COMMIT after every commitFreq INSERT statements
    int commitFreq;
    int insertCount;

    // prepared statements and their parameter bindings
    MYSQL_STMT *insertVectorStmt;
    MYSQL_BIND insVectorBind[3];
    MYSQL_STMT *insertVecAttrStmt;
    MYSQL_BIND insVecAttrBind[3];
    MYSQL_STMT *insertVecdataStmt;
    MYSQL_BIND insVecdataBind[3];

    // these variables are bound to the above bind parameters
    int runidBuf;
    char moduleBuf[201];
    unsigned long moduleLength;
    char nameBuf[81];
    unsigned long nameLength;
    int vectoridBuf;
    double timeBuf;
    double valueBuf;
    char vecAttrNameBuf[201];
    unsigned long vecAttrNameLength;
    char vecAttrValueBuf[201];
    unsigned long vecAttrValueLength;

  protected:
    void openDB();
    void closeDB();
    void commitDB();
    void insertRunIntoDB();
    void initVector(sVectorData *vp);
    virtual sVectorData *createVectorData();

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cMySQLOutputVectorManager();

    /**
     * Destructor. Closes the output file if it is still open.
     */
    virtual ~cMySQLOutputVectorManager();
    //@}

    /** @name Redefined cIOutputVectorManager member functions. */
    //@{

    /**
     * Deletes output vector file if exists (left over from previous runs).
     * The file is not yet opened, it is done inside registerVector() on demand.
     */
    virtual void startRun();

    /**
     * Closes the output file.
     */
    virtual void endRun();

    /**
     * Registers a vector and returns a handle.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname);

    /**
     * Deregisters the output vector.
     */
    virtual void deregisterVector(void *vectorhandle);

    /**
     * Sets an attribute of an output vector.
     */
    virtual void setVectorAttribute(void *vectorhandle, const char *name, const char *value);

    /**
     * Writes the (time, value) pair into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);

    /**
     * Returns NULL, because this class doesn't use a file.
     */
    const char *getFileName() const {return nullptr;}

    /**
     * Performs a database commit.
     */
    virtual void flush();
    //@}
};

#endif

