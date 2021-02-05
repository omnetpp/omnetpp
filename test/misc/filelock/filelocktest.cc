//=========================================================================
//  FILELOCKTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include "omnetpp/platdep/platmisc.h"
#include "common/filereader.h"
#include "common/exception.h"
#include "common/filelock.h"
#include "common/exception.h"

#define RUN_TIME 3
#define LINE_SIZE 6666
#define MAX_LINE_COUNT 100
#define MAX_FILE_SIZE (LINE_SIZE * MAX_LINE_COUNT)
#define FILE_NAME "filelocktest.txt"

using namespace omnetpp::common;

void reader(bool lock)
{
    printf("Reader running\n");
    char buffer[MAX_FILE_SIZE];
    long begin = (long)clock();
    FILE *file = fopen(FILE_NAME, "rb");
    if (!file)
        throw opp_runtime_error("Cannot open file");
    FileLock fileLock(file, FILE_NAME);
    while ((long)clock() - begin < CLOCKS_PER_SEC * RUN_TIME) {
        FileLockAcquirer fileLockAcquirer(&fileLock, FILE_LOCK_SHARED, lock);
        fseek(file, 0, SEEK_SET);
        if (ferror(file))
            throw opp_runtime_error("Cannot seek in file '%s', error code %d", FILE_NAME, ferror(file));
        int byteSize = fread(buffer, 1, MAX_FILE_SIZE, file);
        if (ferror(file))
            throw opp_runtime_error("Read error in file '%s', error code %d", FILE_NAME, ferror(file));
        for (int i = 0; i < byteSize / LINE_SIZE - 1; i++) {
            for (int j = 0; j < LINE_SIZE - 1; j++) {
                if (buffer[i * LINE_SIZE] != buffer[i * LINE_SIZE + j]) {
                    printf("Error at %d in line: %.*s\n", j, LINE_SIZE - 1, buffer + i * LINE_SIZE);
                    break;
                }
            }
            char c1 = buffer[i * LINE_SIZE];
            char c2 = buffer[(i + 1) * LINE_SIZE];
            if (i && (c1 + 1 - 65) % 26 != (c2 - 65) % 26)
                throw opp_runtime_error("Error in line %d and line %d, values %d and %d", i, i + 1, c1, c2);
        }
    }
    fclose(file);
    printf("Reader finished\n");
}

void writer(bool lock)
{
    printf("Writer running\n");
    char buffer[LINE_SIZE];
    long begin = (long)clock();
    int lineCount = 0;
    buffer[LINE_SIZE - 1] = '\n';
    FILE *file = fopen(FILE_NAME, "r+b"); // we must be able to read/write existing files
    if (!file) {
        file = fopen(FILE_NAME, "w+b");
        if (!file)
            throw opp_runtime_error("Cannot open file");
    }
    FileLock fileLock(file, FILE_NAME);
    while ((long)clock() - begin < CLOCKS_PER_SEC * RUN_TIME) {
        lineCount = lineCount % MAX_LINE_COUNT + 1;
        int base = rand();
        FileLockAcquirer fileLockAcquirer(&fileLock, FILE_LOCK_EXCLUSIVE, lock);
        fseek(file, 0, SEEK_SET);
        if (ferror(file))
            throw opp_runtime_error("Cannot seek in file '%s', error code %d", FILE_NAME, ferror(file));
        for (int i = 0; i < lineCount; i++) {
            memset(buffer, 65 + (i + base) % 26, LINE_SIZE - 1);
            fwrite(buffer, 1, LINE_SIZE, file);
            if (ferror(file))
                throw opp_runtime_error("Cannot write to file '%s', error code %d", FILE_NAME, ferror(file));
        }
        // TODO: non portable utime(FILE_NAME, NULL);
        opp_ftruncate(fileno(file), LINE_SIZE * lineCount);
        fflush(file);
    }
    fclose(file);
    printf("Writer finished\n");
}

void filereader(bool lock)
{
    printf("Filereader running\n");
    long begin = (long)clock();
    FileReader fileReader(FILE_NAME);
    fileReader.setFileLocking(lock);
    while ((long)clock() - begin < CLOCKS_PER_SEC * RUN_TIME) {
        try {
            char previousCharacter = -1;
            const char *line;
            fileReader.seekTo(0);
            while ((line = fileReader.getNextLineBufferPointer()) != NULL) {
                char character = line[0];
                for (int i = 0; i < LINE_SIZE - 1; i++)
                    if (line[0] != line[i])
                        throw opp_runtime_error("Error at %d in line: %.*s\n", i, LINE_SIZE - 1, line);
                if (previousCharacter != -1 && (previousCharacter + 1 - 65) % 26 != (character - 65) % 26)
                    throw opp_runtime_error("Error in lines, values %d and %d", previousCharacter, character);
                previousCharacter = character;
            }
        }
        catch (FileChangedError& e) {
            printf("Reader caught file changed error, trying again\n");
        }
    }
    printf("Filereader finished\n");
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);
    fprintf(stderr, ""
"Usage:\n"
"   filelocktest reader|writer|filereader lock|nolock\n"
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
            bool lock = !strcmp("lock", argv[2]);
            if (!strcmp("reader", argv[1]))
                reader(lock);
            else if (!strcmp("writer", argv[1]))
                writer(lock);
            else if (!strcmp("filereader", argv[1]))
                filereader(lock);
            else
                throw opp_runtime_error("Unknown argument specified");
            return 0;
        }
    }
    catch (std::exception& e) {
        printf("Exception caught: %s\n", e.what());
        return -2;
    }
}
