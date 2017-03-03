//==========================================================================
//  SECTIONBASEDCONFIG.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_SECTIONBASEDCONFIG_H
#define __OMNETPP_ENVIR_SECTIONBASEDCONFIG_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include "common/stringpool.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigreader.h"
#include "envirdefs.h"
#include "scenario.h"

namespace omnetpp {

namespace common { class PatternMatcher; };

namespace envir {

class Scenario;

/**
 * Wraps a cConfigurationReader (usually an InifileReader), and presents
 * its contents on a higher level towards the simulation.
 *
 * This object "flattens out" the configuration with respect to an
 * active section, i.e. the section fallback sequence ("extends") is
 * resolved, and sections are made invisible to the user.
 */
class ENVIR_API SectionBasedConfiguration : public cConfigurationEx
{
  private:
    typedef omnetpp::common::StringPool StringPool;
    typedef omnetpp::common::PatternMatcher PatternMatcher;
    typedef std::set<std::string> StringSet;
    typedef std::map<std::string,std::string> StringMap;

    class Entry : public cConfiguration::KeyValue {
      public:
        static std::string nullBasedir;
        const std::string *basedirRef; // points into basedirs[]
        std::string key;
        std::string value;
        Entry() {basedirRef = &nullBasedir;}
        Entry(const std::string *bd, const char *k, const char *v) {basedirRef = bd; key = k; value = v;}

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const override   {return key.c_str();}
        virtual const char *getValue() const override {return value.c_str();}
        virtual const char *getBaseDirectory() const override {return basedirRef->c_str();}
    };

    class MatchableEntry : public Entry {
      public:
        PatternMatcher *ownerPattern; // key without the suffix
        PatternMatcher *suffixPattern; // only filled in when this is a wildcard group
        PatternMatcher *fullPathPattern; // when present, match against this instead of ownerPattern & suffixPattern

        MatchableEntry(const Entry& e) : Entry(e) {ownerPattern = suffixPattern = fullPathPattern = nullptr;}
        MatchableEntry(const MatchableEntry& e);
        ~MatchableEntry();
    };

    //
    // getConfigEntry() etc return a reference to nullEntry when the requested key is not found
    //
    class NullEntry : public cConfiguration::KeyValue
    {
      private:
        std::string defaultBasedir;
      public:
        void setBaseDirectory(const char *s) {defaultBasedir = s;}
        virtual const char *getKey() const override   {return nullptr;}
        virtual const char *getValue() const override {return nullptr;}
        virtual const char *getBaseDirectory() const override {return defaultBasedir.c_str();}
    };

    // Some explanation. Basically we could just store all entries in order,
    // and when a param fullPath etc comes in, just match it against all entries
    // linearly. However, we optimize on this: the parameter names in the keys
    // rarely contain wildcards, so if we group all entries by parameter
    // name, we can find the right group by just one map lookup, thereby significantly
    // reducing the number of entries to match against. For example,
    // if the parameter "Net.host[1].radio.power" comes in, we only match it
    // against the group which contains the keys ending in ".power" (e.g.
    // "**.server.radio.power", "**.host[0].**.power", etc).
    // If there is an entry which contains a wildcard in the parameter name part,
    // that unfortunately has to be added to all suffix groups.
    //
    // Examples:
    // Parameter keys:
    //   **.host[*].address  ==> goes into the "address" suffix group; ownerPattern="**.host[*]"
    //   **.host[*].addr*    ==> goes into the wildcard suffix group; ownerPattern="**.host[*]", suffixPattern="addr*"
    //   **.address          ==> goes into the "address" suffix group; ownerPattern="**"
    //   **.addr*            ==> goes into the wildcard suffix group; ownerPattern="**"
    //   **                  ==> goes into the wildcard suffix group as "*"; ownerPattern="**"
    //   **.**               ==> goes into the wildcard suffix group as "*"; ownerPattern="**"
    //   **-*                ==> goes into the wildcard suffix group as "*-*"; ownerPattern="**"
    //   **-**               ==> goes into the wildcard suffix group as "*-*"(?); ownerPattern="**"
    //
    // Per-object config keys:
    //   **.tcp.eedVector.record-interval ==> goes into the "record-interval" suffix group; ownerPattern="**.tcp.eedVector"
    //   **.tcp.eedVector.record-*"       ==> goes into the wildcard suffix group; ownerPattern="**.tcp.eedVector", suffixPattern="record-*"
    //
    struct SuffixGroup {
        std::vector<MatchableEntry> entries;
    };

  private:
    // input data
    cConfigurationReader *ini;
    std::vector<Entry> commandLineOptions;

    // section inheritance chains, computed from the input data
    mutable std::vector<std::vector<int> > cachedSectionChains;

    // THE ACTIVE CONFIGURATION:

    std::string activeConfig;
    int activeRunNumber;
    std::string runId;

    StringSet basedirs;  // stores ini file locations (absolute paths)

    std::vector<Entry> entries; // entries of the activated configuration, with itervars substituted
    std::map<std::string,Entry> config; // config entries (i.e. keys not containing a dot or wildcard)
    std::map<std::string,SuffixGroup> suffixGroups;
    SuffixGroup wildcardSuffixGroup;

    // predefined variables (${configname} etc) and iteration variables
    StringMap variables;  // varName-to-value map
    StringMap locationToVarName; // location-to-varName, for identifying unnamed variables inside substituteVariables()

    NullEntry nullEntry;

    // storage for values returned by substituteVariables()
    mutable StringPool stringPool;

  private:
    void clear();
    void activateGlobalConfig();
    int internalFindSection(const char *section) const;
    int internalGetSectionId(const char *section) const;
    int internalFindEntry(int sectionId, const char *key) const;
    const char *internalGetValue(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue=nullptr, int *sectionIdPtr=nullptr, int *entryIdPtr=nullptr) const;
    int resolveConfigName(const char *configName) const;
    std::vector<int> resolveSectionChain(int sectionId) const;
    std::vector<int> resolveSectionChain(const char *configName) const;
    std::vector<int> computeSectionChain(int sectionId) const;
    std::vector<int> getBaseConfigIds(int sectionId) const;
    void addEntry(const Entry& entry);
    static void splitKey(const char *key, std::string& outOwnerName, std::string& outGroupName);
    static bool entryMatches(const MatchableEntry& entry, const char *moduleFullPath, const char *paramName);
    std::vector<Scenario::IterationVariable> collectIterationVariables(const std::vector<int>& sectionChain, StringMap& outLocationToNameMap) const;
    static void parseVariable(const char *pos, std::string& outVarname, std::string& outValue, std::string& outParVar, const char *&outEndPos);
    std::string substituteVariables(const char *text, int sectionId, int entryId, const StringMap& variables, const StringMap& locationToVarName) const;
    bool isPredefinedVariable(const char *varname) const;
    StringMap computeVariables(const char *configName, int runNumber, std::vector<int> sectionChain, const Scenario *scenario, const StringMap& locationToVarName) const;
    std::string resolveConfigOption(cConfigOption *option, const std::vector<int>& sectionChain, const StringMap& variables, const StringMap& locationToVarName) const;
    static bool isIgnorableConfigKey(const char *ignoredKeyPatterns, const char *key);
    static cConfigOption *lookupConfigOption(const char *key);
    const std::string *getPooledBaseDir(const char *basedir);

  public:
    SectionBasedConfiguration();
    virtual ~SectionBasedConfiguration();

    /**
     * This cConfiguration uses a cConfigurationReader as input; the reader
     * should be passed with this method.
     */
    virtual void setConfigurationReader(cConfigurationReader *ini);

    /**
     * Returns the pointer of current cConfigurationReader.
     */
    virtual cConfigurationReader *getConfigurationReader() { return ini; }

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

    /** @name Methods that implement the cConfiguration(Ex) interface. */
    //@{
    virtual void initializeFrom(cConfiguration *bootConfig) override;
    virtual const char *getFileName() const override;
    virtual void validate(const char *ignorableConfigKeys) const override;
    virtual std::vector<std::string> getConfigNames() override;
    virtual void activateConfig(const char *configName, int runNumber=0) override;
    virtual std::string getConfigDescription(const char *configName) const override;
    virtual std::vector<std::string> getBaseConfigs(const char *configName) const override;
    virtual std::vector<std::string> getConfigChain(const char *configName) const override;
    virtual int getNumRunsInConfig(const char *configName) const override;
    virtual std::vector<RunInfo> unrollConfig(const char *configName) const override;
    virtual const char *getActiveConfigName() const override;
    virtual int getActiveRunNumber() const override;
    virtual const char *getConfigValue(const char *key) const override;
    virtual const KeyValue& getConfigEntry(const char *key) const override;
    virtual std::vector<const char *> getMatchingConfigKeys(const char *pattern) const override;
    virtual const char *getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const override;
    virtual const KeyValue& getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const override;
    virtual std::vector<const char *> getKeyValuePairs() const override;
    virtual std::vector<const char *> getParameterKeyValuePairs() const override;
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const override;
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const override;
    virtual std::vector<const char *> getMatchingPerObjectConfigKeys(const char *objectFullPath, const char *keySuffixPattern) const override;
    virtual std::vector<const char *> getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const override;
    virtual const char *getVariable(const char *varname) const override;
    virtual std::vector<const char *> getIterationVariableNames() const override;
    virtual std::vector<const char *> getPredefinedVariableNames() const override;
    virtual const char *getVariableDescription(const char *varname) const override;
    virtual const char *substituteVariables(const char *value) const override;
    virtual void dump() const override;
    //@}
};

} // namespace envir
}  // namespace omnetpp


#endif


