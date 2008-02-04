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
#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedsyntaxvalidator.h"
#include "nedsemanticvalidator.h"

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
#include "fileglobber.h"

USING_NAMESPACE

cNEDLoader *cNEDLoader::instance_;

cNEDLoader *cNEDLoader::instance()
{
    if (!instance_)
        instance_ = new cNEDLoader();
    return instance_;
}

void cNEDLoader::clear()
{
    delete instance_;
    instance_ = NULL;
}

cNEDLoader::cNEDLoader()
{
    registerBuiltinDeclarations();
}

void cNEDLoader::registerBuiltinDeclarations()
{
    // NED code to define built-in types
    const char *nedcode = NEDParser::getBuiltInDeclarations();

    NEDErrorStore errors;
    NEDParser parser(&errors);
    NEDElement *tree = parser.parseNEDText(nedcode);
    if (errors.containsError())
    {
        delete tree;
        throw cRuntimeError("error during parsing of internal NED declarations");
    }
    //FIXME check errors; run validation perhaps, etc!

    try
    {
        // note: file must be called package.ned so that @namespace("") takes effect
        addFile("/[builtin-declarations]/package.ned", tree);
    }
    catch (NEDException& e)
    {
        throw cRuntimeError("NED error: %s", e.what()); // FIXME or something
    }
}

void cNEDLoader::addNedType(const char *qname, NEDElement *node)
{
    // we'll process it later, from done()
    pendingList.push_back(PendingNedType(qname,node));
}

cNEDDeclaration *cNEDLoader::getDecl(const char *qname) const
{
    cNEDDeclaration *decl = dynamic_cast<cNEDDeclaration *>(lookup(qname));
    if (!decl)
        throw cRuntimeError("NED declaration '%s' not found", qname);
    return decl;
}

NEDElement *cNEDLoader::parseAndValidateNedFile(const char *fname, bool isXML)
{
    // load file
    NEDElement *tree = 0;
    NEDErrorStore errors;
    errors.setPrintToStderr(true); //XXX
    if (isXML)
    {
        tree = parseXML(fname, &errors);
    }
    else
    {
        NEDParser parser(&errors);
        parser.setParseExpressions(true);
        parser.setStoreSource(false);
        tree = parser.parseNEDFile(fname);
    }
    if (errors.containsError())
    {
        delete tree;
        throw cRuntimeError("errors while loading or parsing file `%s'", fname);
    }

    // DTD validation and additional syntax validation
    NEDDTDValidator dtdvalidator(&errors);
    dtdvalidator.validate(tree);
    if (errors.containsError())
    {
        delete tree;
        throw cRuntimeError("errors during DTD validation of file `%s'", fname);
    }

    NEDSyntaxValidator syntaxvalidator(true, &errors);
    syntaxvalidator.validate(tree);
    if (errors.containsError())
    {
        delete tree;
        throw cRuntimeError("errors during validation of file `%s'", fname);
    }
    return tree;
}

void cNEDLoader::loadNedFile(const char *nedfname, bool isXML)
{
    if (getFile(nedfname))
        return;  // already loaded

    // parse file
    NEDElement *tree = parseAndValidateNedFile(nedfname, isXML);

    try
    {
        addFile(nedfname, tree);
    }
    catch (NEDException& e)
    {
        throw cRuntimeError("NED error: %s", e.what());
    }
}

int cNEDLoader::loadNedSourceFolder(const char *foldername)
{
    try
    {
        //FIXME somehow store what's the NED source folder with each file, so we can check if everything is in the right package?
        return doLoadNedSourceFolder(foldername);
    }
    catch (std::exception& e)
    {
        throw cRuntimeError("Error loading NED sources from `%s': %s", foldername, e.what());
    }
}

int cNEDLoader::doLoadNedSourceFolder(const char *foldername)
{
    PushDir pushDir(foldername);
    int count = 0;

    FileGlobber globber("*");
    const char *filename;
    while ((filename=globber.getNext())!=NULL)
    {
        if (filename[0] == '.')
        {
            continue;  // ignore ".", "..", and dotfiles
        }
        if (isDirectory(filename))
        {
            count += doLoadNedSourceFolder(filename);
        }
        else if (opp_stringendswith(filename, ".ned"))
        {
            loadNedFile(filename, false);
            count++;
        }
    }
    return count;
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

NEDLookupContext cNEDLoader::getParentContextOf(const char *qname, NEDElement *node)
{
    NEDElement *contextnode = node->getParent();
    if (contextnode->getTagCode()==NED_TYPES)
        contextnode = contextnode->getParent();
    const char *lastdot = strrchr(qname, '.');
    std::string contextqname = !lastdot ? "" : std::string(qname, lastdot-qname);
    return NEDLookupContext(contextnode, contextqname.c_str());
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

void cNEDLoader::done()
{
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
