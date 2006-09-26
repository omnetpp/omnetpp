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


void printUsage()
{
    fprintf(stderr,
       "scavetool -- part of OMNeT++/OMNEST, (C) 2006 Andras Varga\n"
       "Release: " OMNETPP_RELEASE ", edition: " OMNETPP_EDITION ".\n"
       "\n"
       "Usage: scavetool <command> [options] <files>...\n"
       "\n"
       "Input files are vector files (.vec) or scalar files (.sca).\n"
       "\n"
       "Commands:\n"
       "   f or filter:  filter data in input files\n"
       "   s or summary: prints summary info about input files\n"
       "   i or info:    prints list of available functions (to be used with `filter -a')\n"
       "Options:\n"
       "  `filter' command:\n"
       "    -n <pattern>:  filter for statistics name (see pattern syntax below)\n"
       "    -m <pattern>:  filter for module name\n"
       "    -r <pattern>:  filter for run Id\n"
       "    -c <pattern>:  filter for configuration Id (aka run number)\n"
       "    -f <pattern>:  filter for input file name (.vec or .sca)\n"
       "    -a <function>: apply the given processing to the vector (see syntax below)\n"
       "                   This option may occur multiple times.\n"
       "  `summary' command:\n"
       "    -n :  print list of unique statistics names\n"
       "    -m :  print list of unique module name\n"
       "    -r :  print list of unique run Ids\n"
       "    -c :  print list of unique configuration Ids (aka run numbers)\n"
       "  `info' command:\n"
       "    -b :  print filter names only (brief)\n"
       "\n"
       "Function syntax:\n"
       //TODO
       "  Examples\n"
       "    winavg(10)\n"
       "Pattern syntax: Glob-type patterns are accepted.\n"
       //TODO
    );
}

int main(int argc, char **argv)
{
    if (argc<2)
    {
        printUsage();
        exit(0);
    }

//TODO implement the above options...
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

