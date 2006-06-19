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

//XXX remove class
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
	
}
