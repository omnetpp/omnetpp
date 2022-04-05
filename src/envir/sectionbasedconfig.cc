//==========================================================================
//  SECTIONBASEDCONFIG.CC - part of
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

#include <cassert>
#include <algorithm>
#include <sstream>
#include "common/opp_ctype.h"
#include "common/patternmatcher.h"
#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "common/stlutil.h"
#include "omnetpp/cconfigreader.h"
#include "omnetpp/cexception.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/platdep/platmisc.h"  // getpid()
#include "sectionbasedconfig.h"
#include "valueiterator.h"
#include "scenario.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

// XXX make sure quoting "$\{" works!

Register_GlobalConfigOption(CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS, "sectionbasedconfig-configreader-class", CFG_STRING, "", "When `configuration-class=SectionBasedConfiguration`: selects the configuration reader C++ class, which must subclass from `cConfigurationReader`.");
Register_PerRunConfigOption(CFGID_DESCRIPTION, "description", CFG_STRING, nullptr, "Descriptive name for the given simulation configuration. Descriptions get displayed in the run selection dialog.");
Register_PerRunConfigOption(CFGID_EXTENDS, "extends", CFG_STRING, nullptr, "Name of the configuration this section is based on. Entries from that section will be inherited and can be overridden. In other words, configuration lookups will fall back to the base section.");
Register_PerRunConfigOption(CFGID_CONSTRAINT, "constraint", CFG_STRING, nullptr, "For scenarios. Contains an expression that iteration variables (`${}` syntax) must satisfy for that simulation to run. Example: `$i < $j+1`.");
Register_PerRunConfigOption(CFGID_ITERATION_NESTING_ORDER, "iteration-nesting-order", CFG_STRING, nullptr, "Specifies the loop nesting order for iteration variables (`${}` syntax). The value is a comma-separated list of iteration variables; the list may also contain at most one asterisk. Variables that are not explicitly listed will be inserted at the position of the asterisk, or appended to the list if there is no asterisk. The first variable will become the outermost loop, and the last one the innermost loop. Example: `repetition,numHosts,*,iaTime`.");
Register_PerRunConfigOption(CFGID_REPEAT, "repeat", CFG_INT, "1", "For scenarios. Specifies how many replications should be done with the same parameters (iteration variables). This is typically used to perform multiple runs with different random number seeds. The loop variable is available as `${repetition}`. See also: `seed-set` key.");
Register_PerRunConfigOption(CFGID_EXPERIMENT_LABEL, "experiment-label", CFG_STRING, "${configname}", "Identifies the simulation experiment (which consists of several, potentially repeated measurements). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigOption(CFGID_MEASUREMENT_LABEL, "measurement-label", CFG_STRING, "${iterationvars}", "Identifies the measurement within the experiment. This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigOption(CFGID_REPLICATION_LABEL, "replication-label", CFG_STRING, "#${repetition}", "Identifies one replication of a measurement (see `repeat` and `measurement-label` options as well). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigOption(CFGID_RUNNUMBER_WIDTH, "runnumber-width", CFG_INT, "0", "Setting a nonzero value will cause the `$runnumber` variable to get padded with leading zeroes to the given length.");
Register_PerRunConfigOption(CFGID_RESULTDIR_SUBDIVISION, "resultdir-subdivision", CFG_BOOL, "false", "Makes the results directory hierarchical by appending `${iterationvarsd}` to the value of the `result-dir` config option. This is useful if a parameter study produces a large number of runs (>10000), as many file managers and other tools (including the OMNeT++ IDE) struggle with directories containing that many files. An alternative to using this option is to include iteration variables directly in the value of the `result-dir` option.");

extern cConfigOption *CFGID_NETWORK;
extern cConfigOption *CFGID_RESULT_DIR;
extern cConfigOption *CFGID_SEED_SET;
extern cConfigOption *CFGID_SIM_TIME_LIMIT;

Register_Class(SectionBasedConfiguration);

// table to be kept consistent with scave/fields.cc
static struct ConfigVarDescription { const char *name, *description; } configVarDescriptions[] = {
    { CFGVAR_RUNID,            "A reasonably globally unique identifier for the run, produced by concatenating the configuration name, run number, date/time, etc." },
    { CFGVAR_INIFILE,          "Name of the (primary) inifile" },
    { CFGVAR_CONFIGNAME,       "Name of the active configuration" },
    { CFGVAR_RUNNUMBER,        "Sequence number of the current run within all runs in the active configuration" },
    { CFGVAR_NETWORK,          "Value of the `network` configuration option" },
    { CFGVAR_EXPERIMENT,       "Value of the `experiment-label` configuration option" },
    { CFGVAR_MEASUREMENT,      "Value of the `measurement-label` configuration option" },
    { CFGVAR_REPLICATION,      "Value of the `replication-label` configuration option" },
    { CFGVAR_PROCESSID,        "PID of the simulation process" },
    { CFGVAR_DATETIME,         "Date and time the simulation run was started" },
    { CFGVAR_DATETIMEF,        "Like ${datetime}, but sanitized for use as part of a file name" },
    { CFGVAR_RESULTDIR,        "Value of the `result-dir` configuration option" },
    { CFGVAR_REPETITION,       "The iteration number in `0..N-1`, where `N` is the value of the `repeat` configuration option" },
    { CFGVAR_SEEDSET,          "Value of the `seed-set` configuration option" },
    { CFGVAR_ITERATIONVARS,    "Concatenation of all user-defined iteration variables in `name=value` form" },
    { CFGVAR_ITERATIONVARSF,   "Like ${iterationvars}, but sanitized for use as part of a file name" },
    { CFGVAR_ITERATIONVARSD,   "Like ${iterationvars}, but for use as hierarchical folder name (it contains slashes where ${iterationvarsf} has commas)" },
    { nullptr,                 nullptr }
};

#define VARPOS_PREFIX    std::string("&")

SectionBasedConfiguration::MatchableEntry::MatchableEntry(const MatchableEntry& e) :
    Entry(e),
    ownerPattern(e.ownerPattern ? new PatternMatcher(*e.ownerPattern) : nullptr),
    suffixPattern(e.suffixPattern ? new PatternMatcher(*e.suffixPattern) : nullptr),
    fullPathPattern(e.fullPathPattern ? new PatternMatcher(*e.fullPathPattern) : nullptr)
{
}

SectionBasedConfiguration::MatchableEntry::~MatchableEntry()
{
    delete ownerPattern;
    delete suffixPattern;
    delete fullPathPattern;
}

std::string SectionBasedConfiguration::MatchableEntry::str() const
{
    return std::string("ownerPattern=") + (ownerPattern ? ownerPattern->str() : "nullptr") +
           " suffixPattern=" + (suffixPattern ? suffixPattern->str() : "nullptr") +
           " fullPathPattern=" + (fullPathPattern ? fullPathPattern->str() : "nullptr");
}

//----

SectionBasedConfiguration::~SectionBasedConfiguration()
{
    clear();
    delete ini;
}

void SectionBasedConfiguration::setConfigurationReader(cConfigurationReader *ini)
{
    ASSERT(activeConfig.empty()); // only allow setConfigurationReader() during setup
    clear();
    this->ini = ini;
    nullEntry.setBaseDirectory(ini->getDefaultBaseDirectory());
    cachedSectionChains.assign(ini->getNumSections(), std::vector<int>());
    activateGlobalConfig();
}

void SectionBasedConfiguration::setCommandLineConfigOptions(const std::map<std::string, std::string>& options, const char *baseDir)
{
    ASSERT(activeConfig.empty()); // only allow setCommandLineConfigOptions() during setup
    commandLineOptions.clear();
    for (const auto & it : options) {
        const char *key = it.first.c_str();
        const char *value = it.second.c_str();
        if (opp_isempty(value))
            throw cRuntimeError("Missing value for command-line option --%s", key);
        commandLineOptions.push_back(Entry(baseDir, key, value));
    }
    activateGlobalConfig();
}

void SectionBasedConfiguration::clear()
{
    // note: this gets called between activateConfig() calls, so "ini" must NOT be nullptr'ed out here
    activeConfig = "";
    activeRunNumber = 0;
    entries.clear();
    config.clear();
    suffixBins.clear();
    wildcardSuffixBin.entries.clear();
    variables.clear();
}

void SectionBasedConfiguration::initializeFrom(cConfiguration *bootConfig)
{
    std::string classname = bootConfig->getAsString(CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS);
    if (classname.empty())
        throw cRuntimeError("SectionBasedConfiguration: No configuration reader class specified (missing %s option)",
                CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS->getName());
    cConfigurationReader *reader = dynamic_cast<cConfigurationReader *>(createOne(classname.c_str()));
    if (!reader)
        throw cRuntimeError("Class \"%s\" is not subclassed from cConfigurationReader", classname.c_str());
    reader->initializeFrom(bootConfig);
    setConfigurationReader(reader); // implies activateGlobalConfig()
}

const char *SectionBasedConfiguration::getFileName() const
{
    return ini == nullptr ? nullptr : ini->getFileName();
}

const char *SectionBasedConfiguration::getActiveConfigName() const
{
    return activeConfig.c_str();
}

int SectionBasedConfiguration::getActiveRunNumber() const
{
    return activeRunNumber;
}

std::vector<std::string> SectionBasedConfiguration::getConfigNames()
{
    std::vector<std::string> result;
    for (int i = 0; i < ini->getNumSections(); i++)
        result.push_back(ini->getSectionName(i));
    return result;
}

std::string SectionBasedConfiguration::getConfigDescription(const char *configName) const
{
    // determine the list of sections, from this one up to [General]
    std::vector<int> sectionChain = resolveSectionChain(configName);
    return internalGetValueAsString(sectionChain, CFGID_DESCRIPTION->getName());
}

std::vector<std::string> SectionBasedConfiguration::getBaseConfigs(const char *configName) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1)
        throw cRuntimeError("No such config: %s", configName);
    int entryId = internalFindEntry(sectionId, CFGID_EXTENDS->getName());
    std::string extends = entryId == -1 ? "" : ini->getEntry(sectionId, entryId).getValue();
    if (extends.empty())
        extends = "General";

    std::vector<std::string> result;
    StringTokenizer tokenizer(extends.c_str(), ", \t");
    while (tokenizer.hasMoreTokens()) {
        const char *sectionName = tokenizer.nextToken();
        int sectionId = resolveConfigName(sectionName);
        if (sectionId != -1)
            result.push_back(sectionName);
    }
    return result;
}

std::vector<int> SectionBasedConfiguration::getBaseConfigIds(int sectionId) const
{
    int generalSectionId = internalFindSection("General");
    int entryId = internalFindEntry(sectionId, CFGID_EXTENDS->getName());
    std::string extends = entryId == -1 ? "" : ini->getEntry(sectionId, entryId).getValue();

    std::vector<int> result;
    if (sectionId == generalSectionId && generalSectionId != -1) {
        // 'General' can not have base config
    }
    else if (extends.empty()) {
        // implicitly inherit form 'General'
        if (generalSectionId != -1)
            result.push_back(generalSectionId);
    }
    else {
        StringTokenizer tokenizer(extends.c_str(), ", \t");
        while (tokenizer.hasMoreTokens()) {
            const char *configName = tokenizer.nextToken();
            int sectionId = resolveConfigName(configName);
            if (sectionId != -1)
                result.push_back(sectionId);
            else
                throw cRuntimeError("Section not found: %s", configName);
        }
    }
    return result;
}

std::vector<std::string> SectionBasedConfiguration::getConfigChain(const char *configName) const
{
    std::vector<std::string> result;
    std::vector<int> sectionIds = resolveSectionChain(configName);
    for (int sectionId : sectionIds)
        result.push_back(ini->getSectionName(sectionId));
    return result;
}

int SectionBasedConfiguration::resolveConfigName(const char *configName) const
{
    if (!configName || !configName[0])
        throw cRuntimeError("Empty config name specified");
    return internalFindSection(configName);
}

void SectionBasedConfiguration::activateGlobalConfig()
{
    clear();
    for (auto & e : commandLineOptions)
        addEntry(Entry(e.getBaseDirectory(), e.getKey(), e.getValue())); // note: no substituteVariables() on value, it's too early for that
    int sectionId = resolveConfigName("General");
    if (sectionId != -1) {
        for (int entryId = 0; entryId < ini->getNumEntries(sectionId); entryId++) {
            const auto& e = ini->getEntry(sectionId, entryId);
            addEntry(Entry(e.getBaseDirectory(), e.getKey(), e.getValue())); // note: no substituteVariables() on value, it's too early for that
        }
    }
}

void SectionBasedConfiguration::activateConfig(const char *configName, int runNumber)
{
    clear();

    activeConfig = configName == nullptr ? "" : configName;
    activeRunNumber = runNumber;

    // determine the list of sections, from this one up to [General]
    std::vector<int> sectionChain = resolveSectionChain(configName);

    // extract all iteration vars from values within this section
    std::vector<Scenario::IterationVariable> itervars = collectIterationVariables(sectionChain, locationToVarName);

    // see if there's a constraint and/or iteration nesting order given
    const char *constraint = internalGetValue(sectionChain, CFGID_CONSTRAINT->getName(), nullptr);
    const char *nestingSpec = internalGetValue(sectionChain, CFGID_ITERATION_NESTING_ORDER->getName(), nullptr);

    // determine the values to substitute into the iteration vars (${...})
    try {
        Scenario scenario(itervars, constraint, nestingSpec);
        int numRuns = scenario.getNumRuns();
        if (runNumber < 0 || runNumber >= numRuns)
            throw cRuntimeError("Run number %d is out of range for configuration '%s': It contains %d run(s)", runNumber, configName, numRuns);

        scenario.gotoRun(runNumber);
        variables = computeVariables(getActiveConfigName(), getActiveRunNumber(), sectionChain, &scenario, locationToVarName);
        runId = variables[CFGVAR_RUNID];
    }
    catch (std::exception& e) {
        throw cRuntimeError("Scenario generator: %s", e.what());
    }

    auto addWildcardIfNeeded = [](const char *key) {
        if (strchr(key, '.') == nullptr) {
            cConfigOption *e = lookupConfigOption(key);
            if (e && e->isPerObject())
                return std::string("**.") + key; // if it's per-object option but specified without an object path, pretend it was specified with "**."
        }
        return std::string(key);
    };

    // walk the list of fallback sections, and add entries to our tables
    // (config[] and suffixBins[]). Meanwhile, substitute the iteration values.
    // Note: entries added first will have precedence over those added later.
    for (auto & e : commandLineOptions) {
        std::string key = addWildcardIfNeeded(e.getKey());
        std::string value = substituteVariables(e.getValue());
        addEntry(Entry(e.getBaseDirectory(), key.c_str(), value.c_str(), FileLine("<commandline>")));
    }
    for (int sectionId : sectionChain) {
        for (int entryId = 0; entryId < ini->getNumEntries(sectionId); entryId++) {
            // add entry to our tables
            const auto& e = ini->getEntry(sectionId, entryId);
            std::string key = addWildcardIfNeeded(e.getKey());
            std::string value = substituteVariables(e.getValue(), sectionId, entryId, variables, locationToVarName);
            addEntry(Entry(e.getBaseDirectory(), key.c_str(), value.c_str(), e.getSourceLocation()));
        }
    }
}

inline std::string unquote(const std::string& txt)
{
    if (txt.find('"') == std::string::npos)
        return txt;
    try {
        return opp_parsequotedstr(txt.c_str());
    }
    catch (std::exception& e) {
        return txt;
    }
}


SectionBasedConfiguration::StringMap SectionBasedConfiguration::computeVariables(const char *configName, int runNumber, std::vector<int> sectionChain, const Scenario *scenario, const StringMap& locationToVarName) const
{
    StringMap result;

    // store iteration variables
    std::vector<std::string> varNames = scenario->getIterationVariableNames();
    for (std::string varName : varNames)
        result[varName] = scenario->getVariable(varName.c_str());

    // assemble ${iterationvars}, ${iterationvarsf} and ${iterationvarsd}
    std::string iterationvars, iterationvarsf, iterationvarsd;
    for (std::string varName : varNames) {
        if (varName != CFGVAR_REPETITION) {
            std::string value = scenario->getVariable(varName.c_str());
            iterationvars += std::string(iterationvars.empty() ? "" : ", ") + "$" + varName + "=" + value;
            std::string segment = (opp_isalpha(varName[0]) ? varName + "=" : "") + opp_filenameencode(unquote(value)); // without dollar and space, possible quotes removed
            iterationvarsf += std::string(iterationvarsf.empty() ? "" : ",") + segment;
            iterationvarsd += std::string(iterationvarsd.empty() ? "" : "/") + segment;
        }
    }
    if (!iterationvarsf.empty())
        iterationvarsf += "-";  // for filenames
    result[CFGVAR_ITERATIONVARS] = iterationvars;
    result[CFGVAR_ITERATIONVARSF] = iterationvarsf;
    result[CFGVAR_ITERATIONVARSD] = iterationvarsd;

    // create variables
    int runnumberWidth = internalGetConfigAsInt(CFGID_RUNNUMBER_WIDTH, sectionChain, result, locationToVarName);
    runnumberWidth = std::max(0, std::min(6, runnumberWidth));
    std::string datetime = opp_makedatetimestring();
    result[CFGVAR_INIFILE] = opp_nulltoempty(getFileName());
    result[CFGVAR_CONFIGNAME] = configName;
    result[CFGVAR_RUNNUMBER] = opp_stringf("%0*d", runnumberWidth, runNumber);
    result[CFGVAR_NETWORK] = internalGetConfigAsString(CFGID_NETWORK, sectionChain, result, locationToVarName);
    result[CFGVAR_PROCESSID] = opp_stringf("%d", (int)getpid());
    result[CFGVAR_DATETIME] = datetime;
    result[CFGVAR_DATETIMEF] = opp_replacesubstring(datetime, ":", "", true);
    result[CFGVAR_RUNID] = result[CFGVAR_CONFIGNAME]+"-"+result[CFGVAR_RUNNUMBER]+"-"+result[CFGVAR_DATETIME]+"-"+result[CFGVAR_PROCESSID];

    // the following variables should be done last, because they may depend on the variables computed above
    std::string resultdir = internalGetConfigAsString(CFGID_RESULT_DIR, sectionChain, result, locationToVarName);
    bool subdivideResultdir = internalGetConfigAsBool(CFGID_RESULTDIR_SUBDIVISION, sectionChain, result, locationToVarName);
    if (subdivideResultdir)
        resultdir += std::string("/") + configName + "/" + iterationvarsd;
    result[CFGVAR_RESULTDIR] = resultdir;
    result[CFGVAR_SEEDSET] = std::to_string(internalGetConfigAsInt(CFGID_SEED_SET, sectionChain, result, locationToVarName));
    result[CFGVAR_EXPERIMENT] = internalGetConfigAsString(CFGID_EXPERIMENT_LABEL, sectionChain, result, locationToVarName);
    result[CFGVAR_MEASUREMENT] = internalGetConfigAsString(CFGID_MEASUREMENT_LABEL, sectionChain, result, locationToVarName);
    result[CFGVAR_REPLICATION] = internalGetConfigAsString(CFGID_REPLICATION_LABEL, sectionChain, result, locationToVarName);
    return result;
}

std::string SectionBasedConfiguration::internalGetConfigAsString(cConfigOption *option, const std::vector<int>& sectionChain, const StringMap& variables, const StringMap& locationToVarName) const
{
    try {
        int sectionId, entryId;
        const char *value = internalGetValue(sectionChain, option->getName(), option->getDefaultValue(), &sectionId, &entryId);
        std::string str = substituteVariables(value, sectionId, entryId, variables, locationToVarName);
        if (str[0] == '"')
            str = Expression().parse(str.c_str()).stringValue();
        return str;
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error getting value of config option '%s': %s", option->getName(), e.what());
    }
}

intval_t SectionBasedConfiguration::internalGetConfigAsInt(cConfigOption *option, const std::vector<int>& sectionChain, const StringMap& variables, const StringMap& locationToVarName) const
{
    try {
        int sectionId, entryId;
        const char *value = internalGetValue(sectionChain, option->getName(), option->getDefaultValue(), &sectionId, &entryId);
        std::string str = substituteVariables(value, sectionId, entryId, variables, locationToVarName);
        return Expression().parse(str.c_str()).intValue();
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error getting value of config option '%s': %s", option->getName(), e.what());
    }
}

bool SectionBasedConfiguration::internalGetConfigAsBool(cConfigOption *option, const std::vector<int>& sectionChain, const StringMap& variables, const StringMap& locationToVarName) const
{
    try {
        int sectionId, entryId;
        const char *value = internalGetValue(sectionChain, option->getName(), option->getDefaultValue(), &sectionId, &entryId);
        std::string str = substituteVariables(value, sectionId, entryId, variables, locationToVarName);
        return Expression().parse(str.c_str()).boolValue();
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error getting value of config option '%s': %s", option->getName(), e.what());
    }
}

int SectionBasedConfiguration::getNumRunsInConfig(const char *configName) const
{
    // extract all iteration vars from values within this config
    std::vector<int> sectionChain = resolveSectionChain(configName);
    StringMap locationToVarNameMap;
    std::vector<Scenario::IterationVariable> v = collectIterationVariables(sectionChain, locationToVarNameMap);

    // see if there's a constraint given
    const char *constraint = internalGetValue(sectionChain, CFGID_CONSTRAINT->getName(), nullptr);

    // count the runs and return the result
    try {
        return Scenario(v, constraint, "").getNumRuns();
    }
    catch (std::exception& e) {
        throw cRuntimeError("Could not compute number of runs in config %s: %s", configName, e.what());
    }
}

std::vector<cConfiguration::RunInfo> SectionBasedConfiguration::unrollConfig(const char *configName) const
{
    // extract all iteration vars from values within this section
    std::vector<int> sectionChain = resolveSectionChain(configName);
    StringMap locationToVarNameMap;
    std::vector<Scenario::IterationVariable> itervars = collectIterationVariables(sectionChain, locationToVarNameMap);

    // see if there's a constraint and/or iteration nesting order given
    const char *constraint = internalGetValue(sectionChain, CFGID_CONSTRAINT->getName(), nullptr);
    const char *nestingSpec = internalGetValue(sectionChain, CFGID_ITERATION_NESTING_ORDER->getName(), nullptr);

    // iterate over all runs in the scenario
    try {
        Scenario scenario(itervars, constraint, nestingSpec);
        std::vector<RunInfo> result;
        if (scenario.restart()) {
            for (;;) {
                RunInfo runInfo;
                runInfo.info = scenario.str();
                StringMap variables = computeVariables(configName, result.size(), sectionChain, &scenario, locationToVarNameMap);
                runInfo.runAttrs = variables;

                // collect entries that contain ${..}
                std::string tmp;
                for (int sectionId : sectionChain) {
                    for (int entryId = 0; entryId < ini->getNumEntries(sectionId); entryId++) {
                        const auto& entry = ini->getEntry(sectionId, entryId);
                        if (strstr(entry.getValue(), "${") != nullptr) {
                            std::string expandedValue = substituteVariables(entry.getValue(), sectionId, entryId, variables, locationToVarNameMap);
                            tmp += std::string(entry.getKey()) + " = " + expandedValue + "\n";
                        }
                    }
                }
                runInfo.configBrief = tmp;
                result.push_back(runInfo);

                // move to the next run
                if (!scenario.next())
                    break;
            }
        }
        return result;
    }
    catch (std::exception& e) {
        throw cRuntimeError("Scenario generator: %s", e.what());
    }
}

std::vector<Scenario::IterationVariable> SectionBasedConfiguration::collectIterationVariables(const std::vector<int>& sectionChain, StringMap& outLocationToNameMap) const
{
    std::vector<Scenario::IterationVariable> v;
    int unnamedCount = 0;
    outLocationToNameMap.clear();
    for (int sectionId : sectionChain) {
        for (int entryId = 0; entryId < ini->getNumEntries(sectionId); entryId++) {
            const auto& entry = ini->getEntry(sectionId, entryId);
            const char *pos = entry.getValue();
            int k = 0;
            while ((pos = strstr(pos, "${")) != nullptr) {
                Scenario::IterationVariable iterVar;
                try {
                    parseVariable(pos, iterVar.varName, iterVar.value, iterVar.parvar, pos);
                }
                catch (std::exception& e) {
                    throw cRuntimeError("Scenario generator: %s at %s=%s", e.what(), entry.getKey(), entry.getValue());
                }
                if (!iterVar.value.empty()) {
                    // store variable
                    if (!iterVar.varName.empty()) {
                        // check it does not conflict with other iteration variables or predefined variables
                        for (auto & j : v)
                            if (j.varName == iterVar.varName)
                                throw cRuntimeError("Scenario generator: Redefinition of iteration variable ${%s} in the configuration", iterVar.varName.c_str());

                        if (isPredefinedVariable(iterVar.varName.c_str()))
                            throw cRuntimeError("Scenario generator: ${%s} is a predefined variable and cannot be changed", iterVar.varName.c_str());
                    }
                    else {
                        // unnamed variable: generate name ($0, $1, $2, etc.), and store its location
                        iterVar.varName = opp_stringf("%d", unnamedCount++);
                        std::string location = opp_stringf("%d:%d:%d", sectionId, entryId, k);
                        outLocationToNameMap[location] = iterVar.varName;
                    }
                    v.push_back(iterVar);
                }
                k++;
            }
        }
    }

    // register ${repetition}, based on the repeat= config entry
    int repeatCount = internalGetValueAsInt(sectionChain, CFGID_REPEAT->getName(), 1);
    Scenario::IterationVariable repetition;
    repetition.varName = CFGVAR_REPETITION;
    repetition.value = opp_stringf("0..%d", repeatCount-1);
    v.push_back(repetition);

    return v;
}

void SectionBasedConfiguration::parseVariable(const char *txt, std::string& outVarname, std::string& outValue, std::string& outParvar, const char *& outEndPtr)
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

std::string SectionBasedConfiguration::substituteVariables(const char *text, int sectionId, int entryId, const StringMap& variables, const StringMap& locationToVarName) const
{
    std::string result = opp_nulltoempty(text);
    int k = 0;  // counts "${" occurrences
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        std::string varName, dummy1, dummy2;
        const char *endPtr;
        parseVariable(result.c_str() + pos, varName, dummy1, dummy2, endPtr);
        size_t len = endPtr - (result.c_str() + pos);

        // handle named and unnamed iteration variable references
        if (varName.empty()) {
            std::string location = opp_stringf("%d:%d:%d", sectionId, entryId, k);
            StringMap::const_iterator it = locationToVarName.find(location);
            Assert(it != locationToVarName.end());
            varName = it->second;
        }
        StringMap::const_iterator it = variables.find(varName);
        if (it == variables.end())
            throw cRuntimeError("No such variable: ${%s}", varName.c_str());
        std::string value = it->second;

        result.replace(pos, len, value);
        pos += value.length(); // skip over contents just inserted

        k++;
    }
    return result;
}

const char *SectionBasedConfiguration::substituteVariables(const char *value) const
{
    if (value == nullptr || strstr(value, "${") == nullptr)
        return value;

    // returned string needs to be stringpooled
    std::string result = substituteVariables(value, -1, -1, variables, locationToVarName);
    return opp_staticpooledstring::get(result.c_str());
}

const char *SectionBasedConfiguration::getVariable(const char *varname) const
{
    StringMap::const_iterator it = variables.find(varname);
    return it == variables.end() ? nullptr : it->second.c_str();
}

std::vector<const char *> SectionBasedConfiguration::getIterationVariableNames() const
{
    std::vector<const char *> result;
    for (const auto & variable : variables)
        if (opp_isalphaext(variable.first[0]) && !isPredefinedVariable(variable.first.c_str()))  // skip unnamed and predefined ones
            result.push_back(variable.first.c_str());

    return result;
}

std::vector<const char *> SectionBasedConfiguration::getPredefinedVariableNames() const
{
    std::vector<const char *> result;
    for (int i = 0; configVarDescriptions[i].name; i++)
        result.push_back(configVarDescriptions[i].name);
    return result;
}

const char *SectionBasedConfiguration::getVariableDescription(const char *varname) const
{
    for (int i = 0; configVarDescriptions[i].name; i++)
        if (strcmp(varname, configVarDescriptions[i].name) == 0)
            return configVarDescriptions[i].description;

    if (!opp_isempty(getVariable(varname)))
        return "User-defined iteration variable";
    return nullptr;
}

bool SectionBasedConfiguration::isPredefinedVariable(const char *varname) const
{
    for (int i = 0; configVarDescriptions[i].name; i++)
        if (strcmp(varname, configVarDescriptions[i].name) == 0)
            return true;

    return false;
}

typedef std::vector<int> SectionChain;
typedef std::vector<SectionChain> SectionChainList;

std::vector<int> SectionBasedConfiguration::resolveSectionChain(int sectionId) const
{
    if (sectionId == -1)
        return std::vector<int>();  // assume implicit [General] section
    if (cachedSectionChains.at(sectionId).empty())
        cachedSectionChains[sectionId] = computeSectionChain(sectionId);
    return cachedSectionChains[sectionId];
}

std::vector<int> SectionBasedConfiguration::resolveSectionChain(const char *configName) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1 && strcmp(configName, "General") != 0)  // allow implicit [General] section
        throw cRuntimeError("No such config: %s", configName);
    return resolveSectionChain(sectionId);
}

static bool mergeSectionChains(SectionChainList& remainingInputs, std::vector<int>& result);

/*
 * Computes the linearization of given section and all of its base sections.
 * The result is the merge of the linearization of base classes and base classes itself.
 */
std::vector<int> SectionBasedConfiguration::computeSectionChain(int sectionId) const
{
    std::vector<int> baseConfigs = getBaseConfigIds(sectionId);
    SectionChainList chainsToBeMerged;
    for (std::vector<int>::const_iterator it = baseConfigs.begin(); it != baseConfigs.end(); ++it) {
        SectionChain chain = resolveSectionChain(*it);
        chainsToBeMerged.push_back(chain);
    }
    chainsToBeMerged.push_back(baseConfigs);

    std::vector<int> result;
    result.push_back(sectionId);
    if (mergeSectionChains(chainsToBeMerged, result))
        return result;
    else
        throw cRuntimeError("Detected section fallback chain inconsistency at [%s]", ini->getSectionName(sectionId));  // TODO more details?
}

static int selectNext(const SectionChainList& remainingInputs);

/*
 * Merges the the section chains given as 'sectionChains' and collects the result in 'result'.
 *
 * The logic of selecting the next element for the output is implemented in the selectNext() function.
 * The Dylan and C3 linearization differs only in the selectNext() function.
 *
 * The 'sectionChains' is modified during the merge. If the was successful then
 * the function returns 'true' and all chain within 'sectionChains' is empty,
 * otherwise it returns false and the 'sectionChains' contains conflicting chains.
 */
static bool mergeSectionChains(SectionChainList& sectionChains, std::vector<int>& result)
{
    SectionChainList::iterator begin = sectionChains.begin();
    SectionChainList::iterator end = sectionChains.end();

    // reverse the chains, so they are accessed at the end instead of the begin (more efficient to remove from the end of a vector)
    bool allChainsAreEmpty = true;
    for (SectionChainList::iterator chain = begin; chain != end; ++chain) {
        if (!chain->empty()) {
            reverse(chain->begin(), chain->end());
            allChainsAreEmpty = false;
        }
    }

    while (!allChainsAreEmpty) {
        // select next
        int nextId = selectNext(sectionChains);
        if (nextId == -1)
            break;
        // add it to the output and remove from the inputs
        result.push_back(nextId);
        allChainsAreEmpty = true;
        for (SectionChainList::iterator chain = begin; chain != end; ++chain) {
            if (!chain->empty()) {
                if (chain->back() == nextId)
                    chain->pop_back();
                if (!chain->empty())
                    allChainsAreEmpty = false;
            }
        }
    }

    return allChainsAreEmpty;
}

/*
 * Select next element for the merge of 'sectionChains'.
 *
 * This implements C3 linearization:
 *   When there are more that one possible candidates,
 *   choose the section that appears in the linearization of the earliest
 *   direct base section in the local precedence order (i.e. as enumerated in the 'extends' entry).
 */
static int selectNext(const SectionChainList& sectionChains)
{
    SectionChainList::const_iterator begin = sectionChains.begin();
    SectionChainList::const_iterator end = sectionChains.end();

    for (SectionChainList::const_iterator chain = begin; chain != end; ++chain) {
        if (!chain->empty()) {
            int candidate = chain->back();

            // check if candidate is not contained any tail of input chains
            // note: the head of the chains are at their back()
            bool found = false;
            for (SectionChainList::const_iterator chainPtr = begin; !found && chainPtr != end; ++chainPtr)
                if (!chainPtr->empty())
                    for (SectionChain::const_reverse_iterator idPtr = chainPtr->rbegin()+1; !found && idPtr != chainPtr->rend(); ++idPtr)
                        if (*idPtr == candidate)
                            found = true;

            if (!found)
                return candidate;
        }
    }

    return -1;
}

void SectionBasedConfiguration::addEntry(const Entry& entry)
{
    entries.push_back(entry);
    const std::string& key = entry.key;
    const char *lastDot = strrchr(key.c_str(), '.');
    if (!lastDot && !PatternMatcher::containsWildcards(key.c_str())) {
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
        splitKey(key.c_str(), ownerName, suffix);
        bool suffixContainsWildcards = PatternMatcher::containsWildcards(suffix.c_str());

        MatchableEntry entry2(entry);
        if (!ownerName.empty())
            entry2.ownerPattern = new PatternMatcher(ownerName.c_str(), true, true, true);
        else
            entry2.fullPathPattern = new PatternMatcher(key.c_str(), true, true, true);
        entry2.suffixPattern = suffixContainsWildcards ? new PatternMatcher(suffix.c_str(), true, true, true) : nullptr;

        // find which bin it should go into
        if (!suffixContainsWildcards) {
            // no wildcard in suffix (=bin name)
            if (suffixBins.find(suffix) == suffixBins.end()) {
                // suffix bin not yet exists, create it
                SuffixBin& bin = suffixBins[suffix];

                // initialize bin with matching wildcard keys seen so far
                for (auto & wildcardEntry : wildcardSuffixBin.entries)
                    if (wildcardEntry.suffixPattern->matches(suffix.c_str()))
                        bin.entries.push_back(wildcardEntry);
            }
            suffixBins[suffix].entries.push_back(entry2);
        }
        else {
            // suffix contains wildcards: we need to add it to all existing suffix bins it matches
            // Note: if suffix also contains a hyphen, that's actually illegal (per-object
            // config entry names cannot be wildcarded, ie. "foo.bar.cmdenv-*" is illegal),
            // but causes no harm, because getPerObjectConfigEntry() won't look into the
            // wildcard bin
            wildcardSuffixBin.entries.push_back(entry2);
            for (auto & suffixBin : suffixBins)
                if (entry2.suffixPattern->matches(suffixBin.first.c_str()))
                    (suffixBin.second).entries.push_back(entry2);
        }
    }
}

void SectionBasedConfiguration::splitKey(const char *key, std::string& outOwnerName, std::string& outBinName)
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

int SectionBasedConfiguration::internalFindSection(const char *section) const
{
    // not very efficient (linear search), but we only invoke it a few times during activateConfig()
    for (int i = 0; i < ini->getNumSections(); i++)
        if (strcmp(section, ini->getSectionName(i)) == 0)
            return i;

    return -1;
}

int SectionBasedConfiguration::internalGetSectionId(const char *section) const
{
    int sectionId = internalFindSection(section);
    if (sectionId == -1)
        throw cRuntimeError("No such section: %s", section);
    return sectionId;
}

int SectionBasedConfiguration::internalFindEntry(int sectionId, const char *key) const
{
    // not very efficient (linear search), but we only invoke from activateConfig(),
    // and only once per section
    for (int i = 0; i < ini->getNumEntries(sectionId); i++)
        if (strcmp(key, ini->getEntry(sectionId, i).getKey()) == 0)
            return i;

    return -1;
}

const char *SectionBasedConfiguration::internalGetValue(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue, int *outSectionIdPtr, int *outEntryIdPtr) const
{
    if (outSectionIdPtr)
        *outSectionIdPtr = -1;
    if (outEntryIdPtr)
        *outEntryIdPtr = -1;

    for (const auto & commandLineOption : commandLineOptions)
        if (strcmp(key, commandLineOption.getKey()) == 0)
            return commandLineOption.getValue();


    for (int sectionId : sectionChain) {
        int entryId = internalFindEntry(sectionId, key);
        if (entryId != -1) {
            if (outSectionIdPtr)
                *outSectionIdPtr = sectionId;
            if (outEntryIdPtr)
                *outEntryIdPtr = entryId;
            return ini->getEntry(sectionId, entryId).getValue();
        }
    }
    return fallbackValue;
}

std::string SectionBasedConfiguration::internalGetValueAsString(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue, int *outSectionIdPtr, int *outEntryIdPtr) const
{
    const char *s = internalGetValue(sectionChain, key, fallbackValue, outSectionIdPtr, outEntryIdPtr);
    return !s ? "" : s[0]=='"' ? Expression().parse(s).stringValue() : s;
}

intval_t SectionBasedConfiguration::internalGetValueAsInt(const std::vector<int>& sectionChain, const char *key, intval_t fallbackValue, int *outSectionIdPtr, int *outEntryIdPtr) const
{
    const char *s = internalGetValue(sectionChain, key, nullptr, outSectionIdPtr, outEntryIdPtr);
    return !s ? fallbackValue : Expression().parse(s).intValue();
}

enum { WHITE, GREY, BLACK };

class SectionGraphNode
{
  public:
    int id;
    std::vector<int> nextNodes;
    int color;

    SectionGraphNode(int id) : id(id), color(WHITE) {}
};

typedef std::vector<SectionGraphNode> SectionGraph;

static bool visit(SectionGraph& graph, SectionGraphNode& node);

static int findCycle(SectionGraph& graph)
{
    for (SectionGraph::iterator node = graph.begin(); node != graph.end(); ++node)
        if (node->color == WHITE && visit(graph, *node))
            return node->id;
    return -1;
}

static bool visit(SectionGraph& graph, SectionGraphNode& node)
{
    node.color = GREY;
    for (int & nextNode : node.nextNodes) {
        SectionGraphNode& node2 = graph[nextNode];
        if (node2.color == GREY || (node2.color == WHITE && visit(graph, node2)))
            return true;
    }
    node.color = BLACK;
    return false;
}

void SectionBasedConfiguration::validate(const char *ignorableConfigKeys) const
{
    // check command-line options
    for (const auto & entry : commandLineOptions) {
        const char *key = entry.key.c_str();
        bool containsDot = strchr(key, '.') != nullptr;
        const char *option = strchr(key, '.') ? strrchr(key, '.')+1 : key;  // check only the part after the last dot, i.e. recognize per-object keys as well
        bool lastSegmentContainsHyphen = strchr(option, '-') != nullptr;
        bool isConfigKey = !containsDot || lastSegmentContainsHyphen;
        if (isConfigKey) {
            cConfigOption *e = lookupConfigOption(option);
            if (!e)
                throw cRuntimeError("Unknown command-line configuration option --%s", key);
            if (containsDot && !e->isPerObject())
                throw cRuntimeError("Invalid command-line configuration option --%s: %s is not a per-object option", key, e->getName());
        }
    }

    // check section names
    std::set<std::string> configNames;
    for (int i = 0; i < ini->getNumSections(); i++) {
        const char *configName = ini->getSectionName(i);
        if (configName) {
            if (strchr(configName, ' ') != nullptr)
                throw cRuntimeError("Invalid section name [%s]: It may not contain spaces", configName);
            if (!opp_isalphaext(*configName) && *configName != '_')
                throw cRuntimeError("Invalid section name [%s]: Must begin with a letter or underscore", configName);
            for (const char *s = configName; *s; s++)
                if (!opp_isalnumext(*s) && strchr("-_@", *s) == nullptr)
                    throw cRuntimeError("Invalid section name [%s]: Contains illegal character '%c'", configName, *s);
            if (configNames.find(configName) != configNames.end())
                throw cRuntimeError("Duplicate section [%s]", configName);
            configNames.insert(configName);
        }
    }

    // check keys, build inheritance graph
    SectionGraph graph;
    for (int i = 0; i < ini->getNumSections(); i++) {
        graph.push_back(SectionGraphNode(i));
        const char *section = ini->getSectionName(i);
        int numEntries = ini->getNumEntries(i);
        for (int j = 0; j < numEntries; j++) {
            const char *key = ini->getEntry(i, j).getKey();
            bool containsDot = strchr(key, '.') != nullptr;

            if (!containsDot && !PatternMatcher::containsWildcards(key)) {
                // warn for unrecognized (or misplaced) config keys
                // NOTE: values don't need to be validated here, that will be
                // done when the config gets actually used
                cConfigOption *e = lookupConfigOption(key);
                if (!e && isIgnorableConfigKey(ignorableConfigKeys, key))
                    continue;
                if (!e)
                    throw cRuntimeError("Unknown configuration option: %s", key);
                if (e->isPerObject())
                    throw cRuntimeError("Configuration option %s should be specified per object, try **.%s=", key, key); //TODO this could now be allowed (it works)
                if (e->isGlobal() && strcmp(section, "General") != 0)
                    throw cRuntimeError("Configuration option %s may only occur in the [General] section", key);

                // check section hierarchy
                if (strcmp(key, CFGID_EXTENDS->getName()) == 0) {
                    if (strcmp(section, "General") == 0)
                        throw cRuntimeError("The [General] section cannot extend other sections");

                    // warn for invalid "extends" names
                    const char *value = ini->getEntry(i, j).getValue();
                    StringTokenizer tokenizer(value, ", \t");
                    while (tokenizer.hasMoreTokens()) {
                        const char *configName = tokenizer.nextToken();
                        int configId = internalFindSection(configName);
                        if (strcmp(configName, "General") != 0 && configId == -1)
                            throw cRuntimeError("No such config: %s", configName);
                        if (configId != -1)
                            graph.back().nextNodes.push_back(configId);
                    }
                }
            }
            else {
                // check for per-object configuration subkeys (".ev-enabled", ".record-interval")
                std::string ownerName;
                std::string suffix;
                splitKey(key, ownerName, suffix);
                bool containsHyphen = strchr(suffix.c_str(), '-') != nullptr;
                if (containsHyphen) {
                    // this is a per-object config
                    cConfigOption *e = lookupConfigOption(suffix.c_str());
                    if (!e && isIgnorableConfigKey(ignorableConfigKeys, suffix.c_str()))
                        continue;
                    if (!e || !e->isPerObject()) {
                        throw cRuntimeError("Unknown per-object configuration option '%s' in %s", suffix.c_str(), key);
                    }
                }
            }
        }
    }

    // check circularity in inheritance graph
    int sectionId = findCycle(graph);
    if (sectionId != -1)
        throw cRuntimeError("Cycle detected in section fallback chain at [%s]", ini->getSectionName(sectionId));

    // check for inconsistent hierarchy
    for (int i = 0; i < ini->getNumSections(); i++)
        resolveSectionChain(i);
}

cConfigOption *SectionBasedConfiguration::lookupConfigOption(const char *key)
{
    cConfigOption *e = (cConfigOption *)configOptions.getInstance()->lookup(key);
    if (e)
        return e;  // found it, great

    // Maybe it matches on a cConfigOption which has '*' or '%' in its name,
    // such as "seed-1-mt" matches on the "seed-%-mt" cConfigOption.
    // We have to iterate over all cConfigOptions to verify this.
    // "%" means "any number" in config keys.
    int n = configOptions.getInstance()->size();
    for (int i = 0; i < n; i++) {
        cConfigOption *e = (cConfigOption *)configOptions.getInstance()->get(i);
        if (PatternMatcher::containsWildcards(e->getName()) || strchr(e->getName(), '%') != nullptr) {
            std::string pattern = opp_replacesubstring(e->getName(), "%", "{..}", true);
            if (PatternMatcher(pattern.c_str(), false, true, true).matches(key))
                return e;
        }
    }
    return nullptr;
}

bool SectionBasedConfiguration::isIgnorableConfigKey(const char *ignoredKeyPatterns, const char *key)
{
    // see if any element in ignoredKeyPatterns matches it
    StringTokenizer tokenizer(ignoredKeyPatterns ? ignoredKeyPatterns : "");
    while (tokenizer.hasMoreTokens())
        if (PatternMatcher(tokenizer.nextToken(), true, true, true).matches(key))
            return true;

    return false;
}

const char *SectionBasedConfiguration::getConfigValue(const char *key) const
{
    std::map<std::string, Entry>::const_iterator it = config.find(key);
    return it == config.end() ? nullptr : it->second.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getConfigEntry(const char *key) const
{
    std::map<std::string, Entry>::const_iterator it = config.find(key);
    return it == config.end() ? (const KeyValue&)nullEntry : (const KeyValue&)it->second;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingConfigKeys(const char *pattern) const
{
    std::vector<const char *> result;
    PatternMatcher matcher(pattern, true, true, true);

    // iterate over the map -- this is going to be sloooow...
    for (const auto & it : config)
        if (matcher.matches(it.first.c_str()))
            result.push_back(it.first.c_str());
    return result;
}

const char *SectionBasedConfiguration::getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // note: if there's no such entry, getParameterEntry() will return a NullEntry&, whose getValue() returns nullptr
    const KeyValue& entry = getParameterEntry(moduleFullPath, paramName, hasDefaultValue);
    return entry.getValue();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // look up which bin; paramName serves as suffix (ie. bin name)
    std::map<std::string, SuffixBin>::const_iterator it = suffixBins.find(paramName);
    const SuffixBin *bin = it == suffixBins.end() ? &wildcardSuffixBin : &it->second;

    // find first match in the bin
    for (const auto & entry : bin->entries) {
        if (entryMatches(entry, moduleFullPath, paramName))
            if (hasDefaultValue || entry.value != "default")
                return entry;
    }
    return nullEntry;  // not found
}

bool SectionBasedConfiguration::entryMatches(const MatchableEntry& entry, const char *moduleFullPath, const char *paramName)
{
    if (!entry.fullPathPattern) {
        // typical
        return entry.ownerPattern->matches(moduleFullPath) && (entry.suffixPattern == nullptr || entry.suffixPattern->matches(paramName));
    }
    else {
        // less efficient, but very rare
        std::string paramFullPath = std::string(moduleFullPath) + "." + paramName;
        return entry.fullPathPattern->matches(paramFullPath.c_str());
    }
}

inline bool isSet(int value, int bits) {return (value & bits) == bits;}

std::vector<const char *> SectionBasedConfiguration::getKeyValuePairs(int flags) const
{
    std::vector<const char *> result;
    for (const auto & entry : entries) {
        bool add = false;
        if (isSet(flags, FILT_ALL))
            add = true;
        else {
            const char *lastDotPos = strrchr(entry.getKey(), '.');
            if (!lastDotPos) {
                if (isSet(flags, FILT_GLOBAL_CONFIG))
                    add = true;
                else if (isSet(flags, FILT_ESSENTIAL_CONFIG) && isEssentialOption(entry.getKey()))
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
            result.push_back(entry.getKey());
            result.push_back(entry.getValue());
        }
    }
    return result;
}

bool SectionBasedConfiguration::isEssentialOption(const char *key) const
{
    std::string str = key;
    return str == CFGID_NETWORK->getName() || str == CFGID_REPEAT->getName() || str == CFGID_SIM_TIME_LIMIT->getName();
}

const char *SectionBasedConfiguration::getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const
{
    // note: if there's no such entry, getPerObjectConfigEntry() will return a NullEntry&, whose getValue() returns nullptr
    const KeyValue& entry = getPerObjectConfigEntry(objectFullPath, keySuffix);
    return entry.getValue();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const
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
            return entry;  // found value
    }
    return nullEntry;  // not found
}

static const char *partAfterLastDot(const char *s)
{
    const char *lastDotPos = strrchr(s, '.');
    return lastDotPos == nullptr ? nullptr : lastDotPos+1;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const
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
                if (entry.fullPathPattern) {
                    if (PatternMatcher((std::string(objectFullPath)+"."+keySuffixPattern).c_str(), true, true, true).matches(entry.key.c_str()))
                        result.push_back(partAfterLastDot(entry.key.c_str()));
                }
                else if ((anyObject || entry.ownerPattern->matches(objectFullPath))
                    &&
                    (entry.suffixPattern == nullptr ||
                     suffixMatcher.matches(partAfterLastDot(entry.key.c_str())) ||
                     entry.suffixPattern->matches(keySuffixPattern)))
                    result.push_back(partAfterLastDot(entry.key.c_str()));
            }
        }
    }
    //TODO remove duplicates
    return result;
}

void SectionBasedConfiguration::dump() const
{
    printf("Config:\n");
    for (const auto & it : config)
        printf("  %s = %s\n", it.first.c_str(), it.second.value.c_str());

    for (const auto & suffixBin : suffixBins) {
        const std::string& suffix = suffixBin.first;
        const SuffixBin& bin = suffixBin.second;
        printf("Suffix Bin %s:\n", suffix.c_str());
        for (const auto & entry : bin.entries)
            printf("  %s = %s\n", entry.key.c_str(), entry.value.c_str());
    }
    printf("Wildcard Suffix Bin:\n");
    for (const auto & entry : wildcardSuffixBin.entries)
        printf("  %s = %s\n", entry.key.c_str(), entry.value.c_str());
}

}  // namespace envir
}  // namespace omnetpp

