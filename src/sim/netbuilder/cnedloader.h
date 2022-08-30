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

class ExprRef {
  private:
    NedElement *node;
    int attrId;
    const char *cachedExprText;
  public:
    ExprRef(NedElement *node, int attrId) : node(node), attrId(attrId) {
        cachedExprText = node->getAttribute(attrId);
    }
    bool empty() const {return !cachedExprText[0];}
    const char *getExprText() const {return cachedExprText;}
    NedElement *getNode() const {return node;}
    bool operator<(const ExprRef& other) const {return node!=other.node ? node<other.node : attrId<other.attrId;}
};

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
    std::map<ExprRef, cDynamicExpression*> cachedExpresssions;

  protected:
    // constructor is protected, because we want only one instance
    cNedLoader()  = default;

    // reimplemented to create specialized type info
    virtual cNedDeclaration *createTypeInfo(const char *qname, bool isInnerType, ASTNode *node) override;

    // reimplemented so that we can add cModuleType/cChannelType
    virtual void registerNedType(const char *qname, bool isInnerType, NedElement *node) override;

  public:
    virtual ~cNedLoader();

    /** Access to the singleton instance */
    static cNedLoader *getInstance();

    /** Disposes of the singleton instance */
    static void clear();

    /** Redefined to make return type more specific. */
    virtual cNedDeclaration *getDecl(const char *qname) override;

    /** Compile NED expression (which occurs in given attribute of the given node) to a cDynamicExpression, and cache it */
    virtual cDynamicExpression *getCompiledExpression(const ExprRef& expr);
};

}  // namespace omnetpp


#endif

