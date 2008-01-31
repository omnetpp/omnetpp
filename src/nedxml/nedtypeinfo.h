//==========================================================================
// NEDTYPEINFO.H -
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


#ifndef __NEDTYPEINFO_H
#define __NEDTYPEINFO_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"

NAMESPACE_BEGIN


/**
 * Wraps a NEDElement tree of a NED declaration (module, channel, module
 * interface or channel interface), or declaration in a msg file (enum,
 * class, struct). May be extended by subclassing.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDTypeInfo
{
  protected:
    std::string qualifiedName;
    NEDElement *tree;

  public:
    /** Constructor. It expects fully qualified name */
    NEDTypeInfo(const char *qname, NEDElement *tree);

    /** Destructor */
    virtual ~NEDTypeInfo();

    /** Returns the simple name of the NED type */
    virtual const char *name() const;

    /** Returns the fully qualified name of the NED type */
    virtual const char *fullName() const;

    /** Returns the raw NEDElement tree representing the component */
    virtual NEDElement *getTree() const;
};

NAMESPACE_END


#endif

