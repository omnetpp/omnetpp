//==========================================================================
// CNEDDECLARATION.H -
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


#ifndef __OMNETPP_CNEDDECLARATION_H
#define __OMNETPP_CNEDDECLARATION_H

#include <string>
#include <vector>
#include <map>
#include "nedxml/nedtypeinfo.h"
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/globals.h"
#include "omnetpp/cownedobject.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"

namespace omnetpp {

namespace common { class PatternMatcher; };

using namespace omnetpp::nedxml;

/**
 * @brief Extends NedTypeInfo with property and cached expression storage,
 * suitable for the sim kernel (cDynamicModuleType/cDynamicChannelType).
 *
 * cNedDeclarations are used during network setup (and dynamic module
 * creation) to add gates and parameters to the freshly created module
 * object, and also to verify that module parameters are set correctly.
 *
 * Adds the following to NedTypeInfo:
 *
 *  - parameter and gate descriptions extracted from the NedElement trees,
 *    also following the inheritance chain. Inherited parameters and
 *    gates are included, and values (parameters and gate sizes) are
 *    converted into and stored in cPar form.
 *  - properties, merged along the inheritance chain.
 *
 * @ingroup Internals
 */
class SIM_API cNedDeclaration : public NedTypeInfo
{
  public:
    typedef common::PatternMatcher PatternMatcher;
    typedef internal::cParImpl cParImpl;
    struct PatternData {PatternMatcher *matcher; ParamElement *patternNode;};
  protected:
    // properties
    typedef std::map<std::string, cProperties *> StringPropsMap;
    mutable cProperties *props = nullptr;
    mutable StringPropsMap paramPropsMap;
    mutable StringPropsMap gatePropsMap;
    mutable StringPropsMap submodulePropsMap;
    mutable StringPropsMap connectionPropsMap;

    // cached expressions: NED expressions (ExpressionElement) compiled into
    // cParImpl get cached here, indexed by exprNode->getId().
    typedef std::map<long, cParImpl *> SharedParImplMap;
    SharedParImplMap parimplMap;

    // wildcard-based parameter assignments
    std::vector<PatternData> patterns;  // contains patterns defined in super types as well
    bool patternsValid = false;  // whether patterns[] was already filled in
    typedef std::map<std::string, std::vector<PatternData> > StringPatternDataMap;
    StringPatternDataMap submodulePatterns;  // contains patterns defined in the "submodules" section

    // super types in base-to-derived order, including (and ending with) the "this" pointer; empty if unfilled
    std::vector<cNedDeclaration*> inheritanceChain;

  protected:
    void putIntoPropsMap(StringPropsMap& propsMap, const std::string& name, cProperties *props) const;
    cProperties *getFromPropsMap(const StringPropsMap& propsMap, const std::string& name) const;
    void appendPropsMap(StringPropsMap& toPropsMap, const StringPropsMap& fromPropsMap);

    void clearPropsMap(StringPropsMap& propsMap);

    cProperties *updateProperties(cProperties *baseprops, NedElement *withPropsParent) const;
    void updateProperty(cProperty *prop, PropertyElement *withPropNode) const;
    void updateDisplayProperty(cProperty *prop, PropertyElement *withPropNode) const;

    cProperties *doProperties() const;
    cProperties *doParamProperties(const char *paramName) const;
    cProperties *doGateProperties(const char *gateName) const;
    cProperties *doSubmoduleProperties(const char *submoduleName, const char *submoduleType) const;
    cProperties *doConnectionProperties(int connectionId, const char *channelType) const;
    void collectPatternsFrom(ParametersElement *paramsNode, std::vector<PatternData>& v);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. It takes the fully qualified name.
     */
    cNedDeclaration(NedResourceCache *resolver, const char *qname, bool isInnerType, NedElement *tree);

    /**
     * Destructor.
     */
    virtual ~cNedDeclaration();
    //@}

    /** @name Misc */
    //@{
    /**
     * Redefined to change return type (covariant return type)
     */
    virtual cNedDeclaration *getSuperDecl() const override;

    /**
     * Returns the inheritance chain. Starts with the base types, and ends
     * with the "this" pointer.
     */
    virtual const std::vector<cNedDeclaration*>& getInheritanceChain();

    /**
     * Returns the pattern-based parameter assignments on the type (i.e. the
     * compound module) and in super types as well.
     */
    virtual const std::vector<PatternData>& getParamPatterns();

    /**
     * Returns the pattern-based parameter assignments on the given submodule;
     * searches the super types as well (due to inherited submodules).
     */
    virtual const std::vector<PatternData>& getSubmoduleParamPatterns(const char *submoduleName);

    // NOTE: connections have no submodules or sub-channels, so they cannot contain pattern-based param assignments either
    //@}

    /** @name Properties of this type, its parameters, gates etc. */
    //@{
    virtual cProperties *getProperties() const;
    virtual cProperties *getParamProperties(const char *paramName) const;
    virtual cProperties *getGateProperties(const char *gateName) const;
    virtual cProperties *getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const;
    virtual cProperties *getConnectionProperties(int connectionId, const char *channelType) const;
    //@}

    /** @name Caching of pre-built cParImpls, so that we we do not have to build them from ASTNodes every time */
    //@{
    virtual cParImpl *getSharedParImplFor(NedElement *node);
    virtual void putSharedParImplFor(NedElement *node, cParImpl *value);
    virtual void clearSharedParImpls();
    //@}
};

}  // namespace omnetpp


#endif


