//=========================================================================
//  SCALARMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCALARMANAGER_H_
#define _SCALARMANAGER_H_

#include <string>
#include <vector>
#include <set>
#include <map>

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

    struct File
    {
        std::string filePath;
        std::string directory;
        std::string fileName;
        IntVector runNumbers;
    };

    typedef std::map<std::string,File> FileMap;
    typedef FileMap::const_iterator FileRef;

    typedef std::set<std::string> StringSet;
    typedef StringSet::const_iterator StringRef;

    struct Datum
    {
        FileRef fileRef;
        int runNumber;
        StringRef moduleNameRef;
        StringRef scalarNameRef;
        double value;
    };

    typedef std::vector<Datum> Values;

  private:
    FileMap scalarFiles;
    StringSet moduleNames;
    StringSet scalarNames;

    Values scalarValues;

    StringRef lastInsertedModuleRef;

    void processLine(char *&line, FileMap::iterator fileRef, int& runNumber);

  public:
    ScalarManager();
    ~ScalarManager();

    const Values& getValues() const  {return scalarValues;}
    const Datum& getValue(int id) const  {return scalarValues[id];}
    const FileMap& getFiles() const  {return scalarFiles;}
    const StringSet& getModuleNames() const  {return moduleNames;}
    const StringSet& getScalarNames() const  {return scalarNames;}

    void addValue(FileRef fileRef, int runNumber, const char *moduleName, const char *scalarName, double value);

    FileRef loadFile(const char *filename);

    void dump(FileRef fileRef, int runNumber, std::ostream out) const;
};

#endif


