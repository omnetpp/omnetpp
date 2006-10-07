//==========================================================================
//  MYSQLOUTPUTSCALARMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MYSQLOUTPUTSCALARMGR_H
#define __MYSQLOUTPUTSCALARMGR_H

#include <omnetpp.h>
#include <mysql.h>


/**
 * An Output Scalar Manager that writes into a MySQL database instead of
 * a .sca file.
 *
 * It expects to find two tables in the database: RUN and SCALAR.
 * RUN corresponds to the "run" lines in the normal .sca files,
 * and SCALAR to the "scalar" (data) lines.
 *
 * Note that the tables are created with the MyISAM engine. This is important,
 * because the InnoDB engine doesn't support INSERT DELAYED, which completely
 * ruins performance.
 *
 * <pre>
 * CREATE TABLE run (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      runnumber INT,
 *      network VARCHAR(80),
 *      date TIMESTAMP
 * ) ENGINE = MYISAM;
 *
 * CREATE TABLE scalar (
 *      runid INT,
 *      module VARCHAR(200),
 *      name VARCHAR(80),
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
 * mysqloutputscalarmanager-commit-freq = <integer> # COMMIT every n INSERTs, default=10
 * mysqloutputscalarmanager-connectprefix = <string> # look for connect parameters with the given prefix
 * </pre>
 *
 * @ingroup Envir
 */
class cMySQLOutputScalarManager : public cOutputScalarManager
{
  protected:
    MYSQL *mysql;

    // we COMMIT after every commitFreq INSERT statements
    int commitFreq;
    int insertCount;

    // database id (run.id) of current run
    long runId;

    bool initialized;

    // prepared statements and their parameter bindings
    MYSQL_STMT *insertScalarStmt;
    MYSQL_BIND insScalarBind[4];

    // these variables are bound to the above bind parameters
    int runidBuf;
    char moduleBuf[201];
    unsigned long moduleLength;
    char nameBuf[81];
    unsigned long nameLength;
    double valueBuf;

  protected:
    void openDB();
    void closeDB();
    void commitDB();
    void insertRunIntoDB();

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cMySQLOutputScalarManager();

    /**
     * Destructor.
     */
    virtual ~cMySQLOutputScalarManager();
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun();

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun();

    /** @name Scalar statistics */
    //@{

    /**
     * Records a double scalar result into the scalar result file.
     */
    void recordScalar(cModule *module, const char *name, double value);

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

