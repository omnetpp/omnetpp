//==========================================================================
//  CMYSQLCONFIGREADER.H - part of
//
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

#ifndef __CMYSQLCONFIGREADER_H
#define __CMYSQLCONFIGREADER_H

#include <string>
#include <vector>

#include <mysql.h>

#include <omnetpp.h>
#include "cconfigreader.h"

using namespace omnetpp;

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
 *      name VARCHAR(80) NOT NULL
 * );
 *
 * CREATE TABLE configsection (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      configid INT NOT NULL,
 *      name VARCHAR(80) NOT NULL,
 *      UNIQUE KEY (configid, name),
 *      FOREIGN KEY (configid) REFERENCES config(id)
 * );
 *
 * CREATE TABLE configentry (
 *      sectionid INT NOT NULL,
 *      name VARCHAR(200) NOT NULL,
 *      value VARCHAR(200) NOT NULL,
 *      entryorder INT,
 *      KEY (sectionid, name),
 *      KEY (sectionid, entryorder),
 *      FOREIGN KEY (sectionid) REFERENCES configsection(id)
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
 * mysqlconfig-connectionname = <string> # look for connection parameters within given object
 * mysqlconfig-dumpbootcfg = <true/false>  # dump ini file as SQL INSERTs; default=false
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class cMySQLConfigReader : public cConfigurationReader, public cObject
{
  protected:
    class KeyValue1 : public KeyValue {
      private:
        const std::string *basedir;  // points into basedirs[]
        const std::string *filename; // points into filenames[]
        int lineNumber;
        std::string key;
        std::string value; //XXX stringpool it?

      public:
        KeyValue1(const std::string *bdir, const std::string *fname, int li, const char *k, const char *v) {
            basedir = bdir; filename = fname; lineNumber = li; key = k; value = v;
        }

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const   {return key.c_str();}
        virtual const char *getValue() const {return value.c_str();}
        virtual const char *getBaseDirectory() const  {return basedir->c_str();}
        virtual const char *getFileName() const  {return filename->c_str();}
        virtual int getLineNumber() const  {return lineNumber;}
    };

    std::string rootfilename;  // name of "root" ini file read
    std::string defaultbasedir; // the default base directory

    typedef std::set<std::string> StringSet;
    StringSet filenames; // stores ini file names (absolute paths)
    StringSet basedirs;  // stores base directory names (absolute paths)

    struct Section {
        std::string name;
        std::vector<KeyValue1> entries;
    };
    std::vector<Section> sections;

  protected:
    const Section& getSection(int sectionId) const;
    static const char *findEndContent(const char *line, const char *filename, int lineNumber);
    static bool readLineInto(std::string& line, FILE *file);
    static std::string trim(const char *start, const char *end);
    static std::string escape(const char *str);
    int findSection(const char * sectionName) const;
    void copyExistingConfig(cConfigurationReader *cfg);
    void readDB(MYSQL *mysql, const char *databaseConfigName);

  public:
    cMySQLConfigReader();
    virtual ~cMySQLConfigReader();
    void initializeFrom(cConfiguration *cfg);

    /** @name Methods implementing cConfigurationReader */
    //@{
    virtual const char *getFileName() const;
    virtual const char *getDefaultBaseDirectory() const;
    virtual int getNumSections() const;
    virtual const char *getSectionName(int sectionId) const;
    virtual int getNumEntries(int sectionId) const;
    virtual const KeyValue& getEntry(int sectionId, int entryId) const;
    virtual void dump() const;
    //@}

    virtual void dumpConfig(cConfigurationReader *cfg) const;
    virtual void clearContents();
};


#endif  // __CMYSQLCONFIGREADER_H

