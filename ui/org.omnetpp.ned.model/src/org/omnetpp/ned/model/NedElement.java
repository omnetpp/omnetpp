/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.PlatformObject;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IMsgTypeResolver;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NedChangeListenerList;
import org.omnetpp.ned.model.notification.NedMarkerChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.notification.NedStructuralChangeEvent;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;

/**
 * The default implementation of INedElement.
 *
 * It extends PlatformObject to have a default IAdaptable implementation
 * primarily for PropertySheet support.
 *
 * @author andras, rhornig
 */
public abstract class NedElement extends PlatformObject implements INedElement, INedModelProvider
{
    private static final int SEVERITY_INVALID = Integer.MIN_VALUE;

    private long id;
    private String srcloc;
    private NedSourceRegion srcregion;
    private NedElement parent;
    private NedElement firstchild;
    private NedElement lastchild;
    private NedElement prevsibling;
    private NedElement nextsibling;
    private String source;
    private int numChildren = 0;
    private INedElement[] cachedChildArray;
    private INedElement original;
    private static long lastid;

    // store maximum severity of error markers associated with this element.
    // "syntax": NEDSYNTAXPROBLEM_MARKERID; "consistency": NEDCONSISTENCYPROBLEM_MARKERID;
    // "local": this NedElement; "cumulated": this element and its subtree
    private int syntaxProblemMaxLocalSeverity = SEVERITY_NONE;
    private int consistencyProblemMaxLocalSeverity = SEVERITY_NONE;
    private int syntaxProblemMaxCumulatedSeverity = SEVERITY_INVALID;
    private int consistencyProblemMaxCumulatedSeverity = SEVERITY_INVALID;

    private transient NedChangeListenerList listeners = null;

    // needed because we cannot write NedResourcesPlugin.getNedResources() (plug-in dependency cycle)
    private static INedTypeResolver defaultNedTypeResolver = null;

    // needed because we cannot write NedResourcesPlugin.getMsgResources() (plug-in dependency cycle)
    private static IMsgTypeResolver defaultMsgTypeResolver = null;

    public Iterator<INedElement> iterator() {
        final INedElement e = this;
        return new Iterator<INedElement> () {
            private INedElement oldChild = null;
            private INedElement child = e.getFirstChild();

            public boolean hasNext() {
                return child != null;
            }

            public INedElement next() {
                oldChild = child;
                child = child.getNextSibling();
                return oldChild;
            }

            public void remove() {
                oldChild.getParent().removeChild(oldChild);
            }

        };
    }

    protected static boolean stringToBool(String s) {
        if (s.equals("true"))
            return true;
        else if (s.equals("false"))
            return false;
        else
            throw new RuntimeException("invalid attribute value  '"+s+"': should be 'true' or 'false'");
    }

    protected static String boolToString(boolean b) {
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

    public static String gateTypeToString(int gatetype) {
        return enumToString(gatetype, NedElementConstants.gatetype_vals, NedElementConstants.gatetype_nums, NedElementConstants.gatetype_n);
    }

    public static String parTypeToString(int partype) {
        return enumToString(partype, NedElementConstants.partype_vals, NedElementConstants.partype_nums, NedElementConstants.partype_n);
    }

    public static String subGateTypeToString(int subgate) {
        return enumToString(subgate, NedElementConstants.subgate_vals, NedElementConstants.subgate_nums, NedElementConstants.subgate_n);
    }

    /**
     * Sets the default NED type resolver. May only be invoked once.
     */
    public static void setDefaultNedTypeResolver(INedTypeResolver typeResolver) {
        Assert.isTrue(defaultNedTypeResolver == null);
        defaultNedTypeResolver = typeResolver;
    }

    /**
     * Returns the default NED type resolver. Guaranteed to be non-null.
     */
    public static INedTypeResolver getDefaultNedTypeResolver() {
        Assert.isTrue(defaultNedTypeResolver != null); // must have been set previously
        return defaultNedTypeResolver;
    }

    /**
     * Resolves the given type name using the default NED type resolver,
     * or returns null if the given string is null or "".
     */
    public static INedTypeInfo resolveTypeName(String typeName, INedTypeLookupContext context) {
        return StringUtils.isEmpty(typeName) ? null : context.getResolver().lookupNedType(typeName, context);
    }

    /**
     * Sets the default Msg type resolver. May only be invoked once.
     */
    public static void setDefaultMsgTypeResolver(IMsgTypeResolver typeResolver) {
        Assert.isTrue(defaultMsgTypeResolver == null);
        defaultMsgTypeResolver = typeResolver;
    }

    /**
     * Returns the default Msg type resolver. Guaranteed to be non-null.
     */
    public static IMsgTypeResolver getDefaultMsgTypeResolver() {
        Assert.isTrue(defaultMsgTypeResolver != null); // must have been set previously
        return defaultMsgTypeResolver;
    }

    /**
     * Constructor
     */
    public NedElement() {
        id = ++lastid;
    }

    /**
     * Constructor. Takes parent element.
     */
    public NedElement(INedElement parent) {
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

    public NedSourceRegion getSourceRegion() {
        return srcregion;
    }

    public void setSourceRegion(NedSourceRegion region) {
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

    public INedElement getParent() {
        return parent;
    }

    public INedElement getChild(int index) {
        ensureCachedChildArray();
        return cachedChildArray[index];
    }

    private void ensureCachedChildArray() {
        if (cachedChildArray == null) {
            cachedChildArray = new INedElement[numChildren];
            int i = 0;
            for (INedElement child : this)
                cachedChildArray[i++] = child;
            Assert.isTrue(i == numChildren, "cached numChildren is off");
        }
    }

    public INedElement getFirstChild() {
        return firstchild;
    }

    public INedElement getLastChild() {
        return lastchild;
    }

    public INedElement getNextSibling() {
        return nextsibling;
    }

    public INedElement getPrevSibling() {
        return prevsibling;
    }

    public void appendChild(INedElement inode) {
        NedElement node = (NedElement) inode;
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
        numChildren++;
        cachedChildArray = null;
        fireChildInsertedBefore(node,null);
    }

    public void insertChildBefore(INedElement iwhere, INedElement inode) {
        if (iwhere == null) {
            appendChild(inode);
            return;
        }
        Assert.isTrue(iwhere.getParent() == this);
        NedElement node = (NedElement) inode;
        NedElement where = (NedElement) iwhere;
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
        numChildren++;
        cachedChildArray = null;
        fireChildInsertedBefore(node, where);
    }

    public INedElement removeChild(INedElement inode) {
        Assert.isTrue(inode.getParent() == this);
        NedElement node = (NedElement) inode;
        if (node.prevsibling!=null)
            node.prevsibling.nextsibling = node.nextsibling;
        else
            firstchild = node.nextsibling;
        if (node.nextsibling!=null)
            node.nextsibling.prevsibling = node.prevsibling;
        else
            lastchild = node.prevsibling;
        node.parent = node.prevsibling = node.nextsibling = null;
        numChildren--;
        cachedChildArray = null;
        fireChildRemoved(node);
        return node;
    }

    public void removeAllChildren() {
        while (firstchild != null)
            removeChild(firstchild);
    }

    public List<INedElement> getChildren() {
        ensureCachedChildArray();
        return Arrays.asList(cachedChildArray);
    }

    public INedElement getFirstChildWithTag(int tagcode) {
        INedElement node = firstchild;
        while (node!=null)
        {
            if (node.getTagCode()==tagcode)
                return node;
            node = node.getNextSibling();
        }
        return null;
    }

    public INedElement getNextSiblingWithTag(int tagcode) {
        INedElement node = this.nextsibling;
        while (node!=null)
        {
            if (node.getTagCode()==tagcode)
                return node;
            node = node.getNextSibling();
        }
        return null;
    }

    public List<INedElement> getChildrenWithTag(int tagcode) {
        List<INedElement> result = new ArrayList<INedElement>();
        for (INedElement child = getFirstChildWithTag(tagcode); child != null; child = child.getNextSiblingWithTag(tagcode))
            result.add(child);
        return result;
    }

    public int getNumChildren() {
        return numChildren;
    }

    public int getNumChildrenWithTag(int tagcode) {
        int n = 0;
        for (INedElement node = firstchild; node!=null; node = node.getNextSibling())
            if (node.getTagCode()==tagcode)
                n++;
        return n;
    }

    public INedElement getFirstChildWithAttribute(int tagcode, String attr, String attrvalue) {
        for (INedElement child=getFirstChildWithTag(tagcode); child!=null; child = child.getNextSiblingWithTag(tagcode))
        {
            String val = child.getAttribute(attr);
            if (val!=null && val.equals(attrvalue))
                return child;
        }
        return null;
    }

    public INedElement getSelfOrAncestorWithTag(int tagcode) {
        INedElement node = this;
        while (node!=null && node.getTagCode()!=tagcode)
            node = node.getParent();
        return node;
    }

    public INedElement findElementWithId(long id) {
        if (getId() == id)
            return this;
        for (INedElement child : this) {
            INedElement node = child.findElementWithId(id);
            if (node != null)
                return node;
        }
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
        return NedModelLabelProvider.getInstance().getText(this) + " from " + getSourceLocation();
    }

    public INedElement dup() {
        return dup(getDefaultNedTypeResolver(), false, false);
    }

    public INedElement dup(INedTypeResolver targetResolver, boolean rememberOriginal, boolean cloneId) {
        INedElement clone = NedElementFactoryEx.getInstance().createElement(targetResolver, getTagCode());

        if (rememberOriginal)
            ((NedElement)clone).original = this;

        if (cloneId)
            clone.setId(getId());

        for (int i = 0; i < getNumAttributes(); ++i)
            clone.setAttribute(i, getAttribute(i));

        clone.setSourceLocation(getSourceLocation());
        clone.setSourceRegion(getSourceRegion());
        clone.setSyntaxProblemMaxLocalSeverity(getSyntaxProblemMaxLocalSeverity());
        clone.setConsistencyProblemMaxLocalSeverity(getConsistencyProblemMaxLocalSeverity());

        return clone;
    }

    public INedElement deepDup() {
        return deepDup(getDefaultNedTypeResolver(), false, false);
    }

    public INedElement deepDup(INedTypeResolver targetResolver, boolean rememberOriginal, boolean cloneId) {
        INedElement result = dup(targetResolver, rememberOriginal, cloneId);

        for (INedElement child : this)
            result.appendChild(child.deepDup(targetResolver, rememberOriginal, cloneId));

        return result;
    }

    public INedElement getOriginal() {
        return original;
    }

    public INedTypeElement getEnclosingTypeElement() {
        INedElement node = getParent();
        while (node != null && !(node instanceof INedTypeElement))
            node = node.getParent();
        return (INedTypeElement) node;
    }

    public INedTypeElement getSelfOrEnclosingTypeElement() {
        INedElement node = this;
        while (node != null && !(node instanceof INedTypeElement))
            node = node.getParent();
        return (INedTypeElement) node;
    }

    public NedFileElementEx getContainingNedFileElement() {
        return (NedFileElementEx) getSelfOrAncestorWithTag(NedElementTags.NED_NED_FILE);
    }

    public MsgFileElementEx getContainingMsgFileElement() {
        return (MsgFileElementEx) getSelfOrAncestorWithTag(NedElementTags.NED_MSG_FILE);
    }

    public INedTypeLookupContext getEnclosingLookupContext() {
        INedElement node = getParent();
        while (node != null && !(node instanceof INedTypeLookupContext))
            node = node.getParent();
        return (INedTypeLookupContext) node;
    }

    /**
     * Returns the listener list attached to this element
     */
    protected NedChangeListenerList getListeners() {
        if (listeners == null)
            listeners = new NedChangeListenerList();
        return listeners;
    }

    public void addNedChangeListener(INedChangeListener listener) {
        getListeners().add(listener);
    }

    public void removeNedChangeListener(INedChangeListener listener) {
        getListeners().remove(listener);
    }

    public void fireModelEvent(NedModelEvent event) {
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
        fireModelEvent(new NedAttributeChangeEvent(this, attr, newValue, oldValue));
    }

    /**
     * Notifies the listeners of all parents up to the root that the child was
     * inserted into this element.
     */
    protected void fireChildInsertedBefore(INedElement child, INedElement where) {
        fireModelEvent(new NedStructuralChangeEvent(this, child, NedStructuralChangeEvent.Type.INSERTION, where, null));
    }

    /**
     * Notifies the listeners of all parents up to the root that the child was
     * removed from this element.
     */
    protected void fireChildRemoved(INedElement child) {
        fireModelEvent(new NedStructuralChangeEvent(this, child, NedStructuralChangeEvent.Type.REMOVAL, null, child.getNextSibling()));
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.model.interfaces.IModelProvider#getNEDModel()
     */
    public INedElement getModel() {
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
        for (INedElement child : nedTypeElement)
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
        CommentElement comment = (CommentElement)getFirstChildWithAttribute(NedElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "banner");
        if (comment == null)
            comment = (CommentElement)getFirstChildWithAttribute(NedElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "right");
        return comment == null ? null : comment.getContent().trim();
    }

    public String getNedSource() {
        if (source == null)
            source = NedTreeUtil.generateSource(this, true);

        return source;
    }

    /* problem markers */

    public void clearSyntaxProblemMarkerSeverities() {
        if (syntaxProblemMaxLocalSeverity != SEVERITY_NONE) {
            syntaxProblemMaxLocalSeverity = SEVERITY_NONE;
            fireModelEvent(new NedMarkerChangeEvent(this));
        }
        for (INedElement child : this)
            child.clearSyntaxProblemMarkerSeverities();
    }

    public void clearConsistencyProblemMarkerSeverities() {
        if (consistencyProblemMaxLocalSeverity != SEVERITY_NONE) {
            consistencyProblemMaxLocalSeverity = SEVERITY_NONE;
            fireModelEvent(new NedMarkerChangeEvent(this));
        }
        for (INedElement child : this)
            child.clearConsistencyProblemMarkerSeverities();
    }

    static {
        // code below exploits numeric order of severities, lets assert it
        Assert.isTrue(SEVERITY_NONE < IMarker.SEVERITY_WARNING && IMarker.SEVERITY_WARNING < IMarker.SEVERITY_ERROR);
    }

    public void syntaxProblemMarkerAdded(int severity) {
        if (syntaxProblemMaxLocalSeverity < severity) {
            syntaxProblemMaxLocalSeverity = severity;
            fireModelEvent(new NedMarkerChangeEvent(this));
        }
    }

    public void consistencyProblemMarkerAdded(int severity) {
        if (consistencyProblemMaxLocalSeverity < severity) {
            consistencyProblemMaxLocalSeverity = severity;
            fireModelEvent(new NedMarkerChangeEvent(this));
        }
    }

    public void setSyntaxProblemMaxLocalSeverity(int severity) {
        if (syntaxProblemMaxLocalSeverity != severity) {
            syntaxProblemMaxLocalSeverity = severity;
            fireModelEvent(new NedMarkerChangeEvent(this));
        }
    }

    public void setConsistencyProblemMaxLocalSeverity(int severity) {
        if (consistencyProblemMaxLocalSeverity != severity) {
            consistencyProblemMaxLocalSeverity = severity;
            fireModelEvent(new NedMarkerChangeEvent(this));
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
        int syntaxSeverity = syntaxProblemMaxLocalSeverity;
        int consistencySeverity = consistencyProblemMaxLocalSeverity;
        for (INedElement child : this) {
            syntaxSeverity = Math.max(syntaxSeverity, child.getSyntaxProblemMaxCumulatedSeverity());
            consistencySeverity = Math.max(consistencySeverity, child.getConsistencyProblemMaxCumulatedSeverity());
        }
        syntaxProblemMaxCumulatedSeverity = syntaxSeverity;
        consistencyProblemMaxCumulatedSeverity = consistencySeverity;
    }

    // For debugging purposes only
    @Override
    public String toString() {
        //return NedTreeUtil.getNedModelLabelProvider().getText(this) + " from " + getSourceLocation();
        return getClass().getSimpleName() + " " + (hasAttribute("name") ? getAttribute("name") : "");
        //return NedTreeUtil.generateXmlFromPojoElementTree(this, "  ");
    }

}

