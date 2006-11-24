//==========================================================================
// CNEDDECLARATIONBASE.H -
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


#ifndef __CNEDDECLARATIONBASE_H
#define __CNEDDECLARATIONBASE_H

#include <string>
#include <vector>
#include <map>
#include "defs.h"
#include "globals.h"
#include "cownedobject.h"
#include "cparvalue.h"
#include "cgate.h"

class cProperties;


/**
 * Represents NED declarations of modules, module interfaces,
 * channels and channel interfaces.
 *
 * This is a passive, data-only class.
 *
 * @ingroup FIXME
 */
class SIM_API cNEDDeclarationBase : public cNamedObject, noncopyable
{
  public:
    /**
     * Describes a parameter declaration with its value
     */
    struct ParamDescription //FIXME fully encapsulate, getter/setter!
    {
        cParValue *value;  // stores name, type, isInput and isVolatile flags as well -- never NULL
        std::string declaredOn;

        ParamDescription();
        ~ParamDescription();
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
        cParValue *gatesize; // NULL if not vector or gatesize unspecified
        std::string declaredOn;

        GateDescription();
        ~GateDescription();
        GateDescription deepCopy() const;
    };

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cNEDDeclarationBase(const char *name) : cNamedObject(name) {}

    /**
     * Destructor.
     */
    virtual ~cNEDDeclarationBase() {}
    //@}

    /**
     * NED declaration as text, if available
     */
    virtual std::string nedSource() const = 0;

    /** @name Inheritance */
    //@{
    /**
     * Returns the number of "extends" names.
     */
    virtual int numExtendsNames() const = 0;

    /**
     * Returns the name of the kth "extends" name (k=0..numExtendsNames()-1).
     */
    virtual const char *extendsName(int k) const = 0;

    /**
     * Returns the number of interfaces.
     */
    virtual int numInterfaceNames() const = 0;

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1).
     */
    virtual const char *interfaceName(int k) const = 0;

    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    virtual const char *implementationClassName() const = 0;
    //@}

    /** @name Parameters */
    //@{
    /**
     * Returns total number of the parameters.
     */
    virtual int numPars() const = 0;

    /**
     * Returns the name of the kth parameter.
     */
    virtual const char *parName(int k) const = 0;

    /**
     * Returns the description of the kth parameter.
     * Throws an error if the parameter does not exist.
     */
    virtual const ParamDescription& paramDescription(int k) const = 0;

    /**
     * Returns index of the parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    virtual int findPar(const char *parname) const = 0;

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
    virtual int numGates() const = 0;

    /**
     * Returns the name of the kth gate.
     */
    virtual const char *gateName(int k) const = 0;

    /**
     * Returns the description of the kth gate.
     * Throws an error if the gate does not exist.
     */
    virtual const GateDescription& gateDescription(int k) const = 0;

    /**
     * Returns index of the given gate (0..numGates()), or -1 if not found
     */
    virtual int findGate(const char *name) const = 0;

    /**
     * Check if a gate exists.
     */
    bool hasGate(const char *s) const  {return findGate(s)>=0;}
    //@}

    /** @name Properties */
    //@{
    /**
     * Returns the properties for this component.
     */
    virtual cProperties *properties() const = 0;

    /**
     * Returns the properties of parameter
     */
    virtual cProperties *paramProperties(const char *paramName) const = 0;

    /**
     * Returns the properties of gate
     */
    virtual cProperties *gateProperties(const char *gateName) const = 0;

    /**
     * Returns the properties of a submodule or a contained channel
     */
    virtual cProperties *subcomponentProperties(const char *subcomponentName) const = 0;

    /**
     * Returns the properties of a parameter of a submodule or a contained channel
     */
    virtual cProperties *subcomponentParamProperties(const char *subcomponentName, const char *paramName) const = 0;

    /**
     * Returns the properties of a submodule gate
     */
    virtual cProperties *subcomponentGateProperties(const char *subcomponentName, const char *gateName) const = 0;
    //@}
};

#endif


