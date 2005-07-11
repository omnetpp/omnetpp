//=========================================================================
//  FILTERING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _FILTERING_H_
#define _FILTERING_H_


#include "scalarmanager.h"


/**
 * Get a filtered subset of all scalars. All four filter parameters may be omitted (NULL)
 */
/* not used
void getFilteredScalarList(const ScalarManager& scalarMgr,
               ScalarManager::RunRef runRef, const char *moduleName, const char *scalarName,
               ScalarManager::IntVector& outVec);
*/

/**
 * Get a filtered subset of all scalars. All three filter parameters may be null,
 * or may contain wildcards (*,?). Full string search (substrings need * both ends).
 */
void getFilteredScalarList2(const ScalarManager& scalarMgr,
               const char *fileAndRunFilter,
               const char *moduleFilter,
               const char *nameFilter,
               ScalarManager::IntVector& outVec);

#endif


