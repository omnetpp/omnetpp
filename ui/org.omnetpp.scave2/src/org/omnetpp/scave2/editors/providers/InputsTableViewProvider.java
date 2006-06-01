package org.omnetpp.scave2.editors.providers;

import java.util.List;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public abstract class InputsTableViewProvider {

	protected ScaveEditor editor;
	
	public InputsTableViewProvider(ScaveEditor editor) {
		this.editor = editor;
	}
	
	public abstract IStructuredContentProvider getContentProvider();
	public abstract ITableLabelProvider getLabelProvider();
	
	abstract class ContentProvider implements IStructuredContentProvider, INotifyChangedListener {
		
		protected Inputs inputs;
		protected IDList idlist;
		protected Viewer viewer;
		
		public Object[] getElements(Object inputElement) {
			if (inputElement instanceof Inputs) {
				if (inputElement != inputs)
					rebuildIDList((Inputs)inputElement);
				if (idlist != null)
					return idlist.toArray();
			}
			return null;
		}
		
		protected abstract void buildIDList();

		public void dispose() {
		}

		public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			this.viewer = viewer;
			if (oldInput instanceof Inputs)
				removeListener((Inputs)oldInput);
			rebuildIDList(newInput);
			if (newInput instanceof Inputs) {
				addListener((Inputs)newInput);
			}
		}
		
		public void notifyChanged(Notification notification) {
			if (notification.isTouch())
				return;
			rebuildIDList(inputs);
			if (viewer != null && viewer.getControl() != null && !viewer.getControl().isDisposed())
				viewer.refresh();
		}
		
		public void addListener(Inputs inputs) {
			if (editor.getAdapterFactory() instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)editor.getAdapterFactory();
				notifier.addListener(this);
			}
		}
		
		public void removeListener(Inputs inputs) {
			if (editor.getAdapterFactory() instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)editor.getAdapterFactory();
				notifier.removeListener(this);
			}
		}
		
		private void rebuildIDList(Object input) {
			if (input instanceof Inputs) {
				inputs = (Inputs)input;
				idlist = null;
				buildIDList();
			}
		}
	}
	
	class LabelProvider implements ITableLabelProvider {

		public Image getColumnImage(Object element, int columnIndex) {
			return null;
		}

		public String getColumnText(Object element, int columnIndex) {
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
}
