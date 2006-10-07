#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine/resultfilemanager.h"

main()
{
    printf("started\n");
    ResultFileManager manager;

    printf("loading\n");
    manager.loadScalarFile("kicsi.sca");

    printf("getAllScalars\n");
    IDList all = manager.getAllScalars();

    //printf("getFilteredList\n");
    //IDList sel = manager.getFilteredList(all, "*", "*", "*");

    printf("sortByModule\n");
    all.sortByModule(&manager, true);

    printf("sortByValue\n");
    all.sortScalarsByValue(&manager, true);

    printf("sortByFileName\n");
    all.sortByFileName(&manager, true);

    return 0;
}


