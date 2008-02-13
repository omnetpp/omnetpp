//==========================================================================
// CNEDLOADER.H -
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


#ifndef __CNEDLOADER_H
#define __CNEDLOADER_H

#include "simkerneldefs.h"
#include "nedresourcecache.h"
#include "cneddeclaration.h"

NAMESPACE_BEGIN

class cProperties;
class cProperty;

class NEDElement;
class ParamElement;
class GateElement;
class PropertyElement;



/**
 * Stores dynamically loaded NED files, and one can look up NED declarations
 * of modules, channels, module interfaces and channel interfaces in them.
 * NED declarations are wrapped in cNEDDeclaration objects, which
 * point back into the NEDElement trees of the loaded NED files.
 *
 * This cNEDLoader class extends nedxml's NEDResourceCache, and
 * cNEDDeclaration extends nexml's corresponding NEDTypeInfo.
 */
class SIM_API cNEDLoader : public NEDResourceCache
{
  public:
    class ComponentTypeNames : public INEDTypeNames {
      public:
        virtual bool contains(const char *qname) const  {return componentTypes.instance()->lookup(qname)!=NULL;}
        virtual int size() const  {return componentTypes.instance()->size();}
        virtual const char *get(int k) const  {return componentTypes.instance()->get(k)->fullName();}
    };

  protected:
    // the singleton instance
    static cNEDLoader *instance_;

    struct PendingNedType {
        std::string qname;
        NEDElement *node;
        PendingNedType(const char *q, NEDElement *e) {qname=q;node=e;}
    };

    // storage for NED components not resolved yet because of missing dependencies
    std::vector<PendingNedType> pendingList;

  protected:
    /** Redefined to return a cNEDDeclaration. */
    virtual void addNedType(const char *qname, NEDElement *node);

  protected:
    // utility functions
    virtual bool areDependenciesResolved(const char *qname, NEDElement *node);
    virtual void registerNedTypes();
    virtual void registerNedType(const char *qname, NEDElement *node);

    // constructor is protected, because we want only one instance
    cNEDLoader()  {}

  public:
    /** Access to the singleton instance */
    static cNEDLoader *instance();

    /** Disposes of the singleton instance */
    static void clear();

    /** Reimplemented from NEDResourceCache */
    virtual void doneLoadingNedFiles();

    /** Redefined to make return type more specific. */
    virtual cNEDDeclaration *getDecl(const char *qname) const;

    /**
     * Resolve a NED module/channel type name, for a submodule or channel
     * instance. Lookup is based on component names registered in the simkernel,
     * NOT on the NED files loaded. This allows the user to instantiate
     * cModuleTypes/cChannelTypes which are not declared in NED.
     */
    virtual std::string resolveComponentType(const NEDLookupContext& context, const char *nedtypename) {
        return NEDResourceCache::resolveNedType(context, nedtypename, &ComponentTypeNames());
    }
};

NAMESPACE_END


#endif

