//==========================================================================
//  ENVIRUTILS.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "common/opp_ctype.h"
#include "common/sqliteresultfileschema.h"
#include "nedxml/nedparser.h"  // NedParser::getBuiltInDeclarations()
#include "omnetpp/ccanvas.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/resultfilters.h"  // ExpressionFilter
#include "omnetpp/resultrecorders.h"  // ExpressionRecorder
#include "sim/expressionfilter.h"
#include "args.h"
#include "appreg.h"
#include "envirutils.h"

using namespace omnetpp::common;
using namespace omnetpp::nedxml;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

using std::ostream;

std::string EnvirUtils::getConfigOptionType(cConfigOption *option)
{
    if (option->isGlobal())
        return "Global setting (applies to all simulation runs)";
    else if (!option->isPerObject())
        return "Per-simulation-run setting";
    else {
        const char *typeComment = "";
        switch (option->getObjectKind()) {
            case cConfigOption::KIND_COMPONENT: typeComment = "modules and channels"; break;
            case cConfigOption::KIND_CHANNEL: typeComment = "channels"; break;
            case cConfigOption::KIND_MODULE: typeComment = "modules"; break;
            case cConfigOption::KIND_SIMPLE_MODULE: typeComment = "simple modules"; break;
            case cConfigOption::KIND_UNSPECIFIED_TYPE: typeComment = "modules and channels"; break; // note: this is currently only used for **.typename
            case cConfigOption::KIND_PARAMETER: typeComment = "module/channel parameters"; break;
            case cConfigOption::KIND_STATISTIC: typeComment = "statistics (@statistic)"; break;
            case cConfigOption::KIND_SCALAR: typeComment = "scalar results"; break;
            case cConfigOption::KIND_VECTOR: typeComment = "vector results"; break;
            case cConfigOption::KIND_OTHER: typeComment = "other objects"; break;
            case cConfigOption::KIND_NONE: ASSERT(false); break;
        }
        return std::string("Per-object setting for ") + typeComment;
    }

}

void EnvirUtils::dumpComponentList(std::ostream& out, const char *category, bool verbose)
{
    cConfigurationEx *config = getEnvir()->getConfigEx();
    bool wantAll = !strcmp(category, "all");
    bool processed = false;
    out << "\n";
    if (wantAll || !strcmp(category, "config") || !strcmp(category, "configdetails")) {
        processed = true;
        if (verbose)
            out << "Supported configuration options:\n";
        bool printDescriptions = strcmp(category, "configdetails") == 0;

        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *obj = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(obj);
            if (!printDescriptions)
                out << "  ";
            if (obj->isPerObject())
                out << "**.";
            out << obj->getName() << "=";
            out << "<" << cConfigOption::getTypeName(obj->getType()) << ">";
            if (obj->getUnit())
                out << ", unit=\"" << obj->getUnit() << "\"";
            if (obj->getDefaultValue())
                out << ", default:" << obj->getDefaultValue() << "";
            if (!printDescriptions)
                out << "; " << (obj->isGlobal() ? "global" : obj->isPerObject() ? "per-object" : "per-run") << " setting"; // TODO getOptionKindName()

            out << "\n";
            if (printDescriptions)
                out << "    " << getConfigOptionType(obj) << ".\n";
            if (printDescriptions && !opp_isempty(obj->getDescription()))
                out << opp_indentlines(opp_breaklines(obj->getDescription(), 75), "    ") << "\n";
            if (printDescriptions)
                out << "\n";
        }
        if (!wantAll && verbose)
            out << "Use '-h configvars' to print the list of dollar variables that can be used in configuration values.\n";
        out << "\n";
    }
    if (wantAll || !strcmp(category, "configvars")) {
        processed = true;
        if (verbose)
            out << "Predefined variables that can be used in config values:\n";
        std::vector<const char *> varNames = config->getPredefinedVariableNames();
        for (const char *varName : varNames) {
            out << "${" << varName << "}\n";
            const char *desc = config->getVariableDescription(varName);
            out << opp_indentlines(opp_breaklines(desc, 75), "    ") << "\n";
        }
        out << "\n";
    }
    if (!strcmp(category, "latexconfig")) {  // internal undocumented option, for maintenance purposes
        // generate LaTeX code for the appendix in the User Manual
        processed = true;
        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        out << "\\begin{description}\n";
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *obj = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(obj);
            out << "\\item[" << (obj->isPerObject() ? "**." : "") << opp_latexquote(obj->getName()) << "] = ";
            out << "\\textit{<" << cConfigOption::getTypeName(obj->getType()) << ">}";
            if (obj->getUnit())
                out << ", unit=\\ttt{" << obj->getUnit() << "}";
            if (obj->getDefaultValue())
                out << ", default: \\ttt{" << opp_latex_insert_breaks(opp_latexquote(obj->getDefaultValue())) << "}";
            out << "\\\\\n";
            out << "    \\textit{" << getConfigOptionType(obj) << ".}\\\\\n";
            out << opp_indentlines(opp_breaklines(opp_markup2latex(opp_latexquote(obj->getDescription())), 75), "    ") << "\n";
        }
        out << "\\end{description}\n\n";
    }
    if (!strcmp(category, "latexconfigvars")) {  // internal undocumented option, for maintenance purposes
        processed = true;
        std::vector<const char *> varNames = config->getPredefinedVariableNames();
        out << "\\begin{description}\n";
        for (auto & varName : varNames) {
            out << "\\item[\\$\\{" << varName << "\\}] : \\\\\n";
            const char *desc = config->getVariableDescription(varName);
            out << opp_indentlines(opp_breaklines(opp_markup2latex(opp_latexquote(desc)), 75), "    ") << "\n";
        }
        out << "\\end{description}\n\n";
    }
    if (!strcmp(category, "jconfig")) {  // internal undocumented option, for maintenance purposes
        // generate Java code for ConfigurationRegistry.java in the IDE
        processed = true;
        if (verbose)
            out << "Supported configuration options (as Java code):\n";
        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *key = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(key);

            std::string id = "CFGID_";
            for (const char *s = key->getName(); *s; s++)
                id.append(1, opp_isalpha(*s) ? opp_toupper(*s) : *s == '-' ? '_' : *s == '%' ? 'n' : *s);
            const char *method = key->isGlobal() ? "addGlobalOption" :
                                 !key->isPerObject() ? "addPerRunOption" :
                                 "addPerObjectOption";
            #define CASE(X)  case cConfigOption::X: typestring = #X; break;
            const char *typestring;
            switch (key->getType()) {
                CASE(CFG_BOOL)
                CASE(CFG_INT)
                CASE(CFG_DOUBLE)
                CASE(CFG_STRING)
                CASE(CFG_FILENAME)
                CASE(CFG_FILENAMES)
                CASE(CFG_PATH)
                CASE(CFG_CUSTOM)
            }
            #undef CASE

            #define CASE(X)  case cConfigOption::X: kindstring = #X; break;
            const char *kindstring;
            switch (key->getObjectKind()) {
                CASE(KIND_COMPONENT)
                CASE(KIND_CHANNEL)
                CASE(KIND_MODULE)
                CASE(KIND_SIMPLE_MODULE)
                CASE(KIND_UNSPECIFIED_TYPE)
                CASE(KIND_PARAMETER)
                CASE(KIND_STATISTIC)
                CASE(KIND_SCALAR)
                CASE(KIND_VECTOR)
                CASE(KIND_NONE)
                CASE(KIND_OTHER)
            }
            #undef CASE

            out << "    public static final ConfigOption " << id << " = ";
            out << method << (key->getUnit() ? "U" : "") << "(\n";
            out << "        \"" << key->getName() << "\", ";
            if (key->isPerObject())
                out << kindstring << ", ";
            if (!key->getUnit())
                out << typestring << ", ";
            else
                out << "\"" << key->getUnit() << "\", ";
            if (!key->getDefaultValue())
                out << "null";
            else
                out << "\"" << opp_replacesubstring(key->getDefaultValue(), "\"", "\\\"", true) << "\"";
            out << ",\n";

            std::string desc = key->getDescription();
            desc = opp_replacesubstring(desc, "\n", "\\n\n", true);  // keep explicit line breaks
            desc = opp_breaklines(desc, 75);  // break long lines
            desc = opp_replacesubstring(desc, "\"", "\\\"", true);
            desc = opp_replacesubstring(desc, "\n", " \" +\n\"", true);
            desc = opp_replacesubstring(desc, "\\n \"", "\\n\"", true);  // remove bogus space after explicit line breaks
            desc = "\"" + desc + "\"";

            out << opp_indentlines(desc, "        ") << ");\n";
        }
        out << "\n";

        std::vector<const char *> varNames = config->getPredefinedVariableNames();
        for (const char *varName : varNames) {
            const char *desc = config->getVariableDescription(varName);
            out << "    public static final String CFGVAR_" << opp_strupper(varName) << " = addConfigVariable(";
            out << "\"" << varName << "\", \"" << opp_replacesubstring(desc, "\"", "\\\"", true) << "\");\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "classes")) {
        processed = true;
        if (verbose)
            out << "Registered C++ classes, including modules, channels and messages:\n";
        cRegistrationList *table = classes.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  class " << obj->getFullName() << "\n";
        }
        if (verbose) {
            out << "Note: if your class is not listed, it needs to be registered in the\n";
            out << "C++ code using Define_Module(), Define_Channel() or Register_Class().\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "classdesc")) {
        processed = true;
        if (verbose)
            out << "Classes that have associated reflection information (needed for e.g. Qtenv inspectors):\n";
        cRegistrationList *table = classDescriptors.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  class " << obj->getFullName() << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "nedfunctions")) {
        processed = true;
        if (verbose)
            out << "Functions that can be used in NED expressions and in omnetpp.ini:\n";
        cRegistrationList *table = nedFunctions.getInstance();
        table->sort();
        std::set<std::string> categories;
        for (int i = 0; i < table->size(); i++) {
            cNedFunction *nf = dynamic_cast<cNedFunction *>(table->get(i));
            cNedMathFunction *mf = dynamic_cast<cNedMathFunction *>(table->get(i));
            categories.insert(nf ? nf->getCategory() : mf ? mf->getCategory() : "<uncategorized>");
        }
        bool printDescriptions = true; // for now
        for (auto category : categories) {
            out << "\n Category \"" << category << "\":\n";
            for (int i = 0; i < table->size(); i++) {
                cObject *obj = table->get(i);
                cNedFunction *nf = dynamic_cast<cNedFunction *>(table->get(i));
                cNedMathFunction *mf = dynamic_cast<cNedMathFunction *>(table->get(i));
                const char *fcat = nf ? nf->getCategory() : mf ? mf->getCategory() : "<uncategorized>";
                const char *description = nf ? nf->getDescription() : mf ? mf->getDescription() : "<no description>";
                if (fcat == category) {
                    out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
                    if (printDescriptions && !opp_isempty(description))
                        out << opp_indentlines(opp_breaklines(description, 75), "    ") << "\n";
                    if (printDescriptions)
                        out << "\n";
                }
            }
        }
        out << "\n Category \"units/conversion\":\n";
        std::stringstream tmp;
        tmp << "All measurement unit names can be used as one-argument functions that "
                "convert from a compatible unit or a dimensionless number. Substitute "
                "underscore for any hyphen in the name, and '_per_' for any slash: "
                "milliampere-hour --> milliampere_hour(), meter/sec --> meter_per_sec().\n\n";
        for (const char *unit : UnitConversion::getKnownUnits()) {
            std::string longName = opp_replacesubstring(opp_replacesubstring(UnitConversion::getLongName(unit), "-", "_", true), "/", "_per_", true);
            tmp << unit << "(), " << longName << "(), ";
        }
        tmp << "etc.";
        if (printDescriptions) {
            out << "  <unit_name> : quantity <unit_name>(quantity x)" << "\n";
            out << opp_indentlines(opp_breaklines(tmp.str().c_str(), 75), "    ") << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "neddecls")) {
        processed = true;
        if (verbose) {
            out << "Built-in NED declarations:\n\n";
            out << "---START---\n";
        }
        out << NedParser::getBuiltInDeclarations();
        if (verbose)
            out << "---END---\n";
        out << "\n";
    }
    if (wantAll || !strcmp(category, "units")) {
        processed = true;
        if (verbose) {
            out << "Recognized measurement units (note: other units can be used as well, only\n";
            out << "no automatic conversion will be available for them):\n";
        }
        std::vector<const char *> units = UnitConversion::getKnownUnits();
        for (auto u : units) {
            const char *bu = UnitConversion::getBaseUnit(u);
            out << "  " << u << "\t" << UnitConversion::getLongName(u);
            if (opp_strcmp(u, bu) != 0)
                out << "\t" << UnitConversion::getConversionDescription(u);
            out << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "enums")) {
        processed = true;
        if (verbose)
            out << "Enums defined in msg files:\n";
        cRegistrationList *table = enums.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "userinterfaces")) {
        processed = true;
        if (verbose)
            out << "User interfaces loaded:\n";
        cRegistrationList *table = omnetapps.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "resultfilters")) {
        processed = true;
        if (verbose)
            out << "Result filters that can be used in @statistic properties:\n";
        bool printDescriptions = true; // for now
        cRegistrationList *table = resultFilters.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cResultFilterType *obj = (cResultFilterType *)table->get(i);
            out << "  " << obj->getFullName() << "\n";
            if (printDescriptions && !opp_isempty(obj->getDescription()))
                out << opp_indentlines(opp_breaklines(obj->getDescription(), 75), "    ") << "\n";
            if (printDescriptions)
                out << "\n";
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "resultrecorders")) {
        processed = true;
        if (verbose)
            out << "Result recorders that can be used in @statistic properties:\n";
        bool printDescriptions = true; // for now
        cRegistrationList *table = resultRecorders.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cResultRecorderType *obj = (cResultRecorderType *)table->get(i);
            out << "  " << obj->getFullName() << "\n";
            if (printDescriptions && !opp_isempty(obj->getDescription()))
                out << opp_indentlines(opp_breaklines(obj->getDescription(), 75), "    ") << "\n";
            if (printDescriptions)
                out << "\n";
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "figures")) {
        processed = true;
        if (verbose)
            out << "Figure types and their accepted @figure property keys:\n";
        for (auto it : figureTypes) {
            std::string type = it.first;
            std::string className = it.second;
            cFigure *figure = check_and_cast<cFigure *>(createOne(className.c_str()));
            out << "  " << type << " (" << className << "): " << opp_join(figure->getAllowedPropertyKeys(), ", ") << "\n";
            delete figure;
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "sqliteschema")) {
        processed = true;
        if (verbose) {
            out << "Database schema for the SQLite result files:\n";
            out << "---START---\n";
        }
        out << opp_trim(opp_replacesubstring(SQL_CREATE_TABLES, "\n    ", "\n", true)) << "\n";
        if (verbose) {
            out << "---END---\n";
        }
    }

    if (!processed)
        throw cRuntimeError("Unrecognized category for '-h' option: %s", category);
}

void EnvirUtils::dumpResultRecorders(std::ostream& out, cComponent *component)
{
    dumpComponentResultRecorders(out, component);
    if (component->isModule()) {
        cModule *module = (cModule *)component;
        for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
            dumpResultRecorders(out, *it);
        for (cModule::ChannelIterator it(module); !it.end(); ++it)
            dumpResultRecorders(out, *it);
    }
}

void EnvirUtils::dumpComponentResultRecorders(std::ostream& out, cComponent *component)
{
    bool componentPathPrinted = false;
    std::vector<simsignal_t> signals = component->getLocalListenedSignals();
    for (int signalID : signals) {
        bool signalNamePrinted = false;
        std::vector<cIListener *> listeners = component->getLocalSignalListeners(signalID);
        for (auto & listener : listeners) {
            if (dynamic_cast<cResultListener *>(listener)) {
                if (!componentPathPrinted) {
                    out << component->getFullPath() << " (" << component->getNedTypeName() << "):\n";
                    componentPathPrinted = true;
                }
                if (!signalNamePrinted) {
                    out << "    \"" << cComponent::getSignalName(signalID) << "\" (signalID="  << signalID << "):\n";
                    signalNamePrinted = true;
                }
                dumpResultRecorderChain(out, (cResultListener *)listener, 0);
            }
        }
    }
}

void EnvirUtils::dumpResultRecorderChain(std::ostream& out, cResultListener *listener, int depth)
{
    std::string indent(4*depth+8, ' ');
    out << indent;
    if (ExpressionFilter *expressionFilter = dynamic_cast<ExpressionFilter *>(listener))
        out << expressionFilter->getExpression().str(Expression::SPACIOUSNESS_MAX) << " (" << listener->getClassName() << ")";
    else
        out << listener->getClassName();

    if (cResultRecorder *resultRecorder = dynamic_cast<cResultRecorder *>(listener))
        out << " ==> " << resultRecorder->getResultName();
    out << "\n";

    if (cResultFilter *resultFilter = dynamic_cast<cResultFilter *>(listener)) {
        std::vector<cResultListener *> delegates = resultFilter->getDelegates();
        for (auto & delegate : delegates)
            dumpResultRecorderChain(out, delegate, depth+1);
    }
}

}  // namespace envir
}  // namespace omnetpp

