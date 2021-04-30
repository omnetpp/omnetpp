#include <omnetpp/platdep/platmisc.h>
#include "testutil.h"

namespace testlib {

void printEnum(const char *enumname)
{
    cEnum *e = cEnum::find(enumname);
    ASSERT(e != nullptr);
    ASSERT(strcmp(e->getFullName(), enumname) == 0);
    EV << e->getFullName() << ": " << e->str() << "\n";
}

bool isRunningTest(const char *testname)
{
    // exploit the fact that every test case is run in its own extraction directory whose name is the test name
    char buf[2048];
    return strstr(getcwd(buf,sizeof(buf)), testname) != nullptr;
}

}
