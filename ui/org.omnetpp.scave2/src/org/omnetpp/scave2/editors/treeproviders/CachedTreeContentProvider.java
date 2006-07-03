package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.jface.viewers.Viewer;

/**
 * A specific tree content provider: displays a tree generated from 
 * an input element. The tree is cached, and gets re-generated on 
 * change notifications. EMF's IChangeNotifier interface is implemented 
 * for this purpose. 
 */
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
