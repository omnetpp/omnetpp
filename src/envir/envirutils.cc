//==========================================================================
//  ENVIRUTILS.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "common/opp_ctype.h"
#include "nedxml/nedparser.h"  // NEDParser::getBuiltInDeclarations()
#include "omnetpp/ccanvas.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cchannel.h"
#include "sim/resultfilters.h"  // ExpressionFilter
#include "sim/resultrecorders.h"  // ExpressionRecorder
#include "args.h"
#include "appreg.h"
#include "envirutils.h"

using namespace omnetpp::common;
using namespace omnetpp::nedxml;

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

void EnvirUtils::dumpComponentList(const char *category)
{
    cConfigurationEx *config = getEnvir()->getConfigEx();
    bool wantAll = !strcmp(category, "all");
    bool processed = false;
    std::cout << "\n";
    if (wantAll || !strcmp(category, "config") || !strcmp(category, "configdetails")) {
        processed = true;
        std::cout << "Supported configuration options:\n";
        bool printDescriptions = strcmp(category, "configdetails") == 0;

        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *obj = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(obj);
            if (!printDescriptions)
                std::cout << "  ";
            if (obj->isPerObject())
                std::cout << "**.";
            std::cout << obj->getName() << "=";
            std::cout << "<" << cConfigOption::getTypeName(obj->getType()) << ">";
            if (obj->getUnit())
                std::cout << ", unit=\"" << obj->getUnit() << "\"";
            if (obj->getDefaultValue())
                std::cout << ", default:" << obj->getDefaultValue() << "";
            if (!printDescriptions)
                std::cout << "; " << (obj->isGlobal() ? "global" : obj->isPerObject() ? "per-object" : "per-run") << " setting"; // TODO getOptionKindName()

            std::cout << "\n";
            if (printDescriptions)
                std::cout << "    " << getConfigOptionType(obj) << ".\n";
            if (printDescriptions && !opp_isempty(obj->getDescription()))
                std::cout << opp_indentlines(opp_breaklines(obj->getDescription(), 75).c_str(), "    ") << "\n";
            if (printDescriptions)
                std::cout << "\n";
        }
        std::cout << "\n";

        std::cout << "Predefined variables that can be used in config values:\n";
        std::vector<const char *> v = config->getPredefinedVariableNames();
        for (int i = 0; i < (int)v.size(); i++) {
            if (!printDescriptions)
                std::cout << "  ";
            std::cout << "${" << v[i] << "}\n";
            const char *desc = config->getVariableDescription(v[i]);
            if (printDescriptions && !opp_isempty(desc))
                std::cout << opp_indentlines(opp_breaklines(desc, 75).c_str(), "    ") << "\n";
        }
        std::cout << "\n";
    }
    if (!strcmp(category, "latexconfig")) {  // internal undocumented option, for maintenance purposes
        // generate LaTeX code for the appendix in the User Manual
        processed = true;
        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        std::cout << "\\begin{description}\n";
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *obj = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(obj);
            std::cout << "\\item[" << (obj->isPerObject() ? "**." : "") << opp_latexQuote(obj->getName()) << "] = ";
            std::cout << "\\textit{<" << cConfigOption::getTypeName(obj->getType()) << ">}";
            if (obj->getUnit())
                std::cout << ", unit=\\ttt{" << obj->getUnit() << "}";
            if (obj->getDefaultValue())
                std::cout << ", default: \\ttt{" << opp_latexInsertBreaks(opp_latexQuote(obj->getDefaultValue()).c_str()) << "}";
            std::cout << "\\\\\n";
            std::cout << "    \\textit{" << getConfigOptionType(obj) << ".}\\\\\n";
            std::cout << opp_indentlines(opp_breaklines(opp_markup2Latex(opp_latexQuote(obj->getDescription()).c_str()).c_str(), 75).c_str(), "    ") << "\n";
        }
        std::cout << "\\end{description}\n\n";

        std::cout << "Predefined variables that can be used in config values:\n\n";
        std::vector<const char *> v = config->getPredefinedVariableNames();
        std::cout << "\\begin{description}\n";
        for (int i = 0; i < (int)v.size(); i++) {
            std::cout << "\\item[\\$\\{" << v[i] << "\\}] : \\\\\n";
            const char *desc = config->getVariableDescription(v[i]);
            std::cout << opp_indentlines(opp_breaklines(opp_markup2Latex(opp_latexQuote(desc).c_str()).c_str(), 75).c_str(), "    ") << "\n";
        }
        std::cout << "\\end{description}\n\n";
    }
    if (!strcmp(category, "jconfig")) {  // internal undocumented option, for maintenance purposes
        // generate Java code for ConfigurationRegistry.java in the IDE
        processed = true;
        std::cout << "Supported configuration options (as Java code):\n";
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

            std::cout << "    public static final ConfigOption " << id << " = ";
            std::cout << method << (key->getUnit() ? "U" : "") << "(\n";
            std::cout << "        \"" << key->getName() << "\", ";
            if (key->isPerObject())
                std::cout << kindstring << ", ";
            if (!key->getUnit())
                std::cout << typestring << ", ";
            else
                std::cout << "\"" << key->getUnit() << "\", ";
            if (!key->getDefaultValue())
                std::cout << "null";
            else
                std::cout << "\"" << opp_replacesubstring(key->getDefaultValue(), "\"", "\\\"", true) << "\"";
            std::cout << ",\n";

            std::string desc = key->getDescription();
            desc = opp_replacesubstring(desc.c_str(), "\n", "\\n\n", true);  // keep explicit line breaks
            desc = opp_breaklines(desc.c_str(), 75);  // break long lines
            desc = opp_replacesubstring(desc.c_str(), "\"", "\\\"", true);
            desc = opp_replacesubstring(desc.c_str(), "\n", " \" +\n\"", true);
            desc = opp_replacesubstring(desc.c_str(), "\\n \"", "\\n\"", true);  // remove bogus space after explicit line breaks
            desc = "\"" + desc + "\"";

            std::cout << opp_indentlines(desc.c_str(), "        ") << ");\n";
        }
        std::cout << "\n";

        std::vector<const char *> vars = config->getPredefinedVariableNames();
        for (int i = 0; i < (int)vars.size(); i++) {
            opp_string id = vars[i];
            opp_strupr(id.buffer());
            const char *desc = config->getVariableDescription(vars[i]);
            std::cout << "    public static final String CFGVAR_" << id << " = addConfigVariable(";
            std::cout << "\"" << vars[i] << "\", \"" << opp_replacesubstring(desc, "\"", "\\\"", true) << "\");\n";
        }
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "classes")) {
        processed = true;
        std::cout << "Registered C++ classes, including modules, channels and messages:\n";
        cRegistrationList *table = classes.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            std::cout << "  class " << obj->getFullName() << "\n";
        }
        std::cout << "Note: if your class is not listed, it needs to be registered in the\n";
        std::cout << "C++ code using Define_Module(), Define_Channel() or Register_Class().\n";
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "classdesc")) {
        processed = true;
        std::cout << "Classes that have associated reflection information (needed for Tkenv inspectors):\n";
        cRegistrationList *table = classDescriptors.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            std::cout << "  class " << obj->getFullName() << "\n";
        }
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "nedfunctions")) {
        processed = true;
        std::cout << "Functions that can be used in NED expressions and in omnetpp.ini:\n";
        cRegistrationList *table = nedFunctions.getInstance();
        table->sort();
        std::set<std::string> categories;
        for (int i = 0; i < table->size(); i++) {
            cNEDFunction *nf = dynamic_cast<cNEDFunction *>(table->get(i));
            cNEDMathFunction *mf = dynamic_cast<cNEDMathFunction *>(table->get(i));
            categories.insert(nf ? nf->getCategory() : mf ? mf->getCategory() : "???");
        }
        for (std::set<std::string>::iterator ci = categories.begin(); ci != categories.end(); ++ci) {
            std::string category = (*ci);
            std::cout << "\n Category \"" << category << "\":\n";
            for (int i = 0; i < table->size(); i++) {
                cObject *obj = table->get(i);
                cNEDFunction *nf = dynamic_cast<cNEDFunction *>(table->get(i));
                cNEDMathFunction *mf = dynamic_cast<cNEDMathFunction *>(table->get(i));
                const char *fcat = nf ? nf->getCategory() : mf ? mf->getCategory() : "???";
                const char *desc = nf ? nf->getDescription() : mf ? mf->getDescription() : "???";
                if (fcat == category) {
                    std::cout << "  " << obj->getFullName() << " : " << obj->info() << "\n";
                    if (desc)
                        std::cout << "    " << desc << "\n";
                }
            }
        }
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "neddecls")) {
        processed = true;
        std::cout << "Built-in NED declarations:\n\n";
        std::cout << "---START---\n";
        std::cout << NEDParser::getBuiltInDeclarations();
        std::cout << "---END---\n";
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "units")) {
        processed = true;
        std::cout << "Recognized physical units (note: other units can be used as well, only\n";
        std::cout << "no automatic conversion will be available for them):\n";
        std::vector<const char *> units = UnitConversion::getAllUnits();
        for (int i = 0; i < (int)units.size(); i++) {
            const char *u = units[i];
            const char *bu = UnitConversion::getBaseUnit(u);
            std::cout << "  " << u << "\t" << UnitConversion::getLongName(u);
            if (omnetpp::opp_strcmp(u, bu) != 0)
                std::cout << "\t" << UnitConversion::convertUnit(1, u, bu) << bu;
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "enums")) {
        processed = true;
        std::cout << "Enums defined in .msg files\n";
        cRegistrationList *table = enums.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            std::cout << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
        std::cout << "\n";
    }
    if (wantAll || !strcmp(category, "userinterfaces")) {
        processed = true;
        std::cout << "User interfaces loaded:\n";
        cRegistrationList *table = omnetapps.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            std::cout << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
    }

    if (wantAll || !strcmp(category, "resultfilters")) {
        processed = true;
        std::cout << "Result filters that can be used in @statistic properties:\n";
        cRegistrationList *table = resultFilters.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            std::cout << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
    }

    if (wantAll || !strcmp(category, "resultrecorders")) {
        processed = true;
        std::cout << "Result recorders that can be used in @statistic properties:\n";
        cRegistrationList *table = resultRecorders.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            std::cout << "  " << obj->getFullName() << " : " << obj->info() << "\n";
        }
    }

    if (wantAll || !strcmp(category, "figures")) {
        processed = true;
        std::cout << "Figure types and their accepted @figure property keys:\n";
        cRegistrationList *table = classes.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObjectFactory *factory = check_and_cast<cObjectFactory *>(table->get(i));
            const char *className = factory->getFullName();
            if (opp_stringendswith(className, "Figure")) {
                cObject *obj = factory->createOne();
                cFigure *figure = dynamic_cast<cFigure *>(obj);
                if (figure) {
                    opp_string type = className[0] == 'c' ? className+1 : className;
                    opp_strlwr(type.buffer());
                    type.buffer()[type.size()-6] = '\0';
                    std::cout << "  " << type.c_str() << " (" << className << "): " << opp_join(figure->getAllowedPropertyKeys(), ", ") << "\n";
                }
                delete obj;
            }
        }
        std::cout << "\n";
    }

    if (!processed)
        throw cRuntimeError("Unrecognized category for '-h' option: %s", category);
}

void EnvirUtils::dumpResultRecorders(cComponent *component)
{
    dumpComponentResultRecorders(component);
    if (component->isModule()) {
        cModule *module = (cModule *)component;
        for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
            dumpResultRecorders(*it);
        for (cModule::ChannelIterator it(module); !it.end(); ++it)
            dumpResultRecorders(*it);
    }
}

void EnvirUtils::dumpComponentResultRecorders(cComponent *component)
{
    bool componentPathPrinted = false;
    std::vector<simsignal_t> signals = component->getLocalListenedSignals();
    for (unsigned int i = 0; i < signals.size(); i++) {
        bool signalNamePrinted = false;
        simsignal_t signalID = signals[i];
        std::vector<cIListener *> listeners = component->getLocalSignalListeners(signalID);
        for (unsigned int j = 0; j < listeners.size(); j++) {
            if (dynamic_cast<cResultListener *>(listeners[j])) {
                if (!componentPathPrinted) {
                    std::cout << component->getFullPath() << " (" << component->getNedTypeName() << "):\n";
                    componentPathPrinted = true;
                }
                if (!signalNamePrinted) {
                    std::cout << "    \"" << cComponent::getSignalName(signalID) << "\" (signalID="  << signalID << "):\n";
                    signalNamePrinted = true;
                }
                dumpResultRecorderChain((cResultListener *)listeners[j], 0);
            }
        }
    }
}

void EnvirUtils::dumpResultRecorderChain(cResultListener *listener, int depth)
{
    std::string indent(4*depth+8, ' ');
    std::cout << indent;
    if (ExpressionRecorder *expressionRecorder = dynamic_cast<ExpressionRecorder *>(listener))
        std::cout << expressionRecorder->getExpression().str() << " (" << listener->getClassName() << ")";
    else if (ExpressionFilter *expressionFilter = dynamic_cast<ExpressionFilter *>(listener))
        std::cout << expressionFilter->getExpression().str() << " (" << listener->getClassName() << ")";
    else
        std::cout << listener->getClassName();

    if (cResultRecorder *resultRecorder = dynamic_cast<cResultRecorder *>(listener))
        std::cout << " ==> " << resultRecorder->getResultName();
    std::cout << "\n";

    if (cResultFilter *resultFilter = dynamic_cast<cResultFilter *>(listener)) {
        std::vector<cResultListener *> delegates = resultFilter->getDelegates();
        for (unsigned int i = 0; i < delegates.size(); i++)
            dumpResultRecorderChain(delegates[i], depth+1);
    }
}

}  // namespace envir
}  // namespace omnetpp

