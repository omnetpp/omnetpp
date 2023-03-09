#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <omnetpp.h>

using namespace omnetpp;

// Assertion that is not only is effective in DEBUG mode.
#define TEST_ASSERT(expr) \
  ((void) ((expr) ? 0 : (throw omnetpp::cRuntimeError("TEST_ASSERT: Condition '%s' does not hold in function '%s' at %s:%d", \
                                   #expr, __FUNCTION__, __FILE__, __LINE__), 0)))


namespace testlib {
}

#endif
