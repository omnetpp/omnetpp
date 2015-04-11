/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorActionDelegate;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.simulation.canvas.SimulationCanvas;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.ISimulationChangeListener;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationChangeEvent;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * Base class to factor out common code in Action classes.
 *
 * @author andras
 */
public abstract class AbstractSimulationActionDelegate implements IEditorActionDelegate {
    private IAction thisAction;
    private SimulationEditor simulationEditor;

    private ISimulationChangeListener listener = new ISimulationChangeListener() {
        @Override
        public void simulationStateChanged(SimulationChangeEvent e) {
            //TODO if (e.reason == ... what exactly?
            updateState();
        }
    };

    /**
     * Default constructor.
     */
    public AbstractSimulationActionDelegate() {
    }

    public SimulationController getSimulationController() {
        return simulationEditor.getSimulationController();
    }

    public SimulationCanvas getSimulationCanvas() {
        return simulationEditor.getSimulationCanvas();
    }

    @Override
    public void setActiveEditor(IAction action, IEditorPart targetEditor) {
        if (thisAction == null) {
            thisAction = action;
            registerInContributor(action);
        }

        if (simulationEditor != null && !simulationEditor.isDisposed())  //XXX action will hold on to dead editors until you switch to another simulation editor!!!
            simulationEditor.removeSimulationChangeListener(listener);
        simulationEditor = (SimulationEditor) targetEditor;
        if (simulationEditor != null)
            simulationEditor.addSimulationChangeListener(listener);

        if (simulationEditor == null)
            setEnabled(false);
        else
            updateState();
    }

    protected abstract void registerInContributor(IAction action);

    @Override
    public void selectionChanged(IAction action, ISelection selection) {
    }

    public abstract void updateState();

    protected void setEnabled(boolean b) {
        thisAction.setEnabled(b);
    }

    protected void setChecked(boolean b) {
        thisAction.setChecked(b);
    }

    protected boolean isEnabled() {
        return thisAction.isEnabled();
    }

    protected boolean isChecked() {
        return thisAction.isChecked();
    }

    /**
     * Utility function.
     */
    protected boolean haveSimulationProcess() {
        if (!getSimulationController().hasSimulationProcess()) {
            MessageDialog.openConfirm(getShell(), "Error", "No simulation process.");
            return false;
        }
        return true;
    }

    /**
     * Utility function. Returns true on success, false on failure.
     */
    protected boolean ensureNetworkReady() {
        if (!haveSimulationProcess())
            return false;
        SimulationController controller = getSimulationController();
        if (!controller.isNetworkPresent())
            return false;
        if (controller.isSimulationOK())
            return true;
        if (MessageDialog.openQuestion(getShell(), "Warning", "Cannot continue this simulation. Rebuild network?")) {
            try {
                controller.rebuildNetwork();
            }
            catch (CommunicationException e) {
                // ignore -- logging etc already done by the thrower
            }
        }
        return controller.isSimulationOK();
    }

    /**
     * Utility function. Returns true on success, false on failure.
     */
    protected boolean ensureNotRunning(SimulationController controller) {
        if (!haveSimulationProcess())
            return false;
        if (controller.getUIState() == SimState.RUNNING) {
            MessageDialog.openInformation(getShell(), "Simulation Running", "Sorry, you cannot do this while the simulation is running.");
            return false;
        }
//        if (simulationController.getState() == SimState.BUSY) {
//            MessageDialog.openInformation(getShell(), "Simulation Busy", "The simulation is waiting for external synchronization -- press STOP to interrupt it.");
//            return false;
//        }
        return true;
    }

//    /**
//     * Utility function: returns the active SimulationEditor's simulationController.
//     * Throws an exception if there is no editor or the active editor is
//     * not a SimulationEditor.
//     */
//    protected SimulationController getActiveSimulationController() throws CoreException {
//        return getActiveSimulationEditor().getSimulationController();
//    }
//
//    /**
//     * Utility function: returns the active SimulationEditor. Throws an exception
//     * if there is no editor or the active editor is not a SimulationEditor.
//     */
//    protected SimulationEditor getActiveSimulationEditor() throws CoreException {
//        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
//        IWorkbenchPage page = workbenchWindow == null ? null : workbenchWindow.getActivePage();
//        IEditorPart editor = page == null ? null : page.getActiveEditor();
//        SimulationEditor result = (editor instanceof SimulationEditor) ? (SimulationEditor)editor : null;
//        if (result == null)
//            throw new RuntimeException("There is no active editor or it is not a Simulation Front-end");
//        return result;
//    }

    /**
     * Utility function
     */
    protected Shell getShell() {
        Shell activeShell = Display.getCurrent().getActiveShell();
        if (activeShell != null)
            return activeShell;
        return PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
    }
}
