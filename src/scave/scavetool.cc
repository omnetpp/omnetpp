//=========================================================================
//  SCAVETOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <sstream>
#include "../utils/ver.h"
#include "resultfilemanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
#include "vectorfilereader.h"
#include "vectorfilewriter.h"
#include "ivectorfilewriter.h"
#include "filternodes.h"
#include "filewriter.h"
#include "arraybuilder.h"
#include "octaveexport.h"


void printUsage()
{
    fprintf(stderr,
       "scavetool -- part of OMNeT++/OMNEST, (C) 2006 Andras Varga\n"
       "Release: " OMNETPP_RELEASE ", edition: " OMNETPP_EDITION ".\n"
       "\n"
       "Usage: scavetool <command> [options] <files>...\n"
       "\n"
       "For processing result files written by simulations: vector files (.vec) and\n"
       "scalar files (.sca).\n"
       "\n"
       "Commands:\n"
       "   f, filter:  filter data in input files\n"
       "   s, summary: prints summary info about input files\n"
       "   i, info:    prints list of available functions (to be used with `filter -a')\n"
       "   x, index:   generates index files for input files (rearranges records in input files)\n"
       "Options:\n"
       "`filter' command:\n"
       "    -n <pattern>    filter for statistics name (see pattern syntax below)\n"
       "    -m <pattern>    filter for module name\n"
       "    -r <pattern>    filter for run Id\n"
       "    -c <pattern>    filter for configuration Id (aka run number)\n"
       "    -f <pattern>    filter for input file name (.vec or .sca)\n"
       "    -a <function>   apply the given processing to the vector (see syntax below)\n"
       "                    This option may occur multiple times.\n"
       "    -O <filename>   output file name\n"   //FIXME separate file for vectors and scalars I guess
       "    -F <formatname> format of output file: vec, sca, ...\n" //TODO
       "    -V              print info about progress (verbose)\n"
       //TODO option: print matching vectorIDs and exit
       //TODO: dump scalars too!!!
       "`summary' command:\n"
       //TODO allow filtering by patterns here too?
       //TODO specifying more than one flag should list tuples e.g. (module,statistic) pairs
       // occurring in the input files
       "    -n   print list of unique statistics names\n"
       "    -m   print list of unique module name\n"
       "    -r   print list of unique run Ids\n"
       "    -c   print list of unique configuration Ids (aka run numbers)\n"
       "`info' command:\n"
       "    -b   list filter names only (brief)\n"
       "    -s   list filter names with parameter list (summary)\n"
       "    -v   include descriptions in the output (default)\n"
       "`index' command:\n"
       "    -V   print info about progress (verbose)\n"
       "\n"
       "Function syntax: name(parameterlist). Examples: winavg(10), mean()\n"
       "\n"
       "Pattern syntax: Glob-type patterns are accepted.\n"
       //TODO
       "Examples:\n"
       " scavetool -n 'queueing time' -a winavg(10) -O out.vec\n\n" //TODO more
    );
}

int filterCommand(int argc, char **argv)
{
    // options
    bool opt_verbose = false;
    std::string opt_statisticNamePattern;
    std::string opt_moduleNamePattern;
    std::string opt_runIdPattern;
    std::string opt_configurationIdPattern;
    std::string opt_filenamePattern = "*";  //XXX why is "" not equivalent??
    std::string opt_outputFileName;
    std::string opt_outputFormat;  //TBD vec, splitvec, octave, split octave (and for octave: x, y, both),...
    std::vector<std::string> opt_filterList;
    std::vector<std::string> opt_fileNames;
    StringMap opt_runAttrPatterns; //FIXME options to fill this

    //FIXME only exactly one of the next ones may be true
    bool opt_writeVectorFile = false;    //TODO create option for this
    bool opt_writeSeparateFiles = false; //TODO create option for this
    bool opt_writeOctaveFile = true; //TODO create option for this

    // parse options
    bool endOpts = false;
    for (int i=2; i<argc; i++)
    {
        const char *opt = argv[i];
        if (endOpts)
            opt_fileNames.push_back(argv[i]);
        else if (!strcmp(opt, "--"))
            endOpts = true;
        else if (!strcmp(opt, "-n") && i!=argc-1)
            opt_statisticNamePattern = argv[++i];
        else if (!strcmp(opt, "-m") && i!=argc-1)
            opt_moduleNamePattern = argv[++i];
        else if (!strcmp(opt, "-r") && i!=argc-1)
            opt_runIdPattern = argv[++i];
        else if (!strcmp(opt, "-c") && i!=argc-1)
            opt_configurationIdPattern = argv[++i];
        else if (!strcmp(opt, "-f") && i!=argc-1)
            opt_filenamePattern = argv[++i];
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
        else
            {fprintf(stderr, "unknown option `%s'", opt);return 1;}
    }

    try
    {
        // load files
        ResultFileManager resultFileManager;
        for (int i=0; i<opt_fileNames.size(); i++)
        {
            //TODO on Windows: manual globbing of wildcards
            const char *fileName = opt_fileNames[i].c_str();
            if (opt_verbose) printf("reading %s...", fileName);
            try {
                ResultFile *f = resultFileManager.loadFile(fileName);
                if (!f)
                {
                    if (opt_verbose) printf("\n");
                    fprintf(stderr, "Error: %s: load() returned null\n", fileName);
                }
                else if (f->numUnrecognizedLines>0)
                {
                    if (opt_verbose) printf("\n");
                    fprintf(stderr, "WARNING: %s: %d invalid/incomplete lines out of %d\n", fileName, f->numUnrecognizedLines, f->numLines);
                }
                else
                {
                    if (opt_verbose) printf(" %d lines\n", f->numLines);
                }
            } catch (Exception *e) {
                fprintf(stdout, "Exception: %s\n", e->message());
                delete e;
            }
        }

        // get matching fileRuns
        RunList runList = resultFileManager.filterRunList(
                            resultFileManager.getRuns(),
                            opt_runIdPattern.c_str(),
                            opt_runAttrPatterns);
        if (opt_verbose) printf("run filter matches %d runs\n", runList.size());

        ResultFileList fileList = resultFileManager.filterFileList(
                            resultFileManager.getFiles(),
                            opt_filenamePattern.c_str());
        if (opt_verbose) printf("filename filter matches %d files\n", fileList.size());

        FileRunList fileRunList = resultFileManager.getFileRuns(&fileList, &runList);
        if (opt_verbose) printf("total %d matching file-runs\n", fileRunList.size());

        // filter statistics by module and name
        IDList vectorIDList = resultFileManager.filterIDList(
                            resultFileManager.getAllVectors(),
                            &fileRunList,
                            opt_moduleNamePattern.c_str(),
                            opt_statisticNamePattern.c_str());
        IDList scalarIDList = resultFileManager.filterIDList(
                            resultFileManager.getAllScalars(),
                            &fileRunList,
                            opt_moduleNamePattern.c_str(),
                            opt_statisticNamePattern.c_str());
        if (opt_verbose) printf("module and name filter matches %d vectors and %d scalars\n",
                            vectorIDList.size(), scalarIDList.size());

        if (opt_verbose) printf("done collecting inputs\n\n");

        //
        // assemble dataflow network for vectors
        //
        DataflowManager dataflowManager;

        // create filereader for each vector file
        if (opt_verbose) printf("creating vector file reader(s)\n");
        ResultFileList& filteredVectorFileList = *resultFileManager.getUniqueFiles(vectorIDList); //FIXME delete after done?
        std::map<ResultFile*, VectorFileReaderNode*> vectorFileReaders;
        for (int i=0; i<filteredVectorFileList.size(); i++)
        {
            ResultFile *resultFile = filteredVectorFileList[i];
            VectorFileReaderNode *readerNode = new VectorFileReaderNode(resultFile->fileSystemFilePath.c_str(), 64*1024);
            vectorFileReaders[resultFile] = readerNode;
            dataflowManager.addNode(readerNode);
        }

        // create a port for each vector on its reader node
        std::vector<Port*> vectorPorts;
        for (int i=0; i<vectorIDList.size(); i++)
        {
             const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
             assert(vectorFileReaders.find(vector.fileRunRef->fileRef) != vectorFileReaders.end());
             VectorFileReaderNode *readerNode = vectorFileReaders[vector.fileRunRef->fileRef];
             vectorPorts.push_back(readerNode->addVector(vector.vectorId));
        }

        // add filters
        for (int k=0; k<opt_filterList.size(); k++)
        {
//TODO support filter to merge all into a single vector
            if (opt_verbose) printf("adding filter to each vector: %s\n", opt_filterList[k].c_str());
            for (int i=0; i<vectorIDList.size(); i++)
            {
                 Node *node = NodeTypeRegistry::instance()->createNode(opt_filterList[k].c_str(), &dataflowManager);
                 FilterNode *filterNode = dynamic_cast<FilterNode *>(node);
                 if (!filterNode)
                     throw new Exception("%s is not a filter node", opt_filterList[k].c_str());
                 dataflowManager.connect(vectorPorts[i], &(filterNode->in));
                 vectorPorts[i] = &(filterNode->out);
            }
        }

        // eventually, connect ports to writer node
        std::vector<ArrayBuilderNode*> arrayBuilders; // for Octave output
        if (opt_writeVectorFile)
        {
            // everything goes to a common vector file
            if (opt_verbose) printf("adding vector file writer\n");
            VectorFileWriterNode *writerNode = new VectorFileWriterNode("_out_.vec", "# generated by scavetool"); //FIXME for now
            dataflowManager.addNode(writerNode);
            for (int i=0; i<vectorIDList.size(); i++)
            {
                const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
                Port *writerNodePort = writerNode->addVector(i, vector.moduleNameRef->c_str(), vector.nameRef->c_str()); // vectors get renumbered
                dataflowManager.connect(vectorPorts[i], writerNodePort);
            }
        }
        else if (opt_writeSeparateFiles)
        {
            // every vector goes to its own file, with two columns (time+value) separated by spaces/tab
            if (opt_verbose) printf("adding separate writers for each vector\n");
            for (int i=0; i<vectorIDList.size(); i++)
            {
                char buf[16];
                std::string fname = std::string("_out_")+itoa(i,buf,10)+".out";
                const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));

                std::stringstream header;
                header << "# vector " << vector.vectorId << " \"" << vector.moduleNameRef->c_str() << " \" \"" << vector.nameRef->c_str() << "\"\n";
                header << "# file generated by scavetool\n";

                FileWriterNode *writerNode = new FileWriterNode(fname.c_str(), header.str().c_str());
                dataflowManager.addNode(writerNode);
                dataflowManager.connect(vectorPorts[i], &(writerNode->in));
            }
        }
        else if (opt_writeOctaveFile)
        {
            // for Octave, we must build arrays
            if (opt_verbose) printf("adding array builders for Octave output\n");
            for (int i=0; i<vectorIDList.size(); i++)
            {
                ArrayBuilderNode *arrayBuilderNode = new ArrayBuilderNode();
                dataflowManager.addNode(arrayBuilderNode);
                dataflowManager.connect(vectorPorts[i], &(arrayBuilderNode->in));
                arrayBuilders.push_back(arrayBuilderNode);
            }
        }
        else
        {
            //XXX error - no output method
        }

        // run!
        if (opt_verbose) printf("running dataflow network...\n");
        dataflowManager.execute();

        if (opt_writeOctaveFile)
        {
            // here we have to actually save it
            OctaveExport exporter("data.octave"); //XXX filename
            for (int i=0; i<vectorIDList.size(); i++)
            {
                const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
                std::string uniqueName = exporter.makeUniqueName(vector.nameRef->c_str());

                std::string descr = *vector.nameRef + "; "
                                  + *vector.moduleNameRef + "; "
                                  + vector.fileRunRef->fileRef->fileSystemFilePath + "; "
                                  + vector.fileRunRef->runRef->runName;
                XYArray *xyArray = arrayBuilders[i]->getArray();
                exporter.saveVector(uniqueName.c_str(), descr.c_str(), xyArray);
                delete xyArray;
            }
            exporter.close();
        }

        if (opt_verbose) printf("done\n");
    }
    catch (Exception *e)
    {
        fprintf(stdout, "Exception: %s\n", e->message());
        delete e;
        return 1;
    }

    return 0;
}

int summaryCommand(int argc, char **argv)
{
    //TODO implement...
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

    NodeTypeRegistry *registry = NodeTypeRegistry::instance();
    NodeTypeVector nodeTypes = registry->getNodeTypes();
    for (int i=0; i<nodeTypes.size(); i++)
    {
        NodeType *nodeType = nodeTypes[i];
        if (nodeType->isHidden())
            continue;

        if (opt_brief)
        {
            // this is the -b format
            printf("%s\n", nodeType->name());
        }
        else
        {
            // print name(parameters,...)
            printf("%s", nodeType->name());
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
                printf(":  %s\n", nodeType->description());
                for (StringMap::iterator it=attrs.begin(); it!=attrs.end(); ++it)
                {
                    printf("   - %s: %s\n", it->first.c_str(), it->second.c_str());
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
    char *end;
    bool opt_verbose = false;
    std::vector<std::string> opt_fileNames;
    for (int i=2; i<argc; i++)
    {
        const char *opt = argv[i];
        if (!strcmp(opt, "-V"))
            opt_verbose = true;
        else if (opt[0] != '-')
            opt_fileNames.push_back(argv[i]);
        else
            {fprintf(stderr, "unknown option `%s'", opt);return 1;}
    }

    try
    {
        // load files
        ResultFileManager resultFileManager;
        for (int i=0; i<opt_fileNames.size(); i++)
        {
            //TODO on Windows: manual globbing of wildcards
            const char *fileName = opt_fileNames[i].c_str();
            if (opt_verbose) printf("reading %s...", fileName);
            try {
                ResultFile *f = resultFileManager.loadFile(fileName); // TODO: limit number of lines read
                if (!f)
                {
                    if (opt_verbose) printf("\n");
                    fprintf(stderr, "Error: %s: load() returned null\n", fileName);
                }
                else if (f->numUnrecognizedLines>0)
                {
                    if (opt_verbose) printf("\n");
                    fprintf(stderr, "WARNING: %s: %d invalid/incomplete lines out of %d\n", fileName, f->numUnrecognizedLines, f->numLines);
                }
                else
                {
                    if (opt_verbose) printf(" %d lines\n", f->numLines);
                }
            } catch (Exception *e) {
                fprintf(stdout, "Exception: %s\n", e->message());
                delete e;
            }
        }


        //
        // assemble dataflow network for vectors
        //
        DataflowManager dataflowManager;

        // create filereader for each vector file
        if (opt_verbose) printf("creating vector file reader(s)\n");
        NodeType *readerNodeType=NodeTypeRegistry::instance()->getNodeType("vectorfilereader");
        std::map<std::string, VectorFileReaderNode*> vectorFileReaders;
        StringMap attrs;
        for (int i=0; i<opt_fileNames.size(); i++)
        {
            std::string fileName = opt_fileNames[i];
            attrs["filename"] = fileName;
            vectorFileReaders[fileName] = (VectorFileReaderNode*)readerNodeType->create(&dataflowManager, attrs);
        }

        // create filewriter for each vector file
        if (opt_verbose) printf("creating vector file writer(s)\n");
        NodeType *writerNodeType=NodeTypeRegistry::instance()->getNodeType("indexedvectorfilewriter");
        std::map<std::string, IndexedVectorFileWriterNode*> vectorFileWriters;
        attrs.clear();
        attrs["fileheader"]="# generated by scavetool";
        attrs["blocksize"]="65536";
        for (int i=0; i<opt_fileNames.size(); i++)
        {
            std::string fileName = opt_fileNames[i];
            attrs["filename"]=fileName;
            vectorFileWriters[fileName] = (IndexedVectorFileWriterNode*)writerNodeType->create(&dataflowManager, attrs);
        }
        
        // create a ports for each vector on its reader node and writer node and connect them
        if (opt_verbose) printf("connecting readers with writers\n");
        IDList vectorIDList = resultFileManager.getAllVectors();
        for (int i=0; i<vectorIDList.size(); i++)
        {
             const VectorResult& vector = resultFileManager.getVector(vectorIDList.get(i));
             std::string fileName = vector.fileRunRef->fileRef->fileSystemFilePath;
             assert(vectorFileReaders.find(fileName) != vectorFileReaders.end());
             VectorFileReaderNode *readerNode = vectorFileReaders[fileName];
             Port *readerPort = readerNode->addVector(vector.vectorId);
             IndexedVectorFileWriterNode *writerNode = vectorFileWriters[fileName];
             Port *writerPort = writerNode->addVector(vector.vectorId, *(vector.moduleNameRef), *(vector.nameRef));
             dataflowManager.connect(readerPort, writerPort);
        }

        // run!
        if (opt_verbose) printf("running dataflow network...\n");
        dataflowManager.execute();
        if (opt_verbose) printf("done\n");
    }
    catch (Exception *e)
    {
        fprintf(stdout, "Exception: %s\n", e->message());
        delete e;
        return 1;
    }

    return 0;
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


