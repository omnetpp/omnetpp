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
#include "defs.h"
#include "globals.h"
#include "cownedobject.h"
#include "cparvalue.h"
#include "cgate.h"
#include "cneddeclarationbase.h"

#include "nedcomponent.h"


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
 * object, and also to verify that module parameters set correctly.
 *
 * cNEDDeclaration is a passive, data-only class.
 *
 * @ingroup Internals
 */
class SIM_API cNEDDeclaration : public cNEDDeclarationBase, public NEDComponent // noncopyable
{
  protected:
    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string,int> StringToIntMap;

    // inheritance
    StringVector extendsnames;
    StringVector interfacenames;

    // simple module/channel C++ class to instantiate
    std::string implClassName;

    // parameters
    typedef std::vector<ParamDescription> ParamDescriptions;
    ParamDescriptions params;
    StringToIntMap paramNameMap;

    typedef std::map<std::string, cParValue *> ParamValueMap;
    ParamValueMap subcomponentParamMap;

    // gates
    typedef std::vector<GateDescription> GateDescriptions;
    GateDescriptions gates;
    StringToIntMap gateNameMap;
    ParamValueMap subcomponentGatesizeMap;

    // locking
    bool locked;

    // properties
    typedef std::map<std::string, cProperties *> PropertiesMap;
    cProperties *props;
    PropertiesMap paramPropsMap;
    PropertiesMap gatePropsMap;
    PropertiesMap subcomponentPropsMap;
    PropertiesMap subcomponentParamPropsMap;
    PropertiesMap subcomponentGatePropsMap;

    // cached expressions: NED expressions (ExpressionNode) compiled into
    // cParValue get cached here, indexed by exprNode->id().
    typedef std::map<long, cParValue *> ExpressionMap;
    ExpressionMap expressionMap;

  protected:
    void putIntoPropsMap(PropertiesMap& propsMap, const std::string& name, cProperties *props);
    cProperties *getFromPropsMap(const PropertiesMap& propsMap, const std::string& name) const;
    void appendPropsMap(PropertiesMap& toPropsMap, const PropertiesMap& fromPropsMap);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cNEDDeclaration(const char *name, NEDElement *tree);

    /**
     * Destructor.
     */
    virtual ~cNEDDeclaration();
    //@}

    /** @name Redefined cObject member functions */
    //@{
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
     * NED declaration
     */
    virtual std::string nedSource() const;

    /** @name Setup */
    //@{
    /**
     * Adds everything from the "other" declaration. Use: initializing a type
     * from a super type.
     */
    virtual void appendFrom(const cNEDDeclaration *superDecl);

    /**
     * Adds a parameter to the declaration.
     */
    virtual void addPar(const ParamDescription& paramDesc);

    /**
     * Sets the parameter of a submodule.
     */
    virtual void setSubcomponentParamValue(const char *subcomponentname, const char *paramName, cParValue *value);

    /**
     * Adds a gate to the declaration.
     */
    virtual void addGate(const GateDescription& gateDesc);

    /**
     * Sets the gate vector size.
     */
    virtual void setGateSize(const char *gateName, cParValue *gatesize);

    /**
     * Sets the gate vector size of a submodule.
     */
    virtual void setSubcomponentGateSize(const char *subcomponentname, const char *gateName, cParValue *gatesize);

    /**
     * Add an "extends" name (super component) to this declaration.
     */
    virtual void addExtendsName(const char *name);

    /**
     * Add a "like" interface to this declaration.
     */
    virtual void addInterfaceName(const char *name);

    /**
     * For simple modules and channels, sets the name of the C++ class that
     * has to be instantiated.
     */
    virtual void setImplementationClassName(const char *name);

    /**
     * Sets the properties in this declaration.
     */
    virtual void setProperties(cProperties *props);

    /**
     * Sets the properties of parameter
     */
    virtual void setParamProperties(const char *paramName, cProperties *props);

    /**
     * Sets the properties of gate
     */
    virtual void setGateProperties(const char *gateName, cProperties *props);

    /**
     * Sets the properties of a submodule or a contained channel
     */
    virtual void setSubcomponentProperties(const char *subcomponentName, cProperties *props);

    /**
     * Sets the properties of a parameter of a submodule or a contained channel
     */
    virtual void setSubcomponentParamProperties(const char *subcomponentName, const char *paramName, cProperties *props);

    /**
     * Sets the properties of a submodule gate
     */
    virtual void setSubcomponentGateProperties(const char *subcomponentName, const char *gateName, cProperties *props);

    /**
     * Call lock() after setup is done.
     */
    void lock() {locked = true;}
    //@}

    /** @name Methods that implement cNEDDeclarationBase */
    //@{
    /**
     * Returns the number of "extends" names.
     */
    virtual int numExtendsNames() const  {return extendsnames.size();}

    /**
     * Returns the name of the kth "extends" name (k=0..numExtendsNames()-1).
     */
    virtual const char *extendsName(int k) const;

    /**
     * Returns the number of interfaces.
     */
    virtual int numInterfaceNames() const  {return interfacenames.size();}

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1).
     */
    virtual const char *interfaceName(int k) const;

    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    virtual const char *implementationClassName() const;
    //@}

    /** @name Parameters */
    //@{
    /**
     * Returns total number of the parameters.
     */
    virtual int numPars() const;

    /**
     * Returns the name of the kth parameter.
     */
    virtual const char *parName(int k) const {return paramDescription(k).value->name();}

    /**
     * Returns the description of the kth parameter.
     * Throws an error if the parameter does not exist.
     */
    virtual const ParamDescription& paramDescription(int k) const;

    /**
     * Returns the description of the ith parameter.
     * Throws an error if the parameter does not exist.
     */
//XXX needed?
    virtual const ParamDescription& paramDescription(const char *name) const;

    /**
     * Returns index of the parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
//XXX needed?
    virtual int findPar(const char *parname) const;

    /**
     * Returns the number of gates
     */
    virtual int numGates() const;

    /**
     * Returns the name of the kth gate.
     */
    virtual const char *gateName(int k) const {return gateDescription(k).name.c_str();}

    /**
     * Returns the description of the kth gate.
     * Throws an error if the gate does not exist.
     */
    virtual const GateDescription& gateDescription(int k) const;

    /**
     * Returns the description of the ith gate.
     * Throws an error if the gate does not exist.
     */
//XXX needed?
    virtual const GateDescription& gateDescription(const char *name) const;

    /**
     * Returns index of the given gate (0..numGates()), or -1 if not found
     */
//XXX needed?
    virtual int findGate(const char *name) const;

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
    virtual cProperties *subcomponentProperties(const char *subcomponentName) const;

    /**
     * Returns the properties of a parameter of a submodule or a contained channel
     */
    virtual cProperties *subcomponentParamProperties(const char *subcomponentName, const char *paramName) const;

    /**
     * Returns the properties of a submodule gate
     */
    virtual cProperties *subcomponentGateProperties(const char *subcomponentName, const char *gateName) const;
    //@}

    /** @name Expression caching */
    //@{
    virtual cParValue *getCachedExpression(ExpressionNode *expr);
    virtual void putCachedExpression(ExpressionNode *expr, cParValue *value);
    //@}

    /** @name Help for the dynamic builder */
    //@{
    /**
     * Returns the <submodules> element from the NEDElement tree of this
     * NED component declaration.
     * Returns NULL if this declaration doesn't contain submodules.
     */
    virtual SubmodulesNode *getSubmodules();

    /**
     * Returns the <connections> element from the NEDElement tree of this
     * NED component declaration.
     * Returns NULL if this declaration doesn't contain connections.
     */
    virtual ConnectionsNode *getConnections();
    //@}
};

#endif


