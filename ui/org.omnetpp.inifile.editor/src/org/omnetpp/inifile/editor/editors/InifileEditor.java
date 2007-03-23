package org.omnetpp.inifile.editor.editors;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.text.IDocument;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.inifile.editor.model.InifileDocument;
import org.omnetpp.inifile.editor.text.InifileTextEditor;

/**
 * Editor for omnetpp.ini files.
 */
//FIXME File|Revert is always diabled
//FIXME outline view doesn't get displayed - move creation from TextEditor into here
public class InifileEditor extends MultiPageEditorPart implements IResourceChangeListener, IGotoMarker {
	/** The text editor */
	private InifileTextEditor textEditor;
	
	private InifileEditorFormPage formEditor;

	/** The data model */
	private InifileEditorData editorData = new InifileEditorData();
	
	/**
	 * Creates the ini file editor.
	 */
	public InifileEditor() {
		super();
		ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
	}
 
	public InifileEditorData getEditorData() {
		return editorData;
	}
	
	/**
	 * Creates the text editor page of the multi-page editor.
	 */
	void createTextEditorPage() {
		try {
			textEditor = new InifileTextEditor(this);
			int index = addPage(textEditor, getEditorInput());
			setPageText(index, "Text");
		} catch (PartInitException e) {
			ErrorDialog.openError(getSite().getShell(), "Error creating nested text editor", null, e.getStatus());
		}
	}

	/**
	 * Creates the pages of the multi-page editor.
	 */
	@Override
	protected void createPages() {
		// create form page
		formEditor = new InifileEditorFormPage(getContainer(), this);
		addEditorPage(formEditor, "Form");

		// create texteditor
		createTextEditorPage();
		
		IFile file = ((IFileEditorInput)getEditorInput()).getFile();
		IDocument document = textEditor.getDocumentProvider().getDocument(getEditorInput());
		editorData.setInifiledocument(new InifileDocument(document, file));
	}

	/**
	 * Adds an editor page at the last position.
	 */
	public int addEditorPage(Control page, String label) {
		int index = addPage(page);
		setPageText(index, label);
		return index;
	}

	/**
	 * The <code>MultiPageEditorPart</code> implementation of this 
	 * <code>IWorkbenchPart</code> method disposes all nested editors.
	 * Subclasses may extend.
	 */
	@Override
	public void dispose() {
		ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
		super.dispose();
	}

	/**
	 * Saves the multi-page editor's document.
	 */
	@Override
	public void doSave(IProgressMonitor monitor) {
		textEditor.doSave(monitor);
	}
	
	/**
	 * Saves the multi-page editor's document as another file.
	 * Also updates the text for page 0's tab, and updates this multi-page editor's input
	 * to correspond to the nested editor's.
	 */
	@Override
	public void doSaveAs() {
		textEditor.doSaveAs();
		setInput(textEditor.getEditorInput());
	}
	
	/* (non-Javadoc)
	 * Method declared on IGotoMarker
	 */
	public void gotoMarker(IMarker marker) {
		setActivePage(0); //XXX
		IDE.gotoMarker(textEditor, marker);
	}

	/**
	 * The <code>MultiPageEditorExample</code> implementation of this method
	 * checks that the input is an instance of <code>IFileEditorInput</code>.
	 */
	@Override
	public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		if (!(editorInput instanceof IFileEditorInput))
			throw new PartInitException("Invalid input: it must be a file in the workspace");
		super.init(site, editorInput);
		setPartName(editorInput.getName());
	}

	/* (non-Javadoc)
	 * Method declared on IEditorPart.
	 */
	@Override
	public boolean isSaveAsAllowed() {
		return true;
	}

	/**
	 * Notification about page change.
	 */
	@Override
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
		if (getControl(newPageIndex) == formEditor) {
			formEditor.pageSelected();
		}
		else {
			formEditor.pageDeselected();
		}
	}

	/**
	 * Called on workspace changes.
	 */
	public void resourceChanged(final IResourceChangeEvent event){
		// close editor on project close
		if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
			final IEditorPart thisEditor = this;
			Display.getDefault().asyncExec(new Runnable(){
				public void run(){
					if (((FileEditorInput)thisEditor.getEditorInput()).getFile().getProject().equals(event.getResource())) {
						thisEditor.getSite().getPage().closeEditor(thisEditor, true);
					}
				}            
			});
		}
	}
}
