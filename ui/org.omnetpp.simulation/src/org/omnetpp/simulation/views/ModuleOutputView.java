package org.omnetpp.simulation.views;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.widgets.TextViewer;
import org.omnetpp.simulation.widgets.TextViewerContent;

/**
 *
 * @author Andras
 */
//TODO when view is closed, it should deregister its listener from LogBuffer!!! (see ModuleOutputContent's ctor)
public class ModuleOutputView extends ViewWithMessagePart {
    // note: view ID is defined in IConstants.java
    protected TextViewer viewer;
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");
    protected TextViewerContent BLANK_TEXT_CONTENT = new TextViewerContent("<blank text content>\n"); //XXX

    protected SimulationEditor associatedSimulationEditor;
    protected IPartListener partListener;

    protected ISimulationStateListener simulationListener;
    protected ISelectionChangedListener selectionChangeListener;


    /**
     * This is a callback that will allow us to create the viewer and initialize it.
     */
    public Control createViewControl(Composite parent) {
        viewer = new TextViewer(parent, SWT.V_SCROLL);
        viewer.setContent(BLANK_TEXT_CONTENT);
        viewer.setFont(JFaceResources.getTextFont());

        // create context menu
        getViewSite().registerContextMenu(contextMenuManager, viewer);
        viewer.setMenu(contextMenuManager.createContextMenu(viewer));
        //TODO dynamic menu based on which object is selected

        // export our selection to the workbench
        getViewSite().setSelectionProvider(new SelectionProvider());
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                getViewSite().getSelectionProvider().setSelection(event.getSelection());
            }
        });

        // listen on editor changes
        partListener = new IPartListener() {
            public void partActivated(IWorkbenchPart part) {
                if (part instanceof IEditorPart)
                    editorActivated((IEditorPart) part);
            }

            public void partBroughtToTop(IWorkbenchPart part) {
            }

            public void partClosed(IWorkbenchPart part) {
                if (part instanceof IEditorPart)
                    editorClosed((IEditorPart) part);
            }

            public void partDeactivated(IWorkbenchPart part) {
            }

            public void partOpened(IWorkbenchPart part) {
            }
        };
        getSite().getPage().addPartListener(partListener);  //TODO unhookListeners() etc -- see PinnableView!

        simulationListener = new ISimulationStateListener() {
            @Override
            public void simulationStateChanged(final SimulationController controller) {
                DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                    @Override
                    public void run() {
                        ModuleOutputView.this.simulationStateChanged(controller);
                    }
                });
            }
        };

        selectionChangeListener = new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent e) {
                System.out.println("module output view: got selection change, new selection: " + e.getSelection());  //TODO
                if (e.getSelection() instanceof IStructuredSelection) {
                    IStructuredSelection sel = (IStructuredSelection) e.getSelection();
                    if (sel.getFirstElement() instanceof cModule) { //FIXME not only the 1st elem, and use adaptable not instaceof
                        cModule module = (cModule) sel.getFirstElement();
                        ((ModuleOutputContent)viewer.getContent()).setFilterModuleFullPath(module.getFullPath()); //XXX
                    }
                }
            }
        };

        // associate ourselves with the current simulation editor
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                SimulationEditor editor = getActiveSimulationEditor();
                if (editor != null)
                    associateWithEditor(editor);
                else
                    showMessage("No associated simulation.");
            }
        });

        return viewer;
    }

    protected void simulationStateChanged(SimulationController controller) {
        if (controller.getState() == SimState.DISCONNECTED)
            showMessage("No simulation process.");
        else {
            hideMessage();
            if (!(viewer.getContent() instanceof ModuleOutputContent))
                viewer.setContent(new ModuleOutputContent(controller.getLogBuffer(), new EventEntryLinesProvider()));
            viewer.setCaretPosition(viewer.getContent().getLineCount()-1, 0);  // "go to end"  FIXME but not if caret has been moved by user somewhere else!
            viewer.refresh();
        }
    }

    protected void editorActivated(IEditorPart editor) {
        if (editor != associatedSimulationEditor && editor instanceof SimulationEditor)
            associateWithEditor((SimulationEditor)editor);
    }

    protected void editorClosed(IEditorPart editor) {
        if (editor == associatedSimulationEditor)
            disassociateFromEditor();
    }

    protected void associateWithEditor(SimulationEditor editor) {
        associatedSimulationEditor = editor;

        SimulationController controller = associatedSimulationEditor.getSimulationController();
        controller.addSimulationStateListener(simulationListener);

        associatedSimulationEditor.getSite().getSelectionProvider().addSelectionChangedListener(selectionChangeListener);

        if (controller.getState() == SimState.DISCONNECTED || !controller.hasRootObjects()) {
            showMessage("No simulation process.");
        }
        else {
            hideMessage();
            ModuleOutputContent content = new ModuleOutputContent(controller.getLogBuffer(), new EventEntryLinesProvider());
            viewer.setContent(content);
            viewer.setCaretPosition(viewer.getContent().getLineCount()-1, 0);  // "go to end"  FIXME but not if caret has been moved by user somewhere else!
            viewer.refresh();
        }
    }

    protected void disassociateFromEditor() {
        associatedSimulationEditor.getSimulationController().removeSimulationStateListener(simulationListener);
        associatedSimulationEditor.getSite().getSelectionProvider().removeSelectionChangedListener(selectionChangeListener);
        associatedSimulationEditor = null;
        viewer.setContent(BLANK_TEXT_CONTENT);
        viewer.refresh();

        showMessage("No associated simulation.");
    }

    protected SimulationEditor getActiveSimulationEditor() {
        IWorkbenchPartSite site = getSite();
        IWorkbenchPage page = site==null ? null : site.getWorkbenchWindow().getActivePage();
        if (page != null) {
            if (page.getActiveEditor() instanceof SimulationEditor)
                return (SimulationEditor)page.getActiveEditor();
            // no active simulation editor; just return the first one we find
            for (IEditorReference ref : page.getEditorReferences())
                if (ref.getEditor(false) instanceof SimulationEditor)
                    return (SimulationEditor)ref.getEditor(false);
        }
        return null;
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
        viewer.setFocus();
    }

    @Override
    public void dispose() {
        if (associatedSimulationEditor != null)
            associatedSimulationEditor.getSimulationController().removeSimulationStateListener(simulationListener);
        super.dispose();
    }
}
