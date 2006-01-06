//=========================================================================
//  SCALARMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCALARMANAGER_H_
#define _SCALARMANAGER_H_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "texception.h"


/**
 * Loads and efficiently stores an OMNeT++ output scalar file. (In fact,
 * several output scalar files.)
 *
 * Gives access to the list of module names and scalar data labels used in
 * the file as well as the list of all data.
 *
 * Data can be extracted, filtered, etc by external functions.
 */
class ScalarManager
{
  public:
    typedef std::vector<int> IntVector;
    typedef std::set<std::string> StringSet;
    typedef StringSet::const_iterator StringRef;
    typedef std::vector<StringRef> StringRefVector;

    struct File
    {
        std::string filePath; // directory + fileName
        std::string directory;
        std::string fileName;
    };

    typedef std::list<File> FileList;
    typedef FileList::const_iterator FileRef;

    struct Run
    {
        FileRef fileRef;
        int runNumber;
        std::string networkName;
        std::string date;
        int lineNum;

        // these fields are concatenation of the above ones, cached for fast string matching
        std::string runName;
        std::string fileAndRunName;
    };

    typedef std::list<Run> RunList;
    typedef RunList::const_iterator RunRef;

    struct Datum
    {
        RunRef runRef;
        StringRef moduleNameRef;
        StringRef scalarNameRef;
        double value;
    };

    typedef std::vector<Datum> Values;

  private:
    FileList fileList;
    RunList runList;
    StringSet moduleNames;
    StringSet scalarNames;
    StringSet classNames;

    Values scalarValues;

    StringRef lastInsertedModuleRef;

  private:
    // utility, called during processing one line
    void processLine(char **vec, int numtokens, RunRef& runRef, FileRef fileRef, int lineNum);

    static StringRef stringMapFindOrInsert(StringSet& set, const std::string& str);

  public:
    ScalarManager();
    ~ScalarManager();

    const Values& getValues() const  {return scalarValues;}
    const Datum& getValue(int id) const  {return scalarValues[id];}
    const FileList& getFiles() const  {return fileList;}
    const RunList& getRuns() const  {return runList;}
    const StringSet& getModuleNames() const  {return moduleNames;}
    const StringSet& getScalarNames() const  {return scalarNames;}

    void addValue(RunRef runRef, const char *moduleName, const char *scalarName, double value);

    FileRef loadFile(const char *filename);

    void dump(FileRef fileRef, std::ostream& out) const;
};

#endif


