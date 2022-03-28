//==========================================================================
//  MYSQLOUTPUTSCALARMGR.H - part of
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

#ifndef __DATABASE_MYSQLOUTPUTSCALARMGR_H
#define __DATABASE_MYSQLOUTPUTSCALARMGR_H

#include <mysql/mysql.h>
#include <omnetpp.h>

using namespace omnetpp;


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
 *      runnumber INT NOT NULL,
 *      network VARCHAR(80) NOT NULL,
 *      date TIMESTAMP NOT NULL
 * );
 *
 * CREATE TABLE scalar (
 *      runid INT  NOT NULL,
 *      module VARCHAR(200) NOT NULL,
 *      name VARCHAR(80) NOT NULL,
 *      value DOUBLE PRECISION NOT NULL,
 *      KEY (runid,module,name),
 *      FOREIGN KEY (runid) REFERENCES run(id)
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
 * mysqloutputscalarmanager-connectionname = <string> # look for connect parameters in the given object
 * </pre>
 *
 * @ingroup Envir
 */
class cMySQLOutputScalarManager : public cIOutputScalarManager
{
  protected:
    cConfiguration *cfg = nullptr;

    MYSQL *mysql = nullptr;

    // we COMMIT after every commitFreq INSERT statements
    int commitFreq;
    int insertCount;

    // database id (run.id) of current run
    long runId;

    bool initialized;

    // prepared statements and their parameter bindings
    MYSQL_STMT *insertScalarStmt = nullptr;
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
     * Sets the configuration database to use for configuring this object.
     */
    virtual void configure(cConfiguration *cfg);

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /** @name Scalar statistics */
    //@{

    /**
     * Records a double scalar result into the scalar result file.
     */
    virtual bool recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) override;

    /**
     * Records a histogram or statistic object into the scalar result file.
     */
    virtual bool recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) override;

    /**
     * Records a module or channel parameter, in a default configuration into the scalar result file.
     * The return value indicates whether the recording has actually taken place (true=yes).
     */
    virtual bool recordParameter(cPar *par) override;

    /**
     * Records the runtime NED type of module or channel, in a default configuration into the scalar result file.
     * The return value indicates whether the recording has actually taken place (true=yes).
     */
    virtual bool recordComponentType(cComponent *component) override;

    /**
     * Returns nullptr, because this class doesn't use a file.
     */
    const char *getFileName() const override {return nullptr;}

    /**
     * Performs a database commit.
     */
    virtual void flush() override;
    //@}
};

#endif

