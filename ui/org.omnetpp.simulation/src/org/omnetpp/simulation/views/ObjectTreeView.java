package org.omnetpp.simulation.views;

import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.simulation.controller.ISimulationChangeListener;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectFieldsViewer;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;

/**
 *
 * @author Andras
 */
//FIXME hasznaljuk a sajat hoverSupportunkat, ne az editoret!!! (forget() nem megy!!!)
public class ObjectTreeView extends ViewWithMessagePart implements ISimulationEditorChangeListener, ISimulationChangeListener {
    // note: view ID is defined in IConstants.java
    protected ObjectFieldsViewer viewer;
    protected SimulationEditorProxy simulationEditorProxy;

    /**
     * This is a callback that will allow us to create the viewer and initialize it.
     */
    public Control createViewControl(Composite parent) {
        viewer = new ObjectFieldsViewer(parent, SWT.DOUBLE_BUFFERED | SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
        viewer.setMode(Mode.CHILDREN);

        // create context menu
        final MenuManager contextMenuManager = new MenuManager("#PopupMenu");
        getViewSite().registerContextMenu(contextMenuManager, viewer.getTreeViewer());
        viewer.getTree().setMenu(contextMenuManager.createContextMenu(viewer.getTree()));
        contextMenuManager.setRemoveAllWhenShown(true);
        contextMenuManager.addMenuListener(new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager manager) {
                SimulationEditor associatedSimulationEditor = simulationEditorProxy.getAssociatedSimulationEditor();
                associatedSimulationEditor.populateContextMenu(contextMenuManager, viewer.getSelection());
            }
        });

        // double-click opens an inspector
        viewer.getTreeViewer().addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                Object element = ((IStructuredSelection)event.getSelection()).getFirstElement();
                if (element instanceof cObject) {
                    SimulationEditor associatedSimulationEditor = simulationEditorProxy.getAssociatedSimulationEditor();
                    associatedSimulationEditor.openInspector((cObject)element);
                }
            }
        });

        // export our selection to the workbench
        getViewSite().setSelectionProvider(new SelectionProvider());
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                getViewSite().getSelectionProvider().setSelection(event.getSelection());
            }
        });

        simulationEditorProxy = new SimulationEditorProxy();
        simulationEditorProxy.addSimulationEditorChangeListener(this);
        simulationEditorProxy.addSimulationStateListener(this);

        // associate ourselves with the current simulation editor
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                if (!isDisposed())
                    refresh();
            }
        });

        return viewer.getTree();
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // ensure this view has a custom page in Properties View; see PropertySheet documentation
        if (adapter.equals(IPropertySheetPage.class))
            return new SimulationObjectPropertySheetPage();
        return super.getAdapter(adapter);
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
        viewer.getTree().setFocus();
    }

    @Override
    public void associatedSimulationEditorChanged(SimulationEditor editor) {
        refresh();
    }

    public void simulationStateChanged(SimulationController controller) {
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            @Override
            public void run() {
                if (!isDisposed())
                    refresh();
            }
        });
    }

    public void refresh() {
        SimulationEditor editor = simulationEditorProxy.getAssociatedSimulationEditor();
        SimulationController controller = (editor == null) ? null : editor.getSimulationController();
        if (controller == null || controller.getUIState() == SimState.DISCONNECTED || !controller.getSimulation().hasRootObjects()) {
            showMessage("No simulation process.");
        }
        else {
            hideMessage();
            cObject input = controller.getSimulation().getRootObject(Simulation.ROOTOBJ_SIMULATION);
            if (!input.equals(viewer.getInput()))
                viewer.setInput(input);
            viewer.refresh();
        }
    }

    @Override
    public void dispose() {
        simulationEditorProxy.dispose();
        simulationEditorProxy = null;
        super.dispose();
    }

}
