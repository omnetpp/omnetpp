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

#include "nedcomponent.h"

class cProperties;
class cDynamicExpression;


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
class SIM_API cNEDDeclaration : public cNoncopyableOwnedObject, public NEDComponent
{
  public:
    /**
     * Describes a parameter declaration with its value
     */
    struct ParamDescription //FIXME fully encapsulate, getter/setter!
    {
        bool isInput;  // whether it was assigned as default(...)
        cParValue *value;  // stores name, type and isVolatile flag as well -- never NULL
        cProperties *properties;  // never NULL
        std::string declaredOn;

        ParamDescription();
        ParamDescription deepCopy() const;
    };

    /**
     * Describes a gate declaration, with its size if vector
     */
    struct GateDescription  //FIXME fully encapsulate, getter/setter!
    {
        std::string name;
        cGate::Type type; // input, output, inout
        bool isVector;
        cDynamicExpression *gatesize; // NULL if not vector or gatesize unspecified
        cProperties *properties;  // never NULL
        std::string declaredOn;

        GateDescription();
        GateDescription deepCopy() const;
    };

  protected:
    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string,int> StringToIntMap;

    // inheritance
    StringVector extendsnames;
    StringVector interfacenames;

    // simple module/channel C++ class to instantiate
    std::string implClassName;

    // properties
    cProperties *props;

    // parameters
    typedef std::vector<ParamDescription> ParamDescriptions;
    ParamDescriptions params;
    StringToIntMap paramNameMap;

    // gates
    typedef std::vector<GateDescription> GateDescriptions;
    GateDescriptions gates;
    StringToIntMap gateNameMap;

    // locking
    bool locked;

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

    /** @name Redefined cOwnedObject methods */
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
    virtual std::string declaration() const;

    /** @name Setup */
    //@{
    /**
     * Adds a parameter to the declaration. The contained cPar and cProperties
     * objects in the struct will be taken over by cNEDDeclaration (and not
     * duplicated/copied).
     */
    virtual void addPar(const ParamDescription& paramDesc);

    /**
     * Adds a gate to the declaration. The contained cPar and cProperties
     * objects in the struct will be taken over by cNEDDeclaration (and not
     * duplicated/copied).
     */
    virtual void addGate(const GateDescription& gateDesc);

    /**
     * Sets the properties in this declaration.
     */
    virtual void setProperties(cProperties *props);

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
     * Call lock() after setup is done.
     */
    void lock() {locked = true;}
    //@}

    /** @name Inheritance */
    //@{
    /**
     * Returns the number of "extends" names.
     */
    int numExtendsNames() const  {return extendsnames.size();}

    /**
     * Returns the name of the kth "extends" name (k=0..numExtendsNames()-1).
     */
    const char *extendsName(int k) const;

    /**
     * Returns the number of interfaces.
     */
    int numInterfaceNames() const  {return interfacenames.size();}

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1).
     */
    const char *interfaceName(int k) const;

    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    const char *implementationClassName() const;
    //@}

    /** @name Properties */
    //@{
    /**
     * Return the properties for this component. The properties are locked against
     * modification, use dup() to create a modifiable copy.
     */
    virtual cProperties *properties() const;
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
    virtual const ParamDescription& paramDescription(const char *name) const;

    /**
     * Returns index of the parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    virtual int findPar(const char *parname) const;

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}
    //@}

    /** @name Gates */
    //@{
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
    virtual const GateDescription& gateDescription(const char *name) const;

    /**
     * Returns index of the given gate (0..numGates()), or -1 if not found
     */
    virtual int findGate(const char *name) const;

    /**
     * Check if a gate exists.
     */
    bool hasGate(const char *s) const  {return findGate(s)>=0;}

    /**
     * Sets the gate vector size.
     */
    virtual void setGateSize(const char *gateName, cDynamicExpression *gatesize);
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


