#include <assert.h>
#include <stdio.h>

#include "engine/resultfilemanager.h"
#include "engine/datasorter.h"

void printData(ResultFileManager *mgr, IDVector *ids)
{
    printf("========================================\n");
    printf("%d items:\n", ids->size());
    for (int i=0; i<ids->size(); i++)
    {
        const ResultItem& d = mgr->getItem(ids->at(i));
        printf(" %s %s %s\n", d.runRef->fileAndRunName.c_str(), d.moduleNameRef->c_str(), d.nameRef->c_str());
    }
}

void printRunList(RunList& l)
{
    printf("========================================\n");
    printf("%d runs: ", l.size());
    for (RunList::iterator i=l.begin(); i!=l.end(); i++)
    {
        Run *r = (*i);
        printf(" %s,", r->fileAndRunName.c_str());
    }
    printf("\n");
}

void printStringSet(StringSet& set)
{
    printf("========================================\n");
    printf("%d items: ", set.size());
    for (StringSet::iterator i=set.begin(); i!=set.end(); i++)
        printf(" %s,", (*i).c_str());
    printf("\n");
}

main()
{
   try {
        ResultFileManager *mgr = new ResultFileManager();

        mgr->loadScalarFile("demo.sca");
        mgr->loadScalarFile("scatter.sca");

        IDVector *v = mgr->getAllScalars();
        printData(mgr, v);

        RunList *runs = mgr->getUniqueRuns(*v);
        StringSet *modNames = mgr->getUniqueModuleNames(*v);
        StringSet *names = mgr->getUniqueNames(*v);

        printRunList(*runs);
        printStringSet(*modNames);
        printStringSet(*names);

        IDVector *v2 = ScalarDataSorter(mgr).getFilteredList(*v, "*", "*", "*frames");
        printData(mgr, v2);
    }
    catch (Exception *ex)
    {
        printf("exception: %s", ex->message());
        delete ex;
    }

    return 0;
}


