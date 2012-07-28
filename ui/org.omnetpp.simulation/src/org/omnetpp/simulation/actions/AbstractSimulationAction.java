/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.controller.SimulationController.SimState;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.inspectors.SimulationCanvas;

/**
 * Base class to factor out common code in Action classes.
 *
 * @author andras
 */
//TODO disable if simulationController is DETACHED
public abstract class AbstractSimulationAction extends Action {
    private SimulationEditor simulationEditor;

    private ISimulationStateListener listener = new ISimulationStateListener() {
        @Override
        public void simulationStateChanged(SimulationController controller) {
            updateState();
        }
    };

    /**
     * Default constructor.
     */
    public AbstractSimulationAction(SimulationEditor simulationEditor) {
        this.simulationEditor = simulationEditor;
        simulationEditor.getSimulationController().addSimulationStateListener(listener);
    }

    /**
     * Constructor to set style.
     */
    public AbstractSimulationAction(SimulationEditor simulationEditor, int style) {
        super("", style);
        this.simulationEditor = simulationEditor;
        simulationEditor.getSimulationController().addSimulationStateListener(listener);
    }

    public SimulationController getSimulationController() {
        return simulationEditor.getSimulationController();
    }

    public SimulationCanvas getSimulationCanvas() {
        return simulationEditor.getSimulationCanvas();
    }
    
    public abstract void updateState();

    /**
     * Utility function.
     */
    protected boolean haveSimulation(SimulationController controller) {
        if (controller.getState() == SimState.DISCONNECTED) {
            MessageDialog.openConfirm(getShell(), "Error", "No simulation process.");
            return false;
        }
        return true;
    }

    /**
     * Utility function. Returns true on success, false on failure.
     */
    protected boolean ensureNetworkReady(SimulationController controller) {
        if (!haveSimulation(controller))
            return false;
        if (!controller.isNetworkPresent())
            return false;
        if (controller.isSimulationOK())
            return true;
        if (MessageDialog.openQuestion(getShell(), "Warning", "Cannot continue this simulation. Rebuild network?")) {
            try {
                controller.rebuildNetwork();
            }
            catch (Exception e) { // notably IOException / HttpException
                MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
                SimulationPlugin.logError(e);
            }
        }
        return controller.isSimulationOK();
    }

    /**
     * Utility function. Returns true on success, false on failure.
     */
    protected boolean ensureNotRunning(SimulationController controller) {
        if (!haveSimulation(controller))
            return false;
        if (controller.getState() == SimState.RUNNING) {
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
