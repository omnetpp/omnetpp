//==========================================================================
//  CMESSAGEPRINTER.H - part of
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

#ifndef __OMNETPP_CMESSAGEPRINTER_H
#define __OMNETPP_CMESSAGEPRINTER_H

#include <set>
#include <vector>
#include "cownedobject.h"

namespace omnetpp {

class cMessage;

/**
 * @brief Base class for message printers.
 *
 * Message printers is a somewhat experimental feature, intended at giving
 * models a way to control how network packets or in general, cMessage
 * objects) are displayed. Currently, message printer output is used for
 * the Messages mode display of the Log viewer in Qtenv.
 *
 * Several message printers may be registered (with Register_MessagePrinter()),
 * independent of each other, and OMNeT++ will choose the "best" or most
 * specific one for each message object. The best one is decided based on
 * scoring: every message printer will be polled by passing the message
 * object to its getScoreFor() method, and the message printer returning
 * the highest score will "win".
 *
 * OMNeT++ calls printMessage() to obtain information to be displayed about a
 * message.
 *
 * @see Register_MessagePrinter()
 * @ingroup EnvirAndExtensions
 */
class SIM_API cMessagePrinter : public cNoncopyableOwnedObject
{
    public:
        /**
         * Options for printing
         */
        struct Options {
            std::set<std::string> enabledTags;
        };

    public:
        /**
         * Constructor
         */
        cMessagePrinter() {}

        /**
         * Destructor
         */
        virtual ~cMessagePrinter() {}

        /**
         * Scoring function that lets OMNeT++ decide which message printer to
         * use for a particular message object. The "better", more specific
         * the message printer is for that message, the higher score it should
         * return. Guideline:
         *    0 or negative: does not support this message;
         *    10: returned by cDefaultMessagePrinter
         */
        virtual int getScoreFor(cMessage *msg) const = 0;

        /**
         * Returns the tags supported by this printer.
         *
         * Note: These are normally presented to the user on the graphical
         * interfaces unmodified, and there are no constraints on the format
         * or content of the tags, so for example using "Show 'Details' column"
         * is preferred to using "col_details".
         */
        virtual std::set<std::string> getSupportedTags() const {return {};}

        /**
         * Returns the list of tags that this printer recommends to enable by default.
         * The result must only contain tags that are also returned by "getSupportedTags()".
         */
        virtual std::set<std::string> getDefaultEnabledTags() const {return {};}

        /**
         * Returns column names for the specified options.
         * The options parameter should never be nullptr.
         * The list of enabled tags in "options" may contain tags not known or
         * supported by this printer. Those should be silently ignored.
         */
        virtual std::vector<std::string> getColumnNames(const Options *options) const {(void)options; return {};}

        /**
         * Print a single-line text about the message object into the given
         * output stream. Use Tab characters to separate columns.
         * Splitting the text into more or fewer columns than returned by
         * getColumnNames is allowed, but being consistent with those is
         * highly recommended. The options parameter should never be nullptr.
         */
        virtual void printMessage(std::ostream& os, cMessage *msg, const Options *options) const = 0;
};

/**
 * @brief A default message printer that displays the ID, kind, and length
 * (for packets) of the message.
 *
 * Note: The name and class name do not need to be returned, as they
 * are already displayed by default.
 */
class SIM_API cDefaultMessagePrinter : public cMessagePrinter
{
    public:
        cDefaultMessagePrinter() {}
        virtual ~cDefaultMessagePrinter() {}
        virtual int getScoreFor(cMessage *msg) const override;

        virtual std::set<std::string> getSupportedTags() const override;
        virtual std::set<std::string> getDefaultEnabledTags() const override;
        virtual std::vector<std::string> getColumnNames(const Options *options) const override;

        virtual void printMessage(std::ostream& os, cMessage *msg, const Options *options) const override;
};

}  // namespace omnetpp

#endif


