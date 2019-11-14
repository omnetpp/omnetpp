//==========================================================================
//   NEDSUPPORT.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
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
#include "nedsupport.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedsupport {

cValue makeNedValue(const ExprValue& value)
{
    switch (value.getType()) {
    case ExprValue::UNDEF: return cValue();
    case ExprValue::BOOL: return cValue(value.boolValue());
    case ExprValue::INT: return cValue(value.intValue(), value.getUnit());
    case ExprValue::DOUBLE: return cValue(value.doubleValue(), value.getUnit());
    case ExprValue::STRING: return cValue(value.stringValue());
    case ExprValue::OBJECT: return cValue(value.objectValue());
    }
    Assert(false);
}

ExprValue makeExprValue(const cValue& value)
{
    switch (value.getType()) {
    case cValue::UNDEF: return ExprValue();
    case cValue::BOOL: return ExprValue(value.boolValue());
    case cValue::INT: return ExprValue(value.intValue(), value.getUnit());
    case cValue::DOUBLE: return ExprValue(value.doubleValue(), value.getUnit());
    case cValue::STRING: return ExprValue(value.stringValue());
    case cValue::OBJECT: return ExprValue(value.objectValue());
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

std::string NedFunctionNode::makeErrorMessage(std::exception& e) const
{
    return getName() + "(): " + e.what();
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

ExprValue ModuleIndex::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    cModule *module = dynamic_cast<cModule *>(context->component);
    if (!module)
        throw cRuntimeError("'index' may only occur in submodule context");
    return (intval_t)module->getIndex();
}

void ModuleIndex::print(std::ostream& out, int spaciousness) const
{
    out << "index";
}

//----

ExprValue Exists::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    cModule *module = inSubcomponentScope ? context->component->getParentModule() : dynamic_cast<cModule *>(context->component);
    if (!module)
        throw cRuntimeError("'exists()' may only occur in module or submodule context");
    bool exists = module->getSubmodule(name.c_str()) || module->getSubmodule(name.c_str(), 0);
    return exists;
}

void Exists::print(std::ostream& out, int spaciousness) const
{
    out << "exists(" << name << ")";
}

//----

ExprValue Typename::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError("'typename' may only occur in submodule context");
    NedExpressionContext *nedContext = dynamic_cast<NedExpressionContext*>(context);
    if (!nedContext)
        return context->component->getNedTypeName();
    switch(nedContext->role) {
        case NedExpressionContext::SUBMODULE_CONDITION: return nedContext->computedTypename;
        case NedExpressionContext::SUBMODULE_ARRAY_CONDITION: throw cRuntimeError("'typename' may not occur in the condition of a submodule vector");
        default: return context->component->getNedTypeName();
    }
}

void Typename::print(std::ostream& out, int spaciousness) const
{
    out << "typename";
}

//----

ExprValue ParameterRef::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError("Parameter reference '%s' may only occur in component context", getName().c_str());
    if (ofThis) {
        // note: same code to do for both inSubcomponentScope=true and =false
        cComponent *component = context->component;
        return makeExprValue(component->par(paramName.c_str()));
    }
    else {
        cComponent *component = inSubcomponentScope ? context->component->getParentModule() : context->component;
        if (!component)
            throw cRuntimeError(context->component, E_ENOPARENT);
        // In inner types, a "paramName" should be first tried as the enclosing type's
        // parameter, only then as local parameter.
        if (!inSubcomponentScope && component->getComponentType()->isInnerType())
            if (component->getParentModule() && component->getParentModule()->hasPar(paramName.c_str()))
                return makeExprValue(component->getParentModule()->par(paramName.c_str()));
        return makeExprValue(component->par(paramName.c_str()));
    }
}

void ParameterRef::print(std::ostream& out, int spaciousness) const
{
    out << (ofThis ? "this." : "") << paramName << ")";
}

//----

ExprValue SubmoduleParameterRef::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError(context->component, "Cannot evaluate parameter '%s' without component context", getName().c_str());
    cModule *compoundModule = dynamic_cast<cModule *>(inSubcomponentScope ? context->component->getParentModule() : context->component);  // this works for channels too
    if (!compoundModule)
        throw cRuntimeError(context->component, E_ENOPARENT);
    cModule *submodule = compoundModule->getSubmodule(submoduleName.c_str());
    if (!submodule)
        throw cRuntimeError("'%s': Submodule '%s' not found", getName().c_str(), submoduleName.c_str());
    return makeExprValue(submodule->par(paramName.c_str()));
}

void SubmoduleParameterRef::print(std::ostream& out, int spaciousness) const
{
    out << submoduleName << "." << paramName;
}

//----

IndexedSubmoduleParameterRef::IndexedSubmoduleParameterRef(const char *moduleName, const char *paramName, bool inSubcomponentScope)
{
    ASSERT(!opp_isempty(moduleName) && !opp_isempty(paramName) && omnetpp::opp_strcmp(moduleName, "this") != 0);
    this->submoduleName = moduleName;
    this->paramName = paramName;
    this->inSubcomponentScope = inSubcomponentScope;
}

ExprValue IndexedSubmoduleParameterRef::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError(context->component, "Cannot evaluate parameter '%s' without component context", getName().c_str());
    cModule *compoundModule = dynamic_cast<cModule *>(inSubcomponentScope ? context->component->getParentModule() : context->component);  // this works for channels too
    if (!compoundModule)
        throw cRuntimeError(context->component, E_ENOPARENT);

    ExprValue indexValue = child->tryEvaluate(context_);
    int index = indexValue.intValue();
    if (indexValue.getUnit() != nullptr)
        throw cRuntimeError("'%s': Module index must be dimensionless, got '%s'", getName().c_str(), indexValue.str().c_str());

    cModule *submodule = compoundModule->getSubmodule(submoduleName.c_str(), index);
    if (!submodule)
        throw cRuntimeError("'%s': Submodule '%s[%d]' not found", getName().c_str(), submoduleName.c_str(), index);
    return makeExprValue(submodule->par(paramName.c_str()));
}

void IndexedSubmoduleParameterRef::print(std::ostream& out, int spaciousness) const
{
    out << submoduleName << "[";
    printChild(out, child, spaciousness);
    out<< "]." << paramName;
}

//----

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

ExprValue SizeofGateOrSubmodule::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError(context->component, "Cannot evaluate '%s' without component context", getName().c_str());

    if (ofThis) {
        ASSERT(inSubcomponentScope);
        cModule *module = dynamic_cast<cModule *>(context->component);
        if (!module)
            throw cRuntimeError("'%s': Size of gate vector is only meaningful in module context", getName().c_str());
        return (intval_t)module->getVectorSize();
    }
    else {
        cModule *module = dynamic_cast<cModule *>(inSubcomponentScope ? context->component->getParentModule() : context->component);
        if (!module)
            throw cRuntimeError(context->component, E_ENOPARENT);
        // name is either a submodule vector or a gate vector of the module
        if (module->hasGate(name.c_str())) {
            return (intval_t)module->gateSize(name.c_str());  // returns 1 if it's not a vector
        }
        else {
            // Find ident among submodules. If there's no such submodule, it may
            // be that such submodule vector never existed, or can be that it's zero
            // size -- we cannot tell, so we have to return 0 (and cannot throw error).
            cModule *submodule0 = module->getSubmodule(name.c_str(), 0);  // returns nullptr if submodule is not a vector
            if (!submodule0 && module->getSubmodule(name.c_str()))
                return (intval_t)1;  // return 1 if submodule exists but not a vector
            return (intval_t)(submodule0 ? submodule0->getVectorSize() : 0L);
        }
    }
}

void SizeofGateOrSubmodule::print(std::ostream& out, int spaciousness) const
{
    out << "sizeof(" << (ofThis ? "this." : "") << name << ")";
}

//---

ExprValue SizeofSubmoduleGate::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError(context->component, "Cannot evaluate '%s' without component context", getName().c_str());
    cModule *module = dynamic_cast<cModule *>(inSubcomponentScope ? context->component->getParentModule() : context->component);
    if (!module)
        throw cRuntimeError(context->component, E_ENOPARENT);

    cModule *submodule = module->getSubmodule(submoduleName.c_str());
    if (!submodule)
        throw cRuntimeError("'%s': Submodule '%s' not found", getName().c_str(), submoduleName.c_str());
     return (intval_t)submodule->gateSize(gateName.c_str());
}

void SizeofSubmoduleGate::print(std::ostream& out, int spaciousness) const
{
    out << "sizeof(" << submoduleName << "." << gateName << ")";
}

//---

ExprValue SizeofIndexedSubmoduleGate::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    if (!context->component)
        throw cRuntimeError(context->component, "Cannot evaluate '%s' without component context", getName().c_str());
    cModule *module = dynamic_cast<cModule *>(inSubcomponentScope ? context->component->getParentModule() : context->component);
    if (!module)
        throw cRuntimeError(context->component, E_ENOPARENT);

    ExprValue indexValue = child->tryEvaluate(context_);
    int index = indexValue.intValue();
    if (indexValue.getUnit() != nullptr)
        throw cRuntimeError("'%s': Module index must be dimensionless, got '%s'", getName().c_str(), indexValue.str().c_str());

    cModule *submodule = module->getSubmodule(submoduleName.c_str(), index);
    if (!submodule)
        throw cRuntimeError("'%s': Submodule '%s[%d]' not found", getName().c_str(), submoduleName.c_str(), index);
     return (intval_t)submodule->gateSize(gateName.c_str());
}

void SizeofIndexedSubmoduleGate::print(std::ostream& out, int spaciousness) const
{
    out << "sizeof(" << submoduleName << "[";
    printChild(out, child, spaciousness);
    out << "]." << gateName << ")";
}

//---

class cDefaultOwnerSwitcher
{
  private:
    cDefaultOwner *oldOwner;
    cDefaultOwner tmpOwner;
  public:
    cDefaultOwnerSwitcher() {
        oldOwner = cOwnedObject::getDefaultOwner();
        cOwnedObject::setDefaultOwner(&tmpOwner);
    }
    cDefaultOwner *getOwner() {return &tmpOwner;}
    ~cDefaultOwnerSwitcher() {
        cOwnedObject::setDefaultOwner(oldOwner);
    }
};

//---

ExprValue ObjectNode::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    ASSERT(children.size() == fieldNames.size());
    if (typeName.empty()) {
        cValueMap *object = new cValueMap();
        cDefaultOwnerSwitcher owner;
        object->setName("object");
        for (int i = 0; i < fieldNames.size(); i++)
            object->set(fieldNames[i].c_str(), makeNedValue(children[i]->tryEvaluate(context_)));
        object->takeAllObjectsFrom(owner.getOwner());
        return object;
    }
    else {
        cObject *object = createOne(typeName.c_str());
        cDefaultOwnerSwitcher owner;
        cClassDescriptor *desc = object->getDescriptor();
        for (int i = 0; i < fieldNames.size(); i++)
            setField(desc, object, fieldNames[i].c_str(), makeNedValue(children[i]->tryEvaluate(context_)));
        object->takeAllObjectsFrom(owner.getOwner());
        return object;
    }
}

void ObjectNode::setField(cClassDescriptor *desc, void *object, const char *fieldName, const cValue& value) const
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

void ObjectNode::setFieldElement(cClassDescriptor *desc, void *object, const char *fieldName, int fieldIndex, int arrayIndex, const cValue& value) const
{
    bool isCompoundField = desc->getFieldIsCompound(fieldIndex);
    bool isPointerField = desc->getFieldIsPointer(fieldIndex);
    bool isCObjectField = desc->getFieldIsCObject(fieldIndex);

    if (!isCompoundField) { // atomic
        if (value.getType() == cValue::OBJECT)
            throw cRuntimeError("Cannot put object value into non-object field '%s%s' inside a '%s'",
                    fieldName, (fieldIndex==-1 ? "" : "[]"), desc->getFullName());
        desc->setFieldValueAsString(object, fieldIndex, arrayIndex, value.getType()==cValue::STRING ? value.stringValue() : value.str().c_str());
    }
    else if (isPointerField) {
        if (!isCObjectField)
            throw cRuntimeError("Non-cObject pointer fields are currently not supported (field '%s' in class '%s')", fieldName, desc->getFullName());
        if (value.getType() != cValue::OBJECT)
            throw cRuntimeError("Cannot put %s value into object field '%s%s' inside a '%s'",
                    cValue::getTypeName(value.getType()), fieldName, (fieldIndex==-1 ? "" : "[]"), desc->getFullName());
        desc->setFieldStructValuePointer(object, fieldIndex, arrayIndex, value.objectValue());
    }
    else {
        // non-pointer: set individual fields
        void *fieldPtr = desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex);

        cClassDescriptor *fieldDesc = isCObjectField ?
                static_cast<cObject *>(fieldPtr)->getDescriptor() :
                cClassDescriptor::getDescriptorFor(desc->getFieldStructName(fieldIndex));

        cValueMap *valueMap = value.getType()==cValue::OBJECT ? dynamic_cast<cValueMap *>(value.objectValue()) : nullptr;
        if (valueMap == nullptr)
            throw cRuntimeError("Map value expected for object field '%s%s' inside a '%s'",
                    cValue::getTypeName(value.getType()), fieldName, (fieldIndex==-1 ? "" : "[]"), desc->getFullName());

        fillObject(fieldDesc, fieldPtr, valueMap);
    }
}

void ObjectNode::fillObject(cClassDescriptor *desc, void *object, const cValueMap *valueMap) const
{
    for (auto it : valueMap->getFields()) {
        const char *fieldName = it.first.c_str();
        const cValue& value = it.second;
        setField(desc, object, fieldName, value);
    }
}

void ObjectNode::print(std::ostream& out, int spaciousness) const
{
    if (!typeName.empty())
        out << getName() << " ";
    out << "{ ";
    std::vector<ExprNode*> children = getChildren();
    ASSERT(children.size() == fieldNames.size());
    for (size_t i = 0; i < children.size(); i++) {
        if (i != 0)
            out << (spaciousness >= LASTPREC-ARITHM_LAST ? ", " : ",");
        out << fieldNames[i] << ": ";
        printChild(out, children[i], spaciousness);
    }
    out << " }";
}

//---

ExprValue ArrayNode::evaluate(Context *context_) const
{
    cExpression::Context *context = dynamic_cast<cExpression::Context*>(context_->simContext);
    ASSERT(context != nullptr);
    cValueArray *array = new cValueArray();
    array->setName("array");
    cDefaultOwnerSwitcher owner;
    for (ExprNode *child : children)
        array->add(makeNedValue(child->tryEvaluate(context_)));
    array->takeAllObjectsFrom(owner.getOwner());
    return ExprValue(array);
}

void ArrayNode::print(std::ostream& out, int spaciousness) const
{
    out << "[ ";
    std::vector<ExprNode*> children = getChildren();
    for (size_t i = 0; i < children.size(); i++) {
        if (i != 0)
            out << (spaciousness >= LASTPREC-ARITHM_LAST ? ", " : ",");
        printChild(out, children[i], spaciousness);
    }
    out << " ]";
}

//---

typedef Expression::AstNode AstNode;

inline bool isIdent(AstNode *astNode) {
    return astNode->type == AstNode::IDENT;
}
inline bool isIdentWithIndex(AstNode *astNode) {
    return astNode->type == AstNode::IDENT_W_INDEX;
}
inline bool isFunction(AstNode *astNode) {
    return astNode->type == AstNode::FUNCTION;
}
inline bool isMember(AstNode *astNode) {
    return astNode->type == AstNode::MEMBER;
}
inline bool isIdent(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::IDENT && astNode->name == name;
}
inline bool isFunction(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::FUNCTION && astNode->name == name;
}
inline bool isMember(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::MEMBER && astNode->name == name;
}
inline AstNode *getSingleChild(AstNode *astNode) {
    if (astNode->children.size() != 1)
        throw cRuntimeError("%s() expects one argument", astNode->name.c_str());
    return astNode->children[0];
}

ExprNode *NedOperatorTranslator::translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren)
{
    (void)inInifile; // eliminate "unused variable" warning
    if (isFunction(astNode, "sizeof"))
        return translateSizeof(astNode, translatorForChildren);
    else if (isFunction(astNode, "exists"))
        return translateExists(astNode, translatorForChildren);
    else if (isIdent(astNode, "typename"))
        return new Typename();
    else if (isIdent(astNode, "index"))
        return translateIndex(astNode, translatorForChildren);
    else if (isIdent(astNode))
        return translateIdent(astNode, translatorForChildren);
    else if (isMember(astNode))
        return translateMember(astNode, translatorForChildren);
    else
        return nullptr; // not recognized here
}

ExprNode *NedOperatorTranslator::translateSizeof(AstNode *sizeofNode, AstTranslator *translatorForChildren)
{
    // Excerpt from the NED Reference:
    // """
    // The sizeof() operator expects one argument, and it is only accepted in compound module definitions.
    //
    // The sizeof(identifier) syntax occurring anywhere in a compound module yields the size of the
    // named submodule or gate vector of the compound module.
    //
    // Inside submodule bodies, the size of a gate vector of the same submodule can be referred to
    // with the this qualifier: sizeof(this.out).
    //
    // To refer to the size of a submodule's gate vector defined earlier in the NED file, use the
    // sizeof(submoduleName.gateVectorName) or sizeof(submoduleName[index].gateVectorName) syntax.
    // """

    AstNode *childNode = getSingleChild(sizeofNode);
    if (isIdent(childNode)) { // sizeof(ident)
        const char *name = childNode->name.c_str();
        return new SizeofGateOrSubmodule(name, inSubcomponentScope, false);
    }
    else if (isMember(childNode)) {
        ASSERT(childNode->children.size() >= 1);
        AstNode *grandChildNode = childNode->children[0];
        if (isIdent(grandChildNode, "this")) { // sizeof(this.ident)
            if (!inSubcomponentScope)
                throw cRuntimeError("sizeof(this.<ident>) may only occur in submodule context");
            const char *name = childNode->name.c_str();
            return new SizeofGateOrSubmodule(name, inSubcomponentScope, true);
        }
        else if (isIdent(grandChildNode)) { // sizeof(ident.ident)
            const char *submoduleName = grandChildNode->name.c_str();
            const char *gateVectorName = childNode->name.c_str();
            return new SizeofSubmoduleGate(submoduleName, gateVectorName, inSubcomponentScope);
        }
        else if (isIdentWithIndex(grandChildNode)) { // sizeof(ident[expr].ident)
            const char *submoduleName = grandChildNode->name.c_str();
            const char *gateVectorName = childNode->name.c_str();
            auto result = new SizeofIndexedSubmoduleGate(submoduleName, gateVectorName, inSubcomponentScope);
            translateChildren(grandChildNode, result, translatorForChildren);
            return result;
        }
        else
            throw cRuntimeError("Illegal argument for sizeof()");
    }
    else {
        throw cRuntimeError("Illegal argument for sizeof()");
    }
}

ExprNode *NedOperatorTranslator::translateExists(AstNode *existsNode, AstTranslator *translatorForChildren)
{
    AstNode *argNode = getSingleChild(existsNode);
    if (isIdent(argNode)) {
        // exists(ident)
        const char *name = argNode->name.c_str();
        return new Exists(name, inSubcomponentScope);
    }
    else
        throw cRuntimeError("Illegal argument for exists()");
}

ExprNode *NedOperatorTranslator::translateIndex(AstNode *identNode, AstTranslator *translatorForChildren)
{
    if (!inSubcomponentScope)
        throw cRuntimeError("'index' may only occur in submodule context");
    return new ModuleIndex();
}

ExprNode *NedOperatorTranslator::translateIdent(AstNode *identNode, AstTranslator *translatorForChildren)
{
    const char *name = identNode->name.c_str();
    if (LoopVar::contains(name))
        return new LoopVar(name);
    else
        return new ParameterRef(name, inSubcomponentScope, false);
}

ExprNode *NedOperatorTranslator::translateMember(AstNode *memberNode, AstTranslator *translatorForChildren)
{
    ASSERT(memberNode->children.size() == 1);
    AstNode *objectNode = memberNode->children[0];
    if (isIdent(objectNode, "this")) {
        // this.foo
        bool explicitKeyword = true;
        return new ParameterRef(memberNode->name.c_str(), inSubcomponentScope, explicitKeyword);
    }
    else if (isIdent(objectNode)) {
        // foo.bar
        return new SubmoduleParameterRef(objectNode->name.c_str(), memberNode->name.c_str(), inSubcomponentScope);
    }
    else if (isIdentWithIndex(objectNode)) {
        // foo[i].bar
        ASSERT(objectNode->children.size() == 1);
        ExprNode *node = new IndexedSubmoduleParameterRef(objectNode->name.c_str(), memberNode->name.c_str(), inSubcomponentScope);
        translateChildren(objectNode, node, translatorForChildren);
        return node;
    }
    else
        throw cRuntimeError("'.%s': invalid member access", memberNode->name.c_str());
}

//---


ExprNode *NedFunctionTranslator::createFunctionNode(const char *functionName, int argCount)
{
    if (cNedFunction *nedFunction = cNedFunction::find(functionName))
        return createNedFunctionNode(nedFunction, argCount);
    if (cNedMathFunction *mathFunction = cNedMathFunction::find(functionName, argCount))
        return createMathFunctionNode(mathFunction, argCount);
    return nullptr;
}

ExprNode *NedFunctionTranslator::createObjectNode(const char *typeName, const std::vector<std::string>& fieldNames)
{
    return new ObjectNode(typeName, fieldNames);
}

ExprNode *NedFunctionTranslator::createArrayNode(int argCount)
{
    return new ArrayNode();
}

ExprNode *NedFunctionTranslator::createNedFunctionNode(cNedFunction *nedFunction, int argCount)
{
    int minArgs = nedFunction->getMinArgs();
    int maxArgs = nedFunction->getMaxArgs();
    bool varArgs = nedFunction->hasVarArgs();
    if (argCount < minArgs || (argCount > maxArgs && !varArgs)) {
        if (varArgs)
            throw opp_runtime_error("NED function '%s' expects at least %d", nedFunction->getName(), minArgs);
        else if (minArgs == maxArgs)
            throw opp_runtime_error("NED function '%s' expects %d arguments", nedFunction->getName(), minArgs);
        else
            throw opp_runtime_error("NED function '%s' expects %d to %d arguments", nedFunction->getName(), minArgs, maxArgs);
    }
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
} // namespace nedsupport
