package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
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
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;

/**
 * Edit section name and some of its attributes 
 * @author Andras
 */
//FIXME sectionTypeCombo is not finished! to be used everywhere!!!!
public class SectionDialog extends TitleAreaDialog {
	private IInifileDocument doc;
	
	// dialog config
	private String dialogTitle;
	private String dialogMessage;
	private String originalSectionName;

	// widgets
	private Text configNameText;
	private Text descriptionText;
	private Combo extendsCombo;
	private Combo networkCombo;
    private Button okButton;

    private boolean insideValidation = false;

	// initial parameters, which also double as dialog result
    private String sectionName;
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
		this.sectionName = this.originalSectionName = sectionName;
		
		// if we are editing an existing section, initialize dialog fields from the inifile
		if (sectionName != null && doc.containsSection(sectionName)) {
			description = doc.getValue(sectionName, ConfigRegistry.CFGID_DESCRIPTION.getKey());
			if (description != null)
				try {description = Common.parseQuotedString(description);} catch (RuntimeException e) {}
			extendsSection = InifileUtils.resolveBaseSectionPretendingGeneralExists(doc, sectionName);
			networkName = doc.getValue(sectionName, ConfigRegistry.CFGID_NETWORK.getKey());;
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
		
		configNameText = new Text(group1, SWT.SINGLE | SWT.BORDER);
		configNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

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

		// fill "extends" combo
		if (GENERAL.equals(originalSectionName))
			extendsCombo.setEnabled(false);
		else {
			String[] sectionNames = getNonCycleSectionNames(originalSectionName);
			if (sectionNames.length==0) 
				sectionNames = new String[] {GENERAL};  // we lie that [General] exists
			extendsCombo.setItems(sectionNames);
			extendsCombo.setVisibleItemCount(Math.min(20, extendsCombo.getItemCount()));
			extendsCombo.select(0);
		}

		// fill network combo
		Set<String> networkNameSet = NEDResourcesPlugin.getNEDResources().getNetworkQNames();
		String[] networkNames = networkNameSet.toArray(new String[]{});
		Arrays.sort(networkNames);
		networkCombo.setItems(networkNames);
		networkCombo.setVisibleItemCount(Math.min(20, networkCombo.getItemCount()));
		
		// fill dialog fields with initial contents
		if (sectionName!=null) 
			configNameText.setText(sectionName.replaceFirst(CONFIG_+" *", ""));
		if (description!=null) 
			descriptionText.setText(description);
        if (extendsSection!=null) 
        	extendsCombo.setText(extendsSection); 
        if (networkName!=null) 
        	networkCombo.setText(networkName);
		configNameText.selectAll();
		descriptionText.selectAll();
        
        // set up validation on content changes
        ModifyListener listener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                validateDialogContents();
            }
        };
        configNameText.addModifyListener(listener);
        descriptionText.addModifyListener(listener);
        extendsCombo.addModifyListener(listener);
        networkCombo.addModifyListener(listener);

        // note: do initial validation when OK button is already created, from createButtonsForButtonBar()

        // focus on first field
		configNameText.setFocus();

		return composite;
	}

	/**
	 * Return list of sections that would not create a cycle if the given 
	 * section extended them.
	 */
	protected String[] getNonCycleSectionNames(String section) {
		String[] sectionNames = doc.getSectionNames();
		if (!doc.containsSection(section))
			return sectionNames;  // a new section can extend anything

		// choose sections whose fallback chain doesn't contain the given section
		List<String> result = new ArrayList<String>();
		for (String candidate : sectionNames)
			if (!InifileUtils.sectionChainContains(doc, candidate, section))
				result.add(candidate);
		return result.toArray(new String[]{});
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

	@Override
	protected void createButtonsForButtonBar(Composite parent) {
        // create OK and Cancel buttons by default
        okButton = createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
        
        // do this here because setting the text will set enablement on the OK button
        validateDialogContents();
    }

	protected void validateDialogContents() {
		// prevent notification loops
		if (insideValidation) return;
		insideValidation = true;
		
		// when a base section with "network=" is selected, make it impossible to override it here
        extendsSection = extendsCombo.getText();
        String ownNetworkName = doc.getValue(originalSectionName, ConfigRegistry.CFGID_NETWORK.getKey());
        String inheritedNetworkName = InifileUtils.lookupConfig(extendsSection, ConfigRegistry.CFGID_NETWORK.getKey(), doc);
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
		String configName = configNameText.getText().trim();
		if (configName.equals(""))
			return "Config name cannot be empty";
		if (configName.contains(" ") || configName.contains("\t"))
			throw new RuntimeException("Config name must not contain spaces");
		if (!configName.replaceAll("[a-zA-Z0-9-_]", "").isEmpty())
			throw new RuntimeException("Config name contains illegal character(s)");
		if (!(CONFIG_+configName).equals(originalSectionName))
			if (doc.containsSection(CONFIG_+configName)) 
				return "A section named ["+CONFIG_+configName+"] already exists";

		//XXX check that selected network exists

		return null;
	}
	
	private String assembleSectionName() {
		String configName = configNameText.getText().trim();
		return configName.equals(GENERAL) ? GENERAL : CONFIG_+configName;
	}
	
    @Override
	@SuppressWarnings("unchecked")
	protected void okPressed() {
    	// save dialog state into variables, so that client can retrieve them after the dialog was disposed
        sectionName = assembleSectionName();
        description = descriptionText.getText().trim();
        extendsSection = extendsCombo.getText().trim();
        if (extendsSection.equals(""))
        	extendsSection = GENERAL;
        networkName = networkCombo.isEnabled() ? networkCombo.getText().trim() : "";
        super.okPressed();
    }

	public String getSectionName() {
		return sectionName;
	}

	public void setSectionName(String sectionName) {
		this.sectionName = sectionName;
	}

	public String getDescription() {
		return description.equals("") ? "" : Common.quoteString(description);
	}

	public void setDescription(String description) {
		if (description != null)
			try {description = Common.parseQuotedString(description);} catch (RuntimeException e) {}
		this.description = description;
	}

	/**
	 * Returns the section name the edited section extends, or "General". Never returns "" or null.
	 */
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
