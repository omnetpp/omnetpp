//==========================================================================
//  OBJECTPRINTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stringutil.h"
#include "cconfiguration.h"
#include "cmodule.h"
#include "cclassdescriptor.h"
#include "commonutil.h"
#include "objectprinter.h"

USING_NAMESPACE

static bool defaultRecurseIntoCObject(void *object, cClassDescriptor *descriptor, int fieldIndex, cObject *fieldValue, int level)
{
    cArray *carray = dynamic_cast<cArray *>(fieldValue);

    return !((carray && !carray->items()) ||
             dynamic_cast<cModule *>(fieldValue) ||
             dynamic_cast<cGate *>(fieldValue));
}

static bool defaultRecurseInto(void *object, cClassDescriptor *descriptor, int fieldIndex, void *fieldValue, int level)
{
    const char *fieldName = descriptor->getFieldName(object, fieldIndex);
    bool isCObject = descriptor->getFieldIsCObject(object, fieldIndex);

    return level < 5 &&
        (fieldValue == NULL || !isCObject || defaultRecurseIntoCObject(object, descriptor, fieldIndex, (cObject *)fieldValue, level)) &&
        strcmp(fieldName, "owner") &&
        strcmp(fieldName, "className");
}

//----

ObjectPrinter::ObjectPrinter(std::vector<MatchExpression> &objectMatchExpressions,
                             std::vector<std::vector<MatchExpression> > &fieldNameMatchExpressionsList,
                             int indentSize)
{
    Assert(objectMatchExpressions.size() == fieldNameMatchExpressionsList.size());
    this->objectMatchExpressions = objectMatchExpressions;
    this->fieldNameMatchExpressionsList = fieldNameMatchExpressionsList;
    this->indentSize = indentSize;
}

void ObjectPrinter::printObjectToStream(std::ostream& ostream, cObject *object)
{
    cClassDescriptor *descriptor = cClassDescriptor::getDescriptorFor(object);
    ostream << "class " << descriptor->name() << " {\n";
    printObjectToStream(ostream, object, descriptor, 1);
    ostream << "}\n";
}

std::string ObjectPrinter::printObjectToString(cObject *object)
{
    std::stringstream out;
    printObjectToStream(out, object);
    return out.str();
}

void ObjectPrinter::printObjectToStream(std::ostream& ostream, void *object, cClassDescriptor *descriptor, int level)
{
    if (!descriptor) {
        printIndent(ostream, level);
        if (level == 0)
            ostream << "{...}\n";
        else
            ostream << "...\n";
    }
    else {
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

                if (!defaultRecurseInto(object, descriptor, fieldIndex, fieldValue, level) ||
                    (descriptor->extendsCObject() && !matchesObjectField((cObject *)object, fieldIndex)))
                    continue;

                printIndent(ostream, level);
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

                        if (fieldDescriptor) {
                            if (isCObject)
                                ostream << "class " << ((cObject *)fieldValue)->className() << " ";
                            else
                                ostream << "struct " << descriptor->getFieldStructName(object, fieldIndex) << " ";

                            ostream << "{\n";
                            printObjectToStream(ostream, fieldValue, fieldDescriptor, level + 1);
                            printIndent(ostream, level);
                            ostream << "}\n";
                        }
                        else {
                            descriptor->getFieldAsString(object, fieldIndex, elementIndex, buffer, sizeof(buffer));
                            ostream << QUOTE(buffer) << "\n";
                        }
                    }
                    else
                        ostream << "<NULL>\n";
                }
                else {
                    descriptor->getFieldAsString(object, fieldIndex, elementIndex, buffer, sizeof(buffer));
                    ostream << QUOTE(buffer) << "\n";
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
        MatchExpression &objectMatchExpression = objectMatchExpressions[i];

        if (objectMatchExpression.matches(&matchableObject)) {
            std::vector<MatchExpression> &fieldNameMatchExpressions = fieldNameMatchExpressionsList[i];

            for (int j = 0; j < (int)fieldNameMatchExpressions.size(); j++) {
                MatchExpression &fieldNameMatchExpression = fieldNameMatchExpressions[j];
                const MatchableFieldAdapter matchableField(object, fieldIndex);

                if (fieldNameMatchExpression.matches(&matchableField))
                    return true;
            }
        }
    }

    return false;
}
