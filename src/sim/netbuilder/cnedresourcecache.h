//==========================================================================
// CNEDRESOURCECACHE.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __CNEDRESOURCECACHE_H
#define __CNEDRESOURCECACHE_H

#include "nedresourcecache.h"


/**
 * Stores dynamically loaded NED files. Extends nedxml's NEDResourceCache,
 * so that we can subclass NEDComponent.
 */
class SIM_API cNEDResourceCache : public NEDResourceCache
{
  protected:
    /** Redefined to return a cNEDComponent. */
    virtual NEDComponent *createNEDComponent(NEDElement *tree);

  public:
    /** Constructor */
    cNEDResourceCache() {}

    /** Just a typecast on base class's lookup(), utilizing covariant return types */
    virtual cNEDComponent *lookup(const char *name);
};

#endif

