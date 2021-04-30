#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

void printEnum(const char *enumname);

bool isRunningTest(const char *testname);

}

#endif
