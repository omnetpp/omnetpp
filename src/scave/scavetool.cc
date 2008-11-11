//=========================================================================
//  SCAVETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <sstream>
#include "../common/ver.h"
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
#include "stringutil.h"
#include "scaveutils.h"

USING_NAMESPACE


void printUsage()
{
    printf(
       "scavetool -- part of " OMNETPP_PRODUCT ", (C) 2006-2008 Andras Varga, OpenSim Ltd.\n"
       "Version: " OMNETPP_VERSION ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n"
       "\n"
       "Usage: scavetool <command> [options] <files>...\n"
       "\n"
       "For processing result files written by simulations: vector files (.vec) and\n"
       "scalar files (.sca).\n"
       "\n"
       "Commands:\n"
       "    f, filter   filter data in input files\n"
       "    s, summary  prints summary info about input files\n"
       "    i, info     prints list of available functions (to be used with `filter -a')\n"
       "    x, index    generates index files for input files\n"
       "Options:\n"
       " `filter' command:\n"
       "    -p <pattern>    the filter expression (see syntax below)\n"
       "    -a <function>   apply the given processing to the vector (see syntax below)\n"
       "                    This option may occur multiple times.\n"
       "    -O <filename>   output file name\n"   //FIXME separate file for vectors and scalars I guess
       "    -F <formatname> format of output file: vec, sca, matlab, octave, csv, ...\n" //TODO
       "    -V              print info about progress (verbose)\n"
       //TODO option: print matching vectorIDs and exit
       //TODO: dump scalars too!!!
       " `summary' command:\n"
       //TODO allow filtering by patterns here too?
       //TODO specifying more than one flag should list tuples e.g. (module,statistic) pairs
       // occurring in the input files
       "    -n   print list of unique statistics names\n"
       "    -m   print list of unique module name\n"
       "    -r   print list of unique run Ids\n"
       "    -c   print list of unique configuration Ids (aka run numbers)\n"
       //TODO other attributes
       " `info' command:\n"
       "    -b   list filter names only (brief)\n"
       "    -s   list filter names with parameter list (summary)\n"
       "    -v   include descriptions in the output (default)\n"
       " `index' command:\n"
       "    -r   rebuild vector file (rearranges records into blocks)\n"
       "    -V   print info about progress (verbose)\n"
       "\n"
       "Function syntax (for `filter -a'): <name>(<parameterlist>).\n"
       "Examples: winavg(10), mean()\n"
       "\n"
       "Pattern syntax: one or more <fieldname>=<pattern> pairs, combined with AND,\n"
       "OR, NOT operators.\n"
       "  <fieldname> is one of:\n"
       "    file:         full path of the result file\n"
       "    run:          unique run Id\n"
       "    experiment:   the experiment attribute of the run\n"
       "    measurement:  the measurement attribute of the run\n"
       "    replication:  the replication attribute of the run\n"
       "    module:       module full path\n"
       "    name:         name of the statistic\n"
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
       "    module=\"**.sink\" AND (name=\"queueing time\" OR name=\"transmission time\")\n"
       "\n"
       "Examples:\n"
       "    scavetool -p \"queueing time\" -a winavg(10) -O out.vec\n\n" //TODO more
    );
}

static void loadFiles(ResultFileManager &manager, const std::vector<std::string> &fileNames, bool verbose)
{
    // load files
    ResultFileManager resultFileManager;
    for (int i=0; i<(int)fileNames.size(); i++)
    {
        //TODO on Windows: manual globbing of wildcards
        const char *fileName = fileNames[i].c_str();
        if (verbose) printf("reading %s...", fileName);
        try {
            ResultFile *f = manager.loadFile(fileName);
            if (!f)
            {
                if (verbose) printf("\n");
                fprintf(stderr, "Error: %s: load() returned null\n", fileName);
            }
            else if (f->numUnrecognizedLines>0)
            {
                if (verbose) printf("\n");
                fprintf(stderr, "WARNING: %s: %d invalid/incomplete lines out of %d\n", fileName, f->numUnrecognizedLines, f->numLines);
            }
            else
            {
                if (verbose) printf(" %d lines\n", f->numLines);
            }
        }
        catch (std::exception& e) {
            fprintf(stdout, "Exception: %s\n", e.what());
        }
    }
    if (verbose) printf("%d file(s) loaded\n", (int)manager.getFiles().size());
}

int filterCommand(int argc, char **argv)
{
    // options
    bool opt_verbose = false;
    std::string opt_filterExpression;
    std::string opt_outputFileName = "_out_";
    std::string opt_outputFormat;  //TBD vec, splitvec, octave, split octave (and for octave: x, y, both),...
    std::string opt_readerNodeType = "vectorfilereader";
    std::vector<std::string> opt_filterList;
    std::vector<std::string> opt_fileNames;
    StringMap opt_runAttrPatterns; //FIXME options to fill this

    // parse options
    bool endOpts = false;
    for (int i=2; i<argc; i++)
    {
        const char *opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (!strcmp(opt, "--"))
            endOpts = true;
        else if (!strcmp(opt, "-p") && i!=argc-1)
            opt_filterExpression = unquoteString(argv[++i]);
        else if (!strcmp(opt, "-a") && i!=argc-1)
            opt_filterList.push_back(argv[++i]);
        else if (!strcmp(opt, "-O") && i!=argc-1)
            opt_outputFileName = argv[++i];
        else if (!strcmp(opt, "-F") && i!=argc-1)
            opt_outputFormat = argv[++i];
        else if (!strcmp(opt, "-V"))
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else if (!strcmp(opt, "-r") && i!=argc-1) // for testing only
            opt_readerNodeType = argv[++i];
        else
            {fprintf(stderr, "unknown option `%s'", opt);return 1;}
    }

    // only exactly one of the next ones may be true
    bool opt_writeVectorFile = (opt_outputFormat.empty() || opt_outputFormat == "vec");
    bool opt_writeSeparateFiles = false; //TODO create option for this
    bool opt_writeExportFile = !(opt_writeVectorFile || opt_writeSeparateFiles);

    try
    {
        // load files
        ResultFileManager resultFileManager;
        loadFiles(resultFileManager, opt_fileNames, opt_verbose);

        // filter statistics
        IDList vectorIDList = resultFileManager.filterIDList(resultFileManager.getAllVectors(), opt_filterExpression.c_str());
        IDList scalarIDList = resultFileManager.filterIDList(resultFileManager.getAllScalars(), opt_filterExpression.c_str());

        if (opt_verbose) printf("module and name filter matches %d vectors and %d scalars\n",
                            vectorIDList.size(), scalarIDList.size());

        if (opt_verbose) printf("done collecting inputs\n\n");

        //
        // assemble dataflow network for vectors
        //
        DataflowManager dataflowManager;
        NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();

        // create filereader for each vector file
        if (opt_verbose) printf("creating vector file reader(s)\n");
        ResultFileList& filteredVectorFileList = *resultFileManager.getUniqueFiles(vectorIDList); //FIXME delete after done?
        std::map<ResultFile*, Node*> vectorFileReaders;
        NodeType *readerNodeType = registry->getNodeType(opt_readerNodeType.c_str());
        if (!readerNodeType)
        {
            fprintf(stdout, "There is no node type %s in the registry\n", opt_readerNodeType.c_str());
            return 1;
        }

        // create reader nodes
        StringMap attrs;
        for (int i=0; i<(int)filteredVectorFileList.size(); i++)
        {
            ResultFile *resultFile = filteredVectorFileList[i];
            attrs["filename"] = resultFile->fileSystemFilePath;
            Node *readerNode = readerNodeType->create(&dataflowManager, attrs);
            vectorFileReaders[resultFile] = readerNode;
        }

        // create writer node, if each vector is written into the same file
        VectorFileWriterNode *vectorFileWriterNode = NULL;

        std::vector<ArrayBuilderNode*> arrayBuilders; // for exporting

        for (int i=0; i<vectorIDList.size(); i++)
        {
            // create a port for each vector on its reader node
            char portName[30];
            const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
            assert(vectorFileReaders.find(vector.fileRunRef->fileRef) != vectorFileReaders.end());
            sprintf(portName, "%d", vector.vectorId);
            Node *readerNode = vectorFileReaders[vector.fileRunRef->fileRef];
            Port *outPort = readerNodeType->getPort(readerNode, portName);

            // add filters
                for (int k=0; k<(int)opt_filterList.size(); k++)
            {
                //TODO support filter to merge all into a single vector
                if (opt_verbose) printf("adding filter to vector: %s\n", opt_filterList[k].c_str());
                Node *node = registry->createNode(opt_filterList[k].c_str(), &dataflowManager);
                FilterNode *filterNode = dynamic_cast<FilterNode *>(node);
                if (!filterNode)
                    throw opp_runtime_error("%s is not a filter node", opt_filterList[k].c_str());
                dataflowManager.connect(outPort, &(filterNode->in));
                outPort = &(filterNode->out);
            }

            // create writer getNode(s) and connect
            if (opt_writeVectorFile)
            {
                // everything goes to a common vector file
                if (!vectorFileWriterNode)
                {
                    if (opt_verbose) printf("adding vector file writer\n");
                    std::string fileName = opt_outputFileName + ".vec";
                    vectorFileWriterNode = new VectorFileWriterNode(fileName.c_str(), "# generated by scavetool");
                    dataflowManager.addNode(vectorFileWriterNode);
                }

                Port *writerNodePort = vectorFileWriterNode->addVector(vector); // FIXME: renumber vectors
                dataflowManager.connect(outPort, writerNodePort);
            }
            else if (opt_writeSeparateFiles)
            {
                // every vector goes to its own file, with two columns (time+value) separated by spaces/tab
                if (opt_verbose) printf("adding separate writers for each vector\n");
                char buf[16];
                sprintf(buf, "%d", i);
                std::string fname = opt_outputFileName+buf+".vec";

                std::stringstream header;
                header << "# vector " << vector.vectorId << " " <<
                          QUOTE(vector.moduleNameRef->c_str()) << " " <<
                          QUOTE(vector.nameRef->c_str()) << "\n";
                header << "# file generated by scavetool\n";

                FileWriterNode *writerNode = new FileWriterNode(fname.c_str(), header.str().c_str());
                dataflowManager.addNode(writerNode);
                dataflowManager.connect(outPort, &(writerNode->in));
            }
            else if (opt_writeExportFile)
            {
                // for Octave, we must build arrays
                if (opt_verbose) printf("adding array builders for Octave output\n");
                ArrayBuilderNode *arrayBuilderNode = new ArrayBuilderNode();
                dataflowManager.addNode(arrayBuilderNode);
                dataflowManager.connect(outPort, &(arrayBuilderNode->in));
                arrayBuilders.push_back(arrayBuilderNode);
            }
            else
            {
                fprintf(stdout, "No output method specified.\n");
                return 1;
            }
        }

        // run!
        if (opt_verbose) printf("running dataflow network...\n");
        dataflowManager.execute();

        if (opt_writeExportFile)
        {
            ScaveExport *exporter = ExporterFactory::createExporter(opt_outputFormat);
            if (exporter)
            {
                try
                {
                    exporter->setBaseFileName(opt_outputFileName);
                    // write vectors
                    for (int i=0; i<vectorIDList.size(); i++)
                    {
                        ID vectorID = vectorIDList.get(i);
                        bool computed = opt_filterList.size() > 0;
                        const VectorResult& vector = resultFileManager.getVector(vectorID);
                        std::string name = *vector.nameRef;
                        std::string descr = *vector.nameRef + "; "
                                          + *vector.moduleNameRef + "; "
                                          + vector.fileRunRef->fileRef->fileSystemFilePath + "; "
                                          + vector.fileRunRef->runRef->runName;
                        XYArray *xyArray = arrayBuilders[i]->getArray();
                        exporter->saveVector(name, descr, vectorID, computed, xyArray, resultFileManager);
                        delete xyArray;
                    }
                    // write scalars
                    if (!scalarIDList.isEmpty())
                    {
                        ResultItemFields fields(ResultItemField::NAME);
                        exporter->saveScalars("scalars", "scalar desc", scalarIDList,
                            fields.complement(), resultFileManager);
                    }

                    delete exporter;
                }
                catch (std::exception&)
                {
                    delete exporter;
                    throw;
                }
            }
            else
            {
                fprintf(stdout, "Unknown output file format: %s\n", opt_outputFormat.c_str());
                return 1;
            }
        }

        if (opt_verbose) printf("done\n");
    }
    catch (std::exception& e)
    {
        fprintf(stdout, "Exception: %s\n", e.what());
        return 1;
    }

    return 0;
}

//TODO allow filtering by patterns here too?
//TODO specifying more than one flag should list tuples e.g. (module,statistic) pairs
// occurring in the input files
int summaryCommand(int argc, char **argv)
{
	bool opt_name = false;
	bool opt_module = false;
	bool opt_run = false;
	bool opt_config = false;
	int count = 0;
    std::vector<std::string> opt_fileNames;

    for (int i=2; i<argc; i++)
    {
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
            {fprintf(stderr, "unknown option `%s'", opt);return 1;}
    }
    if (count == 0)
    	opt_name = true;
    else if (count > 1)
    {
    	fprintf(stderr, "expects only one option");
    	return 1;
    }

    ResultFileManager manager;
    loadFiles(manager, opt_fileNames, false);

    StringSet *result = NULL;
    if (opt_name)
    {
    	IDList ids = manager.getAllItems();
    	result = manager.getUniqueNames(ids);
    }
    else if (opt_module)
    {
    	IDList ids = manager.getAllItems();
    	result = manager.getUniqueModuleNames(ids);
    }
    else if (opt_run)
    {
    	const RunList &runs = manager.getRuns();
    	result = new StringSet;
    	for (RunList::const_iterator it=runs.begin(); it!=runs.end(); ++it)
    	{
    		result->insert((*it)->runName);
    	}
    }
    else if (opt_config)
    {
    	const RunList &runs = manager.getRuns();
    	result = manager.getUniqueRunAttributeValues(runs, RunAttribute::CONFIGNAME);
    }

    if (result != NULL)
    {
    	for (StringSet::iterator it=result->begin(); it != result->end(); ++it)
    	{
    		printf("%s\n", it->c_str());
    	}
    	delete result;
    }

    return 0;
}

int infoCommand(int argc, char **argv)
{
    // process args
    bool opt_brief = false;
    bool opt_summary = false;
    for (int i=2; i<argc; i++)
    {
        const char *opt = argv[i];
        if (!strcmp(opt, "-b"))
            opt_brief = true;
        else if (!strcmp(opt, "-s"))
            opt_summary = true;
        else if (!strcmp(opt, "-v"))
            ; // no-op
        else
            {fprintf(stderr, "unknown option `%s'", opt);return 1;}
    }

    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
    NodeTypeVector nodeTypes = registry->getNodeTypes();
    for (int i=0; i<(int)nodeTypes.size(); i++)
    {
        NodeType *nodeType = nodeTypes[i];
        if (nodeType->isHidden())
            continue;

        if (opt_brief)
        {
            // this is the -b format
            printf("%s\n", nodeType->getName());
        }
        else
        {
            // print name(parameters,...)
            printf("%s", nodeType->getName());
            StringMap attrs, attrDefaults;
            nodeType->getAttributes(attrs);
            nodeType->getAttrDefaults(attrDefaults);
            printf("(");
            for (StringMap::iterator it=attrs.begin(); it!=attrs.end(); ++it)
            {
                if (it!=attrs.begin()) printf(",");
                printf("%s", it->first.c_str());
            }
            printf(")");

            if (opt_summary)
            {
                printf("\n");
            }
            else
            {
                // print filter description and parameter descriptions
                printf(":\n%s\n", opp_indentlines(opp_breaklines(nodeType->getDescription(),76).c_str(),"  ").c_str());
                for (StringMap::iterator it=attrs.begin(); it!=attrs.end(); ++it)
                {
                    printf("    - %s: %s\n", it->first.c_str(), it->second.c_str());
                }
                printf("\n");
            }
        }
    }
    return 0;
}

int indexCommand(int argc, char **argv)
{
    // process args
    bool opt_verbose = false;
    bool opt_rebuild = false;
    std::vector<std::string> opt_fileNames;
    for (int i=2; i<argc; i++)
    {
        const char *opt = argv[i];
        if (strcmp(opt, "-V") == 0)
            opt_verbose = true;
        else if (strcmp(opt, "-r") == 0)
            opt_rebuild = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            {fprintf(stderr, "unknown option `%s'", opt);return 1;}
    }

    VectorFileIndexer indexer;
    int rc=0;
    for (int i=0; i<(int)opt_fileNames.size(); i++)
    {
        const char *fileName = opt_fileNames[i].c_str();
        if (opt_verbose) printf("indexing %s...\n", fileName);
        try
        {
            if (opt_rebuild)
                indexer.rebuildVectorFile(fileName);
            else
                indexer.generateIndex(fileName);
        }
        catch (std::exception& e) {
            fprintf(stderr, "Exception: %s\n", e.what());
            rc=1;
        }
    }

    if (opt_verbose) printf("done\n");

    return rc;
}

int main(int argc, char **argv)
{
    if (argc<2)
    {
        printUsage();
        exit(0);
    }

    const char *command = argv[1];
    if (!strcmp(command, "f") || !strcmp(command, "filter"))
        return filterCommand(argc, argv);
    else if (!strcmp(command, "s") || !strcmp(command, "summary"))
        return summaryCommand(argc, argv);
    else if (!strcmp(command, "i") || !strcmp(command, "info"))
        return infoCommand(argc, argv);
    else if (!strcmp(command, "x") || !strcmp(command, "index"))
        return indexCommand(argc, argv);
    else
        {fprintf(stderr, "unknown command `%s'", command);return 1;}
}


