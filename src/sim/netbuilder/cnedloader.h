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

#include <map>
#include "nedxml/nedresourcecache.h"
#include "omnetpp/simkerneldefs.h"
#include "cneddeclaration.h"

namespace omnetpp {

/**
 * @brief Stores dynamically loaded NED files, and one can look up NED declarations
 * of modules, channels, module interfaces and channel interfaces in them.
 * NED declarations are wrapped in cNedDeclaration objects, which
 * point back into the NedElement trees of the loaded NED files.
 *
 * This cNedLoader class extends nedxml's NedResourceCache, and
 * cNedDeclaration extends nexml's corresponding NedTypeInfo.
 */
class SIM_API cNedLoader : public NedResourceCache
{
  protected:
    // the singleton instance
    static cNedLoader *inst;

    // expression cache
    std::map<ExpressionElement*, cDynamicExpression*> cachedExpresssions;

  protected:
    // constructor is protected, because we want only one instance
    cNedLoader()  {}

    // reimplemented so that we can add cModuleType/cChannelType
    virtual void registerNedType(const char *qname, bool isInnerType, NedElement *node) override;

  public:
    virtual ~cNedLoader();

    /** Access to the singleton instance */
    static cNedLoader *getInstance();

    /** Disposes of the singleton instance */
    static void clear();

    /** Redefined to make return type more specific. */
    virtual cNedDeclaration *getDecl(const char *qname) const override;

    /** Compile NED expression to a cDynamicExpression, and cache it */
    virtual cDynamicExpression *getCompiledExpression(ExpressionElement *exprNode, bool inSubcomponentScope);
};

}  // namespace omnetpp


#endif

