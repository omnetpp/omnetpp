package org.omnetpp.ide;

import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.IDocument;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.editors.text.TextFileDocumentProvider;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.ned.editor.graph.ModuleEditor;
import org.omnetpp.ned.editor.text.NedEditor;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.swig.NEDElement;

public class GraphAndTextEditor extends MultiPageEditorPart implements
		IResourceChangeListener {

	private ModuleEditor graphEditor;
	private NedEditor nedEditor;
	// contains the edited ned file's parsed tree
	private NEDElement nedModel = null;
	private int graphPageIndex;
	private int textPageIndex;
	
	public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		super.init(site, editorInput);
		setPartName(editorInput.getName());
		// load and parse the editor's input ned file
		loadDoc(editorInput);
	}
	
	// open ned file and parse it to crate the document model
	private void loadDoc(IEditorInput editorInput) {
		IFileEditorInput fileInput = (IFileEditorInput)editorInput;
		try {
			// FIXME do it simpler if possible
			String filename = fileInput.getFile().getLocation().toFile().getPath();
			System.out.println(filename);
			nedModel = ModelUtil.loadNedSource(filename);
			
		} catch (Exception e) {
			MessageDialog.openError(new Shell(), "Error opening file",
					"Error opening file "+fileInput.getName()+": "+e.getMessage());
		}
	}
	
	@Override
	protected void createPages() {
		graphEditor = new ModuleEditor();
		nedEditor = new NedEditor();
		
		try {
			graphPageIndex = addPage(graphEditor, getEditorInput());
			setPageText(graphPageIndex,"Graphical");
			textPageIndex = addPage(nedEditor, getEditorInput());
			setPageText(textPageIndex,"Text");
		} catch (PartInitException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
	@Override
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
		if (newPageIndex == textPageIndex) {
			String textEditorContent = ModelUtil.generateNedSource(nedModel);
			TextDocument td = new TextDocument();
			IDocument editorDoc = 
				((TextFileDocumentProvider)nedEditor.getDocumentProvider()).getDocument(getEditorInput());
			editorDoc.set(textEditorContent);
				
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
