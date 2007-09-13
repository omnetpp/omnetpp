package org.omnetpp.ned.model;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.PlatformObject;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NEDChangeListenerList;
import org.omnetpp.ned.model.notification.NEDMarkerChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
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
	private static final int SEVERITY_INVALID = Integer.MIN_VALUE;

	private long id;
	private String srcloc;
	private NEDSourceRegion srcregion;
	private NEDElement parent;
	private NEDElement firstchild;
	private NEDElement lastchild;
	private NEDElement prevsibling;
	private NEDElement nextsibling;
	private String source;
	private HashMap<Object,Object> userData;
	private static long lastid;

	// store maximum severity of error markers associated with this element.
	// "syntax": NEDSYNTAXPROBLEM_MARKERID; "consistency": NEDCONSISTENCYPROBLEM_MARKERID;
	// "local": this NEDElement; "cumulated": this element and its subtree
    private int syntaxProblemMaxLocalSeverity = SEVERITY_NONE;
    private int consistencyProblemMaxLocalSeverity = SEVERITY_NONE;
    private int syntaxProblemMaxCumulatedSeverity = SEVERITY_INVALID;
    private int consistencyProblemMaxCumulatedSeverity = SEVERITY_INVALID;

    private transient NEDChangeListenerList listeners = null;

    // needed because we cannot write NEDResourcesPlugin.getNEDResources() (plug-in dependency cycle)
    private static INEDTypeResolver defaultTypeResolver = null;


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
	 * Sets the default NED type resolver. May only be invoked once.
	 */
	public static void setDefaultTypeResolver(INEDTypeResolver typeResolver) {
		Assert.isTrue(defaultTypeResolver == null);
		defaultTypeResolver = typeResolver;
	}

	/**
	 * Returns the default NED type resolver. Guaranteed to be non-null.
	 */
	public static INEDTypeResolver getDefaultTypeResolver() {
		Assert.isTrue(defaultTypeResolver != null); // must have been set previously
		return defaultTypeResolver;
	}

	/**
	 * Resolves the given type name using the default NED type resolver,
	 * or returns null if the given string is null or "".
	 */
	public static INEDTypeInfo resolveTypeName(String typeName, INedTypeLookupContext context) {
		return StringUtils.isEmpty(typeName) ? null : getDefaultTypeResolver().lookupNedType(typeName, context);
	}

	/**
	 * Constructor
	 */
	public NEDElement() {
		id = ++lastid;
	}

	/**
	 * Constructor. Takes parent element.
	 */
	public NEDElement(INEDElement parent) {
		this();
		if (parent != null)
			parent.appendChild(this);
	}

	abstract public String getTagName();

	abstract public int getTagCode();

    public String getReadableTagName() {
    	return getTagName().replace('-', ' ');  // override if something more sophisticated is needed
    }

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

	public boolean hasAttribute(String attr) {
		return lookupAttribute(attr) != -1;
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
		Assert.isTrue(iwhere.getParent() == this);
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
		Assert.isTrue(inode.getParent() == this);
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
		INEDElement node = this;
		while (node!=null && node.getTagCode()!=tagcode)
			node = node.getParent();
		return node;
	}

	public INEDElement findElementWithId(long id) {
		if (getId() == id)
			return this;
		for (INEDElement child : this) {
			INEDElement node = child.findElementWithId(id);
			if (node != null)
				return node;
		}
		return null;
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
        return NEDTreeUtil.getNedModelLabelProvider().getText(this) + " from " + getSourceLocation();
    }

    public INEDElement dup() {
        INEDElement cloned = NEDElementFactoryEx.getInstance().createElement(getTagCode());

        for (int i = 0; i < getNumAttributes(); ++i)
        	cloned.setAttribute(i, getAttribute(i));

    	cloned.setSourceLocation(getSourceLocation());
    	cloned.setSourceRegion(getSourceRegion());
    	cloned.setSyntaxProblemMaxLocalSeverity(getSyntaxProblemMaxLocalSeverity());
    	cloned.setConsistencyProblemMaxLocalSeverity(getConsistencyProblemMaxLocalSeverity());

        return cloned;
    }

    public INEDElement deepDup() {
        INEDElement result = dup();

        for (INEDElement child : this)
            result.appendChild(child.deepDup());

        return result;
    }

	public INedTypeElement getEnclosingTypeElement() {
		INEDElement node = getParent();
		while (node != null && !(node instanceof INedTypeElement))
			node = node.getParent();
		return (INedTypeElement) node;
	}

	public INedTypeElement getSelfOrEnclosingTypeElement() {
		INEDElement node = this;
		while (node != null && !(node instanceof INedTypeElement))
			node = node.getParent();
		return (INedTypeElement) node;
	}

	public NedFileElementEx getContainingNedFileElement() {
		return (NedFileElementEx) getParentWithTag(NEDElementTags.NED_NED_FILE);
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

    public void fireModelEvent(NEDModelEvent event) {
    	// invalidate cached data
    	source = null;
    	syntaxProblemMaxCumulatedSeverity = SEVERITY_INVALID;
    	consistencyProblemMaxCumulatedSeverity = SEVERITY_INVALID;

    	// notify listeners: first local, then parents
        if (listeners != null)
        	listeners.fireModelChanged(event);

        if (parent != null)
        	parent.fireModelEvent(event);
    }

    /**
     * Notifies the listeners of all parents up to the root that an attribute of
     * this element was changed.
     */
    protected void fireAttributeChanged(String attr, Object newValue, Object oldValue) {
    	fireModelEvent(new NEDAttributeChangeEvent(this, attr, newValue, oldValue));
    }

    /**
     * Notifies the listeners of all parents up to the root that the child was
     * inserted into this element.
     */
    protected void fireChildInsertedBefore(INEDElement child, INEDElement where) {
    	fireModelEvent(new NEDStructuralChangeEvent(this, child, NEDStructuralChangeEvent.Type.INSERTION, where, null));
    }

    /**
     * Notifies the listeners of all parents up to the root that the child was
     * removed from this element.
     */
    protected void fireChildRemoved(INEDElement child) {
    	fireModelEvent(new NEDStructuralChangeEvent(this, child, NEDStructuralChangeEvent.Type.REMOVAL, null, child.getNextSibling()));
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.model.interfaces.IModelProvider#getNEDModel()
     */
    public INEDElement getNEDModel() {
        return this;
    }

	/*
	 * Utility function for INedTypeElement subclasses
	 */
    protected static INedTypeLookupContext getParentLookupContextFor(INedTypeElement nedTypeElement) {
        INedTypeElement enclosingType = nedTypeElement.getEnclosingTypeElement();
   		Assert.isTrue(enclosingType==null || enclosingType instanceof CompoundModuleElementEx, "only compound modules may have inner types");
   		return enclosingType != null ? (CompoundModuleElementEx)enclosingType : nedTypeElement.getContainingNedFileElement();
	}

	/*
	 * Utility function for INedTypeElement subclasses
	 */
    protected static List<ExtendsElement> getAllExtendsFrom(INedTypeElement nedTypeElement) {
        List<ExtendsElement> result = new ArrayList<ExtendsElement>();
        for (INEDElement child : nedTypeElement)
            if (child instanceof ExtendsElement)
                result.add((ExtendsElement)child);
        return result;
    }

	/*
	 * Utility function for subclasses: returns node.getDisplayString(), or null if node==null.
	 */
	protected static DisplayString displayStringOf(IHasDisplayString element) {
		return element == null ? null : element.getDisplayString();
	}

	public String getComment() {
        CommentElement comment = (CommentElement)getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "banner");
        if (comment == null)
        	comment = (CommentElement)getFirstChildWithAttribute(NEDElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "right");
        return comment == null ? null : comment.getContent().trim();
    }

    public String getNEDSource() {
		if (source == null)
			source = NEDTreeUtil.generateNedSource(this, true);

		return source;
    }

    /* problem markers */

    public void clearSyntaxProblemMarkerSeverities() {
    	if (syntaxProblemMaxLocalSeverity != SEVERITY_NONE) {
    		syntaxProblemMaxLocalSeverity = SEVERITY_NONE;
        	fireModelEvent(new NEDMarkerChangeEvent(this));
    	}
    	for (INEDElement child : this)
    		child.clearSyntaxProblemMarkerSeverities();
    }

    public void clearConsistencyProblemMarkerSeverities() {
    	if (consistencyProblemMaxLocalSeverity != SEVERITY_NONE) {
    		consistencyProblemMaxLocalSeverity = SEVERITY_NONE;
        	fireModelEvent(new NEDMarkerChangeEvent(this));
    	}
    	for (INEDElement child : this)
    		child.clearConsistencyProblemMarkerSeverities();
    }

    static {
    	// code below exploits numeric order of severities, lets assert it
    	Assert.isTrue(SEVERITY_NONE < IMarker.SEVERITY_WARNING && IMarker.SEVERITY_WARNING < IMarker.SEVERITY_ERROR);
    }

    public void syntaxProblemMarkerAdded(int severity) {
    	if (syntaxProblemMaxLocalSeverity < severity) {
    		syntaxProblemMaxLocalSeverity = severity;
    		fireModelEvent(new NEDMarkerChangeEvent(this));
    	}
    }

    public void consistencyProblemMarkerAdded(int severity) {
    	if (consistencyProblemMaxLocalSeverity < severity) {
    		consistencyProblemMaxLocalSeverity = severity;
    		fireModelEvent(new NEDMarkerChangeEvent(this));
    	}
    }

    public void setSyntaxProblemMaxLocalSeverity(int severity) {
    	if (syntaxProblemMaxLocalSeverity != severity) {
    		syntaxProblemMaxLocalSeverity = severity;
    		fireModelEvent(new NEDMarkerChangeEvent(this));
    	}
    }

    public void setConsistencyProblemMaxLocalSeverity(int severity) {
    	if (consistencyProblemMaxLocalSeverity != severity) {
    		consistencyProblemMaxLocalSeverity = severity;
    		fireModelEvent(new NEDMarkerChangeEvent(this));
    	}
    }

    public int getSyntaxProblemMaxLocalSeverity() {
		return syntaxProblemMaxLocalSeverity;
	}

    public int getConsistencyProblemMaxLocalSeverity() {
		return consistencyProblemMaxLocalSeverity;
	}

    public int getSyntaxProblemMaxCumulatedSeverity() {
    	if (syntaxProblemMaxCumulatedSeverity == SEVERITY_INVALID)
    		updateCumulatedProblemSeverities();
		return syntaxProblemMaxCumulatedSeverity;
	}

	public int getConsistencyProblemMaxCumulatedSeverity() {
    	if (consistencyProblemMaxCumulatedSeverity == SEVERITY_INVALID)
    		updateCumulatedProblemSeverities();
		return consistencyProblemMaxCumulatedSeverity;
	}

    public boolean hasSyntaxError() {
    	return getSyntaxProblemMaxCumulatedSeverity() == IMarker.SEVERITY_ERROR;
	}

    public boolean hasConsistencyError() {
    	return getConsistencyProblemMaxCumulatedSeverity() == IMarker.SEVERITY_ERROR;
	}

	public int getMaxProblemSeverity() {
    	return Math.max(getSyntaxProblemMaxCumulatedSeverity(), getConsistencyProblemMaxCumulatedSeverity());
    }

	protected void updateCumulatedProblemSeverities() {
		int nedSeverity = syntaxProblemMaxLocalSeverity;
		int consistencySeverity = consistencyProblemMaxLocalSeverity;
		for (INEDElement child : this) {
			nedSeverity = Math.max(nedSeverity, child.getSyntaxProblemMaxCumulatedSeverity());
			consistencySeverity = Math.max(consistencySeverity, child.getConsistencyProblemMaxCumulatedSeverity());
		}
		syntaxProblemMaxCumulatedSeverity = nedSeverity;
		consistencyProblemMaxCumulatedSeverity = consistencySeverity;
	}

    // For debugging purposes only
    @Override
    public String toString() {
        //return NEDTreeUtil.getNedModelLabelProvider().getText(this) + " from " + getSourceLocation();
        return getClass().getSimpleName() + " " + (hasAttribute("name") ? getAttribute("name") : "");
        //return NEDTreeUtil.generateXmlFromPojoElementTree(this, "  ");
    }

}

