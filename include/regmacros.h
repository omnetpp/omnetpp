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
 * Commonly used \<math.h\> functions have Define_Function() lines in the \opp
 * simulation kernel.
 *
 * @see Define_NED_Function
 *
 * @hideinitializer
 */
#define Define_Function(NAME,ARGCOUNT) \
  EXECUTE_ON_STARTUP(nedFunctions.getInstance()->add(new cMathFunction(#NAME,NAME,ARGCOUNT));)

/**
 * Like Define_Function(), but takes three arguments, the second one being the
 * pointer to the function. This macro allows registering a function with a
 * different name from its implementation.
 *
 * @hideinitializer
 */
#define Define_Function2(NAME,FUNCTION,ARGCOUNT) \
  EXECUTE_ON_STARTUP(nedFunctions.getInstance()->add(new cMathFunction(#NAME,FUNCTION,ARGCOUNT));)

/**
 * Like Define_Function(), but takes category and description strings as well.
 *
 * @hideinitializer
 */
#define Define_Function3(NAME,ARGCOUNT,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(nedFunctions.getInstance()->add(new cMathFunction(#NAME,NAME,ARGCOUNT,CATEGORY,DESCRIPTION));)

/**
 * Like Define_Function2(), but takes category and description strings as well.
 *
 * @hideinitializer
 */
#define Define_Function4(NAME,FUNCTION,ARGCOUNT,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(nedFunctions.getInstance()->add(new cMathFunction(#NAME,FUNCTION,ARGCOUNT,CATEGORY,DESCRIPTION));)

/**
 * Registers a C/C++ function for use in NED and ini files; see cNEDFunction.
 * FUNCTION is a pointer to the function, and SIGNATURE is the function's
 * signature in NED.
 *
 * The C++ function should have the following signature:
 * <pre>Value f(cComponent *context, Value argv[], int argc)</pre>
 * where Value stands for cDynamicExpression::Value.
 *
 * SIGNATURE is a string with the following syntax:
 * <pre>returntype name(argtype argname,...)</pre>
 * where types can be bool, long, double, quantity, string, xml, any
 * (where quantity is a double with a unit of measurement); names of
 * optional args should end in '?'.
 *
 * Example:
 * <pre>
 * Define_NED_Function(uniformFunc,"quantity uniform(quantity a, quantity b, long rng?)")
 * </pre>
 *
 * @hideinitializer
 */
#define Define_NED_Function(FUNCTION,SIGNATURE) \
  EXECUTE_ON_STARTUP(nedFunctions.getInstance()->add(new cNEDFunction(FUNCTION,SIGNATURE));)

/**
 * Like Define_NED_Function(), but allows one to specify a category string
 * and documentation as well.
 *
 * @hideinitializer
 */
#define Define_NED_Function2(FUNCTION,SIGNATURE,CATEGORY,DESCRIPTION) \
  EXECUTE_ON_STARTUP(nedFunctions.getInstance()->add(new cNEDFunction(FUNCTION,SIGNATURE,CATEGORY,DESCRIPTION));)

/**
 * Register class. This defines a factory object which makes it possible
 * to create an object by the passing class name to the createOne() function.
 * The class must be a subclass of cObject, otherwise a compile-time error
 * will occur: <i>"cannot convert..."</i>
 *
 * @hideinitializer
 */
#define Register_Class(CLASSNAME) \
  static cObject *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(classes.getInstance()->add(new cClassFactory(opp_typename(typeid(CLASSNAME)),__FILEUNIQUENAME__));)

/**
 * Announces the C++ simple module class to \opp, and couples it with the
 * NED simple module declaration of the same name.
 *
 * @hideinitializer
 */
// Implementation note: this is basically a Register_Class(), making sure the class subclasses from cModule.
#define Define_Module(CLASSNAME) \
  static cObject *__FILEUNIQUENAME__() {cModule *ret = new CLASSNAME; return ret; } \
  EXECUTE_ON_STARTUP(classes.getInstance()->add(new cClassFactory(opp_typename(typeid(CLASSNAME)),__FILEUNIQUENAME__,"module"));)

/**
 * Announces the C++ simple module class to \opp, and couples it with the
 * NED simple module declaration of the given name.
 *
 * This macro has been previously marked as deprecated, and was removed in \opp 4.0.
 *
 * @hideinitializer
 */
#define Define_Module_Like(CLASSNAME,NEDNAME)  NOTE_the_deprecated_Define_Module_Like_macro_has_been_removed_in_version_4_0;

/**
 * Announces the C++ channel class to \opp, and couples it with the
 * NED channel declaration of the same name.
 *
 * @hideinitializer
 */
// Implementation note: this is basically a Register_Class().
#define Define_Channel(CLASSNAME) \
  static cObject *__FILEUNIQUENAME__() {cChannel *ret = new CLASSNAME; return ret; } \
  EXECUTE_ON_STARTUP(classes.getInstance()->add(new cClassFactory(opp_typename(typeid(CLASSNAME)),__FILEUNIQUENAME__, "channel"));)

/**
 * Announces the C++ channel class to \opp, and couples it with the
 * NED channel declaration of the given name.
 *
 * This macro has been previously marked as deprecated, and was removed in \opp 4.0.
 *
 * @hideinitializer
 */
// Implementation note: this is basically a Register_Class(), only we lie about the class name.
#define Define_Channel_Like(CLASSNAME,NEDNAME) NOTE_the_deprecated_Define_Channel_Like_macro_has_been_removed_in_version_4_0;

/**
 * Internal. Registers a class descriptor which provides reflection information.
 *
 * @hideinitializer
 */
#define Register_ClassDescriptor(DESCRIPTORCLASS) \
  EXECUTE_ON_STARTUP(classDescriptors.getInstance()->add(new DESCRIPTORCLASS());)

/**
 * This macro has been previously marked as deprecated, and was removed in \opp 4.0.
 * Existing occurrences should be replaced with a public default constructor. That is,
 *
 * <pre>
 * Module_Class_Members(Foo, cSimpleModule, 0)
 * </pre>
 *
 * should become
 *
 * <pre>
 * public:
 *   Foo() : cSimpleModule() {}
 * </pre>
 *
 * @hideinitializer
 */
#define Module_Class_Members(CLASSNAME,BASECLASS,STACK)  NOTE_the_deprecated_Module_Class_Members_macro_has_been_removed_in_version_4_0
//@}


/**
 * @ingroup Macros
 * @defgroup MacrosConfigReg Configuration entry declaration macros
 */
//@{

// internal
#define __REGISTER_CONFIGOPTION(ID, ARGLIST) \
  cConfigOption *ID; \
  EXECUTE_ON_STARTUP(configOptions.getInstance()->add(ID = new cConfigOption ARGLIST);)

/**
 * Generic, with unit==NULL.
 * @hideinitializer
 */
#define Register_GlobalConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, true, cConfigOption::TYPE, NULL, DEFAULTVALUE, DESCRIPTION))

/**
 * Generic, with unit==NULL.
 * @hideinitializer
 */
#define Register_PerRunConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, false, cConfigOption::TYPE, NULL, DEFAULTVALUE, DESCRIPTION))

/**
 * For type==CFG_DOUBLE and a unit.
 * @hideinitializer
 */
#define Register_GlobalConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, true, cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * For type==CFG_DOUBLE and a unit.
 * @hideinitializer
 */
#define Register_PerRunConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, false, false, cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

/**
 * Per-object option (can be configured per run), with unit==NULL.
 * @hideinitializer
 */
#define Register_PerObjectConfigOption(ID, NAME, TYPE, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, true, false, cConfigOption::TYPE, NULL, DEFAULTVALUE, DESCRIPTION))

/**
 * Per-object option (can be configured per run), for type==CFG_DOUBLE and a unit.
 * @hideinitializer
 */
#define Register_PerObjectConfigOptionU(ID, NAME, UNIT, DEFAULTVALUE, DESCRIPTION) \
  __REGISTER_CONFIGOPTION(ID, (NAME, true, false, cConfigOption::CFG_DOUBLE, UNIT, DEFAULTVALUE, DESCRIPTION))

//@}

NAMESPACE_END


#endif

