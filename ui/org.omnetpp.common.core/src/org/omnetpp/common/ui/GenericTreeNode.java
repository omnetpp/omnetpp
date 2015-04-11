/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import java.util.Arrays;
import java.util.Comparator;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;

/**
 * Node for a generic tree where every node contains a payload object.
 * We use this with GenericTreeContentProvider to display an arbitrary
 * object tree in a TreeViewer.
 *
 * The children list of this node can be built by using the addChild() methods,
 * or lazily by overriding computeChildren().
 *
 * @author andras
 */
//XXX use org.eclipse.jface.viewers.TreeNode instead? (just discovered such thing exists)
public class GenericTreeNode {
    protected static final GenericTreeNode[] EMPTY_ARRAY = new GenericTreeNode[0];

    private GenericTreeNode parent;
    private GenericTreeNode[] children; // ==null, if not yet computed
    private Object payload;
    private Comparator<GenericTreeNode> childOrder; // order of the children (maybe null)

    @SuppressWarnings("unchecked")
    private static class PayloadComparator implements Comparator<GenericTreeNode> {
        private Comparator comparator;

        public PayloadComparator(Comparator<? extends Object> comparator) {
            this.comparator = comparator;
        }

        public int compare(GenericTreeNode left, GenericTreeNode right) {
            return comparator.compare(left.payload, right.payload);
        }
    }

    /**
     * Constructs a new tree node with the given payload.
     * @param payload may NOT be null
     */
    public GenericTreeNode(Object payload) {
        this(payload, null);
    }

    public GenericTreeNode(Object payload, Comparator<? extends Object> childOrder) {
        Assert.isTrue(payload!=null);
        this.payload = payload;
        this.children = null;
        this.childOrder = childOrder != null ? new PayloadComparator(childOrder) : null;
    }

    /**
     * Add a child node to this node.
     * @param child may NOT be null
     */
    public void addChild(GenericTreeNode child) {
        if (child.parent!=null)
            throw new RuntimeException("child node already has a parent");

        GenericTreeNode[] childrenNew = new GenericTreeNode[children == null ? 1 : children.length + 1];
        if (childOrder == null || childrenNew.length == 1) {
            if (children != null)
                System.arraycopy(children, 0, childrenNew, 0, children.length);  //XXX potential bottleneck -- use ArrayList? (Andras)
            childrenNew[childrenNew.length - 1] = child;
        }
        else {
            int insertionPoint = Arrays.binarySearch(children, child, childOrder);
            if (insertionPoint < 0)
                insertionPoint = - insertionPoint - 1;

            if (insertionPoint > 0)
                System.arraycopy(children, 0, childrenNew, 0, insertionPoint);
            childrenNew[insertionPoint] = child;
            if (insertionPoint < children.length)
                System.arraycopy(children, insertionPoint, childrenNew, insertionPoint + 1, children.length - insertionPoint);

        }

        child.parent = this;
        children = childrenNew;
    }

    public void addChild(int index, GenericTreeNode child) {
        Assert.isTrue(childOrder == null);
        children = (GenericTreeNode[])ArrayUtils.add(children, index, child); // children == null is accepted by ArrayUtils.add().
        child.parent = this;
    }

    /**
     * Returns the payload object, which cannot be null.
     */
    public Object getPayload() {
        return payload;
    }

    /**
     * Sets the payload object. null is not accepted.
     */
    public void setPayload(Object payload) {
        Assert.isTrue(payload!=null);
        this.payload = payload;
    }

    public int getChildCount() {
        ensureChildren();
        return children.length;
    }

    public boolean hasChildren() {
        ensureChildren();
        return children.length > 0;
    }

    /**
     * Returns the children. The result is never null.
     */
    public GenericTreeNode[] getChildren() {
        ensureChildren();
        return children;
    }

    protected GenericTreeNode[] internalGetChildren() {
        return children;
    }

    public GenericTreeNode getChild(int index) {
        ensureChildren();
        Assert.isLegal(0 <= index && index < children.length);
        return children[index];
    }

    /**
     * Returns the parent node.
     */
    public GenericTreeNode getParent() {
        return parent;
    }

    /**
     * Returns the index of this node within its parent.
     * Returns -1 if this is the root node.
     */
    public int indexInParent() {
        if (parent == null)
            return -1;
        GenericTreeNode[] siblings = parent.children;
        Assert.isNotNull(siblings);
        for (int i = 0; i < siblings.length; ++i)
            if (siblings[i] == this)
                return i;
        throw new RuntimeException("tree inconsistency");
    }

    /**
     * Adds a child node with the given payload if it not already exists.
     */
    public GenericTreeNode getOrCreateChild(Object payload) {
        return getOrCreateChild(payload, null);
    }

    /**
     * Adds a child node with the given payload and child order if it not already exists.
     */
    public GenericTreeNode getOrCreateChild(Object payload, Comparator<? extends Object> childOrder) {
        ensureChildren();
        for (int i = 0; i < children.length; ++i) {
            GenericTreeNode child = children[i];
            if (child.payload.equals(payload))
                return child;
        }

        GenericTreeNode child = new GenericTreeNode(payload, childOrder);
        addChild(child);

        return child;
    }

    public void unlink() {
        Assert.isLegal(parent != null);
        Assert.isNotNull(parent.children);
        parent.children = (GenericTreeNode[])ArrayUtils.remove(parent.children, indexInParent());
        parent = null;
    }

    public GenericTreeNode findPayload(Object payload) {
        if (this.payload==payload || this.payload.equals(payload))
            return this;
        GenericTreeNode result;
        ensureChildren();
        for (GenericTreeNode child : children)
            if ((result=child.findPayload(payload)) != null)
                return result;
        return null;
    }

    /**
     * Clones this node and its descendants recursively.
     * The comparators and the payloads are not cloned though.
     * The parent of the returned node is always null.
     *
     * If there are any uncomputed child list in the tree,
     * they are computed before cloning.
     */
    public GenericTreeNode cloneTree() {
        ensureChildren();
        GenericTreeNode node = new GenericTreeNode(payload, childOrder);
        for (GenericTreeNode child : children)
            node.addChild(child.cloneTree());
        return node;
    }

    /**
     * Ensures that the {@code children} field is filled in
     * by calling computeChildren() when needed.
     */
    private void ensureChildren() {
        if (children == null) {
            children = computeChildren();
            Assert.isNotNull(children);
        }
    }

    /**
     * Override this method if the children are computed on demand.
     *
     * This method is not allowed to return null.
     */
    protected GenericTreeNode[] computeChildren() {
        return EMPTY_ARRAY;
    }

    /**
     * Delegates to payload's toString().
     */
    @Override
    public String toString() {
        return payload.toString();
    }

    /**
     * Compares payloads only (of two GenericTreeNodes)
     */
    @Override
    public boolean equals(Object obj) {
        if (obj==null || getClass()!=obj.getClass())
            return false;
        if (this==obj)
            return true;
        GenericTreeNode node = (GenericTreeNode)obj;
        return ObjectUtils.equals(node.payload, payload);
    }

    @Override
    public int hashCode() {
        return payload==null ? super.hashCode() : payload.hashCode();
    }
}
