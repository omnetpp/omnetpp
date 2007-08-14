package org.omnetpp.ned.model;

import java.util.Iterator;
import java.util.List;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public interface INEDElement extends Iterable<INEDElement> {

	public Iterator<INEDElement> iterator();

	/**
	 * Overridden in subclasses to return the name of the XML element the class
	 * represents.
	 */
	public String getTagName();

	/**
	 * Overridden in subclasses to return the numeric code (NED_xxx) of the
	 * XML element the class represents.
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
	 * that corresponds to this element.
	 */
	public NEDSourceRegion getSourceRegion();

	/**
	 * Sets source region, containing a line:col region in the source file
	 * that corresponds to this element. Info comes from the NED parser.
	 */
	public void setSourceRegion(NEDSourceRegion region);

	/**
	 * Sets every attribute to its default value (as returned by getAttributeDefault()).
	 * Attributes without a default value are not affected.
	 *
	 * This method is called from the constructors of derived classes.
	 */
	public void applyDefaults();

	/**
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the number of attributes defined in the DTD.
	 */
	public int getNumAttributes();

	/**
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the name of the kth attribute as defined in the DTD.
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
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the value of the kth attribute (i.e. the attribute with the name
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
	 * Pure virtual method, it should be redefined in subclasses to set
	 * the value of the kth attribute (i.e. the attribute with the name
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
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the default value of the kth attribute, as defined in the DTD.
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
	public INEDElement getParent();

	/**
	 * Returns the index'th child element, or null if this element
	 * has no children.
	 */
	public INEDElement getChild(int index);

	/**
	 * Returns pointer to the first child element, or null if this element
	 * has no children.
	 */
	public INEDElement getFirstChild();

	/**
	 * Returns pointer to the last child element, or null if this element
	 * has no children.
	 */
	public INEDElement getLastChild();

	/**
	 * Returns pointer to the next sibling of this element (i.e. the next child
	 * in the parent element). Returns null if there're no subsequent elements.
	 *
	 * getFirstChild() and getNextSibling() can be used to loop through
	 * the child list:
	 *
	 * <pre>
	 * for (INEDElement *child=node.getFirstChild(); child; child = child.getNextSibling())
	 * {
	 *    ...
	 * }
	 * </pre>
	 *
	 */
	public INEDElement getNextSibling();

	/**
	 * Returns pointer to the previous sibling of this element (i.e. the previous child
	 * in the parent element). Returns null if there're no elements before this one.
	 */
	public INEDElement getPrevSibling();

	/**
	 * Appends the given element at the end of the child element list.
	 *
	 * The node pointer passed should not be null.
	 */
	public void appendChild(INEDElement node);

	/**
	 * Inserts the given element just before the specified child element
	 * in the child element list.
	 *
	 * The where element must be a child of this element. If where == NULL
	 * the node is appended at the end of the list.
	 * The node pointer passed should not be null.
	 */
	public void insertChildBefore(INEDElement where, INEDElement node);

	/**
	 * Removes the given element from the child element list.
	 *
	 * The pointer passed should be a child of this element.
	 */
	public INEDElement removeChild(INEDElement node);

	/**
	 * Returns pointer to the first child element with the given tag code,
	 * or null if this element has no such children.
	 */
	public INEDElement getFirstChildWithTag(int tagcode);

	/**
	 * Returns pointer to the next sibling of this element with the given
	 * tag code. Return null if there're no such subsequent elements.
	 *
	 * getFirstChildWithTag() and getNextSiblingWithTag() are a convenient way
	 * to loop through elements with a certain tag code in the child list:
	 *
	 * <pre>
	 * for (INEDElement *child=node.getFirstChildWithTag(tagcode); child; child = child.getNextSiblingWithTag(tagcode))
	 * {
	 *     ...
	 * }
	 * </pre>
	 */
	public INEDElement getNextSiblingWithTag(int tagcode);

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
	public INEDElement getFirstChildWithAttribute(int tagcode, String attr,
			String attrvalue);

	/**
	 * Climb up in the element tree until it finds an element with the given tagcode.
	 * Returns null if not found.
	 */
	public INEDElement getParentWithTag(int tagcode);

	/**
	 * UserData not belonging directly to the model can be stored using a key. If the value
	 * is NULL the data will be deleted.
	 */
	public void setUserData(Object key, Object value);

	/**
	 * Returns user specific data, not belonging to the model directly
	 */
	public Object getUserData(Object key);

	/**
	 * remove this node from the parent if any.
	 */
	public void removeFromParent();

	public boolean hasChildren();

	/**
	 * Derived classes can override this method to print extra transient information for debugging
	 */
	public String debugString();

	/**
	 * Creates a shallow copy of the tree, but removes it from the tree hierarchy and throws away all children
	 */
	public INEDElement dup(INEDElement parent);

	/**
	 * Creates a deep copy of the tree, optionally providing the new node's parent too
	 */
	public INEDElement deepDup(INEDElement parent);

	/**
	 * Returns the TypeInfo belonging to the containing (toplevel) component
	 * that was added by the incremental builder (type resolver). Or null if none was found.
	 * Cross references and other supporting lists can be accessed via typeInfo.
	 * The typeInfo can be NULL if this element is duplicated or invalid.
	 */
	public INEDTypeInfo getContainerNEDTypeInfo();

	/**
	 * Sets the component type info data. Should be used by the incremental builder ONLY.
	 */
	public void setNEDTypeInfo(INEDTypeInfo typeInfo);

	/**
	 * Adds a new NED change listener to the element. The
	 * listener will receive notifications from the whole
	 * NEDElement subtree (i.e. this element and all elements 
	 * under it.
	 */
	public void addNEDChangeListener(INEDChangeListener listener);

	/**
	 * Removes a NED change listener from the element.
	 */
	public void removeNEDChangeListener(INEDChangeListener listener);

	/**
	 * Returns true if NED change notifications are enabled.
	 */
	public boolean isNotificationEnabled();

	/**
	 * Enable/disable NED change notifications.
	 */
	public boolean setNotificationEnabled(boolean enabled);
	
	/**
	 * Fires a model change element (forwards it to the listener list if any)
	 */
	public void fireModelChanged(NEDModelEvent event);

	/* (non-Javadoc)
	 * @see org.omnetpp.common.displaymodel.IDisplayStringChangeListener#propertyChanged(org.omnetpp.common.displaymodel.IDisplayString, org.omnetpp.common.displaymodel.IDisplayString.Prop, java.lang.Object, java.lang.Object)
	 * this method pass back the modified display string to the model, but it should be called only if the element
	 * really support the additional display string property (ie. IHasDisplayString)
	 * also fires a model attribute change event (converts the propertyChange event to attribute change)
	 */
	public void propertyChanged(IDisplayString source, Prop changedProp,
			Object newValue, Object oldValue);

	/* (non-Javadoc)
	 * @see org.omnetpp.ned.model.interfaces.IModelProvider#getModel()
	 * We provide ourselves as a model
	 */
	public INEDElement getNEDModel();

	/**
	 * Returns the banner comment belonging to the element (if any)
	 */
	public String getComment();

	/**
	 * Returns the re-generated source (text form) of this element
	 */
	public String getSource();

	/**
	 * Returns a collection that contains the error marker ids attached to this ned element
	 * if the size is > 0 the element has an error
	 */
	public List<Integer> getErrorMarkerIds();

	/**
	 * Returns the true if the element has attached errors/markers
	 */
	public boolean hasErrorMarkers();

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 * For debugging purposes
	 */
	public String toString();

}