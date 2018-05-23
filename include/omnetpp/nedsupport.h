//==========================================================================
//   NEDSUPPORT.H  - part of
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

#ifndef __OMNETPP_NEDSUPPORT_H
#define __OMNETPP_NEDSUPPORT_H

#include "cdynamicexpression.h"

namespace omnetpp {

namespace nedsupport {

class NedExpressionContext : public cExpression::Context
{
  public:
    enum ExpressionRole { SUBMODULE_CONDITION, SUBMODULE_ARRAY_CONDITION, OTHER };
    explicit NedExpressionContext(cComponent *component, ExpressionRole role, const char *computedTypename) :
            cExpression::Context(component), role(role), computedTypename(computedTypename) {}
    ExpressionRole role;
    const char *computedTypename;
};

class ModuleIndex : public cDynamicExpression::Functor
{
  public:
    ModuleIndex();
    ModuleIndex *dup() const override {return new ModuleIndex();}
    virtual const char *getFullName() const override {return "index";}
    virtual const char *getArgTypes() const override {return "";}
    virtual char getReturnType() const override {return 'L';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};

class Exists : public cDynamicExpression::Functor
{
    std::string ident;
    bool ofParent;
  public:
    Exists(const char *ident, bool ofParent);
    Exists *dup() const override {return new Exists(ident.c_str(), ofParent);}
    virtual const char *getFullName() const override {return "exists";}
    virtual const char *getArgTypes() const override {return "";}
    virtual char getReturnType() const override {return 'B';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};

class Typename : public cDynamicExpression::Functor
{
  public:
    Typename();
    Typename *dup() const override {return new Typename();}
    virtual const char *getFullName() const override {return "typename";}
    virtual const char *getArgTypes() const override {return "";}
    virtual char getReturnType() const override {return 'S';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};

/**
 * @brief Variations: parameter, parentParameter
 */
class ParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->par(paramName)
    bool explicitKeyword; // when ofParent==false: whether the NED file contained the explicit "this" qualifier
    std::string paramName;
  public:
    ParameterRef(const char *paramName, bool ofParent, bool explicitKeyword);
    ParameterRef *dup() const override {return new ParameterRef(paramName.c_str(), ofParent, explicitKeyword);}
    virtual const char *getFullName() const override {return paramName.c_str();}
    virtual const char *getArgTypes() const override {return "";}
    virtual char getReturnType() const override {return '*';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};

/**
 * @brief siblingModuleParameter, indexedSiblingModuleParameter
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
    SiblingModuleParameterRef *dup() const override {return new SiblingModuleParameterRef(moduleName.c_str(), paramName.c_str(), ofParent, withModuleIndex);}
    virtual const char *getFullName() const override {return paramName.c_str();}
    virtual const char *getArgTypes() const override {return withModuleIndex ? "L" : "";}
    virtual char getReturnType() const override {return '*';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};

/**
 * @brief i,j in NED "for" loops
 */
class LoopVar : public cDynamicExpression::Functor
{
  private:
    // the loopvar stack (vars of nested loops are pushed on the stack by cNedNetworkBuilder)
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
    LoopVar *dup() const override {return new LoopVar(varName.c_str());}
    virtual const char *getFullName() const override {return varName.c_str();}
    virtual const char *getArgTypes() const override {return "";}
    virtual char getReturnType() const override {return 'L';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};


/**
 * @brief sizeof operator. See also: SiblingModuleSizeof
 */
class Sizeof : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->gateSize(ident)
    bool explicitKeyword; // when ofParent==false: whether the NED file contained the explicit "this" qualifier
    std::string ident;
  public:
    Sizeof(const char *ident, bool ofParent, bool explicitKeyword);
    Sizeof *dup() const override {return new Sizeof(ident.c_str(), ofParent, explicitKeyword);}
    virtual const char *getFullName() const override {return ident.c_str();}
    virtual const char *getArgTypes() const override {return "";}
    virtual char getReturnType() const override {return 'L';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) override;
    virtual std::string str(std::string args[], int numargs) override;
};

/*XXX TODO
static cNedValue getSizeofIdent(Context *context, cNedValue args[], int numargs);
static cNedValue getSizeofGate(Context *context, cNedValue args[], int numargs);
static cNedValue getSizeofParentModuleGate(Context *context, cNedValue args[], int numargs);
static cNedValue getSizeofSiblingModuleGate(Context *context, cNedValue args[], int numargs);
static cNedValue getSizeofIndexedSiblingModuleGate(Context *context, cNedValue args[], int numargs);

class Sizeof : public Functor
{
  protected:
    bool ofParent;
    bool bstd::string paramName;
    std::string moduleName;
  public:
    virtual const char *getArgTypes() const {return "";}
    virtual char getReturnType() const {return 'L';}
    virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs);
    virtual std::string str(std::string args[], int numargs) {return "index";}
};
*/

};

}  // namespace omnetpp


#endif


