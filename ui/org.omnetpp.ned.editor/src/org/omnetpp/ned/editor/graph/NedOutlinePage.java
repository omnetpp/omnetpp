package org.omnetpp.ned.editor.graph;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.dnd.TemplateTransferDropTargetListener;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.parts.AbstractEditPartViewer;
import org.eclipse.gef.ui.parts.ContentOutlinePage;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.util.TransferDropTargetListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.part.IPageSite;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.graph.actions.GNEDContextMenuProvider;
import org.omnetpp.ned.editor.graph.parts.outline.NedTreeEditPartFactory;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * Outline viewer for graphical ned editor
 */
class NedOutlinePage extends ContentOutlinePage implements INEDChangeListener, ISelectionListener {
    private final GraphicalNedEditor graphicalNedEditor;
	private boolean linkWithEditor = false;

    public NedOutlinePage(GraphicalNedEditor graphicalNedEditor, EditPartViewer viewer) {
        super(viewer);
        this.graphicalNedEditor = graphicalNedEditor;
    }

    @Override
    public void init(IPageSite pageSite) {
        super.init(pageSite);
        NEDResources.getInstance().addNEDModelChangeListener(this);
        getSite().getPage().addSelectionListener(this);
        getSite().getPage().addPostSelectionListener(this);
        // register actions for the editor
        ActionRegistry registry = this.graphicalNedEditor.getActionRegistry();
        IActionBars bars = pageSite.getActionBars();
        String id = ActionFactory.UNDO.getId();
        bars.setGlobalActionHandler(id, registry.getAction(id));
        id = ActionFactory.REDO.getId();
        bars.setGlobalActionHandler(id, registry.getAction(id));
        id = ActionFactory.DELETE.getId();
        bars.setGlobalActionHandler(id, registry.getAction(id));

        id = ActionFactory.CUT.getId();
        bars.setGlobalActionHandler(id, registry.getAction(id));
        id = ActionFactory.COPY.getId();
        bars.setGlobalActionHandler(id, registry.getAction(id));
        id = ActionFactory.PASTE.getId();
        bars.setGlobalActionHandler(id, registry.getAction(id));
        bars.updateActionBars();
    }

    @Override
    public void createControl(Composite parent) {
    	// TODO KLUDGE: This block removes the drag source and drop targets so that dragging will work when switch back and forth
    	//              between the text and graphical editors. See comment in MultiPageNedEditor in pageChange.
    	//              Both the drag source and the drag target are disposed by their corresponding setter methods.
    	Method method = ReflectionUtils.getMethod(AbstractEditPartViewer.class, "setDragSource", DragSource.class);
    	ReflectionUtils.setAccessible(method);
    	ReflectionUtils.invokeMethod(method, getViewer(), (DragSource)null);
    	method = ReflectionUtils.getMethod(AbstractEditPartViewer.class, "setDropTarget", DropTarget.class);
    	ReflectionUtils.setAccessible(method);
    	ReflectionUtils.invokeMethod(method, getViewer(), (DropTarget)null);
    	// KLUDGE END

    	super.createControl(parent);
    	
        getViewer().setEditDomain(graphicalNedEditor.getEditDomain());
        getViewer().setEditPartFactory(new NedTreeEditPartFactory());
        ContextMenuProvider provider = new GNEDContextMenuProvider(getViewer(), graphicalNedEditor.getActionRegistry());
        getViewer().setContextMenu(provider);
        getViewer().setKeyHandler(this.graphicalNedEditor.getCommonKeyHandler());
        getViewer().addDropTargetListener((TransferDropTargetListener)
                new TemplateTransferDropTargetListener(getViewer()));
        graphicalNedEditor.getSelectionSynchronizer().addViewer(getViewer());
        setContents(graphicalNedEditor.getModel());
    }

    @Override
    public void dispose() {
        graphicalNedEditor.getSelectionSynchronizer().removeViewer(getViewer());
        getSite().getPage().removeSelectionListener(this);
        getSite().getPage().removePostSelectionListener(this);
        NEDResources.getInstance().removeNEDModelChangeListener(this);
        super.dispose();
    }

    public void setContents(Object contents) {
        getViewer().setContents(contents);
    }

    private void refresh() {
        getViewer().getContents().refresh();
    }

	public void modelChanged(NEDModelEvent event) {
	    if (Display.getCurrent() != null) {
	        refresh();
	    }
	    else {
	        Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    refresh();
                }
	        });
	    }
	}

	public void selectionChanged(IWorkbenchPart part, ISelection selection) {
		// we don't care about selection change events if they were not generated by the active nedEditor
		if (getActiveEditor() != part || !(part instanceof NedEditor) || !linkWithEditor)
			return;

		// generally the synchronization is done by the graphicalEditors SelectionSynchronizer
		// but we handle the SelSync with the TextEditor differently. We receive text selections from there
		// and if any selection change event received we get the selection from the active editor
		// turn it to editparts (as text editor selection contains model objects not edit parts)
		// and pass it to the outline view. We disable the general sync to avoid syncing back the 
		// selection to the text editor
		// Debug.println("*** NedOutline selecion changed from: "+part);
		if (selection instanceof ITextSelection) { 
			ISelection activeEditorSelection = getActiveEditorSelection();
			if (activeEditorSelection instanceof IStructuredSelection) {
				List partSelList = new ArrayList();
				for (Object sel : ((IStructuredSelection)activeEditorSelection).toArray()) {
					Object selPart = getViewer().getEditPartRegistry().get(sel);
					if (selPart != null)
						partSelList.add(selPart);
				}
				getViewer().setSelection(new StructuredSelection(partSelList));
			}
		}
	}
	
	
	
	/**
	 * Utility method: Returns the active editor, or null.
	 */
	protected IEditorPart getActiveEditor() {
		IPageSite site = getSite();
		IWorkbenchPage activePage = site==null ? null : site.getWorkbenchWindow().getActivePage();
		return activePage==null ? null : activePage.getActiveEditor();
	}
	
	/**
	 * Utility method: Return the active editor's selection, or null.
	 */
	protected ISelection getActiveEditorSelection() {
		IEditorPart editor = getActiveEditor();
		ISelectionProvider selectionProvider = editor==null ? null : editor.getSite().getSelectionProvider();
		return selectionProvider==null ? null : selectionProvider.getSelection();
	}
	
}