//=========================================================================
//  SCALARMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "patmatch.h"
#include "filtering.h"


void getFilteredScalarList(const ScalarManager& scalarMgr,
               ScalarManager::FileRef fileRef, int runNumber,
               const char *moduleName, const char *scalarName,
               ScalarManager::IntVector& outVec)
{
    const ScalarManager::Values& scalars = scalarMgr.getValues();
    ScalarManager::FileRef noFile = scalarMgr.getFiles().end();
    int k=0;
    for (ScalarManager::Values::const_iterator i=scalars.begin(); i!=scalars.end(); i++,k++)
    {
        const ScalarManager::Datum& d = *i;
        if ((fileRef!=noFile && d.fileRef==fileRef) &&
            (runNumber!=-1 && d.runNumber==runNumber) &&
            (moduleName && *d.moduleNameRef==moduleName) &&
            (scalarName && *d.scalarNameRef==scalarName)
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

    // iterate over all values and add matching ones to outVec
    const ScalarManager::Values& scalars = scalarMgr.getValues();
    std::string lastFnameAndRun;
    ScalarManager::FileRef lastFileRef = scalarMgr.getFiles().end();
    int lastRunNumber = -1;
    for (ScalarManager::Values::const_iterator i=scalars.begin(); i!=scalars.end(); i++)
    {
        const ScalarManager::Datum& d = *i;

        if (fileAndRunFilter && fileAndRunFilter[0])
        {
            // we can exploit the fact that ScalarManager contains the data in the order
            // they were read from file, i.e. grouped by file and run number
            if (lastFileRef!=d.fileRef || lastRunNumber!=d.runNumber)
            {
                char buf[16];
                sprintf(buf,"%d",d.runNumber);
                lastFnameAndRun = d.fileRef->second.fileName + " #" + buf;
                lastFileRef = d.fileRef;
                lastRunNumber = d.runNumber;
            }
            if (patMatchFile ? !stringmatch(filePattern, lastFnameAndRun.c_str())
                             : strcmp(lastFnameAndRun.c_str(), fileAndRunFilter))
                continue; // no match
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


