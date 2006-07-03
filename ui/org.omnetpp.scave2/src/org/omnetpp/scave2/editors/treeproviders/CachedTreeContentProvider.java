package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.jface.viewers.Viewer;

/**
 * A specific tree content provider: displays a GenericTreeNode tree 
 * generated from an input element by the buildTree() method which
 * has to be provided by the user. The resulting tree is cached, so
 * buildTree() is only called once. Refresh (discarding and rebuilding
 * the tree) can be triggered by calling setInput() on the viewer.
 */
//XXX this is reusable and does not reference any Scave stuff -- move to the common plug-in!
public abstract class CachedTreeContentProvider extends GenericTreeContentProvider {
	private GenericTreeNode root;

	/**
	 * Rebuilds the tree
	 */
	public abstract GenericTreeNode buildTree(Object element);

	@Override 
	public void dispose() {
	}

	@Override 
	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		root = null; // force rebuild
	}

	/**
	 * Maps the input element to the cached GenericTreeNode tree.
	 */
	@Override
	public Object[] getElements(Object inputElement) {
		if (root==null)
			root = buildTree(inputElement);
		return root.getChildren();
	}
}
