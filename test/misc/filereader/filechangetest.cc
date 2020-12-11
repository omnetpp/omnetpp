//=========================================================================
//  FILEECHOTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "common/exception.h"
#include "common/filereader.h"

#ifdef _WIN32
#include <io.h>
#else
#define _setmode(a,b)
#endif

using namespace omnetpp::common;

void testFileChange(const char *fileName)
{
    int i = 0;
    FileReader fileReader(fileName);
    fileReader.ensureFileOpen();
    FileReader::FileChange change;
    while (getchar() != EOF) {
        switch (change = fileReader.getFileChange()) {
            case FileReader::UNCHANGED:
                printf("Unchanged (%d)\n", i++);
                break;
            case FileReader::APPENDED:
                printf("Appended (%d)\n", i++);
                break;
            case FileReader::OVERWRITTEN:
                printf("Overwritten (%d)\n", i++);
                break;
            default:
                throw opp_runtime_error("Unknown file change: %d", change);
        }
        fileReader.synchronize(change);
    }
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"Usage:\n"
"   filechangetest <input-file-name>\n"
);
}

int main(int argc, char **argv)
{
    try {
        if (argc < 2) {
            usage("Not enough arguments specified");

            return -1;
        }
        else {
            testFileChange(argv[1]);

            return 0;
        }
    }
    catch (std::exception& e) {
        printf("FAIL: %s", e.what());

        return -2;
    }
}
