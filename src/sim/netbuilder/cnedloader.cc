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
        throw cRuntimeError("errors while loading or parsing file `%s'", fname);  //FIXME these errors print relative path????
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

void cNEDLoader::doLoadNedFile(const char *nedfname, const char *expectedPackage, bool isXML)
{
    if (getFile(nedfname))
        return;  // already loaded

    // parse file
    NEDElement *tree = parseAndValidateNedFile(nedfname, isXML);

    // check that declared package matches expected package
    PackageNode *packageDecl = (PackageNode *)tree->getFirstChildWithTag(NED_PACKAGE);
    std::string declaredPackage = packageDecl ? packageDecl->getName() : "";
    if (declaredPackage != expectedPackage)
        throw cRuntimeError("NED error in file `%s': declared package `%s' does not match expected package `%s'",
                            nedfname, declaredPackage.c_str(), expectedPackage);  //FIXME fname misses directory here

    // register it
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
        std::string canonicalFolderName = tidyFilename(toAbsolutePath(foldername).c_str(), true);
        std::string rootPackageName = determineRootPackageName(foldername);
        folderPackages[canonicalFolderName] = rootPackageName;
        return doLoadNedSourceFolder(foldername, rootPackageName.c_str());
    }
    catch (std::exception& e)
    {
        throw cRuntimeError("Error loading NED sources from `%s': %s", foldername, e.what());
    }
}

void cNEDLoader::loadNedFile(const char *nedfname, const char *expectedPackage, bool isXML)
{
    //FIXME revise this, and compare with documentation!!!!!!!!
    //FIXME potentially change it so that one needs to call doneLoadingNedFiles() after it (but then mutually dependent files can be loaded too)
    doLoadNedFile(nedfname, expectedPackage, isXML);
    doneLoadingNedFiles();
}

std::string cNEDLoader::determineRootPackageName(const char *foldername)
{
    // determine if a package.ned file exists
    std::string packageNedFilename = std::string(foldername) + "/package.ned";
    FILE *f = fopen(packageNedFilename.c_str(), "r");
    if (!f)
        return "";
    fclose(f);

    // read package declaration from it
    NEDElement *tree = parseAndValidateNedFile(packageNedFilename.c_str(), false);
    ASSERT(tree);
    PackageNode *packageDecl = (PackageNode *)tree->getFirstChildWithTag(NED_PACKAGE);
    std::string result = packageDecl ? packageDecl->getName() : "";
    delete tree;
    return result;
}

int cNEDLoader::doLoadNedSourceFolder(const char *foldername, const char *expectedPackage)
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
            count += doLoadNedSourceFolder(filename, opp_join(".", expectedPackage, filename).c_str());
        }
        else if (opp_stringendswith(filename, ".ned"))
        {
            doLoadNedFile(filename, expectedPackage, false);
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

void cNEDLoader::doneLoadingNedFiles()
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

std::string cNEDLoader::getNedPackageForFolder(const char *folder) const
{
    std::string folderName = tidyFilename(toAbsolutePath(folder).c_str(), true);
    for (StringMap::const_iterator it = folderPackages.begin(); it!=folderPackages.end(); ++it)
    {
        if (opp_stringbeginswith(folderName.c_str(), it->first.c_str()))
        {
            std::string suffix = folderName.substr(it->first.size());
            std::string subpackage = opp_replacesubstring(suffix.c_str(), "/", ".", true);
            return opp_join(".", it->second.c_str(), subpackage.c_str());
        }
    }
    return "-";
}

