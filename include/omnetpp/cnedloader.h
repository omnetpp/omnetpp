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

#include "cownedobject.h"

namespace omnetpp {

class cComponentType;

/**
 * @brief Dynamically loads NED files, and creates module/channel type objects.
 */
class SIM_API cINedLoader : public cNoncopyableOwnedObject
{
  public:
    /** Constructor */
    cINedLoader(const char *name=nullptr) : cNoncopyableOwnedObject(name) {}

    /** Destructor */
    virtual ~cINedLoader();

    /**
     * Sets the NED path loadNedFiles() will load from. The argument is a
     * semicolon-separated list of directories.
     */
    virtual void setNedPath(const char *nedPath) = 0;

    /**
     * Returns the NED path loadNedFiles() will load from, as a string
     * containing a semicolon-separated list.
     */
    virtual const char *getNedPath() const = 0;

    /**
     * Sets the list of NED packages to exclude by loadNedFiles(). The argument
     * is a semicolon-separated list of package names.
     */
    virtual void setNedExcludedPackages(const char *nedExcludedPackages) = 0;

    /**
     * Returns the list of NED packages to exclude by loadNedFiles(), as a
     * string containing a semicolon-separated list.
     */
    virtual const char *getNedExcludedPackages() const = 0;

    /**
     * Sets the NED path and the list of excluded packages via setNedPath() and
     * setNedExcludedPackages(), based on the options in the passed configuration
     * object and the values of the "-n" and "-x" command-line arguments.
     */
    virtual void configure(cConfiguration *cfg, const char *nArg, const char *xArg) = 0;

    /**
     * Load the NED files from the configured NED path ignoring the configured
     * excluded packages, as defined by getNedPath() and getNedExcludedPackages().
     * This translates to a series of loadNedSourceFolder() calls.
     */
    virtual void loadNedFiles() = 0;

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
    virtual void loadNedFile(const char *nedfname, const char *expectedPackage=nullptr, bool isXML=false) = 0;

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
    virtual void loadNedText(const char *name, const char *nedtext, const char *expectedPackage=nullptr, bool isXML=false) = 0;

    /**
     * Returns the list of NED source folders loaded by loadNedSourceFolder()
     * in their canonical forms.
     */
    virtual std::vector<std::string> getLoadedNedFolders() const = 0;

    /**
     * To be called after all NED folders / files have been loaded. May be
     * redefined to issue errors for components that could not be fully
     * resolved because of missing base types or interfaces.
     */
    virtual void doneLoadingNedFiles() = 0;

    /**
     * Returns the NED package that corresponds to the given folder. Returns ""
     * for the default package, and "-" if the folder is outside all NED folders.
     */
    virtual std::string getNedPackageForFolder(const char *folder) const = 0;

    /**
     * Allows a custom-made module or channel type object to be registered
     * in this NED loader.
     */
    virtual void registerComponentType(cComponentType *type) = 0;

    /**
     * Returns the module and channel type object with the given fully qualified name,
     * or nullptr if one does not exist.
     */
    virtual cComponentType *lookupComponentType(const char *qname) const = 0;

    /**
     * Returns the list of all module and channel types objects registered
     * in this NED loader.
     */
    virtual const std::vector<cComponentType*>& getComponentTypes() const = 0;
};

}  // namespace omnetpp


#endif

