/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import java.util.List;

import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.NedElementTags;

/**
 * Base class for objects in a NED object tree, the XML-based
 * in-memory representation for NED files. An instance of a INedElement
 * subclass represent an XML element.
 *
 * INedElement provides a DOM-like, generic access to the tree;
 * subclasses additionally provide a typed interface.
 *
 * @author andras, rhornig
 */
public interface INedElement extends Iterable<INedElement>, NedElementTags, NedElementConstants {

    public static final int SEVERITY_NONE = -1;

    /**
     * Returns the name of the XML element the class represents.
     */
    public String getTagName();

    /**
     * Returns the element type as a lower case string suitable for displaying
     * in the UI; e.g. for a SimpleModuleElement it returns "simple module".
     * Usually derived from getTagName().
     */
    public String getReadableTagName();

    /**
     * Returns the numeric code (NED_xxx) of the XML element the class represents.
     */
    public int getTagCode();

    /**
     * Returns a unique id, originally set by the constructor.
     */
    public long getId();

    /**
     * Unique id assigned by the constructor can be overwritten here.
     */
    public void setId(long _id);

    /**
     * Returns a string containing a file/line position showing where this
     * element originally came from.
     */
    public String getSourceLocation();

    /**
     * Sets location string (a string containing a file/line position showing
     * where this element originally came from). Called by the (NED/XML) parser.
     */
    public void setSourceLocation(String loc);

    /**
     * Returns the source region, containing a line:col region in the source file
     * that corresponds to this element. May be null.
     */
    public NedSourceRegion getSourceRegion();

    /**
     * Sets source region, containing a line:col region in the source file
     * that corresponds to this element. Info comes from the NED parser.
     */
    public void setSourceRegion(NedSourceRegion region);

    /**
     * Sets every attribute to its default value (as returned by getAttributeDefault()).
     * Attributes without a default value are not affected.
     *
     * This method is called from the constructors of derived classes.
     */
    public void applyDefaults();

    /**
     * Returns the number of attributes defined in the DTD.
     */
    public int getNumAttributes();

    /**
     * Returns the name of the kth attribute as defined in the DTD.
     *
     * It should return null if k is out of range (i.e. negative or greater than
     * getNumAttributes()).
     */
    public String getAttributeName(int k);

    /**
     * Returns the index of the given attribute. It returns -1 if the attribute
     * is not found. Relies on getNumAttributes() and getAttributeName().
     */
    public int lookupAttribute(String attr);

    /**
     * Returns true if the element supports the given attribute, whether currently set or not.
     */
    public boolean hasAttribute(String attr);

    /**
     * Returns the value of the kth attribute (i.e. the attribute with the name
     * getAttributeName(k)).
     *
     * It should return null if k is out of range (i.e. negative or greater than
     * getNumAttributes()).
     */
    public String getAttribute(int k);

    /**
     * Returns the value of the attribute with the given name.
     * Relies on lookupAttribute() and getAttribute().
     *
     * It returns null if the given attribute is not found.
     */
    public String getAttribute(String attr);

    /**
     * Sets the value of the kth attribute (i.e. the attribute with the name
     * getAttributeName(k)).
     *
     * If k is out of range (i.e. negative or greater than getNumAttributes()),
     * the call should be ignored.
     */
    public void setAttribute(int k, String value);

    /**
     * Sets the value of the attribute with the given name.
     * Relies on lookupAttribute() and setAttribute().
     *
     * If the given attribute is not found, the call has no effect.
     */
    public void setAttribute(String attr, String value);

    /**
     * Returns the default value of the kth attribute, as defined in the DTD.
     *
     * It should return null if k is out of range (i.e. negative or greater than
     * getNumAttributes()), or if the attribute is #REQUIRED; and return ""
     * if the attribute is #IMPLIED.
     */
    public String getAttributeDefault(int k);

    /**
     * Returns the default value of the given attribute, as defined in the DTD.
     * Relies on lookupAttribute() and getAttributeDefault().
     *
     * It returns null if the given attribute is not found.
     */
    public String getAttributeDefault(String attr);

    /**
     * Returns the parent element, or null if this element has no parent.
     */
    public INedElement getParent();

    /**
     * Returns the index'th child element. Throws ArrayIndexOutOfBoundsException
     * if the index is out of bounds (see getNumChildren()).
     */
    public INedElement getChild(int index);

    /**
     * Returns pointer to the first child element, or null if this element
     * has no children.
     */
    public INedElement getFirstChild();

    /**
     * Returns pointer to the last child element, or null if this element
     * has no children.
     */
    public INedElement getLastChild();

    /**
     * Returns pointer to the next sibling of this element (i.e. the next child
     * in the parent element). Returns null if there're no subsequent elements.
     *
     * getFirstChild() and getNextSibling() can be used to loop through
     * the child list:
     *
     * <pre>
     * for (INedElement *child=node.getFirstChild(); child; child = child.getNextSibling())
     * {
     *    ...
     * }
     * </pre>
     *
     */
    public INedElement getNextSibling();

    /**
     * Returns pointer to the previous sibling of this element (i.e. the previous child
     * in the parent element). Returns null if there're no elements before this one.
     */
    public INedElement getPrevSibling();

    /**
     * Appends the given element at the end of the child element list.
     *
     * The node pointer passed should not be null.
     */
    public void appendChild(INedElement node);

    /**
     * Inserts the given element just before the specified child element
     * in the child element list.
     *
     * The where element must be a child of this element. If where == NULL
     * the node is appended at the end of the list.
     * The node pointer passed should not be null.
     */
    public void insertChildBefore(INedElement where, INedElement node);

    /**
     * Removes the given element from the child element list.
     *
     * The pointer passed should be a child of this element.
     */
    public INedElement removeChild(INedElement node);

    /**
     * Removes all child elements from the element.
     */
    public void removeAllChildren();

    /**
     * Creates and returns a list containing the child elements.
     */
    public List<INedElement> getChildren();

    /**
     * Returns pointer to the first child element with the given tag code,
     * or null if this element has no such children.
     */
    public INedElement getFirstChildWithTag(int tagcode);

    /**
     * Returns pointer to the next sibling of this element with the given
     * tag code. Return null if there're no such subsequent elements.
     *
     * getFirstChildWithTag() and getNextSiblingWithTag() are a convenient way
     * to loop through elements with a certain tag code in the child list:
     *
     * <pre>
     * for (INedElement *child=node.getFirstChildWithTag(tagcode); child; child = child.getNextSiblingWithTag(tagcode))
     * {
     *     ...
     * }
     * </pre>
     */
    public INedElement getNextSiblingWithTag(int tagcode);

    /**
     * Creates and returns a list containing the child elements that have the given tagcode.
     */
    public List<INedElement> getChildrenWithTag(int tagcode);

    /**
     * Returns the number of child elements.
     */
    public int getNumChildren();

    /**
     * Returns the number of child elements with the given tag code.
     */
    public int getNumChildrenWithTag(int tagcode);

    /**
     * Returns find first child element with the give tagcode and the given
     * attribute (optionally) having the given value. Returns null if not found.
     */
    public INedElement getFirstChildWithAttribute(int tagcode, String attr,
            String attrvalue);

    /**
     * Climb up in the element tree until it finds an element with the given tagcode.
     * Returns null if not found and the element itself if it matches the tag immediately.
     */
    public INedElement getSelfOrAncestorWithTag(int tagcode);

    /**
     * Finds and returns the element with the given Id in this subtree, or null if not found.
     */
    public INedElement findElementWithId(long id);

    /**
     * Remove this node from the parent if it has one.
     */
    public void removeFromParent();

    /**
     * Returns whether this element has any child elements
     */
    public boolean hasChildren();

    /**
     * Derived classes can override this method to print extra transient information for debugging
     */
    public String debugString();

    /**
     * Creates a shallow copy of the node, for use in the default NED type resolver.
     * Equivalent to dup(NedElement.getDefaultNedTypeResolver(), false).
     * @see NedElement.getDefaultNedTypeResolver().
     */
    public INedElement dup();

    /**
     * Creates a shallow copy of the node, for use in the given target NED type resolver.
     * The rememberOriginal parameter controls whether the clone should store a pointer
     * back to the original element.
     * @see getOriginal()
     */
    public INedElement dup(INedTypeResolver targetResolver, boolean rememberOriginal, boolean cloneId);

    /**
     * Creates a deep copy of the tree, for use in the default NED type resolver.
     * Equivalent to deepDup(NedElement.getDefaultNedTypeResolver(), false).
     * @see NedElement.getDefaultNedTypeResolver().
     */
    public INedElement deepDup();

    /**
     * Creates a deep copy of the tree, for use in the given target NED type resolver.
     * The rememberOriginal parameter controls whether the clone should store a pointer
     * back to the original element.
     * @see getOriginal()
     */
    public INedElement deepDup(INedTypeResolver targetResolver, boolean rememberOriginal, boolean cloneId);

    /**
     * If this element was created by dup() or deepDup() called with rememberOriginal=true,
     * this method returns the INedElement that was cloned to produce this object;
     * otherwise it returns null.
     */
    public INedElement getOriginal();

    /**
     * Returns the first parent which is an INedTypeElement, or null. For example,
     * for a parameter it returns the container NED type; for a submodule node or a
     * submodule display string it finds the compound module; for an inner type it
     * returns the parent type; and for a toplevel type it returns null.
     */
    public INedTypeElement getEnclosingTypeElement();

    /**
     * Like getEnclosingTypeElement(), but for NED types (INedTypeElement) it returns itself.
     */
    public INedTypeElement getSelfOrEnclosingTypeElement();

    /**
     * Returns the (nearest) NedFileElementEx ancestor of this element (or the element itself
     * if called on NedFileElementEx), or null if not found.
     */
    public NedFileElementEx getContainingNedFileElement();

    /**
     * Returns the (nearest) MsgFileElementEx ancestor of this element (or the element itself
     * if called on MsgFileElementEx), or null if not found.
     */
    public MsgFileElementEx getContainingMsgFileElement();

    /**
     * Returns the nearest INedTypeLookupContext parent of the element. This will
     * be the containing compound module, or the NED file.
     */
    public INedTypeLookupContext getEnclosingLookupContext();

    /**
     * Adds a new NED change listener to the element. The
     * listener will receive notifications from the whole
     * NedElement subtree (i.e. this element and all elements
     * under it.
     */
    public void addNedChangeListener(INedChangeListener listener);

    /**
     * Removes a NED change listener from the element.
     */
    public void removeNedChangeListener(INedChangeListener listener);

    /**
     * Fires a model change element (forwards it to the listener list if any)
     * and propagates it up to the parent.
     */
    public void fireModelEvent(NedModelEvent event);

    /**
     * Returns the banner comment belonging to the element (if any)
     */
    public String getComment();

    /**
     * Returns the re-generated source (text form) of this element
     */
    public String getNedSource();

    /**
     * Clears stored syntax problem severities recursively on this subtree
     */
    public void clearSyntaxProblemMarkerSeverities();

    /**
     * Clears stored consistency problem severities recursively on this subtree
     */
    public void clearConsistencyProblemMarkerSeverities();

    /**
     * To be called when a syntax problem (error or warning) is detected for this
     * element. It invokes setSyntaxProblemMaxLocalSeverity(severity) if the new
     * is more severe than the currently stored maximum local severity, otherwise
     * it does nothing. Severity is one of the IMarker severity constants.
     */
    public void syntaxProblemMarkerAdded(int severity);

    /**
     * To be called when a consistency problem (error or warning) is detected for this
     * element. It invokes setConsistencyProblemMaxLocalSeverity(severity) if the new
     * is more severe than the currently stored maximum local severity, otherwise
     * it does nothing. Severity is one of the IMarker severity constants.
     */
    public void consistencyProblemMarkerAdded(int severity);

    /**
     * Sets the syntax problem max local severity to the given value.
     * Use IMarker severity constants or INedElement.SEVERITY_NONE.
     */
    public void setSyntaxProblemMaxLocalSeverity(int severity);

    /**
     * Sets the consistency problem max local severity to the given value.
     * Use IMarker severity constants or INedElement.SEVERITY_NONE.
     */
    public void setConsistencyProblemMaxLocalSeverity(int severity);

    /**
     * Returns the severity of the most severe syntax problem annotated onto
     * this element since error marker severities were last cleared.
     * Returns one of the IMarker severity constants or INedElement.SEVERITY_NONE.
     * @see clearSyntaxProblemMarkerSeverities(), syntaxProblemMarkerAdded()
     */
    public int getSyntaxProblemMaxLocalSeverity();

    /**
     * Returns the severity of the most severe consistency problem annotated onto
     * this element since error marker severities were last cleared.
     * Returns one of the IMarker severity constants or INedElement.SEVERITY_NONE.
     * @see clearConsistencyProblemMarkerSeverities(), consistencyProblemMarkerAdded()
     */
    public int getConsistencyProblemMaxLocalSeverity();

    /**
     * Returns the maximum of the local syntax problem max severities in this subtree.
     */
    public int getSyntaxProblemMaxCumulatedSeverity();

    /**
     * Returns the maximum of the local consistency problem max severities in this subtree.
     */
    public int getConsistencyProblemMaxCumulatedSeverity();

    /**
     * Convenience method: returns true if this element or any element below
     * contains a syntax error. Based on getSyntaxProblemMaxCumulatedSeverity().
     */
    public boolean hasSyntaxError();

    /**
     * Convenience method: returns true if this element or any element below
     * contains a consistency error. Based on getConsistencyProblemMaxCumulatedSeverity().
     */
    public boolean hasConsistencyError();

    /**
     * Convenience method: returns the maximum severity of syntax or consistency
     * problems on this element or any element below, i.e.
     * max(getSyntaxProblemMaxCumulatedSeverity(), getConsistencyProblemMaxCumulatedSeverity()).
     * The result is one of the IMarker severity constants or INedElement.SEVERITY_NONE.
     */
    public int getMaxProblemSeverity();
}
