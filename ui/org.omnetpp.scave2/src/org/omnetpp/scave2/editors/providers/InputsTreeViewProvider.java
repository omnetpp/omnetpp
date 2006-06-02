package org.omnetpp.scave2.editors.providers;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public abstract class InputsTreeViewProvider {
	
	private static final Object[] EMPTY_ARRAY = new Object[0];

	protected ScaveEditor editor;
	
	public InputsTreeViewProvider(ScaveEditor editor) {
		this.editor = editor;
	}
	
	public void configureTreeViewer(TreeViewer viewer) {
		viewer.setContentProvider(getContentProvider());
		viewer.setLabelProvider(getLabelProvider());
	}

	/* Methods to be implemented by concrete subclasses. */
	protected abstract ITreeContentProvider getContentProvider();
	protected abstract ILabelProvider getLabelProvider();
	

	protected abstract class ContentProvider implements ITreeContentProvider, INotifyChangedListener {
		
		protected Object lastInputElement;
		protected TreeNode root;
		protected Viewer viewer;
		
		protected abstract TreeNode buildTree(Inputs inputs);
		
		public Object[] getChildren(Object parentElement) {
			if (parentElement instanceof TreeNode)
				return ((TreeNode)parentElement).children;
			return EMPTY_ARRAY;
		}
	
		public Object getParent(Object element) {
			if (element instanceof TreeNode) {
				TreeNode node = (TreeNode)element;
				return node.parent == root ? null : node.parent;
			}
			return null;
		}
	
		public boolean hasChildren(Object element) {
			return getChildren(element).length > 0;
		}
	
		public Object[] getElements(Object inputElement) {
			if (inputElement instanceof Inputs) {
				if (inputElement != lastInputElement || root == null) {
					lastInputElement = inputElement;
					root = buildTree((Inputs)inputElement);
				}
				
				if (root != null)
					return root.children;
			}
			return EMPTY_ARRAY;
		}
		
		public void dispose() {
			viewer = null;
		}
		
		/* Notification from the viewer that its input has changed.*/
		public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			this.viewer = viewer;
			if (oldInput instanceof Inputs)
				removeListener((Inputs)oldInput);
			if (newInput instanceof Inputs)
				addListener((Inputs)newInput);
		}
		
		public void notifyChanged(Notification notification) {
			if (notification.isTouch())
				return;
			root = null;
			if (viewer != null)
				viewer.refresh();
		}
		
		private void addListener(Inputs inputs) {
			AdapterFactory adapterFactory = editor.getAdapterFactory();
			if (adapterFactory instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
				notifier.addListener(this);
			}
		}
		
		private void removeListener(Inputs inputs) {
			AdapterFactory adapterFactory = editor.getAdapterFactory();
			if (adapterFactory instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
				notifier.removeListener(this);
			}
		}
		
		protected TreeNode getOrCreateNode(TreeNode parent, String payload) {
			if (parent.children != null)
				for (int i = 0; i < parent.children.length; ++i) {
					TreeNode child = parent.children[i];
					if (InputsTreeViewProvider.equals(payload, child.payload))
						return child;
				}
			
			TreeNode child = new TreeNode(parent, payload);
			parent.addChild(child);
			
			return child;
		}
	}
	
	public static boolean equals(Object first, Object second) {
		return first == null && second == null ||
				first != null && first.equals(second);
	}

	protected static class TreeNode {
		private static final TreeNode[] EMPTY_ARRAY = new TreeNode[0];
		
		public TreeNode parent;
		public TreeNode[] children;
		public Object payload;
		
		public TreeNode(TreeNode parent, Object payload) {
			this.parent = parent;
			this.payload = payload;
			this.children = EMPTY_ARRAY;
		}
		
		public void addChild(TreeNode child) {
			TreeNode[] childrenNew = new TreeNode[children.length + 1];
			System.arraycopy(children, 0, childrenNew, 0, children.length);
			children = childrenNew;
			children[children.length - 1] = child;
		}
		
		public int index() {
			if (parent == null)
				return -1;
			TreeNode[] siblings = parent.children;
			for (int i = 0; i < siblings.length; ++i)
				if (siblings[i] == this)
					return i;
			return -1;
		}
	}
}
