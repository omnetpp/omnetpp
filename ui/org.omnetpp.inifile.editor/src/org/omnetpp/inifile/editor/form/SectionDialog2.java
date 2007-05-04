package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Edit section name and its description. 
 * @author Andras
 */
//XXX copy validation code from InputDialog
public class SectionDialog2 extends TitleAreaDialog {
	private IInifileDocument doc;
	
	// dialog config
	private String dialogTitle;
	private String dialogMessage;

	// widgets
	private Text sectionNameText;
	private Text descriptionText;
	private Combo extendsCombo;
	private Text networkNameText;
    private Button okButton;

	// dialog result
    private String sectionName;
    private String description;
    private String extendsSection;
    private String networkName;
    
    
	public SectionDialog2(Shell parentShell, String dialogTitle, String dialogMessage, IInifileDocument doc) {
		super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
		this.doc = doc;
		this.dialogTitle = dialogTitle;
		this.dialogMessage = dialogMessage;
	}

	@Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
		shell.setText(dialogTitle);
    }

	@Override
	protected Control createDialogArea(Composite parent) {
        setTitle(dialogTitle);
        setMessage(dialogMessage);

		Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

        Group group1 = createGroup(composite, "Name and description");
        
		// section name
		createLabel(group1, "Section Name:", parent.getFont());
		sectionNameText = new Text(group1, SWT.SINGLE | SWT.BORDER);
		sectionNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		// section description
		createLabel(group1, "Description (optional):", parent.getFont());
		descriptionText = new Text(group1, SWT.SINGLE | SWT.BORDER);
		descriptionText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Group group2 = createGroup(composite, "Basic configuration");

		// "extends" section
		createLabel(group2, "Fall back to section:", parent.getFont());
		extendsCombo = new Combo(group2, SWT.READ_ONLY | SWT.BORDER);
		extendsCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		// network name
		createLabel(group2, "NED Network:", parent.getFont());
		networkNameText = new Text(group2, SWT.SINGLE | SWT.BORDER);
		networkNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		// fill "sections" combo
		String[] sectionNames = doc.getSectionNames();
		if (sectionNames.length==0) 
			sectionNames = new String[] {"General"};  //XXX we lie that [General] exists
		extendsCombo.setItems(sectionNames);
		extendsCombo.setVisibleItemCount(Math.min(20, extendsCombo.getItemCount()));
		extendsCombo.select(0);

		// fill dialog fields with initial contents
		if (sectionName!=null) sectionNameText.setText(sectionName);
        if (description!=null) descriptionText.setText(description);
        if (extendsSection!=null) extendsCombo.setText(extendsSection);
        if (networkName!=null) networkNameText.setText(networkName);
		
		return composite;
	}

	protected static Label createLabel(Composite parent, String text, Font font) {
		Label label = new Label(parent, SWT.WRAP);
		label.setText(text);
		label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
		label.setFont(font);
		return label;
	}

	protected static Group createGroup(Composite parent, String text) {
		Group group = new Group(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setText(text);
		group.setLayout(new GridLayout(2, false));
		return group;
	}

	protected void createButtonsForButtonBar(Composite parent) {
        // create OK and Cancel buttons by default
        okButton = createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
        
        // do this here because setting the text will set enablement on the ok button
        String value = "bubu"; //XXX
        sectionNameText.setFocus();
        if (value != null) {
            sectionNameText.setText(value);
            sectionNameText.selectAll();
        }
    }

    @SuppressWarnings("unchecked")
	protected void okPressed() {
    	// save dialog state into variables, so that client can retrieve them after the dialog was disposed
        sectionName = sectionNameText.getText();
        description = descriptionText.getText();
        extendsSection = extendsCombo.getText();
        networkName = networkNameText.getText();
        super.okPressed();
    }

	public String getSectionName() {
		return sectionName;
	}

	public void setSectionName(String sectionName) {
		this.sectionName = sectionName;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public String getExtendsSection() {
		return extendsSection;
	}

	public void setExtendsSection(String extendsSection) {
		this.extendsSection = extendsSection;
	}

	public String getNetworkName() {
		return networkName;
	}

	public void setNetworkName(String networkName) {
		this.networkName = networkName;
	}
	
}
