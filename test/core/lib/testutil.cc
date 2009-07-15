#include "testutil.h"

void printEnum(const char *enumname)
{
    cEnum *e = cEnum::find(enumname);
    ASSERT(e!=NULL);
    ASSERT(strcmp(e->getFullName(), enumname)==0);
    ev << e->getFullName() << ": " << e->info() << "\n";
}
