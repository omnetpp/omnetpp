//==========================================================================
//   CWATCH.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cWatch    : shell for a char/int/long/double/char*/cObject* variable
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CWATCH_H
#define __CWATCH_H

#include "cobject.h"

//=== classes declared here
class  cWatch;

/** @name Watches
 *
 * @ingroup Macros
 */
//@{

/**
 * Enables int, long, double, char, etc. variables to be inspectable in Tkenv
 * and to be included into the snapshot file. Example usage:
 * <PRE>
 *   int i;
 *   WATCH(i);
 * </PRE>
 *
 * The WATCH() macro expands to a dynamically created cWatch object, like
 * this:
 * <PRE>
 *   new cWatch("i",i);
 * </PRE>
 *
 * The WATCH() macro must be executed exactly once for a variable,
 * otherwise a new cWatch object is created every time. The alternative
 * is LWATCH which works by creating a local variable.
 *
 * LWATCH expands to something like
 * <PRE>
 *   cWatch i__varshell("i",i);
 * </PRE>
 *
 * @hideinitializer
 * @see LWATCH()
 */
#define WATCH(var)   new cWatch( #var, var );

/**
 * Like WATCH(), but may be executed more than once.
 *
 * @hideinitializer
 * @see WATCH()
 */
#define LWATCH(var)  cWatch var##__varshell( #var, var );
//@}

//==========================================================================

/**
 * Shell for an ordinary char, int, long, double, char* or cObject* variable.
 * Its purpose is to make variables visible in Tkenv.
 *
 * Users rarely need to create cWatch objects directly, they rather use
 * the WATCH() and LWATCH() macros.
 *
 * @ingroup SimSupport
 */
class SIM_API cWatch : public cObject
{
  private:
    void *ptr;
    char type;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cWatch(const cWatch& vs);

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, char& c)  : cObject(name) {ptr=&c; type='c';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, signed char& c)  : cObject(name) {ptr=&c; type='c';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, unsigned char& c)  : cObject(name) {ptr=&c; type='c';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, bool& b)  : cObject(name) {ptr=&b; type='b';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, int& i)  : cObject(name) {ptr=&i; type='i';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, unsigned int& i)  : cObject(name) {ptr=&i; type='i';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, long& l)  : cObject(name) {ptr=&l; type='l';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, unsigned long& l)  : cObject(name) {ptr=&l; type='l';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, double& d): cObject(name) {ptr=&d; type='d';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, const char* &s) : cObject(name) {ptr=&s; type='s';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, const signed char* &s) : cObject(name) {ptr=&s; type='s';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, const unsigned char* &s) : cObject(name) {ptr=&s; type='s';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, const cObject* &o) : cObject(name) {ptr=&o; type='o';}

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cWatch& operator=(const cWatch& vs)     {ptr=vs.ptr;type=vs.type;return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const   {return new cWatch(*this);}

    /**
     * Produces a one-line information about the object.
     * Output is like this: "int samples = 12 (12U, 0xC)"
     */
    virtual void info(char *buf);

    /**
     * Writes the value of the variable to the output stream.
     * The output looks like this: "int samples = 12 (12U, 0xC)".
     */
    virtual void writeContents(ostream& os);
    //@}

    /** @name Accessing the stored variable reference. */
    //@{

    /**
     * Does actual work for info() and writeContents().
     * The output looks like this: "int samples = 12 (12U, 0xC)".
     */
    virtual void printTo(char *s);  //FIXME: why public?

    /**
     * Returns the type of the referenced variable.
     * The return value is 'c','i','l','d','s','o' for char, int, long,
     * double, string, cObject pointer, respectively.
     */
    char typeChar() const {return type;}

    /**
     * Returns pointer to the referenced variable.
     */
    void *pointer() const {return ptr;}
    //@}
};

#endif

