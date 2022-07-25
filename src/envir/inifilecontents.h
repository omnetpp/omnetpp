//==========================================================================
//  INIFILECONTENTS.H - part of
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

#ifndef __OMNETPP_ENVIR_INIFILECONTENTS_H
#define __OMNETPP_ENVIR_INIFILECONTENTS_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include "common/pooledstring.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigurationreader.h"
#include "omnetpp/fileline.h"
#include "envirdefs.h"
#include "scenario.h"

using omnetpp::common::opp_staticpooledstring;

namespace omnetpp {
namespace envir {

class ENVIR_API InifileContents
{
  public:
    typedef std::map<std::string,std::string> StringMap;

    class Entry : public cConfiguration::KeyValue {
      public:
        opp_staticpooledstring baseDir;
        std::string key;
        std::string value;
        FileLine loc;
        Entry() {}
        Entry(const char *baseDir, const char *k, const char *v, FileLine loc=FileLine()) : baseDir(baseDir), key(k), value(v), loc(loc) {}

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const override   {return key.c_str();}
        virtual const char *getValue() const override {return value.c_str();}
        virtual const char *getBaseDirectory() const override {return baseDir.c_str();}
        virtual FileLine getSourceLocation() const override {return loc;}
    };

    /**
     * @brief Struct used by unrollConfig() to return information.
     */
    struct RunInfo {
        std::string info; // concatenated
        std::map<std::string,std::string> runAttrs;
        std::map<std::string,std::string> iterVars;
        std::string configBrief; // config options that contain inifile variables (${foo}), expanded
    };

  private:
    struct VariablesInfo {
        StringMap locationToVarName; // for unnamed variables
        StringMap iterationVariables;
        StringMap predefinedVariables;
        StringMap allVariables; // union of the previous two
    };

    std::string rootFilename;  // name of "root" ini file read
    std::string defaultBasedir; // the default base directory

    struct Section {
        std::string name;
        std::vector<Entry> entries;
    };

    std::vector<Section> sections;
    std::vector<Entry> commandLineOptions;

    // section inheritance chains, computed from the input data
    mutable std::vector<std::vector<int> > cachedSectionChains;

  private:
    static void parseVariable(const char *txt, std::string& outVarname, std::string& outValue, std::string& outParVar, const char *&outEndPtr);
    static bool isIgnorableConfigKey(const char *ignoredKeyPatterns, const char *key);
    static cConfigOption *lookupConfigOption(const char *key);
    static void splitKey(const char *key, std::string& outOwnerName, std::string& outBinName);
    static bool isPredefinedVariable(const char *varname);
    static bool isGlobalOrPerRunOption(const char *key);

    const Section& getSection(int sectionId) const;
    void checkSectionIndex(int sectionId) const;
    void checkEntryIndex(const Section& section, int entryId) const;
    const char *internalGetValue(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue=nullptr, int *sectionIdPtr=nullptr, int *entryIdPtr=nullptr) const;
    intval_t internalGetValueAsInt(const std::vector<int>& sectionChain, const char *key, intval_t fallbackValue, int *sectionIdPtr=nullptr, int *entryIdPtr=nullptr) const;
    std::string internalGetValueAsString(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue=nullptr, int *sectionIdPtr=nullptr, int *entryIdPtr=nullptr) const;
    int resolveConfigName(const char *configName) const;
    std::vector<int> resolveSectionChain(int sectionId) const;
    std::vector<int> resolveSectionChain(const char *configName) const;
    std::vector<int> computeSectionChain(int sectionId) const;
    std::vector<int> getBaseConfigIds(int sectionId) const;
    std::vector<Scenario::IterationVariable> collectIterationVariables(const std::vector<int>& sectionChain, StringMap& outLocationToNameMap) const;
    std::string substituteVariables(const char *text, const VariablesInfo& variables, int sectionId, int entryId) const;
    VariablesInfo computeVariables(const char *configName, int runNumber, std::vector<int> sectionChain, const Scenario *scenario, const StringMap& locationToVarName) const;
    std::string internalGetConfigAsString(cConfigOption *option, const std::vector<int>& sectionChain, const VariablesInfo& variables) const;
    intval_t internalGetConfigAsInt(cConfigOption *option, const std::vector<int>& sectionChain, const VariablesInfo& variables) const;
    bool internalGetConfigAsBool(cConfigOption *option, const std::vector<int>& sectionChain, const VariablesInfo& variables) const;
    void invalidateCaches();

  public:
    InifileContents() {}
    InifileContents(const char *filename) {readFile(filename);}
    virtual ~InifileContents();

    virtual void readUsing(cConfigurationReader *reader, cConfiguration *readerConfig);
    virtual void readFile(const char *filename);
    virtual void readText(const char *text, const char *filename, const char *baseDir);
    virtual void readStream(std::istream& in, const char *filename, const char *baseDir);

   /**
    * Specifies config options passed on the command line. These options
    * are global (effective for all configs), cannot be per-object options,
    * and take precedence over the ones read from the ini file (i.e. from
    * cConfigurationReader). This method immediately validates the option
    * names, and throws an error for unrecognized/unacceptable ones.
    * The baseDir parameter is going to be used for resolving filename
    * options that contain relative paths.
    */
   virtual void setCommandLineConfigOptions(const std::map<std::string,std::string>& options, const char *baseDir);

   /** @name Accessing content */
   //@{
   virtual const char *getFileName() const;
   virtual const char *getDefaultBaseDirectory() const;
   virtual int getNumSections() const;
   virtual const char *getSectionName(int sectionId) const;
   virtual int getNumEntries(int sectionId) const;
   virtual const Entry& getEntry(int sectionId, int entryId) const;

   virtual int findSection(const char *section) const;
   virtual int addSection(const char *sectionName);
   virtual int findOrAddSection(const char *section);
   virtual void insertSection(int atSectionId, const char *sectionName);
   virtual int findEntry(int sectionId, const char *key) const;
   virtual int addEntry(int sectionId, const Entry& entry);
   virtual void insertEntry(int sectionId, int atEntryId, const Entry& entry);
   virtual void deleteSection(int sectionId); // note: subsequent sections are shifted down
   virtual void deleteEntry(int sectionId, int entryId); // note: subsequent entries are shifted down
   virtual void clear();
   //@}

   virtual std::vector<std::string> getPredefinedVariableNames() const;
   virtual const char *getVariableDescription(const char *varname) const;

   virtual void validate(const char *ignorableConfigKeys) const;
   virtual std::vector<std::string> getConfigNames();
   virtual cConfiguration *extractGlobalConfig();
   virtual cConfiguration *extractConfig(const char *configName, int runNumber=0);
   virtual std::string getConfigDescription(const char *configName) const;
   virtual std::vector<std::string> getBaseConfigs(const char *configName) const;
   virtual std::vector<std::string> getConfigChain(const char *configName) const;
   virtual int getNumRunsInConfig(const char *configName) const;
   virtual std::vector<RunInfo> unrollConfig(const char *configName) const;
   virtual void dump(bool printBaseDir=true) const;
};


}  // namespace envir
}  // namespace omnetpp


#endif


