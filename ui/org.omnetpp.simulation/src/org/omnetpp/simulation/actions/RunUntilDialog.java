/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.actions;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TrayDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;


/**
 * Implements the "Run Until..." dialog.
 *
 * @author Andras
 */
public class RunUntilDialog extends TrayDialog {
    private String title;

    // widgets
    private Text simtimeText;
    private Text eventNumberText;
    private Text moduleText;
    //TODO: private Text messageText;
    private Button normalRunButton;
    private Button fastRunButton;
    private Button expressRunButton;
    private Widget lastFocus;

    // the result
    private RunMode mode;
    private BigDecimal simtime;
    private long eventNumber;
    private cModule module;
    private cMessage message;

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
        composite.setLayout(new GridLayout(1,false));

        Group limitsGroup = new Group(composite, SWT.NONE);
        limitsGroup.setText("Run the simulation until:");
        limitsGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        limitsGroup.setLayout(new GridLayout(2, false));

        createLabel(limitsGroup, "Simulation time (s):", 1);
        simtimeText = new Text(limitsGroup, SWT.BORDER);
        simtimeText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        createLabel(limitsGroup, "Event number:", 1);
        eventNumberText = new Text(limitsGroup, SWT.BORDER);
        eventNumberText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        createLabel(limitsGroup, "Module path or Id:", 1);
        moduleText = new Text(limitsGroup, SWT.BORDER);
        moduleText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

//TODO:
//        createLabel(limitsGroup, "Message:", 1);
//        messageText = new Text(limitsGroup, SWT.BORDER);
//        messageText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        Group modeGroup = new Group(composite, SWT.NONE);
        modeGroup.setText("Run mode:");
        modeGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        modeGroup.setLayout(new GridLayout(1, false));
        normalRunButton = new Button(modeGroup, SWT.RADIO);
        normalRunButton.setText("Normal");
        fastRunButton = new Button(modeGroup, SWT.RADIO);
        fastRunButton.setText("Fast");
        expressRunButton = new Button(modeGroup, SWT.RADIO);
        expressRunButton.setText("Express");

        // track focus so we can save/restore it (in okPressed() it's too late)
        FocusListener focusListener = new FocusAdapter() {
            public void focusGained(FocusEvent e) { lastFocus = e.widget; }
        };
        simtimeText.addFocusListener(focusListener);
        eventNumberText.addFocusListener(focusListener);
        moduleText.addFocusListener(focusListener);

        // restore last dialog content
        IDialogSettings settings = getDialogBoundsSettings();
        simtimeText.setText(StringUtils.defaultString(settings.get("simtime")));
        eventNumberText.setText(StringUtils.defaultString(settings.get("eventNumber")));
        moduleText.setText(StringUtils.defaultString(settings.get("module")));
        simtimeText.selectAll();
        eventNumberText.selectAll();
        moduleText.selectAll();
        RunMode runMode = Enum.valueOf(RunMode.class, StringUtils.defaultIfEmpty(settings.get("runMode"), "NORMAL"));
        if (runMode == RunMode.NORMAL)
            normalRunButton.setSelection(true);
        else if (runMode == RunMode.FAST)
            fastRunButton.setSelection(true);
        else if (runMode == RunMode.EXPRESS)
            expressRunButton.setSelection(true);
        simtimeText.setFocus();  // default
        String focus = StringUtils.defaultString(settings.get("focus"));
        if (focus.equals("simtime")) simtimeText.setFocus();
        if (focus.equals("eventNumber")) eventNumberText.setFocus();
        if (focus.equals("module")) moduleText.setFocus();

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

    public RunMode readRunMode() {
        if (normalRunButton.getSelection())
            return RunMode.NORMAL;
        else if (fastRunButton.getSelection())
            return RunMode.FAST;
        else if (expressRunButton.getSelection())
            return RunMode.EXPRESS;
        else
            return RunMode.NONE;
    }

    protected void okPressed() {
        // save dialog settings
        IDialogSettings settings = getDialogBoundsSettings();
        settings.put("simtime", simtimeText.getText());
        settings.put("eventNumber", eventNumberText.getText());
        settings.put("module", moduleText.getText());
        settings.put("runMode", readRunMode().toString());
        String focus = lastFocus == simtimeText ? "simtime" :
                       lastFocus == eventNumberText ? "eventNumber" :
                       lastFocus == moduleText ? "module" : "";
        settings.put("focus", focus);

        // store dialog state in variables, so client can retrieve them after the dialog was disposed
        mode = readRunMode();

        //TODO error handling!!!
        String simtimeString = simtimeText.getText();
        simtime = StringUtils.isBlank(simtimeString) ? null : BigDecimal.parse(simtimeString);
        String eventNumberString = eventNumberText.getText();
        eventNumber = StringUtils.isBlank(eventNumberString) ? 0 : Long.valueOf(eventNumberString);
        //TODO module = StringUtils.isBlank(eventNumberString) ? 0 : Long.valueOf(eventNumberString);
        //TODO message
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

    public cModule getModule() {
        return module;
    }

    public cMessage getMessage() {
        return message;
    }
}
