//=========================================================================
//  SCAVETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "common/ver.h"
#include "common/fileutil.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
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

#include "scavetool.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

//TODO change "histogram" to "statistic"
//TODO support exporting in sca files

void ScaveTool::helpCommand(int argc, char **argv)
{
    string page = argc==0 ? "options" : argv[0];
    printHelpPage(page);
}

void ScaveTool::printHelpPage(std::string& page)
{
    if (page == "options") {
        cout <<
        "scavetool -- part of " OMNETPP_PRODUCT ", (C) 2006-2016 OpenSim Ltd.\n"
        "Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n"
        "\n";
        cout <<
        "Usage: scavetool <command> [<options>] <files>...\n"
        "\n"
        "For processing result files written by simulations: vector files (.vec) and\n"
        "scalar files (.sca).\n"
        "\n"
        "Commands:\n"
        "    q, query    Query the contents of result files\n"
        "    v, vector   Export vector data\n"
        "    s, scalar   Export scalar data\n"
        "    i, index    Generate index files (.vci) for vector files\n"
        "    h, help     Print this help text\n"
        "\n"
        "The default command is 'query'.\n"
        "\n"
        "To get help, use scavetool help <topic>. Available help topics: any command name, 'patterns',\n"
        "and 'filters'.\n"
        "\n";
    }
    else if (page == "q" || page == "query") {
        cout <<
        "Usage: scavetool query [<options>] <output-vector-and-scalar-files>\n"
        "\n"
        "Query the contents of result files.\n"
        "\n"
        "Options:\n"
        "    -s              Report the number of result items\n"
        "    -l              List result items\n"
        "    -a              List run attributes\n"
        "    -n              List unique result (vector, scalar, etc) names\n"
        "    -m              List unique module names\n"
        "    -e              List unique result names qualified with the module names they occur with\n"
        "    -r              List unique runs\n"
        "    -c              List unique configuration names\n"
        "    -R              Per-run reporting (where applicable)\n"
        "    -Q              Suppress labels (more suitable for machine processing)\n"
        "    -G              Grep-friendly: with -R, put run names at the start of each line,\n"
        "                    not above groups as headings.\n"
        "    -T <types>      Limit item types; <types> is concatenation of type characters\n"
        "                    (v=vector, s=scalar, t=statistic, h=histogram)\n"
        "    -p <filter>     Filter pattern for result items (vectors, scalars, statistics, histograms)\n"
        "                    matched by filter expression (try 'help patterns')\n"
        "    -D <type>       Display mode for run; <type> can be any of:\n"
        "                       'runid'       Displays ${runid} (this is the default)\n"
        "                       'runnumber'   Displays ${configname} ${runnumber}\n"
        "                       'itervars'    Displays ${configname} ${iterationvars} ${repetition}\n"
        "                       'experiment'  Displays ${experiment} ${measurement} ${replication}\n"
        "    -X              Disallow automatic indexing of vector files\n"
        "    -V              Print info about progress (verbose)\n"
        "\n"
        "See also the following help topics: 'patterns'\n"
        "\n";
    }
    else if (page == "v" || page == "vector") {
        cout <<
        "Usage: scavetool vector [<options>] <output-vector-files>\n"
        "\n"
        "Export vector data.\n"
        "\n"
        "Options:\n"
        "    -p <filter>     Filter pattern for vectors matched by the filter expression\n"
        "    -a <function>   Apply the given processing to the selected vectors. Try 'help filters'\n"
        "                    for the list of available operations. This option may occur multiple times.\n"
        "    -O <filename>   Output file name\n"
        "    -F <formatname> Format of output file: vec (default), splitvec, matlab, octave, csv, splitcsv\n"
        "    -X              Disallow automatic indexing of vector files\n"
        "    -V              Print info about progress (verbose)\n"
        "\n"
        "Example:\n"
        "    scavetool vector -p \"queueing time\" -a winavg(10) -O out.vec *.vec\n"
        "        Saves the vectors named 'queueing time' into out.vec, after applying the window average filter.\n"
        "\n"
        "See also the following help topics: 'patterns', 'filters'\n"
        "\n";
    }
    else if (page == "s" || page == "scalar") {
        cout <<
        "Usage: scavetool scalar [<options>] <output-scalar-files>\n"
        "\n"
        "Export scalar data.\n"
        "\n"
        "Options:\n"
        "    -p <pattern>    Filter for scalars matched by filter expression (try 'help patterns')\n"
        "    -a <function>   Apply the given processing to the selected scalars (try 'help filters');\n"
        "                    the 'info' command prints the names of available operations. This option\n"
        "                    may occur multiple times.\n"
        "    -g <grouping>   Specifies how the scalars are grouped. It is a comma separated list of field\n"
        "                    names ('file','run','module','name'). Scalars are grouped by the values of these\n"
        "                    fields and written into separate columns (csv) or variables.\n"
        "                    Default is '' (empty list), meaning that each row contains only one scalar value.\n"
        "    -O <filename>   Output file name\n"
        "    -F <formatname> Format of output file: csv (default), matlab, octave\n"
        "    -X              Disallow automatic indexing of (potentially specified) vector files\n"
        "    -V              Print info about progress (verbose)\n"
        "\n"
        "Example:\n"
        "    scavetool scalar -p \"module(sink) OR module(queue)\" -a \"scatter(.,load,queue,\\\"queue capacity\\\")\" -O out.csv -F csv *.sca\n"
        "        Creates a data table for a scatter plot with the 'load' attribute on the X axis and\n"
        "        'queue capacity' for iso lines, and saves in CSV format.\n"
        "\n"
        "See also the following help topics: 'patterns', 'filters'\n"
        "\n";
    }
    else if (page == "i" || page == "index") {
        cout <<
        "Usage: scavetool index [<options>] <output-vector-files>\n"
        "\n"
        "Generate index files (.vci) for vector files. Note that this command is usually not needed, as\n"
        "other scavetool commands automatically create indices for loaded vector files if they are missing\n"
        "or out of date, unless indexing is explicitly disabled.\n"
        "\n"
        "Options:\n"
        "    -r   Rebuild vector file (rearranges records into blocks)\n"
        "    -V   Print info about progress (verbose)\n"
        "\n";
    }
    else if (page == "patterns") {
        cout <<
        "\nSeveral commands have a -p <filter> option that accepts a match pattern for filtering\n"
        "result items. This page describes the syntax available for match patterns.\n"
        "\n"
        "Filter patterns consist of '<pattern>' and '<fieldname>(<pattern>)' elements, combined\n"
        "with AND, OR, and NOT operators. A plain '<pattern>' is equivalent to 'name(<pattern>)'.\n"
        "\n"
        "<fieldname> is one of:\n"
        "    file              Full path of the result file\n"
        "    run               Unique run Id\n"
        "    module            Module full path\n"
        "    name              Name of the result item (vector, scalar, etc)\n"
        "    attr:<runattr>    The value of a run attribute, e.g. of attr:configname\n"
        "\n"
        "<runattr> may be the name of a standard run attribute ('runid', 'inifile',\n"
        "'configname', 'runnumber', 'network', 'experiment', 'measurement', 'replication',\n"
        "'processid', 'datetime', 'resultdir', 'repetition', 'seedset', 'iterationvars',\n"
        "'iterationvarsf').\n"
        "\n"
        "Inifile iteration variables (e.g. 'numHosts') can also be used for <runattr>,\n"
        "as they are also saved as run attributes.\n"
        "\n"
        "<pattern> is a glob-like pattern:\n"
        "    ?             Matches any character except '.'\n"
        "    *             Matches zero or more characters, except '.'\n"
        "    **            Matches zero or more characters (any character)\n"
        "    {a-z}         Matches a character in range a-z\n"
        "    {^a-z}        Matches a character NOT in range a-z\n"
        "    {250..300}    Matches an embedded integer number in the given range\n"
        "    [250..300]    Matches an integer number in the given range in square brackets\n"
        "    \\ (backslash) Takes away the special meaning of the subsequent character\n"
        "\n"
        "Pattern example:\n"
        "    module(\"**.sink\") AND (name(\"queueing time\") OR name(\"transmission time\"))\n"
        "\n";
    }
    else if (page == "filters") {
        cout << "\nScalar operations:\n\n";
        cout << "scatter(module, scalar, ...)\n"; // currently this is the only scalar operation
        const char *description = "Create scatter plot dataset. The first two arguments identify the "
                "scalar  selected for the X axis. Additional arguments identify the iso attributes; "
                "they are (module, scalar) pairs, or names of run attributes.";
        cout << opp_indentlines(opp_breaklines(description, 76), "  ") << "\n";

        cout << "\nVector operations:\n\n";
        NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
        NodeTypeVector nodeTypes = registry->getNodeTypes();
        for (int i = 0; i < (int)nodeTypes.size(); i++) {
            NodeType *nodeType = nodeTypes[i];
            if (nodeType->isHidden())
                continue;

            // print name(parameters,...)
            cout << nodeType->getName();
            StringMap attrs, attrDefaults;
            nodeType->getAttributes(attrs);
            nodeType->getAttrDefaults(attrDefaults);
            cout << "(";
            for (StringMap::iterator it = attrs.begin(); it != attrs.end(); ++it)
                cout << (it != attrs.begin() ? ", " : "") << it->first;
            cout << ")\n";

            // print filter description and parameter descriptions
            cout << opp_indentlines(opp_breaklines(nodeType->getDescription(), 76), "  ") << "\n";
            for (StringMap::iterator it = attrs.begin(); it != attrs.end(); ++it)
                cout << "    - " << it->first << ": " << it->second << "\n";
            cout << "\n";
        }
    }
    else {
        throw opp_runtime_error("No help topic '%s'", page.c_str());
    }
}

void ScaveTool::loadFiles(ResultFileManager& manager, const vector<string>& fileNames, bool indexingAllowed, bool verbose)
{
    // load files
    ResultFileManager resultFileManager;
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
        ResultFile *f = manager.loadFile(fileName);
        if (verbose)
            cout << "done\n";
        Assert(f); // or exception
        if (f->fileType == ResultFile::FILETYPE_TEXT && f->numUnrecognizedLines > 0)
            cerr << "WARNING: " << fileName << ": " << f->numUnrecognizedLines << " invalid/incomplete lines out of " << f->numLines << "\n";
    }
    if (verbose)
        cout << manager.getFiles().size() << " file(s) loaded\n";
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

static void printAndDelete(StringSet *strings, const string& prefix = "")
{
    if (strings != nullptr) {
        for (StringSet::iterator it = strings->begin(); it != strings->end(); ++it)
            cout << prefix << *it << "\n";
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
        return run->runName;
    case RUNDISPLAY_CONFIG_RUNNUMBER:
        return string(opp_emptytodefault(run->getAttribute("configname"), "null")) + "\t" +
                opp_emptytodefault(run->getAttribute("runnumber"), "null");
    case RUNDISPLAY_CONFIG_ITERVARS_REPETITION:
        return string(opp_emptytodefault(run->getAttribute("configname"), "null")) + "\t" +
                opp_emptytodefault(run->getAttribute("iterationvars"), "null") + "\t" +
                opp_emptytodefault(run->getAttribute("repetition"), "null");
    case RUNDISPLAY_EXPERIMENT_MEASUREMENT_REPLICATION:
        return string(opp_emptytodefault(run->getAttribute("experiment"), "null")) + "\t" +
                opp_emptytodefault(run->getAttribute("measurement"), "null") + "\t" +
                opp_emptytodefault(run->getAttribute("replication"), "null");
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
    int opt_resultTypeFilter = ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::HISTOGRAM;
    RunDisplayMode opt_runDisplayMode = RUNDISPLAY_RUNID;
    bool opt_labels = true;
    bool opt_perRun = false;
    bool opt_grepFriendly = false;
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
        else if (opt == "-s")
            opt_mode = PRINT_SUMMARY;
        else if (opt == "-l")
            opt_mode = LIST_RESULTS;
        else if (opt == "-a")
            opt_mode = LIST_RUNATTRS;
        else if (opt == "-n")
            opt_mode = LIST_NAMES;
        else if (opt == "-m")
            opt_mode = LIST_MODULES;
        else if (opt == "-e")
            opt_mode = LIST_MODULE_AND_NAME_PAIRS;
        else if (opt == "-r")
            opt_mode = LIST_RUNS;
        else if (opt == "-c")
            opt_mode = LIST_CONFIGS;
        else if (opt == "-p" && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (opt == "-T" && i != argc-1)
            opt_resultTypeFilterStr = unquoteString(argv[++i]);
        else if (opt.substr(0,2) == "-T")
            opt_resultTypeFilterStr = opt.substr(2);
        else if (opt == "-D" && i != argc-1)
            opt_runDisplayModeStr = unquoteString(argv[++i]);
        else if (opt.substr(0,2) == "-D")
            opt_runDisplayModeStr = opt.substr(2);
        else if (opt == "-R")
            opt_perRun = true;
        else if (opt == "-Q")
            opt_labels = false;
        else if (opt == "-G")
            opt_grepFriendly = true;
        else if (opt == "-X")
            opt_indexingAllowed = false;
        else if (opt == "-V")
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    // resolve -T, filter by result type
    if (opt_resultTypeFilterStr != "") {
        opt_resultTypeFilter = 0;
        for (char c : opt_resultTypeFilterStr) {
            switch(c) {
            case 's': opt_resultTypeFilter |= ResultFileManager::SCALAR; break;
            case 'v': opt_resultTypeFilter |= ResultFileManager::VECTOR; break;
            case 'h': opt_resultTypeFilter |= ResultFileManager::HISTOGRAM; break;
            default: throw opp_runtime_error("Invalid result type '%c' in '-T' option", c);
            }
        }
    }

    // resolve -D, run display mode
    if (opt_runDisplayModeStr == "runid")
        opt_runDisplayMode = RUNDISPLAY_RUNID;
    else if (opt_runDisplayModeStr == "runnumber")
        opt_runDisplayMode = RUNDISPLAY_CONFIG_RUNNUMBER;
    else if (opt_runDisplayModeStr == "itervars")
        opt_runDisplayMode = RUNDISPLAY_CONFIG_ITERVARS_REPETITION;
    else if (opt_runDisplayModeStr == "experiment")
        opt_runDisplayMode = RUNDISPLAY_EXPERIMENT_MEASUREMENT_REPLICATION;
    else if (opt_runDisplayModeStr != "")
        throw opp_runtime_error("Invalid run display mode '%s' in '-D' option", opt_runDisplayModeStr.c_str());

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_indexingAllowed, opt_verbose);

    // filter statistics
    IDList results = resultFileManager.getAllItems(false, false);
    results.set(results.filterByTypes(opt_resultTypeFilter));
    results.set(resultFileManager.filterIDList(results, opt_filterExpression.c_str()));

    RunList *runs = resultFileManager.getUniqueRuns(results);
    IDList scalars = results.filterByTypes(ResultFileManager::SCALAR);
    IDList vectors = results.filterByTypes(ResultFileManager::VECTOR);
    IDList histograms = results.filterByTypes(ResultFileManager::HISTOGRAM);

    switch (opt_mode) {
    case PRINT_SUMMARY: {
#define L(label) (opt_labels ? label: "\t")
        if (!opt_perRun) {
            cout << L("runs: ") << runs->size() << " ";
            if ((opt_resultTypeFilter & ResultFileManager::SCALAR) != 0)
                cout << L("\tscalars: ") << scalars.size();
            if ((opt_resultTypeFilter & ResultFileManager::VECTOR) != 0)
                cout << L("\tvectors: ") << vectors.size();
            if ((opt_resultTypeFilter & ResultFileManager::HISTOGRAM) != 0)
                cout << L("\thistograms: ") << histograms.size();
            cout << endl;
        }
        else {
            for (Run *run : *runs) {
                cout << runStr(run, opt_runDisplayMode);
                if ((opt_resultTypeFilter & ResultFileManager::SCALAR) != 0) {
                    IDList runScalars = resultFileManager.filterIDList(scalars, run, nullptr, nullptr);
                    cout << L("\tscalars: ") << runScalars.size();
                }
                if ((opt_resultTypeFilter & ResultFileManager::VECTOR) != 0) {
                    IDList runVectors = resultFileManager.filterIDList(vectors, run, nullptr, nullptr);
                    cout << L("\tvectors: ") << runVectors.size();
                }
                if ((opt_resultTypeFilter & ResultFileManager::HISTOGRAM) != 0) {
                    IDList runHistograms = resultFileManager.filterIDList(histograms, run, nullptr, nullptr);
                    cout << L("\thistograms: ") << runHistograms.size();
                }
                cout << endl;
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
                cout << runName << ":" << endl << endl;
#define L(label) (opt_labels ? "\t" #label "=" : "\t")
            IDList runScalars = resultFileManager.filterIDList(scalars, run, nullptr, nullptr);
            IDList runVectors = resultFileManager.filterIDList(vectors, run, nullptr, nullptr);
            IDList runHistograms = resultFileManager.filterIDList(histograms, run, nullptr, nullptr);

            for (int i = 0; i < runScalars.size(); i++) {
                const ScalarResult& s = resultFileManager.getScalar(runScalars.get(i));
                cout << maybeRunColumnWithTab << "scalar\t" << *s.moduleNameRef << "\t" << *s.nameRef << "\t" << s.value << endl;
            }

            for (int i = 0; i < runVectors.size(); i++) {
                const VectorResult& v = resultFileManager.getVector(runVectors.get(i));
                cout << maybeRunColumnWithTab << "vector\t" << *v.moduleNameRef << "\t" << *v.nameRef << L(vectorId) << v.vectorId;
                const Statistics& s = v.getStatistics();
                if (s.getCount() >= 0) // information is valid, i.e. index file exists
                    cout << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin()  << L(max) << s.getMax();
                cout << endl;
            }

            for (int i = 0; i < runHistograms.size(); i++) {
                const HistogramResult& h = resultFileManager.getHistogram(runHistograms.get(i));
                const Statistics& s = h.getStatistics();
                cout << maybeRunColumnWithTab << "histogram\t" << *h.moduleNameRef << "\t" << *h.nameRef << L(count) << s.getCount() << L(mean) << s.getMean() << L(min) << s.getMin()  << L(max) << s.getMax() << L(#bins) << h.bins.size() << endl;
            }
            cout << endl;
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
                cout << runName << ":" << endl << endl;
            for (auto& runAttr : run->attributes)
                cout << maybeRunColumnWithTab << runAttr.first << "\t" << runAttr.second << std::endl;
            cout << endl;
        }
        break;
    }
    case LIST_NAMES: {
        if (!opt_perRun)
            printAndDelete(resultFileManager.getUniqueNames(results));
        else {
            for (Run *run : *runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    cout << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                printAndDelete(resultFileManager.getUniqueNames(runResults), opt_grepFriendly ? runName + "\t" : "");
                cout << endl;
            }
        }
        break;
    }
    case LIST_MODULES: {
        if (!opt_perRun)
            printAndDelete(resultFileManager.getUniqueModuleNames(results));
        else {
            for (Run *run : *runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    cout << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                printAndDelete(resultFileManager.getUniqueModuleNames(runResults), opt_grepFriendly ? runName + "\t" : "");
                cout << endl;
            }
        }
        break;
    }
    case LIST_MODULE_AND_NAME_PAIRS: {
        if (!opt_perRun)
            printAndDelete(resultFileManager.getUniqueModuleAndResultNamePairs(results));
        else {
            for (Run *run : *runs) {
                string runName = runStr(run, opt_runDisplayMode);
                if (!opt_grepFriendly)
                    cout << runName << ":" << endl << endl;
                IDList runResults = resultFileManager.filterIDList(results, run, nullptr, nullptr);
                printAndDelete(resultFileManager.getUniqueModuleAndResultNamePairs(runResults), opt_grepFriendly ? runName + "\t" : "");
                cout << endl;
            }
        }
        break;
    }
    case LIST_RUNS: {
        // note: we ignore opt_perRun, as it makes no sense here
        StringSet *uniqueRunNames = new StringSet;
        for (RunList::const_iterator it = runs->begin(); it != runs->end(); ++it)
            uniqueRunNames->insert((*it)->runName);
        printAndDelete(uniqueRunNames);
        break;
    }
    case LIST_CONFIGS: {
        // note: we ignore opt_perRun, as it makes no sense here
        StringSet *uniqueConfigNames = resultFileManager.getUniqueRunAttributeValues(*runs, RunAttribute::CONFIGNAME);
        printAndDelete(uniqueConfigNames);
        break;
    }
    default: {
        Assert(false);
    }
    } // switch

    delete runs;
}

void ScaveTool::vectorCommand(int argc, char **argv)
{
    // options
    string opt_filterExpression;
    string opt_outputFileName = "out";
    string opt_outputFormat = "vec";  // TBD vec, splitvec, octave, split octave (and for octave: x, y, both),...
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
        attrs["filename"] = resultFile->fileSystemFilePath;
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
        Assert(vectorFileReaders.find(vector.fileRunRef->fileRef) != vectorFileReaders.end());
        sprintf(portName, "%d", vector.vectorId);
        Node *readerNode = vectorFileReaders[vector.fileRunRef->fileRef];
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
                header << "# vector " << vector.vectorId << " "
                       << QUOTE(vector.moduleNameRef->c_str()) << " "
                       << QUOTE(vector.nameRef->c_str()) << "\n";
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
            // for Octave, we must build arrays
            if (opt_verbose)
                cout << "adding array builders for Octave output\n";
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
        ScaveExport *exporter = ExporterFactory::createExporter(opt_outputFormat);
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
                    string name = *vector.nameRef;
                    string descr = *vector.nameRef + "; "
                            + *vector.moduleNameRef + "; "
                            + vector.fileRunRef->fileRef->fileSystemFilePath + "; "
                            + vector.fileRunRef->runRef->runName;
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
        ScaveExport *exporter = ExporterFactory::createExporter(opt_outputFormat);
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
        if (opt == "-V")
            opt_verbose = true;
        else if (opt == "-r")
            opt_rebuild = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt.c_str());
    }

    VectorFileIndexer indexer;
    for (int i = 0; i < (int)opt_fileNames.size(); i++) {
        const char *fileName = opt_fileNames[i].c_str();
        if (opt_verbose)
            cout << "indexing " << fileName << "...\n";
        if (opt_rebuild)
            indexer.rebuildVectorFile(fileName);
        else
            indexer.generateIndex(fileName);
    }

    if (opt_verbose)
        cout << "done\n";
}

int ScaveTool::main(int argc, char **argv)
{
    if (argc < 2) {
        helpCommand(argc-1, argv+1);
        exit(0);
    }

    try {
        string command = argv[1];
        if (command == "q" || command == "query")
            queryCommand(argc-2, argv+2);
        else if (command == "v" || command == "vector")
            vectorCommand(argc-2, argv+2);
        else if (command == "s" || command == "scalar")
            scalarCommand(argc-2, argv+2);
        else if (command == "x" || command == "index")
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

