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
  Copyright (C) 1992,99 Andras Varga
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
// cWatch : shell for a char/int/long/double/char*/cObject* variable
//   The snapshot() call outputs state of simulation into a text file,
//   which is excellent for debugging. Unfortunately, ordinary variables
//   (int, char etc) do not appear in the snapshot file.
//   cWatch helps this. Use it like this:
//       int count;
//       cWatch dontcare( "count", count );
//   or
//       new cWatch( "count", count );
//   Now, the cWatch object will make the count variable appear
//   in the snapshot file.
//   The above lines can be shortened using the WATCH/LWATCH macro:
//       WATCH( count );
//   or
//       LWATCH( count );
//
class cWatch : public cObject
{
    private:
        void *ptr;
        char type;
    public:
        cWatch(cWatch& vs);
        cWatch(char *namestr, char& c)  : cObject(namestr) {ptr=&c; type='c';}
        cWatch(char *namestr, int&  i)  : cObject(namestr) {ptr=&i; type='i';}
        cWatch(char *namestr, long& l)  : cObject(namestr) {ptr=&l; type='l';}
        cWatch(char *namestr, double& d): cObject(namestr) {ptr=&d; type='d';}
        cWatch(char *namestr, char* &s) : cObject(namestr) {ptr=&s; type='s';}
        cWatch(char *namestr, cObject* &o) : cObject(namestr) {ptr=&o; type='o';}
        virtual char *isA()   {return "cWatch";}
        virtual cObject *dup()   {return new cWatch(*this);}
        virtual void info(char *buf);
        virtual char *inspectorFactoryName() {return "cWatchIFC";}
        cWatch& operator=(cWatch& vs)
              {ptr=vs.ptr;type=vs.type;return *this;}
        virtual void writeContents(ostream& os);

        virtual void printTo(char *s);
        char typeChar() {return type;}
        void *pointer() {return ptr;}
        //virtual int netPack();
        //virtual int netUnpack();
};

#endif
