//==========================================================================
// CNEDLOADER.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "common/fileutil.h"
#include "common/stringtokenizer.h"
#include "nedxml/nedelements.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/globals.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cdynamicexpression.h"
#include "cnedloader.h"
#include "nedxml/errorstore.h"
#include "cdynamicmoduletype.h"
#include "cdynamicchanneltype.h"

using namespace omnetpp::nedxml;
using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

Register_GlobalConfigOption(CFGID_NED_PATH, "ned-path", CFG_PATH, "", "A semicolon-separated list of directories. The directories will be regarded as roots of the NED package hierarchy, and all NED files will be loaded from their subdirectory trees. This option is normally left empty, as the OMNeT++ IDE sets the NED path automatically, and for simulations started outside the IDE it is more convenient to specify it via command-line option (-n) or via environment variable (OMNETPP_NED_PATH, NEDPATH).");
Register_GlobalConfigOption(CFGID_NED_PACKAGE_EXCLUSIONS, "ned-package-exclusions", CFG_CUSTOM, "", "A semicolon-separated list of NED packages to be excluded when loading NED files. Sub-packages of excluded ones are also excluded. Additional items may be specified via the `-x` command-line option and the `OMNETPP_NED_PACKAGE_EXCLUSIONS` environment variable.");

#define LOCK   std::lock_guard<std::recursive_mutex> guard(NedResourceCache::nedMutex)

cINedLoader::~cINedLoader()
{
}

cNedLoader::cNedLoader(const char *name) : cINedLoader(name)
{
    loadEmbeddedNedFiles();
}

cNedLoader::~cNedLoader()
{
    for (auto it : cachedExpresssions)
        delete it.second;

    // delete module/channel types
    for (cComponentType *type : types)
        dropAndDelete(type);
    types.clear();
    typesByQName.clear();
}

void cNedLoader::forEachChild(cVisitor *v)
{
    LOCK;
    for (cComponentType *type : types)
        if (!v->visit(type))
            return;
}

void cNedLoader::loadEmbeddedNedFiles()
{
    LOCK;
    // TODO if (verbose) out << "Loading embedded NED files: " << embeddedNedFiles.size() << endl;
    for (const auto& file : embeddedNedFiles) {
        std::string nedText = file.nedText;
        if (!file.garblephrase.empty())
            nedText = opp_ungarble(file.nedText, file.garblephrase);
        loadNedText(file.fileName.c_str(), nedText.c_str());
    }
}

void cNedLoader::configure(cConfiguration *cfg, const char *nArg, const char *xArg)
{
    LOCK;
    setNedPath(extractNedPath(cfg, nArg).c_str());
    setNedExcludedPackages(extractNedExcludedPackages(cfg, xArg).c_str());
}

std::string cNedLoader::extractNedPath(cConfiguration *cfg, const char *nArg)
{
    LOCK;
    // NED path. It is taken from the "-n" command-line options, the OMNETPP_NED_PATH
    // environment variable, and the "ned-path=" config option. If the result is still
    // empty, we fall back to "." -- this is needed for single-directory models to work.
    std::string nedPath = nArg;
    nedPath = opp_join(";", nedPath, cfg->getAsPath(CFGID_NED_PATH));
    nedPath = opp_join(";", nedPath, opp_nulltoempty(getenv("OMNETPP_NED_PATH")));
    nedPath = opp_join(";", nedPath, opp_nulltoempty(getenv("NEDPATH")));
    if (nedPath.empty())
        nedPath = ".";
    return nedPath;
}

std::string cNedLoader::extractNedExcludedPackages(cConfiguration *cfg, const char *xArg)
{
    LOCK;
    std::string nedExcludedPackages = xArg;
    nedExcludedPackages = opp_join(";", nedExcludedPackages, opp_nulltoempty(cfg->getAsCustom(CFGID_NED_PACKAGE_EXCLUSIONS)));
    nedExcludedPackages = opp_join(";", nedExcludedPackages, opp_nulltoempty(getenv("OMNETPP_NED_PACKAGE_EXCLUSIONS")));
    return nedExcludedPackages;
}

void cNedLoader::loadNedFiles()
{
    LOCK;
    // load NED files from folders on the NED path
    std::set<std::string> foldersLoaded;
    for (std::string folder : opp_splitpath(nedPath)) {
        if (foldersLoaded.find(folder) == foldersLoaded.end()) {
//TODO verbose mode log:
//            if (opt->verbose)
//                out << "Loading NED files from " << folder << ": ";
            int count = loadNedSourceFolder(folder.c_str(), nedExcludedPackages.c_str());
//            if (opt->verbose)
//                out << " " << count << endl;
            foldersLoaded.insert(folder);
        }
    }
    checkLoadedTypes();
}

cNedDeclaration *cNedLoader::createTypeInfo(const char *qname, bool isInnerType, ASTNode *node)
{
    return new cNedDeclaration(this, qname, isInnerType, node);
}

void cNedLoader::registerNedType(const char *qname, bool isInnerType, NedElement *node)
{
    LOCK;
    NedResourceCache::registerNedType(qname, isInnerType, node);

    // if module or channel, register corresponding object which can be used to instantiate it
    cComponentType *type = nullptr;
    if (node->getTagCode() == NED_SIMPLE_MODULE || node->getTagCode() == NED_COMPOUND_MODULE)
        type = new cDynamicModuleType(this, qname);
    else if (node->getTagCode() == NED_CHANNEL)
        type = new cDynamicChannelType(this, qname);

    if (type)
        registerComponentType(type);
}

void cNedLoader::registerComponentType(cComponentType *type)
{
    LOCK;
    take(type);
    types.push_back(type);
    typesByQName[type->getFullName()] = type;
}

cNedDeclaration *cNedLoader::getDecl(const char *qname) const
{
    cNedDeclaration *decl = dynamic_cast<cNedDeclaration *>(NedResourceCache::getDecl(qname));
    ASSERT(decl);
    return decl;
}

cComponentType *cNedLoader::lookupComponentType(const char *qname) const
{
    LOCK;
    auto it = typesByQName.find(qname);
    return it == typesByQName.end() ? nullptr : it->second;
}

cDynamicExpression *cNedLoader::getCompiledExpression(const ExprRef& key)
{
    LOCK;
    auto it = cachedExpresssions.find(key);
    if (it != cachedExpresssions.end())
        return it->second;
    cDynamicExpression *expr = new cDynamicExpression();
    expr->parseNedExpr(key.getExprText());
    cachedExpresssions[key] = expr;
    return expr;
}

}  // namespace omnetpp

