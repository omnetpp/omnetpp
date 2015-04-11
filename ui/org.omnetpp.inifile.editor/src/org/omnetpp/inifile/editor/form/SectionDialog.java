/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.inifile.editor.model.InifileUtils.configNameToSectionName;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.fieldassist.SimpleContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
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
import org.eclipse.ui.dialogs.ElementListSelectionDialog;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;

/**
 * Edit section name and some of its attributes
 * @author Andras
 */
//FIXME sectionTypeCombo is not finished! to be used everywhere!!!!
public class SectionDialog extends TitleAreaDialog {
    private IReadonlyInifileDocument doc;

    // dialog config
    private String dialogTitle;
    private String dialogMessage;
    private String originalSectionName;

    // widgets
    private Text configNameText;
    private Text descriptionText;
    private Text baseConfigsText;
    private Combo networkCombo;
    private Button okButton;

    private boolean insideValidation = false;

    // initial parameters, which also double as dialog result
    private String sectionName;
    private String description;
    private String baseConfigNames;
    private String networkName;


    /**
     * Operation depends on the sectionName parameter: if sectionName names an
     * existing section, that's interpreted as "rename/edit section"; if there's
     * no such section or sectionName is null, it's "create new section".
     */
    public SectionDialog(Shell parentShell, String dialogTitle, String dialogMessage, IReadonlyInifileDocument doc, String sectionName) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
        this.doc = doc;
        this.dialogTitle = dialogTitle;
        this.dialogMessage = dialogMessage;
        this.sectionName = this.originalSectionName = sectionName;

        // if we are editing an existing section, initialize dialog fields from the inifile
        if (sectionName != null && doc.containsSection(sectionName)) {
            description = doc.getValue(sectionName, ConfigRegistry.CFGID_DESCRIPTION.getName());
            if (description != null)
                try {description = Common.parseQuotedString(description);} catch (RuntimeException e) {}
            List<String> baseConfigs = InifileUtils.sectionNamesToConfigNames(InifileUtils.resolveBaseSectionsPretendingGeneralExists(doc, sectionName));
            baseConfigNames = InifileUtils.formatExtendsList(baseConfigs);
            networkName = doc.getValue(sectionName, ConfigRegistry.CFGID_NETWORK.getName());;
        }
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(InifileEditorPlugin.getDefault(), getClass().getName());
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

        Group group1 = createGroup(composite, "Name and description", 2);

        // section name field
        createLabel(group1, "Section Name:", parent.getFont());

        configNameText = new Text(group1, SWT.SINGLE | SWT.BORDER);
        configNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // description field
        createLabel(group1, "Description (optional):", parent.getFont());
        descriptionText = new Text(group1, SWT.SINGLE | SWT.BORDER);
        descriptionText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Group group2 = createGroup(composite, "Basic configuration", 3);

        // "extends" section field
        createLabel(group2, "Fall back to sections:", parent.getFont());
        baseConfigsText = new Text(group2, SWT.BORDER);
        baseConfigsText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Button button = new Button(group2, SWT.PUSH);
        button.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
        button.setText("...");
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                openBaseConfigsSelectionDialog();
            }
        });

        // network name field
        createLabel(group2, "NED Network:", parent.getFont());
        networkCombo = new Combo(group2, SWT.BORDER);

        networkCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));

        // fill "extends" text's content assist
        if (GENERAL.equals(originalSectionName)) {
            baseConfigsText.setEnabled(false);
            button.setEnabled(false);
        }
        else {
            List<String> configNames = getNonCycleConfigNames(originalSectionName);
            if (configNames.size() > 0)
                new ContentAssistCommandAdapter(baseConfigsText,
                        new TextContentAdapter(),
                        new SimpleContentProposalProvider(configNames.toArray(new String[configNames.size()])),
                        ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,
                        ",".toCharArray() /*auto-activation*/,
                        true /*installDecoration*/);
        }

        // fill network combo
        IProject contextProject = doc.getDocumentFile().getProject();
        Set<String> networkNameSet = NedResourcesPlugin.getNedResources().getNetworkQNames(contextProject); // no need to work on a copy of ned resources
        String[] networkNames = networkNameSet.toArray(new String[]{});
        Arrays.sort(networkNames);
        networkCombo.setItems(networkNames);
        networkCombo.setVisibleItemCount(Math.min(20, networkCombo.getItemCount()));

        // fill dialog fields with initial contents
        if (sectionName!=null)
            configNameText.setText(sectionName.replaceFirst(CONFIG_+" *", ""));
        if (description!=null)
            descriptionText.setText(description);
        if (baseConfigNames!=null)
            baseConfigsText.setText(baseConfigNames);
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
        baseConfigsText.addModifyListener(listener);
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
    protected List<String> getNonCycleConfigNames(String section) {
        String[] sectionNames = doc.getSectionNames();
        if (section == null || !doc.containsSection(section))
            return InifileUtils.sectionNamesToConfigNames(Arrays.asList(sectionNames));  // a new section can extend anything

        // choose sections whose fallback chain doesn't contain the given section
        List<String> result = new ArrayList<String>();
        for (String candidate : sectionNames)
            if (!InifileUtils.sectionChainContains(doc, candidate, section))
                result.add(candidate);
        return InifileUtils.sectionNamesToConfigNames(result);
    }

    protected static Label createLabel(Composite parent, String text, Font font) {
        Label label = new Label(parent, SWT.WRAP);
        label.setText(text);
        label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        label.setFont(font);
        return label;
    }

    protected static Group createGroup(Composite parent, String text, int numColumns) {
        Group group = new Group(parent, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        group.setText(text);
        group.setLayout(new GridLayout(numColumns, false));
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

    protected void openBaseConfigsSelectionDialog() {
        ElementListSelectionDialog dialog = new ElementListSelectionDialog(getShell(), new LabelProvider());
        dialog.setMultipleSelection(true);
        dialog.setTitle("Fallback configurations");
        String section = originalSectionName;
        dialog.setMessage(String.format("Select the base configurations%s.", section!=null?" of '" + section + "'" : ""));
        List<String> elements = getNonCycleConfigNames(section);
        dialog.setElements(elements.toArray());
        List<String> oldValue = InifileUtils.parseExtendsList(baseConfigsText.getText().trim());
        dialog.setInitialSelections(oldValue.toArray());
        if (dialog.open() == Window.OK) {
            List<Object> newValue = Arrays.asList(dialog.getResult());
            if (!oldValue.equals(newValue)) {
                List<String> configNames = new ArrayList<String>();
                for (Object configName : newValue)
                    configNames.add((String)configName);
                baseConfigsText.setText(InifileUtils.formatExtendsList(configNames));
            }
        }
    }

    protected void validateDialogContents() {
        // prevent notification loops
        if (insideValidation) return;
        insideValidation = true;

        // when a base section with "network=" is selected, make it impossible to override it here
        baseConfigNames = baseConfigsText.getText();
        String ownNetworkName = doc.getValue(originalSectionName, ConfigRegistry.CFGID_NETWORK.getName());
        String inheritedNetworkName = null;
        for (String configName : InifileUtils.parseExtendsList(baseConfigNames)) {
            inheritedNetworkName = InifileUtils.lookupConfig(CONFIG_+configName, ConfigRegistry.CFGID_NETWORK.getName(), doc);
            if (inheritedNetworkName != null)
                break;
        }
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
            return "Config name must not contain spaces";
        if (!configName.replaceAll("[a-zA-Z0-9-_]", "").equals(""))
            return "Config name contains illegal character(s)";
        if (!(CONFIG_+configName).equals(originalSectionName))
            if (doc.containsSection(CONFIG_+configName))
                return "A section named ["+CONFIG_+configName+"] already exists";

        // validate base config names
        List<String> baseConfigNames = InifileUtils.parseExtendsList(baseConfigsText.getText().trim());
        for (String baseConfigName : baseConfigNames)
        {
            if (!baseConfigName.matches("[a-zA-Z0-9-_]+"))
                return "Base config name contains illegal character(s)";
            if (!doc.containsSection(CONFIG_+baseConfigName))
                return "Base config '"+baseConfigName+"' does not exist";
        }

        //XXX check that selected network exists

        return null;
    }

    @Override
    protected void okPressed() {
        // save dialog state into variables, so that client can retrieve them after the dialog was disposed
        sectionName = configNameToSectionName(configNameText.getText().trim());
        description = descriptionText.getText().trim();
        baseConfigNames = baseConfigsText.getText().trim();
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
    public String getBaseConfigNames() {
        return baseConfigNames;
    }

    public void setBaseConfigNames(String baseConfigNames) {
        this.baseConfigNames = baseConfigNames;
    }

    public String getNetworkName() {
        return networkName;
    }

    public void setNetworkName(String networkName) {
        this.networkName = networkName;
    }

}
