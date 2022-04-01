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


#ifndef __OMNETPP_CNEDLOADER_H
#define __OMNETPP_CNEDLOADER_H

#include <map>
#include "nedxml/nedresourcecache.h"
#include "nedxml/astnode.h"
#include "omnetpp/simkerneldefs.h"
#include "cneddeclaration.h"

namespace omnetpp {

/**
 * @brief Dynamically loads NED files, and creates module/channel type objects.
 */
class SIM_API cINedLoader : public cNoncopyableOwnedObject
{
  public:
    /** Constructor */
    cINedLoader() {}

    /** Destructor */
    virtual ~cINedLoader();

    /**
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * A list of packages to skip may be specified in the excludedPackages parameter
     * (items must be separated with a semicolon).
     *
     * The function returns the number of NED files loaded.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    virtual int loadNedSourceFolder(const char *foldername, const char *excludedPackages) = 0;

    /**
     * Load a single NED file. If the expected package is given (non-nullptr),
     * it should match the package declaration inside the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    virtual void loadNedFile(const char *nedfname, const char *expectedPackage, bool isXML) = 0;

    /**
     * Parses and loads the NED source code passed in the nedtext argument.
     * The name argument will be used as filename in error messages, and
     * and should be unique among the files loaded. If the expected package
     * is given (non-nullptr), it should match the package declaration inside
     * the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    virtual void loadNedText(const char *name, const char *nedtext, const char *expectedPackage, bool isXML) = 0;

    /**
     * To be called after all NED folders / files have been loaded. May be
     * redefined to issue errors for components that could not be fully
     * resolved because of missing base types or interfaces.
     */
    virtual void doneLoadingNedFiles() = 0;
};

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
    // expression cache
    std::map<ExprRef, cDynamicExpression*> cachedExpresssions;

  protected:
    // reimplemented to create specialized type info
    virtual cNedDeclaration *createTypeInfo(const char *qname, bool isInnerType, NedElement *node) override;

    // reimplemented so that we can add cModuleType/cChannelType
    virtual void registerNedType(const char *qname, bool isInnerType, NedElement *node) override;

  public:
    /** Constructor */
    cNedLoader() = default;

    /** Destructor */
    virtual ~cNedLoader();

    /** @name NED loading methods. */
    //@{
    virtual int loadNedSourceFolder(const char *foldername, const char *excludedPackages) override {return nedxml::NedResourceCache::loadNedSourceFolder(foldername,excludedPackages);}
    virtual void loadNedFile(const char *nedfname, const char *expectedPackage, bool isXML) override {nedxml::NedResourceCache::loadNedFile(nedfname,expectedPackage,isXML);}
    virtual void loadNedText(const char *name, const char *nedtext, const char *expectedPackage, bool isXML) override {nedxml::NedResourceCache::loadNedText(name,nedtext,expectedPackage,isXML);}
    virtual void doneLoadingNedFiles() override {nedxml::NedResourceCache::doneLoadingNedFiles();}
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

