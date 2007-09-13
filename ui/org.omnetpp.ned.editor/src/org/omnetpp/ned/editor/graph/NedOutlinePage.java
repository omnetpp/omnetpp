package org.omnetpp.ned.editor.graph;

import java.lang.reflect.Method;

import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.dnd.TemplateTransferDropTargetListener;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.parts.AbstractEditPartViewer;
import org.eclipse.gef.ui.parts.ContentOutlinePage;
import org.eclipse.jface.util.TransferDropTargetListener;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.part.IPageSite;

import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ned.editor.graph.actions.GNEDContextMenuProvider;
import org.omnetpp.ned.editor.graph.edit.outline.NedTreeEditPartFactory;

/**
 * Outline viewer for graphical ned editor
 */
class NedOutlinePage extends ContentOutlinePage {
    /**
     * 
     */
    private final GraphicalNedEditor graphicalNedEditor;

    public NedOutlinePage(GraphicalNedEditor graphicalNedEditor, EditPartViewer viewer) {
        super(viewer);
        this.graphicalNedEditor = graphicalNedEditor;
    }

    @Override
    public void init(IPageSite pageSite) {
        super.init(pageSite);
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
        getViewer().setEditDomain(this.graphicalNedEditor.getEditDomain());
        getViewer().setEditPartFactory(new NedTreeEditPartFactory());
        ContextMenuProvider provider = new GNEDContextMenuProvider(getViewer(), this.graphicalNedEditor.getActionRegistry());
        getViewer().setContextMenu(provider);
        getViewer().setKeyHandler(this.graphicalNedEditor.getCommonKeyHandler());
        getViewer().addDropTargetListener((TransferDropTargetListener)
                new TemplateTransferDropTargetListener(getViewer()));
        this.graphicalNedEditor.getSelectionSynchronizer().addViewer(getViewer());
        setContents(this.graphicalNedEditor.getModel());
    }

    @Override
    public void dispose() {
        this.graphicalNedEditor.getSelectionSynchronizer().removeViewer(getViewer());
        super.dispose();
    }

    public void setContents(Object contents) {
        getViewer().setContents(contents);
    }

}