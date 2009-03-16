//==========================================================================
//  JUTIL.CC - part of
//
//                    OMNeT++/OMNEST
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "jutil.h"
#include "ccomponent.h"
#include "ccomponenttype.h"

NAMESPACE_BEGIN

#define TRY2(CODE)  try { CODE; } catch (std::exception& e) {printf("<!> Warning: %s\n", e.what());}

const char *getObjectShortTypeName(cObject *object)
{
    if (dynamic_cast<cComponent*>(object))
        TRY2( return ((cComponent*)object)->getComponentType()->getName() );
    return object->getClassName();
}

const char *getObjectFullTypeName(cObject *object)
{
    if (dynamic_cast<cComponent*>(object))
        TRY2( return ((cComponent*)object)->getComponentType()->getFullName() );
    return object->getClassName();
}

NAMESPACE_END


