//==========================================================================
// CNEDDECLARATION.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __CNEDDECLARATION_H
#define __CNEDDECLARATION_H

#include <string>
#include <vector>
#include <map>
#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"
#include "cparimpl.h"
#include "cgate.h"
#include "cproperties.h"
#include "cproperty.h"

#include "nedtypeinfo.h"

NAMESPACE_BEGIN


/**
 * Extends NEDTypeInfo with property and cached expression storage,
 * suitable for the sim kernel (cDynamicModuleType/cDynamicChannelType).
 *
 * cNEDDeclarations are used during network setup (and dynamic module
 * creation) to add gates and parameters to the freshly created module
 * object, and also to verify that module parameters are set correctly.
 *
 * Adds the following to NEDTypeInfo:
 *
 *  - parameter and gate descriptions extracted from the NEDElement trees,
 *    also following the inheritance chain. Inherited parameters and
 *    gates are included, and values (parameters and gate sizes) are
 *    converted into and stored in cPar form.
 *  - properties, merged along the inheritance chain.
 *
 * @ingroup Internals
 */
class SIM_API cNEDDeclaration : public NEDTypeInfo
{
  protected:
    // properties
    typedef std::map<std::string, cProperties *> StringPropsMap;
    mutable cProperties *props;
    mutable StringPropsMap paramPropsMap;
    mutable StringPropsMap gatePropsMap;
    mutable StringPropsMap submodulePropsMap;
    mutable StringPropsMap connectionPropsMap;

    // cached expressions: NED expressions (ExpressionElement) compiled into
    // cParImpl get cached here, indexed by exprNode->getId().
    typedef std::map<long, cParImpl *> SharedParImplMap;
    SharedParImplMap parimplMap;

  protected:
    void putIntoPropsMap(StringPropsMap& propsMap, const std::string& name, cProperties *props) const;
    cProperties *getFromPropsMap(const StringPropsMap& propsMap, const std::string& name) const;
    void appendPropsMap(StringPropsMap& toPropsMap, const StringPropsMap& fromPropsMap);
    virtual cNEDDeclaration *getSuperDecl() const; // covariant return value

    void clearPropsMap(StringPropsMap& propsMap);
    void clearSharedParImplMap(SharedParImplMap& parimplMap);

    static cProperties *mergeProperties(const cProperties *baseprops, NEDElement *parent);
    static void updateProperty(PropertyElement *propNode, cProperty *prop);
    static void updateDisplayProperty(PropertyElement *propNode, cProperty *prop);

    cProperties *doProperties() const;
    cProperties *doParamProperties(const char *paramName) const;
    cProperties *doGateProperties(const char *gateName) const;
    cProperties *doSubmoduleProperties(const char *submoduleName, const char *submoduleType) const;
    cProperties *doConnectionProperties(int connectionId, const char *channelType) const;

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. It takes the fully qualified name.
     */
    cNEDDeclaration(NEDResourceCache *resolver, const char *qname, NEDElement *tree);

    /**
     * Destructor.
     */
    virtual ~cNEDDeclaration();
    //@}

    /** @name Properties of this type, its parameters, gates etc. */
    //@{
    virtual cProperties *getProperties() const;
    virtual cProperties *getParamProperties(const char *paramName) const;
    virtual cProperties *getGateProperties(const char *gateName) const;
    virtual cProperties *getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const;
    virtual cProperties *getConnectionProperties(int connectionId, const char *channelType) const;
    //@}

    /** @name Caching of pre-built cParImpls, so that we we don't have to build them from NEDElements every time */
    //@{
    virtual cParImpl *getSharedParImplFor(NEDElement *node);
    virtual void putSharedParImplFor(NEDElement *node, cParImpl *value);
    //@}
};

NAMESPACE_END


#endif


