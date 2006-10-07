//=========================================================================
//  NODETYPE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "util.h"
#include "nodetype.h"


void NodeType::checkAttrNames(const StringMap& attrs) const
{
    StringMap allowedAttrs;
    getAttributes(allowedAttrs);

    // are there illegal attributes?
    for (StringMap::const_iterator it=attrs.begin(); it!=attrs.end(); ++it)
    {
        const char *attr = it->first.c_str();
        StringMap::iterator j = allowedAttrs.find(attr);
        if (j==allowedAttrs.end())
            throw new Exception("illegal attribute `%s'", attr);
    }

    // do we have all attributes?
    for (StringMap::const_iterator i=allowedAttrs.begin(); i!=allowedAttrs.end(); ++i)
    {
        const char *attr = i->first.c_str();
        StringMap::const_iterator it = attrs.find(attr);
        if (it==attrs.end())
            throw new Exception("missing attribute `%s'", attr);
    }
}


