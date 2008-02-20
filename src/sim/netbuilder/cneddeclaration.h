//==========================================================================
// CNEDDECLARATION.H -
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


#ifndef __CNEDDECLARATION_H
#define __CNEDDECLARATION_H

#include <string>
#include <vector>
#include <map>
#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"
#include "cparvalue.h"
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
    typedef std::map<std::string, cProperties *> PropertiesMap;
    mutable cProperties *props;
    mutable PropertiesMap paramPropsMap;
    mutable PropertiesMap gatePropsMap;
    mutable PropertiesMap subcomponentPropsMap;
    mutable PropertiesMap subcomponentParamPropsMap;
    mutable PropertiesMap subcomponentGatePropsMap;

    // cached expressions: NED expressions (ExpressionElement) compiled into
    // cParValue get cached here, indexed by exprNode->id().
    typedef std::map<long, cParValue *> ExpressionMap;
    ExpressionMap expressionMap;

  protected:
    void putIntoPropsMap(PropertiesMap& propsMap, const std::string& name, cProperties *props) const;
    cProperties *getFromPropsMap(const PropertiesMap& propsMap, const std::string& name) const;
    void appendPropsMap(PropertiesMap& toPropsMap, const PropertiesMap& fromPropsMap);
    virtual cNEDDeclaration *getSuperDecl() const; // covariant return value

    static void clearPropsMap(PropertiesMap& propsMap);
    static void clearExpressionMap(ExpressionMap& exprMap);

    static cProperties *mergeProperties(const cProperties *baseprops, NEDElement *parent);
    static void updateProperty(PropertyElement *propNode, cProperty *prop);
    static void updateDisplayProperty(PropertyElement *propNode, cProperty *prop);


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

    /**
     * Returns the properties for this component.
     */
    virtual cProperties *properties() const;

    /**
     * Returns the properties of parameter
     */
    virtual cProperties *paramProperties(const char *paramName) const;

    /**
     * Returns the properties of gate
     */
    virtual cProperties *gateProperties(const char *gateName) const;

    /**
     * Returns the properties of a submodule or a contained channel
     */
    virtual cProperties *subcomponentProperties(const char *subcomponentName, const char *subcomponentType) const;

    /**
     * Returns the properties of a parameter of a submodule or a contained channel
     */
    virtual cProperties *subcomponentParamProperties(const char *subcomponentName, const char *subcomponentType, const char *paramName) const;

    /**
     * Returns the properties of a submodule gate
     */
    virtual cProperties *subcomponentGateProperties(const char *subcomponentName, const char *subcomponentType, const char *gateName) const;
    //@}

    /** @name Expression caching */
    //@{
    virtual cParValue *getCachedExpression(NEDElement *node);
    virtual void putCachedExpression(NEDElement *node, cParValue *value);
    //@}
};

NAMESPACE_END


#endif


