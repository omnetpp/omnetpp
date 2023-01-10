//==========================================================================
//  CONFIGURATION.H - part of
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

#ifndef __OMNETPP_ENVIR_CONFIGURATION_H
#define __OMNETPP_ENVIR_CONFIGURATION_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include "common/pooledstring.h"
#include "omnetpp/cconfiguration.h"
#include "envirdefs.h"
#include "inifilecontents.h"

namespace omnetpp {

namespace common { class PatternMatcher; };

namespace envir {


/**
 * Wraps a cConfigurationReader (usually an InifileReader), and presents
 * its contents on a higher level towards the simulation.
 *
 * This object "flattens out" the configuration with respect to an
 * active section, i.e. the section fallback sequence ("extends") is
 * resolved, and sections are made invisible to the user.
 */
class ENVIR_API Configuration : public cConfiguration  //TODO rename Configuration, cIConfiguration?
{
  private:
    typedef omnetpp::common::opp_staticpooledstring opp_staticpooledstring;
    typedef omnetpp::common::PatternMatcher PatternMatcher;
    typedef std::set<std::string> StringSet;
    typedef std::map<std::string,std::string> StringMap;

    class Entry : public InifileContents::Entry {
      private:
        mutable bool accessed = false;
      public:
        Entry() {}
        Entry(const InifileContents::Entry& e) : InifileContents::Entry(e) {}
        Entry(const char *baseDir, const char *key, const char *value, FileLine loc=FileLine()) : InifileContents::Entry(baseDir, key, value, loc) {}

        bool isAccessed() const {return accessed;}
        Entry& markAccessed() {accessed = true; return *this;}
        const Entry& markAccessed() const {accessed = true; return *this;}
        void clearAccessInfo() {accessed = false;}
    };

    class MatchableEntry : public Entry {
      public:
        PatternMatcher *ownerPattern = nullptr; // key without the suffix
        PatternMatcher *suffixPattern = nullptr; // only filled in when this is a wildcard bin
        PatternMatcher *fullPathPattern = nullptr; // when present, match against this instead of ownerPattern & suffixPattern

        MatchableEntry(const Entry& e) : Entry(e) {}
        MatchableEntry(const MatchableEntry& e);   // apparently only used for std::vector storage
        ~MatchableEntry();
        std::string debugStr() const;
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
    // rarely contain wildcards, so if we sort the entries into bins by parameter
    // name, we can find the right bin by just one map lookup, thereby significantly
    // reducing the number of entries to match against. For example,
    // if the parameter "Net.host[1].radio.power" comes in, we only match it
    // against the bin which contains the keys ending in ".power" (e.g.
    // "**.server.radio.power", "**.host[0].**.power", etc).
    // If there is an entry which contains a wildcard in the parameter name part,
    // that unfortunately has to be added to all bins.
    //
    // Examples:
    // Parameter keys:
    //   **.host[*].address  ==> goes into the "address" bin; ownerPattern="**.host[*]"
    //   **.host[*].addr*    ==> goes into the wildcard bin; ownerPattern="**.host[*]", suffixPattern="addr*"
    //   **.address          ==> goes into the "address" bin; ownerPattern="**"
    //   **.addr*            ==> goes into the wildcard bin; ownerPattern="**"
    //   **                  ==> goes into the wildcard bin as "*"; ownerPattern="**"
    //   **.**               ==> goes into the wildcard bin as "*"; ownerPattern="**"
    //   **-*                ==> goes into the wildcard bin as "*-*"; ownerPattern="**"
    //   **-**               ==> goes into the wildcard bin as "*-*"(?); ownerPattern="**"
    //
    // Per-object config keys:
    //   **.tcp.eedVector.record-interval ==> goes into the "record-interval" bin; ownerPattern="**.tcp.eedVector"
    //   **.tcp.eedVector.record-*"       ==> goes into the wildcard bin; ownerPattern="**.tcp.eedVector", suffixPattern="record-*"
    //
    struct SuffixBin {
        std::vector<MatchableEntry> entries;
    };

  private:
    std::vector<Entry> entries; // entries of the activated configuration, with itervars substituted
    std::map<std::string,Entry> config; // config entries (i.e. keys not containing a dot or wildcard)
    std::map<std::string,SuffixBin> suffixBins;  // bins for each non-wildcard suffix
    SuffixBin wildcardSuffixBin; // bin for entries that contain wildcards

    // predefined variables (${configname} etc) and iteration variables
    StringMap predefinedVariables;
    StringMap iterationVariables;
    StringMap allVariables; // union of the two

    NullEntry nullEntry;
    std::string fileName;

  private:
    void addEntry(const Entry& entry);
    static void parseVariable(const char *txt, std::string& outVarname, std::string& outValue, std::string& outParVar, const char *&outEndPtr);
    static void splitKey(const char *key, std::string& outOwnerName, std::string& outBinName);
    static bool entryMatches(const MatchableEntry& entry, const char *moduleFullPath, const char *paramName);
    static bool isPredefinedVariable(const char *varname);
    virtual bool isEssentialOption(const char *key) const;
    virtual std::string substituteVariables(const char *text, const StringMap& variables) const;

  public:
    Configuration() {}
    Configuration(const std::vector<InifileContents::Entry>& entries, const StringMap& predefinedVariables, const StringMap& iterationVariables, const char *fileName=nullptr);
    virtual ~Configuration();

    virtual const char *getFileName() const override;
    virtual const char *getConfigValue(const char *key) const override;
    virtual const KeyValue& getConfigEntry(const char *key) const override;
    virtual std::vector<const char *> getMatchingConfigKeys(const char *pattern) const override;
    virtual const char *getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const override;
    virtual const KeyValue& getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const override;
    virtual std::vector<const char *> getKeyValuePairs(int flags) const override;
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const override;
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const override;
    virtual std::vector<const char *> getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const override;
    virtual std::map<std::string,std::string> getPredefinedVariables() const override;
    virtual std::map<std::string,std::string> getIterationVariables() const override;
    virtual const char *getVariable(const char *varname) const override;
    virtual const char *substituteVariables(const char *value) const override;
    virtual std::vector<const KeyValue*> getUnusedEntries(bool all=false) const override;
    virtual void clearUsageInfo() override;
    virtual void dump() const;
    virtual void dumpUnusedEntries() const;

};

}  // namespace envir
}  // namespace omnetpp


#endif


