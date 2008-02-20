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
#include <io.h>
#include "exception.h"
#include "filereader.h"

void testFileEcho(const char *file, bool forward)
{
    _setmode(_fileno(stdout), _O_BINARY);
    FileReader fileReader(file);

    if (forward)
        fileReader.seekTo(0);
    else
        fileReader.seekTo(fileReader.getFileSize());

    char *line;

    while (line = forward ? fileReader.getNextLineBufferPointer() : fileReader.getPreviousLineBufferPointer())
        printf("%.*s", fileReader.getCurrentLineLength(), line);
}

void usage(char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"Usage:\n"
"   filereadertest <input-file-name> (forward|backward)\n"
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
