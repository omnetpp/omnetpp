//==========================================================================
//   CTYPES.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Declaration of the following functions:
//    createOne( char *classname )
//
//  Declaration of the following classes:
//    cModuleInterface: defines a module interface (gates+parameters)
//    cModuleType     : module class + interface pairs
//    cLinkType       : channel type (propagation delay, error rate, data rate)
//    cNetworkType    : network
//    cClassRegister  : creates an object of a specific type
//    cInspectorFactory : inspector creation
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CTYPES_H
#define __CTYPES_H

#include <stdarg.h>
#include "chead.h"
#include "cobject.h"

//=========================================================================
//=== classes declared here
class  cModuleInterface;
class  cModuleType;
class  cLinkType;
class  cNetworkType;
class  cFunctionType;
class  cInspectorFactory;

//=== class mentioned
class  cModule;
class  cPar;

//=== function types used by cModuleType & cLinkType
typedef cModule *(*ModuleCreateFunc)(const char *, cModule *);
typedef cPar *(*ParCreateFunc)();

//==========================================================================

//
// Used internally by cModuleInterface
//
struct sDescrItem {
    char what;
    char *name;
    char *types;
    char type;
};

/**
 * Represents a module interface: gates and parameter names.
 *
 * cModuleInterfaces are the compiled form of NED declarations of simple
 * modules. They are created in the following way:
 *
 *  1) starting point is the NED declarations of simple modules, e.g:
 *     <PRE><TT>
 *        simple Generator
 *          parameters: ia_rate;
 *          gates: out: out;
 *        endsimple
 *     </PRE></TT>
 *
 *  2) the nedc compiler translates the NED declaration into a
 *     ModuleInterface...End macro and places it into the _n.cc file.
 *     <PRE><TT>
 *        ModuleInterface( Generator )
 *          Parameter ( "ia_rate", AnyType )
 *          Gate      ( "out",     Output  )
 *        End
 *     </PRE></TT>
 *
 *     The macro translates into a static cModuleInterface object declaration
 *     (#define for macros in macros.h)
 *
 *  3) When the program starts up, cModuleInterface constructor runs and
 *     places the object on the modinterfaces list.
 *
 *  4) When a module is created, the appropriate cModuleInterface object
 *     is looked up from the list, and the module's gates and parameters
 *     are created according to the description in the cModuleInterface
 *     object.
 */
class SIM_API cModuleInterface : public cObject
{
    // structures used in a cModuleInterface
    struct sGateInfo {
       char *name;
       char type;
       bool vect;
    };

    struct sParamInfo {
       char *name;
       char *types;
    };

    struct sMachineInfo {
       char *name;
    };

  protected:
    int ngate;
    sGateInfo *gatev;
    int nparam;
    sParamInfo *paramv;
    int nmachine;            // NET
    sMachineInfo *machinev;  // NET
  private:

    /**
     * FIXME: structures used in a cModuleInterface
     */
    void allocate(int ngte, int npram, int nmach );

    /**
     * MISSINGDOC: cModuleInterface:void check_consistency()
     */
    void check_consistency();


    /**
     * MISSINGDOC: cModuleInterface:void setup(sDescrItem*)
     */
    void setup(sDescrItem *descr_table );

  public:

    /**
     * Constructor.
     */
    cModuleInterface(const char *name, sDescrItem *descr_table );

    /**
     * Copy constructor.
     */
    cModuleInterface(cModuleInterface& mi );

    /**
     * Destructor.
     */
    virtual ~cModuleInterface();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cModuleInterface";}

    /**
     * MISSINGDOC: cModuleInterface:cObject*dup()
     */
    virtual cObject *dup()  {return new cModuleInterface(*this);}

    /**
     * MISSINGDOC: cModuleInterface:cModuleInterface&operator=(cModuleInterface&)
     */
    cModuleInterface& operator=(cModuleInterface& mi);

    // the important functions

    /**
     * FIXME: the important functions
     */
    void addParametersGatesTo( cModule *module);

    /**
     * MISSINGDOC: cModuleInterface:void checkParametersOf(cModule*)
     */
    void checkParametersOf( cModule *module );
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
 * each module type must have a Define_Module() macro like this:
 *
 * Define_Module( Generator, "Generator");
 *
 * (The second argument defines the module interface, see cModuleInterface.)
 * Nedc automatically generates Define_Module for compound modules, but the
 * user is responsible for writing it for each simple module type.
 */
class SIM_API cModuleType : public cObject
{
    friend class cModule;
  private:
    char *interface_name;
    cModuleInterface *interface;
    ModuleCreateFunc create_func;
  public:

    /**
     * Constructor.
     */
    cModuleType(const char *classname, const char *interf_name, ModuleCreateFunc cf);

    /**
     * Copy constructor.
     */
    cModuleType(cModuleType& mi );

    /**
     * Destructor.
     */
    virtual ~cModuleType();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cModuleType";}

    /**
     * MISSINGDOC: cModuleType:cObject*dup()
     */
    virtual cObject *dup()     {return new cModuleType(*this);}

    /**
     * MISSINGDOC: cModuleType:cModuleType&operator=(cModuleType&)
     */
    cModuleType& operator=(cModuleType& mi);


    /**
     * MISSINGDOC: cModuleType:cModule*create(char*,cModule*,bool)
     */
    cModule *create(const char *name, cModule *parentmod, bool local=true);

    /**
     * MISSINGDOC: cModuleType:void buildInside(cModule*)
     */
    void buildInside(cModule *mod);

    // convenience function: create()+buildInside()+scheduleStart(now)+callInitialize()

    /**
     * FIXME: convenience function: create()+buildInside()+scheduleStart(now)+callInitialize()
     */
    cModule *createScheduleInit(char *name, cModule *parentmod);
};


//==========================================================================

/**
 * Represents a connection type: name, delay, bit error rate, data rate.
 */
class SIM_API cLinkType : public cObject
{
  private:

    /**
     * MISSINGDOC: cLinkType:cPar*(*)()
     */
    cPar *(*delayfunc)();     // delay

    /**
     * MISSINGDOC: cLinkType:cPar*(*)()
     */
    cPar *(*errorfunc)();     // bit error rate

    /**
     * MISSINGDOC: cLinkType:cPar*(*)()
     */
    cPar *(*dataratefunc)();  // data rate
  public:

    /**
     * Constructor.
     */
    cLinkType(const char *name, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)() );

    /**
     * Copy constructor.
     */
    cLinkType(cLinkType& li );

    /**
     * Destructor.
     */
    virtual ~cLinkType()    {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cLinkType";}

    /**
     * MISSINGDOC: cLinkType:cObject*dup()
     */
    virtual cObject *dup()     {return new cLinkType(*this);}

    /**
     * MISSINGDOC: cLinkType:cLinkType&operator=(cLinkType&)
     */
    cLinkType& operator=(cLinkType& o);

    // new functions

    /**
     * FIXME: new functions
     */
    cPar *createDelay();

    /**
     * MISSINGDOC: cLinkType:cPar*createError()
     */
    cPar *createError();

    /**
     * MISSINGDOC: cLinkType:cPar*createDataRate()
     */
    cPar *createDataRate();
};

//==========================================================================

/**
 * Contains information to setup a network. Data stored: name, setup function.
 */
class SIM_API cNetworkType : public cObject
{
  public:

    /**
     * MISSINGDOC: cNetworkType:void(*)()
     */
    void (*setupfunc)();

  public:
    /**
     * Constructor.
     */
    cNetworkType(const char *name, void (*f)()) :

    /**
     * Constructor.
     */
      cObject(name,(cObject *)&networks), setupfunc(f) {}

    /**
     * Destructor.
     */
    virtual ~cNetworkType() {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cNetworkType";}
};

//==========================================================================

/**
 * Stores a function pointer (returning a double).
 */
class SIM_API cFunctionType : public cObject
{
  public:
    MathFunc f;
    int argcount;
  public:

    /**
     * Constructor.
     */
    cFunctionType(const char *name, MathFunc f0, int argc) :

    /**
     * Constructor.
     */
      cObject(name,(cObject *)&functions) {f=f0;argcount=argc;}

    /**
     * Destructor.
     */
    virtual ~cFunctionType() {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cFunctionType";}
};

cFunctionType *findfunctionbyptr(MathFunc f);

//==========================================================================

/**
 * Reflection support class. There is a cClassRegister object for most
 * OMNeT++ classes. The cClassRegister objects know how to create an object
 * of that type.
 */
class SIM_API cClassRegister : public cObject
{

    /**
     * MISSINGDOC: cClassRegister:cObject*(*)()
     */
    cObject *(*creatorfunc)();
  public:

    /**
     * Constructor.
     */
    cClassRegister(const char *name, cObject *(*f)()) :

    /**
     * Constructor.
     */
      cObject(name,(cObject *)&classes), creatorfunc(f) {}

    /**
     * Destructor.
     */
    virtual ~cClassRegister() {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cClassRegister";}

    // new functions

    /**
     * FIXME: new functions
     */
    cObject *createOne()  {return creatorfunc();}
};

SIM_API cObject *createOne(const char *type);

//==========================================================================

/**
 * Internal class. Serves as a base class for inspector factories of
 * specific classes.
 */
class SIM_API cInspectorFactory : public cObject
{
    /**
     * MISSINGDOC: cInspectorFactory:TInspector*(*)(cObject*,,void*)
     */
    TInspector *(*inspFactoryFunc)(cObject *,int,void *);

  public:
    /**
     * Constructor.
     */
    cInspectorFactory(const char *name, TInspector *(*f)(cObject *,int,void *));

    /**
     * Destructor.
     */
    virtual ~cInspectorFactory() {}

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cInspectorFactory";}

    // new functions;

    /**
     * FIXME: new functions;
     */
    TInspector *createInspectorFor(cObject *object,int type,void *data);
};


#endif


