package org.omnetpp.simulation.actions;

import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * Brings up a dialog with properties of the simulation process we are attached to.
 *
 * @author Andras
 */
public class ProcessInfoAction extends AbstractSimulationActionDelegate {

    public class InfoDialog extends TitleAreaDialog {
        private String shellTitle;
        private String dialogTitle;
        private String[] labelsValues;

        public InfoDialog(Shell parentShell, String shellTitle, String dialogTitle, String[] labelsValues) {
            super(parentShell);
            setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
            this.shellTitle = shellTitle;
            this.dialogTitle = dialogTitle;
            this.labelsValues = labelsValues;
        }

        protected void configureShell(Shell shell) {
            super.configureShell(shell);
            if (shellTitle != null)
                shell.setText(shellTitle);
        }

        protected Control createDialogArea(Composite parent) {
            setTitle(dialogTitle);

            Composite dialogArea = (Composite) super.createDialogArea(parent);

            Composite composite = new Composite(dialogArea, SWT.NONE);
            composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            GridLayout layout = new GridLayout(2,false);
            layout.marginWidth = 10;
            composite.setLayout(layout);

            for (int i = 0; i < labelsValues.length; i += 2) {
                if (labelsValues[i+1] == null) {
                    Label title = new Label(composite, SWT.NONE);
                    title.setText(labelsValues[i]);
                    title.setFont(JFaceResources.getBannerFont());
                    title.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
                    ((GridData)title.getLayoutData()).horizontalSpan = 2;
                }
                else {
                    Label name = new Label(composite, SWT.NONE);
                    name.setText(labelsValues[i]);

                    Text value = new Text(composite, SWT.READ_ONLY | SWT.BORDER);
                    value.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
                    value.setText(labelsValues[i+1]);
                }
            }

            Dialog.applyDialogFont(composite);

            return composite;
        }

        @Override
        protected Point getInitialSize() {
            Point size = getShell().computeSize(SWT.DEFAULT, SWT.DEFAULT, true);
            size.y += 25; // for some unknown reason, by default the dialog will be a bit smaller than needed (Win7, text size set to 125%)
            return size;
        }

        protected void createButtonsForButtonBar(Composite parent) {
            createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
            createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
        }
    }

    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.processInfoAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            Simulation simulation = getSimulationController().getSimulation();

            if (!haveSimulationProcess())
                return;

            String[] items = new String[] {  // Note: name, value; if value==null then name is interpreted as title
                    "Process:", null,
                    "Host:", simulation.getHostName(),
                    "Port:", ""+simulation.getPortNumber(),
                    "URL:", simulation.getUrlBase(),
                    "Command line:", StringUtils.join(simulation.getArgv(), " "),
                    "Working directory:", simulation.getWorkingDir(),
                    "Process Id:", ""+simulation.getProcessId(),
                    "Simulation State:", null,
                    "Configuration name:", StringUtils.defaultString(simulation.getConfigName(), "n/a"),
                    "Run number:", simulation.getConfigName()==null ? "n/a" : ""+simulation.getRunNumber(),
                    "Network name:", StringUtils.defaultString(simulation.getNetworkName(), "n/a"),
                    "Simulation state:", simulation.getState().name(),
                    "Last event's event number:", ""+simulation.getLastEventNumber(),
                    "Last event's simulation time:", simulation.getLastEventSimulationTime().toString(),
            };

            InfoDialog dialog = new InfoDialog(getShell(), "Simulation Process Info", "Info about the Attached Simulation Process", items);
            dialog.open();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void updateState() {
        SimState state = getSimulationController().getUIState();
        setEnabled(state != SimState.DISCONNECTED);
    }
}
