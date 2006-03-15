package org.omnetpp.ned2.model;

import java.util.HashMap;
import java.util.Iterator;

import org.eclipse.core.runtime.PlatformObject;

/**
 * Base class for objects in a NED object tree, the XML-based
 * in-memory representation for NED files. An instance of a NEDElement
 * subclass represent an XML element.
 * NEDElement provides a DOM-like, generic access to the tree;
 * subclasses additionally provide a typed interface.
 * It extends PlatformObject to have a default IAdaptable implementation
 * primarily for PropertySheet support 
 */
public abstract class NEDElement extends PlatformObject implements Iterable<NEDElement> 
{
	private long id;
	private String srcloc;
	private NEDElement parent;
	private NEDElement firstchild;
	private NEDElement lastchild;
	private NEDElement prevsibling;
	private NEDElement nextsibling;
	private static long lastid;
    private HashMap userData;
    
    private transient NEDChangeListenerList listeners = null;

	public Iterator<NEDElement> iterator() {
		final NEDElement e = this;
		return new Iterator<NEDElement> () {
			private NEDElement oldChild = null;
			private NEDElement child = e.getFirstChild();

			public boolean hasNext() {
				return child != null;
			}

			public NEDElement next() {
				oldChild = child;
				child = child.getNextSibling();
				return oldChild;
			}

			public void remove() {
				oldChild.getParent().removeChild(oldChild);
			}

		};
	}

	protected static boolean stringToBool(String s)
	{
		if (s.equals("true"))
			return true;
		else if (s.equals("false"))
			return false;
		else
			throw new RuntimeException("invalid attribute value  '"+s+"': should be 'true' or 'false'");
	}

	protected static String boolToString(boolean b)
	{
		return b ? "true" : "false";
	}

	protected static int stringToEnum(String s, String vals[], int nums[], int n)
	{
		if (s==null)
			throw new RuntimeException("attribute cannot be empty: should be one of the allowed words '"+vals[0]+"', etc.");
		for (int i=0; i<n; i++)
			if (s.equals(vals[i]))
				return nums[i];
		if (n==0) throw new RuntimeException("call to stringToEnum() with n=0");
		throw new RuntimeException("invalid attribute value '"+s+"': should be one of the allowed words '"+vals[0]+"', etc.");
	}

	protected static String enumToString(int b, String vals[], int nums[], int n)
	{
		for (int i=0; i<n; i++)
			if (nums[i]==b)
				return vals[i];
		if (n==0) throw new RuntimeException("call to enumToString() with n=0");
		throw new RuntimeException("invalid integer value "+b+" for enum attribute (not one of '"+vals[0]+"'="+nums[0]+" etc)");
	}

	protected static void validateEnum(int b, String vals[], int nums[], int n)
	{
		// code almost identical to enumToString()
		for (int i=0; i<n; i++)
			if (nums[i]==b)
				return;
		if (n==0) throw new RuntimeException("call to validateEnum() with n=0");
		throw new RuntimeException("invalid integer value "+b+" for enum attribute");
	}


	/**
	 * Constructor
	 */
	public NEDElement()
	{
		parent = null;
		firstchild = null;
		lastchild = null;
		prevsibling = null;
		nextsibling = null;

		id = ++lastid;
	}

	/**
	 * Constructor. Takes parent element.
	 */
	public NEDElement(NEDElement parent)
	{
		super();
		if (parent != null)
			parent.appendChild(this);
	}

	/**
	 * Overridden in subclasses to return the name of the XML element the class
	 * represents.
	 */
	abstract public String getTagName();

	/**
	 * Overridden in subclasses to return the numeric code (NED_xxx) of the
	 * XML element the class represents.
	 */
	abstract public int getTagCode();

	/**
	 * Returns a unique id, originally set by the contructor.
	 */
	public long getId()
	{
		return id;
	}

	/**
	 * Unique id assigned by the constructor can be overwritten here.
	 */
	public void setId(long _id)
	{
		id = _id;
	}

	/**
	 * Returns a string containing a file/line position showing where this
	 * element originally came from.
	 */
	public String getSourceLocation() 
	{
		return srcloc;
	}

	/**
	 * Sets location string (a string containing a file/line position showing
	 * where this element originally came from). Called by the (NED/XML) parser.
	 */
	public void setSourceLocation(String loc)
	{
		srcloc = loc;
	}

	/**
	 * Sets every attribute to its default value (as returned by getAttributeDefault()).
	 * Attributes without a default value are not affected.
	 *
	 * This method is called from the constructors of derived classes.
	 */
	public void applyDefaults()
	{
		int n = getNumAttributes();
		for (int i=0; i<n; i++)
		{
			String defaultval = getAttributeDefault(i);
			if (defaultval!=null)
				setAttribute(i,defaultval);
		}
	}

	/**
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the number of attributes defined in the DTD.
	 */
	abstract public int getNumAttributes();

	/**
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the name of the kth attribute as defined in the DTD.
	 *
	 * It should return null if k is out of range (i.e. negative or greater than
	 * getNumAttributes()).
	 */
	abstract public String getAttributeName(int k);

	/**
	 * Returns the index of the given attribute. It returns -1 if the attribute
	 * is not found. Relies on getNumAttributes() and getAttributeName().
	 */
	public int lookupAttribute(String attr) 
	{
		int n = getNumAttributes();
		for (int i=0; i<n; i++)
		{
			String attnamei = getAttributeName(i);
			if (attr.equals(attnamei))
				return i;
		}
		return -1;
	}

	/**
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the value of the kth attribute (i.e. the attribute with the name
	 * getAttributeName(k)).
	 *
	 * It should return null if k is out of range (i.e. negative or greater than
	 * getNumAttributes()).
	 */
	abstract public String getAttribute(int k);

	/**
	 * Returns the value of the attribute with the given name.
	 * Relies on lookupAttribute() and getAttribute().
	 *
	 * It returns null if the given attribute is not found.
	 */
	public String getAttribute(String attr)
	{
		int k = lookupAttribute(attr);
		return getAttribute(k);
	}

	/**
	 * Pure virtual method, it should be redefined in subclasses to set
	 * the value of the kth attribute (i.e. the attribute with the name
	 * getAttributeName(k)).
	 *
	 * If k is out of range (i.e. negative or greater than getNumAttributes()),
	 * the call should be ignored.
	 */
	abstract public void setAttribute(int k, String value);

	/**
	 * Sets the value of the attribute with the given name.
	 * Relies on lookupAttribute() and setAttribute().
	 *
	 * If the given attribute is not found, the call has no effect.
	 */
	public void setAttribute(String attr, String value)
	{
		int k = lookupAttribute(attr);
		setAttribute(k,value);
	}

	/**
	 * Pure virtual method, it should be redefined in subclasses to return
	 * the default value of the kth attribute, as defined in the DTD.
	 *
	 * It should return null if k is out of range (i.e. negative or greater than
	 * getNumAttributes()), or if the attribute is #REQUIRED; and return ""
	 * if the attribute is #IMPLIED.
	 */
	abstract public String getAttributeDefault(int k);

	/**
	 * Returns the default value of the given attribute, as defined in the DTD.
	 * Relies on lookupAttribute() and getAttributeDefault().
	 *
	 * It returns null if the given attribute is not found.
	 */
	public String getAttributeDefault(String attr)
	{
		int k = lookupAttribute(attr);
		return getAttributeDefault(k);
	}

	/**
	 * Returns the parent element, or null if this element has no parent.
	 */
	public NEDElement getParent()
	{
		return parent;
	}

	/**
	 * Returns pointer to the first child element, or null if this element
	 * has no children.
	 */
	public NEDElement getFirstChild()
	{
		return firstchild;
	}

	/**
	 * Returns pointer to the last child element, or null if this element
	 * has no children.
	 */
	public NEDElement getLastChild() 
	{
		return lastchild;
	}

	/**
	 * Returns pointer to the next sibling of this element (i.e. the next child
	 * in the parent element). Returns null if there're no subsequent elements.
	 *
	 * getFirstChild() and getNextSibling() can be used to loop through
	 * the child list:
	 *
	 * <pre>
	 * for (NEDElement *child=node.getFirstChild(); child; child = child.getNextSibling())
	 * {
	 *    ...
	 * }
	 * </pre>
	 *
	 */
	public NEDElement getNextSibling() 
	{
		return nextsibling;
	}

	/**
	 * Returns pointer to the previous sibling of this element (i.e. the previous child
	 * in the parent element). Returns null if there're no elements before this one.
	 */
	public NEDElement getPrevSibling() 
	{
		return prevsibling;
	}

	/**
	 * Appends the given element at the end of the child element list.
	 *
	 * The node pointer passed should not be null.
	 */
	public void appendChild(NEDElement node)
	{
		if (node.parent!=null)
			node.parent.removeChild(node);
		node.parent = this;
		node.prevsibling = lastchild;
		node.nextsibling = null;
		if (node.prevsibling!=null)
			node.prevsibling.nextsibling = node;
		else
			firstchild = node;
		lastchild = node;
		fireChildInsertedToAncestors(null, node);
	}

	/**
	 * Inserts the given element just before the specified child element
	 * in the child element list.
	 *
	 * The where element must be a child of this element. If where == NULL
	 * the node is appended at the end of the list. 
	 * The node pointer passed should not be null.
	 */
	public void insertChildBefore(NEDElement where, NEDElement node)
	{
		if(where == null) {
			appendChild(node);
			return;
		}
		if (node.parent!=null)
			node.parent.removeChild(node);
		node.parent = this;
		node.prevsibling = where.prevsibling;
		node.nextsibling = where;
		where.prevsibling = node;
		if (node.prevsibling!=null)
			node.prevsibling.nextsibling = node;
		else
			firstchild = node;
		fireChildInsertedToAncestors(where, node);
	}

	/**
	 * Removes the given element from the child element list.
	 *
	 * The pointer passed should be a child of this element.
	 */
	public NEDElement removeChild(NEDElement node)
	{
		if (node.prevsibling!=null)
			node.prevsibling.nextsibling = node.nextsibling;
		else
			firstchild = node.nextsibling;
		if (node.nextsibling!=null)
			node.nextsibling.prevsibling = node.prevsibling;
		else
			lastchild = node.prevsibling;
		node.parent = node.prevsibling = node.nextsibling = null;
		fireChildRemovedToAncestors(node);
		return node;
	}

	/**
	 * Returns pointer to the first child element with the given tag code,
	 * or null if this element has no such children.
	 */
	public NEDElement getFirstChildWithTag(int tagcode)
	{
		NEDElement node = firstchild;
		while (node!=null)
		{
			if (node.getTagCode()==tagcode)
				return node;
			node = node.getNextSibling();
		}
		return null;
	}

	/**
	 * Returns pointer to the next sibling of this element with the given
	 * tag code. Return null if there're no such subsequent elements.
	 *
	 * getFirstChildWithTag() and getNextSiblingWithTag() are a convient way
	 * to loop through elements with a certain tag code in the child list:
	 *
	 * <pre>
	 * for (NEDElement *child=node.getFirstChildWithTag(tagcode); child; child = child.getNextSiblingWithTag(tagcode))
	 * {
	 *     ...
	 * }
	 * </pre>
	 */
	public NEDElement getNextSiblingWithTag(int tagcode)
	{
		NEDElement node = this.nextsibling;
		while (node!=null)
		{
			if (node.getTagCode()==tagcode)
				return node;
			node = node.getNextSibling();
		}
		return null;
	}

	/**
	 * Returns the number of child elements.
	 */
	public int getNumChildren()
	{
		int n=0;
		for (NEDElement node = firstchild; node!=null; node = node.getNextSibling())
			n++;
		return n;
	}

	/**
	 * Returns the number of child elements with the given tag code.
	 */
	public int getNumChildrenWithTag(int tagcode)
	{
		int n=0;
		for (NEDElement node = firstchild; node!=null; node = node.getNextSibling())
			if (node.getTagCode()==tagcode)
				n++;
		return n;
	}

	/**
	 * Returns find first child element with the give tagcode and the given
	 * attribute (optionally) having the given value. Returns null if not found.
	 */
	public NEDElement getFirstChildWithAttribute(int tagcode, String attr, String attrvalue)
	{
		for (NEDElement child=getFirstChildWithTag(tagcode); child!=null; child = child.getNextSiblingWithTag(tagcode))
		{
			String val = child.getAttribute(attr);
			if (val!=null && val.equals(attrvalue))
				return child;
		}
		return null;
	}

	/**
	 * Climb up in the element tree until it finds an element with the given tagcode.
	 * Returns null if not found.
	 */
	public NEDElement getParentWithTag(int tagcode)
	{
		NEDElement parent = this.getParent();
		while (parent!=null && parent.getTagCode()!=tagcode)
			parent = parent.getParent();
		return parent;
	}

	/**
     * Walk upwards in  the tree and send the notification when a listener exists for that node 
	 * @param attr
	 */
	protected void fireAttributeChangedToAncestors(String attr) {
		NEDElement node = this;
		while (node != null) {
            node.fireAttributeChanged(this,attr);
			node = node.getParent();
        }
	}

	protected void fireChildInsertedToAncestors(NEDElement where, NEDElement child) {
        NEDElement node = this;
        while (node != null) {
            node.fireChildInserted(this, where, child);
            node = node.getParent();
        }
	}

	protected void fireChildRemovedToAncestors(NEDElement child) {
        NEDElement node = this;
        while (node != null) {
            node.fireChildRemoved(this, child);
            node = node.getParent();
        }
	}

    protected void fireAttributeChanged(NEDElement node, String attr) {
        if(listeners != null) 
            listeners.fireAttributeChanged(node, attr);
    }

    protected void fireChildInserted(NEDElement node, NEDElement where, NEDElement child) {
        if(listeners != null) 
            listeners.fireChildInserted(node, where, child);
    }

    protected void fireChildRemoved(NEDElement node, NEDElement child) {
        if(listeners != null) 
            listeners.fireChildRemoved(node, child);
    }

    /**
     * Add a listener to this node. It will receive ALL change notifications
     * generated by this node and ALL its descendants.
     */
    public void addListener(INEDChangeListener listener) {
        if( listeners == null)
            listeners = new NEDChangeListenerList();
        listeners.add(listener);
    }

    /**
     * @param listener
     */
    public void removeListener(INEDChangeListener listener) {
        if(listeners != null)
            listeners.remove(listener);
    }
  
    /**
     * UserData not belonging directly to the model can be stored using a key. If the value
     * is NULL the data will be deleted. 
     */
    public void setUserData(Object key, Object value) {
        if (userData == null)
            userData = new HashMap();
        if(value != null)
            userData.put(key, value);
        else 
            userData.remove(key);
    }
    
    /**
     * @return User specific data, not belonging to the model directly
     */
    public Object getUserData(Object key) {
        if(userData != null)
            return userData.get(key);
        return null;
    }
    
    /**
     * remove this node from the parent.
     */
    public void removeFromParent() {
        getParent().removeChild(this);
    }
    
    /**
     * @return Derived classes can override to print extra transient data for debugging
     */
    public String debugString() {
        return "";
    }
};

