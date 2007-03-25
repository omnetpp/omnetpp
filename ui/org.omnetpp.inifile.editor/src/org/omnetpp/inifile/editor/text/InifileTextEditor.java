package org.omnetpp.inifile.editor.text;


import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.text.ITextViewerExtension5;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.IVerticalRuler;
import org.eclipse.jface.text.source.projection.ProjectionSupport;
import org.eclipse.jface.text.source.projection.ProjectionViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.TextOperationAction;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.text.actions.DefineFoldingRegionAction;


/**
 * Text editor for ini files.
 */
public class InifileTextEditor extends TextEditor {
	/** The projection support */
	private ProjectionSupport fProjectionSupport;

	/**
	 * Default constructor.
	 */
	public InifileTextEditor(InifileEditor parentEditor) {
		super();

		// Note: we should actually override initializeEditor() and place the 
		// setSourceViewerConfiguration() call there. Problem is, parentEditor 
		// is not yet available at that point.
		setSourceViewerConfiguration(new InifileSourceViewerConfiguration(parentEditor.getEditorData()));
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method extend the 
	 * actions to add those specific to the receiver
	 */
	protected void createActions() {
		super.createActions();
		
		IAction a= new TextOperationAction(InifileEditorMessages.getResourceBundle(), "ContentAssistProposal.", this, ISourceViewer.CONTENTASSIST_PROPOSALS); //$NON-NLS-1$
		a.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
		setAction("ContentAssistProposal", a); //$NON-NLS-1$
		
		a= new TextOperationAction(InifileEditorMessages.getResourceBundle(), "ContentAssistTip.", this, ISourceViewer.CONTENTASSIST_CONTEXT_INFORMATION);  //$NON-NLS-1$
		a.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_CONTEXT_INFORMATION);
		setAction("ContentAssistTip", a); //$NON-NLS-1$
		
		a= new DefineFoldingRegionAction(InifileEditorMessages.getResourceBundle(), "DefineFoldingRegion.", this); //$NON-NLS-1$
		setAction("DefineFoldingRegion", a); //$NON-NLS-1$
	}
	
//XXX make sure this isn't needed after all
//	/** The <code>TextualNedEditor</code> implementation of this 
//	 * <code>AbstractTextEditor</code> method performs any extra 
//	 * revert behavior required by the ned editor.
//	 */
//	public void doRevertToSaved() {
//		super.doRevertToSaved();
//		if (fOutlinePage != null)
//			fOutlinePage.update();
//	}
	
//XXX make sure this isn't needed after all
//	/** The <code>TextualNedEditor</code> implementation of this 
//	 * <code>AbstractTextEditor</code> method performs any extra 
//	 * save behavior required by the ned editor.
//	 * 
//	 * @param monitor the progress monitor
//	 */
//	public void doSave(IProgressMonitor monitor) {
//		super.doSave(monitor);
//		if (fOutlinePage != null)
//			fOutlinePage.update();
//	}
	
//XXX make sure this isn't needed after all
//	/** The <code>TextualNedEditor</code> implementation of this 
//	 * <code>AbstractTextEditor</code> method performs any extra 
//	 * save as behavior required by the ned editor.
//	 */
//	public void doSaveAs() {
//		super.doSaveAs();
//		if (fOutlinePage != null)
//			fOutlinePage.update();
//	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs sets the 
	 * input of the outline page after AbstractTextEditor has set input.
	 * 
	 * @param input the editor input
	 * @throws CoreException in case the input can not be set
	 */ 
	public void doSetInput(IEditorInput input) throws CoreException {
		super.doSetInput(input);
		//XXX when to parse the input? does reconcile get called?
//XXX update the outline page		
//		if (fOutlinePage != null)
//			fOutlinePage.setInput(input);
	}
	
	/**
	 * Sets the content of the text editor with the given string
	 * @param content
	 */
	public void setText(String content) {
		getDocumentProvider().getDocument(getEditorInput()).set(content);
	}
	
	/**
	 * @return The content of the text editor
	 */
	public String getText() {
		return getDocumentProvider().getDocument(getEditorInput()).get();
	}

	/*
	 * @see org.eclipse.ui.texteditor.ExtendedTextEditor#editorContextMenuAboutToShow(org.eclipse.jface.action.IMenuManager)
	 */
	protected void editorContextMenuAboutToShow(IMenuManager menu) {
		super.editorContextMenuAboutToShow(menu);
		addAction(menu, "ContentAssistProposal"); //$NON-NLS-1$
		addAction(menu, "ContentAssistTip"); //$NON-NLS-1$
		addAction(menu, "DefineFoldingRegion");  //$NON-NLS-1$
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method gets
	 * the ned content outline page if request is for a an 
	 * outline page.
	 * 
	 * @param required the required type
	 * @return an adapter for the required type or <code>null</code>
	 */ 
	public Object getAdapter(Class required) {
		if (fProjectionSupport != null) {
			Object adapter = fProjectionSupport.getAdapter(getSourceViewer(), required);
			if (adapter != null)
				return adapter;
		}
		
		return super.getAdapter(required);
	}
		
	/*
	 * @see org.eclipse.ui.texteditor.ExtendedTextEditor#createSourceViewer(org.eclipse.swt.widgets.Composite, org.eclipse.jface.text.source.IVerticalRuler, int)
	 */
	protected ISourceViewer createSourceViewer(Composite parent, IVerticalRuler ruler, int styles) {
		
		fAnnotationAccess= createAnnotationAccess();
		fOverviewRuler= createOverviewRuler(getSharedColors());
		
		ISourceViewer viewer= new ProjectionViewer(parent, ruler, getOverviewRuler(), isOverviewRulerVisible(), styles);
		// ensure decoration support has been created and configured.
		getSourceViewerDecorationSupport(viewer);
		
		return viewer;
	}
	
	/*
	 * @see org.eclipse.ui.texteditor.ExtendedTextEditor#createPartControl(org.eclipse.swt.widgets.Composite)
	 */
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		ProjectionViewer viewer= (ProjectionViewer) getSourceViewer();
		fProjectionSupport= new ProjectionSupport(viewer, getAnnotationAccess(), getSharedColors());
		fProjectionSupport.addSummarizableAnnotationType("org.eclipse.ui.workbench.texteditor.error"); //$NON-NLS-1$
		fProjectionSupport.addSummarizableAnnotationType("org.eclipse.ui.workbench.texteditor.warning"); //$NON-NLS-1$
		fProjectionSupport.install();
		viewer.doOperation(ProjectionViewer.TOGGLE);
	}
	
	/*
	 * @see org.eclipse.ui.texteditor.AbstractTextEditor#adjustHighlightRange(int, int)
	 */
	protected void adjustHighlightRange(int offset, int length) {
		ISourceViewer viewer= getSourceViewer();
		if (viewer instanceof ITextViewerExtension5) {
			ITextViewerExtension5 extension= (ITextViewerExtension5) viewer;
			extension.exposeModelRange(new Region(offset, length));
		}
	}
}
