//==========================================================================
//  SECTIONBASEDCONFIG.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <algorithm>
#include <sstream>
#include "opp_ctype.h"
#include "sectionbasedconfig.h"
#include "cconfigreader.h"
#include "patternmatcher.h"
#include "valueiterator.h"
#include "cexception.h"
#include "scenario.h"
#include "globals.h"
#include "cconfigkey.h"
#include "stringtokenizer.h"
#include "timeutil.h"
#include "platmisc.h"   //getpid()

USING_NAMESPACE


//XXX test the likes of: **.apply-default, **whatever.apply=default, whatever**.apply-default!!! make them illegal?
//XXX error messages (exceptions) should contain file/line info!
//XXX make sure quoting "$\{" works!
//TODO optimize storage (now keys with wildcard groupName are stored multiple times, in several groups)


Register_PerRunConfigEntry(CFGID_DESCRIPTION, "description", CFG_STRING, NULL, "Descriptive name for the given simulation configuration. Descriptions get displayed in the run selection dialog.");
Register_PerRunConfigEntry(CFGID_EXTENDS, "extends", CFG_STRING, NULL, "Name of the configuration this section is based on. Entries from that section will be inherited and can be overridden. In other words, configuration lookups will fall back to the base section.");
Register_PerRunConfigEntry(CFGID_CONSTRAINT, "constraint", CFG_STRING, NULL, "For scenarios. Contains an expression that iteration variables (${} syntax) must satisfy for that simulation to run. Example: $i < $j+1.");
Register_PerRunConfigEntry(CFGID_REPEAT, "repeat", CFG_INT, "1", "For scenarios. Specifies how many replications should be done with the same parameters (iteration variables). This is typically used to perform multiple runs with different random number seeds. The loop variable is available as ${repetition}. See also: seed-set= key.");
Register_PerRunConfigEntry(CFGID_EXPERIMENT_LABEL, "experiment-label", CFG_STRING, "${configname}", "Identifies the simulation experiment (which consists of several, potentially repeated measurements). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_MEASUREMENT_LABEL, "measurement-label", CFG_STRING, "${iterationvars}", "Identifies the measurement within the experiment. This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_REPLICATION_LABEL, "replication-label", CFG_STRING, "#${repetition}", "Identifies one replication of a measurement (see repeat= and measurement-label= as well). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_RUNNUMBER_WIDTH, "runnumber-width", CFG_INT, "0", "Setting a nonzero value will cause the $runnumber variable to get padded with leading zeroes to the given length.");
Register_PerObjectConfigEntry(CFGID_APPLY_DEFAULT, "apply-default", CFG_BOOL, "false", "Applies to module parameters: whether NED default values should be assigned if present.");

extern cConfigKey *CFGID_NETWORK;
extern cConfigKey *CFGID_RESULT_DIR;
extern cConfigKey *CFGID_SEED_SET;

// table to be kept consistent with scave/fields.cc
static struct ConfigVarDescription { const char *name, *description; } configVarDescriptions[] = {
    { CFGVAR_RUNID,            "A reasonably globally unique identifier for the run, produced by concatenating the configuration name, run number, date/time, etc." },
    { CFGVAR_INIFILE,          "Name of the (primary) inifile" },
    { CFGVAR_CONFIGNAME,       "Name of the active configuration" },
    { CFGVAR_RUNNUMBER,        "Sequence number of the current run within all runs in the active configuration" },
    { CFGVAR_NETWORK,          "Value of the \"network\" configuration option" },
    { CFGVAR_EXPERIMENT,       "Value of the \"experiment-label\" configuration option" },
    { CFGVAR_MEASUREMENT,      "Value of the \"measurement-label\" configuration option" },
    { CFGVAR_REPLICATION,      "Value of the \"replication-label\" configuration option" },
    { CFGVAR_PROCESSID,        "PID of the simulation process" },
    { CFGVAR_DATETIME,         "Date and time the simulation run was started" },
    { CFGVAR_RESULTDIR,        "Value of the \"result-dir\" configuration option" },
    { CFGVAR_REPETITION,       "The iteration number in 0..N-1, where N is the value of the \"repeat\" configuration option" },
    { CFGVAR_SEEDSET,          "Value of the \"seed-set\" configuration option" },
    { CFGVAR_ITERATIONVARS,    "Concatenation of all user-defined iteration variables in name=value form" },
    { CFGVAR_ITERATIONVARS2,   "Concatenation of all user-defined iteration variables in name=value form, plus ${repetition}" },
    { NULL,                    NULL }
};

#define VARPOS_PREFIX  std::string("&")

std::string SectionBasedConfiguration::KeyValue1::nullbasedir;


SectionBasedConfiguration::SectionBasedConfiguration()
{
    ini = NULL;
    activeRunNumber = 0;
}

SectionBasedConfiguration::~SectionBasedConfiguration()
{
    clear();
    delete ini;
}

void SectionBasedConfiguration::setConfigurationReader(cConfigurationReader *ini)
{
    clear();
    this->ini = ini;
    nullEntry.setBaseDirectory(ini->getDefaultBaseDirectory());
}

void SectionBasedConfiguration::setCommandLineConfigOptions(const std::map<std::string,std::string>& options)
{
    commandLineOptions.clear();
    for (StringMap::const_iterator it=options.begin(); it!=options.end(); it++)
    {
        // validate the key, then store the option
        const char *key = it->first.c_str();
        const char *value = it->second.c_str();
        cConfigKey *e = lookupConfigKey(key);
        if (!e)
            throw cRuntimeError("Unknown command-line configuration option: --%s", key);
        if (e->isPerObject())
            throw cRuntimeError("Per-object configuration options cannot be specified on the command line: --%s", key);
        commandLineOptions.push_back(KeyValue1(NULL, key, value));
    }
}

void SectionBasedConfiguration::clear()
{
    // note: this gets called between activateConfig() calls, so "ini" must NOT be NULL'ed out here
    activeConfig = "";
    activeRunNumber = 0;
    config.clear();
    groups.clear();
    wildcardGroup.entries.clear();
    variables.clear();
}

void SectionBasedConfiguration::initializeFrom(cConfiguration *conf)
{
    throw cRuntimeError("SectionBasedConfiguration: initializeFrom() not supported");
}

const char *SectionBasedConfiguration::getFileName() const
{
    return ini==NULL ? NULL : ini->getFileName();
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
    for (int i=0; i<ini->getNumSections(); i++)
    {
        const char *section = ini->getSectionName(i);
        if (strcmp(section, "General")==0)
            result.push_back(section);
        else if (strncmp(section, "Config ", 7)==0)
            result.push_back(section+7);
        else
            ; // nothing - leave out bogus section names
    }
    return result;
}

std::string SectionBasedConfiguration::getConfigDescription(const char *configName) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1)
        throw cRuntimeError("No such config: %s", configName);

    // determine the list of sections, from this one up to [General]
    std::vector<int> sectionChain = resolveSectionChain(sectionId);
    return opp_nulltoempty(internalGetValue(sectionChain, "description"));
}

std::string SectionBasedConfiguration::getBaseConfig(const char *configName) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1)
        throw cRuntimeError("No such config: %s", configName);
    int entryId = internalFindEntry(sectionId, "extends");
    std::string extends = entryId==-1 ? "" : ini->getEntry(sectionId, entryId).getValue();
    if (extends.empty())
        extends = "General";
    int baseSectionId = resolveConfigName(extends.c_str());
    return baseSectionId==-1 ? "" : extends;
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

static std::string opp_makedatetimestring()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char timestr[32];
    sprintf(timestr, "%04d%02d%02d-%02d:%02d:%02d",
            1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    return timestr;
}

void SectionBasedConfiguration::activateConfig(const char *configName, int runNumber)
{
    clear();

    activeConfig = configName==NULL ? "" : configName;
    activeRunNumber = runNumber;

    int sectionId = resolveConfigName(configName);
    if (sectionId == -1 && !strcmp(configName, "General"))
        return;  // allow activating "General" even if it's empty
    if (sectionId == -1)
        throw cRuntimeError("No such config: %s", configName);

    // determine the list of sections, from this one up to [General]
    std::vector<int> sectionChain = resolveSectionChain(sectionId);

    // extract all iteration vars from values within this section
    std::vector<IterationVariable> itervars = collectIterationVariables(sectionChain);

    // see if there's a constraint given
    int constraintEntryId = internalFindEntry(sectionId, "constraint");
    const char *constraint = constraintEntryId!=-1 ? ini->getEntry(sectionId, constraintEntryId).getValue() : NULL;

    // determine the values to substitute into the iteration vars (${...})
    try
    {
        Scenario scenario(itervars, constraint);
        int numRuns = scenario.getNumRuns();
        if (runNumber<0 || runNumber>=numRuns)
            throw cRuntimeError("Run number %d is out of range for configuration `%s': it contains %d run(s)", runNumber, configName, numRuns);

        scenario.gotoRun(runNumber);
        setupVariables(getActiveConfigName(), getActiveRunNumber(), &scenario, sectionChain);
    }
    catch (std::exception& e)
    {
        throw cRuntimeError("Scenario generator: %s", e.what());
    }

    // walk the list of fallback sections, and add entries to our tables
    // (config[] and params[]). Meanwhile, substitute the iteration values.
    // Note: entries added first will have precedence over those added later.
    for (int i=0; i < (int)commandLineOptions.size(); i++)
    {
        addEntry(commandLineOptions[i]);
    }
    for (int i=0; i < (int)sectionChain.size(); i++)
    {
        int sectionId = sectionChain[i];
        for (int entryId=0; entryId<ini->getNumEntries(sectionId); entryId++)
        {
            // add entry to our tables
            addEntry(convert(sectionId, entryId));
        }
    }
}

void SectionBasedConfiguration::setupVariables(const char *configName, int runNumber, Scenario *scenario, const std::vector<int>& sectionChain)
{
    // create variables
    int runnumberWidth = std::max(0,atoi(opp_nulltoempty(internalGetValue(sectionChain, CFGID_RUNNUMBER_WIDTH->getName()))));
    variables[CFGVAR_INIFILE] = opp_nulltoempty(getFileName());
    variables[CFGVAR_CONFIGNAME] = configName;
    variables[CFGVAR_RUNNUMBER] = opp_stringf("%0*d", runnumberWidth, runNumber);
    variables[CFGVAR_NETWORK] = opp_nulltoempty(internalGetValue(sectionChain, CFGID_NETWORK->getName()));
    variables[CFGVAR_PROCESSID] = opp_stringf("%d", (int) getpid());
    variables[CFGVAR_DATETIME] = opp_makedatetimestring();
    variables[CFGVAR_RESULTDIR] = opp_nulltoempty(internalGetValue(sectionChain, CFGID_RESULT_DIR->getName(), CFGID_RESULT_DIR->getDefaultValue()));
    variables[CFGVAR_RUNID] = runId = variables[CFGVAR_CONFIGNAME]+"-"+variables[CFGVAR_RUNNUMBER]+"-"+variables[CFGVAR_DATETIME]+"-"+variables[CFGVAR_PROCESSID];

    // store iteration variables, and also their "positions" (iteration count) as "&varid"
    const std::vector<IterationVariable>& itervars = scenario->getIterationVariables();
    for (int i=0; i<(int)itervars.size(); i++)
    {
        const char *varid = itervars[i].varid.c_str();
        variables[varid] = scenario->getVariable(varid);
        variables[VARPOS_PREFIX+varid] = opp_stringf("%d", scenario->getIteratorPosition(varid));
    }

    // assemble ${iterationvars}
    std::string iterationvars, iterationvars2;
    for (int i=0; i<(int)itervars.size(); i++)
    {
        std::string txt = "$" + itervars[i].varname + "=" + scenario->getVariable(itervars[i].varid.c_str());
        if (itervars[i].varname != CFGVAR_REPETITION)
            iterationvars += std::string(i>0?", ":"") + txt;
        iterationvars2 += std::string(i>0?", ":"") + txt;
    }
    variables[CFGVAR_ITERATIONVARS] = iterationvars;
    variables[CFGVAR_ITERATIONVARS2] = iterationvars2;

    // experiment/measurement/replication must be done as last, because they may depend on the above vars
    variables[CFGVAR_SEEDSET] = substituteVariables(internalGetValue(sectionChain, CFGID_SEED_SET->getName(), CFGID_SEED_SET->getDefaultValue()), -1, -1);
    variables[CFGVAR_EXPERIMENT] = substituteVariables(internalGetValue(sectionChain, CFGID_EXPERIMENT_LABEL->getName(), CFGID_EXPERIMENT_LABEL->getDefaultValue()), -1, -1);
    variables[CFGVAR_MEASUREMENT] = substituteVariables(internalGetValue(sectionChain, CFGID_MEASUREMENT_LABEL->getName(), CFGID_MEASUREMENT_LABEL->getDefaultValue()), -1, -1);
    variables[CFGVAR_REPLICATION] = substituteVariables(internalGetValue(sectionChain, CFGID_REPLICATION_LABEL->getName(), CFGID_REPLICATION_LABEL->getDefaultValue()), -1, -1);
}

int SectionBasedConfiguration::getNumRunsInScenario(const char *configName) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1)
        return 0;  // no such config

    // extract all iteration vars from values within this section
    std::vector<int> sectionChain = resolveSectionChain(sectionId);
    std::vector<IterationVariable> v = collectIterationVariables(sectionChain);

    // see if there's a constraint given
    int constraintEntryId = internalFindEntry(sectionId, "constraint");
    const char *constraint = constraintEntryId!=-1 ? ini->getEntry(sectionId, constraintEntryId).getValue() : NULL;

    // count the runs and return the result
    try {
        return Scenario(v, constraint).getNumRuns();
    }
    catch (std::exception& e) {
        throw cRuntimeError("Scenario generator: %s", e.what());
    }
}

std::vector<std::string> SectionBasedConfiguration::unrollScenario(const char *configName, bool detailed) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1)
        throw cRuntimeError("No such config: %s", configName);

    // extract all iteration vars from values within this section
    std::vector<int> sectionChain = resolveSectionChain(sectionId);
    std::vector<IterationVariable> itervars = collectIterationVariables(sectionChain);

    // see if there's a constraint given
    int constraintEntryId = internalFindEntry(sectionId, "constraint"); //XXX use constant (multiple places here!)
    const char *constraint = constraintEntryId!=-1 ? ini->getEntry(sectionId, constraintEntryId).getValue() : NULL;

    // setupVariables() overwrites variables[], so we need to save/restore it
    StringMap savedVariables = variables;

    // iterate over all runs in the scenario
    try {
        Scenario scenario(itervars, constraint);
        std::vector<std::string> result;
        if (scenario.restart())
        {
            for (;;)
            {
                // generate a string for the current run
                std::string runstring;
                if (!detailed)
                {
                    runstring = scenario.str();
                }
                else
                {
                    // itervars, plus all entries that contain ${..}
                    runstring += std::string("\t# ") + scenario.str() + "\n";
                    (const_cast<SectionBasedConfiguration *>(this))->setupVariables(configName, result.size(), &scenario, sectionChain);
                    for (int i=0; i<(int)sectionChain.size(); i++)
                    {
                        int sectionId = sectionChain[i];
                        for (int entryId=0; entryId<ini->getNumEntries(sectionId); entryId++)
                        {
                            // add entry to our tables
                            const cConfigurationReader::KeyValue& entry = ini->getEntry(sectionId, entryId);
                            if (strstr(entry.getValue(), "${")!=NULL)
                            {
                                std::string actualValue = substituteVariables(entry.getValue(), sectionId, entryId);
                                runstring += std::string("\t") + entry.getKey() + " = " + actualValue + "\n";
                            }
                        }
                    }
                }
                result.push_back(runstring);

                // move to the next run
                if (!scenario.next())
                    break;
            }
        }
        (const_cast<SectionBasedConfiguration *>(this))->variables = savedVariables;
        return result;
    }
    catch (std::exception& e)
    {
        (const_cast<SectionBasedConfiguration *>(this))->variables = savedVariables;
        throw cRuntimeError("Scenario generator: %s", e.what());
    }
}

std::vector<SectionBasedConfiguration::IterationVariable> SectionBasedConfiguration::collectIterationVariables(const std::vector<int>& sectionChain) const
{
    std::vector<IterationVariable> v;
    int unnamedCount = 0;
    for (int i=0; i<(int)sectionChain.size(); i++)
    {
        int sectionId = sectionChain[i];
        for (int entryId=0; entryId<ini->getNumEntries(sectionId); entryId++)
        {
            const cConfigurationReader::KeyValue& entry = ini->getEntry(sectionId, entryId);
            const char *pos = entry.getValue();
            int k = 0;
            while ((pos = strstr(pos, "${")) != NULL)
            {
                IterationVariable loc;
                try {
                    parseVariable(pos, loc.varname, loc.value, loc.parvar, pos);
                } catch (std::exception& e) {
                    throw cRuntimeError("Scenario generator: %s at %s=%s", e.what(), entry.getKey(), entry.getValue());
                }

                if (!loc.value.empty() && loc.parvar.empty())
                {
                    // store variable
                    if (!loc.varname.empty())
                    {
                        // check it does not conflict with other iteration variables or predefined variables
                        for (int j=0; j<(int)v.size(); j++)
                            if (v[j].varname==loc.varname)
                                throw cRuntimeError("Scenario generator: redefinition of iteration variable ${%s} in the configuration", loc.varname.c_str());
                        if (isPredefinedVariable(loc.varname.c_str()))
                            throw cRuntimeError("Scenario generator: ${%s} is a predefined variable and cannot be changed", loc.varname.c_str());
                        // use name for id
                        loc.varid = loc.varname;
                    }
                    else
                    {
                        // unnamed variable: generate id (identifies location) and name ($0,$1,$2,etc)
                        loc.varid = opp_stringf("%d-%d-%d", sectionId, entryId, k);
                        loc.varname = opp_stringf("%d", unnamedCount++);
                    }
                    v.push_back(loc);
                }
                k++;
            }
        }
    }

    // register ${repetition}, based on the repeat= config entry
    const char *repeat = internalGetValue(sectionChain, "repeat");
    int repeatCount = (int) parseLong(repeat, NULL, 1);
    IterationVariable repetition;
    repetition.varid = repetition.varname = CFGVAR_REPETITION;
    repetition.value = opp_stringf("0..%d", repeatCount-1);
    v.push_back(repetition);

    return v;
}

void SectionBasedConfiguration::parseVariable(const char *pos, std::string& outVarname, std::string& outValue, std::string& outParvar, const char *&outEndPos)
{
    Assert(pos[0]=='$' && pos[1]=='{'); // this is the way we've got to be invoked
    outEndPos = strchr(pos, '}');
    if (!outEndPos)
        throw cRuntimeError("missing '}' for '${'");

    // parse what's inside the ${...}
    const char *varbegin = NULL;
    const char *varend = NULL;
    const char *valuebegin = NULL;
    const char *valueend = NULL;
    const char *parvarbegin = NULL;
    const char *parvarend = NULL;

    const char *s = pos+2;
    while (opp_isspace(*s)) s++;
    if (opp_isalpha(*s))
    {
        // must be a variable or a variable reference
        varbegin = varend = s;
        while (opp_isalnum(*varend)) varend++;
        s = varend;
        while (opp_isspace(*s)) s++;
        if (*s=='}') {
            // ${x} syntax -- OK
        }
        else if (*s=='=' && *(s+1)!='=') {
            // ${x=...} syntax -- OK
            valuebegin = s+1;
        }
        else {
            throw cRuntimeError("missing '=' after '${varname'");
        }
    } else {
        valuebegin = s;
    }
    valueend = outEndPos;

    if (valuebegin)
    {
        // try to parse parvar, present when value ends in "! variable"
        const char *exclamationMark = strrchr(valuebegin, '!');
        if (exclamationMark)
        {
            const char *s = exclamationMark+1;
            while (opp_isspace(*s)) s++;
            if (opp_isalpha(*s))
            {
                parvarbegin = s;
                while (opp_isalnum(*s)) s++;
                parvarend = s;
                while (opp_isspace(*s)) s++;
                if (s!=valueend)
                {
                    parvarbegin = parvarend = NULL; // no parvar after all
                }
            }
            if (parvarbegin)  {
                valueend = exclamationMark;  // chop off "!parvarname"
            }
        }
    }

    if (varbegin && parvarbegin)
        throw cRuntimeError("the ${var=...} and ${...!var} syntaxes cannot be used together");

    outVarname = outValue = outParvar = "";
    if (varbegin)
        outVarname.assign(varbegin, varend-varbegin);
    if (valuebegin)
        outValue.assign(valuebegin, valueend-valuebegin);
    if (parvarbegin)
        outParvar.assign(parvarbegin, parvarend-parvarbegin);
    //printf("DBG: var=`%s', value=`%s', parvar=`%s'\n", outVarname.c_str(), outValue.c_str(), outParvar.c_str());
}

std::string SectionBasedConfiguration::substituteVariables(const char *text, int sectionId, int entryId) const
{
    std::string result = opp_nulltoempty(text);
    int k = 0;  // counts "${" occurrences
    const char *pos, *endPos;
    while ((pos = strstr(result.c_str(), "${")) != NULL)
    {
        std::string varname, iterationstring, parvar;
        parseVariable(pos, varname, iterationstring, parvar, endPos);
        std::string value;
        if (parvar.empty())
        {
            // handle named and unnamed iteration variable references
            std::string varid = !varname.empty() ? varname : opp_stringf("%d-%d-%d", sectionId, entryId, k);
            StringMap::const_iterator it = variables.find(varid.c_str());
            if (it==variables.end())
                throw cRuntimeError("no such variable: ${%s}", varid.c_str());
            value = it->second;
        }
        else
        {
            // handle parallel iterations: if parvar is at its kth value,
            // we should take the kth value from iterationstring as well
            StringMap::const_iterator it = variables.find(VARPOS_PREFIX+parvar);
            if (it==variables.end())
                throw cRuntimeError("no such variable: ${%s}", parvar.c_str());
            int parvarPos = atoi(it->second.c_str());
            value = ValueIterator(iterationstring.c_str()).get(parvarPos);
        }
        result.replace(pos-result.c_str(), endPos-pos+1, value);
        k++;
    }
    return result;
}

const char *SectionBasedConfiguration::substituteVariables(const char *value)
{
    if (value==NULL || strstr(value, "${")==NULL)
        return value;

    // returned string needs to be stringpooled
    std::string result = substituteVariables(value, -1, -1);
    return stringPool.get(result.c_str());
}

const char *SectionBasedConfiguration::getVariable(const char *varname) const
{
    StringMap::const_iterator it = variables.find(varname);
    return it==variables.end() ? NULL : it->second.c_str();
}

std::vector<const char *> SectionBasedConfiguration::getIterationVariableNames() const
{
    std::vector<const char *> result;
    for (StringMap::const_iterator it = variables.begin(); it!=variables.end(); ++it)
        if (opp_isalpha(it->first[0]) && !isPredefinedVariable(it->first.c_str()))  // skip unnamed and predefined ones
            result.push_back(it->first.c_str());
    return result;
}

std::vector<const char *> SectionBasedConfiguration::getPredefinedVariableNames() const
{
    std::vector<const char *> result;
    for (int i=0; configVarDescriptions[i].name; i++)
        result.push_back(configVarDescriptions[i].name);
    return result;
}

const char *SectionBasedConfiguration::getVariableDescription(const char *varname) const
{
    for (int i=0; configVarDescriptions[i].name; i++)
        if (strcmp(varname, configVarDescriptions[i].name)==0)
            return configVarDescriptions[i].description;
    if (!opp_isempty(getVariable(varname)))
        return "User-defined iteration variable";
    return NULL;
}

bool SectionBasedConfiguration::isPredefinedVariable(const char *varname) const
{
    for (int i=0; configVarDescriptions[i].name; i++)
        if (strcmp(varname, configVarDescriptions[i].name)==0)
            return true;
    return false;
}

std::vector<int> SectionBasedConfiguration::resolveSectionChain(int sectionId) const
{
    return resolveSectionChain(ini->getSectionName(sectionId));
}

std::vector<int> SectionBasedConfiguration::resolveSectionChain(const char *sectionName) const
{
    // determine the list of sections, from this one following the "extends" chain up to [General]
    std::vector<int> sectionChain;
    int generalSectionId = internalFindSection("General");
    int sectionId = internalGetSectionId(sectionName);
    while (true)
    {
        if (std::find(sectionChain.begin(), sectionChain.end(), sectionId) != sectionChain.end())
            throw cRuntimeError("Cycle detected in section fallback chain at: [%s]", ini->getSectionName(sectionId));
        sectionChain.push_back(sectionId);
        int entryId = internalFindEntry(sectionId, "extends");
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

void SectionBasedConfiguration::addEntry(const KeyValue1& entry)
{
    const std::string& key = entry.key;
    const char *lastDot = strrchr(key.c_str(), '.');
    if (!lastDot)
    {
        // config: add if not already in there
        if (PatternMatcher::containsWildcards(key.c_str()))
            throw cRuntimeError("invalid config key '%s': config keys cannot contain wildcard characters", key.c_str());
        if (config.find(key)==config.end())
            config[key] = entry;
    }
    else
    {
        // key contains a dot: parameter or per-object configuration
        // Note: since the last part of they key might contain widcards, it is not really possible
        // to distinguish the two. Cf "vector-recording", "vector-*" and "vector*"

        // analyze key and create appropriate entry
        std::string ownerName;
        std::string groupName;
        bool isApplyDefault;
        splitKey(key.c_str(), ownerName, groupName, isApplyDefault);
        bool groupContainsWildcards = PatternMatcher::containsWildcards(groupName.c_str());

        KeyValue2 entry2(entry);
        if (!ownerName.empty())
            entry2.ownerPattern = new PatternMatcher(ownerName.c_str(), true, true, true);
         else
            entry2.fullPathPattern = new PatternMatcher(key.c_str(), true, true, true);
        entry2.groupPattern = groupContainsWildcards ? new PatternMatcher(groupName.c_str(), true, true, true) : NULL;
        entry2.isApplyDefault = isApplyDefault;
        if (isApplyDefault)
            entry2.applyDefaultValue = strcmp(entry.value.c_str(), "true")==0;

        // find which group it should go into
        if (!groupContainsWildcards)
        {
            // no wildcard in group name
            if (groups.find(groupName)==groups.end()) {
                // group not yet exists, create it
                Group& group = groups[groupName];

                // initialize group with matching wildcard keys seen so far
                for (int k=0; k<(int)wildcardGroup.entries.size(); k++)
                    if (wildcardGroup.entries[k].groupPattern->matches(groupName.c_str()))
                        group.entries.push_back(wildcardGroup.entries[k]);
            }
            groups[groupName].entries.push_back(entry2);
        }
        else
        {
            // groupName contains wildcards: we need to add it to all existing groups it matches
            wildcardGroup.entries.push_back(entry2);
            for (std::map<std::string,Group>::iterator it = groups.begin(); it!=groups.end(); it++)
                if (entry2.groupPattern->matches(it->first.c_str()))
                    (it->second).entries.push_back(entry2);
        }
    }
}

void SectionBasedConfiguration::splitKey(const char *key, std::string& outOwnerName, std::string& outGroupName, bool& outIsApplyDefault)
{
    std::string tmp = key;
    int keyLen = strlen(key);

    outIsApplyDefault = false;
    if (keyLen>14 && strcmp(key+keyLen-14, ".apply-default")==0)
    {
        // cut off ".apply-default"
        outIsApplyDefault = true;
        tmp = std::string(key, keyLen-14);
        key = tmp.c_str();
    }

    const char *lastDotPos = strrchr(key, '.');
    const char *doubleAsterisk = !lastDotPos ? NULL : strstr(lastDotPos, "**");
    if (strcmp(key, "**")==0)
    {
        // frequent special case ("**.apply-default=true")
        outOwnerName = "**";
        outGroupName = "*";
    }
    else if (!lastDotPos || doubleAsterisk)
    {
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
        outOwnerName = ""; // empty owner means "do fullPath match"
        outGroupName = !doubleAsterisk ? "*" : doubleAsterisk+1;
    }
    else
    {
        // normal case: group is the part after the last dot
        outOwnerName.assign(key, lastDotPos - key);
        outGroupName.assign(lastDotPos+1);
    }
}

SectionBasedConfiguration::KeyValue1 SectionBasedConfiguration::convert(int sectionId, int entryId)
{
    const cConfigurationReader::KeyValue& e = ini->getEntry(sectionId, entryId);
    std::string value = substituteVariables(e.getValue(), sectionId, entryId);

    StringSet::iterator it = basedirs.find(e.getBaseDirectory());
    if (it == basedirs.end()) {
        basedirs.insert(e.getBaseDirectory());
        it = basedirs.find(e.getBaseDirectory());
    }
    const std::string *basedirRef = &(*it);
    return KeyValue1(basedirRef, e.getKey(), value.c_str());
}

int SectionBasedConfiguration::internalFindSection(const char *section) const
{
    // not very efficient (linear search), but we only invoke it a few times during activateConfig()
    for (int i=0; i<ini->getNumSections(); i++)
        if (strcmp(section, ini->getSectionName(i))==0)
            return i;
    return -1;
}

int SectionBasedConfiguration::internalGetSectionId(const char *section) const
{
    int sectionId = internalFindSection(section);
    if (sectionId == -1)
        throw cRuntimeError("no such section: %s", section);
    return sectionId;
}

int SectionBasedConfiguration::internalFindEntry(const char *section, const char *key) const
{
    return internalFindEntry(internalGetSectionId(section), key);
}

int SectionBasedConfiguration::internalFindEntry(int sectionId, const char *key) const
{
    // not very efficient (linear search), but we only invoke from activateConfig(),
    // and only once per section
    for (int i=0; i<ini->getNumEntries(sectionId); i++)
        if (strcmp(key, ini->getEntry(sectionId, i).getKey())==0)
            return i;
    return -1;
}

bool SectionBasedConfiguration::internalFindEntry(const std::vector<int>& sectionChain, const char *key, int& outSectionId, int& outEntryId) const
{
    for (int i=0; i<(int)sectionChain.size(); i++)
    {
        int sectionId = sectionChain[i];
        int entryId = internalFindEntry(sectionId, key);
        if (entryId != -1)
        {
            outSectionId = sectionId;
            outEntryId = entryId;
            return true;
        }
    }
    return false;
}

const char *SectionBasedConfiguration::internalGetValue(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue) const
{
    for (int i=0; i<(int)sectionChain.size(); i++)
    {
        int sectionId = sectionChain[i];
        int entryId = internalFindEntry(sectionId, key);
        if (entryId != -1)
            return ini->getEntry(sectionId, entryId).getValue();
    }
    return fallbackValue;
}

static int findInArray(const char *s, const char **array)
{
    for (int i=0; array[i]!=NULL; i++)
        if (!strcmp(s, array[i]))
            return i;
    return -1;
}

void SectionBasedConfiguration::validate(const char *ignorableConfigKeys) const
{
    const char *obsoleteSections[] = {
        "Parameters", "Cmdenv", "Tkenv", "OutVectors", "Partitioning", "DisplayStrings", NULL
    };
    const char *cmdenvNames[] = {
        "autoflush", "event-banner-details", "event-banners", "express-mode",
        "message-trace", "module-messages", "output-file", "performance-display",
        "runs-to-execute", "status-frequency", NULL
    };
    const char *tkenvNames[] = {
        "anim-methodcalls", "animation-enabled", "animation-msgclassnames",
        "animation-msgcolors", "animation-msgnames", "animation-speed",
        "default-run", "expressmode-autoupdate", "image-path", "methodcalls-delay",
        "next-event-markers", "penguin-mode", "plugin-path", "print-banners",
        "senddirect-arrows", "show-bubbles", "show-layouting", "slowexec-delay",
        "update-freq-express", "update-freq-fast", "use-mainwindow",
        "use-new-layouter", NULL
    };

    // warn for obsolete section names and config keys
    for (int i=0; i<ini->getNumSections(); i++)
    {
        const char *section = ini->getSectionName(i);
        if (findInArray(section, obsoleteSections) != -1)
            throw cRuntimeError("Obsolete section name [%s] found, please convert the ini file to 4.x format", section);

        int numEntries = ini->getNumEntries(i);
        for (int j=0; j<numEntries; j++)
        {
            const char *key = ini->getEntry(i, j).getKey();
            if (findInArray(key, cmdenvNames) != -1 || findInArray(key, tkenvNames) != -1)
                throw cRuntimeError("Obsolete configuration key %s= found, please convert the ini file to 4.x format", key);
        }
    }

    // check section names
    std::set<std::string> configNames;
    for (int i=0; i<ini->getNumSections(); i++)
    {
        const char *section = ini->getSectionName(i);
        const char *configName = NULL;
        if (strcmp(section, "General")==0)
            ; // OK
        else if (strncmp(section, "Config ", 7)==0)
            configName  = section+7;
        else
            throw cRuntimeError("Invalid section name [%s], should be [General] or [Config <name>]", section);
        if (configName)
        {
            if (*configName == ' ')
                throw cRuntimeError("Invalid section name [%s]: too many spaces", section);
            if (!opp_isalpha(*configName) && *configName!='_')
                throw cRuntimeError("Invalid section name [%s]: config name must begin with a letter or underscore", section);
            for (const char *s=configName; *s; s++)
                if (!opp_isalnum(*s) && strchr("-_@", *s)==NULL)
                    throw cRuntimeError("Invalid section name [%s], contains illegal character '%c'", section, *s);
            if (configNames.find(configName)!=configNames.end())
                throw cRuntimeError("Configuration name '%s' not unique", configName, section);
            configNames.insert(configName);
        }

    }

    // check keys
    for (int i=0; i<ini->getNumSections(); i++)
    {
        const char *section = ini->getSectionName(i);
        int numEntries = ini->getNumEntries(i);
        for (int j=0; j<numEntries; j++)
        {
            const char *key = ini->getEntry(i, j).getKey();
            bool containsDot = strchr(key, '.')!=NULL;

            if (!containsDot)
            {
                // warn for unrecognized (or misplaced) config keys
                // NOTE: values don't need to be validated here, that will be
                // done when the config gets actually used
                cConfigKey *e = lookupConfigKey(key);
                if (!e && isIgnorableConfigKey(ignorableConfigKeys, key))
                    continue;
                if (!e)
                    throw cRuntimeError("Unknown configuration key: %s", key);
                if (e->isPerObject())
                    throw cRuntimeError("Configuration key %s should be specified per object, try **.%s=", key, key);
                if (e->isGlobal() && strcmp(section, "General")!=0)
                    throw cRuntimeError("Configuration key %s may only occur in the [General] section", key);

                // check section hierarchy
                if (strcmp(key, "extends")==0)
                {
                    if (strcmp(section, "General")==0)
                        throw cRuntimeError("The [General] section cannot extend other sections");

                    // warn for invalid "extends" names
                    const char *value = ini->getEntry(i, j).getValue();
                    if (configNames.find(value)==configNames.end())
                        throw cRuntimeError("No such config: %s", value);

                    // check for section cycles
                    resolveSectionChain(section);  //XXX move that check here?
                }
            }
            else
            {
                // check for per-object configuration subkeys (".ev-enabled", ".record-interval", ".apply-default")
                std::string ownerName;
                std::string groupName;
                bool isApplyDefault;
                splitKey(key, ownerName, groupName, isApplyDefault);
                bool containsHyphen = strchr(groupName.c_str(), '-')!=NULL;
                if (containsHyphen)
                {
                    // this is a per-object config
                    //XXX groupName (probably) should not contain wildcard
                    // FIXME but surely not "**" !!!!
                    cConfigKey *e = lookupConfigKey(groupName.c_str());
                    if (!e && isIgnorableConfigKey(ignorableConfigKeys, groupName.c_str()))
                        continue;
                    if (!e || !e->isPerObject())
                        throw cRuntimeError("Unknown per-object configuration key `%s' in %s", groupName.c_str(), key);
                }
            }
        }
    }
}

cConfigKey *SectionBasedConfiguration::lookupConfigKey(const char *key)
{
    cConfigKey *e = (cConfigKey *) configKeys.getInstance()->lookup(key);
    if (e)
        return e;  // found it, great

    // Maybe it matches on a cConfigKey which has '*' or '%' in its name,
    // such as "seed-1-mt" matches on the "seed-%-mt" cConfigKey.
    // We have to iterate over all cConfigKeys to verify this.
    // "%" means "any number" in config keys.
    int n = configKeys.getInstance()->size();
    for (int i=0; i<n; i++)
    {
        cConfigKey *e = (cConfigKey *) configKeys.getInstance()->get(i);
        if (PatternMatcher::containsWildcards(e->getName()) || strchr(e->getName(),'%')!=NULL)
        {
            std::string pattern = opp_replacesubstring(e->getName(), "%", "{..}", true);
            if (PatternMatcher(pattern.c_str(), false, true, true).matches(key))
                return e;
        }
    }
    return NULL;
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
    std::map<std::string,KeyValue1>::const_iterator it = config.find(key);
    return it==config.end() ? NULL : it->second.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getConfigEntry(const char *key) const
{
    std::map<std::string,KeyValue1>::const_iterator it = config.find(key);
    return it==config.end() ? (KeyValue&)nullEntry : (KeyValue&)it->second;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingConfigKeys(const char *pattern) const
{
    std::vector<const char *> result;
    PatternMatcher matcher(pattern, true, true, true);

    // iterate over the map -- this is going to be sloooow...
    for (std::map<std::string,KeyValue1>::const_iterator it = config.begin(); it != config.end(); ++it)
        if (matcher.matches(it->first.c_str()))
            result.push_back(it->first.c_str());
    return result;
}

const char *SectionBasedConfiguration::getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    const SectionBasedConfiguration::KeyValue2& entry = (KeyValue2&) getParameterEntry(moduleFullPath, paramName, hasDefaultValue);
    // NULL ==> not found,   "" ==> apply the default value
    return entry.getKey()==NULL ? NULL : entry.isApplyDefault ? "" : entry.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // look up which group; paramName serves as group name
    std::map<std::string,Group>::const_iterator it = groups.find(paramName);
    const Group *group = it==groups.end() ? &wildcardGroup : &it->second;

    // find first match in the group
    bool dontApplyDefault = false;
    for (int i=0; i<(int)group->entries.size(); i++)
    {
        const KeyValue2& entry = group->entries[i];
        if (entryMatches(entry, moduleFullPath, paramName))
        {
            if (entry.isApplyDefault)
            {
                if (dontApplyDefault)
                    ; // ignore this apply-default line
                else if (entry.applyDefaultValue)
                    return entry;  // ==> "yes, apply the default value"
                else
                    dontApplyDefault = true; // ignore further .apply-default=true lines
            }
            else
                return entry;  // found value
        }
    }
    return nullEntry; // not found
}

bool SectionBasedConfiguration::entryMatches(const KeyValue2& entry, const char *moduleFullPath, const char *paramName)
{
    if (!entry.fullPathPattern)
    {
        // typical
        return entry.ownerPattern->matches(moduleFullPath) && (entry.groupPattern==NULL || entry.groupPattern->matches(paramName));
    }
    else
    {
        // less efficient, but very rare
        std::string paramFullPath = std::string(moduleFullPath) + "." + paramName;
        return entry.fullPathPattern->matches(paramFullPath.c_str());
    }
}

std::vector<const char *> SectionBasedConfiguration::getParameterKeyValuePairs() const
{
    std::vector<const char *> result;
    //FIXME TBD
    return result;
}

const char *SectionBasedConfiguration::getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const
{
    const SectionBasedConfiguration::KeyValue2& entry = (KeyValue2&) getPerObjectConfigEntry(objectFullPath, keySuffix);
    return entry.getKey()==NULL ? NULL : entry.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const
{
    // look up which group; keySuffix serves as group name
    std::map<std::string,Group>::const_iterator it = groups.find(keySuffix);
    const Group *group = it==groups.end() ? &wildcardGroup : &it->second;

    // find first match in the group
    for (int i=0; i<(int)group->entries.size(); i++)
    {
        const KeyValue2& entry = group->entries[i];
        if (!entry.isApplyDefault && entry.ownerPattern->matches(objectFullPath) && (entry.groupPattern==NULL || entry.groupPattern->matches(keySuffix)))
            return entry;  // found value
    }
    return nullEntry; // not found
}

static const char *partAfterLastDot(const char *s)
{
    const char *lastDotPos = strrchr(s, '.');
    return lastDotPos==NULL ? NULL : lastDotPos+1;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingPerObjectConfigKeys(const char *objectFullPath, const char *keySuffixPattern) const
{
    std::vector<const char *> result;

    // check all groups whose name matchs the pattern
    PatternMatcher matcher(keySuffixPattern, true, true, true); //XXX check flags
    for (std::map<std::string,Group>::const_iterator it = groups.begin(); it != groups.end(); ++it)
    {
        if (matcher.matches(it->first.c_str()))
        {
            // find all matching entries from this group.
            // We'll have a little problem where key ends in wildcard (i.e. entry.groupPattern!=NULL);
            // there we'd have to determine whether two *patterns* match. We resolve this
            // by checking whether one pattern matches the other one as string, and vica versa.
            const Group& group = it->second;
            for (int i=0; i<(int)group.entries.size(); i++)
            {
                const KeyValue2& entry = group.entries[i];
                if (!entry.isApplyDefault && entry.ownerPattern->matches(objectFullPath) && (entry.groupPattern==NULL || matcher.matches(partAfterLastDot(entry.key.c_str())) || entry.groupPattern->matches(keySuffixPattern)))
                    result.push_back(entry.key.c_str());
            }
        }
    }
    return result;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const
{
    std::vector<const char *> result = getMatchingPerObjectConfigKeys(objectFullPath, keySuffixPattern);
    for (int i=0; i<(int)result.size(); i++)
        result[i] = partAfterLastDot(result[i]);
    return result;
}

void SectionBasedConfiguration::dump() const
{
    printf("Config:\n");
    for (std::map<std::string,KeyValue1>::const_iterator it = config.begin(); it!=config.end(); it++)
        printf("  %s = %s\n", it->first.c_str(), it->second.value.c_str());

    for (std::map<std::string,Group>::const_iterator it = groups.begin(); it!=groups.end(); it++)
    {
        const std::string& groupName = it->first;
        const Group& group = it->second;
        printf("Group %s:\n", groupName.c_str());
        for (int i=0; i<(int)group.entries.size(); i++)
            printf("  %s = %s\n", group.entries[i].key.c_str(), group.entries[i].value.c_str());
    }
    printf("Wildcard Group:\n");
    for (int i=0; i<(int)wildcardGroup.entries.size(); i++)
        printf("  %s = %s\n", wildcardGroup.entries[i].key.c_str(), wildcardGroup.entries[i].value.c_str());
}


