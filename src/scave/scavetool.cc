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

#include "resultfilemanager.h"

//XXX currently doesn't do anything useful, only for testing the lib

int main(int argc, char **argv)
{
    if (argc<2)
    {
        fprintf(stderr, "%s: Usage: scavetool <resultfiles>\n", argv[0]);
        exit(0);
    }

    ResultFileManager resultFileManager;
    for (int i=1; i<argc; i++)
    {
        fprintf(stdout, "Loading result file %s...\n", argv[i]);
        try {
            File *f = resultFileManager.loadFile(argv[i]);
            if (!f)
                fprintf(stdout, "Error: load() returned null\n");
        } catch (Exception *e) {
            fprintf(stdout, "Exception: %s\n", e->message());
            delete e;
        }
    }
    return 0;
}
