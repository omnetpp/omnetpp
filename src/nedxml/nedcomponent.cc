//==========================================================================
// NEDNEDTYPEINFO.CC -
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

#include <stdio.h>
#include <string.h>
#include "nederror.h"
#include "nedcomponent.h"

USING_NAMESPACE


NEDTypeInfo::NEDTypeInfo(const char *qname, NEDElement *tree)
{
	this->qualifiedName = qname;
    this->tree = tree;
}

NEDTypeInfo::~NEDTypeInfo()
{
    // nothing -- we don't manage the tree, only cache a pointer to it
}

const char *NEDTypeInfo::name() const 
{
	const char *qname = fullName();
	const char *lastdot = strrchr(qname, '.');
	return !lastdot ? qname : lastdot + 1;
}

const char *NEDTypeInfo::fullName() const 
{
	return qualifiedName.c_str();
}

NEDElement *NEDTypeInfo::getTree() const
{
    return tree;
}




