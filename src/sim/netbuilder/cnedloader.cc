//==========================================================================
// CNEDLOADER.CC -
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

#include "cnedloader.h"
#include "nedelements.h"
#include "nederror.h"

#include "cproperty.h"
#include "cproperties.h"
#include "ccomponenttype.h"
#include "cexception.h"
#include "cenvir.h"
#include "cexpressionbuilder.h"
#include "cpar.h"
#include "clongpar.h"
#include "cdoublepar.h"
#include "globals.h"
#include "cdynamicmoduletype.h"
#include "cdynamicchanneltype.h"
#include "cdisplaystring.h"

#include "stringutil.h"
#include "fileutil.h"

USING_NAMESPACE

cNEDLoader *cNEDLoader::instance_;

cNEDLoader *cNEDLoader::instance()
{
    if (!instance_) {
        instance_ = new cNEDLoader();
        instance_->registerBuiltinDeclarations();
    }
    return instance_;
}

void cNEDLoader::clear()
{
    delete instance_;
    instance_ = NULL;
}

void cNEDLoader::addNedType(const char *qname, NEDElement *node)
{
    // we'll process it later, from doneLoadingNedFiles()
    pendingList.push_back(PendingNedType(qname,node));
}

cNEDDeclaration *cNEDLoader::getDecl(const char *qname) const
{
    cNEDDeclaration *decl = dynamic_cast<cNEDDeclaration *>(lookup(qname));
    if (!decl)
        throw cRuntimeError("NED declaration '%s' not found", qname);
    return decl;
}

bool cNEDLoader::areDependenciesResolved(const char *qname, NEDElement *node)
{
    // check that all base types are resolved
    NEDLookupContext context = getParentContextOf(qname, node);
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()!=NED_EXTENDS && child->getTagCode()!=NED_INTERFACE_NAME)
            continue;

        const char *name = child->getAttribute("name");
        std::string qname = resolveNedType(context, name);
        if (qname.empty())
            return false;
    }
    return true;
}

void cNEDLoader::registerNedTypes()
{
    bool again = true;
    while (again)
    {
        again = false;
        for (int i=0; i<(int)pendingList.size(); i++)
        {
            PendingNedType type = pendingList[i];
            if (areDependenciesResolved(type.qname.c_str(), type.node))
            {
                registerNedType(type.qname.c_str(), type.node);
                pendingList.erase(pendingList.begin() + i--);
                again = true;
            }
        }
    }
}

void cNEDLoader::registerNedType(const char *qname, NEDElement *node)
{
    // Note: base class (nedxml's NEDResourceCache) has already checked for duplicates, no need here
    cNEDDeclaration *decl = new cNEDDeclaration(qname, node);
    nedTypes[qname] = decl;

    // if module or channel, register corresponding object which can be used to instantiate it
    cComponentType *type = NULL;
    if (node->getTagCode()==NED_SIMPLE_MODULE || node->getTagCode()==NED_COMPOUND_MODULE)
        type = new cDynamicModuleType(qname);
    else if (node->getTagCode()==NED_CHANNEL)
        type = new cDynamicChannelType(qname);
    if (type)
        componentTypes.instance()->add(type);
}

void cNEDLoader::doneLoadingNedFiles()
{
    NEDResourceCache::doneLoadingNedFiles();
    
    // register NED types from all the files we've loaded
    registerNedTypes();

    for (int i=0; i<(int)pendingList.size(); i++)
    {
        PendingNedType type = pendingList[i];
        ev.printfmsg("WARNING: Type `%s' at %s could not be fully resolved, dropped (base type or interface missing)",
                     type.node->getAttribute("name"), type.node->getSourceLocation()); // FIXME create an ev.warning() or something...
        delete type.node;
    }
}


