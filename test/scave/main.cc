// =========================================================================
//  VECTORFILEREADERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
// =========================================================================

/*--------------------------------------------------------------*
   Copyright (C) 1992-2015 Andras Varga

   This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <common/exception.h>

using namespace omnetpp;
using namespace std;

void testReaderWriter(const char *inputfile, const char *outputfile);
void testReaderBuilder(const char *inputfile, const char *outputfile);
void testResultFileManager(const char *inputfile);
void testIndexer(const char *inputFile);
void testReader(const char *readerNodeType, const char *inputFile, int *vectorIds, int count);

static void usage(char *message)
{
    if (message)
        cerr << "Error: " << message << "\n\n";

    cerr << "Usage:\n";
    cerr << "   test <test-name> <args>...\n";
    cerr << "\n";
    cerr << "Tests are:\n\n";
    cerr << "reader-writer <input-file> <output-file>\n";
    cerr << "reader-builder <input-file> <output-file>\n";
    cerr << "resultfilemanager <input-file>\n";
    cerr << "indexer <input-file>\n";
    cerr << "reader <input-file> <vector-id-list>\n";
    cerr << "indexedreader <input-file> <vector-id-list>\n\n";
}

static void parseIntList(const char *str, int *& result, int& len)
{
    result = nullptr;
    len = 0;

    vector<int> ids;

    int start, end;
    int num = 0;
    const char *ptr = str;

    while (*ptr) {
        if (!isdigit(*ptr))
            throw exception("Malformed interval start");

        start = 0;
        while (isdigit(*ptr))
            start = 10 * start + (*(ptr++) - '0');

        if (*ptr == ',' || *ptr == '\0') {
            end = start;
        }
        else if (*ptr == '-') {
            if (!isdigit(*(++ptr)))
                throw exception("Missing interval end");

            end = 0;
            while (isdigit(*ptr))
                end = 10 * end + (*(ptr++) - '0');
        }
        else if (*ptr)
            throw exception("Unexpected char in int list");

        if (start <= end) {
            for (int id = start; id <= end; ++id)
                ids.push_back(id);
        }
        else {
            for (int id = start; id >= end; --id)
                ids.push_back(id);
        }

        if (*ptr == ',')
            ptr++;
    }

    result = new int[ids.size()];
    for (int i = 0; i < ids.size(); ++i)
        result[i] = ids[i];
    len = ids.size();
}

int main(int argc, char **argv)
{
    try {
        if (argc < 2) {
            usage("Not enough arguments specified");
            return -1;
        }
        else {
            if (strcmp(argv[1], "reader-writer") == 0) {
                if (argc < 4) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testReaderWriter(argv[2], argv[3]);
            }
            else if (strcmp(argv[1], "reader-builder") == 0) {
                if (argc < 4) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testReaderBuilder(argv[2], argv[3]);
            }
            else if (strcmp(argv[1], "resultfilemanager") == 0) {
                if (argc < 3) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testResultFileManager(argv[2]);
            }
            else if (strcmp(argv[1], "indexer") == 0) {
                if (argc < 3) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testIndexer(argv[2]);
            }
            else if (strcmp(argv[1], "indexedvectorfilereader") == 0 ||
                     strcmp(argv[1], "indexedvectorfilereader2") == 0 ||
                     strcmp(argv[1], "vectorfilereader") == 0)
            {
                if (argc < 4) {
                    usage("Not enough arguments specified");
                    return -1;
                }

                int *vectorIds, count;
                parseIntList(argv[3], vectorIds, count);
                if (count == 0) {
                    usage("No vectors specified");
                    return -1;
                }

                testReader(argv[1], argv[2], vectorIds, count);
            }

            cout << "PASS\n";
            return 0;
        }
    }
    catch (std::exception& e) {
        cout << "FAIL: " << e.what();
        return -2;
    }
}

