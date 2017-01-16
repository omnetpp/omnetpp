//==========================================================================
//  SECTIONBASEDCONFIG.CC - part of
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

#include <cassert>
#include <algorithm>
#include <sstream>
#include "common/opp_ctype.h"
#include "common/patternmatcher.h"
#include "common/stringtokenizer.h"
#include "common/stringtokenizer2.h"
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

namespace omnetpp {
namespace envir {

// XXX error messages (exceptions) should contain file/line info!
// XXX make sure quoting "$\{" works!
// TODO optimize storage (now keys with wildcard suffix are stored multiple times, in several suffix groups)

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

extern cConfigOption *CFGID_NETWORK;
extern cConfigOption *CFGID_RESULT_DIR;
extern cConfigOption *CFGID_SEED_SET;

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
    { CFGVAR_RESULTDIR,        "Value of the `result-dir` configuration option" },
    { CFGVAR_REPETITION,       "The iteration number in `0..N-1`, where `N` is the value of the `repeat` configuration option" },
    { CFGVAR_SEEDSET,          "Value of the `seed-set` configuration option" },
    { CFGVAR_ITERATIONVARS,    "Concatenation of all user-defined iteration variables in `name=value` form" },
    { CFGVAR_ITERATIONVARSF,   "Like ${iterationvars}, but sanitized for use as part of file names" },
    { nullptr,                 nullptr }
};

#define VARPOS_PREFIX    std::string("&")

std::string SectionBasedConfiguration::Entry::nullBasedir;

SectionBasedConfiguration::MatchableEntry::MatchableEntry(const MatchableEntry& e) : Entry(e)
{
    // apparently only used for std::vector storage
    ownerPattern = e.ownerPattern ? new PatternMatcher(*e.ownerPattern) : nullptr;
    suffixPattern = e.suffixPattern ? new PatternMatcher(*e.suffixPattern) : nullptr;
    fullPathPattern = e.fullPathPattern ? new PatternMatcher(*e.fullPathPattern) : nullptr;
}

SectionBasedConfiguration::MatchableEntry::~MatchableEntry()
{
    delete ownerPattern;
    delete suffixPattern;
    delete fullPathPattern;
}

//----

SectionBasedConfiguration::SectionBasedConfiguration()
{
    ini = nullptr;
    activeRunNumber = 0;
}

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
    const std::string *basedirRef = getPooledBaseDir(baseDir);
    for (const auto & it : options) {
        // validate the key, then store the option
        // XXX we should better use the code in the validate() method...
        const char *key = it.first.c_str();
        const char *value = it.second.c_str();
        const char *option = strchr(key, '.') ? strrchr(key, '.')+1 : key;  // check only the part after the last dot, i.e. recognize per-object keys as well
        cConfigOption *e = lookupConfigOption(option);
        if (!e)
            throw cRuntimeError("Unknown command-line configuration option --%s", key);
        if (!e->isPerObject() && key != option)
            throw cRuntimeError("Wrong command-line configuration option --%s: %s is not a per-object option", key, e->getName());
        std::string tmp;
        if (e->isPerObject() && key == option)
            key = (tmp = std::string("**.")+key).c_str();  // prepend with "**." (XXX this should be done in inifile contents too)
        if (!value[0])
            throw cRuntimeError("Missing value for command-line configuration option --%s", key);
        commandLineOptions.push_back(Entry(basedirRef, key, value));
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
    suffixGroups.clear();
    wildcardSuffixGroup.entries.clear();
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
    for (int i = 0; i < ini->getNumSections(); i++) {
        const char *section = ini->getSectionName(i);
        if (strcmp(section, "General") == 0)
            result.push_back(section);
        else if (strncmp(section, "Config ", 7) == 0)
            result.push_back(section+7);
        else
            ;  // nothing - leave out bogus section names
    }
    return result;
}

std::string SectionBasedConfiguration::getConfigDescription(const char *configName) const
{
    // determine the list of sections, from this one up to [General]
    std::vector<int> sectionChain = resolveSectionChain(configName);
    return opp_nulltoempty(internalGetValue(sectionChain, CFGID_DESCRIPTION->getName()));
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
    for (int & sectionId : sectionIds) {
        const char *section = ini->getSectionName(sectionId);
        if (strcmp(section, "General") == 0)
            result.push_back(section);
        else if (strncmp(section, "Config ", 7) == 0)
            result.push_back(section+7);
        else
            ;  // nothing - leave out bogus section names
    }
    return result;
}

int SectionBasedConfiguration::resolveConfigName(const char *configName) const
{
    if (!configName || !configName[0])
        throw cRuntimeError("Empty config name specified");
    int id = -1;
    if (!strcmp(configName, "General"))
        id = internalFindSection("General");
    if (id == -1)
        id = internalFindSection((std::string("Config ")+configName).c_str());
    return id;
}

void SectionBasedConfiguration::activateGlobalConfig()
{
    clear();
    for (auto & e : commandLineOptions) {
        std::string value = e.getValue(); // note: no substituteVariables(), too early for that
        const std::string *basedirRef = getPooledBaseDir(e.getBaseDirectory());
        addEntry(Entry(basedirRef, e.getKey(), value.c_str()));
    }
    int sectionId = resolveConfigName("General");
    if (sectionId != -1) {
        for (int entryId = 0; entryId < ini->getNumEntries(sectionId); entryId++) {
            // add entry to our tables
            const auto& e = ini->getEntry(sectionId, entryId);
            std::string value = e.getValue(); // note: no substituteVariables(), too early for that
            const std::string *basedirRef = getPooledBaseDir(e.getBaseDirectory());
            addEntry(Entry(basedirRef, e.getKey(), value.c_str()));
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

    // walk the list of fallback sections, and add entries to our tables
    // (config[] and params[]). Meanwhile, substitute the iteration values.
    // Note: entries added first will have precedence over those added later.
    for (auto & e : commandLineOptions) {
        std::string value = substituteVariables(e.getValue());
        const std::string *basedirRef = getPooledBaseDir(e.getBaseDirectory());
        addEntry(Entry(basedirRef, e.getKey(), value.c_str()));
    }
    for (int sectionId : sectionChain) {
        for (int entryId = 0; entryId < ini->getNumEntries(sectionId); entryId++) {
            // add entry to our tables
            const auto& e = ini->getEntry(sectionId, entryId);
            std::string value = substituteVariables(e.getValue(), sectionId, entryId, variables, locationToVarName);
            const std::string *basedirRef = getPooledBaseDir(e.getBaseDirectory());
            addEntry(Entry(basedirRef, e.getKey(), value.c_str()));
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

    // create variables
    int runnumberWidth = std::max(0, atoi(opp_nulltoempty(internalGetValue(sectionChain, CFGID_RUNNUMBER_WIDTH->getName()))));
    result[CFGVAR_INIFILE] = opp_nulltoempty(getFileName());
    result[CFGVAR_CONFIGNAME] = configName;
    result[CFGVAR_RUNNUMBER] = opp_stringf("%0*d", runnumberWidth, runNumber);
    result[CFGVAR_NETWORK] = opp_nulltoempty(internalGetValue(sectionChain, CFGID_NETWORK->getName()));
    result[CFGVAR_PROCESSID] = opp_stringf("%d", (int)getpid());
    result[CFGVAR_DATETIME] = opp_makedatetimestring();
    result[CFGVAR_RESULTDIR] = opp_nulltoempty(internalGetValue(sectionChain, CFGID_RESULT_DIR->getName(), CFGID_RESULT_DIR->getDefaultValue()));
    result[CFGVAR_RUNID] = result[CFGVAR_CONFIGNAME]+"-"+result[CFGVAR_RUNNUMBER]+"-"+result[CFGVAR_DATETIME]+"-"+result[CFGVAR_PROCESSID];

    // store iteration variables
    std::vector<std::string> varNames = scenario->getIterationVariableNames();
    for (std::string varName : varNames)
        result[varName] = scenario->getVariable(varName.c_str());

    // assemble ${iterationvars} and ${iterationvarsf}
    std::string iterationvars, iterationvarsf;
    for (std::string varName : varNames) {
        if (varName != CFGVAR_REPETITION) {
            std::string value = scenario->getVariable(varName.c_str());
            iterationvars += std::string(iterationvars.empty() ? "" : ", ") + "$" + varName + "=" + value;
            iterationvarsf += std::string(iterationvarsf.empty() ? "" : ",") + (opp_isalpha(varName[0]) ? varName + "=" : "") + opp_filenameencode(unquote(value)); // without dollar and space, possible quotes removed
        }
    }
    if (!iterationvarsf.empty())
        iterationvarsf += "-";  // for filenames
    result[CFGVAR_ITERATIONVARS] = iterationvars;
    result[CFGVAR_ITERATIONVARSF] = iterationvarsf;

    // experiment/measurement/replication must be done as last, because they may depend on the above vars
    result[CFGVAR_SEEDSET] = resolveConfigOption(CFGID_SEED_SET, sectionChain, result, locationToVarName);
    result[CFGVAR_EXPERIMENT] = resolveConfigOption(CFGID_EXPERIMENT_LABEL, sectionChain, result, locationToVarName);
    result[CFGVAR_MEASUREMENT] = resolveConfigOption(CFGID_MEASUREMENT_LABEL, sectionChain, result, locationToVarName);
    result[CFGVAR_REPLICATION] = resolveConfigOption(CFGID_REPLICATION_LABEL, sectionChain, result, locationToVarName);
    return result;
}

std::string SectionBasedConfiguration::resolveConfigOption(cConfigOption *option, const std::vector<int>& sectionChain, const StringMap& variables, const StringMap& locationToVarName) const
{
    int sectionId, entryId;
    const char *value = internalGetValue(sectionChain, option->getName(), option->getDefaultValue(), &sectionId, &entryId);
    return substituteVariables(value, sectionId, entryId, variables, locationToVarName);
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
    const char *repeat = internalGetValue(sectionChain, CFGID_REPEAT->getName());
    int repeatCount = (int)parseLong(repeat, nullptr, 1);
    Scenario::IterationVariable repetition;
    repetition.varName = CFGVAR_REPETITION;
    repetition.value = opp_stringf("0..%d", repeatCount-1);
    v.push_back(repetition);

    return v;
}

void SectionBasedConfiguration::parseVariable(const char *pos, std::string& outVarname, std::string& outValue, std::string& outParvar, const char *& outEndPos)
{
    Assert(pos[0] == '$' && pos[1] == '{');  // this is the way we've got to be invoked

    StringTokenizer2 tokenizer(pos+1, "}", "{}", "\"");
    const char *token = tokenizer.nextToken();  // ends at matching '}'
    if (!token)
        throw cRuntimeError("Missing '}' for '${'");
    outEndPos = pos + 1 + tokenizer.getTokenLength();

    // parse what's inside the ${...}
    const char *varbegin = nullptr;
    const char *varend = nullptr;
    const char *valuebegin = nullptr;
    const char *valueend = nullptr;
    const char *parvarbegin = nullptr;
    const char *parvarend = nullptr;

    const char *s = pos+2;
    while (opp_isspace(*s))
        s++;
    if (opp_isalphaext(*s)) {
        // must be a variable or a variable reference
        varbegin = varend = s;
        while (opp_isalnumext(*varend))
            varend++;
        s = varend;
        while (opp_isspace(*s))
            s++;
        if (*s == '}') {
            // ${x} syntax -- OK
        }
        else if (*s == '=' && *(s+1) != '=') {
            // ${x=...} syntax -- OK
            valuebegin = s+1;
        }
        else if (strchr(s, ',')) {
            // part of a valuelist -- OK
            valuebegin = varbegin;
            varbegin = varend = nullptr;
        }
        else {
            throw cRuntimeError("Missing '=' after '${varname'");
        }
    }
    else {
        valuebegin = s;
    }
    valueend = outEndPos;

    if (valuebegin) {
        // try to parse parvar, present when value ends in "! variable"
        const char *exclamationMark = strrchr(valuebegin, '!');
        if (exclamationMark) {
            const char *s = exclamationMark+1;
            while (opp_isspace(*s))
                s++;
            if (opp_isalphaext(*s)) {
                parvarbegin = s;
                while (opp_isalnumext(*s))
                    s++;
                parvarend = s;
                while (opp_isspace(*s))
                    s++;
                if (s != valueend) {
                    parvarbegin = parvarend = nullptr;  // no parvar after all
                }
            }
            if (parvarbegin) {
                valueend = exclamationMark;  // chop off "!parvarname"
            }
        }
    }

    outVarname = outValue = outParvar = "";
    if (varbegin)
        outVarname.assign(varbegin, varend-varbegin);
    if (valuebegin)
        outValue.assign(valuebegin, valueend-valuebegin);
    if (parvarbegin)
        outParvar.assign(parvarbegin, parvarend-parvarbegin);
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
        size_t endPos = endPtr - result.c_str();

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

        result.replace(pos, endPos-pos+1, value);
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
    return stringPool.get(result.c_str());
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

/*
   std::vector<int> SectionBasedConfiguration::resolveSectionChain(const char *sectionName) const
   {
    // determine the list of sections, from this one following the "extends" chain up to [General]
    std::vector<int> sectionChain;
    int generalSectionId = internalFindSection("General");
    int sectionId = internalGetSectionId(sectionName);
    while (true)
    {
        if (std::find(sectionChain.begin(), sectionChain.end(), sectionId) != sectionChain.end())
            throw cRuntimeError("Cycle detected in section fallback chain at [%s]", ini->getSectionName(sectionId));
        sectionChain.push_back(sectionId);
        int entryId = internalFindEntry(sectionId, CFGID_EXTENDS->getName());
        std::string extends = entryId==-1 ? "" : ini->getEntry(sectionId, entryId).getValue();
        if (extends.empty() && generalSectionId!=-1 && sectionId!=generalSectionId)
            extends = "General";
        if (extends.empty())
            break;
        sectionId = resolveConfigName(extends.c_str());
        if (sectionId == -1)
            break; // wrong config name
    }

    return sectionChain;
}
*/

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

        // find which group it should go into
        if (!suffixContainsWildcards) {
            // no wildcard in suffix (=group name)
            if (suffixGroups.find(suffix) == suffixGroups.end()) {
                // suffix group not yet exists, create it
                SuffixGroup& group = suffixGroups[suffix];

                // initialize group with matching wildcard keys seen so far
                for (auto & wildcardEntry : wildcardSuffixGroup.entries)
                    if (wildcardEntry.suffixPattern->matches(suffix.c_str()))
                        group.entries.push_back(wildcardEntry);
            }
            suffixGroups[suffix].entries.push_back(entry2);
        }
        else {
            // suffix contains wildcards: we need to add it to all existing suffix groups it matches
            // Note: if suffix also contains a hyphen, that's actually illegal (per-object
            // config entry names cannot be wildcarded, ie. "foo.bar.cmdenv-*" is illegal),
            // but causes no harm, because getPerObjectConfigEntry() won't look into the
            // wildcard group
            wildcardSuffixGroup.entries.push_back(entry2);
            for (auto & suffixGroup : suffixGroups)
                if (entry2.suffixPattern->matches(suffixGroup.first.c_str()))
                    (suffixGroup.second).entries.push_back(entry2);
        }
    }
}

void SectionBasedConfiguration::splitKey(const char *key, std::string& outOwnerName, std::string& outGroupName)
{
    std::string tmp = key;

    const char *lastDotPos = strrchr(key, '.');
    const char *doubleAsterisk = !lastDotPos ? nullptr : strstr(lastDotPos, "**");

    if (!lastDotPos || doubleAsterisk) {
        // complicated special case: there's a "**" after the last dot
        // (or there's no dot at all). Examples: "**baz", "net.**.foo**",
        // "net.**.foo**bar**baz"
        // Problem with this: are "foo" and "bar" part of the paramname (=group)
        // or the module name (=owner)? Can be either way. Only feasible solution
        // is to force matching of the full path (modulepath.paramname) against
        // the full pattern. Group name can be "*" plus segment of the pattern
        // after the last "**". (For example, for "net.**foo**bar", the group name
        // will be "*bar".)

        // find last "**"
        while (doubleAsterisk && strstr(doubleAsterisk+1, "**"))
            doubleAsterisk = strstr(doubleAsterisk+1, "**");
        outOwnerName = "";  // empty owner means "do fullPath match"
        outGroupName = !doubleAsterisk ? "*" : doubleAsterisk+1;
    }
    else {
        // normal case: group is the part after the last dot
        outOwnerName.assign(key, lastDotPos - key);
        outGroupName.assign(lastDotPos+1);
    }
}

const std::string *SectionBasedConfiguration::getPooledBaseDir(const char *basedir)
{
    StringSet::iterator it = basedirs.find(basedir);
    if (it == basedirs.end()) {
        basedirs.insert(basedir);
        it = basedirs.find(basedir);
    }
    const std::string *basedirRef = &(*it);
    return basedirRef;
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

static int findInArray(const char *s, const char **array)
{
    for (int i = 0; array[i] != nullptr; i++)
        if (!strcmp(s, array[i]))
            return i;
    return -1;
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
    const char *obsoleteSections[] = {
        "Parameters", "Cmdenv", "Tkenv", "OutVectors", "Partitioning", "DisplayStrings", nullptr
    };
    const char *cmdenvNames[] = {
        "autoflush", "event-banner-details", "event-banners", "express-mode",
        "message-trace", "module-messages", "output-file", "performance-display",
        "runs-to-execute", "status-frequency", nullptr
    };
    const char *tkenvNames[] = {
        "anim-methodcalls", "animation-enabled", "animation-msgclassnames",
        "animation-msgcolors", "animation-msgnames", "animation-speed",
        "default-run", "expressmode-autoupdate", "methodcalls-delay",
        "next-event-markers", "penguin-mode", "plugin-path", "print-banners",
        "senddirect-arrows", "show-bubbles", "show-layouting", "slowexec-delay",
        "update-freq-express", "update-freq-fast", "use-mainwindow",
        "use-new-layouter", nullptr
    };

    // warn for obsolete section names and config keys
    for (int i = 0; i < ini->getNumSections(); i++) {
        const char *section = ini->getSectionName(i);
        if (findInArray(section, obsoleteSections) != -1)
            throw cRuntimeError("Obsolete section name [%s] found, please convert the ini file to 4.x format", section);

        int numEntries = ini->getNumEntries(i);
        for (int j = 0; j < numEntries; j++) {
            const char *key = ini->getEntry(i, j).getKey();
            if (findInArray(key, cmdenvNames) != -1 || findInArray(key, tkenvNames) != -1)
                throw cRuntimeError("Obsolete configuration option %s= found, please convert the ini file to 4.x format", key);
        }
    }

    // check section names
    std::set<std::string> configNames;
    for (int i = 0; i < ini->getNumSections(); i++) {
        const char *section = ini->getSectionName(i);
        const char *configName = nullptr;
        if (strcmp(section, "General") == 0)
            ;  // OK
        else if (strncmp(section, "Config ", 7) == 0)
            configName = section+7;
        else
            throw cRuntimeError("Invalid section name [%s], should be [General] or [Config <name>]", section);
        if (configName) {
            if (*configName == ' ')
                throw cRuntimeError("Invalid section name [%s]: Too many spaces", section);
            if (!opp_isalphaext(*configName) && *configName != '_')
                throw cRuntimeError("Invalid section name [%s]: Config name must begin with a letter or underscore", section);
            for (const char *s = configName; *s; s++)
                if (!opp_isalnumext(*s) && strchr("-_@", *s) == nullptr)
                    throw cRuntimeError("Invalid section name [%s], contains illegal character '%c'", section, *s);

            if (configNames.find(configName) != configNames.end())
                throw cRuntimeError("Configuration name '%s' not unique", configName);
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
                    throw cRuntimeError("Configuration option %s should be specified per object, try **.%s=", key, key);
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
                        std::string sectionName = std::string("Config ") + configName;
                        int configId = internalFindSection(sectionName.c_str());
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
                    // XXX suffix (probably) should not contain wildcard; but surely not "**" !!!!
                    cConfigOption *e = lookupConfigOption(suffix.c_str());
                    if (!e && isIgnorableConfigKey(ignorableConfigKeys, suffix.c_str()))
                        continue;
                    if (!e || !e->isPerObject()) {
                        if (suffix == "type-name")
                            throw cRuntimeError("Configuration option 'type-name' has been renamed to 'typename', please update key '%s' in the ini file", key);
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
    return it == config.end() ? (KeyValue&)nullEntry : (KeyValue&)it->second;
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
    const SectionBasedConfiguration::MatchableEntry& entry = (MatchableEntry&)getParameterEntry(moduleFullPath, paramName, hasDefaultValue);
    return entry.getKey() == nullptr ? nullptr : entry.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // look up which group; paramName serves as suffix (ie. group name)
    std::map<std::string, SuffixGroup>::const_iterator it = suffixGroups.find(paramName);
    const SuffixGroup *group = it == suffixGroups.end() ? &wildcardSuffixGroup : &it->second;

    // find first match in the group
    for (const auto & entry : group->entries) {
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

std::vector<const char *> SectionBasedConfiguration::getKeyValuePairs() const
{
    std::vector<const char *> result;
    for (const auto & entry : entries) {
        result.push_back(entry.getKey());
        result.push_back(entry.getValue());
    }
    return result;
}

std::vector<const char *> SectionBasedConfiguration::getParameterKeyValuePairs() const
{
    std::vector<const char *> result;
    for (const auto & entry : entries) {
        const char *lastDotPos = strrchr(entry.getKey(), '.');
        bool containsHyphen = lastDotPos && strchr(lastDotPos, '-') != nullptr;
        if (lastDotPos && !containsHyphen) {
            result.push_back(entry.getKey());
            result.push_back(entry.getValue());
        }
    }
    return result;
}

const char *SectionBasedConfiguration::getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const
{
    const SectionBasedConfiguration::MatchableEntry& entry = (MatchableEntry&)getPerObjectConfigEntry(objectFullPath, keySuffix);
    return entry.getKey() == nullptr ? nullptr : entry.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const
{
    // look up which group; keySuffix serves as group name
    // Note: we do not accept wildcards in the config key's name (ie. "**.record-*" is invalid),
    // so we ignore the wildcard group.
    std::map<std::string, SuffixGroup>::const_iterator it = suffixGroups.find(keySuffix);
    if (it == suffixGroups.end())
        return nullEntry;  // no such group

    const SuffixGroup *suffixGroup = &it->second;

    // find first match in the group
    for (const auto & entry : suffixGroup->entries) {
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

std::vector<const char *> SectionBasedConfiguration::getMatchingPerObjectConfigKeys(const char *objectFullPathPattern, const char *keySuffixPattern) const
{
    std::vector<const char *> result;

    // only concrete objects or "**" is accepted, because we are not prepared
    // to handle the "pattern matches pattern" case (see below as well).
    bool anyObject = strcmp(objectFullPathPattern, "**") == 0;
    if (!anyObject && PatternMatcher::containsWildcards(objectFullPathPattern))
        throw cRuntimeError("getMatchingPerObjectConfigKeys: Invalid objectFullPath parameter: The only wildcard pattern accepted is '**'");

    // check all suffix groups whose name matches the pattern
    PatternMatcher suffixMatcher(keySuffixPattern, true, true, true);
    for (const auto & suffixGroup : suffixGroups) {
        const char *suffix = suffixGroup.first.c_str();
        if (suffixMatcher.matches(suffix)) {
            // find all matching entries from this suffix group.
            // We'll have a little problem where key ends in wildcard (i.e. entry.suffixPattern!=nullptr);
            // there we'd have to determine whether two *patterns* match. We resolve this
            // by checking whether one pattern matches the other one as string, and vica versa.
            const SuffixGroup& group = suffixGroup.second;
            for (const auto & entry : group.entries) {
                if ((anyObject || entry.ownerPattern->matches(objectFullPathPattern))
                    &&
                    (entry.suffixPattern == nullptr ||
                     suffixMatcher.matches(partAfterLastDot(entry.key.c_str())) ||
                     entry.suffixPattern->matches(keySuffixPattern)))
                    result.push_back(entry.key.c_str());
            }
        }
    }
    return result;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const
{
    std::vector<const char *> keys = getMatchingPerObjectConfigKeys(objectFullPath, keySuffixPattern);
    for (auto & key : keys)
        key = partAfterLastDot(key);
    return keys;
}

void SectionBasedConfiguration::dump() const
{
    printf("Config:\n");
    for (const auto & it : config)
        printf("  %s = %s\n", it.first.c_str(), it.second.value.c_str());

    for (const auto & suffixGroup : suffixGroups) {
        const std::string& suffix = suffixGroup.first;
        const SuffixGroup& group = suffixGroup.second;
        printf("Suffix Group %s:\n", suffix.c_str());
        for (const auto & entry : group.entries)
            printf("  %s = %s\n", entry.key.c_str(), entry.value.c_str());
    }
    printf("Wildcard Suffix Group:\n");
    for (const auto & entry : wildcardSuffixGroup.entries)
        printf("  %s = %s\n", entry.key.c_str(), entry.value.c_str());
}

}  // namespace envir
}  // namespace omnetpp

