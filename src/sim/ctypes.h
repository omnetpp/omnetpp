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
  Copyright (C) 1992,99 Andras Varga
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
typedef cModule *(*ModuleCreateFunc)(char *, cModule *);
typedef cPar *(*ParCreateFunc)();

//==========================================================================
// structures used in a cModuleInterface
// NOTE: not cObject descendants
struct sDescrItem {
   char what;
   char *namestr;
   char *types;
   char type;
};

struct sGateInfo {
   char *namestr;
   char type;
   bool vect;
};

struct sParamInfo {
   char *namestr;
   char *types;
};

struct sMachineInfo {    // that's structuritis. sorry. --VA
   char *namestr;
};

//==========================================================================
// cModuleInterface - represents a module interface: gate + parameter names
//
// cModuleInterfaces are the compiled form of NED declarations of simple
// modules. They are created in the following way:
//  1) stating point is the NED declarations of simple modules, e.g:
//        simple Generator
//          parameters: ia_rate;
//          gates: out: out;
//        endsimple
//  2) the nedc compiler translates the NED declaration into a
//     ModuleInterface...End macro and places it into the _n.cc file.
//        ModuleInterface( Generator )
//          Parameter ( "ia_rate", AnyType )
//          Gate      ( "out",     Output  )
//        End
//     The macro translates into a static cModuleInterface object declaration
//     (#define for macros in macros.h)
//  3) When the program starts up, cModuleInterface constructor runs and
//     places the object on the modinterfaces list.
//  4) When a module is created, the appropriate cModuleInterface object
//     is looked up from the list, and the module's gates and parameters
//     are created according to the description in the cModuleInterface
//     object.
//
class cModuleInterface : public cObject
{
  protected:
        int ngate;
        sGateInfo *gatev;
        int nparam;
        sParamInfo *paramv;
        int nmachine;            // NET
        sMachineInfo *machinev;  // NET
  private:
        void allocate( int ngte, int npram, int nmach );
        void check_consistency();

        void setup( sDescrItem *descr_table );

  public:
        cModuleInterface( char *namestr, sDescrItem *descr_table );
        cModuleInterface( cModuleInterface& mi );
        virtual ~cModuleInterface();

        // redefined functions
        virtual char *className()  {return "cModuleInterface";}
        virtual cObject *dup()  {return new cModuleInterface(*this);}
        cModuleInterface& operator=(cModuleInterface& mi);

        // the important functions
        void addParametersGatesTo( cModule *module);
        void checkParametersOf( cModule *module );
};

//==========================================================================
// cModuleType - able to create a module
//
// A cModuleType object exist for each module type (simple or compound).
// A cModuleType object 'knows' how to create a module of a given type,
// thus a module can be created without having to include the .h file
// with the C++ declaration of the module class ("class FddiMAC...").
// A cModuleType object is created through a Define_Module macro. Thus,
// each module type must have a Define_Module() macro like this:
//   Define_Module( Generator, "Generator")
// (The second argument defines the module interface, see cModuleInterface.)
// Nedc automatically generates Define_Module for compound modules, but the
// user is responsible for writing it for each simple module type.
//

class cModuleType : public cObject
{
        friend class cModule;
  private:
        char *interface_name;
        cModuleInterface *interface;
        ModuleCreateFunc create_func;
  public:
        cModuleType( char *classname, char *interf_name, ModuleCreateFunc cf);
        cModuleType( cModuleType& mi );
        virtual ~cModuleType();

        // redefined functions
        virtual char *className()  {return "cModuleType";}
        virtual cObject *dup()     {return new cModuleType(*this);}
        cModuleType& operator=(cModuleType& mi);

        cModule *create(char *namestr, cModule *parentmod, bool local=TRUE);
        void buildInside(cModule *mod);

        // convenience function: create()+buildInside()+scheduleStart(now)+callInitialize()
        cModule *createScheduleInit(char *namestr, cModule *parentmod);
};


//==========================================================================
// cLinkType - holds a connection type: namestr, delay, bit error rate, data rate

class cLinkType : public cObject
{
  private:
        cPar *(*delayfunc)();     // delay
        cPar *(*errorfunc)();     // bit error rate
        cPar *(*dataratefunc)();  // data rate
  public:
        cLinkType( char *namestr, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)() );
        cLinkType( cLinkType& li );
        virtual ~cLinkType()    {}

        // redefined functions
        virtual char *className()  {return "cLinkType";}
        virtual cObject *dup()     {return new cLinkType(*this);}
        cLinkType& operator=(cLinkType& o);

        // new functions
        cPar *createDelay();
        cPar *createError();
        cPar *createDataRate();
};

//==========================================================================
// cNetworkType - Info to setup a network: namestr, no. of modules, setup function
//  NOTE: CANNOT dup() itself!

class cNetworkType : public cObject
{
   public:
        void (*setupfunc)();
   public:
        cNetworkType(char *namestr, void (*f)()) :
          cObject(namestr,(cObject *)&networks), setupfunc(f) {}
        virtual ~cNetworkType() {}

        // redefined functions
        virtual char *className()  {return "cNetworkType";}
};

//==========================================================================
// cFunctionType - Stores a function pointer (returning a double)
//  NOTE: CANNOT dup() itself!

class cFunctionType : public cObject
{
   public:
        MathFunc f;
        int argcount;
   public:
        cFunctionType(char *namestr, MathFunc f0, int argc) :
          cObject(namestr,(cObject *)&functions) {f=f0;argcount=argc;}
        virtual ~cFunctionType() {}

        // redefined functions
        virtual char *className()  {return "cFunctionType";}
};

cFunctionType *findfunctionbyptr(MathFunc f);

//==========================================================================
// cClassRegister - knows how to create an object of a specific type
//  NOTE: CANNOT dup() itself!

class cClassRegister : public cObject
{
        cObject *(*creatorfunc)();
   public:
        cClassRegister(char *namestr, cObject *(*f)()) :
          cObject(namestr,(cObject *)&classes), creatorfunc(f) {}
        virtual ~cClassRegister() {}

        // redefined functions
        virtual char *className()  {return "cClassRegister";}

        // new functions
        cObject *createOne()  {return creatorfunc();}
};

cObject *createOne(char *type);

//==========================================================================
// cInspectorFactory - base class for inspector factories of specific classes
//  NOTE: CANNOT dup() itself!

class cInspectorFactory : public cObject
{
        TInspector *(*inspFactoryFunc)(cObject *,int,void *);
   public:
        cInspectorFactory(char *name, TInspector *(*f)(cObject *,int,void *));
        virtual ~cInspectorFactory() {}

        // redefined functions
        virtual char *className()  {return "cInspectorFactory";}

        // new functions;
        TInspector *createInspectorFor(cObject *object,int type,void *data);
};


#endif

