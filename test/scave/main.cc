//=========================================================================
//  VECTORFILEREADERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include "exception.h"

using namespace std;

void testReaderWriter(const char *inputfile, const char *outputfile);
void testReaderBuilder(const char *inputfile, const char *outputfile);
void testResultFileManager(const char *inputfile);

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
    cerr << "resultfilemanager <input-file>\n\n";
}

int main(int argc, char **argv)
{
    try {
        if (argc < 2) {
            usage("Not enough arguments specified");
            return -1;
        }
        else {
            if (strcmp(argv[1], "reader-writer") == 0)
            {
                if (argc < 4) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testReaderWriter(argv[2], argv[3]);
            }
            else if (strcmp(argv[1], "reader-builder") == 0)
            {
                if (argc < 4) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testReaderBuilder(argv[2], argv[3]);
            }
            else if (strcmp(argv[1], "resultfilemanager") == 0)
            {
                if (argc < 3) {
                    usage("Not enough arguments specified");
                    return -1;
                }
                testResultFileManager(argv[2]);
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
