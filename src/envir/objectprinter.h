//==========================================================================
//  OBJECTPRINTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cClassDescriptor  : metainfo about structs and classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OBJECTPRINTER_H
#define __OBJECTPRINTER_H

#include <vector>
#include <iostream>
#include "patternmatcher.h"
#include "matchexpression.h"
#include "matchableobject.h"

extern bool defaultRecurseInto(void *object, cClassDescriptor *descriptor, int fieldIndex,  void *fieldValue, int level);

class ObjectPrinter
{
    protected:
        int indentSize;
        char buffer[1024];
        std::vector<MatchExpression> *objectMatchExpressions;
        std::vector<std::vector<PatternMatcher>> *fieldNamePatternMatchersList;

    public:
        ObjectPrinter(std::vector<MatchExpression> *objectMatchExpressions,
                      std::vector<std::vector<PatternMatcher>> *fieldNamePatternMatchersList,
                      int indentSize);
        virtual ~ObjectPrinter();

        void printObjectToStream(std::ostream& ostream, cObject *object);
        std::string printObjectToString(cObject *object);

    protected:
        void printIndent(std::ostream& ostream, int level);
        void printObjectToStream(std::ostream& ostream, void *object, cClassDescriptor *descriptor, int level);
        bool matchesObjectField(cObject *object, const char *fieldName);
};

#endif
