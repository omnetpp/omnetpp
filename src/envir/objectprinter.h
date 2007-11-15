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


/**
 * A utility class to serialize an object in text form. It is currently used
 * to record message data into the event log file.
 */
class ENVIR_API ObjectPrinter
{
    protected:
        int indentSize;
        char buffer[1024];
        std::vector<MatchExpression> *objectMatchExpressions;
        std::vector<std::vector<PatternMatcher> > *fieldNamePatternMatchersList;

    public:
        /**
         * FIXME Levy: pls document parameters and operation
         */
        ObjectPrinter(std::vector<MatchExpression> *objectMatchExpressions,
                      std::vector<std::vector<PatternMatcher> > *fieldNamePatternMatchersList,
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
