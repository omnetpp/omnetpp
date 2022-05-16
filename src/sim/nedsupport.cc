//==========================================================================
//   NEDSUPPORT.CC  - part of
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

#include <memory>
#include "common/stringutil.h"
#include "common/exprutil.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#include "omnetpp/cvalueholder.h"
#include "nedsupport.h"
#include "ctemporaryowner.h"

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

namespace omnetpp {
namespace nedsupport {

inline omnetpp::any_ptr convert(omnetpp::common::any_ptr p) { return omnetpp::any_ptr(p.raw(), p.pointerType()); }
inline omnetpp::common::any_ptr convert(omnetpp::any_ptr p) { return omnetpp::common::any_ptr(p.raw(), p.pointerType()); }

cValue makeNedValue(const ExprValue& value)
{
    switch (value.getType()) {
    case ExprValue::UNDEF: return cValue();
    case ExprValue::BOOL: return cValue(value.boolValue());
    case ExprValue::INT: return cValue(value.intValue(), value.getUnit());
    case ExprValue::DOUBLE: return cValue(value.doubleValue(), value.getUnit());
    case ExprValue::STRING: return cValue(value.stringValue());
    case ExprValue::POINTER: return cValue(convert(value.pointerValue()));
    }
    Assert(false);
}

ExprValue makeExprValue(const cValue& value)
{
    switch (value.getType()) {
    case cValue::UNDEF: return ExprValue();
    case cValue::BOOL: return ExprValue(value.boolValue());
    case cValue::INT: return ExprValue(value.intValueRaw(), value.getUnit());
    case cValue::DOUBLE: return ExprValue(value.doubleValueRaw(), value.getUnit());
    case cValue::STRING: return ExprValue(value.stringValue());
    case cValue::POINTER: return ExprValue(convert(value.pointerValue()));
    }
    Assert(false);
}

ExprValue makeExprValue(const cPar& par)
{
    switch (par.getType()) {
    case cPar::BOOL: return ExprValue(par.boolValue());
    case cPar::INT: return ExprValue(par.intValue(), par.getUnit());
    case cPar::DOUBLE: return ExprValue(par.doubleValue(), par.getUnit());
    case cPar::STRING: return ExprValue(par.stdstringValue());
    case cPar::OBJECT: return ExprValue(par.objectValue());
    case cPar::XML: return ExprValue(par.xmlValue());
    }
    Assert(false);
}

cValue *makeNedValues(cValue *&buffer, const ExprValue argv[], int argc)
{
    if (!buffer)
        buffer = new cValue[argc];
    for (int i = 0; i < argc; i++)
        buffer[i] = makeNedValue(argv[i]);
    return buffer;
}

//----

std::string NedFunctionNode::getName() const
{
    return nedFunction->getName();
}

ExprValue NedFunctionNode::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    int argc = children.size();
    std::unique_ptr<cValue[]> argv(new cValue[argc]);
    int i = 0;
    for (ExprNode *child : children)
        argv[i++] = makeNedValue(child->tryEvaluate(context_));
    return makeExprValue(nedFunction->invoke(context, argv.get(), argc));
}

void NedFunctionNode::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

//----

static cComponent *getOptionalContextComponent(Context *context_, IdentQualifier qualifier)
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    cComponent *component = context->component;
    if (qualifier == NONE || qualifier == THIS)
        return component;
    else if (qualifier == PARENT) {
        if (component == nullptr)
            throw cRuntimeError("'parent' may only occur in module or channel context");
        cModule *parent = component->getParentModule();
        if (parent == nullptr)
            throw cRuntimeError("%s has no parent module", component->getNedTypeAndFullPath().c_str());
        return parent;
    }
    else
        throw cRuntimeError("Internal error");
}

static cComponent *getContextComponent(Context *context, IdentQualifier qualifier)
{
    cComponent *component = getOptionalContextComponent(context, qualifier);
    if (component == nullptr)
        throw cRuntimeError("May only be used in the context of a module or channel");
    return component;
}

static cModule *getContextModule(Context *context, IdentQualifier qualifier)
{
    cComponent *component = getOptionalContextComponent(context, qualifier);
    cModule *module = dynamic_cast<cModule*>(component);
    if (module == nullptr)
        throw cRuntimeError("May only be used in the context of a module");
    return module;
}

static cModule *getContextSubmodule(Context *context, IdentQualifier qualifier, const char *name, int index=-1)
{
    cModule *module = getContextModule(context, qualifier);
    cModule *submodule = module->getSubmodule(name, index);
    if (!submodule) {
        std::string fullName = opp_indexedname(name, index);
        std::vector<std::string> notes;
        if (!module->parametersFinalized())
            notes.push_back("submodules not yet created at this point");
        if (index != -1 && module->hasSubmodule(name))
            notes.push_back(opp_stringf("possibly superfluous index, as '%s' is a scalar submodule", name));
        if (index == -1 && module->hasSubmoduleVector(name))
            notes.push_back(opp_stringf("possibly missing index, as '%s' is a submodule vector", name));
        if (qualifier == NONE && module->getParentModule() && (module->getParentModule()->hasSubmodule(name) || module->getParentModule()->hasSubmoduleVector(name)) )
            notes.push_back(opp_stringf("did you mean 'parent.%s'?", fullName.c_str()));
        std::string note = notes.empty() ? "" : " (" + opp_join(notes,  "; ") + ")";
        throw cRuntimeError("Submodule '%s' not found in module %s%s", fullName.c_str(), module->getNedTypeAndFullPath().c_str(), note.c_str());
    }
    return submodule;
}

//----

ExprValue Index::evaluate(Context *context) const
{
    cModule *module = getContextModule(context, qualifier);
    return (intval_t)module->getIndex();
}

void Index::print(std::ostream& out, int spaciousness) const
{
    out << qualifierToPrefix(qualifier) << "index";
}

//----

ExprValue Exists::evaluate(Context *context) const
{
    const char *n1 = name1.c_str();
    const char *n2 = name2.c_str();
    int index = children.empty() ? -1 : getIndex(context);

    switch (syntax) {
    case QUALIFIER: {
        return ExprValue(true);
    }
    case OPTQUALIFIER_NAME1: {
        cModule *module = getContextModule(context, qualifier);
        return module->hasSubmodule(n1) || module->hasSubmoduleVector(n1)
                || module->hasGate(n1) || module->hasGateVector(n1)
                || module->hasPar(n1);
    }
    case OPTQUALIFIER_INDEXEDNAME1: {
        cModule *module = getContextModule(context, qualifier);
        return module->hasSubmodule(n1, index) || module->hasGate(n1, index);
    }
    case OPTQUALIFIER_NAME1_DOT_NAME2:
    case OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2: {
        cModule *submodule = getContextSubmodule(context, qualifier, n1, index);
        // Alternative that doesn't throw if submodule (e.g. "foo" in "foo.bar") doesn't exist:
        // cModule *module = getContextModule(context, qualifier);
        // cModule *submodule = module->getSubmodule(name, index);
        // if (submodule == nullptr)
        //     return ExprValue(false);
        return submodule->hasGate(n2) || submodule->hasGateVector(n2) || submodule->hasPar(n2);
    }
    default: ASSERT(false); return ExprValue(false);
    }
}

int Exists::getIndex(Context *context) const
{
    ExprValue indexValue = children[0]->tryEvaluate(context);
    if (indexValue.getUnit() != nullptr)
        throw cRuntimeError("Module index must be dimensionless, got '%s'", indexValue.str().c_str());
    return indexValue.intValue();
}

void Exists::print(std::ostream& out, int spaciousness) const
{
    out << "exists(";
    out << (qualifier == THIS ? "this" : qualifier == PARENT ? "parent" : "");
    if (!name1.empty()) {
        if (qualifier != NONE)
            out << ".";
        out << name1;
        if (!children.empty()) {
            out << "[";
            printChild(out, children[0], spaciousness);
            out << "]";
        }
        if (!name2.empty())
            out << "." << name2;
    }
    out << ")";
}

//----

ExprValue Typename::evaluate(Context *context) const
{
    NedExpressionContext *nedContext = dynamic_cast<NedExpressionContext*>(context->simContext);
    if (!nedContext) {
        cComponent *component = getContextComponent(context, qualifier);
        return component->getNedTypeName();
    }
    switch(nedContext->role) {
        case NedExpressionContext::SUBMODULE_CONDITION: return nedContext->computedTypename;
        case NedExpressionContext::SUBMODULE_ARRAY_CONDITION: throw cRuntimeError("'typename' may not occur in the condition of a submodule vector");
        default: {return getContextComponent(context, qualifier)->getNedTypeName();}
    }
}

void Typename::print(std::ostream& out, int spaciousness) const
{
    out << qualifierToPrefix(qualifier) << "typename";
}

//----

ExprValue Parameter::evaluate(Context *context) const
{
    cComponent *component = getOptionalContextComponent(context, qualifier);
    if (!component)
        throw cRuntimeError("Parameter references may only occur in component context");

    cExpression::Context *simContext = dynamic_cast<cExpression::Context*>(context->simContext);
    if (qualifier == NONE && simContext->paramName != nullptr && paramName == simContext->paramName)
        throw cRuntimeError("Parameter refers to itself in its assignment (did you mean 'parent.%s'?)", paramName.c_str());

    int parId = component->findPar(paramName.c_str());
    if (parId == -1) {
        if (qualifier == NONE && component->getParentModule() && component->getParentModule()->hasPar(paramName.c_str()))
            throw cRuntimeError("%s has no parameter named '%s' (did you mean 'parent.%s'?)", component->getNedTypeAndFullPath().c_str(), paramName.c_str(), paramName.c_str());
        else
            throw cRuntimeError("%s has no parameter named '%s'", component->getNedTypeAndFullPath().c_str(), paramName.c_str());
    }
    return makeExprValue(component->par(parId));
}

void Parameter::print(std::ostream& out, int spaciousness) const
{
    out << qualifierToPrefix(qualifier) << paramName;
}

//----

ExprValue SubmoduleParameter::evaluate(Context *context) const
{
    cModule *submodule = getContextSubmodule(context, qualifier, submoduleName.c_str());
    return makeExprValue(submodule->par(paramName.c_str()));
}

void SubmoduleParameter::print(std::ostream& out, int spaciousness) const
{
    out << qualifierToPrefix(qualifier) << submoduleName << "." << paramName;
}

//----

ExprValue IndexedSubmoduleParameter::evaluate(Context *context) const
{
    ExprValue indexValue = child->tryEvaluate(context);
    if (indexValue.getUnit() != nullptr)
        throw cRuntimeError("Module index must be dimensionless, got '%s'", indexValue.str().c_str());
    int index = indexValue.intValue();
    cModule *submodule = getContextSubmodule(context, qualifier, submoduleName.c_str(), index);
    return makeExprValue(submodule->par(paramName.c_str()));
}

void IndexedSubmoduleParameter::print(std::ostream& out, int spaciousness) const
{
    out << qualifierToPrefix(qualifier) << submoduleName << "[";
    printChild(out, child, spaciousness);
    out<< "]." << paramName;
}

//----

ExprValue Sizeof::evaluate(Context *context) const
{
    cModule *module = getContextModule(context, qualifier);
    if (optName1.empty()) { // both names empty, i.e. sizeof(this) or sizeof(parent)
        ASSERT(optName2.empty());
        if (!module->isVector())
            throw cRuntimeError("Module %s is not part of a submodule vector", module->getNedTypeAndFullPath().c_str());
        return (intval_t)module->getVectorSize();
    }
    else if (optName2.empty())
        return gateOrSubmoduleSize(module, optName1.c_str());
    else {
        cModule *submodule = getContextSubmodule(context, qualifier, optName1.c_str());
        return gateOrSubmoduleSize(submodule, optName2.c_str());
    }
}

intval_t Sizeof::gateOrSubmoduleSize(cModule *module, const char *name) const
{
    // name is either a submodule vector or a gate vector of the module
    if (!module->parametersFinalized())
        throw cRuntimeError("Call too early, gate vectors have not yet been finalized and submodules not yet created at this point");
    if (module->hasGateVector(name))
        return (intval_t)module->gateSize(name);
    else if (module->hasSubmoduleVector(name))
        return (intval_t)module->getSubmoduleVectorSize(name);
    else
        throw cRuntimeError("Module %s has no gate vector or submodule vector named '%s'", module->getNedTypeAndFullPath().c_str(), name);
}

void Sizeof::print(std::ostream& out, int spaciousness) const
{
    out << "sizeof(" << qualifierToPrefix(qualifier) << opp_join(".", optName1.c_str(), optName2.c_str()) << ")";
}

//---

ExprValue SizeofIndexedSubmoduleGate::evaluate(Context *context) const
{
    ExprValue indexValue = child->tryEvaluate(context);
    if (indexValue.getUnit() != nullptr)
        throw cRuntimeError("Module index must be dimensionless, got '%s'", indexValue.str().c_str());
    int index = indexValue.intValue();
    cModule *submodule = getContextSubmodule(context, qualifier, submoduleName.c_str(), index);
    if (!submodule->parametersFinalized())
        throw cRuntimeError("Call too early, gate vectors have not yet been finalized at this point");
    return (intval_t)submodule->gateSize(gateName.c_str());
}

void SizeofIndexedSubmoduleGate::print(std::ostream& out, int spaciousness) const
{
    out << "sizeof(" << qualifierToPrefix(qualifier) << submoduleName << "[";
    printChild(out, child, spaciousness);
    out << "]." << gateName << ")";
}

//---

const char *LoopVar::varNames[32];
long LoopVar::vars[32];
int LoopVar::varCount = 0;

long& LoopVar::pushVar(const char *varName)
{
    ASSERT(varCount < 32);
    varNames[varCount] = varName;
    return vars[varCount++];
}

void LoopVar::popVar()
{
    ASSERT(varCount > 0);
    varCount--;
}

void LoopVar::clear()
{
    varCount = 0;
}

bool LoopVar::contains(const char *varName)
{
    for (int i = 0; i < varCount; i++)
        if (strcmp(varNames[i], varName) == 0)
            return true;
    return false;
}

ExprValue LoopVar::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    const char *var = varName.c_str();
    for (int i = 0; i < varCount; i++)
        if (strcmp(var, varNames[i]) == 0)
            return (intval_t)vars[i];

    throw cRuntimeError(context->component, "Loop variable '%s' not found", varName.c_str());
}

void LoopVar::print(std::ostream& out, int spaciousness) const
{
    out << varName;
}

//---

ExprValue DynExpr::evaluate(Context *context) const
{
    cOwnedDynamicExpression *exprObj = new cOwnedDynamicExpression();
    exprObj->parse(exprText.c_str());
    return ExprValue(exprObj);
}

void DynExpr::print(std::ostream& out, int spaciousness) const
{
    out << "expr(" + exprText + ")";
}

//---

ExprValue NedObjectNode::evaluate(Context *context_) const
{
    ASSERT(children.size() == fieldNames.size());
    if (typeName.empty()) {
        static int counter = 0;
        cValueMap *object = new cValueMap(opp_stringf("map%d", ++counter).c_str());
        cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // dispose temp objects created during evaluation
        for (int i = 0; i < fieldNames.size(); i++)
            object->set(fieldNames[i].c_str(), makeNedValue(children[i]->tryEvaluate(context_))); // note: set() includes taking added cOwnedObject
        return object;
    }
    else {
        cObject *object = createOne(typeName.c_str());
        if (object->isOwnedObject()) {
            static int counter = 0;
            static_cast<cOwnedObject*>(object)->setName(opp_stringf("obj%d", ++counter).c_str());
        }
        cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // dispose temp objects created during evaluation
        cClassDescriptor *desc = object->getDescriptor();
        for (int i = 0; i < fieldNames.size(); i++)
            setField(desc, toAnyPtr(object), fieldNames[i].c_str(), makeNedValue(children[i]->tryEvaluate(context_))); // SHOULD include taking added cOwnedObjects
        return object;
    }
}

void NedObjectNode::setField(cClassDescriptor *desc, any_ptr object, const char *fieldName, const cValue& value) const
{
    int fieldIndex = desc->findField(fieldName);
    if (fieldIndex == -1)
        throw cRuntimeError("Class '%s' has no field named '%s'", desc->getFullName(), fieldName);

    bool isArrayField = desc->getFieldIsArray(fieldIndex);

    if (!isArrayField) {
        // scalar field
        setFieldElement(desc, object, fieldName, fieldIndex, -1, value);
    }
    else {
        // array field
        cValueArray *array = check_and_cast<cValueArray *>(value.objectValue());
        int arraySize = array->size();
        desc->setFieldArraySize(object, fieldIndex, arraySize);
        for (int i = 0; i < arraySize; i++)
            setFieldElement(desc, object, fieldName, fieldIndex, i, array->remove(i));
    }
}

void NedObjectNode::setFieldElement(cClassDescriptor *desc, any_ptr object, const char *fieldName, int fieldIndex, int arrayIndex, const cValue& value) const
{
    bool isCompoundField = desc->getFieldIsCompound(fieldIndex);
    bool isPointerField = desc->getFieldIsPointer(fieldIndex);
    bool isCObjectField = desc->getFieldIsCObject(fieldIndex);

    // scalar field
    if (!isCompoundField) { // atomic
        if (value.getType() == cValue::POINTER)
            throw cRuntimeError("Cannot put object value into non-object field '%s%s' inside a '%s'",
                    fieldName, (fieldIndex==-1 ? "" : "[]"), desc->getFullName());
        desc->setFieldValueAsString(object, fieldIndex, arrayIndex, value.getType()==cValue::STRING ? value.stringValue() : value.str().c_str());
    }
    else if (isPointerField) {
        if (!isCObjectField)
            throw cRuntimeError("Non-cObject pointer fields are currently not supported (field '%s' in class '%s')", fieldName, desc->getFullName());
        if (value.getType() != cValue::POINTER)
            throw cRuntimeError("Cannot put %s value into object field '%s%s' inside a '%s'",
                    cValue::getTypeName(value.getType()), fieldName, (fieldIndex==-1 ? "" : "[]"), desc->getFullName());
        desc->setFieldStructValuePointer(object, fieldIndex, arrayIndex, toAnyPtr(value.objectValue()));
    }
    else {
        // non-pointer: set individual fields
        any_ptr fieldPtr = desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex);

        cClassDescriptor *fieldDesc = isCObjectField ?
                fromAnyPtr<cObject>(fieldPtr)->getDescriptor() :
                cClassDescriptor::getDescriptorFor(desc->getFieldStructName(fieldIndex));

        cValueMap *valueMap = value.getType()==cValue::POINTER ? dynamic_cast<cValueMap *>(value.objectValue()) : nullptr;
        if (valueMap == nullptr)
            throw cRuntimeError("Map value expected for object field '%s%s' inside a '%s'",
                    fieldName, (fieldIndex==-1 ? "" : "[]"), desc->getFullName());

        fillObject(fieldDesc, fieldPtr, valueMap);
    }
}

void NedObjectNode::fillObject(cClassDescriptor *desc, any_ptr object, const cValueMap *valueMap) const
{
    for (auto it : valueMap->getFields()) {
        const char *fieldName = it.first.c_str();
        const cValue& value = it.second;
        setField(desc, object, fieldName, value);
    }
}

//---

ExprValue NedArrayNode::evaluate(Context *context_) const
{
    static int counter = 0;
    cValueArray *array = new cValueArray(opp_stringf("array%d", ++counter).c_str());
    cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // dispose temp objects created during evaluation
    for (ExprNode *child : children)
        array->add(makeNedValue(child->tryEvaluate(context_))); // note: add() includes taking added cOwnedObject
    return ExprValue(array);
}

//---

inline AstNode *getSingleChild(AstNode *astNode)
{
    if (astNode->children.size() != 1)
        throw cRuntimeError("%s() expects one argument", astNode->name.c_str());
    return astNode->children.at(0);
}

IdentSyntax NedOperatorTranslator::matchSyntax(AstNode *astNode, IdentQualifier& qualifier, std::string& name1, AstNode *& indexNode, std::string& name2)
{
    qualifier = NONE;
    name1 = name2 = "";
    indexNode = nullptr;

    // linearize the tree: ident1.ident2.ident3
    AstNode *ident1 = astNode, *ident2 = nullptr, *ident3 = nullptr;
    ident1 = astNode;
    if (isMemberOrIndexedMember(ident1)) {
        ident2 = ident1;
        ident1 = ident1->children.at(0);
        if (isMemberOrIndexedMember(ident1)) {
            ident3 = ident2;
            ident2 = ident1;
            ident1 = ident1->children.at(0);
            if (isMemberOrIndexedMember(ident1))
                return UNKNOWN;  // chain too long -- not supported
        }
    }
    else if (hasIndex(ident1)) {
        return UNKNOWN;  // ident1[] -- not supported
    }

    // resolve if ident1 is "this." or "parent."
    if (isIdent(ident1, "this") || isIdent(ident1, "parent")) {
        qualifier = (ident1->name == "this") ? THIS : PARENT;
        ident1 = ident2;
        ident2 = ident3;
        ident3 = nullptr;
    }

    if (ident3 != nullptr)
        return UNKNOWN;  // only two levels are supported

    if (ident2) {
        if (!isMember(ident2))
            return UNKNOWN;  // must be member, may NOT be indexed
        if (ident2->name == "this" || ident2->name == "parent")
            return UNKNOWN; // may not be "this" or "parent"
        name2 = ident2->name;
        if (hasIndex(ident2))
            indexNode = getIndexChild(ident2);
    }

    if (ident1) {
        if (!isIdentOrIndexedIdent(ident1) && !isMemberOrIndexedMember(ident1))
            return UNKNOWN; // must be a member or ident, potentially indexed
        if (ident1->name == "this" || ident1->name == "parent")
            return UNKNOWN; // may not be "this" or "parent"
        name1 = ident1->name;
        if (hasIndex(ident1)) {
            if (ident2 && hasIndex(ident2))
                return UNKNOWN; // ident1[...].ident2[...] is not a supported syntax
            indexNode = getIndexChild(ident1);
        }
    }

    // return code
    if (!ident1)
        return QUALIFIER;
    else if (!ident2)
        return !indexNode ? OPTQUALIFIER_NAME1 : OPTQUALIFIER_INDEXEDNAME1;
    else
        return !indexNode ? OPTQUALIFIER_NAME1_DOT_NAME2 : OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2;
}

ExprNode *NedOperatorTranslator::translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren)
{
    if (isFunction(astNode, "sizeof"))
        return translateSizeof(astNode, translatorForChildren);
    else if (isFunction(astNode, "exists"))
        return translateExists(astNode, translatorForChildren);
    else if (isIdent(astNode, "index") || isMember(astNode, "index"))
        return translateIndex(astNode, translatorForChildren);
    else if (isIdent(astNode, "typename") || isMember(astNode, "typename"))
        return translateTypename(astNode, translatorForChildren);
    else if (isIdentOrIndexedIdent(astNode) || isMemberOrIndexedMember(astNode))
        return translateParameter(astNode, translatorForChildren);
    else if (isFunction(astNode, "expr"))
        return translateExpr(astNode, translatorForChildren);
    else
        return nullptr; // not recognized here
}

ExprNode *NedOperatorTranslator::translateSizeof(AstNode *sizeofNode, AstTranslator *translatorForChildren)
{
    IdentQualifier qualifier;
    std::string name1, name2;
    AstNode *indexNode;

    IdentSyntax syntax = matchSyntax(getSingleChild(sizeofNode), qualifier, name1, indexNode, name2);

    switch (syntax) {
    case UNKNOWN:
        throw cRuntimeError("Illegal argument for sizeof() in '%s'", sizeofNode->unparse().c_str());
    case QUALIFIER:
        return new Sizeof(qualifier);
    case OPTQUALIFIER_NAME1:
        return new Sizeof(qualifier, name1.c_str());
    case OPTQUALIFIER_INDEXEDNAME1:
        throw cRuntimeError("Illegal argument for sizeof() in '%s'", sizeofNode->unparse().c_str());
    case OPTQUALIFIER_NAME1_DOT_NAME2:
        return new Sizeof(qualifier, name1.c_str(), name2.c_str());
    case OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2:
        auto result = new SizeofIndexedSubmoduleGate(qualifier, name1.c_str(), name2.c_str());
        result->setChild(translateChild(indexNode, translatorForChildren));
        return result;
    }
    throw cRuntimeError("internal error");
}

ExprNode *NedOperatorTranslator::translateExists(AstNode *existsNode, AstTranslator *translatorForChildren)
{
    IdentQualifier qualifier;
    std::string name1, name2;
    AstNode *indexNode;

    IdentSyntax syntax = matchSyntax(getSingleChild(existsNode), qualifier, name1, indexNode, name2);
    if (syntax == UNKNOWN)
        throw cRuntimeError("Illegal argument for exists() in '%s'", existsNode->unparse().c_str());

    Exists *result = new Exists(syntax, qualifier, name1, name2);
    if (indexNode)
        result->appendChild(translateChild(indexNode, translatorForChildren));
    return result;
}

ExprNode *NedOperatorTranslator::translateIndex(AstNode *indexNode, AstTranslator *translatorForChildren)
{
    IdentQualifier qualifier;
    std::string name1, name2;
    AstNode *nameIndexNode;

    IdentSyntax syntax = matchSyntax(indexNode, qualifier, name1, nameIndexNode, name2);

    switch (syntax) {
    case OPTQUALIFIER_NAME1:  // NAME1 is "index"
       return new Index(qualifier);
    case QUALIFIER:
    case UNKNOWN:
    case OPTQUALIFIER_INDEXEDNAME1:
    case OPTQUALIFIER_NAME1_DOT_NAME2:
    case OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2:
       throw cRuntimeError("Invalid use of 'index' in '%s'", indexNode->unparse().c_str());
    }
    throw cRuntimeError("internal error");
}

ExprNode *NedOperatorTranslator::translateTypename(AstNode *typenameNode, AstTranslator *translatorForChildren)
{
    IdentQualifier qualifier;
    std::string name1, name2;
    AstNode *indexNode;

    IdentSyntax syntax = matchSyntax(typenameNode, qualifier, name1, indexNode, name2);

    switch (syntax) {
    case OPTQUALIFIER_NAME1:
        return new Typename(qualifier); // NAME1 is "typename"
    case UNKNOWN:
    case QUALIFIER:
    case OPTQUALIFIER_INDEXEDNAME1:
    case OPTQUALIFIER_NAME1_DOT_NAME2:
    case OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2:
        throw cRuntimeError("Invalid use of 'typename' in '%s'", typenameNode->unparse().c_str());
    }
    throw cRuntimeError("internal error");
}

ExprNode *NedOperatorTranslator::translateParameter(AstNode *paramNode, AstTranslator *translatorForChildren)
{
    IdentQualifier qualifier;
    std::string name1, name2;
    AstNode *indexNode;

    IdentSyntax syntax = matchSyntax(paramNode, qualifier, name1, indexNode, name2);

    switch (syntax) {
    case UNKNOWN:
    case QUALIFIER:
        throw cRuntimeError("Invalid parameter reference '%s'", paramNode->unparse().c_str());
    case OPTQUALIFIER_NAME1:
        if (qualifier == NONE && LoopVar::contains(name1.c_str()))
            return new LoopVar(name1.c_str());
        else
            return new Parameter(qualifier, name1.c_str());
    case OPTQUALIFIER_INDEXEDNAME1:
        throw cRuntimeError("Invalid parameter reference '%s'", paramNode->unparse().c_str());
    case OPTQUALIFIER_NAME1_DOT_NAME2:
        return new SubmoduleParameter(qualifier, name1.c_str(), name2.c_str());
    case OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2: {
        auto result = new IndexedSubmoduleParameter(qualifier, name1.c_str(), name2.c_str());
        result->setChild(translateChild(indexNode, translatorForChildren));
        return result;
    }
    }
    throw cRuntimeError("internal error");
}

ExprNode *NedOperatorTranslator::translateExpr(AstNode *exprFunctionNode, AstTranslator *translatorForChildren)
{
    if (exprFunctionNode->children.size() != 1)
        throw cRuntimeError("'expr' expects exactly one argument, got %d", (int)exprFunctionNode->children.size());

    AstNode *exprAst = exprFunctionNode->children[0];
    std::string exprText = exprAst->unparse();  // TODO eliminate unparse+reparse step
    return new DynExpr(exprText.c_str());
}

//---

ExprNode *NedFunctionTranslator::createFunctionNode(const char *functionName, int argCount)
{
    if (cNedFunction *nedFunction = cNedFunction::find(functionName, argCount))
        return createNedFunctionNode(nedFunction, argCount);
    if (cNedMathFunction *mathFunction = cNedMathFunction::find(functionName, argCount))
        return createMathFunctionNode(mathFunction, argCount);
    return nullptr;
}

ExprNode *NedFunctionTranslator::createMethodNode(const char *functionName, int argCount)
{
    return createFunctionNode(functionName, argCount+1);
}

ExprNode *NedFunctionTranslator::createObjectNode(const char *typeName, const std::vector<std::string>& fieldNames)
{
    return new NedObjectNode(typeName, fieldNames);
}

ExprNode *NedFunctionTranslator::createArrayNode(int argCount)
{
    return new NedArrayNode();
}

ExprNode *NedFunctionTranslator::createNedFunctionNode(cNedFunction *nedFunction, int argCount)
{
//    if (!nedFunction->acceptsArgCount(argCount)) {
//        int minArgs = nedFunction->getMinArgs();
//        int maxArgs = nedFunction->getMaxArgs();
//        bool varArgs = nedFunction->hasVarArgs();
//        if (varArgs)
//            throw opp_runtime_error("NED function '%s' expects at least %d", nedFunction->getName(), minArgs);
//        else if (minArgs == maxArgs)
//            throw opp_runtime_error("NED function '%s' expects %d arguments", nedFunction->getName(), minArgs);
//        else
//            throw opp_runtime_error("NED function '%s' expects %d to %d arguments", nedFunction->getName(), minArgs, maxArgs);
//    }
    ASSERT(nedFunction->acceptsArgCount(argCount));
    return new NedFunctionNode(nedFunction);
}

ExprNode *NedFunctionTranslator::createMathFunctionNode(cNedMathFunction *mathFunction, int argCount)
{
    ASSERT(mathFunction->getNumArgs() == argCount);
    const char *name = mathFunction->getName();
    switch (argCount) {
        case 0: return new MathFunc0Node(name, mathFunction->getMathFuncNoArg());
        case 1: return new MathFunc1Node(name, mathFunction->getMathFunc1Arg());
        case 2: return new MathFunc2Node(name, mathFunction->getMathFunc2Args());
        case 3: return new MathFunc3Node(name, mathFunction->getMathFunc3Args());
        case 4: return new MathFunc4Node(name, mathFunction->getMathFunc4Args());
        default: throw opp_runtime_error("'%s()': NED math functions only support up to 4 arguments", name);
    }
}

}  // namespace omnetpp
}  // namespace nedsupport
