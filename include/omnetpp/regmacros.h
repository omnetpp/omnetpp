//==========================================================================
//  REGMACROS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_REGMACROS_H
#define __OMNETPP_REGMACROS_H

#include "simkerneldefs.h"
#include "simutil.h"  // opp_typename

namespace omnetpp {

/**
 * @ingroup RegMacros
 * @{
 */

/**
 * @brief Registers a C/C++ mathematical function for use in NED and ini files. The function
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
  EXECUTE_ON_STARTUP(omnetpp::internal::nedFunctions.getInstance()->add(new omnetpp::cNedMathFunction(#NAME,NAME,ARGCOUNT));)

/**
 * @brief Like Define_NED_Math_Function(), but takes three arguments, the second one being the
 * pointer to the function. This macro allows registering a function with a
 * different name from its implementation.
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function2(NAME,FUNCTION,ARGCOUNT) \
  EXECUTE_ON_STARTUP(omnetpp::internal::nedFunctions.getInstance()->add(new omnetpp::cNedMathFunction(#NAME,FUNCTION,ARGCOUNT));)

/**
 * @brief Like Define_NED_Math_Function(), but takes category and description strings as well.
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function3(NAME,ARGCOUNT,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(omnetpp::internal::nedFunctions.getInstance()->add(new omnetpp::cNedMathFunction(#NAME,NAME,ARGCOUNT,CATEGORY,DESCRIPTION));)

/**
 * @brief Like Define_NED_Math_Function2(), but takes category and description strings as well.
 *
 * @hideinitializer
 */
#define Define_NED_Math_Function4(NAME,FUNCTION,ARGCOUNT,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(omnetpp::internal::nedFunctions.getInstance()->add(new omnetpp::cNedMathFunction(#NAME,FUNCTION,ARGCOUNT,CATEGORY,DESCRIPTION));)

/**
 * @brief Registers a C/C++ function for use in NED and ini files; see cNedFunction.
 * FUNCTION is a pointer to the function, and SIGNATURE is the function's
 * signature in NED.
 *
 * The C++ function can have either of the following two signatures:
 * <pre>
 * cValue f(cComponent *context, cValue argv[], int argc);  // typedef NedFunction
 * cValue f(cExpression::Context *context, cValue argv[], int argc);  // typedef NedFunctionExt
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
  EXECUTE_ON_STARTUP(omnetpp::internal::nedFunctions.getInstance()->add(new omnetpp::cNedFunction(FUNCTION,SIGNATURE));)

/**
 * @brief Like Define_NED_Function(), but allows one to specify a category string
 * and documentation as well.
 *
 * @hideinitializer
 */
#define Define_NED_Function2(FUNCTION,SIGNATURE,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(omnetpp::internal::nedFunctions.getInstance()->add(new omnetpp::cNedFunction(FUNCTION,SIGNATURE,CATEGORY,DESCRIPTION));)

/**
 * @brief Register class. This defines a factory object which makes it possible
 * to create an object by the passing class name to the createOne() function.
 * The class must be a subclass of cObject, otherwise a compile-time error
 * will occur: <i>"cannot convert..."</i>
 *
 * @see cObjectFactory, cObjectFactory::createOne(), cObjectFactory::isInstance()
 * @hideinitializer
 */
#define Register_Class(CLASSNAME)  __REGISTER_CLASS(CLASSNAME, omnetpp::cObject, "class")

/**
 * @brief Register an abstract class. Registration makes it possible to dynamically test
 * whether an object is a subclass of the registered class (see cObjectFactory::isInstance()).
 * The class must be a subclass of cObject, otherwise a compile-time error
 * will occur: <i>"cannot convert..."</i>
 *
 * @see cObjectFactory, cObjectFactory::isInstance()
 * @hideinitializer
 */
#define Register_Abstract_Class(CLASSNAME)  __REGISTER_ABSTRACT_CLASS(CLASSNAME, omnetpp::cObject, "class")

/**
 * @brief Announces the C++ simple module class to \opp, and couples it with the
 * NED simple module declaration of the same name.
 *
 * @hideinitializer
 */
#define Define_Module(CLASSNAME)  __REGISTER_CLASS(CLASSNAME, omnetpp::cModule, "module")

/**
 * @brief Announces the C++ channel class to \opp, and couples it with the
 * NED channel declaration of the same name.
 *
 * @hideinitializer
 */
#define Define_Channel(CLASSNAME)  __REGISTER_CLASS(CLASSNAME, omnetpp::cChannel, "channel")

// internal
#define __REGISTER_CLASS(CLASSNAME, BASECLASS, DESC) \
  __REGISTER_CLASS_X(CLASSNAME, BASECLASS, DESC, /*nothing*/ )

// internal
#define __REGISTER_CLASS_X(CLASSNAME, BASECLASS, DESC, EXTRACODE) \
  static omnetpp::cObject *MAKE_UNIQUE_WITHIN_FILE(__factoryfunc_)() {BASECLASS *ret = new CLASSNAME; return ret; } \
  static void *MAKE_UNIQUE_WITHIN_FILE(__castfunc_)(omnetpp::cObject *obj) {return (void*)dynamic_cast<CLASSNAME*>(obj);} \
  EXECUTE_ON_STARTUP(omnetpp::internal::classes.getInstance()->add(new omnetpp::cObjectFactory(omnetpp::opp_typename(typeid(CLASSNAME)), MAKE_UNIQUE_WITHIN_FILE(__factoryfunc_), MAKE_UNIQUE_WITHIN_FILE(__castfunc_), DESC)); EXTRACODE; )

// internal
#define __REGISTER_ABSTRACT_CLASS(CLASSNAME, BASECLASS /*unused*/, DESC) \
  static void *MAKE_UNIQUE_WITHIN_FILE(__castfunc_)(omnetpp::cObject *obj) {return (void*)dynamic_cast<CLASSNAME*>(obj);} \
  EXECUTE_ON_STARTUP(omnetpp::internal::classes.getInstance()->add(new omnetpp::cObjectFactory(omnetpp::opp_typename(typeid(CLASSNAME)), nullptr, MAKE_UNIQUE_WITHIN_FILE(__castfunc_), DESC));)

/**
 * @brief Internal. Registers a class descriptor which provides reflection information.
 *
 * @hideinitializer
 */
#define Register_ClassDescriptor(DESCRIPTORCLASS) \
  EXECUTE_ON_STARTUP(omnetpp::internal::classDescriptors.getInstance()->add(new DESCRIPTORCLASS());)

// internal
#define __REGISTER_CONFIGOPTION(ID, ARGLIST) \
  omnetpp::cConfigOption *ID; \
  EXECUTE_ON_STARTUP(omnetpp::internal::configOptions.getInstance()->add(ID = new omnetpp::cConfigOption ARGLIST);)

/**
 * @brief Registers a global configuration option.
 *
 * @hideinitializer
 */
#define Register_GlobalConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, true, omnetpp::cConfigOption::TYPE, nullptr, DEFAULTVALUE, DESCRIPTION))

/**
 * @brief Registers a per-run configuration option.
 *
 * @hideinitializer
 */
#define Register_PerRunConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, omnetpp::cConfigOption::TYPE, nullptr, DEFAULTVALUE, DESCRIPTION))

/**
 * @brief Registers a global configuration option with type==CFG_DOUBLE and an
 * expected unit of measurement.
 *
 * @hideinitializer
 */
#define Register_GlobalConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, true, omnetpp::cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * @brief Registers a per-run configuration option with type==CFG_DOUBLE and an
 * expected unit of measurement.
 *
 * @hideinitializer
 */
#define Register_PerRunConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, omnetpp::cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * @brief Registers a per-object configuration option.
 *
 * @hideinitializer
 */
#define Register_PerObjectConfigOption(ID, NAME, KIND, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, omnetpp::cConfigOption::KIND, omnetpp::cConfigOption::TYPE, nullptr, DEFAULTVALUE, DESCRIPTION))

/**
 * @brief Registers a per-object configuration option with type==CFG_DOUBLE and an
 * expected unit of measurement.
 *
 * @hideinitializer
 */
#define Register_PerObjectConfigOptionU(ID, NAME, KIND, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, omnetpp::cConfigOption::KIND, omnetpp::cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * @brief Registers a message printer class (cMessagePrinter)
 *
 * @hideinitializer
 */
#define Register_MessagePrinter(CLASSNAME) \
  EXECUTE_ON_STARTUP(omnetpp::internal::messagePrinters.getInstance()->add(new CLASSNAME());)

/**
 * @brief Registers an enum.
 *
 * Example:
 * <pre>
 * enum State { IDLE, BUSY, SLEEPING };
 * Register_Enum(State, (
 *    State::IDLE,
 *    State::BUSY,
 *    State::SLEEPING));
 * </pre>
 *
 * @see cEnum
 * @hideinitializer
 */
#define Register_Enum(NAME, VALUES)  \
  EXECUTE_ON_STARTUP(omnetpp::internal::enums.getInstance()->add((new omnetpp::cEnum(omnetpp::opp_typename(typeid(NAME))))->registerNames(#VALUES)->registerValues VALUES))

/**
 * @brief Registers an enum, and makes it accessible via a global cEnum* pointer,
 * using explicit strings for the enum type and it member names.
 *
 * Example:
 * <pre>
 * enum State { IDLE, BUSY, SLEEPING };
 * Register_Enum2(stateEnum, "State", (
 *    "idle",     State::IDLE,
 *    "busy",     State::BUSY,
 *    "sleeping", State::SLEEPING,
 *    nullptr)); // see note below
 * </pre>
 *
 * Note: One may need to put (void*)nullptr as the last item, due to a bug in
 * certain compilers (they push a 32-bit zero value instead a of a 64-bit one
 * in the "..." arg list, causing the called function to be unable to determine
 * the end of the argument list).
 *
 * @see cEnum
 * @hideinitializer
 */
#define Register_Enum2(VAR, NAME, VALUES)  \
  static omnetpp::cEnum *VAR; \
  EXECUTE_ON_STARTUP(VAR = new omnetpp::cEnum(NAME); VAR->bulkInsert VALUES; omnetpp::internal::enums.getInstance()->add(VAR))

/**
 * @brief Registers a new figure type. The macro expects a type name string
 * (an identifier that can be used in the type attribute of the \@figure property),
 * and a C++ class name. The class must be derived from cFigure.
 *
 * Note: Register_Figure() includes registration the figure class via a Register_Class().
 *
 * Example:
 * <pre>
 * Register_Figure("polyline", cPolylineFigure);
 * </pre>
 *
 * @hideinitializer
 */
#define Register_Figure(NAME, CLASSNAME)  \
  __REGISTER_CLASS_X(CLASSNAME, omnetpp::cFigure, "figure", omnetpp::internal::figureTypes[NAME] = omnetpp::opp_typename(typeid(CLASSNAME)))

/** @} */

}  // namespace omnetpp


#endif

