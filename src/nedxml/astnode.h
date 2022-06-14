//==========================================================================
// astnode.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class ASTNode
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_ASTNODE_H
#define __OMNETPP_NEDXML_ASTNODE_H

#ifdef _MSC_VER
// disable 4996: VC8.0 warnings on Unix syscalls
#pragma warning(disable: 4996)
#endif

#include <string>
#include "nedxmldefs.h"
#include "common/pooledstring.h"

namespace omnetpp {
namespace nedxml {

using omnetpp::common::opp_staticpooledstring;

/**
 * @brief Subclass from this if you want to attach extra data to ASTNode objects.
 *
 * @ingroup Data
 */
class NEDXML_API UserData
{
  public:
    /** Constructor */
    UserData() {}

    /** Destructor */
    virtual ~UserData() {}
};


/**
 * @brief Stores a line:col..line:col region in a source file. Used for mapping
 * ASTNodes back to the source code.
 *
 * @ingroup Data
 */
struct SourceRegion
{
    int startLine = 0;
    int startColumn = 0;
    int endLine = 0;
    int endColumn = 0;
};

struct NEDXML_API FileLine
{
    opp_staticpooledstring file;
    int line = -1;

    FileLine() {}
    FileLine(const char *file, int line=-1) : file(file), line(line) {}
    bool empty() const {return file.empty();}
    std::string str() const {return empty() ? "" : line == -1 ? file.str() : file.str() + ":" + std::to_string(line);}
};

/**
 * Base class for objects in a NED object tree, the XML-based
 * in-memory representation for NED files. An instance of an ASTNode
 * subclass represent an XML element.
 * ASTNode provides a DOM-like, generic access to the tree;
 * subclasses additionally provide a typed interface.
 *
 * @ingroup Data
 */
class NEDXML_API ASTNode
{
  private:
    long id;
    FileLine srcLoc;
    opp_staticpooledstring directory;
    SourceRegion srcRegion;
    ASTNode *parent = nullptr;
    ASTNode *firstChild = nullptr;
    ASTNode *lastChild = nullptr;
    ASTNode *prevSibling = nullptr;
    ASTNode *nextSibling = nullptr;
    UserData *userData = nullptr;

    static long lastId;
    static long numCreated;
    static long numExisting;

  protected:
    static bool stringToBool(const char *s);
    static const char *boolToString(bool b);
    static int stringToEnum(const char *s, const char *vals[], int nums[], int n);
    static const char *enumToString(int b, const char *vals[], int nums[], int n);
    static void validateEnum(int b, const char *vals[], int nums[], int n);

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor
     */
    ASTNode();

    /**
     * Constructor. Takes parent element.
     */
    ASTNode(ASTNode *parent);

    /**
     * Destructor. Destroys children too.
     */
    virtual ~ASTNode();

    /**
     * Creates and returns a duplicate of the element. Child elements
     * are not copied.
     */
    virtual ASTNode *dup() const = 0;

    /**
     * Recursive version of dup(): duplicates the whole subtree.
     */
    virtual ASTNode *dupTree() const;
    //@}

    /** @name Common properties */
    //@{

    /**
     * Overridden in subclasses to return the name of the XML element the class
     * represents.
     */
    virtual const char *getTagName() const = 0;

    /**
     * Overridden in subclasses to return the numeric code (NED_xxx) of the
     * XML element the class represents.
     */
    virtual int getTagCode() const = 0;

    /**
     * Returns a unique id, originally set by the constructor.
     */
    virtual long getId() const;

    /**
     * The unique id assigned by the constructor can be overwritten here.
     */
    virtual void setId(long id);

    /**
     * Returns the file:line line position where this element originally came from.
     */
    virtual FileLine getSourceLocation() const;

    /**
     * Sets the file:line position indicating where this element originally came from.
     */
    virtual void setSourceLocation(FileLine loc);

    /**
     * Utility function based on setSourceLocation(). Needed by the IDE.
     */
    void setSourceLocation(const char *fileName, int lineNumber) {setSourceLocation(FileLine(fileName,lineNumber));}

    /**
     * Utility function based on getSourceLocation(). Needed by the IDE.
     */
    virtual const char *getSourceFileName() const;

    /**
     * Utility function based on getSourceLocation(). Needed by the IDE.
     */
    virtual int getSourceLineNumber() const;

    /**
     * Returns the directory this element was loaded from.
     */
    virtual const char *getSourceFileDirectory() const;

    /**
     * Returns the source region, containing a line:col region in the source file
     * that corresponds to this element.
     */
    virtual const SourceRegion& getSourceRegion() const;

    /**
     * Sets source region, containing a line:col region in the source file
     * that corresponds to this element. Called by the (NED/XML) parser.
     */
    virtual void setSourceRegion(const SourceRegion& region);
    //@}

    /** @name Generic access to attributes (Methods have to be redefined in subclasses!) */
    //@{

    /**
     * Sets every attribute to its default value (as returned by getAttributeDefault()).
     * Attributes without a default value are not affected.
     *
     * This method is called from the constructors of derived classes.
     */
    virtual void applyDefaults();

    /**
     * Pure virtual method, it should be redefined in subclasses to return
     * the number of attributes defined in the DTD.
     */
    virtual int getNumAttributes() const = 0;

    /**
     * Pure virtual method, it should be redefined in subclasses to return
     * the name of the kth attribute as defined in the DTD.
     *
     * It should return nullptr if k is out of range (i.e. negative or greater than
     * getNumAttributes()).
     */
    virtual const char *getAttributeName(int k) const = 0;

    /**
     * Returns the index of the given attribute. It returns -1 if the attribute
     * is not found. Relies on getNumAttributes() and getAttributeName().
     */
    virtual int lookupAttribute(const char *attr) const;

    /**
     * Pure virtual method, it should be redefined in subclasses to return
     * the value of the kth attribute (i.e. the attribute with the name
     * getAttributeName(k)).
     *
     * It should return nullptr if k is out of range (i.e. negative or greater than
     * getNumAttributes()).
     */
    virtual const char *getAttribute(int k) const = 0;

    /**
     * Returns the value of the attribute with the given name.
     * Relies on lookupAttribute() and getAttribute().
     *
     * It returns nullptr if the given attribute is not found.
     */
    virtual const char *getAttribute(const char *attr) const;

    /**
     * Pure virtual method, it should be redefined in subclasses to set
     * the value of the kth attribute (i.e. the attribute with the name
     * getAttributeName(k)).
     *
     * If k is out of range (i.e. negative or greater than getNumAttributes()),
     * the call should be ignored.
     */
    virtual void setAttribute(int k, const char *value) = 0;

    /**
     * Sets the value of the attribute with the given name.
     * Relies on lookupAttribute() and setAttribute().
     *
     * If the given attribute is not found, the call has no effect.
     */
    virtual void setAttribute(const char *attr, const char *value);

    /**
     * Pure virtual method, it should be redefined in subclasses to return
     * the default value of the kth attribute, as defined in the DTD.
     *
     * It should return nullptr if k is out of range (i.e. negative or greater than
     * getNumAttributes()), or if the attribute is \#REQUIRED; and return ""
     * if the attribute is \#IMPLIED.
     */
    virtual const char *getAttributeDefault(int k) const = 0;

    /**
     * Returns the default value of the given attribute, as defined in the DTD.
     * Relies on lookupAttribute() and getAttributeDefault().
     *
     * It returns nullptr if the given attribute is not found.
     */
    virtual const char *getAttributeDefault(const char *attr) const;
    //@}

    /** @name Generic access to children and siblings */
    //@{

    /**
     * Returns the parent element, or nullptr if this element has no parent.
     */
    virtual ASTNode *getParent() const;

    /**
     * Returns pointer to the first child element, or nullptr if this element
     * has no children.
     */
    virtual ASTNode *getFirstChild() const;

    /**
     * Returns pointer to the last child element, or nullptr if this element
     * has no children.
     */
    virtual ASTNode *getLastChild() const;

    /**
     * Returns pointer to the next sibling of this element (i.e. the next child
     * in the parent element). Returns nullptr if there're no subsequent elements.
     *
     * getFirstChild() and getNextSibling() can be used to loop through
     * the child list:
     *
     * <pre>
     * for (ASTNode *child=node->getFirstChild(); child; child = child->getNextSibling())
     * {
     *    ...
     * }
     * </pre>
     *
     */
    virtual ASTNode *getNextSibling() const;

    /**
     * Returns pointer to the previous sibling of this element (i.e. the previous child
     * in the parent element). Returns nullptr if there're no elements before this one.
     */
    virtual ASTNode *getPrevSibling() const;

    /**
     * Appends the given element at the end of the child element list.
     *
     * The node pointer passed should not be nullptr.
     */
    virtual void appendChild(ASTNode *node);

    /**
     * Inserts the given element just before the specified child element
     * in the child element list, or at the end of the child list if
     * nullptr is specified as the insert position.
     *
     * The where element must be a child of this element, or nullptr.
     * The node pointer passed should not be nullptr.
     */
    virtual void insertChildBefore(ASTNode *where, ASTNode *newnode);

    /**
     * Removes the given element from the child element list.
     *
     * The pointer passed should be a child of this element.
     */
    virtual ASTNode *removeChild(ASTNode *node);

    /**
     * Returns pointer to the first child element with the given tag code,
     * or nullptr if this element has no such children.
     */
    virtual ASTNode *getFirstChildWithTag(int tagcode) const;

    /**
     * Returns pointer to the next sibling of this element with the given
     * tag code. Returns nullptr if there's no such element.
     *
     * getFirstChildWithTag() and getNextSiblingWithTag() are a convenient way
     * to loop through elements with a certain tag code in the child list:
     *
     * <pre>
     * for (ASTNode *child=node->getFirstChildWithTag(tagcode); child; child = child->getNextSiblingWithTag(tagcode))
     * {
     *     ...
     * }
     * </pre>
     */
    virtual ASTNode *getNextSiblingWithTag(int tagcode) const;

    /**
     * Returns pointer to the previous sibling of this element with the given
     * tag code. Returns nullptr if there's no such element.
     */
    virtual ASTNode *getPreviousSiblingWithTag(int tagcode) const;

    /**
     * Returns the number of child elements.
     */
    virtual int getNumChildren() const;

    /**
     * Returns the number of child elements with the given tag code.
     */
    virtual int getNumChildrenWithTag(int tagcode) const;
    //@}

    /** @name Utility functions */

    //@{
    /**
     * Returns first child element with the given tagcode and the given
     * attribute (optionally) having the given value. Returns nullptr if not found.
     */
    ASTNode *getFirstChildWithAttribute(int tagcode, const char *attr, const char *attrvalue=nullptr);

    /**
     * Climb up in the element tree until it finds an element with the given tagcode.
     * Returns "this" if its tagcode already matches. Returns nullptr if not found.
     */
    ASTNode *getParentWithTag(int tagcode);
    //@}

    /** @name Counters */
    //@{

    /**
     * Returns ASTNodes constructed so far (this number can only increase).
     */
    static long getNumCreated() {return numCreated;}

    /**
     * Returns the number of ASTNodes currently in existence (created minus deleted).
     * Useful for detecting memory leaks.
     */
    static long getNumExisting() {return numExisting;}
    //@}

    /** @name User data */
    //@{

    /**
     * Replaces user data object with the given one.
     */
    virtual void setUserData(UserData *data);

    /**
     * Return pointer to the user data object, or nullptr if
     * setUserData() has not been called yet.
     */
    virtual UserData *getUserData() const;
    //@}
};

typedef ASTNode NedElement;
typedef ASTNode MsgElement;

/**
 * @brief Base class for ASTNode factories.
 *
 * @ingroup Data
 */
class NEDXML_API ASTNodeFactory
{
  public:
    virtual ~ASTNodeFactory() {}

    /**
     * Creates ASTNode subclass which corresponds to tagName
     */
    virtual ASTNode *createElementWithTag(const char *tagName) = 0;

    /**
     * Creates ASTNode subclass which corresponds to tagCode
     */
    virtual ASTNode *createElementWithTag(int tagCode) = 0;
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

