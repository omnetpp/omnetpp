//==========================================================================
// CNEDLOADER.H -
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


#ifndef __OMNETPP_NETBUILDER_CNEDLOADER_H
#define __OMNETPP_NETBUILDER_CNEDLOADER_H

#include <map>
#include "omnetpp/cnedloader.h"
#include "nedxml/nedresourcecache.h"
#include "nedxml/astnode.h"
#include "cneddeclaration.h"

namespace omnetpp {

/**
 * @brief Stores dynamically loaded NED files, and one can look up NED declarations
 * of modules, channels, module interfaces and channel interfaces in them.
 * NED declarations are wrapped in cNedDeclaration objects, which
 * point back into the NedElement trees of the loaded NED files.
 *
 * This cNedLoader class extends nedxml's NedResourceCache, and
 * cNedDeclaration extends nexml's corresponding NedTypeInfo.
 */
class SIM_API cNedLoader : public cINedLoader, public nedxml::NedResourceCache
{
  public:
    typedef nedxml::NedElement NedElement;
    typedef nedxml::NedResourceCache NedResourceCache;

    class ExprRef
    {
      public:
        typedef nedxml::NedElement NedElement;
      private:
        NedElement *node;
        int attrId;
        const char *cachedExprText;
      public:
        ExprRef(NedElement *node, int attrId) : node(node), attrId(attrId) {
            cachedExprText = node->getAttribute(attrId);
        }
        bool empty() const {return !cachedExprText[0];}
        const char *getExprText() const {return cachedExprText;}
        NedElement *getNode() const {return node;}
        bool operator<(const ExprRef& other) const {return node!=other.node ? node<other.node : attrId<other.attrId;}
    };

  protected:
    // from the configuration
    std::string nedPath;
    std::string nedExcludedPackages;

    // types
    std::vector<cComponentType *> types;  // for fast iteration
    std::map<std::string,cComponentType*> typesByQName;

    // expression cache
    std::map<ExprRef, cDynamicExpression*> cachedExpresssions;

  protected:
    // reimplemented to create specialized type info
    virtual cNedDeclaration *createTypeInfo(const char *qname, bool isInnerType, NedElement *node) override;

    // reimplemented so that we can add cModuleType/cChannelType
    virtual void registerNedType(const char *qname, bool isInnerType, NedElement *node) override;
    virtual void loadEmbeddedNedFiles();
    virtual std::string extractNedPath(cConfiguration *cfg, const char *nArg);
    virtual std::string extractNedExcludedPackages(cConfiguration *cfg, const char *xArg);

  public:
    /** Constructor */
    cNedLoader(const char *name = nullptr);

    /** Destructor */
    virtual ~cNedLoader();

    /** Iterate over component types */
    virtual void forEachChild(cVisitor *v) override;

    const char *getNedExcludedPackages() const override {return nedExcludedPackages.c_str();}
    void setNedExcludedPackages(const char *nedExcludedPackages) override {this->nedExcludedPackages = nedExcludedPackages;}
    const char *getNedPath() const override {return nedPath.c_str();}
    void setNedPath(const char *nedPath) override {this->nedPath = nedPath;}

    virtual void configure(cConfiguration *cfg, const char *nArg, const char *xArg) override;
    virtual void loadNedFiles() override;

    /** @name Implementation of the cINedLoader interface. */
    //@{
    virtual int loadNedSourceFolder(const char *foldername, const char *excludedPackages) override {return nedxml::NedResourceCache::loadNedSourceFolder(foldername,excludedPackages);}
    virtual void loadNedFile(const char *nedfname, const char *expectedPackage=nullptr, bool isXML=false) override {nedxml::NedResourceCache::loadNedFile(nedfname,expectedPackage,isXML);}
    virtual void loadNedText(const char *name, const char *nedtext, const char *expectedPackage=nullptr, bool isXML=false) override {nedxml::NedResourceCache::loadNedText(name,nedtext,expectedPackage,isXML);}
    virtual std::vector<std::string> getLoadedNedFolders() const override {return nedxml::NedResourceCache::getLoadedNedFolders();}
    virtual void checkLoadedTypes() override {nedxml::NedResourceCache::checkLoadedTypes();}
    virtual std::string getNedPackageForFolder(const char *folder) const override {return nedxml::NedResourceCache::getNedPackageForFolder(folder);}
    virtual void registerComponentType(cComponentType *type) override;
    virtual cComponentType *lookupComponentType(const char *qname) const override;
    virtual const std::vector<cComponentType*>& getComponentTypes() const override {return types;}
    //@}

    /** @name Methods for use by the module/channel types created by this NED loader. */
    //@{
    /** Returns the NED declaration for the given type. */
    virtual cNedDeclaration *getDecl(const char *qname) override;

    /** Compile NED expression (which occurs in given attribute of the given node) to a cDynamicExpression, and cache it */
    virtual cDynamicExpression *getCompiledExpression(const ExprRef& expr);
    //@}
};

}  // namespace omnetpp

#endif

