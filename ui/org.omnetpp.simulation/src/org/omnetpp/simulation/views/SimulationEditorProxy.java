package org.omnetpp.simulation.views;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * Allows adding simulation state listeners and selection change listeners to
 * the active simulation editor; when the active simulation editor changes, the
 * listeners will be moved over to the new editor. Clients can also request to
 * be notified when the active simulation editor changes (the current one is
 * closed or the user switches to another one.)
 *
 * @author Andras
 */
public class SimulationEditorProxy {
    private IWorkbenchPage workbenchPage;
    private IPartListener partListener;
    private SimulationEditor associatedSimulationEditor;
    private List<ISimulationStateListener> simulationStateListeners = new ArrayList<ISimulationStateListener>();
    private List<ISelectionChangedListener> selectionListeners = new ArrayList<ISelectionChangedListener>();
    private ListenerList editorChangeListeners = new ListenerList(); // of ISimulationEditorChangeListeners

    public SimulationEditorProxy() {
        partListener = new IPartListener() {
            public void partActivated(IWorkbenchPart part) {
                if (part != associatedSimulationEditor && part instanceof SimulationEditor)
                    setAssociatedEditor((SimulationEditor) part);
            }

            public void partBroughtToTop(IWorkbenchPart part) {
            }

            public void partClosed(IWorkbenchPart part) {
                if (part == associatedSimulationEditor)
                    setAssociatedEditor(null);
            }

            public void partDeactivated(IWorkbenchPart part) {
            }

            public void partOpened(IWorkbenchPart part) {
            }
        };

        // call init() now, or schedule doing it later in asyncExec when workbenchPage becomes available
        new Runnable() {
            @Override
            public void run() {
                IWorkbenchPage page = getActiveWorkbenchPage();
                if (page != null)
                    init(page);
                else
                    // try calling init() later
                    Display.getDefault().asyncExec(this);
            }
        }.run();
    }

    private void init(IWorkbenchPage page) {
        this.workbenchPage = page;
        workbenchPage.addPartListener(partListener);

        // associate with currently active simulation editor
        SimulationEditor editor = getActiveSimulationEditor();
        if (editor != null)
            setAssociatedEditor(editor);
    }

    private static IWorkbenchPage getActiveWorkbenchPage() {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        return window == null ? null : window.getActivePage();
    }


    public void addSimulationStateListener(ISimulationStateListener listener) {
        simulationStateListeners.add(listener);
        if (associatedSimulationEditor != null)
            associatedSimulationEditor.addSimulationStateListener(listener);
    }

    public void removeSimulationStateListener(ISimulationStateListener listener) {
        simulationStateListeners.remove(listener);
        if (associatedSimulationEditor != null)
            associatedSimulationEditor.removeSimulationStateListener(listener);
    }

    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionListeners.add(listener);
        if (associatedSimulationEditor != null)
            associatedSimulationEditor.getSite().getSelectionProvider().addSelectionChangedListener(listener);
    }

    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionListeners.remove(listener);
        if (associatedSimulationEditor != null)
            associatedSimulationEditor.getSite().getSelectionProvider().removeSelectionChangedListener(listener);
    }

    public void addSimulationEditorChangeListener(ISimulationEditorChangeListener listener) {
        editorChangeListeners.add(listener);
    }

    public void removeSimulationEditorChangeListener(ISimulationEditorChangeListener listener) {
        editorChangeListeners.remove(listener);
    }

    public SimulationEditor getAssociatedSimulationEditor() {
        return associatedSimulationEditor;
    }

    protected void setAssociatedEditor(SimulationEditor editor) {
        Assert.isTrue(associatedSimulationEditor != editor);

        // remove listeners from old editor
        if (associatedSimulationEditor != null) {
            for (ISimulationStateListener listener : simulationStateListeners)
                associatedSimulationEditor.removeSimulationStateListener(listener);
            for (ISelectionChangedListener listener : selectionListeners)
                associatedSimulationEditor.getSite().getSelectionProvider().removeSelectionChangedListener(listener);
        }

        // add listeners to new editor
        associatedSimulationEditor = editor;
        if (associatedSimulationEditor != null) {
             for (ISimulationStateListener listener : simulationStateListeners)
                 associatedSimulationEditor.addSimulationStateListener(listener);
             for (ISelectionChangedListener listener : selectionListeners)
                 associatedSimulationEditor.getSite().getSelectionProvider().addSelectionChangedListener(listener);
        }

        // notify about editor change
        for (Object listener : editorChangeListeners.getListeners()) {
            try {
                ((ISimulationEditorChangeListener)listener).associatedSimulationEditorChanged(associatedSimulationEditor);
            }
            catch (Exception e) {
                SimulationPlugin.logError(e);
            }
        }
    }

    public void dispose() {
        if (associatedSimulationEditor != null)
            setAssociatedEditor(null);
        workbenchPage.removePartListener(partListener);
    }

    protected SimulationEditor getActiveSimulationEditor() {
        if (workbenchPage != null) {
            if (workbenchPage.getActiveEditor() instanceof SimulationEditor)
                return (SimulationEditor)workbenchPage.getActiveEditor();
            // no active simulation editor; just return the first one we find
            for (IEditorReference ref : workbenchPage.getEditorReferences())
                if (ref.getEditor(false) instanceof SimulationEditor)
                    return (SimulationEditor)ref.getEditor(false);
        }
        return null;
    }
}
