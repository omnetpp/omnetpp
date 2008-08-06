//==========================================================================
//  OBJECTPRINTER.H - part of
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

#ifndef __OBJECTPRINTER_H
#define __OBJECTPRINTER_H

#include <vector>
#include <iostream>
#include "matchexpression.h"
#include "matchablefield.h"
#include "matchableobject.h"

NAMESPACE_BEGIN


/**
 * A utility class to serialize an object in text form. It is currently used
 * to record message data into the event log file.
 */
class ENVIR_API ObjectPrinter
{
    protected:
        int indentSize;
        char buffer[1024];
        std::vector<MatchExpression> objectMatchExpressions;
        std::vector<std::vector<MatchExpression> > fieldNameMatchExpressionsList;

    public:
        /**
         * Pattern syntax is that of the "eventlog-message-detail-pattern"
         * configuration entry -- see documentation there.
         *
         * Just some examples here:
         * "*":
         *     captures all fields of all messages
         * "*Msg | *Packet":
         *     captures all fields of classes named AnythingMsg or AnythingPacket
         * "*Frame:*Address,*Id":
         *     captures all fields named anythingAddress and anythingId from
         *     objects of any class named AnythingFrame
         * "MyMessage:declaredOn(MyMessage)":
         *     captures instances of MyMessage recording the fields
         *     declared on the MyMessage class
         * "*:(not declaredOn(cMessage) and not declaredOn(cNamedObject) and
         * not declaredOn(cObject))":
         *     records user-defined fields from all objects
         */
        ObjectPrinter(const char *pattern, int indentSize);

        /**
         * Accepts the parsed form of the pattern string.
         * Size of the two vectors must be the same.
         */
        ObjectPrinter(std::vector<MatchExpression> &objectMatchExpressions,
                      std::vector<std::vector<MatchExpression> > &fieldNameMatchExpressionsList,
                      int indentSize);

        void printObjectToStream(std::ostream& ostream, cObject *object);

        std::string printObjectToString(cObject *object);

    protected:
        void printIndent(std::ostream& ostream, int level);
        void printObjectToStream(std::ostream& ostream, void *object, cClassDescriptor *descriptor, int level);
        bool matchesObjectField(cObject *object, int fieldIndex);
};

NAMESPACE_END


#endif

