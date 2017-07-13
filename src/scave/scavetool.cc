//=========================================================================
//  SCAVETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include <iomanip>
#include "common/ver.h"
#include "common/fileutil.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/formattedprinter.h"
#include "common/stlutil.h"
#include "resultfilemanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
#include "indexfile.h"
#include "vectorfilereader.h"
#include "vectorfilewriter.h"
#include "vectorfileindexer.h"
#include "filternodes.h"
#include "filewriter.h"
#include "arraybuilder.h"
#include "export.h"
#include "fields.h"
#include "scaveutils.h"
#include "sqliteresultfileutils.h"
#include "exporter.h"

#include "scavetool.h"

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
        help.line("scavetool -- part of " OMNETPP_PRODUCT ", (C) 2006-2017 OpenSim Ltd.");
        help.line("Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION);
        help.line();
        help.para("Usage: scavetool <command> [<options>] <files>...");
        help.para("For processing result files written by simulations: vector files (.vec) and scalar files (.sca).");
        help.line("Commands:");
        help.option("q, query", "Query the contents of result files");
        help.option("x, export", "Export results in various formats");
        help.option("i, index", "Generate index files (.vci) for vector files");
        help.option("h, help", "Print this help text");
        help.line();
        help.para("There are two additional commands, 'vector' and 'scalar', which have been deprecated. The replacement for both is 'export'.");
        help.para("The default command is 'query'.");
        help.para("To get help, use scavetool help <topic>. Available help topics: any command name, 'filter', and 'operations'.");
    }
    else if (page == "q" || page == "query") {
        help.line("Usage: scavetool query [<mode-option>] [<options>] <output-vector-and-scalar-files>");
        help.para("Query the contents of result files.");
        help.line("Mode options (mutually exclusive):");
        help.option("-s, --print-summary", "Report the number of result items. (This is the default.)");
        help.option("-l, --list-results", "List result items");
        help.option("-a, --list-runattrs", "List run attributes");
        help.option("-n, --list-names", "List unique result (vector, scalar, etc) names");
        help.option("-m, --list-modules", "List unique module names");
        help.option("-e  --list-qnames", "List unique result names qualified with the module names they occur with");
        help.option("-r, --list-runs", "List unique runs");
        help.option("-c, --list-configs", "List unique configuration names");
        help.line();
        help.line("Options:");
        help.option("-T, --type <types>", "Limit item types; <types> is concatenation of type characters (v=vector, s=scalar, t=statistic, h=histogram).");
        help.option("-f, --filter <filter>", "Filter for result items (vectors, scalars, statistics, histograms) matched by filter expression (try 'help filter')");
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
        help.option("-v, --verbose", "Print info about progress (verbose)");
        help.line();
        help.para("See also the following help topics: 'filter'");
    }
    else if (page == "x" || page == "export") {
        help.para("Usage: scavetool export [<options>] <output-vector-and-scalar-files>");
        help.para("Export results in various formats.");
        help.line("Options:");
        help.option("-T, --type <types>", "Limit item types; <types> is concatenation of type characters (v=vector, s=scalar, t=statistic, h=histogram).");
        help.option("-f, --filter <filter>", "Filter for result items (vectors, scalars, statistics, histograms) matched by filter expression (try 'help filter')");
        help.option("-w, --add-fields-as-scalars", "Add statistics fields (count, sum, mean, stddev, min, max, etc) as scalars");
        help.option("-o <filename>", "Output file name, or '-' for the standard output. This option is mandatory.");
        help.option("-F <format>", "Selects the exporter. The exporter's operation may further be customized via -x options.");
        help.option("-x <key>=<value>", "Option for the exporter. This option may occur multiple times.");
        help.option("-k, --no-indexing", "Disallow automatic indexing of vector files");
        help.option("-v, --verbose", "Print info about progress (verbose)");
        help.line();
        help.para("Supported export formats: " + opp_join(ExporterFactory::getSupportedFormats(), ", ", '\''));
        help.para("See the 'exporters' help topic for the list of available options for each export format.");
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
    else if (page == "v" || page == "vector") {
        help.para("Usage: scavetool vector [<options>] <output-vector-files>");
        help.para("IMPORTANT: THIS COMMAND HAS BEEN DEPRECATED, USE export INSTEAD!");
        help.para("Export vector data.");
        help.line("Options:");
        help.option("-p <filter>", "Filter for vectors matched by the filter expression");
        help.option("-a <operation>", "Apply the given processing to the selected vectors. Try 'help operations' for the list of available operations. This option may occur multiple times.");
        help.option("-O <filename>", "Output file name");
        help.option("-F <formatname>", "Format of output file: vec (default), splitvec, matlab, octave, csv, splitcsv");
        help.option("-X", "Disallow automatic indexing of vector files");
        help.option("-V", "Print info about progress (verbose)");
        help.line();
        help.line("Example:\n");
        help.indentPara("scavetool vector -p \"queueing time\" -a winavg(10) -O out.vec *.vec");
        help.indentPara("Saves the vectors named 'queueing time' into out.vec, after applying the window average filter.");
        help.para("See also the following help topics: 'filter', 'operations'");
    }
    else if (page == "s" || page == "scalar") {
        help.para("Usage: scavetool scalar [<options>] <output-scalar-files>");
        help.para("IMPORTANT: THIS COMMAND HAS BEEN DEPRECATED, USE export INSTEAD!");
        help.para("Export scalar data.");
        help.line("Options:");
        help.option("-p <filter>", "Filter for scalars matched by filter expression (try 'help filter')");
        help.option("-a <operation>", "Apply the given processing to the selected scalars (try 'help operations'); the 'info' command prints the names of available operations. This option may occur multiple times.");
        help.option("-g <grouping>", "Specifies how the scalars are grouped. It is a comma separated list of field names ('file','run','module','name'). Scalars are grouped by the values of these fields and written into separate columns (csv) or variables. Default is '' (empty list), meaning that each row contains only one scalar value.");
        help.option("-O <filename>", "Output file name");
        help.option("-F <formatname>", "Format of output file: csv (default), matlab, octave");
        help.option("-X", "Disallow automatic indexing of (potentially specified) vector files");
        help.option("-V", "Print info about progress (verbose)");
        help.line();
        help.line("Example:");
        help.indentPara("scavetool scalar -p \"module(sink) OR module(queue)\" -a \"scatter(.,load,queue,\\\"queue capacity\\\")\" -O out.csv -F csv *.sca\n");
        help.indentPara("Creates a data table for a scatter plot with the 'load' attribute on the X axis and queue capacity' for iso lines, and saves in CSV format.)");
        help.para("See also the following help topics: 'filter', 'operations'\n");
    }
    else if (page == "i" || page == "index") {
        help.para("Usage: scavetool index [<options>] <output-vector-files>");
        help.para("Generate index files (.vci) for vector files. Note that this command is usually not needed, as other scavetool commands automatically create indices for loaded vector files if they are missing or out of date, unless indexing is explicitly disabled.");
        help.line("Options:");
        help.option("-r, --rebuild", "Rebuild vector files (rearrange their content into blocks) in addition to indexing them");
        help.option("-v, --verbose", "Print info about progress (verbose)");
        help.line();
    }
    else if (page == "filter") {
        help.para("Several commands have a -f <filter> option that accepts a match expression for filtering result items. This page describes the syntax available for match expressions.");
        help.para("A match expression consist of '<pattern>' and '<fieldname>(<pattern>)' elements, combined with AND, OR, and NOT operators. A plain '<pattern>' is equivalent to 'name(<pattern>)'.");
        help.line("<fieldname> is one of:");
        help.para(
                "    file              Full path of the result file\n"
                "    run               Unique run Id\n"
                "    module            Module full path\n"
                "    name              Name of the result item (vector, scalar, etc)\n"
                "    attr:<runattr>    The value of a run attribute, e.g. of attr:configname");
        help.para("<runattr> may be the name of a standard run attribute ('runid', 'inifile', 'configname', 'runnumber', 'network', 'experiment', 'measurement', 'replication', 'iterationvarsf').");
        help.para("Inifile iteration variables (e.g. 'numHosts') can also be used for <runattr>, as they are also saved as run attributes.");
        help.para("<pattern> is a glob-like pattern:");
        help.para(
                "    ?             Matches any character except '.'\n"
                "    *             Matches zero or more characters, except '.'\n"
                "    **            Matches zero or more characters (any character)\n"
                "    {a-z}         Matches a character in range a-z\n"
                "    {^a-z}        Matches a character NOT in range a-z\n"
                "    {250..300}    Matches an embedded integer number in the given range\n"
                "    [250..300]    Matches an integer number in the given range in square brackets\n"
                "    \\ (backslash) Takes away the special meaning of the subsequent character");
        help.line("Match expression example:");
        help.indentPara("module(\"**.sink\") AND (name(\"queueing time\") OR name(\"transmission time\"))");
    }
    else if (page == "operations") {
        help.para("Scalar operations:");
        help.para("scatter(module, scalar, ...)"); // currently this is the only scalar operation
        help.indentPara("Create scatter plot dataset. The first two arguments identify the scalar selected for the X axis. Additional arguments identify the iso attributes; they are (module, scalar) pairs, or names of run attributes.");
        help.para("Vector operations:");
        NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
        NodeTypeVector nodeTypes = registry->getNodeTypes();
        for (int i = 0; i < (int)nodeTypes.size(); i++) {
            NodeType *nodeType = nodeTypes[i];
            if (nodeType->isHidden())
                continue;

            // query parameters
            StringMap attrs, attrDefaults;
            nodeType->getAttributes(attrs);
            nodeType->getAttrDefaults(attrDefaults);

            help.para((string)nodeType->getName() + "(" + opp_join(keys(attrs), ", ") + ")");
            help.indentPara(nodeType->getDescription());
            for (auto pair : attrs)
                help.option(pair.first, pair.second);
            if (!attrs.empty())
                help.line();
        }
    }
    else {
        throw opp_runtime_error("No help topic '%s'", page.c_str());
    }
}

void ScaveTool::loadFiles(ResultFileManager& manager, const vector<string>& fileNames, bool indexingAllowed, bool verbose)
{
    if (fileNames.empty()) {
        cerr << "scavetool: Warning: No input files\n";
        return;
    }

    // load files
    for (int i = 0; i < (int)fileNames.size(); i++) {
        // TODO on Windows: manual globbing of wildcards
        const char *fileName = fileNames[i].c_str();

        if (indexingAllowed && IndexFile::isExistingVectorFile(fileName) && !IndexFile::isIndexFileUpToDate(fileName)) {
            if (verbose)
                cout << "index file for " << fileName << " is missing or out of date, indexing...\n";
            VectorFileIndexer().generateIndex(fileName, nullptr);
            if (verbose)
                cout << "done\n";
        }

        if (verbose)
            cout << "reading " << fileName << "...\n";
        manager.loadFile(fileName);
        if (verbose)
            cout << "done\n";
    }
    if (verbose)
        cout << manager.getFiles().size() << " file(s) loaded\n";
}

int ScaveTool::resolveResultTypeFilter(const std::string& filter)
{
    int result = 0;
    if (filter.length() <= 3) {
        // short form
        for (char c : filter) {
            switch(c) {
            case 's': result |= ResultFileManager::SCALAR; break;
            case 'v': result |= ResultFileManager::VECTOR; break;
            case 't': result |= ResultFileManager::STATISTICS; break;
            case 'h': result |= ResultFileManager::HISTOGRAM; break;
            default: throw opp_runtime_error("Invalid result type '%c' in '-T' option", c);
            }
        }
    }
    else {
        // long form
        for (string token : StringTokenizer(filter.c_str(), ",").asVector()) {
            if (token == "scalars")
                result |= ResultFileManager::SCALAR;
            else if (token == "vectors")
                result |= ResultFileManager::VECTOR;
            else if (token == "statistics")
                result |= ResultFileManager::STATISTICS;
            else if (token == "histograms")
                result |= ResultFileManager::HISTOGRAM;
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

static void printAndDelete(std::ostream& out, StringSet *strings, const string& prefix = "")
{
    if (strings != nullptr) {
        for (auto str : *strings)
            out << prefix << str << "\n";
        delete strings;
    }
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

void ScaveTool::queryCommand(int argc, char **argv)
{
    enum QueryMode {
        PRINT_SUMMARY, LIST_RESULTS, LIST_RUNATTRS, LIST_MODULES, LIST_NAMES,
        LIST_MODULE_AND_NAME_PAIRS, LIST_RUNS, LIST_CONFIGS
    };

    QueryMode opt_mode = PRINT_SUMMARY;
    vector<string> opt_fileNames;
    string opt_filterExpression;
    string opt_resultTypeFilterStr;
    string opt_runDisplayModeStr;
    int opt_resultTypeFilter = ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;
    RunDisplayMode opt_runDisplayMode = RUNDISPLAY_RUNID;
    bool opt_includeFields = false;
    bool opt_bare = false;
    bool opt_perRun = false;
    bool opt_grepFriendly = false;
    bool opt_useTabs = false;
    bool opt_verbose = false;
    bool opt_indexingAllowed = true;

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
            opt_resultTypeFilterStr = unquoteString(argv[++i]);
        else if (opt.substr(0,2) == "-T")
            opt_resultTypeFilterStr = opt.substr(2);
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
        else if (opt == "-v" || opt == "--verbose")
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    // resolve -T, filter by result type
    if (opt_resultTypeFilterStr != "")
        opt_resultTypeFilter = resolveResultTypeFilter(opt_resultTypeFilterStr);

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
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_verbose);

    // filter statistics
    IDList results = resultFileManager.getAllItems(true, opt_includeFields);
    results.set(results.filterByTypes(opt_resultTypeFilter));
    results.set(resultFileManager.filterIDList(results, opt_filterExpression.c_str()));

    RunList *runs = resultFileManager.getUniqueRuns(results);
    std::sort(runs->begin(), runs->end(), [](Run *a, Run *b)  {return a->getRunName() < b->getRunName();}); // sort runs by runId, for consistent output
    IDList scalars = results.filterByTypes(ResultFileManager::SCALAR);
    IDList vectors = results.filterByTypes(ResultFileManager::VECTOR);
    IDList statistics = results.filterByTypes(ResultFileManager::STATISTICS);
    IDList histograms = results.filterByTypes(ResultFileManager::HISTOGRAM);

    stringstream buffer;
    ostream& out = opt_useTabs ? cout : buffer;

    switch (opt_mode) {
    case PRINT_SUMMARY: {
#define L(label) (opt_bare ? "\t" : label)
        if (!opt_perRun) {
            out << L("runs: ") << runs->size() << " ";
            if ((opt_resultTypeFilter & ResultFileManager::SCALAR) != 0)
                out << L("\tscalars: ") << scalars.size();
            if ((opt_resultTypeFilter & ResultFileManager::VECTOR) != 0)
                out << L("\tvectors: ") << vectors.size();
            if ((opt_resultTypeFilter & ResultFileManager::STATISTICS) != 0)
                out << L("\tstatistics: ") << statistics.size();
            if ((opt_resultTypeFilter & ResultFileManager::HISTOGRAM) != 0)
                out << L("\thistograms: ") << histograms.size();
            out << endl;
        }
        else {
            for (Run *run : *runs) {
                out << runStr(run, opt_runDisplayMode);
                if ((opt_resultTypeFilter & ResultFileManager::SCALAR) != 0) {
                    IDList runScalars = resultFileManager.filterIDList(scalars, run, nullptr, nullptr);
                    out << L("\tscalars: ") << runScalars.size();
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
        for (Run *run : *runs) {
            string runName = runStr(run, opt_runDisplayMode);
            string maybeRunColumnWithTab = opt_grepFriendly ? runName + "\t" : "";
            if (!opt_grepFriendly)
                out << runName << ":" << endl << endl;
#define L(label) (opt_bare ? "\t" : "\t" #label "=")
            IDList runScalars = resultFileManager.filterIDList(scalars, run, nullptr, nullptr);
            IDList runVectors = resultFileManager.filterIDList(vectors, run, nullptr, nullptr);
            IDList runStatistics = resultFileManager.filterIDList(statistics, run, nullptr, nullptr);
            IDList runHistograms = resultFileManager.filterIDList(histograms, run, nullptr, nullptr);

            for (int i = 0; i < runScalars.size(); i++) {
                const ScalarResult& s = resultFileManager.getScalar(runScalars.get(i));
                out << maybeRunColumnWithTab << "scalar\t" << s.getModuleName() << "\t" << s.getName() << "\t" << s.getValue() << endl;
            }

            for (int i = 0; i < runVectors.size(); i++) {
                const VectorResult& v = resultFileManager.getVector(runVectors.get(i));
                out << maybeRunColumnWithTab << "vector\t" << v.getModuleName() << "\t" << v.getName() << L(vectorId) << v.getVectorId();
                const Statistics& s = v.getStatistics();
                if (s.getCount() >= 0) // information is valid, i.e. index file exists
                    out << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin()  << L(max) << s.getMax();
                out << endl;
            }

            for (int i = 0; i < runStatistics.size(); i++) {
                const StatisticsResult& h = resultFileManager.getStatistics(runStatistics.get(i));
                const Statistics& s = h.getStatistics();
                out << maybeRunColumnWithTab << "statistics\t" << h.getModuleName() << "\t" << h.getName() << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin() << L(max) << s.getMax() << endl;
            }

            for (int i = 0; i < runHistograms.size(); i++) {
                const HistogramResult& h = resultFileManager.getHistogram(runHistograms.get(i));
                const Statistics& s = h.getStatistics();
                out << maybeRunColumnWithTab << "histogram\t" << h.getModuleName() << "\t" << h.getName() << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin() << L(max) << s.getMax() << L(#bins) << h.getBinValues().size() << endl;
            }
            out << endl;
        }
        break;
    }
#undef L
    case LIST_RUNATTRS: {
        // note: we ignore opt_perRun, as it makes no sense here
        for (Run *run : *runs) {
            string runName = runStr(run, opt_runDisplayMode);
            string maybeRunColumnWithTab = opt_grepFriendly ? runName + "\t" : "";
            if (!opt_grepFriendly)
                out << runName << ":" << endl << endl;
            for (auto& runAttr : run->getAttributes())
                out << maybeRunColumnWithTab << runAttr.first << "\t" << runAttr.second << std::endl;
            out << endl;
        }
        break;
    }
    case LIST_NAMES: {
        if (!opt_perRun)
            printAndDelete(out, resultFileManager.getUniqueNames(results));
        else {
            for (Run *run : *runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    out << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                printAndDelete(out, resultFileManager.getUniqueNames(runResults), opt_grepFriendly ? runName + "\t" : "");
                out << endl;
            }
        }
        break;
    }
    case LIST_MODULES: {
        if (!opt_perRun)
            printAndDelete(out, resultFileManager.getUniqueModuleNames(results));
        else {
            for (Run *run : *runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    out << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                printAndDelete(out, resultFileManager.getUniqueModuleNames(runResults), opt_grepFriendly ? runName + "\t" : "");
                out << endl;
            }
        }
        break;
    }
    case LIST_MODULE_AND_NAME_PAIRS: {
        if (!opt_perRun)
            printAndDelete(out, resultFileManager.getUniqueModuleAndResultNamePairs(results));
        else {
            for (Run *run : *runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    out << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                printAndDelete(out, resultFileManager.getUniqueModuleAndResultNamePairs(runResults), opt_grepFriendly ? runName + "\t" : "");
                out << endl;
            }
        }
        break;
    }
    case LIST_RUNS: {
        // note: we ignore opt_perRun, as it makes no sense here
        for (Run *run : *runs)
            out << runStr(run, opt_runDisplayMode) << endl;
        break;
    }
    case LIST_CONFIGS: {
        // note: we ignore opt_perRun, as it makes no sense here
        StringSet *uniqueConfigNames = resultFileManager.getUniqueRunAttributeValues(*runs, RunAttribute::CONFIGNAME);
        printAndDelete(out, uniqueConfigNames);
        break;
    }
    default: {
        Assert(false);
    }
    } // switch

    if (&out == &buffer)
        cout << opp_formatTable(buffer.str());

    delete runs;
}

inline void pushCountIfPositive(vector<string>& v, int count, const string& noun, const string& pluralSuffix="s")
{
    if (count > 0) {
        stringstream os;
        os << count << " " << noun << (count > 1 ? pluralSuffix : "");
        v.push_back(os.str());
    }
}

void ScaveTool::exportCommand(int argc, char **argv)
{
    vector<string> opt_fileNames;
    string opt_filterExpression;
    string opt_resultTypeFilterStr;
    int opt_resultTypeFilter = ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;
    bool opt_verbose = false;
    bool opt_indexingAllowed = true;
    bool opt_includeFields = false;
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
            opt_resultTypeFilterStr = unquoteString(argv[++i]);
        else if (opt.substr(0,2) == "-T")
            opt_resultTypeFilterStr = opt.substr(2);
        else if ((opt == "-f" || opt == "--filter") && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (opt == "-w" || opt == "--add-fields-as-scalars")
            opt_includeFields = true;
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
        else if (opt == "-v" || opt == "--verbose")
            opt_verbose = true;
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
    Exporter *exporter = ExporterFactory::createExporter(opt_exporter);
    if (!exporter)
        throw opp_runtime_error("Unrecognized export format '%s'", opt_exporter.c_str());

    // convert exporter options from vector to map
    map<string,string> exporterOptions;
    for (string item : opt_exporterOptions) {
        if (item.find("=") == item.npos)
            throw opp_runtime_error("Invalid exporter option '%s': <option>=<value> expected", item.c_str());
        exporterOptions[opp_substringbefore(item, "=")] = opp_substringafter(item, "=");
    }

    // resolve -T, filter by result type
    if (opt_resultTypeFilterStr != "")
        opt_resultTypeFilter = resolveResultTypeFilter(opt_resultTypeFilterStr);

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_verbose);

    // filter statistics
    IDList results = resultFileManager.getAllItems(true, opt_includeFields);
    results.set(results.filterByTypes(opt_resultTypeFilter));
    results.set(resultFileManager.filterIDList(results, opt_filterExpression.c_str()));

    // check items are supported by the format
    int itemTypes = results.getItemTypes();
    int supportedTypes = ExporterFactory::getByFormat(opt_exporter)->getSupportedResultTypes();
    int unsupportedItemTypes = itemTypes & ~supportedTypes;
    if (unsupportedItemTypes != 0)
        throw opp_runtime_error("Data set contains items of type not supported by the export format, use -T option to filter");

    // export
    exporter->setOptions(exporterOptions);
    exporter->saveResults(opt_fileName, &resultFileManager, results);

    // report summary
    if (opt_fileName != "-") {
        vector<string> v;
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::SCALAR), "scalar");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::VECTOR), "vector");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::STATISTICS), "statistics", "");
        pushCountIfPositive(v, results.countByTypes(ResultFileManager::HISTOGRAM), "histogram");
        cout << "Exported " << (results.isEmpty() ? "empty data set" : opp_join(v, ", ")) << endl;
    }

    delete exporter;
}

void ScaveTool::vectorCommand(int argc, char **argv)
{
    // issue warning
    cerr << "scavetool: Warning: deprecated command 'vector', use 'export' instead" << endl;

    // options
    string opt_filterExpression;
    string opt_outputFileName = "out";
    string opt_outputFormat = "vec";
    string opt_readerNodeType = "vectorreaderbyfiletype";
    vector<string> opt_filterList;
    vector<string> opt_fileNames;
    bool opt_verbose = false;
    bool opt_indexingAllowed = true;

    // parse options
    bool endOpts = false;
    for (int i = 0; i < argc; i++) {
        string opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (opt == "--")
            endOpts = true;
        else if (opt == "-p" && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (opt == "-a" && i != argc-1)
            opt_filterList.push_back(argv[++i]);
        else if (opt == "-O" && i != argc-1)
            opt_outputFileName = argv[++i];
        else if (opt == "-F" && i != argc-1)
            opt_outputFormat = argv[++i];
        else if (opt == "-X")
            opt_indexingAllowed = false;
        else if (opt == "-V")
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else if (opt == "-r" && i != argc-1)  // for testing only
            opt_readerNodeType = argv[++i];
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    bool opt_writeSeparateFiles = false;
    if (opt_outputFormat.find("split") == 0) {
        opt_outputFormat = opt_outputFormat.substr(5);
        opt_writeSeparateFiles = true;
    }

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_verbose);

    // filter statistics
    IDList vectorIDList = resultFileManager.filterIDList(resultFileManager.getAllVectors(), opt_filterExpression.c_str());

    if (opt_verbose)
        cout << "filter expression matches " << vectorIDList.size() << " vectors\n";
    if (opt_verbose)
        cout << "done collecting inputs\n\n";

    // add index to the SQLite vector files if not present
    for (std::string filename : opt_fileNames) {
        if (SqliteResultFileUtils::isSqliteFile(filename.c_str())) {
            if (opt_verbose)
                cout << "adding vectordata index to SQLite file '" << filename << "' if not yet added\n";
            SqliteResultFileUtils::addIndexToVectorData(filename.c_str());
            if (opt_verbose)
                cout << "done\n\n";
        }
    }

    //
    // assemble dataflow network for vectors
    //
    DataflowManager dataflowManager;
    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();

    // create filereader for each vector file
    if (opt_verbose)
        cout << "creating vector file reader(s)\n";
    NodeType *readerNodeType = registry->getNodeType(opt_readerNodeType.c_str());
    if (!readerNodeType)
        throw opp_runtime_error("Unknown node type '%s'", opt_readerNodeType.c_str());
    ResultFileList *filteredVectorFileList = resultFileManager.getUniqueFiles(vectorIDList);
    map<ResultFile *, Node *> vectorFileReaders;
    StringMap attrs;
    for (int i = 0; i < (int)filteredVectorFileList->size(); i++) {
        ResultFile *resultFile = filteredVectorFileList->at(i);
        attrs["filename"] = resultFile->getFileSystemFilePath();
        attrs["allowindexing"] = "true";
        Node *readerNode = readerNodeType->create(&dataflowManager, attrs);
        vectorFileReaders[resultFile] = readerNode;
    }
    delete filteredVectorFileList;

    // create writer node, if each vector is written into the same file
    VectorFileWriterNode *vectorFileWriterNode = nullptr;

    vector<ArrayBuilderNode *> arrayBuilders;  // for exporting

    for (int i = 0; i < vectorIDList.size(); i++) {
        // create a port for each vector on its reader node
        char portName[30];
        const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
        Assert(vectorFileReaders.find(vector.getFile()) != vectorFileReaders.end());
        sprintf(portName, "%d", vector.getVectorId());
        Node *readerNode = vectorFileReaders[vector.getFile()];
        Port *outPort = readerNodeType->getPort(readerNode, portName);

        // add filters
        for (int k = 0; k < (int)opt_filterList.size(); k++) {
            // TODO support filter to merge all into a single vector
            if (opt_verbose)
                cout << "adding filter to vector: " << opt_filterList[k] << "\n";
            Node *node = registry->createNode(opt_filterList[k].c_str(), &dataflowManager);
            FilterNode *filterNode = dynamic_cast<FilterNode *>(node);
            if (!filterNode)
                throw opp_runtime_error("'%s' is not a filter node", opt_filterList[k].c_str());
            dataflowManager.connect(outPort, &(filterNode->in));
            outPort = &(filterNode->out);
        }

        // create writer node(s) and connect them
        if (opt_outputFormat == "vec") {
            if (opt_writeSeparateFiles) {
                // every vector goes to its own file, with two columns (time+value) separated by spaces/tab
                if (opt_verbose)
                    cout << "adding separate writers for each vector\n";
                char buf[16];
                sprintf(buf, "%d", i);
                string fname = opt_outputFileName+buf+".vec";

                stringstream header;
                header << "# vector " << vector.getVectorId() << " "
                       << QUOTE(vector.getModuleName().c_str()) << " "
                       << QUOTE(vector.getName().c_str()) << "\n";
                header << "# file generated by scavetool\n";

                FileWriterNode *writerNode = new FileWriterNode(fname.c_str(), header.str().c_str());
                dataflowManager.addNode(writerNode);
                dataflowManager.connect(outPort, &(writerNode->in));
            }
            else {
                // everything goes to a common vector file
                if (!vectorFileWriterNode) {
                    if (opt_verbose)
                        cout << "adding vector file writer\n";
                    string fileName = opt_outputFileName + ".vec";
                    vectorFileWriterNode = new VectorFileWriterNode(fileName.c_str(), "# generated by scavetool");
                    dataflowManager.addNode(vectorFileWriterNode);
                }

                Port *writerNodePort = vectorFileWriterNode->addVector(vector);  // FIXME: renumber vectors
                dataflowManager.connect(outPort, writerNodePort);
            }
        }
        else {
            // for other formats, we must build arrays
            if (opt_verbose)
                cout << "adding array builders\n";
            ArrayBuilderNode *arrayBuilderNode = new ArrayBuilderNode();
            dataflowManager.addNode(arrayBuilderNode);
            dataflowManager.connect(outPort, &(arrayBuilderNode->in));
            arrayBuilders.push_back(arrayBuilderNode);
        }
    }

    // run!
    if (opt_verbose)
        cout << "running dataflow network...\n";
    dataflowManager.execute();

    if (opt_outputFormat != "vec") {
        OldScaveExport *exporter = OldExporterFactory::createExporter(opt_outputFormat);
        if (!exporter)
            throw opp_runtime_error("Unknown output file format '%s'", opt_outputFormat.c_str());

        try {
            exporter->setBaseFileName(opt_outputFileName);
            if (opt_writeSeparateFiles) {
                // separate vectors
                for (int i = 0; i < vectorIDList.size(); i++) {
                    ID vectorID = vectorIDList.get(i);
                    bool computed = !opt_filterList.empty();
                    const VectorResult& vector = resultFileManager.getVector(vectorID);
                    string name = vector.getName();
                    string descr = vector.getName() + "; "
                            + vector.getModuleName() + "; "
                            + vector.getFile()->getFileSystemFilePath() + "; "
                            + vector.getRun()->getRunName();
                    XYArray *xyArray = arrayBuilders[i]->getArray();
                    exporter->saveVector(name, descr, vectorID, computed, xyArray, resultFileManager);
                    delete xyArray;
                }
            }
            else {  // same file
                if (vectorIDList.size() > 0) {
                    // all vectors in one file
                    vector<XYArray *> xyArrays;
                    for (int i = 0; i < vectorIDList.size(); i++)
                        xyArrays.push_back(arrayBuilders[i]->getArray());

                    string desc = "generated by '" + rebuildCommandLine(argc, argv) + "'";
                    exporter->saveVectors("vectors", desc, vectorIDList, xyArrays, resultFileManager);

                    for (int i = 0; i < vectorIDList.size(); i++)
                        delete arrayBuilders[i]->getArray();
                }
            }

            delete exporter;
        }
        catch (exception&) {
            delete exporter;
            throw;
        }
    }

    if (opt_verbose)
        cout << "done\n";
}

void ScaveTool::parseScalarFunction(const string& functionCall,  /*out*/ string& name,  /*out*/ vector<string>& params)
{
    params.clear();
    string::size_type paren = functionCall.find('(');
    if (paren == string::npos) {
        // no left paren -- treat the whole string as function name
        name = functionCall;
        return;
    }

    // check that string ends in right paren
    string::size_type size = functionCall.length();
    if (functionCall[size-1] != ')')
        throw opp_runtime_error("Syntax error in filter spec '%s'", functionCall.c_str());

    // filter name is the part before the left paren
    name.assign(functionCall, 0, paren);

    // param list is the part between the parens -- split it up along commas
    string paramlist(functionCall, paren+1, size-paren-2);
    LineTokenizer tokenizer(paramlist.length()+1, 100, ',', ',');
    tokenizer.tokenize(paramlist.c_str(), paramlist.length());
    char **tokens = tokenizer.tokens();
    for (int i = 0; i < tokenizer.numTokens(); ++i)
        params.push_back(unquoteString(tokens[i]));
}

void ScaveTool::scalarCommand(int argc, char **argv)
{
    // issue warning
    cerr << "scavetool: Warning: deprecated command 'scalar', use 'export' instead" << endl;

    // options
    string opt_filterExpression;
    string opt_outputFileName = "out";
    string opt_outputFormat = "csv";
    string opt_applyFunction;
    string opt_groupingFields = "";
    vector<string> opt_fileNames;
    bool opt_verbose = false;
    bool opt_indexingAllowed = true;

    // parse options
    bool endOpts = false;
    for (int i = 0; i < argc; i++) {
        string opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (opt == "--")
            endOpts = true;
        else if (opt == "-p" && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (opt == "-a" && i != argc-1)
            opt_applyFunction = unquoteString(argv[++i]);
        else if (opt == "-g" && i != argc-1)
            opt_groupingFields = unquoteString(argv[++i]);
        else if (opt == "-O" && i != argc-1)
            opt_outputFileName = argv[++i];
        else if (opt == "-F" && i != argc-1)
            opt_outputFormat = argv[++i];
        else if (opt == "-X")
            opt_indexingAllowed = false;
        else if (opt == "-V")
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_verbose);

    // filter scalars
    IDList scalarIDList = resultFileManager.filterIDList(resultFileManager.getAllScalars(), opt_filterExpression.c_str());
    if (opt_verbose)
        cout << "filter expression matches " << scalarIDList.size() << " scalars" << endl;
    if (opt_verbose)
        cout << "done collecting inputs" << endl << endl;

    if (!scalarIDList.isEmpty()) {
        OldScaveExport *exporter = OldExporterFactory::createExporter(opt_outputFormat);
        if (!exporter)
            throw opp_runtime_error("Unknown output file format '%s'", opt_outputFormat.c_str());
        try {
            exporter->setBaseFileName(opt_outputFileName);
            string desc = "generated by '" + rebuildCommandLine(argc, argv) + "'";

            if (opt_applyFunction.empty()) {
                StringTokenizer tokenizer(opt_groupingFields.c_str(), ", \t");
                StringVector fieldNames = tokenizer.asVector();
                ResultItemFields fields(fieldNames);
                exporter->saveScalars("scalars", desc, scalarIDList,
                        fields.complement(), resultFileManager);
            }
            else {
                string function;
                vector<string> params;
                parseScalarFunction(opt_applyFunction, function, params);
                if (function == "scatter") {
                    if (params.size() < 2)
                        throw opp_runtime_error("Missing parameters in '%s'", opt_applyFunction.c_str());

                    string moduleName = params[0];
                    string scalarName = params[1];
                    vector<string> rowFields;
                    rowFields.push_back(ResultItemField::MODULE);
                    rowFields.push_back(ResultItemField::NAME);
                    vector<string> isoModuleNames;
                    vector<string> isoScalarNames;
                    vector<string> isoRunAttributes;
                    for (vector<string>::iterator param = params.begin()+2; param != params.end(); ++param) {
                        if (RunAttribute::isAttributeName(*param)) {
                            isoRunAttributes.push_back(*param);
                        }
                        else {
                            if ((param+1) == params.end())
                                throw opp_runtime_error("Missing scalar name after '%s'", param->c_str());
                            isoModuleNames.push_back(*param);
                            isoScalarNames.push_back(*++param);
                        }
                    }

                    exporter->saveScalars("scalars", desc, scalarIDList,
                            moduleName, scalarName, ResultItemFields(rowFields).complement(),
                            isoModuleNames, isoScalarNames, ResultItemFields(isoRunAttributes),
                            resultFileManager);
                }
                else {
                    throw opp_runtime_error("Unknown scalar function '%s'", function.c_str());
                }
            }

            delete exporter;
        }
        catch (exception&) {
            delete exporter;
            throw;
        }
    }

    if (opt_verbose)
        cout << "done" << endl;
}

void ScaveTool::indexCommand(int argc, char **argv)
{
    // process args
    bool opt_verbose = false;
    bool opt_rebuild = false;
    vector<string> opt_fileNames;
    for (int i = 0; i < argc; i++) {
        string opt = argv[i];
        if (opt == "-v" || opt == "--verbose")
            opt_verbose = true;
        else if (opt == "-r" || opt == "--rebuild")
            opt_rebuild = true;
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
            cout << "indexing " << fileName << "...\n";
        if (opt_rebuild)
            indexer.rebuildVectorFile(fileName);
        else
            indexer.generateIndex(fileName);
        count++;
    }

    if (opt_verbose)
        cout << "done\n";

    cout << (opt_rebuild ? "Rebuilt and indexed " : "Indexed ") << count << " file(s)\n";
}

int ScaveTool::main(int argc, char **argv)
{
    if (argc < 2) {
        helpCommand(argc-1, argv+1);
        exit(0);
    }

    try {
        string command = argv[1];
        if (argc >= 3 && command[0] != '-' && (string(argv[2]) == "-h" || string(argv[2]) == "--help"))  // "scavetool query -h"
            printHelpPage(command);
        else if (command == "q" || command == "query")
            queryCommand(argc-2, argv+2);
        else if (command == "x" || command == "export")
            exportCommand(argc-2, argv+2);
        else if (command == "v" || command == "vector")  // deprecated command
            vectorCommand(argc-2, argv+2);
        else if (command == "s" || command == "scalar")  // deprecated command
            scalarCommand(argc-2, argv+2);
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
        cerr << "scavetool: " << e.what() << endl;
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

