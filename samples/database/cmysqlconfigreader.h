//==========================================================================
//  CMYSQLCONFIGURATION.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMYSQLCONFIGURATION_H
#define __CMYSQLCONFIGURATION_H

#include <omnetpp.h>
#include <string>
#include <vector>
#include <mysql.h>
#include "cconfigreader.h"

//FIXME code is incomplete, and won't even compile

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
class cMySQLConfigReader : public cConfigurationReader
{
  protected:
    class KeyValue1 : public KeyValue {
      private:
        std::string *basedir;  // points into basedirs[]
        std::string *filename; // points into filenames[]
        int lineNumber;
        std::string key;
        std::string value; //XXX stringpool it?

      public:
        KeyValue1(std::string *bdir, std::string *fname, int li, const char *k, const char *v) {
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
    void internalReadFile(const char *filename);
    const Section& getSection(int sectionId) const;
    static const char *findEndContent(const char *line, const char *filename, int lineNumber);
    static bool readLineInto(std::string& line, FILE *file);
    static std::string trim(const char *start, const char *end);

  public:
    cMySQLConfigReader();
    virtual ~cMySQLConfigReader();
    virtual void readDB(MYSQL *mysql, const char *databaseConfigName);
    void cMySQLConfigReader::initializeFrom(cConfiguration *cfg); //XXX currently not called from anywhere

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
};


#endif

