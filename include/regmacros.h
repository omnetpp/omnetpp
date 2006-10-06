//==========================================================================
//  REGMACROS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __REGMACROS_H
#define __REGMACROS_H

#include "defs.h"
#include "onstartup.h"


// Note: this header file is included from "defs.h" directly

/**
 * @name NED function registration macros
 * @ingroup Macros
 */
//@{

/**
 * Registers a C/C++ mathematical function for use in NED and ini files. The function
 * may take 0, 1, 2 or 3 double arguments, and must return a double.
 * Corresponding C/C++ typedefs: MathFuncNoArg, MathFunc1Arg, MathFunc2Args,
 * MathFunc3Args, MathFunc4Args.
 *
 * Commonly used <math.h> functions have Define_Function() lines in the \opp
 * simulation kernel.
 *
 * @see Define_NED_Function
 *
 * @hideinitializer
 */
#define Define_Function(NAME,ARGCOUNT) \
  EXECUTE_ON_STARTUP(NAME##__##ARGCOUNT##__func, nedFunctions.instance()->add(new cMathFunction(#NAME,NAME,ARGCOUNT));)

/**
 * Like Define_Function(), but takes three arguments, the second one being the
 * pointer to the function. This macro allows registering a function with a
 * different name than its implementation.
 *
 * @hideinitializer
 */
#define Define_Function2(NAME,FUNCTION,ARGCOUNT) \
  EXECUTE_ON_STARTUP(NAME##__##ARGCOUNT##__func, nedFunctions.instance()->add(new cMathFunction(#NAME,FUNCTION,ARGCOUNT));)

/**
 * Registers a C/C++ function for use in NED and ini files. The function
 * may take several arguments of type bool, long, double, string or XML element,
 * and must return a value of any of the above types. The argument types
 * should be passed into the macro as a string literal consisting of letters
 * "B", "L", "D", "S", "X", "*" (for any), and the return type is a similar
 * one-letter string ("*" is for unknown or variable return type).
 * Corresponding C/C++ typedefs: MathFuncNoArg, MathFunc1Arg, MathFunc2Args,
 * MathFunc3Args, MathFunc4Args.
 *
 * @hideinitializer
 */
#define Define_NED_Function(NAME,ARGTYPES,RETURNTYPE) \
  EXECUTE_ON_STARTUP(NAME##__afunc, nedFunctions.instance()->add(new cNEDFunction(#NAME,NAME,ARGTYPES,RETURNTYPE));)

/**
 * Like Define_NED_Function(), but it allows registering a function with a
 * different name than its implementation.
 *
 * @hideinitializer
 */
//FIXME argtypes without quote, and then it can be included in varname too?
#define Define_NED_Function2(NAME,FUNCTION,ARGTYPES,RETURNTYPE) \
  EXECUTE_ON_STARTUP(NAME##__##FUNCTION##__afunc, nedFunctions.instance()->add(new cMathFunction(#NAME,FUNCTION,ARGTYPES,RETURNTYPE));)
//@}


/**
 * @name Misc registration macros
 * @ingroup Macros
 */
//@{

/**
 * Register class. This defines a factory object which makes it possible
 * to create an object by the passing class name to the createOne() function.
 * The class must be a subclass of cPolymorphic, otherwise a compile-time error
 * will occur: <i>"cannot convert..."</i>
 *
 * @hideinitializer
 */
#define Register_Class(CLASSNAME) \
  cPolymorphic *CLASSNAME##__create() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(CLASSNAME##__class, classes.instance()->add(new cClassFactory(#CLASSNAME,CLASSNAME##__create));)
//@}


/**
 * @name Module declaration macros
 * @ingroup Macros
 */
//@{

/**
 * Announces the class as a module to \opp and couples it with the
 * NED interface of the same name. The macro expands to the definition
 * a cModuleType object.
 *
 * The NEDC compiler generates Define_Module() lines for all compound modules.
 * However, it is the user's responsibility to put Define_Module() lines for
 * all simple module types into one of the C++ sources.
 *
 * @hideinitializer
 */
#define Define_Module(CLASSNAME) \
  static cModule *CLASSNAME##__create() {return new CLASSNAME();} \
  EXECUTE_ON_STARTUP(CLASSNAME##__mod, modtypes.instance()->add(new cModuleType(#CLASSNAME,#CLASSNAME,(ModuleCreateFunc)CLASSNAME##__create));)

/**
 * Similar to Define_Module(), except that it couples the class with the
 * NED interface of the given name.
 *
 * While this macro continues to be supported, it is NOT RECOMMENDED because
 * modules defined with it don't show up in documentation generated with
 * opp_neddoc. One can use NED's <tt>like</tt> feature with the normal
 * Define_Module() macro too, it doesn't require Define_Module_Like().
 *
 * @hideinitializer
 */
#define Define_Module_Like(CLASSNAME,INTERFACENAME) \
  static cModule *CLASSNAME##__create() {return new CLASSNAME();} \
  EXECUTE_ON_STARTUP(CLASSNAME##__mod, modtypes.instance()->add(new cModuleType(#CLASSNAME,#INTERFACENAME,(ModuleCreateFunc)CLASSNAME##__create));)

/**
 * This macro facilitates the declaration of a simple module class, and
 * it expands to the definition of mandatory member functions.
 * (Currently only a constructor.)
 *
 * The macro is used like this:
 *
 * <PRE>
 *  class CLASSNAME : public cSimpleModule
 *  {
 *     Module_Class_Members(CLASSNAME,cSimpleModule,8192)
 *     virtual void activity();
 *  };
 * </PRE>
 *
 * @hideinitializer
 */
// TODO dummy args can be removed in a later version, when all models have been ported
#define Module_Class_Members(CLASSNAME,BASECLASS,STACK) \
    public: \
      CLASSNAME(const char *dummy1=0, cModule *dummy2=0, unsigned stk=STACK) : BASECLASS(0,0,stk) {}
//@}

#endif

