package org.omnetpp.simulation.views;

import java.util.List;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.controller.ISimulationChangeListener;
import org.omnetpp.simulation.controller.SimulationChangeEvent;
import org.omnetpp.simulation.controller.SimulationChangeEvent.Reason;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.ui.StringTextViewerContentProvider;
import org.omnetpp.simulation.ui.TextViewer;

/**
 *
 * @author Andras
 */
public class ModuleOutputView extends ViewWithMessagePart implements ISimulationEditorChangeListener, ISimulationChangeListener, ISelectionChangedListener {
    // note: view ID is defined in IConstants.java
    protected TextViewer viewer;
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");
    protected StringTextViewerContentProvider BLANK_TEXT_CONTENT = new StringTextViewerContentProvider("<blank text content>\n"); //XXX

    protected SimulationEditorProxy simulationEditorProxy;

    /**
     * This is a callback that will allow us to create the viewer and initialize it.
     */
    public Control createViewControl(Composite parent) {
        viewer = new TextViewer(parent, SWT.V_SCROLL);
        viewer.setContentProvider(BLANK_TEXT_CONTENT);
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

        simulationEditorProxy = new SimulationEditorProxy();
        simulationEditorProxy.addSimulationEditorChangeListener(this);
        simulationEditorProxy.addSimulationStateListener(this);
        simulationEditorProxy.addSelectionChangedListener(this);
        //or to follow the global selection: getSite().getPage().getWorkbenchWindow().getSelectionService().addSelectionListener(this);

        // associate ourselves with the current simulation editor
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                if (!isDisposed())
                    refresh();
            }
        });

        return viewer;
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
        viewer.setFocus();
    }

    @Override
    public void associatedSimulationEditorChanged(SimulationEditor editor) {
        refresh();
    }

    @Override
    public void simulationStateChanged(SimulationChangeEvent e) {
        if (e.reason == Reason.OBJECTCACHE_REFRESH && !isDisposed())
            refresh();
    }

    public void refresh() {
        SimulationEditor editor = simulationEditorProxy.getAssociatedSimulationEditor();
        SimulationController controller = (editor == null) ? null : editor.getSimulationController();
        if (controller == null || !controller.hasSimulationProcess()) {
            showMessage("No simulation process.");
            viewer.setContentProvider(BLANK_TEXT_CONTENT);
            viewer.refresh();
        }
        else {
            hideMessage();
            if (!(viewer.getContentProvider() instanceof ModuleOutputContentProvider) ||
                    ((ModuleOutputContentProvider)viewer.getContentProvider()).getLogBuffer() != controller.getSimulation().getLogBuffer())
                viewer.setContentProvider(new ModuleOutputContentProvider(controller.getSimulation().getLogBuffer(), new EventEntryLinesProvider()));
            ((ModuleOutputContentProvider)viewer.getContentProvider()).refresh();
            viewer.setCaretPosition(viewer.getContentProvider().getLineCount()-1, 0);  // "go to end"  FIXME but not if caret has been moved by user somewhere else!
            viewer.refresh();
        }
    }

    @Override
    public void selectionChanged(SelectionChangedEvent e) {
        ISelection selection = e.getSelection();
        Debug.println("module output view: got selection change, new selection: " + selection);
        if (selection instanceof IStructuredSelection) {
            // update the module filter
            List<cModule> selectedModules = SelectionUtils.getObjects(selection, cModule.class);
            String[] modulePaths = new String[selectedModules.size()];
            for (int i = 0; i < modulePaths.length; i++)
                modulePaths[i] = selectedModules.get(i).getFullPath();
            ModuleOutputContentProvider moduleOutputContentProvider = (ModuleOutputContentProvider)viewer.getContentProvider();
            moduleOutputContentProvider.setFilter(new ModulePathsEventEntryFilter(modulePaths));
        }
    }

    @Override
    public void dispose() {
        simulationEditorProxy.dispose();
        simulationEditorProxy = null;
        super.dispose();
    }

}
