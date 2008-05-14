//==========================================================================
// nederror.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDERROR_H
#define __NEDERROR_H

#include <vector>
#include <exception>
#include <stdexcept>
#include "nedxmldefs.h"

NAMESPACE_BEGIN

class NEDElement;

// Note: this cannot be inner type because of swig wrapping
enum NEDErrorSeverity {NED_SEVERITY_INFO, NED_SEVERITY_WARNING, NED_SEVERITY_ERROR};

/**
 * Stores error messages
 */
class NEDXML_API NEDErrorStore
{
    private:
        struct Entry {
            NEDElement *context;
            int severity;
            std::string location;
            std::string message;
        };

        std::vector<Entry> entries;
        bool doprint;

     private:
        void doAdd(NEDElement *context, const char *loc, int severity, const char *message);

     public:
        /**
         * Ctor
         */
        NEDErrorStore() {doprint = false;}
        ~NEDErrorStore() {}

        /**
         * If set, errors get dumped to stderr as well as stored
         */
        void setPrintToStderr(bool p) {doprint = p;}

        /**
         * Add an error message with the severity ERROR.
         */
        void addError(NEDElement *context, const char *messagefmt, ...);

        /**
         * Add an error message with the severity ERROR.
         */
        void addError(const char *location, const char *messagefmt, ...);

        /**
         * Add an error message with the severity WARNING.
         */
        void addWarning(NEDElement *context, const char *messagefmt, ...);

        /**
         * Add an error message with the severity WARNING.
         */
        void addWarning(const char *location, const char *messagefmt, ...);

        /**
         * Add an error message.
         */
        void add(NEDElement *context, int severity, const char *messagefmt, ...);

        /**
         * Add an error message.
         */
        void add(const char *location, int severity, const char *messagefmt, ...);

        /**
         * Return true if there're no messages stored.
         */
        bool empty() const {return entries.empty();}

        /**
         * Total number of error, warning and info messages.
         */
        int numMessages() const {return entries.size();}

        /**
         * Returns true if there's an error or fatal error stored.
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
        NEDElement *errorContext(int i) const;
        const char *errorText(int i) const;
        //@}

        /**
         * Return the first message with index >= startIndex whose
         * context is the given node. Returns -1 if none found.
         */
        int findFirstErrorFor(NEDElement *node, int startIndex) const;

        /**
         * Convert severities from numeric to textual form.
         */
        static const char *severityName(int severity);
};


#define INTERNAL_ERROR0(context,msg) NEDInternalError(__FILE__,__LINE__,context,msg)
#define INTERNAL_ERROR1(context,msg,arg1) NEDInternalError(__FILE__,__LINE__,context,msg,arg1)
#define INTERNAL_ERROR2(context,msg,arg1,arg2)   NEDInternalError(__FILE__,__LINE__,context,msg,arg1,arg2)
#define INTERNAL_ERROR3(context,msg,arg1,arg2,arg3) NEDInternalError(__FILE__,__LINE__,context,msg,arg1,arg2,arg3)

/**
 * Called when an internal error occurs. It prints an appropriate message, then calls abort().
 * This method is typically used via the INTERNAL_ERROR0()...INTERNAL_ERROR4() macros that
 * add the __FILE__, __LINE__ args implicitly.
 */
void NEDInternalError(const char *file, int line, NEDElement *context, const char *messagefmt, ...);


NAMESPACE_END


#endif

