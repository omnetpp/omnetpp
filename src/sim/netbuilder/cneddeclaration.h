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
//XXX move more stuff into the base class NEDTypeInfo? except for ExpressionMap, PropertiesMap etc
class SIM_API cNEDDeclaration : public NEDTypeInfo
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

    // cached expressions: NED expressions (ExpressionElement) compiled into
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
    NEDElement *getSubcomponentElement(const char *subcomponentName) const;
    std::string getCxxNamespace() const;

    static cProperties *mergeProperties(const cProperties *baseprops, NEDElement *parent);
    static void updateProperty(PropertyElement *propNode, cProperty *prop);
    static void updateDisplayProperty(PropertyElement *propNode, cProperty *prop);
    static const char *getSingleValueLocalProperty(NEDElement *parent, const char *name);

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

    /**
     * Returns the type of this declaration: simple module, compound module,
     * channel, etc.
     */
    virtual Type getType() const {return type;}

    /**
     * Returns the package name (from the package declaration of the containing
     * NED file
     */
    virtual std::string getPackage() const;

    /**
     * Returns a one-line summary (base class, implemented interfaces, etc)
     */
    virtual std::string info() const;

    /**
     * Returns the NED declaration.
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

    //FIXME the following could become protected methods of cModuleType / cChannelType
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
    virtual cParValue *getCachedExpression(ExpressionElement *expr);
    virtual void putCachedExpression(ExpressionElement *expr, cParValue *value);
    //@}

    /** @name Help for the dynamic builder */
    //@{
    ParametersElement *getParametersElement() const;         //XXX

    GatesElement *getGatesElement() const;

    /**
     * Returns the <submodules> element from the NEDElement tree of this
     * NED component declaration.
     * Returns NULL if this declaration doesn't contain submodules.
     */
    virtual SubmodulesElement *getSubmodulesElement() const;

    /**
     * Returns the <connections> element from the NEDElement tree of this
     * NED component declaration.
     * Returns NULL if this declaration doesn't contain connections.
     */
    virtual ConnectionsElement *getConnectionsElement() const;
    //@}
};

NAMESPACE_END


#endif


