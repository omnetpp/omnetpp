package org.omnetpp.inifile.editor.editors;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPropertyListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.eclipse.ui.part.ShowInContext;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.form.InifileFormEditor;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileConverter;
import org.omnetpp.inifile.editor.model.InifileDocument;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.inifile.editor.text.InifileTextEditor;
import org.omnetpp.inifile.editor.views.InifileContentOutlinePage;

/**
 * Editor for omnetpp.ini files.
 */
//FIXME File|Revert is always disabled
//FIXME crashes if file gets renamed or moved
//FIXME if workbench startup with an open inifile editor blocks for a while, 
// that's because forms are filled in an asyncExec() that refers to InifileAnalyzer, 
// and InifileAnalyzer is synchronized on NEDResources and has to wait until NED validation ends.
// Solution: NED validation shouldn't lock NEDResources? (ie run validation on a *clone* of the trees)
//TODO for units, tooltip should display "seconds" not only "s" 
public class InifileEditor extends MultiPageEditorPart implements IResourceChangeListener, IGotoMarker, IGotoInifile, IShowInSource, IShowInTargetList {
	/* editor pages */
	private InifileTextEditor textEditor;
	private InifileFormEditor formEditor;
	public static final int FORMEDITOR_PAGEINDEX = 0;
	public static final int TEXTEDITOR_PAGEINDEX = 1;

	private InifileEditorData editorData = new InifileEditorData();
	private InifileContentOutlinePage outlinePage;
	private DelayedJob postSelectionChangedJob;
	
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

	public InifileTextEditor getTextEditor() {
		return textEditor;
	}

	public InifileFormEditor getFormEditor() {
		return formEditor;
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
		formEditor = new InifileFormEditor(getContainer(), this);
		addEditorPage(formEditor, "Form");

		// create text editor
		createTextEditorPage();

		// assert page constants are OK
		Assert.isTrue(getControl(FORMEDITOR_PAGEINDEX)==formEditor && getEditor(TEXTEDITOR_PAGEINDEX)==textEditor);
		
		// set up editorData (the InifileDocument)
		IFile file = ((IFileEditorInput)getEditorInput()).getFile();
		IDocument document = textEditor.getDocumentProvider().getDocument(getEditorInput());
		InifileDocument inifileDocument = new InifileDocument(document, file);
		editorData.initialize(this, inifileDocument, new InifileAnalyzer(inifileDocument));

		// replace original MultiPageSelectionProvider with our own, as we want to
		// publish our own selection (with InifileSelectionItem) for both pages.
		getSite().setSelectionProvider(new SelectionProvider());
		
		// propagate property changes (esp. PROP_DIRTY) from our text editor
		textEditor.addPropertyListener(new IPropertyListener() {
			public void propertyChanged(Object source, int propertyId) {
				firePropertyChange(propertyId);
			}
		});
		
//		//XXX experimental
//		// see registration of InformationDispatchAction in AbstractTextEditor
//		IAction action = new Action("F2!!!") {
//			public void run() {
//				System.out.println("F2 pressed!");
//			}
//		};
//		action.setActionDefinitionId(ITextEditorActionDefinitionIds.SHOW_INFORMATION);
////		IKeyBindingService keyBindingService = getEditorSite().getKeyBindingService();
////		keyBindingService.registerAction(action);
//		
//		//IWorkbench workbench = PlatformUI.getWorkbench();
//		//IHandlerService handlerService = (IHandlerService)workbench.getAdapter(IHandlerService.class);
//		//IHandlerService handlerService = (IHandlerService)getEditorSite().getService(IHandlerService.class);
//		IHandlerService handlerService = (IHandlerService)textEditor.getEditorSite().getService(IHandlerService.class);
//		IHandler actionHandler = new ActionHandler(action);
//		handlerService.activateHandler(ITextEditorActionDefinitionIds.SHOW_INFORMATION, actionHandler);
		
		// this DelayedJob will, after a delay, publish a new editor selection towards the workbench
		postSelectionChangedJob = new DelayedJob(600) {
			public void run() {
				updateSelection();
			}
		};
		
		// we want to update the selection whenever the document changes, or the cursor position in the text editor changes
		editorData.getInifileDocument().addInifileChangeListener(new IInifileChangeListener() {
			public void modelChanged() {
				postSelectionChangedJob.restartTimer();
			}
		});
		textEditor.setPostCursorPositionChangeJob(new Runnable() {
			public void run() {
				postSelectionChangedJob.restartTimer();
			}
		});

		Display.getDefault().asyncExec(new Runnable() {
			public void run() {
				// schedule initial analysis of the inifile
				InifileDocument doc = (InifileDocument) editorData.getInifileDocument();
				InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
				doc.parse();
				analyzer.analyze();
				
				// open the "Module Parameters" view
				try {
					IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
					if (workbenchPage != null) { // note: may be null during platform startup...
						workbenchPage.showView(IConstants.MODULEPARAMETERS_VIEW_ID);
						workbenchPage.activate(InifileEditor.this); // otherwise editor loses focus
					}
				} catch (PartInitException e) {
					InifileEditorPlugin.logError(e);
				}

				// if the file is in the old format, offer upgrading it
				convertOldInifile();
				
				// publish an initial selection (select first section)
				String[] sectionNames = doc.getSectionNames();
				if (sectionNames.length > 0)
					setSelection(sectionNames[0], null);
			}

		});
	}

	protected void updateSelection() {
		int cursorLine = textEditor.getCursorLine();
		String section = getEditorData().getInifileDocument().getSectionForLine(cursorLine);
		String key = getEditorData().getInifileDocument().getKeyForLine(cursorLine);
		setSelection(section, key);
	}

	/**
	 * Sets the editor's selection to an InifileSelectionItem containing
	 * the given section and key.
	 */
	public void setSelection(String section, String key) {
		ISelection selection = new StructuredSelection(new InifileSelectionItem(getEditorData().getInifileDocument(), getEditorData().getInifileAnalyzer(), section, key));
		ISelectionProvider selectionProvider = getSite().getSelectionProvider();
		selectionProvider.setSelection(selection);
	}

	/**
	 * Adds an editor page at the last position.
	 */
	protected int addEditorPage(Control page, String label) {
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
		if (outlinePage != null)
			outlinePage.setInput(null); //XXX ?
		((InifileDocument)editorData.getInifileDocument()).dispose();
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
	 * Returns true if the editor is currently switched to the form editor.
	 */
	public boolean isFormPageDisplayed() {
		return getActivePage()==FORMEDITOR_PAGEINDEX;
	}
	
	/**
	 * Detect when the file is in the old format, and offer converting it.
	 */
	protected void convertOldInifile() {
		IDocument doc = textEditor.getDocumentProvider().getDocument(getEditorInput());
		if (InifileConverter.needsConversion(doc.get())) {
			String fileName = getEditorInput().getName();
			if (MessageDialog.openQuestion(getSite().getShell(), "Old Ini File Format", 
					"File \""+fileName+"\" is in the old (3.x) format, and needs to be converted. " +
					"This includes renaming some sections and configuration keys. " +
					"Do you want to convert the editor contents now?")) {
				String newText = InifileConverter.convert(doc.get());
				doc.set(newText);
			}
			else {
				setActivePage(1);  // activate the text editor so that user can see the errors
			}
		}
	}

	@Override
    @SuppressWarnings("unchecked")
	public Object getAdapter(Class required) {
		if (IContentOutlinePage.class.equals(required)) {
			if (outlinePage == null) {
				outlinePage = new InifileContentOutlinePage(this);
				outlinePage.setInput(getEditorData().getInifileDocument());
			}
			return outlinePage;
		}
		return super.getAdapter(required);
	}
	
	/**
	 * Called on workspace changes.
	 */
	public void resourceChanged(IResourceChangeEvent event) {
		// close editor on project close
		if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
			final IEditorPart thisEditor = this;
			final IResource resource = event.getResource();
			Display.getDefault().asyncExec(new Runnable(){
				public void run(){
					if (((FileEditorInput)thisEditor.getEditorInput()).getFile().getProject().equals(resource)) {
						thisEditor.getSite().getPage().closeEditor(thisEditor, true);
					}
				}            
			});
		}
	}

	/* (non-Javadoc)
	 * Method declared on IGotoMarker
	 */
	public void gotoMarker(IMarker marker) {
		setActivePage(TEXTEDITOR_PAGEINDEX);
		IDE.gotoMarker(textEditor, marker);
	}

	/* (non-Javadoc)
	 * Method declared on IGotoInifile
	 */
	public void gotoSection(String section, Mode mode) {
		gotoSectionOrEntry(section, null, mode);
	}

	/* (non-Javadoc)
	 * Method declared on IGotoInifile
	 */
	public void gotoEntry(String section, String key, Mode mode) {
		gotoSectionOrEntry(section, key, mode);
	}
	
	private void gotoSectionOrEntry(String section, String key, Mode mode) {
		// switch to the requested page. If mode==AUTO, stay where we are.
		// Note: setActivePage() gives focus to the editor, so don't call it with AUTO mode.
		if (mode==IGotoInifile.Mode.FORM)
			setActivePage(FORMEDITOR_PAGEINDEX); 
		else if (mode==IGotoInifile.Mode.FORM)
			setActivePage(TEXTEDITOR_PAGEINDEX);
		
		// perform "go to" on whichever page is displayed
		if (getActivePage()==FORMEDITOR_PAGEINDEX) {
			// form editor
			if (section != null) {
				if (key==null)
					formEditor.gotoSection(section);
				else
					formEditor.gotoEntry(section, key);
			}
		}
		else {
			// text editor
			LineInfo line = section==null ? null : key==null ? 
					editorData.getInifileDocument().getSectionLineDetails(section) :
					editorData.getInifileDocument().getEntryLineDetails(section, key);
			highlightLineInTextEditor(line); //XXX highlight the whole section
		}
	}

	protected void highlightLineInTextEditor(LineInfo line) {
		if (line==null) {
			textEditor.resetHighlightRange();
			return;
		}
		//XXX check IFile matches!!!!
		try {
			IDocument docu = textEditor.getDocumentProvider().getDocument(textEditor.getEditorInput());
			int startOffset = docu.getLineOffset(line.getLineNumber()-1);
			int endOffset = docu.getLineOffset(line.getLineNumber())-2;
			textEditor.setHighlightRange(endOffset, 0, true); // move cursor to end of selected line
			textEditor.setHighlightRange(startOffset, endOffset-startOffset, false); // set range without moving cursor
		} catch (BadLocationException e) {
		}
		catch (IllegalArgumentException x) {
			textEditor.resetHighlightRange();
		}
	}

	/* (non-Javadoc)
	 * Method declared on IShowInSource
	 */
	public ShowInContext getShowInContext() {
		return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
	}

	/* (non-Javadoc)
	 * Method declared on IShowInTargetList
	 */
	public String[] getShowInTargetIds() {
		// contents of the "Show In..." context menu
		return new String[] {
				IConstants.MODULEHIERARCHY_VIEW_ID, 
				IConstants.MODULEPARAMETERS_VIEW_ID,
				IPageLayout.ID_OUTLINE,
				IPageLayout.ID_PROBLEM_VIEW,
				IPageLayout.ID_RES_NAV,
				};
	}
}
