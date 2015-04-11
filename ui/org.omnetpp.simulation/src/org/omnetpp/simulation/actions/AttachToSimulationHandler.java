/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.actions;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.simulation.SimulationEditorInput;
import org.omnetpp.common.ui.InputDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * Implements the "Attach to simulation" action.
 *
 * @author andras
 */
public class AttachToSimulationHandler extends AbstractHandler {
    @Override
    public Object execute(ExecutionEvent event) throws ExecutionException {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage workbenchPage = workbenchWindow.getActivePage();
        Shell shell = workbenchWindow.getShell();

        InputDialog dialog = new InputDialog(shell, "Attach to OMNeT++ Simulation", "Enter hostname and port to connect to:", "localhost:8080", null);

        if (dialog.open() == Dialog.OK) {
            // parse the result
            String result = dialog.getValue();
            String hostName = StringUtils.substringBefore(result, ":").trim();
            String portNumberStr = StringUtils.substringAfter(result, ":").trim();
            if (hostName.equals("") || portNumberStr.equals("")) {
                MessageDialog.openError(shell, "Error", "Invalid syntax: <hostname>:<portnumber> expected, e.g. localhost:1234.");
                return null;
            }
            int portNumber;
            try {
                portNumber = Integer.valueOf(portNumberStr);
            } catch (NumberFormatException e) {
                MessageDialog.openError(shell, "Error", "Not a number: " + portNumberStr);
                return null;
            }

            // open the editor
            try {
                IEditorInput input = new SimulationEditorInput(result, hostName, portNumber);
                IDE.openEditor(workbenchPage, input, SimulationEditor.EDITOR_ID);
            }
            catch (PartInitException e) {
                ErrorDialog.openError(shell, "Error", "Could not open animation window for the running simulation.", e.getStatus());
                SimulationPlugin.logError(e);
            }
            //TODO offer switching to "Simulation" perspective!
        }
        return null;
    }
}
