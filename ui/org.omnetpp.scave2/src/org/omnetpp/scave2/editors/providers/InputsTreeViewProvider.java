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

//XXX some docu needed for this class
public abstract class InputsTreeViewProvider {
	
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
	
	protected abstract class ContentProvider extends GenericTreeContentProvider implements INotifyChangedListener {
		
		protected GenericTreeNode root;
		protected Viewer viewer;
		
		protected abstract GenericTreeNode buildTree(Inputs inputs);
		
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

		@Override
		public Object[] getElements(Object inputElement) {
			if (inputElement instanceof Inputs) {
				if (root==null)
					root = buildTree((Inputs)inputElement);
				return root.getChildren();
			}
			return super.getElements(inputElement);
		}
	}
	
	public static boolean equals(Object first, Object second) {
		return first == null && second == null ||
				first != null && first.equals(second);
	}

}
