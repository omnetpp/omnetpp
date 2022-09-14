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
#include "common/stlutil.h"
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
#include "envirbase.h"
#include "inifilecontents.h"
#include "envirutils.h"

using namespace omnetpp::common;
using namespace omnetpp::nedxml;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

using std::ostream;

std::string EnvirUtils::getConfigOptionType(cConfigOption *option)
{
    if (!option->isPerObject())
        return "Global setting";
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
    InifileContents tmp;
    InifileContents *ini = &tmp;
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
            cConfigOption *option = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(option);
            if (!printDescriptions)
                out << "  ";
            if (option->isPerObject())
                out << "**.";
            out << option->getName() << "=";
            out << "<" << cConfigOption::getTypeName(option->getType()) << ">";
            if (option->getUnit())
                out << ", unit=\"" << option->getUnit() << "\"";
            if (option->getDefaultValue())
                out << ", default:" << option->getDefaultValue() << "";
            if (!printDescriptions)
                out << "; " << (option->isPerObject() ? "per-object" : "global") << " setting"; // TODO getOptionKindName()

            out << "\n";
            if (printDescriptions)
                out << "    " << getConfigOptionType(option) << ".\n";
            if (printDescriptions && !opp_isempty(option->getDescription()))
                out << opp_indentlines(opp_breaklines(option->getDescription(), 75), "    ") << "\n";
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
        auto varNames = ini->getPredefinedVariableNames();
        for (const std::string& varName : varNames) {
            out << "${" << varName << "}\n";
            const char *desc = ini->getVariableDescription(varName.c_str());
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
            cConfigOption *option = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(option);
            out << "\\item[" << (option->isPerObject() ? "**." : "") << opp_latexquote(option->getName()) << "] = ";
            out << "\\textit{<" << cConfigOption::getTypeName(option->getType()) << ">}";
            if (option->getUnit())
                out << ", unit=\\ttt{" << option->getUnit() << "}";
            if (option->getDefaultValue())
                out << ", default: \\ttt{" << opp_latex_insert_breaks(opp_latexquote(option->getDefaultValue())) << "}";
            out << "\\\\\n";
            out << "    \\textit{" << getConfigOptionType(option) << ".}\\\\\n";
            out << opp_indentlines(opp_breaklines(opp_markup2latex(opp_latexquote(option->getDescription())), 75), "    ") << "\n";
        }
        out << "\\end{description}\n\n";
    }
    if (!strcmp(category, "latexconfigvars")) {  // internal undocumented option, for maintenance purposes
        processed = true;
        auto varNames = ini->getPredefinedVariableNames();
        out << "\\begin{description}\n";
        for (const std::string& varName : varNames) {
            out << "\\item[\\$\\{" << varName << "\\}] : \\\\\n";
            const char *desc = ini->getVariableDescription(varName.c_str());
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
            cConfigOption *option = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(option);

            std::string id = "CFGID_";
            for (const char *s = option->getName(); *s; s++)
                id.append(1, opp_isalpha(*s) ? opp_toupper(*s) : *s == '-' ? '_' : *s == '%' ? 'n' : *s);
            const char *method = option->isPerObject() ? "addPerObjectOption" : "addGlobalOption";
            #define CASE(X)  case cConfigOption::X: typestring = #X; break;
            const char *typestring;
            switch (option->getType()) {
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
            switch (option->getObjectKind()) {
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

            out << "    public OPP_THREAD_LOCAL final ConfigOption " << id << " = ";
            out << method << (option->getUnit() ? "U" : "") << "(\n";
            out << "        \"" << option->getName() << "\", ";
            if (option->isPerObject())
                out << kindstring << ", ";
            if (!option->getUnit())
                out << typestring << ", ";
            else
                out << "\"" << option->getUnit() << "\", ";
            if (!option->getDefaultValue())
                out << "null";
            else
                out << "\"" << opp_replacesubstring(option->getDefaultValue(), "\"", "\\\"", true) << "\"";
            out << ",\n";

            std::string desc = option->getDescription();
            desc = opp_replacesubstring(desc, "\n", "\\n\n", true);  // keep explicit line breaks
            desc = opp_breaklines(desc, 75);  // break long lines
            desc = opp_replacesubstring(desc, "\"", "\\\"", true);
            desc = opp_replacesubstring(desc, "\n", " \" +\n\"", true);
            desc = opp_replacesubstring(desc, "\\n \"", "\\n\"", true);  // remove bogus space after explicit line breaks
            desc = "\"" + desc + "\"";

            out << opp_indentlines(desc, "        ") << ");\n";
        }
        out << "\n";

        auto varNames = ini->getPredefinedVariableNames();
        for (const std::string& varName : varNames) {
            const char *desc = ini->getVariableDescription(varName.c_str());
            out << "    public static final String CFGVAR_" << opp_strupper(varName.c_str()) << " = addConfigVariable(";
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
        for (const char *unit : UnitConversion::getAllUnits()) {
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
        std::vector<const char *> units = UnitConversion::getAllUnits();
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

}  // namespace envir
}  // namespace omnetpp

