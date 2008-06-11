//==========================================================================
// NEDTYPEINFO.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDTYPEINFO2_H
#define __NEDTYPEINFO2_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"
#include "commonutil.h"

NAMESPACE_BEGIN

class NEDResourceCache;

/**
 * Wraps a NEDElement tree of a NED declaration (module, channel, module
 * interface or channel interface), or declaration in a msg file (enum,
 * class, struct). May be extended by subclassing.
 *
 * Represents NED declarations of modules, module interfaces,
 * channels and channel interfaces. All instances are created and managed
 * by NEDResourceCache.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDTypeInfo2
{
  public:
    enum Type {SIMPLE_MODULE, COMPOUND_MODULE, MODULEINTERFACE, CHANNEL, CHANNELINTERFACE};

  protected:
    //XXX currently unused; TBD use the next ones for network building and validation
    class NEDElementMap
    {
      private:
        std::map<std::string,NEDElement*> map;
        std::vector<std::string> keys;
        std::vector<NEDElement *> values;
      public:
        bool contains(const std::string& key) const {return map.count(key)!=0;}
        NEDElement *get(const std::string& key) const
            {std::map<std::string,NEDElement*>::const_iterator it = map.find(key); return it==map.end() ? NULL : it->second;}
        void put(const std::string& key, NEDElement *node)
            {Assert(!contains(key)); map[key]=node; keys.push_back(key); values.push_back(node);}
        int size() const {return keys.size();}
        const std::string& key(int i) const {return keys[i];}
        NEDElement *value(int i) const {return values[i];}
        void putAll(const NEDElementMap& map)
            {for (int i=0; i<map.size(); i++) put(map.key(i),map.value(i));}
    };

    // the resolver this type is in
    NEDResourceCache *resolver;

    Type type;
    std::string qualifiedName;
    NEDElement *tree; // points into resolver

    // simple module/channel C++ class to instantiate
    std::string implClassName;

    // inheritance. String key is fully qualified name.
    NEDElementMap localExtendsNamesMap;

    NEDElementMap localInterfacesMap;
    NEDElementMap localParamDeclsMap;
    NEDElementMap localParamValuesMap;
    NEDElementMap localPropertiesMap;
    NEDElementMap localGateDeclsMap;
    NEDElementMap localGateSizesMap;
    NEDElementMap localInnerTypesMap;
    NEDElementMap localSubmodulesMap;
    NEDElementMap localMembersMap;  // union of all above

    NEDElementMap allInterfacesMap;
    NEDElementMap allParamDeclsMap;
    NEDElementMap allParamValuesMap;
    NEDElementMap allPropertiesMap;
    NEDElementMap allGateDeclsMap;
    NEDElementMap allGateSizesMap;
    NEDElementMap allInnerTypesMap;
    NEDElementMap allSubmodulesMap;
    NEDElementMap allMembersMap; // union of all above

  protected:
    NEDElement *buildFlattenedTree() const; //XXX mostly unused
    void mergeNEDType(NEDElement *basetree, const NEDElement *tree) const;
    void mergeProperties(NEDElement *basetree, const NEDElement *tree) const;
    void mergeProperty(PropertyElement *baseprop, const PropertyElement *prop) const;
    void mergePropertyKey(PropertyKeyElement *basekey, const PropertyKeyElement *key) const;

    NEDElement *getSubmoduleElement(const char *submoduleName) const;
    NEDElement *getConnectionElement(long id) const;

    void checkComplianceToInterface(NEDTypeInfo2 *interfaceDecl);

    void collect(NEDElementMap& map, int sectionTagCode, bool (*predicate)(NEDElement*));
    void addDeclarations(NEDElementMap& allSomethingMap, const NEDElementMap& localSomethingMap, const char *what);

    void collectLocalMembers();
    void collectAllMembers();

  public:
    /** Constructor. It expects fully qualified name */
    NEDTypeInfo2(NEDResourceCache *resolver, const char *qname, NEDElement *tree);

    /** Destructor */
    virtual ~NEDTypeInfo2();

    /** Returns the simple name of the NED type */
    virtual const char *getName() const;

    /** Returns the fully qualified name of the NED type */
    virtual const char *getFullName() const;

    /** Returns the raw NEDElement tree representing the component */
    virtual NEDElement *getTree() const;

    /** The NED type resolver this type is registered in */
    NEDResourceCache *getResolver() const  {return resolver;}

    /**
     * Returns the type of this declaration: simple module, compound module,
     * channel, etc.
     */
    virtual Type getType() const {return type;}

    /**
     * Returns the package name (from the package declaration of the containing
     * NED file
     */
    virtual std::string getPackage() const;

    /**
     * Returns a one-line summary (base class, implemented interfaces, etc)
     */
    virtual std::string info() const;

    /**
     * Returns the NED declaration.
     */
    virtual std::string nedSource() const;

    /**
     * Returns true if this NED type has a local (non-inherited)
     * \@network (or \@network(true)) property.
     */
    virtual bool isNetwork();

    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    virtual const char *implementationClassName() const;

    /** The C++ namespace for this NED type (from @namespace() properties) */
    virtual std::string getCxxNamespace() const;

    /** Returns the first "extends" clause, or NULL */
    virtual NEDTypeInfo2 *getSuperDecl() const;

    virtual bool supportsInterface(const char *qname) {return allInterfacesMap.contains(qname);}

    virtual const NEDElementMap& getLocalExtendsNames() const {return localExtendsNamesMap;}

    virtual const NEDElementMap& getLocalInterfaces() const {return localInterfacesMap;}
    virtual const NEDElementMap& getLocalParamDeclarations() const {return localParamDeclsMap;}
    virtual const NEDElementMap& getLocalParamAssignments() const {return localParamValuesMap;}
    virtual const NEDElementMap& getLocalGateDeclarations() const {return localGateDeclsMap;}
    virtual const NEDElementMap& getLocalGateSizes() const {return localGateSizesMap;}
    virtual const NEDElementMap& getLocalInnerTypes() const {return localInnerTypesMap;}
    virtual const NEDElementMap& getLocalSubmodules() const {return localSubmodulesMap;}
    virtual const NEDElementMap& getLocalMembers() const {return localMembersMap;}

    virtual const NEDElementMap& getInterfaces() const {return allInterfacesMap;}
    virtual const NEDElementMap& getParamDeclarations() const {return allParamDeclsMap;}
    virtual const NEDElementMap& getParamAssignments() const {return allParamValuesMap;}
    virtual const NEDElementMap& getGateDeclarations() const {return allGateDeclsMap;}
    virtual const NEDElementMap& getGateSizes() const {return allGateSizesMap;}
    virtual const NEDElementMap& getInnerTypes() const {return allInnerTypesMap;}
    virtual const NEDElementMap& getSubmodules() const {return allSubmodulesMap;}
    virtual const NEDElementMap& getMembers() const {return allMembersMap;}
};

NAMESPACE_END


#endif

