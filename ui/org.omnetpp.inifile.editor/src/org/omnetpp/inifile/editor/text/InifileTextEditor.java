package org.omnetpp.inifile.editor.text;


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
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.TextOperationAction;

import org.omnetpp.inifile.editor.actions.AddInifileKeysAction;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.text.actions.ToggleCommentAction;


/**
 * Text editor for ini files.
 */
public class InifileTextEditor extends TextEditor {
    public static final String[] KEY_BINDING_SCOPES = { "org.omnetpp.context.inifileEditor" };
    /** The projection support */
	private ProjectionSupport projectionSupport;
	private Runnable cursorPositionChangedJob;

	/**
	 * Default constructor.
	 */
	public InifileTextEditor(InifileEditor parentEditor) {
		super();

        setKeyBindingScopes(KEY_BINDING_SCOPES);
		// Note: we should actually override initializeEditor() and place the
		// setSourceViewerConfiguration() call there. Problem is, parentEditor
		// is not yet available at that point.
		setSourceViewerConfiguration(new InifileSourceViewerConfiguration(parentEditor.getEditorData()));
	}

	/** The <code>TextualNedEditor</code> implementation of this
	 * <code>AbstractTextEditor</code> method extend the
	 * actions to add those specific to the receiver
	 */
	@Override
	protected void createActions() {
		super.createActions();

		IAction a= new TextOperationAction(InifileEditorMessages.getResourceBundle(), "ContentAssistProposal.", this, ISourceViewer.CONTENTASSIST_PROPOSALS);
		a.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
		a.setId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
		setAction(a.getId(), a);

        a = new ToggleCommentAction(this);
        setAction(a.getId(), a);
        markAsSelectionDependentAction(a.getId(), true);

        a = new AddInifileKeysAction();
		setAction(a.getId(), a);
	}

    @Override
    protected void editorContextMenuAboutToShow(IMenuManager menu) {
        super.editorContextMenuAboutToShow(menu);

        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, ToggleCommentAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, AddInifileKeysAction.ID);
    }

    /**
	 * Returns the text document.
	 */
	public IDocument getDocument() {
		return getDocumentProvider().getDocument(getEditorInput());
	}

	/**
	 * Sets the content of the text editor with the given string
	 * @param content
	 */
	public void setText(String content) {
		getDocument().set(content);
	}

	/**
	 * Returns the content of the text editor
	 */
	public String getText() {
		return getDocument().get();
	}

	/**
	 * Return projection support for the editor.
	 */
	@Override
    @SuppressWarnings("unchecked")
	public Object getAdapter(Class required) {
		if (projectionSupport != null) {
			Object adapter = projectionSupport.getAdapter(getSourceViewer(), required);
			if (adapter != null)
				return adapter;
		}

		return super.getAdapter(required);
	}

	/*
	 * @see org.eclipse.ui.texteditor.ExtendedTextEditor#createSourceViewer(org.eclipse.swt.widgets.Composite, org.eclipse.jface.text.source.IVerticalRuler, int)
	 */
	@Override
	protected ISourceViewer createSourceViewer(Composite parent, IVerticalRuler ruler, int styles) {

		fAnnotationAccess = createAnnotationAccess();
		fOverviewRuler = createOverviewRuler(getSharedColors());

		ISourceViewer viewer = new ProjectionViewer(parent, ruler, getOverviewRuler(), isOverviewRulerVisible(), styles);
		// ensure decoration support has been created and configured.
		getSourceViewerDecorationSupport(viewer);

		return viewer;
	}

	/*
	 * @see org.eclipse.ui.texteditor.ExtendedTextEditor#createPartControl(org.eclipse.swt.widgets.Composite)
	 */
	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		ProjectionViewer viewer = (ProjectionViewer) getSourceViewer();
		projectionSupport = new ProjectionSupport(viewer, getAnnotationAccess(), getSharedColors());
		projectionSupport.addSummarizableAnnotationType("org.eclipse.ui.workbench.texteditor.error"); //$NON-NLS-1$
		projectionSupport.addSummarizableAnnotationType("org.eclipse.ui.workbench.texteditor.warning"); //$NON-NLS-1$
		projectionSupport.install();
		viewer.doOperation(ProjectionViewer.TOGGLE);
	}

	@Override
	protected boolean isEditorInputIncludedInContextMenu() {
		// do not include editor input menus like Debug As, Compare and numerous unrelated CDT build menus
		return false;
	}
	
	/**
	 * Installs a function to be called after the cursor rests at one place for 600ms.
	 * @param runnable  contains the function
	 */
	public void setPostCursorPositionChangeJob(Runnable runnable) {
		cursorPositionChangedJob = runnable;
	}

	/*
	 * @see org.eclipse.ui.texteditor.AbstractTextEditor#handleCursorPositionChanged()
	 */
	@Override
	protected void handleCursorPositionChanged() {
		super.handleCursorPositionChanged();
		if (cursorPositionChangedJob != null)
			cursorPositionChangedJob.run();
	}

	/**
	 * Returns the line number where the cursor is currently located.
	 */
	public int getCursorLine() {
		String pos = super.getCursorPosition();  // "12 : 45"
		try {
			return Integer.valueOf(pos.replaceFirst(" *:.*", ""));
		} catch (NumberFormatException e) {
			return 1;
		}
	}

	/*
	 * @see org.eclipse.ui.texteditor.AbstractTextEditor#adjustHighlightRange(int, int)
	 */
	@Override
	protected void adjustHighlightRange(int offset, int length) {
		ISourceViewer viewer= getSourceViewer();
		if (viewer instanceof ITextViewerExtension5) {
			ITextViewerExtension5 extension= (ITextViewerExtension5) viewer;
			extension.exposeModelRange(new Region(offset, length));
		}
	}
}
