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
#include "envirdefs.h"
#include "cconfiguration.h"
#include "cconfigreader.h"
#include "stringpool.h"

NAMESPACE_BEGIN

class PatternMatcher;
class Scenario;


/**
 * Wraps a cConfigurationReader (usually an InifileReader), and presents
 * its contents on a higher level towards the simulation.
 *
 * This object "flattens out" the configuration with respect to an
 * active section, i.e. the section fallback sequence ("extends") is
 * resolved, and sections are made invisible to the user.
 */
class ENVIR_API SectionBasedConfiguration : public cConfiguration
{
  private:
    // if we make our own copy, we only need cConfigurationReader for initialization, and after that it can be disposed of
    class KeyValue1 : public cConfiguration::KeyValue {
      public:
        static std::string nullbasedir;
        const std::string *basedirRef; // points into basedirs[]
        std::string key;
        std::string value;
        KeyValue1() {basedirRef = &nullbasedir;}
        KeyValue1(const std::string *bd, const char *k, const char *v) {basedirRef = bd; key = k; value = v;}

        // virtual functions implementing the KeyValue interface
        virtual const char *getKey() const   {return key.c_str();}
        virtual const char *getValue() const {return value.c_str();}
        virtual const char *getBaseDirectory() const {return basedirRef->c_str();}
    };

    cConfigurationReader *ini;
    std::string activeConfig;
    int activeRunNumber;
    std::string runId;

    typedef std::set<std::string> StringSet;
    StringSet basedirs;  // stores ini file locations (absolute paths)

    // config entries (i.e. keys not containing a dot or wildcard)
    std::map<std::string,KeyValue1> config;  //XXX use const char * and CommonStringPool

    class KeyValue2 : public KeyValue1 {
      public:
        PatternMatcher *ownerPattern; // key without the group name (and without ".apply-default")
        PatternMatcher *groupPattern; // only filled in when this is a wildcard group
        PatternMatcher *fullPathPattern; // when present, match against this instead of ownerPattern & groupPattern
        bool isApplyDefault;  // whether key has the ".apply-default" suffix)
        bool applyDefaultValue;  // the value of the ".apply-default" key which can be true or false

        KeyValue2(const KeyValue1& e) : KeyValue1(e) {
            ownerPattern = groupPattern = fullPathPattern = NULL;
            isApplyDefault = false; applyDefaultValue = true;
        }
        KeyValue2(const KeyValue2& e) : KeyValue1(e) {
            ownerPattern = e.ownerPattern;
            groupPattern = e.groupPattern;
            fullPathPattern = e.fullPathPattern;
            isApplyDefault = e.isApplyDefault;
            applyDefaultValue = e.applyDefaultValue;
        }
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

    // predefined variables (${configname} etc) and iteration variables. id-to-value map.
    // Also stores iterator positions (i.e. the iteration counter), with key "&varname"
    typedef std::map<std::string,std::string> StringMap;
    StringMap variables;

    // storage for values returned by substituteVariables()
    CommonStringPool stringPool;

  public:
    /**
     * Used during scenario resolution: an iteration variable in the
     * configuration. An iteration spec may be in one of the following forms:
     * ${1,2,5,10}; ${x=1,2,5,10}. (Note: ${x} is just an iteration variable
     * _reference_, not an an iteration variable itself.
     *
     * varid identifies the variable; for named variables it's the same as
     * varname. For unnamed ones, it is a string like "2-5-0", composed of
     * (sectionId, entryId, index), so that it identifies the place
     * where the value has to be substituted back.
     *
     * It is also possible to do "parallel iterations": the ${1..9 ! varname}
     * notation means that "if variable varname is at its kth iteration,
     * take the kth value from 0..9 as well". That is, this iteration and
     * varname's iterator are advanced in lockstep.
     */
    struct IterationVariable {
        std::string varid;   // identifies the variable, see above
        std::string varname; // printable variable name ("x"); may be a generated one like "0"; never empty
        std::string value;   // "1,2,5..10"; never empty
        std::string parvar;  // "in parallel to" variable", as in the ${1,2,5..10 ! var} notation
    };

  private:
    void clear();
    int internalFindSection(const char *section) const;
    int internalGetSectionId(const char *section) const;
    int internalFindEntry(const char *section, const char *key) const;
    int internalFindEntry(int sectionId, const char *key) const;
    bool internalFindEntry(const std::vector<int>& sectionChain, const char *key, int& outSectionId, int& outEntryId) const;
    const char *internalGetValue(const std::vector<int>& sectionChain, const char *key) const;
    int resolveConfigName(const char *scenarioOrConfigName) const;
    std::vector<int> resolveSectionChain(int sectionId) const;
    std::vector<int> resolveSectionChain(const char *section) const;
    void addEntry(const KeyValue1& entry);
    static void splitKey(const char *key, std::string& outOwnerName, std::string& outGroupName, bool& outIsApplyDefault);
    static bool entryMatches(const KeyValue2& entry, const char *moduleFullPath, const char *paramName);
    std::vector<IterationVariable> collectIterationVariables(const std::vector<int>& sectionChain) const;
    static void parseVariable(const char *pos, std::string& outVarname, std::string& outValue, std::string& outParVar, const char *&outEndPos);
    std::string substituteVariables(const char *text, int sectionId, int entryId) const;
    bool isPredefinedVariable(const char *varname) const;
    void setupVariables(const char *scenarioOrConfigName, int runNumber, Scenario *scenario, const std::vector<int>& sectionChain);
    KeyValue1 convert(int sectionId, int entryId);
    static bool isIgnorableConfigKey(const char *ignoredKeyPatterns, const char *key);
    static cConfigKey *lookupConfigKey(const char *key);

  protected:
    // cConfiguration method
    virtual const char *substituteVariables(const char *value);

  public:
    SectionBasedConfiguration();
    virtual ~SectionBasedConfiguration();

    /**
     * This cConfiguration uses a cConfigurationReader as input; the reader
     * should be passed with this method.
     */
    virtual void setConfigurationReader(cConfigurationReader *ini);

    /** @name Methods that implement the cConfiguration interface. */
    //@{
    virtual void initializeFrom(cConfiguration *conf);
    virtual const char *getFileName() const;
    virtual void validate(const char *ignorableConfigKeys) const;
    virtual std::vector<std::string> getConfigNames();
    virtual void activateConfig(const char *scenarioOrConfigName, int runNumber=0);
    virtual std::string getConfigDescription(const char *scenarioOrConfigName) const;
    virtual std::string getBaseConfig(const char *scenarioOrConfigName) const;
    virtual int getNumRunsInScenario(const char *scenarioName) const;
    virtual std::vector<std::string> unrollScenario(const char *scenarioName, bool detailed) const;
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
    virtual std::vector<const char *> getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const;
    virtual const char *getVariable(const char *varname) const;
    virtual std::vector<const char *> getIterationVariableNames() const;
    virtual std::vector<const char *> getPredefinedVariableNames() const;
    virtual void dump() const;
    //@}
};

NAMESPACE_END


#endif


