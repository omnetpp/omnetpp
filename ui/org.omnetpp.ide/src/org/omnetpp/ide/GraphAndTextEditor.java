package org.omnetpp.ide;

import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.ned.editor.graph.ModuleEditor;
import org.omnetpp.ned.editor.graph.model.NedFileNodeEx;
import org.omnetpp.ned.editor.text.NedEditor;
import org.omnetpp.ned2.model.ModelUtil;

public class GraphAndTextEditor extends MultiPageEditorPart implements
		IResourceChangeListener {

	private ModuleEditor graphEditor;
	private NedEditor nedEditor;
	private int graphPageIndex;
	private int textPageIndex;
	
	public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		super.init(site, editorInput);
		setPartName(editorInput.getName());
	}
	
	// open ned file and parse it to crate the document model
	private NedFileNodeEx loadDoc(IEditorInput editorInput) {
		IFileEditorInput fileInput = (IFileEditorInput)editorInput;
		try {
			// FIXME do it simpler if possible
			String filename = fileInput.getFile().getLocation().toFile().getPath();
			return (NedFileNodeEx)ModelUtil.loadNedSource(filename);
		} catch (Exception e) {
			MessageDialog.openError(new Shell(), "Error opening file",
					"Error opening file "+fileInput.getName()+": "+e.getMessage());
			e.printStackTrace();
		}
		return null;
	}
	
	@Override
	protected void createPages() {
		graphEditor = new ModuleEditor();
		nedEditor = new NedEditor();
		
		try {
			// fill graphical editor
			// load and parse the editor's input ned file
			NedFileNodeEx modelRoot = loadDoc(getEditorInput());
			graphEditor.setModel(modelRoot);
			graphPageIndex = addPage(graphEditor, getEditorInput());
			setPageText(graphPageIndex,"Graphical");

			// fill text editor
			textPageIndex = addPage(nedEditor, getEditorInput());
			setPageText(textPageIndex,"Text");
			// don't fill the text editor, because the graph editor is the default one
		} catch (PartInitException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
	@Override
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
		if (newPageIndex == textPageIndex) {
			// generate text representation from the model
			NedFileNodeEx modelRoot = graphEditor.getModel();
			String textEditorContent = ModelUtil.generateNedSource(modelRoot);
			// put it into the text editor
			nedEditor.setText(textEditorContent);
		} else if (newPageIndex == graphPageIndex) {
			NedFileNodeEx modelRoot = (NedFileNodeEx)ModelUtil.parseNedSource(nedEditor.getText());
			graphEditor.setModel(modelRoot);
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
