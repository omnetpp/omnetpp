//=========================================================================
//  FILEECHOTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <common/exception.h>
#include <common/filereader.h>

#ifdef _WIN32
#include <io.h>
#else
#define _setmode(a, b)
#endif

using namespace omnetpp;
using namespace omnetpp::common;

void testFileEcho(const char *file, bool forward)
{
    _setmode(_fileno(stdout), _O_BINARY);
    FileReader fileReader(file);

    if (forward)
        fileReader.seekTo(0);
    else
        fileReader.seekTo(fileReader.getFileSize());

    char *line;

    while ((line = (forward ? fileReader.getNextLineBufferPointer() : fileReader.getPreviousLineBufferPointer())) != nullptr)
        printf("%.*s", (int)fileReader.getCurrentLineLength(), line);
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
                    "Usage:\n"
                    "   fileechotest <input-file-name> (forward|backward)\n"
            );
}

int main(int argc, char **argv)
{
    try {
        if (argc < 3) {
            usage("Not enough arguments specified");
            return -1;
        }
        else {
            testFileEcho(argv[1], strcmp(argv[2], "backward"));
            return 0;
        }
    }
    catch (std::exception& e) {
        printf("FAIL: %s", e.what());
        return -2;
    }
}

