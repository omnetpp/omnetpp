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
#include "cconfigoption.h"
#include "stringtokenizer.h"
#include "stringtokenizer2.h"
#include "timeutil.h"
#include "platmisc.h"   //getpid()

NAMESPACE_BEGIN

//XXX error messages (exceptions) should contain file/line info!
//XXX make sure quoting "$\{" works!
//TODO optimize storage (now keys with wildcard suffix are stored multiple times, in several suffix groups)


Register_GlobalConfigOption(CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS, "sectionbasedconfig-configreader-class", CFG_STRING, "", "When configuration-class=SectionBasedConfiguration: selects the configuration reader C++ class, which must subclass from cConfigurationReader.");
Register_PerRunConfigOption(CFGID_DESCRIPTION, "description", CFG_STRING, NULL, "Descriptive name for the given simulation configuration. Descriptions get displayed in the run selection dialog.");
Register_PerRunConfigOption(CFGID_EXTENDS, "extends", CFG_STRING, NULL, "Name of the configuration this section is based on. Entries from that section will be inherited and can be overridden. In other words, configuration lookups will fall back to the base section.");
Register_PerRunConfigOption(CFGID_CONSTRAINT, "constraint", CFG_STRING, NULL, "For scenarios. Contains an expression that iteration variables (${} syntax) must satisfy for that simulation to run. Example: $i < $j+1.");
Register_PerRunConfigOption(CFGID_REPEAT, "repeat", CFG_INT, "1", "For scenarios. Specifies how many replications should be done with the same parameters (iteration variables). This is typically used to perform multiple runs with different random number seeds. The loop variable is available as ${repetition}. See also: seed-set= key.");
Register_PerRunConfigOption(CFGID_EXPERIMENT_LABEL, "experiment-label", CFG_STRING, "${configname}", "Identifies the simulation experiment (which consists of several, potentially repeated measurements). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigOption(CFGID_MEASUREMENT_LABEL, "measurement-label", CFG_STRING, "${iterationvars}", "Identifies the measurement within the experiment. This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigOption(CFGID_REPLICATION_LABEL, "replication-label", CFG_STRING, "#${repetition}", "Identifies one replication of a measurement (see repeat= and measurement-label= as well). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigOption(CFGID_RUNNUMBER_WIDTH, "runnumber-width", CFG_INT, "0", "Setting a nonzero value will cause the $runnumber variable to get padded with leading zeroes to the given length.");

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

SectionBasedConfiguration::KeyValue2::KeyValue2(const KeyValue2& e) : KeyValue1(e)
{
    // apparently only used for std::vector storage
    ownerPattern = e.ownerPattern ? new PatternMatcher(*e.ownerPattern) : NULL;
    suffixPattern = e.suffixPattern ? new PatternMatcher(*e.suffixPattern) : NULL;
    fullPathPattern = e.fullPathPattern ? new PatternMatcher(*e.fullPathPattern) : NULL;
}

SectionBasedConfiguration::KeyValue2::~KeyValue2()
{
    delete ownerPattern;
    delete suffixPattern;
    delete fullPathPattern;
}

//----

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
    sectionChains.assign(ini->getNumSections(), std::vector<int>());
}

void SectionBasedConfiguration::setCommandLineConfigOptions(const std::map<std::string,std::string>& options, const char *baseDir)
{
    commandLineOptions.clear();
    const std::string *basedirRef = getPooledBaseDir(baseDir);
    for (StringMap::const_iterator it=options.begin(); it!=options.end(); it++)
    {
        // validate the key, then store the option
        //XXX we should better use the code in the validate() method...
        const char *key = it->first.c_str();
        const char *value = it->second.c_str();
        const char *option = strchr(key,'.') ? strrchr(key,'.')+1 : key; // check only the part after the last dot, i.e. recognize per-object keys as well
        cConfigOption *e = lookupConfigOption(option);
        if (!e)
            throw cRuntimeError("Unknown command-line configuration option --%s", key);
        if (!e->isPerObject() && key!=option)
            throw cRuntimeError("Wrong command-line configuration option --%s: %s is not a per-object option", key, e->getName());
        std::string tmp;
        if (e->isPerObject() && key==option)
            key = (tmp=std::string("**.")+key).c_str(); // prepend with "**." (XXX this should be done in inifile contents too)
        if (!value[0])
            throw cRuntimeError("Missing value for command-line configuration option --%s", key);
        commandLineOptions.push_back(KeyValue1(basedirRef, key, value));
    }
}

void SectionBasedConfiguration::clear()
{
    // note: this gets called between activateConfig() calls, so "ini" must NOT be NULL'ed out here
    activeConfig = "";
    activeRunNumber = 0;
    config.clear();
    suffixGroups.clear();
    wildcardSuffixGroup.entries.clear();
    variables.clear();
}

void SectionBasedConfiguration::initializeFrom(cConfiguration *bootConfig)
{
    std::string classname = bootConfig->getAsString(CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS);
    if (classname.empty())
        throw cRuntimeError("SectionBasedConfiguration: no configuration reader class specified (missing %s option)",
                             CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS->getName());
    cConfigurationReader *reader = dynamic_cast<cConfigurationReader *>(createOne(classname.c_str()));
    if (!reader)
        throw cRuntimeError("Class \"%s\" is not subclassed from cConfigurationReader", classname.c_str());
    reader->initializeFrom(bootConfig);
    setConfigurationReader(reader);
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
    std::string extends = entryId==-1 ? "" : ini->getEntry(sectionId, entryId).getValue();
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
    std::string extends = entryId==-1 ? "" : ini->getEntry(sectionId, entryId).getValue();

    std::vector<int> result;
    if (sectionId == generalSectionId && generalSectionId != -1)
    {
        // 'General' can not have base config
    }
    else if (extends.empty())
    {
        // implicitly inherit form 'General'
        if (generalSectionId != -1)
            result.push_back(generalSectionId);
    }
    else
    {
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
    for (std::vector<int>::iterator it = sectionIds.begin(); it != sectionIds.end(); ++it)
    {
        const char *section = ini->getSectionName(*it);
        if (strcmp(section, "General")==0)
            result.push_back(section);
        else if (strncmp(section, "Config ", 7)==0)
            result.push_back(section+7);
        else
            ; // nothing - leave out bogus section names
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

    // determine the list of sections, from this one up to [General]
    std::vector<int> sectionChain = resolveSectionChain(configName);

    // extract all iteration vars from values within this section
    std::vector<IterationVariable> itervars = collectIterationVariables(sectionChain);

    // see if there's a constraint given
    const char *constraint = internalGetValue(sectionChain, CFGID_CONSTRAINT->getName(), NULL);

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
        KeyValue1& e = commandLineOptions[i];
        std::string value = substituteVariables(e.getValue());
        const std::string *basedirRef = getPooledBaseDir(e.getBaseDirectory());
        addEntry(KeyValue1(basedirRef, e.getKey(), value.c_str()));
    }
    for (int i=0; i < (int)sectionChain.size(); i++)
    {
        int sectionId = sectionChain[i];
        for (int entryId=0; entryId<ini->getNumEntries(sectionId); entryId++)
        {
            // add entry to our tables
            const cConfigurationReader::KeyValue& e = ini->getEntry(sectionId, entryId);
            std::string value = substituteVariables(e.getValue(), sectionId, entryId);
            const std::string *basedirRef = getPooledBaseDir(e.getBaseDirectory());
            addEntry(KeyValue1(basedirRef, e.getKey(), value.c_str()));
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
    variables[CFGVAR_SEEDSET] = resolveConfigOption(CFGID_SEED_SET, sectionChain);
    variables[CFGVAR_EXPERIMENT] = resolveConfigOption(CFGID_EXPERIMENT_LABEL, sectionChain);
    variables[CFGVAR_MEASUREMENT] = resolveConfigOption(CFGID_MEASUREMENT_LABEL, sectionChain);
    variables[CFGVAR_REPLICATION] = resolveConfigOption(CFGID_REPLICATION_LABEL, sectionChain);
}

std::string SectionBasedConfiguration::resolveConfigOption(cConfigOption *option, const std::vector<int>& sectionChain) const
{
    int sectionId, entryId;
    const char *value = internalGetValue(sectionChain, option->getName(), option->getDefaultValue(), &sectionId, &entryId);
    return substituteVariables(value, sectionId, entryId);
}

int SectionBasedConfiguration::getNumRunsInConfig(const char *configName) const
{
    // extract all iteration vars from values within this config
    std::vector<int> sectionChain = resolveSectionChain(configName);
    std::vector<IterationVariable> v = collectIterationVariables(sectionChain);

    // see if there's a constraint given
    const char *constraint = internalGetValue(sectionChain, CFGID_CONSTRAINT->getName(), NULL);

    // count the runs and return the result
    try {
        return Scenario(v, constraint).getNumRuns();
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error while computing the number of runs in config %s: %s", configName, e.what());
    }
}

std::vector<std::string> SectionBasedConfiguration::unrollConfig(const char *configName, bool detailed) const
{
    // extract all iteration vars from values within this section
    std::vector<int> sectionChain = resolveSectionChain(configName);
    std::vector<IterationVariable> itervars = collectIterationVariables(sectionChain);

    // see if there's a constraint given
    const char *constraint = internalGetValue(sectionChain, CFGID_CONSTRAINT->getName(), NULL);

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

                if (!loc.value.empty())
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
    const char *repeat = internalGetValue(sectionChain, CFGID_REPEAT->getName());
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

    StringTokenizer2 tokenizer(pos+1, "}", "{}", "\"");
    const char *token = tokenizer.nextToken(); // ends at matching '}'
    if (!token)
        throw cRuntimeError("missing '}' for '${'");
    outEndPos = pos + 1 + tokenizer.getTokenLength();

    // parse what's inside the ${...}
    const char *varbegin = NULL;
    const char *varend = NULL;
    const char *valuebegin = NULL;
    const char *valueend = NULL;
    const char *parvarbegin = NULL;
    const char *parvarend = NULL;

    const char *s = pos+2;
    while (opp_isspace(*s)) s++;
    if (opp_isalphaext(*s))
    {
        // must be a variable or a variable reference
        varbegin = varend = s;
        while (opp_isalnumext(*varend)) varend++;
        s = varend;
        while (opp_isspace(*s)) s++;
        if (*s=='}') {
            // ${x} syntax -- OK
        }
        else if (*s=='=' && *(s+1)!='=') {
            // ${x=...} syntax -- OK
            valuebegin = s+1;
        }
        else if (strchr(s,',')) {
            // part of a valuelist -- OK
            valuebegin = varbegin;
            varbegin = varend = NULL;
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
            if (opp_isalphaext(*s))
            {
                parvarbegin = s;
                while (opp_isalnumext(*s)) s++;
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

        // handle named and unnamed iteration variable references
        std::string varid = !varname.empty() ? varname : opp_stringf("%d-%d-%d", sectionId, entryId, k);
        StringMap::const_iterator it = variables.find(varid.c_str());
        if (it==variables.end())
            throw cRuntimeError("no such variable: ${%s}", varid.c_str());
        value = it->second;

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
        if (opp_isalphaext(it->first[0]) && !isPredefinedVariable(it->first.c_str()))  // skip unnamed and predefined ones
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

typedef std::vector<int> SectionChain;
typedef std::vector<SectionChain> SectionChainList;

std::vector<int> SectionBasedConfiguration::resolveSectionChain(int sectionId) const
{
    if (sectionId == -1)
        return std::vector<int>(); // assume implicit [General] section
    if (sectionChains.at(sectionId).empty())
        sectionChains[sectionId] = computeSectionChain(sectionId);
    return sectionChains[sectionId];
}

std::vector<int> SectionBasedConfiguration::resolveSectionChain(const char *configName) const
{
    int sectionId = resolveConfigName(configName);
    if (sectionId == -1 && strcmp(configName, "General") != 0)  // allow implicit [General] section
        throw cRuntimeError("No such config: %s", configName);
    return resolveSectionChain(sectionId);
}

static bool mergeSectionChains(SectionChainList &remainingInputs, std::vector<int> &result);

/*
 * Computes the linearization of given section and all of its base sections.
 * The result is the merge of the linearization of base classes and base classes itself.
 */
std::vector<int> SectionBasedConfiguration::computeSectionChain(int sectionId) const
{
    std::vector<int> baseConfigs = getBaseConfigIds(sectionId);
    SectionChainList chainsToBeMerged;
    for (std::vector<int>::const_iterator it = baseConfigs.begin(); it != baseConfigs.end(); ++it)
    {
        SectionChain chain = resolveSectionChain(*it);
        chainsToBeMerged.push_back(chain);
    }
    chainsToBeMerged.push_back(baseConfigs);

    std::vector<int> result;
    result.push_back(sectionId);
    if (mergeSectionChains(chainsToBeMerged, result))
        return result;
    else
        throw cRuntimeError("Detected section fallback chain inconsistency at: [%s]", ini->getSectionName(sectionId)); // TODO more details?
}

static int selectNext(const SectionChainList &remainingInputs);

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
static bool mergeSectionChains(SectionChainList &sectionChains, std::vector<int> &result)
{
    SectionChainList::iterator begin = sectionChains.begin();
    SectionChainList::iterator end = sectionChains.end();

    // reverse the chains, so they are accessed at the end instead of the begin (more efficient to remove from the end of a vector)
    bool allChainsAreEmpty = true;
    for (SectionChainList::iterator chain = begin; chain != end; ++chain)
    {
        if (!chain->empty())
        {
            reverse(chain->begin(), chain->end());
            allChainsAreEmpty = false;
        }
    }

    while (!allChainsAreEmpty)
    {
        // select next
        int nextId = selectNext(sectionChains);
        if (nextId == -1)
            break;
        // add it to the output and remove from the inputs
        result.push_back(nextId);
        allChainsAreEmpty = true;
        for (SectionChainList::iterator chain = begin; chain != end; ++chain)
        {
            if (!chain->empty())
            {
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
static int selectNext(const SectionChainList &sectionChains)
{
    SectionChainList::const_iterator begin = sectionChains.begin();
    SectionChainList::const_iterator end = sectionChains.end();

    for (SectionChainList::const_iterator chain = begin; chain != end; ++chain)
    {
        if (!chain->empty())
        {
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
            throw cRuntimeError("Cycle detected in section fallback chain at: [%s]", ini->getSectionName(sectionId));
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

void SectionBasedConfiguration::addEntry(const KeyValue1& entry)
{
    const std::string& key = entry.key;
    const char *lastDot = strrchr(key.c_str(), '.');
    if (!lastDot && !PatternMatcher::containsWildcards(key.c_str()))
    {
        // config: add if not already in there
        if (config.find(key)==config.end())
            config[key] = entry;
    }
    else
    {
        // key contains wildcard or dot: parameter or per-object configuration
        // (example: "**", "**.param", "**.partition-id")
        // Note: since the last part of they key might contain wildcards, it is not really possible
        // to distinguish the two. Cf "vector-recording", "vector-*" and "vector*"

        // analyze key and create appropriate entry
        std::string ownerName;
        std::string suffix;
        splitKey(key.c_str(), ownerName, suffix);
        bool suffixContainsWildcards = PatternMatcher::containsWildcards(suffix.c_str());

        KeyValue2 entry2(entry);
        if (!ownerName.empty())
            entry2.ownerPattern = new PatternMatcher(ownerName.c_str(), true, true, true);
         else
            entry2.fullPathPattern = new PatternMatcher(key.c_str(), true, true, true);
        entry2.suffixPattern = suffixContainsWildcards ? new PatternMatcher(suffix.c_str(), true, true, true) : NULL;

        // find which group it should go into
        if (!suffixContainsWildcards)
        {
            // no wildcard in suffix (=group name)
            if (suffixGroups.find(suffix)==suffixGroups.end()) {
                // suffix group not yet exists, create it
                SuffixGroup& group = suffixGroups[suffix];

                // initialize group with matching wildcard keys seen so far
                for (int k=0; k<(int)wildcardSuffixGroup.entries.size(); k++)
                    if (wildcardSuffixGroup.entries[k].suffixPattern->matches(suffix.c_str()))
                        group.entries.push_back(wildcardSuffixGroup.entries[k]);
            }
            suffixGroups[suffix].entries.push_back(entry2);
        }
        else
        {
            // suffix contains wildcards: we need to add it to all existing suffix groups it matches
            // Note: if suffix also contains a hyphen, that's actually illegal (per-object
            // config entry names cannot be wildcarded, ie. "foo.bar.cmdenv-*" is illegal),
            // but causes no harm, because getPerObjectConfigEntry() won't look into the
            // wildcard group
            wildcardSuffixGroup.entries.push_back(entry2);
            for (std::map<std::string,SuffixGroup>::iterator it = suffixGroups.begin(); it!=suffixGroups.end(); it++)
                if (entry2.suffixPattern->matches(it->first.c_str()))
                    (it->second).entries.push_back(entry2);
        }
    }
}

void SectionBasedConfiguration::splitKey(const char *key, std::string& outOwnerName, std::string& outGroupName)
{
    std::string tmp = key;

    const char *lastDotPos = strrchr(key, '.');
    const char *doubleAsterisk = !lastDotPos ? NULL : strstr(lastDotPos, "**");

    if (!lastDotPos || doubleAsterisk)
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

int SectionBasedConfiguration::internalFindEntry(int sectionId, const char *key) const
{
    // not very efficient (linear search), but we only invoke from activateConfig(),
    // and only once per section
    for (int i=0; i<ini->getNumEntries(sectionId); i++)
        if (strcmp(key, ini->getEntry(sectionId, i).getKey())==0)
            return i;
    return -1;
}

const char *SectionBasedConfiguration::internalGetValue(const std::vector<int>& sectionChain, const char *key, const char *fallbackValue, int *outSectionIdPtr, int *outEntryIdPtr) const
{
    if (outSectionIdPtr) *outSectionIdPtr = -1;
    if (outEntryIdPtr) *outEntryIdPtr = -1;

    for (int i=0; i<(int)commandLineOptions.size(); i++)
        if (strcmp(key, commandLineOptions[i].getKey())==0)
            return commandLineOptions[i].getValue();

    for (int i=0; i<(int)sectionChain.size(); i++)
    {
        int sectionId = sectionChain[i];
        int entryId = internalFindEntry(sectionId, key);
        if (entryId != -1) {
            if (outSectionIdPtr) *outSectionIdPtr = sectionId;
            if (outEntryIdPtr) *outEntryIdPtr = entryId;
            return ini->getEntry(sectionId, entryId).getValue();
        }
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

static bool visit(SectionGraph &graph, SectionGraphNode& node);

static int findCycle(SectionGraph &graph)
{
    for (SectionGraph::iterator node = graph.begin(); node != graph.end(); ++node)
        if (node->color == WHITE && visit(graph, *node))
            return node->id;
    return -1;
}

static bool visit(SectionGraph &graph, SectionGraphNode& node)
{
    node.color = GREY;
    for (std::vector<int>::iterator nodeId = node.nextNodes.begin(); nodeId != node.nextNodes.end(); ++nodeId)
    {
        SectionGraphNode &node2 = graph[*nodeId];
        if (node2.color == GREY || (node2.color == WHITE && visit(graph, node2)))
            return true;
    }
    node.color = BLACK;
    return false;
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
                throw cRuntimeError("Obsolete configuration option %s= found, please convert the ini file to 4.x format", key);
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
            if (!opp_isalphaext(*configName) && *configName!='_')
                throw cRuntimeError("Invalid section name [%s]: config name must begin with a letter or underscore", section);
            for (const char *s=configName; *s; s++)
                if (!opp_isalnumext(*s) && strchr("-_@", *s)==NULL)
                    throw cRuntimeError("Invalid section name [%s], contains illegal character '%c'", section, *s);
            if (configNames.find(configName)!=configNames.end())
                throw cRuntimeError("Configuration name '%s' not unique", configName);
            configNames.insert(configName);
        }

    }

    // check keys, build inheritance graph
    SectionGraph graph;
    for (int i=0; i<ini->getNumSections(); i++)
    {
        graph.push_back(SectionGraphNode(i));

        const char *section = ini->getSectionName(i);
        int numEntries = ini->getNumEntries(i);
        for (int j=0; j<numEntries; j++)
        {
            const char *key = ini->getEntry(i, j).getKey();
            bool containsDot = strchr(key, '.')!=NULL;

            if (!containsDot && !PatternMatcher::containsWildcards(key))
            {
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
                if (e->isGlobal() && strcmp(section, "General")!=0)
                    throw cRuntimeError("Configuration option %s may only occur in the [General] section", key);

                // check section hierarchy
                if (strcmp(key, CFGID_EXTENDS->getName())==0)
                {
                    if (strcmp(section, "General")==0)
                        throw cRuntimeError("The [General] section cannot extend other sections");

                    // warn for invalid "extends" names
                    const char *value = ini->getEntry(i, j).getValue();
                    StringTokenizer tokenizer(value, ", \t");
                    while (tokenizer.hasMoreTokens())
                    {
                        const char *configName = tokenizer.nextToken();
                        std::string sectionName = std::string("Config ") + configName;
                        int configId = internalFindSection(sectionName.c_str());
                        if (strcmp(configName,"General")!=0 && configId == -1)
                            throw cRuntimeError("No such config: %s", configName);
                        if (configId != -1)
                            graph.back().nextNodes.push_back(configId);
                    }
                }
            }
            else
            {
                // check for per-object configuration subkeys (".ev-enabled", ".record-interval")
                std::string ownerName;
                std::string suffix;
                splitKey(key, ownerName, suffix);
                bool containsHyphen = strchr(suffix.c_str(), '-')!=NULL;
                if (containsHyphen)
                {
                    // this is a per-object config
                    //XXX suffix (probably) should not contain wildcard; but surely not "**" !!!!
                    cConfigOption *e = lookupConfigOption(suffix.c_str());
                    if (!e && isIgnorableConfigKey(ignorableConfigKeys, suffix.c_str()))
                        continue;
                    if (!e || !e->isPerObject()) {
                        if (suffix == "type-name")
                            throw cRuntimeError("Configuration option `type-name' has been renamed to `typename', please update key `%s' in the ini file", key);
                        throw cRuntimeError("Unknown per-object configuration option `%s' in %s", suffix.c_str(), key);
                    }
                }
            }
        }
    }

    // check circularity in inheritance graph
    int sectionId = findCycle(graph);
    if (sectionId != -1)
        throw cRuntimeError("Cycle detected in section fallback chain at: [%s]", ini->getSectionName(sectionId));

    // check for inconsistent hierarchy
    for (int i=0; i<ini->getNumSections(); i++)
        resolveSectionChain(i);
}

cConfigOption *SectionBasedConfiguration::lookupConfigOption(const char *key)
{
    cConfigOption *e = (cConfigOption *) configOptions.getInstance()->lookup(key);
    if (e)
        return e;  // found it, great

    // Maybe it matches on a cConfigOption which has '*' or '%' in its name,
    // such as "seed-1-mt" matches on the "seed-%-mt" cConfigOption.
    // We have to iterate over all cConfigOptions to verify this.
    // "%" means "any number" in config keys.
    int n = configOptions.getInstance()->size();
    for (int i=0; i<n; i++)
    {
        cConfigOption *e = (cConfigOption *) configOptions.getInstance()->get(i);
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
    return entry.getKey()==NULL ? NULL : entry.value.c_str();
}

const cConfiguration::KeyValue& SectionBasedConfiguration::getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const
{
    // look up which group; paramName serves as suffix (ie. group name)
    std::map<std::string,SuffixGroup>::const_iterator it = suffixGroups.find(paramName);
    const SuffixGroup *group = it==suffixGroups.end() ? &wildcardSuffixGroup : &it->second;

    // find first match in the group
    for (int i=0; i<(int)group->entries.size(); i++)
    {
        const KeyValue2& entry = group->entries[i];
        if (entryMatches(entry, moduleFullPath, paramName))
            if (hasDefaultValue || entry.value != "default")
                return entry;
    }
    return nullEntry; // not found
}

bool SectionBasedConfiguration::entryMatches(const KeyValue2& entry, const char *moduleFullPath, const char *paramName)
{
    if (!entry.fullPathPattern)
    {
        // typical
        return entry.ownerPattern->matches(moduleFullPath) && (entry.suffixPattern==NULL || entry.suffixPattern->matches(paramName));
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
    // Note: we do not accept wildcards in the config key's name (ie. "**.record-*" is invalid),
    // so we ignore the wildcard group.
    std::map<std::string,SuffixGroup>::const_iterator it = suffixGroups.find(keySuffix);
    if (it==suffixGroups.end())
        return nullEntry; // no such group

    const SuffixGroup *suffixGroup = &it->second;

    // find first match in the group
    for (int i=0; i<(int)suffixGroup->entries.size(); i++)
    {
        const KeyValue2& entry = suffixGroup->entries[i];
        if (entryMatches(entry, objectFullPath, keySuffix))
            return entry;  // found value
    }
    return nullEntry; // not found
}

static const char *partAfterLastDot(const char *s)
{
    const char *lastDotPos = strrchr(s, '.');
    return lastDotPos==NULL ? NULL : lastDotPos+1;
}

std::vector<const char *> SectionBasedConfiguration::getMatchingPerObjectConfigKeys(const char *objectFullPathPattern, const char *keySuffixPattern) const
{
    std::vector<const char *> result;

    // only concrete objects or "**" is accepted, because we are not prepared
    // to handle the "pattern matches pattern" case (see below as well).
    bool anyObject = strcmp(objectFullPathPattern, "**")==0;
    if (!anyObject && PatternMatcher::containsWildcards(objectFullPathPattern))
        throw cRuntimeError("getMatchingPerObjectConfigKeys: invalid objectFullPath parameter: the only wildcard pattern accepted is '**'");

    // check all suffix groups whose name matches the pattern
    PatternMatcher suffixMatcher(keySuffixPattern, true, true, true);
    for (std::map<std::string,SuffixGroup>::const_iterator it = suffixGroups.begin(); it != suffixGroups.end(); ++it)
    {
        const char *suffix = it->first.c_str();
        if (suffixMatcher.matches(suffix))
        {
            // find all matching entries from this suffix group.
            // We'll have a little problem where key ends in wildcard (i.e. entry.suffixPattern!=NULL);
            // there we'd have to determine whether two *patterns* match. We resolve this
            // by checking whether one pattern matches the other one as string, and vica versa.
            const SuffixGroup& group = it->second;
            for (int i=0; i<(int)group.entries.size(); i++)
            {
                const KeyValue2& entry = group.entries[i];
                if ((anyObject || entry.ownerPattern->matches(objectFullPathPattern))
                    &&
                    (entry.suffixPattern==NULL ||
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

    for (std::map<std::string,SuffixGroup>::const_iterator it = suffixGroups.begin(); it!=suffixGroups.end(); it++)
    {
        const std::string& suffix = it->first;
        const SuffixGroup& group = it->second;
        printf("Suffix Group %s:\n", suffix.c_str());
        for (int i=0; i<(int)group.entries.size(); i++)
            printf("  %s = %s\n", group.entries[i].key.c_str(), group.entries[i].value.c_str());
    }
    printf("Wildcard Suffix Group:\n");
    for (int i=0; i<(int)wildcardSuffixGroup.entries.size(); i++)
        printf("  %s = %s\n", wildcardSuffixGroup.entries[i].key.c_str(), wildcardSuffixGroup.entries[i].value.c_str());
}

NAMESPACE_END

