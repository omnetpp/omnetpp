//=========================================================================
//  EVENTLOGTEST.CC - part of
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
#include "lcgrandom.h"
#include "filereader.h"

int testFileReader(const char *file, long numberOfLines)
{
    _setmode(_fileno(stdout), _O_BINARY);
    FileReader fileReader(file);
    LCGRandom random;
    int i = 1000;
    int64 fileSize = fileReader.getFileSize();

    while (i--) {
        int64 offset = random.next01() * fileSize;
        printf("Seeking to offset: %lld\n", offset);
        fileReader.seekTo(offset);

        int j = 32;
        long expectedLineNumber = -1;
        bool forward;

        while (j--) {
            char *line;

            if (random.next01() < 0.5) {
                line = fileReader.readPreviousLine();

                if (line) {
                    printf("Read previous line: %.*s", fileReader.getLastLineLength(), line);

                    if (expectedLineNumber != -1) {
                        if (!forward)
                            expectedLineNumber--;
                    }
                    else
                        expectedLineNumber = atol(line);
                }
                else {
                    if (expectedLineNumber != -1 &&
                        expectedLineNumber != 0) {
                        printf("*** No more previous lines but not at the very beginning of the file");
                        return -1;
                    }
                }

                forward = false;
            }
            else {
                line = fileReader.readNextLine();

                if (line) {
                    printf("Read next line: %.*s", fileReader.getLastLineLength(), line);

                    if (expectedLineNumber != -1) {
                        if (forward)
                            expectedLineNumber++;
                    }
                    else
                        expectedLineNumber = atol(line);
                }
                else {
                    if (expectedLineNumber != -1 &&
                        expectedLineNumber != numberOfLines - 1) {
                        printf("*** No more next lines but not at the very end of the file");
                        return -2;
                    }
                }

                forward = true;
            }

            long lineNumber = !line || *line == '\r' || *line == '\n'? -1 : atol(line);

            if (lineNumber != -1 && lineNumber != expectedLineNumber) {
                printf("*** Line number %ld, %ld comparison failed for line: %.*s", lineNumber, expectedLineNumber, fileReader.getLastLineLength(), line);
                return -3;
            }
        }
    }

    return 0;
}

int usage(char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
"Usage:\n"
"   filereadertest <input-file-name> <number-of-lines>\n"
);

    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3)
        return usage("Not enough arguments specified");
    else
        return testFileReader(argv[1], atol(argv[2]));
}
