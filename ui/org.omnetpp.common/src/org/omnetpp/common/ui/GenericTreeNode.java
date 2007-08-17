package org.omnetpp.common.ui;

import java.util.Arrays;
import java.util.Comparator;

import org.eclipse.core.runtime.Assert;

/**
 * Node for a generic tree where every node contains a payload object.
 * We use this with GenericTreeContentProvider to display an arbitrary 
 * object tree in a TreeViewer.
 * 
 * @author andras
 */
public class GenericTreeNode {
	private static final GenericTreeNode[] EMPTY_ARRAY = new GenericTreeNode[0];

	private GenericTreeNode parent;
	private GenericTreeNode[] children;
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
		this.children = EMPTY_ARRAY;
		this.childOrder = childOrder != null ? new PayloadComparator(childOrder) : null;
	}

	/**
	 * Add a child node to this node.
	 * @param child may NOT be null
	 */
	public void addChild(GenericTreeNode child) {
		if (child.parent!=null)
			throw new RuntimeException("child node already has a parent");
		
		GenericTreeNode[] childrenNew = new GenericTreeNode[children.length + 1];
		if (childOrder == null) {
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

	/**
	 * Returns the children. The result is never null. 
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
	 * Returns the index of this node within its parent. 
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
		return getOrCreateChild(payload, null);
	}
	
	/**
	 * Adds a child node with the given payload and child order if it not already exists.
	 */
	public GenericTreeNode getOrCreateChild(Object payload, Comparator<? extends Object> childOrder) {
		if (children != null) {
			for (int i = 0; i < children.length; ++i) {
				GenericTreeNode child = children[i];
				if (child.payload.equals(payload))
					return child;
			}
		}
		
		GenericTreeNode child = new GenericTreeNode(payload, childOrder);
		addChild(child);
		
		return child;
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
		return node.payload==payload || node.payload.equals(payload);
	}
}
