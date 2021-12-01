//==========================================================================
//  OBJECTPRINTER.H - part of
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

#ifndef __OMNETPP_ENVIR_OBJECTPRINTER_H
#define __OMNETPP_ENVIR_OBJECTPRINTER_H

#include <vector>
#include <iostream>
#include "envirdefs.h"
#include "common/matchexpression.h"

namespace omnetpp {

class cClassDescriptor;
class cObject;
class any_ptr;

namespace envir {

/**
 * Controls recursion depth in OpbjectPrinter.
 */
enum ObjectPrinterRecursionControl {
    SKIP,      // don't print this field
    RECURSE,   // print this field in detail by recursing down
    FULL_NAME, // print the full name only (applicable to cObject)
    FULL_PATH  // print the full Path only (applicable to cObject)
};

/**
 * This is function type that controls recursion during printing an object.
 * It will be asked what to do whenever recursion occurs at a compound object's field.
 *
 * Parameters: object that has the field, object's class descriptor, field index, field value,
 * parent objects collected during recursion, recursion level.
 */
typedef ObjectPrinterRecursionControl (*ObjectPrinterRecursionPredicate)(any_ptr, cClassDescriptor *, int, any_ptr, any_ptr*, int);

/**
 * A utility class to serialize an object in text form. It is currently used
 * to record message data into the event log file.
 */
class ENVIR_API ObjectPrinter
{
    public:
        typedef omnetpp::common::MatchExpression MatchExpression;

    protected:
        int indentSize;
        char buffer[1024];
        std::vector<MatchExpression*> objectMatchExpressions;
        std::vector<std::vector<MatchExpression*> > fieldNameMatchExpressionsList;
        ObjectPrinterRecursionPredicate recursionPredicate;

    public:
        /**
         * Accepts the parsed form of the pattern string. The two vectors
         * must be of the same size. The contained MatchExpression objects
         * will be deallocated by this ObjectPrinter.
         */
        ObjectPrinter(ObjectPrinterRecursionPredicate recursionPredicate,
                      const std::vector<MatchExpression*>& objectMatchExpressions,
                      const std::vector<std::vector<MatchExpression*> >& fieldNameMatchExpressionsList,
                      int indentSize=4);

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
         * "MyMessage:declaredOn=~MyMessage":
         *     captures instances of MyMessage recording the fields
         *     declared on the MyMessage class
         * "*:(not declaredOn=~cMessage and not declaredOn=~cNamedObject and
         * not declaredOn=~cObject)":
         *     records user-defined fields from all objects
         */
        ObjectPrinter(ObjectPrinterRecursionPredicate recursionPredicate=nullptr, const char *pattern="*", int indentSize=4);

        /**
         * Destructor.
         */
        ~ObjectPrinter();

        void printObjectToStream(std::ostream& ostream, cObject *object);

        std::string printObjectToString(cObject *object);

    protected:
        void printIndent(std::ostream& ostream, int level);
        void printObjectToStream(std::ostream& ostream, any_ptr object, cClassDescriptor *descriptor, any_ptr *objects, int level);
        bool matchesObjectField(cObject *object, int fieldIndex);
};

}  // namespace envir
}  // namespace omnetpp


#endif

