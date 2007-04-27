package org.omnetpp.ned.model;

import java.util.HashMap;
import java.util.Iterator;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.PlatformObject;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringChangeListener;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NEDChangeListenerList;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;
import org.omnetpp.ned.model.pojo.CommentNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * Base class for objects in a NED object tree, the XML-based
 * in-memory representation for NED files. An instance of a INEDElement
 * subclass represent an XML element.
 * INEDElement provides a DOM-like, generic access to the tree;
 * subclasses additionally provide a typed interface.
 * It extends PlatformObject to have a default IAdaptable implementation
 * primarily for PropertySheet support.
 */
public abstract class NEDElement extends PlatformObject
            implements INEDElement, IDisplayStringChangeListener, IModelProvider
{
	private long id;
	private String srcloc;
	private NEDSourceRegion srcregion;
	private NEDElement parent;
	private NEDElement firstchild;
	private NEDElement lastchild;
	private NEDElement prevsibling;
	private NEDElement nextsibling;
	private static long lastid;
    private HashMap<Object,Object> userData;
    private INEDTypeInfo typeInfo;

    private transient NEDChangeListenerList listeners = null;

	public Iterator<INEDElement> iterator() {
		final INEDElement e = this;
		return new Iterator<INEDElement> () {
			private INEDElement oldChild = null;
			private INEDElement child = e.getFirstChild();

			public boolean hasNext() {
				return child != null;
			}

			public INEDElement next() {
				oldChild = child;
				child = child.getNextSibling();
				return oldChild;
			}

			public void remove() {
				oldChild.getParent().removeChild(oldChild);
			}

		};
	}

	protected static boolean stringToBool(String s)	{
		if (s.equals("true"))
			return true;
		else if (s.equals("false"))
			return false;
		else
			throw new RuntimeException("invalid attribute value  '"+s+"': should be 'true' or 'false'");
	}

	protected static String boolToString(boolean b)	{
		return b ? "true" : "false";
	}

	protected static int stringToEnum(String s, String vals[], int nums[], int n) {
		if (s==null)
			throw new RuntimeException("attribute cannot be empty: should be one of the allowed words '"+vals[0]+"', etc.");
		for (int i=0; i<n; i++)
			if (s.equals(vals[i]))
				return nums[i];
		if (n==0) throw new RuntimeException("call to stringToEnum() with n=0");
		throw new RuntimeException("invalid attribute value '"+s+"': should be one of the allowed words '"+vals[0]+"', etc.");
	}

	protected static String enumToString(int b, String vals[], int nums[], int n) {
		for (int i=0; i<n; i++)
			if (nums[i]==b)
				return vals[i];
		if (n==0) throw new RuntimeException("call to enumToString() with n=0");
		throw new RuntimeException("invalid integer value "+b+" for enum attribute (not one of '"+vals[0]+"'="+nums[0]+" etc)");
	}

	protected static void validateEnum(int b, String vals[], int nums[], int n) {
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
	public NEDElement() {
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
	public NEDElement(INEDElement parent) {
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
	public long getId() {
		return id;
	}

	/**
	 * Unique id assigned by the constructor can be overwritten here.
	 */
	public void setId(long _id) {
		id = _id;
	}

	/**
	 * Returns a string containing a file/line position showing where this
	 * element originally came from.
	 */
	public String getSourceLocation() {
		return srcloc;
	}

	/**
	 * Sets location string (a string containing a file/line position showing
	 * where this element originally came from). Called by the (NED/XML) parser.
	 */
	public void setSourceLocation(String loc) {
		srcloc = loc;
	}

    /**
     * Returns the source region, containing a line:col region in the source file
     * that corresponds to this element.
     */
	public NEDSourceRegion getSourceRegion() {
		return srcregion;
	}

    /**
     * Sets source region, containing a line:col region in the source file
     * that corresponds to this element. Info comes from the NED parser.
     */
	public void setSourceRegion(NEDSourceRegion region) {
		srcregion = region;
	}

    /**
	 * Sets every attribute to its default value (as returned by getAttributeDefault()).
	 * Attributes without a default value are not affected.
	 *
	 * This method is called from the constructors of derived classes.
	 */
	public void applyDefaults() {
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
	public int lookupAttribute(String attr) {
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
	public String getAttribute(String attr) {
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
	public void setAttribute(String attr, String value) {
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
	public String getAttributeDefault(String attr) {
		int k = lookupAttribute(attr);
		return getAttributeDefault(k);
	}

	/**
	 * Returns the parent element, or null if this element has no parent.
	 */
	public INEDElement getParent() {
		return parent;
	}

    /**
     * Returns the index'th child element, or null if this element
     * has no children.
     */
    public INEDElement getChild(int index) {
        int i = 0;
        for(INEDElement elem : this) {
            if (i >= index)
                return elem;
        }
        return null;
    }

    /**
	 * Returns pointer to the first child element, or null if this element
	 * has no children.
	 */
	public INEDElement getFirstChild() {
		return firstchild;
	}

	/**
	 * Returns pointer to the last child element, or null if this element
	 * has no children.
	 */
	public INEDElement getLastChild() {
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
	 * for (INEDElement *child=node.getFirstChild(); child; child = child.getNextSibling())
	 * {
	 *    ...
	 * }
	 * </pre>
	 *
	 */
	public INEDElement getNextSibling() {
		return nextsibling;
	}

	/**
	 * Returns pointer to the previous sibling of this element (i.e. the previous child
	 * in the parent element). Returns null if there're no elements before this one.
	 */
	public INEDElement getPrevSibling() {
		return prevsibling;
	}

	/**
	 * Appends the given element at the end of the child element list.
	 *
	 * The node pointer passed should not be null.
	 */
	public void appendChild(INEDElement inode) {
		NEDElement node = (NEDElement) inode;
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
		fireChildInserted(node,null);
	}

	/**
	 * Inserts the given element just before the specified child element
	 * in the child element list.
	 *
	 * The where element must be a child of this element. If where == NULL
	 * the node is appended at the end of the list.
	 * The node pointer passed should not be null.
	 */
	public void insertChildBefore(INEDElement iwhere, INEDElement inode) {
		if (iwhere == null) {
			appendChild(inode);
			return;
		}
		NEDElement node = (NEDElement) inode;
		NEDElement where = (NEDElement) iwhere;
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
		fireChildInserted(node, where);
	}

	/**
	 * Removes the given element from the child element list.
	 *
	 * The pointer passed should be a child of this element.
	 */
	public INEDElement removeChild(INEDElement inode) {
		NEDElement node = (NEDElement) inode;
		if (node.prevsibling!=null)
			node.prevsibling.nextsibling = node.nextsibling;
		else
			firstchild = node.nextsibling;
		if (node.nextsibling!=null)
			node.nextsibling.prevsibling = node.prevsibling;
		else
			lastchild = node.prevsibling;
		node.parent = node.prevsibling = node.nextsibling = null;
		fireChildRemoved(node);
		return node;
	}

	/**
	 * Returns pointer to the first child element with the given tag code,
	 * or null if this element has no such children.
	 */
	public INEDElement getFirstChildWithTag(int tagcode) {
		INEDElement node = firstchild;
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
	 * for (INEDElement *child=node.getFirstChildWithTag(tagcode); child; child = child.getNextSiblingWithTag(tagcode))
	 * {
	 *     ...
	 * }
	 * </pre>
	 */
	public INEDElement getNextSiblingWithTag(int tagcode) {
		INEDElement node = this.nextsibling;
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
	public int getNumChildren() {
		int n = 0;
		for (INEDElement node = firstchild; node!=null; node = node.getNextSibling())
			n++;
		return n;
	}

	/**
	 * Returns the number of child elements with the given tag code.
	 */
	public int getNumChildrenWithTag(int tagcode) {
		int n = 0;
		for (INEDElement node = firstchild; node!=null; node = node.getNextSibling())
			if (node.getTagCode()==tagcode)
				n++;
		return n;
	}

	/**
	 * Returns find first child element with the give tagcode and the given
	 * attribute (optionally) having the given value. Returns null if not found.
	 */
	public INEDElement getFirstChildWithAttribute(int tagcode, String attr, String attrvalue) {
		for (INEDElement child=getFirstChildWithTag(tagcode); child!=null; child = child.getNextSiblingWithTag(tagcode))
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
	public INEDElement getParentWithTag(int tagcode) {
		INEDElement parent = this.getParent();
		while (parent!=null && parent.getTagCode()!=tagcode)
			parent = parent.getParent();
		return parent;
	}

    /**
     * UserData not belonging directly to the model can be stored using a key. If the value
     * is NULL the data will be deleted.
     */
    public void setUserData(Object key, Object value) {
        if (userData == null)
            userData = new HashMap<Object,Object>();
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
     * remove this node from the parent if any.
     */
    public void removeFromParent() {
        if (getParent() != null)
        	getParent().removeChild(this);
    }

    public boolean hasChildren() {
    	return getFirstChild() != null;
    }

    /**
     * @return Derived classes can override to print extra transient data for debugging
     */
    public String debugString() {
        return getTagName() + " " + getAttribute("name") + getSourceLocation();
    }

    /**
     * Creates a shallow copy of the tree, but removes it from the tree hierarchy and throws away all children
     */
    public INEDElement dup(INEDElement parent) {
        INEDElement cloned = NEDElementFactoryEx.getInstance().createNodeWithTag(getTagCode());

        // FIXME maybe we should add to the parent at the end, so there would be less notifications
        if (parent != null)
            parent.appendChild(cloned);

        for (int i = 0; i< getNumAttributes(); ++i) {
        	cloned.setAttribute(i, getAttribute(i));
        }

        return cloned;
    }

    /**
     * Creates a deep copy of the tree, optionally providing the new node's parent too
     * @return
     */
    public INEDElement deepDup(INEDElement parent) {
        INEDElement result = dup(parent);

        for (INEDElement child : this)
            child.deepDup(result);

        return result;
    }

    /**
     * Returns the TypeInfo belonging to the containing (toplevel) component
     * that was added by the incremental builder (type resolver). Or null if none was found.
     * Cross references and other supporting lists can be accessed via typeInfo.
     * The typeInfo can be NULL if this element is duplicated or invalid.
     */
    public INEDTypeInfo getContainerNEDTypeInfo() {
    	// if we don't have it, fetch it from the parent
    	return (typeInfo != null || getParent() == null) ? typeInfo : getParent().getContainerNEDTypeInfo();
    }

    /**
     * @param typeInfo Sets the component type info data. Should be used by the incremental builder ONLY.
     */
    public void setNEDTypeInfo(INEDTypeInfo typeInfo) {
        Assert.isNotNull(typeInfo);
        Assert.isTrue(this instanceof ITopLevelElement, "TypeInfo should be set only on a TopLevelElement");
        // remove the old type info and add the new one as a listener
        getListeners().remove(this.typeInfo);
        getListeners().add(typeInfo);
        this.typeInfo = typeInfo;
    }

    // ******************* notification helpers ************************************

    /**
     * @return The listener list attached to this element
     */
    public NEDChangeListenerList getListeners() {
        if (listeners == null)
            listeners = new NEDChangeListenerList();
        return listeners;
    }

    /**
     * Fires a model change element (forwards it to he listener list if any)
     * @param event
     */
    public void fireModelChanged(NEDModelEvent event) {
        if(listeners == null || !getListeners().isEnabled())
            return;
        //forward to the listerList
        listeners.fireModelChanged(event);
    }

    /* (non-Javadoc)
     * @see org.omnetpp.common.displaymodel.IDisplayStringChangeListener#propertyChanged(org.omnetpp.common.displaymodel.IDisplayString, org.omnetpp.common.displaymodel.IDisplayString.Prop, java.lang.Object, java.lang.Object)
     * this method pass back the modified display string to the model, but it should be called only if the element
     * really support the additional display string property (ie. IHasDisplayString)
     * also fires a model attribute change event (converts the propertyChange event to attribute change)
     */
    public void propertyChanged(IDisplayString source, Prop changedProp, Object newValue, Object oldValue) {
        Assert.isTrue(this instanceof IHasDisplayString, "propertyChanged should be called only as a result of notificaton from an attached DisplayString");
        // syncronize it to the underlying model
        NEDElementUtilEx.setDisplayString(this, source.toString());
        String propertyName =
            IDisplayString.ATT_DISPLAYSTRING + (changedProp != null ? "."+changedProp : "");
        fireAttributeChanged(propertyName, newValue, oldValue);
    }

    /**
     * Walk upwards in  the tree and send the notification when a listener exists for that node.
     * Fires notification only if the notification on the starting node's listener list is enebled
     * @param attr the attribute under change
     * @param newValue the new value of the attribute
     * @param newValue the old value of the attribute
     */
    protected void fireAttributeChanged(String attr, Object newValue, Object oldValue) {
        if(listeners != null && !getListeners().isEnabled())
            return;

        NEDModelEvent event = new NEDAttributeChangeEvent(this, attr, newValue, oldValue);
        INEDElement node = this;
        while (node != null) {
            node.fireModelChanged(event);
            node = node.getParent();
        }
    }

    /**
     * Walk upwards in  the tree and send the notification when a listener exists for that node
     * @param attr the attribute under change
     * @param newValue the new value of the attribute
     * @param newValue the old value of the attribute
     */
    protected void fireChildInserted(INEDElement child, INEDElement where) {
        if(listeners != null && !getListeners().isEnabled())
            return;

        NEDModelEvent event =
            new NEDStructuralChangeEvent(this, child, NEDStructuralChangeEvent.Type.INSERTION, where, null);
        INEDElement node = this;
        while (node != null) {
            node.fireModelChanged(event);
            node = node.getParent();
        }
    }

    /**
     * Walk upwards in  the tree and send the notification when a listener exists for that node
     * @param attr the attribute under change
     * @param newValue the new value of the attribute
     * @param newValue the old value of the attribute
     */
    protected void fireChildRemoved(INEDElement child) {
        if (listeners != null && !getListeners().isEnabled())
            return;

        NEDModelEvent event =
            new NEDStructuralChangeEvent(this, child, NEDStructuralChangeEvent.Type.REMOVAL, null, child.getNextSibling());
        INEDElement node = this;
        while (node != null) {
            node.fireModelChanged(event);
            node = node.getParent();
        }
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.model.interfaces.IModelProvider#getModel()
     * We provide ourselves as a model
     */
    public INEDElement getNEDModel() {
        return this;
    }

    /**
     * @return The banner comment belonging to the element (if any)
     */
    public String getComment() {
        CommentNode cn = (CommentNode)getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentNode.ATT_LOCID, "banner");
        if (cn == null)
        	cn = (CommentNode)getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentNode.ATT_LOCID, "right");
        return cn == null ? null : cn.getContent().trim();
    }

    /**
     * @return The re-generated source (text form) of this element
     */
    public String getSource() {
        return NEDTreeUtil.generateNedSource(this, true);
    }

    /* (non-Javadoc)
     * @see java.lang.Object#toString()
     * For debugging purposes
     */
    @Override
    public String toString() {
        return getClass().getSimpleName() + " " + (getAttribute("name") != null ? getAttribute("name") : "");
    }
};

