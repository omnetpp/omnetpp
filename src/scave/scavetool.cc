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

#include <cassert>
#include <sstream>
#include "common/ver.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "resultfilemanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
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

void ScaveTool::printUsage()
{
    cout <<
       "scavetool -- part of " OMNETPP_PRODUCT ", (C) 2006-2015 OpenSim Ltd.\n"
       "Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n"
       "\n"
       "Usage: scavetool <command> [options] <files>...\n"
       "\n"
       "For processing result files written by simulations: vector files (.vec) and\n"
       "scalar files (.sca).\n"
       "\n"
       "Commands:\n"
       "    v, vector   filter and process data in vector files\n"
       "    s, scalar   filter and process data in scalar files\n"
       "    l, list     list summary info about input files\n"
       "    i, info     print list of available functions (to be used with '-a')\n"
       "    x, index    generate index files for vector files\n"
       "Options:\n"
       " 'vector' command:\n"
       "    -p <pattern>    the filter expression (see syntax below)\n"
       "    -a <function>   apply the given processing to the selected vectors (see syntax below);\n"
       "                    the 'info' command prints the names of available operations. This option\n"
       "                    may occur multiple times.\n"
       "    -O <filename>   output file name\n"
       "    -F <formatname> format of output file: vec (default), splitvec, matlab, octave, csv, splitcsv\n"
       "    -V              print info about progress (verbose)\n"
       //TODO option: print matching vectorIDs and exit
       " 'scalar' command:\n"
       "    -p <pattern>    the filter expression (see syntax below)\n"
       "    -a <function>   apply the given processing to the selected scalars (see syntax below);\n"
       "                    the 'info' command prints the names of available operations. This option\n"
       "                    may occur multiple times.\n"
       "    -g <grouping>   specifies how the scalars are grouped. It is a comma separated list of field\n"
       "                    names ('file','run','module','name'). Scalars are grouped by the values of these\n"
       "                    fields and written into separate columns (csv) or variables.\n"
       "                    Default is '' (empty list), meaning that each row contains only one scalar value.\n"
       "    -O <filename>   output file name\n"
       "    -F <formatname> format of output file: csv (default), matlab, octave\n" //TODO sca files
       "    -V              print info about progress (verbose)\n"
       " 'list' command:\n"
       //TODO allow filtering by patterns here too?
       //TODO specifying more than one flag should list tuples e.g. (module,statistic) pairs
       // occurring in the input files
       "    -n   print list of unique statistics names\n"
       "    -m   print list of unique module name\n"
       "    -r   print list of unique run Ids\n"
       "    -c   print list of unique configuration Ids (aka run numbers)\n"
       //TODO other attributes
       " 'info' command:\n"
       "    -b   list filter names only (brief)\n"
       "    -s   list filter names with parameter list (summary)\n"
       "    -v   include descriptions in the output (default)\n"
       " 'index' command:\n"
       "    -r   rebuild vector file (rearranges records into blocks)\n"
       "    -V   print info about progress (verbose)\n"
       "\n"
       "Function syntax (for 'vector -a'): <name>(<parameterlist>).\n"
       "Examples: winavg(10), mean()\n"
       "\n"
       // TODO scalar functions
       "Pattern syntax: one or more <fieldname>(<pattern>) pairs, combined with AND,\n"
       "OR, NOT operators.\n"
       "  <fieldname> is one of:\n"
       "    file:             full path of the result file\n"
       "    run:              unique run Id\n"
       "    module:           module full path\n"
       "    name:             name of the statistic\n"
       "    attr:experiment:  the experiment attribute of the run\n"
       "    attr:measurement: the measurement attribute of the run\n"
       "    attr:replication: the replication attribute of the run\n"
       "  <pattern> is a glob-like pattern:\n"
       "    ?             matches any character except '.'\n"
       "    *             matches zero or more characters, except '.'\n"
       "    **            matches zero or more characters (any character)\n"
       "    {a-z}         matches a character in range a-z\n"
       "    {^a-z}        matches a character NOT in range a-z\n"
       "    {32..255}     any number (ie. sequence of digits) in range 32..255  (e.g. \"99\")\n"
       "    [32..255]     any number in square brackets in range 32..255 (e.g. \"[99]\")\n"
       "    \\ (backslash) takes away the special meaning of the subsequent character\n"
       "Pattern example:\n"
       "    module(\"**.sink\") AND (name(\"queueing time\") OR name(\"transmission time\"))\n"
       "\n"
       "Examples:\n"
       "    scavetool vector -p \"queueing time\" -a winavg(10) -O out.vec *.vec\n"
       "        Saves the queueing time vectors to out.vec, after applying a window \n"
       "        average filter.\n"
       "\n"
       "    scavetool scalar -p \"module(sink) OR module(queue)\" -a \"scatter(.,load,queue,\\\"queue length\\\")\" -O out.csv -F csv *.sca\n"
       "        Creates a scatter plot with the load attribute on the x axis, and queue length\n"
       "        for iso lines.\n"
    ;
}

void ScaveTool::loadFiles(ResultFileManager& manager, const vector<string>& fileNames, bool verbose)
{
    // load files
    ResultFileManager resultFileManager;
    for (int i = 0; i < (int)fileNames.size(); i++) {
        // TODO on Windows: manual globbing of wildcards
        const char *fileName = fileNames[i].c_str();
        if (verbose)
            cout << "reading " << fileName << "...\n";
        ResultFile *f = manager.loadFile(fileName);
        Assert(f); // or exception
        if (f->fileType == ResultFile::FILETYPE_TEXT && f->numUnrecognizedLines > 0)
            cerr << "WARNING: " << fileName << ": " << f->numUnrecognizedLines << " invalid/incomplete lines out of " << f->numLines << "\n";
    }
    if (verbose)
        cout << manager.getFiles().size() << " file(s) loaded\n";
}

string ScaveTool::rebuildCommandLine(int argc, char **argv)
{
    // FIXME quotes
    string result;
    for (int i = 0; i < argc; i++) {
        if (i != 0)
            result += " ";
        result += argv[i];
    }
    return result;
}

void ScaveTool::vectorCommand(int argc, char **argv)
{
    // options
    bool opt_verbose = false;
    string opt_filterExpression;
    string opt_outputFileName = "_out_";
    string opt_outputFormat = "vec";  // TBD vec, splitvec, octave, split octave (and for octave: x, y, both),...
    string opt_readerNodeType = "vectorreaderbyfiletype";
    vector<string> opt_filterList;
    vector<string> opt_fileNames;

    // parse options
    bool endOpts = false;
    for (int i = 2; i < argc; i++) {
        const char *opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (!strcmp(opt, "--"))
            endOpts = true;
        else if (!strcmp(opt, "-p") && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (!strcmp(opt, "-a") && i != argc-1)
            opt_filterList.push_back(argv[++i]);
        else if (!strcmp(opt, "-O") && i != argc-1)
            opt_outputFileName = argv[++i];
        else if (!strcmp(opt, "-F") && i != argc-1)
            opt_outputFormat = argv[++i];
        else if (!strcmp(opt, "-V"))
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else if (!strcmp(opt, "-r") && i != argc-1)  // for testing only
            opt_readerNodeType = argv[++i];
        else
            throw opp_runtime_error("Unknown option '%s'", opt);
    }

    bool opt_writeSeparateFiles = false;
    if (opt_outputFormat.find("split") == 0) {
        opt_outputFormat = opt_outputFormat.substr(5);
        opt_writeSeparateFiles = true;
    }

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_verbose);

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
    ResultFileList& filteredVectorFileList = *resultFileManager.getUniqueFiles(vectorIDList);  // FIXME delete after done?
    map<ResultFile *, Node *> vectorFileReaders;
    NodeType *readerNodeType = registry->getNodeType(opt_readerNodeType.c_str());
    if (!readerNodeType)
        throw opp_runtime_error("Unknown node type '%s'", opt_readerNodeType.c_str());

    // create reader nodes
    StringMap attrs;
    for (int i = 0; i < (int)filteredVectorFileList.size(); i++) {
        ResultFile *resultFile = filteredVectorFileList[i];
        attrs["filename"] = resultFile->fileSystemFilePath;
        attrs["allowindexing"] = "true";
        Node *readerNode = readerNodeType->create(&dataflowManager, attrs);
        vectorFileReaders[resultFile] = readerNode;
    }

    // create writer node, if each vector is written into the same file
    VectorFileWriterNode *vectorFileWriterNode = nullptr;

    vector<ArrayBuilderNode *> arrayBuilders;  // for exporting

    for (int i = 0; i < vectorIDList.size(); i++) {
        // create a port for each vector on its reader node
        char portName[30];
        const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
        assert(vectorFileReaders.find(vector.fileRunRef->fileRef) != vectorFileReaders.end());
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
                        <<QUOTE(vector.moduleNameRef->c_str()) << " "
                        <<QUOTE(vector.nameRef->c_str()) << "\n";
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
    bool opt_verbose = false;
    string opt_filterExpression;
    string opt_outputFileName = "_out_";
    string opt_outputFormat = "csv";
    string opt_applyFunction;
    string opt_groupingFields = "";
    vector<string> opt_fileNames;

    // parse options
    bool endOpts = false;
    for (int i = 2; i < argc; i++) {
        const char *opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (!strcmp(opt, "--"))
            endOpts = true;
        else if (!strcmp(opt, "-p") && i != argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (!strcmp(opt, "-a") && i != argc-1)
            opt_applyFunction = unquoteString(argv[++i]);
        else if (!strcmp(opt, "-g") && i != argc-1)
            opt_groupingFields = unquoteString(argv[++i]);
        else if (!strcmp(opt, "-O") && i != argc-1)
            opt_outputFileName = argv[++i];
        else if (!strcmp(opt, "-F") && i != argc-1)
            opt_outputFormat = argv[++i];
        else if (!strcmp(opt, "-V"))
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt);
    }

    // load files
    ResultFileManager resultFileManager;
    loadFiles(resultFileManager, opt_fileNames, opt_verbose);

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

// TODO allow filtering by patterns here too?
// TODO specifying more than one flag should list tuples e.g. (module,statistic) pairs
// occurring in the input files
void ScaveTool::listCommand(int argc, char **argv)
{
    bool opt_name = false;
    bool opt_module = false;
    bool opt_run = false;
    bool opt_config = false;
    int count = 0;
    vector<string> opt_fileNames;

    for (int i = 2; i < argc; i++) {
        const char *opt = argv[i];
        if (opt[0] == '-')
            count++;
        if (strcmp(opt, "-n") == 0)
            opt_name = true;
        else if (strcmp(opt, "-m") == 0)
            opt_module = true;
        else if (strcmp(opt, "-r") == 0)
            opt_run = true;
        else if (strcmp(opt, "-c") == 0)
            opt_config = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt);
    }
    if (count == 0)
        opt_name = true;
    else if (count > 1)
        throw opp_runtime_error("One option expected");

    ResultFileManager manager;
    loadFiles(manager, opt_fileNames, false);

    StringSet *result = nullptr;
    if (opt_name) {
        IDList ids = manager.getAllItems();
        result = manager.getUniqueNames(ids);
    }
    else if (opt_module) {
        IDList ids = manager.getAllItems();
        result = manager.getUniqueModuleNames(ids);
    }
    else if (opt_run) {
        const RunList& runs = manager.getRuns();
        result = new StringSet;
        for (RunList::const_iterator it = runs.begin(); it != runs.end(); ++it) {
            result->insert((*it)->runName);
        }
    }
    else if (opt_config) {
        const RunList& runs = manager.getRuns();
        result = manager.getUniqueRunAttributeValues(runs, RunAttribute::CONFIGNAME);
    }

    if (result != nullptr) {
        for (StringSet::iterator it = result->begin(); it != result->end(); ++it) {
            cout << *it << "\n";
        }
        delete result;
    }
}

void ScaveTool::infoCommand(int argc, char **argv)
{
    // process args
    bool opt_brief = false;
    bool opt_summary = false;
    for (int i = 2; i < argc; i++) {
        const char *opt = argv[i];
        if (!strcmp(opt, "-b"))
            opt_brief = true;
        else if (!strcmp(opt, "-s"))
            opt_summary = true;
        else if (!strcmp(opt, "-v"))
            ;  // no-op
        else
            throw opp_runtime_error("Unknown option '%s'", opt);
    }

    cout << "\nScalar operations:\n\n";
    cout << "scatter(module,scalar,...):\n"
           "  Create scatter plot dataset. The first two arguments identifies the scalar\n"
           "  selected for the X axis. Additional arguments identify the iso attributes;\n"
           "  they are (module, scalar) pairs, or names of run attributes.\n";

    cout << "\nVector operations:\n\n";
    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
    NodeTypeVector nodeTypes = registry->getNodeTypes();
    for (int i = 0; i < (int)nodeTypes.size(); i++) {
        NodeType *nodeType = nodeTypes[i];
        if (nodeType->isHidden())
            continue;

        if (opt_brief) {
            // this is the -b format
            cout << nodeType->getName() << "\n";
        }
        else {
            // print name(parameters,...)
            cout << nodeType->getName();
            StringMap attrs, attrDefaults;
            nodeType->getAttributes(attrs);
            nodeType->getAttrDefaults(attrDefaults);
            cout << "(";
            for (StringMap::iterator it = attrs.begin(); it != attrs.end(); ++it) {
                if (it != attrs.begin())
                    cout << ",";
                cout << it->first;
            }
            cout << ")";

            if (opt_summary) {
                cout << "\n";
            }
            else {
                // print filter description and parameter descriptions
                cout << ":\n" << opp_indentlines(opp_breaklines(nodeType->getDescription(), 76), "  ") << "\n";
                for (StringMap::iterator it = attrs.begin(); it != attrs.end(); ++it) {
                    cout << "    - " << it->first << ": " << it->second << "\n";
                }
                cout << "\n";
            }
        }
    }
}

void ScaveTool::indexCommand(int argc, char **argv)
{
    // process args
    bool opt_verbose = false;
    bool opt_rebuild = false;
    vector<string> opt_fileNames;
    for (int i = 2; i < argc; i++) {
        const char *opt = argv[i];
        if (strcmp(opt, "-V") == 0)
            opt_verbose = true;
        else if (strcmp(opt, "-r") == 0)
            opt_rebuild = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            throw opp_runtime_error("Unknown option '%s'", opt);
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
        printUsage();
        exit(0);
    }

    try {
        const char *command = argv[1];
        if (!strcmp(command, "v") || !strcmp(command, "vector"))
            vectorCommand(argc, argv);
        else if (!strcmp(command, "s") || !strcmp(command, "scalar"))
            scalarCommand(argc, argv);
        else if (!strcmp(command, "l") || !strcmp(command, "list"))
            listCommand(argc, argv);
        else if (!strcmp(command, "i") || !strcmp(command, "info"))
            infoCommand(argc, argv);
        else if (!strcmp(command, "x") || !strcmp(command, "index"))
            indexCommand(argc, argv);
        else
            throw opp_runtime_error("Unknown command '%s'", command);
    }
    catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
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

