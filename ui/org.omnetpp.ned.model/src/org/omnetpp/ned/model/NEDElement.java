package org.omnetpp.ned.model;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.PlatformObject;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NEDChangeListenerList;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;
import org.omnetpp.ned.model.pojo.CommentNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * The default implementation of INEDElement.
 * 
 * It extends PlatformObject to have a default IAdaptable implementation
 * primarily for PropertySheet support.
 *
 * @author andras, rhornig
 */
public abstract class NEDElement extends PlatformObject implements INEDElement, IModelProvider
{
	private String source;
	private long id;
	private String srcloc;
	private NEDSourceRegion srcregion;
	private NEDElement parent;
	private NEDElement firstchild;
	private NEDElement lastchild;
	private NEDElement prevsibling;
	private NEDElement nextsibling;
	private HashMap<Object,Object> userData;
	private static long lastid;
    
    private transient List<Integer> errorMarkerIds = new ArrayList<Integer>();

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
		errorMarkerIds.clear();

		id = ++lastid;
	}

	public NEDElement(INEDElement parent) {
		super();
		if (parent != null)
			parent.appendChild(this);
	}

	abstract public String getTagName();

	abstract public int getTagCode();

	public long getId() {
		return id;
	}

	public void setId(long _id) {
		id = _id;
	}

	public String getSourceLocation() {
		return srcloc;
	}

	public void setSourceLocation(String loc) {
		srcloc = loc;
	}

	public NEDSourceRegion getSourceRegion() {
		return srcregion;
	}

	public void setSourceRegion(NEDSourceRegion region) {
		srcregion = region;
	}

	public void applyDefaults() {
		int n = getNumAttributes();
		for (int i=0; i<n; i++)
		{
			String defaultval = getAttributeDefault(i);
			if (defaultval!=null)
				setAttribute(i,defaultval);
		}
	}

	abstract public int getNumAttributes();

	abstract public String getAttributeName(int k);

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

	abstract public String getAttribute(int k);

	public String getAttribute(String attr) {
		int k = lookupAttribute(attr);
		return getAttribute(k);
	}

	abstract public void setAttribute(int k, String value);

	public void setAttribute(String attr, String value) {
		int k = lookupAttribute(attr);
		setAttribute(k,value);
	}

	abstract public String getAttributeDefault(int k);

	public String getAttributeDefault(String attr) {
		int k = lookupAttribute(attr);
		return getAttributeDefault(k);
	}

	public INEDElement getParent() {
		return parent;
	}

    public INEDElement getChild(int index) {
        int i = 0;
        for (INEDElement elem : this) {
            if (i++ == index)
                return elem;
        }
        return null;
    }

	public INEDElement getFirstChild() {
		return firstchild;
	}

	public INEDElement getLastChild() {
		return lastchild;
	}

	public INEDElement getNextSibling() {
		return nextsibling;
	}

	public INEDElement getPrevSibling() {
		return prevsibling;
	}

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
		fireChildInsertedBefore(node,null);
	}

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
		fireChildInsertedBefore(node, where);
	}

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

	public int getNumChildren() {
		int n = 0;
		for (INEDElement node = firstchild; node!=null; node = node.getNextSibling())
			n++;
		return n;
	}

	public int getNumChildrenWithTag(int tagcode) {
		int n = 0;
		for (INEDElement node = firstchild; node!=null; node = node.getNextSibling())
			if (node.getTagCode()==tagcode)
				n++;
		return n;
	}

	public INEDElement getFirstChildWithAttribute(int tagcode, String attr, String attrvalue) {
		for (INEDElement child=getFirstChildWithTag(tagcode); child!=null; child = child.getNextSiblingWithTag(tagcode))
		{
			String val = child.getAttribute(attr);
			if (val!=null && val.equals(attrvalue))
				return child;
		}
		return null;
	}

	public INEDElement getParentWithTag(int tagcode) {
		INEDElement parent = this.getParent();
		while (parent!=null && parent.getTagCode()!=tagcode)
			parent = parent.getParent();
		return parent;
	}

    public void setUserData(Object key, Object value) {
        if (userData == null)
            userData = new HashMap<Object,Object>();
        if (value != null)
            userData.put(key, value);
        else
            userData.remove(key);
    }

    public Object getUserData(Object key) {
        if (userData != null)
            return userData.get(key);
        return null;
    }

    public void removeFromParent() {
        if (getParent() != null)
        	getParent().removeChild(this);
    }

    public boolean hasChildren() {
    	return getFirstChild() != null;
    }

    public String debugString() {
        return getTagName() + " " + StringUtils.defaultIfEmpty(getAttribute("name"), "") + " from " + getSourceLocation();
    }

    public INEDElement dup() {
        INEDElement cloned = NEDElementFactoryEx.getInstance().createNodeWithTag(getTagCode());

        for (int i = 0; i < getNumAttributes(); ++i)
        	cloned.setAttribute(i, getAttribute(i));

        return cloned;
    }

    public INEDElement deepDup() {
        INEDElement result = dup();

        for (INEDElement child : this)
            result.appendChild(child.deepDup());

        return result;
    }

    public INEDTypeInfo getNEDTypeInfo() {
    	// delegate to parent; recursion stops at INedTypeNode classes which override this method.
    	Assert.isTrue(getParent() != null);
    	return getParent().getNEDTypeInfo();
    }

    /**
     * Returns the listener list attached to this element
     */
    protected NEDChangeListenerList getListeners() {
        if (listeners == null)
            listeners = new NEDChangeListenerList();
        return listeners;
    }

	public void addNEDChangeListener(INEDChangeListener listener) {
		getListeners().add(listener);
	}

	public void removeNEDChangeListener(INEDChangeListener listener) {
		getListeners().remove(listener);
	}

    public void fireModelChanged(NEDModelEvent event) {
    	source = null; // invalidate cached NED source code 

        if (listeners != null)
        	listeners.fireModelChanged(event);

        if (parent != null)
        	parent.fireModelChanged(event);
    }

    /**
     * Notifies the listeners of all parents up to the root that an attribute of
     * this element was changed.
     */
    protected void fireAttributeChanged(String attr, Object newValue, Object oldValue) {
    	fireModelChanged(new NEDAttributeChangeEvent(this, attr, newValue, oldValue));
    }

    /**
     * Notifies the listeners of all parents up to the root that the child was
     * inserted into this element.
     */
    protected void fireChildInsertedBefore(INEDElement child, INEDElement where) {
    	fireModelChanged(new NEDStructuralChangeEvent(this, child, NEDStructuralChangeEvent.Type.INSERTION, where, null));
    }

    /**
     * Notifies the listeners of all parents up to the root that the child was
     * removed from this element.
     */
    protected void fireChildRemoved(INEDElement child) {
    	fireModelChanged(new NEDStructuralChangeEvent(this, child, NEDStructuralChangeEvent.Type.REMOVAL, null, child.getNextSibling()));
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.model.interfaces.IModelProvider#getNEDModel()
     */
    public INEDElement getNEDModel() {
        return this;
    }

    public String getComment() {
        CommentNode cn = (CommentNode)getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentNode.ATT_LOCID, "banner");
        if (cn == null)
        	cn = (CommentNode)getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentNode.ATT_LOCID, "right");
        return cn == null ? null : cn.getContent().trim();
    }

    public String getNEDSource() {
		if (source == null)
			source = NEDTreeUtil.generateNedSource(this, true);
		
		return source;
    }

    public List<Integer> getErrorMarkerIds() {
        return errorMarkerIds;
    }

    public int getMaxProblemSeverity() {
        return IMarker.SEVERITY_WARNING; //XXX errorMarkerIds.size() > 0;
    }

    // For debugging purposes only
    @Override
    public String toString() {
        return getClass().getSimpleName() + " " + (getAttribute("name") != null ? getAttribute("name") : "");
        //return NEDTreeUtil.generateXmlFromPojoElementTree(this, "  ");
    }
};

