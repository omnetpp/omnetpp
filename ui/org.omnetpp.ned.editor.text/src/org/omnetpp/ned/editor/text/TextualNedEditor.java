package org.omnetpp.ned.editor.text;



import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextViewerExtension5;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.IVerticalRuler;
import org.eclipse.jface.text.source.projection.ProjectionSupport;
import org.eclipse.jface.text.source.projection.ProjectionViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.TextOperationAction;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.omnetpp.ned.editor.text.actions.ConvertToNewFormatAction;
import org.omnetpp.ned.editor.text.actions.DefineFoldingRegionAction;
import org.omnetpp.ned.editor.text.outline.NedContentOutlinePage;


public class TextualNedEditor extends TextEditor {
	
	/** The outline page */
	private NedContentOutlinePage fOutlinePage;
	/** The projection support */
	private ProjectionSupport fProjectionSupport;

	/**
	 * Default constructor.
	 */
	public TextualNedEditor() {
		super();
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method extend the 
	 * actions to add those specific to the receiver
	 */
	protected void createActions() {
		super.createActions();
		
		IAction a= new TextOperationAction(NedEditorMessages.getResourceBundle(), "ContentAssistProposal.", this, ISourceViewer.CONTENTASSIST_PROPOSALS); //$NON-NLS-1$
		a.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
		setAction("ContentAssistProposal", a); //$NON-NLS-1$
		
		a= new DefineFoldingRegionAction(NedEditorMessages.getResourceBundle(), "DefineFoldingRegion.", this); //$NON-NLS-1$
		setAction("DefineFoldingRegion", a); //$NON-NLS-1$
        
        a= new ConvertToNewFormatAction(NedEditorMessages.getResourceBundle(), "ConvertToNewFormat.", this); //$NON-NLS-1$
        setAction("ConvertToNewFormat", a); //$NON-NLS-1$
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs any extra 
	 * disposal actions required by the ned editor.
	 */
	public void dispose() {
		if (fOutlinePage != null)
			fOutlinePage.setInput(null);
		super.dispose();
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs any extra 
	 * revert behavior required by the ned editor.
	 */
	public void doRevertToSaved() {
		super.doRevertToSaved();
		if (fOutlinePage != null)
			fOutlinePage.update();
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs any extra 
	 * save behavior required by the ned editor.
	 * 
	 * @param monitor the progress monitor
	 */
	public void doSave(IProgressMonitor monitor) {
		super.doSave(monitor);
		if (fOutlinePage != null)
			fOutlinePage.update();
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs any extra 
	 * save as behavior required by the ned editor.
	 */
	public void doSaveAs() {
		super.doSaveAs();
		if (fOutlinePage != null)
			fOutlinePage.update();
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs sets the 
	 * input of the outline page after AbstractTextEditor has set input.
	 * 
	 * @param input the editor input
	 * @throws CoreException in case the input can not be set
	 */ 
	public void doSetInput(IEditorInput input) throws CoreException {
		super.doSetInput(input);
		if (fOutlinePage != null)
			fOutlinePage.setInput(input);
	}
	
	/**
	 * Sets the content of the text editor with the given string
	 * @param content
	 */
	public void setText(String content) {
		getDocument().set(content);
	}
	
	/**
	 * @return The content of the text editor
	 */
	public String getText() {
		return getDocument().get();
	}
    
    /**
     * @return The current document under editing
     */
    public IDocument getDocument() {
        return getDocumentProvider().getDocument(getEditorInput());
    }

	/*
	 * @see org.eclipse.ui.texteditor.ExtendedTextEditor#editorContextMenuAboutToShow(org.eclipse.jface.action.IMenuManager)
	 */
	protected void editorContextMenuAboutToShow(IMenuManager menu) {
		super.editorContextMenuAboutToShow(menu);
		addAction(menu, ITextEditorActionConstants.GROUP_EDIT, "ConvertToNewFormat"); //$NON-NLS-1$
		addAction(menu, ITextEditorActionConstants.GROUP_EDIT, "ContentAssistProposal"); //$NON-NLS-1$
		addAction(menu,  ITextEditorActionConstants.GROUP_EDIT, "DefineFoldingRegion");  //$NON-NLS-1$
	}
	
	/** The <code>TextualNedEditor</code> implementation of this 
	 * <code>AbstractTextEditor</code> method performs gets
	 * the ned content outline page if request is for a an 
	 * outline page.
	 * 
	 * @param required the required type
	 * @return an adapter for the required type or <code>null</code>
	 */ 
	public Object getAdapter(Class required) {
		if (IContentOutlinePage.class.equals(required)) {
			if (fOutlinePage == null) {
				fOutlinePage= new NedContentOutlinePage(getDocumentProvider(), this);
				if (getEditorInput() != null)
					fOutlinePage.setInput(getEditorInput());
			}
			return fOutlinePage;
		}
		
		if (fProjectionSupport != null) {
			Object adapter= fProjectionSupport.getAdapter(getSourceViewer(), required);
			if (adapter != null)
				return adapter;
		}
		
		return super.getAdapter(required);
	}
		
	/* (non-Javadoc)
	 * Method declared on AbstractTextEditor
	 */
	protected void initializeEditor() {
		super.initializeEditor();
		setSourceViewerConfiguration(new NedSourceViewerConfiguration(this));
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
        // we should set the selection provider as late as possible because the outer multipage esitor overrides it
        // during editor initializatiopn
        // install a selection provider that provides StructuredSelection of NEDModel elements in getSelection
        // instead of ITestSelection
        getSite().setSelectionProvider(new NedSelectionProvider(this));
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
