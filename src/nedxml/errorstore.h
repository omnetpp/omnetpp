//==========================================================================
// errorstore.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_ERRORSTORE_H
#define __OMNETPP_NEDXML_ERRORSTORE_H

#include <vector>
#include <exception>
#include <stdexcept>
#include <string>
#include "nedxmldefs.h"

namespace omnetpp {
namespace nedxml {

class ASTNode;

// Note: this cannot be inner type because of swig wrapping
enum ProblemSeverity {SEVERITY_INFO, SEVERITY_WARNING, SEVERITY_ERROR};

/**
 * Stores error and warning messages
 */
class NEDXML_API ErrorStore
{
    private:
        struct Entry {
            ASTNode *context;
            int severity;
            std::string location;
            std::string message;
        };

        std::vector<Entry> entries;
        bool doprint;

     private:
        void doAdd(ASTNode *context, const char *loc, int severity, const char *message);

     public:
        /**
         * Ctor
         */
        ErrorStore() {doprint = false;}
        ~ErrorStore() {}

        /**
         * If set, errors get dumped to stderr as well as stored
         */
        void setPrintToStderr(bool p) {doprint = p;}

        /**
         * Add an error message with the severity ERROR.
         */
        _OPP_GNU_ATTRIBUTE(format(printf, 3, 4))
        void addError(ASTNode *context, const char *messagefmt, ...);

        /**
         * Add an error message with the severity ERROR.
         */
        _OPP_GNU_ATTRIBUTE(format(printf, 3, 4))
        void addError(const char *location, const char *messagefmt, ...);

        /**
         * Add an error message with the severity WARNING.
         */
        _OPP_GNU_ATTRIBUTE(format(printf, 3, 4))
        void addWarning(ASTNode *context, const char *messagefmt, ...);

        /**
         * Add an error message with the severity WARNING.
         */
        _OPP_GNU_ATTRIBUTE(format(printf, 3, 4))
        void addWarning(const char *location, const char *messagefmt, ...);

        /**
         * Add an error message.
         */
        _OPP_GNU_ATTRIBUTE(format(printf, 4, 5))
        void add(ASTNode *context, int severity, const char *messagefmt, ...);

        /**
         * Add an error message.
         */
        _OPP_GNU_ATTRIBUTE(format(printf, 4, 5))
        void add(const char *location, int severity, const char *messagefmt, ...);

        /**
         * Return true if there are no messages stored.
         */
        bool empty() const {return entries.empty();}

        /**
         * Total number of error, warning and info messages.
         */
        int numMessages() const {return entries.size();}

        /**
         * Returns true if there is an error or fatal error stored.
         */
        bool containsError() const;

        /**
         * Discard all messages stored.
         */
        void clear() {entries.clear();}

        /** @name Returns properties of the ith message stored (i=0..numMessages-1) */
        //@{
        const char *errorSeverity(int i) const;
        int errorSeverityCode(int i) const;
        const char *errorLocation(int i) const;
        ASTNode *errorContext(int i) const;
        const char *errorText(int i) const;
        //@}

        /**
         * Return the first message with index >= startIndex whose
         * context is the given node. Returns -1 if none found.
         */
        int findFirstErrorFor(ASTNode *node, int startIndex) const;

        /**
         * Convert severities from numeric to textual form.
         */
        static const char *severityName(int severity);
};


#define INTERNAL_ERROR0(context,msg) NedInternalError(__FILE__,__LINE__,context,msg)
#define INTERNAL_ERROR1(context,msg,arg1) NedInternalError(__FILE__,__LINE__,context,msg,arg1)
#define INTERNAL_ERROR2(context,msg,arg1,arg2)   NedInternalError(__FILE__,__LINE__,context,msg,arg1,arg2)
#define INTERNAL_ERROR3(context,msg,arg1,arg2,arg3) NedInternalError(__FILE__,__LINE__,context,msg,arg1,arg2,arg3)

/**
 * Called when an internal error occurs. It prints an appropriate message, then calls abort().
 * This method is typically used via the INTERNAL_ERROR0()...INTERNAL_ERROR4() macros that
 * add the __FILE__, __LINE__ args implicitly.
 */
_OPP_GNU_ATTRIBUTE(format(printf, 4, 5))
void NedInternalError(const char *file, int line, ASTNode *context, const char *messagefmt, ...);


}  // namespace nedxml
}  // namespace omnetpp


#endif

