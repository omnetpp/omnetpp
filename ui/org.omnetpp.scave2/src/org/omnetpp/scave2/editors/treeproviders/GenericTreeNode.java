package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.core.runtime.Assert;

/**
 * Node for a generic tree where every node contains a payload object.
 * We use this with GenericTreeContentProvider to display an abitrary 
 * object tree in a TreeViewer.
 * 
 * @author andras
 */
//XXX move to "common" plug-in?
public class GenericTreeNode {

	private static final GenericTreeNode[] EMPTY_ARRAY = new GenericTreeNode[0];

	private GenericTreeNode parent;
	private GenericTreeNode[] children;
	private Object payload;

	/**
	 * Constructs a new tree node with the given parent and payload.
	 * @param parent may be null if this is the root
	 * @param payload may NOT be null
	 */
	public GenericTreeNode(GenericTreeNode parent, Object payload) {
		//XXX this is shit!!!! does not add this node into parent's child list -- possible inconsistency!
		Assert.isTrue(payload!=null);
		this.parent = parent;
		this.payload = payload;
		this.children = EMPTY_ARRAY;
	}

	/**
	 * Add a child node to this node.
	 * @param child may NOT be null
	 */
	public void addChild(GenericTreeNode child) {
		//XXX shit shit shit!!! goes set child's parent field -- possible inconsistency!
		GenericTreeNode[] childrenNew = new GenericTreeNode[children.length + 1];
		System.arraycopy(children, 0, childrenNew, 0, children.length);  //XXX potential bottleneck -- use ArrayList? (Andras)
		children = childrenNew;
		children[children.length - 1] = child;
	}

	/**
	 * Returns the payload object.
	 */
	public Object getPayload() {
		return payload;
	}

	/**
	 * Sets the payload object.
	 */
	public void setPayload(Object payload) {
		Assert.isTrue(payload!=null);
		this.payload = payload;
	}

	/**
	 * Returns the chilren.
	 */
	public GenericTreeNode[] getChildren() {
		return children;
	}

	/**
	 * Returns the parent node.
	 */
	public GenericTreeNode getParent() {
		return parent;
	}

	/**
	 * Returns the index of this node within irs parent. 
	 * Returns -1 if this is the root node. 
	 */
	public int indexInParent() {
		if (parent == null)
			return -1;
		GenericTreeNode[] siblings = parent.children;
		for (int i = 0; i < siblings.length; ++i)
			if (siblings[i] == this)
				return i;
		throw new RuntimeException("tree inconsistency");
	}
	
	/**
	 * Adds a child node with the given payload if it not already exists.
	 */
	public GenericTreeNode getOrCreateChild(Object payload) {
		if (children != null) {
			for (int i = 0; i < children.length; ++i) {
				GenericTreeNode child = children[i];
				if (child.payload.equals(payload))
					return child;
			}
		}
		
		GenericTreeNode child = new GenericTreeNode(this, payload);
		addChild(child);
		
		return child;
	}
	
}
