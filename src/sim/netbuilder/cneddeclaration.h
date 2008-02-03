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
#include "cneddeclarationbase.h"
#include "cproperties.h"
#include "cproperty.h"

#include "nedtypeinfo.h"

NAMESPACE_BEGIN


/**
 * Represents NED declarations of modules, module interfaces,
 * channels and channel interfaces. All cNEDDeclaration objects are
 * created and managed by cNEDLoader.
 *
 * cNEDDeclaration stores two things:
 *  - a pointer to the complete NEDElement tree of the NED declaration;
 *    this pointer points into the cNEDLoader.
 *  - parameter and gate descriptions extracted from the NEDElement trees,
 *    also following the inheritance chain. Inherited parameters and
 *    gates are included, and values (parameters and gate sizes) are
 *    converted into and stored in cPar form.
 *
 * cNEDDeclarations are used during network setup (and dynamic module
 * creation) to add gates and parameters to the freshly created module
 * object, and also to verify that module parameters are set correctly.
 *
 * cNEDDeclaration is a passive, data-only class.
 *
 * @ingroup Internals
 */
class SIM_API cNEDDeclaration : public cNEDDeclarationBase, public NEDTypeInfo // noncopyable
{
  public:
    enum Type {SIMPLE_MODULE, COMPOUND_MODULE, MODULEINTERFACE, CHANNEL, CHANNELINTERFACE};
    
  protected:
    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string,int> StringToIntMap;

    Type type;
    
    // inheritance
    StringVector extendsnames;
    StringVector interfacenames;

    // simple module/channel C++ class to instantiate
    std::string implClassName;

    // properties
    typedef std::map<std::string, cProperties *> PropertiesMap;
    mutable cProperties *props;
    mutable PropertiesMap paramPropsMap;
    mutable PropertiesMap gatePropsMap;
    mutable PropertiesMap subcomponentPropsMap;
    mutable PropertiesMap subcomponentParamPropsMap;
    mutable PropertiesMap subcomponentGatePropsMap;

    // cached expressions: NED expressions (ExpressionNode) compiled into
    // cParValue get cached here, indexed by exprNode->id().
    typedef std::map<long, cParValue *> ExpressionMap;
    ExpressionMap expressionMap;

  protected:
    void putIntoPropsMap(PropertiesMap& propsMap, const std::string& name, cProperties *props) const;
    cProperties *getFromPropsMap(const PropertiesMap& propsMap, const std::string& name) const;
    void appendPropsMap(PropertiesMap& toPropsMap, const PropertiesMap& fromPropsMap);
    static void clearPropsMap(PropertiesMap& propsMap);
    static void clearExpressionMap(ExpressionMap& exprMap);
    cNEDDeclaration *getSuperDecl() const;
    NEDElement *getSubcomponentNode(const char *subcomponentName) const;

    static cProperties *mergeProperties(const cProperties *baseprops, NEDElement *parent);
    static void updateProperty(PropertyNode *propNode, cProperty *prop);
    static void updateDisplayProperty(PropertyNode *propNode, cProperty *prop);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. It takes the fully qualified name.
     */
    cNEDDeclaration(const char *qname, NEDElement *tree);

    /**
     * Destructor.
     */
    virtual ~cNEDDeclaration();
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Returns the fully qualified name (i.e. the simple name prefixed
     * with the package name and any existing enclosing NED type names).
     */
    virtual const char *name() const  {return NEDTypeInfo::name();}

    /**
     * Returns the fully qualified name (i.e. the simple name prefixed
     * with the package name and any existing enclosing NED type names).
     */
    virtual const char *fullName() const  {return NEDTypeInfo::fullName();}

    /**
     * Changing the name is not possible after creation.
     */
    virtual void setName(const char *s);

    /**
     * Produces a one-line description of object contents.
     */
    virtual std::string info() const;

    /**
     * Returns detailed info including gates and parameters in a multi-line string.
     */
    virtual std::string detailedInfo() const;
    //@}

    /**
     * Returns the type of this declaration: simple module, compound module,
     * channel, etc. 
     */
    virtual Type getType() const {return type;}
    
    /**
     * NED declaration
     */
    virtual std::string nedSource() const;

    /**
     * Returns the number of "extends" names.
     */
    virtual int numExtendsNames() const  {return extendsnames.size();}

    /**
     * Returns the name of the kth "extends" name (k=0..numExtendsNames()-1),
     * resolved to fully qualified name.
     */
    virtual const char *extendsName(int k) const;

    /**
     * Returns the number of interfaces.
     */
    virtual int numInterfaceNames() const  {return interfacenames.size();}

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1),
     * resolved to fully qualified name.
     */
    virtual const char *interfaceName(int k) const;

    /**
     * Returns true if this NED type extends/"is like" the given module interface
     * or channel interface
     */
    virtual bool supportsInterface(const char *qname);
    
    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    virtual const char *implementationClassName() const;

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
    virtual cParValue *getCachedExpression(ExpressionNode *expr);
    virtual void putCachedExpression(ExpressionNode *expr, cParValue *value);
    //@}

    /** @name Help for the dynamic builder */
    //@{
    ParametersNode *getParametersNode() const;         //XXX

    GatesNode *getGatesNode() const;

    /**
     * Returns the <submodules> element from the NEDElement tree of this
     * NED component declaration.
     * Returns NULL if this declaration doesn't contain submodules.
     */
    virtual SubmodulesNode *getSubmodulesNode() const;

    /**
     * Returns the <connections> element from the NEDElement tree of this
     * NED component declaration.
     * Returns NULL if this declaration doesn't contain connections.
     */
    virtual ConnectionsNode *getConnectionsNode() const;
    //@}
};

NAMESPACE_END


#endif


