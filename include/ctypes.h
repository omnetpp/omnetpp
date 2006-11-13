//==========================================================================
//  CTYPES.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following functions:
//    createOne( char *classname )
//
//  Declaration of the following classes:
//    cModuleInterface: defines a module interface (gates+parameters)
//    cModuleType     : module class + interface pairs
//    cChannelType    : channel type (propagation delay, error rate, data rate)
//    cNetworkType    : network
//    cClassRegister  : creates an object of a specific type
//    cInspectorFactory : inspector creation
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CTYPES_H
#define __CTYPES_H

#include <stdarg.h>
#include "defs.h"
#include "globals.h"
#include "cobject.h"

//=========================================================================
//=== classes declared here
class  cModuleInterface;
class  cModuleType;
class  cChannelType;
class  cNetworkType;
class  cFunctionType;
class  cInspectorFactory;

//=== class mentioned
class  cModule;
class  cPar;
class  cChannel;

//=== function types used by cModuleType & cChannelType

/**
 * Prototype for functions that are called by cModuleType objects
 * to create modules of a specific type.
 * @ingroup EnumsTypes
 */
typedef cModule *(*ModuleCreateFunc)();

/**
 * DEPRECATED. Prototype for functions that are called by cLinkType objects
 * to create parameter objects for a link of a specific type.
 * @ingroup EnumsTypes
 */
typedef cPar *(*ParCreateFunc)();

//==========================================================================

/**
 * Describes the interface (modules, gates,...) of a module type.
 * cModuleInterfaces are used during network setup (and dynamic module
 * creation) to add gates and parameters to the freshly created module
 * object, and also to verify that module parameters set correctly.
 *
 * cModuleInterfaces may be created either dynamically or by the NED compiler.
 * In the latter case, the are the compiled form of NED declarations of simple
 * modules. They are created in the following way:
 *
 *  1) starting point is the NED declarations of simple modules, e.g:
 *     <pre>
 *        simple Generator
 *          parameters: ia_rate;
 *          gates: out: out;
 *        endsimple
 *     </pre>
 *
 *  2) the nedtool compiler translates the NED declaration into a
 *     ModuleInterface...End macro and places it into the _n.cc file.
 *     <pre>
 *        ModuleInterface(Generator)
 *          Parameter("speed", Numeric)
 *          Gate( "out", Output)
 *        EndInterface
 *     </pre>
 *
 *     The above translates into cModuleInterface::DeclarationItem array
 *     by means of #define macros.
 *
 *  3) When the program starts up, cModuleInterfaces are registered,
 *     and cModuleInterface constructor parses the array into an internal
 *     data structure.
 *
 *  4) When a module is created, the appropriate cModuleInterface object
 *     is looked up from the list, and the module's gates and parameters
 *     are created according to the description in the cModuleInterface
 *     object.
 *
 * @ingroup Internals
 */
class SIM_API cModuleInterface : public cObject
{
  public:
    // used by the ModuleInterface macro
    struct DeclarationItem
    {
        char what;
        char *name;
        char *types;
        char type;
    };

  protected:
    struct GateDecl
    {
       char *name;
       char type;
       bool vect;
    };

    struct ParamDecl
    {
       char *name;
       char *types;
    };

    int maxnumgates;  // allocated room for gate declarations
    int numgates;     // actually used
    GateDecl *gatev;  // the vector

    int maxnumparams; // as above
    int numparams;
    ParamDecl *paramv;

  protected:
    // internal
    void checkConsistency();

    // internal
    void setup(DeclarationItem *decltable);

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cModuleInterface(const char *name);

    /**
     * Constructor.
     */
    cModuleInterface(const char *name, DeclarationItem *decltable);

    /**
     * Copy constructor.
     */
    cModuleInterface(const cModuleInterface& mi);

    /**
     * Destructor.
     */
    virtual ~cModuleInterface();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cModuleInterface& operator=(const cModuleInterface& mi);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cModuleInterface(*this);}
    //@}

    /** @name Setting up a module interface manually */
    //@{
    /**
     * Make room for a given number of gate declarations
     */
    void allocateGateDecls(int maxnumgates);

    /**
     * Make room for a given number of parameter declarations
     */
    void allocateParamDecls(int maxnumparams);

    /**
     * Add a gate declaration
     */
    void addGateDecl(const char *name, const char type, bool isvector=false);

    /**
     * Add a parameter declaration
     */
    void addParamDecl(const char *name, const char *types);
    //@}

    /** @name Query on the module interface */
    //@{
    /**
     * Returns the number of parameters
     */
    int numParams();

    /**
     * Returns index of the given param (0..numParams()), or -1 if not found
     */
    int findParam(const char *name);

    /**
     * Returns the name of the kth parameter
     */
    const char *paramName(int k);

    /**
     * Returns the cPar types allowed for the kth parameter
     */
    const char *paramType(int k);

    /**
     * Returns if the kth parameter is declarared to be <tt>const</tt>
     */
    bool isParamConst(int k);

    /**
     * Returns the number of gates
     */
    int numGates();

    /**
     * Returns index of the given gate (0..numGates()), or -1 if not found
     */
    int findGate(const char *name);

    /**
     * Returns the name of the kth gate
     */
    const char *gateName(int k);

    /**
     * Returns if the kth gate is input or output
     */
    char gateType(int k);

    /**
     * Returns if the kth gate is a gate vector
     */
    bool isGateVector(int k);
    //@}

    /** @name Applying the interface to modules. */
    //@{

    /**
     * Adds parameters and gates specified by the interface to the given module.
     */
    void addParametersGatesTo(cModule *module);

    /**
     * Checks that the types of the module's parameters comply to the interface,
     * and calls convertToConst() on the parameters declared as const in the
     * interface.
     */
    void checkParametersOf(cModule *module);
    //@}
};

//==========================================================================

/**
 * Class for creating a module of a specific type.
 *
 * A cModuleType object exist for each module type (simple or compound).
 * A cModuleType object 'knows' how to create a module of a given type,
 * thus a module can be created without having to include the .h file
 * with the C++ declaration of the module class ("class FddiMAC...").
 * A cModuleType object is created through a Define_Module macro. Thus,
 * each module type must have a Define_Module() line, e.g:
 *
 * Define_Module( MySimpleModule );
 *
 * nedtool automatically generates Define_Module for compound modules, but the
 * user is responsible for adding one for each simple module type.
 *
 * @ingroup Internals
 */
class SIM_API cModuleType : public cObject
{
    friend class cModule;

  protected:
    char *interface_name;
    cModuleInterface *iface;
    ModuleCreateFunc create_func;

    // internal: here it invokes create_func
    virtual cModule *createModuleObject();

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cModuleType(const char *classname, const char *interf_name, ModuleCreateFunc cf);

    /**
     * Copy constructor.
     */
    cModuleType(const cModuleType& mi);

    /**
     * Destructor.
     */
    virtual ~cModuleType();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cModuleType& operator=(const cModuleType& mi);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const     {return new cModuleType(*this);}
    //@}

    /** @name Module creation */
    //@{

    /**
     * Creates a module which is not element of a module vector.
     * In addition to creating an object of the correct type,
     * this function inserts it into cSimulation's module vector and adds the
     * parameters and gates specified in the interface description.
     */
    virtual cModule *create(const char *name, cModule *parentmod);

    /**
     * Creates a module to be an element of a module vector.
     * The last two arguments specify the vector size and the index
     * of the new module within the vector.
     */
    virtual cModule *create(const char *name, cModule *parentmod, int vectorsize, int index);

    /**
     * DEPRECATED. Use <tt>mod->buildInside()</tt> instead; that's what
     * this method does anyway.
     */
    virtual void buildInside(cModule *mod) _OPPDEPRECATED;

    /**
     * This is a convenience function to get a module up and running in one step.
     *
     * First, the module is created using create() and buildInside(), then
     * starter messages are created (using mod->scheduleStart(simulation.simTime())),
     * then initialize() is called (mod->callInitialize()). It is important that
     * scheduleStart() be called before initialize(), because initialize()
     * functions might contain scheduleAt() calls which could otherwise insert
     * a message BEFORE the starter messages for module.
     *
     * This method works for simple and compound modules alike. Not applicable
     * if the module:
     *  - has parameters to be set
     *  - gate vector sizes to be set
     *  - gates to be connected before initialize()
     */
    virtual cModule *createScheduleInit(char *name, cModule *parentmod);

    /**
     * Returns pointer to the module interface object corresponding to this
     * module type.
     *
     * @see cModuleInterface
     */
    virtual cModuleInterface *moduleInterface();
    //@}
};


//==========================================================================

/**
 * Abstract base class for channel types. One is expected to redefine the
 * create() method to construct and return a channel object (cChannel subclass)
 * of the appropriate type and attributes set. The class has to be registered
 * via the Define_Channel() macro.
 *
 * @ingroup Internals
 */
class SIM_API cChannelType : public cObject
{
  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cChannelType(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cChannelType() {}

    /**
     * Assignment is not supported by this class: this method throws a cRuntimeError when called.
     */
    cChannelType& operator=(const cChannelType&)  {copyNotSupported();return *this;}
    //@}

    /** @name Channel object creation */
    //@{

    /**
     * Factory method to create a channel object.
     */
    virtual cChannel *create(const char *name) = 0;
    //@}
};


/**
 * DEPRECATED.
 *
 * A channel type for backward compatibility. Objects of this class are
 * created via the Define_Link() macro.
 *
 * @ingroup Internals
 */
class SIM_API _OPPDEPRECATED cLinkType : public cChannelType
{
  private:
    cPar *(*delayfunc)();     // delay
    cPar *(*errorfunc)();     // bit error rate
    cPar *(*dataratefunc)();  // data rate

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor. It takes three function pointers; the corresponding
     * functions should be 'factory' functions that create the delay,
     * bit error rate and data rate objects (cPars) for this channel type.
     */
    cLinkType(const char *name, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)());

    /**
     * Copy constructor.
     */
    cLinkType(const cLinkType& li);

    /**
     * Destructor.
     */
    virtual ~cLinkType() {}

    /**
     * Assignment is not supported by this class: this method throws a cRuntimeError when called.
     */
    cLinkType& operator=(const cLinkType&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cLinkType(*this);}
    //@}

    /** @name Channel object creation */
    //@{

    /**
     * Creates a channel object.
     */
    virtual cChannel *create(const char *name);
    //@}
};

//==========================================================================

/**
 * Abstract base class for network types. cNetworkType has to be subclassed
 * and the setupNetwork() method redefined, and the subclass to be registered
 * via the Define_Network(). The result will be factory object which can
 * set up a concrete network.
 *
 * All this is usually taken care of by the NED compiler, so normal users
 * should not need to know about cNetworkType.
 *
 * @ingroup Internals
 */
class SIM_API cNetworkType : public cObject
{
  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cNetworkType(const cNetworkType& n) : cObject() {setName(n.name());operator=(n);}

    /**
     * Constructor. It takes pointer to a function that can build up a network.
     */
    cNetworkType(const char *name=NULL) : cObject(name) {}

    /**
     * Destructor.
     */
    virtual ~cNetworkType() {}

    /**
     * Assignment is not supported by this class: this method throws a cRuntimeError when called.
     */
    cNetworkType& operator=(const cNetworkType&)  {copyNotSupported();return *this;}
    //@}

    /**
     * Network setup function. This is redefined in subclasses.
     */
    virtual void setupNetwork() = 0;

};

//==========================================================================

/**
 * Registration class for extending NED with new functions. Stores a function
 * pointer (returning a double).
 *
 * Objects of this class are usually created via the Define_Function() macro.
 *
 * @ingroup Internals
 */
class SIM_API cFunctionType : public cObject
{
  private:
    MathFunc f;
    int argc;
  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cFunctionType(const cFunctionType& ft) : cObject() {setName(ft.name());operator=(ft);}

    /**
     * Constructor.
     */
    cFunctionType(const char *name, MathFuncNoArg f, int argc=-1);

    /**
     * Constructor.
     */
    cFunctionType(const char *name, MathFunc1Arg f, int argc=-1);

    /**
     * Constructor.
     */

    cFunctionType(const char *name, MathFunc2Args f, int argc=-1);

    /**
     * Constructor.
     */
    cFunctionType(const char *name, MathFunc3Args f, int argc=-1);

    /**
     * Constructor.
     */
    cFunctionType(const char *name, MathFunc4Args f, int argc=-1);

    /**
     * Destructor.
     */
    virtual ~cFunctionType() {}

    /**
     * Assignment is not supported by this class: this method throws a cRuntimeError when called.
     */
    cFunctionType& operator=(const cFunctionType&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cFunctionType(*this);}
    //@}

    /** @name Member access. */
    //@{
    /**
     * Argument count to function.
     */
    int argCount() {return argc;}

    /**
     * Returns function pointer as double function with unchecked arg list
     * (no type safety!)
     */
    MathFunc mathFunc()  {return f;}

    /**
     * Returns function pointer as double function with no args.
     * Throws exception is actual arg count is different.
     */
    MathFuncNoArg mathFuncNoArg();

    /**
     * Returns function pointer as double function with 1 double arg.
     * Throws exception is actual arg count is different.
     */
    MathFunc1Arg mathFunc1Arg();

    /**
     * Returns function pointer as double function with 2 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc2Args mathFunc2Args();

    /**
     * Returns function pointer as double function with 3 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc3Args mathFunc3Args();

    /**
     * Returns function pointer as double function with 4 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc4Args mathFunc4Args();
    //@}

};

cFunctionType *findfunctionbyptr(MathFunc f);

//==========================================================================

/**
 * The class behind the createOne() function and the Register_Class() macro.
 * Each instance is a factory for a particular class: it knows how to create
 * an object of that class.
 *
 * @see ::createOne() function
 * @see Register_Class() macro
 * @ingroup Internals
 */
class SIM_API cClassRegister : public cObject
{
    cPolymorphic *(*creatorfunc)();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cClassRegister(const cClassRegister& c) : cObject() {setName(c.name());operator=(c);}

    /**
     * Constructor.
     */
    cClassRegister(const char *name, cPolymorphic *(*f)());

    /**
     * Destructor.
     */
    virtual ~cClassRegister() {}

    /**
     * Assignment is not supported by this class: this method throws a cRuntimeError when called.
     */
    cClassRegister& operator=(const cClassRegister&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cClassRegister(*this);}
    //@}

    /** @name Factory function. */
    //@{

    /**
     * Creates an instance of a particular class by calling the creator
     * function. The result has to be cast to the appropriate type
     * (preferably by dynamic_cast or check_and_cast).
     */
    cPolymorphic *createOne() const  {return creatorfunc();}
    //@}
};


/**
 * @name Miscellaneous functions.
 * @ingroup Functions
 */
//@{

/**
 * Creates an instance of a particular class; the result has to be cast
 * to the appropriate type by hand. The class must have been registered
 * previously with the Register_Class() macro. This function internally
 * relies on the cClassRegister class.
 *
 * If the class is not registered, this function throws an exception.
 * If you'd prefer having a NULL pointer returned instead, use the
 * createOneIfClassIsKnown() function.
 *
 * Example:
 *
 * <tt>cObject *param = createOne( "cPar" );</tt>
 *
 * createOne() is used e.g. in parallel simulation when an object is received
 * from another partition and it has to be demarshalled.
 *
 * @see createOneIfClassIsKnown()
 * @see Register_Class() macro
 * @see cClassRegister class
 */
// TBD could go into some class, as static member
SIM_API cPolymorphic *createOne(const char *classname);

/**
 * A variant of the createOne() function; this function doesn't throw an
 * exception if the class is not registered, but returns a NULL pointer
 * instead.
 *
 * @see createOne()
 */
// TBD could go into some class, as static member
SIM_API cPolymorphic *createOneIfClassIsKnown(const char *classname);
//@}

//==========================================================================

#endif


