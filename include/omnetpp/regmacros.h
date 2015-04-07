//==========================================================================
//  REGMACROS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __REGMACROS_H
#define __REGMACROS_H

#include "simkerneldefs.h"

NAMESPACE_BEGIN

/**
 * @ingroup Macros
 * @defgroup MacrosReg Registration macros
 */
//@{

/**
 * Registers a C/C++ mathematical function for use in NED and ini files. The function
 * may take 0, 1, 2 or 3 double arguments, and must return a double.
 * Corresponding C/C++ typedefs: MathFuncNoArg, MathFunc1Arg, MathFunc2Args,
 * MathFunc3Args, MathFunc4Args.
 *
 * Commonly used \<math.h\> functions have Define_NED_Math_Function() lines in the \opp
 * simulation kernel.
 *
 * @see Define_NED_Function
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function(NAME,ARGCOUNT) \
  EXECUTE_ON_STARTUP(OPP::nedFunctions.getInstance()->add(new OPP::cNEDMathFunction(#NAME,NAME,ARGCOUNT));)

/**
 * Like Define_NED_Math_Function(), but takes three arguments, the second one being the
 * pointer to the function. This macro allows registering a function with a
 * different name from its implementation.
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function2(NAME,FUNCTION,ARGCOUNT) \
  EXECUTE_ON_STARTUP(OPP::nedFunctions.getInstance()->add(new OPP::cNEDMathFunction(#NAME,FUNCTION,ARGCOUNT));)

/**
 * Like Define_NED_Math_Function(), but takes category and description strings as well.
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function3(NAME,ARGCOUNT,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(OPP::nedFunctions.getInstance()->add(new OPP::cNEDMathFunction(#NAME,NAME,ARGCOUNT,CATEGORY,DESCRIPTION));)

/**
 * Like Define_NED_Math_Function2(), but takes category and description strings as well.
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function4(NAME,FUNCTION,ARGCOUNT,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(OPP::nedFunctions.getInstance()->add(new OPP::cNEDMathFunction(#NAME,FUNCTION,ARGCOUNT,CATEGORY,DESCRIPTION));)

/**
 * Registers a C/C++ function for use in NED and ini files; see cNEDFunction.
 * FUNCTION is a pointer to the function, and SIGNATURE is the function's
 * signature in NED.
 *
 * The C++ function should have the following signature:
 * <pre>
 * cNEDValue f(cComponent *context, cNEDValue argv[], int argc)
 * </pre>
 *
 * SIGNATURE is a string with the following syntax:
 * <tt>returntype functionname(argtype1 argname1, ...)</tt>, where a type
 * can be one of 'bool', 'int', 'double', 'quantity' ('double' with measurement
 * unit), 'string', 'xml' and 'any'. Optional arguments are denoted by
 * appending a '?' to their names. To let the function accept any number of
 * additional arguments of arbitrary types, add ',...' to the signature.
 *
 * Example:
 * <pre>
 * Define_NED_Function(uniformFunc,"quantity uniform(quantity a, quantity b, long rng?)")
 * </pre>
 *
 * @hideinitializer
 */
#define Define_NED_Function(FUNCTION,SIGNATURE) \
  EXECUTE_ON_STARTUP(OPP::nedFunctions.getInstance()->add(new OPP::cNEDFunction(FUNCTION,SIGNATURE));)

/**
 * Like Define_NED_Function(), but allows one to specify a category string
 * and documentation as well.
 *
 * @hideinitializer
 */
#define Define_NED_Function2(FUNCTION,SIGNATURE,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(OPP::nedFunctions.getInstance()->add(new OPP::cNEDFunction(FUNCTION,SIGNATURE,CATEGORY,DESCRIPTION));)

/**
 * Register class. This defines a factory object which makes it possible
 * to create an object by the passing class name to the createOne() function.
 * The class must be a subclass of cObject, otherwise a compile-time error
 * will occur: <i>"cannot convert..."</i>
 *
 * @see cObjectFactory, cObjectFactory::createOne(), cObjectFactory::isInstance()
 * @hideinitializer
 */
#define Register_Class(CLASSNAME)  __REGISTER_CLASS(CLASSNAME, OPP::cObject, "class")

/**
 * Register an abstract class. Registration makes it possible to dynamically test
 * whether an object is a subclass of the registered class (see cObjectFactory::isInstance()).
 * The class must be a subclass of cObject, otherwise a compile-time error
 * will occur: <i>"cannot convert..."</i>
 *
 * @see cObjectFactory, cObjectFactory::isInstance()
 * @hideinitializer
 */
#define Register_Abstract_Class(CLASSNAME)  __REGISTER_ABSTRACT_CLASS(CLASSNAME, OPP::cObject, "class")

/**
 * Announces the C++ simple module class to \opp, and couples it with the
 * NED simple module declaration of the same name.
 *
 * @hideinitializer
 */
#define Define_Module(CLASSNAME)  __REGISTER_CLASS(CLASSNAME, OPP::cModule, "module")

/**
 * Announces the C++ channel class to \opp, and couples it with the
 * NED channel declaration of the same name.
 *
 * @hideinitializer
 */
#define Define_Channel(CLASSNAME)  __REGISTER_CLASS(CLASSNAME, OPP::cChannel, "channel")

// internal
#define __REGISTER_CLASS(CLASSNAME, BASECLASS, DESC) \
  static OPP::cObject *MAKE_UNIQUE_WITHIN_FILE(__factoryfunc_)() {BASECLASS *ret = new CLASSNAME; return ret; } \
  static void *MAKE_UNIQUE_WITHIN_FILE(__castfunc_)(OPP::cObject *obj) {return (void*)dynamic_cast<CLASSNAME*>(obj);} \
  EXECUTE_ON_STARTUP(OPP::classes.getInstance()->add(new OPP::cObjectFactory(OPP::opp_typename(typeid(CLASSNAME)), MAKE_UNIQUE_WITHIN_FILE(__factoryfunc_), MAKE_UNIQUE_WITHIN_FILE(__castfunc_), DESC));)

// internal
#define __REGISTER_ABSTRACT_CLASS(CLASSNAME, BASECLASS /*unused*/, DESC) \
  static void *MAKE_UNIQUE_WITHIN_FILE(__castfunc_)(OPP::cObject *obj) {return (void*)dynamic_cast<CLASSNAME*>(obj);} \
  EXECUTE_ON_STARTUP(OPP::classes.getInstance()->add(new OPP::cObjectFactory(OPP::opp_typename(typeid(CLASSNAME)), NULL, MAKE_UNIQUE_WITHIN_FILE(__castfunc_), DESC));)

/**
 * Internal. Registers a class descriptor which provides reflection information.
 *
 * @hideinitializer
 */
#define Register_ClassDescriptor(DESCRIPTORCLASS) \
  EXECUTE_ON_STARTUP(OPP::classDescriptors.getInstance()->add(new DESCRIPTORCLASS());)

// internal
#define __REGISTER_CONFIGOPTION(ID, ARGLIST) \
  cConfigOption *ID; \
  EXECUTE_ON_STARTUP(OPP::configOptions.getInstance()->add(ID = new OPP::cConfigOption ARGLIST);)

/**
 * Registers a global configuration option.
 *
 * @hideinitializer
 */
#define Register_GlobalConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, true, OPP::cConfigOption::TYPE, NULL, DEFAULTVALUE, DESCRIPTION))

/**
 * Registers a per-run configuration option.
 *
 * @hideinitializer
 */
#define Register_PerRunConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, OPP::cConfigOption::TYPE, NULL, DEFAULTVALUE, DESCRIPTION))

/**
 * Registers a global configuration option with type==CFG_DOUBLE and an
 * expected unit of measurement.
 *
 * @hideinitializer
 */
#define Register_GlobalConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, true, OPP::cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * Registers a per-run configuration option with type==CFG_DOUBLE and an
 * expected unit of measurement.
 *
 * @hideinitializer
 */
#define Register_PerRunConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, OPP::cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * Registers a per-object configuration option.
 *
 * @hideinitializer
 */
#define Register_PerObjectConfigOption(ID, NAME, KIND, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, OPP::cConfigOption::KIND, OPP::cConfigOption::TYPE, NULL, DEFAULTVALUE, DESCRIPTION))

/**
 * Registers a per-object configuration option with type==CFG_DOUBLE and an
 * expected unit of measurement.
 *
 * @hideinitializer
 */
#define Register_PerObjectConfigOptionU(ID, NAME, KIND, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, OPP::cConfigOption::KIND, OPP::cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * Registers a message printer class (cMessagePrinter)
 *
 * @hideinitializer
 */
#define Register_MessagePrinter(CLASSNAME) \
  EXECUTE_ON_STARTUP(OPP::messagePrinters.getInstance()->add(new CLASSNAME());)

/**
 * Registers an enum.
 *
 * Example:
 * <pre>
 * enum State { IDLE, BUSY, SLEEPING };
 * Register_Enum(State, (
 *    "idle",     State::IDLE,
 *    "busy",     State::BUSY,
 *    "sleeping", State::SLEEPING,
 *    NULL));
 * </pre>
 *
 * @see cEnum
 * @hideinitializer
 */
#define Register_Enum(NAME, VALUES)  \
  EXECUTE_ON_STARTUP(OPP::enums.getInstance()->add((new OPP::cEnum(#NAME))->registerNames(#VALUES)->registerValues VALUES))

/**
 * Registers an enum, and makes it accessible via a global cEnum* pointer.
 *
 * Example:
 * <pre>
 * enum State { IDLE, BUSY, SLEEPING };
 * Register_Enum2(stateEnum, "State", (
 *    "idle",     State::IDLE,
 *    "busy",     State::BUSY,
 *    "sleeping", State::SLEEPING,
 *    NULL));
 * </pre>
 *
 * @see cEnum
 * @hideinitializer
 */
#define Register_Enum2(VAR, NAME, VALUES)  \
  static OPP::cEnum *VAR; \
  EXECUTE_ON_STARTUP(VAR = new OPP::cEnum(NAME); VAR->bulkInsert VALUES; OPP::enums.getInstance()->add(VAR))

//@}

NAMESPACE_END


#endif

