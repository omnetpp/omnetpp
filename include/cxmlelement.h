//==========================================================================
//  CXMLELEMENT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class cXMLElement
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CXMLELEMENT_H
#define __CXMLELEMENT_H

#include <string>
#include <map>
#include <vector>
#include "defs.h"
#include "cenvir.h"

class cXMLElement;
class cModule;

/**
 * A list of XML elements. Used with cXMLElement.
 */
typedef std::vector<cXMLElement*> cXMLElementList;

/**
 * Attributes of an XML element. Used with cXMLElement.
 */
typedef std::map<std::string,std::string> cXMLAttributeMap;


/**
 * Represents an XML element in an XML configuration file. XML-typed
 * NED parameters are accessible as cXMLElement via the cPar::xmlValue()
 * method.
 *
 * cXMLElement provides read only access to the XML, with functionality that
 * resembles DOM. (A full-featured DOM implementation would have
 * been too bloated for the purpose of accessing readonly
 * configuration files).
 *
 * Features: only readonly (getter) methods; represents only elements
 * and text (entities, processing instructions, comments are ignored);
 * attributes are presented as part of an element node (not as separate
 * attribute nodes as in DOM); mixed content model not supported
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
 * File inclusion via limited support of the XInclude 1.0 spec.
 * An element <xi:include href="doc.xml"/> gets replaced with
 * the content of the corresponding document. The "href" and "parse"
 * attributes from the XInclude spec are supported.
 *
 * @ingroup SimSupport
 */
// TBD if namespaces are supported by Expat & libxml in an easy way, maybe do it here
class SIM_API cXMLElement
{
  public:
    /**
     * Base class for classes that resolve parameters ($PARAM) that occur in
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
    std::string ename;
    std::string srcloc;
    std::string value;
    cXMLAttributeMap attrs;
    cXMLElement *parent;
    cXMLElement *firstchild;
    cXMLElement *lastchild;
    cXMLElement *prevsibling;
    cXMLElement *nextsibling;

  private:
     void doGetElementsByTagName(const char *tagname, cXMLElementList& list) const;

  public:
    // internal: Constructor
    cXMLElement(const char *tagname, const char *srcloc, cXMLElement *parent);

    // internal: sets text node within element
    virtual void setNodeValue(const char *s, int len);

    // internal: appends to text node within element
    virtual void appendNodeValue(const char *s, int len);

    // internal: Destructor. Destroys children too.
    virtual ~cXMLElement();

    // internal: Sets the value of the attribute with the given name.
    virtual void setAttribute(const char *attr, const char *value);

    // internal: Appends the given element at the end of the child element list.
    virtual void appendChild(cXMLElement *node);

    // internal: Inserts the given element just before the specified child element
    // in the child element list. The where element must be a child of this element.
    virtual void insertChildBefore(cXMLElement *where, cXMLElement *newnode);

    // internal: Removes the given element from the child element list.
    // The pointer passed should be a child of this element.
    virtual cXMLElement *removeChild(cXMLElement *node);

    // internal: matches from root element
    static cXMLElement *getDocumentElementByPath(cXMLElement *documentnode, const char *pathexpr, ParamResolver *resolver=NULL);

    // internal
    std::string tostr(int level) const;

    /** @name Common properties */
    //@{

    /**
     * Returns the element name.
     */
    virtual const char *getTagName() const;

    /**
     * Returns a string containing a file/line position showing where this
     * element originally came from.
     */
    virtual const char *getSourceLocation() const;

    /**
     * Returns text node in the element, or NULL otherwise.
     * (Mixing text and child elements is not supported.)
     */
    virtual const char *getNodeValue() const;

    /**
     * Returns the value of the attribute with the given name.
     * It returns NULL if the given attribute is not found.
     */
    virtual const char *getAttribute(const char *attr) const;

    /**
     * Returns true if the node has attributes
     */
    virtual bool hasAttributes() const;

    /**
     * Returns attributes as a const (immutable) std::map.
     */
    virtual const cXMLAttributeMap& getAttributes() const;
    //@}

    /** @name Generic access to children and siblings */
    //@{
    /**
     * Returns the parent element, or NULL if this element has no parent.
     */
    virtual cXMLElement *getParentNode() const;

    /**
     * Returns true if the node has children.
     */
    virtual bool hasChildren() const;

    /**
     * Returns pointer to the first child element, or NULL if this element
     * has no children.
     */
    virtual cXMLElement *getFirstChild() const;

    /**
     * Returns pointer to the last child element, or NULL if this element
     * has no children.
     */
    virtual cXMLElement *getLastChild() const;

    /**
     * Returns pointer to the next sibling of this element (i.e. the next child
     * in the parent element). Returns NULL if there're no subsequent elements.
     *
     * getFirstChild() and getNextSibling() can be used to loop through
     * the child list:
     *
     * <pre>
     * for (cXMLElement *child=node->getFirstChild(); child; child = child->getNextSibling())
     * {
     *    ...
     * }
     * </pre>
     *
     */
    virtual cXMLElement *getNextSibling() const;

    /**
     * Returns pointer to the previous sibling of this element (i.e. the
     * previous child in the parent element). Returns NULL if there're no
     * elements before this one.
     */
    virtual cXMLElement *getPreviousSibling() const;

    /**
     * Returns pointer to the first child element with the given tag name,
     * or NULL if this element has no such children.
     */
    virtual cXMLElement *getFirstChildWithTag(const char *tagname) const;

    /**
     * Returns pointer to the next sibling of this element with the given
     * tag name. Return NULL if there're no such subsequent elements.
     *
     * getFirstChildWithTag() and getNextSiblingWithTag() are a convient way
     * to loop through elements with a certain tag name in the child list:
     *
     * <pre>
     * for (cXMLElement *child=node->getFirstChildWithTag("foo"); child; child = child->getNextSiblingWithTag("foo"))
     * {
     *     ...
     * }
     * </pre>
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
     * tagname might be NULL -- then any element with the given attribute
     * will be accepted. Returns NULL if not found.
     */
    cXMLElement *getFirstChildWithAttribute(const char *tagname, const char *attr, const char *attrvalue=NULL) const;

    /**
     * Returns the first element which has an "id" attribute with the given
     * value. ("id" attributes are supposed to be unique in an XML document.)
     * Returns NULL if not found.
     */
    cXMLElement *getElementById(const char *idattrvalue) const;

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
     *  - <tt>.</tt> -- this element
     *  - <tt>./foo</tt> -- first "foo" child of this node
     *  - <tt>foo</tt> -- same as <tt>./foo</tt> (initial <tt>./</tt> can be omitted)
     *  - <tt>./foo</tt> -- first "foo" child of this node
     *  - <tt>./foo/bar</tt> -- first "bar" child of first "foo" child of this node
     *  - <tt>.//bar</tt> -- first "bar" anywhere under this node (depth-first search!)
     *  - <tt>./ * /bar</tt> -- first "bar" child two levels below this node (omit spaces)
     *  - <tt>./foo[0]</tt> -- first "foo" child of this node
     *  - <tt>./foo[1]</tt> -- second "foo" child of this node
     *  - <tt>./foo[\@color='green']</tt> -- first "foo" child which has attribute "color" with value "green"
     *  - <tt>.//bar[1]</tt> -- a "bar" anywhere under this node which is the second "bar" among its siblings
     *  - <tt>.// * [\@color='yellow']</tt> -- an element anywhere under this node with attribute color="yellow" (omit spaces)
     *  - <tt>.// * [\@color='yellow']/foo/bar</tt> -- first "bar" child of first "foo" child of a "yellow-colored" node (omit spaces)
     *
     * The method throws an exception if the path expression is invalid,
     * and returns NULL if the element is not found.
     */
    cXMLElement *getElementByPath(const char *pathexpression, cXMLElement *root=NULL, ParamResolver *resolver=NULL) const;

    /**
     * Prints detailedInfo() on ev.
     */
    void debugDump() const {ev << detailedInfo();}

    /**
     * Returns tree contents in an XML-like format. This method is only
     * useful for debugging, because it does not perform necessary escaping
     * in text nodes and attribute values, so the output is not necessarily
     * well-formed XML.
     */
    virtual std::string detailedInfo() const;
    //@}
};

/**
 * A parameter resolver class for cXMLElement (more precisely, for
 * cXMLElement::getElementByPath()) that, given a cModule pointer, resolves
 * the following parameters: $MODULE_FULLPATH, $MODULE_FULLNAME, $MODULE_NAME,
 * $MODULE_INDEX, $MODULE_ID; $PARENTMODULE_FULLPATH etc;
 * $GRANDPARENTMODULE_FULLPATH etc.
 */
class SIM_API ModNameParamResolver : public cXMLElement::ParamResolver
{
  protected:
    cModule *mod;
  public:
    ModNameParamResolver(cModule *mod)  {this->mod = mod;}
    virtual bool resolve(const char *paramname, std::string& value);
};

/**
 * A parameter resolver class for cXMLElement (more precisely, for
 * cXMLElement::getElementByPath()), which resolves parameters from
 * a string map that contains (parametername, value) pairs.
 */
class SIM_API StringMapParamResolver : public cXMLElement::ParamResolver
{
  public:
    typedef std::map<std::string,std::string> StringMap;
  protected:
    StringMap params;
  public:
    StringMapParamResolver(const StringMap& m)  {params = m;}
    virtual bool resolve(const char *paramname, std::string& value);
};

#endif

