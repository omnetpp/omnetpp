//==========================================================================
//   NEDSUPPORT.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDSUPPORT_H
#define __NEDSUPPORT_H

#include "cdynamicexpression.h"

NAMESPACE_BEGIN

namespace NEDSupport {

class ModuleIndex : public cDynamicExpression::Functor
{
  public:
    ModuleIndex();
    ModuleIndex *dup() const {return new ModuleIndex();}
    virtual const char *getFullName() const {return "index";}
    virtual const char *getArgTypes() const {return "";}
    virtual char getReturnType() const {return 'L';}
    virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs);
};

/**
 * Variations: parameter, parentParameter
 */
class ParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->par(paramName)
    bool explicitKeyword; // when ofParent==false: whether the NED file contained the explicit "this" qualifier
    std::string paramName;
  public:
    ParameterRef(const char *paramName, bool ofParent, bool explicitKeyword);
    ParameterRef *dup() const {return new ParameterRef(paramName.c_str(), ofParent, explicitKeyword);}
    virtual const char *getFullName() const {return paramName.c_str();}
    virtual const char *getArgTypes() const {return "";}
    virtual char getReturnType() const {return '*';}
    virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs);
};

/**
 * siblingModuleParameter, indexedSiblingModuleParameter
 */
class SiblingModuleParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->getSubmodule(...)->par(parname)
    std::string moduleName;
    bool withModuleIndex;
    std::string paramName;
  public:
    SiblingModuleParameterRef(const char *moduleName, const char *paramName, bool ofParent, bool withModuleIndex);
    SiblingModuleParameterRef *dup() const {return new SiblingModuleParameterRef(moduleName.c_str(), paramName.c_str(), ofParent, withModuleIndex);}
    virtual const char *getFullName() const {return paramName.c_str();}
    virtual const char *getArgTypes() const {return withModuleIndex ? "L" : "";}
    virtual char getReturnType() const {return '*';}
    virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs);
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
    virtual const char *getFullName() const {return varName.c_str();}
    virtual const char *getArgTypes() const {return "";}
    virtual char getReturnType() const {return 'L';}
    virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs);
};


/**
 * sizeof operator. See also: SiblingModuleSizeof
 */
class Sizeof : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->gateSize(ident)
    bool explicitKeyword; // when ofParent==false: whether the NED file contained the explicit "this" qualifier
    std::string ident;
  public:
    Sizeof(const char *ident, bool ofParent, bool explicitKeyword);
    Sizeof *dup() const {return new Sizeof(ident.c_str(), ofParent, explicitKeyword);}
    virtual const char *getFullName() const {return ident.c_str();}
    virtual const char *getArgTypes() const {return "";}
    virtual char getReturnType() const {return 'L';}
    virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs);
};

/*XXX TODO
static cNEDValue getSizeofIdent(cComponent *context, cNEDValue args[], int numargs);
static cNEDValue getSizeofGate(cComponent *context, cNEDValue args[], int numargs);
static cNEDValue getSizeofParentModuleGate(cComponent *context, cNEDValue args[], int numargs);
static cNEDValue getSizeofSiblingModuleGate(cComponent *context, cNEDValue args[], int numargs);
static cNEDValue getSizeofIndexedSiblingModuleGate(cComponent *context, cNEDValue args[], int numargs);

class Sizeof : public Functor
{
  protected:
    bool ofParent;
    bool bstd::string paramName;
    std::string moduleName;
  public:
    virtual const char *getArgTypes() const {return "";}
    virtual char getReturnType() const {return 'L';}
    virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs) {return "index";}
};
*/

};

NAMESPACE_END


#endif


