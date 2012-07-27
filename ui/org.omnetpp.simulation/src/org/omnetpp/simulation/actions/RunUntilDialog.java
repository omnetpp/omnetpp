/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.actions;

import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TrayDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.SimulationController.RunMode;


/**
 * Implements the "Run Until..." dialog.
 *
 * @author Andras
 */
public class RunUntilDialog extends TrayDialog {
    private String title;

    // widgets
    private Combo modeCombo;
    private Text simtimeText;
    private Text eventNumberText;

    // the result
    private RunMode mode;
    private BigDecimal simtime;
    private long eventNumber;

    /**
     * Creates the dialog.
     */
    public RunUntilDialog(Shell parentShell) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Run Until";
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(SimulationPlugin.getDefault(), getClass().getName());
    }

    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
            shell.setText(title);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(2,false));

        createLabel(composite, "Enter simulation time or event number to run until.\n\n", 2);

        createLabel(composite, "Simulation time (s):", 1);
        simtimeText = new Text(composite, SWT.BORDER);
        simtimeText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        createLabel(composite, "Event number:", 1);
        eventNumberText = new Text(composite, SWT.BORDER);
        eventNumberText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        createLabel(composite, "Run mode:", 1);
        modeCombo = new Combo(composite, SWT.BORDER | SWT.READ_ONLY);
        modeCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        modeCombo.add(StringUtils.capitalize(RunMode.NORMAL.name().toLowerCase()));
        modeCombo.add(StringUtils.capitalize(RunMode.FAST.name().toLowerCase()));
        modeCombo.add(StringUtils.capitalize(RunMode.EXPRESS.name().toLowerCase()));
        modeCombo.select(1);

        simtimeText.setFocus();

        //TODO set up validation!
        //TODO restore last values from preference store

        Dialog.applyDialogFont(composite);

        return composite;
    }

    protected Label createLabel(Composite composite, String text, int horizontalSpan) {
        Label label = new Label(composite, SWT.NONE);
        GridData gridData = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        gridData.horizontalSpan = horizontalSpan;
        label.setLayoutData(gridData);
        label.setText(text);
        return label;
    }

    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }

    protected void okPressed() {
        // save dialog state into variables, so that client can retrieve them after the dialog was disposed
        mode = RunMode.valueOf(modeCombo.getText().toUpperCase());

        //TODO error handling!!!
        String simtimeString = simtimeText.getText();
        simtime = StringUtils.isBlank(simtimeString) ? BigDecimal.getZero() : BigDecimal.parse(simtimeString);
        String eventNumberString = eventNumberText.getText();
        eventNumber = StringUtils.isBlank(eventNumberString) ? 0 : Long.valueOf(eventNumberString);
        super.okPressed();
    }

    public RunMode getRunMode() {
        return mode;
    }

    public BigDecimal getSimTime() {
        return simtime;
    }

    public long getEventNumber() {
        return eventNumber;
    }
}
