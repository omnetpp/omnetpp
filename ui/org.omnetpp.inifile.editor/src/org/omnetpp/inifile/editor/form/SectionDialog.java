package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.Set;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
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
import org.omnetpp.common.engine.Common;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;

/**
 * Edit section name and some of its attributes 
 * @author Andras
 */
public class SectionDialog extends TitleAreaDialog {
	private IInifileDocument doc;
	
	// dialog config
	private String dialogTitle;
	private String dialogMessage;
	private String originalSectionName;

	// widgets
	private Text sectionNameText;
	private Text descriptionText;
	private Combo extendsCombo;
	private Combo networkCombo;
    private Button okButton;

    private boolean insideValidation = false;

	// dialog result
    private String newSectionName;
    private String description;
    private String extendsSection;
    private String networkName;
    
    
	/**
	 * Operation depends on the sectionName parameter: if sectionName names an 
	 * existing section, that's interpreted as "rename/edit section"; if there's
	 * no such section or sectionName is null, it's "create new section".
	 */
    public SectionDialog(Shell parentShell, String dialogTitle, String dialogMessage, IInifileDocument doc, String sectionName) {
		super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
		this.doc = doc;
		this.dialogTitle = dialogTitle;
		this.dialogMessage = dialogMessage;
		this.originalSectionName = sectionName;
		
		// if we are editing an existing section, initialize dialog fields from the inifile
		if (sectionName != null && doc.containsSection(sectionName)) {
			newSectionName = originalSectionName;
			description = doc.getValue(sectionName, ConfigurationRegistry.CFGID_DESCRIPTION.getKey());
			if (description != null)
				try {description = Common.parseQuotedString(description);} catch (RuntimeException e) {}
			extendsSection = doc.getValue(sectionName, ConfigurationRegistry.CFGID_EXTENDS.getKey());
			extendsSection = extendsSection==null ? GENERAL : CONFIG_ + extendsSection;
			networkName = doc.getValue(sectionName, ConfigurationRegistry.CFGID_NETWORK.getKey());;
		}
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
        
		// section name field
		createLabel(group1, "Section Name:", parent.getFont());
		sectionNameText = new Text(group1, SWT.SINGLE | SWT.BORDER);
		sectionNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		// description field
		createLabel(group1, "Description (optional):", parent.getFont());
		descriptionText = new Text(group1, SWT.SINGLE | SWT.BORDER);
		descriptionText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Group group2 = createGroup(composite, "Basic configuration");

		// "extends" section field
		createLabel(group2, "Fall back to section:", parent.getFont());
		extendsCombo = new Combo(group2, SWT.READ_ONLY | SWT.BORDER);
		extendsCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		// network name field
		createLabel(group2, "NED Network:", parent.getFont());
		networkCombo = new Combo(group2, SWT.BORDER);
		networkCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		// fill "sections" combo
		//XXX don't offer sections which would create a circle!!!!!!!!
		String[] sectionNames = doc.getSectionNames();
		if (sectionNames.length==0) 
			sectionNames = new String[] {"General"};  //XXX we lie that [General] exists
		extendsCombo.setItems(sectionNames);
		extendsCombo.setVisibleItemCount(Math.min(20, extendsCombo.getItemCount()));
		extendsCombo.select(0);

		// fill network combo
		Set<String> networkNames = NEDResourcesPlugin.getNEDResources().getNetworkNames();
		networkCombo.setItems(networkNames.toArray(new String[] {}));
		networkCombo.setVisibleItemCount(Math.min(20, networkCombo.getItemCount()));
		
		// fill dialog fields with initial contents
		if (newSectionName!=null) sectionNameText.setText(newSectionName);
        if (description!=null) descriptionText.setText(description);
        if (extendsSection!=null) extendsCombo.setText(extendsSection); 
        if (networkName!=null) networkCombo.setText(networkName);
		sectionNameText.selectAll();
		descriptionText.selectAll();
        
        // set up validation on content changes
        ModifyListener listener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                validateDialogContents();
            }
        };
        sectionNameText.addModifyListener(listener);
        descriptionText.addModifyListener(listener);
        extendsCombo.addModifyListener(listener);
        networkCombo.addModifyListener(listener);

        // note: do initial validation when OK button is already created, from createButtonsForButtonBar()

        // focus on first field
		sectionNameText.setFocus();

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
        validateDialogContents();
    }

	protected void validateDialogContents() {
		// prevent notification loops
		if (insideValidation) return;
		insideValidation = true;
		
		// if as section with "network=" is selected, make it impossible to override here
        extendsSection = extendsCombo.getText();
        String ownNetworkName = doc.getValue(originalSectionName, ConfigurationRegistry.CFGID_NETWORK.getKey());
        String inheritedNetworkName = InifileUtils.lookupConfig(extendsSection, ConfigurationRegistry.CFGID_NETWORK.getKey(), doc);
        if (ownNetworkName == null && inheritedNetworkName != null) {
        	networkCombo.setText(inheritedNetworkName);
        	networkCombo.setEnabled(false);
        }
        else {
        	networkCombo.setEnabled(true);
        }
        
		// validate contents
        String errorMessage = validate();
		setErrorMessage(errorMessage);
		if (okButton != null)  // it is initially null
			okButton.setEnabled(errorMessage==null);
		
		insideValidation = false;
	}

	protected String validate() {
		// validate section name
		String sectionName = sectionNameText.getText();
		sectionName = sectionName.trim();
		if (sectionName.equals(""))
			return "Section name cannot be empty";
		if (!sectionName.equals(GENERAL) && !sectionName.startsWith(CONFIG_))
			sectionName = CONFIG_+sectionName;
		if (doc.containsSection(sectionName) && (originalSectionName==null || !originalSectionName.equals(sectionName)))
			return "Section ["+sectionName+"] already exists";

		//XXX check that selected network exists

		return null;
	}
	
    @SuppressWarnings("unchecked")
	protected void okPressed() {
    	// save dialog state into variables, so that client can retrieve them after the dialog was disposed
        newSectionName = sectionNameText.getText().trim();
		if (!newSectionName.equals(GENERAL) && !newSectionName.startsWith(CONFIG_))
			newSectionName = CONFIG_+newSectionName;
        description = descriptionText.getText().trim();
        extendsSection = extendsCombo.getText().trim();
        networkName = networkCombo.isEnabled() ? networkCombo.getText().trim() : "";
        super.okPressed();
    }

	public String getSectionName() {
		return newSectionName;
	}

	public void setSectionName(String sectionName) {
		this.newSectionName = sectionName;
	}

	public String getDescription() {
		return description.equals("") ? "" : Common.quoteString(description);
	}

	public void setDescription(String description) {
		if (description != null)
			try {description = Common.parseQuotedString(description);} catch (RuntimeException e) {}
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
