//==========================================================================
// CNEDLOADER.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_CNEDLOADER_H
#define __OMNETPP_CNEDLOADER_H

#include "nedxml/nedresourcecache.h"
#include "omnetpp/simkerneldefs.h"
#include "cneddeclaration.h"

namespace omnetpp {

/**
 * @brief Stores dynamically loaded NED files, and one can look up NED declarations
 * of modules, channels, module interfaces and channel interfaces in them.
 * NED declarations are wrapped in cNEDDeclaration objects, which
 * point back into the NEDElement trees of the loaded NED files.
 *
 * This cNEDLoader class extends nedxml's NEDResourceCache, and
 * cNEDDeclaration extends nexml's corresponding NEDTypeInfo.
 */
class SIM_API cNEDLoader : public NEDResourceCache
{
  protected:
    // the singleton instance
    static cNEDLoader *inst;

  protected:
    // constructor is protected, because we want only one instance
    cNEDLoader()  {}

    // reimplemented so that we can add cModuleType/cChannelType
    virtual void registerNedType(const char *qname, bool isInnerType, NEDElement *node) override;

  public:
    /** Access to the singleton instance */
    static cNEDLoader *getInstance();

    /** Disposes of the singleton instance */
    static void clear();

    /** Redefined to make return type more specific. */
    virtual cNEDDeclaration *getDecl(const char *qname) const override;
};

}  // namespace omnetpp


#endif

