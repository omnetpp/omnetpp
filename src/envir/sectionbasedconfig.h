//==========================================================================
//  SECTIONBASEDCONFIG.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef SECTIONBASEDCONFIG_H_
#define SECTIONBASEDCONFIG_H_

#include <map>
#include <vector>
#include <set>
#include <string>
#include "cconfig.h"
#include "configreader.h"

class PatternMatcher;


/**
 * Wraps an ConfigurationReader, and presents the contents of an inifile
 * on a higher level towards the simulation.
 *
 * This object "flattens out" the configuration with respect to an
 * active section, i.e. the section fallback sequence ("extends") is
 * resolved, and sections are made invisible to the user.
 */
class SectionBasedConfiguration : public cConfiguration
{
  private:
    // if we make our own copy, we only need ConfigurationReader for initialization, and after that it can be disposed of
    class KeyValue1 : public cConfiguration::KeyValue {
      public:
        static std::string nullbasedir;
        std::string *basedirRef; // points into basedirs[]
        std::string key;
        std::string value;
        KeyValue1() {basedirRef = &nullbasedir;}
        KeyValue1(std::string *bd, const char *k, const char *v) {basedirRef = bd; key = k; value = v;}

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const   {return key.c_str();}
        virtual const char *getValue() const {return value.c_str();}
        virtual const char *getBaseDirectory() const {return basedirRef->c_str();}
    };

    ConfigurationReader *ini;
    std::string activeConfig;
    int activeRunNumber;

    typedef std::set<std::string> StringSet;
    StringSet basedirs;  // stores ini file locations (absolute paths)

    // config entries (i.e. keys not containing a dot or wildcard)
    std::map<std::string,KeyValue1> config;  //XXX use const char * and StringPool

    class KeyValue2 : public KeyValue1 {
      public:
        PatternMatcher *ownerPattern; // key without the group name (and without ".apply-default")
        PatternMatcher *groupPattern; // only filled in when this is a wildcard group
        bool isApplyDefault;  // whether key has the ".apply-default" suffix)
        bool applyDefaultValue;  // the value of the ".apply-default" key which can be true or false

        KeyValue2(const KeyValue1& e) : KeyValue1(e) {ownerPattern = groupPattern = NULL; isApplyDefault = false; applyDefaultValue = true;}
        KeyValue2(const KeyValue2& e) : KeyValue1(e) {ownerPattern = e.ownerPattern; groupPattern = e.groupPattern; isApplyDefault = e.isApplyDefault; applyDefaultValue = e.applyDefaultValue; }
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
    // that unfortunately has to be added to all groups. Same thing for all the
    // "apply-default" entries: they must be added to the same group as the parameter
    // entries (because relative order matters).
    //
    // Examples:
    // Parameter keys:
    //   **.host[*].address               ==> goes into the "address" group; ownerPattern="**.host[*]"
    //   **.host[*].address.apply-default ==> goes into the "address" group; ownerPattern="**.host[*]" and isApplyDefault=true
    //   **.host[*].addr*                 ==> goes into the wildcard group; ownerPattern="**.host[*]", groupPattern="addr*"
    //   **.host[*].addr*.apply-default   ==> goes into the wildcard group; ownerPattern="**.host[*]", groupPattern="addr*" and isApplyDefault=true
    //
    // Per-object config keys:
    //   **.tcp.eedVector.record-interval ==> goes into the "record-interval" group; ownerPattern="**.tcp.eedVector"
    //   **.tcp.eedVector.record-*"       ==> goes into the wildcard group; ownerPattern="**.tcp.eedVector", groupPattern="record-*"
    //
    struct Group {
        std::vector<KeyValue2> entries;
    };

    std::map<std::string,Group> groups;
    Group wildcardGroup;

    // getConfigEntry() etc return a reference to nullEntry when the
    // requested key is not found
    class NullKeyValue : public cConfiguration::KeyValue
    {
      private:
        std::string defaultBasedir;
      public:
        void setBaseDirectory(const char *s) {defaultBasedir = s;}
        virtual const char *getKey() const   {return NULL;}
        virtual const char *getValue() const {return NULL;}
        virtual const char *getBaseDirectory() const {return defaultBasedir.c_str();}
    };
    NullKeyValue nullEntry;

  public:
    /**
     * Used during scenario resulution: stores the location of an iteration
     * spec "${...}" in the configuration. An iteration spec may be in
     * one of the following forms: ${1,2,5,10}; ${x=1,2,5,10}; $x or ${x}.
     */
    struct IterationSpec {
        int entryId;
        int startPos;
        int length;
        std::string varname; // "x"; may be empty
        std::string value;   // "1,2,5,10"; may be empty
    };

  private:
    void clear();
    int internalFindSection(const char *section) const;
    int internalGetSectionId(const char *section) const;
    int internalFindEntry(const char *section, const char *key) const;
    int internalFindEntry(int sectionId, const char *key) const;
    int resolveConfigName(const char *scenarioOrConfigName) const;
    std::vector<int> resolveSectionChain(const char *section) const;
    void addEntry(const KeyValue1& entry);
    static void splitKey(const char *key, std::string& outOwnerName, std::string& outGroupName, bool& outIsApplyDefault);
    void validateConfig() const;
    std::vector<IterationSpec> collectIterationSpecs(int sectionId) const;
    void validateIterations(const std::vector<IterationSpec>& list) const;
    KeyValue1 convert(const ConfigurationReader::KeyValue& e);
    void doActivateConfig(int sectionId);
    void doActivateScenario(int sectionId, int runNumber);
    int internalGetNumRunsInScenario(int sectionId) const;

  public:
    SectionBasedConfiguration();
    virtual ~SectionBasedConfiguration();

    /**
     *XXX
     */
    virtual void setConfigurationReader(ConfigurationReader *ini);

    /** @name Methods that implement the cConfiguration interface. */
    //@{
    virtual void initializeFrom(cConfiguration *conf);
    virtual const char *getFileName() const;
    virtual std::vector<std::string> getConfigNames();
    virtual void activateConfig(const char *scenarioOrConfigName, int runNumber=0);
    virtual std::string getConfigDescription(const char *scenarioOrConfigName) const;
    virtual int getNumRunsInScenario(const char *scenarioName) const;
    virtual std::string unrollScenario(const char *scenarioName) const;
    virtual const char *getActiveConfigName() const;
    virtual int getActiveRunNumber() const;
    virtual const char *getConfigValue(const char *key) const;
    virtual const KeyValue& getConfigEntry(const char *key) const;
    virtual std::vector<const char *> getMatchingConfigKeys(const char *pattern) const;
    virtual const char *getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const;
    virtual const KeyValue& getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const;
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const;
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const;
    virtual std::vector<const char *> getMatchingPerObjectConfigKeys(const char *objectFullPath, const char *keySuffixPattern) const;
    virtual void dump() const;
    //@}
};

#endif


