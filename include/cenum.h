//==========================================================================
//   CENUM.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cEnum : effective integer-to-string mapping
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENUM_H
#define __CENUM_H

#include "cobject.h"

//=== classes declared here
class  cEnum;

//==========================================================================

/**
 * Defines string representation for enums. The class basically implements
 * effective integer-to-string mapping. Primary usage is to support displaying
 * symbolic names for integer values that represent some code
 * (such as an enum or #define). To be used mostly from Tkenv and possibly
 * other user interfaces.
 */
class cEnum : public cObject
{
   private:
	struct sEnum {
	    int key;
	    const char *string;
	};
        sEnum *vect;      // vector of objects
        int size;         // size of vector; always prime
        int items;        // number if items in the vector
   public:
	cEnum(cEnum& cenum);
        cEnum(const char *name=NULL, int siz=17);
        virtual ~cEnum();

        // redefined functions
        virtual const char *className()  {return "cEnum";}
        virtual cObject *dup()  {return new cEnum(*this);}
        virtual void info(char *buf);
        cEnum& operator=(cEnum& list);

        virtual int netPack();
        virtual int netUnpack();

        // new functions
        // add an item; if exist, overwrite
        void insert(int key, const char *str);
        // look up key; null ptr if not found
        const char *stringFor(int key);
        // look up string; if not found, returns second argument (or -1)
        int lookup(const char *str, int fallback=-1);
};

//==========================================================================

/**
 * Supporting class for cEnum. Helps with creating and filling globally
 * available cEnums. Enums of the same name will be merged.
 *
 * Usage:
 *  <PRE><TT>
 *   enum PDUType {CONNECT_REQ, CONNECT_CONFIRM, ... };
 *
 *   static sEnumBuilder _enum1( "PDUType",
 *       CONNECT_REQ,     "CONNECT_REQ",
 *       CONNECT_CONFIRM, "CONNECT_CONFIRM",
 *       ...
 *       0, NULL);
 *  </TT></PRE>
 */
class sEnumBuilder
{
    public:
        sEnumBuilder(const char *name, ...);
};

#endif

