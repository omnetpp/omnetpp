package org.omnetpp.scave2.editors.providers;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public abstract class InputsTreeViewProvider {
	
	private static final Object[] EMPTY_ARRAY = new Object[0];

	private ScaveEditor editor;
	
	public InputsTreeViewProvider(ScaveEditor editor) {
		this.editor = editor;
	}
	
	public void configureTreeViewer(TreeViewer viewer) {
		viewer.setContentProvider(getContentProvider());
		viewer.setLabelProvider(getLabelProvider());
	}

	public abstract ITreeContentProvider getContentProvider();
	public abstract ILabelProvider getLabelProvider();
	

	protected abstract class ContentProvider implements ITreeContentProvider, INotifyChangedListener {
		
		protected TreeNode root;
		protected Viewer viewer;
		
		public ScaveEditor getEditor() {
			return editor;
		}
		
		protected abstract void buildTree(Inputs inputs);
		
		public Object[] getChildren(Object parentElement) {
			if (parentElement instanceof TreeNode)
				return ((TreeNode)parentElement).children;
			else if (parentElement instanceof Inputs)
				return root.children; // FIXME: parent of the children is a TreeNode, not the Inputs!
			return EMPTY_ARRAY;
		}
	
		public Object getParent(Object element) {
			if (element instanceof TreeNode)
				return ((TreeNode)element).parent;
			return null;
		}
	
		public boolean hasChildren(Object element) {
			return getChildren(element).length > 0;
		}
	
		public Object[] getElements(Object inputElement) {
			return getChildren(inputElement);
		}
		
		public void dispose() {
			root = null;
		}
		
		public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			this.viewer = viewer;
			if (oldInput instanceof Inputs)
				removeListener((Inputs)oldInput);
			rebuildTree(newInput);
			if (newInput instanceof Inputs)
				addListener((Inputs)newInput);
		}
		
		public void notifyChanged(Notification notification) {
			if (notification.isTouch())
				return;
			rebuildTree(root.payload);
			if (viewer != null && viewer.getControl() != null && !viewer.getControl().isDisposed())
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
		
		private void rebuildTree(Object input) {
			root = null;
			if (input != null && input instanceof Inputs)
				buildTree((Inputs)input);
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
	
	protected static class LabelProvider implements ILabelProvider {

		public Image getImage(Object element) {
			return null;
		}

		public String getText(Object element) {
			return null;
		}

		public void addListener(ILabelProviderListener listener) {
		}

		public void dispose() {
		}

		public boolean isLabelProperty(Object element, String property) {
			return false;
		}

		public void removeListener(ILabelProviderListener listener) {
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
