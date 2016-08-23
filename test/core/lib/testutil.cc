#include "testutil.h"

namespace testlib {

void printEnum(const char *enumname)
{
    cEnum *e = cEnum::find(enumname);
    ASSERT(e != nullptr);
    ASSERT(strcmp(e->getFullName(), enumname) == 0);
    EV << e->getFullName() << ": " << e->str() << "\n";
}

}
