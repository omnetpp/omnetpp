package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.Viewer;

/**
 * A specific tree content provider: displays a tree generated from 
 * an input element. The tree is cached, and gets re-generated on 
 * change notifications. EMF's IChangeNotifier interface is implemented 
 * for this purpose. 
 */
//XXX move to common plug-in
//FIXME tree has to be rebuilt upon resource changes as well! ResultFileTracker should fire some notification
public abstract class CachedTreeContentProvider extends GenericTreeContentProvider implements INotifyChangedListener {

	private IChangeNotifier notifier;
	private GenericTreeNode root;
	private Viewer viewer;

	public CachedTreeContentProvider(IChangeNotifier notifier) {
		this.notifier = notifier;
	}

	/**
	 * Rebuilds the tree
	 */
	protected abstract GenericTreeNode buildTree(Object element);

	@Override 
	public void dispose() {
		viewer = null;
	}

	/**
	 * Notification from the viewer that its input has changed.
	 * Stop listening to the old input's changes, and start listening
	 * to the new one's.
	 */
	@Override 
	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		this.viewer = viewer;
		if (oldInput!=null)
			notifier.removeListener(this);
		if (newInput!=null)
			notifier.addListener(this);
	}

	/**
	 * Receive EMF notifications, and rebuild tree if something has changed.
	 */
	//XXX narrow down notifications to relevant changes!
	public void notifyChanged(Notification notification) {
		if (notification.isTouch())
			return;
		root = null;
		if (viewer != null)
			viewer.refresh();
	}

	/**
	 * Maps the input element (which is an EMF model object) to the 
	 * cached GenericTreeNode tree.
	 */
	@Override
	public Object[] getElements(Object inputElement) {
		if (root==null)
			root = buildTree(inputElement);
		return root.getChildren();
	}
}
