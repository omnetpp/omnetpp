//=========================================================================
//  CCLASSFACTORY.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include "cclassfactory.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#include "parsim/cplaceholdermod.h"
#endif


cClassFactory::cClassFactory(const char *name, cPolymorphic *(*f)()) : cNoncopyableObject(name)
{
    creatorfunc = f;
}

cPolymorphic *createOne(const char *classname)
{
    cClassFactory *p = (cClassFactory *)classes.instance()->get(classname);
    if (!p)
        throw new cRuntimeError("Class \"%s\" not found -- perhaps its code was not linked in, or the class wasn't registered via Register_Class()", classname);
    return p->createOne();
}

cPolymorphic *createOneIfClassIsKnown(const char *classname)
{
    cClassFactory *p = (cClassFactory *)classes.instance()->get(classname);
    if (!p)
        return NULL;
    return p->createOne();
}

