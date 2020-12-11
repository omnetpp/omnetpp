//=========================================================================
//  FILEREADERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <common/lcgrandom.h>
#include <common/exception.h>
#include <common/filereader.h>

#ifdef _WIN32
#include <io.h>
#else
#define _setmode(a, b)
#endif

using namespace omnetpp;
using namespace omnetpp::common;

int parseLineNumber(char *line)
{
    return !line || *line == '\r' || *line == '\n' ? -1 : atol(line);
}

void testFileReader(const char *file, long numberOfLines, int numberOfSeeks, int numberOfReadLines)
{
    _setmode(_fileno(stdout), _O_BINARY);
    FileReader fileReader(file);
    LCGRandom random;
    int64_t fileSize = fileReader.getFileSize();

    while (numberOfSeeks--) {
        file_offset_t offset;

        if (numberOfSeeks == 1)
            offset = fileReader.getFileSize();
        else if (numberOfSeeks == 2)
            offset = 0;
        else
            offset = random.next01() * fileSize;

        printf("Seeking to offset: %" PRId64 "\n", offset);
        fileReader.seekTo(offset);

        int j = numberOfReadLines;
        long expectedLineNumber = -1;
        bool forward;

        while (j--) {
            char *line;

            // either read forward or backward a line
            if (random.next01() < 0.5) {
                line = fileReader.getPreviousLineBufferPointer();

                if (line) {
                    printf("Read previous line: %.*s", (int)fileReader.getCurrentLineLength(), line);

                    // calculate expected line number based on previous expected if any
                    if (expectedLineNumber != -1) {
                        if (!forward)
                            expectedLineNumber--;
                    }
                    else
                        expectedLineNumber = parseLineNumber(line);
                }
                else {
                    if (expectedLineNumber != -1 &&
                        expectedLineNumber != 0)
                        throw opp_runtime_error("*** No more previous lines (%ld) but not at the very beginning of the file\n", expectedLineNumber);
                }

                forward = false;
            }
            else {
                line = fileReader.getNextLineBufferPointer();

                if (line) {
                    printf("Read next line: %.*s", (int)fileReader.getCurrentLineLength(), line);

                    // calculate expected line number based on previous expected if any
                    if (expectedLineNumber != -1) {
                        if (forward)
                            expectedLineNumber++;
                    }
                    else
                        expectedLineNumber = parseLineNumber(line);
                }
                else {
                    if (expectedLineNumber != -1 &&
                        expectedLineNumber != numberOfLines - 1)
                        throw opp_runtime_error("*** No more next lines (%ld) but not at the very end of the file\n", expectedLineNumber);
                }

                forward = true;
            }

            // compare expected and actual line numbers
            long lineNumber = parseLineNumber(line);
            if (lineNumber != -1 && lineNumber != expectedLineNumber)
                throw opp_runtime_error("*** Line number %ld, %ld comparison failed for line: %.*s", lineNumber, expectedLineNumber, fileReader.getCurrentLineLength(), line);
        }
    }
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
                    "Usage:\n"
                    "   filereadertest <input-file-name> <number-of-lines> <number-of-seeks> <number-of-read-lines-per-seek>\n"
            );
}

int main(int argc, char **argv)
{
    try {
        if (argc < 5) {
            usage("Not enough arguments specified");

            return -1;
        }
        else {
            testFileReader(argv[1], atol(argv[2]), atoi(argv[3]), atoi(argv[4]));
            printf("PASS\n");

            return 0;
        }
    }
    catch (std::exception& e) {
        printf("FAIL: %s", e.what());

        return -2;
    }
}

