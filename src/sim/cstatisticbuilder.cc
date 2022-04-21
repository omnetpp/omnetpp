//==========================================================================
//  CSTATISTICBUILDER.CC - part of
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

#include "omnetpp/cstatisticbuilder.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/resultfilters.h"
#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "common/opp_ctype.h"
#include "statisticsourceparser.h"
#include "statisticrecorderparser.h"

namespace omnetpp {

using namespace omnetpp::common;

const char *PROPKEY_STATISTIC_SOURCE = "source";
const char *PROPKEY_STATISTIC_RECORD = "record";
const char *PROPKEY_STATISTIC_CHECKSIGNALS = "checkSignals";
const char *PROPKEY_STATISTIC_AUTOWARMUPFILTER = "autoWarmupFilter";

Register_PerObjectConfigOption(CFGID_STATISTIC_RECORDING, "statistic-recording", KIND_STATISTIC, CFG_BOOL, "true", "Whether the matching `@statistic` should be recorded. This option lets one completely disable all recording from a @statistic. Disabling a `@statistic` this way is more efficient than specifying `**.scalar-recording=false` and `**.vector-recording=false` together.\nUsage: `<module-full-path>.<statistic-name>.statistic-recording=true/false`.\nExample: `**.ping.roundTripTime.statistic-recording=false`");
Register_PerObjectConfigOption(CFGID_RESULT_RECORDING_MODES, "result-recording-modes", KIND_STATISTIC, CFG_STRING, "default", "Defines how to calculate results from the matching `@statistic`.\nUsage: `<module-full-path>.<statistic-name>.result-recording-modes=<modes>`. Special values: `default`, `all`: they select the modes listed in the `record` key of `@statistic`; all selects all of them, default selects the non-optional ones (i.e. excludes the ones that end in a question mark). Example values: `vector`, `count`, `last`, `sum`, `mean`, `min`, `max`, `timeavg`, `stats`, `histogram`. More than one values are accepted, separated by commas. Expressions are allowed. Items prefixed with `-` get removed from the list. Example: `**.queueLength.result-recording-modes=default,-vector,+timeavg`");

typedef cStatisticBuilder::TristateBool TristateBool;

static int search_(std::vector<std::string>& v, const char *s)
{
    for (int i = 0; i < (int)v.size(); i++)
        if (strcmp(v[i].c_str(), s) == 0)
            return i;
    return -1;
}

inline void addIfNotContains_(std::vector<std::string>& v, const char *s)
{
    if (search_(v, s) == -1)
        v.push_back(s);
}

inline void addIfNotContains_(std::vector<std::string>& v, const std::string& s)
{
    if (search_(v, s.c_str()) == -1)
        v.push_back(s);
}

TristateBool cStatisticBuilder::parseTristateBool(const char *s, const char *what)
{
    if (opp_isempty(s))
        return TRISTATE_DEFAULT;
    else if (strcmp(s, "true") == 0)
        return TRISTATE_TRUE;
    else if (strcmp(s, "false") == 0)
        return TRISTATE_FALSE;
    else
        throw opp_runtime_error("Invalid value '%s' for %s, expecting 'true' or 'false'", s, what);
}

void cStatisticBuilder::addResultRecorders(cComponent *component)
{
    std::vector<const char *> statisticNames = component->getProperties()->getIndicesFor("statistic");
    std::string componentFullPath;
    for (auto statisticName : statisticNames) {
        if (componentFullPath.empty())
            componentFullPath = component->getFullPath();
        cProperty *statisticProperty = component->getProperties()->get("statistic", statisticName);
        ASSERT(statisticProperty != nullptr);
        doAddResultRecorders(component, componentFullPath, statisticName, statisticProperty, SIMSIGNAL_NULL);
    }
}

void cStatisticBuilder::addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty)
{
    std::string dummy;
    doAddResultRecorders(component, dummy, statisticName, statisticTemplateProperty, signal);
}

void cStatisticBuilder::doAddResultRecorders(cComponent *component, std::string& componentFullPath, const char *statisticName, cProperty *statisticProperty, simsignal_t signal)
{
    if (componentFullPath.empty())
        componentFullPath = component->getFullPath();
    std::string statisticFullPath = componentFullPath + "." + statisticName;

    bool enabled = config->getAsBool(statisticFullPath.c_str(), CFGID_STATISTIC_RECORDING);
    if (!enabled)
        return;

    // collect the list of result recorders
    std::string modesOption = config->getAsString(statisticFullPath.c_str(), CFGID_RESULT_RECORDING_MODES, "");
    std::vector<std::string> modes = extractRecorderList(modesOption.c_str(), statisticProperty);

    // if there are result recorders, add source filters and recorders
    if (!modes.empty()) {
        TristateBool checkSignalDecl = parseTristateBool(statisticProperty->getValue(PROPKEY_STATISTIC_CHECKSIGNALS), "checkSignals attribute");
        // determine source: use either the signal from the argument list, or the source= key in the @statistic property
        SignalSource source;
        if (signal == SIMSIGNAL_NULL) {
            const char *sourceSpec = opp_emptytodefault(statisticProperty->getValue(PROPKEY_STATISTIC_SOURCE, 0), statisticName);
            bool hasWarmupPeriod = getSimulation()->getWarmupPeriod() != SIMTIME_ZERO;
            TristateBool warmupAttr = parseTristateBool(statisticProperty->getValue(PROPKEY_STATISTIC_AUTOWARMUPFILTER), "warmup attribute");
            bool needWarmupFilter = hasWarmupPeriod && warmupAttr != TRISTATE_FALSE;
            source = doStatisticSource(component, statisticProperty, statisticName, sourceSpec, checkSignalDecl, needWarmupFilter);
        }
        else {
            source = SignalSource(component, signal);
            StatisticSourceParser::checkSignalDeclaration(component, cComponent::getSignalName(signal), checkSignalDecl);
        }

        // add result recorders
        for (auto & mode : modes)
            doResultRecorder(source, mode.c_str(), component, statisticName, statisticProperty);
    }
}

std::vector<std::string> cStatisticBuilder::extractRecorderList(const char *modesOption, cProperty *statisticProperty)
{
    // "-" means "none"
    if (!modesOption[0] || (modesOption[0] == '-' && !modesOption[1]))
        return std::vector<std::string>();

    std::vector<std::string> modes;  // the result

    // if first configured mode starts with '+' or '-', assume "default" as base
    if (modesOption[0] == '-' || modesOption[0] == '+') {
        // collect the mandatory record= items from @statistic (those not ending in '?')
        int n = statisticProperty->getNumValues(PROPKEY_STATISTIC_RECORD);
        for (int i = 0; i < n; i++) {
            const char *m = statisticProperty->getValue(PROPKEY_STATISTIC_RECORD, i);
            if (m[strlen(m)-1] != '?')
                addIfNotContains_(modes, m);
        }
    }

    // loop through all modes
    StringTokenizer tokenizer(modesOption, ",");  // XXX we should ignore commas within parens
    while (tokenizer.hasMoreTokens()) {
        const char *mode = tokenizer.nextToken();
        if (!strcmp(mode, "default")) {
            // collect the mandatory record= items from @statistic (those not ending in '?')
            int n = statisticProperty->getNumValues(PROPKEY_STATISTIC_RECORD);
            for (int i = 0; i < n; i++) {
                const char *m = statisticProperty->getValue(PROPKEY_STATISTIC_RECORD, i);
                if (m[strlen(m)-1] != '?')
                    addIfNotContains_(modes, m);
            }
        }
        else if (!strcmp(mode, "all")) {
            // collect all record= items from @statistic (strip trailing '?' if present)
            int n = statisticProperty->getNumValues(PROPKEY_STATISTIC_RECORD);
            for (int i = 0; i < n; i++) {
                const char *m = statisticProperty->getValue(PROPKEY_STATISTIC_RECORD, i);
                if (m[strlen(m)-1] != '?')
                    addIfNotContains_(modes, m);
                else
                    addIfNotContains_(modes, std::string(m, strlen(m)-1));
            }
        }
        else if (mode[0] == '-') {
            // remove from modes
            int k = search_(modes, mode+1);
            if (k != -1)
                modes.erase(modes.begin()+k);
        }
        else {
            // add to modes
            addIfNotContains_(modes, mode[0] == '+' ? mode+1 : mode);
        }
    }
    return modes;
}

SignalSource cStatisticBuilder::doStatisticSource(cComponent *component, cProperty *statisticProperty, const char *statisticName, const char *sourceSpec, TristateBool checkSignalDecl, bool needWarmupFilter)
{
    try {
        if (opp_isvalididentifier(sourceSpec)) {
            // simple case: just a signal name
            StatisticSourceParser::checkSignalDeclaration(component, sourceSpec, checkSignalDecl);
            simsignal_t signalID = cComponent::registerSignal(sourceSpec);
            if (!needWarmupFilter)
                return SignalSource(component, signalID);
            else {
                WarmupPeriodFilter *warmupFilter = new WarmupPeriodFilter();
                cResultFilter::Context ctx {component, statisticProperty};
                warmupFilter->init(&ctx);
                component->subscribe(signalID, warmupFilter);
                return SignalSource(warmupFilter);
            }
        }
        else {
            StatisticSourceParser parser;
            return parser.parse(component, statisticProperty, statisticName, sourceSpec, checkSignalDecl, needWarmupFilter);
        }
    }
    catch (std::exception& e) {
        throw cRuntimeError("Cannot add statistic '%s' to module %s (NED type: %s): Error in source=%s: %s",
                statisticName, component->getFullPath().c_str(), component->getNedTypeName(), sourceSpec, e.what());
    }
}

void cStatisticBuilder::doResultRecorder(const SignalSource& source, const char *recordingMode, cComponent *component, const char *statisticName, cProperty *attrsProperty)
{
    try {
        if (opp_isvalididentifier(recordingMode)) {
            // simple case: just a plain recorder
            //TODO if disabled, don't add
            cResultRecorder *recorder = cResultRecorderType::get(recordingMode)->create();
            cResultRecorder::Context ctx { component, statisticName, recordingMode, attrsProperty };
            recorder->init(&ctx);
            source.subscribe(recorder);
        }
        else {
            // something more complicated: use parser
            StatisticRecorderParser parser;
            parser.parse(source, recordingMode, component, statisticName, attrsProperty);
        }
    }
    catch (std::exception& e) {
        throw cRuntimeError("Cannot add statistic '%s' to module %s (NED type: %s): Bad recording mode '%s': %s",
                statisticName, component->getFullPath().c_str(), component->getNedTypeName(), recordingMode, e.what());
    }
}

}  // namespace omnetpp

