//==========================================================================
// nedelement.h  - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   class NEDElement
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDELEMENT_H
#define __NEDELEMENT_H

#include "nedstring.h"

/**
 * Base class for objects in a NED object tree, the XML-based
 * in-memory representation for NED files. An instance of a NEDElement
 * subclass represent an XML element.
 * NEDElement provides a DOM-like, generic access to the tree;
 * subclasses additionally provide a typed interface.
 *
 * @ingroup Data
 */
class NEDElement
{
  private:
    int id;
    NEDString srcloc;
    NEDElement *parent;
    NEDElement *firstchild;
    NEDElement *lastchild;
    NEDElement *prevsibling;
    NEDElement *nextsibling;
    static long lastid;
  protected:
    static bool stringToBool(const char *s);
    static const char *boolToString(bool b);
    static int stringToEnum(const char *s, const char *vals[], int nums[], int n);
    static const char *enumToString(int b, const char *vals[], int nums[], int n);
  public:
    NEDElement();
    NEDElement(NEDElement *parent);
    virtual ~NEDElement();

    /** @name Common properties */
    //@{
    virtual const char *getTagName() const = 0;
    virtual int getTagCode() const = 0;
    virtual long getId() const;
    virtual void setId(long id);
    virtual const char *getSourceLocation() const;
    virtual void setSourceLocation(const char *loc);
    //@}

    /** @name Generic access to attributes (Methods have to be redefined in subclasses!) */
    //@{
    virtual void applyDefaults();
    virtual int getNumAttributes() const = 0;
    virtual const char *getAttributeName(int k) const = 0;
    virtual int lookupAttribute(const char *attr) const;
    virtual const char *getAttribute(int k) const = 0;
    virtual const char *getAttribute(const char *attr) const;
    virtual void setAttribute(int k, const char *value) = 0;
    virtual void setAttribute(const char *attr, const char *value);
    virtual const char *getAttributeDefault(int k) const = 0;
    virtual const char *getAttributeDefault(const char *attr) const;
    //@}

    /** @name Generic access to children and siblings */
    //@{
    virtual NEDElement *getParent() const;
    virtual NEDElement *getFirstChild() const;
    virtual NEDElement *getLastChild() const;
    virtual NEDElement *getNextSibling() const;
    virtual void appendChild(NEDElement *node);
    virtual void insertChildBefore(NEDElement *where, NEDElement *newnode);
    virtual NEDElement *removeChild(NEDElement *node);
    virtual NEDElement *getFirstChildWithTag(int tagcode) const;
    virtual NEDElement *getNextSiblingWithTag(int tagcode) const;
    //@}
};

#endif

