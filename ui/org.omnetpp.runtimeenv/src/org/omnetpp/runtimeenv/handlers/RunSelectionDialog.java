package org.omnetpp.runtimeenv.handlers;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.experimental.simkernel.swig.Javaenv;
import org.omnetpp.experimental.simkernel.swig.cSimulation;

/**
 * Select configuration and run number
 * @author Andras
 */
//FIXME move to the right package
public class RunSelectionDialog extends Dialog {
	private String title = "Select Configuration";
	private String message = "Select inifile configuration and run number:";
	private String configName;
	private int runNumber;

	private Combo configCombo;
	private Combo runNumberCombo;
	private Button okButton;

	protected RunSelectionDialog(Shell parentShell) {
		super(parentShell);
	}

	public void setTitle(String title) {
		this.title = title;
	}
	
	public void setMessage(String message) {
		this.message = message;
	}

	public void setData(String configName, int runNumber) {
		this.configName = configName;
		this.runNumber = runNumber;
	}

    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
			shell.setText(title);
    }
	
    public String getConfigName() {
		return configName;
	}
    
    public int getRunNumber() {
		return runNumber;
	}
    
    protected Control createDialogArea(Composite parent) {
        // create composite
        Composite composite = (Composite) super.createDialogArea(parent);
        
        // create message
        if (message != null) {
            Label label = new Label(composite, SWT.WRAP);
            label.setText(message);
            GridData data = new GridData(SWT.FILL, SWT.CENTER, true, true);
            data.widthHint = convertHorizontalDLUsToPixels(IDialogConstants.MINIMUM_MESSAGE_AREA_WIDTH);
            label.setLayoutData(data);
            label.setFont(parent.getFont());
        }

        Composite group = new Composite(composite, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        group.setLayout(new GridLayout(2,false));

        // create combo boxes
        Label l = new Label(group, SWT.NONE);
        l.setText("Configuration name:");
        configCombo = new Combo(group, SWT.BORDER | SWT.READ_ONLY);
        l = new Label(group, SWT.NONE);
        l.setText("Run number:");
        runNumberCombo = new Combo(group, SWT.BORDER | SWT.READ_ONLY);
        configCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        runNumberCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        
        final Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
        final String[] configNames = env.getConfigEx().getConfigNames().toArray();
        configCombo.setItems(configNames);
        
        ModifyListener listener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
            	okButton.setEnabled(!StringUtils.isEmpty(configCombo.getText()));

            	// when section changes, update runNumber combo
            	String cfg = configCombo.getText();
            	if (!ArrayUtils.contains(configNames, cfg)) 
            		runNumberCombo.setItems(new String[]{});
            	else {
            		int n = env.getConfigEx().getNumRunsInConfig(cfg);
            		String[] runLabels = new String[n];
            		for (int i=0; i<n; i++)
            			runLabels[i] = "Run #" + i;
            		runNumberCombo.setItems(runLabels);
            	}
            }
        };
		configCombo.addModifyListener(listener);

		// note: combo contents will be set in createButtonsForButtonBar()
		
        applyDialogFont(composite);
        return composite;
    }

    protected void createButtonsForButtonBar(Composite parent) {
        // create OK and Cancel buttons by default
        okButton = createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);

        // do this here because setting the text will set enablement on the OK button
        configCombo.setFocus();
        if (configName != null) {
        	configCombo.setText(configName);
            runNumberCombo.setText(""+runNumber);
        }
    }

    protected void buttonPressed(int buttonId) {
        if (buttonId == IDialogConstants.OK_ID) {
            configName = configCombo.getText();
            try { runNumber = Integer.parseInt(runNumberCombo.getText()); } catch (NumberFormatException e) {runNumber = 0;}
        } else {
            configName = null;
            runNumber = -1;
        }
        super.buttonPressed(buttonId);
    }
   
}
