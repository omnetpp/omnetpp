package org.omnetpp.ned.editor;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.eclipse.ui.part.ShowInContext;
import org.eclipse.ui.views.contentoutline.ContentOutline;
import org.omnetpp.common.IConstants;
import org.omnetpp.ned.core.IGotoNedElement;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.engine.NEDErrorStore;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

//FIXME why doesn't this comment go into the normal class comment? --Andras
// MultiPageNedEditor binds the two separate NED based editor together. Both the text and the graphical
// editor maintains its own model independent of each other. The two model should be synchronized during
// page change / or save operation. Additionally the model should be put back into the NEDResources, so
// name lookup and other services will work correctly. Save is done by delegation to the Text editor's save
// method (ie. The graphical editor itself cannot save its model, the multipage editor must first obtain
// the model from the graphical editor, convert to text and pass it to the Text based editor and then
// call the Text editor to save its content.
//
// when setting the input of the multipage editor both embedded editor should be notified (ie setInput must be
// delegated)

/**
 * Multi-page NED editor.
 *
 * FIXME File|Open in Eclipse won't work!!! it creates a JavaFileEditorInput which is NOT an IFileEditorInput!!!
 *
 * @author rhornig
 */
public class MultiPageNedEditor
	extends MultiPageEditorPart
	implements IGotoNedElement, IGotoMarker, IShowInTargetList, IShowInSource
{
    private GraphicalNedEditor graphEditor;
	private TextualNedEditor textEditor;
    private final ResourceTracker resourceListener = new ResourceTracker();

	private int graphPageIndex;
	private int textPageIndex;
	private boolean insidePageChange = false;
    private boolean initPhase = true;

	@Override
    public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		if (!(editorInput instanceof IFileEditorInput))
            throw new PartInitException("Invalid input type (only workspace files can be opened): " + editorInput);
		
		super.init(site, editorInput);

        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceListener);

		getSite().getPage().addPartListener(new IPartListener() {
			public void partOpened(IWorkbenchPart part) {
			}

			public void partClosed(IWorkbenchPart part) {
			}

			public void partActivated(IWorkbenchPart part) {
				if (getActivePage() == textPageIndex)
					textEditor.pullChangesFromNEDResources();
			}

			public void partDeactivated(IWorkbenchPart part) {
				if (getActivePage() == textPageIndex && textEditor.hasContentChanged())
					textEditor.pushChangesIntoNEDResources();
			}

			public void partBroughtToTop(IWorkbenchPart part) {
			}
		});
	}

    @Override
    public void dispose() {
        // detach the editor file from the core plugin and do not set a new file
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceListener);
        
        // disconnect the editor from the ned resources plugin
        setInput(null);
        super.dispose();
    }

    @Override
    protected void setInput(IEditorInput newInput) {
		System.out.println("setInput()");

		IEditorInput oldInput = getEditorInput();
		if (ObjectUtils.equals(oldInput, newInput))
            return; // no change 

        if (oldInput != null) {
        	// disconnect() must be *after* setInput(null)
            super.setInput(null);
            if (graphEditor != null)
                graphEditor.setInput(null);
            if (textEditor != null)
                textEditor.setInput(null);
            
            IFile oldFile = ((IFileEditorInput) oldInput).getFile();
            NEDResourcesPlugin.getNEDResources().disconnect(oldFile);
        }

        if (newInput != null) {
        	// connect() must take place *before* setInput()
        	IFile newFile = ((IFileEditorInput) newInput).getFile();
        	NEDResourcesPlugin.getNEDResources().connect(newFile);

        	super.setInput(newInput);
        	if (graphEditor != null)
        		graphEditor.setInput(newInput);
        	if (textEditor != null)
        		textEditor.setInput(newInput);
        	setPartName(newFile.getName());
        }
    }

	@Override
	protected void createPages() {
		System.out.println("createPages()");

		graphEditor = new GraphicalNedEditor();
		textEditor = new TextualNedEditor();

        IFile file = ((FileEditorInput)getEditorInput()).getFile();

		try {
            // setup graphical editor
            graphPageIndex = addPage(graphEditor, getEditorInput());
            graphEditor.markContent();
            setPageText(graphPageIndex,"Graphical");

            // setup text editor
            // we don't have to set the content because it's set
            // automatically by the text editor (from the FileEditorInput)
            textPageIndex = addPage(textEditor, getEditorInput());
            textEditor.markContent();
            setPageText(textPageIndex,"Text");
            // remember the initial content so we can detect any change later

    		//XXX force parsing this file now -- needed???
            //NEDResourcesPlugin.getNEDResources().setNEDFileText(file, textEditor.getText());

            // switch to graphics mode initially if there's no error in the file
            if (!NEDResourcesPlugin.getNEDResources().hasError(file))
                setActivePage(graphPageIndex);
		} 
		catch (PartInitException e) {
		    NedEditorPlugin.logError(e);
		}

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
	 *
	 */
	@Override
	protected void pageChange(int newPageIndex) {
	    //	prevent recursive call from setActivePage() below
	    if (insidePageChange)
	        return;

	    insidePageChange = true;

        super.pageChange(newPageIndex);

        // XXX this is a MEGA hack because currently the workbench do not send a partActivated, deactivated message
        // for the embedded editors in a MultiPageEditorView (this is a missing unimplemented feature, it works with MultiEditor however)
        // to make the ned editor outline page active we should send activate/deactivate directly
        // we look for the outline view directly and send the notification by hand. once the MultiPageEditors are handled correctly
        // this can be removed
        // on each page change we emulate a close/open cycle of the multipage editor, this removed the associated
        // outline page, so the outline view will re-request the multipageeditor for a ContentOutlinePage (via getAdapter)
        // the current implementation of MultipageEditorPart.getAdapter delegates this request to the active
        // embedded editor.
        ContentOutline coutline = (ContentOutline)getEditorSite().getPage().findView(IPageLayout.ID_OUTLINE);
        if (coutline != null) {
            // notify from the old closed editor
            coutline.partClosed(this);
            coutline.partActivated(this);
        }
        // end of the hack

        IFile file = ((FileEditorInput)getEditorInput()).getFile();
		NEDResources res = NEDResourcesPlugin.getNEDResources();
        // XXX FIXME this may be a way too slow as invalidates everything
        // it is needed only to display consistency errors correctly during page switching
        // it would be OK to invalidate only the components inside this file


		// switch from graphics to text:
        if (newPageIndex == textPageIndex) {
            if (graphEditor.hasContentChanged()) {
                // TODO refresh the editor annotations to show the error marks
                String source = graphEditor.getModel().getNEDSource();
                // we try to reformat and re-parse the model so the element line number attributes will be correct
                NEDErrorStore errors = new NEDErrorStore();
                INEDElement reformattedModel = NEDTreeUtil.parseNedSource(source, errors, file.getLocation().toOSString());
                // if the re parse was successful use the reformatted tree
                if (!errors.containsError())
                    res.setNEDFileModel(file, reformattedModel);
                else
                    // otherwise we use the original tree
                    res.setNEDFileModel(file, graphEditor.getModel());

                // generate text representation from the model
                textEditor.pullChangesFromNEDResources();
                textEditor.markContent();
            }

            // keep the current selection between the two editors
            INEDElement currentNEDElementSelection = null;
            Object object = ((IStructuredSelection)graphEditor.getSite()
                    .getSelectionProvider().getSelection()).getFirstElement();
            if (object != null)
                currentNEDElementSelection = ((IModelProvider)object).getNEDModel();

            if (currentNEDElementSelection != null)
                showInEditor(currentNEDElementSelection, Mode.TEXT);
		}
		else if (newPageIndex == graphPageIndex) {
		    if (textEditor.hasContentChanged()) {
		    	textEditor.pushChangesIntoNEDResources();
    		    graphEditor.markContent();
		    }

            // keep the current selection between the two editors
	        INEDElement currentNEDElementSelection = null;
	        if (textEditor.getSite().getSelectionProvider().getSelection() instanceof IStructuredSelection) {
	            Object object = ((IStructuredSelection)textEditor.getSite()
	                    .getSelectionProvider().getSelection()).getFirstElement();
	            if (object != null)
	                currentNEDElementSelection = ((IModelProvider)object).getNEDModel();
	        }
            if (currentNEDElementSelection!=null)
                showInEditor(currentNEDElementSelection, Mode.GRAPHICAL);

            // only start in graphics mode if there's no error in the file
            // FIXME sometimes the editor goes into graphical mode even if the file has errors
            // could it be because the error store is synchronized in the background???
            if (res.hasError(file)) {
                // this happens if the parsing was unsuccessful when we wanted to switch from text to graph mode
				// parse error: switch back immediately to text view (we should never have
				// switched away from it in the first place)
				setActivePage(textPageIndex);

				if (!initPhase) {
		            MessageDialog.openError(Display.getDefault().getActiveShell(), "Error",
		                    "The editor contents has errors, switching is not possible. Please fix the errors first.");
	            }
			}
		}
		else
			throw new RuntimeException("Unknown page index");

        insidePageChange = false;
        initPhase = false;
	}

    /**
     * Prepares the content of the text editor before save.
     * If we are in a graphical mode it generates the text version and puts it into the text editor.
     */
    private void prepareForSave() {
        if (getActivePage() == graphPageIndex) {
            textEditor.pullChangesFromNEDResources();
            graphEditor.getEditDomain().getCommandStack().markSaveLocation();
		}
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        prepareForSave();
        // delegate the save task to the TextEditor's save method
        textEditor.doSave(monitor);
        graphEditor.markSaved();
    }

    @Override
    public void doSaveAs() {
        prepareForSave();
        textEditor.doSaveAs();
        graphEditor.markSaved();
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
            if (delta.getKind() == IResourceDelta.REMOVED) {
                if ((IResourceDelta.MOVED_TO & delta.getFlags()) == 0) {
                    // if the file was deleted
                    display.asyncExec(new Runnable() {
                        public void run() {
                            inputFileDeletedFromDisk();
                        }
                    });
                }
                else { 
                	// else if it was moved or renamed
                    final IFile newFile = ResourcesPlugin.getWorkspace().getRoot().getFile(delta.getMovedToPath());
                    display.asyncExec(new Runnable() {
                        public void run() {
                            inputFileMovedOrRenamedOnDisk(newFile);
                        }
                    });
                }
            }
            else if (delta.getKind() == IResourceDelta.CHANGED) {
                display.asyncExec(new Runnable() {
                    public void run() {
                    	inputFileModifiedOnDisk();
                    }
                });
            }
            return false;
        }
    }

	protected void inputFileDeletedFromDisk() {
		if (!isDirty()) 
			closeEditor(false);
		else
			; //TODO ask user?
	}

	protected void inputFileMovedOrRenamedOnDisk(IFile newFile) {
		setInput(new FileEditorInput(newFile));
	}

	protected void inputFileModifiedOnDisk() {
        // the file was overwritten somehow (could have been
        // replaced by another version in the repository)
        // TODO ask the user and reload the file
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
            mode = model instanceof INedTypeNode || model instanceof SubmoduleNode ? Mode.GRAPHICAL : Mode.TEXT;
        }

        if (mode == Mode.GRAPHICAL) {
            setActivePage(graphPageIndex);
            graphEditor.reveal(model);
        }
        else {
            setActivePage(textPageIndex);
            IDocument document = textEditor.getDocumentProvider().getDocument(getEditorInput());
            if (model.getSourceRegion() != null)
                try {
                    int startLine = model.getSourceRegion().startLine;
                    int endLine = model.getSourceRegion().endLine;
                    int startOffset = document.getLineOffset(startLine-1)+model.getSourceRegion().startColumn;
                    int endOffset = document.getLineOffset(endLine-1)+model.getSourceRegion().endColumn;
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
                             IConstants.MODULEPARAMETERS_VIEW_ID};
    }

    public ShowInContext getShowInContext() {
        return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
    }

    public GraphicalNedEditor getGraphEditor() {
        return graphEditor;
    }

    public TextualNedEditor getTextEditor() {
        return textEditor;
    }
    
    public boolean isActiveEditor(IEditorPart editorPart) {
    	return getActiveEditor() == editorPart;
    }
}
