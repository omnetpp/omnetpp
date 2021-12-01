//==========================================================================
//  OBJECTPRINTER.CC - part of
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

#include "common/stringutil.h"
#include "common/commonutil.h"
#include "common/stringtokenizer.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/carray.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cclassdescriptor.h"
#include "matchablefield.h"
#include "matchableobject.h"
#include "objectprinter.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

#define MAXIMUM_OBJECT_PRINTER_LEVEL    20

static ObjectPrinterRecursionControl defaultRecurseIntoMessageFields(any_ptr object, cClassDescriptor *descriptor, int fieldIndex, any_ptr fieldValue, any_ptr *parents, int level)
{
    const char *fieldName = descriptor->getFieldName(fieldIndex);
    return strcmp(fieldName, "owner") ? RECURSE : SKIP;
}

ObjectPrinter::ObjectPrinter(ObjectPrinterRecursionPredicate recursionPredicate,
        const std::vector<MatchExpression *>& objectMatchExpressions,
        const std::vector<std::vector<MatchExpression *> >& fieldNameMatchExpressionsList,
        int indentSize)
{
    Assert(objectMatchExpressions.size() == fieldNameMatchExpressionsList.size());
    this->recursionPredicate = recursionPredicate ? recursionPredicate : defaultRecurseIntoMessageFields;
    this->objectMatchExpressions = objectMatchExpressions;
    this->fieldNameMatchExpressionsList = fieldNameMatchExpressionsList;
    this->indentSize = indentSize;
}

ObjectPrinter::ObjectPrinter(ObjectPrinterRecursionPredicate recursionPredicate, const char *objectFieldMatcherPattern, int indentSize)
{
    std::vector<MatchExpression *> objectMatchExpressions;
    std::vector<std::vector<MatchExpression *> > fieldNameMatchExpressionsList;

    std::vector<std::string> patterns = opp_splitandtrim(objectFieldMatcherPattern, "|"); // TODO: use ; when it does not mean comment anymore

    for (auto & pattern : patterns) {
        char *objectPattern = (char *)pattern.c_str();
        char *fieldNamePattern = strchr(objectPattern, ':');

        if (fieldNamePattern) {
            *fieldNamePattern = '\0';
            std::vector<std::string> fieldNamePatterns = opp_splitandtrim(fieldNamePattern + 1, ",");
            std::vector<MatchExpression *> fieldNameMatchExpressions;

            for (auto & fieldNamePattern : fieldNamePatterns)
                fieldNameMatchExpressions.push_back(new MatchExpression(fieldNamePattern.c_str(), false, true, true));

            fieldNameMatchExpressionsList.push_back(fieldNameMatchExpressions);
        }
        else {
            std::vector<MatchExpression *> fieldNameMatchExpressions;
            fieldNameMatchExpressions.push_back(new MatchExpression("*", false, true, true));
            fieldNameMatchExpressionsList.push_back(fieldNameMatchExpressions);
        }

        objectMatchExpressions.push_back(new MatchExpression(objectPattern, false, true, true));
    }

    Assert(objectMatchExpressions.size() == fieldNameMatchExpressionsList.size());
    this->recursionPredicate = recursionPredicate ? recursionPredicate : defaultRecurseIntoMessageFields;
    this->objectMatchExpressions = objectMatchExpressions;
    this->fieldNameMatchExpressionsList = fieldNameMatchExpressionsList;
    this->indentSize = indentSize;
}

ObjectPrinter::~ObjectPrinter()
{
    for (int i = 0; i < (int)objectMatchExpressions.size(); i++) {
        delete objectMatchExpressions[i];
        std::vector<MatchExpression *>& fieldNameMatchExpressions = fieldNameMatchExpressionsList[i];
        for (auto & fieldNameMatchExpression : fieldNameMatchExpressions)
            delete fieldNameMatchExpression;
    }
}

void ObjectPrinter::printObjectToStream(std::ostream& ostream, cObject *object)
{
    any_ptr parents[MAXIMUM_OBJECT_PRINTER_LEVEL];
    cClassDescriptor *descriptor = object->getDescriptor();
    ostream << "class " << descriptor->getName() << " {\n";
    printObjectToStream(ostream, toAnyPtr(object), descriptor, parents, 0);
    ostream << "}\n";
}

std::string ObjectPrinter::printObjectToString(cObject *object)
{
    std::stringstream out;
    printObjectToStream(out, object);
    return out.str();
}

void ObjectPrinter::printObjectToStream(std::ostream& ostream, any_ptr object, cClassDescriptor *descriptor, any_ptr *parents, int level)
{
    if (level == MAXIMUM_OBJECT_PRINTER_LEVEL) {
        printIndent(ostream, level);
        ostream << "<pruned>\n";
        return;
    }
    else {
        for (int i = 0; i < level; i++) {
            if (parents[i] == object) {
                printIndent(ostream, level);
                ostream << "<recursion>\n";
                return;
            }
        }
    }
    if (!descriptor) {
        printIndent(ostream, level);
        if (level == 0)
            ostream << "{...}\n";
        else
            ostream << "...\n";
    }
    else {
        parents[level] = object;
        for (int fieldIndex = 0; fieldIndex < descriptor->getFieldCount(); fieldIndex++) {
            bool isArray = descriptor->getFieldIsArray(fieldIndex);
            bool isPointer = descriptor->getFieldIsPointer(fieldIndex);
            bool isCompound = descriptor->getFieldIsCompound(fieldIndex);
            bool isCObject = descriptor->getFieldIsCObject(fieldIndex);
            const char *fieldType = descriptor->getFieldTypeString(fieldIndex);
            const char *fieldName = descriptor->getFieldName(fieldIndex);

            int size = isArray ? descriptor->getFieldArraySize(object, fieldIndex) : 1;
            for (int elementIndex = 0; elementIndex < size; elementIndex++) {
                any_ptr fieldValue = isCompound ? descriptor->getFieldStructValuePointer(object, fieldIndex, elementIndex) : any_ptr(nullptr);

                ObjectPrinterRecursionControl result = RECURSE;
                if (recursionPredicate)
                    result = recursionPredicate(object, descriptor, fieldIndex, fieldValue, parents, level);
                if (result == SKIP || (descriptor->extendsCObject() && !matchesObjectField(fromAnyPtr<cObject>(object), fieldIndex)))
                    continue;

                printIndent(ostream, level + 1);
                ostream << fieldType << " ";

                if (isPointer)
                    ostream << "*";
                ostream << fieldName;

                if (isArray)
                    ostream << "[" << elementIndex << "]";
                ostream << " = ";

                if (isCompound) {
                    if (fieldValue != nullptr) {
                        cClassDescriptor *fieldDescriptor = isCObject ? fromAnyPtr<cObject>(fieldValue)->getDescriptor() :
                            cClassDescriptor::getDescriptorFor(descriptor->getFieldStructName(fieldIndex));

                        if (isCObject && result == FULL_NAME)
                            ostream << fromAnyPtr<cObject>(fieldValue)->getFullName() << "\n";
                        else if (isCObject && result == FULL_PATH)
                            ostream << fromAnyPtr<cObject>(fieldValue)->getFullPath() << "\n";
                        else if (fieldDescriptor) {
                            if (isCObject)
                                ostream << "class " << fromAnyPtr<cObject>(fieldValue)->getClassName() << " ";
                            else
                                ostream << "struct " << descriptor->getFieldStructName(fieldIndex) << " ";

                            ostream << "{\n";
                            printObjectToStream(ostream, fieldValue, fieldDescriptor, parents, level + 1);
                            printIndent(ostream, level + 1);
                            ostream << "}\n";
                        }
                        else {
                            std::string value = descriptor->getFieldValueAsString(object, fieldIndex, elementIndex);
                            ostream << QUOTE(value.c_str()) << "\n";
                        }
                    }
                    else
                        ostream << "nullptr\n";
                }
                else {
                    std::string value = descriptor->getFieldValueAsString(object, fieldIndex, elementIndex);
                    ostream << QUOTE(value.c_str()) << "\n";
                }
            }
        }
    }
}

void ObjectPrinter::printIndent(std::ostream& ostream, int level)
{
    int count = level * indentSize;
    for (int i = 0; i < count; i++)
        ostream << " ";
}

bool ObjectPrinter::matchesObjectField(cObject *object, int fieldIndex)
{
    const MatchableObjectAdapter matchableObject(MatchableObjectAdapter::CLASSNAME, object);

    for (int i = 0; i < (int)objectMatchExpressions.size(); i++) {
        MatchExpression *objectMatchExpression = objectMatchExpressions[i];

        if (objectMatchExpression->matches(&matchableObject)) {
            std::vector<MatchExpression *>& fieldNameMatchExpressions = fieldNameMatchExpressionsList[i];

            for (auto fieldNameMatchExpression : fieldNameMatchExpressions) {
                const MatchableFieldAdapter matchableField(object, fieldIndex);

                if (fieldNameMatchExpression->matches(&matchableField))
                    return true;
            }
        }
    }

    return false;
}

}  // namespace envir
}  // namespace omnetpp

