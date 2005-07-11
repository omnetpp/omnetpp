//=========================================================================
//  SCALARMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "patmatch.h"
#include "filtering.h"


void getFilteredScalarList(const ScalarManager& scalarMgr,
               ScalarManager::RunRef runRef, const char *moduleName, const char *scalarName,
               ScalarManager::IntVector& outVec)
{
    ScalarManager::StringRef null = NULL;
    ScalarManager::StringRef moduleNameRef = moduleName ? scalarMgr.getModuleNames().find(moduleName) : null;
    ScalarManager::StringRef scalarNameRef = scalarName ? scalarMgr.getScalarNames().find(scalarName) : null;

    const ScalarManager::Values& scalars = scalarMgr.getValues();
    int k=0;
    for (ScalarManager::Values::const_iterator i=scalars.begin(); i!=scalars.end(); i++,k++)
    {
        const ScalarManager::Datum& d = *i;
        if ((runRef==NULL || d.runRef==runRef) &&
            (moduleNameRef==null || d.moduleNameRef==moduleNameRef) &&
            (scalarNameRef==null || d.scalarNameRef==scalarNameRef)
           )
        {
            outVec.push_back(k);
        }
    }
}


void getFilteredScalarList2(const ScalarManager& scalarMgr,
               const char *fileAndRunFilter,
               const char *moduleFilter,
               const char *nameFilter,
               ScalarManager::IntVector& outVec)
{
    // prepare for wildcard matches
    bool patMatchFile = contains_wildcards(fileAndRunFilter);
    bool patMatchModule = contains_wildcards(moduleFilter);
    bool patMatchName = contains_wildcards(nameFilter);

    short *filePattern = NULL;
    if (patMatchFile)
    {
        filePattern = new short[512]; // FIXME!
        transform_pattern(fileAndRunFilter, filePattern);
    }

    short *modulePattern = NULL;
    if (patMatchModule)
    {
        modulePattern = new short[512]; // FIXME!
        transform_pattern(moduleFilter, modulePattern);
    }
    short *namePattern = NULL;
    if (patMatchName)
    {
        namePattern = new short[512]; // FIXME!
        transform_pattern(nameFilter, namePattern);
    }

    // iterate over all values and add matching ones to outVec.
    // we can exploit the fact that ScalarManager contains the data in the order
    // they were read from file, i.e. grouped by file and run number

    const ScalarManager::Values& scalars = scalarMgr.getValues();
    ScalarManager::RunRef lastRunRef = NULL;
    bool lastRunMatched = false;
    for (ScalarManager::Values::const_iterator i=scalars.begin(); i!=scalars.end(); i++)
    {
        const ScalarManager::Datum& d = *i;

        if (fileAndRunFilter && fileAndRunFilter[0])
        {
            if (lastRunRef!=d.runRef)
            {
                lastRunRef = d.runRef;
                lastRunMatched = (patMatchFile
                    ? stringmatch(filePattern, lastRunRef->fileAndRunName.c_str())
                    : !strcmp(lastRunRef->fileAndRunName.c_str(), fileAndRunFilter));
            }
            if (!lastRunMatched)
                continue;
        }

        if (moduleFilter && moduleFilter[0] &&
            (patMatchModule ? !stringmatch(modulePattern, d.moduleNameRef->c_str())
                            : strcmp(d.moduleNameRef->c_str(), moduleFilter))
           )
            continue; // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !stringmatch(namePattern, d.scalarNameRef->c_str())
                          : strcmp(d.scalarNameRef->c_str(), nameFilter))
           )
            continue; // no match

        // everything matched
        int id = i - scalars.begin();
        outVec.push_back(id);
    }
}


