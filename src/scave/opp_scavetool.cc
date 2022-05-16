//=========================================================================
//  OPP_SCAVETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include <iomanip>
#include <map>
#include <memory>
#include <algorithm>
#include "common/ver.h"
#include "common/fileutil.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/formattedprinter.h"
#include "common/unitconversion.h"
#include "common/stlutil.h"
#include "resultfilemanager.h"
#include "indexfileutils.h"
#include "fields.h"
#include "scaveutils.h"
#include "sqliteresultfileutils.h"
#include "exporter.h"
#include "opp_scavetool.h"
#include "vectorfileindex.h"
#include "vectorfileindexer.h"

#if defined(__MINGW32__)
int _CRT_glob = 0;  // Turn off runtime file globbing support on MinGW. The shell already handles file globbing on the command line.
#endif

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

//TODO change "histogram" to "statistic"

void ScaveTool::helpCommand(int argc, char **argv)
{
    string page = argc==0 ? "options" : argv[0];
    printHelpPage(page);
}

void ScaveTool::printHelpPage(const std::string& page)
{
    FormattedPrinter help(cout);
    if (page == "options") {
        help.line("opp_scavetool -- part of " OMNETPP_PRODUCT ", (C) 2006-2018 OpenSim Ltd.");
        help.line("Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION);
        help.line();
        help.para("Usage: opp_scavetool <command> [<options>] <files>...");
        help.para("For processing result files written by simulations: vector files (.vec) and scalar files (.sca).");
        help.line("Commands:");
        help.option("q, query", "Query the contents of result files");
        help.option("x, export", "Export results in various formats");
        help.option("i, index", "Generate index files (.vci) for vector files");
        help.option("h, help", "Print help text");
        help.line();
        help.para("The <files> argument accepts directories and glob patterns as well, in addition to file names. "
                "Glob patterns may contain '**' which can match any number of directory levels. "
                "Be sure to quote the pattern on the command line to prevent the shell from expanding them before invoking scavetool.");
        help.para("The default command is 'query'.");
        help.para("To get help, use opp_scavetool help <topic>. Available help topics: 'filter', 'exporters', and all short and long command names.");
    }
    else if (page == "h" || page == "help") {
        help.para("Usage: opp_scavetool help <topic>");
        help.para("Print help text on the given topic.");
    }
    else if (page == "q" || page == "query") {
        help.para("Usage: opp_scavetool query [<mode-option>] [<options>] <dirs-or-output-vector-and-scalar-files>");
        help.para("Query the contents of result files.");
        help.line("Mode options (mutually exclusive):");
        help.option("-s, --print-summary", "Report the number of result items. (This is the default.)");
        help.option("-l, --list-results", "List result items");
        help.option("-a, --list-runattrs", "List run attributes");
        help.option("-i, --list-itervars", "List iteration variables");
        help.option("-j, --list-configentries", "List config entries");
        help.option("-t, --list-paramassignments", "List NED parameter assignments (a subset of config entries)");
        help.option("-n, --list-names", "List unique result (vector, scalar, etc) names");
        help.option("-m, --list-modules", "List unique module names");
        help.option("-e  --list-qnames", "List unique result names qualified with the module names they occur with");
        help.option("-r, --list-runs", "List unique runs");
        help.option("-c, --list-configs", "List unique configuration names");
        help.line();
        help.line("Options:");
        help.option("-T, --type <types>", "Limit item types; <types> is concatenation of type characters (v=vector, s=scalar, t=statistic, h=histogram, p=parameter).");
        help.option("-f, --filter <filter>", "Filter the queried items using a filter expression (try 'help filter')");
        help.option("-p, --per-run", "Per-run reporting (where applicable)");
        help.option("-b, --bare", "Suppress labels (more suitable for machine processing)");
        help.option("-g, --grep-friendly", "Grep-friendly: with -p, put run names at the start of each line, not above groups as headings.");
        help.option("    --tabs", "Use tabs in tables instead of padding with spaces.");
        help.option("-w, --add-fields-as-scalars", "Add statistics fields (count, sum, mean, stddev, min, max, etc) as scalars");
        help.option("-D, --rundisplay <format>", "Display format for run; <format> can be any of:\n"
                    "  'runid'       Displays ${runid} (this is the default)\n"
                    "  'runnumber'   Displays ${configname} ${runnumber}\n"
                    "  'itervars'    Displays ${configname} ${iterationvars} ${repetition}\n"
                    "  'experiment'  Displays ${experiment} ${measurement} ${replication}\n");
        help.option("-k, --no-indexing", "Disallow automatic indexing of vector files");
        help.option("--allow-nonmatching", "Allow non-matching glob patterns on the command line");
        help.option("-v, --verbose", "Print info about progress (verbose)");
        help.line();
        help.para("The <files> argument accepts directories and glob/globstar patterns as well, in addition to file names. See main help page for details.");
        help.para("See also the following help topics: 'filter'");
    }
    else if (page == "x" || page == "export") {
        help.para("Usage: opp_scavetool export [<options>] <output-vector-and-scalar-files>");
        help.para("Export results in various formats.");
        help.line("Options:");
        help.option("-T, --type <types>", "Limit item types; <types> is concatenation of type characters (v=vector, s=scalar, t=statistic, h=histogram, p=parameter).");
        help.option("-f, --filter <filter>", "Filter for result items (vectors, scalars, statistics, histograms, parameters) matched by filter expression (try 'help filter')");
        help.option("-w, --add-fields-as-scalars", "Add statistics fields (count, sum, mean, stddev, min, max, etc) as scalars");
        help.option("--start-time", "Limit vector data to after the given simulation time (inclusive)");
        help.option("--end-time", "Limit vector data to before the given simulation time (exclusive)");
        help.option("-o <filename>", "Output file name, or '-' for the standard output. This option is mandatory.");
        help.option("-F <format>", "Selects the exporter. The exporter's operation may further be customized via -x options.");
        help.option("-x <key>=<value>", "Option for the exporter. This option may occur multiple times.");
        help.option("--<key>=<value>", "Same as -x <key>=<value>.");
        help.option("-k, --no-indexing", "Disallow automatic indexing of vector files");
        help.option("--allow-nonmatching", "Allow non-matching glob patterns on the command line");
        help.option("-v, --verbose", "Print info about progress (verbose)");
        help.line();
        help.para("Supported export formats: " + opp_join(ExporterFactory::getSupportedFormats(), ", ", '\''));
        help.para("See the 'exporters' help topic for the list of available options for each export format.");
        help.para("The <files> argument accepts directories and glob/globstar patterns as well, in addition to file names. See main help page for details.");
        help.para("See also the following help topics: 'filter'");
    }
    else if (page == "exporters") {
        help.para("Scavetool can export data the following formats:");
        help.indentPara(opp_join(ExporterFactory::getSupportedFormats(), ", ", '\''));
        for (string format : ExporterFactory::getSupportedFormats()) {
            ExporterType *exporter = ExporterFactory::getByFormat(format);
            help.para("'" + format + "' (" + exporter->getDisplayName() + ")");
            help.indentPara(exporter->getDescription());
            help.indentPara("Options:");
            auto options = ExporterFactory::getByFormat(format)->getSupportedOptions();
            if (options.empty())
                help.option("None", "");
            else
                for (auto pair : options)
                    help.option(pair.first, pair.second);
            help.line();
        }
        help.line();
    }
    else if (page == "i" || page == "index") {
        help.para("Usage: opp_scavetool index [<options>] <output-vector-files>");
        help.para("Generate index files (.vci) for vector files. Note that this command "
                  "is usually not needed, as other opp_scavetool commands automatically "
                  "create indices for loaded vector files if they are missing or out of date, "
                  "unless indexing is explicitly disabled.");
        help.line("Options:");
        help.option("-v, --verbose", "Print info about progress (verbose)");
        help.para("The <files> argument accepts directories and glob/globstar patterns as well, in addition to file names. See main help page for details.");
        help.line();
    }
    else if (page == "filter") {
        help.para("Several commands have a -f <filter> option that accepts a match expression "
                  "for filtering result items. This page describes the syntax available for "
                  "match expressions.");
        help.para("A match expression consist of '<pattern>' and '<fieldname> =~ <pattern>' "
                  "elements, combined with AND, OR, and NOT operators. A plain '<pattern>' is "
                  "equivalent to 'name =~ <pattern>'. Patterns need to be enclosed in "
                  "quotation marks if they contain space or other characters that make "
                  "their parsing ambiguous.");
        help.line("<fieldname> is one of:");
        help.option("file",   "Full path of the result file");
        help.option("run",    "Name (unique ID string) of the result's simulation run");
        help.option("type",   "Type of the result item: scalar, vector, statistics, histogram, or parameter");
        help.option("module", "Full path of the result item's module");
        help.option("name",   "Name of the result item");
        help.option("runattr:<name>", "Value of a run attribute in the result item's run");
        help.option("itervar:<name>", "Value of an iteration variable in the result item's run");
        help.option("config:<name>",  "Value of a configuration entry in the result item's run");
        help.option("attr:<name>",    "Value of a result attribute");
        help.line();
        help.line("<pattern> is a glob-like pattern:");
        help.option("?", "Matches any character except '.'");
        help.option("*", "Matches zero or more characters, except '.'");
        help.option("**", "Matches zero or more characters (any character)");
        help.option("{a-z}", "Matches a character in range a-z");
        help.option("{^a-z}", "Matches a character NOT in range a-z");
        help.option("{250..300}", "Matches an integer number in the given range");
        help.option("[250..300]", "Matches an integer inside brackets in the given range'");
        help.option("\\ (backslash)", "Takes away the special meaning of the subsequent character");
        help.line();
        help.line("Examples:");
        help.line("  module=~\"**.sink\" AND (name=~\"queueing time\" OR name=~\"transmission time\")");
        help.line("  runattr:experiment=~Aloha AND itervar:numHosts=~{1..10} AND channelUtilization");
        help.line("  config:network =~ Mesh AND type =~ scalar AND module=~ \"*[5..8]\"");
        help.line("  type =~ scalar AND attr:unit =~ \"s\"");
        help.line();
    }
    else {
        throw opp_runtime_error("No help topic '%s'", page.c_str());
    }
}

void ScaveTool::loadFiles(ResultFileManager& manager, const vector<string>& fileNames, bool indexingAllowed, bool allowNonmatching, bool verbose)
{
    if (fileNames.empty()) {
        cerr << "opp_scavetool: Warning: No input files\n";
        return;
    }

    typedef ResultFileManager RFM;
    int loadFlags = RFM::NEVER_RELOAD | (indexingAllowed ? RFM::ALLOW_INDEXING : RFM::ALLOW_LOADING_WITHOUT_INDEX) | RFM::SKIP_IF_LOCKED | (verbose ? RFM::VERBOSE : 0);

    // load files
    for (auto& i : fileNames) {
        const char *fileArg = i.c_str();
        std::vector<std::string> filesToLoad;

        if (isDirectory(fileArg)) {
            filesToLoad = collectFilesInDirectory(fileArg, true, ".sca");
            addAll(filesToLoad, collectFilesInDirectory(fileArg, true, ".vec"));
        }
        else if (strchr(fileArg, '*') != nullptr || strchr(fileArg, '?') != nullptr) {
            filesToLoad = collectMatchingFiles(fileArg);
            if (filesToLoad.empty() && !allowNonmatching)
                filesToLoad.push_back(fileArg); // like "bash" does; allows reporting errors in the pattern ("**/foo*.vec: no such file")
        }
        else {
            filesToLoad.push_back(fileArg);
        }

        for (auto& fileName : filesToLoad)
            manager.loadFile(fileName.c_str(), fileName.c_str(), loadFlags, nullptr);
    }

    if (verbose)
        cout << manager.getFiles().size() << " file(s) loaded\n";
}

int ScaveTool::resolveResultTypeFilter(const std::string& filter)
{
    int result = 0;
    if (filter.length() <= 5) {
        // short form
        for (char c : filter) {
            switch(c) {
            case 's': result |= ResultFileManager::SCALAR; break;
            case 'v': result |= ResultFileManager::VECTOR; break;
            case 't': result |= ResultFileManager::STATISTICS; break;
            case 'h': result |= ResultFileManager::HISTOGRAM; break;
            case 'p': result |= ResultFileManager::PARAMETER; break;
            default: throw opp_runtime_error("Invalid result type '%c' in '-T' option", c);
            }
        }
    }
    else {
        // long form
        for (string token : opp_splitandtrim(filter, ",")) {
            if (token == "scalars")
                result |= ResultFileManager::SCALAR;
            else if (token == "vectors")
                result |= ResultFileManager::VECTOR;
            else if (token == "statistics")
                result |= ResultFileManager::STATISTICS;
            else if (token == "histograms")
                result |= ResultFileManager::HISTOGRAM;
            else if (token == "parameters")
                result |= ResultFileManager::PARAMETER;
            else
                throw opp_runtime_error("Invalid result type '%s' in '-T' option", token.c_str());
        }
    }
    return result;
}

string ScaveTool::rebuildCommandLine(int argc, char **argv)
{
    string result;
    for (int i = 0; i < argc; i++) {
        if (i != 0)
            result += " ";
        result += opp_quotestr_ifneeded(argv[i]);
    }
    return result;
}

static void print(std::ostream& out, const StringSet& strings, const string& prefix = "")
{
    for (auto str : strings)
        out << prefix << str << "\n";
}

enum RunDisplayMode {
    RUNDISPLAY_RUNID, RUNDISPLAY_CONFIG_RUNNUMBER, RUNDISPLAY_CONFIG_ITERVARS_REPETITION,
    RUNDISPLAY_EXPERIMENT_MEASUREMENT_REPLICATION
};

static string runStr(const Run *run, RunDisplayMode mode)
{
    switch(mode) {
    case RUNDISPLAY_RUNID:
        return run->getRunName();
    case RUNDISPLAY_CONFIG_RUNNUMBER:
        return string(opp_emptytodefault(run->getAttribute("configname").c_str(), "null")) + "\t" +
                opp_emptytodefault(run->getAttribute("runnumber").c_str(), "null");
    case RUNDISPLAY_CONFIG_ITERVARS_REPETITION:
        return string(opp_emptytodefault(run->getAttribute("configname").c_str(), "null")) + "\t" +
                opp_emptytodefault(run->getAttribute("iterationvars").c_str(), "null") + "\t" +
                opp_emptytodefault(run->getAttribute("repetition").c_str(), "null");
    case RUNDISPLAY_EXPERIMENT_MEASUREMENT_REPLICATION:
        return string(opp_emptytodefault(run->getAttribute("experiment").c_str(), "null")) + "\t" +
                opp_emptytodefault(run->getAttribute("measurement").c_str(), "null") + "\t" +
                opp_emptytodefault(run->getAttribute("replication").c_str(), "null");
    default:
        Assert(false); return "???";
    }
}

typedef std::string get_value_t(Run * run, const std::string &name);

static void listRunMetadata(std::ostream &out, const RunAndValueList& metadata, get_value_t getValue, RunDisplayMode runDisplayMode, bool grepFriendly)
{
    Run *prevRun = nullptr;

    // note: we ignore opt_perRun, as it makes no sense here
    for (std::pair<Run *, std::string> i : metadata) {
        string runName = runStr(i.first, runDisplayMode);
        string maybeRunColumnWithTab = grepFriendly ? runName + "\t" : "";

        if (i.first != prevRun && !grepFriendly) {
            if (prevRun != nullptr)
                out << endl;
            out << runName << ":" << endl << endl;
        }

        out << maybeRunColumnWithTab << i.second << "\t" << getValue(i.first, i.second) << std::endl;

        prevRun = i.first;
    }
    out << endl;
}

void ScaveTool::queryCommand(int argc, char **argv)
{
    enum QueryMode {
        PRINT_SUMMARY, LIST_RESULTS, LIST_RUNATTRS, LIST_ITERVARS, LIST_CONFIGENTRIES, LIST_PARAMASSIGNMENTS,
        LIST_MODULES, LIST_NAMES, LIST_MODULE_AND_NAME_PAIRS, LIST_RUNS, LIST_CONFIGS
    };

    QueryMode opt_mode = PRINT_SUMMARY;
    vector<string> opt_fileNames;
    string opt_filterExpression = "*";
    string opt_runDisplayModeStr;
    int opt_resultTypeFilter = ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM | ResultFileManager::PARAMETER;
    RunDisplayMode opt_runDisplayMode = RUNDISPLAY_RUNID;
    bool opt_includeFields = false;
    bool opt_bare = false;
    bool opt_perRun = false;
    bool opt_grepFriendly = false;
    bool opt_useTabs = false;
    bool opt_verbose = false;
    bool opt_indexingAllowed = true;
    bool opt_allowNonmatching = false;

    // parse options
    bool endOpts = false;
    for (int i = 0; i < argc; i++) {
        string opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (opt == "--")
            endOpts = true;
        else if (opt == "-s" || opt == "--print-summary")
            opt_mode = PRINT_SUMMARY;
        else if (opt == "-l" || opt == "--list-results")
            opt_mode = LIST_RESULTS;
        else if (opt == "-a" || opt == "--list-runattrs")
            opt_mode = LIST_RUNATTRS;
        else if (opt == "-i" || opt == "--list-itervars")
            opt_mode = LIST_ITERVARS;
        else if (opt == "-j" || opt == "--list-configentries")
            opt_mode = LIST_CONFIGENTRIES;
        else if (opt == "-t" || opt == "--list-paramassignments")
            opt_mode = LIST_PARAMASSIGNMENTS;
        else if (opt == "-n" || opt == "--list-names")
            opt_mode = LIST_NAMES;
        else if (opt == "-m" || opt == "--list-modules")
            opt_mode = LIST_MODULES;
        else if (opt == "-e" || opt == "--list-qnames")
            opt_mode = LIST_MODULE_AND_NAME_PAIRS;
        else if (opt == "-r" || opt == "--list-runs")
            opt_mode = LIST_RUNS;
        else if (opt == "-c" || opt == "--list-configs")
            opt_mode = LIST_CONFIGS;
        else if ((opt == "-T" || opt == "--type") && i != argc-1)
            opt_resultTypeFilter = resolveResultTypeFilter(unquoteString(argv[++i]));
        else if (opt.substr(0,2) == "-T")
            opt_resultTypeFilter = resolveResultTypeFilter(opt.substr(2));
        else if ((opt == "-f" || opt == "--filter") && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if ((opt == "-D" || opt == "--rundisplay") && i != argc-1)
            opt_runDisplayModeStr = unquoteString(argv[++i]);
        else if (opt.substr(0,2) == "-D")
            opt_runDisplayModeStr = opt.substr(2);
        else if (opt == "-w" || opt == "--add-fields-as-scalars")
            opt_includeFields = true;
        else if (opt == "-p" || opt == "--per-run")
            opt_perRun = true;
        else if (opt == "-b" || opt == "--bare")
            opt_bare = true;
        else if (opt == "-g" || opt == "--grep-friendly")
            opt_grepFriendly = true;
        else if (opt == "--tabs")
            opt_useTabs = true;
        else if (opt == "-k" || opt == "--no-indexing")
            opt_indexingAllowed = false;
        else if (opt == "--allow-nonmatching")
            opt_allowNonmatching = true;
        else if (opt == "-v" || opt == "--verbose")
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    // resolve -D, run display mode
    if (opt_runDisplayModeStr != "") {
        if (opt_runDisplayModeStr == "runid")
            opt_runDisplayMode = RUNDISPLAY_RUNID;
        else if (opt_runDisplayModeStr == "runnumber")
            opt_runDisplayMode = RUNDISPLAY_CONFIG_RUNNUMBER;
        else if (opt_runDisplayModeStr == "itervars")
            opt_runDisplayMode = RUNDISPLAY_CONFIG_ITERVARS_REPETITION;
        else if (opt_runDisplayModeStr == "experiment")
            opt_runDisplayMode = RUNDISPLAY_EXPERIMENT_MEASUREMENT_REPLICATION;
        else
            throw opp_runtime_error("Invalid run display mode '%s' in '-D' option", opt_runDisplayModeStr.c_str());
    }

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_allowNonmatching, opt_verbose);

    // filter statistics
    IDList results = resultFileManager.getAllItems(opt_includeFields);
    if (opt_mode != LIST_RUNS && opt_mode != LIST_RUNATTRS && opt_mode != LIST_ITERVARS && opt_mode != LIST_CONFIGENTRIES && opt_mode != LIST_PARAMASSIGNMENTS) {
        results = results.filterByTypes(opt_resultTypeFilter);
        results = resultFileManager.filterIDList(results, opt_filterExpression.c_str());
    }

    RunList runs = resultFileManager.getUniqueRuns(results);
    std::sort(runs.begin(), runs.end(), [](Run *a, Run *b)  {return a->getRunName() < b->getRunName();}); // sort runs by runId, for consistent output
    IDList scalars = results.filterByTypes(ResultFileManager::SCALAR);
    IDList parameters = results.filterByTypes(ResultFileManager::PARAMETER);
    IDList vectors = results.filterByTypes(ResultFileManager::VECTOR);
    IDList statistics = results.filterByTypes(ResultFileManager::STATISTICS);
    IDList histograms = results.filterByTypes(ResultFileManager::HISTOGRAM);

    stringstream buffer;
    ostream& out = opt_useTabs ? cout : buffer;

    switch (opt_mode) {
    case PRINT_SUMMARY: {
#define L(label) (opt_bare ? "\t" : label)
        if (!opt_perRun) {
            out << L("runs: ") << runs.size() << " ";
            if ((opt_resultTypeFilter & ResultFileManager::SCALAR) != 0)
                out << L("\tscalars: ") << scalars.size();
            if ((opt_resultTypeFilter & ResultFileManager::PARAMETER) != 0)
                out << L("\tparameters: ") << parameters.size();
            if ((opt_resultTypeFilter & ResultFileManager::VECTOR) != 0)
                out << L("\tvectors: ") << vectors.size();
            if ((opt_resultTypeFilter & ResultFileManager::STATISTICS) != 0)
                out << L("\tstatistics: ") << statistics.size();
            if ((opt_resultTypeFilter & ResultFileManager::HISTOGRAM) != 0)
                out << L("\thistograms: ") << histograms.size();
            out << endl;
        }
        else {
            for (Run *run : runs) {
                out << runStr(run, opt_runDisplayMode);
                if ((opt_resultTypeFilter & ResultFileManager::SCALAR) != 0) {
                    IDList runScalars = resultFileManager.filterIDList(scalars, run, nullptr, nullptr);
                    out << L("\tscalars: ") << runScalars.size();
                }
                if ((opt_resultTypeFilter & ResultFileManager::PARAMETER) != 0) {
                    IDList runParameters = resultFileManager.filterIDList(parameters, run, nullptr, nullptr);
                    out << L("\tparameters: ") << runParameters.size();
                }
                if ((opt_resultTypeFilter & ResultFileManager::VECTOR) != 0) {
                    IDList runVectors = resultFileManager.filterIDList(vectors, run, nullptr, nullptr);
                    out << L("\tvectors: ") << runVectors.size();
                }
                if ((opt_resultTypeFilter & ResultFileManager::STATISTICS) != 0) {
                    IDList runStatistics = resultFileManager.filterIDList(statistics, run, nullptr, nullptr);
                    out << L("\tstatistics: ") << runStatistics.size();
                }
                if ((opt_resultTypeFilter & ResultFileManager::HISTOGRAM) != 0) {
                    IDList runHistograms = resultFileManager.filterIDList(histograms, run, nullptr, nullptr);
                    out << L("\thistograms: ") << runHistograms.size();
                }
                out << endl;
            }
        }
        break;
    }
#undef L
    case LIST_RESULTS: {
        // note: we ignore opt_perRun, as it makes no sense here
        for (Run *run : runs) {
            string runName = runStr(run, opt_runDisplayMode);
            string maybeRunColumnWithTab = opt_grepFriendly ? runName + "\t" : "";
            if (!opt_grepFriendly)
                out << runName << ":" << endl << endl;
#define L(label) (opt_bare ? "\t" : "\t" #label "=")
            IDList runScalars = resultFileManager.filterIDList(scalars, run, nullptr, nullptr);
            IDList runParameters = resultFileManager.filterIDList(parameters, run, nullptr, nullptr);
            IDList runVectors = resultFileManager.filterIDList(vectors, run, nullptr, nullptr);
            IDList runStatistics = resultFileManager.filterIDList(statistics, run, nullptr, nullptr);
            IDList runHistograms = resultFileManager.filterIDList(histograms, run, nullptr, nullptr);

            ScalarResult buffer;
            for (ID id : runScalars) {
                const ScalarResult *s = resultFileManager.getScalar(id, buffer);
                out << maybeRunColumnWithTab << "scalar\t" << s->getModuleName() << "\t" << s->getName() << "\t" << s->getValue() << endl;
            }

            for (ID id : runParameters) {
                const ParameterResult *s = resultFileManager.getParameter(id);
                out << maybeRunColumnWithTab << "parameter\t" << s->getModuleName() << "\t" << s->getName() << "\t" << s->getValue() << endl;
            }

            for (ID id : runVectors) {
                const VectorResult *v = resultFileManager.getVector(id);
                out << maybeRunColumnWithTab << "vector\t" << v->getModuleName() << "\t" << v->getName() << L(vectorId) << v->getVectorId();
                const Statistics& s = v->getStatistics();
                if (s.getCount() >= 0) // information is valid, i.e. index file exists
                    out << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin()  << L(max) << s.getMax();
                out << endl;
            }

            for (ID id : runStatistics) {
                const StatisticsResult *h = resultFileManager.getStatistics(id);
                const Statistics& s = h->getStatistics();
                out << maybeRunColumnWithTab << "statistics\t" << h->getModuleName() << "\t" << h->getName() << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin() << L(max) << s.getMax() << endl;
            }

            for (ID id : runHistograms) {
                const HistogramResult *h = resultFileManager.getHistogram(id);
                const Statistics& s = h->getStatistics();
                out << maybeRunColumnWithTab << "histogram\t" << h->getModuleName() << "\t" << h->getName() << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin() << L(max) << s.getMax() << L(#bins) << h->getHistogram().getNumBins() << endl;
            }
            out << endl;
        }
        break;
    }
#undef L
    case LIST_RUNATTRS: {
        RunAndValueList filteredRunattrs = resultFileManager.getMatchingRunattrs(runs, opt_filterExpression.c_str());
        listRunMetadata(out, filteredRunattrs, [](Run *run, const std::string& name) { return run->getAttribute(name); }, opt_runDisplayMode, opt_grepFriendly);
        break;
    }
    case LIST_ITERVARS: {
        RunAndValueList filteredItervars = resultFileManager.getMatchingItervars(runs, opt_filterExpression.c_str());
        listRunMetadata(out, filteredItervars, [](Run *run, const std::string& name) { return run->getIterationVariable(name); }, opt_runDisplayMode, opt_grepFriendly);
        break;
    }
    case LIST_CONFIGENTRIES: {
        RunAndValueList filteredConfigEntries = resultFileManager.getMatchingConfigEntries(runs, opt_filterExpression.c_str());
        listRunMetadata(out, filteredConfigEntries, [](Run *run, const std::string& name) { return run->getConfigValue(name); }, opt_runDisplayMode, opt_grepFriendly);
        break;
    }
    case LIST_PARAMASSIGNMENTS: {
        RunAndValueList filteredParamAssignments = resultFileManager.getMatchingParamAssignmentConfigEntries(runs, opt_filterExpression.c_str());
        listRunMetadata(out, filteredParamAssignments, [](Run *run, const std::string& name) { return run->getConfigValue(name); }, opt_runDisplayMode, opt_grepFriendly);
        break;
    }
    case LIST_NAMES: {
        if (!opt_perRun)
            print(out, resultFileManager.getUniqueResultNames(results));
        else {
            for (Run *run : runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    out << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                print(out, resultFileManager.getUniqueResultNames(runResults), opt_grepFriendly ? runName + "\t" : "");
                out << endl;
            }
        }
        break;
    }
    case LIST_MODULES: {
        if (!opt_perRun)
            print(out, resultFileManager.getUniqueModuleNames(results));
        else {
            for (Run *run : runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    out << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                print(out, resultFileManager.getUniqueModuleNames(runResults), opt_grepFriendly ? runName + "\t" : "");
                out << endl;
            }
        }
        break;
    }
    case LIST_MODULE_AND_NAME_PAIRS: {
        if (!opt_perRun)
            print(out, resultFileManager.getUniqueModuleAndResultNamePairs(results));
        else {
            for (Run *run : runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    out << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                print(out, resultFileManager.getUniqueModuleAndResultNamePairs(runResults), opt_grepFriendly ? runName + "\t" : "");
                out << endl;
            }
        }
        break;
    }
    case LIST_RUNS: {
        RunList filteredRuns = resultFileManager.filterRunList(runs, opt_filterExpression.c_str());
        // note: we ignore opt_perRun, as it makes no sense here
        for (Run *run : filteredRuns)
            out << runStr(run, opt_runDisplayMode) << endl;
        break;
    }
    case LIST_CONFIGS: {
        // note: we ignore opt_perRun, as it makes no sense here
        StringSet uniqueConfigNames = resultFileManager.getUniqueRunAttributeValues(runs, Scave::CONFIGNAME);
        print(out, uniqueConfigNames);
        break;
    }
    default: {
        Assert(false);
    }
    } // switch

    if (&out == &buffer)
        cout << opp_format_table(buffer.str());
}

inline void pushCountIfPositive(vector<string>& v, int count, const string& noun, const string& pluralSuffix="s")
{
    if (count > 0) {
        stringstream os;
        os << count << " " << noun << (count > 1 ? pluralSuffix : "");
        v.push_back(os.str());
    }
}

inline double parseTime(const char *str)
{
    std::string actualUnit;
    double d = UnitConversion::parseQuantity(str, actualUnit);
    if (actualUnit.empty())
        return d;
    else
        return UnitConversion::convertUnit(d, actualUnit.c_str(), "s");
}

void ScaveTool::exportCommand(int argc, char **argv)
{
    vector<string> opt_fileNames;
    string opt_filterExpression = "*";
    int opt_resultTypeFilter = ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM | ResultFileManager::PARAMETER;
    bool opt_verbose = false;
    bool opt_indexingAllowed = true;
    bool opt_allowNonmatching = false;
    bool opt_includeFields = false;
    double opt_vectorStartTime = -INFINITY;
    double opt_vectorEndTime = INFINITY;
    string opt_fileName;
    string opt_exporter;
    vector<string> opt_exporterOptions;

    // parse options
    bool endOpts = false;
    for (int i = 0; i < argc; i++) {
        string opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (opt == "--")
            endOpts = true;
        else if ((opt == "-T" || opt == "--type") && i != argc-1)
            opt_resultTypeFilter = resolveResultTypeFilter(unquoteString(argv[++i]));
        else if (opt.substr(0,2) == "-T")
            opt_resultTypeFilter = resolveResultTypeFilter(opt.substr(2));
        else if ((opt == "-f" || opt == "--filter") && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (opt == "-w" || opt == "--add-fields-as-scalars")
            opt_includeFields = true;
        else if (opt == "--start-time" && i != argc-1)
            opt_vectorStartTime = parseTime(argv[++i]);
        else if (opt == "--end-time" && i != argc-1)
            opt_vectorEndTime = parseTime(argv[++i]);
        else if (opt == "-o" && i != argc-1)
            opt_fileName = argv[++i];
        else if (opt == "-F" && i != argc-1)
            opt_exporter = argv[++i];
        else if (opt == "-x" && i != argc-1)
            opt_exporterOptions.push_back(argv[++i]);
        else if (opt.substr(0,2) == "-x")
            opt_exporterOptions.push_back(opt.substr(2));
        else if (opt == "-k" || opt == "--no-indexing")
            opt_indexingAllowed = false;
        else if (opt == "--allow-nonmatching")
            opt_allowNonmatching = true;
        else if (opt == "-v" || opt == "--verbose")
            opt_verbose = true;
        else if (opt[0] == '-' && opt[1]== '-' && opt[2])
            opt_exporterOptions.push_back(opt.c_str()+2);
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    // resolve filename and exporter
    if (opt_fileName == "")
        throw opp_runtime_error("Output file name must be specified (-o option)");
    if (opt_exporter == "") {
        opt_exporter = ExporterFactory::getFormatFromOutputFileName(opt_fileName);
        if (opt_verbose && opt_exporter != "")
            cout << "inferred export format from file name: " << opt_exporter << endl;
    }
    if (opt_exporter == "")
        throw opp_runtime_error("Exporter type could not be deduced from file name, must be specified (-F option)");

    std::unique_ptr<Exporter> exporter(ExporterFactory::createExporter(opt_exporter));
    if (!exporter)
        throw opp_runtime_error("Unrecognized export format '%s' (accepted ones: %s)", opt_exporter.c_str(), opp_join(ExporterFactory::getSupportedFormats(), ", ", '\'').c_str());

    // convert exporter options from vector to map
    map<string,string> exporterOptions;
    for (string item : opt_exporterOptions) {
        if (item.find("=") == item.npos)
            throw opp_runtime_error("Invalid exporter option '%s': <option>=<value> expected", item.c_str());
        exporterOptions[opp_substringbefore(item, "=")] = opp_substringafter(item, "=");
    }

    exporter->setVectorStartTime(opt_vectorStartTime);
    exporter->setVectorEndTime(opt_vectorEndTime);

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_allowNonmatching, opt_verbose);

    // filter results
    IDList results = resultFileManager.getAllItems(opt_includeFields);
    results = results.filterByTypes(opt_resultTypeFilter);
    results = resultFileManager.filterIDList(results, opt_filterExpression.c_str());

    // check items are supported by the format
    int itemTypes = results.getItemTypes();
    int supportedTypes = ExporterFactory::getByFormat(opt_exporter)->getSupportedResultTypes();
    int unsupportedItemTypes = itemTypes & ~supportedTypes;
    if (unsupportedItemTypes != 0)
        throw opp_runtime_error("Data set contains items of type not supported by the export format, use -T option to filter");

    // export
    if (opt_verbose)
        cout << "exporting to " << opt_fileName << "... " << std::flush;
    exporter->setOptions(exporterOptions);
    exporter->saveResults(opt_fileName, &resultFileManager, results);
    if (opt_verbose)
        cout << "done\n";

    // report summary
    if (opt_fileName != "-") {
        vector<string> v;
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::SCALAR), "scalar");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::PARAMETER), "parameter");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::VECTOR), "vector");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::STATISTICS), "statistics", "");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::HISTOGRAM), "histogram");
        cout << "Exported " << (results.isEmpty() ? "empty data set" : opp_join(v, ", ")) << endl;
    }

    //TODO delete output file in case of exception?
}

void ScaveTool::indexCommand(int argc, char **argv)
{
    // process args
    bool opt_verbose = false;
    vector<string> opt_fileNames;
    for (int i = 0; i < argc; i++) {
        string opt = argv[i];
        if (opt == "-v" || opt == "--verbose")
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    VectorFileIndexer indexer;
    int count = 0;
    for (int i = 0; i < (int)opt_fileNames.size(); i++) {
        const char *fileName = opt_fileNames[i].c_str();
        if (opt_verbose)
            cout << "indexing " << fileName << "... " << std::flush;
        indexer.generateIndex(fileName);
        count++;
    }

    if (opt_verbose)
        cout << "done\n";

    cout << "Indexed " << count << " file(s)\n";
}

int ScaveTool::main(int argc, char **argv)
{
    if (argc < 2) {
        helpCommand(argc-1, argv+1);
        exit(0);
    }

    try {
        string command = argv[1];
        if (argc >= 3 && command[0] != '-' && (string(argv[2]) == "-h" || string(argv[2]) == "--help"))  // "opp_scavetool query -h"
            printHelpPage(command);
        else if (command == "q" || command == "query")
            queryCommand(argc-2, argv+2);
        else if (command == "x" || command == "export")
            exportCommand(argc-2, argv+2);
        else if (command == "i" || command == "index")
            indexCommand(argc-2, argv+2);
        else if (command == "h" || command == "help" || command == "-h" || command == "--help")
            helpCommand(argc-2, argv+2);
        else if (command[0] == '-' || isFile(command.c_str())) // use default command
            queryCommand(argc-1, argv+1);
        else
            throw opp_runtime_error("Unknown command or file name '%s'", command.c_str());
    }
    catch (exception& e) {
        cerr << "opp_scavetool: " << e.what() << endl;
        return 1;
    }
    return 0;
}

}  // namespace scave
}  // namespace omnetpp

using namespace omnetpp::scave;

int main(int argc, char **argv)
{
    ScaveTool scaveTool;
    return scaveTool.main(argc, argv);
}

