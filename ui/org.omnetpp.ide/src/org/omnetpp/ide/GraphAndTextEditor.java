package org.omnetpp.ide;

import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.ned.editor.graph.ModuleEditor;
import org.omnetpp.ned.editor.text.NedEditor;

public class GraphAndTextEditor extends MultiPageEditorPart implements
		IResourceChangeListener {

	private ModuleEditor graphEditor;
	private NedEditor nedEditor;
	
	public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		super.init(site, editorInput);
		setPartName(editorInput.getName());
	}
	
	
	@Override
	protected void createPages() {
		graphEditor = new ModuleEditor();
		nedEditor = new NedEditor();
		
		
		try {
			int index = addPage(graphEditor, getEditorInput());
			setPageText(index,"Graphical");
			index = addPage(nedEditor, getEditorInput());
			setPageText(index,"Text");
		} catch (PartInitException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		graphEditor.doSave(monitor);
		nedEditor.doSave(monitor);
	}

	@Override
	public void doSaveAs() {
		// TODO open SaveAs dialog
		graphEditor.doSaveAs();
		nedEditor.doSaveAs();
	}

	@Override
	public boolean isSaveAsAllowed() {
		return true;
	}

	public void resourceChanged(IResourceChangeEvent event) {
		// TODO Auto-generated method stub

	}

}
