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
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPageLayout;
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
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Multi-page NED editor.
 *
 * FIXME File|Open in Eclipse won't work!!! it creates a JavaFileEditorInput which is NOT an IFileEditorInput!!!
 */
public class MultiPageNedEditor extends MultiPageEditorPart implements
		IGotoNedElement, IGotoMarker, IShowInTargetList, IShowInSource {

    private GraphicalNedEditor graphEditor;
	private TextualNedEditor textEditor;
    private final ResourceTracker resourceListener = new ResourceTracker();

	private int graphPageIndex;
	private int textPageIndex;
	private boolean insidePageChange = false;
    private boolean initPhase = true;

	@Override
    public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {
		super.init(site, editorInput);
        if (!(editorInput instanceof IFileEditorInput))
            throw new PartInitException("Invalid input type!");
        setInput(editorInput);

        ((IFileEditorInput)getEditorInput()).getFile()
            .getWorkspace().addResourceChangeListener(resourceListener);
	}

    @Override
    public void dispose() {
        // detach the editor file from the core plugin and do not set a new file
        ((IFileEditorInput)getEditorInput()).getFile()
                .getWorkspace().removeResourceChangeListener(resourceListener);
        super.dispose();
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.part.EditorPart#setInput(org.eclipse.ui.IEditorInput)
     * Add remove listeners on input change
     */
    @Override
    protected void setInput(IEditorInput input) {
        // do nothing if no change has occured
        if (ObjectUtils.equals(getEditorInput(), input))
            return;
        // remove the listeners from the old file
        if (getEditorInput() != null) {
            IFile file = ((IFileEditorInput) getEditorInput()).getFile();
            NEDResourcesPlugin.getNEDResources().disconnect(file);
        }

        super.setInput(input);
        // set the input on the embedded editors
        if (graphEditor != null)
            graphEditor.setInput(input);
        if (textEditor != null)
            textEditor.setInput(input);

        // add listeners to the new file in workspace and in the ned resource manager
        if (getEditorInput() != null) {
            IFile file = ((IFileEditorInput) getEditorInput()).getFile();
            NEDResourcesPlugin.getNEDResources().connect(file);
            setPartName(file.getName());
        }
    }

	@Override
	protected void createPages() {
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

            // switch to graphics mode initially if there's no error in the file
            if (!NEDResourcesPlugin.getNEDResources().containsNEDErrors(file))
                setActivePage(graphPageIndex);

		} catch (PartInitException e) {
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

	@Override
	protected void pageChange(int newPageIndex) {
	    //	prevent recursive call from setActivePage() below
	    if (insidePageChange)
	        return;

	    insidePageChange = true;

        super.pageChange(newPageIndex);

        // XXX this is a MEGA hack because currently the workbench do not send a partActivated,deactivated messge
        // for the embedded editors in a MultiPageEditorView (this is a missing unimplemented feature, it works with MultiEditor however)
        // to make the nedded outline page active we should send activate/deactivate directly
        // we look for the outline view directy and send the notification by hand. once the MultiPageEditors are handled correctly
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
        // it would be ok to invalidate only the components inside this file


		// switch from graphics to text:
        if (newPageIndex == textPageIndex) {
            if (graphEditor.hasContentChanged()) {
                // TODO refresh the editor annotations to show the error marks
                res.setNEDFileModel(file, graphEditor.getModel());
                // XXX this is a hack so the NED elements will have a correct source position/location info
                // after the parser/generator reformats it (maybe we would need a
                res.formatNEDFileText(file);
                // generate text representation from the model
                textEditor.setText(res.getNEDFileText(file));
                textEditor.markContent();
            }
            //
            // keep the current selection between the two editors
            INEDElement currentNEDElementSelection = null;
            Object object = ((IStructuredSelection)graphEditor.getSite()
                    .getSelectionProvider().getSelection()).getFirstElement();
            if (object != null)
                currentNEDElementSelection = ((IModelProvider)object).getNEDModel();

            if (currentNEDElementSelection != null)
                showInEditor(currentNEDElementSelection, Mode.TEXT);
		}
		else if (newPageIndex==graphPageIndex) {

		    if (textEditor.hasContentChanged()) {
    			// parse the text editor content if it has changed since the last editor switch
    		    res.setNEDFileText(file, textEditor.getText());
		    }
		    // set the parsed ned model to the graphical editor
		    graphEditor.setModel((NedFileNodeEx)res.getNEDFileModel(file));
		    graphEditor.markContent();
            //
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
            if (res.containsNEDErrors(file)) {
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
		insidePageChange = false;
        initPhase = false;
	}

    /**
     * Prepares the content of the text editor before save
     * if we are in a graphical mode it generate the text version and puts it into the
     * text editor
     */
    private void prepareForSave() {
        if (getActivePage() == graphPageIndex) {
			// switch from graphics to text:
			// generate text representation from the model
            IFile file = ((FileEditorInput)getEditorInput()).getFile();

            // put the actual model state back to the incremental builder
    		NEDResourcesPlugin.getNEDResources().setNEDFileModel(file, graphEditor.getModel());
            // put it into the text editor
            textEditor.setText(NEDResourcesPlugin.getNEDResources().getNEDFileText(file));
            graphEditor.getEditDomain().getCommandStack().markSaveLocation();
		}
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        prepareForSave();
        // delegate the save task to the TextEditor's save method
        textEditor.doSave(monitor);
    }

    @Override
    public void doSaveAs() {
        prepareForSave();
        textEditor.doSaveAs();
        setInput(textEditor.getEditorInput());
    }

	@Override
	public boolean isSaveAsAllowed() {
		return true;
	}

    /**
     * closes the editor and optionally saves it.
     * @param save
     */
    protected void closeEditor(boolean save) {
        getSite().getPage().closeEditor(this, save);
    }

    // resource management open, close the editor depending on workspace notification
    // This class listens to changes to the file system in the workspace, and
    // makes changes accordingly.
    // 1) An open, saved file gets deleted -> close the editor
    // 2) An open file gets renamed or moved -> change the editor's input
    // accordingly
    class ResourceTracker implements IResourceChangeListener, IResourceDeltaVisitor {
        public void resourceChanged(IResourceChangeEvent event) {
            IResourceDelta delta = event.getDelta();
            try {
                if (delta != null) delta.accept(this);
            } catch (CoreException exception) {
                // What should be done here?
            }
        }

        public boolean visit(IResourceDelta delta) {
            if (delta == null || !delta.getResource().equals(((IFileEditorInput) getEditorInput()).getFile()))
                return true;

            if (delta.getKind() == IResourceDelta.REMOVED) {
                Display display = getSite().getShell().getDisplay();
                if ((IResourceDelta.MOVED_TO & delta.getFlags()) == 0) {
                    // if the file was deleted
                    display.asyncExec(new Runnable() {
                        public void run() {
                            if (!isDirty()) closeEditor(false);
                        }
                    });
                } else { // else if it was moved or renamed
                    final IFile newFile = ResourcesPlugin.getWorkspace().getRoot().getFile(
                            delta.getMovedToPath());
                    display.asyncExec(new Runnable() {
                        public void run() {
                            setInput(new FileEditorInput(newFile));
                        }
                    });
                }
            } else if (delta.getKind() == IResourceDelta.CHANGED) {
                // guard that we shoul dnot reload while save is in progress
//                if (!editorSaving) {

                  // the file was overwritten somehow (could have been
                  // replaced by another version in the respository)
                  // TODO ask the user and reload the file

//                }
            }
            return false;
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
            mode = model instanceof ITopLevelElement || model instanceof SubmoduleNode ? Mode.GRAPHICAL : Mode.TEXT;
        }

        if (mode == Mode.GRAPHICAL) {
            setActivePage(graphPageIndex);
            graphEditor.reveal(model);
        } else {
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
}
