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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CWATCH_H
#define __CWATCH_H

#include "cobject.h"

//=== classes declared here
class  cWatch;


#define WATCH(var)   new cWatch( #var, var );
#define LWATCH(var)  cWatch var##__varshell( #var, var );


//==========================================================================

/**
 * Shell for an ordinary char, int, long, double, char* or cObject* variable.
 * Its purpose is to make variables visible in Tkenv.
 *
 * Users rarely need to create cWatch objects directly, they rather use
 * the WATCH() and LWATCH() macros.
 */
class SIM_API cWatch : public cObject
{
  private:
    void *ptr;
    char type;
  public:

    /**
     * Copy constructor.
     */
    cWatch(cWatch& vs);

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, char& c)  : cObject(name) {ptr=&c; type='c';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, int&  i)  : cObject(name) {ptr=&i; type='i';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, long& l)  : cObject(name) {ptr=&l; type='l';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, double& d): cObject(name) {ptr=&d; type='d';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, char* &s) : cObject(name) {ptr=&s; type='s';}

    /**
     * Initialize the shell to hold the given variable.
     */
    cWatch(const char *name, cObject* &o) : cObject(name) {ptr=&o; type='o';}

    /**
     * All usual virtual functions redefined.
     */
    virtual const char *className() const {return "cWatch";}

    /**
     * All usual virtual functions redefined.
     */
    virtual cObject *dup()   {return new cWatch(*this);}

    /**
     * These functions are redefined to display the value of the variable.
     * Output is like this: "int samples = 12 (12U, 0xC)"
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cWatch:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cWatchIFC";}

    /**
     * MISSINGDOC: cWatch:cWatch&operator=(cWatch&)
     */
    cWatch& operator=(cWatch& vs)
          {ptr=vs.ptr;type=vs.type;return *this;}

    /**
     * These functions are redefined to display the value of the variable.
     * Output is like this: "int samples = 12 (12U, 0xC)"
     */
    virtual void writeContents(ostream& os);


    /**
     * Does actual work for info() and writeContents().
     */
    virtual void printTo(char *s);

    /**
     * MISSINGDOC: cWatch:char typeChar()
     */
    char typeChar() {return type;}

    /**
     * MISSINGDOC: cWatch:void*pointer()
     */
    void *pointer() {return ptr;}
};

#endif
