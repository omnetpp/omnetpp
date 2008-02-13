//==========================================================================
// NEDTYPEINFO.H -
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


#ifndef __NEDTYPEINFO_H
#define __NEDTYPEINFO_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"

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
 * NEDTypeInfo stores:
 *  - a pointer to the complete NEDElement tree of the NED declaration;
 *    this pointer points into the NEDResourceCache.
 *  - parameter and gate descriptions extracted from the NEDElement trees,
 *    also following the inheritance chain. Inherited parameters and
 *    gates are included, and values (parameters and gate sizes) are
 *    converted into and stored in cPar form.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDTypeInfo
{
  public:
    enum Type {SIMPLE_MODULE, COMPOUND_MODULE, MODULEINTERFACE, CHANNEL, CHANNELINTERFACE};

  protected:
    // the resolver this type is in
    NEDResourceCache *resolver;

    std::string qualifiedName;
    NEDElement *tree;

    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string,int> StringToIntMap;

    Type type;

    // inheritance
    StringVector extendsnames;
    StringVector interfacenames;

    // simple module/channel C++ class to instantiate
    std::string implClassName;

  protected:
    // utility function
    static const char *getSingleValueLocalProperty(NEDElement *parent, const char *name);
    // utility function: XXX needed here? or move to subclass?
    NEDElement *getSubcomponentElement(const char *subcomponentName) const;

  public:
    /** Constructor. It expects fully qualified name */
    NEDTypeInfo(NEDResourceCache *resolver, const char *qname, NEDElement *tree);

    /** Destructor */
    virtual ~NEDTypeInfo();

    /** Returns the simple name of the NED type */
    virtual const char *name() const;

    /** Returns the fully qualified name of the NED type */
    virtual const char *fullName() const;

    /** Returns the raw NEDElement tree representing the component */
    virtual NEDElement *getTree() const;

    /** The NED type resolver this type is registered in */
    NEDResourceCache *getResolver() const  {return resolver;}

    /** @name Return one element from the tree */
    //@{
    virtual ParametersElement *getParametersElement() const;
    virtual GatesElement *getGatesElement() const;
    virtual SubmodulesElement *getSubmodulesElement() const;
    virtual ConnectionsElement *getConnectionsElement() const;
    //@}

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
     * Returns the number of "extends" names.
     */
    virtual int numExtendsNames() const  {return extendsnames.size();}

    /**
     * Returns the name of the kth "extends" name (k=0..numExtendsNames()-1),
     * resolved to fully qualified name.
     */
    virtual const char *extendsName(int k) const;

    /**
     * Returns the number of interfaces.
     */
    virtual int numInterfaceNames() const  {return interfacenames.size();}

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1),
     * resolved to fully qualified name.
     */
    virtual const char *interfaceName(int k) const;

    /**
     * Returns true if this NED type extends/"is like" the given module interface
     * or channel interface
     */
    virtual bool supportsInterface(const char *qname);

    /**
     * For simple modules and channels, it returns the name of the C++ class that
     * has to be instantiated; otherwise it returns NULL.
     */
    virtual const char *implementationClassName() const;

    /** The C++ namespace for this NED type (from @namespace() properties) */
    virtual std::string getCxxNamespace() const;

    /** Returns the first "extends" clause, or NULL */
    virtual NEDTypeInfo *getSuperDecl() const;
};

NAMESPACE_END


#endif

