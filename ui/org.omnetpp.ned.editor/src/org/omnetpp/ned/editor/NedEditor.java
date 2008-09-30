package org.omnetpp.ned.editor;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.editors.text.EditorsUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.eclipse.ui.part.ShowInContext;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.ned.core.IGotoNedElement;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDFileRemovedEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * Multi-page NED editor.
 * MultiPageNedEditor binds the two separate NED-based editors together. Both the text and the graphical
 * editor maintain their own models independent of each other.
 * Saving is done by delegation to the text editor's save method (i.e. the graphical
 * editor itself cannot save its model, the multipage editor must first obtain
 * the model from the graphical editor, convert to text and pass it to the Text based editor and then
 * call the Text editor to save its content. When setting the input of the multipage editor both
 * embedded editor should be notified (i.e. setInput must be delegated)
 *
 * @author rhornig
 */
public class NedEditor
	extends MultiPageEditorPart
	implements IGotoNedElement, IGotoMarker, IShowInTargetList, IShowInSource
{
    public static final String ID = "org.omnetpp.ned.editor";

    private GraphicalNedEditor graphicalEditor;
	private TextualNedEditor textEditor;
    private final ResourceTracker resourceListener = new ResourceTracker();

	private int graphPageIndex;
	private int textPageIndex;

    protected IPartListener partListener = new IPartListener() {
        public void partOpened(IWorkbenchPart part) {
        }

        public void partClosed(IWorkbenchPart part) {
        }

        public void partActivated(IWorkbenchPart part) {
        	System.out.println(part.toString()+" activated");
            if (part == NedEditor.this) {
                if (getEditorInput() != null && NEDResourcesPlugin.getNEDResources().containsNedFileElement(getFile())) {
                    // when switching from another MultiPageNedEditor to this one for the same file
                    // we need to immediately pull the changes, because editing in this editor
                    // can be done correctly only if it is synchronized with NEDResources
                    // synchronization is normally done in a delayed job and here we enforce to happen it right now
                    NedFileElementEx nedFileElement = getModel();
                    if (getActivePage() == textPageIndex && graphicalEditor.hasContentChanged() &&
                    	!nedFileElement.isReadOnly() && !nedFileElement.hasSyntaxError())
                        textEditor.pullChangesFromNEDResourcesWhenPending();
                }

                if (getControl(getActivePage()).isVisible())
                    graphicalEditor.refresh();
            }
        }

        public void partDeactivated(IWorkbenchPart part) {
            if (part == NedEditor.this) {
                // when switching from one MultiPageNedEditor to another for the same file
                // we need to immediately push the changes, because editing in the other editor
                // can be done correctly only if it is synchronized with the one just being deactivated
                // synchronization is normally done in a delayed job and here we enforce to happen it right now
                if (getActivePage() == textPageIndex &&
                        NEDResourcesPlugin.getNEDResources().containsNedFileElement(getFile()))
                    textEditor.pushChangesIntoNEDResources();
            }
        }

		public void partBroughtToTop(IWorkbenchPart part) {
        }
    };

    protected INEDChangeListener nedModelListener = new INEDChangeListener() {
        public void modelChanged(NEDModelEvent event) {
            if (event instanceof NEDFileRemovedEvent && ((NEDFileRemovedEvent)event).getFile().equals(getFile())) {
                final boolean dirty = isDirty(); // this must be called before closeEditor
                // FIXME IMPORTANT
                final String oldContent = getTextEditor().getText();
                final IFile file = getFile();
                // the problem is that workspace changes don't happen in the UI thread
                // so we switch to it and call close from there
                DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
        			public void run() {
        			    closeEditor(false);
                        if (file.isAccessible() && dirty) {
                            IWorkbench workbench = PlatformUI.getWorkbench();
                            IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
                            IWorkbenchPage page = workbenchWindow.getActivePage();
                            try {
                                ITextEditor editor = (ITextEditor)IDE.openEditor(page, file, EditorsUI.DEFAULT_TEXT_EDITOR_ID);
                                editor.getDocumentProvider().getDocument(editor.getEditorInput()).set(oldContent);
                            } catch (PartInitException e) {
                                NedEditorPlugin.logError(e);
                            }
                        }
					}
        		});
            }
        }
    };

    @Override
    public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		if (!(editorInput instanceof IFileEditorInput))
            throw new PartInitException("Invalid input type (only workspace files can be opened): " + editorInput);

		IFile file = ((FileEditorInput)editorInput).getFile();
		if (!file.exists()) {
            IStatus status = new Status(IStatus.WARNING, NedEditorPlugin.PLUGIN_ID, 0, "File "+file.getFullPath()+" does not exist", null);
            throw new PartInitException(status);
		}
        if (NEDResourcesPlugin.getNEDResources().getNedSourceFolderFor(file) == null) {
		    IStatus status = new Status(IStatus.WARNING, NedEditorPlugin.PLUGIN_ID, 0, "NED File is not in a NED Source Folder of an OMNeT++ Project, and cannot be opened with this editor.", null);
		    throw new PartInitException(status);
		}

		super.init(site, editorInput);

        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceListener);
        NEDResourcesPlugin.getNEDResources().addNEDModelChangeListener(nedModelListener);
        getSite().getPage().addPartListener(partListener);
	}

    @Override
    public void dispose() {
        // detach the editor file from the core plugin and do not set a new file
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceListener);
        if (getSite() != null && getSite().getPage() != null)
        	getSite().getPage().removePartListener(partListener);
        NEDResourcesPlugin.getNEDResources().removeNEDModelChangeListener(nedModelListener);

        // super must be called before disconnect to let the child editors remove their listeners
        super.dispose();

        // disconnect the editor from the ned resources plugin
        if (getEditorInput() != null)
            NEDResourcesPlugin.getNEDResources().disconnect(getFile());
    }

    @Override
    protected void setInput(IEditorInput newInput) {
		//System.out.println("setInput()");
        Assert.isNotNull(newInput, "input should not be null");

    	//FIXME it should be checked that the file is a valid NED file (inside an
    	// OMNeT++ project, inside a NED source folder), i.e. NEDResources knows about it.
    	// Otherwise the will be a NULL POINTER EXCEPTION pretty soon. --Andras

		IEditorInput oldInput = getEditorInput();
		if (ObjectUtils.equals(oldInput, newInput))
            return; // no change

		// disconnect from the old file (if there was any)
        if (oldInput != null)
            NEDResourcesPlugin.getNEDResources().disconnect(getFile());

        // connect() must take place *before* setInput()
        IFile newFile = ((IFileEditorInput) newInput).getFile();
        NEDResourcesPlugin.getNEDResources().connect(newFile);

        // set the new input
        super.setInput(newInput);
        if (graphicalEditor != null)
            graphicalEditor.setInput(newInput);
        if (textEditor != null)
            textEditor.setInput(newInput);

        setPartName(getFile().getName());
    }

	@Override
	protected void createPages() {
		//System.out.println("createPages()");

        graphicalEditor = new GraphicalNedEditor();
        textEditor = new TextualNedEditor();

		try {
            // setup graphical editor
            graphPageIndex = addPage(graphicalEditor, getEditorInput());
            graphicalEditor.markContent();
            setPageText(graphPageIndex, "Design");

            // setup text editor
            // we don't have to set the content because it's set
            // automatically by the text editor (from the FileEditorInput)
            textPageIndex = addPage(textEditor, getEditorInput());
            setPageText(textPageIndex,"Source");

            // switch to graphics mode initially if there's no error in the file
            setActivePage(maySwitchToGraphicalEditor() ? graphPageIndex : textPageIndex);

		}
		catch (PartInitException e) {
		    NedEditorPlugin.logError(e);
		}

	}

	/**
	 * Returns true if the editor is allowed to switch to the graphical editor page.
	 * By default, the criteria is that there should be no parse error or basic validation error.
	 * (Consistency errors are allowed).
	 */
	protected boolean maySwitchToGraphicalEditor() {
		return getModel().getSyntaxProblemMaxCumulatedSeverity() < IMarker.SEVERITY_ERROR;
	}

	@Override
	protected void setActivePage(int pageIndex) {
	    // check if there was a change at all (this prevents possible recursive calling)
	    if (pageIndex == getActivePage())
	        return;

	    super.setActivePage(pageIndex);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.part.MultiPageEditorPart#pageChange(int)
	 * Responsible of synchronizing the two editor's model with each other and the NEDResources plugin
	 */
	@Override
	protected void pageChange(int newPageIndex) {
        super.pageChange(newPageIndex);

        // XXX Kludge: currently the workbench do not send a partActivated/deactivated messages
        // for embedded editors in a MultiPageEditor. (This is a missing unimplemented feature,
        // it works with MultiEditor though).
        // To make the NED editor Outline page active, we need to send activate/deactivate directly.
        // We find the Outline View directly, and send the notification by hand.
        // Once the platform MultiPageEditor class handles this correctly, this code can be removed.
        // On each page change we emulate a close/open cycle of the multi-page editor, this removes
        // the associated Outline page, so the Outline View will re-request the multi-page editor
        // for a ContentOutlinePage (via getAdapter). The current implementation of
        // MultipageEditorPart.getAdapter delegates this request to the active embedded editor.
        //
//        ContentOutline contentOutline = (ContentOutline)getEditorSite().getPage().findView(IPageLayout.ID_OUTLINE);
//        if (contentOutline != null) {
//            // notify from the old closed editor
//        	// TODO: after switching to text page and back to graphical page the dragging will be broken in the outline view
//        	//       look at somewhere near hookControl() and refreshDragSourceAdapter() in AbstractEditPartView
//        	//       somehow the getDragSource() method returns a non null value and listeners are not correctly hooked up
//        	//       and that's why dragging will not work AFAICT now. KLUDGE: this has been worked around in NedOutlinePage
//            contentOutline.partClosed(this);
//            contentOutline.partActivated(this);
//        }
        // end of kludge

		// switch from graphics to text:
        if (newPageIndex == textPageIndex) {
        	// generate text representation from the model NOW
            NedFileElementEx nedFileElement = getModel();
            if (graphicalEditor.hasContentChanged() && !nedFileElement.isReadOnly() && !nedFileElement.hasSyntaxError()) {
                textEditor.pullChangesFromNEDResourcesWhenPending();
            }

            // keep the current selection between the two editors
            INEDElement currentNEDElementSelection = null;
            Object object = ((IStructuredSelection)graphicalEditor.getSite()
                    .getSelectionProvider().getSelection()).getFirstElement();
            if (object != null)
                currentNEDElementSelection = ((IModelProvider)object).getNedModel();

            if (currentNEDElementSelection != null)
                showInEditor(currentNEDElementSelection, Mode.TEXT);
		}
		else if (newPageIndex == graphPageIndex) {
		    textEditor.pushChangesIntoNEDResources();
		    graphicalEditor.markContent();
		    // earlier ned changes may not caused a refresh (because of optimizations)
		    // in the graphical editor (ie. the editor was not visible) so we must do it now
		    graphicalEditor.refresh();

            // keep the current selection between the two editors
	        INEDElement currentNEDElementSelection = null;
	        if (textEditor.getSite().getSelectionProvider().getSelection() instanceof IStructuredSelection) {
	            Object object = ((IStructuredSelection)textEditor.getSite()
	                    .getSelectionProvider().getSelection()).getFirstElement();
	            if (object != null)
	                currentNEDElementSelection = ((IModelProvider)object).getNedModel();
	        }
            if (currentNEDElementSelection!=null)
                showInEditor(currentNEDElementSelection, Mode.GRAPHICAL);
		}
		else
			throw new RuntimeException("Unknown page index");
	}
	
	@Override
	public boolean isDirty() {
        // The default behavior is wrong when undoing changes in both editors.
	    // This way at least the text editor dirtiness flag will be good.
	    return textEditor.isDirty();
	}

    /**
     * Prepares the content of the text editor before save.
     * If we are in a graphical mode it generates the text version and puts it into the text editor.
     */
    private void prepareForSave() {
        NedFileElementEx nedFileElement = getModel();

        if (getActivePage() == graphPageIndex && !nedFileElement.isReadOnly() && !nedFileElement.hasSyntaxError()) {
            textEditor.pullChangesFromNEDResourcesWhenPending();
            graphicalEditor.markSaved();
		}
		
		if (getActivePage() == textPageIndex)
		    textEditor.pushChangesIntoNEDResources();
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        prepareForSave();
        // delegate the save task to the TextEditor's save method
        textEditor.doSave(monitor);
        graphicalEditor.markSaved();
    }

    @Override
    public void doSaveAs() {
        prepareForSave();
        textEditor.doSaveAs();
        graphicalEditor.markSaved();
        setInput(textEditor.getEditorInput());
    }

	@Override
	public boolean isSaveAsAllowed() {
		return true;
	}

    /**
     * Closes the editor and optionally saves it.
     */
    protected void closeEditor(boolean save) {
        getSite().getPage().closeEditor(this, save);
    }

    /**
     * This class listens to changes to the file system in the workspace, and
     * makes changes accordingly.
     * 1) An open, saved file gets deleted -> close the editor
     * 2) An open file gets renamed or moved -> change the editor's input accordingly
     */
    class ResourceTracker implements IResourceChangeListener, IResourceDeltaVisitor {
        public void resourceChanged(IResourceChangeEvent event) {
            IResourceDelta delta = event.getDelta();
            try {
                if (delta != null) delta.accept(this);
            }
            catch (CoreException e) {
            	throw new RuntimeException(e);
            }
        }

        public boolean visit(IResourceDelta delta) {
            if (delta == null || !delta.getResource().equals(((IFileEditorInput) getEditorInput()).getFile()))
                return true;

            Display display = getSite().getShell().getDisplay();
            if (delta.getKind() == IResourceDelta.CHANGED) {
                display.asyncExec(new Runnable() {
                    public void run() {
                    	inputFileModifiedOnDisk();
                    }
                });
            }
            return false;
        }
    }

	protected void inputFileModifiedOnDisk() {
        // the file was overwritten somehow (could have been
        // replaced by another version in the repository)
        // TODO ask the user and reload the file
		if (isDirty()) {
		    ; //FIXME ask user!
		}
	}

    public void gotoMarker(IMarker marker) {
        // switch to text page and delegate to it
        setActivePage(textPageIndex);
        IGotoMarker gm = (IGotoMarker)textEditor.getAdapter(IGotoMarker.class);
        if (gm != null)
            gm.gotoMarker(marker);
    }

    public void showInEditor(INEDElement model, Mode mode) {
        if (mode == Mode.AUTOMATIC) {
            mode = model instanceof INedTypeElement || model instanceof SubmoduleElement ? Mode.GRAPHICAL : Mode.TEXT;
        }

        if (mode == Mode.GRAPHICAL) {
            setActivePage(graphPageIndex);
            graphicalEditor.reveal(model);
        }
        else {
            setActivePage(textPageIndex);
            IDocument document = textEditor.getDocumentProvider().getDocument(getEditorInput());
            if (model.getSourceRegion() != null)
                try {
                    int startLine = model.getSourceRegion().getStartLine();
                    int endLine = model.getSourceRegion().getEndLine();
                    int startOffset = document.getLineOffset(startLine-1)+model.getSourceRegion().getStartColumn();
                    int endOffset = document.getLineOffset(endLine-1)+model.getSourceRegion().getEndColumn();
                    textEditor.setHighlightRange(startOffset, endOffset - startOffset,
                                                 true);

                } catch (Exception e) {
                }
        }
    }

    // provides show in view options
    public String[] getShowInTargetIds() {
        return new String[] {IPageLayout.ID_PROP_SHEET,
                             IPageLayout.ID_OUTLINE,
                             IPageLayout.ID_RES_NAV,
                             IConstants.MODULEHIERARCHY_VIEW_ID,
                             IConstants.MODULEPARAMETERS_VIEW_ID,
                             IConstants.NEDINHERITANCE_VIEW_ID};
    }

    public ShowInContext getShowInContext() {
        return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
    }

    public GraphicalNedEditor getGraphEditor() {
        return graphicalEditor;
    }

    public TextualNedEditor getTextEditor() {
        return textEditor;
    }

    public IFile getFile() {
		return ((FileEditorInput)getEditorInput()).getFile();
	}

    public NedFileElementEx getModel() {
    	return NEDResourcesPlugin.getNEDResources().getNedFileElement(getFile());
    }

    public boolean isActiveEditor(IEditorPart editorPart) {
    	return getActiveEditor() == editorPart;
    }
}
