//==========================================================================
//  CMYSQLCONFIG.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMYSQLCONFIG_H
#define __CMYSQLCONFIG_H

#include <omnetpp.h>
#include <mysql.h>

class PatternMatcher;


/**
 * Configuration class which reads data from a MYSQL database. The two
 * configurations, the boot-time omnetpp.ini and the database get merged.
 * Config entries read from the database are appended after the ones
 * read from omnetpp.ini, so if there is a collision, omnetpp.ini
 * takes precedence.
 *
 * <pre>
 * CREATE TABLE config (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      name VARCHAR(80)
 * );
 *
 * CREATE TABLE configsection (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      configid INT,
 *      name VARCHAR(80)
 * );
 *
 * CREATE TABLE configentry (
 *      sectionid INT,
 *      name VARCHAR(200),
 *      value VARCHAR(200),
 *      entryorder INT
 * );
 * </pre>
 *
 * To insert example data:
 *
 * <pre>
 * INSERT INTO configsection (configid,name) VALUES (1,"General");
 * INSERT INTO configsection (configid,name) VALUES (1,"Run 1");
 * INSERT INTO configsection (configid,name) VALUES (1,"Run 2");
 * </pre>
 *
 * For database configuration parameters, please refer to the documentation
 * of opp_mysql_connectToDB().
 *
 * Additional config parameters:
 *
 * <pre>
 * [General]
 * mysqlconfiguration-connectprefix = <string> # look for connect parameters with the given prefix
 * mysqlconfiguration-dumpbootcfg = <true/false>  # dump ini file as SQL INSERTs; default=false
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class cMySQLConfiguration : public cConfiguration
{
  private:

    struct sFile {
        char *fname;            // name of the file (absolute path)
        char *directory;        // directory
    };

    struct sEntry {             // one entry contains:
        int section_id;         //  section it belongs to
        char *key;              //  key
        PatternMatcher *keypattern; // key as pattern
        char *value;            //  its value (without quotes)
        char *rawvalue;         //  original value with quotes, or NULL if it had no quotes
        int file_id;            //  file it was read from
        int lineno;             //  line number in file
        bool accessed;          //  has it been accessed?
    };

    std::vector<sFile> files;          // table of ini files
    std::vector<char *> sections;      // table of section names
    std::vector<sEntry> entries;       // table of entries

    sEntry *_findEntry(const char *section, const char *key);
    const char *_getValue(const char *section, const char *key, bool raw);
    static int getSectionId(cConfiguration *cfg, const char *sectionName);
    int getFileId(const char *fileName);
    void copyExistingConfig(cConfiguration *cfg);
    void readDB(MYSQL *mysql, const char *configName);
    void dumpConfig(cConfiguration *cfg);
    void clearContents();

    bool warnings;
    bool notfound;

  public:
    cMySQLConfiguration();
    virtual ~cMySQLConfiguration();

    /**
     * Redefined method from cConfiguration.
     */
    virtual void initializeFrom(cConfiguration *conf);

    /** @name Specific methods. */
    //@{
    void setWarnings(bool warn) {warnings = warn;}
    //@}

    /** @name Redefined section query methods from cConfiguration */
    //@{
    virtual int getNumSections();
    virtual const char *getSectionName(int k);
    //@}

    /** @name Redefined getter methods from cConfiguration */
    //@{
    virtual bool exists(const char *section, const char *key);
    virtual bool getAsBool(const char *section, const char *key, bool defaultvalue=false);
    virtual long getAsInt(const char *section, const char *key, long defaultvalue=0);
    virtual double getAsDouble(const char *section, const char *key, double defaultvalue=0.0);
    virtual double getAsTime(const char *section, const char *key, double defaultvalue=0.0);
    virtual const char *getAsString(const char *section, const char *key, const char *defaultvalue=""); // quotes stripped (if any)
    virtual std::string getAsFilename(const char *section, const char *key, const char *defaultvalue="");
    virtual std::string getAsFilenames(const char *section, const char *key, const char *defaultvalue="");
    virtual const char *getAsCustom(const char *section, const char *key, const char *defaultvalue=NULL); // with quotes (if any)
    virtual const char *getBaseDirectoryFor(const char *section, const char *key);
    virtual std::string getLocation(const char *section, const char *key);
    virtual bool notFound();
    virtual std::vector<opp_string> getEntriesWithPrefix(const char *section, const char *keypart1, const char *keypart2);
    //@}

    /** @name Redefined two-section getter methods from cConfiguration */
    //@{
    virtual bool exists2(const char *section1, const char *section2, const char *key);
    virtual bool getAsBool2(const char *section1, const char *section2, const char *key, bool defaultvalue=false);
    virtual long getAsInt2(const char *section1, const char *section2, const char *key, long defaultvalue=0);
    virtual double getAsDouble2(const char *section1, const char *section2, const char *key, double defaultvalue=0.0);
    virtual double getAsTime2(const char *section1, const char *section2, const char *key, double defaultvalue=0.0);
    virtual const char *getAsString2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual std::string getAsFilename2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual std::string getAsFilenames2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual const char *getAsCustom2(const char *section1, const char *section2, const char *key, const char *defaultvalue="");
    virtual const char *getBaseDirectoryFor(const char *section1, const char *section2, const char *key);
    virtual std::string getLocation(const char *section1, const char *section2, const char *key);
    virtual std::vector<opp_string> getEntriesWithPrefix(const char *section1, const char *section2, const char *keypart1, const char *keypart2);
    //@}

    /**
     * Redefined method from cConfiguration.
     */
    virtual const char *fileName() const;
};

#endif

