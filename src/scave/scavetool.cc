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


#include "../utils/ver.h"
#include "resultfilemanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"

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
       "Options:\n"
       "`filter' command:\n"
       "    -n <pattern>:    filter for statistics name (see pattern syntax below)\n"
       "    -m <pattern>:    filter for module name\n"
       "    -r <pattern>:    filter for run Id\n"
       "    -c <pattern>:    filter for configuration Id (aka run number)\n"
       "    -f <pattern>:    filter for input file name (.vec or .sca)\n"
       "    -a <function>:   apply the given processing to the vector (see syntax below)\n"
       "                     This option may occur multiple times.\n"
       "    -O <filename>:   output file name\n"
       "    -F <formatname>: format of output file: vec, sca, ...\n" //TODO
       "`summary' command:\n"
       "    -n :  print list of unique statistics names\n"
       "    -m :  print list of unique module name\n"
       "    -r :  print list of unique run Ids\n"
       "    -c :  print list of unique configuration Ids (aka run numbers)\n"
       "`info' command:\n"
       "    -b :  list filter names only (brief)\n"
       "    -s :  list filter names with parameter list (summary)\n"
       "    -v :  include descriptions in the output (default)\n"
       "\n"
       "Function syntax:\n"
       //TODO
       "  Examples:\n"
       "    winavg(10)\n"
       "Pattern syntax: Glob-type patterns are accepted.\n"
       //TODO
       "Examples:\n"
       " scavetool -n 'queueing time' -a winavg(10) -O out.vec\n\n" //TODO more
    );
}

int filterCommand(int argc, char **argv)
{
    //TODO implement...
    return 0;
}

int summaryCommand(int argc, char **argv)
{
    //TODO implement...
    ResultFileManager resultFileManager;
    for (int i=1; i<argc; i++)
    {
        printf("Loading result file %s...\n", argv[i]);
        try {
            ResultFile *f = resultFileManager.loadFile(argv[i]);
            if (!f)
                fprintf(stdout, "Error: load() returned null\n");
            else
                printf("Done - %d unrecognized lines out of %d\n", f->numUnrecognizedLines, f->numLines);
        } catch (Exception *e) {
            fprintf(stdout, "Exception: %s\n", e->message());
            delete e;
        }
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
    else
        {fprintf(stderr, "unknown command `%s'", command);return 1;}
}


