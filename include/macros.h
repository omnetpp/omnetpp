//==========================================================================
//   MACROS.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Definition of the following macros:
//    Define_Network, Define_Link
//    ModuleInterface..End
//    Define_Module
//    Module_Class_Members
//    Define_Function
//    Register_Class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MACROS_H
#define __MACROS_H

#include "ctypes.h"

//=========================================================================

/**
 * @name Declaration macros
 * @ingroup Macros
 */
//@{

/**
 * Network declaration macro.
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Define_Network(NAME,SETUPFUNC) \
  void SETUPFUNC(); \
  static cNetworkType NAME##__network(#NAME,SETUPFUNC);

/**
 * Actual link definition.
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Define_Link(NAME,DELAY,ERROR,DATARATE) \
  static cLinkType NAME##__link(#NAME, DELAY, ERROR, DATARATE);

/**
 * Math function registration (makes it possible to use func in NED files).
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Define_Function(FUNCTION,ARGCOUNT) \
  cFunctionType FUNCTION##__functype(#FUNCTION,(MathFunc)FUNCTION,ARGCOUNT);

/**
 * Register class. This defines a factory object which makes it possible
 * to create an object by the passing class name to the createOne() function.
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Register_Class(CLASSNAME) \
  cObject *CLASSNAME##__create() {return new CLASSNAME;} \
  cClassRegister CLASSNAME##__reg(#CLASSNAME,CLASSNAME##__create);

/**
 * Register inspector factory. Used internally by graphical interface
 * libraries like Tkenv.
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Register_InspectorFactory(FACTORYNAME,FUNCTION) \
  TInspector *FUNCTION(cObject *, int, void *); \
  cInspectorFactory FACTORYNAME##__inspfct(#FACTORYNAME,FUNCTION);

//@}

//=========================================================================

// declaration of module gates and params
#define Interface(CLASSNAME)    static sDescrItem CLASSNAME##__descr[] = {
#define Gate(NAME,TYPE)         {'G', #NAME, NULL,  TYPE},
#define Parameter(NAME,TYPES)   {'P', #NAME, TYPES, 0   },
#define Machine(NAME)           {'M', #NAME, NULL,  0   },
#define EndInterface            {'E', NULL,  NULL,  0   }};

#define Register_Interface(CLASSNAME) \
  static cModuleInterface CLASSNAME##__interface( #CLASSNAME, CLASSNAME##__descr);

// gate types:
#define GateDir_Input      'I'
#define GateDir_Output     'O'

// param allowed types:
#define ParType_Const      "#"
#define ParType_Any        "*"
#define ParType_Numeric    "LDXFT"
#define ParType_Bool       "B"
#define ParType_String     "S"

/**
 * @name Module declaration macros
 * @ingroup Macros
 */
//@{

/**
 * Create cModuleType. Version 1: module interface has the same name as module.
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Define_Module(CLASSNAME) \
  static cModule *CLASSNAME##__create(const char *name, cModule *parentmod ) \
  { \
     return (cModule *) new CLASSNAME(name, parentmod); \
  } \
  cModuleType CLASSNAME##__type(#CLASSNAME,#CLASSNAME,(ModuleCreateFunc)CLASSNAME##__create);

/**
 * Create cModuleType. Version 2: use different module interface (support for
 * 'like' phrase in NED).
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Define_Module_Like(CLASSNAME,INTERFACE) \
  static cModule *CLASSNAME##__create(const char *name, cModule *parentmod ) \
  { \
     return (cModule *) new CLASSNAME(name, parentmod); \
  } \
  cModuleType CLASSNAME##__type(#CLASSNAME,#INTERFACE,(ModuleCreateFunc)CLASSNAME##__create);

/**
 * Declaration of module class members.
 * FIXME: refine doc!
 * @hideinitializer
 */
#define Module_Class_Members(CLASSNAME,BASECLASS,STACK) \
    public: \
      CLASSNAME(const char *name, cModule *parentmod, unsigned stk=STACK) : \
           BASECLASS(name, parentmod, stk) {} \
      virtual const char *className() const {return #CLASSNAME;}
//@}

#endif

