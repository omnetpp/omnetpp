//==========================================================================
//  OBJECTPRINTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stringutil.h"
#include "cconfiguration.h"
#include "carray.h"
#include "cmodule.h"
#include "cgate.h"
#include "cclassdescriptor.h"
#include "commonutil.h"
#include "stringtokenizer.h"
#include "matchexpression.h"
#include "matchablefield.h"
#include "matchableobject.h"
#include "objectprinter.h"

NAMESPACE_BEGIN

#define MAXIMUM_OBJECT_PRINTER_LEVEL 20

static ObjectPrinterRecursionControl defaultRecurseIntoMessageFields(void *object, cClassDescriptor *descriptor, int fieldIndex, void *fieldValue, void **parents, int level) {
    const char *fieldName = descriptor->getFieldName(object, fieldIndex);
    return strcmp(fieldName, "owner") ? RECURSE : SKIP;
}

ObjectPrinter::ObjectPrinter(ObjectPrinterRecursionPredicate recursionPredicate,
                             const std::vector<MatchExpression*>& objectMatchExpressions,
                             const std::vector<std::vector<MatchExpression*> >& fieldNameMatchExpressionsList,
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
    std::vector<MatchExpression*> objectMatchExpressions;
    std::vector<std::vector<MatchExpression*> > fieldNameMatchExpressionsList;

    StringTokenizer tokenizer(objectFieldMatcherPattern, "|"); // TODO: use ; when it does not mean comment anymore
    std::vector<std::string> patterns = tokenizer.asVector();

    for (int i = 0; i < (int)patterns.size(); i++) {
        char *objectPattern = (char *)patterns[i].c_str();
        char *fieldNamePattern = strchr(objectPattern, ':');

        if (fieldNamePattern) {
            *fieldNamePattern = '\0';
            StringTokenizer fieldNameTokenizer(fieldNamePattern + 1, ",");
            std::vector<std::string> fieldNamePatterns = fieldNameTokenizer.asVector();
            std::vector<MatchExpression*> fieldNameMatchExpressions;

            for (int j = 0; j < (int)fieldNamePatterns.size(); j++)
                fieldNameMatchExpressions.push_back(new MatchExpression(fieldNamePatterns[j].c_str(), false, true, true));

            fieldNameMatchExpressionsList.push_back(fieldNameMatchExpressions);
        }
        else {
            std::vector<MatchExpression*> fieldNameMatchExpressions;
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
        std::vector<MatchExpression*>& fieldNameMatchExpressions = fieldNameMatchExpressionsList[i];
        for (int j = 0; j < (int)fieldNameMatchExpressions.size(); j++)
            delete fieldNameMatchExpressions[j];
    }

}

void ObjectPrinter::printObjectToStream(std::ostream& ostream, cObject *object)
{
    void *parents[MAXIMUM_OBJECT_PRINTER_LEVEL];
    cClassDescriptor *descriptor = cClassDescriptor::getDescriptorFor(object);
    ostream << "class " << descriptor->getName() << " {\n";
    printObjectToStream(ostream, object, descriptor, parents, 0);
    ostream << "}\n";
}

std::string ObjectPrinter::printObjectToString(cObject *object)
{
    std::stringstream out;
    printObjectToStream(out, object);
    return out.str();
}

void ObjectPrinter::printObjectToStream(std::ostream& ostream, void *object, cClassDescriptor *descriptor, void **parents, int level)
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
        for (int fieldIndex = 0; fieldIndex < descriptor->getFieldCount(object); fieldIndex++) {
            bool isArray = descriptor->getFieldIsArray(object, fieldIndex);
            bool isPointer = descriptor->getFieldIsPointer(object, fieldIndex);
            bool isCompound = descriptor->getFieldIsCompound(object, fieldIndex);
            bool isCObject = descriptor->getFieldIsCObject(object, fieldIndex);
            const char *fieldType = descriptor->getFieldTypeString(object, fieldIndex);
            const char *fieldName = descriptor->getFieldName(object, fieldIndex);

            int size = isArray ? descriptor->getArraySize(object, fieldIndex) : 1;
            for (int elementIndex = 0; elementIndex < size; elementIndex++) {
                void *fieldValue = isCompound ? descriptor->getFieldStructPointer(object, fieldIndex, elementIndex) : NULL;

                ObjectPrinterRecursionControl result = RECURSE;
                if (recursionPredicate)
                    result = recursionPredicate(object, descriptor, fieldIndex, fieldValue, parents, level);
                if (result == SKIP || (descriptor->extendsCObject() && !matchesObjectField((cObject *)object, fieldIndex)))
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
                    if (fieldValue) {
                        cClassDescriptor *fieldDescriptor = isCObject ? cClassDescriptor::getDescriptorFor((cObject *)fieldValue) :
                                                                        cClassDescriptor::getDescriptorFor(descriptor->getFieldStructName(object, fieldIndex));

                        if (isCObject && result == FULL_NAME)
                            ostream << ((cObject *)fieldValue)->getFullName() << "\n";
                        else if (isCObject && result == FULL_PATH)
                            ostream << ((cObject *)fieldValue)->getFullPath() << "\n";
                        else if (fieldDescriptor) {
                            if (isCObject)
                                ostream << "class " << ((cObject *)fieldValue)->getClassName() << " ";
                            else
                                ostream << "struct " << descriptor->getFieldStructName(object, fieldIndex) << " ";

                            ostream << "{\n";
                            printObjectToStream(ostream, fieldValue, fieldDescriptor, parents, level + 1);
                            printIndent(ostream, level + 1);
                            ostream << "}\n";
                        }
                        else {
                            std::string value = descriptor->getFieldAsString(object, fieldIndex, elementIndex);
                            ostream << QUOTE(value.c_str()) << "\n";
                        }
                    }
                    else
                        ostream << "NULL\n";
                }
                else {
                    std::string value = descriptor->getFieldAsString(object, fieldIndex, elementIndex);
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
            std::vector<MatchExpression*>& fieldNameMatchExpressions = fieldNameMatchExpressionsList[i];

            for (int j = 0; j < (int)fieldNameMatchExpressions.size(); j++) {
                MatchExpression *fieldNameMatchExpression = fieldNameMatchExpressions[j];
                const MatchableFieldAdapter matchableField(object, fieldIndex);

                if (fieldNameMatchExpression->matches(&matchableField))
                    return true;
            }
        }
    }

    return false;
}

NAMESPACE_END

