//==========================================================================
// nederror.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDERROR_H
#define __NEDERROR_H

#include <vector>
#include <exception>
#include <stdexcept>
#include "nedelement.h"


enum NEDErrorCategory {ERRCAT_INFO, ERRCAT_WARNING, ERRCAT_ERROR, ERRCAT_FATAL};

/**
 * Stores error messages
 */
class NEDErrorStore
{
    private:
        struct Entry {
            NEDElement *context;
            int category;
            std::string location;
            std::string message;
        };

        std::vector<Entry> entries;
        bool doprint;

     private:
        void doAdd(NEDElement *context, const char *loc, int category, const char *message);

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
         * Add an error message with the default category being ERROR.
         */
        void add(NEDElement *context, const char *message, ...);

        /**
         * Add an error message.
         */
        void add(NEDElement *context, int category, const char *message, ...);

        /**
         * Add an error message.
         */
        void add(const char *location, int category, const char *message, ...);

        /**
         * Return true if there're no messages stored.
         */
        bool empty() const {return entries.empty();}

        /**
         * Total number of fatal, error, warning and info messages.
         */
        int numMessages() const {return entries.size();}

        /**
         * Returns true if there's an error or fatal error stored.
         */
        bool containsError() const;

        /**
         * Returns true if there's a fatal error stored.
         */
        bool containsFatal() const;

        /**
         * Discard all messages stored.
         */
        void clear() {entries.clear();}

        /** @name Returns properties of the ith message stored (i=0..numMessages-1) */
        //@{
        const char *errorCategory(int i) const;
        int errorCategoryCode(int i) const;
        const char *errorLocation(int i) const;
        NEDElement *errorContext(int i) const;
        const char *errorText(int i) const;
        //@}

        /**
         * Convert categories from numeric to textual form.
         */
        static const char *categoryName(int cat);
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
void NEDInternalError(const char *file, int line, NEDElement *context, const char *message, ...);


/**
 * Low-level routines throw an exception instead of calling NEDErrorStore->add().
 */
class NEDException : public std::runtime_error   //FIXME into separate header file
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    NEDException(const char *msg,...);

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() {return errormsg.c_str();}
};


#endif

