/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text;

import java.io.IOException;
import java.util.ResourceBundle;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentExtension4;
import org.eclipse.jface.text.ITextViewerExtension5;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.IVerticalRuler;
import org.eclipse.jface.text.source.projection.ProjectionSupport;
import org.eclipse.jface.text.source.projection.ProjectionViewer;
import org.eclipse.jface.text.templates.ContextTypeRegistry;
import org.eclipse.jface.text.templates.persistence.TemplateStore;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.editors.text.templates.ContributionContextTypeRegistry;
import org.eclipse.ui.editors.text.templates.ContributionTemplateStore;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.TextOperationAction;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.editor.text.TextDifferenceUtils;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.text.actions.ConvertToNewFormatAction;
import org.omnetpp.ned.editor.text.actions.CorrectIndentationAction;
import org.omnetpp.ned.editor.text.actions.DistributeAllGateLabelsAction;
import org.omnetpp.ned.editor.text.actions.FindTextInNedFilesActionDelegate;
import org.omnetpp.ned.editor.text.actions.FormatSourceAction;
import org.omnetpp.ned.editor.text.actions.GotoDeclarationAction;
import org.omnetpp.ned.editor.text.actions.InferAllGateLabelsAction;
import org.omnetpp.ned.editor.text.actions.OrganizeImportsAction;
import org.omnetpp.ned.editor.text.actions.ToggleCommentAction;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedSourceRegion;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedMarkerChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;


/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class TextualNedEditor extends TextEditor implements INedChangeListener, ISelectionListener {

    private static final String CUSTOM_TEMPLATES_KEY = "org.omnetpp.ned.editor.text.customtemplates";
    public static final String[] KEY_BINDING_SCOPES = { "org.omnetpp.context.nedTextEditor" };
    public final static String ID = NedEditor.ID+".text";

    private static boolean pushingChanges;

    private static TemplateStore fStore;

    /** The context type registry. */
    private static ContributionContextTypeRegistry fRegistry;

    /** The outline page */
//  private NedContentOutlinePage fOutlinePage;

    /** The projection support */
    private ProjectionSupport fProjectionSupport;

    private DelayedJob pullChangesJob;
    private NedSelectionProvider nedSelectionProvider;

    /**
     * Default constructor.
     */
    public TextualNedEditor() {
        super();

        // delay update to avoid concurrent access to document
        pullChangesJob = new DelayedJob(500) {
            public void run() {
                pullChangesFromNedResourcesWhenPending();
            }
        };

    }

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        super.init(site, input);
        // listen on NED model changes
        NedResourcesPlugin.getNedResources().addNedModelChangeListener(this);
        getSite().getPage().addSelectionListener(this);
    }

    @Override
    protected void initializeKeyBindingScopes() {
        setKeyBindingScopes(KEY_BINDING_SCOPES);
    }

    /** The <code>TextualNedEditor</code> implementation of this
     * <code>AbstractTextEditor</code> method performs any extra
     * disposal actions required by the ned editor.
     */
    @Override
    public void dispose() {
        if (pullChangesJob != null)
            pullChangesJob.cancel();

        getSite().getPage().removeSelectionListener(this);
        NedResourcesPlugin.getNedResources().removeNedModelChangeListener(this);
        super.dispose();
    }

    /* (non-Javadoc)
     * Method declared on AbstractTextEditor
     */
    @Override
    protected void initializeEditor() {
        super.initializeEditor();
        // redefine the main context menu id so we can contribute to it from the outside
        setEditorContextMenuId(ID);
        //XXX for now NedResourcesPlugin.getNEDResources().addNEDModelChangeListener(this);
        setSourceViewerConfiguration(new NedSourceViewerConfiguration(this));
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.texteditor.ExtendedTextEditor#createSourceViewer(org.eclipse.swt.widgets.Composite, org.eclipse.jface.text.source.IVerticalRuler, int)
     */
    @Override
    protected ISourceViewer createSourceViewer(Composite parent, IVerticalRuler ruler, int styles) {
        fAnnotationAccess= createAnnotationAccess();
        fOverviewRuler= createOverviewRuler(getSharedColors());

        ISourceViewer viewer= new ProjectionViewer(parent, ruler, getOverviewRuler(), isOverviewRulerVisible(), styles);
        // ensure decoration support has been created and configured.
        getSourceViewerDecorationSupport(viewer);

        return viewer;
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.texteditor.ExtendedTextEditor#createPartControl(org.eclipse.swt.widgets.Composite)
     */
    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);
        ProjectionViewer viewer= (ProjectionViewer) getSourceViewer();
        fProjectionSupport= new ProjectionSupport(viewer, getAnnotationAccess(), getSharedColors());
        fProjectionSupport.addSummarizableAnnotationType("org.eclipse.ui.workbench.texteditor.error");
        fProjectionSupport.addSummarizableAnnotationType("org.eclipse.ui.workbench.texteditor.warning");
        fProjectionSupport.install();
        viewer.doOperation(ProjectionViewer.TOGGLE);
        nedSelectionProvider = new NedSelectionProvider(this);
        getSite().setSelectionProvider(nedSelectionProvider);
    }

    @Override
    protected boolean isEditorInputIncludedInContextMenu() {
        // do not include the contributions for the editor input
        // (otherwise we will get a ton of unnecessary menus like Debug As, Run As, Team menus etc.)
        return false;
    }

    /**
     * Returns this plug-in's template store.
     */
    public static TemplateStore getTemplateStore() {
        if (fStore == null) {
            fStore = new ContributionTemplateStore(getContextTypeRegistry(),
                    NedEditorPlugin.getDefault().getPreferenceStore(), CUSTOM_TEMPLATES_KEY);
            try {
                fStore.load();
            } catch (IOException e) {
                NedEditorPlugin.logError(e);
            }
        }
        return fStore;
    }

    /**
     * Returns this plug-in's context type registry.
     */
    public static ContextTypeRegistry getContextTypeRegistry() {
        if (fRegistry == null) {
            // create an configure the contexts available in the template editor
            fRegistry= new ContributionContextTypeRegistry();
            fRegistry.addContextType(NedCompletionHelper.DEFAULT_NED_CONTEXT_TYPE);
        }
        return fRegistry;
    }

    /**
     * Makes getSourceViewer() public. (Name differs because original method is final.)
     */
    public ISourceViewer getSourceViewerPublic() {
        return super.getSourceViewer();
    }

    /** The <code>TextualNedEditor</code> implementation of this
     * <code>AbstractTextEditor</code> method extend the
     * actions to add those specific to the receiver
     */
    @Override
    protected void createActions() {
        super.createActions();

        ResourceBundle bundle = NedEditorMessages.getResourceBundle();
        IAction a= new TextOperationAction(bundle, "ContentAssistProposal.", this, ISourceViewer.CONTENTASSIST_PROPOSALS);
        a.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
        setAction(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, a);

        a = new ConvertToNewFormatAction(this);
        setAction(a.getId(), a);
        markAsContentDependentAction(a.getId(), true);

        a = new FormatSourceAction(this);
        setAction(a.getId(), a);
        markAsContentDependentAction(a.getId(), true);

        a = new GotoDeclarationAction(this);
        setAction(a.getId(), a);
        markAsContentDependentAction(a.getId(), true);

        a = new OrganizeImportsAction(this);
        setAction(a.getId(), a);
        markAsContentDependentAction(a.getId(), true);

        a = new ToggleCommentAction(this);
        setAction(a.getId(), a);
        markAsSelectionDependentAction(a.getId(), true);

        a = new CorrectIndentationAction(this);
        setAction(a.getId(), a);
        markAsSelectionDependentAction(a.getId(), true);

        a = new FindTextInNedFilesActionDelegate();
        setAction(a.getId(), a);
        //markAsSelectionDependentAction(a.getId(), true);

        a = new InferAllGateLabelsAction(this);
        setAction(a.getId(), a);

        a = new DistributeAllGateLabelsAction(this);
        setAction(a.getId(), a);
    }

    /*
     * @see org.eclipse.ui.texteditor.ExtendedTextEditor#editorContextMenuAboutToShow(org.eclipse.jface.action.IMenuManager)
     */
    @Override
    protected void editorContextMenuAboutToShow(IMenuManager menu) {
        super.editorContextMenuAboutToShow(menu);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, ToggleCommentAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, CorrectIndentationAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, FormatSourceAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, ConvertToNewFormatAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, OrganizeImportsAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, InferAllGateLabelsAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, DistributeAllGateLabelsAction.ID);

        addAction(menu, ITextEditorActionConstants.GROUP_FIND, GotoDeclarationAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_FIND, FindTextInNedFilesActionDelegate.ID);
    }

    /**
     * Sets the content of the text editor to the given string.
     */
    public void setText(String content) {
        getDocument().set(content);
    }

    /**
     * Returns the content of the text editor
     */
    public String getText() {
        return getDocument() != null ?  getDocument().get() : null;
    }

    /**
     * Returns the current document under editing
     */
    public IDocument getDocument() {
        return getDocumentProvider().getDocument(getEditorInput());
    }

    protected IFile getFile() {
        return ((IFileEditorInput)getEditorInput()).getFile();
    }

    public NedFileElementEx getModel() {
        return NedResourcesPlugin.getNedResources().getNedFileElement(getFile());
    }

    @Override
    public void setAction(String actionID, IAction action) {
        if (!actionID.equals(ITextEditorActionConstants.SAVE))
            super.setAction(actionID, action);
    }

    /**
     * The <code>TextualNedEditor</code> implementation of this
     * <code>AbstractTextEditor</code> method gets
     * the ned content outline page if request is for an
     * outline page.
     *
     * @param required the required type
     * @return an adapter for the required type or <code>null</code>
     */
    @SuppressWarnings("unchecked") @Override
    public Object getAdapter(Class required) {
        if (fProjectionSupport != null) {
            Object adapter= fProjectionSupport.getAdapter(getSourceViewer(), required);
            if (adapter != null)
                return adapter;
        }

        return super.getAdapter(required);
    }


    /* (non-Javadoc)
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

    @Override
    public void setHighlightRange(int offset, int length, boolean moveCursor) {
        // Overridden to work around for bug in AbstractTextEditor: if range indicator is
        // already at the desired offset/length, it ignores moveCursor (i.e. does not move
        // the cursor).
        if (getSourceViewer() == null)
            return;
        super.setHighlightRange(offset, length, moveCursor);
        if (moveCursor && getSourceViewer().getSelectedRange().x != offset)
            getSourceViewer().setSelectedRange(offset, 0);
    }

    /**
     * Whether it is the currently active editor (not necessarily the active part however if a view is currently the
     * active part.
     * NOTE: this method is called from NON-UI threads. do not access SWT widgets
     */
    public boolean isActive() {
        IWorkbenchPage activePage = getSite().getWorkbenchWindow().getActivePage(); // may be null during startup
        IEditorPart activeEditorPart = activePage==null ? null : activePage.getActiveEditor();
        return activeEditorPart == this ||
            (activeEditorPart instanceof NedEditor && ((NedEditor)activeEditorPart).isActiveEditor(this));
    }

    /**
     * Whether it is the currently active editor (not necessarily the active part however if a view is currently the
     * active part.
     * NOTE: this method is called from NON-UI threads. do not access SWT widgets
     */
    public boolean isActivePart() {
        IWorkbenchPage activePage = getSite().getWorkbenchWindow().getActivePage(); // may be null during startup
         IWorkbenchPart activePart = activePage==null ? null : activePage.getActivePart();
        return activePart == this ||
            (activePart instanceof NedEditor && ((NedEditor)activePart).isActiveEditor(this));
    }

    /**
     * NOTE: this method is called from NON-UI threads. do not access SWT widgets
     */
    public void modelChanged(NedModelEvent event) {
        if (event.getSource() != null && !(event instanceof NedMarkerChangeEvent) && !isActive() && !pushingChanges) { //XXX looks like sometimes this condition is not enough!
            INedElement nedFileElement = event.getSource().getContainingNedFileElement();

            if (nedFileElement == getModel())
                pullChangesJob.restartTimer();
        }
    }

    public synchronized void pushChangesIntoNedResources() {
        pushChangesIntoNedResources(true);
    }

    /**
     * Pushes down text changes from document into NEDResources.
     */
    public synchronized void pushChangesIntoNedResources(final boolean evenIfEditorIsInactive) {
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            public synchronized void run() {
                Assert.isTrue(!pushingChanges);
                if (evenIfEditorIsInactive || isActive()) {
                    try {
                        // this must be static, because we may have several text editors open
                        // (via Window|New Editor) which are internally synchronized to each other
                        // by the platform -- so we need to block *all* reconcilers from running.
                        // Being static causes no problems with multiple reconcilers, because
                        // the access is serialized through asyncExec.
                        pushingChanges = true;
                        // perform parsing (of full text, we ignore the changed region)
                        NedResourcesPlugin.getNedResources().setNedFileText(getFile(), getText());
                    }
                    finally {
                        pushingChanges = false;
                    }
                }
            }
        });
    }

    /**
     * When a "pull changes from NEDResources" operation has been scheduled, do it now.
     */
    public synchronized void pullChangesFromNedResourcesWhenPending() {
        // if the job is not scheduled then there are no changes at all and we don't pull
        // because that would only pretty print the source
        if (pullChangesJob.isScheduled()) {
            pullChangesJob.cancel();
            DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                public synchronized void run() {
                    pullChangesFromNedResources();
                }
            });
        }
    }

    /**
     * Unconditionally pulls changes from NEDResources and applies them to the
     * document as text changes.
     *
     * This needs to be called from all text editor actions that modify the
     * NED tree and want those changes to be reflected in the text editor.
     * (e.g. reformat source, organize imports, etc).
     */
    public void pullChangesFromNedResources() {
        Assert.isTrue(Display.getCurrent() != null);
//        Debug.println("texteditor: pulling changes from NEDResources");
        IDocument document = getDocument();
        String nedSource = getModel().getNedSource().replace("\n", ((IDocumentExtension4)document).getDefaultLineDelimiter());
        TextDifferenceUtils.modifyTextEditorContentByApplyingDifferences(document, nedSource);
        TextEditorUtil.resetMarkerAnnotations(TextualNedEditor.this); // keep markers from disappearing
        // TODO: then parse in again, and update line numbers with the resulting tree? I think this should not be done here but somewhere else
    }


    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
        // do not react to notification changes if we are the active editor or we are currently
        // setting our selection
        if (isActivePart() || nedSelectionProvider.isNotificationInProgress())
            return;

        // Debug.println("*** TextEditor selection changed from: "+part+" selection: "+selection);
        if (selection instanceof IStructuredSelection) {
            Object firstElement = ((IStructuredSelection) selection).getFirstElement();
            if (selection.isEmpty())
                resetHighlightRange();
            else if (firstElement instanceof INedModelProvider){
                INedElement node = ((INedModelProvider)firstElement).getModel();
                //Debug.println("selected: "+node);
                NedSourceRegion region = node.getSourceRegion();
                if (region!=null) {
                    IDocument docu = getDocument();
                    try {
                        int startOffset = docu.getLineOffset(region.getStartLine()-1)+region.getStartColumn();
                        int endOffset = docu.getLineOffset(region.getEndLine()-1)+region.getEndColumn();
                        setHighlightRange(startOffset, endOffset-startOffset, true);
                    } catch (BadLocationException e) {
                    }
                    catch (IllegalArgumentException x) {
                        resetHighlightRange();
                    }
                }
            }
        }
    }
}
