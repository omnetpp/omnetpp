package org.omnetpp.inifile.editor.actions;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;


/**
 * Dialog for choosing parameter keys to be inserted into the ini file.
 * @author Andras
 */
//XXX doesn't work if there's no [General] section
//XXX print "network=" setting for that section
//XXX in the dialog: warn if inifile doesn't have "network=" setting for that section !!!!
public class AddInifileKeysDialog extends TitleAreaDialog {
	private String title;
	private InifileAnalyzer analyzer;
	
	// widgets
    private Combo sectionsCombo;
    private Label networkNameLabel;
	private Label sectionChainLabel;
	private Button skipCheckbox;
    private CheckboxTableViewer listViewer;

    // dialog state
    private enum KeyType { PARAM_ONLY, MODULE_AND_PARAM, ANYNETWORK_FULLPATH, FULLPATH };
    private KeyType keyType;

	// the result
    private String[] keysToAdd;
    private String selectedSection;

	// sizing constants
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 120;
    private final static int SIZING_SELECTION_WIDGET_WIDTH = 300;

    /**
     * Creates the dialog.
     */
    public AddInifileKeysDialog(Shell parentShell, InifileAnalyzer analyzer, String initialSection) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.analyzer = analyzer;
        this.title = "Add Inifile Keys";
        this.selectedSection = initialSection;
        if (analyzer.getDocument().getSectionNames().length==0)
        	throw new IllegalStateException("Inifile should contain at least one section.");
    }

	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		if (title != null)
			shell.setText(title);
	}

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    protected Control createDialogArea(Composite parent) {
        setTitle("Add Inifile Keys");
        setMessage("Generate keys into the ini file for currently unassigned parameters.");
    	
        // page group
        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

		// Section combobox
		Composite comboWithLabel = new Composite(composite, SWT.NONE);
		comboWithLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		Label comboLabel = new Label(comboWithLabel, SWT.NONE);
		comboLabel.setText("Section:");
		sectionsCombo = new Combo(comboWithLabel, SWT.BORDER | SWT.READ_ONLY);
		comboWithLabel.setLayout(new GridLayout(2, false));
		comboLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		sectionsCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

		networkNameLabel = createLabel(composite, "Network: n/a");
		sectionChainLabel = createLabel(composite, "Section fallback chain: n/a  ");

		sectionsCombo.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				buildTableContents();
			}
		});
		
		// radiobutton group
        Group group = new Group(composite, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setText("Pattern style");
		group.setLayout(new GridLayout(1, false));

        // radiobuttons
		createRadioButton(group, "Parameter name only (**.queueSize)", KeyType.PARAM_ONLY);
		Button b = createRadioButton(group, "Module and parameter only (**.mac.queueSize)", KeyType.MODULE_AND_PARAM);
		createRadioButton(group, "Full path except network name (*.host[*].mac.queueSize)", KeyType.ANYNETWORK_FULLPATH);
		createRadioButton(group, "Full path (Network.host[*].mac.queueSize)", KeyType.FULLPATH);
		b.setSelection(true);
		keyType = KeyType.MODULE_AND_PARAM; // must agree with selected radiobutton
        
		// table group
        Group group2 = new Group(composite, SWT.NONE);
		group2.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		group2.setText("Select keys to insert");
		group2.setLayout(new GridLayout(1, false));
      
		// checkboxes
		skipCheckbox = new Button(group2, SWT.CHECK);
		skipCheckbox.setText("Skip parameters that have a default value");
		skipCheckbox.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				buildTableContents();
			}
		});
		GridData gridData = new GridData();
		gridData.horizontalIndent = 20;
		
        // table and buttons
		listViewer = CheckboxTableViewer.newCheckList(group2, SWT.BORDER);
        GridData data = new GridData(GridData.FILL_BOTH);
        data.heightHint = SIZING_SELECTION_WIDGET_HEIGHT;
        data.widthHint = SIZING_SELECTION_WIDGET_WIDTH;
        listViewer.getTable().setLayoutData(data);

        listViewer.setContentProvider(new ArrayContentProvider());
        listViewer.setLabelProvider(new LabelProvider() {
			@Override
			public String getText(Object element) {
				ParamResolution res = (ParamResolution) element;
				return getKeyFor(res);
			}
        });

        addSelectionButtons(group2);

        buildTableContents();
        
        Dialog.applyDialogFont(composite);
        
        return composite;
    }

	protected Label createLabel(Composite composite, String text) {
		Label label = new Label(composite, SWT.NONE);
		label.setLayoutData(new GridData(SWT.END, SWT.BEGINNING, true, false));
		label.setText(text);
		return label;
	}

    protected Button createRadioButton(Group group, String label, final KeyType value) {
		Button rb = new Button(group, SWT.RADIO);
		rb.setText(label);
		rb.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (keyType != value) { 
					keyType = value;
					buildTableContents();
				}
			}
		});
		return rb;
	}

	/**
     * Add the selection and deselection buttons to the dialog.
     */
    private void addSelectionButtons(Composite composite) {
        Composite buttonComposite = new Composite(composite, SWT.NONE);
        GridLayout layout = new GridLayout();
        layout.numColumns = 0;
		layout.marginWidth = 0;
		layout.horizontalSpacing = convertHorizontalDLUsToPixels(IDialogConstants.HORIZONTAL_SPACING);
        buttonComposite.setLayout(layout);
        buttonComposite.setLayoutData(new GridData(SWT.END, SWT.TOP, true, false));

        Button selectButton = createButton(buttonComposite, IDialogConstants.SELECT_ALL_ID, "Select All", false);

        SelectionListener listener = new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                listViewer.setAllChecked(true);
            }
        };
        selectButton.addSelectionListener(listener);

        Button deselectButton = createButton(buttonComposite, IDialogConstants.DESELECT_ALL_ID, "Deselect All", false);

        listener = new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                listViewer.setAllChecked(false);
            }
        };
        deselectButton.addSelectionListener(listener);
    }

    protected void buildTableContents() {
		// refresh combo with the current section names, trying to preserve existing selection
    	// Note: initially, sectionsCombo is empty, and selectedSection contains the default section to activate.
		IInifileDocument doc = analyzer.getDocument();
		if (!sectionsCombo.getText().equals(""))
			selectedSection = sectionsCombo.getText();
		String[] sectionNames = doc.getSectionNames();
		if (sectionNames.length==0) 
			sectionNames = new String[] {"General"};  //XXX we lie that [General] exists
		sectionsCombo.setItems(sectionNames);
		sectionsCombo.setVisibleItemCount(Math.min(20, sectionsCombo.getItemCount()));
		int i = ArrayUtils.indexOf(sectionNames, selectedSection);
		sectionsCombo.select(i<0 ? 0 : i);
		selectedSection = sectionsCombo.getText();

		// compute fallback chain for selected section, and fill table with their contents
		String[] sectionChain = InifileUtils.resolveSectionChain(doc, selectedSection);
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);

		// update labels: "Network" and "Section fallback chain"
		networkNameLabel.setText("Network: "+(networkName==null ? "<not configured>" : networkName)+"  ");
		sectionChainLabel.setText("Section fallback chain: "+(sectionChain.length==0 ? "<no sections>" : StringUtils.join(sectionChain, " > "))+"  ");
		sectionChainLabel.getParent().layout();
		
		skipCheckbox.setEnabled(true);
		
		// get list of unassigned parameters
		List<ParamResolution> unassignedParams = Arrays.asList(analyzer.getUnassignedParams(selectedSection));
		if (skipCheckbox.getSelection()) {
			// only choose those that don't have a default value
			List<ParamResolution> list = new ArrayList<ParamResolution>();
			for (ParamResolution res : unassignedParams)
				if (res.paramValueNode == null || StringUtils.isEmpty(res.paramValueNode.getValue()))
					list.add(res);
			unassignedParams = list;
		}

		// keep only those that generate unique keys
		Set<String> uniqueKeys = new HashSet<String>();
		List<ParamResolution> list = new ArrayList<ParamResolution>();
		for (ParamResolution res : unassignedParams) {
			if (!uniqueKeys.contains(getKeyFor(res))) {
				uniqueKeys.add(getKeyFor(res));
				list.add(res);
			}
		}
		unassignedParams = list;

		// fill the table
		Object[] currentInput = (Object[]) listViewer.getInput();
		if (!Arrays.equals(unassignedParams.toArray(), currentInput)) {
			listViewer.setInput(unassignedParams.toArray());
			listViewer.setAllChecked(true);
		}
	
		listViewer.refresh();
	}
    
	protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}

	protected String getKeyFor(ParamResolution res) {
		String paramName = res.paramDeclNode.getName();
		String fullPath = res.moduleFullPath;
		switch (keyType) {
			case PARAM_ONLY: return "**."+paramName;
			case MODULE_AND_PARAM: return fullPath.replaceFirst(".*?(\\.[^.]*)?$", "**$1")+"."+paramName;
			case ANYNETWORK_FULLPATH: return fullPath.replaceFirst("^[^.]*", "*") + "." + paramName;
			case FULLPATH: return fullPath + "." + paramName;
			default: return null;
		}
	}
	
	protected void okPressed() {
    	// save dialog state into variables, so that client can retrieve them after 
    	// the dialog was disposed
    	ArrayList<String> result = new ArrayList<String>(); 
    	for (Object res : listViewer.getCheckedElements())
    		result.add(getKeyFor((ParamResolution)res));
    	this.keysToAdd = result.toArray(new String[]{});
        super.okPressed();
    }
	
	/**
	 * Returns the list of keys to be inserted into the file.
	 */
	public String[] getKeys() {
		return keysToAdd;
	}

	/**
	 * Returns the section to insert the keys into.
	 */
	public String getSection() {
		return selectedSection;
	}
}
