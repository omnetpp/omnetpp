package org.omnetpp.ide;

import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.MessageBox;
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
	private boolean insidePageChange = false;
	
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
		
        //	prevent recursive call from setActivePage() below
		if (insidePageChange)
			return;
		insidePageChange = true;

		if (newPageIndex == textPageIndex) { 
			// switch from graphics to text:
			// generate text representation from the model
			NedFileNodeEx modelRoot = graphEditor.getModel();
			String textEditorContent = ModelUtil.generateNedSource(modelRoot, false);
			// put it into the text editor
			nedEditor.setText(textEditorContent);
		} 
		else if (newPageIndex == graphPageIndex) { 
			// switch from text to graphics
			NedFileNodeEx modelRoot = (NedFileNodeEx)ModelUtil.parseNedSource(nedEditor.getText());
			if (modelRoot!=null) {
				// give the backparsed model to the graphical editor 
				graphEditor.setModel(modelRoot);
			}
			else {
				// parse error: switch back immediately to text view (we should never have 
				// switched away from it in the first place)
				setActivePage(textPageIndex);
				
				// ask user what to do
		        MessageBox messageBox = new MessageBox(getEditorSite().getShell(), 
		        		                               SWT.ICON_WARNING | SWT.YES | SWT.NO);
		        messageBox.setText("Warning");
		        messageBox.setMessage("The editor contents has syntax errors, "+
		        		              "switching is only possible with losing text mode changes. "+
		        		              "Do you want to revert to graphics view (and lose your changes)?"); // XXX better dialog, with "Continue editing" and "Lose changes" buttons
		        int buttonID = messageBox.open();
		        if (buttonID==SWT.YES) {
					setActivePage(graphPageIndex);
		        }
			}
		}
		insidePageChange = false;
	}

	@Override
	public Object getAdapter(Class type) {
		Object adapter = getActiveEditor().getAdapter(type);
		if (adapter == null) 
			adapter = super.getAdapter(type);
		return adapter;
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
