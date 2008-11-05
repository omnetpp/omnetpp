package org.omnetpp.common.eventlog;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.UIUtils;

public class EventLogFindTextDialog extends Dialog {
    private String value = "";

    private Text text;

    private Button forward;

    private Button backward;

    private Button caseSensitive;

    private Button caseInsensitive;

    private boolean isBackward;

    private boolean isCaseInsensitive;

    protected EventLogFindTextDialog(Shell parentShell) {
        super(parentShell);
    }
    
    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(CommonPlugin.getDefault(), getClass().getName());
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        shell.setText("Find raw text");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite parentComposite = (Composite)super.createDialogArea(parent);
        Composite composite = new Composite(parentComposite, SWT.NONE);
        composite.setLayout(new GridLayout(2, true));
        composite.setLayoutData(new GridData(GridData.BEGINNING, GridData.BEGINNING, true, true));

        Label label = new Label(composite, SWT.WRAP);
        label.setText("The search will go through the raw content of the eventlog file starting from the current selection. Please enter the text to find:");
        GridData data = new GridData(GridData.FILL, GridData.BEGINNING, true, false, 2, 1);
        data.widthHint = convertHorizontalDLUsToPixels(IDialogConstants.MINIMUM_MESSAGE_AREA_WIDTH);
        label.setLayoutData(data);
        label.setFont(parent.getFont());

        text = new Text(composite, SWT.SINGLE | SWT.BORDER);
        text.setLayoutData(new GridData(GridData.FILL, GridData.BEGINNING, true, false, 2, 1));

        Group group = new Group(composite, SWT.SHADOW_ETCHED_IN);
        group.setText("Direction");
        group.setLayout(new GridLayout());
        group.setLayoutData(new GridData(GridData.FILL, GridData.BEGINNING, true, false, 1, 1));
        
        forward = new Button(group, SWT.RADIO);
        forward.setText("Forward");
        forward.setSelection(!isBackward);

        backward = new Button(group, SWT.RADIO);
        backward.setText("Bacward");
        backward.setSelection(isBackward);

        group = new Group(composite, SWT.SHADOW_ETCHED_IN);
        group.setText("Case");
        group.setLayout(new GridLayout());
        group.setLayoutData(new GridData(GridData.FILL, GridData.BEGINNING, true, false, 1, 1));
        
        caseSensitive = new Button(group, SWT.RADIO);
        caseSensitive.setText("Sensitive");
        caseSensitive.setSelection(!isCaseInsensitive);

        caseInsensitive = new Button(group, SWT.RADIO);
        caseInsensitive.setText("Insensitive");
        caseInsensitive.setSelection(isCaseInsensitive);

        return parentComposite;
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
        text.setFocus();
        if (value != null) {
            text.setText(value);
            text.selectAll();
        }
    }

    @Override
    protected void okPressed() {
        isBackward = backward.getSelection();
        isCaseInsensitive = caseInsensitive.getSelection();
        value = text.getText();

        super.okPressed();
    }

    public boolean isBackward() {
        return isBackward;
    }

    public boolean isCaseInsensitive() {
        return isCaseInsensitive;
    }

    public String getValue() {
        return value;
    }
}
