//=========================================================================
//  FILEREADERCONSUMER.CC - part of
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
#include <ios>
#include <fstream>
#include <omnetpp/platdep/platmisc.h>
#include <common/exception.h>
#include <omnetpp/platdep/timeutil.h>

using namespace omnetpp;

void runProducer(const char *filename, double duration, int numberOfLines)
{
    int sleep_usec = (duration / numberOfLines) * 1E6;
    if (sleep_usec >= 1000000)
        sleep_usec = 999999;

    std::ofstream outfile(filename, std::ios::out);
    for (int i = 0; i < numberOfLines; ++i) {
        outfile << i << std::endl;
        if (sleep_usec > 1000)
            usleep(sleep_usec);
    }
    outfile.close();
}

void usage(const char *message)
{
    if (message)
        fprintf(stderr, "Error: %s\n\n", message);

    fprintf(stderr, ""
                    "Usage:\n"
                    "   filereaderproducer <output-file-name> <duration> [<number-of-lines>]\n"
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
            const char *filename = argv[1];
            char *e;
            double duration = strtod(argv[2], &e);
            int numberOfLines = 1000;

            if (*e || duration < 0) {
                usage("<duration> should be a non-negative number");
                return -1;
            }

            if (argc >= 4) {
                numberOfLines = atoi(argv[3]);
            }

            runProducer(filename, duration, numberOfLines);

            return 0;
        }
    }
    catch (std::exception& e) {
        printf("FAIL: %s", e.what());

        return -2;
    }
}

