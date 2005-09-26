//==========================================================================
//  MYSQLOUTPUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MYSQLOUTPUTVECTORMGR_H
#define __MYSQLOUTPUTVECTORMGR_H

#include <omnetpp.h>
#include <mysql.h>


/**
 * An Output Vector Manager that writes into a MySQL database instead of
 * a .vec file.
 *
 * It expects to find two tables in the database: VECTOR and VECDATA.
 * VECTOR corresponds to the "vector" lines in the normal .vec files,
 * and VECDATA to the data lines.
 *
 * Note that the tables are created with the MyISAM engine. This is important,
 * because the InnoDB engine doesn't support INSERT DELAYED, which completely
 * ruins performance.
 *
 * <pre>
 * CREATE TABLE vrun (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      runnumber INT,
 *      network VARCHAR(80),
 *      date TIMESTAMP
 * ) ENGINE = MYISAM;
 *
 * CREATE TABLE vector (
 *      runid INT,
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      module VARCHAR(200),
 *      name VARCHAR(80)
 * ) ENGINE = MYISAM;
 *
 * CREATE TABLE vecdata (
 *      vectorid INT,
 *      time DOUBLE PRECISION,
 *      value DOUBLE PRECISION
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
 * mysqloutputscalarmanager-connectprefix = <string> # look for connect parameters with the given prefix
 * </pre>
 *
 * @ingroup Envir
 */
class cMySQLOutputVectorManager : public cOutputVectorManager
{
  protected:
    struct sVectorData {
       int tuple;           // 1 or 2
       long id;             // vector ID
       opp_string modulename; // module of cOutVector object
       opp_string vectorname; // cOutVector object name
       bool initialised;    // true if the "label" line is already written out
       bool enabled;        // write to the output file can be enabled/disabled
       simtime_t starttime; // write begins at starttime
       simtime_t stoptime;  // write stops at stoptime
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
    MYSQL_STMT *insertVecdataStmt;
    MYSQL_BIND insVectorBind[3];
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

    /** @name Redefined cOutputVectorManager member functions. */
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
    virtual void *registerVector(const char *modulename, const char *vectorname, int tuple);

    /**
     * Deregisters the output vector.
     */
    virtual void deregisterVector(void *vechandle);

    /**
     * Writes the (time, value) pair into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);

    /**
     * Writes the (time, value1, value2) triplet into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value1, double value2);

    /**
     * Returns NULL, because this class doesn't use a file.
     */
    const char *fileName() const {return NULL;}

    /**
     * Performs a database commit.
     */
    virtual void flush();
    //@}
};

#endif

