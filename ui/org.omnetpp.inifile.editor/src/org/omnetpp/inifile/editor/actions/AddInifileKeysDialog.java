package org.omnetpp.inifile.editor.actions;

import javax.swing.text.TableView;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TrayDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;


/**
 * A standard dialog which solicits a list of selections from the user.
 * This class is configured with an arbitrary data model represented by content
 * and label provider objects. The <code>getResult</code> method returns the
 * selected elements.
 */
public class AddInifileKeysDialog extends TrayDialog {
	// the final collection of selected elements, or null if this dialog was
	// canceled
	private Object[] result;

	private String title;
	private String message = "";
	
	// table input and providers 
    private ILabelProvider labelProvider;
    private IStructuredContentProvider contentProvider;
    
    private enum KeyType { PARAM_ONLY, MODULE_AND_PARAM, FULLPATH };
    private KeyType keyType = KeyType.PARAM_ONLY;

    // the visual selection widget group
    CheckboxTableViewer listViewer;

    // sizing constants
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 80;
    private final static int SIZING_SELECTION_WIDGET_WIDTH = 300;

    /**
     * Creates the dialog.
     */
    public AddInifileKeysDialog(Shell parentShell, InifileAnalyzer analyzer, String message) {
        super(parentShell);
        setTitle("Generate Inifile Contents");
        this.contentProvider = new ArrayContentProvider();
        this.labelProvider = new LabelProvider();
        setMessage(message!=null ? message : "Choose keys to be added to the file.");
    }

	/**
	 * Sets the title for this dialog.
	 */
	public void setTitle(String title) {
		this.title = title;
	}

	/**
	 * Sets the message for this dialog.
	 */
	public void setMessage(String message) {
		this.message = message;
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

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    protected Control createDialogArea(Composite parent) {
        // page group
        Composite composite = (Composite) super.createDialogArea(parent);
        initializeDialogUnits(composite);
        createMessageArea(composite);

        Button applyDefault = new Button(composite, SWT.CHECK);
        applyDefault.setText("Apply default value of parameters that have one");
        
		// radiobutton group
        Group group = new Group(composite, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setText("Pattern style");
		group.setLayout(new GridLayout(1, false));

        // radiobuttons
		Button b = createRadioButton(group, "Parameter name only (**.queueSize)", KeyType.PARAM_ONLY);
		createRadioButton(group, "Module and parameter only (**.mac.queueSize)", KeyType.MODULE_AND_PARAM);
		createRadioButton(group, "Full path (Network.host[*].mac.queueSize)", KeyType.FULLPATH);
		b.setSelection(true);
        
		// table group
        Group group2 = new Group(composite, SWT.NONE);
		group2.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group2.setText("Keys to insert");
		group2.setLayout(new GridLayout(1, false));
        
        // table and buttons
		listViewer = CheckboxTableViewer.newCheckList(group2, SWT.BORDER);
        GridData data = new GridData(GridData.FILL_BOTH);
        data.heightHint = SIZING_SELECTION_WIDGET_HEIGHT;
        data.widthHint = SIZING_SELECTION_WIDGET_WIDTH;
        listViewer.getTable().setLayoutData(data);

        listViewer.setLabelProvider(labelProvider);
        listViewer.setContentProvider(contentProvider);
        addSelectionButtons(group2);

        buildTableContents();
        
        Dialog.applyDialogFont(composite);
        
        return composite;
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

    @SuppressWarnings("unchecked")
	protected void okPressed() {
    	result = listViewer.getCheckedElements();
        super.okPressed();
    }

	protected void buildTableContents() {
		Object[] inputElement = new String[] {"one", "two", "three", keyType.toString()};
		listViewer.setInput(inputElement);
	}

    
	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		if (title != null)
			shell.setText(title);
	}

	/**
	 * Creates the message area for this dialog.
	 */
	protected Label createMessageArea(Composite composite) {
		Label label = new Label(composite, SWT.NONE);
		if (message != null) {
			label.setText(message);
		}
		label.setFont(composite.getFont());
		return label;
	}

	protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}

	/**
	 * Returns the list of selections made by the user, or <code>null</code>
	 * if the selection was canceled.
	 */
	public Object[] getResult() {
		return result;
	}
}
