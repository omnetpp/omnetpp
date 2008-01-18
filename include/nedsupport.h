//==========================================================================
//   NEDSUPPORT.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDSUPPORT_H
#define __NEDSUPPORT_H

#include "cdynamicexpression.h"

NAMESPACE_BEGIN

namespace NEDSupport {

typedef cDynamicExpression::StkValue StkValue; // abbreviation for local use

class ModuleIndex : public cDynamicExpression::Functor
{
  public:
    ModuleIndex();
    ModuleIndex *dup() const {return new ModuleIndex();}
    virtual const char *fullName() const {return "index";}
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return 'L';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/**
 * Variations: parameter, parentParameter
 */
class ParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->par(paramname)
    bool printThis; // whether toString() should prefix paramName with "this."
    std::string paramName;
  public:
    ParameterRef(const char *paramName, bool ofParent, bool printThis);
    ParameterRef *dup() const {return new ParameterRef(paramName.c_str(), ofParent, printThis);}
    virtual const char *fullName() const {return paramName.c_str();}
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return '*';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/**
 * siblingModuleParameter, indexedSiblingModuleParameter
 */
class SiblingModuleParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->submodule(...)->par(parname)
    std::string moduleName;
    bool withModuleIndex;
    std::string paramName;
  public:
    SiblingModuleParameterRef(const char *moduleName, const char *paramName, bool ofParent, bool withModuleIndex);
    SiblingModuleParameterRef *dup() const {return new SiblingModuleParameterRef(moduleName.c_str(), paramName.c_str(), ofParent, withModuleIndex);}
    virtual const char *fullName() const {return paramName.c_str();}
    virtual const char *argTypes() const {return withModuleIndex ? "L" : "";}
    virtual char returnType() const {return '*';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/**
 * i,j in NED "for" loops
 */
class LoopVar : public cDynamicExpression::Functor
{
  private:
    // the loopvar stack (vars of nested loops are pushed on the stack by cNEDNetworkBuilder)
    static const char *varNames[32];
    static long vars[32];
    static int varCount;
  public:
    static long& pushVar(const char *varName);
    static void popVar();
    static void reset();
    static const char **getVarNames() {return varNames;}
    static int getNumVars() {return varCount;}

  protected:
    std::string varName;
  public:
    LoopVar(const char *varName) {this->varName = varName;}
    LoopVar *dup() const {return new LoopVar(varName.c_str());}
    virtual const char *fullName() const {return varName.c_str();}
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return 'L';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};


/**
 * sizeof operator. See also: SiblingModuleSizeof
 */
class Sizeof : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->gateSize(ident)
    bool printThis; // whether toString() should prefix paramName with "this."
    std::string ident;
  public:
    Sizeof(const char *ident, bool ofParent, bool printThis);
    Sizeof *dup() const {return new Sizeof(ident.c_str(), ofParent, printThis);}
    virtual const char *fullName() const {return ident.c_str();}
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return 'L';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/**
 * xmldoc operator: xmldoc(file) or xmldoc(file,xpath)
 */
class XMLDoc : public cDynamicExpression::Functor
{
  protected:
    bool withXPath;  // whether 1 or 2 args
  public:
    XMLDoc(bool withXPath) {this->withXPath = withXPath;}
    XMLDoc *dup() const {return new XMLDoc(withXPath);}
    virtual const char *fullName() const {return "xmldoc";}
    virtual const char *argTypes() const {return withXPath ? "SS" : "S";}
    virtual char returnType() const {return 'X';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/*XXX TODO
static StkValue sizeofIdent(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofGate(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofParentModuleGate(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofSiblingModuleGate(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofIndexedSiblingModuleGate(cComponent *context, StkValue args[], int numargs);

class Sizeof : public Functor
{
  protected:
    bool ofParent;
    bool bstd::string paramName;
    std::string moduleName;
  public:
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return 'L';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs) {return "index";}
};
*/

};

NAMESPACE_END


#endif


