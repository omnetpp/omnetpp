//==========================================================================
//  CXMLELEMENT.H - part of
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

#ifndef __OMNETPP_CXMLELEMENT_H
#define __OMNETPP_CXMLELEMENT_H

#include <string>
#include <map>
#include <vector>
#include "simkerneldefs.h"
#include "cenvir.h"
#include "fileline.h"
#include "opp_pooledstring.h"

namespace omnetpp {

class cXMLElement;
class cModule;

/**
 * @brief A list of XML elements. Used with cXMLElement.
 */
typedef std::vector<cXMLElement*> cXMLElementList;

/**
 * @brief Attributes of an XML element. Used with cXMLElement.
 */
typedef std::map<std::string,std::string> cXMLAttributeMap;


/**
 * @brief Represents an XML element in an XML configuration file.
 *
 * XML-typed NED parameters are accessible as cXMLElement via the
 * cPar::xmlValue() method.
 *
 * cXMLElement provides readonly access to XML documents via a DOM-like API.
 * (A full-featured DOM implementation would have been too bloated for
 * the purpose of accessing readonly configuration files).
 *
 * Features: readonly access (getter methods) only; represents only elements,
 * attributes and text content (i.e. entities, processing instructions, comments
 * are ignored); attributes are presented as part of an element node (not as
 * separate attribute nodes as in DOM); mixed content model is not supported
 * (element body cannot mix text with further elements);
 * text is represented as a property of its enclosing element (and not
 * as separate node as in DOM); CDATA sections are also represented as
 * text (with the above restrictions); strings are presented in UTF-8 format
 * (which is normal ASCII string if only characters 0x01-0x7F are used;
 * encoding of the XML file itself may use arbitrary encoding provided it's
 * supported by the underlying XML parser); no namespace support.
 *
 * Supports XPath-like addressing via the getElementByPath() member function.
 *
 * File inclusion is provided via limited support of the XInclude 1.0 spec,
 * if the underlying XML parser supports it (e.g. Expat does not).
 * An element \<xi:include href="doc.xml"/\> will be replaced with
 * the content of the corresponding document. The "href" and "parse"
 * attributes from the XInclude spec are supported.
 *
 * @ingroup Expressions
 */
class SIM_API cXMLElement : public cOwnedObject
{
    friend class cXMLElementDescriptor; // getAttr(i), getChild(i), etc.

  public:
    /**
     * @brief Base class for classes that resolve parameters ($PARAM) that occur in
     * in XPath expressions to their values.
     */
    class SIM_API ParamResolver
    {
      public:
        /**
         * To be redefined in subclasses. If paramname is recognized, the method
         * should store the value in the 'value' argument and return true;
         * otherwise it should return false.
         */
        virtual bool resolve(const char *paramname, std::string& value) = 0;
        virtual ~ParamResolver() {}
    };

  private:
    opp_pooledstring value;
    typedef std::pair<opp_pooledstring,opp_pooledstring> Attr;
    std::vector<Attr> attrs;
    cXMLElement *parent = nullptr;
    cXMLElement *firstChild = nullptr;
    cXMLElement *lastChild = nullptr;
    cXMLElement *prevSibling = nullptr;
    cXMLElement *nextSibling = nullptr;
    FileLine loc;

  private:
     void doGetElementsByTagName(const char *tagname, cXMLElementList& list) const;

  public:
    // internal: constructor
    cXMLElement(const char *tagname, cXMLElement *parent=nullptr);

    // internal: constructor - for backward compatibility
    cXMLElement(const char *tagname, const char *ignored, cXMLElement *parent) : cXMLElement(tagname, parent) {}

    // internal: copy constructor
    cXMLElement(const cXMLElement& other);

    // internal: Destructor. Destroys children too.
    virtual ~cXMLElement();

    // internal: creates and returns an exact copy of this object. Child nodes are not duplicated.
    virtual cXMLElement *dup() const override  {return new cXMLElement(*this);}

    virtual cXMLElement *dupTree() const;

    virtual void setTagName(const char *tagName) {setName(tagName);}

    // internal: sets source location
    virtual void setSourceLocation(const char *fname, int line);

    // internal: sets text node within element
    virtual void setNodeValue(const char *s);

    // internal: sets text node within element
    virtual void setNodeValue(const char *s, int len);

    // internal: appends to text node within element
    virtual void appendNodeValue(const char *s, int len);

    // internal: Sets the value of the attribute with the given name.
    virtual void setAttribute(const char *attr, const char *value);

    // internal: Set attributes of the element: name,value,name,value,...,nullptr
    virtual void setAttributes(const char **attrs);

    // internal: Appends the given element at the end of the child element list.
    virtual void appendChild(cXMLElement *node);

    // internal: Inserts the given element just before the specified child element
    // in the child element list. The where element must be a child of this element.
    virtual void insertChildBefore(cXMLElement *where, cXMLElement *newnode);

    // internal: Removes the given element from the child element list.
    // The pointer passed should be a child of this element.
    virtual cXMLElement *removeChild(cXMLElement *node);

    // internal: matches from root element
    static cXMLElement *getDocumentElementByPath(cXMLElement *documentnode, const char *pathexpr, ParamResolver *resolver=nullptr);

  private:
    // internal
    Attr *findAttr(const char *name) const;
    void addAttr(const char *name, const char *value);
    virtual void print(std::ostream& os, int indentLevel) const;

    // internal, for inspectors only; O(n) complexity!
    int getNumAttrs() const;
    const char *getAttrName(int index) const;
    const char *getAttrValue(int index) const;
    std::string getAttrDesc(int index) const;
    int getNumChildren() const;
    cXMLElement *getChild(int index) const;

  public:
    /** @name Redefined cObject methods. */
    //@{
    /**
     * Returns a one-line description of the element.
     */
    virtual std::string str() const override;

    /**
     * Enables traversing the object tree, performing some operation on
     * each object. The operation is encapsulated in the particular subclass
     * of cVisitor.
     *
     * This method should be redefined in every subclass to call v->visit(obj)
     * for every obj object contained.
     */
    virtual void forEachChild(cVisitor *v) override;
    //@}

    /** @name Common properties */
    //@{

    /**
     * Returns the element name.
     */
    virtual const char *getTagName() const {return getName();}

    /**
     * Returns the file name this element originally came from.
     */
    virtual const char *getSourceFileName() const {return loc.getFilename();}

    /**
     * Returns the line number in the file this element originally came from.
     */
    virtual int getSourceLineNumber() const {return loc.getLineNumber();}

    /**
     * Returns a string containing a file/line position showing where this
     * element originally came from.
     */
    virtual const char *getSourceLocation() const;

    /**
     * Returns text node in the element, or nullptr otherwise.
     * (Mixing text and child elements is not supported.)
     */
    virtual const char *getNodeValue() const;

    /**
     * Returns the value of the attribute with the given name.
     * It returns nullptr if the given attribute is not found.
     */
    virtual const char *getAttribute(const char *attr) const;

    /**
     * Returns true if the element has attributes
     */
    virtual bool hasAttributes() const;

    /**
     * Returns attributes in an a std::map.
     */
    virtual cXMLAttributeMap getAttributes() const;

    /**
     * Returns the subtree as an XML fragment.
     */
    virtual std::string getXML() const;
    //@}

    /** @name Generic access to children and siblings */
    //@{
    /**
     * Returns the parent element, or nullptr if this element has no parent.
     */
    virtual cXMLElement *getParentNode() const;

    /**
     * Returns true if the element has child elements.
     */
    virtual bool hasChildren() const;

    /**
     * Returns pointer to the first child element, or nullptr if this element
     * has no children.
     */
    virtual cXMLElement *getFirstChild() const;

    /**
     * Returns pointer to the last child element, or nullptr if this element
     * has no children.
     */
    virtual cXMLElement *getLastChild() const;

    /**
     * Returns pointer to the next sibling of this element (i.e. the next child
     * in the parent element). Returns nullptr if there're no subsequent elements.
     *
     * getFirstChild() and getNextSibling() can be used to loop through
     * the child list:
     *
     * ```
     * for (cXMLElement *child=element->getFirstChild(); child; child = child->getNextSibling())
     * {
     *    ...
     * }
     * ```
     *
     */
    virtual cXMLElement *getNextSibling() const;

    /**
     * Returns pointer to the previous sibling of this element (i.e. the
     * previous child in the parent element). Returns nullptr if there're no
     * elements before this one.
     */
    virtual cXMLElement *getPreviousSibling() const;

    /**
     * Returns pointer to the first child element with the given tag name,
     * or nullptr if this element has no such children.
     */
    virtual cXMLElement *getFirstChildWithTag(const char *tagname) const;

    /**
     * Returns pointer to the next sibling of this element with the given
     * tag name. Return nullptr if there're no such subsequent elements.
     *
     * getFirstChildWithTag() and getNextSiblingWithTag() are a convient way
     * to loop through elements with a certain tag name in the child list:
     *
     * ```
     * for (cXMLElement *child=element->getFirstChildWithTag("foo"); child; child = child->getNextSiblingWithTag("foo"))
     * {
     *     ...
     * }
     * ```
     */
    virtual cXMLElement *getNextSiblingWithTag(const char *tagname) const;

    /**
     * Returns list of child elements.
     */
    virtual cXMLElementList getChildren() const;

    /**
     * Returns list of child elements with the given tag name.
     */
    virtual cXMLElementList getChildrenByTagName(const char *tagname) const;

    /**
     * Returns list of contained elements with the given tag name, in preorder
     * traversal order.
     */
    virtual cXMLElementList getElementsByTagName(const char *tagname) const;
    //@}

    /** @name Utility functions */
    //@{
    /**
     * Returns find first child element with the given tagname and the given
     * attribute present, and (optionally) having the given value.
     * tagname might be nullptr -- then any element with the given attribute
     * will be accepted. Returns nullptr if not found.
     */
    virtual cXMLElement *getFirstChildWithAttribute(const char *tagname, const char *attr, const char *attrvalue=nullptr) const;

    /**
     * Returns the first element which has an "id" attribute with the given
     * value. ("id" attributes are supposed to be unique in an XML document.)
     * Returns nullptr if not found.
     */
    virtual cXMLElement *getElementById(const char *idattrvalue) const;

    /**
     * Returns the first element designated by the given path expression.
     * ("First" in the sense of preorder depth-first traversal.)
     * Path expressions must be given in an XPath-like notation:
     * they consist of path components (or "steps") separated by "/" or "//".
     * A path component can be an element tag name, "*", "." or "..";
     * tag name and "*" can have an optional predicate in the form "[position]"
     * or "[@attribute='value']". "/" means child element; "//" means a element
     * any levels under the current one; ".", ".." and "*" mean what you
     * expect them to: current element, parent element, element with any tag name.
     *
     * Examples:
     *  - `.` -- this element
     *  - `./foo` -- first "foo" child of this element
     *  - `foo` -- same as `./foo` (initial `./` can be omitted)
     *  - `./foo` -- first "foo" child of this element
     *  - `./foo/bar` -- first "bar" child of first "foo" child of this element
     *  - `.//bar` -- first "bar" anywhere under this element (depth-first search!)
     *  - `./ * /bar` -- first "bar" child two levels below this element (remove the spaces!)
     *  - `./foo[0]` -- first "foo" child of this element
     *  - `./foo[1]` -- second "foo" child of this element
     *  - `./foo[@color='green']` -- first "foo" child which has attribute "color" with value "green"
     *  - `.//bar[1]` -- a "bar" anywhere under this element which is the second "bar" among its siblings
     *  - `.// * [@color='yellow']` -- an element anywhere under this element with attribute color="yellow" (remove the spaces!)
     *  - `.// * [@color='yellow']/foo/bar` -- first "bar" child of first "foo" child of a "yellow-colored" element (remove the spaces!)
     *
     * The method throws an exception if the path expression is invalid,
     * and returns nullptr if the element is not found.
     */
    virtual cXMLElement *getElementByPath(const char *pathexpression, cXMLElement *root=nullptr, ParamResolver *resolver=nullptr) const;
    //@}
};

/**
 * @brief A parameter resolver class for cXMLElement cXMLElement::getElementByPath().
 *
 * Given a cModule pointer, this class resolves the following parameters:
 * $MODULE_FULLPATH, $MODULE_FULLNAME, $MODULE_NAME, $MODULE_INDEX, $MODULE_ID;
 * $PARENTMODULE_FULLPATH etc; $GRANDPARENTMODULE_FULLPATH etc.
 */
class SIM_API ModNameParamResolver : public cXMLElement::ParamResolver
{
  protected:
    cModule *mod;
  public:
    ModNameParamResolver(cModule *mod)  {this->mod = mod;}
    virtual bool resolve(const char *paramname, std::string& value) override;
};

/**
 * @brief A parameter resolver class for cXMLElement::getElementByPath().
 *
 * It resolves parameters from a string map that contains (parametername, value)
 * pairs.
 */
class SIM_API StringMapParamResolver : public cXMLElement::ParamResolver
{
  public:
    typedef std::map<std::string,std::string> StringMap;
  protected:
    StringMap params;
  public:
    StringMapParamResolver(const StringMap& m)  {params = m;}
    virtual bool resolve(const char *paramname, std::string& value) override;
};

}  // namespace omnetpp


#endif

