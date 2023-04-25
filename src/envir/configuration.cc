//==========================================================================
//  CONFIGURATION.CC - part of
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

#include "common/opp_ctype.h"
#include "common/patternmatcher.h"
#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/cexception.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfigoption.h"
#include "configuration.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

extern cConfigOption *CFGID_NETWORK;
extern cConfigOption *CFGID_SIM_TIME_LIMIT;

namespace envir {

extern cConfigOption *CFGID_REPEAT;

Register_Class(Configuration);

#define VARPOS_PREFIX    std::string("&")

Configuration::MatchableEntry::MatchableEntry(const MatchableEntry& e) :
    Entry(e),
    ownerPattern(e.ownerPattern ? new PatternMatcher(*e.ownerPattern) : nullptr),
    suffixPattern(e.suffixPattern ? new PatternMatcher(*e.suffixPattern) : nullptr),
    fullPathPattern(e.fullPathPattern ? new PatternMatcher(*e.fullPathPattern) : nullptr)
{
}

Configuration::MatchableEntry::~MatchableEntry()
{
    delete ownerPattern;
    delete suffixPattern;
    delete fullPathPattern;
}

std::string Configuration::MatchableEntry::debugStr() const
{
    return std::string("ownerPattern=") + (ownerPattern ? ownerPattern->str() : "nullptr") +
           " suffixPattern=" + (suffixPattern ? suffixPattern->str() : "nullptr") +
           " fullPathPattern=" + (fullPathPattern ? fullPathPattern->str() : "nullptr");
}

//----

Configuration::Configuration(const std::vector<InifileContents::Entry>& entries, const StringMap& predefinedVars, const StringMap& iterationVars, const char *fileName)
{
    for (const InifileContents::Entry& entry : entries)
        addEntry(Entry(entry));

    predefinedVariables = predefinedVars;
    iterationVariables = iterationVars;
    allVariables = unionOf(predefinedVariables, iterationVariables);

    fileName = opp_nulltoempty(fileName);
}

Configuration::~Configuration()
{
    for (Entry *entry : entries)
        delete entry;
}

const char *Configuration::getFileName() const
{
    return fileName.empty() ? nullptr : fileName.c_str();
}

void Configuration::parseVariable(const char *txt, std::string& outVarname, std::string& outValue, std::string& outParvar, const char *& outEndPtr)
{
    Assert(txt[0] == '$' && txt[1] == '{');  // this is the way we've got to be invoked

    StringTokenizer tokenizer(txt+2, "}", StringTokenizer::NO_TRIM | StringTokenizer::HONOR_QUOTES | StringTokenizer::HONOR_PARENS); // NO_TRIM is important because do strlen(token) to find the "}"!
    const char *token = tokenizer.nextToken();  // ends at matching '}'
    if (!token)
        throw cRuntimeError("Missing '}' for '${'");
    outEndPtr = txt + 2 + strlen(token) + 1;
    Assert(*(outEndPtr-1) == '}');

    // parse what's inside the ${...}
    const char *varNameBegin = nullptr;
    const char *varNameEnd = nullptr;
    const char *valueBegin = nullptr;
    const char *valueEnd = nullptr;
    const char *parvarBegin = nullptr;
    const char *parvarEnd = nullptr;

    const char *s = txt+2;
    while (opp_isspace(*s))
        s++;
    if (opp_isalphaext(*s)) {
        // must be a variable or a variable reference
        varNameBegin = varNameEnd = s;
        while (opp_isalnumext(*varNameEnd))
            varNameEnd++;
        s = varNameEnd;
        while (opp_isspace(*s))
            s++;
        if (*s == '}') {
            // ${x} syntax -- OK
        }
        else if (*s == '=' && *(s+1) != '=') {
            // ${x=...} syntax -- OK
            valueBegin = s+1;
        }
        else if (strchr(s, ',')) {
            // part of a valuelist -- OK
            valueBegin = varNameBegin;
            varNameBegin = varNameEnd = nullptr;
        }
        else {
            throw cRuntimeError("Missing '=' after '${varname'");
        }
    }
    else {
        valueBegin = s;
    }
    valueEnd = outEndPtr-1;

    if (valueBegin) {
        // try to parse parvar, present when value ends in "! variable"
        const char *exclamationMark = strrchr(valueBegin, '!');
        if (exclamationMark) {
            const char *s = exclamationMark+1;
            while (opp_isspace(*s))
                s++;
            if (opp_isalphaext(*s)) {
                parvarBegin = s;
                while (opp_isalnumext(*s))
                    s++;
                parvarEnd = s;
                while (opp_isspace(*s))
                    s++;
                if (s != valueEnd) {
                    parvarBegin = parvarEnd = nullptr;  // no parvar after all
                }
            }
            if (parvarBegin) {
                valueEnd = exclamationMark;  // chop off "!parvarname"
            }
        }
    }

    outVarname = outValue = outParvar = "";
    if (varNameBegin)
        outVarname.assign(varNameBegin, varNameEnd-varNameBegin);
    if (valueBegin)
        outValue.assign(valueBegin, valueEnd-valueBegin);
    if (parvarBegin)
        outParvar.assign(parvarBegin, parvarEnd-parvarBegin);
}

std::string Configuration::substituteVariables(const char *text, const StringMap& variables) const
{
    //TODO perhaps use opp_substitutevariables() here
    std::string result = opp_nulltoempty(text);
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        std::string varName, dummy1, dummy2;
        const char *endPtr;
        parseVariable(result.c_str() + pos, varName, dummy1, dummy2, endPtr);
        size_t len = endPtr - (result.c_str() + pos);

        if (varName.empty())
            throw cRuntimeError("Unnamed iteration variables are not accepted here");
        StringMap::const_iterator it = variables.find(varName);
        if (it == variables.end())
            throw cRuntimeError("No such variable: ${%s}", varName.c_str());
        std::string value = it->second;

        result.replace(pos, len, value);
        pos += value.length(); // skip over contents just inserted
    }
    return result;
}

const char *Configuration::substituteVariables(const char *value) const
{
    if (value == nullptr || strstr(value, "${") == nullptr)
        return value;

    // returned string needs to be stringpooled
    std::string result = substituteVariables(value, allVariables);
    return opp_staticpooledstring::get(result.c_str());
}

std::map<std::string,std::string> Configuration::getPredefinedVariables() const
{
    return predefinedVariables;
}

std::map<std::string,std::string> Configuration::getIterationVariables() const
{
    return iterationVariables;
}

const char *Configuration::getVariable(const char *varname) const
{
    auto it = iterationVariables.find(varname);
    if (it != iterationVariables.end())
        return it->second.c_str();
    it = predefinedVariables.find(varname);
    if (it != predefinedVariables.end())
        return it->second.c_str();
    return nullptr;
}

void Configuration::addEntry(const InifileContents::Entry& iniEntry)
{
    const char *key = iniEntry.getKey();
    const char *lastDot = strrchr(key, '.');
    if (!lastDot && !PatternMatcher::containsWildcards(key)) {
        Entry *entry = new Entry(iniEntry);
        entries.push_back(entry);

        // config: add if not already in there
        if (config.find(key) == config.end())
            config[key] = entry;
    }
    else {
        // key contains wildcard or dot: parameter or per-object configuration
        // (example: "**", "**.param", "**.partition-id")
        // Note: since the last part of they key might contain wildcards, it is not really possible
        // to distinguish the two. Cf "vector-recording", "vector-*" and "vector*"

        // analyze key and create appropriate entry
        std::string ownerName;
        std::string suffix;
        splitKey(key, ownerName, suffix);
        bool suffixContainsWildcards = PatternMatcher::containsWildcards(suffix.c_str());

        MatchableEntry *entry = new MatchableEntry(iniEntry);
        if (!ownerName.empty())
            entry->ownerPattern = new PatternMatcher(ownerName.c_str(), true, true, true);
        else
            entry->fullPathPattern = new PatternMatcher(key, true, true, true);
        entry->suffixPattern = suffixContainsWildcards ? new PatternMatcher(suffix.c_str(), true, true, true) : nullptr;
        entries.push_back(entry);

        // find which bin it should go into
        if (!suffixContainsWildcards) {
            // no wildcard in suffix
            SuffixBin& bin = getOrCreateBin(suffix);
            addToBin(bin, entry);
        }
        else {
            // suffix contains wildcard
            addToBin(wildcardSuffixBin, entry);

            // We also need to add it to all existing suffix bins it matches.
            // Note: if suffix also contains a hyphen, that's actually illegal (per-object
            // config entry names cannot be wildcarded, ie. "foo.bar.cmdenv-*" is illegal),
            // but causes no harm, because getPerObjectConfigEntry() won't look into the
            // wildcard bin
            for (auto & suffixBin : suffixBins)
                if (entry->suffixPattern->covers(suffixBin.first.c_str()))
                    (suffixBin.second).entries.push_back(entry);
        }
    }
}

Configuration::SuffixBin& Configuration::getOrCreateBin(const std::string& suffix)
{
    auto it = suffixBins.find(suffix);
    if (it != suffixBins.end())
        return it->second;

    // suffix bin not yet exists, create it
    SuffixBin& bin = suffixBins[suffix];

    // initialize bin with matching wildcard keys seen so far
    for (auto wildcardEntry : wildcardSuffixBin.entries)
        if (wildcardEntry->suffixPattern->covers(suffix.c_str()))
            bin.entries.push_back(wildcardEntry);
    return bin;
}

void Configuration::addToBin(SuffixBin& bin, MatchableEntry *entry)
{
    bin.entries.push_back(entry);
}

void Configuration::splitKey(const char *key, std::string& outOwnerName, std::string& outBinName)
{
    std::string tmp = key;

    const char *lastDotPos = strrchr(key, '.');
    const char *doubleAsterisk = !lastDotPos ? nullptr : strstr(lastDotPos, "**");

    if (!lastDotPos || doubleAsterisk) {
        // complicated special case: there's a "**" after the last dot
        // (or there's no dot at all). Examples: "**baz", "net.**.foo**",
        // "net.**.foo**bar**baz"
        // Problem with this: are "foo" and "bar" part of the paramname (=bin)
        // or the module name (=owner)? Can be either way. Only feasible solution
        // is to force matching of the full path (modulepath.paramname) against
        // the full pattern. Bin name can be "*" plus segment of the pattern
        // after the last "**". (For example, for "net.**foo**bar", the bin name
        // will be "*bar".)

        // find last "**"
        while (doubleAsterisk && strstr(doubleAsterisk+1, "**"))
            doubleAsterisk = strstr(doubleAsterisk+1, "**");
        outOwnerName = "";  // empty owner means "do fullPath match"
        outBinName = !doubleAsterisk ? "*" : doubleAsterisk+1;
    }
    else {
        // normal case: bin is the part after the last dot
        outOwnerName.assign(key, lastDotPos - key);
        outBinName.assign(lastDotPos+1);
    }
}

const char *Configuration::getConfigValue(const char *key) const
{
    auto it = config.find(key);
    return it == config.end() ? nullptr : it->second->markAccessed().getValue();
}

const cConfiguration::KeyValue& Configuration::getConfigEntry(const char *key) const
{
    auto it = config.find(key);
    if (it == config.end())
        return nullEntry;
    return it->second->markAccessed();
}

std::vector<const char *> Configuration::getMatchingConfigKeys(const char *pattern) const
{
    std::vector<const char *> result;
    PatternMatcher matcher(pattern, true, true, true);

    // iterate over the map -- this is going to be sloooow...
    for (const auto & it : config)
        if (matcher.matches(it.first.c_str()))
            result.push_back(it.first.c_str());
    return result;
}

const char *Configuration::getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // note: if there's no such entry, getParameterEntry() will return a NullEntry&, whose getValue() returns nullptr
    const KeyValue& entry = getParameterEntry(moduleFullPath, paramName, hasDefaultValue);
    return entry.getValue();
}

const cConfiguration::KeyValue& Configuration::getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // look up which bin; paramName serves as suffix (ie. bin name)
    auto it = suffixBins.find(paramName);
    const SuffixBin *bin = it == suffixBins.end() ? &wildcardSuffixBin : &it->second;

    // find first match in the bin
    for (const auto & entry : bin->entries) {
        if (entryMatches(entry, moduleFullPath, paramName))
            if (hasDefaultValue || !opp_streq(entry->getValue(), "default"))
                return entry->markAccessed();
    }
    return nullEntry;
}

bool Configuration::entryMatches(const MatchableEntry *entry, const char *moduleFullPath, const char *paramName)
{
    if (!entry->fullPathPattern) {
        // typical
        return entry->ownerPattern->matches(moduleFullPath) && (entry->suffixPattern == nullptr || entry->suffixPattern->matches(paramName));
    }
    else {
        // less efficient, but very rare
        std::string paramFullPath = std::string(moduleFullPath) + "." + paramName;
        return entry->fullPathPattern->matches(paramFullPath.c_str());
    }
}

inline bool isSet(int value, int bits) {return (value & bits) == bits;}

std::vector<const char *> Configuration::getKeyValuePairs(int flags) const
{
    std::vector<const char *> result;
    for (auto entry : entries) {
        bool add = false;
        if (isSet(flags, FILT_ALL))
            add = true;
        else {
            const char *lastDotPos = strrchr(entry->getKey(), '.');
            if (!lastDotPos) {
                if (isSet(flags, FILT_GLOBAL_CONFIG))
                    add = true;
                else if (isSet(flags, FILT_ESSENTIAL_CONFIG) && isEssentialOption(entry->getKey()))
                    add = true;
            }
            else {
                bool lastSegmentContainsHyphen = lastDotPos && strchr(lastDotPos, '-') != nullptr;
                bool isParam = !lastSegmentContainsHyphen;
                if (isParam) {
                    if (isSet(flags, FILT_PARAM))
                        add = true;
                }
                else {
                    if (isSet(flags, FILT_PER_OBJECT_CONFIG))
                        add = true;
                }
            }
        }

        if (add) {
            // note: intentionally no markAccessed()
            result.push_back(entry->getKey());
            result.push_back(entry->getValue());
        }
    }
    return result;
}

bool Configuration::isEssentialOption(const char *key) const
{
    std::string str = key;
    return str == CFGID_NETWORK->getName() || str == CFGID_REPEAT->getName() || str == CFGID_SIM_TIME_LIMIT->getName();
}

const char *Configuration::getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const
{
    // note: if there's no such entry, getPerObjectConfigEntry() will return a NullEntry&, whose getValue() returns nullptr
    const KeyValue& entry = getPerObjectConfigEntry(objectFullPath, keySuffix);
    return entry.getValue();
}

const cConfiguration::KeyValue& Configuration::getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const
{
    // look up which bin; keySuffix serves as bin name
    // Note: we do not accept wildcards in the config key's name (ie. "**.record-*" is invalid),
    // so we ignore the wildcard bin.
    std::map<std::string, SuffixBin>::const_iterator it = suffixBins.find(keySuffix);
    if (it == suffixBins.end())
        return nullEntry;  // no such bin

    const SuffixBin *suffixBin = &it->second;

    // find first match in the bin
    for (const auto & entry : suffixBin->entries) {
        if (entryMatches(entry, objectFullPath, keySuffix))
            return entry->markAccessed();  // found value
    }
    return nullEntry;  // not found
}

static const char *partAfterLastDot(const char *s)
{
    const char *lastDotPos = strrchr(s, '.');
    return lastDotPos == nullptr ? nullptr : lastDotPos+1;
}

std::vector<const char *> Configuration::getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const
{
    std::vector<const char *> result;

    // only concrete objects or "**" is accepted, because we are not prepared
    // to handle the "pattern matches pattern" case (see below as well).
    bool anyObject = strcmp(objectFullPath, "**") == 0;
    if (!anyObject && PatternMatcher::containsWildcards(objectFullPath))
        throw cRuntimeError("getMatchingPerObjectConfigKeySuffixes: Unsupported objectFullPath parameter: The only wildcard pattern accepted is '**'");

    // check all suffix bins whose name matches the pattern
    PatternMatcher suffixMatcher(keySuffixPattern, true, true, true);
    for (const auto & suffixBin : suffixBins) {
        const char *suffix = suffixBin.first.c_str();
        if (suffixMatcher.matches(suffix)) {
            // find all matching entries from this suffix bin.
            // We'll have a little problem where key ends in wildcard (i.e. entry.suffixPattern!=nullptr);
            // there we'd have to determine whether two *patterns* match. We resolve this
            // by checking whether one pattern matches the other one as string, and vice versa.
            const SuffixBin& bin = suffixBin.second;
            for (const auto & entry : bin.entries) {
                if (entry->fullPathPattern) {
                    if (PatternMatcher((std::string(objectFullPath)+"."+keySuffixPattern).c_str(), true, true, true).matches(entry->getKey()))
                        result.push_back(partAfterLastDot(entry->getKey()));
                }
                else if ((anyObject || entry->ownerPattern->matches(objectFullPath))
                    &&
                    (entry->suffixPattern == nullptr ||
                     suffixMatcher.matches(partAfterLastDot(entry->getKey())) ||
                     entry->suffixPattern->matches(keySuffixPattern)))
                    result.push_back(partAfterLastDot(entry->getKey()));
            }
        }
    }
    //TODO remove duplicates
    return result;
}

void Configuration::dump() const
{
    std::cout << "Config:" << std::endl;
    for (const auto & pair : config)
        std::cout << "  " << pair.second->str() << std::endl;

    for (const auto & suffixBin : suffixBins) {
        const std::string& suffix = suffixBin.first;
        const SuffixBin& bin = suffixBin.second;
        std::cout << "Suffix Bin " << suffix << ":" << std::endl;
        for (const auto & entry : bin.entries)
            std::cout << "  " << entry->str() << std::endl;
    }
    std::cout << "Wildcard Suffix Bin:" << std::endl;
    for (const auto & entry : wildcardSuffixBin.entries)
        std::cout << "  " << entry->str() << std::endl;

    std::cout << "Iteration Variables:" << std::endl;
    for (const auto & entry : iterationVariables)
        std::cout << "  " << entry.first << " = " << entry.second << std::endl;

    std::cout << "Predefined Variables:" << std::endl;
    for (const auto & entry : predefinedVariables)
        std::cout << "  " << entry.first << " = " << entry.second << std::endl;
}

std::vector<const cConfiguration::KeyValue*> Configuration::getUnusedEntries(bool all, bool postsimulation) const
{
    std::vector<const KeyValue*> result;
    for (auto entry : entries)
        if (!entry->isAccessed() && (all || reportAsUnaccessed(entry, postsimulation)))
            result.push_back(entry);
    return result;
}

inline bool fromSameSection(const cConfiguration::KeyValue *a, const cConfiguration::KeyValue *b)
{
    return opp_streq(a->getOriginSection(), b->getOriginSection());
}

bool Configuration::reportAsUnaccessed(const Entry *entry, bool postsimulation) const
{
    //
    // Common causes of an entry being unused are:
    // 1. Mistyped or otherwise bogus key -- this is the type we primarily want to detect and report
    // 2. Accidental shadowing by another entry -- to be reported too
    // 3. Intentional shadowing (i.e. override) from a derived section
    // 4. Parameter is to be used by a future (dynamically created) module
    // 5. Per-object config options that refer to result names are not used until a matching result is recorded
    // 6. Some config options like "description", "extends" or "repeat" are not consumed via cConfiguration
    //

    // If shadowed by another entry, it can never be accessed.
    // However, if shadowed from a derived section, that's probably an
    // intentional override and should NOT be reported.
    const Entry *shadowedBy = findFirstEntryThatShadows(entry);
    if (shadowedBy != nullptr)
        return fromSameSection(shadowedBy, entry);

    // Some config options like "description", "extends" or "repeat" are not
    // accessed, so don't report them. Mistyping of option names is detected
    // anyway.
    bool isOption = strchr(entry->getKey(), '.') == nullptr;
    if (isOption)
        return false;

    // Don't report result recording related keys, only at end of simulation
    bool isPerObjectOption = strchr(entry->getKey(), '-') != nullptr;  // noteworthy exception: "**.typename"
    if (!postsimulation && isPerObjectOption)  // rudimentary check
        return false;

    return true;
}

const Configuration::Entry *Configuration::findFirstEntryThatShadows(const Entry *entry) const
{
    for (auto e : entries)
        if (e == entry)
            break;
        else if (PatternMatcher(e->getKey(), true, true, true).covers(entry->getKey()))
            return e;
    return nullptr;
}

void Configuration::clearUsageInfo()
{
    for (auto entry : entries)
        if (!entry->isAccessed())
            entry->clearAccessInfo();
}

}  // namespace envir
}  // namespace omnetpp

